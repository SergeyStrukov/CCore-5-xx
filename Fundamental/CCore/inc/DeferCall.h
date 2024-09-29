/* DeferCall.h */
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

#ifndef CCore_inc_DeferCall_h
#define CCore_inc_DeferCall_h

#include <CCore/inc/PlanInit.h>
#include <CCore/inc/MemBase.h>
#include <CCore/inc/List.h>
#include <CCore/inc/Task.h>
#include <CCore/inc/Tuple.h>
#include <CCore/inc/MemSpaceHeap.h>
#include <CCore/inc/FunctorType.h>

namespace CCore {

/* GetPlanInitNode_...() */

PlanInitNode * GetPlanInitNode_DeferCall();

/* classes */

class DeferCall;

class DeferCallHeap;

class DeferCallQueue;

struct DeferCouple;

class DeferTick;

template <class S> class DeferInput;

/* class DeferCall */

class DeferCall : NoCopy
 {
   DLink<DeferCall> link;

   friend class DeferCallQueue;

  public:

   DeferCall() {}

   ~DeferCall() {}

   virtual void call()=0;

   virtual void destroy(DeferCallQueue *defer_queue) noexcept =0;

   void safeCall();

   static void * operator new(std::size_t len,DeferCallQueue *defer_queue);

   static void * operator new(std::size_t len,JustTryType,DeferCallQueue *defer_queue) noexcept;

   static void operator delete(void *mem,DeferCallQueue *defer_queue) noexcept;

   static void operator delete(void *mem,JustTryType,DeferCallQueue *defer_queue) noexcept;
 };

/* class DeferCallHeap */

class DeferCallHeap : NoCopy
 {
   SpaceHeap heap;

   // cache

   static constexpr ulen MaxCount = 100 ;

   struct Node : NoCopy
    {
     Node *next;

     explicit Node(Node *next_) : next(next_) {}
    };

   Node *cache;
   ulen count;

  public:

   explicit DeferCallHeap(ulen mem_len);

   ~DeferCallHeap();

   void * alloc(ulen len);

   void free(void *mem) noexcept;

   static ulen GetMaxLen(); // max cached length
 };

/* class DeferCallQueue */

class DeferCallQueue : NoCopy
 {
   using Algo = DLink<DeferCall>::LinearAlgo<&DeferCall::link> ;

   Algo::FirstLast list;
   Algo::Top tick_list;
   Algo::Cur tick_cur;
   bool stop_flag;

   MSec tick_period;

   DeferCallHeap heap;

  private:

   void cleanup() noexcept;

   bool pump() noexcept;

   bool tick() noexcept;

   virtual void forward(TimeScope time_scope)=0;

  protected:

   void activate();

   void deactivate();

  public:

   // constructors

   static constexpr MSec DefaultTickPeriod = 40_msec ; // 25 Hz

   static constexpr unsigned DefaultTicksPerSec = 25 ;

   static constexpr ulen DefaultMemLen = 1_MByte ;

   explicit DeferCallQueue(MSec tick_period=DefaultTickPeriod,ulen mem_len=DefaultMemLen);

   ~DeferCallQueue();

   // heap

   void * try_alloc(ulen len) { return heap.alloc(len); }

   void * alloc(ulen len);

   void free(void *mem) noexcept { heap.free(mem); } // mem!=0

   template <NothrowDtorType T>
   void destroy(T *obj) noexcept
    {
     obj->~T();

     free(obj);
    }

   // call

   void post(DeferCall *defer_call) noexcept { list.ins_last(defer_call); }

   void post_first(DeferCall *defer_call) noexcept { list.ins_first(defer_call); }

   void start_tick(DeferCall *defer_call) noexcept { tick_list.ins(defer_call); }

   void stop_tick(DeferCall *defer_call) noexcept ;

   // loop

   void loop();

   void stop() { stop_flag=true; }

   // per-thread queue

   static DeferCallQueue * Get();

