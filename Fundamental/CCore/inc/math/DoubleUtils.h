/* DoubleUtils.h */
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

#ifndef CCore_inc_math_DoubleUtils_h
#define CCore_inc_math_DoubleUtils_h

#include <CCore/inc/base/DoubleFormat.h>

#include <CCore/inc/Printf.h>

namespace CCore {

/* classes */

//enum DoubleKind;

//enum DoubleAlign;

//enum DoubleFmt;

struct DoublePrintOpt;

struct DoubleTo2Based;

struct DoubleTo10Based;

class DoubleToDec;

class DoubleToHex;

class DoubleDecPrint;

class DoubleHexPrint;

class DoublePrint;

struct DoublePrintProxy;

/* enum DoubleKind */

enum DoubleKind
 {
  DoubleIsNan,
  DoubleIsInf,
  DoubleIsPos,
  DoubleIsNull,
  DoubleIsNeg
 };

/* enum DoubleAlign */

enum DoubleAlign
 {
  DoubleAlignLeft,
  DoubleAlignRight,

  DoubleAlignDefault = DoubleAlignRight
 };

template <CharPeekType Dev>
void Parse_empty(Dev &dev,DoubleAlign &ret)
 {
  typename Dev::Peek peek(dev);

  if( !peek )
    {
     ret=DoubleAlignDefault;
    }
  else
    {
     switch( *peek )
       {
        case 'l' : case 'L' : ret=DoubleAlignLeft;  ++dev; break;
        case 'r' : case 'R' : ret=DoubleAlignRight; ++dev; break;

        default: ret=DoubleAlignDefault;
       }
    }
 }

/* enum DoubleFmt */

enum DoubleFmt
 {
  DoubleFmtF,
  DoubleFmtE,
  DoubleFmtX,

  DoubleFmtDefault = DoubleFmtF
 };

template <CharPeekType Dev>
void Parse_empty(Dev &dev,DoubleFmt &ret)
 {
  typename Dev::Peek peek(dev);

  if( !peek )
    {
     ret=DoubleFmtDefault;
    }
  else
    {
     switch( *peek )
       {
        case 'e' : case 'E' : ret=DoubleFmtE; ++dev; break;
        case 'f' : case 'F' : ret=DoubleFmtF; ++dev; break;
        case 'x' : case 'X' : ret=DoubleFmtX; ++dev; break;

        default: ret=DoubleFmtDefault;
       }
    }
 }

/* struct DoublePrintOpt */

enum DoubleLenPrefix
 {
  DoubleLenNone,
  DoubleDigitLen,
  DoubleExpLen
 };

template <CharPeekType Dev>
DoubleLenPrefix Parse_DoubleLenPrefix(Dev &dev)
 {
  if( ParseChar_try(dev,'.') )
    {
     if( ParseChar_try(dev,'e') )
       {
        return DoubleExpLen;
       }
     else
       {
        return DoubleDigitLen;
       }
    }
  else
    {
     return DoubleLenNone;
    }
 }

struct DoublePrintOpt
 {
  static constexpr unsigned DefaultDigitLen =  6 ;
  static constexpr unsigned DefaultExpLen   = 10 ;

  ulen width;
  unsigned digit_len; // 3.14 -> 3
  unsigned exp_len;
  DoubleFmt fmt;
  DoubleAlign align;
  IntShowSign show_sign;

  void setDefault()
   {
    width=0;
    digit_len=DefaultDigitLen;
    exp_len=DefaultExpLen;
    fmt=DoubleFmtDefault;
    align=DoubleAlignDefault;
    show_sign=IntShowSignDefault;
   }

  DoublePrintOpt() { setDefault(); }

  DoublePrintOpt(const char *ptr,const char *lim);

  //
  // [+][width=0][e|E|f|F|x|X=f][.<digit_len>=.6][.e<exp_len=.e10>][l|L|r|R=R]
  //
 };

/* struct DoubleTo2Based */

struct DoubleTo2Based : NoCopy
 {
  DoubleKind kind;

  // only for Pos or Neg values

  using BaseType = uint64;

  int bin_exp;
  BaseType base; // odd

