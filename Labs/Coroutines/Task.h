/* Task.h */
//----------------------------------------------------------------------------------------
//
//  Project: Labs/Coroutines
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef App_Task_h
#define App_Task_h

#include <CCore/inc/MemBase.h>
#include <CCore/inc/Exception.h>

#include <coroutine>
#include <exception>

namespace App {

/* using */

using namespace CCore;

/* type cotask */

using cotask = std::coroutine_handle<> ;

/* classes */

struct ResumeTask;

template <class T> class GeneratorPromise;

template <class T> class Task;

/* struct ResumeTask */

struct ResumeTask
 {
  cotask handle;

  ResumeTask(cotask handle_)
   : handle(handle_)
   {
    if( !handle || handle.done() )
      {
       Printf(Con,"App::ResumeTask(...) : finished");
      }
   }

  template <class T>
  ResumeTask(std::coroutine_handle<T> handle_) : handle(handle_)
   {
    if( !handle || handle.done() )
      {
       Printf(Con,"App::ResumeTask(...) : finished");
      }
   }

  bool await_ready() { return false; }

  auto await_suspend(cotask) { return handle; }

  void await_resume() {}
 };

/* class TaskPromise<T> */

template <class T>
class TaskPromise : public MemBase
 {
   T value;
   std::exception_ptr exception;

  public:

   TaskPromise() noexcept : value{},exception{} {}

   Task<T> get_return_object() noexcept;

   std::suspend_always initial_suspend() const noexcept { return {}; }

   std::suspend_always final_suspend() const noexcept { return {}; }

   void unhandled_exception() { exception=std::current_exception(); }

   void return_value(const T &value_) { value=value_; }

   // methods

   T getValue() const { return value; }

   void rethrowException() { if( exception ) std::rethrow_exception(exception); }
 };

template <>
class TaskPromise<void> : public MemBase
 {
   std::exception_ptr exception;

  public:

   TaskPromise() noexcept : exception{} {}

   Task<void> get_return_object() noexcept;

   std::suspend_always initial_suspend() const noexcept { return {}; }

   std::suspend_always final_suspend() const noexcept { return {}; }

   void unhandled_exception() { exception=std::current_exception(); }

   void return_void() {}

   // methods

   void rethrowException() { if( exception ) std::rethrow_exception(exception); }
 };

/* class Task<T> */

template <class T>
class Task
 {
  public:

   using promise_type = TaskPromise<T> ;

   using HandleType = std::coroutine_handle<promise_type> ;

  private:

   HandleType handle;

  private:

   static void Destroy(HandleType handle) { if( handle ) handle.destroy(); }

  public:

   // constructors

   Task(HandleType handle_={}) noexcept : handle(handle_) {}

   ~Task() { Destroy(handle); }

   // no copy

   Task(const Task &obj) = delete ;

   Task & operator = (Task &obj) = delete ;

   // std move

   Task(Task &&obj) : handle(Replace_null(obj.handle)) {}

   Task & operator = (Task &&obj)
    {
     if( this!=&obj )
       {
        Destroy(Replace(handle,Replace_null(obj.handle)));
       }

     return *this;
    }

   // methods

   bool operator + () const { return bool(handle); }

   bool operator ! () const { return !handle; }

   ResumeTask getResumer() const { return handle; }

   bool done() const { return handle.done(); }

   struct PushResult
    {
     T value;
     bool ok;

     PushResult() : value{},ok(false) {}

     PushResult(const T &value_) : value(value_),ok(true) {}
    };

   PushResult push() const
    {
     handle.resume();

     if( handle.done() )
       {
        auto &promise=handle.promise();

        promise.rethrowException();

        return {promise.getValue()};
       }
     else
       {
        return {};
       }
    }
 };

template <>
class Task<void>
 {
  public:

   using promise_type = TaskPromise<void> ;

   using HandleType = std::coroutine_handle<promise_type> ;

  private:

   HandleType handle;

  private:

   static void Destroy(HandleType handle) { if( handle ) handle.destroy(); }

  public:

   // constructors

   Task(HandleType handle_={}) noexcept : handle(handle_) {}

   ~Task() { Destroy(handle); }

   // no copy

   Task(const Task &obj) = delete ;

   Task & operator = (Task &obj) = delete ;

   // std move

   Task(Task &&obj) : handle(Replace_null(obj.handle)) {}

   Task & operator = (Task &&obj)
    {
     if( this!=&obj )
       {
        Destroy(Replace(handle,Replace_null(obj.handle)));
       }

     return *this;
    }

   // methods

   bool operator + () const { return bool(handle); }

   bool operator ! () const { return !handle; }

   ResumeTask getResumer() const { return handle; }

   bool done() const { return handle.done(); }

   void resume() { handle.resume(); }

   bool push() const
    {
     handle.resume();

     if( handle.done() )
       {
        auto &promise=handle.promise();

        promise.rethrowException();

        return true;
       }
     else
       {
        return false;
       }
    }
 };

template <class T>
Task<T> TaskPromise<T>::get_return_object() noexcept
 {
  return {std::coroutine_handle<TaskPromise>::from_promise(*this)};
 }

inline
Task<void> TaskPromise<void>::get_return_object() noexcept
 {
  return {std::coroutine_handle<TaskPromise<void> >::from_promise(*this)};
 }

} // namespace App

#endif
