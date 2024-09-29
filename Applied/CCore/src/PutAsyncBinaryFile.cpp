/* PutAsyncBinaryFile.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/PutAsyncBinaryFile.h>

#include <CCore/inc/FeedBuf.h>
#include <CCore/inc/Exception.h>

namespace CCore {

/* class PutAsyncBinaryFile */

void PutAsyncBinaryFile::provide()
 {
  flush();

  auto result=file.getWritePacket();

  packet=result.packet;

  out=result.buf;
  buf_len=out.len;
 }

PutAsyncBinaryFile::PutAsyncBinaryFile(MSec timeout,ulen max_packets)
 : file(timeout,max_packets)
 {
 }

PutAsyncBinaryFile::PutAsyncBinaryFile(StrLen file_name,FileOpenFlags oflags,MSec timeout,ulen max_packets)
 : file(timeout,max_packets)
 {
  open(file_name,oflags);
 }

PutAsyncBinaryFile::PutAsyncBinaryFile(StrLen dev_name,StrLen dev_file_name,FileOpenFlags oflags,MSec timeout,ulen max_packets)
 : file(timeout,max_packets)
 {
  open(dev_name,dev_file_name,oflags);
 }

PutAsyncBinaryFile::~PutAsyncBinaryFile()
 {
  flush();
 }

void PutAsyncBinaryFile::open(StrLen file_name,FileOpenFlags oflags)
 {
  FilePosType file_len=file.open(file_name,oflags);

  file_pos=(oflags&Open_PosEnd)?file_len:0;
 }

void PutAsyncBinaryFile::open(StrLen dev_name,StrLen dev_file_name,FileOpenFlags oflags)
 {
  FilePosType file_len=file.open(dev_name,dev_file_name,oflags);

  file_pos=(oflags&Open_PosEnd)?file_len:0;
 }

void PutAsyncBinaryFile::close(FileMultiError &errout)
 {
  flush();

  file.close(errout);
 }

void PutAsyncBinaryFile::close()
 {
  flush();

  file.close();
 }

void PutAsyncBinaryFile::do_put(const uint8 *ptr,ulen len)
 {
  auto src=Range(ptr,len);

  while( +src )
    {
     if( !out ) provide();

     Pumpup(out,src);
    }
 }

void PutAsyncBinaryFile::flush()
 {
  if( +packet )
    {
     ulen len=buf_len-out.len;

     out=Nothing;

     file.write(file_pos,len,Replace_null(packet));

     file_pos+=len;
    }
 }

} // namespace CCore


