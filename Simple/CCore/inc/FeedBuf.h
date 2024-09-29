/* FeedBuf.h */
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

#ifndef CCore_inc_FeedBuf_h
#define CCore_inc_FeedBuf_h

#include <CCore/inc/Cmp.h>

namespace CCore {

/* classes */

template <class T> struct FeedBuf;

/* struct FeedBuf<T> */

template <class T>
struct FeedBuf
 {
  ulen delta;
  CmpResult cmp;

  bool overflow() const { return cmp<0; } // src has extra data

  bool underflow() const { return cmp>0; } // dst has extra space

  FeedBuf(PtrLen<T> dst,PtrLen<const T> src)
   {
    cmp=Cmp(dst.len,src.len);

    if( cmp>=0 )
      {
       src.copyTo(dst.ptr);

       delta=src.len;
      }
    else
      {
       dst.copy(src.ptr);

       delta=dst.len;
      }
   }
 };

/* Feedup()  */

template <class T>
bool /* overflow */ Feedup(PtrLen<T> &dst,PtrLen<const T> src)
 {
  FeedBuf feed(dst,src);

  dst+=feed.delta;

  return feed.overflow();
 }

/* Pumpup() */

template <class T>
bool /* underflow */ Pumpup(PtrLen<T> &dst,PtrLen<const T> &src)
 {
  FeedBuf feed(dst,src);

  dst+=feed.delta;
  src+=feed.delta;

  return feed.underflow();
 }

} // namespace CCore

#endif

