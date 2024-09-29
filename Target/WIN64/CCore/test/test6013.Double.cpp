/* test6013.Double.cpp */
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

#include <CCore/test/test.h>

#include <math.h>

#include <CCore/inc/base/DoubleFormat.h>
#include <CCore/inc/Random.h>

namespace CCore {

/* struct PrintProxy<DoubleFormat> */

template <>
struct PrintProxy<DoubleFormat>
 {
  struct ProxyType
   {
    DoubleFormat obj;

    explicit ProxyType(const DoubleFormat &obj_) : obj(obj_) {}

    void print(PrinterType auto &out) const
     {
      Printf(out,"( #; , #; , #; )",obj.getSign(),obj.getExp(),obj.getFract());
     }
   };
 };

} // namespace CCore

namespace App {

namespace Private_6013 {

/* test1() */

void test1()
 {
  DoubleFormat obj(1.);

  Printf(Con,"#;\n",obj);
 }

/* class Engine */

class Engine : NoCopy
 {
   Random random;

  public:

   Engine() {}

   double gen()
    {
     DoubleFormat obj;

     obj.body=random.next_uint<DoubleFormat::BodyType>();

     unsigned sign=random.next8()&1u;
     unsigned exp=random.select(1,DoubleFormat::MaxExp-1);
     DoubleFormat::BodyType fract=random.next_uint<DoubleFormat::BodyType>()&DoubleFormat::MaxFract;

     obj.setSign(sign);
     obj.setExp(exp);
     obj.setFract(fract);

     if( obj.getSign()!=sign || obj.getExp()!=exp || obj.getFract()!=fract )
       {
        Printf(Exception,"fail 1");
       }

     return obj.get();
    }

   static double MakeVal(const DoubleFormat &obj)
    {
     double val=1+ldexp(obj.getFract(),-(int)DoubleFormat::FractBits);

     val=ldexp(val,(int)obj.getExp()-(int)DoubleFormat::ExpBias);

     if( obj.getSign() ) val=-val;

     return val;
    }

   void test(double val)
    {
     DoubleFormat obj(val);

     if( val!=MakeVal(obj) )
       {
        Printf(Exception,"fail 2");
       }
    }

   void run(unsigned count=1'000'000)
    {
     for(; count ;count--)
       {
        test(gen());
       }
    }
 };

/* test2() */

void test2()
 {
  Engine engine;

  engine.run();
 }

} // namespace Private_6013

using namespace Private_6013;

/* Testit<6013> */

template<>
const char *const Testit<6013>::Name="Test6013 Double";

template<>
bool Testit<6013>::Main()
 {
  test1();
  test2();

  return true;
 }

} // namespace App

