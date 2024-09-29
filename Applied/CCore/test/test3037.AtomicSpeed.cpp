/* test3037.AtomicSpeed.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/task/Atomic.h>

namespace App {

namespace Private_3037 {

void testAtomic(ulen cnt)
 {
  Atomic test;

  ClockTimer timer;

  for(; cnt ;cnt--) test++;

  Printf(Con,"#;\n",timer.get());
 }

void testSimple(ulen cnt)
 {
  volatile unsigned test = 0 ;

  ClockTimer timer;

  for(; cnt ;cnt--) test=test+1;

  Printf(Con,"#;\n",timer.get());
 }

} // namespace Private_3037

using namespace Private_3037;

/* Testit<3037> */

template<>
const char *const Testit<3037>::Name="Test3037 AtomicSpeed";

template<>
bool Testit<3037>::Main()
 {
  // 1

  testAtomic(1000);
  testAtomic(2000);
  testAtomic(3000);

  // 2

  testSimple(1000);
  testSimple(2000);
  testSimple(3000);

  return true;
 }

} // namespace App

