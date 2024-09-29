/* test2061.UniqueToFront.cpp */
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

#include <CCore/inc/algon/UniqueToFront.h>
#include <CCore/inc/PrintSet.h>

namespace App {

namespace Private_2061 {

/* test1() */

void test1(PtrLen<int> data)
 {
  auto tail=Algon::CopyUniqueToFront(data);

  Printf(Con,"#;\n",PrintSet(data.prefix(tail)));
 }

void test2(PtrLen<int> data)
 {
  auto tail=Algon::MoveUniqueToFront(data);

  Printf(Con,"#;\n",PrintSet(data.prefix(tail)));
 }

void test3(PtrLen<int> data)
 {
  auto tail=Algon::SwapUniqueToFront(data);

  Printf(Con,"#;\n",PrintSet(data.prefix(tail)));
 }

void test4(PtrLen<int> data)
 {
  auto tail=Algon::CopyUniqueToFrontBy(data, [] (int x) { return x; } );

  Printf(Con,"#;\n",PrintSet(data.prefix(tail)));
 }

void test5(PtrLen<int> data)
 {
  auto tail=Algon::MoveUniqueToFrontBy(data, [] (int x) { return x; } );

  Printf(Con,"#;\n",PrintSet(data.prefix(tail)));
 }

void test6(PtrLen<int> data)
 {
  auto tail=Algon::SwapUniqueToFrontBy(data, [] (int x) { return x; } );

  Printf(Con,"#;\n",PrintSet(data.prefix(tail)));
 }

} // namespace Private_2061

using namespace Private_2061;

/* Testit<2061> */

template<>
const char *const Testit<2061>::Name="Test2061 UniqueToFront";

template<>
bool Testit<2061>::Main()
 {
  {
   int data[]={1,2,2,3,3,3,4,4,4,4,5,5,5,5,5};

   test1(Range(data));
  }

  {
   int data[]={1,2,2,3,3,3,4,4,4,4,5,5,5,5,5};

   test2(Range(data));
  }

  {
   int data[]={1,2,2,3,3,3,4,4,4,4,5,5,5,5,5};

   test3(Range(data));
  }

  {
   int data[]={1,2,2,3,3,3,4,4,4,4,5,5,5,5,5};

   test4(Range(data));
  }

  {
   int data[]={1,2,2,3,3,3,4,4,4,4,5,5,5,5,5};

   test5(Range(data));
  }

  {
   int data[]={1,2,2,3,3,3,4,4,4,4,5,5,5,5,5};

   test6(Range(data));
  }

  return true;
 }

} // namespace App

