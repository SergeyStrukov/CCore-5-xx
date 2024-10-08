/* TaskHeap.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/TaskHeap.h>

#include <CCore/inc/MemPageHeap.h>

namespace CCore {

/* struct TaskHeapControl */

const Unid TaskHeapControl::TypeUnid={{0x324D'C95F,0xF624'43C7,0xF8FA'956C,0x7F2F'A4BF,0x767F'6F25,0x89B2'C248,0x159E'F3BD,0xB618'A35C}};

class TaskHeapControl::ObjectType : public MemBase_nocopy
 {
   PageHeap heap;

  private:

   union Prefix
    {
     ObjectType *heap;
     Prefix *next;

     explicit Prefix(ObjectType *heap_) : heap(heap_) {}

     ObjectType * getHeap() const
      {
       ObjectType *ret=heap;

       if( ret==TryGetTaskObject<TaskHeapControl>() ) return ret;

       return 0;
      }
    };

   static constexpr ulen Delta = Align(sizeof (Prefix)) ;
   static constexpr ulen MaxAllocLen = MaxULen-Delta ;

   static Prefix * GetPrefix(void *mem) { return static_cast<Prefix *>(PtrSub(mem,Delta)); }

  private:

   Mutex mutex;

   Prefix *list = 0 ;

   Atomic flag;

  private:

   void freeList()
    {
     Prefix *ptr;

     {
      Mutex::Lock lock(mutex);

      ptr=Replace_null(list);

      flag=0;
     }

     while( ptr )
       {
        Prefix *next=ptr->next;

        heap.free(ptr);

        ptr=next;
       }
    }

  public:

   ObjectType() {}

   explicit ObjectType(ulen min_page_alloc_len) : heap(min_page_alloc_len) {}

   ~ObjectType()
    {
     freeList();
    }

   void destroy() noexcept { delete this; }

   // methods

   void * alloc(ulen len) noexcept
    {
     if( len>MaxAllocLen ) return 0;

     if( flag ) freeList();

     if( void *mem=heap.alloc(len+Delta) )
       {
        new(PlaceAt(mem)) Prefix(this);

        return PtrAdd(mem,Delta);
       }

     return 0;
    }

   bool extend(Prefix *prefix,ulen len) noexcept
    {
     return heap.extend(prefix,len);
    }

   bool shrink(Prefix *prefix,ulen len) noexcept
    {
     return heap.shrink(prefix,len);
    }

   void free(Prefix *prefix) noexcept
    {
     heap.free(prefix);
    }

   void foreign_free(Prefix *prefix) noexcept
    {
     Mutex::Lock lock(mutex);

     prefix->next=list;

     list=prefix;

     flag=1;
    }

   // global

   static bool MemExtend(void *mem,ulen len) noexcept
    {
     if( !mem || len>MaxAllocLen ) return false;

     Prefix *prefix=GetPrefix(mem);

     if( ObjectType *heap=prefix->getHeap() )
       {
        return heap->extend(prefix,len+Delta);
       }
     else
       {
        return false;
       }
    }

   static bool MemShrink(void *mem,ulen len) noexcept
    {
     if( !mem || len>MaxAllocLen ) return false;

     Prefix *prefix=GetPrefix(mem);

     if( ObjectType *heap=prefix->getHeap() )
       {
        return heap->shrink(prefix,len+Delta);
       }
     else
       {
        return false;
       }
    }

   static void Free(void *mem) noexcept
    {
     if( !mem ) return;

     Prefix *prefix=GetPrefix(mem);

     if( ObjectType *heap=prefix->getHeap() )
       {
        heap->free(prefix);
       }
     else
       {
        prefix->heap->foreign_free(prefix);
       }
    }
 };

auto TaskHeapControl::BuilderType::create() -> ObjectType *
 {
  if( min_page_alloc_len ) return new ObjectType(min_page_alloc_len);

  return new ObjectType();
 }

/* class TaskHeap */

TaskHeap::TaskHeap(ulen min_page_alloc_len)
 : TaskObjectBuild<TaskHeapControl>(min_page_alloc_len)
 {
 }

TaskHeap::~TaskHeap()
 {
 }

void * TaskHeap::TryMemAlloc(ulen len) noexcept
 {
  TaskHeapControl::ObjectType *obj=TryGetTaskObject<TaskHeapControl>();

  if( !obj ) return 0;

  return obj->alloc(len);
 }

void * TaskHeap::MemAlloc(ulen len)
 {
  void *ret=TryMemAlloc(len);

  if( !ret ) GuardNoMem(len);

  return ret;
 }

bool TaskHeap::MemExtend(void *mem,ulen len) noexcept
 {
  return TaskHeapControl::ObjectType::MemExtend(mem,len);
 }

bool TaskHeap::MemShrink(void *mem,ulen len) noexcept
 {
  return TaskHeapControl::ObjectType::MemShrink(mem,len);
 }

void TaskHeap::MemFree(void *mem) noexcept
 {
  TaskHeapControl::ObjectType::Free(mem);
 }

} // namespace CCore


