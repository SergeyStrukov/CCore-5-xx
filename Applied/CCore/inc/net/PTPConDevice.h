/* PTPConDevice.h */
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

#ifndef CCore_inc_net_PTPConDevice_h
#define CCore_inc_net_PTPConDevice_h

#include <CCore/inc/net/PTPSupport.h>

#include <CCore/inc/net/PTPConBase.h>

#include <CCore/inc/CancelPacketList.h>

namespace CCore {
namespace Net {
namespace PTPCon {

/* classes */

struct Cfg;

struct OpenExt;

struct ReadExt;

struct WriteExt;

struct CloseExt;

class ClientDevice;

/* struct Cfg */

struct Cfg
 {
  uint32 write_timeout_msec;
  uint32 read_timeout_msec;
  TriggerMask trigger_mask;
  MSec timeout;

  // defaults

  void setDefaultTimeouts()
   {
    write_timeout_msec=1'000; //  1 sec
    read_timeout_msec=10'000; // 10 sec
   }

  // constructors

  explicit Cfg(MSec timeout_=DefaultTimeout)
   : trigger_mask(TriggerDefault),
     timeout(timeout_)
   {
    setDefaultTimeouts();
   }

  explicit Cfg(const char *trigger,MSec timeout_=DefaultTimeout)
   : trigger_mask(trigger),
     timeout(timeout_)
   {
    setDefaultTimeouts();
   }

  Cfg(Trigger trigger,MSec timeout_=DefaultTimeout)
   : trigger_mask(trigger),
     timeout(timeout_)
   {
    setDefaultTimeouts();
   }
 };

/* struct OpenExt */

struct OpenExt : ExtBase<ServiceId,FunctionId_Open>
 {
  using InputType = OpenInput ;
  using OutputType = OpenOutput ;

  ConId con_id;

  void done(const OutputType &output)
   {
    con_id=output.con_id;
   }
 };

/* struct ReadExt */

struct ReadExt : ExtBase<ServiceId,FunctionId_Read,MovePacketBuf>
 {
  using InputType = ReadInput ;
  using OutputType = Tailed<ReadOutput> ;

  PtrLen<const uint8> data;

  void done(const OutputType &output)
   {
    data=output.tail;
   }
 };

/* struct WriteExt */

struct WriteExt : ExtBase<ServiceId,FunctionId_Write>
 {
  using InputType = WriteInput ;
  using OutputType = WriteOutput ;

  void done(const OutputType &)
   {
   }
 };

/* struct CloseExt */

struct CloseExt : ExtBase<ServiceId,FunctionId_Close>
 {
  using InputType = CloseInput ;
  using OutputType = CloseOutput ;

  void done(const OutputType &)
   {
   }
 };

/* class ClientDevice */

class ClientDevice : public ObjBase , public FunchorDock<ClientDevice>
 {
   ObjHook hook;

   PTP::ClientDevice *ptp;
   PacketSet<uint8> pset;

   PacketFormat open_format;
   PacketFormat write_format;

   PacketList list;
   CancelPacketList canlist;

   Atomic write_error_flag;

   Mutex mutex;

   ConId con_id;
   uint32 write_number;
   ReadInput read_input;
   bool is_opened;

   ulen list_count;
   ulen packet_count;

  private:

   // open/close

   struct OpenResult : TransStatus , FunchorDock_nocopy<OpenResult>
    {
     ConId con_id;

     void complete(PacketHeader *packet);
    };

   struct CloseResult : TransStatus , FunchorDock_nocopy<CloseResult>
    {
     void complete(PacketHeader *packet);
    };

   void do_open(const ConId &con_id);

   bool do_close(ConId &con_id);

   void open(PacketSet<uint8> &pset,StrLen name,const Cfg &cfg);

   void close(PacketSet<uint8> &pset,MSec timeout);

   // write

   void write_error() { write_error_flag=1; }

   void push_write();

   void complete_write(PacketHeader *packet);

   void start_write(Packet<uint8> packet);

   void start_write();

   // read

   bool getReadInput(ReadInput &input);

   void complete_read(PacketHeader *packet);

  public:

   static const Unid TypeUnid;

   // constructors

   explicit ClientDevice(StrLen ptp_dev_name);

   virtual ~ClientDevice();

   // write

   bool getWriteErrorFlag() const { return write_error_flag; }

   PacketFormat getWriteFormat() const { return write_format; }

   void write(Packet<uint8> packet);

   // read

   struct InputProc
    {
     virtual void input(PacketBuf &pbuf,PtrLen<const uint8> str)=0;

     virtual void stop()=0;
    };

   void start_read(InputProc *proc);

   void stop_read();

   // open/close

   class OpenClose : NoCopy
    {
      ClientDevice &obj;
      PacketSet<uint8> pset;
      MSec timeout;

     public:

      OpenClose(ClientDevice &obj_,StrLen name,const Cfg &cfg=Cfg())
       : obj(obj_),
         pset("PTPCon::ClientDevice::OpenClose"),
         timeout(cfg.timeout)
       {
        obj_.open(pset,name,cfg);
       }

      ~OpenClose()
       {
        obj.stop_read();
        obj.close(pset,timeout);
       }
    };
 };

} // namespace PTPCon
} // namespace Net
} // namespace CCore

#endif


