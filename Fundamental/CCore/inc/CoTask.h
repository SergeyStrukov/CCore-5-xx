/* CoTask.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_CoTask_h
#define CCore_inc_CoTask_h

#include <CCore/inc/MemBase.h>

#include <coroutine>
#include <exception>

namespace CCore {

/* type cotask */

using cotask = std::coroutine_handle<> ;

/* guard functions */

void GuardCoTaskNone();

inline void GuardCoTaskExist(cotask handle)
 {
  if( !handle ) GuardCoTaskNone();
 }

void GuardCoTaskFinished();

inline void GuardCoTaskActive(cotask handle)
 {
  if( !handle ) GuardCoTaskNone();

  if( handle.done() ) GuardCoTaskFinished();
 }

/* classes */

struct CoTaskResume;

template <class T> struct CoTaskResult;

class CoTaskPromiseBase;

template <class T> class CoTaskPromise;

template <class T,template <class> class Promise=CoTaskPromise> class CoTask;

template <ulen Len> class CoTaskStack;

/* struct CoTaskResume */

struct CoTaskResume
 {
  cotask handle;

  CoTaskResume(cotask handle_) : handle(handle_) { GuardCoTaskActive(handle_); }

  template <class T>
  CoTaskResume(std::coroutine_handle<T> handle_) : handle(handle_) { GuardCoTaskActive(handle); }

  bool await_ready() { return false; }

  auto await_suspend(cotask) { return handle; }

  void await_resume() {}
 };

/* struct CoTaskResult<T> */

template <>
struct CoTaskResult<void>
 {
  bool ok;

  CoTaskResult(bool ok_=false) : ok(ok_) {}

  bool operator + () const { return ok; }

  bool operator ! () const { return !ok; }
 };

template <class T>
struct CoTaskResult
 {
  T obj;
  bool ok;

  CoTaskResult() : obj{},ok(false) {}

  bool operator + () const { return ok; }

  bool operator ! () const { return !ok; }
 };

/* class CoTaskPromiseBase */

class CoTaskPromiseBase : public MemBase_nocopy
 {
   std::exception_ptr exception;

  public:

   CoTaskPromiseBase() : exception{} {}

   std::suspend_always initial_suspend() const noexcept { return {}; }

   std::suspend_always final_suspend() const noexcept { return {}; }

   void unhandled_exception() { exception=std::current_exception(); }

   // methods

   void rethrowException() { if( exception ) std::rethrow_exception(exception); }
 };

/* class CoTaskPromise<T> */

template <>
class CoTaskPromise<void> : public CoTaskPromiseBase
 {
  public:

   CoTaskPromise() {}

   CoTask<void,CoTaskPromise> get_return_object() noexcept;

   void return_void() {}

   // methods

   CoTaskResult<void> getResult() const { return true; }
 };

template <class T>
class CoTaskPromise : public CoTaskPromiseBase
 {
   CoTaskResult<T> result;

  public:

   CoTaskPromise() {}

   CoTask<T,CoTaskPromise> get_return_object() noexcept;

   void return_value(T &&obj)
    {
     result.obj=std::move(obj);
     result.ok=true;
    }

   void return_value(const T &obj)
    {
     result.obj=obj;
     result.ok=true;
    }

   // methods

   CoTaskResult<T> getResult() { return std::move(result); }
 };

/* class CoTask<T,Promise> */

template <class T,template <class> class Promise>
class CoTask
 {
  public:

   using promise_type = Promise<T> ;

   using HandleType = std::coroutine_handle<promise_type> ;

  private:

   HandleType handle;

  private:

   static void Destroy(HandleType handle) { if( handle ) handle.destroy(); }

  public:

   // constructors

   CoTask(HandleType handle_={}) noexcept : handle(handle_) {}

   ~CoTask() { Destroy(handle); }

   // no copy

   CoTask(const CoTask &obj) = delete ;

   CoTask & operator = (CoTask &obj) = delete ;

   // std move

   CoTask(CoTask &&obj) : handle(Replace_null(obj.handle)) {}

   CoTask & operator = (CoTask &&obj)
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

   cotask getHandle() const { return handle; }

   CoTaskResume getResumer() const { return handle; }

   bool done() const { return handle.done(); }

   // push

   CoTaskResult<T> push() const
    {
     GuardCoTaskExist(handle);

     if( !handle.done() ) handle.resume();

     if( handle.done() )
       {
        auto &promise=handle.promise();

        promise.rethrowException();

        return promise.getResult();
       }
     else
       {
        return {};
       }
    }
 };

template <class T>
CoTask<T,CoTaskPromise> CoTaskPromise<T>::get_return_object() noexcept
 {
  return {std::coroutine_handle< CoTaskPromise<T> >::from_promise(*this)};
 }

inline
CoTask<void,CoTaskPromise> CoTaskPromise<void>::get_return_object() noexcept
 {
  return {std::coroutine_handle< CoTaskPromise<void> >::from_promise(*this)};
 }

/* class CoTaskStack<ulen Len> */

template <ulen Len>
class CoTaskStack : NoCopy
 {
   CoTask<void> * stack[Len];
   ulen len = 0 ;

  private:

   CoTask<void> & top()
    {
     GuardNotEmpty(len);

     return *(stack[len-1]);
    }

   void push(CoTask<void> &task)
    {
     GuardIndex(len,Len);

     stack[len++]=&task;
    }

   void pop()
    {
     GuardNotEmpty(len);

     --len;
    }

  public:

   CoTaskStack() {}

   void prepare(CoTask<void> &task)
    {
     push(task);
    }

   CoTaskResume cocall(CoTask<void> &task)
    {
     push(task);

     return task.getResumer();
    }

   CoTaskResume coret()
    {
     pop();

     return top().getResumer();
    }

   bool push()
    {
     return top().push().ok;
    }
 };

} // namespace CCore

#endif

