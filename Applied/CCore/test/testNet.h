/* testNet.h */
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

#ifndef CCore_test_testNet_h
#define CCore_test_testNet_h

#include <CCore/test/testRun.h>

#include <CCore/inc/Print.h>
#include <CCore/inc/ObjHost.h>
#include <CCore/inc/PacketSet.h>
#include <CCore/inc/Counters.h>
#include <CCore/inc/Timer.h>
#include <CCore/inc/StartStop.h>
#include <CCore/inc/AsyncFileToMem.h>
#include <CCore/inc/CmdInput.h>
#include <CCore/inc/String.h>

#include <CCore/inc/net/CheckedData.h>
#include <CCore/inc/net/PacketEndpointDevice.h>

#include <CCore/inc/net/PTPEchoTest.h>
#include <CCore/inc/net/PTPClientDevice.h>

#include <CCore/inc/net/PTPConDevice.h>
#include <CCore/inc/net/HFSFileSystemDevice.h>

namespace App {

/* ShowStat() */

template <class T>
void ShowStat(T &dev,StrLen title)
 {
  typename T::StatInfo info;

  dev.getStat(info);

  Printf(Con,"#;\n\n#20;\n\n",Title(title),info);
 }

template <class R>
void ShowRange(StrLen title,R r)
 {
  Printf(Con,"#;\n\n",Title(title));

  for(; +r ;++r) Printf(Con,"#;\n",*r);

  Printf(Con,"\n");
 }

/* classes */

class PacketSource;

class PacketTask;

class PacketEchoTest;

class PTPEchoTest;

class PacketRead;

class PTPServerTest;

/* class PacketSource */

class PacketSource : public FunchorDock_nocopy<PacketSource>
 {
  public:

   enum Event
    {
     Event_nofill,
     Event_broken,

     EventLim
    };

   friend const char * GetTextDesc(Event ev);

   using StatInfo = Counters<Event,EventLim> ;

  private:

   ulen max_len;
   PacketSet<uint8> pset;
   Atomic run_flag;

   Mutex mutex;

   Net::CheckedData cdata;
   StatInfo stat;

  private:

   void count(Event ev);

  public:

   PacketSource(ulen max_packets,ulen max_len);

   ~PacketSource();

   void getStat(StatInfo &ret);

   bool getRunFlag() { return run_flag; }

   void stop() { run_flag=0; }

   Function<void (void)> function_stop() { return functionOf<&PacketSource::stop>(); }

   Packet<uint8> get();

   ulen getDataLen();

   void fill(PtrLen<uint8> data);

   void check(PtrLen<const uint8> data);
 };

/* class PacketTask */

class PacketTask : public FunchorDock_nocopy<PacketTask> , public Net::PacketEndpointDevice::InboundProc , public Net::PacketEndpointDevice::ConnectionProc
 {
  public:

   enum Event
    {
     Event_nopacket,
     Event_outbound,
     Event_badformat,

     Event_inbound,
     Event_tick,

     Event_lost,
     Event_close,

     EventLim
    };

   friend const char * GetTextDesc(Event ev);

   using StatInfo = Counters<Event,EventLim> ;

  private:

   PacketSource &src;

   ObjHook hook;

   Net::PacketEndpointDevice *ep;

   Mutex mutex;

   StatInfo stat;

  private:

   void count(Event ev);

   // Net::PacketEndpointDevice::InboundProc

   virtual void inbound(Packet<uint8> packet,PtrLen<const uint8> data);

   virtual void tick();

   // Net::PacketEndpointDevice::ConnectionProc

   virtual void connection_lost();

   virtual void connection_close();

  private:

   void run();

  public:

   PacketTask(PacketSource &src,StrLen ep_dev_name);

   ~PacketTask();

   void getStat(StatInfo &ret);

   Function<void (void)> function_run() { return functionOf<&PacketTask::run>(); }
 };

/* class PacketEchoTest */

class PacketEchoTest : NoCopy , public Net::PacketEndpointDevice::InboundProc , public Net::PacketEndpointDevice::ConnectionProc
 {
  public:

   enum Event
    {
     Event_inbound,
     Event_outbound,

     Event_broken,
     Event_nosem,

     Event_lost,
     Event_close,

     EventLim
    };

   using StatInfo = Counters<Event,EventLim> ;

   friend const char * GetTextDesc(Event ev);

  private:

   ObjHook hook;

   Net::PacketEndpointDevice *ep;

   PacketFormat format;

   Net::CheckedData cdata;

   Sem sem;

   Mutex mutex;

   StatInfo stat;

   MSecTimer timer;

   ulen packet_count;
   uint64 total_data;
   MSecTimer::ValueType run_time;

  private:

   void traffic_init(ulen count);

   void traffic(ulen len);

   void traffic_exit();

   void show_traffic();

   void count(Event ev);

   void showStat();

   void send();

   // Net::PacketEndpointDevice::InboundProc

   virtual void inbound(Packet<uint8> packet,PtrLen<const uint8> data);

