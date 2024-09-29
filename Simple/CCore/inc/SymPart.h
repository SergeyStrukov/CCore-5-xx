/* SymPart.h */
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

#ifndef CCore_inc_SymPart_h
#define CCore_inc_SymPart_h

#include <CCore/inc/Utf8.h>

namespace CCore {

/* functions */

inline StrLen SymPrefix(StrLen str,ulen len)
 {
  return str.prefix(Utf8Move(str,len));
 }

inline StrLen SymPart(StrLen str,ulen off)
 {
  return Utf8Move(str,off);
 }

inline StrLen SymPart(StrLen str,ulen off,ulen len)
 {
  return SymPrefix(SymPart(str,off),len);
 }

} // namespace CCore

#endif

