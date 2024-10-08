/* PSec.h */
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

#ifndef CCore_inc_net_PSec_h
#define CCore_inc_net_PSec_h

#include <CCore/inc/net/PSecCore.h>
#include <CCore/inc/net/PacketEndpointDevice.h>

#include <CCore/inc/PacketSet.h>

#include <CCore/inc/TreeMap.h>
#include <CCore/inc/ObjHost.h>
#include <CCore/inc/AttachmentHost.h>
#include <CCore/inc/SaveLoad.h>
#include <CCore/inc/Counters.h>

#include <CCore/inc/task/TaskEvent.h>

namespace CCore {
namespace Net {
namespace PSec {

/* classes */

//enum ProcessorEvent;

struct EventRegType;

struct ProtoEvent;

class ProcessorStatInfo;

struct PSecHeader;

class PacketProcessor;

class EndpointDevice;

class MultipointDevice;

/* enum ProcessorEvent */

enum ProcessorEvent
 {
  ProcessorEvent_Rx,
  ProcessorEvent_RxBadLen,
  ProcessorEvent_RxBadPadLen,
  ProcessorEvent_RxReplay,
  ProcessorEvent_RxBadKeyIndex,
  ProcessorEvent_RxBadHash,
  ProcessorEvent_RxDone,

  ProcessorEvent_Tx,
  ProcessorEvent_TxBadFormat,
  ProcessorEvent_TxNoKey,
  ProcessorEvent_TxDone,

  ProcessorEvent_KeyNoPacket,
  ProcessorEvent_KeyBadFormat,

  ProcessorEvent_KeyAlert,
  ProcessorEvent_KeyReady,
  ProcessorEvent_KeyAck,
  ProcessorEvent_KeyStop,

  ProcessorEvent_Ping,
  ProcessorEvent_Pong,
  ProcessorEvent_Close,

  ProcessorEventLim
 };

const char * GetTextDesc(ProcessorEvent ev);

/* struct EventRegType */

struct EventRegType
 {
  static EventIdType Register(EventMetaInfo &info);
 };

/* struct ProtoEvent */

struct ProtoEvent
 {
  EventTimeType time;
  EventIdType id;

  uint8 ev;

  void init(EventTimeType time_,EventIdType id_,ProcessorEvent ev_)
   {
    time=time_;
    id=id_;

    ev=ev_;
   }

  static void Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc);
 };

/* class ProcessorStatInfo */

class ProcessorStatInfo : public Counters<ProcessorEvent,ProcessorEventLim>
 {
  public:

   void count(ProcessorEvent ev)
    {
     TaskEventHost.addProto<ProtoEvent>(ev);

     Counters<ProcessorEvent,ProcessorEventLim>::count(ev);
    }
 };

/* struct PSecHeader */

struct PSecHeader
 {
  SequenceNumber sequence_number = 0 ;
  KeyIndex key_index = 0 ;
  PadLen pad_len = 0 ;
  PacketType type = 0 ;

  // constructors

  PSecHeader() {}

  // save/load object

  using SaveLoadOrder = BeOrder ;

  template <template <class ...> class Ret,class T>
  static constexpr auto Fold()
   {
    return Ret(&T::sequence_number,&T::key_index,&T::pad_len,&T::type);
   }
 };

/* class PacketProcessor */

class PacketProcessor : public MemBase_nocopy
 {
   ProcessorCore core;

   ulen header_len;
   ulen min_out_len;
   ulen min_inp_len;

   SequenceNumber out_sequence_number = 0 ;

   AntiReplay anti_replay;

   KeepAlive keep_alive;

   ProcessorStatInfo stat;

  private:

   static constexpr ulen HeaderLen = SaveLenCounter<PSecHeader>::SaveLoadLen ;

   static constexpr ulen KeyIndexLen = SaveLenCounter<KeyIndex>::SaveLoadLen ;

  private:

   ulen outLen(ulen len) const
    {
     ulen l=len+core.getHLen();

     ulen s=RoundUpCount(l,DLen)+l;

     return header_len+RoundUp(s,core.getBLen());
    }

   ulen inpLen(ulen len) const
    {
     ulen s=RoundDown(len-header_len,core.getBLen());

     ulen l=s-RoundUpCount(s,DLen+1);

     return l-core.getHLen();
    }

   void consume(ControlResponse &resp,PacketType type,PtrLen<const uint8> data);

  public:

   PacketProcessor(const MasterKey &master_key,MSec keep_alive_timeout);

   ~PacketProcessor();

   ulen getOutDelta(ulen len) const;

   ulen getMaxInpLen(ulen len) const;

   struct InboundResult
    {
     PtrLen<const uint8> data;
     bool consumed;
     bool close;

     InboundResult(NothingType) : consumed(true),close(false) {}

     InboundResult(PtrLen<const uint8> data_) : data(data_),consumed(false),close(false) {}

     enum CloseType { Close };

