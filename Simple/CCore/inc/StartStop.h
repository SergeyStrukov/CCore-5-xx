/* StartStop.h */
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

#ifndef CCore_inc_StartStop_h
#define CCore_inc_StartStop_h

#include <CCore/inc/Task.h>

namespace CCore {

/* concept RunableType<T> */

template <class T>
concept RunableType = requires(T &obj)
 {
  obj.prepareRun();

  obj.objRun();

  { obj.completeRun() } noexcept;

  { obj.signalStop() } noexcept;

  { obj.completeStop() } noexcept;
 } ;

/* classes */

template <class T> class StartStopObject;

/* class StartStopObject<T> */

template <class T>
class StartStopObject : NoCopy
 {
   T &obj;
   Sem stop_sem;

  public:

   template <class ... TT>
   StartStopObject(T &obj_,TT ... tt) requires RunableType<T>
    : obj(obj_),
      stop_sem("StartStopObject")
    {
     obj_.prepareRun();

     RunFuncTask( [&] () { obj_.objRun(); } ,stop_sem.function_give(),tt...);

     obj_.completeRun(); // assume no throw
    }

   ~StartStopObject()
    {
     obj.signalStop();

     stop_sem.take();

     obj.completeStop();
    }
 };

} // namespace CCore

#endif


