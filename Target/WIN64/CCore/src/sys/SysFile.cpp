/* SysFile.cpp */
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

#include <CCore/inc/sys/SysFile.h>
#include <CCore/inc/sys/SysFileInternal.h>
#include <CCore/inc/sys/SysWaitInternal.h>

#include <CCore/inc/Exception.h>

namespace CCore {
namespace Sys {

/* GetPlanInitNode_...() */

namespace Private_SysFile {

/* AutoDelete() */

bool AutoDelete(WinNN::handle_t handle,bool flag)
 {
  WinNN::FileDispositionInfoData data;

  data.delete_file=flag;

  return WinNN::SetFileInformationByHandle(handle,WinNN::FileDispositionInfo,&data,sizeof data);
 }

/* Make...() */

WinNN::flags_t MakeAccessFlags(FileOpenFlags oflags)
 {
  WinNN::flags_t ret=0;

  if( oflags&Open_Read  ) ret|=WinNN::AccessRead;

  if( oflags&Open_Write ) ret|=WinNN::AccessWrite;

  if( oflags&Open_AutoDelete ) ret|=WinNN::AccessDelete;

  return ret;
 }

WinNN::flags_t MakeShareFlags(FileOpenFlags)
 {
  return WinNN::ShareRead;
 }

WinNN::options_t MakeCreationOptions(FileOpenFlags oflags)
 {
  WinNN::options_t ret;

  if( oflags&Open_New )
    {
     ret=WinNN::CreateNew;
    }
  else
    {
     if( oflags&Open_Create )
       {
        if( oflags&Open_Erase )
          ret=WinNN::CreateAlways;
        else
          ret=WinNN::OpenAlways;
       }
     else
       {
        if( oflags&Open_Erase )
          ret=WinNN::TruncateExisting;
        else
          ret=WinNN::OpenExisting;
       }
    }

  return ret;
 }

WinNN::flags_t MakeFileFlags(FileOpenFlags oflags)
 {
  WinNN::flags_t ret=WinNN::FileAttributeNormal;

  if( !(oflags&Open_Pos) ) ret|=WinNN::FileSequentialScan;

  return ret;
 }

WinNN::flags_t MakeAltFileFlags(FileOpenFlags)
 {
  WinNN::flags_t ret=WinNN::FileAttributeNormal|WinNN::FileAsyncIO;

  return ret;
 }

/* struct OpenFile */

struct OpenFile : File::OpenType
 {
  bool set(WinNN::handle_t h_file)
   {
    handle=h_file;

    if( h_file==WinNN::InvalidFileHandle )
      {
       error=MakeError(FileError_OpenFault);

       return false;
      }
    else
      {
       error=FileError_Ok;

       return true;
      }
   }

  void close()
   {
    error=MakeError(FileError_OpenFault);

    WinNN::CloseHandle(handle); // ignore unprobable error

    handle=WinNN::InvalidFileHandle;
   }

  explicit OpenFile(FileError fe)
   {
    handle=WinNN::InvalidFileHandle;
    error=fe;
   }

  OpenFile(const WChar *file_name,FileOpenFlags oflags)
   {
    // flags & options

    WinNN::flags_t access_flags = MakeAccessFlags(oflags) ;

    WinNN::flags_t share_flags = MakeShareFlags(oflags) ;

    WinNN::options_t creation_options = MakeCreationOptions(oflags) ;

    WinNN::flags_t file_flags = MakeFileFlags(oflags) ;

    // open

    if( !set( WinNN::CreateFileW(file_name,access_flags,share_flags,0,creation_options,file_flags,0) ) ) return;

    // post-open

    if( oflags&Open_PosEnd )
      {
       if( !WinNN::SetFilePointerEx(handle,0,0,WinNN::FromEnd) )
         {
          close();

          return;
         }
      }

    if( oflags&Open_AutoDelete )
      {
       if( !AutoDelete(handle,true) )
         {
          close();

          return;
         }
      }
   }
 };

/* struct OpenAltFile */

struct OpenAltFile : AltFile::OpenType
 {
  bool set(WinNN::handle_t h_file)
   {
    handle=h_file;

    if( h_file==WinNN::InvalidFileHandle )
      {
       h_event=0;
       file_len=0;
       error=MakeError(FileError_OpenFault);

       return false;
      }
    else
      {
       error=FileError_Ok;

       return true;
      }
   }