     InboundResult(CloseType) : consumed(true),close(true) {}
    };

   InboundResult inbound(ControlResponse &resp,PtrLen<uint8> data);

   struct OutboundResult
    {
     ulen delta;
     bool ok;

     OutboundResult(NothingType) : delta(0),ok(false) {}

     OutboundResult(ulen delta_) : delta(delta_),ok(true) {}
    };

   OutboundResult outbound(PtrLen<uint8> data,ulen delta,PacketType type);

   void tick(ControlResponse &resp);

   void count(ProcessorEvent ev) { stat.count(ev); }

   void getStat(ProcessorStatInfo &ret) const { ret=stat; }

   bool response(const ControlResponse &resp,Packet<uint8> packet,PacketFormat format);

   class IOLen : NoCopy
    {
      ulen blen;
      ulen hlen;

      ulen header_len;
      ulen min_out_len;

     private:

      ulen outLen(ulen len) const
       {
        ulen l=len+hlen;

        ulen s=RoundUpCount(l,DLen)+l;

        return header_len+RoundUp(s,blen);
       }

      ulen inpLen(ulen len) const
       {
        ulen s=RoundDown(len-header_len,blen);

        ulen l=s-RoundUpCount(s,DLen+1);

        return l-hlen;
       }

     public:

      explicit IOLen(AlgoLen algo_len);

      ulen getOutDelta(ulen len) const;

      ulen getMaxInpLen(ulen len) const;
    };
 };

/* class EndpointDevice */

class EndpointDevice : public ObjBase , public PacketEndpointDevice
 {
   class Engine : public NoCopyBase<InboundProc,ConnectionProc>
    {
      PacketEndpointDevice *dev;

      AttachmentHost<InboundProc> host;

      using Hook = AttachmentHost<InboundProc>::Hook ;

      using ProcLocked = Locked<Mutex,PacketProcessor> ;

      ulen outbound_delta;
      PacketFormat outbound_format;
      ulen max_inbound_len;

      PacketSet<uint8> pset;

      mutable Mutex mutex;

      PacketProcessor proc;

     private:

      void call_connection_lost();

      void call_connection_close();

      void response(const ControlResponse &resp);

      // InboundProc

      virtual void inbound(Packet<uint8> packet,PtrLen<const uint8> data);

      virtual void tick();

      // ConnectionProc

      virtual void connection_lost();

      virtual void connection_close();

     public:

      Engine(PacketEndpointDevice *dev,const MasterKey &master_key,MSec keep_alive_timeout);

      ~Engine();

      void close();

      void getStat(ProcessorStatInfo &ret) const;

      PacketFormat getOutboundFormat() const;

      void outbound(Packet<uint8> packet,Packets type=Packet_Data);

      ulen getMaxInboundLen() const;

      void attach(InboundProc *proc);

      void detach();
    };

   ObjHook hook;

   Engine engine;

  public:

   EndpointDevice(StrLen ep_dev_name,const MasterKey &master_key,MSec keep_alive_timeout=Null);

   virtual ~EndpointDevice();

   void close() { engine.close(); } // send Close to the peer and assert connection_lost

   // stat

   using StatInfo = ProcessorStatInfo ;

   void getStat(StatInfo &ret) const { engine.getStat(ret); }

   // InterfaceHost

   virtual void requestInterface(InterfaceCaster &caster);

   // PacketEndpointDevice

   virtual PacketFormat getOutboundFormat() const;

   virtual void outbound(Packet<uint8> packet);

   virtual ulen getMaxInboundLen() const;

   virtual void attach(InboundProc *proc);

   virtual void detach();
 };

/* class MultipointDevice */

class MultipointDevice : public ObjBase , public PacketMultipointDevice , public EndpointManager
 {
   class Engine;

   struct PointNode : MemBase_nocopy
    {
     SLink<PointNode> link;

     XPoint point;

     explicit PointNode(XPoint point_) : point(point_) {}
    };

   class PointList;

   class Proc : NoCopy
    {
      ulen use_count = 0 ;
      bool opened = true ;

      using ProcLocked = Locked<Mutex,PacketProcessor> ;

      mutable Mutex mutex;

      OwnPtr<PointNode> node;
      OwnPtr<PacketProcessor> proc;
      ClientProfilePtr client_profile;

      OwnPtr<PointNode> replace_node;
      OwnPtr<PacketProcessor> replace_proc;
      ClientProfilePtr replace_client_profile;

     private:

      void connection_lost(XPoint point,Engine *engine);

      void connection_close(XPoint point,Engine *engine);

      void response(XPoint point,Engine *engine,const ControlResponse &resp);

      bool response(XPoint point,Engine *engine,const ControlResponse &resp,PacketList &list); // true to close

     public:

      Proc(XPoint point,const MasterKey &master_key,MSec keep_alive_timeout,ClientProfilePtr &&client_profile);

      ~Proc();

      void getStat(ProcessorStatInfo &ret) const;

