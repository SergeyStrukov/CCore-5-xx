/* DeferCall.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/DeferCall.h>

#include <CCore/inc/OwnPtr.h>
#include <CCore/inc/TlsSlot.h>
#include <CCore/inc/Exception.h>

namespace CCore {

/* class DeferCall */

void DeferCall::safeCall()
 {
  try
    {
     call();
    }
  catch(...) {}
 }

/* class DeferCallHeap */

namespace Private_DeferCall {

inline constexpr ulen MaxLen = DeferInput<Meta::Empty>::GetMessageLength<int,int,int,int,int,int,int,int>() ;

} // namespace Private_DeferCall

using namespace Private_DeferCall;

DeferCallHeap::DeferCallHeap(ulen mem_len)
 : heap(mem_len),
   cache(0),
   count(0)
 {
 }

DeferCallHeap::~DeferCallHeap()
 {
  for(Node *node=cache; node ;)
    {
     Node *next=node->next;

     heap.free(node);

     node=next;
    }
 }

void * DeferCallHeap::alloc(ulen len)
 {
  if( len<=MaxLen )
    {
     if( Node *ret=cache )
       {
        cache=ret->next;

        count--;

        return ret;
       }
     else
       {
        return heap.alloc(MaxLen);
       }
    }

  return heap.alloc(len);
 }

void DeferCallHeap::free(void *mem) noexcept
 {
  static_assert( MaxLen>=sizeof (Node) ,"CCore::DeferCallHeap::MaxLen is too short");

  if( count<MaxCount )
    {
     cache=new(PlaceAt(mem)) Node(cache);

     count++;
    }
  else
    {
     heap.free(mem);
    }
 }

ulen DeferCallHeap::GetMaxLen() { return MaxLen; }

/* per-thread DeferCallQueue */

namespace Private_DeferCall {

struct Slot : TlsSlot
 {
  Slot() : TlsSlot("DeferCall") {}
 };

Slot Object CCORE_INITPRI_1 ;

} // namespace Private_DeferCall

using namespace Private_DeferCall;

PlanInitNode * GetPlanInitNode_DeferCall() { return &Object; }

void DeferCallQueue::activate()
 {
  if( Object.get() )
    {
     Printf(Exception,"CCore::DeferCallQueue::activate() : another queue is active");
    }

  Object.set(this);
 }

void DeferCallQueue::deactivate()
 {
  if( Object.get()==this ) Object.set(0);
 }

DeferCallQueue * DeferCallQueue::Get()
 {
  DeferCallQueue *ret=static_cast<DeferCallQueue *>(Object.get());

  if( !ret )
    {
     Printf(Exception,"CCore::DeferCallQueue::Get() : no active queue");
    }

  return ret;
 }

bool DeferCallQueue::IsActive()
 {
  return Object.get();
 }

/* class DeferCallQueue */

void DeferCallQueue::cleanup() noexcept
 {
  while( DeferCall *defer_call=list.del_first() ) defer_call->destroy(this);
 }

bool DeferCallQueue::pump() noexcept
 {
  while( DeferCall *defer_call=list.del_first() )
    {
     defer_call->safeCall();

     defer_call->destroy(this);

     if( stop_flag ) return true;
    }

  return false;
 }

bool DeferCallQueue::tick() noexcept
 {
  tick_cur=tick_list.start();

  while( +tick_cur )
    {
     auto temp=tick_cur.ptr;

     ++tick_cur;

     temp->safeCall();

     if( stop_flag ) return true;
    }

  return false;
 }

DeferCallQueue::DeferCallQueue(MSec tick_period_,ulen mem_len)
 : tick_cur(0),
   stop_flag(false),
   tick_period(tick_period_),
   heap(mem_len)
 {
 }

DeferCallQueue::~DeferCallQueue()
 {
  cleanup();
 }

void * DeferCallQueue::alloc(ulen len)
 {
  void *ret=try_alloc(len);

  if( !ret )
    {
     stop();

     GuardNoMem(len);
    }

  return ret;
 }

void DeferCallQueue::stop_tick(DeferCall *defer_call) noexcept
 {
  if( defer_call==tick_cur.ptr )
    {
     tick_list.del_and_move(tick_cur);
    }
  else
    {
     tick_list.del(defer_call);
    }
 }

void DeferCallQueue::loop()
 {
  stop_flag=false;

  TimeScope time_scope(tick_period);

  for(;;)
    {
     if( pump() ) break;

     forward(time_scope);

     if( stop_flag ) break;

     if( time_scope.nextScope_skip() )
       {
        if( tick() ) break;
       }
    }
 }

/* class DeferTick */

bool DeferTick::set(DeferCouple couple_) noexcept
 {
  if( active )
    {
     couple.stop_tick();
    }

  couple.cleanup();

  couple=couple_;

  if( active )
    {
     if( !couple )
       {
        active=false;

        return true;
       }

     couple.start_tick();
    }

  return false;
 }

bool DeferTick::start() noexcept
 {
  if( active || !couple ) return false;

  couple.start_tick();

  active=true;

  return true;
 }

bool DeferTick::stop() noexcept
 {
  if( !active ) return false;

  couple.stop_tick();

  active=false;

  return true;
 }

} // namespace CCore


