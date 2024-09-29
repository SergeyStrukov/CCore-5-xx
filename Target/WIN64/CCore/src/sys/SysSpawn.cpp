/* SysSpawn.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN64
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/sys/SysSpawn.h>

#include <CCore/inc/sys/SysEnv.h>
#include <CCore/inc/sys/SysSpawnInternal.h>

#include <CCore/inc/MemBase.h>
#include <CCore/inc/Array.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* GetShell() */

StrLen GetShell(char buf[MaxPathLen+1]) noexcept
 {
  TryGetEnv<32,MaxPathLen> tryget(Range(buf,MaxPathLen),"UNIXSHELL");

  if( tryget.ok ) return tryget.str;

  return "sh.exe"_c;
 }

/* struct GetEnviron */

auto GetEnviron::transform(ulen slen,Unicode sym,PtrLen<const WChar> text) noexcept -> TransformResult
 {
  TransformCountLen count(slen,sym,text);

  if( count.ok )
    {
     if( char *new_str=PlaceAt(TryMemAlloc(count.len)) )
       {
        ulen retlen=FinishTransform(Range(new_str,count.len),Range(str,slen),sym,text);

        if( str!=small ) MemFree(str);

        str=new_str;
        len=count.len;

        return {retlen,NoError};
       }
    }

  return {0,ErrorType(WinNN::ErrorNotEnoughMemory)};
 }

auto GetEnviron::transform(PtrLen<const WChar> text) noexcept -> TransformResult
 {
  if( !str )
    {
     str=small;
     len=DimOf(small);
    }

  auto out=Range(str,len);

  StartTransform result(text,out);

  if( result.ok )
    {
     return {result.len,NoError};
    }
  else
    {
     return transform(result.len,result.next_sym,result.rest);
    }
 }

ErrorType GetEnviron::init() noexcept
 {
  envblock=WinNN::GetEnvironmentStringsW();

  if( !envblock ) return NonNullError();

  ptr=envblock;
  str=0;

  return NoError;
 }

ErrorType GetEnviron::exit() noexcept
 {
  if( str && str!=small ) MemFree(Replace_null(str));

  if( !WinNN::FreeEnvironmentStringsW(Replace_null(envblock)) ) return NonNullError();

  return NoError;
 }

auto GetEnviron::next() noexcept -> NextResult
 {
  const WChar *wstr=ptr;

  if( *wstr )
    {
     const WChar *lim=ZScan(wstr);

     ptr=lim+1;

     auto result=transform(Range(wstr,lim));

     if( result.error ) return {Empty,result.error,false};

     return {StrLen(str,result.len),NoError,false};
    }
  else
    {
     return {Empty,NoError,true};
    }
 }

/* struct SpawnChild */

ErrorType SpawnChild::spawn(char *wdir,char *path,char **argv,char **envp) noexcept
 {
  ProcessSetup setup(wdir,path,argv,envp);

  return setup.create(handle);
 }

auto SpawnChild::wait() noexcept -> WaitResult
 {
  WaitResult ret;

  switch( WinNN::WaitForSingleObject(handle,WinNN::NoTimeout) )
    {
     case WinNN::WaitObject_0 :
      {
       unsigned exit_code;

       if( WinNN::GetExitCodeProcess(handle,&exit_code) )
         {
          ret.status=(int)exit_code;
          ret.error=NoError;
         }
       else
         {
          ret.status=1000;
          ret.error=NonNullError();
         }
      }
     break;

     case WinNN::WaitFailed :
      {
       ret.status=1000;
       ret.error=NonNullError();
      }
     break;

     default:
      {
       ret.status=1000;
       ret.error=Error_Spawn;
      }
    }

  WinNN::CloseHandle(handle);

  return ret;
 }

/* class SpawnWaitList::Engine */

class SpawnWaitList::Engine : public MemBase_nocopy
 {
   DynArray<handle_t> list;
   DynArray<void *> arglist;

  private:

   static WaitResult Finish(handle_t handle,void *arg)
    {
     WaitResult ret;

     ret.arg=arg;

     unsigned exit_code;

     if( WinNN::GetExitCodeProcess(handle,&exit_code) )
       {
        ret.status=(int)exit_code;
        ret.error=NoError;
       }
     else
       {
        ret.status=1000;
        ret.error=NonNullError();
       }

     WinNN::CloseHandle(handle);

     return ret;
    }

   WaitResult finish(ulen ind)
    {
     auto handle=list[ind];
     auto arg=arglist[ind];

     ulen last=list.getLen()-1;

     if( ind<last )
       {
        list[ind]=list[last];
        arglist[ind]=arglist[last];
       }

     list.shrink_one();
     arglist.shrink_one();

     return Finish(handle,arg);
    }

  public:

   explicit Engine(ulen reserve) : list(DoReserve,reserve),arglist(DoReserve,reserve) {}

   ~Engine() {}

   bool notEmpty() const { return list.notEmpty(); }

   void add(handle_t handle,void *arg)
    {
     arglist.reserve(1);

     list.append_copy(handle);
     arglist.append_copy(arg);
    }

   WaitResult wait()
    {
     if( list.isEmpty() ) return {0,0,NoError};

     auto result=WaitAny(Range(list));

     if( result.error ) return {0,0,result.error};

     return finish(result.ind);
    }

   struct WaitAnyResult
    {
     ulen ind;
     ErrorType error;
    };

   static WaitAnyResult WaitAny(PtrLen<handle_t> list)
    {
     auto result=WinNN::WaitForMultipleObjects(list.len,list.ptr,false,WinNN::NoTimeout);

     if( result==WinNN::WaitFailed )
       {
        return {0,NonNullError()};
       }

     if( result>=WinNN::WaitObject_0 && ulen(result-WinNN::WaitObject_0)<list.len )
       {
        return {ulen(result-WinNN::WaitObject_0),NoError};
       }

     return {0,Error_Spawn};
    }
 };

/* struct SpawnWaitList */

ErrorType SpawnWaitList::init(ulen reserve) noexcept
 {
  SilentReportException report;

  try
    {
     engine=new Engine(reserve);

     return NoError;
    }
  catch(CatchType)
    {
     return ErrorType(WinNN::ErrorNotEnoughMemory);
    }
 }

ErrorType SpawnWaitList::exit() noexcept
 {
  bool nok=engine->notEmpty();

  delete Replace_null(engine);

  return nok?Error_Running:NoError;
 }

ErrorType SpawnWaitList::add(SpawnChild *spawn,void *arg) noexcept
 {
  SilentReportException report;

  try
    {
     engine->add(spawn->handle,arg);

     return NoError;
    }
  catch(CatchType)
    {
     return ErrorType(WinNN::ErrorNotEnoughMemory);
    }
 }

auto SpawnWaitList::wait() noexcept -> WaitResult
 {
  return engine->wait();
 }

} // namespace Sys
} // namespace CCore
