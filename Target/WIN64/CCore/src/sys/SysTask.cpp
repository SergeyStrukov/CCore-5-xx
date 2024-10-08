/* SysTask.cpp */
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

#include <CCore/inc/sys/SysTask.h>
#include <CCore/inc/sys/SysAtomic.h>
#include <CCore/inc/sys/SysTypes.h>

#include <CCore/inc/win64/Win64.h>

#include <pthread.h>

namespace CCore {
namespace Sys {

/* assert */

static_assert( MaxUInt<TaskIdType> <= MaxUInt<Atomic::Type> ,"CCore::Sys::TaskIdType : too long type");

/* functions */

void YieldTask() noexcept { WinNN::Sleep(0); }

void SleepTask(MSec time) noexcept { WinNN::Sleep(+time); }

TaskIdType GetTaskId() noexcept { return WinNN::GetCurrentThreadId(); }

/* struct TaskEntry */

void * TaskEntry::Entry(void *obj_)
 {
  TaskEntry *obj=static_cast<TaskEntry *>(obj_);

  obj->entry(obj);

  return 0;
 }

ErrorType TaskEntry::create() noexcept
 {
  pthread_t tid;

  if( pthread_create(&tid,0,Entry,this) ) return Error_Task;

  return NoError;
 }

} // namespace Sys
} // namespace CCore


