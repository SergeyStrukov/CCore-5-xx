/* test6005.IntelRandom.cpp */
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

#include <CCore/test/test.h>

#include <CCore/inc/crypton/IntelCPUID.h>
#include <CCore/inc/crypton/IntelRandom.h>

namespace App {

/* Testit<6005> */

template<>
const char *const Testit<6005>::Name="Test6005 IntelRandom";

template<>
bool Testit<6005>::Main()
 {
  Crypton::IntelCPUID1 cpuid;

  Printf(Con,"EAX=#.h; EBX=#.h; ECX=#.h; EDX=#.h;\n",cpuid.EAX,cpuid.EBX,cpuid.ECX,cpuid.EDX);

  Crypton::IntelRandom random;

  for(unsigned cnt=10; cnt ;cnt--) Printf(Con,"#.h;\n",random.next());

  return true;
 }

} // namespace App

