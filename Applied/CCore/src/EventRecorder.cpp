/* EventRecorder.cpp */
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

#include <CCore/inc/EventRecorder.h>

#include <CCore/inc/Task.h>
#include <CCore/inc/Exception.h>

namespace CCore {

/* functions */

void WaitAtomicZero(Atomic &count)
 {
  while( count ) Task::Yield();
 }

/* enum EventMarker */

const char * GetTextDesc(EventMarker marker)
 {
  static const char *const Table[]=
   {
    "None",      // EventMarker_None

    "Up",        // EventMarker_Up,
    "Down",      // EventMarker_Down

    "UpBlock",   // EventMarker_UpBlock
    "DownBlock", // EventMarker_DownBlock

    "UpUp",      // EventMarker_UpUp
    "UpPush",    // EventMarker_UpPush
    "UpUpPush",  // EventMarker_UpUpPush

    "Push",      // EventMarker_Push

    "Inc",       // EventMarker_Inc
    "Dec",       // EventMarker_Dec

    "Wait",      // EventMarker_Wait
    "Pass",      // EventMarker_Pass

    "Tick",      // EventMarker_Tick

    "Stop",      // EventMarker_Stop

    "Error"      // EventMarker_Error
   };

  return Table[marker];
 }

/* class EventMetaInfo */

const char * GetTextDesc(EventMetaInfo::Kind kind)
 {
  static const char *const Table[]=
   {
    "uint8",
    "uint16",
    "uint32",

    "enum uint8",
    "enum uint16",
    "enum uint32",

    "struct"
   };

  return Table[kind];
 }

const String * EventMetaInfo::EnumDesc::findValueName(uint32 value) const
 {
  if( auto *ptr=map.find(value) ) return &ptr->name;

  return 0;
 }

auto EventMetaInfo::EnumDesc::addValueName(uint32 value,const String &name,EventMarker marker) -> EnumDesc &
 {
  auto result=map.find_or_add(value,name,marker);

  if( !result.new_flag )
    {
     Printf(Exception,"CCore::EventMetaInfo::EnumDesc::addValueName(#;,...) : duplication, enum #;",value,name);
    }

  return *this;
 }

EventIdType EventMetaInfo::IndexToId(ulen index)
 {
  if( index>MaxUInt<EventIdType> )
    {
     Printf(Exception,"CCore::EventMetaInfo::IndexToId(...) : too many types");
    }

  return EventIdType(index);
 }

EventMetaInfo::EventMetaInfo(uint64 time_freq_)
 : time_freq(time_freq_)
 {
 }

EventMetaInfo::~EventMetaInfo()
 {
 }

auto EventMetaInfo::addEnum(Kind kind,const String &name) -> EnumDesc &
 {
  if( kind<Kind_enum_uint8 || kind>Kind_enum_uint32 )
    {
     Printf(Exception,"CCore::EventMetaInfo::addEnum(#;,...) : bad kind",kind);
    }

  EventIdType id=IndexToId(enum_list.getLen());

  return *enum_list.append_fill(id,name,kind);
 }

auto EventMetaInfo::addStruct(const String &name) -> StructDesc &
 {
  EventIdType id=IndexToId(struct_list.getLen());

  return *struct_list.append_fill(id,name);
 }

auto EventMetaInfo::addEvent(ulen alloc_len) -> EventDesc &
 {
  EventIdType id=IndexToId(event_list.getLen());

  return *event_list.append_fill(id,alloc_len,id);
 }

void EventMetaInfo::setClassId()
 {
  for(auto &desc : event_list ) desc.setClassId();
 }

void EventMetaInfo::appendEnums()
 {
  for(auto &desc : enum_list ) desc.append();
 }

auto EventMetaInfo::getEnum(EventIdType id) const -> const EnumDesc &
 {
  if( id>=enum_list.getLen() )
    {
     Printf(Exception,"CCore::EventMetaInfo::getEnum(...) : bad enum id");
    }

  return enum_list[id];
 }

auto EventMetaInfo::getStruct(EventIdType id) const -> const StructDesc &
 {
  if( id>=struct_list.getLen() )
    {
     Printf(Exception,"CCore::EventMetaInfo::getStruct(...) : bad struct id");
    }

  return struct_list[id];
 }

auto EventMetaInfo::getEvent(EventIdType id) const -> const EventDesc &
 {
  if( id>=event_list.getLen() )
   {
    Printf(Exception,"CCore::EventMetaInfo::getEvent(...) : bad event id");
   }

  return event_list[id];
 }

/* struct EventIdNode */

EventIdNode * EventIdNode::List = 0 ;

EventIdNode::EventIdNode(RegisterFunc func_,ulen size_of_)
 {
  next=List;
  List=this;

  id=0;
  size_of=size_of_;
  func=func_;
 }

void EventIdNode::reg(EventMetaInfo &info,ulen align)
 {
  auto &desc=info.addEvent(Align(size_of,align));

  id=desc.getId();

  func(info,desc);
 }

void EventIdNode::Register(EventMetaInfo &info,ulen align)
 {
  for(EventIdNode *ptr=List; ptr ;ptr=ptr->next) ptr->reg(info,align);

  info.setClassId();
 }

/* class EventTypeIdNode */

EventTypeIdNode * EventTypeIdNode::List = 0 ;

EventTypeIdNode::EventTypeIdNode(RegisterFunc func_)
 {
  next=List;
  List=this;

  func=func_;
  id=0;
  state=None;
 }

EventIdType EventTypeIdNode::getId(EventMetaInfo &info)
 {
  if( state==InProgress )
    {
     Printf(Exception,"CCore::EventTypeIdNode::getId(...) : type registration cyclic dependency");
    }

  if( state==None )
    {
     state=InProgress;

     id=func(info);

     state=Done;
    }

  return id;
 }

void EventTypeIdNode::Register(EventMetaInfo &info)
 {
  for(EventTypeIdNode *ptr=List; ptr ;ptr=ptr->next) ptr->state=None;

  for(EventTypeIdNode *ptr=List; ptr ;ptr=ptr->next) ptr->getId(info);
 }

/* struct EventControl */

void EventControl::Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc)
 {
  auto id_Type=info.addEnum_uint8("EventControlType"_str)
                   .addValueName(Type_Start,"Start"_str,EventMarker_Push)
                   .addValueName(Type_Tick,"Tick"_str,EventMarker_Tick)
                   .addValueName(Type_Stop,"Stop"_str,EventMarker_Stop)
                   .addValueName(Type_End,"End"_str)
                   .getId();

  auto id=info.addStruct("EventControl"_str)
              .addField_uint32<&EventControl::time>("time"_str)
              .addField_uint16<&EventControl::id>("id"_str)
              .addField_enum_uint8<&EventControl::type>(id_Type,"type"_str)
              .getId();

  desc.setStructId(info,id);
 }

} // namespace CCore


