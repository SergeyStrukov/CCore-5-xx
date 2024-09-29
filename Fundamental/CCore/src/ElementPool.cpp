/* ElementPool.cpp */
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

#include <CCore/inc/ElementPool.h>

#include <CCore/inc/MemBase.h>
#include <CCore/inc/Exception.h>

namespace CCore {

/* class MemPool */

Place<void> MemPool::allocBlock(ulen alloc_len)
 {
  Place<void> ptr=PlaceAt(MemAlloc(alloc_len));

  list.ins(new(ptr) Node);

  return ptr;
 }

void MemPool::newBlock()
 {
  Place<void> new_block=allocBlock(block_len);

  if( avail ) MemShrink(block,block_len-avail);

  block=new_block;
  cur=new_block+Delta;
  avail=block_len-Delta;
 }

void MemPool::moveFrom(MemPool &obj)
 {
  block_len=obj.block_len;
  initial_mem_cap=obj.initial_mem_cap;

  list=Replace_null(obj.list);

  block=Replace_null(obj.block);
  cur=Replace_null(obj.cur);
  avail=Replace_null(obj.avail);

  mem_cap=Replace_null(obj.mem_cap);
 }

void MemPool::delList()
 {
  while( Node *node=list.del() ) MemFree(node);
 }

MemPool::MemPool(ulen block_len_,ulen mem_cap_)
 : initial_mem_cap(mem_cap_),
   avail(0),
   mem_cap(mem_cap_)
 {
  block_len=AlignDown(block_len_);

  if( block_len<=Delta )
    {
     Printf(Exception,"CCore::MemPool::MemPool(block_len=#;,...) : too short",block_len_);
    }
 }

MemPool::~MemPool()
 {
  delList();
 }

 // std move

MemPool::MemPool(MemPool &&obj) noexcept
 : block(0),
   cur(0)
 {
  moveFrom(obj);
 }

MemPool & MemPool::operator = (MemPool &&obj) noexcept
 {
  if( this!=&obj )
    {
     delList();

     moveFrom(obj);
    }

  return *this;
 }

 // methods

Place<void> MemPool::alloc(ulen len)
 {
  if( len>MaxLen ) GuardNoMem(len);

  len=Align(len);

  if( !len ) len=MaxAlign;

  if( len>mem_cap ) GuardNoMem(len);

  if( avail<len )
    {
     if( avail>block_len/2 || len+Delta>block_len )
       {
        auto ret=allocBlock(len+Delta)+Delta;

        mem_cap-=len;

        return ret;
       }

     newBlock();
    }

  avail-=len;

  mem_cap-=len;

  return cur+=len;
 }

void MemPool::shrink_extra()
 {
  if( avail )
    {
     MemShrink(block,block_len-avail);

     avail=0;
    }
 }

void MemPool::erase()
 {
  delList();

  block=Null;
  cur=Null;
  avail=0;

  mem_cap=initial_mem_cap;
 }

 // swap/move objects

void MemPool::objSwap(MemPool &obj) noexcept
 {
  Swap(block_len,obj.block_len);
  Swap(initial_mem_cap,obj.initial_mem_cap);

  Swap(list,obj.list);

  Swap(block,obj.block);
  Swap(cur,obj.cur);
  Swap(avail,obj.avail);

  Swap(mem_cap,obj.mem_cap);
 }

MemPool::MemPool(ToMoveCtor<MemPool> obj) noexcept
 : block_len(obj->block_len),
   initial_mem_cap(obj->initial_mem_cap),

   list(Replace_null(obj->list)),

   block(obj->block),
   cur(obj->cur),
   avail(obj->avail),

   mem_cap(obj->mem_cap)
 {
 }

/* class ElementPool */

StrLen ElementPool::dup(StrLen str)
 {
  char *ptr=pool.alloc(str.len);

  str.copyTo(ptr);

  return StrLen(ptr,str.len);
 }

} // namespace CCore


