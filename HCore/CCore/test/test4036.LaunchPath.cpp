/* test4036.LaunchPath.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/LaunchPath.h>

namespace App {

/* Testit<4036> */

template<>
const char *const Testit<4036>::Name="Test4036 LaunchPath";

template<>
bool Testit<4036>::Main()
 {
  LaunchPath obj;

  Printf(Con,"dir = #;\npath = #;\n",obj.getDir(),obj.getPath());

  return true;
 }

} // namespace App

