/* Classification.h */
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

#ifndef CCore_inc_gadget_Classification_h
#define CCore_inc_gadget_Classification_h

#include <CCore/inc/gadget/Meta.h>

namespace CCore {

/* Ground() */

template <class T>
void Ground(T /* arg */) {}

/* simple concepts */

template <class T>
concept AnyType = true ;

template <class T>
concept NoType = false ;


template <class T>
concept UIntType = Meta::IsUInt<T> ;

template <class T>
concept SIntType = Meta::IsSInt<T> ;

template <class T>
concept S2sIntType = SIntType<T> && Is2sComplementArithmetic ;

template <class T>
concept SUIntType = SIntType<T> || UIntType<T> ;


template <class T>
concept VoidType = Meta::OneOf<T,void,const void> ;

template <class T>
concept ConstCharType = Meta::OneOf<T,const char,const unsigned char> ;

template <class T>
concept CharType = ConstCharType<T> || Meta::OneOf<T,char,unsigned char> ;

template <class T>
concept ULenType = Meta::IsSame<T,ulen> ;

template <class T,class S>
concept IsType = Meta::IsSame<T,S> ;

template <class T,class ... TT>
concept OneOfTypes = Meta::OneOf<T,TT...> ;

template <class T,class S>
concept IsDerivedFrom = Meta::IsBaseOf<S,T> ;


template <class T>
concept EnumType = Meta::IsEnum<T> ;

template <class T>
concept ClassType = Meta::IsClass<T> ;

template <class T>
concept PODType = Meta::IsPOD<T> ;


template <class T>
concept NothrowDtorType = Meta::HasNothrowDtor<T> ;

template <class T>
concept TrivDtorType = NothrowDtorType<T> && Meta::HasTrivDtor<T> ;

template <class T>
concept DefaultCtorType = Meta::HasDefaultCtor<T> ;

template <class T>
concept NothrowDefaultCtorType = DefaultCtorType<T> && Meta::HasNothrowDefaultCtor<T> ;

template <class T>
concept MoveCtorType = Meta::HasMoveCtor<T> ;

template <class T>
concept CopyCtorType = Meta::HasCopyCtor<T> ;

template <class T>
concept NothrowCopyCtorType = CopyCtorType<T> && Meta::HasNothrowCopyCtor<T> ;

template <class T>
concept MovableType = Meta::IsMovable<T> ;

template <class T>
concept CopyableType = Meta::IsCopyable<T> ;

template <class T>
concept NothrowCopyableType = CopyableType<T> && Meta::IsNothrowCopyable<T> ;

//----------------------------------------------------------------------------------------

/* complex concepts */

/* concept ConstructibleType<T,SS> */

template <class T,class ... SS>
concept ConstructibleType = requires(SS && ... ss) { T( std::forward<SS>(ss)... ); } ;

/* concept FuncArgType<Func,AA> */

template <class Func,class ... AA>
concept FuncArgType = requires(Func func,AA && ... aa) { func( std::forward<AA>(aa)... ); } ;

/* concept FuncType<Func,R,AA> */

template <class Func,class R,class ... AA>
concept FuncType = FuncArgType<Func,AA...> && requires(Func func,AA && ... aa)
 {
  Ground<R>( func( std::forward<AA>(aa)... ) );
 } ;

/* concept OpLessType<T> */

template <class T> // ref extended
concept OpLessType = requires(Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  Ground<bool>( a < b );
 } ;

/* concept OpEqualType<T> */

template <class T> // ref extended
concept OpEqualType = requires(Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  Ground<bool>( a == b );
 } ;

/* concept OpNotEqualType<T> */

template <class T> // ref extended
concept OpNotEqualType = requires(Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  Ground<bool>( a != b );
 } ;

/* concept OpCmpType<T> */

template <class T> // ref extended
concept OpCmpType = requires(Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  Ground<bool>( a <  b );
  Ground<bool>( a <= b );
  Ground<bool>( a >  b );
  Ground<bool>( a >= b );
  Ground<bool>( a == b );
  Ground<bool>( a != b );
 } ;

/* concept OpLessTypes<T,S> */

template <class T,class S> // ref extended
concept OpLessTypes = requires(Meta::ToConst<T> &a,Meta::ToConst<S> &b)
 {
  Ground<bool>( a < b );
 } ;

/* concept OpLessEqualTypes<T,S> */

template <class T,class S> // ref extended
concept OpLessEqualTypes = requires(Meta::ToConst<T> &a,Meta::ToConst<S> &b)
 {
  Ground<bool>( a <= b );
 } ;

/* concept OpGreaterTypes<T,S> */

template <class T,class S> // ref extended
concept OpGreaterTypes = requires(Meta::ToConst<T> &a,Meta::ToConst<S> &b)
 {
  Ground<bool>( a > b );
 } ;

/* concept OpGreaterEqualTypes<T,S> */

template <class T,class S> // ref extended
concept OpGreaterEqualTypes = requires(Meta::ToConst<T> &a,Meta::ToConst<S> &b)
 {
  Ground<bool>( a >= b );
 } ;

/* concept NullableType<T> */

template <class T> // ref extended
concept NullableType = NothrowDefaultCtorType<T> && requires(Meta::ToConst<T> &cobj)
 {
  Ground<bool>( +cobj );

  Ground<bool>( !cobj );
 } ;

/* concept IndirectOverType<P,T> */

template <class P,class T> // P ref extended
concept IndirectOverType = requires(Meta::ToConst<P> &cobj)
 {
  Ground<T>( *cobj );
 } ;

/* concept IndirectCastType<P,T> */

template <class P,class T> // P ref extended
concept IndirectCastType = requires(Meta::ToConst<P> &cobj)
 {
  T(*cobj);
 } ;

/* concept RangeAccessType<T> */

template <class T,class T1,class T2,ULenType ULen> requires ( Meta::IsSame<T1,T2> && Meta::OneOf<T1,T,const T> )
void RangeAccessHelper(T *,T1 *,T2 *,ULen) {}

template <class T> // ref extended
concept RangeAccessType = requires(T &obj,Meta::ToConst<T> &cobj)
 {
  RangeAccessHelper(obj.getPtr(),cobj.getPtr_const(),cobj.getPtr(),cobj.getLen());
 } ;

/* concept ForType<R> */

template <class R>
concept ForType = NothrowCopyableType<R> && requires(R &obj,Meta::ToConst<R> &cobj)
 {
  Ground<bool>( +cobj );

  *cobj;

  ++obj;
 } ;

/* concept CursorType<R> */

template <class R>
concept CursorType = NothrowCopyableType<R> && NullableType<R> && requires(R &obj,Meta::ToConst<R> &cobj)
 {
  *cobj;

  cobj.operator -> () ;

  ++obj;
 } ;

/* concept CursorOverType<R,T> */

template <class R,class T>
concept CursorOverType = CursorType<R> && IndirectOverType<R,T> ;

/* concept CursorCastType<R,T> */

template <class R,class T>
concept CursorCastType = CursorType<R> && IndirectCastType<R,T> ;

/* concept RecursorType<R> */

template <class R>
concept RecursorType = NothrowCopyableType<R> && NullableType<R> && requires(Meta::ToConst<R> &cobj)
 {
  *cobj;

  cobj.operator -> () ;

  Ground<R>( cobj.prev() );

  Ground<R>( cobj.next() );
 } ;

/* concept RecursorOverType<R,T> */

template <class R,class T>
concept RecursorOverType = RecursorType<R> && IndirectOverType<R,T> ;

/* concept RecursorOverType<R,T> */

template <class R,class T>
concept RecursorCastType = RecursorType<R> && IndirectCastType<R,T> ;

/* concept RanType<Ran> */

template <class Ran>
concept RanType = NothrowCopyableType<Ran> && NothrowDefaultCtorType<Ran> && OpCmpType<Ran> && requires(Ran ptr,ulen len)
 {
  *ptr;

  ptr++;
  ptr--;

  ++ptr;
  --ptr;

  Ground<Ran>( ptr+len );

  ptr+=len;

  Ground<Ran>( ptr-len );

  ptr-=len;

  Ground<ulen>( ptr-ptr );

  ptr[len];
 } ;

/* concept ToOpNotEqualFuncType<Func,T> */

template <class Func,class T>
concept ToOpNotEqualFuncType = requires(Func func,Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  Ground<bool>( func(a)!=func(b) );
 } ;

/* concept ToOpLessFuncType<Func,T> */

template <class Func,class T>
concept ToOpLessFuncType = requires(Func func,Meta::ToConst<T> &a,Meta::ToConst<T> &b)
 {
  Ground<bool>( func(a)<func(b) );
 } ;

/* concept PtrToOpLessFuncType<Func,Ptr> */

template <class Func,class Ptr>
concept PtrToOpLessFuncType = ToOpLessFuncType<Func,Meta::PtrObjType<Ptr> > ;

/* concept PrinterType<P> */

template <class P>
concept PrinterType = requires(P &out,char ch,ulen len,const char *str)
 {
  typename P::PrintOutType;

  Ground<typename P::PrintOutType>( out.printRef() );

  out.put(ch);

  out.put(ch,len);

  out.put(str,len);

  out.flush();
 } ;

/* concept ScannerType<S> */

template <class S>
concept ScannerType = requires(S &obj,const S &cobj)
 {
  typename S::ScanInpType;

  Ground<typename S::ScanInpType>( obj.scanRef() );

  Ground<bool>( +cobj );

  Ground<bool>( !cobj );

  Ground<char>( *cobj );

  ++obj;

  Ground<bool>( cobj.isOk() );

  Ground<bool>( cobj.isFailed() );

  obj.fail();
 } ;

} // namespace CCore

#endif