   static bool IsActive();

   static void Loop() { Get()->loop(); }

   static void Stop() { Get()->stop(); }
 };

inline void * DeferCall::operator new(std::size_t len,DeferCallQueue *defer_queue)
 {
  return defer_queue->alloc(len);
 }

inline void * DeferCall::operator new(std::size_t len,JustTryType,DeferCallQueue *defer_queue) noexcept
 {
  return defer_queue->try_alloc(len);
 }

inline void DeferCall::operator delete(void *mem,DeferCallQueue *defer_queue) noexcept
 {
  defer_queue->free(mem);
 }

inline void DeferCall::operator delete(void *mem,JustTryType,DeferCallQueue *defer_queue) noexcept
 {
  defer_queue->free(mem);
 }

inline constexpr unsigned operator "" _tick (unsigned long long time) { return time; }

inline constexpr unsigned operator "" _sectick (unsigned long long time) { return DeferCallQueue::DefaultTicksPerSec*time; }

/* struct DeferCouple */

struct DeferCouple
 {
  DeferCallQueue *defer_queue;
  DeferCall *defer_call;

  // constructors

  DeferCouple() noexcept : defer_queue(0),defer_call(0) {}

  DeferCouple(NothingType) : DeferCouple() {}

  DeferCouple(DeferCallQueue *defer_queue_,DeferCall *defer_call_)
   : defer_queue(defer_queue_),
     defer_call(defer_call_)
   {
    if( !defer_call ) defer_queue=0;
   }

  // props

  bool operator ! () const { return !defer_queue; }

  // cleanup

  void cleanup() noexcept
   {
    if( defer_queue )
      {
       defer_call->destroy(defer_queue);

       defer_queue=0;
       defer_call=0;
      }
   }

  // post

  void post() noexcept { defer_queue->post(defer_call); }

  void post_first() noexcept { defer_queue->post_first(defer_call); }

  void try_post() noexcept { if( defer_queue ) defer_queue->post(defer_call); }

  void try_post_first() noexcept { if( defer_queue ) defer_queue->post_first(defer_call); }

  void start_tick() noexcept { defer_queue->start_tick(defer_call); }

  void stop_tick() noexcept { defer_queue->stop_tick(defer_call); }
 };

/* class DeferTick */

class DeferTick : NoCopy
 {
   DeferCouple couple;
   bool active;

  public:

   // constructors

   explicit DeferTick(DeferCouple couple_=Nothing) : couple(couple_),active(false) {}

   ~DeferTick() { reset(); }

   // set/reset

   bool set(DeferCouple couple) noexcept;

   bool reset() noexcept { return set(Nothing); }

   void operator = (DeferCouple couple) noexcept { set(couple); }

   // start/stop

   bool start() noexcept;

   bool stop() noexcept;
 };

/* class DeferInput<S> */

template <class S>
class DeferInput : NoCopy
 {
   S *obj;
   DeferCallQueue *defer_queue;

   class DeferBase : public DeferCall
    {
      DLink<DeferBase> link;
      DeferInput<S> *parent;

      friend class DeferInput<S>;

     protected:

      S *obj;

     private:

      void cancel()
       {
        parent=0;
        obj=0;
       }

     public:

      explicit DeferBase(DeferInput<S> *parent_)
       : parent(parent_),
         obj(parent_->obj)
       {
        parent->list.ins(this);
       }

      ~DeferBase()
       {
        if( parent )
          {
           parent->list.del(this);
          }
       }
    };

   using Algo = typename DLink<DeferBase>::template LinearAlgo<&DeferBase::link> ;

   typename Algo::Top list;

   template <CopyCtorType ... TT>
   class DeferMethod : public DeferBase
    {
      void (S::* method)(TT... tt);
      Tuple<TT...> args;

     public:

      DeferMethod(DeferInput<S> *parent,void (S::* method_)(TT... tt),const TT & ... tt)
       : DeferBase(parent),
         method(method_),
         args(tt...)
       {
       }

