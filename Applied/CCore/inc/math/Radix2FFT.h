/* Radix2FFT.h */
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

#ifndef CCore_inc_math_Radix2FFT_h
#define CCore_inc_math_Radix2FFT_h

#include <CCore/inc/Array.h>

namespace CCore {
namespace Math {

/* guard functions */

void GuardOrder(unsigned order,unsigned max_order);

/* functions */

ulen BitReverse(unsigned order,ulen ind);

template <class T>
void FFT2(T &a,T &b)
 {
  T c=a-b;

  a=a+b;
  b=c;
 }

template <class T>
void FFT2(T &a,T &b,T root)
 {
  T c=a-b;

  a=a+b;
  b=c*root;
 }

template <class T>
void Radix2FFT(T *buf,ulen buflen,FuncType<T,ulen> auto root,FuncType<ulen,ulen> auto bitrev)
 {
  ulen count=1;
  ulen len=buflen;

  for(; len>=2 ;len>>=1,count<<=1)
    {
     ulen len2=len/2;
     ulen cnt=count;

     for(T *ptr=buf; cnt ;ptr+=len,cnt--)
       {
        // step

        FFT2(ptr[0],ptr[len2]);

        for(ulen i=1; i<len2 ;i++) FFT2(ptr[i],ptr[i+len2],root(i*count));
       }
    }

  // permutate

  for(ulen i=0; i<buflen ;i++)
    {
     ulen j=bitrev(i);

     if( i<j ) Swap(buf[i],buf[j]);
    }
 }

/* classes */

template <class T> struct FFTRootAlgo;

template <class T,class Algo=FFTRootAlgo<T> > class Radix2FFTEngine;

/* struct FFTRootAlgo */

template <class T>
struct FFTRootAlgo
 {
  static T Root(ulen ind,ulen len) // ind > 0 && ind < len/2 , len is 2^order
   {
    return T::UniExp(ind,len);
   }

  static T Inv(T root) { return root.conj(); }
 };

/* class Radix2FFTEngine<T,Algo> */

template <class T,class Algo>
class Radix2FFTEngine : NoCopy
 {
   unsigned max_order;
   SimpleArray<T> buf;
   SimpleArray<T> roots;
   SimpleArray<ulen> bitrev;

  public:

   explicit Radix2FFTEngine(unsigned max_order);

   ~Radix2FFTEngine();

   T * getPtr() { return buf.getPtr(); }

   const T * getPtr() const { return buf.getPtr(); }

   const T * getPtr_const() const { return buf.getPtr(); }

   ulen getLen() const { return buf.getLen(); }

   T getRoot(ulen ind) const { return roots[ind-1]; } // ind > 0 && ind < len/2 , len is 2^max_order

   void run(unsigned order,bool inv);
 };

template <class T,class Algo>
Radix2FFTEngine<T,Algo>::Radix2FFTEngine(unsigned max_order_)
 : max_order(max_order_)
 {
  ulen cap=Meta::UIntBits<ulen>-1;

  if( max_order>cap )
    {
     GuardOrder(max_order,cap);
    }

  if( max_order==0 ) return;

  ulen len=ulen(1)<<max_order;
  ulen len2=len/2;

  buf=SimpleArray<T>(len);
  roots=SimpleArray<T>(len2-1);
  bitrev=SimpleArray<ulen>(len);

  for(ulen ind=0; ind<len ;ind++) bitrev[ind]=BitReverse(max_order,ind);

  for(ulen ind=0; ind<len2-1 ;ind++) roots[ind]=Algo::Root(ind+1,len);
 }

template <class T,class Algo>
Radix2FFTEngine<T,Algo>::~Radix2FFTEngine()
 {
 }

template <class T,class Algo>
void Radix2FFTEngine<T,Algo>::run(unsigned order,bool inv)
 {
  if( order==0 ) return;

  if( order>max_order )
    {
     GuardOrder(order,max_order);
    }

  ulen len=ulen(1)<<order;
  ulen shift=max_order-order;

  if( inv )
    {
     Radix2FFT(getPtr(),len, [this,shift] (ulen ind) { return Algo::Inv(roots[(ind<<shift)-1]); } ,
                             [this,shift] (ulen ind) { return bitrev[ind]>>shift; } );
    }
  else
    {
     Radix2FFT(getPtr(),len, [this,shift] (ulen ind) { return roots[(ind<<shift)-1]; } ,
                             [this,shift] (ulen ind) { return bitrev[ind]>>shift; } );
    }
 }

} // namespace Math
} // namespace CCore

#endif

