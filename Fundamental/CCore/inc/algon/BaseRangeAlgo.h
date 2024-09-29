/* BaseRangeAlgo.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_algon_BaseRangeAlgo_h
#define CCore_inc_algon_BaseRangeAlgo_h

#include <CCore/inc/Swap.h>

namespace CCore {
namespace Algon {

/* concept RangeAlgo<Algo,R> */

template <class Algo,class R,class LenType>
concept RangeAlgo2 = UIntType<LenType> && requires(R &r,LenType len)
 {
  r[len];

  Algo::GetPtr(r);

  Ground<LenType>( Algo::GetLen(r) );

  Ground<R>( Algo::GetPrefix(r,len) );

  Ground<R>( Algo::GetSuffix(r,len) );

  Ground<R>( Algo::Split(r,len) );

  Ground<R>( Algo::GetPrefix(r,r) );

  Algo::RangeSwap(r,r);

  Algo::Reverse(r);
 } ;

template <class Algo,class R>
concept RangeAlgo = requires(R r)
 {
  typename Algo::LenType;

  requires RangeAlgo2<Algo,R,typename Algo::LenType> ;
 } ;

/* concept Has_RangeAlgo<R> */

template <class R>
concept Has_RangeAlgo = requires()
 {
  typename R::RangeAlgo;

  requires RangeAlgo<typename R::RangeAlgo,R> ;
 } ;

/* concept RangeType<R> */

template <class R>
concept RangeType = CursorType<R> && ( IsPtrLen<R> || IsPtrLenReverse<R> || Has_RangeAlgo<R> ) ;

/* classes */

template <class R> struct BaseRangeAlgo_gen;

template <class R> struct BaseRangeAlgo;

/* struct BaseRangeAlgo_gen<R> */

template <class R>
struct BaseRangeAlgo_gen
 {
  using LenType = ulen ;

  static R GetPtr(R r) { return r; }

  static LenType GetLen(R r) { return r.len; }

  static R GetPrefix(R r,LenType len) { return r.prefix(len); }

  static R GetSuffix(R r,LenType len) { return r.suffix(len); }

  static R Split(R &r,LenType prefix) { return r+=prefix; }

  static R GetPrefix(R r,R suffix) { return r.prefix(suffix); }

  static void RangeSwap(R a,R b)
   {
    for(; +a ;++a,++b) Swap(*a,*b);
   }

  static auto Reverse(R r) { return r.reverse(); }
 };

/* struct BaseRangeAlgo<R> */

template <Has_RangeAlgo R>
struct BaseRangeAlgo<R> : R::RangeAlgo {};

template <class T>
struct BaseRangeAlgo<PtrLen<T> > : BaseRangeAlgo_gen<PtrLen<T> >
 {
  static T * GetPtr(PtrLen<T> r) { return r.ptr; }

  static void RangeSwap(PtrLen<T> a,PtrLen<T> b)
   {
    for(T *p=b.ptr; +a ;++a,++p) Swap(*a,*p);
   }

  static PtrLenReverse<T> Reverse(PtrLen<T> r) { return RangeReverse(r); }
 };

template <class T>
struct BaseRangeAlgo<PtrLenReverse<T> > : BaseRangeAlgo_gen<PtrLenReverse<T> >
 {
  static T * GetPtr(PtrLenReverse<T> r) { return r.ptr-1; }

  static void RangeSwap(PtrLenReverse<T> a,PtrLenReverse<T> b)
   {
    for(T *p=b.ptr; +a ;++a,--p) Swap(*a,p[-1]);
   }
 };

} // namespace Algon
} // namespace CCore

#endif


