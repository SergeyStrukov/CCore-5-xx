/* PlatformBase.h */
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

#ifndef CCore_inc_base_PlatformBase_h
#define CCore_inc_base_PlatformBase_h

/* target label */

#define CCORE_TARGET WIN64

/* init priority */

#define CCORE_INITPRI_0 __attribute__((init_priority(101)))
#define CCORE_INITPRI_1 __attribute__((init_priority(102)))
#define CCORE_INITPRI_2 __attribute__((init_priority(103)))
#define CCORE_INITPRI_3 __attribute__((init_priority(104)))

/* inlining disabler */

#define CCORE_NOINLINE __attribute__((noinline))

/* restrict */

#define restrict __restrict

/* includes */

#include <cstddef>
#include <type_traits>
#include <initializer_list>
#include <utility>

/* namespace CCore */

namespace CCore {

/* consts */

inline constexpr bool IsLEPlatform  = true ;

inline constexpr bool Is2sComplementArithmetic = true ;

inline constexpr unsigned MaxBitLen = 64   ;

inline constexpr unsigned MaxAlign  = 16   ;

/* types */

using ulen = unsigned long ;
using uptr = unsigned long ;

using uint8  =  unsigned char ;
using uint16 = unsigned short ;
using uint32 =   unsigned int ;
using uint64 =  unsigned long ;

using sint8  =    signed char ;
using sint16 =   signed short ;
using sint32 =     signed int ;
using sint64 =    signed long ;

/* consts */

inline constexpr ulen DefaultPacketCount = 50'000 ;

inline constexpr unsigned DefaultEventElementCount = 10'000 ;

/* namespace Bits */

namespace Bits {

inline constexpr unsigned unsigned_char      =  8 ;
inline constexpr unsigned unsigned_short     = 16 ;
inline constexpr unsigned unsigned_int       = 32 ;
inline constexpr unsigned unsigned_long      = 64 ;
inline constexpr unsigned unsigned_long_long = 64 ;

} // namespace Bits

/* namespace ExtraInt */

namespace ExtraInt {

/* classes */

template <class T> struct Prop;

/* struct Prop<T> */

template <class T>
struct Prop
 {
  static constexpr bool IsSigned   = false ;
  static constexpr bool IsUnsigned = false ;
 };

 //
 // template <>
 // struct Prop<???>
 //  {
 //   static constexpr bool IsSigned   =  true ;
 //   static constexpr bool IsUnsigned = false ;
 //
 //   using SType = ??? ;
 //   using UType = ??? ;
 //
 //   using PromoteSType = ??? ;
 //   using PromoteUType = ??? ;
 //  };
 //
 // template <>
 // struct Prop<???>
 //  {
 //   static constexpr bool IsSigned   = false ;
 //   static constexpr bool IsUnsigned =  true ;
 //
 //   static constexpr unsigned Bits = ??? ;
 //
 //   using PromoteUType = ??? ;
 //  };
 //

} // namespace ExtraInt

/* namespace Meta */

namespace Meta {

/* classes */

template <class UInt> struct DoubleUIntCtor;

/* struct DoubleUIntCtor<UInt> */

template <class UInt>
struct DoubleUIntCtor
 {
  using Ret = void ;
 };

template <>
struct DoubleUIntCtor<uint8>
 {
  using Ret = uint16 ;
 };

template <>
struct DoubleUIntCtor<uint16>
 {
  using Ret = uint32 ;
 };

template <>
struct DoubleUIntCtor<uint32>
 {
  using Ret = uint64 ;
 };

/* type DoubleUInt<UInt> */

template <class UInt>
using DoubleUInt = typename DoubleUIntCtor<UInt>::Ret ;

} // namespace Meta

/* Used() */

template <class T> void Used(T &) {}

/* OptimizeBarrier() */

void OptimizeBarrier(void *ptr,ulen len) noexcept;

} // namespace CCore

#endif

