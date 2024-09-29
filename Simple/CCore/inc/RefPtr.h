/* RefPtr.h */
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

#ifndef CCore_inc_RefPtr_h
#define CCore_inc_RefPtr_h

#include <CCore/inc/Move.h>

namespace CCore {

/* concept RefPtrAlgo<Algo,T> */

template <class Algo,class T>
concept RefPtrAlgo = requires(T *obj)
 {
  { Algo::IncRef(obj) } noexcept;

  { Algo::DecRef(obj) } noexcept;

  Ground<bool>( Algo::DecRef(obj) );

  { Algo::Destroy(obj) } noexcept;
 } ;

/* concept RefCountType<T> */

template <class T>
concept RefCountType = requires(T &obj)
 {
  { obj.incRef() } noexcept;

  { obj.decRef() } noexcept;

  Ground<bool>( obj.decRef() );

  { obj.destroy() } noexcept ;
 } ;

/* classes */

template <class T> struct RefAlgo;

template <class T,class Algo=RefAlgo<T> > class RefPtr;

/* struct RefAlgo<T> */

template <class T>
struct RefAlgo
 {
  static void IncRef(T *ptr) noexcept requires ( RefCountType<T> ) { ptr->incRef(); }

  static bool DecRef(T *ptr) noexcept requires ( RefCountType<T> ) { return ptr->decRef(); }

  static void Destroy(T *ptr) noexcept requires ( RefCountType<T> ) { ptr->destroy(); }
 };

/* class RefPtr<T,Algo> */

template <class T,class Algo>
class RefPtr : public ObjectNonNullPtr<T>
 {
   using ObjectNonNullPtr<T>::ptr;

  private:

   static void SoftDestroy(RefPtr<T,Algo> *obj) noexcept
    {
     obj->ptr=0;

     obj->~RefPtr();
    }

   static void Hold(T *ptr) noexcept requires( RefPtrAlgo<Algo,T> )
    {
     Algo::IncRef(ptr);
    }

   static void Release(T *ptr) noexcept requires( RefPtrAlgo<Algo,T> )
    {
     if( Algo::DecRef(ptr) ) Algo::Destroy(ptr);
    }

  public:

   // constructors

   explicit RefPtr(T *ptr) noexcept : ObjectNonNullPtr<T>(ptr) {} // ptr!=0

   ~RefPtr() { if( ptr ) this->Release(ptr); }

   // copying

   RefPtr(const RefPtr<T,Algo> &obj) noexcept : ObjectNonNullPtr<T>(obj.ptr) { this->Hold(ptr); }

   RefPtr<T,Algo> & operator = (const RefPtr<T,Algo> &obj) noexcept
    {
     T *new_ptr=obj.ptr;
     T *old_ptr=Replace(ptr,new_ptr);

     this->Hold(new_ptr);
     this->Release(old_ptr);

     return *this;
    }

   // reset

   void set(T *new_ptr) noexcept // new_ptr!=0
    {
     this->Release(Replace(ptr,new_ptr));
    }

   // update

   void update(FuncType<T *,T *> auto func) { ptr=func(ptr); }

   // swap/move objects

   void objSwap(RefPtr<T,Algo> &obj) noexcept { Swap(ptr,obj.ptr); }

   explicit RefPtr(ToMoveCtor< RefPtr<T,Algo> > obj) noexcept : ObjectNonNullPtr<T>(Replace_null(obj->ptr)) {}

   RefPtr<T,Algo> * objMove(Place<void> place) noexcept
    {
     RefPtr<T,Algo> *ret=new(place) RefPtr<T,Algo>(ptr);

     SoftDestroy(this);

     return ret;
    }
 };

} // namespace CCore

#endif