      ~DeferMethod() {}

      virtual void call()
       {
        if( S *obj_=this->obj )
          {
           args.call( [obj_,method=this->method] (const TT & ... tt) { (obj_->*method)(tt...); } );
          }
       }

      virtual void destroy(DeferCallQueue *defer_queue) noexcept
       {
        defer_queue->destroy(this);
       }
    };

   template <FuncInitArgType<S &> FuncInit>
   class DeferFunc : public DeferBase
    {
      FuncInit func_init;

     public:

      DeferFunc(DeferInput<S> *parent,const FuncInit &func_init_) : DeferBase(parent),func_init(func_init_) {}

      ~DeferFunc() {}

      virtual void call()
       {
        if( S *obj_=this->obj )
          {
           FunctorTypeOf<FuncInit> func(func_init);

           func(*obj_);
          }
       }

      virtual void destroy(DeferCallQueue *defer_queue) noexcept
       {
        defer_queue->destroy(this);
       }
    };

  public:

   template <class ... TT>
   static constexpr ulen GetMessageLength() { return sizeof (DeferMethod<TT...>); }

   // constructors

   explicit DeferInput(S *obj);

   ~DeferInput() { cancel(); }

   void cancel();

   // post

   template <CopyCtorType ... TT>
   DeferCouple create(void (S::* method)(TT... tt),const TT & ... tt)
    {
     return DeferCouple(defer_queue,new(defer_queue) DeferMethod<TT...>(this,method,tt...));
    }

   template <CopyCtorType ... TT>
   void post(void (S::* method)(TT... tt),const TT & ... tt)
    {
     create(method,tt...).post();
    }

   template <CopyCtorType ... TT>
   void post_first(void (S::* method)(TT... tt),const TT & ... tt)
    {
     create(method,tt...).post_first();
    }

   // try post

   template <CopyCtorType ... TT>
   DeferCouple try_create(void (S::* method)(TT... tt),const TT & ... tt)
    {
     return DeferCouple(defer_queue,new(JustTry,defer_queue) DeferMethod<TT...>(this,method,tt...));
    }

   template <CopyCtorType ... TT>
   void try_post(void (S::* method)(TT... tt),const TT & ... tt)
    {
     try_create(method,tt...).try_post();
    }

   template <CopyCtorType ... TT>
   void try_post_first(void (S::* method)(TT... tt),const TT & ... tt)
    {
     try_create(method,tt...).try_post_first();
    }

   // post interface

   template <FuncInitArgType<S &> FuncInit>
   DeferCouple create(FuncInit func_init)
    {
     return DeferCouple(defer_queue,new(defer_queue) DeferFunc<FuncInit>(this,func_init));
    }

   template <FuncInitArgType<S &> FuncInit>
   void post(FuncInit func_init)
    {
     create(func_init).post();
    }

   template <FuncInitArgType<S &> FuncInit>
   void post_first(FuncInit func_init)
    {
     create(func_init).post_first();
    }

   // try post interface

   template <FuncInitArgType<S &> FuncInit>
   DeferCouple try_create(FuncInit func_init)
    {
     return DeferCouple(defer_queue,new(JustTry,defer_queue) DeferFunc<FuncInit>(this,func_init));
    }

   template <FuncInitArgType<S &> FuncInit>
   void try_post(FuncInit func_init)
    {
     try_create(func_init).try_post();
    }

   template <FuncInitArgType<S &> FuncInit>
   void try_post_first(FuncInit func_init)
    {
     try_create(func_init).try_post_first();
    }
 };

template <class S>
DeferInput<S>::DeferInput(S *obj_)
 : obj(obj_),
   defer_queue(DeferCallQueue::Get())
 {
 }

template <class S>
void DeferInput<S>::cancel()
 {
  for(auto cur=list.start(); +cur ;++cur) cur->cancel();

  list.init();
 }

} // namespace CCore

#endif


