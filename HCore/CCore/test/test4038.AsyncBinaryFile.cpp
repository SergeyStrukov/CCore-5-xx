/* test4038.AsyncBinaryFile.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/AsyncFileDevice.h>
#include <CCore/inc/PutAsyncBinaryFile.h>
#include <CCore/inc/GetAsyncBinaryFile.h>
#include <CCore/inc/AsyncFileToMem.h>
#include <CCore/inc/PlatformRandom.h>

namespace App {

namespace Private_4038 {

struct Test
 {
  uint16 a = 0 ;
  uint32 b = 0 ;
  uint64 c = 0 ;

  Test() {}

  static uint32 FuncB(uint32 x) { return x*x; }

  static uint64 FuncC(uint64 x) { return x*x*x; }

  void set(uint16 x)
   {
    a=x;
    b=FuncB(x);
    c=FuncC(x);
   }

  void test()
   {
    if( b!=FuncB(a) || c!=FuncC(a) )
      {
       Printf(Exception,"fail");
      }
   }

  // save/load object

  using SaveLoadOrder = BeOrder ;

  template <template <class ...> class Ret,class T>
  static constexpr auto Fold()
   {
    return Ret(&T::a,&T::b,&T::c);
   }
 };

} // namespace Private_4038

using namespace Private_4038;

/* Testit<4038> */

template<>
const char *const Testit<4038>::Name="Test4038 AsyncBinaryFile";

template<>
bool Testit<4038>::Main()
 {
  AsyncFileDevice host;

  ObjMaster host_master(host,"host");

  constexpr ulen ObjCount = 1'000'000 ;

  {
   PlatformRandom random;
   PutAsyncBinaryFile out("host:test.bin");

   Test test;

   for(ulen count=ObjCount; count ;count--)
     {
      test.set(random.next16());

      out(test);
     }
  }

  {
   AsyncFileToMem file("host:test.bin");
   RangeGetDev dev(Range(file));

   Test test;

   for(ulen count=ObjCount; count ;count--)
     {
      dev(test);

      test.test();
     }

   if( !dev.finish() )
     {
      Printf(Exception,"finish fail");
     }
  }

  {
   GetAsyncBinaryFile dev("host:test.bin");

   Test test;

   for(ulen count=ObjCount; count ;count--)
     {
      dev(test);

      test.test();
     }

   if( dev.more() )
     {
      Printf(Exception,"finish fail");
     }
  }

  return true;
 }

} // namespace App

