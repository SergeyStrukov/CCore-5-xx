/* SymCount.h */
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

#ifndef CCore_inc_SymCount_h
#define CCore_inc_SymCount_h

#include <CCore/inc/Utf8.h>

namespace CCore {

/* SymLen() */

inline ulen SymLen(StrLen text) { return Utf8Len(text); }

/* class SymCounter */

class SymDetector;

class SymCounter;

/* class SymDetector */

class SymDetector : NoCopy
 {
   unsigned len;

  public:

   SymDetector() { reset(); }

   void reset() { len=0; }

   bool put(char ch)
    {
     if( len )
       {
        len--;

        return !len;
       }
     else
       {
        if( unsigned l=Utf8Len(ch) )
          {
           if( l==1 )
             {
              return true;
             }
           else
             {
              len=l-1;

              return false;
             }
          }
        else
          {
           return true;
          }
       }
    }
 };

/* class SymCounter */

class SymCounter : NoCopy
 {
   ulen count;
   SymDetector det;

  public:

   SymCounter() : count(0) {}

   operator ulen() const { return count; }

   void put(char ch)
    {
     if( det.put(ch) ) count++;
    }

   void put(char ch,ulen len)
    {
     for(; len ;len--) put(ch);
    }

   void put(const char *str,ulen len)
    {
     for(; len ;len--,str++) put(*str);
    }
 };

} // namespace CCore

#endif