  void close()
   {
    h_event=0;
    file_len=0;
    error=MakeError(FileError_OpenFault);

    WinNN::CloseHandle(handle); // ignore unprobable error

    handle=WinNN::InvalidFileHandle;
   }

  explicit OpenAltFile(FileError fe)
   {
    handle=WinNN::InvalidFileHandle;
    h_event=0;
    file_len=0;
    error=fe;
   }

  OpenAltFile(const WChar *file_name,FileOpenFlags oflags)
   {
    // flags & options

    WinNN::flags_t access_flags = MakeAccessFlags(oflags) ;

    WinNN::flags_t share_flags = MakeShareFlags(oflags) ;

    WinNN::options_t creation_options = MakeCreationOptions(oflags) ;

    WinNN::flags_t file_flags = MakeAltFileFlags(oflags) ;

    // open

    if( !set( WinNN::CreateFileW(file_name,access_flags,share_flags,0,creation_options,file_flags,0) ) ) return;

    // post-open

    if( !WinNN::GetFileSizeEx(handle,&file_len) )
      {
       close();

       return;
      }

    if( oflags&Open_AutoDelete )
      {
       if( !AutoDelete(handle,true) )
         {
          close();

          return;
         }
      }

    h_event=WinNN::CreateEventW(0,false,false,0);

    if( h_event==0 )
      {
       close();

       return;
      }
   }
 };

/* struct OpenAltAsyncFile */

struct OpenAltAsyncFile : AltAsyncFile::OpenType
 {
  bool set(WinNN::handle_t h_file)
   {
    handle=h_file;

    if( h_file==WinNN::InvalidFileHandle )
      {
       file_len=0;
       error=MakeError(FileError_OpenFault);

       return false;
      }
    else
      {
       error=FileError_Ok;

       return true;
      }
   }

  void close()
   {
    file_len=0;
    error=MakeError(FileError_OpenFault);

    WinNN::CloseHandle(handle); // ignore unprobable error

    handle=WinNN::InvalidFileHandle;
   }

  explicit OpenAltAsyncFile(FileError fe)
   {
    handle=WinNN::InvalidFileHandle;
    file_len=0;
    error=fe;
   }

