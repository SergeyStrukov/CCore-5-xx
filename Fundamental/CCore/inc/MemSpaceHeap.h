/* MemSpaceHeap.h */
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

#ifndef CCore_inc_MemSpaceHeap_h
#define CCore_inc_MemSpaceHeap_h

#include <CCore/inc/RadixHeap.h>

namespace CCore {

/* classes */

class MemSpace;

class SpaceHeap;

/* class MemSpace */

class MemSpace : NoCopy
 {
   ulen max_store_len;

   RadixHeapBlock *cache;

  public:

   explicit MemSpace(Space space) noexcept; // aligned

   void cleanup(bool clean) noexcept;

   ulen getMaxStoreLen() const noexcept { return max_store_len; }

   RadixHeapBlock * alloc(ulen blen) noexcept;

   void free(RadixHeapBlock *block) noexcept;

   bool extend(RadixHeapBlock *block,ulen blen) noexcept;

   ulen shrink(RadixHeapBlock *block,ulen blen) noexcept;
 };

/* class SpaceHeap */

class SpaceHeap : NoCopy
 {
   class AllocSpace : public NoCopyBase<Space>
    {
     public:

      explicit AllocSpace(ulen mem_len);

      ~AllocSpace();
    };

   AllocSpace space;
   RadixHeap<MemSpace> heap;

  public:

   explicit SpaceHeap(ulen mem_len);

   ~SpaceHeap();

   Space getSpace() const { return space; }

   bool isEmpty() const { return heap.isEmpty(); }

   void * alloc(ulen len) noexcept;

   ulen getLen(const void *mem) noexcept;    // mem!=0

   void free(void *mem) noexcept;            // mem!=0

   bool extend(void *mem,ulen len) noexcept; // mem!=0

   bool shrink(void *mem,ulen len) noexcept; // mem!=0
 };

} // namespace CCore

#endif


