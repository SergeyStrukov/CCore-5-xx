/* SysSpawnInternal.cpp */
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

#include <CCore/inc/sys/SysSpawnInternal.h>

#include <CCore/inc/GenFile.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* class ProcessSetup::BuildStr */

class ProcessSetup::BuildStr : NoCopy
 {
   Collector<WChar> out;

  public:

   BuildStr() {}

   ~BuildStr() {}

   void put(WChar ch) { out.append_copy(ch); }

   void putStr(StrLen str)
    {
     while( +str )
       {
        Unicode sym=CutUtf8_unicode(str);

        if( sym==Unicode(-1) ) throw Error_BrokenUtf8;

        if( IsSurrogate(sym) )
          {
           SurrogateCouple couple(sym);

           put(couple.hi);
           put(couple.lo);
          }
        else
          { // may insert forbidden symbol
           put(WChar(sym));
          }
       }
    }

   void putArg(StrLen str)
    {
     put('"');

     while( +str )
       {
        Unicode sym=CutUtf8_unicode(str);

        if( sym==Unicode(-1) ) throw Error_BrokenUtf8;

        if( IsSurrogate(sym) )
          {
           SurrogateCouple couple(sym);

           put(couple.hi);
           put(couple.lo);
          }
        else
          { // may insert forbidden symbol

           switch( sym )
             {
              case '"' : put('\\'); put('"'); break;

              case '\\' : put('\\'); put('\\'); break;

              default: put(WChar(sym));
             }
          }
       }

     put('"');
    }

   void extractTo(DynArray<WChar> &ret) { out.extractTo(ret); }
 };

/* class ProcessSetup */

void ProcessSetup::makeDir(char *wdir)
 {
  if( wdir )
    {
     BuildStr out;

     out.putStr(wdir);
     out.put(0);

     out.extractTo(dir);

     WChar temp[MaxPathLen+1];

     ulen len=WinNN::GetFullPathNameW(dir.getPtr(),MaxPathLen+1,temp,0);

     if( !len ) throw NonNullError();

     if( len>MaxPathLen ) throw Error_TooLong;

     dir.erase().extend_copy(Range(temp,len+1));

     has_dir=true;
    }
  else
    {
     has_dir=false;
    }
 }

void ProcessSetup::makeCmdline(char *path,char **argv)
 {
  BuildStr out;

  out.putArg(path);

  if( *argv )
    {
     for(argv++; const char *str=*argv ;argv++)
       {
        out.put(' ');

        out.putArg(str);
       }
    }

  out.put(0);

  out.extractTo(cmdline);
 }

void ProcessSetup::makeEnvblock(char **envp)
 {
  BuildStr out;

  for(; const char *str=*envp ;envp++)
    {
     if( *str )
       {
        out.putStr(str);

        out.put(0);
       }
    }

  out.put(0);

  out.extractTo(envblock);
 }

ProcessSetup::ProcessSetup(char *wdir,char *path,char **argv,char **envp) noexcept
 {
  error=ErrorType(WinNN::ErrorNotEnoughMemory);

  SilentReportException report;

  try
    {
     makeDir(wdir);

     makeCmdline(path,argv);

     makeEnvblock(envp);

     error=NoError;
    }
  catch(CatchType)
    {
    }
  catch(ErrorType e)
    {
     error=e;
    }
 }

ProcessSetup::~ProcessSetup()
 {
 }

ErrorType ProcessSetup::create(handle_t &handle) noexcept
 {
  if( error ) return error;

  WinNN::flags_t flags=WinNN::UnicodeEnvironment;

  WinNN::StartupInfo info{};

  info.cb=sizeof info;

  WinNN::ProcessInfo pinfo;

  const WChar *wdir=0;

  if( has_dir ) wdir=dir.getPtr();

  if( WinNN::CreateProcessW(0,cmdline.getPtr(),0,0,false,flags,envblock.getPtr(),wdir,&info,&pinfo) )
    {
     WinNN::CloseHandle(pinfo.h_thread);

     handle=pinfo.h_process;

     return NoError;
    }
  else
    {
     return NonNullError();
    }
 }

} // namespace Sys
} // namespace CCore

