/* StrToChar.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_StrToChar_h
#define CCore_inc_StrToChar_h

#include <CCore/inc/Symbol.h>
#include <CCore/inc/Array.h>

namespace CCore {

/* classes */

class StrToChar;

class CharToStr;

/* class StrToChar */

class StrToChar : NoCopy
 {
   DynArray<Char> buf;

  public:

   explicit StrToChar(StrLen text);

   ~StrToChar() {}

   PtrLen<const Char> operator + () const { return Range(buf); }

   operator PtrLen<const Char>() const { return Range(buf); }
 };

/* class CharToStr */

class CharToStr : NoCopy
 {
   DynArray<char> buf;

  public:

   explicit CharToStr(PtrLen<const Char> text);

   ~CharToStr() {}

   StrLen operator + () const { return Range(buf); }

   operator StrLen() const { return Range(buf); }
 };

} // namespace CCore

#endif

