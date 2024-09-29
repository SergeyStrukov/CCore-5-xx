/* test1039.ScanRange.cpp */
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

#include <CCore/inc/ScanRange.h>
#include <CCore/inc/CharProp.h>

namespace App {

namespace Private_1039 {

void Show(const ScanStr &scan)
 {
  Printf(Con,"#; $ #;\n",scan.before,scan.next);
 }

template <class Pred>
void Show(StrLen str,Pred pred)
 {
  Show(ScanStr(str,pred));
 }

} // namespace Private_1039

using namespace Private_1039;

/* Testit<1039> */

template<>
const char *const Testit<1039>::Name="Test1039 ScanRange";

template<>
bool Testit<1039>::Main()
 {
  Show("12345 abcdef"_c, charIsSpace<char> );

  Show("12345 // abcdef"_c, [] (StrLen str) { return str.hasPrefix("//"_c); } );

  return true;
 }

} // namespace App

