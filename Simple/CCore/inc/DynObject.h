/* DynObject.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_DynObject_h
#define CCore_inc_DynObject_h

#include <CCore/inc/NewDelete.h>
#include <CCore/inc/Move.h>

namespace CCore {

/* classes */

template <NothrowDtorType T> class DynObject;

/* class DynObject<T> */

template <NothrowDtorType T>
class DynObject
 {
   T *ptr;

  private:

   static void SoftDestroy(DynObject<T> *obj) noexcept
    {
     obj->ptr=0;
     obj->~DynObject();
    }

   static void Destroy(T *ptr) noexcept
    {
     if( ptr ) Delete(DefaultHeapAlloc(),ptr);
    }

  public:

   // constructors

   DynObject() noexcept : ptr(0) {}

   explicit DynObject(NothingType) requires ( ConstructibleType<T> ) : ptr(New<T>(DefaultHeapAlloc())) {}

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit DynObject(SS && ... ss)  : ptr(New<T>(DefaultHeapAlloc(), std::forward<SS>(ss)... )) {}

   ~DynObject() { Destroy(ptr); }

   // no copy

   DynObject(const DynObject<T> &obj) = delete ;

   DynObject<T> & operator = (const DynObject<T> &obj) = delete ;

   // std move

   DynObject(DynObject<T> &&obj) noexcept : ptr(Replace_null(obj.ptr)) {}

   DynObject<T> & operator = (DynObject<T> &&obj) noexcept
    {
     if( this!=&obj )
       {
        Destroy(Replace(ptr,Replace_null(obj.ptr)));
       }

     return *this;
    }

   // methods

   void create(NothingType) requires ( ConstructibleType<T> )
    {
     Destroy(Replace(ptr, New<T>(DefaultHeapAlloc()) ));
    }

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   void create(SS && ... ss)
    {
     Destroy(Replace(ptr, New<T>(DefaultHeapAlloc(), std::forward<SS>(ss)... ) ));
    }

   void destroy() noexcept
    {
     Destroy(Replace_null(ptr));
    }

   // object pointer, const enforcement

   const T * operator + () const { return ptr; }

   bool operator ! () const { return !ptr; }

   T * getPtr() { return ptr; }

   const T * getPtr() const { return ptr; }

   const T * getPtr_const() const { return ptr; }

   T & operator * () { return *ptr; }

   const T & operator * () const { return *ptr; }

   T * operator -> () { return ptr; }

   const T * operator -> () const { return ptr; }

   // object swap/move

   void objSwap(DynObject<T> &obj) noexcept
    {
     Swap(ptr,obj.ptr);
    }

   explicit DynObject(ToMoveCtor<DynObject<T> > obj) noexcept : ptr(Replace_null(obj->ptr)) {}

   DynObject<T> * objMove(Place<void> place) noexcept
    {
     DynObject<T> *ret=new(place) DynObject<T>();

     ret->ptr=ptr;

     SoftDestroy(this);

     return ret;
    }
 };

} // namespace CCore

#endif


