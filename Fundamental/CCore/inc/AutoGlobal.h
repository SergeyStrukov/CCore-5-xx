/* AutoGlobal.h */
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

#ifndef CCore_inc_AutoGlobal_h
#define CCore_inc_AutoGlobal_h

#include <CCore/inc/Task.h>

namespace CCore {

/* classes */

template <NothrowDtorType T> class AutoGlobal;

/* class AutoGlobal<T> */

template <NothrowDtorType T>
class AutoGlobal : NoCopy
 {
   Mutex mutex;

   ulen refs = 0 ;
   InitExitObject<T> obj;

  private:

   void lock()
    {
     Mutex::Lock lock(mutex);

     if( !refs )
       {
        obj.init();
       }

     refs++;
    }

   void unlock()
    {
     Mutex::Lock lock(mutex);

     if( refs==1 )
       {
        obj.exit();
       }

     refs--;
    }

  public:

   AutoGlobal() : AutoGlobal("AutoGlobal") {}

   explicit AutoGlobal(TextLabel name) : mutex(name),obj{0} {}

   ~AutoGlobal() {}

   using Lock = LockObject< AutoGlobal<T> > ;

   friend Lock;

   // object ptr : valid inside Lock scope

   T * getPtr() const { return obj.getPtr(); }

   T & operator * () const { return *getPtr(); }

   T * operator -> () const { return getPtr(); }
 };

} // namespace CCore

#endif

