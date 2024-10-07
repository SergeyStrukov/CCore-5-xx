/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: Labs/Coroutines
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Print.h>
#include <CCore/inc/Exception.h>

#include "Generator.h"
#include "Task.h"
#include "Trace.h"

namespace App {

/* coGenData() */

Generator<int> coGenData()
 {
  for(int i=1; i<=25 ;i++) co_yield i;

  throw 12345;
 }

/* Main1() */

void Main1()
 {
  Generator<int> gen=coGenData();

  TraceBeg();

  while( gen.next() ) TraceValue(gen.getValue());

  try
    {
     gen.rethrowException();
    }
  catch(int ex)
    {
     TraceException(ex);
    }

  TraceEnd();
 }

/* class Buffer */

class Buffer : NoCopy
 {
   static constexpr ulen Len = 30 ;

   int buf[Len];
   ulen len = 0 ;

   Task<void> cofill;
   Task<void> cosum;

   // fill

   static constexpr int Last = 10'000 ;

   int next = 0 ;

   Task<void> coFillBuffer()
    {
     while( next<=Last )
       {
        while( len>=Len )
          {
           co_await cosum.getResumer();
          }

        buf[len++]=next++;
       }

     next=0;

     co_return;
    }

   // sum

   int sum = 0 ;

   Task<void> coSumBuffer()
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

   Buffer() {}

   int process()
    {
     cofill=coFillBuffer();
     cosum=coSumBuffer();

     cosum.push();

     if( !cosum.push() )
       {
        Printf(Exception,"App::Buffer::process() : push failed");
       }

     if( !cosum.done() || !cofill.done() )
       {
        Printf(Exception,"App::Buffer::process() : not finished");
       }

     return sum;
    }
 };

/* Main2() */

void Main2()
 {
  Buffer buf;

  TraceBeg();

  TraceValue(buf.process());

  TraceEnd();

  TraceBeg();

  TraceValue(buf.process());

  TraceEnd();
 }

} // namespace App

/* main() */

using namespace App;

int main()
 {
  try
    {
     ReportException report;

     Main1();
     Main2();

     report.guard();

     return 0;
    }
  catch(CatchType)
    {
     return 1;
    }
 }

