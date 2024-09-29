/* Cmp.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_Cmp_h
#define CCore_inc_Cmp_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* enum CmpResult */

enum CmpResult
 {
  CmpLess    = -1,
  CmpEqual   =  0,
  CmpGreater =  1
 };

inline CmpResult operator - (CmpResult cmp) { return CmpResult(-int(cmp)); }

const char * GetTextDesc(CmpResult cmp);

#if 0
inline CmpResult CmpResultOf(std::strong_ordering cmp) { return CmpResult(int(cmp)); } // HACK required, see Hack.txt
#else
inline CmpResult CmpResultOf(std::strong_ordering cmp)
 {
  if( cmp>0 ) return CmpGreater;
  if( cmp<0 ) return CmpLess;
  return CmpEqual;
 }
#endif

inline CmpResult CmpResultOf(CmpResult cmp) { return cmp; }

inline bool operator < (CmpResult cmp,int zero) { return (int)cmp < zero ; }

inline bool operator <= (CmpResult cmp,int zero) { return (int)cmp <= zero ; }

inline bool operator > (CmpResult cmp,int zero) { return (int)cmp > zero ; }

inline bool operator >= (CmpResult cmp,int zero) { return (int)cmp >= zero ; }

/* Str...() */

inline CmpResult CharCmp(char a,char b) { return CmpResultOf( (unsigned char)a <=> (unsigned char)b ); }

inline bool CharLess(char a,char b) { return (unsigned char)a < (unsigned char)b ; }

CmpResult StrCmp(StrLen a,StrLen b);

bool StrLess(StrLen a,StrLen b);

template <class T>
CmpResult StrCmpOf(const T &a,const T &b) requires( TypeRangeableType<const T,const char> ) { return StrCmp(Range(a),Range(b)); }

template <class T>
bool StrLessOf(const T &a,const T &b) requires( TypeRangeableType<const T,const char> ) { return StrLess(Range(a),Range(b)); }

/* classes */

class CmpAsStr;

/* class CmpAsStr */

class CmpAsStr
 {
   StrLen str;

  public:

   explicit CmpAsStr(StrLen str_) : str(str_) {}

   template <class T>
   explicit CmpAsStr(const T &obj) requires( TypeRangeableType<const T,const char> ) : str(Range(obj)) {}

   // cmp objects

   CmpResult operator <=> (const CmpAsStr &obj) const noexcept { return StrCmp(str,obj.str); }

   bool operator == (const CmpAsStr &obj) const noexcept { return str.equal(obj.str); }
 };

/* concept CmpableType<T> */

template <class T> // ref extended
concept CmpableType = requires(Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  CmpResultOf( a <=> b );
 } ;

/* Cmp() */

template <CmpableType T>
CmpResult Cmp(const T &a,const T &b) { return CmpResultOf( a <=> b ); }

/* AlphaCmp() */

template <class ... TT>
CmpResult AlphaCmpAny(const TT & ... tt);

inline CmpResult AlphaCmp(CmpResult cmp)
 {
  return cmp;
 }

inline CmpResult AlphaCmp(CmpResult cmp1,CmpResult cmp2)
 {
  if( cmp1 ) return cmp1;

  return cmp2;
 }

template <class ... TT>
CmpResult AlphaCmp(CmpResult cmp,const TT & ... tt)
 {
  if( cmp ) return cmp;

  return AlphaCmpAny(tt...);
 }

template <class ... TT>
CmpResult AlphaCmp(CmpResult cmp1,CmpResult cmp2,const TT & ... tt)
 {
  if( cmp1 ) return cmp1;

  if( cmp2 ) return cmp2;

  return AlphaCmpAny(tt...);
 }

template <CmpableType T>
CmpResult AlphaCmp(const T &a,const T &b)
 {
  return Cmp(a,b);
 }

template <CmpableType T,class ... TT>
CmpResult AlphaCmp(const T &a,const T &b,const TT & ... tt)
 {
  if( CmpResult ret=Cmp(a,b) ) return ret;

  return AlphaCmp(tt...);
 }

template <class ... TT>
CmpResult AlphaCmpAny(const TT & ... tt)
 {
  return AlphaCmp(tt...);
 }

/* Range...() */

template <CmpableType T>
CmpResult RangeCmp(const T *a,const T *b,ulen count)
 {
  for(; count ;count--)
    if( CmpResult ret=Cmp(*(a++),*(b++)) )
      return ret;

  return CmpEqual;
 }

template <CmpableType T>
CmpResult RangeCmp(PtrLen<T> a,PtrLen<T> b)
 {
  if( a.len<b.len )
    {
     if( CmpResult ret=RangeCmp(a.ptr,b.ptr,a.len) ) return ret;

     return CmpLess;
    }
  else if( a.len>b.len )
    {
     if( CmpResult ret=RangeCmp(a.ptr,b.ptr,b.len) ) return ret;

     return CmpGreater;
    }
  else
    {
     return RangeCmp(a.ptr,b.ptr,b.len);
    }
 }

template <CmpableType T>
bool RangeLess(PtrLen<T> a,PtrLen<T> b)
 {
  if( a.len<b.len )
    {
     return RangeCmp(a.ptr,b.ptr,a.len) <= 0 ;
    }
  else
    {
     return RangeCmp(a.ptr,b.ptr,b.len) < 0 ;
    }
 }

template <class T>
CmpResult RangeCmpOf(const T &a,const T &b) requires ( RangeableType<const T> ) { return RangeCmp(Range(a),Range(b)); }

template <class T>
bool RangeLessOf(const T &a,const T &b) requires ( RangeableType<const T> ) { return RangeLess(Range(a),Range(b)); }

/* concept ToCmpableFuncType<Func,T> */

template <class Func,class T> // T ref extended
concept ToCmpableFuncType = requires(Func func,Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  Ground<CmpResult>( func(a) <=> func(b) );
 } ;

/* Range...By() */

template <class T,ToCmpableFuncType<T> Func>
CmpResult RangeCmpBy(const T *a,const T *b,ulen count,Func by)
 {
  for(; count ;count--)
    if( CmpResult ret=Cmp(by(*(a++)),by(*(b++))) )
      return ret;

  return CmpEqual;
 }

template <class T,ToCmpableFuncType<T> Func>
CmpResult RangeCmpBy(PtrLen<T> a,PtrLen<T> b,Func by)
 {
  if( a.len<b.len )
    {
     if( CmpResult ret=RangeCmpBy(a.ptr,b.ptr,a.len,by) ) return ret;

     return CmpLess;
    }
  else if( a.len>b.len )
    {
     if( CmpResult ret=RangeCmpBy(a.ptr,b.ptr,b.len,by) ) return ret;

     return CmpGreater;
    }
  else
    {
     return RangeCmpBy(a.ptr,b.ptr,b.len,by);
    }
 }

template <class T,ToCmpableFuncType<T> Func>
bool RangeLessBy(PtrLen<T> a,PtrLen<T> b,Func by)
 {
  if( a.len<b.len )
    {
     return RangeCmpBy(a.ptr,b.ptr,a.len,by) <= 0 ;
    }
  else
    {
     return RangeCmpBy(a.ptr,b.ptr,b.len,by) < 0 ;
    }
 }

template <class T,class Func> requires ( RangeableType<const T> )
CmpResult RangeCmpOfBy(const T &a,const T &b,Func by) { return RangeCmpBy(Range(a),Range(b),by); }

template <class T,class Func> requires ( RangeableType<const T> )
bool RangeLessOfBy(const T &a,const T &b,Func by) { return RangeLessBy(Range(a),Range(b),by); }

} // namespace CCore

#endif


