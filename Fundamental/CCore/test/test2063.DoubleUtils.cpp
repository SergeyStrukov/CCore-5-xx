/* test2063.DoubleUtils.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <cmath>

#include <CCore/inc/math/DoubleUtils.h>
#include <CCore/inc/Print.h>
#include <CCore/inc/Random.h>

#include <CCore/inc/math/IntegerFastAlgo.h>
#include <CCore/inc/math/Integer.h>

namespace App {

namespace Private_2063 {

/* toText() */

const char * toText(IntShowSign val)
 {
  switch( val )
    {
     case IntShowMinus : return  "";
     case IntShowPlus  : return "+";
     default: return "???";
    }
 }

const char * toText(DoubleFmt val)
 {
  switch( val )
    {
     case DoubleFmtF : return "F";
     case DoubleFmtE : return "E";
     case DoubleFmtX : return "X";
     default: return "???";
    }
 }

const char * toText(DoubleAlign val)
 {
  switch( val )
    {
     case DoubleAlignLeft  : return "L";
     case DoubleAlignRight : return "R";
     default: return "???";
    }
 }

/* ShowOpt() */

void ShowOpt(StrLen opt)
 {
  DoublePrintOpt obj(opt.ptr,opt.ptr+opt.len);

  Printf(Con,"#; #; #; .#; .e#; #;\n",toText(obj.show_sign),obj.width,toText(obj.fmt),
                                      obj.digit_len,obj.exp_len,toText(obj.align));
 }

/* test1() */

void test1()
 {
  ShowOpt("20");
  ShowOpt("+20.8");
  ShowOpt("+20.8L");
  ShowOpt("+20.8R");
  ShowOpt("+20x.8.e15R");
 }

/* test2() */

void test2()
 {
  Printf(Con,"#;\n",3.14159);
  Printf(Con,"#;\n",1.23456E+100);
  Printf(Con,"#;\n",1.23456E-100);
  Printf(Con,"#;\n",1.23456E+308);
  Printf(Con,"#;\n",1.23456E-308);
  Printf(Con,"#.20;\n",1.234567890123456789E+100);
  Printf(Con,"#.20;\n",1.234567890123456789E-100);
  Printf(Con,"| #30xL; |\n",1.);
  Printf(Con,"| #30xR; |\n",1000000000.);

  for(int e=-50; e<=50 ;e++)
    Printf(Con,"| #30.20L; |\n",1.234567890123456789*std::pow(10.,e));

  for(int e=-50; e<=50 ;e++)
    Printf(Con,"| #30R; |\n",1.234567890123456789*std::pow(10.,e));
 }

/* test3() */

void test3()
 {
  DoubleFormat obj;

  obj.setExp(DoubleFormat::MaxExp);

  Printf(Con,"#;\n",obj.get());

  obj.setFract(1);

  Printf(Con,"#;\n",obj.get());

  Printf(Con,"#;\n",0.);
 }

/* test4() */

