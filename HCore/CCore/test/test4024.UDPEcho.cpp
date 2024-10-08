/* test4024.UDPEcho.cpp */
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

#include <CCore/inc/net/UDPDevice.h>

namespace App {

/* Testit<4024> */

template<>
const char *const Testit<4024>::Name="Test4024 UDP Echo";

template<>
bool Testit<4024>::Main()
 {
  Net::UDPEndpointDevice udp(55'000,Net::UDPoint(127,0,0,1,Net::EchoUDPort));

  ObjMaster udp_master(udp,"udp");

  PacketEchoTest test("udp");

  {
   Net::UDPEndpointDevice::StartStop start_stop(udp);

   Task::Sleep(1_sec);

   test.run(5,100'000);

   Task::Sleep(1_sec);
  }

  ShowStat(udp,"udp");

  return true;
 }

} // namespace App

