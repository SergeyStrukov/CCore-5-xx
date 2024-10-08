/* test3025.DHExp.cpp */
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
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/crypton/DHExp.h>

#include <CCore/inc/math/Integer.h>
#include <CCore/inc/math/IntegerFastAlgo.h>
#include <CCore/inc/math/IntegerOctetBuilder.h>
#include <CCore/inc/math/NoPrimeTest.h>
#include <CCore/inc/Random.h>

namespace App {

namespace Private_3025 {

/* types */

using Int = Math::Integer<Math::IntegerFastAlgo> ;

/* class UBuilder */

class UBuilder
 {
   PtrLen<const Int::Unit> data;

  public:

   explicit UBuilder(PtrLen<const Int::Unit> data_) : data(data_) {}

   ulen getLen() const { return LenAdd(data.len,1); }

   PtrLen<Int::Unit> operator () (Place<void> place) const
    {
     Int::Unit *base=place;

     data.copyTo(base);

     base[data.len]=0;

     return Range(base,data.len+1);
    }
 };

/* test1() */

template <class DHMod>
void test1(StrLen title)
 {
  Random random;
  Printf(Con,"#;\n\n",Title(title));

  Math::OctetInteger<Int> P(Range(DHMod::Mod));
  Int Q=P>>1;

  if( Math::NoPrimeTest<Int>::RandomTest(P,100,random) )
    Printf(Con,"P is probably prime\n");
  else
    Printf(Con,"P is not prime\n");

  if( Math::NoPrimeTest<Int>::RandomTest(Q,100,random) )
    Printf(Con,"Q is probably prime\n");
  else
    Printf(Con,"Q is not prime\n");

  Printf(Con,"\n");
 }

/* test2() */

struct XY
 {
  Int x;
  Int y;

  XY(Int a,Int b)
   {
    if( a>b )
      {
       auto divmod=a.divmod(b);

       if( !divmod.mod )
         {
          x=1;
          y=divmod.div-1;
         }
       else
         {
          XY next(divmod.mod,b);

          x=next.x;
          y=next.y+divmod.div*next.x;
         }
      }
    else
      {
       auto divmod=b.divmod(a);

       if( !divmod.mod )
         {
          x=divmod.div+1;
          y=1;
         }
       else
         {
          XY next(a,divmod.mod);

          x=next.x+divmod.div*next.y;
          y=next.y;
         }
      }

   }

