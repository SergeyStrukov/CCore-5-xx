/* CancelPacketList.h */
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

#ifndef CCore_inc_CancelPacketList_h
#define CCore_inc_CancelPacketList_h

#include <CCore/inc/Packet.h>
#include <CCore/inc/Task.h>
#include <CCore/inc/Counters.h>

namespace CCore {

/* classes */

class CancelPacketList;

/* class CancelPacketList */

class CancelPacketList : public Funchor_nocopy
 {
  public:

   enum Event
    {
     Event_cancel,
     Event_cancel_on_put,

     EventLim
    };

   friend const char * GetTextDesc(Event ev);

   using StatInfo = Counters<Event,EventLim> ;

  private:

   Mutex mutex;

   StatInfo stat;

  private:

   void cancel(PacketHeader *packet);

   PacketFunction function_cancel() { return FunctionOf(this,&CancelPacketList::cancel); }

  public:

   // constructors

   CancelPacketList();

   explicit CancelPacketList(TextLabel name);

   ~CancelPacketList();

   // methods

   void getStat(StatInfo &ret);

   bool try_put(PacketList &list,PacketHeader *packet);

   bool try_put(PacketList &list,PacketType auto packet) { return try_put(list,GetPacketHeader(packet)); }

   void put(PacketList &list,PacketHeader *packet);

   void put(PacketList &list,PacketType auto packet) { put(list,GetPacketHeader(packet)); }

   PacketHeader * get(PacketList &list);

   void get(PacketList &list,PacketList &dst);

   void complete(PacketList &list);
 };

} // namespace CCore

#endif


