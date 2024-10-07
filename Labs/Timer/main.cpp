/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: Labs/Timer
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Print.h>
#include <CCore/inc/Task.h>
#include <CCore/inc/Timer.h>
#include <CCore/inc/Array.h>

using namespace CCore;

/* class TimeLine */

class TimeLine : NoCopy
 {
   struct Rec
    {
     ClockTimer::ValueType time;
     unsigned task;

     void print(PrinterType auto &out) const
      {
       Printf(out,"[#;] + #;",task,time);
      }
    };

   SimpleArray<Rec> list;
   Atomic index;

  public:

   explicit TimeLine(ulen len) : list(len) {}

   ~TimeLine() {}

   void add(ClockTimer::ValueType time,unsigned task)
    {
     ulen ind=index++;

     if( ind<list.getLen() ) list[ind]={time,task};
    }

   void diff()
    {
     ulen total=index;
     ulen count=Min(total,list.getLen());

     if( count>1 )
       for(ulen ind=count-1; ind>0 ;ind--)
         {
          list[ind].time -= list[ind-1].time ;
         }
    }

   void print(PrinterType auto &out) const
    {
     ulen total=index;
     ulen count=Min(total,list.getLen());

     for(ulen ind=0; ind<count ;ind++)
       {
        //if( list[ind].time>2000 )
          Printf(out,"#;) #;\n",ind,list[ind]);
       }

     if( total>count )
       {
        Printf(out,"\nmore #; lines skipped\n",total-count);
       }
    }
 };

/* test1() */

void test1()
 {
  AntiSem asem;
  ClockTimer timer;
  TimeLine timeline(1'000'000);

  for(unsigned task=1; task<=10 ;task++)
    {
     asem.inc();

     RunFuncTask( [&timer,&timeline,task] ()
                  {
                   for(unsigned cnt=100'000; cnt-- ;) timeline.add(timer.get(),task);

                  } , asem.function_dec() );
    }

  asem.wait();

  timeline.diff();

  PrintFile out("out/test1.txt");

  Putobj(out,timeline);
 }

/* test2() */

void test2()
 {
  AntiSem asem;
  TimeLine timeline(1'000'000);

  for(unsigned task=1; task<=10 ;task++)
    {
     asem.inc();

     RunFuncTask( [&timeline,task] ()
                  {
                   ClockDiffTimer timer;

                   for(unsigned cnt=100'000; cnt-- ;) timeline.add(timer.get(),task);

                  } , asem.function_dec() );
    }

  asem.wait();

  PrintFile out("out/test2.txt");

  Putobj(out,timeline);
 }

/* test3() */

uint64 GetPerfFreq();
uint64 GetPerfTime();

using PerfTimer = Timer<uint64,GetPerfTime> ;

void test3()
 {
  Printf(Con,"freq = #; Hz\n",GetPerfFreq());

  AntiSem asem;
  PerfTimer timer;
  TimeLine timeline(1'000'000);

  for(unsigned task=1; task<=10 ;task++)
    {
     asem.inc();

     RunFuncTask( [&timer,&timeline,task] ()
                  {
                   for(unsigned cnt=100'000; cnt-- ;) timeline.add(timer.get(),task);

                  } , asem.function_dec() );
    }

  asem.wait();

  timeline.diff();

  PrintFile out("out/test3.txt");

  Putobj(out,timeline);
 }

/* main() */

int main()
 {
  test1();
  //test2();
  //test3();

  return 0;
 }

/* perf time functions */

#include <windows.h>

uint64 GetPerfFreq()
 {
  LARGE_INTEGER freq;

  QueryPerformanceFrequency(&freq);

  return freq.QuadPart;
 }

uint64 GetPerfTime()
 {
  LARGE_INTEGER time;

  QueryPerformanceCounter(&time);

  return time.QuadPart;
 }
