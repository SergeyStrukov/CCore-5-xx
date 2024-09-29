/* SysSem.cpp */
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

#include <CCore/inc/sys/SysSem.h>

#include <CCore/inc/sys/SysAbort.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* struct Sem */

auto Sem::Create(ulen count) noexcept -> CreateType
 {
  CreateType ret;

  if( count>(ulen)WinNN::MaxSemaphoreCount )
    {
     ret.handle=0;

     ret.error=ErrorType(WinNN::ErrorInvalidParameter);
    }
  else
    {
     ret.handle=WinNN::CreateSemaphoreW(0,(WinNN::sem_count_t)count,WinNN::MaxSemaphoreCount,0);

     ret.error=ErrorIf( ret.handle==0 );
    }

  return ret;
 }

void Sem::Destroy(Type handle) noexcept
 {
  AbortIf( !WinNN::CloseHandle(handle) ,"CCore::Sys::Sem::Destroy()");
 }

void Sem::Give(Type handle) noexcept
 {
  AbortIf( !WinNN::ReleaseSemaphore(handle,1,0) ,"CCore::Sys::Sem::Give()");
 }

void Sem::GiveMany(Type handle,ulen count) noexcept
 {
  if( !count ) return;

  AbortIf( count>(ulen)WinNN::MaxSemaphoreCount || !WinNN::ReleaseSemaphore(handle,(WinNN::sem_count_t)count,0) ,"CCore::Sys::Sem::GiveMany()");
 }

bool Sem::TryTake(Type handle) noexcept
 {
  WinNN::options_t ret=WinNN::WaitForSingleObject(handle,WinNN::TryTimeout);

  AbortIf( ret!=WinNN::WaitObject_0 && ret!=WinNN::WaitTimeout ,"CCore::Sys::Sem::TryTake()");

  return ret==WinNN::WaitObject_0;
 }

void Sem::Take(Type handle) noexcept
 {
  AbortIf( WinNN::WaitForSingleObject(handle,WinNN::NoTimeout)!=WinNN::WaitObject_0 ,"CCore::Sys::Sem::Take()");
 }

bool Sem::Take(Type handle,MSec timeout) noexcept
 {
  WinNN::options_t ret=WinNN::WaitForSingleObject(handle,+timeout);

  AbortIf( ret!=WinNN::WaitObject_0 && ret!=WinNN::WaitTimeout ,"CCore::Sys::Sem::Take()");

  return ret==WinNN::WaitObject_0;
 }

} // namespace Sys
} // namespace CCore


