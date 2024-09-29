/* SysTlsSlot.cpp */
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

#include <CCore/inc/sys/SysTlsSlot.h>
#include <CCore/inc/sys/SysAbort.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* struct TlsSlot */

auto TlsSlot::Alloc() noexcept -> AllocType
 {
  TlsSlot::AllocType ret;

  ret.index=WinNN::TlsAlloc();

  ret.error=ErrorIf( ret.index==WinNN::TlsNoIndex );

  return ret;
 }

void TlsSlot::Free(Type index) noexcept
 {
  AbortIf( !WinNN::TlsFree(index) ,"CCore::Sys::TlsSlot::Free()");
 }

void * TlsSlot::Get(Type index) noexcept
 {
  return WinNN::TlsGetValue(index);
 }

void TlsSlot::Set(Type index,void *value) noexcept
 {
  AbortIf( !WinNN::TlsSetValue(index,value) ,"CCore::Sys::TlsSlot::Set()");
 }

} // namespace Sys
} // namespace CCore


