/* test2058.ApplyToList.cpp */
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

#include <CCore/test/test.h>

#include <CCore/inc/algon/ApplyToList.h>

namespace App {

namespace Private_2058 {


} // namespace Private_2058

using namespace Private_2058;

/* Testit<2058> */

template<>
const char *const Testit<2058>::Name="Test2058 ApplyToList";

template<>
bool Testit<2058>::Main()
 {
  Algon::ApplyToList( [] (auto obj) { Printf(Con,"#;\n",obj); } ,1,2,3,4,5);

  return true;
 }

} // namespace App

