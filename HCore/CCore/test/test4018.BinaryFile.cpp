/* test4018.BinaryFile.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/PutBinaryFile.h>
#include <CCore/inc/GetBinaryFile.h>
#include <CCore/inc/FileToMem.h>
#include <CCore/inc/PlatformRandom.h>

namespace App {

namespace Private_4018 {

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

} // namespace Private_4018

using namespace Private_4018;

/* Testit<4018> */

template<>
const char *const Testit<4018>::Name="Test4018 BinaryFile";

template<>
bool Testit<4018>::Main()
 {
  constexpr ulen ObjCount = 1'000'000 ;

  {
   PlatformRandom random;
   PutBinaryFile out("test.bin");

   Test test;

   for(ulen count=ObjCount; count ;count--)
     {
      test.set(random.next16());

      out(test);
     }
  }

  {
   FileToMem file("test.bin");
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
   GetBinaryFile dev("test.bin");

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

