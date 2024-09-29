/* test3002.EventRecorder.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied Mini
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

namespace App {

namespace Private_3002 {

/* struct EventBody */

struct EventBody
 {
  uint8 task;
  uint32 number;

  static EventIdType Register(EventMetaInfo &info)
   {
    auto id=info.addEnum_uint8("TaskId"_str)
                .addValueName(1,"Task1"_str)
                .addValueName(2,"Task2"_str)
                .addValueName(3,"Task3"_str)
                .addValueName(4,"Task4"_str)
                .addValueName(5,"Task5"_str)
                .addValueName(6,"Task6"_str)
                .addValueName(7,"Task7"_str)
                .addValueName(8,"Task8"_str)
                .getId();

    return info.addStruct("EventBody"_str)
               .addField_enum_uint8<&EventBody::task>(id,"task"_str)
               .addField_uint32<&EventBody::number>("number"_str)
               .getId();
   }
 };

/* struct Event */

struct Event
 {
  EventTimeType time;
  EventIdType id;

  EventBody body;

  void init(EventTimeType time_,EventIdType id_,uint8 task_,uint32 number_)
   {
    time=time_;
    id=id_;

    body.task=task_;
    body.number=number_;
   }

  static void Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc)
   {
    auto id_body=EventBody::Register(info);

    auto id=info.addStruct("Event"_str)
                .addField_uint32<&Event::time>("time"_str)
                .addField_uint16<&Event::id>("id"_str)
                .addField_struct<&Event::body>(id_body,"body"_str)
                .getId();

    desc.setStructId(info,id);
   }
 };

} // namespace Private_3002

using namespace Private_3002;

/* Testit<3002> */

template<>
const char *const Testit<3002>::Name="Test3002 EventRecorder";

template<>
bool Testit<3002>::Main()
 {
  const uint32 MaxNumber = 100'000 ;
  const uint8 MaxTask = 8 ;

  TaskEventRecorder recorder(25_MByte);

  {
   TickTask tick_task(recorder);

   RunTask run_task;

   run_task( [&] () {
                     TaskEventHostType::StartStop start_stop(TaskEventHost,&recorder);

                     for(uint32 number=1; number<=MaxNumber ;number++) TaskEventHost.add<Event>(MaxTask+1,number);

                    } );

   for(uint8 task=1; task<=MaxTask ;task++)
     run_task( [=] () {
                       for(uint32 number=1; number<=MaxNumber ;number++) TaskEventHost.add<Event>(task,number);

                      } );

  }

  PrintFile out("test3002.txt");

  Putobj(out,recorder);

  PutBinaryFile dev("test3002.bin");

  dev(recorder);

  return true;
 }

} // namespace App

