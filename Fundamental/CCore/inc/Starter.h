/* Starter.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_Starter_h
#define CCore_inc_Starter_h

#include <CCore/inc/Task.h>

namespace CCore {

/* GuardStarterRunLock() */

void GuardStarterRunLock();

/* concept StarterFuncType<T> */

template <class T>
concept StarterFuncType = NothrowDefaultCtorType<T> &&
                          NothrowDtorType<T> &&
                          NothrowCopyableType<T> &&
                          FuncArgType<T> ;

/* classes */

template <StarterFuncType T> class Starter;

/* class Starter<T> */

 //
 // assume T has no-throw default, copy and assign
 //

template <StarterFuncType T>
class Starter : public Funchor_nocopy
 {
   T obj;
   bool terminate_flag = false ;

   bool flag = false ;
   unsigned task_count = 0 ;

   Sem * stop_sem = 0 ;

   bool terminate_request = false ;

   unsigned terminate_task_count = 0 ;

   Mutex mutex;
   Sem sem;

   Sem terminate_sem;
   Sem wait_terminate_sem;
   Sem stop_terminate_sem;

   Atomic run_lock;

  private:

   bool setObj(const T &obj_);

   bool setTerminate(unsigned terminate_task_count);

   bool doClear();

   bool clearObj(Sem *stop_sem_);

   bool getObj(T &ret);

   Sem * decCount();

  private:

   bool start(const T &obj);

   void stop(Sem &stop_sem);

   void terminate(unsigned task_count);

   bool take(T &ret);

   void complete();

   void job();

   Function<void (void)> function_job() { return FunctionOf(this,&Starter::job); }

  public:

   Starter();

   ~Starter();

   // control classes

   class Start;

   class Take;

   class Run;
 };

template <StarterFuncType T>
bool Starter<T>::setObj(const T &obj_)
 {
  Mutex::Lock lock(mutex);

  if( !flag )
    {
     obj=obj_;
     flag=true;

     return true;
    }

  return false;
 }

template <StarterFuncType T>
bool Starter<T>::setTerminate(unsigned terminate_task_count_)
 {
  Mutex::Lock lock(mutex);

  terminate_task_count=terminate_task_count_;

  if( !flag )
    {
     terminate_flag=true;
     flag=true;

     return true;
    }

  terminate_request=true;

  return false;
 }

template <StarterFuncType T>
bool Starter<T>::doClear()
 {
  obj=T();
  flag=false;

  if( terminate_request )
    {
     terminate_request=false;

     terminate_flag=true;
     flag=true;

     return true;
    }

  return false;
 }

template <StarterFuncType T>
bool Starter<T>::clearObj(Sem *stop_sem_)
 {
  bool give;

  {
   Mutex::Lock lock(mutex);

   if( task_count )
     {
      stop_sem=stop_sem_;

      return false;
     }

   give=doClear();
  }

  if( give ) terminate_sem.give();

  return true;
 }

template <StarterFuncType T>
bool Starter<T>::getObj(T &ret)
 {
  bool give=false;

  {
   Mutex::Lock lock(mutex);

   task_count++;

   if( !terminate_flag )
     {
      ret=obj;

      return false;
     }

   terminate_task_count--;

   if( !terminate_task_count ) give=true;
  }

  if( give ) wait_terminate_sem.give();

  return true;
 }

template <StarterFuncType T>
Sem * Starter<T>::decCount()
 {
  Sem *ret;
  bool give;

  {
   Mutex::Lock lock(mutex);

   task_count--;

   if( task_count || !stop_sem ) return 0;

   give=doClear();

   ret=Replace_null(stop_sem);
  }

  if( give ) terminate_sem.give();

  return ret;
 }

template <StarterFuncType T>
bool Starter<T>::start(const T &obj_)
 {
  if( setObj(obj_) )
    {
     sem.give();

     return true;
    }

  return false;
 }

template <StarterFuncType T>
void Starter<T>::stop(Sem &stop_sem)
 {
  sem.take();

  if( !clearObj(&stop_sem) ) stop_sem.take();
 }

template <StarterFuncType T>
void Starter<T>::terminate(unsigned task_count)
 {
  if( !task_count ) return;

  // start termination

  if( !setTerminate(task_count) ) terminate_sem.take();

  sem.give();

  // wait tasks

  wait_terminate_sem.take();

  // stop termination

  stop(stop_terminate_sem);
 }

template <StarterFuncType T>
bool Starter<T>::take(T &ret)
 {
  sem.take();

  bool ret_flag=getObj(ret);

  sem.give();

  return ret_flag;
 }

template <StarterFuncType T>
void Starter<T>::complete()
 {
  if( Sem *stop_sem=decCount() ) stop_sem->give();
 }

template <StarterFuncType T>
Starter<T>::Starter()
 : obj(),
   mutex("Starter.mutex"),
   sem("Starter.sem"),
   terminate_sem("Starter.terminate_sem"),
   wait_terminate_sem("Starter.wait_terminate_sem"),
   stop_terminate_sem("Starter.stop_terminate_sem")
 {
 }

template <StarterFuncType T>
Starter<T>::~Starter()
 {
 }

/* class Starter<T>::Start */

template <StarterFuncType T>
class Starter<T>::Start : NoCopy
 {
   Starter<T> *starter;
   Sem stop_sem;

  public:

   Start(Starter<T> *starter_,const T &obj)
    : starter(starter_),
      stop_sem("Starter::Start")
    {
     if( starter && !starter->start(obj) ) starter=0;
    }

   ~Start()
    {
     if( starter ) starter->stop(stop_sem);
    }

   bool operator + () const { return starter; }

   bool operator ! () const { return !starter; }
 };

/* class Starter<T>::Take */

template <StarterFuncType T>
class Starter<T>::Take : NoCopy
 {
   Starter<T> &starter;

  public:

   T obj;
   bool terminate_flag;

   explicit Take(Starter<T> &starter_)
    : starter(starter_)
    {
     terminate_flag=starter.take(obj);
    }

   ~Take()
    {
     starter.complete();
    }
 };

template <StarterFuncType T>
void Starter<T>::job()
 {
  for(;;)
    {
     Take take(*this);

     if( take.terminate_flag ) break;

     take.obj();
    }
 }

/* class Starter<T>::Run */

 //
 // Only single Run per Starter
 //

template <StarterFuncType T>
class Starter<T>::Run : NoCopy
 {
   Starter<T> &starter;
   unsigned task_count;
   AntiSem asem;

  public:

   explicit Run(Starter<T> &starter_)
    : starter(starter_),
      task_count(0),
      asem("Starter::Run")
    {
     if( starter_.run_lock++ )
       {
        starter_.run_lock--;

        GuardStarterRunLock();
       }
    }

   ~Run()
    {
     starter.terminate(task_count);

     asem.wait();
    }

   unsigned getTaskCount() const { return task_count; }

   unsigned run(unsigned cnt)
    {
     for(; cnt ;cnt--)
       {
        asem.inc();

        RunFuncTask(starter.function_job(),asem.function_dec());

        task_count++;
       }

     return task_count;
    }
 };

} // namespace CCore

#endif


