/* PrintPTPCon.h */
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

#ifndef CCore_inc_print_PrintPTPCon_h
#define CCore_inc_print_PrintPTPCon_h

#include <CCore/inc/net/PTPConDevice.h>

#include <CCore/inc/ReadConBase.h>
#include <CCore/inc/BlockFifo.h>

namespace CCore {

/* classes */

class PTPConOpenClose;

class ReadPTPCon;

class PrintPTPCon;

/* class PTPConOpenClose */

class PTPConOpenClose : NoCopy
 {
   ObjHook hook;

   Net::PTPCon::ClientDevice *con;

   Net::PTPCon::ClientDevice::OpenClose open_close;

   PacketFormat format;

  public:

   // constructors

   PTPConOpenClose(StrLen con_dev_name,StrLen name,const Net::PTPCon::Cfg &cfg=Net::PTPCon::Cfg());

   ~PTPConOpenClose();

   // read

   using InputProc = Net::PTPCon::ClientDevice::InputProc ;

   void start_read(InputProc *proc) { con->start_read(proc); }

   void stop_read() { con->stop_read(); }

   // write

   PacketFormat getWriteFormat() const { return format; }

   void write(Packet<uint8> packet) { con->write(packet); }
 };

/* class ReadPTPCon */

class ReadPTPCon : PTPConOpenClose::InputProc , public ReadConBase
 {
   static constexpr ulen FifoLen = 128 ;

   PTPConOpenClose &con_openclose;

   Mutex mutex;
   Event event;
   PacketSet<uint8> pset;
   MSec timeout;

   BlockFifoBuf<char,FifoLen> fifo;
   bool stop_flag = false ;

  private:

   // PTPConOpenClose::InputProc

   virtual void input(PacketBuf &pbuf,PtrLen<const uint8> str);

   virtual void stop();

   // ReadConBase

   ulen do_read(char *ptr,ulen len);

   virtual ulen read(char *ptr,ulen len,TimeScope time_scope);

  public:

   // constructors

   explicit ReadPTPCon(PTPConOpenClose &con_openclose,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   ~ReadPTPCon();

   // put, ignore errors

   void put(char ch) { put(Single(ch)); }

   void put(Symbol sym) { put(SymbolRange(sym)); }

   void put(const char *str,ulen len) { put(StrLen(str,len)); }

   void put(StrLen str);
 };

/* class PrintPTPCon */

class PrintPTPCon : public PrintBase
 {
   ObjHook hook;

   Net::PTPCon::ClientDevice *con;

   PacketFormat format;
   MSec timeout;
   bool enable_exceptions;

   PacketSet<uint8> pset;

   Packet<uint8> packet;

  private:

   virtual PtrLen<char> do_provide(ulen hint_len);

   virtual void do_flush(char *ptr,ulen len);

  public:

   explicit PrintPTPCon(StrLen con_dev_name,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   ~PrintPTPCon();

   void enableExceptions() { enable_exceptions=true; }

   void flush_and_wait();
 };

} // namespace CCore

#endif


