/* ScanRange.h */
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

#ifndef CCore_inc_ScanRange_h
#define CCore_inc_ScanRange_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* classes */

template <class R> struct ScanRange;

struct ScanStr;

/* struct ScanRange<R> */

template <class R>
struct ScanRange
 {
  R before;
  R next; // starts at element satisfies pred

  template <class Pred>
  static auto Test(Pred pred,R t) -> decltype( pred(t) ) { return pred(t); }

  template <class Pred>
  static auto Test(Pred pred,R t) -> decltype( pred(*t) ) { return pred(*t); }

  template <class Pred>
  ScanRange(R r,Pred pred)
   {
    for(R t=r; +t ;++t)
      if( Test(pred,t) )
        {
         before=r.prefix(t);
         next=t;

         return;
        }

    before=r;
    next={};
   }
 };

/* struct ScanStr */

struct ScanStr : ScanRange<StrLen>
 {
  using ScanRange<StrLen>::ScanRange;

  ScanStr(StrLen str,char ch) : ScanStr(str, [=] (char elem) { return elem==ch; } ) {}
 };

} // namespace CCore

#endif

