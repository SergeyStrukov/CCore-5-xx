/* UniqueToFront.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_algon_UniqueToFront_h
#define CCore_inc_algon_UniqueToFront_h

#include <CCore/inc/Sort.h>
#include <CCore/inc/FunctorType.h>
#include <CCore/inc/algon/BaseRangeAlgo.h>

namespace CCore {
namespace Algon {

/* classes */

template <RangeType R,class Algo=BaseRangeAlgo<R> > struct UniqueToFrontAlgo;

/* struct UniqueToFrontAlgo<R,Algo> */

template <RangeType R,class Algo>
struct UniqueToFrontAlgo : Algo
 {
  using Algo::GetPtr;
  using Algo::GetPrefix;

  using T = Meta::PtrObjType<R> ;

  template <FuncArgType<T &,T &> Func>
  static R UniqueToFront(R r,Func func) requires ( OpNotEqualType<T> )
   {
    if( +r )
      {
       auto last=GetPtr(r);

       for(++r; +r ;++r)
         {
          if( *last!=*r )
            {
             last=GetPtr(r);
            }
          else
            {
             R out=r;

             for(++r; +r ;++r)
               {
                if( *last!=*r )
                  {
                   func(*out,*r);

                   last=GetPtr(out);

                   ++out;
                  }
               }

             return out;
            }
         }
      }

    return r;
   }

  static R CopyUniqueToFront(R r) requires ( OpNotEqualType<T> )
   {
    return UniqueToFront(r, [] (T &a,const T &b) { a=b; } );
   }

  static R MoveUniqueToFront(R r) requires ( OpNotEqualType<T> )
   {
    return UniqueToFront(r, [] (T &a,T &b) { a=std::move(b); } );
   }

  static R SwapUniqueToFront(R r) requires ( OpNotEqualType<T> )
   {
    return UniqueToFront(r, [] (T &a,T &b) { Swap(a,b); } );
   }

  template <ToOpNotEqualFuncType<T> By,FuncArgType<T &,T &> Func>
  static R UniqueToFrontBy(R r,By by,Func func) requires ( OpNotEqualType<T> )
   {
    if( +r )
      {
       auto last=GetPtr(r);

       for(++r; +r ;++r)
         {
          if( by(*last)!=by(*r) )
            {
             last=GetPtr(r);
            }
          else
            {
             R out=r;

             for(++r; +r ;++r)
               {
                if( by(*last)!=by(*r) )
                  {
                   func(*out,*r);

                   last=GetPtr(out);

                   ++out;
                  }
               }

             return out;
            }
         }
      }

    return r;
   }

  template <ToOpNotEqualFuncType<T> By>
  static R CopyUniqueToFrontBy(R r,By by) requires ( OpNotEqualType<T> )
   {
    return UniqueToFrontBy(r,by, [] (T &a,const T &b) { a=b; } );
   }

  template <ToOpNotEqualFuncType<T> By>
  static R MoveUniqueToFrontBy(R r,By by) requires ( OpNotEqualType<T> )
   {
    return UniqueToFrontBy(r,by, [] (T &a,T &b) { a=std::move(b); } );
   }

  template <ToOpNotEqualFuncType<T> By>
  static R SwapUniqueToFrontBy(R r,By by) requires ( OpNotEqualType<T> )
   {
    return UniqueToFrontBy(r,by, [] (T &a,T &b) { Swap(a,b); } );
   }
 };

/* CopyUniqueToFront() */

template <RangeType R>
R CopyUniqueToFront(R r) // tail is returned
 {
  return UniqueToFrontAlgo<R>::CopyUniqueToFront(r);
 }

/* MoveUniqueToFront() */

template <RangeType R>
R MoveUniqueToFront(R r) // tail is returned
 {
  return UniqueToFrontAlgo<R>::MoveUniqueToFront(r);
 }

/* SwapUniqueToFront() */

template <RangeType R>
R SwapUniqueToFront(R r) // tail is returned
 {
  return UniqueToFrontAlgo<R>::SwapUniqueToFront(r);
 }

/* CopyUniqueToFrontBy() */

template <RangeType R,PtrToOpLessFuncType<R> By>
R CopyUniqueToFrontBy(R r,By by) // tail is returned
 {
  return UniqueToFrontAlgo<R>::CopyUniqueToFrontBy(r,by);
 }

/* MoveUniqueToFrontBy() */

template <RangeType R,PtrToOpLessFuncType<R> By>
R MoveUniqueToFrontBy(R r,By by) // tail is returned
 {
  return UniqueToFrontAlgo<R>::MoveUniqueToFrontBy(r,by);
 }

/* SwapUniqueToFrontBy() */

template <RangeType R,PtrToOpLessFuncType<R> By>
R SwapUniqueToFrontBy(R r,By by) // tail is returned
 {
  return UniqueToFrontAlgo<R>::SwapUniqueToFrontBy(r,by);
 }

} // namespace Algon
} // namespace CCore

#endif

