/* test4037.ExpandWildcard.cpp */
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

#include <CCore/inc/ExpandWildcard.h>

namespace App {

namespace Private_4037 {


} // namespace Private_4037

using namespace Private_4037;

/* Testit<4037> */

template<>
const char *const Testit<4037>::Name="Test4037 ExpandWildcard";

template<>
bool Testit<4037>::Main()
 {
  FileSystem fs;

  ExpandWildcard(fs,"../../../Simple/CCore/inc/P??.h"_c, [] (StrLen path) { Printf(Con,"#;\n",path); } );

  return true;
 }

} // namespace App

