/* PosSub.h */
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

#ifndef CCore_inc_gadget_PosSub_h
#define CCore_inc_gadget_PosSub_h

#include <CCore/inc/gadget/Classification.h>

namespace CCore {

/* classes */

template <UIntType UInt> struct PosSubDelta;

template <UIntType UInt> struct PosSubResult;

/* struct PosSubDelta<UInt> */

template <UIntType UInt>
struct PosSubDelta
 {
  UInt delta;

  constexpr explicit PosSubDelta(UInt delta_) : delta(delta_) {}

  constexpr operator UInt() const { return delta; }
 };

/* struct PosSubResult<UInt> */

template <UIntType UInt>
struct PosSubResult
 {
  // result

  UInt value;
  bool ok;

  // constructors

  constexpr explicit PosSubResult(UInt value_,bool ok_=true) : value(value_),ok(ok_) {}

  // methods

  constexpr operator UInt() const { return value; }

  constexpr PosSubResult<UInt> operator - (PosSubDelta<UIntType auto> obj) const
   {
    return (obj<=value)? PosSubResult(UInt(value-obj),ok) : PosSubResult(0,false) ;
   }
 };

/* PosSub() */

template <UIntType UInt,UIntType ... Args>
constexpr PosSubResult<UInt> PosSub(UInt value,Args ... args)
 {
  return ( PosSubResult(value) - ... - PosSubDelta(args) );
 }

/* Enough() */

template <UIntType UInt,UIntType ... Args>
constexpr bool Enough(UInt value,Args ... args)
 {
  return PosSub(value,args...).ok;
 }

} // namespace CCore

#endif


