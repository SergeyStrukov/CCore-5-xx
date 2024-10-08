/* SysCon.cpp */
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

#include <CCore/inc/sys/SysCon.h>
#include <CCore/inc/sys/SysUtf8.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* internal */

namespace Private_SysCon {

/* class ConOut */

class ConOut : NoCopy
 {
   WinNN::handle_t handle;
   ErrorType error;

  public:

   ConOut()
    {
     handle=WinNN::GetStdHandle(WinNN::StdOutputHandle);

     if( handle==WinNN::InvalidFileHandle )
       {
        error=NonNullError();
       }
     else if( handle==0 )
       {
        error=ErrorType(WinNN::ErrorFileNotFound);
       }
     else
       {
        error=ErrorIf( !WinNN::SetConsoleOutputCP(WinNN::CodePageUTF8) );
       }
    }

   void print(StrLen str)
    {
     if( error ) return;

     WinNN::ulen_t ret_len;

     WinNN::ExtWriteFile(handle,str.ptr,str.len,&ret_len);
    }
 };

ConOut Object CCORE_INITPRI_0 ;

} // namespace Private_SysCon

using namespace Private_SysCon;

/* functions */

void ConWrite(StrLen str) noexcept
 {
  Object.print(str);
 }

/* struct ConRead */

void ConRead::Symbol::put(unicode_t sym)
 {
  Utf8Code code=ToUtf8(sym);

  if( code.getLen()>DimOf(buf)-len ) return; // should not happen

  code.getRange().copyTo(buf+len);

  len+=code.getLen();
 }

bool ConRead::Symbol::pushUnicode(unicode_t sym)
 {
  if( sym==127 ) sym='\b'; // Windows sends 127 on BS in unicode mode

  put(sym);

  return true;
 }

bool ConRead::Symbol::push(WChar wch)
 {
  if( hi )
    {
     if( IsLoSurrogate(wch) )
       {
        return pushUnicode(Surrogate(Replace_null(hi),wch));
       }
     else if( IsHiSurrogate(wch) )
       {
        // broken, skip

        hi=wch;

        return false;
       }
     else
       {
        // broken, skip

        hi=0;

        return false;
       }
    }
  else
    {
     if( IsHiSurrogate(wch) )
       {
        hi=wch;

        return false;
       }
     else if( IsLoSurrogate(wch) )
       {
        // broken, skip

        return false;
       }
     else
       {
        return pushUnicode(wch);
       }
    }
 }

void ConRead::Symbol::shift(ulen delta)
 {
  if( delta>=len )
    {
     len=0;
    }
  else
    {
     len-=delta;

     for(ulen i=0,count=len; i<count ;i++) buf[i]=buf[i+delta];
    }
 }

auto ConRead::Symbol::get(char *out,ulen out_len) -> IOResult
 {
  Replace_min(out_len,len);

  if( !out_len ) return {0,NoError};

  Range(buf,out_len).copyTo(out);

  shift(out_len);

  return {out_len,NoError};
 }

auto ConRead::Init() noexcept -> InitType
 {
  InitType ret;

  ret.handle=WinNN::GetStdHandle(WinNN::StdInputHandle);

  if( ret.handle==WinNN::InvalidFileHandle )
    {
     ret.modes=0;
     ret.error=NonNullError();

     return ret;
    }

  if( ret.handle==0 )
    {
     ret.modes=0;
     ret.error=ErrorType(WinNN::ErrorFileNotFound);

     return ret;
    }

  if( !WinNN::GetConsoleMode(ret.handle,&ret.modes) )
    {
     ret.modes=0;
     ret.error=NonNullError();

     return ret;
    }

  ModeType new_modes=ret.modes;

  BitClear(new_modes,WinNN::ConEcho|WinNN::ConLineInput);

  if( !WinNN::SetConsoleMode(ret.handle,new_modes) )
    {
     ret.error=NonNullError();

     return ret;
    }

  ret.error=NoError;

  return ret;
 }

ErrorType ConRead::Exit(Type handle,ModeType modes) noexcept
 {
  return ErrorIf( !WinNN::SetConsoleMode(handle,modes) );
 }

auto ConRead::read(char *buf,ulen len) noexcept -> IOResult
 {
  IOResult ret;

  if( len==0 )
    {
     ret.error=NoError;
     ret.len=0;

     return ret;
    }

  do
    {
     ret=read(buf,len,DefaultTimeout);
    }
  while( !ret.error && ret.len==0 );

  return ret;
 }

auto ConRead::read(char *buf,ulen len,TimeScope time_scope) noexcept -> IOResult
 {
  IOResult ret;

  if( len==0 )
    {
     ret.error=NoError;
     ret.len=0;

     return ret;
    }

  if( +symbol )
    {
     return symbol.get(buf,len);
    }

  for(unsigned to_msec; (to_msec=+time_scope.get())!=0 ;)
    {
     WinNN::options_t opt=WinNN::WaitForSingleObject(handle,to_msec);

     if( opt==WinNN::WaitObject_0 )
       {
        WinNN::ConInputRecord input;
        WinNN::ushortlen_t ret_len;

        if( !WinNN::ReadConsoleInputW(handle,&input,1,&ret_len) )
          {
           ret.error=NonNullError();
           ret.len=0;

           return ret;
          }

        if( !ret_len )
          {
           ret.error=ErrorType(WinNN::ErrorReadFault);
           ret.len=0;

           return ret;
          }

        if( input.event_type==WinNN::ConKeyEvent )
          {
           if( input.event.key.key_down && input.event.key.ch.unicode && symbol.push(input.event.key.ch.unicode) )
             {
              return symbol.get(buf,len);
             }
          }
       }
     else if( opt!=WinNN::WaitTimeout )
       {
        ret.error=NonNullError();
        ret.len=0;

        return ret;
       }
    }

  ret.error=NoError;
  ret.len=0;

  return ret;
 }

} // namespace Sys
} // namespace CCore


