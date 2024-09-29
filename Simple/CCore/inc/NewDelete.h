/* NewDelete.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_NewDelete_h
#define CCore_inc_NewDelete_h

#include <CCore/inc/MemBase.h>

namespace CCore {

/* concept AllocInitType<AllocInit> */

template <class AllocInit,class AllocType>
concept AllocInitType2 = requires(AllocInit &init,AllocType &alloc,ulen len,void *mem)
 {
  { AllocType(init) } noexcept;

  Ground<void *>( alloc.alloc(len) );

  { alloc.free(mem,len) } noexcept;
 } ;

template <class AllocInit>
concept AllocInitType = requires()
 {
  typename AllocInit::AllocType;

  requires AllocInitType2<AllocInit,typename AllocInit::AllocType> ;
 } ;

/* classes */

struct DefaultHeapAlloc;

template <AllocInitType AllocInit> class CustomAllocGuard;

/* struct DefaultHeapAlloc */

struct DefaultHeapAlloc
 {
  DefaultHeapAlloc() noexcept {}

  using AllocType = DefaultHeapAlloc ;

  void * alloc(ulen len) { return MemAlloc(len); }

  void free(void *ptr,ulen) noexcept { MemFree(ptr); }
 };

/* class CustomAllocGuard<AllocInit> */

template <AllocInitType AllocInit>
class CustomAllocGuard : NoCopy
 {
   using AllocType = typename AllocInit::AllocType ;

   AllocType alloc;
   ulen len;
   void *mem;

  public:

   CustomAllocGuard(AllocInit init,ulen len_)
    : alloc(init)
    {
     len=len_;
     mem=alloc.alloc(len);
    }

   ~CustomAllocGuard()
    {
     if( mem ) alloc.free(mem,len);
    }

   Place<void> getPlace() { return PlaceAt(mem); }

   void disarm() { mem=0; }
 };

/* New() */

template <NothrowDtorType T,AllocInitType AllocInit,class ... SS> requires ( ConstructibleType<T,SS...> )
T * New(AllocInit init,SS && ... ss)
 {
  CustomAllocGuard<AllocInit> alloc(init,sizeof (T));

  T *ret=new(alloc.getPlace()) T( std::forward<SS>(ss)... );

  alloc.disarm();

  return ret;
 }

/* Delete() */

template <NothrowDtorType T,AllocInitType AllocInit>
void Delete(AllocInit init,T *obj) noexcept
 {
  void *mem=obj;

  using AllocType = typename AllocInit::AllocType ;

  obj->~T();

  AllocType alloc(init);

  alloc.free(mem,sizeof (T));
 }

/* Delete_dynamic() */

template <NothrowDtorType T,AllocInitType AllocInit>
void Delete_dynamic(AllocInit init,T *obj) noexcept
 {
  Space space=obj->objSpace();

  using AllocType = typename AllocInit::AllocType ;

  obj->~T();

  AllocType alloc(init);

  alloc.free(space.mem,space.len);
 }

} // namespace CCore

#endif


