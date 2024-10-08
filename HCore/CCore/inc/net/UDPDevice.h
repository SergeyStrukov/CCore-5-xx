/* UDPDevice.h */
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

#ifndef CCore_inc_net_UDPDevice_h
#define CCore_inc_net_UDPDevice_h

#include <CCore/inc/ObjHost.h>
#include <CCore/inc/StartStop.h>
#include <CCore/inc/Counters.h>
#include <CCore/inc/PacketSet.h>
#include <CCore/inc/AttachmentHost.h>

#include <CCore/inc/sys/SysNet.h>

#include <CCore/inc/net/PacketEndpointDevice.h>

namespace CCore {
namespace Net {

/* functions */

void UDPDeviceExitRunningAbort();

void UDPDeviceWaitAbort();

/* classes */

class UDPInboundPackets;

class UDPSocket;

template <class T> class UDPDevice;

struct UDPEndpointDeviceBase;

class UDPEndpointDevice;

struct UDPMultipointDeviceBase;

class UDPMultipointDevice;

/* class UDPInboundPackets */

class UDPInboundPackets : NoCopy
 {
   PacketSet<uint8> pset;
   TimeScope time_scope;

  public:

   // constructors

   explicit UDPInboundPackets(ulen max_packets);

   ~UDPInboundPackets();

   // methods

   Packet<uint8> getPacket();

   void start();

   TimeScope getScope() const { return time_scope; }

   bool nextScope() { return time_scope.nextScope_skip(); }
 };

/* class UDPSocket */

class UDPSocket : NoCopy
 {
   Sys::UDPSocket sys_sock;

  public:

   // constructors

   explicit UDPSocket(UDPort udport);

   ~UDPSocket();

   // methods

   bool outbound(UDPoint dst,PtrLen<const uint8> data) { return !sys_sock.outbound(dst,data); }

   Sys::WaitResult wait_inbound(TimeScope time_scope) { return sys_sock.wait_in(time_scope); }

   Sys::UDPSocket::InResult inbound(PtrLen<uint8> buf) { return sys_sock.inbound(buf); }
 };

/* class UDPDevice<T> */

template <class T>
class UDPDevice : NoCopy
 {
   using InboundProc = typename T::ProcType ;
   using Event = typename T::Event ;
   using StatInfo = typename T::StatInfo ;
   using Extra = typename T::Extra ;

  private:

   UDPort udport;

   UDPInboundPackets src;

   UDPSocket sock;

   Atomic stop_flag;

   AttachmentHost<InboundProc> host;

   Mutex mutex;

   StatInfo stat;
   bool running;

  private:

   using Hook = typename AttachmentHost<InboundProc>::Hook ;

   void count(Event ev)
    {
     Mutex::Lock lock(mutex);

     stat.count(ev);
    }

   void set(bool running_)
    {
     Mutex::Lock lock(mutex);

     running=running_;
    }

   void tick()
    {
     Hook hook(host);

     if( +hook ) hook->tick();
    }

   template <class Result>
   Event inbound_ok(Packet<uint8> packet,Result result)
    {
     Hook hook(host);

     if( +hook )
       {
        return extra.inbound(hook.getPtr(),packet,result.len,result.src);
       }
     else
       {
        packet.complete();

        return T::Event_noread;
       }
    }

  public:

   Extra extra;

   // constructors

   template <class ... SS>
   UDPDevice(UDPort udport_,ulen max_packets,SS ... ss)
    : udport(udport_),
      src(max_packets),
      sock(udport_),
      host("UDPDevice","UDPDevice.host"),
      mutex("UDPDevice.mutex"),
      running(false),
      extra(ss...)
    {
    }

   ~UDPDevice()
    {
     Mutex::Lock lock(mutex);

     if( running ) UDPDeviceExitRunningAbort();
    }

   // methods

   UDPort getPort() const { return udport; }

   void getStat(StatInfo &ret)
    {
     Mutex::Lock lock(mutex);

     ret=stat;
    }

   // object run

   void prepareRun() { stop_flag=0; }

   void objRun();

   void completeRun() noexcept { set(true); }

   void signalStop() noexcept { stop_flag=1; }

   void completeStop() noexcept { set(false); }

   // device

   PacketFormat getOutboundFormat() const
    {
     PacketFormat ret;

     ret.prefix=0;
     ret.max_data=MaxUDPDataLen;
     ret.suffix=0;

     return ret;
    }

   void outbound(UDPoint dst,Packet<uint8> packet)
    {
     PtrLen<uint8> data=packet.getRange();

     if( data.len<=MaxUDPDataLen && sock.outbound(dst,data) )
       count(T::Event_sent);
     else
       count(T::Event_nosent);

     packet.complete();
    }

   ulen getMaxInboundLen() const
    {
     return MaxUDPDataLen;
    }

   void attach(InboundProc *proc)
    {
     host.attach(proc);
    }

