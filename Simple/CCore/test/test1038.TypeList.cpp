/* test1038.TypeList.cpp */
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

namespace App {

namespace Private_1038 {

template <int I>
struct Test
 {
  static void Show() { Printf(Con,"#;",I); }
 };

template <class T>
void ShowType()
 {
  T::Show();

  Putch(Con,',');
 }

template <class ... TT>
void Show(Meta::TypeListBox<TT...>)
 {
  ( ShowType<TT>() , ... );

  Putch(Con,'\n');
 }

} // namespace Private_1038

using namespace Private_1038;

/* Testit<1038> */

template<>
const char *const Testit<1038>::Name="Test1038 TypeList";

template<>
bool Testit<1038>::Main()
 {
  using Split = Meta::SplitTypeList<4, Test<1>,Test<2>,Test<3>,Test<4>,Test<5>,Test<6>,Test<7>,Test<8>,Test<9>,Test<10>,Test<11>,Test<12> > ;

  Show(Split::First{});
  Show(Split::Last{});

  return true;
 }

} // namespace App

