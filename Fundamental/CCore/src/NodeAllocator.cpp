/* NodeAllocator.cpp */
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

#include <CCore/inc/NodeAllocator.h>

#include <CCore/inc/Abort.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/algon/CommonIntAlgo.h>

namespace CCore {

/* functions */

void NodePoolAbort()
 {
  Abort("Fatal error : node pool object leak");
 }

/* class MemBlockPool */

void * MemBlockPool::alloc_new()
 {
  if( !count )
    {
     Place<void> place(MemAlloc(alloc_len));

     mem_list=new(place+=Delta) FreeNode(mem_list);

     cur=place;
     count=alloc_count;
    }

  void *mem=(cur+=len);

  count--;

  return mem;
 }

void MemBlockPool::Destroy(FreeNode *mem_list)
 {
  for(FreeNode *node=mem_list; node ;)
    {
     FreeNode *next=node->next;

     MemFree(node);

     node=next;
    }
 }

MemBlockPool::MemBlockPool(ulen len_,ulen align_of,ulen alloc_count_)
 : free_list(0),
   mem_list(0),
   count(0)
 {
  align_of=Algon::LCM<ulen>(align_of,alignof (FreeNode));

  len=Max<ulen>(len_,sizeof (FreeNode));

  if( !TryAlign(len,align_of) )
    {
     Printf(Exception,"CCore::MemBlockPool::MemBlockPool(len=#;,...) : too large len",len_);
    }

  alloc_count=Min<ulen>(Max<ulen>(alloc_count_,1),(MaxULen-Delta)/len);

  if( !alloc_count )
    {
     Printf(Exception,"CCore::MemBlockPool::MemBlockPool(len=#;,alloc_count=#;) : bad arguments",len_,alloc_count_);
    }

  alloc_len=Delta+len*alloc_count;
 }

 // std move

MemBlockPool::MemBlockPool(MemBlockPool &&obj) noexcept
  : len(obj.len),
    alloc_count(obj.alloc_count),
    alloc_len(obj.alloc_len),

    free_list(Replace_null(obj.free_list)),
    mem_list(Replace_null(obj.mem_list)),

    cur(Replace_null(obj.cur)),
    count(Replace_null(obj.count))
 {
 }

MemBlockPool & MemBlockPool::operator = (MemBlockPool &&obj) noexcept
 {
  if( this!=&obj)
    {
     len=obj.len;
     alloc_count=obj.alloc_count;
     alloc_len=obj.alloc_len;

     free_list=Replace_null(obj.free_list);
     Destroy(Replace(mem_list,Replace_null(obj.mem_list)));

     cur=Replace_null(obj.cur);
     count=Replace_null(obj.count);
    }

  return *this;
 }

 // swap/move objects

void MemBlockPool::objSwap(MemBlockPool &obj) noexcept
 {
  Swap(len,obj.len);
  Swap(alloc_count,obj.alloc_count);
  Swap(alloc_len,obj.alloc_len);

  Swap(free_list,obj.free_list);
  Swap(mem_list,obj.mem_list);

  Swap(cur,obj.cur);
  Swap(count,obj.count);
 }

MemBlockPool::MemBlockPool(ToMoveCtor<MemBlockPool> obj) noexcept
 : len(obj->len),
   alloc_count(obj->alloc_count),
   alloc_len(obj->alloc_len),

   free_list(obj->free_list),
   mem_list(Replace_null(obj->mem_list)),

   cur(obj->cur),
   count(obj->count)
 {
 }

} // namespace CCore


