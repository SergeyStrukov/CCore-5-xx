/* test3042.Radix2FFT.cpp */
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

#include <CCore/test/test.h>

#include <CCore/inc/math/DoubleUtils.h>
#include <CCore/inc/math/Complex.h>
#include <CCore/inc/math/Radix2FFT.h>

namespace App {

namespace Private_3042 {

using complex = Math::Complex<double> ;

/* test1() */

void test1(unsigned val)
 {
  Printf(Con,"#11.bi; -> #11.bi;\n",val,Math::BitReverse(10,val));
 }

void test1()
 {
  test1(1);
  test1(0x1BC);
 }

/* test2() */

double test2(unsigned order,ulen freq,bool inv,unsigned ext)
 {
  //Printf(Con,"order = #; freq = #;\n",order,freq);

  Math::Radix2FFTEngine<complex> engine(order+ext);

  ulen len=ulen(1)<<order;

  if( inv )
    {
     for(ulen ind=0; ind<len ;ind++) engine.getPtr()[ind]=complex::UniExp((ind*freq)%len,len);
    }
  else
    {
     for(ulen ind=0; ind<len ;ind++) engine.getPtr()[ind]=complex::UniExp((ind*freq)%len,len).conj();
    }

  engine.run(order,inv);

  double diff = 0 ;

  for(ulen ind=0; ind<len ;ind++)
    {
     complex x=engine.getPtr()[ind];

     if( ind==freq )
       {
        Replace_max(diff,Math::SupNorm(x-len));
       }
     else
       {
        Replace_max(diff,Math::SupNorm(x));
       }
    }

  return diff;
 }

void test2(unsigned order,bool inv,unsigned ext=0)
 {
  ulen len = ulen(1)<<order ;

  double diff = 0 ;

  for(ulen freq=0; freq<len ;freq++) Replace_max(diff,test2(order,freq,inv,ext));

  Printf(Con,"diff = #;\n",diff);
 }

void test2()
 {
  test2(10,true);
  test2(10,false);
  test2(10,true,4);
  test2(10,false,4);
 }

} // namespace Private_3042

using namespace Private_3042;

/* Testit<3042> */

template<>
const char *const Testit<3042>::Name="Test3042 Radix2FFT";

template<>
bool Testit<3042>::Main()
 {
  //test1();
  test2();

  return true;
 }

} // namespace App

