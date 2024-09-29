/* GetBinaryFile.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/GetBinaryFile.h>

#include <CCore/inc/FeedBuf.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class GetBinaryFile */

void GetBinaryFile::underflow()
 {
  uint8 *ptr=buf.getPtr();

  auto result=file.read(ptr,buf.getLen());

  if( result.error )
    {
     Printf(Exception,"CCore::GetBinaryFile::underflow() : #;",result.error);
    }
  else if( !result.len )
    {
     Printf(Exception,"CCore::GetBinaryFile::underflow() : EOF");
    }
  else
    {
     cur=Range_const(ptr,result.len);
    }
 }

bool GetBinaryFile::underflow_eof()
 {
  uint8 *ptr=buf.getPtr();

  auto result=file.read(ptr,buf.getLen());

  if( result.error )
    {
     Printf(Exception,"CCore::GetBinaryFile::underflow_eof() : #;",result.error);

     return false;
    }
  else if( !result.len )
    {
     return false;
    }
  else
    {
     cur=Range_const(ptr,result.len);

     return true;
    }
 }

GetBinaryFile::GetBinaryFile() noexcept
 {
 }

GetBinaryFile::GetBinaryFile(StrLen file_name)
 : GetBinaryFile()
 {
  open(file_name);
 }

GetBinaryFile::~GetBinaryFile()
 {
  if( isOpened() )
    {
     FileMultiError errout;

     close(errout);

     if( +errout )
       {
        Printf(NoException,"CCore::GetBinaryFile::~GetBinaryFile() : #;",errout);
       }
    }
 }

 // methods

void GetBinaryFile::open(StrLen file_name)
 {
  if( buf.isEmpty() )
    {
     buf=SimpleArray<uint8>(BufLen);
    }

  if( FileError error=file.open(file_name) )
    {
     Printf(Exception,"CCore::GetBinaryFile::open(#.q;,...) : #;",file_name,error);
    }

  reset();
 }

void GetBinaryFile::close(FileMultiError &errout)
 {
  if( isOpened() )
    {
     file.close(errout);

     reset();
    }
  else
    {
     errout.add(FileError_NoMethod);
    }
 }

void GetBinaryFile::close()
 {
  FileMultiError errout;

  close(errout);

  if( +errout )
    {
     Printf(Exception,"CCore::GetBinaryFile::close() : #;",errout);
    }
 }

 // get

void GetBinaryFile::do_get(uint8 *ptr,ulen len)
 {
  auto dst=Range(ptr,len);

  while( +dst )
    {
     if( !cur ) underflow();

     Pumpup(dst,cur);
    }
 }

} // namespace CCore

