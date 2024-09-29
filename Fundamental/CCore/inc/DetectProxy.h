/* DetectProxy.h */
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

#ifndef CCore_inc_DetectProxy_h
#define CCore_inc_DetectProxy_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* concept Has_ProxyType<T> */

template <class T>
concept Has_ProxyType = requires()
 {
  typename T::ProxyType;
 } ;

/* concept No_ProxyType<T> */

template <class T>
concept No_ProxyType = !Has_ProxyType<T> ;

/* concept Has_OptType<T> */

template <class T>
concept Has_OptType = requires()
 {
  typename T::OptType;
 } ;

/* concept No_OptType<T> */

template <class T>
concept No_OptType = !Has_OptType<T> ;

} // namespace CCore

#endif


