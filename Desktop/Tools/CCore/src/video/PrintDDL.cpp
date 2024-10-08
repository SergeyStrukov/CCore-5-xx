/* PrintDDL.cpp */
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

#include <CCore/inc/video/PrintDDL.h>

#include <CCore/inc/Exception.h>

namespace CCore {
namespace Video {

/* struct DDLPrintableString */

void DDLPrintableString::GuardNotPrintable()
 {
  Printf(Exception,"CCore::Video::DDLPrintableString::PrintChar(...) : not printable character");
 }

} // namespace Video
} // namespace CCore

