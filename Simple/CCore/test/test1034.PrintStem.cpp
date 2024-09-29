/* test1034.PrintStem.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/PrintStem.h>

namespace App {

namespace Private_1034 {

void test1()
 {
  PrintFirst stem(" "_c," , "_c);

  Putobj(Con,"{"_c);
  Putobj(Con,*(stem++),1);
  Putobj(Con,*(stem++),2);
  Putobj(Con,*(stem++),3);

  Putobj(Con,stem.end(" }\n\n","}\n\n"_c));
 }

void test2(ulen count)
 {
  PrintPeriod stem(10,"\n  "_c," , "_c," ,\n  "_c);

  Putobj(Con," {"_c);

  for(ulen i=0; i<count ;i++) Putobj(Con,*(stem++),i);

  Putobj(Con,stem.end("\n }\n\n"_c," }\n\n"_c));
 }

} // namespace Private_1034

using namespace Private_1034;

/* Testit<1034> */

template<>
const char *const Testit<1034>::Name="Test1034 PrintStem";

template<>
bool Testit<1034>::Main()
 {
  test1();

  test2(0);
  test2(1);
  test2(5);
  test2(10);
  test2(11);
  test2(15);
  test2(20);
  test2(21);
  test2(25);

  return true;
 }

} // namespace App

