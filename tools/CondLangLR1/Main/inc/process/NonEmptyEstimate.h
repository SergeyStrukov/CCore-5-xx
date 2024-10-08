/* NonEmptyEstimate.h */
//----------------------------------------------------------------------------------------
//
//  Project: CondLangLR1 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CondLangLR1_process_NonEmptyEstimate_h
#define CondLangLR1_process_NonEmptyEstimate_h

#include "inc/lang/Lang.h"

namespace App {

/* classes */

class NonEmptyEstimate;

/* class NonEmptyEstimate */

class NonEmptyEstimate
 {
   bool value;

  private:

   NonEmptyEstimate(bool value_) : value(value_) {}

  public:

   // constructors

   NonEmptyEstimate() noexcept : value(false) {}

   NonEmptyEstimate(ElementNullType) : value(false) {}

   NonEmptyEstimate(ElementOneType) : value(true) {}

   NonEmptyEstimate(Atom,EmptyContext) : value(true) {}

   // methods

   bool operator + () const { return value; }

   bool operator ! () const { return !value; }

   bool setCmp(NonEmptyEstimate obj) { return SetCmp(value,obj.value); }

   // cmp objects

   bool operator == (NonEmptyEstimate obj) const noexcept { return value==obj.value; }

   CmpResult operator <=> (NonEmptyEstimate obj) const noexcept { return Cmp(value,obj.value); }

   // operations

   friend NonEmptyEstimate operator + (NonEmptyEstimate a,NonEmptyEstimate b) { return a.value|b.value; }

   friend NonEmptyEstimate operator * (NonEmptyEstimate a,NonEmptyEstimate b) { return a.value&b.value; }

   using Accumulator = NonEmptyEstimate ;

   void operator += (NonEmptyEstimate obj) { (*this)=(*this)+obj; }

   // print object

   using PrintOptType = BlockPrintOpt ;

   void print(PrinterType auto &out,PrintOptType opt) const
    {
     if( value )
       Putobj(out,"non-empty");
     else
       Putobj(out,"empty");

     if( opt.flag ) Putch(out,'\n');
    }
 };

} // namespace App

#endif
