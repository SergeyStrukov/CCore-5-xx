/* test3039.IntervalTree.cpp */
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

#include <CCore/inc/IntervalTree.h>
#include <CCore/inc/PlatformRandom.h>

namespace App {

namespace Private_3039 {

class Engine : NoCopy
 {
   PlatformRandom random;

   struct Int
    {
     int a;
     int b;
    };

   struct Rec
    {
     int a;
     int b;
     bool hit;

     void test(int t)
      {
       bool res = ( t>=a && t<b ) ;

       if( res!=hit )
         {
          Printf(Exception,"test failed");
         }

       //Printf(Con,"#;\n",(int)res);
      }
    };

   DynArray<Rec> list;

   IntervalTree<int> tree;

  private:

   Rec gen()
    {
     int a=random.select(1000);
     int b=a+1+random.select(1000);

     return {a,b,false};
    }

   void test()
    {
     int t=random.select(2000);

     for(Rec &rec : list ) rec.hit=false;

     tree.find(t, [&] (ulen ind) { list[ind].hit=true; } );

     for(Rec &rec : list ) rec.test(t);
    }

  public:

   Engine()
    : list(random.select(10,1000))
    {
     for(Rec &obj : list ) obj=gen();

     tree=IntervalTree<int>(Range(list), [] (Rec rec) ->Int { return {rec.a,rec.b}; } );
    }

   void run(unsigned count=1000)
    {
     for(; count ;count--) test();
    }
 };

} // namespace Private_3039

using namespace Private_3039;

/* Testit<3039> */

template<>
const char *const Testit<3039>::Name="Test3039 IntervalTree";

template<>
bool Testit<3039>::Main()
 {
  for(unsigned cnt=10'000; cnt ;cnt--)
    {
     Engine engine;

     engine.run();
    }

  return true;
 }

} // namespace App

