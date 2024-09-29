/* AnyPtr.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_AnyPtr_h
#define CCore_inc_AnyPtr_h

#include <CCore/inc/FunctorType.h>
#include <CCore/inc/Printf.h>

namespace CCore {

/* concept BinaryRetType<Ret> */

template <class Ret>
concept BinaryRetType = requires(unsigned a,unsigned b)
 {
  Ret();

  Ret(a,b);
 } ;

/* ApplyToPtr() */

template <class T,class FuncInit,class ... SS> requires ( FuncInitArgType<FuncInit,T *,SS...> )
void ApplyToPtr(void *ptr,FuncInit func_init,SS && ... ss)
 {
  FunctorTypeOf<FuncInit> func(func_init);

  func(static_cast<T *>(ptr), std::forward<SS>(ss)... );
 }

/* ApplyToPtr_const() */

template <class T,class FuncInit,class ... SS> requires ( FuncInitArgType<FuncInit,const T *,SS...> )
void ApplyToPtr_const(const void *ptr,FuncInit func_init,SS && ... ss)
 {
  FunctorTypeOf<FuncInit> func(func_init);

  func(static_cast<const T *>(ptr), std::forward<SS>(ss)... );
 }

/* ApplyToPtr2() */

template <class Ret,class T,class FuncInit> requires ( FuncInitType<FuncInit,Ret,T *,T *> )
Ret ApplyToPtr2(void *ptr1,void *ptr2,FuncInit func_init)
 {
  FunctorTypeOf<FuncInit> func(func_init);

  return func(static_cast<T *>(ptr1),static_cast<T *>(ptr2));
 }

/* ApplyToPtr2_const() */

template <class Ret,class T,class FuncInit> requires ( FuncInitType<FuncInit,Ret,const T *,const T *> )
Ret ApplyToPtr2_const(const void *ptr1,const void *ptr2,FuncInit func_init)
 {
  FunctorTypeOf<FuncInit> func(func_init);

  return func(static_cast<const T *>(ptr1),static_cast<const T *>(ptr2));
 }

/* classes */

template <PrinterType P> class PrintAnyObj;

template <class ... TT> class AnyPtr;

template <class ... TT> class AnyPtr_const;

template <class T,class S> struct ElaborateAnyPtrBind;

/* class PrintAnyObj<P> */

template <PrinterType P>
class PrintAnyObj
 {
   P &out;

  public:

   explicit PrintAnyObj(P &out_) : out(out_) {}

   void operator () (AnyType auto *obj)
    {
     Putobj(out,*obj);
    }
 };

/* class AnyPtr<TT> */

template <class ... TT>
class AnyPtr
 {
   void *ptr;
   unsigned type;

  public:

   // constructors

   AnyPtr() noexcept : ptr(0),type(0) {}

   AnyPtr(NothingType) : AnyPtr() {}

   AnyPtr(std::nullptr_t) : AnyPtr() {}

   template <class T> requires OneOfTypes<T,TT...>
   AnyPtr(T *ptr_) : ptr(ptr_),type(Meta::IndexOf<T,TT...>) {}

   // methods

   void * operator + () const { return ptr; }

   bool operator ! () const { return !ptr; }

   template <class T> requires OneOfTypes<T,TT...>
   bool hasType() const { return type==Meta::IndexOf<T,TT...>; }

   template <class T> requires OneOfTypes<T,TT...>
   T * castPtr() const { if( hasType<T>() ) return static_cast<T *>(ptr); return 0; }

   template <class FuncInit,class ... SS> requires ( ... && FuncInitArgType<FuncInit,TT *,SS...> )
   void apply(FuncInit func_init,SS && ... ss) const
    {
     using FuncType = void (*)(void *ptr,FuncInit func_init,SS && ...) ;

     static const FuncType Table[]={ ApplyToPtr<TT,FuncInit,SS...>... };

     if( type ) Table[type-1](ptr,func_init, std::forward<SS>(ss)... );
    }

   template <class T,class FuncInit> requires ( FuncInitArgType<FuncInit,T *> )
   void applyFor(FuncInit func_init) const
    {
     if( hasType<T>() ) ApplyToPtr<T>(ptr,func_init);
    }

   // Binary

   template <BinaryRetType Ret,class FuncInit> requires ( ... && FuncInitType<FuncInit,Ret,TT *,TT *> )
   static Ret Binary(AnyPtr<TT...> a,AnyPtr<TT...> b,FuncInit func_init)
    {
     if( a.type!=b.type ) return Ret(a.type,b.type);

     using FuncType = Ret (*)(void *,void *,FuncInit func_init) ;

     static const FuncType Table[]={ ApplyToPtr2<Ret,TT,FuncInit>... };

     if( a.type ) return Table[a.type-1](a.ptr,b.ptr,func_init);

     return Ret();
    }

   // print object

   template <PrinterType P>
   void print(P &out) const
    {
     if( ptr )
       apply(PrintAnyObj<P>(out));
     else
       Putobj(out,"(null)"_c);
    }
 };

