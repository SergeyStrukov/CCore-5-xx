/* PtrStepLen.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_gadget_PtrStepLen_h
#define CCore_inc_gadget_PtrStepLen_h

#include <CCore/inc/gadget/Len.h>
#include <CCore/inc/gadget/Nothing.h>
#include <CCore/inc/gadget/UtilFunc.h>

namespace CCore {

/* classes */

template <class T> struct PtrStepLen;

/* struct PtrStepLen<T> */

template <class T>
struct PtrStepLen
 {
  T *ptr;
  ulen step;
  ulen len;

  // constructors

  PtrStepLen() noexcept : ptr(0),step(0),len(0) {}

  PtrStepLen(NothingType) : PtrStepLen() {}

  PtrStepLen(T *ptr_,ulen step_,ulen len_) : ptr(ptr_),step(step_),len(len_) {}

  operator PtrStepLen<const T>() const { return {ptr,step,len}; }

  // cursor

  ulen operator + () const { return len; }

  bool operator ! () const { return !len; }

  T & operator * () const { return *ptr; }

  T * operator -> () const { return ptr; }

  void operator ++ () { ptr+=step; len--; }

  void operator -- () { ptr-=step; len++; }

  PtrStepLen<T> operator += (ulen delta) // assume fit(delta)
   {
    PtrStepLen<T> ret(ptr,step,delta);

    ptr+=delta*step;
    len-=delta;

    return ret;
   }

  PtrStepLen<T> takeup(ulen delta) { return (*this)+=Min(delta,len); }

  T & take()
   {
    T &ret=*ptr;

    ptr+=step;
    len--;

    return ret;
   }

  PtrStepLen<T> getFinal() const { return {ptr+len*step,step,0}; }

  // fit

  bool fit(ulen length) const { return length<=len; }

  bool fit(ulen off,ulen length) const { return off<=len && length<=(len-off) ; }

  // parts

  PtrStepLen<T> prefix(ulen length) const { return {ptr,step,length}; }                          // assume fit(length)

  PtrStepLen<T> prefix(PtrStepLen<T> suffix) const { return {ptr,step,len-suffix.len}; }

  PtrStepLen<T> suffix(ulen length) const { return {ptr+(len-length)*step,step,length}; }        // assume fit(length)

  PtrStepLen<T> part(ulen off,ulen length) const { return {ptr+off*step,step,length}; }          // assume fit(off,length)

  PtrStepLen<T> part(ulen off) const { return {ptr+off*step,step,len-off}; }                     // assume fit(off)

  PtrStepLen<T> inner(ulen off,ulen endoff) const { return {ptr+off*step,step,len-off-endoff}; } // assume fit(off,endoff)

  PtrStepLen<T> safe_part(ulen off,ulen length) const
   {
    if( off>len ) return Null;

    return part(off,Min_cast(length,len-off));
   }

  PtrStepLen<T> safe_part(ulen off) const
   {
    if( off>len ) return Null;

    return part(off);
   }

  // index access

  T & operator [] (ulen index) const
   {
    return ptr[index*step];
   }

  T & at(ulen index) const
   {
    GuardIndex(index,len);

    return ptr[index*step];
   }

  T & back(ulen index) const // 1 is the last
   {
    return ptr[(len-index)*step];
   }

  // begin()/end() support

  bool operator != (ulen end_len) const { return len!=end_len; }
 };

/* begin()/end() */

template <class T>
PtrStepLen<T> begin(PtrStepLen<T> a) { return a; }

template <class T>
ulen end(PtrStepLen<T>) { return 0; }

} // namespace CCore

#endif


