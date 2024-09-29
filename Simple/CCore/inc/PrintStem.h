/* PrintStem.h */
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

#ifndef CCore_inc_PrintStem_h
#define CCore_inc_PrintStem_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* classes */

class PrintFirst;

class PrintPeriod;

/* class PrintFirst */

class PrintFirst
 {
   bool flag = true ;

   StrLen first;
   StrLen other;

  public:

   PrintFirst(StrLen first_,StrLen other_) : first(first_),other(other_) {}

   void reset() { flag=true; }

   StrLen end(StrLen non_empty,StrLen empty) const { return flag? empty : non_empty ; }

   StrLen operator * () const { return flag? first : other ; }

   PrintFirst operator ++ (int)
    {
     PrintFirst ret=*this;

     flag=false;

     return ret;
    }
 };

/* class PrintPeriod */

class PrintPeriod
 {
   unsigned count = 0 ;

   unsigned period; // > 0
   StrLen first;
   StrLen next;
   StrLen line;

  public:

   PrintPeriod(unsigned period_,StrLen first_,StrLen next_,StrLen line_) : period(period_),first(first_),next(next_),line(line_) {}

   void reset() { count=0; }

   StrLen end(StrLen non_empty,StrLen empty) const { return (!count)? empty : non_empty ; }

   StrLen operator * () const
    {
     if( !count ) return first;

     if( count<period ) return next;

     return line;
    }

   PrintPeriod operator ++ (int)
    {
     PrintPeriod ret=*this;

     if( count<period )
       {
        count++;
       }
     else
       {
        count=1;
       }

     return ret;
    }
 };

} // namespace CCore

#endif

