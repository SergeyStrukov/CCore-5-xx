/* TaskHeap.h */
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

#ifndef CCore_inc_TaskHeap_h
#define CCore_inc_TaskHeap_h

#include <CCore/inc/PerTask.h>
#include <CCore/inc/Array.h>

namespace CCore {

/* classes */

struct TaskHeapControl;

class TaskHeap;

template <class Algo> struct TaskHeapMemAlgo;

template <class T,class Flags=GetNoThrowFlags<T> > struct TaskHeapArrayAlgo;

/* struct TaskHeapControl */

struct TaskHeapControl
 {
  static const Unid TypeUnid;

  class ObjectType;

  class BuilderType
   {
     ulen min_page_alloc_len;

    public:

     explicit BuilderType(ulen min_page_alloc_len_) : min_page_alloc_len(min_page_alloc_len_) {}

     ObjectType * create();
   };
 };

/* class TaskHeap */

class TaskHeap : TaskObjectBuild<TaskHeapControl>
 {
  public:

   explicit TaskHeap(ulen min_page_alloc_len=0);

   ~TaskHeap();

   static void * TryMemAlloc(ulen len) noexcept;

   static void * MemAlloc(ulen len);

   static bool MemExtend(void *mem,ulen len) noexcept;

   static bool MemShrink(void *mem,ulen len) noexcept;

   static void MemFree(void *mem) noexcept;
 };

/* type TaskHeapScope */

using TaskHeapScope = PerTaskObjectScope<TaskHeapControl> ;

/* struct TaskHeapMemAlgo<Algo> */

template <class Algo>
struct TaskHeapMemAlgo : Algo
 {
  static void * MemAlloc(ulen len) { return TaskHeap::MemAlloc(len); }

  static bool MemExtend(void *mem,ulen len) noexcept { return TaskHeap::MemExtend(mem,len); }

  static bool MemShrink(void *mem,ulen len) noexcept { return TaskHeap::MemShrink(mem,len); }

  static void MemFree(void *mem) noexcept { TaskHeap::MemFree(mem); }
 };

/* struct TaskHeapArrayAlgo<T,Flags> */

template <class T,class Flags>
struct TaskHeapArrayAlgo : TaskHeapMemAlgo< ArrayAlgo<T,Flags> > {};

/* type TaskDynArray<T> */

template <class T>
using TaskDynArray = DynArray<T,TaskHeapArrayAlgo<T> > ;

} // namespace CCore

#endif


