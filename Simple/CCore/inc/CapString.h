/* CapString.h */
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

#ifndef CCore_inc_CapString_h
#define CCore_inc_CapString_h

#include <CCore/inc/Symbol.h>

namespace CCore {

/* classes */

template <ulen MaxLen=TextBufLen> requires ( MaxLen>0 ) class CapString;

/* class CapString<ulen MaxLen> */

template <ulen MaxLen> requires ( MaxLen>0 )
class CapString : NoCopy
 {
   char buf[MaxLen+1];

  private:

   void init(StrLen str)
    {
     str.copyTo(buf);

     buf[str.len]=0;
    }

  public:

   explicit CapString(StrLen str)
    {
     Replace_min(str.len,MaxLen); // cap length

     init(TrimText(str));
    }

   operator const char * () const { return buf; }
 };

} // namespace CCore

#endif


