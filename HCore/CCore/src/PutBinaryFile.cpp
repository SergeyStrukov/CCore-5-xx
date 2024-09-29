/* PutBinaryFile.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/PutBinaryFile.h>

#include <CCore/inc/FeedBuf.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class PutBinaryFile */

void PutBinaryFile::provide()
 {
  if( !isOpened() )
    {
     Printf(Exception,"CCore::PutBinaryFile::provide() : file is not opened");
    }

  flush();

  out=Range(buf);
  has_data=true;
 }

PutBinaryFile::PutBinaryFile() noexcept
 {
 }

PutBinaryFile::PutBinaryFile(StrLen file_name,FileOpenFlags oflags)
 : PutBinaryFile()
 {
  open(file_name,oflags);
 }

PutBinaryFile::~PutBinaryFile()
 {
  if( isOpened() )
    {
     FileMultiError errout;

     close(errout);

     if( +errout )
       {
        Printf(NoException,"CCore::PutBinaryFile::~PutBinaryFile() : #;",errout);
       }
    }
 }

void PutBinaryFile::open(StrLen file_name,FileOpenFlags oflags)
 {
  if( buf.isEmpty() )
    {
     buf=SimpleArray<uint8>(BufLen);
    }

  if( FileError error=file.open(file_name,oflags) )
    {
     Printf(Exception,"CCore::PutBinaryFile::open(#.q;,#;) : #;",file_name,oflags,error);
    }

  no_flush_exception=false;
  out=Empty;
  has_data=false;
 }

void PutBinaryFile::disableExceptions()
 {
  if( !no_flush_exception )
    {
     no_flush_exception=true;

     flush_error=FileError_Ok;
    }
 }

void PutBinaryFile::close(FileMultiError &errout)
 {
  if( isOpened() )
    {
     disableExceptions();

     flush();

     errout.add(flush_error);

     file.close(errout);
    }
  else
    {
     errout.add(FileError_NoMethod);
    }
 }

void PutBinaryFile::close()
 {
  FileMultiError errout;

  close(errout);

  if( +errout )
    {
     Printf(Exception,"CCore::PutBinaryFile::close() : #;",errout);
    }
 }

void PutBinaryFile::do_put(const uint8 *ptr,ulen len)
 {
  auto src=Range(ptr,len);

  while( +src )
    {
     if( !out ) provide();

     Pumpup(out,src);
    }
 }

void PutBinaryFile::flush()
 {
  if( !has_data ) return;

  ulen len=buf.getLen()-out.len;

  out=Empty;
  has_data=false;

  if( !len ) return;

  if( FileError error=file.write(buf.getPtr(),len) )
    {
     if( no_flush_exception )
       {
        flush_error=error;
       }
     else
       {
        Printf(Exception,"CCore::PutBinaryFile::flush(...) : #;",error);
       }
    }
 }

} // namespace CCore


