/* ObjectPtr.h */
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

#ifndef CCore_inc_gadget_ObjectPtr_h
#define CCore_inc_gadget_ObjectPtr_h

#include <CCore/inc/gadget/Nothing.h>

namespace CCore {

/* classes */

template <class T,class ... BB> class ObjectPtr;

template <class T,class ... BB> class ObjectNonNullPtr;

/* class ObjectPtr<T,BB> */

template <class T,class ... BB>
class ObjectPtr : public BB...
 {
  protected:

   T *ptr;

  public:

   // constructors

   ObjectPtr() noexcept : ptr(0) {}

   ObjectPtr(NothingType) noexcept : ObjectPtr() {}

   explicit ObjectPtr(T *ptr_) noexcept : ptr(ptr_) {}

   // object ptr

   T * operator + () const { return ptr; }

   bool operator ! () const { return !ptr; }

   T * getPtr() const { return ptr; }

   T & operator * () const { return *ptr; }

   T * operator -> () const { return ptr; }
 };

/* class ObjectNonNullPtr<T,BB> */

template <class T,class ... BB>
class ObjectNonNullPtr : public BB...
 {
  protected:

   T *ptr;

  public:

   // constructors

   ObjectNonNullPtr() noexcept : ptr(0) {}

   ObjectNonNullPtr(NothingType) noexcept : ObjectNonNullPtr() {}

   explicit ObjectNonNullPtr(T *ptr_) noexcept : ptr(ptr_) {}

   // object ptr

   T * getPtr() const { return ptr; }

   T & operator * () const { return *ptr; }

   T * operator -> () const { return ptr; }
 };

} // namespace CCore

#endif