/* class AnyPtr_const<TT> */

template <class ... TT>
class AnyPtr_const
 {
   const void *ptr;
   unsigned type;

  public:

   // constructors

   AnyPtr_const() noexcept : ptr(0),type(0) {}

   AnyPtr_const(NothingType) : AnyPtr_const() {}

   AnyPtr_const(std::nullptr_t) : AnyPtr_const() {}

   template <class T> requires OneOfTypes<T,TT...>
   AnyPtr_const(const T *ptr_) : ptr(ptr_),type(Meta::IndexOf<T,TT...>) {}

   // methods

   const void * operator + () const { return ptr; }

   bool operator ! () const { return !ptr; }

   template <class T> requires OneOfTypes<T,TT...>
   bool hasType() const { return type==Meta::IndexOf<T,TT...>; }

   template <class T> requires OneOfTypes<T,TT...>
   const T * castPtr() const { if( hasType<T>() ) return static_cast<const T *>(ptr); return 0; }

   template <class FuncInit,class ... SS> requires ( ... && FuncInitArgType<FuncInit,const TT *,SS...> )
   void apply(FuncInit func_init,SS && ... ss) const
    {
     using FuncType = void (*)(const void *ptr,FuncInit func_init,SS && ...) ;

     static const FuncType Table[]={ ApplyToPtr_const<TT,FuncInit,SS...>... };

     if( type ) Table[type-1](ptr,func_init, std::forward<SS>(ss)... );
    }

   template <class T,class FuncInit> requires ( FuncInitArgType<FuncInit,const T *> )
   void applyFor(FuncInit func_init) const
    {
     if( hasType<T>() ) ApplyToPtr_const<T>(ptr,func_init);
    }

   // Binary

   template <BinaryRetType Ret,class FuncInit> requires ( ... && FuncInitType<FuncInit,Ret,const TT *,const TT *> )
   static Ret Binary(AnyPtr_const<TT...> a,AnyPtr_const<TT...> b,FuncInit func_init)
    {
     if( a.type!=b.type ) return Ret(a.type,b.type);

     using FuncType = Ret (*)(const void *,const void *,FuncInit func_init) ;

     static const FuncType Table[]={ ApplyToPtr2_const<Ret,TT,FuncInit>... };

     if( a.type ) return Table[a.type-1](a.ptr,b.ptr,func_init);

     return Ret();
    }

   // print object

   template <PrinterType P>
   void print(P &out) const
    {
     if( ptr )
       apply(PrintAnyObj<P>(out));
     else
       Putobj(out,"(null)"_c);
    }
 };

/* namespace Meta */

namespace Meta {

struct DetectAnyPtrType
 {
  template <class T>
  static DefConst<bool,false> Cast(T &obj,int);

  template <class ... TT>
  static DefConst<bool,true> Cast(const AnyPtr<TT...> &obj,NothingType);

  template <class ... TT>
  static DefConst<bool,true> Cast(const AnyPtr_const<TT...> &obj,NothingType);

  template <class T>
  static constexpr bool Ret = decltype( Cast(TypeBox<T &>::Get(),Nothing) )::Ret ;
 };

} // namespace Meta

/* concept ElaborateAnyPtrType<T> */

template <class T>
concept ElaborateAnyPtrType = Meta::DetectAnyPtrType::Ret<T> ;

/* struct ElaborateAnyPtrBind<T,S> */

template <class T,class S>
struct ElaborateAnyPtrBind
 {
  T &obj;
  S &&arg;

  ElaborateAnyPtrBind(T &obj_,S &&arg_) : obj(obj_),arg( std::forward<S>(arg_) ) {}

  template <class ... SS>
  void operator () (SS && ... ss)
   {
    obj( std::forward<S>(arg) , std::forward<SS>(ss)... );
   }
 };

template <class T,ElaborateAnyPtrType S>
struct ElaborateAnyPtrBind<T,S>
 {
  T &obj;
  S &&arg;

  ElaborateAnyPtrBind(T &obj_,S &&arg_) : obj(obj_),arg( std::forward<S>(arg_) ) {}

  template <class ... SS>
  void operator () (SS && ... ss)
   {
    arg.apply( FunctorRef(obj) , std::forward<SS>(ss)... );
   }
 };

/* ElaborateAnyPtr() */

template <class T>
void ElaborateAnyPtr(T &obj)
 {
  obj();
 }

template <class T,class S,class ... SS>
void ElaborateAnyPtr(T &obj,S && s,SS && ... ss)
 {
  ElaborateAnyPtrBind<T,S> bind(obj, std::forward<S>(s) );

  ElaborateAnyPtr(bind, std::forward<SS>(ss)... );
 }

/* CastAnyPtr() */

template <class T,class S>
T CastAnyPtr(S obj)
 {
  T ret;

  obj.apply( [&ret] (auto *ptr) { ret=ptr; } );

  return ret;
 }

} // namespace CCore

#endif
