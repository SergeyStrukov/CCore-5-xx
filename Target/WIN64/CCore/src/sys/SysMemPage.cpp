/* SysMemPage.cpp */
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

#include <CCore/inc/sys/SysMemPage.h>
#include <CCore/inc/sys/SysAbort.h>
#include <CCore/inc/sys/SysTypes.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* consts */

static_assert( (MemPageLen%MaxAlign)==0 ,"CCore::Sys::MemPageLen is not aligned");

/* functions */

void * MemPageAlloc(ulen num_pages) noexcept
 {
  if( num_pages>MaxULen/MemPageLen ) return 0;

  ulen len=num_pages*MemPageLen;

  return WinNN::VirtualAlloc(0,len,WinNN::AllocReserve|WinNN::AllocCommit,WinNN::PageReadWrite);
 }

void MemPageFree(void *mem,ulen) noexcept
 {
  AbortIf( !WinNN::VirtualFree(mem,0,WinNN::FreeRelease) ,"CCore::Sys::MemPageFree()");
 }

bool MemPageExtend(void *,ulen,ulen) noexcept
 {
  // no OS support

  return false;
 }

ulen MemPageShrink(void *,ulen,ulen) noexcept
 {
  // no OS support

  return 0;
 }

} // namespace Sys
} // namespace CCore


