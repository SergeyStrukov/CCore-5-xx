/* GetAsyncBinaryFile.cpp */
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

#include <CCore/inc/GetAsyncBinaryFile.h>

#include <CCore/inc/FeedBuf.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class GetAsyncBinaryFile */

void GetAsyncBinaryFile::underflow()
 {
  cur=engine.pump();

  if( !cur )
    {
     Printf(Exception,"CCore::GetAsyncBinaryFile::underflow() : EOF");
    }
 }

bool GetAsyncBinaryFile::underflow_eof()
 {
  cur=engine.pump();

  return +cur;
 }

 // constructors

GetAsyncBinaryFile::GetAsyncBinaryFile(MSec timeout,ulen max_packets)
 : engine(timeout,max_packets)
 {
 }

GetAsyncBinaryFile::GetAsyncBinaryFile(StrLen file_name,MSec timeout,ulen max_packets)
 : engine(file_name,timeout,max_packets)
 {
 }

GetAsyncBinaryFile::GetAsyncBinaryFile(StrLen dev_name,StrLen dev_file_name,MSec timeout,ulen max_packets)
 : engine(dev_name,dev_file_name,timeout,max_packets)
 {
 }

GetAsyncBinaryFile::~GetAsyncBinaryFile() {}

 // methods

void GetAsyncBinaryFile::open(StrLen file_name)
 {
  engine.open(file_name);

  reset();
 }

void GetAsyncBinaryFile::open(StrLen dev_name,StrLen dev_file_name)
 {
  engine.open(dev_name,dev_file_name);

  reset();
 }

void GetAsyncBinaryFile::close(FileMultiError &errout)
 {
  reset();

  engine.close(errout);
 }

void GetAsyncBinaryFile::close()
 {
  reset();

  engine.close();
 }

 // get

void GetAsyncBinaryFile::do_get(uint8 *ptr,ulen len)
 {
  auto dst=Range(ptr,len);

  while( +dst )
    {
     if( !cur ) underflow();

     Pumpup(dst,cur);
    }
 }

} // namespace CCore


