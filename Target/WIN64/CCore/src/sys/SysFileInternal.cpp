/* SysFileInternal.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN64
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/sys/SysFileInternal.h>

namespace CCore {
namespace Sys {

/* struct MakeZStr */

void MakeZStr::set(ToWChar result)
 {
  if( result.broken )
    {
     setError(FileError_BadName);
    }
  else if( result.overflow )
    {
     setError(FileError_TooLongPath);
    }
  else
    {
     setLen(result.len);
    }
 }

MakeZStr::MakeZStr(StrLen str,PtrLen<WChar> out)
 {
  set(ToWChar(out,str,true));
 }

MakeZStr::MakeZStr(StrLen str1,StrLen str2,PtrLen<WChar> out)
 {
  set(ToWChar(out,str1,str2,true));
 }

} // namespace Sys
} // namespace CCore


