/* ProcessUtils.cpp */
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

#include <CCore/inc/ProcessUtils.h>

#include <CCore/inc/sys/SysTypes.h>
#include <CCore/inc/sys/SysError.h>
#include <CCore/inc/sys/SysUtf8.h>

#include <CCore/inc/Array.h>
#include <CCore/inc/CharProp.h>

#include <CCore/inc/PrintError.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {

namespace Private_ProcessUtils {

using namespace CCore::Sys;

/* class Pipe */

class Pipe : NoCopy
 {
   WinNN::handle_t h_read;
   WinNN::handle_t h_write;

   bool has_read = true ;
   bool has_write = true ;

  public:

   Pipe()
    {
     if( !WinNN::CreatePipe(&h_read,&h_write,0,0) )
       {
        Printf(Exception,"CCore::Private_ProcessUtils::Pipe::Pipe() : #;",PrintError(Sys::NonNullError()));
       }

     if( !WinNN::SetHandleInformation(h_write,WinNN::HandleInherit,WinNN::HandleInherit) )
       {
        Printf(Exception,"CCore::Private_ProcessUtils::Pipe::Pipe() : #;",PrintError(Sys::NonNullError()));
       }
    }

   ~Pipe()
    {
     closeHRead();
     closeHWrite();
    }

   WinNN::handle_t getHRead() const { return h_read; }

   WinNN::handle_t getHWrite() const { return h_write; }

   void closeHRead()
    {
     if( Change(has_read,false) ) WinNN::CloseHandle(h_read);
    }

   void closeHWrite()
    {
     if( Change(has_write,false) ) WinNN::CloseHandle(h_write);
    }
 };

/* class PipeToBuf */

class PipeToBuf : public Pipe
 {
   PtrLen<char> buf;
   PtrLen<char> next;

  public:

   explicit PipeToBuf(PtrLen<char> buf_)
    : buf(buf_)
    {
     next=buf;
    }

   ~PipeToBuf()
    {
    }

   PtrLen<char> getData()
    {
     return buf.prefix(next);
    }

   void pump()
    {
     for(;;)
       {
        if( !next.len )
          {
           Printf(Exception,"CCore::Private_ProcessUtils::PipeToBuf::pump() : read pipe overflow");
          }

        WinNN::ulen_t ret_len;

        if( !WinNN::ExtReadFile(getHRead(),next.ptr,next.len,&ret_len) )
          {
           auto error=NonNullError();

           if( error==(ErrorType)WinNN::ErrorBrokenPipe ) return;

           Printf(Exception,"CCore::Private_ProcessUtils::PipeToBuf::pump() : #;",PrintError(error));
          }

        if( ret_len>next.len )
          {
           Printf(Exception,"CCore::Private_ProcessUtils::PipeToBuf::pump() : read pipe bad len");
          }

        next+=ret_len;
       }
    }
 };

/* class RunProcess */

class RunProcess : NoCopy
 {
   WinNN::handle_t h_process = 0 ;

  public:

   RunProcess(WChar *wcmd,WinNN::handle_t h_write)
    {
     WinNN::flags_t flags=0;

     WinNN::StartupInfo info{};

     info.cb=sizeof info;

     info.flags=WinNN::StartupInfo_std_handles;

     info.h_stdin=WinNN::InvalidFileHandle;
     info.h_stdout=h_write;
     info.h_stderr=WinNN::InvalidFileHandle;

     WinNN::ProcessInfo pinfo;

     if( WinNN::CreateProcessW(0,wcmd,0,0,true,flags,0,0,&info,&pinfo) )
       {
        WinNN::CloseHandle(pinfo.h_thread);

        h_process=pinfo.h_process;
       }
     else
       {
        Printf(Exception,"CCore::Private_ProcessUtils::RunProcess::RunProcess() : #;",PrintError(NonNullError()));
       }
    }

   ~RunProcess()
    {
     WinNN::CloseHandle(h_process);
    }

   void wait()
    {
     if( WinNN::WaitForSingleObject(h_process,WinNN::NoTimeout)!=WinNN::WaitObject_0 )
       {
        Printf(Exception,"CCore::Private_ProcessUtils::RunProcess::wait() : #;",PrintError(NonNullError()));
       }
    }

   unsigned getExitCode()
    {
     unsigned exit_code;

     if( !WinNN::GetExitCodeProcess(h_process,&exit_code) )
       {
        Printf(Exception,"CCore::Private_ProcessUtils::RunProcess::getExitCode() : #;",PrintError(NonNullError()));
       }

     return exit_code;
    }
 };

/* class MakeCmdLine */

class MakeCmdLine : NoCopy
 {
   DynArray<WChar> buf;

  public:

   explicit MakeCmdLine(StrLen str)
    {
     Collector<WChar> out;

     auto put = [&] (WChar ch) { out.append_copy(ch); } ;

     while( +str )
       {
        Unicode sym=CutUtf8_unicode(str);

        if( sym==Unicode(-1) )
          {
           Printf(Exception,"CCore::Private_ProcessUtils::MakeCmdLine::MakeCmdLine(...) : broken utf8");
          }

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

     put(0);

     out.extractTo(buf);
    }

   ~MakeCmdLine()
    {
    }

   WChar * getPtr() { return buf.getPtr(); }
 };

} // namespace Private_ProcessUtils

using namespace Private_ProcessUtils;

/* FromProgram() */

StrLen FromProgram(StrLen cmdline_,PtrLen<char> result_buf)
 {
  MakeCmdLine cmdline(cmdline_);

  PipeToBuf dev(result_buf);

  RunProcess proc(cmdline.getPtr(),dev.getHWrite());

  dev.closeHWrite();

  dev.pump();

  dev.closeHRead();

  proc.wait();

  if( auto exit_code=proc.getExitCode() )
    {
     Printf(Exception,"CCore::FromProgram(#.q;,...) : exit code is nonzero #;",cmdline_,exit_code);
    }

  return dev.getData();
 }

/* class UnixRootDir */

void UnixRootDir::make()
 {
  StrLen result=FromProgram("cygpath -m -a --codepage UTF8 /"_c,Range(buf));

  str=CutLine(result);
 }

/* class UnixHomeDir */

void UnixHomeDir::make()
 {
  StrLen result=FromProgram("cygpath -m -a --codepage UTF8 ~"_c,Range(buf));

  str=CutLine(result);
 }

} // namespace CCore

