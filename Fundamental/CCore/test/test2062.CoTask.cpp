/* test2062.CoTask.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/CoTask.h>

namespace App {

namespace Private_2062 {

/* class Test1 */

class Test1 : NoCopy
 {
   int value = 0 ;

  private:

   CoTask<void> coGen()
    {
     for(int i=1; i<=25 ;i++)
       {
        value=i;

        co_await std::suspend_always{};
       }
    }

  public:

   Test1() {}

   void process()
    {
     CoTask<void> cogen=coGen();

     Printf(Con,"beg\n");

     for(;;)
       {
        auto result=cogen.push();

        if( result.ok )
          {
           break;
          }
        else
          {
           Printf(Con,"#;\n",value);
          }
       }

     Printf(Con,"end\n\n");
    }
 };

/* test1() */

void test1()
 {
  Test1 test;

  test.process();
 }

/* struct Result */

struct Result
 {
  int value;

  Result() : value(0) {}

  explicit Result(int value_) : value(value_) {}

  Result(const Result &obj) : value(obj.value) { Printf(Con,"copy\n"); }

  Result(Result &&obj) : value(obj.value) { Printf(Con,"move\n"); }

  Result & operator = (const Result &obj)
   {
    value=obj.value;

    Printf(Con,"copy =\n");

    return *this;
   }

  Result & operator = (Result &&obj)
   {
    value=obj.value;

    Printf(Con,"move =\n");

    return *this;
   }
 };

/* class Test2 */

class Test2 : NoCopy
 {
   int value = 0 ;

  private:

   CoTask<Result> coGen()
    {
     for(int i=1; i<=25 ;i++)
       {
        value=i;

        co_await std::suspend_always{};
       }

     co_return Result(12345);
    }

  public:

   Test2() {}

   void process()
    {
     CoTask<Result> cogen=coGen();

     Printf(Con,"beg\n");

     for(;;)
       {
        auto result=cogen.push();

        if( result.ok )
          {
           Printf(Con,"result = #;\n",result.obj.value);

           break;
          }
        else
          {
           Printf(Con,"#;\n",value);
          }
       }

     Printf(Con,"end\n\n");
    }
 };

/* test2() */

void test2()
 {
  Test2 test;

  test.process();
 }

/* class Test3 */

class Test3 : NoCopy
 {
   static constexpr ulen Len = 30 ;

   int buf[Len];
   ulen len = 0 ;

   CoTask<void> cofill;
   CoTask<void> cosum;

   // fill

   static constexpr int Last = 10'000 ;

   int next = 0 ;

   CoTask<void> coFillBuffer()
    {
     while( next<=Last )
       {
        while( len>=Len )
          {
           co_await cosum.getResumer();
          }

        buf[len++]=next++;
       }
    }

   // sum

   int sum = 0 ;

   CoTask<void> coSumBuffer()
    {
     for(;;)
       {
        while( len==0 )
          {
           if( cofill.done() ) co_return;

           co_await cofill.getResumer();
          }

        for(int val : Range(buf,len) ) sum+=val;

        len=0;
       }
    }

  public:

   Test3() {}

   void process()
    {
     next=0;
     sum=0;

     cofill=coFillBuffer();
     cosum=coSumBuffer();

     cosum.push();

     if( !cosum.push() )
       {
        Printf(Exception,"Test3::process() : push failed");
       }

     Printf(Con,"sum = #;\n",sum);
    }
 };

/* test3() */

void test3()
 {
  Test3 test;

  test.process();
 }

/* test4() */

class Test4 : CoTaskStack<10>
 {
  private:

   int count = 0 ;

   CoTask<void> process3()
    {
     for(;;)
       {
        while( !count ) co_await std::suspend_always{};

        Printf(Con,"Got #;\n",count);

        if( count<0 )
          {
           Printf(Exception,"Negative #;",count);
          }

        count--;

        co_await coret();
       }
    }

   CoTask<void> coproc3;

  private:

   CoTask<void> process2()
    {
     for(;;)
       {
        co_await cocall(coproc3);

        Printf(Con,"Stage 1\n");

        co_await cocall(coproc3);

        Printf(Con,"Stage 2\n");

        co_await cocall(coproc3);

        Printf(Con,"Stage 3\n");

        co_await coret();
       }
    }

   CoTask<void> coproc2;

  private:

   CoTask<void> process1()
    {
     co_await cocall(coproc2);

     Printf(Con,"Step 1\n");

     co_await cocall(coproc2);

     Printf(Con,"Step 2\n\n");
    }

   CoTask<void> coproc1;

  public:

   Test4()
    {
     coproc1=process1();
     coproc2=process2();
     coproc3=process3();

     prepare(coproc1);
    }

   ~Test4() {}

   void run1()
    {
     while( !push() )
       {
        count = 2 ;
       }
    }

   void run2()
    {
     push();

     count = -1 ;

     Printf(Con,"finish\n");

     push();
    }
 };

void test4()
 {
  {
   Test4 test;

   test.run1();
  }

  try
    {
     Test4 test;

     test.run2();
    }
  catch(CatchType)
    {
     Printf(Con,"exception\n\n");
    }
 }

} // namespace Private_2062

using namespace Private_2062;

/* Testit<2062> */

template<>
const char *const Testit<2062>::Name="Test2062 CoTask";

template<>
bool Testit<2062>::Main()
 {
  //test1();
  //test2();
  //test3();
  test4();

  return true;
 }

} // namespace App

