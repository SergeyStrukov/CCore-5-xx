/* MemBase.h */
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

#ifndef CCore_inc_MemBase_h
#define CCore_inc_MemBase_h

#include <CCore/inc/Printf.h>
#include <CCore/inc/PlanInit.h>

namespace CCore {

/* GetPlanInitNode_...() */

PlanInitNode * GetPlanInitNode_MemBase();

/* word JustTry */

enum JustTryType { JustTry };

/* GuardNoMem() */

void GuardNoMem(ulen len);

/* Mem...() */

void * TryMemAlloc(ulen len) noexcept;

void * MemAlloc(ulen len);

ulen MemLen(const void *mem) noexcept;       // mem may == 0

bool MemExtend(void *mem,ulen len) noexcept; // mem may == 0

bool MemShrink(void *mem,ulen len) noexcept; // mem may == 0

void MemFree(void *mem) noexcept;            // mem may == 0

void MemLim(ulen limit) noexcept;

/* classes */

struct MemStatData;

struct MemStat;

struct MemPeak;

class MemScope;

struct MemBase;

/* struct MemStatData */

struct MemStatData
 {
  ulen block_count = 0 ;
  ulen len_count = 0 ;

  MemStatData() noexcept {}

  // count

  void add(ulen len) { block_count++; len_count+=len; }

  void del(ulen len) { block_count--; len_count-=len; }

  void extend(ulen delta) { len_count+=delta; }

  void shrink(ulen delta) { len_count-=delta; }

  // set

  void set(const MemStatData &obj) { *this=obj; }

  void setMax(const MemStatData &obj)
   {
    if( obj.len_count>len_count ) set(obj);
   }

  // compare

  bool operator == (const MemStatData &obj) const
   {
    return block_count==obj.block_count && len_count==obj.len_count ;
   }

  bool operator != (const MemStatData &obj) const
   {
    return block_count!=obj.block_count || len_count!=obj.len_count ;
   }

  // print object

  void print(PrinterType auto &out) const
   {
    Printf(out,"(#; blocks, #; total length)",block_count,len_count);
   }
 };

/* struct MemStat */

struct MemStat : MemStatData
 {
  MemStat() noexcept;
 };

/* struct MemPeak */

struct MemPeak : MemStatData
 {
  MemPeak() noexcept;
 };

/* class MemScope */

class MemScope : NoCopy
 {
   MemStat on_init;

  public:

   MemScope() noexcept {}

   ~MemScope();
 };

/* struct MemBase */

struct MemBase
 {
  // placement new/delete

  void * operator new(std::size_t,Place<void> place) { return place; }

  void operator delete(void *,Place<void>) noexcept {}

  // new/delete

  void * operator new(std::size_t len) { return MemAlloc(len); }

  void * operator new(std::size_t len,JustTryType) noexcept { return TryMemAlloc(len); }

  void operator delete(void *mem) noexcept { MemFree(mem); }

  void operator delete(void *mem,JustTryType) noexcept { MemFree(mem); }

  // extra space

  template <class T>
  static Place<void> ExtraSpace(T *obj) { return PlaceAt(obj)+Align(sizeof (T)); }

  void * operator new(std::size_t len,ulen extra,ulen size_of=1) { return MemAlloc(LenOf(extra,size_of,Align(len))); }

  void operator delete(void *mem,ulen,ulen) noexcept { MemFree(mem); }
 };

/* type MemBase_nocopy */

using MemBase_nocopy = NoCopyBase<MemBase> ;

} // namespace CCore

#endif


