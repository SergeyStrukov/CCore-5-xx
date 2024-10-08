/* AsyncFileToMem.cpp */
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

#include <CCore/inc/AsyncFileToMem.h>

#include <CCore/inc/Exception.h>
#include <CCore/inc/Path.h>
#include <CCore/inc/ScanRange.h>

namespace CCore {

/* class AsyncFileToMem */

AsyncFileToMem::AsyncFileToMem(StrLen file_name,ulen max_len,MSec timeout,ulen max_packets)
 {
  AsyncFile file(timeout,max_packets);

  auto file_len=file.open(file_name,Open_Read);

  if( file_len>max_len )
    {
     Printf(Exception,"CCore::AsyncFileToMem::AsyncFileToMem(#.q;,max_len=#;,...) : file is too long #;",file_name,max_len,file_len);
    }

  ulen len=(ulen)file_len;

  file.read_all(0,alloc(len),len);
 }

AsyncFileToMem::AsyncFileToMem(StrLen dev_name,StrLen dev_file_name,ulen max_len,MSec timeout,ulen max_packets)
 {
  AsyncFile file(timeout,max_packets);

  auto file_len=file.open(dev_name,dev_file_name,Open_Read);

  if( file_len>max_len )
    {
     Printf(Exception,"CCore::AsyncFileToMem::AsyncFileToMem(#.q;,#.q;,max_len=#;,...) : file is too long #;",dev_name,dev_file_name,max_len,file_len);
    }

  ulen len=(ulen)file_len;

  file.read_all(0,alloc(len),len);
 }

AsyncFileToMem::~AsyncFileToMem()
 {
 }

/* class AsyncFileListToMem */

void AsyncFileListToMem::build(AsyncFileSystem &fs,StrLen dev_path,ulen max_len,MSec timeout,ulen max_packets)
 {
  AsyncFile file(timeout,max_packets);

  auto file_len=fs.getFileList(dev_path,file);

  if( file_len>max_len )
    {
     Printf(Exception,"CCore::AsyncFileListToMem::build(,#.q;,max_len=#;,...) : file is too long #;",dev_path,max_len,file_len);
    }

  ulen len=(ulen)file_len;

  file.read_all(0,alloc(len),len);

  data=Range(*this);

  file_type=FileType_none;
 }

AsyncFileListToMem::AsyncFileListToMem(StrLen path,ulen max_len,MSec timeout,ulen max_packets)
 {
  SplitDevName split(path);

  if( !split )
    {
     Printf(Exception,"CCore::AsyncFileListToMem::AsyncFileListToMem(#.q;,...) : bad name",path);
    }

  AsyncFileSystem fs(split.dev_name,timeout,max_packets);

  build(fs,split.dev_path,max_len,timeout,max_packets);
 }

AsyncFileListToMem::AsyncFileListToMem(AsyncFileSystem &fs,StrLen dev_path,ulen max_len,MSec timeout,ulen max_packets)
 {
  build(fs,dev_path,max_len,timeout,max_packets);
 }

AsyncFileListToMem::~AsyncFileListToMem()
 {
 }

bool AsyncFileListToMem::next()
 {
  if( !data ) return false;

  ScanRange scan(data, [=] (uint8 elem) { return elem=='\n'; } );

  if( !scan.next )
    {
     Printf(Exception,"CCore::AsyncFileListToMem::next() : no \\n");
    }

  PtrLen<const uint8> prefix=scan.before;

  if( prefix.len<3 )
    {
     Printf(Exception,"CCore::AsyncFileListToMem::next() : short line");
    }

  if( prefix.back(2)!=' ' )
    {
     Printf(Exception,"CCore::AsyncFileListToMem::next() : no space");
    }

  switch( prefix.back(1) )
    {
     case 'f' :
      {
       file_type=FileType_file;
      }
     break;

     case 'd' :
      {
       file_type=FileType_dir;
      }
     break;

     default:
      {
       Printf(Exception,"CCore::AsyncFileListToMem::next() : bad type");
      }
    }

  file_name=prefix.inner(0,2);

  data=scan.next.part(1);

  return true;
 }

/* class PartAsyncFileToMem */

PartAsyncFileToMem::PartAsyncFileToMem(StrLen file_name,ulen buf_len,MSec timeout,ulen max_packets)
 : file(timeout,max_packets),
   buf(buf_len),
   off(0)
 {
  file_len=file.open(file_name,Open_ToRead);
 }

PartAsyncFileToMem::PartAsyncFileToMem(StrLen dev_name,StrLen dev_file_name,ulen buf_len,MSec timeout,ulen max_packets)
 : file(timeout,max_packets),
   buf(buf_len),
   off(0)
 {
  file_len=file.open(dev_name,dev_file_name,Open_ToRead);
 }

PartAsyncFileToMem::~PartAsyncFileToMem()
 {
 }

PtrLen<const uint8> PartAsyncFileToMem::pump()
 {
  uint8 *ptr=buf.getPtr();
  ulen len=buf.getLen();

  FilePosType delta=file_len-off;

  if( !delta ) return Empty;

  if( delta<len ) len=(ulen)delta;

  file.read_all(off,ptr,len);

  off+=len;

  return Range(ptr,len);
 }

} // namespace CCore


