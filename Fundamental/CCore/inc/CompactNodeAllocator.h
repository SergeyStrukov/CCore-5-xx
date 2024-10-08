/* CompactNodeAllocator.h */
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

#ifndef CCore_inc_CompactNodeAllocator_h
#define CCore_inc_CompactNodeAllocator_h

#include <CCore/inc/NodeAllocator.h>
#include <CCore/inc/Array.h>

namespace CCore {

/* classes */

template <NothrowDtorType Node> class CompactNodeAllocator;

/* class CompactNodeAllocator<Node> */

template <NothrowDtorType Node>
class CompactNodeAllocator : NoCopy
 {
   using ListAlgo = DLink<CollectorHeader>::LinearAlgo<&CollectorHeader::link> ;

   struct Flags // not using both, warning supression
    {
     enum NoThrowFlagType
      {
       Default_no_throw = true,
       Copy_no_throw = true
      };
    };

   using Base = ArrayBase<Node,CollectorHeader,ArrayAlgo_mini<Node,Flags> > ;

  private:

   ulen block_len;

   ListAlgo::FirstLast list; // list.last is current
   ulen count;

  private:

   CollectorHeader * provide() // provides not default ptr with room for 1 elements
    {
     CollectorHeader *ptr=list.last;

     if( ptr && ptr->len<ptr->maxlen ) return ptr;

     CollectorHeader *new_ptr=Base::Create(block_len);

     list.ins_last(new_ptr);

     return new_ptr;
    }

   void pop() // list.last!=0 and empty
    {
     Base::Destroy(list.del_last());
    }

  public:

   // constructors

   static const ulen DefaultCount = 100 ;

   explicit CompactNodeAllocator(ulen block_len_=DefaultCount) noexcept
    : block_len(block_len_),
      count(0)
    {
    }

   ~CompactNodeAllocator()
    {
     if( count ) NodePoolAbort();
    }

   // std move

   CompactNodeAllocator(CompactNodeAllocator<Node> &&obj) noexcept
    : block_len(obj.block_len),
      list(Replace_null(obj.list)),
      count(Replace_null(obj.count))
    {
    }

   CompactNodeAllocator<Node> & operator = (CompactNodeAllocator<Node> &&obj) noexcept
    {
     if( this!=&obj )
       {
        if( count ) NodePoolAbort();

        block_len=obj.block_len;
        list=Replace_null(obj.list);
        count=Replace_null(obj.count);
       }

     return *this;
    }

   // props

   ulen operator + () const { return count; }

   bool operator ! () const { return !count; }

   ulen getCount() const { return count; }

   // methods

   template <class ... SS> requires ( ConstructibleType<Node,SS...> )
   Node * alloc(SS && ... ss)
    {
     Node *ret=Base::Append_fill(provide(), std::forward<SS>(ss)... );

     count++;

     return ret;
    }

   ulen erase()
    {
     while( CollectorHeader *ptr=list.del_first() ) Base::Destroy(ptr);

     return Replace_null(count);
    }

   Node * todel()
    {
     while( list.last )
       {
        if( ulen len=list.last->len )
          {
           return Base::GetPtr(list.last)+(len-1);
          }

        pop();
       }

     return 0;
    }

   void del()
    {
     while( list.last )
       {
        if( Base::Shrink_one(list.last) )
          {
           count--;

           return;
          }

        pop();
       }
    }

   // swap/move objects

   void objSwap(CompactNodeAllocator<Node> &obj) noexcept
    {
     Swap(block_len,obj.block_len);
     Swap(list,obj.list);
     Swap(count,obj.count);
    }

   explicit CompactNodeAllocator(ToMoveCtor<CompactNodeAllocator<Node> > obj) noexcept
    : block_len(obj->block_len),
      list(Replace_null(obj->list)),
      count(Replace_null(obj->count))
    {
    }
 };

} // namespace CCore

#endif


