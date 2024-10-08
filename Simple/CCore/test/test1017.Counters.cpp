/* test1017.Counters.cpp */
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

#include <CCore/inc/Counters.h>

namespace App {

namespace Private_1017 {

/* enum E */

enum E
 {
  E1,
  E2,
  E3,

  ELim
 };

const char * GetTextDesc(E e)
 {
  switch( e )
    {
     case E1 : return "E1";
     case E2 : return "E2";
     case E3 : return "E3";

     default: return "";
    }
 }

/* functions */

void Add(Counters<E,ELim> &cnt)
 {
  cnt.count(E1,5);
  cnt.count(E2);
  cnt.count(E3,15);
 }

void Show(Counters<E,ELim> &cur,Counters<E,ELim> next)
 {
  Printf(Con,"count\n#;",next);

  cur.diff(next);

  Printf(Con,"diff\n#10;",next);
 }

} // namespace Private_1017

using namespace Private_1017;

/* Testit<1017> */

template<>
const char *const Testit<1017>::Name="Test1017 Counters";

template<>
bool Testit<1017>::Main()
 {
  Counters<E,ELim> cnt;

  Counters<E,ELim> cur;

  Add(cnt);
  Show(cur,cnt);

  Add(cnt);
  Show(cur,cnt);

  Add(cnt);
  Show(cur,cnt);

  return true;
 }

} // namespace App

