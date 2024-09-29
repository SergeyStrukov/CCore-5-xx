/* OwnPtr.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_OwnPtr_h
#define CCore_inc_OwnPtr_h

#include <CCore/inc/Move.h>

namespace CCore {

/* concept OwnPtrAlgo<Algo,T> */

template <class Algo,class T>
concept OwnPtrAlgo = requires(T *obj)
 {
  { Algo::Destroy(obj) } noexcept;
 } ;

/* classes */

template <class T> struct OwnAlgo;

template <class T,class Algo=OwnAlgo<T> > class OwnPtr;

/* struct OwnAlgo<T> */

template <class T>
struct OwnAlgo
 {
  static void Destroy(T *ptr) noexcept requires( NothrowDtorType<T> ) { delete ptr; }
 };

/* class OwnPtr<T,Algo> */

template <class T,class Algo>
class OwnPtr : public ObjectPtr<T,NoCopy>
 {
   using ObjectPtr<T,NoCopy>::ptr;

  private:

   static void SoftDestroy(OwnPtr<T,Algo> *obj) noexcept
    {
     obj->ptr=0;

     obj->~OwnPtr();
    }

   static void Destroy(T *ptr) noexcept requires ( OwnPtrAlgo<Algo,T> )
    {
     if( ptr ) Algo::Destroy(ptr);
    }

  public:

   // constructors

   OwnPtr() noexcept {}

   explicit OwnPtr(T *ptr) noexcept : ObjectPtr<T,NoCopy>(ptr) {}

   ~OwnPtr() { this->Destroy(ptr); }

   // std move

   OwnPtr(OwnPtr<T,Algo> &&obj) noexcept : ObjectPtr<T,NoCopy>(Replace_null(obj.ptr)) {}

   OwnPtr<T,Algo> & operator = (OwnPtr<T,Algo> &&obj) noexcept
    {
     if( this!=&obj )
       {
        this->Destroy(Replace(ptr,Replace_null(obj.ptr)));
       }

     return *this;
    }

   // reset

   void set(T *new_ptr) noexcept { this->Destroy(Replace(ptr,new_ptr)); }

   T * detach(T *new_ptr=0) noexcept { return Replace(ptr,new_ptr); }

   // swap/move objects

   void objSwap(OwnPtr<T,Algo> &obj) noexcept { Swap(ptr,obj.ptr); }

   explicit OwnPtr(ToMoveCtor< OwnPtr<T,Algo> > obj) noexcept : ObjectPtr<T,NoCopy>(Replace_null(obj->ptr)) {}

   OwnPtr<T,Algo> * objMove(Place<void> place) noexcept
    {
     OwnPtr<T,Algo> *ret=new(place) OwnPtr<T,Algo>(ptr);

     SoftDestroy(this);

     return ret;
    }
 };

} // namespace CCore

#endif


