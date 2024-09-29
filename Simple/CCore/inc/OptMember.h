/* OptMember.h */
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

#ifndef CCore_inc_OptMember_h
#define CCore_inc_OptMember_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* GuardOptMemberNoObject() */

void GuardOptMemberNoObject();

/* classes */

template <class T> class OptMember;

/* class OptMember<T> */

template <class T>
class OptMember : NoCopy
 {
   InitExitObject<T> obj;

  public:

   OptMember() noexcept
    : obj{0}
    {
    }

   ~OptMember()
    {
     obj.exit();
    }

   // object ptr

   T * operator + () const { return +obj; }

   bool operator ! () const { return !obj; }

   T * getPtr_unsafe() const { return obj.getPtr_unsafe(); }

   T * getPtr() const
    {
     if( !obj ) GuardOptMemberNoObject();

     return obj.getPtr_unsafe();
    }

   T & operator * () const { return *getPtr(); }

   T * operator -> () const { return getPtr(); }

   // create/destroy

   template <class ... SS>
   void create(SS && ... ss)
    {
     obj.init( std::forward<SS>(ss)... );
    }

   void destroy()
    {
     obj.exit();
    }
 };

} // namespace CCore

#endif

