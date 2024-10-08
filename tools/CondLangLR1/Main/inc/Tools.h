/* Tools.h */
//----------------------------------------------------------------------------------------
//
//  Project: CondLangLR1 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CondLangLR1_Tools_h
#define CondLangLR1_Tools_h

#include <CCore/inc/Print.h>
#include <CCore/inc/TextTools.h>
#include <CCore/inc/Array.h>
#include <CCore/inc/Cmp.h>

namespace App {

/* using */

using namespace CCore;

/* words */

enum ElementNullType
 {
  ElementNull = 0
 };

enum ElementOneType
 {
  ElementOne = 1
 };

/* TrackStage() */

template <class ... TT>
void TrackStage(const char *format,const TT & ... tt)
 {
  Printf(Con,format,tt...);

  Putch(Con,'\n','\n');
 }

/* SetCmp() */

template <class T>
bool SetCmp(T &x,const T &a)
 {
  if( x!=a )
    {
     x=a;

     return true;
    }

  return false;
 }

/* classes */

struct EmptyContext;

struct BlockPrintOpt;

template <class R> class PrintRange;

struct PosStr;

struct DescBase;

class TrackStep;

/* struct EmptyContext */

struct EmptyContext
 {
  explicit EmptyContext() {}
 };

/* struct BlockPrintOpt */

struct BlockPrintOpt
 {
  bool flag;

  void setDefault() { flag=false; }

  BlockPrintOpt() { setDefault(); }

  BlockPrintOpt(const char *ptr,const char *lim);

  //
  // [.b]
  //
 };

/* class PrintRange<R> */

template <class R>
class PrintRange
 {
   R range;

  public:

   explicit PrintRange(const R &range_) : range(range_) {}

   void print(PrinterType auto &out) const
    {
     R p=range;

     if( +p )
       {
        Putobj(out,*p);

        for(++p; +p ;++p)
          {
           Putch(out,' ');

           Putobj(out,*p);
          }
       }
    }
 };

/* struct PosStr */

struct PosStr
 {
  TextPos pos;
  StrLen str;

  PosStr() {}

  explicit PosStr(TextPos pos_) : pos(pos_) {}

  explicit PosStr(StrLen str_) : str(str_) { pos.line=0; pos.col=0; }

  PosStr(TextPos pos_,StrLen str_) : pos(pos_),str(str_) {}
 };

/* struct DescBase */

struct DescBase : NoCopy
 {
  ulen index = MaxULen ;
  StrLen name;

  DescBase() noexcept {}

  bool operator + () const { return index!=MaxULen; }

  bool operator ! () const { return index==MaxULen; }
 };

/* class TrackStep */

class TrackStep : NoCopy
 {
   ulen count = 0 ;

  public:

   TrackStep() {}

   void step()
    {
     ++count;

     if( count%100u==0 )
       {
        Printf(Con,"  step #;\r",count);
       }
    }

   void finish()
    {
     Printf(Con,"  step #;\n\n",count);
    }
 };

} // namespace App

#endif
