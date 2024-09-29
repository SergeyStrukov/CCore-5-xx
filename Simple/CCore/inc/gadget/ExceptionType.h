/* ExceptionType.h */
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

#ifndef CCore_inc_gadget_ExceptionType_h
#define CCore_inc_gadget_ExceptionType_h

namespace CCore {

/* classes */

//enum ExceptionType;

struct CatchType;

/* enum ExceptionType */

enum ExceptionType
 {
  NoException,
  Exception
 };

const char * GetTextDesc(ExceptionType ex);

/* struct CatchType */

struct CatchType
 {
  CatchType() noexcept {}
 };

} // namespace CCore

#endif


