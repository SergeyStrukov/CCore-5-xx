/* NullMovePtr.h */
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

#ifndef CCore_inc_gadget_NullMovePtr_h
#define CCore_inc_gadget_NullMovePtr_h

#include <CCore/inc/gadget/NoCopy.h>
#include <CCore/inc/gadget/Replace.h>
#include <CCore/inc/gadget/ObjectPtr.h>

namespace CCore {

/* classes */

template <class T> class NullMovePtr;

/* class NullMovePtr<T> */

template <class T>
class NullMovePtr : public ObjectPtr<T,NoCopy>
 {
   using ObjectPtr<T,NoCopy>::ptr;

  public:

   // constructors

   NullMovePtr() noexcept {}

   explicit NullMovePtr(T *ptr) noexcept : ObjectPtr<T,NoCopy>(ptr) {}

   // std move

   NullMovePtr(NullMovePtr<T> &&obj) noexcept : ObjectPtr<T,NoCopy>(Replace_null(obj.ptr)) {}

   NullMovePtr<T> & operator = (NullMovePtr<T> &&obj) noexcept
    {
     ptr=Replace_null(obj.ptr);

     return *this;
    }
 };

} // namespace CCore

#endif

