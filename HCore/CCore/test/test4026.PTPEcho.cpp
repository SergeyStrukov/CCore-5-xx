/* test4026.PTPEcho.cpp */
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
#include <CCore/test/testNet.h>

#include <CCore/inc/net/AsyncUDPDevice.h>

namespace App {

namespace Private_4026 {

/* class Engine */

class Engine : NoCopy
 {
   Net::AsyncUDPEndpointDevice udp1;
   Net::AsyncUDPEndpointDevice udp2;

   ObjMaster udp1_master;
   ObjMaster udp2_master;

   Net::PTP::ClientDevice ptp1;
   Net::PTP::ClientDevice ptp2;

   ObjMaster ptp1_master;
   ObjMaster ptp2_master;

   PTPEchoTest test1;
   PTPEchoTest test2;

  public:

   explicit Engine(Net::UDPoint dst)
    : udp1(Net::PTPClientUDPort,dst),
      udp2(Net::PTPClientUDPort+1,dst),

      udp1_master(udp1,"udp[1]"),
      udp2_master(udp2,"udp[2]"),

      ptp1("udp[1]"),
      ptp2("udp[2]"),

      ptp1_master(ptp1,"ptp[1]"),
      ptp2_master(ptp2,"ptp[2]"),

      test1("ptp[1]"),
      test2("ptp[2]")
    {
    }

   ~Engine()
    {
    }

   void showStat()
    {
     ShowStat(udp1,"udp1");

     ShowStat(udp2,"udp2");

     ShowStat(ptp1,"ptp1");

     ShowStat(ptp2,"ptp2");

     ShowStat(test1,"test1");

     ShowStat(test2,"test2");
    }

   class StartStop : NoCopy
    {
      Net::AsyncUDPEndpointDevice::StartStop udp1;
      Net::AsyncUDPEndpointDevice::StartStop udp2;

      struct Pause
       {
        Pause() { Task::Sleep(1_sec); }

        ~Pause() { Task::Sleep(1_sec); }

       } pause;

      PTPEchoTest::StartStop test1;
      PTPEchoTest::StartStop test2;

     public:

      explicit StartStop(Engine &engine)
       : udp1(engine.udp1),
         udp2(engine.udp2),
         test1(engine.test1),
         test2(engine.test2)
       {
       }

      ~StartStop()
       {
       }
    };
 };

} // namespace Private_4026

using namespace Private_4026;

/* Testit<4026> */

template<>
const char *const Testit<4026>::Name="Test4026 PTPEcho";

template<>
bool Testit<4026>::Main()
 {
  Engine engine(Net::UDPoint(127,0,0,1,Net::PTPServerUDPort));

  {
   Engine::StartStop start_stop(engine);

   Task::Sleep(10_sec);
  }

  engine.showStat();

  return true;
 }

} // namespace App

