/* ScanAsyncFile.h */
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

#ifndef CCore_inc_scan_ScanAsyncFile_h
#define CCore_inc_scan_ScanAsyncFile_h

#include <CCore/inc/Scanf.h>

#include <CCore/inc/PacketSet.h>

#include <CCore/inc/sys/SysAsyncFile.h>

namespace CCore {

/* classes */

class PumpAsyncFileEngine;

class ScanAsyncFile;

/* class PumpAsyncFileEngine */

class PumpAsyncFileEngine : public Funchor_nocopy
 {
   Sys::AsyncFile file;

   bool is_opened = false ;

   PacketSet<uint8> pset;
   MSec timeout;
   MSec final_timeout;

   // read engine

   FilePosType file_pos;
   FilePosType file_len;
   ulen max_read_len;

   FilePosType remaining_len;

   PacketBuf pbuf;

   static constexpr ulen MaxSlots = 100 ;
   static constexpr ulen MaxReadLen = 64_KByte ;

   struct Slot : NoCopy
    {
     PacketBuf pbuf;
     FilePosType off;
     ulen len;
     const uint8 *data;
     bool done;
     FileError error;

     PtrLen<const uint8> getRange() const { return Range(data,len);}
    };

   Slot slots[MaxSlots];

   Mutex mutex;
   Sem sem;

   ulen done_ind;
   ulen done_count;
   ulen op_ind;
   ulen op_count;
   ulen free_ind;
   ulen free_count;

   FileError error;

  private:

   void init_slots();

   void clean_slots();

   static ulen NextInd(ulen ind) { return (ind<MaxSlots-1)? ind+1 : 0 ; }

   static ulen PrevInd(ulen ind) { return (ind>0)? ind-1 : (MaxSlots-1) ; }

   Slot * getFreeSlot();

   void backFreeSlot();

   void setError(FileError error);

   FileError getError();

   void complete(Slot *slot);

   void complete_read(PacketHeader *packet);

   PacketFunction function_complete_read() { return FunctionOf(this,&PumpAsyncFileEngine::complete_read); }

   bool add_read(TimeScope time_scope);

   void pump_read(TimeScope time_scope);

  private:

   void complete_open(PacketHeader *packet);

   PacketFunction function_complete_open() { return FunctionOf(this,&PumpAsyncFileEngine::complete_open); }

   void complete_close(PacketHeader *packet);

   PacketFunction function_complete_close() { return FunctionOf(this,&PumpAsyncFileEngine::complete_close); }

  public:

   // constructors

   explicit PumpAsyncFileEngine(MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   explicit PumpAsyncFileEngine(StrLen file_name,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   PumpAsyncFileEngine(StrLen dev_name,StrLen dev_file_name,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   ~PumpAsyncFileEngine();

   // methods

   bool isOpened() const { return is_opened; }

   void setFinalTimeout(MSec t) { final_timeout=t; }

   void open(StrLen file_name);

   void open(StrLen dev_name,StrLen dev_file_name);

   void close(FileMultiError &errout);

   void close();

   // pump raw data

   bool more() { return is_opened && remaining_len ; }

   PtrLen<const uint8> pump();
 };

/* class ScanAsyncFile */

class ScanAsyncFile : public ScanBase , public Funchor
 {
   PumpAsyncFileEngine engine;

  private:

   virtual PtrLen<const char> underflow();

  public:

   // constructors

   explicit ScanAsyncFile(MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   explicit ScanAsyncFile(StrLen file_name,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   ScanAsyncFile(StrLen dev_name,StrLen dev_file_name,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   ~ScanAsyncFile();

   // methods

   bool isOpened() const { return engine.isOpened(); }

   void setFinalTimeout(MSec t) { engine.setFinalTimeout(t); }

   void open(StrLen file_name);

   void open(StrLen dev_name,StrLen dev_file_name);

   void close(FileMultiError &errout);

   void close();
 };

} // namespace CCore

#endif


