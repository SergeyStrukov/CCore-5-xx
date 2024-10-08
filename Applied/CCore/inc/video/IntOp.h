/* IntOp.h */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_IntOp_h
#define CCore_inc_video_IntOp_h

#include <CCore/inc/Gadget.h>

namespace CCore {
namespace Video {

/* guard functions */

void IntGuardAssert();

inline void IntGuard(bool cond) { if( !cond ) IntGuardAssert(); }

/* functions */

template <class T>
T PlusSub(T a,T b) { return (a>b)? a-b : 0 ; }

template <SIntType SInt>
auto IntDist(SInt a,SInt b) // a <= b
 {
  return SIntFunc<SInt>::Dist(a,b);
 }

template <SIntType SInt,UIntType UInt>
auto IntMovePos(SInt a,UInt delta)
 {
  return SIntFunc<SInt>::MovePos(a,delta);
 }

template <SIntType SInt,UIntType UInt>
auto IntMoveNeg(SInt a,UInt delta)
 {
  return SIntFunc<SInt>::MoveNeg(a,delta);
 }

template <SIntType SInt,UIntType UInt>
auto IntMove(SInt a,SInt e,UInt delta)
 {
  return SIntFunc<SInt>::Move(a,e,delta);
 }

template <SIntType SInt>
auto IntAbs(SInt a,SInt b)
 {
  return (a<b)? IntDist(a,b) : IntDist(b,a) ;
 }

template <SIntType SInt>
auto IntAbs(SInt a)
 {
  return IntAbs<SInt>(a,0);
 }

/* functions */

 //
 // Can be substituted to provide checked operations.
 //

template <SIntType SInt>
SInt IntAdd(SInt a,SInt b) { return a+b; } // may overflow

template <SIntType SInt>
SInt IntSub(SInt a,SInt b) { return a-b; } // may overflow

template <SIntType SInt>
SInt IntMul(SInt a,SInt b) { return a*b; } // may overflow

template <SIntType SInt>
SInt IntDiv(SInt a,SInt b) { return a/b; } // may overflow OR crash

 //
 // Can be substituted to provide the proper semantic.
 //

template <SIntType SInt>
SInt IntLShift(SInt a,unsigned s) { return SIntFunc<SInt>::LShift(a,s); }

template <SIntType SInt>
SInt IntRShift(SInt a,unsigned s) { return SIntFunc<SInt>::RShift(a,s); }

template <SIntType SInt,UIntType UInt>
SInt IntMask(SInt a,UInt mask) { return SIntFunc<SInt>::Mask(a,mask); }

/* classes */

template <UIntType UInt> class DownBits;

/* class DownBits<UInt> */

template <UIntType UInt>
class DownBits
 {
   static constexpr unsigned MaxBits = Meta::UIntBits<UInt> ;

   UInt value;
   unsigned shift;

  public:

   DownBits()
    {
     value=0;
     shift=0;
    }

   explicit DownBits(UIntType auto b) { init(b); }

   operator UInt() const { return value; }

   void init(UIntType auto b)
    {
     unsigned bits=UIntBitsOf(b);

     if( bits<=MaxBits )
       {
        shift=0;
        value=UInt(b);
       }
     else
       {
        shift=bits-MaxBits;
        value=UInt(b>>shift);
       }
    }

   UInt operator () (UIntType auto a) const
    {
     return UInt(a>>shift);
    }
 };

} // namespace Video
} // namespace CCore

#endif


