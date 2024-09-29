/* ElementPool.h */
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

#ifndef CCore_inc_ElementPool_h
#define CCore_inc_ElementPool_h

#include <CCore/inc/List.h>
#include <CCore/inc/OutBuf.h>

#include <CCore/inc/array/ArrayAlgo.h>

namespace CCore {

/* classes */

class MemPool;

class ElementPool;

/* class MemPool */

class MemPool : NoCopy
 {
   struct Node
    {
     SLink<Node> link;
    };

   static constexpr ulen Delta = Align(sizeof (Node)) ;
   static constexpr ulen MaxLen = AlignDown(MaxULen)-Delta ;

   using Algo = SLink<Node>::LinearAlgo<&Node::link> ;

  private:

   ulen block_len; // aligned , > Delta
   ulen initial_mem_cap;

   Algo::Top list;

   Place<void> block;
   Place<void> cur;
   ulen avail;

   ulen mem_cap;

  private:

   Place<void> allocBlock(ulen alloc_len);

   void newBlock();

   void moveFrom(MemPool &obj);

   void delList();

  public:

   // constructors

   static constexpr ulen DefaultBlockLen = 64_KByte ;

   MemPool() noexcept : MemPool(DefaultBlockLen) {}

   explicit MemPool(ulen block_len,ulen mem_cap=MaxULen);

   ~MemPool();

   // std move

   MemPool(MemPool &&obj) noexcept;

   MemPool & operator = (MemPool &&obj) noexcept;

   // methods

   Place<void> alloc(ulen len);

   void shrink_extra();

   void erase();

   // swap/move objects

   void objSwap(MemPool &obj) noexcept;

   explicit MemPool(ToMoveCtor<MemPool> obj) noexcept;
 };

/* class ElementPool */

class ElementPool : NoCopy
 {
   MemPool pool;

  private:

   template <class T>
   Place<void> alloc(ulen len)
    {
     return pool.alloc(LenOf(len,sizeof (T)));
    }

   template <class ... TT>
   StrLen catStrLen(TT ... tt)
    {
     ulen len=LenAdd( tt.len ... );
     char *ptr=pool.alloc(len);

     OutBuf<char> out(ptr);

     out(tt...);

     return StrLen(ptr,len);
    }

  public:

   // constructors

   ElementPool() noexcept {}

   template <class ... TT>
   explicit ElementPool(TT ... tt) : pool(tt...) {}

   ~ElementPool() {}

   // std move

   ElementPool(ElementPool &&obj) noexcept : pool( std::move(obj.pool) ) {}

   ElementPool & operator = (ElementPool &&obj) noexcept
    {
     pool=std::move(obj.pool);

     return *this;
    }

   // methods

   template <TrivDtorType T,class ... SS> requires ( ConstructibleType<T,SS...> )
   T * create(SS && ... ss)
    {
     return new(pool.alloc(sizeof (T))) T( std::forward<SS>(ss)... );
    }

   StrLen dup(StrLen str);

   template <class ... TT>
   StrLen cat(TT ... tt)
    {
     return catStrLen( StrLen(tt)... );
    }

   void shrink_extra() { pool.shrink_extra(); }

   void erase() { pool.erase(); }

   // createArray

   template <TrivDtorType T> requires ( DefaultCtorType<T> )
   PtrLen<T> createArray(ulen len)
    {
     using Algo = ArrayAlgo_mini<T> ;

     return Algo::Create_default(alloc<T>(len),len);
    }

   template <TrivDtorType T,CreatorType<T> Creator>
   PtrLen<T> createArray(ulen len,Creator creator)
    {
     using Algo = ArrayAlgo_mini<T> ;

     return Algo::Create(alloc<T>(len),len,creator);
    }

   template <TrivDtorType T> requires ( DefaultCtorType<T> )
   PtrLen<T> createArray_raw(ulen len)
    {
     using Algo = ArrayAlgo_mini<T> ;

     return Algo::Create_raw(alloc<T>(len),len);
    }

   template <TrivDtorType T,class ... SS> requires ( ConstructibleType<T,SS...> )
   PtrLen<T> createArray_fill(ulen len,SS && ... ss)
    {
     using Algo = ArrayAlgo_mini<T> ;

     return Algo::Create_fill(alloc<T>(len),len, std::forward<SS>(ss)... );
    }

   template <TrivDtorType T> requires ( CopyCtorType<T> )
   PtrLen<T> createArray_copy(ulen len,const T src[])
    {
     using Algo = ArrayAlgo_mini<T> ;

     return Algo::Create_copy(alloc<T>(len),len,src);
    }

   template <TrivDtorType T> requires ( CopyCtorType<T> )
   PtrLen<T> createArray_copy(PtrLen<const T> src)
    {
     return createArray_copy(src.len,src.ptr);
    }

   template <TrivDtorType T,class S> requires ( ConstructibleType<T,const S> )
   PtrLen<T> createArray_cast(ulen len,const S src[])
    {
     using Algo = ArrayAlgo_mini<T> ;

     return Algo::Create_cast(alloc<T>(len),len,src);
    }

   template <TrivDtorType T,class S> requires ( ConstructibleType<T,const S> )
   PtrLen<T> createArray_cast(PtrLen<const S> src)
    {
     return createArray_cast(src.len,src.ptr);
    }

   template <TrivDtorType T> requires ( DefaultCtorType<T> )
   PtrLen<T> createArray_swap(ulen len,T objs[])
    {
     using Algo = ArrayAlgo<T> ;

     return Algo::Create_swap(alloc<T>(len),len,objs);
    }

   template <TrivDtorType T> requires ( DefaultCtorType<T> )
   PtrLen<T> createArray_swap(PtrLen<T> objs)
    {
     return createArray_swap(objs.len,objs.ptr);
    }

   // swap/move objects

   void objSwap(ElementPool &obj) noexcept
    {
     Swap(pool,obj.pool);
    }

   explicit ElementPool(ToMoveCtor<ElementPool> obj) noexcept
    : pool(ToMoveCtor(obj->pool))
    {
    }
 };

} // namespace CCore

#endif


