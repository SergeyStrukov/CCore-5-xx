/* test3010.CancelPacketList.cpp */
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
#include <CCore/test/testRun.h>

#include <CCore/inc/CancelPacketList.h>
#include <CCore/inc/PacketSet.h>

namespace App {

namespace Private_3010 {

/* class Device */

class Device : NoCopy
 {
   CancelPacketList canlist;

   PacketList list;

   Sem sem;

  public:

   Device() {}

   ~Device()
    {
     canlist.complete(list);

     showStat();
    }

   void put(Packet<uint8> packet) { canlist.put(list,packet); sem.give(); }

   Packet<uint8> get() { sem.take(100_msec); return canlist.get(list); }

   void showStat()
    {
     CancelPacketList::StatInfo info;

     canlist.getStat(info);

     Printf(Con,"\n#15;\n",info);
    }
 };

/* class Engine */

class Engine : public FunchorDock_nocopy<Engine>
 {
   static constexpr ulen BurstCount = 10 ;

  private:

   MSec timeout;

   Device device;

   Atomic run_flag;

   RunTask run_task;

  private:

   void send()
    {
     PacketSet<uint8> pset(BurstCount);

     while( run_flag )
       {
        TimeScope time_scope(timeout);

        for(ulen count=BurstCount; count ;count--)
          {
           auto packet=pset.get(time_scope);

           if( !packet ) break;

           device.put(packet);
          }

        pset.wait(time_scope);
       }
    }

   void complete()
    {
     ulen done=0;

     while( run_flag )
       {
        auto packet=device.get();

        if( +packet )
          {
           packet.complete();

           done++;
          }
       }

     Printf(Con,"done = #;\n",done);
    }

   void stop()
    {
     run_flag=0;
    }

  public:

   Engine() : timeout(10_msec),run_task(functionOf<&Engine::stop>()) {}

   ~Engine() {}

   void start(ulen send_count,ulen complete_count)
    {
     run_flag=1;

     run_task(complete_count,functionOf<&Engine::complete>());

     run_task(send_count,functionOf<&Engine::send>());
    }
 };

} // namespace Private_3010

using namespace Private_3010;

/* Testit<3010> */

template<>
const char *const Testit<3010>::Name="Test3010 CancelPacketList";

template<>
bool Testit<3010>::Main()
 {
  Engine engine;

  engine.start(3,1);

  Task::Sleep(10_sec);

  return true;
 }

} // namespace App

