/* AbstFileToMem.cpp */
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

#include <CCore/inc/AbstFileToMem.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class AbstFileToMem */

AbstFileToMem::AbstFileToMem(AbstFileToRead file,StrLen file_name,ulen max_len)
 {
  file->open(file_name);

  ScopeGuard guard( [&] () { file->close(); } );

  auto file_len=file->getLen();

  if( file_len>max_len )
    {
     Printf(Exception,"CCore::AbstFileToMem::AbstFileToMem(...,#.q;,max_len=#;) : file is too long #;",file_name,max_len,file_len);
    }

  ulen len=(ulen)file_len;

  file->read_all(0,alloc(len),len);
 }

/* class PartAbstFileToMem */

PartAbstFileToMem::PartAbstFileToMem(AbstFileToRead file_,StrLen file_name,ulen buf_len)
 : file(file_),
   buf(buf_len),
   off(0)
 {
  file->open(file_name);

  file_len=file->getLen();
 }

PartAbstFileToMem::~PartAbstFileToMem()
 {
  file->close();
 }

PtrLen<const uint8> PartAbstFileToMem::pump()
 {
  uint8 *ptr=buf.getPtr();
  ulen len=buf.getLen();

  FilePosType delta=file_len-off;

  if( !delta ) return Empty;

  if( delta<len ) len=(ulen)delta;

  file->read_all(off,ptr,len);

  off+=len;

  return Range(ptr,len);
 }

} // namespace CCore

