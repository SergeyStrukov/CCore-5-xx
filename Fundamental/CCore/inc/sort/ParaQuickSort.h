/* ParaQuickSort.h */
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

#ifndef CCore_inc_sort_ParaQuickSort_h
#define CCore_inc_sort_ParaQuickSort_h

#include <CCore/inc/sort/QuickSort.h>

#include <CCore/inc/Fifo.h>
#include <CCore/inc/Job.h>

namespace CCore {

/* classes */

template <RanType Ran,SortContextType<Ran> Ctx=SortCtx<Ran> > struct ParaQuickSort;

/* struct ParaQuickSort<Ran,Ctx> */

template <RanType Ran,SortContextType<Ran> Ctx>
struct ParaQuickSort
 {
  static constexpr ulen MinLen      = 1000 ;
  static constexpr ulen JobQueueLen =  100 ;
  static constexpr ulen SplitLim    = 1000 ;
  static constexpr ulen SpawnLim    =  500 ;

  template <ULenType Len> class SortEngine;

  template <ULenType Len>
  static void Sort(Ran a,Len len,Ctx ctx);

  template <ULenType Len>
  static void Sort(Ran a,Len len) requires ( DefaultCtorType<Ctx> ) { Sort(a,len,Ctx()); }
 };

template <RanType Ran,SortContextType<Ran> Ctx>
template <ULenType Len>
class ParaQuickSort<Ran,Ctx>::SortEngine : public Funchor_nocopy
 {
   struct Unit
    {
     Ran a;
     Len len;

     Unit() : a(),len() {}

     Unit(Ran a_,Len len_) : a(a_),len(len_) {}
    };

   FifoBuf<Unit,JobQueueLen> queue;

   Mutex mutex;
   Sem sem;
   Atomic active;

   Ctx ctx;

  private:

   bool put(Unit unit)
    {
     Mutex::Lock lock(mutex);

     return queue.put(unit);
    }

   bool get(Unit &unit)
    {
     Mutex::Lock lock(mutex);

     return queue.get(unit);
    }

   void spawn(Ran a,Len len);

   void sort(Ran a,Len len);

  public:

   SortEngine(Ctx ctx,Ran a,Len len);

   ~SortEngine() {}

   void job();

   Function<void (void)> function_job() { return FunctionOf(this,&SortEngine::job); }
 };

template <RanType Ran,SortContextType<Ran> Ctx>
template <ULenType Len>
void ParaQuickSort<Ran,Ctx>::SortEngine<Len>::spawn(Ran a,Len len)
 {
  if( len<SpawnLim )
   {
    QuickSort<Ran,Ctx>::Sort(a,len,ctx);

    return;
   }

  if( put(Unit(a,len)) )
    {
     active++;

     sem.give();
    }
  else
    {
     sort(a,len);
    }
 }

template <RanType Ran,SortContextType<Ran> Ctx>
template <ULenType Len>
void ParaQuickSort<Ran,Ctx>::SortEngine<Len>::sort(Ran a,Len len)
 {
  Len S=len;

  if( len<SplitLim )
    {
     QuickSort<Ran,Ctx>::Sort(a,len,ctx);

     return;
    }

  while( !ShortSort<Ran,Ctx>::Sort(a,len,ctx) )
    {
     Len D=len/16;

     if( D<S )
       {
        S-=D;
       }
     else
       {
        HeapSort<Ran,Ctx>::Sort(a,len,ctx);

        return;
       }

     Ran p=QuickSort<Ran,Ctx>::Partition(a,a+(len/2),a+(len-1),ctx);

     Len len1=Len(p-a);
     Len len2=Len(len-1-len1);

     if( len1<=len2 )
       {
        spawn(a,len1);

        a=p+1;
        len=len2;
       }
     else
       {
        spawn(p+1,len2);

        len=len1;
       }
    }
 }

template <RanType Ran,SortContextType<Ran> Ctx>
template <ULenType Len>
ParaQuickSort<Ran,Ctx>::SortEngine<Len>::SortEngine(Ctx ctx_,Ran a,Len len)
 : mutex("ParaQuickSort.mutex"),
   sem("ParaQuickSort.sem"),
   ctx(ctx_)
 {
  spawn(a,len);
 }

template <RanType Ran,SortContextType<Ran> Ctx>
template <ULenType Len>
void ParaQuickSort<Ran,Ctx>::SortEngine<Len>::job()
 {
  for(;;)
    {
     sem.take();

     Unit unit;

     if( get(unit) )
       {
        sort(unit.a,unit.len);

        active--;
       }

     if( active==0 )
       {
        sem.give();

        break;
       }
    }
 }

template <RanType Ran,SortContextType<Ran> Ctx>
template <ULenType Len>
void ParaQuickSort<Ran,Ctx>::Sort(Ran a,Len len,Ctx ctx)
 {
  if( len<MinLen )
    {
     QuickSort<Ran,Ctx>::Sort(a,len,ctx);
    }
  else
    {
     SortEngine<Len> engine(ctx,a,len);

     Job job(engine.function_job());
    }
 }

} // namespace CCore

#endif


