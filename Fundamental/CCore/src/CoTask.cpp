/* CoTask.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/CoTask.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* guard functions */

void GuardCoTaskNone()
 {
  Printf(Exception,"CCore::GuardCoTaskNone(...) : no coroutine");
 }

void GuardCoTaskFinished()
 {
  Printf(Exception,"CCore::GuardCoTaskActive(...) : coroutine is finished");
 }

} // namespace CCore