   virtual void tick();

   // Net::PacketEndpointDevice::ConnectionProc

   virtual void connection_lost();

   virtual void connection_close();

  public:

   explicit PacketEchoTest(StrLen ep_dev_name);

   ~PacketEchoTest();

   void getStat(StatInfo &ret);

   void run(ulen forward,ulen cnt);
 };

/* class PTPEchoTest */

class PTPEchoTest : public FunchorDock_nocopy<PTPEchoTest>
 {
  public:

   enum Event
    {
     Event_done,
     Event_nodata,
     Event_baddata,
     Event_failed,
     Event_datalen,
     Event_nopacket,
     Event_nomem,
     Event_timeout,

     EventLim
    };

   friend const char * GetTextDesc(Event ev);

   using StatInfo = Counters<Event,EventLim> ;

  private:

   ulen burst_count;

   ObjHook hook;

   Net::PTP::ClientDevice *ptp;

   Atomic run_flag;

   Net::CheckedData cdata;

   Mutex mutex;

   StatInfo stat;

  private:

   void count(Event ev);

   void do_complete(Net::PTP::EchoTest::Ext *ext);

   void complete(PacketHeader *packet_);

   // object run

   void prepareRun() { run_flag=1; }

   void objRun();

   void completeRun() noexcept {}

   void signalStop() noexcept { run_flag=0; }

   void completeStop() noexcept {}

  public:

   explicit PTPEchoTest(StrLen ptp_dev_name,ulen burst_count=10);

   ~PTPEchoTest();

   void getStat(StatInfo &ret);

   friend class StartStopObject<PTPEchoTest>;

   using StartStop = StartStopObject<PTPEchoTest> ;
 };

/* class PacketRead */

class PacketRead : public FunchorDock_nocopy<PacketRead>
 {
   Atomic ecount;
   FileError error;

   FilePosType file_len;

   struct Frame : MemBase_nocopy
    {
     DLink<Frame> link;

     // data

     FilePosType off;
     ulen len;

     PtrLen<const uint8> data;
     PacketBuf pbuf;

     ClockDiffTimer::ValueType time;

     // methods

     void copyTo(uint8 *base)
      {
       data.copyTo(base+(ulen)off);
      }

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"[#; ,+ #; ) : #;",off,len,time);
      }
    };

   class FrameList
    {
      Mutex mutex;

      DLink<Frame>::CircularAlgo<&Frame::link>::Top top;
      ClockDiffTimer timer;

     public:

      // constructors

      FrameList() {}

      ~FrameList();

      // methods

      Frame * alloc();

      void copyTo(uint8 *base);

      // print object

      void print(PrinterType auto &out) const
       {
        for(auto cur=top.start(); +cur ;++cur) Printf(out,"#;\n",*cur);
       }
    };

   FrameList list;

  private:

   void set(FileError fe)
    {
     if( (ecount++)==0 ) error=fe;
    }

   void complete(PacketHeader *packet);

  public:

   // constructors

   explicit PacketRead(StrLen file_name);

   ~PacketRead();

   // methods

   void guard();

   void map(DynArray<uint8> &ret);

   // print object

   void print(PrinterType auto &out) const
    {
     Putobj(out,list);
    }
 };

/* class PTPServerTest */

class PTPServerTest : public NoCopyBase<CmdInput::Target>
 {
   Net::HFS::ClientDevice hfs;

   ObjMaster hfs_master;

   Net::HFS::FileSystemDevice host;

   ObjMaster host_master;

   Net::PTPCon::ClientDevice ptpcon;

   ObjMaster ptpcon_master;

   Net::PTPCon::Cfg ptpcon_cfg;

   PTPConOpenClose ptpcon_open_close;

   PrintPTPCon con;

   CmdInputCon<ReadPTPCon> cmd_input;
   bool run_flag;

   AsyncFileSystem fs;

   RunTask run_task;

  private:

   class Report;
   class Split;

   static void PrintFile(unsigned ind);

   void cmd_empty(StrLen arg);
   void cmd_exit(StrLen arg);
   void cmd_test1(StrLen arg);
   void cmd_test2(StrLen arg);
   void cmd_test3(StrLen arg);
   void cmd_test4(StrLen arg);

   void cmd_list(StrLen arg);
   void cmd_type(StrLen arg);
   void cmd_show(StrLen arg);

   void cmd_createFile(StrLen arg);
   void cmd_deleteFile(StrLen arg);
   void cmd_createDir(StrLen arg);
   void cmd_deleteDir(StrLen arg);
   void cmd_deleteDirRecursive(StrLen arg);

   void cmd_rename(StrLen arg);
   void cmd_renameOver(StrLen arg);
   void cmd_remove(StrLen arg);
   void cmd_exec(StrLen arg);
   void cmd_exec2(StrLen arg);

   virtual void buildCmdList(CmdInput &input);

  public:

   PTPServerTest();

   ~PTPServerTest();

   void run();
 };

} // namespace App

#endif


