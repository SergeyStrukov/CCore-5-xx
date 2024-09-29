/* test6012.SysError.cpp */
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

#include <CCore/inc/PrintError.h>

namespace App {

/* Testit<6012> */

template<>
const char *const Testit<6012>::Name="Test6012 SysError";

template<>
bool Testit<6012>::Main()
 {
  for(int e=1; e<100 ;e++) Printf(Con,"#;\n",PrintError(Sys::ErrorType(e)));

  return true;
 }

} // namespace App

