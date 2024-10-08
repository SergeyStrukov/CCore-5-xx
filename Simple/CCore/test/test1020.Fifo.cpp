/* test1020.Fifo.cpp */
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

#include <CCore/inc/Fifo.h>
#include <CCore/inc/OwnPtr.h>
#include <CCore/inc/PlatformRandom.h>
#include <CCore/inc/MemBase.h>

namespace App {

namespace Private_1020 {

/* class TestFifo */

class TestFifo : NoCopy
 {
   FifoBuf<int,10> fifo;

  public:

   TestFifo() {}

   ulen getCount() const { return fifo.getCount(); }

   bool put(int value) { return fifo.put(value); }

   bool get(int &ret) { return fifo.get(ret); }
 };

/* struct Int */

struct Int : MemBase_nocopy
 {
  int value;

  explicit Int(int value_=0) : value(value_) {}

  operator int() const { return value; }
 };

/* class TestFifo2 */

class TestFifo2 : NoCopy
 {
   FifoBuf<OwnPtr<Int>,10> fifo;

  public:

   TestFifo2() {}

   ulen getCount() const { return fifo.getCount(); }

   bool put(int value);

   bool get(int &ret);
 };

bool TestFifo2::put(int value)
 {
  OwnPtr<Int> obj(new Int(value));

  return fifo.put_swap(obj);
 }

bool TestFifo2::get(int &ret)
 {
  OwnPtr<Int> obj;

  if( fifo.get_swap(obj) )
    {
     ret=*obj;

     return true;
    }

  return false;
 }

/* functions */

template <class T>
void test1()
 {
  PlatformRandom random;
  T fifo;
  int next=1;

  for(unsigned count=100; count ;count--)
    {
     Printf(Con,"[#;]",fifo.getCount());

     switch( random.select(2) )
       {
        case 0 :
         {
          if( fifo.put(next) ) next++;
         }
        break;

        case 1 :
         {
          int ret;

          if( fifo.get(ret) ) Printf(Con," #;",ret);
         }
        break;
       }

     Putch(Con,'\n');
    }
 }

template <class T>
void test2()
 {
  PlatformRandom random;
  T fifo;
  int put_next=1;
  int get_next=1;

  for(unsigned count=1'000'000; count ;count--)
    {
     switch( random.select(2) )
       {
        case 0 :
         {
          if( fifo.put(put_next) ) put_next++;
         }
        break;

        case 1 :
         {
          int ret;

          if( fifo.get(ret) )
            {
             if( ret!=get_next )
               {
                Printf(Exception,"Broken sequence");
               }

             get_next++;
            }
         }
        break;
       }
    }
 }

} // namespace Private_1020

using namespace Private_1020;

/* Testit<1020> */

template<>
const char *const Testit<1020>::Name="Test1020 Fifo";

template<>
bool Testit<1020>::Main()
 {
  test1<TestFifo>();
  test2<TestFifo>();

  test1<TestFifo2>();
  test2<TestFifo2>();

  return true;
 }

} // namespace App

