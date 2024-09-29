/* Radix2FFT.cpp */
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

#include <CCore/inc/math/Radix2FFT.h>

#include <CCore/inc/Exception.h>

namespace CCore {
namespace Math {

/* guard functions */

void GuardOrder(unsigned order,unsigned max_order)
 {
  Printf(Exception,"CCore::Math::GuardOrder(#;,max = #;) : FFT order exceeds limit",order,max_order);
 }

/* functions */

ulen BitReverse(unsigned order,ulen ind)
 {
  ulen ret=0;
  ulen bit=1;
  ulen rbit=ulen(1)<<(order-1);

  for(; rbit ;bit<<=1,rbit>>=1) if( ind&bit ) ret|=rbit;

  return ret;
 }

} // namespace Math
} // namespace CCore
