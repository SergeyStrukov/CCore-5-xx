/* JobMachine.cpp */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/JobMachine.h>

#include <CCore/inc/Exception.h>
#include <CCore/inc/Abort.h>

namespace CCore {

/* class JobList */

JobList::JobList()
 {
 }

JobList::~JobList()
 {
  if( +normal || +pri )
    {
     Abort("Fatal error: non-empty JobList on exit");
    }
 }

void JobList::add(JobObject *job)
 {
  normal.ins_last(job);
 }

void JobList::addPri(JobObject *job)
 {
  pri.ins_last(job);
 }

JobObject * JobList::get()
 {
  if( JobObject *job=pri.del_first() ) return job;

  return normal.del_first();
 }

/* class JobQueue */

JobQueue::JobQueue()
 {
 }

JobQueue::~JobQueue()
 {
  if( +queue )
    {
     Abort("Fatal error: non-empty JobQueue on exit");
    }
 }

void JobQueue::add(JobObject *job)
 {
  queue.ins_last(job);
 }

JobObject * JobQueue::get()
 {
  return queue.del_first();
 }

void JobQueue::destroyAll()
 {
  while( JobObject *job=get() )
    {
     delete job;
    }
 }

/* class JobPool */

JobPool::JobPool()
 : mutex("JobPool.mutex"),
   asem("JobPool.asem")
 {
 }

JobPool::~JobPool()
 {
  list.destroyAll();
 }

void JobPool::fill(JobObject *job)
 {
  list.add(job);
 }

JobObject * JobPool::get()
 {
  JobObject *job;

  {
   Mutex::Lock lock(mutex);

   job=list.get();
  }

  if( job ) asem.inc();

  return job;
 }

void JobPool::put(JobObject *job)
 {
  {
   Mutex::Lock lock(mutex);

   list.add(job);
  }

  asem.dec();
 }

/* class JobMachine */

JobObject * JobMachine::getJob()
 {
  Mutex::Lock lock(mutex);

  return list.get();
 }

void JobMachine::work()
 {
  SilentReportException report;

  for(;;)
    {
     sem.take();

     if( JobObject *job=getJob() )
       {
        try
          {
           job->job(stop_flag!=0);

           report.guard();
          }
        catch(...)
          {
           ex_flag=1;
           report.clear();
          }
       }
     else
       {
        sem.give();
        return;
       }
    }
 }

JobMachine::JobMachine()
 : sem("JobMachine.sem"),
   mutex("JobMachine.mutex"),
   asem("JobMachine.asem")
 {
 }

JobMachine::~JobMachine()
 {
 }

void JobMachine::start(unsigned exec_count)
 {
  if( !exec_count )
    {
     Printf(Exception,"CCore::JobMachine::start(#;) : zero exec_count",exec_count);
    }

  while( exec_count-- )
    {
     asem.inc();

     RunFuncTask( [this] () { work(); } ,asem.function_dec());
    }
 }

void JobMachine::start()
 {
  start(Sys::GetCpuCount());
 }

void JobMachine::stop() noexcept
 {
  stop_flag=1;
  sem.give();
  asem.wait();

  SilentReportException report;

  while( JobObject *job=list.get() )
    {
     try
       {
        job->job(true);

        report.guard();
       }
     catch(...)
       {
        ex_flag=1;
        report.clear();
       }
    }

  if( ex_flag )
    {
     Printf(NoException,"CCore::JobMachine::~JobMachine() : exceptions had happend");
    }
 }

void JobMachine::add(JobObject *job)
 {
  {
   Mutex::Lock lock(mutex);

   list.add(job);
  }

  sem.give();
 }

void JobMachine::addPri(JobObject *job)
 {
  {
   Mutex::Lock lock(mutex);

   list.addPri(job);
  }

  sem.give();
 }

/* class JobMachineInit */

JobMachine * JobMachineInit::Object = 0 ;

JobMachineInit::JobMachineInit()
 {
  Object=&obj;

  obj.start();
 }

JobMachineInit::JobMachineInit(unsigned exec_count)
 {
  Object=&obj;

  obj.start(exec_count);
 }

JobMachineInit::~JobMachineInit()
 {
  obj.stop();

  Object=0;
 }

void JobMachineInit::Add(JobObject *job)
 {
  if( !Object ) Abort("Fatal error: no JobMachine Object");

  Object->add(job);
 }

void JobMachineInit::AddPri(JobObject *job)
 {
  if( !Object ) Abort("Fatal error: no JobMachine Object");

  Object->addPri(job);
 }

} // namespace CCore

