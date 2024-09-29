/* FileNameCmp.cpp */
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

#include <CCore/inc/video/FileNameCmp.h>

#include <CCore/inc/Path.h>

namespace CCore {
namespace Video {

/* functions */

CmpResult NativeNameCmp(StrLen a,StrLen b)
 {
  return StrCmp(a,b);
 }

CmpResult ExtNameCmp(StrLen a,StrLen b)
 {
  if( PathBase::IsDotDot(a) ) return PathBase::IsDotDot(b)?CmpEqual:CmpLess;

  if( PathBase::IsDotDot(b) ) return CmpGreater;

  for(;;)
    {
     SplitExt exta(a);
     SplitExt extb(b);

     if( !extb )
       {
        if( !exta )
          {
           return NativeNameCmp(a,b);
          }
        else
          {
           return CmpGreater;
          }
       }
     else
       {
        if( !exta )
          {
           return CmpLess;
          }
        else
          {
           if( CmpResult cmp=NativeNameCmp(exta.ext,extb.ext) ) return cmp;

           a=exta.name;
           b=extb.name;
          }
       }
    }
 }

} // namespace Video
} // namespace CCore