  // base*2^bin_exp

  void correct();

  explicit DoubleTo2Based(double value);
 };

/* struct DoubleTo10Based */

struct DoubleTo10Based : NoCopy
 {
  DoubleKind kind;

  // only for Pos or Neg values

  using BaseType = uint64;

  int dec_exp;
  BaseType base;

  // base*10^dec_exp

  static double Pow10lo(int dec_exp);
  static double Pow10hi(int dec_exp);
  static int DecExp(int bin_exp);
  static int DecExp(double value,int bin_exp);

  void complete(double value,unsigned digit_len,int bin_exp);

  DoubleTo10Based(double value,unsigned digit_len);
 };

/* class DoubleToDec */

class DoubleToDec : NoCopy
 {
   DoubleKind kind;

   // only for Pos or Neg values

   int dec_exp;
   IntToStr base;

  private:

   void printDec(PrinterType auto &out) const
    {
     StrLen str=base.getStr();

     if( !str )
       {
        out.put('0');
        return;
       }

     if( dec_exp>=0 )
       {
        out.put(str.ptr,str.len);
        out.put('0',ulen(dec_exp));
        out.put('.');
       }
     else
       {
        ulen shift=ulen(-dec_exp);

        if( shift<str.len )
          {
           ulen first=str.len-shift;

           out.put(str.ptr,first); str+=first;
           out.put('.');
           out.put(str.ptr,str.len);
          }
        else
          {
           out.put('0');
           out.put('.');
           if( shift>str.len ) out.put('0',shift-str.len);
           out.put(str.ptr,str.len);
          }
       }
    }

   void printExp(PrinterType auto &out) const
    {
     StrLen str=base.getStr();

     if( !str )
       {
        out.put('0');
        return;
       }

     out.put(*str); ++str;
     out.put('.');
     out.put(str.ptr,str.len);
     out.put('E');

     int exp=dec_exp+(int)str.len;

     Printf(out,"#+;",exp);
    }

  public:

   DoubleToDec(double value,unsigned digit_len);

   ulen getDecDigit() const
    {
     StrLen str=base.getStr();

     if( !str ) return 1;

     if( dec_exp>=0 )
       {
        return str.len+ulen(dec_exp);
       }
     else
       {
        ulen shift=ulen(-dec_exp);

        if( shift<str.len )
          {
           return str.len;
          }
        else
          {
           return shift+1;
          }
       }
    }

   ulen getDecLen(IntShowSign show_sign) const;

   ulen getExpLen(IntShowSign show_sign) const;

   void printDec(PrinterType auto &out,IntShowSign show_sign) const
    {
     switch( kind )
       {
        case DoubleIsNan :
         {
          Putobj(out,"NaN"_c);
         }
        break;

        case DoubleIsInf :
         {
          Putobj(out,"Inf"_c);
         }
        break;

        case DoubleIsPos :
         {
          if( show_sign==IntShowPlus ) out.put('+');
          printDec(out);
         }
        break;

        case DoubleIsNull :
         {
          out.put('0');
         }
        break;

        case DoubleIsNeg :
         {
          out.put('-');
          printDec(out);
         }
        break;
       }
    }

   void printExp(PrinterType auto &out,IntShowSign show_sign) const
    {
     switch( kind )
       {
        case DoubleIsNan :
         {
          Putobj(out,"NaN"_c);
         }
        break;

        case DoubleIsInf :
         {
          Putobj(out,"Inf"_c);
         }
        break;

        case DoubleIsPos :
         {
          if( show_sign==IntShowPlus ) out.put('+');
          printExp(out);
         }
        break;

        case DoubleIsNull :
         {
          out.put('0');
         }
        break;

        case DoubleIsNeg :
         {
          out.put('-');
          printExp(out);
         }
        break;
       }
    }
 };

/* class DoubleToHex */

class DoubleToHex : NoCopy
 {
   DoubleKind kind;

   // only for Pos or Neg values

   int bin_exp;
   IntToStr base;

  private:

