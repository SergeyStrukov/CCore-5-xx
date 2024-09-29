/* test2060.CharUtils.cpp */
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

#include <CCore/inc/String.h>
#include <CCore/inc/CharUtils.h>
#include <CCore/inc/PlatformRandom.h>

namespace App {

namespace Private_2060 {

class Engine : NoCopy
 {
   PlatformRandom random;

   static constexpr ulen MinLen =  10 ;
   static constexpr ulen MaxLen = 100 ;

   Char input[MaxLen];
   ulen len;

   Char buf[MaxLen];

  private:

   PtrLen<Char> getInput() { return Range(input,len); }

   void prepare()
    {
     len=random.select_uint<ulen>(MinLen,MaxLen);

     for(Char &ch : getInput() ) ch=random.select_uint<Unicode>(Bit(21));
    }

   String getString()
    {
     ScanCharBuf inp(getInput());

     PrintString out;

     for(; +inp ;++inp) out.put(*inp);

     return out.close();
    }

  public:

   Engine() {}

   ~Engine() {}

   void step(bool show)
    {
     prepare();

     String str=getString();

     PrintCharBuf out(Range(buf));

     Putobj(out,str);

     if( show ) Putobj(Con,str);

     auto temp=out.close();

     if( !temp.equal(getInput()) )
       {
        Printf(Exception,"fail");
       }
    }

   void run(ulen count=100000)
    {
     for(; count ;count--) step(count==1);
    }
 };

} // namespace Private_2060

using namespace Private_2060;

/* Testit<2060> */

template<>
const char *const Testit<2060>::Name="Test2060 CharUtils";

template<>
bool Testit<2060>::Main()
 {
  Engine engine;

  engine.run();

  return true;
 }

} // namespace App

