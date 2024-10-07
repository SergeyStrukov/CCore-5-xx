/* Generator.h */
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

#ifndef App_Generator_h
#define App_Generator_h

#include <CCore/inc/Gadget.h>

#include <coroutine>
#include <exception>

namespace App {

/* using */

using namespace CCore;

/* classes */

template <class T> class GeneratorPromise;

template <class T> class Generator;

/* class GeneratorPromise<T> */

template <class T>
class GeneratorPromise
 {
   T value;
   std::exception_ptr exception;

  public:

   GeneratorPromise() noexcept : value{},exception{} {}

   static Generator<T> get_return_object_on_allocation_failure();

   Generator<T> get_return_object() noexcept;

   std::suspend_always initial_suspend() const noexcept { return {}; }

   std::suspend_always final_suspend() const noexcept { return {}; }

   std::suspend_always yield_value(const T &value_) noexcept
    {
     value=value_;

     return {};
    }

   void unhandled_exception() { exception=std::current_exception(); }

   void return_void() {}

   // methods

   T getValue() const { return value; }

   void rethrowException() { if( exception ) std::rethrow_exception(exception); }
 };

/* class Generator<T> */

template <class T>
class Generator
 {
  public:

   using promise_type = GeneratorPromise<T> ;

   using HandleType = std::coroutine_handle<promise_type> ;

  private:

   HandleType handle;

  private:

   static void Destroy(HandleType handle) { if( handle ) handle.destroy(); }

  public:

   // constructors

   Generator(HandleType handle_={}) noexcept : handle(handle_) {}

   ~Generator() { Destroy(handle); }

   // no copy

   Generator(const Generator &obj) = delete ;

   Generator & operator = (Generator &obj) = delete ;

   // std move

   Generator(Generator &&obj) : handle(Replace_null(obj.handle)) {}

   Generator & operator = (Generator &&obj)
    {
     if( this!=&obj )
       {
        Destroy(Replace(handle,Replace_null(obj.handle)));
       }

     return *this;
    }

   // methods

   bool next() { return handle? (handle.resume(), !handle.done()) : false ; }

   T getValue() const { return handle.promise().getValue(); }

   void rethrowException() { if( handle ) handle.promise().rethrowException(); }
 };

template <class T>
Generator<T> GeneratorPromise<T>::get_return_object_on_allocation_failure()
 {
  return {};
 }

template <class T>
Generator<T> GeneratorPromise<T>::get_return_object() noexcept
 {
  return {std::coroutine_handle<GeneratorPromise>::from_promise(*this)};
 }

} // namespace App

#endif