   void printExp(PrinterType auto &out) const
    {
     StrLen str=base.getStr();

     if( !str )
       {
        out.put('0');
        return;
       }

     Printf(out,"0x#;P#;",str,bin_exp);
    }

  public:

   explicit DoubleToHex(double value);

   ulen getLen() const;

   void print(PrinterType auto &out) const
    {
     switch( kind )
       {
        case DoubleIsNan :
         {
          Putobj(out,"NaN"_c);
         }
        break;

        case DoubleIsInf :
         {
          Putobj(out,"Inf"_c);
         }
        break;

        case DoubleIsPos :
         {
          printExp(out);
         }
        break;

        case DoubleIsNull :
         {
          out.put('0');
         }
        break;

        case DoubleIsNeg :
         {
          out.put('-');
          printExp(out);
         }
        break;
       }
    }
 };

/* class DoubleDecPrint */

class DoubleDecPrint : NoCopy
 {
   DoubleToDec obj;

  public:

   DoubleDecPrint(double value,unsigned digit_len) : obj(value,digit_len) {}

   void print(PrinterType auto &out,unsigned exp_len,IntShowSign show_sign,ulen width,DoubleAlign align) const
    {
     if( obj.getDecDigit()>=exp_len )
       printExp(out,show_sign,width,align);
     else
       printDec(out,show_sign,width,align);
    }

   void printDec(PrinterType auto &out,IntShowSign show_sign,ulen width,DoubleAlign align) const
    {
     ulen len=obj.getDecLen(show_sign);

     switch( align )
       {
        case DoubleAlignLeft :
         {
          obj.printDec(out,show_sign);
          if( len<width ) out.put(' ',width-len);
         }
        break;

        default:
        case DoubleAlignRight :
         {
          if( len<width ) out.put(' ',width-len);
          obj.printDec(out,show_sign);
         }
        break;
       }
    }

   void printExp(PrinterType auto &out,IntShowSign show_sign,ulen width,DoubleAlign align) const
    {
     ulen len=obj.getExpLen(show_sign);

     switch( align )
       {
        case DoubleAlignLeft :
         {
          obj.printExp(out,show_sign);
          if( len<width ) out.put(' ',width-len);
         }
        break;

        default:
        case DoubleAlignRight :
         {
          if( len<width ) out.put(' ',width-len);
          obj.printExp(out,show_sign);
         }
        break;
       }
    }
 };

/* class DoubleHexPrint */

class DoubleHexPrint : NoCopy
 {
   DoubleToHex obj;

  public:

   explicit DoubleHexPrint(double value) : obj(value) {}

   void print(PrinterType auto &out,ulen width,DoubleAlign align) const
    {
     ulen len=obj.getLen();

     switch( align )
       {
        case DoubleAlignLeft :
         {
          obj.print(out);
          if( len<width ) out.put(' ',width-len);
         }
        break;

        default:
        case DoubleAlignRight :
         {
          if( len<width ) out.put(' ',width-len);
          obj.print(out);
         }
        break;
       }
    }
 };

/* class DoublePrint */

class DoublePrint
 {
   double value;

  public:

   explicit DoublePrint(double value_) : value(value_) {}

   using PrintOptType = DoublePrintOpt ;

   void print(PrinterType auto &out,PrintOptType opt) const
    {
     switch( opt.fmt )
       {
        case DoubleFmtF :
         {
          DoubleDecPrint(value,opt.digit_len).print(out,opt.exp_len,opt.show_sign,opt.width,opt.align);
         }
        break;

        case DoubleFmtE :
         {
          DoubleDecPrint(value,opt.digit_len).printExp(out,opt.show_sign,opt.width,opt.align);
         }
        break;

        case DoubleFmtX :
         {
          DoubleHexPrint(value).print(out,opt.width,opt.align);
         }
        break;
       }
    }
 };

/* struct DoublePrintProxy */

struct DoublePrintProxy
 {
  using OptType = DoublePrintOpt ;
  using ProxyType = DoublePrint ;
 };

/* struct PrintProxy */

template <>
struct PrintProxy<float> : DoublePrintProxy {};

template <>
struct PrintProxy<double> : DoublePrintProxy {};

} // namespace CCore

#endif

