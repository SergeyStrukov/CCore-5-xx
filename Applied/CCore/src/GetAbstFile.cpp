/* GetAbstFile.cpp */
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

#include <CCore/inc/GetAbstFile.h>

#include <CCore/inc/FeedBuf.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class GetAbstFile */

void GetAbstFile::underflow()
 {
  uint8 *ptr=buf.getPtr();

  ulen len=file->read(off,ptr,buf.getLen());

  if( !len )
    {
     Printf(Exception,"CCore::GetAbstFile::underflow() : EOF");
    }
  else
    {
     cur=Range_const(ptr,len);

     off+=len;
    }
 }

bool GetAbstFile::underflow_eof()
 {
  uint8 *ptr=buf.getPtr();

  ulen len=file->read(off,ptr,buf.getLen());

  if( !len )
    {
     return false;
    }
  else
    {
     cur=Range_const(ptr,len);

     off+=len;

     return true;
    }
 }

 // constructors

GetAbstFile::GetAbstFile(AbstFileToRead file_,StrLen file_name)
 : file(file_),
   buf(BufLen)
 {
  file->open(file_name);
 }

GetAbstFile::~GetAbstFile()
 {
  file->close();
 }

 // get

void GetAbstFile::do_get(uint8 *ptr,ulen len)
 {
  auto dst=Range(ptr,len);

  while( +dst )
    {
     if( !cur ) underflow();

     Pumpup(dst,cur);
    }
 }

} // namespace CCore

