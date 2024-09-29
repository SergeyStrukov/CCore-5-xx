/* test2059.BestSearch.cpp */
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

#include <CCore/inc/algon/BestSearch.h>

namespace App {

namespace Private_2059 {


} // namespace Private_2059

using namespace Private_2059;

/* Testit<2059> */

template<>
const char *const Testit<2059>::Name="Test2059 BestSearch";

template<>
bool Testit<2059>::Main()
 {
  int buf[]={1,5,3,0,7,8,9,5,10,2,5,8,6};

  auto r=Range(buf);

  auto by= [] (int a) { return a; } ;

  auto s1=Algon::BestSearchLessBy(r,by);

  auto s2=Algon::BestSearchGreaterBy(r,by);

  Printf(Con,"#; #;\n",*s1,*s2);

  return true;
 }

} // namespace App

