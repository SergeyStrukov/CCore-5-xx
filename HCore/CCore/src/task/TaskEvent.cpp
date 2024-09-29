/* TaskEvent.cpp */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/task/TaskEvent.h>

namespace CCore {

/* global TaskEventHost */

TaskEventHostType TaskEventHost CCORE_INITPRI_1 ;

/* class TickTask */

void TickTask::task()
 {
  while( !stop_flag )
    {
     recorder.setTime(timer.get());

     if( time_scope.nextScope_skip() )
       {
        TaskEventHost.tick(); // 10 Hz
       }
    }
 }

TickTask::TickTask(TaskEventRecorder &recorder_)
 : recorder(recorder_),
   time_scope(100_msec)
 {
  RunFuncTask(function_task(),wait_stop.function_trigger());
 }

TickTask::~TickTask()
 {
  stop_flag=1;

  wait_stop.wait();
 }

} // namespace CCore