  OpenAltAsyncFile(const WChar *file_name,FileOpenFlags oflags)
   {
    // flags & options

    WinNN::flags_t access_flags = MakeAccessFlags(oflags) ;

    WinNN::flags_t share_flags = MakeShareFlags(oflags) ;

    WinNN::options_t creation_options = MakeCreationOptions(oflags) ;

    WinNN::flags_t file_flags = MakeAltFileFlags(oflags) ;

    // open

    if( !set( WinNN::CreateFileW(file_name,access_flags,share_flags,0,creation_options,file_flags,0) ) ) return;

    // post-open

    if( !WinNN::GetFileSizeEx(handle,&file_len) )
      {
       close();

       return;
      }

    if( oflags&Open_AutoDelete )
      {
       if( !AutoDelete(handle,true) )
         {
          close();

          return;
         }
      }
   }
 };

/* FileClose() */

void FileClose(FileMultiError &errout,WinNN::handle_t handle,FileOpenFlags oflags,bool preserve_file)
 {
  if( oflags&Open_Write )
    {
     if( !WinNN::FlushFileBuffers(handle) )
       {
        preserve_file=false;

        AddErrorIf(errout,FileError_CloseFault,true);
       }
    }

  if( preserve_file )
    {
     AddErrorIf(errout,FileError_CloseFault, !AutoDelete(handle,false) );
    }

  AddErrorIf(errout,FileError_CloseFault, !WinNN::CloseHandle(handle) );
 }

} // namespace Private_SysFile

using namespace Private_SysFile;

/* struct File */

auto File::Open(StrLen file_name_,FileOpenFlags oflags) noexcept -> OpenType
 {
  FileName file_name;

  if( auto fe=file_name.prepare(file_name_) ) return OpenFile(fe);

  return OpenFile(file_name,oflags);
 }

void File::Close(FileMultiError &errout,Type handle,FileOpenFlags oflags,bool preserve_file) noexcept
 {
  FileClose(errout,handle,oflags,preserve_file);
 }

auto File::Write(Type handle,FileOpenFlags oflags,const uint8 *buf,ulen len) noexcept -> IOResult
 {
  IOResult ret;

  if( oflags&Open_Write )
    {
     ret.error=MakeErrorIf(FileError_WriteFault, !WinNN::ExtWriteFile(handle,buf,len,&ret.len) );
    }
  else
    {
     ret.len=0;
     ret.error=FileError_NoMethod;
    }

  return ret;
 }

auto File::Read(Type handle,FileOpenFlags oflags,uint8 *buf,ulen len) noexcept -> IOResult
 {
  IOResult ret;

  if( oflags&Open_Read )
    {
     ret.error=MakeErrorIf(FileError_ReadFault, !WinNN::ExtReadFile(handle,buf,len,&ret.len) );
    }
  else
    {
     ret.len=0;
     ret.error=FileError_NoMethod;
    }

  return ret;
 }

auto File::GetLen(Type handle,FileOpenFlags oflags) noexcept -> PosResult
 {
  PosResult ret;

  if( oflags&Open_Pos )
    {
     ret.error=MakeErrorIf(FileError_PosFault, !WinNN::GetFileSizeEx(handle,&ret.pos) );
    }
  else
    {
     ret.pos=0;
     ret.error=FileError_NoMethod;
    }

  return ret;
 }

auto File::GetPos(Type handle,FileOpenFlags oflags) noexcept -> PosResult
 {
  PosResult ret;

  if( oflags&Open_Pos )
    {
     ret.error=MakeErrorIf(FileError_PosFault, !WinNN::SetFilePointerEx(handle,0,&ret.pos,WinNN::FromCurrent) );
    }
  else
    {
     ret.pos=0;
     ret.error=FileError_NoMethod;
    }

  return ret;
 }

FileError File::SetPos(Type handle,FileOpenFlags oflags,FilePosType pos) noexcept
 {
  if( oflags&Open_Pos )
    {
     return MakeErrorIf(FileError_PosFault, !WinNN::SetFilePointerEx(handle,pos,0,WinNN::FromBegin) );
    }
  else
    {
     return FileError_NoMethod;
    }
 }

/* struct AltFile */

auto AltFile::Open(StrLen file_name_,FileOpenFlags oflags) noexcept -> OpenType
 {
  FileName file_name;

  if( auto fe=file_name.prepare(file_name_) ) return OpenAltFile(fe);

  return OpenAltFile(file_name,oflags);
 }

void AltFile::Close(FileMultiError &errout,Type handle,EventType h_event,FileOpenFlags oflags,bool preserve_file) noexcept
 {
  FileClose(errout,handle,oflags,preserve_file);

  AbortIf( !WinNN::CloseHandle(h_event) ,"CCore::Sys::AltFile::Close()");
 }

FileError AltFile::ShortWrite(Type handle,EventType h_event,FilePosType off,const uint8 *buf,WinNN::ushortlen_t len) noexcept
 {
  WinNN::Overlapped olap;
  WinNN::ushortlen_t ret_len;

  olap.internal=0;
  olap.internal_hi=0;
  olap.offset=off;
  olap.h_event=h_event;

  if( WinNN::WriteFile(handle,buf,len,&ret_len,&olap) )
    {
     // all done
    }
  else
    {
     auto error=WinNN::GetLastError();

     if( error==WinNN::ErrorIOPending )
       {
        if( !WinNN::GetOverlappedResult(handle,&olap,&ret_len,true) ) return MakeError(FileError_WriteFault);
       }
     else
       {
        return MakeError(FileError_WriteFault,error);
       }
    }

  if( ret_len!=len ) return FileError_WriteLenMismatch;

  return FileError_Ok;
 }

FileError AltFile::ShortRead(Type handle,EventType h_event,FilePosType off,uint8 *buf,WinNN::ushortlen_t len) noexcept
 {
  WinNN::Overlapped olap;
  WinNN::ushortlen_t ret_len;

  olap.internal=0;
  olap.internal_hi=0;
  olap.offset=off;
  olap.h_event=h_event;

  if( WinNN::ReadFile(handle,buf,len,&ret_len,&olap) )
    {
     // all done
    }
  else
    {
     auto error=WinNN::GetLastError();

     if( error==WinNN::ErrorIOPending )
       {
        if( !WinNN::GetOverlappedResult(handle,&olap,&ret_len,true) ) return MakeError(FileError_ReadFault);
       }
     else
       {
        return MakeError(FileError_ReadFault,error);
       }
    }

  if( ret_len!=len ) return FileError_ReadLenMismatch;

  return FileError_Ok;
 }

FileError AltFile::Write(Type handle,EventType h_event,FileOpenFlags oflags,FilePosType off,const uint8 *buf,ulen len) noexcept
 {
  if( oflags&Open_Write )
    {
     if( len>MaxUInt<FilePosType>-off ) return FileError_LenOverflow;

     if( len<=WinNN::SplitLen )
       {
        return ShortWrite(handle,h_event,off,buf,(WinNN::ushortlen_t)len);
       }
     else
       {
        while( len>2*WinNN::SplitLen )
          {
           if( auto error=ShortWrite(handle,h_event,off,buf,WinNN::SplitLen) ) return error;

           off+=WinNN::SplitLen;
           buf+=WinNN::SplitLen;
           len-=WinNN::SplitLen;
          }

        return ShortWrite(handle,h_event,off,buf,(WinNN::ushortlen_t)len);
       }
    }
  else
    {
     return FileError_NoMethod;
    }
 }

FileError AltFile::Read(Type handle,EventType h_event,FileOpenFlags oflags,FilePosType off,uint8 *buf,ulen len) noexcept
 {
  if( oflags&Open_Read )
    {
    if( len>MaxUInt<FilePosType>-off ) return FileError_LenOverflow;

    if( len<=WinNN::SplitLen )
      {
       return ShortRead(handle,h_event,off,buf,(WinNN::ushortlen_t)len);
      }
    else
      {
       while( len>2*WinNN::SplitLen )
         {
          if( auto error=ShortRead(handle,h_event,off,buf,WinNN::SplitLen) ) return error;

          off+=WinNN::SplitLen;
          buf+=WinNN::SplitLen;
          len-=WinNN::SplitLen;
         }

       return ShortRead(handle,h_event,off,buf,(WinNN::ushortlen_t)len);
      }
    }
  else
    {
     return FileError_NoMethod;
    }
 }

/* struct AltAsyncFile */

struct AltAsyncFile::AsyncState
 {
  WinNN::Overlapped olap;
 };

auto AltAsyncFile::Open(StrLen file_name_,FileOpenFlags oflags) noexcept -> OpenType
 {
  FileName file_name;

  if( auto fe=file_name.prepare(file_name_) ) return OpenAltAsyncFile(fe);

  return OpenAltAsyncFile(file_name,oflags);
 }

void AltAsyncFile::Close(FileMultiError &errout,Type handle,FileOpenFlags oflags,bool preserve_file) noexcept
 {
  FileClose(errout,handle,oflags,preserve_file);
 }

auto AltAsyncFile::StartRead(Type handle,Async async,FilePosType off,uint8 *buf,ulen len) noexcept -> RWResult
 {
  RWResult ret;

  if( len>WinNN::MaxUShortLen )
    {
     ret.error=FileError_BadLen;
     ret.pending=false;

     return ret;
    }

  WinNN::ushortlen_t ret_len;
  WinNN::Overlapped *olap=&async->olap;

  olap->internal=0;
  olap->internal_hi=0;
  olap->offset=off;

  if( WinNN::ReadFile(handle,buf,(WinNN::ushortlen_t)len,&ret_len,olap) )
    {
     if( ret_len==len )
       {
        ret.error=FileError_Ok;
       }
     else
       {
        ret.error=FileError_ReadLenMismatch;
       }

     ret.pending=false;

     return ret;
    }
  else
    {
     auto error=WinNN::GetLastError();

     if( error==WinNN::ErrorIOPending )
       {
        ret.error=FileError_Ok;
        ret.pending=true;

        return ret;
       }
     else
       {
        ret.error=MakeError(FileError_ReadFault,error);
        ret.pending=false;

        return ret;
       }
    }
 }

FileError AltAsyncFile::CompleteRead(Type handle,Async async,ulen len) noexcept
 {
  WinNN::ushortlen_t ret_len;

  if( !WinNN::GetOverlappedResult(handle,&async->olap,&ret_len,false) ) return MakeError(FileError_ReadFault);

  if( ret_len!=len ) return FileError_ReadLenMismatch;

  return FileError_Ok;
 }

auto AltAsyncFile::StartWrite(Type handle,Async async,FilePosType off,const uint8 *buf,ulen len) noexcept -> RWResult
 {
  RWResult ret;

  if( len>WinNN::MaxUShortLen )
    {
     ret.error=FileError_BadLen;
     ret.pending=false;

     return ret;
    }

  WinNN::ushortlen_t ret_len;
  WinNN::Overlapped *olap=&async->olap;

  olap->internal=0;
  olap->internal_hi=0;
  olap->offset=off;

  if( WinNN::WriteFile(handle,buf,(WinNN::ushortlen_t)len,&ret_len,olap) )
    {
     if( ret_len==len )
       {
        ret.error=FileError_Ok;
       }
     else
       {
        ret.error=FileError_WriteLenMismatch;
       }

     ret.pending=false;

     return ret;
    }
  else
    {
     auto error=WinNN::GetLastError();

     if( error==WinNN::ErrorIOPending )
       {
        ret.error=FileError_Ok;
        ret.pending=true;

        return ret;
       }
     else
       {
        ret.error=MakeError(FileError_WriteFault,error);
        ret.pending=false;

        return ret;
       }
    }
 }

FileError AltAsyncFile::CompleteWrite(Type handle,Async async,ulen len) noexcept
 {
  WinNN::ushortlen_t ret_len;

  if( !WinNN::GetOverlappedResult(handle,&async->olap,&ret_len,false) ) return MakeError(FileError_WriteFault);

  if( ret_len!=len ) return FileError_WriteLenMismatch;

  return FileError_Ok;
 }

/* class AsyncFileWait::Engine */

class AsyncFileWait::Engine : public WaitBase<AltAsyncFile::AsyncState,WaitFile>
 {
  public:

   explicit Engine(ulen async_count) : WaitBase<AltAsyncFile::AsyncState,WaitFile>(async_count) {}

   ~Engine() {}
 };

/* struct AsyncFileWait */

FileError AsyncFileWait::init(ulen async_count) noexcept
 {
  if( async_count>MaxAsyncs ) return FileError_BadLen;

  SilentReportException report;

  try
    {
     obj=new Engine(async_count);

     asyncs=obj->getAsyncs();

     return FileError_Ok;
    }
  catch(CatchType)
    {
     return FileError_SysOverload;
    }
 }

void AsyncFileWait::exit() noexcept
 {
  delete Replace_null(obj);

  asyncs=0;
 }

bool AsyncFileWait::addWait(ulen index) noexcept
 {
  return obj->addWait(index);
 }

bool AsyncFileWait::delWait(ulen index) noexcept
 {
  return obj->delWait(index);
 }

auto AsyncFileWait::wait(MSec timeout) noexcept -> WaitResult
 {
  return obj->wait(timeout);
 }

void AsyncFileWait::interrupt() noexcept
 {
  obj->interrupt();
 }

} // namespace Sys
} // namespace CCore


