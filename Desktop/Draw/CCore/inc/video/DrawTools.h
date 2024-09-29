/* DrawTools.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_DragTools_h
#define CCore_inc_video_DragTools_h

#include <CCore/inc/video/MPoint.h>

namespace CCore {
namespace Video {

/* concept PointSetTypes<TT> */

template <class ... TT>
concept PointSetTypes = ( ... && IsType<TT,Point> ) ;

/* concept MPointSetTypes<TT> */

template <class ... TT>
concept MPointSetTypes = ( ... && OneOfTypes<TT,Point,MPoint> ) ;

/* enum SolidFlag */

enum SolidFlag
 {
  SolidOdd,
  SolidAll
 };

/* enum HalfFlag */

enum HalfFlag // contraclockwise
 {
  HalfPos,
  HalfNeg,
 };

} // namespace Video
} // namespace CCore

#endif


