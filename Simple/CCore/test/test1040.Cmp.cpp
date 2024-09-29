/* test1040.Cmp.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple Mini
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

namespace Private_1040 {

struct Test
 {
  int val;

  int operator <=> (const Test &obj) const noexcept
   {
    Printf(Con,"<=>\n");

    return CmpResultOf( val <=> obj.val );
   }

  bool operator == (const Test &obj) const noexcept
   {
    Printf(Con,"==\n");

    return val == obj.val ;
   }
 };

struct Test2
 {
  Test x;
  Test y;

  int operator <=> (const Test2 &obj) const noexcept
   {
    if( int ret =( x <=> obj.x ) ) return ret;

    return y <=> obj.y;
   }
 };

} // namespace Private_1040

using namespace Private_1040;

/* Testit<1040> */

template<>
const char *const Testit<1040>::Name="Test1040 Cmp";

template<>
bool Testit<1040>::Main()
 {
  Test a{0},b{1};

  Printf(Con,"a < b  #;\n",a < b);
  Printf(Con,"a <= b #;\n",a <= b);
  Printf(Con,"a > b  #;\n",a > b);
  Printf(Con,"a >= b #;\n",a >= b);

  Printf(Con,"a == b #;\n",a == b);
  Printf(Con,"a != b #;\n",a != b);

  Test2 A{1,0},B{1,1};

  Printf(Con,"A < B  #;\n",A < B);
  Printf(Con,"A <= B #;\n",A <= B);
  Printf(Con,"A > B  #;\n",A > B);
  Printf(Con,"A >= B #;\n",A >= B);

  return true;
 }

} // namespace App