      void replace(XPoint point,const MasterKey &master_key,MSec keep_alive_timeout,ClientProfilePtr &&client_profile);

      AbstractClientProfile * getClientProfile() const { return client_profile.getPtr(); }

      bool incUse(); // true to use

      bool decUse(); // true to del

      bool close(XPoint point,Engine *engine,PacketList &list,OwnPtr<PointNode> &is_lost); // true to del

      bool close(XPoint point,Engine *engine,PacketList &list,PointList &points); // true to del

      bool tick(XPoint point,Engine *engine,PacketList &list,OwnPtr<PointNode> &is_lost); // true to del

      bool tick(XPoint point,Engine *engine,PacketList &list,PointList &points); // true to del

      // async

      void inbound(XPoint point,Engine *engine,Packet<uint8> packet,PtrLen<const uint8> data);

      void outbound(XPoint point,Engine *engine,Packet<uint8> packet,Packets type);

      void outbound(XPoint point,Engine *engine,Packet<uint8> packet,Packets type,PacketList &list);
    };

   class Engine : public NoCopyBase<InboundProc,ConnectionProc>
    {
      PacketMultipointDevice *dev;

      ulen max_clients;
      MSec keep_alive_timeout;

      AttachmentHost<InboundProc> host;

      ulen outbound_delta;
      PacketFormat outbound_format;
      ulen max_inbound_len;

      PacketSet<uint8> pset;

      mutable Mutex mutex;

      RadixTreeMap<XPoint,Proc> map;

      friend class Proc;

     private:

      static ulen GetMaxInpLen(PtrLen<const AlgoLen> algo_lens,ulen len);

      static ulen GetOutDelta(PtrLen<const AlgoLen> algo_lens,ulen len);

      void call_connection_open(XPoint point);

      void call_connection_lost(XPoint point);

      void call_connection_close(XPoint point);

      void send(PacketList &list);

      void lost(PointList &points);

      // InboundProc

      virtual void inbound(XPoint point,Packet<uint8> packet,PtrLen<const uint8> data);

      virtual void tick();

      // ConnectionProc

      virtual void connection_open(XPoint point);

      virtual void connection_lost(XPoint point);

      virtual void connection_close(XPoint point);

     public:

      Engine(PacketMultipointDevice *dev,PtrLen<const AlgoLen> algo_lens,ulen max_clients,MSec keep_alive_timeout);

      ~Engine();

      void getStat(ProcessorStatInfo &ret) const;

      bool getStat(XPoint point,ProcessorStatInfo &ret) const;

      template <class Func>
      void processStat(Func &func) const
       {
        Mutex::Lock lock(mutex);

        map.applyIncr( [&func] (XPoint point,const Proc &proc) { ProcessorStatInfo stat; proc.getStat(stat); func(point,stat); } );
       }

      StrLen toText(XPoint point,PtrLen<char> buf) const;

      PacketFormat getOutboundFormat() const;

      void outbound(XPoint point,Packet<uint8> packet);

      ulen getMaxInboundLen() const;

      void attach(InboundProc *proc);

      void detach();

      OpenErrorCode open(XPoint point,const MasterKey &skey,ClientProfilePtr &&client_profile);

      bool close(XPoint point);

      ulen closeAll();

      AbstractClientProfile * getClientProfile(XPoint point) const;
    };

   using Hook = AttachmentHost<InboundProc>::Hook ;

   ObjHook hook;

   Engine engine;

  public:

   MultipointDevice(StrLen mp_dev_name,PtrLen<const AlgoLen> algo_lens,ulen max_clients,MSec keep_alive_timeout);

   virtual ~MultipointDevice();

   // stat

   using StatInfo = ProcessorStatInfo ;

   void getStat(StatInfo &ret) const { engine.getStat(ret); }

   bool getStat(XPoint point,StatInfo &ret) const { return engine.getStat(point,ret); }

   template <FuncInitArgType<XPoint,StatInfo &> FuncInit>
   void processStat(FuncInit func_init) const
    {
     FunctorTypeOf<FuncInit> func(func_init);

     engine.processStat(func);
    }

   // InterfaceHost

   virtual void requestInterface(InterfaceCaster &caster);

   // PacketMultipointDevice

   virtual StrLen toText(XPoint point,PtrLen<char> buf) const;

   virtual PacketFormat getOutboundFormat() const;

   virtual void outbound(XPoint point,Packet<uint8> packet);

   virtual ulen getMaxInboundLen() const;

   virtual void attach(InboundProc *proc);

   virtual void detach();

   // EndpointManager

   virtual OpenErrorCode open(XPoint point,const MasterKey &skey,ClientProfilePtr &&client_profile);

   virtual bool close(XPoint point);

   virtual ulen closeAll();

   virtual AbstractClientProfile * getClientProfile(XPoint point); // only inside inbound processing or processStat
 };

} // namespace PSec
} // namespace Net
} // namespace CCore

#endif



