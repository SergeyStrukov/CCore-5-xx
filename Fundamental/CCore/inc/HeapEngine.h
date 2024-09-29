/* HeapEngine.h */
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

#ifndef CCore_inc_HeapEngine_h
#define CCore_inc_HeapEngine_h

#include <CCore/inc/MemBase.h>
#include <CCore/inc/Task.h>

namespace CCore {

/* concept HeapType<Heap> */

template <class Heap>
concept HeapType = requires(Heap &obj,void *mem,ulen len,const void *cmem)
 {
  { obj.alloc(len) } noexcept;

  { obj.free(mem) } noexcept;

  { obj.extend(mem,len) } noexcept;

  { obj.shrink(mem,len) } noexcept;

  { obj.getLen(cmem) } noexcept;

  Ground<Space>( obj.alloc(len) );

  Ground<ulen>( obj.free(mem) );

  Ground<DeltaLen>( obj.extend(mem,len) );

  Ground<DeltaLen>( obj.shrink(mem,len) );

  Ground<ulen>( obj.getLen(cmem) );
 } ;

/* classes */

template <HeapType Heap> class HeapEngine;

/* class HeapEngine<Heap> */

template <HeapType Heap>
class HeapEngine : NoCopy
 {
   Mutex mutex;

   Heap heap;
   MemStatData stat;
   MemStatData peak;
   ulen limit;

  private:

   bool overLim(ulen len) const
    {
     if( !limit ) return false;

     return stat.len_count>limit || len>(limit-stat.len_count) ;
    }

   void addStat(ulen len)
    {
     stat.add(len);
     peak.setMax(stat);
    }

   void delStat(ulen len)
    {
     stat.del(len);
    }

   void extendStat(ulen delta)
    {
     stat.extend(delta);
     peak.setMax(stat);
    }

   void shrinkStat(ulen delta)
    {
     stat.shrink(delta);
    }

  public:

   template <class ... SS>
   explicit HeapEngine(TextLabel name,SS && ... ss);

   ~HeapEngine();

   void * alloc(ulen len) noexcept;

   ulen getLen(const void *mem) noexcept;    // mem may == 0

   void free(void *mem) noexcept;            // mem may == 0

   bool extend(void *mem,ulen len) noexcept; // mem may == 0

   bool shrink(void *mem,ulen len) noexcept; // mem may == 0

   void setLim(ulen limit) noexcept;

   void getStat(MemStatData &ret) noexcept;

   void getPeak(MemStatData &ret) noexcept;
 };

template <HeapType Heap>
template <class ... SS>
HeapEngine<Heap>::HeapEngine(TextLabel name,SS && ... ss)
 : mutex(name),
   heap( std::forward<SS>(ss)... ),
   limit(0)
 {
 }

template <HeapType Heap>
HeapEngine<Heap>::~HeapEngine()
 {
 }

template <HeapType Heap>
void * HeapEngine<Heap>::alloc(ulen len) noexcept
 {
  Mutex::Lock lock(mutex);

  if( overLim(len) ) return 0;

  Space ret=heap.alloc(len);

  if( ret.len )
    {
     addStat(ret.len);

     return ret.mem;
    }

  return 0;
 }

template <HeapType Heap>
ulen HeapEngine<Heap>::getLen(const void *mem) noexcept
 {
  if( !mem ) return 0;

  Mutex::Lock lock(mutex);

  return heap.getLen(mem);
 }

template <HeapType Heap>
void HeapEngine<Heap>::free(void *mem) noexcept
 {
  if( !mem ) return;

  Mutex::Lock lock(mutex);

  ulen len=heap.free(mem);

  delStat(len);
 }

template <HeapType Heap>
bool HeapEngine<Heap>::extend(void *mem,ulen len) noexcept
 {
  if( !mem ) return false;

  Mutex::Lock lock(mutex);

  if( overLim(len) )
    {
     ulen memlen=heap.getLen(mem);

     if( len>=memlen && overLim(len-memlen) ) return false;
    }

  DeltaLen result=heap.extend(mem,len);

  extendStat(result.delta);

  return result.ok;
 }

template <HeapType Heap>
bool HeapEngine<Heap>::shrink(void *mem,ulen len) noexcept
 {
  if( !mem ) return false;

  Mutex::Lock lock(mutex);

  DeltaLen result=heap.shrink(mem,len);

  shrinkStat(result.delta);

  return result.ok;
 }

template <HeapType Heap>
void HeapEngine<Heap>::setLim(ulen limit_) noexcept
 {
  Mutex::Lock lock(mutex);

  limit=limit_;
 }

template <HeapType Heap>
void HeapEngine<Heap>::getStat(MemStatData &ret) noexcept
 {
  Mutex::Lock lock(mutex);

  ret=stat;
 }

template <HeapType Heap>
void HeapEngine<Heap>::getPeak(MemStatData &ret) noexcept
 {
  Mutex::Lock lock(mutex);

  ret=peak;
 }

} // namespace CCore

#endif