  XY(Int a,Int b,Int &d)
   : XY(a,b)
   {
    d=a*x-b*y;
   }
 };

template <class DHMod,class P>
void test2(P &out,StrLen title)
 {
  Printf(out,"#;\n\n",Title(title));

  Math::OctetInteger<Int> M(Range(DHMod::Mod));
  Math::UnitsPowInteger<Int> D(M.getBody().len-1);

  Int d;

  XY xy(M,D,d);

  Printf(Con,"d = #;\n",d);

  Int K=xy.x%D;

  Printf(out,"#;;\n\n",Math::PrintIntegerOctets(M));
  Printf(out,"#;;\n\n",Math::PrintIntegerOctets(D));
  Printf(out,"#;;\n\n",Math::PrintIntegerOctets(K));

  Int L=((D-M)*D)/M;

  Printf(out,"#;;\n\n",Math::PrintIntegerOctets(L));
 }

/* test3() */

template <class DHMod>
void test3(StrLen title)
 {
  Printf(Con,"#;\n\n",Title(title));

  Math::OctetInteger<Int> M(Range(DHMod::Mod));
  Math::OctetInteger<Int> K(Range(DHMod::InvMod));
  Math::OctetInteger<Int> L(Range(DHMod::Lift));

  Math::UnitsPowInteger<Int> D(M.getBody().len-1);

  if( ((M*K-1)%D).sign() )
    {
     Printf(Exception,"failed");
    }

  if( L!=((D-M)*D)/M )
    {
     Printf(Exception,"failed");
    }

  Printf(Con,"#;\n\n",Math::PrintIntegerOctets(M*K));
 }

/* test4() */

template <class DHMod>
void test4(StrLen title)
 {
  Printf(Con,"#;\n\n",Title(title));

  using Exp = Crypton::DHExp<DHMod> ;
  using Unit = typename Exp::Unit ;

  typename Exp::Core core;

  Random random;

  Unit A[Exp::IntLen];
  Unit B[Exp::IntLen];
  Unit C[Exp::IntLen];

  Math::OctetInteger<Int> m(Range(DHMod::Mod));

  for(ulen cnt=10000; cnt ;cnt--)
    {
     random.fill(Range(A));
     random.fill(Range(B));

     core.mul(A,B,C);

     core.direct(A,A);
     core.direct(B,B);
     core.direct(C,C);

     Int a(DoBuild,UBuilder(Range_const(A)));
     Int b(DoBuild,UBuilder(Range_const(B)));
     Int c(DoBuild,UBuilder(Range_const(C)));

     if( (a*b-c)%m != 0 )
       {
        Printf(Exception,"failed");
       }
    }
 }

/* test5() */

template <class DHMod>
void test5(StrLen title)
 {
  Printf(Con,"#;\n\n",Title(title));

  using Exp = Crypton::DHExp<DHMod> ;
  using Unit = typename Exp::Unit ;

  Random random;

  typename Exp::Core core;

  Unit A[Exp::IntLen];
  Unit B[Exp::IntLen];
  Unit C[Exp::IntLen];

  Math::OctetInteger<Int> m(Range(DHMod::Mod));
  Math::UnitsPowInteger<Int> d(m.getBody().len-1);

  for(ulen cnt=10000; cnt ;cnt--)
    {
     random.fill(Range(A));

     ulen len=random.select_uint<ulen>(Exp::IntLen);

     Range(A).suffix(len).set(Unit(-1));

     core.inverse(A,B);
     core.direct(B,C);

     Int a(DoBuild,UBuilder(Range_const(A)));
     Int b(DoBuild,UBuilder(Range_const(B)));
     Int c(DoBuild,UBuilder(Range_const(C)));

     if( c != a%m )
       {
        Printf(Exception,"failed");
       }

     if( (a*d-b)%m != 0 )
       {
        Printf(Exception,"failed");
       }
    }
 }

/* test6() */

template <class DHMod>
void test6(StrLen title)
 {
  Printf(Con,"#;\n\n",Title(title));

  using Exp = Crypton::DHExp<DHMod> ;

  Random random;

  Exp exp;

  uint8 x[DHMod::GLen];
  uint8 y[DHMod::GLen];
  uint8 gx[DHMod::GLen];
  uint8 gy[DHMod::GLen];
  uint8 gxy[DHMod::GLen];
  uint8 gyx[DHMod::GLen];

  for(ulen cnt=100; cnt ;cnt--)
    {
     random.fill(Range(x));
     random.fill(Range(y));

     exp.pow(x,gx);
     exp.pow(gx,y,gxy);

     exp.pow(y,gy);
     exp.pow(gy,x,gyx);

     if( !Range(gxy).equalTo(gyx) )
       {
        Printf(Exception,"failed");
       }
    }
 }

} // namespace Private_3025

using namespace Private_3025;

/* Testit<3025> */

template<>
const char *const Testit<3025>::Name="Test3025 DHExp";

template<>
bool Testit<3025>::Main()
 {
  test1<Crypton::DHModI>("DH Group I module");
  test1<Crypton::DHModII>("DH Group II module");

  PrintFile out("test3025.txt");

  test2<Crypton::DHModI>(out,"DH Group I module");
  test2<Crypton::DHModII>(out,"DH Group II module");

  test3<Crypton::DHModI>("DH Group I module");
  test3<Crypton::DHModII>("DH Group II module");

  test4<Crypton::DHModI>("DH Group I module");
  test4<Crypton::DHModII>("DH Group II module");

  test5<Crypton::DHModI>("DH Group I module");
  test5<Crypton::DHModII>("DH Group II module");

  test6<Crypton::DHModI>("DH Group I module");
  test6<Crypton::DHModII>("DH Group II module");

  return true;
 }

} // namespace App