   void detach()
    {
     host.detach();
    }
 };

template <class T>
void UDPDevice<T>::objRun()
 {
  set(true);

  src.start();

  while( !stop_flag )
    switch( sock.wait_inbound(src.getScope()) )
      {
       case Sys::Wait_timeout :
        {
         if( src.nextScope() )
           {
            tick();

            count(T::Event_tick);
           }
         else
           {
            count(T::Event_extra_timeout);
           }
        }
       break;

       case Sys::Wait_error :
        {
         UDPDeviceWaitAbort();
        }
       break;

       default:
        {
         Packet<uint8> packet=src.getPacket();

         if( !packet )
           {
            count(T::Event_nopacket);

            auto result=sock.inbound(Nothing); // packet drop

            if( result.error ) count(T::Event_noread);
           }
         else
           {
            PtrLen<uint8> data=packet.getMaxRange();

            auto result=sock.inbound(data);

            if( !result.error )
              {
               count(inbound_ok(packet,result));
              }
            else
              {
               packet.complete();

               count(T::Event_noread);
              }
           }
        }
       break;
      }
 }

/* struct UDPEndpointDeviceBase */

struct UDPEndpointDeviceBase
 {
  using ProcType = PacketEndpointDevice::InboundProc ;

  enum Event
   {
    Event_read,
    Event_sent,
    Event_tick,

    Event_extra_timeout,

    Event_nosent,
    Event_nopacket,
    Event_noread,
    Event_nodst,

    EventLim
   };

  friend const char * GetTextDesc(Event ev);

  using StatInfo = Counters<Event,EventLim> ;

  struct Extra
   {
    UDPoint dst;

    explicit Extra(UDPoint dst_) : dst(dst_) {}

    Event inbound(ProcType *proc,Packet<uint8> packet,ulen len,UDPoint src)
     {
      if( src==dst )
        {
         PtrLen<uint8> data=packet.setDataLen(len);

         proc->inbound(packet,data);

         return Event_read;
        }
      else
        {
         packet.complete();

         return Event_nodst;
        }
     }
   };
 };

/* class UDPEndpointDevice */

class UDPEndpointDevice : public ObjBase , public PacketEndpointDevice , public UDPEndpointDeviceBase , public PortManager
 {
  private:

   UDPDevice<UDPEndpointDeviceBase> dev;

   using StartStopDev = StartStopObject< UDPDevice<UDPEndpointDeviceBase> > ;

  public:

   // constructors

   static constexpr ulen DefaultMaxPackets = 500 ;

   UDPEndpointDevice(UDPort udport,UDPoint dst,ulen max_packets=DefaultMaxPackets);

   virtual ~UDPEndpointDevice();

   // methods

   UDPort getPort() const { return dev.getPort(); }

   UDPoint getDst() const { return dev.extra.dst; }

   void getStat(StatInfo &ret) { dev.getStat(ret); }

   // PacketEndpointDevice

   virtual PacketFormat getOutboundFormat() const;

   virtual void outbound(Packet<uint8> packet);

   virtual ulen getMaxInboundLen() const;

   virtual void attach(InboundProc *proc);

   virtual void detach();

   // PortManager

   virtual XPoint getDevicePort() const;

   virtual XPoint getPort(XPoint point) const;

   virtual XPoint changePort(XPoint point,XPoint port) const;

   // start/stop

   class StartStop : public StartStopDev
    {
     public:

      template <class ... TT>
      StartStop(UDPEndpointDevice &obj,TT ... tt) : StartStopDev(obj.dev,tt...) {}

      ~StartStop() {}
    };
 };

/* struct UDPMultipointDeviceBase */

struct UDPMultipointDeviceBase
 {
  using ProcType = PacketMultipointDevice::InboundProc ;

  enum Event
   {
    Event_read,
    Event_sent,
    Event_tick,

    Event_extra_timeout,

    Event_nosent,
    Event_nopacket,
    Event_noread,

    EventLim
   };

  friend const char * GetTextDesc(Event ev);

  using StatInfo = Counters<Event,EventLim> ;

  struct Extra
   {
    Extra() {}

    Event inbound(ProcType *proc,Packet<uint8> packet,ulen len,UDPoint src)
     {
      PtrLen<uint8> data=packet.setDataLen(len);

      proc->inbound(src.get(),packet,data);

      return Event_read;
     }
   };
 };

/* class UDPMultipointDevice */

class UDPMultipointDevice : public ObjBase , public PacketMultipointDevice , public UDPMultipointDeviceBase , public PortManager
 {
   UDPDevice<UDPMultipointDeviceBase> dev;

   using StartStopDev = StartStopObject< UDPDevice<UDPMultipointDeviceBase> > ;

  public:

   // constructors

   static constexpr ulen DefaultMaxPackets = 500 ;

   explicit UDPMultipointDevice(UDPort udport,ulen max_packets=DefaultMaxPackets);

   virtual ~UDPMultipointDevice();

   // methods

   UDPort getPort() const { return dev.getPort(); }

   void getStat(StatInfo &ret) { dev.getStat(ret); }

   // PacketMultipointDevice

   virtual StrLen toText(XPoint point,PtrLen<char> buf) const;

   virtual PacketFormat getOutboundFormat() const;

   virtual void outbound(XPoint point,Packet<uint8> packet);

   virtual ulen getMaxInboundLen() const;

   virtual void attach(InboundProc *proc);

   virtual void detach();

   // PortManager

   virtual XPoint getDevicePort() const;

   virtual XPoint getPort(XPoint point) const;

   virtual XPoint changePort(XPoint point,XPoint port) const;

   // start/stop

   class StartStop : public StartStopDev
    {
     public:

      template <class ... TT>
      StartStop(UDPMultipointDevice &obj,TT ... tt) : StartStopDev(obj.dev,tt...) {}

      ~StartStop() {}
    };
 };

} // namespace Net
} // namespace CCore

#endif


