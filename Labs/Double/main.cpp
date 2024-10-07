/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: Labs/Double
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Print.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/math/IntegerFastAlgo.h>
#include <CCore/inc/math/Integer.h>

#include <CCore/inc/PrintStem.h>

#include <CCore/inc/Array.h>

#include <cmath>

#include <CCore/inc/math/DoubleUtils.h>

namespace App {

using namespace CCore;

/* type Int */

using Int = Math::Integer<Math::IntegerFastAlgo> ;

/* consts */

static constexpr int DExp = DoubleFormat::ExpBias+DoubleFormat::FractBits ;
static constexpr int MinBinExp = 1-DExp ;
static constexpr int MaxBinExp = DoubleFormat::MaxExp-1-DoubleFormat::ExpBias ;

/* struct Rec */

struct Rec
 {
  int bin_exp;
  int dec_exp;
  Int pow2;  // 2^abs(bin_exp)
  Int pow10; // 10^abs(dec_exp)

  void print(PrinterType auto &out) const
   {
    if( bin_exp>=0 )
      Printf(out,"#; #; #; #;",bin_exp,dec_exp,pow2,pow10);
    else
      Printf(out,"#; #; 1/#; 1/#;",bin_exp,dec_exp,pow2,pow10);
   }
 };

/* class RecTable */

class RecTable : NoCopy
 {
   Rec table[MaxBinExp-MinBinExp+1];

  public:

   RecTable() {}

   ~RecTable() {}

   Rec & operator [] (int bin_exp) { return table[bin_exp-MinBinExp]; }
 };

static RecTable Table;

/* Pow10 */

static int MinDecExp=0;
static int MaxDecExp=0;

struct Span
 {
  double lo;
  double hi;
 };

class PowTable : NoCopy
 {
   DynArray<Span> table;

  public:

   PowTable() {}

   ~PowTable() {}

   void alloc() { table.extend_raw(MaxDecExp-MinDecExp+1); }

   Span & operator [] (int dec_exp) { return table[dec_exp-MinDecExp]; }

   auto getRange() { return Range(table); }
 };

static PowTable Pow10;

/* functions */

double BaseToDouble(Int x)
 {
  return x.cast<DoubleFormat::BodyType>();
 }

double BaseToDouble(Int x,int e)
 {
  return std::ldexp(x.cast<DoubleFormat::BodyType>(),e);
 }

Span ToDouble(Int x)
 {
  unsigned bits=x.bitsOf().total();
  unsigned fit=DoubleFormat::FractBits+1;

  if( bits>fit )
    {
     unsigned extra=bits-fit;

     Int y=x>>extra; // fit bits

     double lo=BaseToDouble(y,extra);

     if( x>(y<<extra) )
       {
        double hi=BaseToDouble(y+1,extra);

        return {lo,hi};
       }
     else
       {
        return {lo,lo};
       }
    }
  else
    {
     double lo=BaseToDouble(x);

     return {lo,lo};
    }
 }

Span InvToDouble(Int x) // not pow2
 {
  unsigned bits=x.bitsOf().total();
  unsigned extra=DoubleFormat::FractBits;
  unsigned cap=DoubleFormat::ExpBias-1;

  if( bits>cap )
    {
     if( bits>=extra+cap )
       {
        extra=0;
       }
     else
       {
        extra-=(bits-cap);
       }
    }

  unsigned t=bits+extra;

  Int y=Int(2).pow(t)/x;

  int e=-(int)t;

  double lo=BaseToDouble(y,e);
  double hi=BaseToDouble(y+1,e);

  return {lo,hi};
 }

/* Main1() */

void MakeTable()
 {
  Int x(1);
  Int y(10);
  int dec_exp=1;

  Table[0]={0,dec_exp,x,y};

  for(int bin_exp=1; bin_exp<=MaxBinExp ;bin_exp++)
    {
     x*=2;

     if( x>=y )
       {
        dec_exp++;
        y*=10;
       }

     Table[bin_exp]={bin_exp,dec_exp,x,y};
    }

  x=2;
  y=1;
  dec_exp=0;

  Table[-1]={-1,dec_exp,x,y};

  for(int bin_exp=-2; bin_exp>=MinBinExp ;bin_exp--)
    {
     x*=2;

     Int y1=10*y;

     if( x>y1 )
       {
        dec_exp--;
        y=y1;
       }

     Table[bin_exp]={bin_exp,dec_exp,x,y};
    }
 }

void MakePow10()
 {
  Int y(1);

  Pow10[0]={1,1};

  for(int dec_exp=1; dec_exp<=MaxDecExp ;dec_exp++)
    {
     y*=10;

     Pow10[dec_exp]=ToDouble(y);
    }

  y=1;

  for(int dec_exp=-1; dec_exp>=MinDecExp ;dec_exp--)
    {
     y*=10;

     Pow10[dec_exp]=InvToDouble(y);
    }
 }

void PrintExp(PrintBase &out)
 {
  PrintPeriod stem(20,"    "_c," , "_c," ,\n    "_c);

  for(int bin_exp=MinBinExp; bin_exp<=MaxBinExp ;bin_exp++)
    {
     Printf(out,"#;#4;",*(stem++),Table[bin_exp].dec_exp);
    }

  Printf(out,"\n\n#;\n\n",TextDivider());
 }

void PrintLo(PrintBase &out)
 {
  Printf(out,"Pow10lo\n\n");

  PrintPeriod stem(8,"    "_c," , "_c," ,\n    "_c);

  for(Span span : Pow10.getRange() )
    {
     Printf(out,"#;#22x;",*(stem++),span.lo);
    }

  Printf(out,"\n\n#;\n\n",TextDivider());
 }

void PrintHi(PrintBase &out)
 {
  Printf(out,"Pow10hi\n\n");

  PrintPeriod stem(8,"    "_c," , "_c," ,\n    "_c);

  for(Span span : Pow10.getRange() )
    {
     Printf(out,"#;#22x;",*(stem++),span.hi);
    }

  Printf(out,"\n\n#;\n\n",TextDivider());
 }

void PrintData()
 {
  PrintFile out("out/data.txt");

  Printf(out,"MinDecExp = #; MaxDecExp = #;\n\n",MinDecExp,MaxDecExp);

  Printf(out,"#;\n\n",TextDivider());

  PrintExp(out);
  PrintLo(out);
  PrintHi(out);
 }

void Main1()
 {
  Printf(Con,"DExp = #; MinBinExp = #; MaxBinExp = #;\n",DExp,MinBinExp,MaxBinExp);

  MakeTable();

  MinDecExp=Table[MinBinExp].dec_exp;
  MaxDecExp=Table[MaxBinExp].dec_exp;

  Pow10.alloc();

  MakePow10();

  PrintData();
 }

} // namespace App

/* main() */

using namespace App;

int main()
 {
  try
    {
     ReportException report;

     Main1();

     report.guard();

     return 0;
    }
  catch(CatchType)
    {
     return 1;
    }
 }

