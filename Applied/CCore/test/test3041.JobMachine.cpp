/* test3041.JobMachine.cpp */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/JobMachine.h>
#include <CCore/inc/Array.h>

namespace App {

namespace Private_3041 {

/* test1() */

void AlgoNeg(const int *inp,int *out,ulen len)
 {
  for(; len-- ;) *(out++) = - *(inp++);
 }

NegBool AlgoTestNeg(const int *inp,const int *out,ulen len)
 {
  for(; len-- ;)
    if( *(out++) != - *(inp++) )
      {
       return true;
      }

  return false;
 }

void test1()
 {
  DynArray<int> inp(1'000'000);
  DynArray<int> out(1'000'000);

  {
   int val=1;

   for(int &x : inp ) x=val++;
  }

  AntiSem asem;

  {
   ScopeGuard guard( [&] () { asem.wait(); } );

   ulen delta=1500;
   ulen total=inp.getLen();

   for(ulen ind=0; ind<total ;)
     {
      ulen len=Min<ulen>(delta,total-ind);
      const int *src=inp.getPtr()+ind;
      int *dst=out.getPtr()+ind;

      AddFuncJob( [=] ()
                  {
                   Printf(Con,"[#;,+#;]\n",ind,len);

                   AlgoNeg(src,dst,len);

                  } ,asem);

      ind+=len;
     }
  }

  Atomic nok;

  {
   ScopeGuard guard( [&] () { asem.wait(); } );

   ulen delta=2500;
   ulen total=inp.getLen();

   for(ulen ind=0; ind<total ;)
     {
      ulen len=Min<ulen>(delta,total-ind);
      const int *src=inp.getPtr()+ind;
      int *dst=out.getPtr()+ind;

      AddFuncJob( [=,&nok] ()
                  {
                   Printf(Con,"[#;,+#;]\n",ind,len);

                   if( !AlgoTestNeg(src,dst,len) ) nok=1;

                  } ,asem);

      ind+=len;
     }
  }

  if( nok ) Printf(Con,"test failed\n");
 }

} // namespace Private_3041

using namespace Private_3041;

/* Testit<3041> */

template<>
const char *const Testit<3041>::Name="Test3041 JobMachine";

template<>
bool Testit<3041>::Main()
 {
  JobMachineInit jm_init;

  test1();

  Printf(Con,"cpu #;\n",Sys::GetCpuCount());

  return true;
 }

} // namespace App

