/* TimeScope.h */
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

#ifndef CCore_inc_TimeScope_h
#define CCore_inc_TimeScope_h

#include <CCore/inc/Timer.h>

namespace CCore {

/* classes */

class TimeScope;

/* class TimeScope */

class TimeScope
 {
   MSecTimer timer;

   MSec timeout;

  public:

   explicit TimeScope(MSec timeout_=Null) noexcept : timeout(timeout_) {}

   void start(MSec timeout_)
    {
     timer.reset();

     timeout=timeout_;
    }

   bool nextScope()
    {
     auto t=timer.get();
     auto to=+timeout;

     if( t >= to )
       {
        timer.shift(to);

        return true;
       }

     return false;
    }

   bool nextScope_skip()
    {
     auto t=timer.get();
     auto to=+timeout;

     if( t >= to )
       {
        if( t >= 2*to )
          timer.reset();
        else
          timer.shift(to);

        return true;
       }

     return false;
    }

   MSec get() const { return timeout-timer.get(); }
 };

} // namespace CCore

#endif