void test4()
 {
  Printf(Con,"#x;\n",std::ldexp(1.,1023));         // max pow2
  Printf(Con,"#x;\n",0xF'FFFF'FFFF'FFFFP972);      // max
  Printf(Con,"#x;\n",std::ldexp(2.-0x1P-51,1023)); // max
  Printf(Con,"#x;\n",std::ldexp(1.,1024));         // inf
  Printf(Con,"#x;\n",std::ldexp(1.,-1074));        // min
  Printf(Con,"#x;\n",std::ldexp(1.,-1075));        // 0
 }

/* test5() */

void test5()
 {
  static constexpr int MinDecExp = -323 ;
  static constexpr int MaxDecExp =  308 ;

  PrintFile out("test.txt");

  for(int dec_exp=MinDecExp; dec_exp<=MaxDecExp ;dec_exp++)
    {
     double lo=DoubleTo10Based::Pow10lo(dec_exp);
     double hi=DoubleTo10Based::Pow10hi(dec_exp);

     Printf(out,"#; #.16; #.16;\n",dec_exp,lo,hi);
    }
 }

/* class Engine */

class Engine : NoCopy
 {
   PrintFile out;
   Random random;

  private:

   static constexpr int DExp = DoubleFormat::ExpBias+DoubleFormat::FractBits ;
   static constexpr int MinBinExp = 1-DExp ;
   static constexpr int MinNormBinExp = 1-DoubleFormat::ExpBias ;
   static constexpr int MaxBinExp = DoubleFormat::MaxExp-1-DoubleFormat::ExpBias ;

   using BodyType = DoubleFormat::BodyType;

   using Int = Math::Integer<Math::IntegerFastAlgo> ;

   struct Delta
    {
     Int x;
     Int y;

     Delta(BodyType bin,int bin_exp,BodyType dec,int dec_exp)
      {
       if( dec_exp>=0 )
         {
          if( bin_exp>=0 )
            {
             y=Int(10).pow(dec_exp);
             x=dec*y-bin*Int(2).pow(bin_exp);
            }
          else
            {
             y=Int(10).pow(dec_exp)*Int(2).pow(-bin_exp);
             x=dec*y-bin;
            }
         }
       else
        {
         Int c=bin*Int(10).pow(-dec_exp);

         if( bin_exp>=0 )
           {
            y=1;
            x=dec-c*Int(2).pow(bin_exp);
           }
         else
           {
            y=Int(2).pow(-bin_exp);
            x=dec*y-c;
           }
        }
      }

     struct Val
      {
       bool pos;
       unsigned val;

       void print(PrinterType auto &out) const
        {
         if( pos )
           {
            Printf(out,"+ #.f3;",val);
           }
         else
           {
            Printf(out,"- #.f3;",val);
           }
        }
      };

     Val get() const
      {
       bool pos;
       Int t;

       if( x>=0 )
         {
          t=(1000*x)/y;
          pos=false;
         }
       else
         {
          t=(-1000*x)/y;
          pos=true;
         }

       if( t>100'000 )
         {
          Printf(Exception,"fail");
         }

       return {pos,t.cast<unsigned>()};
      }
    };

   void test(double value,unsigned digit_len)
    {
     DoubleTo2Based bin(value);
     DoubleTo10Based dec(value,digit_len);

     Delta delta(bin.base,bin.bin_exp,dec.base,dec.dec_exp);
     auto val=delta.get();

     addVal(val);

     if( !val.pos && val.val>=1000 )
       {
        Printf(out,"  #; 2^#; ",bin.base,bin.bin_exp);
        Printf(out," ->  #; #; 10^#;\n",dec.base,val,dec.dec_exp);
       }
    }

   void test(double value)
    {
     for(unsigned digit_len=6; digit_len<=16 ;digit_len++)
       {
        test(value,digit_len);
       }
    }

   BodyType genBody() { return random.next_uint<BodyType>(); }

   double gen(int bin_exp,unsigned ones)
    {
     DoubleFormat obj;

     static constexpr auto MaxFract = DoubleFormat::MaxFract;
     static constexpr auto ExpBias = DoubleFormat::ExpBias;

     if( bin_exp>=MinNormBinExp )
       {
        obj.setExp(bin_exp+ExpBias);

        auto fract=genBody();
        auto mask=MaxFract^(MaxFract>>ones);

        obj.setFract(fract|mask);
       }
     else
       {
        obj.setExp(0);

        auto fract=MaxFract+1;
        auto mask=MaxFract^(MaxFract>>ones);

        obj.setFract((fract|mask)>>(MinNormBinExp-bin_exp));
       }

     return obj.get();
    }

  private:

   unsigned posVal = 0 ;
   unsigned negVal = 0 ;

   void addVal(Delta::Val val)
    {
     if( val.pos )
       {
        Replace_max(posVal,val.val);
       }
     else
       {
        Replace_max(negVal,val.val);
       }
    }

  public:

   Engine() : out("test.txt") {}

   ~Engine() {}

   void step()
    {
     for(int bin_exp=MinBinExp; bin_exp<=MaxBinExp ;bin_exp++)
       {
        test(gen(bin_exp,0));
       }
    }

   void run(unsigned count=10000)
    {
     while( count-- )
       {
        step();

        if( count<10 || (count%100)==0 ) Printf(Con,"count = #;\n",count);
       }

     Printf(Con,"- #.f3;\n",negVal);
     Printf(Con,"+ #.f3;\n",posVal);
    }
  };

/* test6() */

void test6()
 {
  Engine engine;

  engine.run();
 }

} // namespace Private_2063

using namespace Private_2063;

/* Testit<2063> */

template<>
const char *const Testit<2063>::Name="Test2063 DoubleUtils";

template<>
bool Testit<2063>::Main()
 {
  //test1();
  test2();
  //test3();
  //test4();
  //test5();
  //test6();

  return true;
 }

} // namespace App

