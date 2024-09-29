/* DoubleFormat.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN64
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_base_DoubleFormat_h
#define CCore_inc_base_DoubleFormat_h

#include <string.h>

namespace CCore {

/* classes */

struct DoubleFormat;

/* struct DoubleFormat */

struct DoubleFormat
 {
  using BodyType = unsigned long;

  static constexpr unsigned FractBits = 52 ;
  static constexpr unsigned ExpBits   = 11 ;
  static constexpr unsigned ExpBias   = 1023 ;

  static constexpr unsigned MaxExp    = 0x7FFu ;
  static constexpr BodyType MaxFract  = 0xF'FFFF'FFFF'FFFFu ;

  BodyType body;
           // sign  : 1
           // exp   : 11
           // fract : 52

  // constructors

  DoubleFormat() : body(0) {}

  DoubleFormat(double value) { memcpy(&body,&value,sizeof (double)); }

  auto operator + () const { return body; }

  bool operator ! () const { return body==0; }

  double get() const
   {
    double ret;

    memcpy(&ret,&body,sizeof (double));

    return ret;
   }

  // fields

  unsigned getSign() const { return unsigned(body>>63); }

  unsigned getExp() const { return unsigned((body>>52)&0x7FFu); }

  BodyType getFract() const { return body&0xF'FFFF'FFFF'FFFFu; }

  void setSign(unsigned val) { body=(body&0x7FFF'FFFF'FFFF'FFFFu)|(BodyType(val)<<63) ; }

  void setExp(unsigned val) { body=(body&0x800F'FFFF'FFFF'FFFFu)|(BodyType(val&0x7FFu)<<52) ; }

  void setFract(BodyType val) { body=(body&0xFFF0'0000'0000'0000u)|(val&0xF'FFFF'FFFF'FFFFu); }
 };

} // namespace CCore

#endif



