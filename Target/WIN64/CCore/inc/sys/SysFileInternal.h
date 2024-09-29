/* SysFileInternal.h */
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

#include <CCore/inc/sys/SysTypes.h>
#include <CCore/inc/sys/SysUtf8.h>

#include <CCore/inc/GenFile.h>
#include <CCore/inc/MemBase.h>

//#include <CCore/inc/Print.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* functions */

inline FileError MakeError(FileError fe,WinNN::error_t error)
 {
  switch( error )
    {
     case WinNN::ErrorFileNotFound     : return FileError_NoFile;
     case WinNN::ErrorPathNotFound     : return FileError_NoPath;
     case WinNN::ErrorTooManyOpenFiles : return FileError_SysOverload;
     case WinNN::ErrorAccessDenied     : return FileError_NoAccess;
     case WinNN::ErrorNotEnoughMemory  : return FileError_SysOverload;
     case WinNN::ErrorOutOfMemory      : return FileError_SysOverload;
     case WinNN::ErrorInvalidDrive     : return FileError_NoDevice;
     case WinNN::ErrorWriteProtect     : return FileError_WriteProtect;
     case WinNN::ErrorSeek             : return FileError_PosFault;
     case WinNN::ErrorWriteFault       : return FileError_WriteFault;
     case WinNN::ErrorReadFault        : return FileError_ReadFault;
     case WinNN::ErrorHandleEOF        : return FileError_BadPosition;
     case WinNN::ErrorFileExists       : return FileError_FileExist;
     case WinNN::ErrorDiskFull         : return FileError_DiskFull;
     case WinNN::ErrorInvalidHandle    : return FileError_BadId;
     case WinNN::ErrorInvalidName      : return FileError_BadName;
     case WinNN::ErrorAlreadyExists    : return FileError_FileExist;
     case WinNN::ErrorDirNotEmpty      : return FileError_DirIsNotEmpty;

     default:
      {
       //Printf(Con,"error = #;\n",error);

       return fe;
      }
    }
 }

inline FileError MakeError(FileError fe)
 {
  return MakeError(fe,WinNN::GetLastError());
 }

inline FileError MakeErrorIf(FileError fe,bool nok)
 {
  if( nok ) return MakeError(fe);

  return FileError_Ok;
 }

inline void AddErrorIf(FileMultiError &errout,FileError fe,bool nok)
 {
  if( nok ) errout.add(MakeError(fe));
 }

inline CmpFileTimeType ToCmpFileTime(WinNN::FileTime ft)
 {
  CmpFileTimeType ret=ft.lo|(CmpFileTimeType(ft.hi)<<32);

  if( !ret ) ret=1;

  return ret;
 }

/* classes */

template <PODType T> class TempBuf;

struct MakeZStr;

struct FileName;

/* class TempBuf<T> */

template <PODType T>
class TempBuf : NoCopy
 {
   T *ptr;

  public:

   explicit TempBuf(ulen len) noexcept
    {
     const ulen MaxLen = MaxULen/sizeof (T) ;

     if( len<=MaxLen )
       {
        ptr=static_cast<T *>( TryMemAlloc(len*sizeof (T)) );
       }
     else
       {
        ptr=0;
       }
    }

   ~TempBuf() { MemFree(ptr); }

   T * operator + () const { return ptr; }

   operator T * () const { return ptr; }
 };

/* struct MakeZStr */

struct MakeZStr
 {
  FileError error;
  ulen len;

  void setError(FileError error_) { error=error_; len=0; }

  void setLen(ulen len_) { error=FileError_Ok; len=len_; }

  void set(ToWChar result);

  MakeZStr(StrLen str,PtrLen<WChar> out);

  MakeZStr(StrLen str1,StrLen str2,PtrLen<WChar> out);
 };

/* struct FileName */

struct FileName : NoCopy
 {
  WChar wbuf[MaxPathLen+1];
  ulen len; // without zero

  operator const WChar * () const { return wbuf; }

  FileError prepare(StrLen str)
   {
    MakeZStr result(str,Range(wbuf));

    len=result.len;

    return result.error;
   }

  FileError prepare(StrLen str1,StrLen str2)
   {
    MakeZStr result(str1,str2,Range(wbuf));

    len=result.len;

    return result.error;
   }
 };

} // namespace Sys
} // namespace CCore

