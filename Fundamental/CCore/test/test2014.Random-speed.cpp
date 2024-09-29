/* test2014.Random-speed.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/PlatformRandom.h>
#include <CCore/inc/Timer.h>

namespace App {

namespace Private_2014 {

template <class T>
void testDev(const char *name,ulen count)
 {
  Printf(Con,"#;\n\n",Title(name));

  Printf(Con,"count = #;\n\n",count);

  T random;
  uint8 sum=0;

  SecTimer timer;

  for(; count ;count--) sum+=random.next8();

  auto time=timer.get();

  Printf(Con,"time = #; sec\n\n",time,sum);
 }

} // namespace Private_2014

using namespace Private_2014;

/* Testit<2014> */

template<>
const char *const Testit<2014>::Name="Test2014 Random speed";

template<>
bool Testit<2014>::Main()
 {
  testDev<Random>("Random",1'000'000'000);
  testDev<PlatformRandom>("PlatformRandom",10'000'000);

  return true;
 }

} // namespace App

