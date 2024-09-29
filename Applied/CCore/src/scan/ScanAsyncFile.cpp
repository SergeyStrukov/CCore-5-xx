/* ScanAsyncFile.cpp */
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

#include <CCore/inc/scan/ScanAsyncFile.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class PumpAsyncFileEngine */

void PumpAsyncFileEngine::init_slots()
 {
  done_ind=0;
  done_count=0;
  op_ind=0;
  op_count=0;
  free_ind=0;
  free_count=MaxSlots;
 }

void PumpAsyncFileEngine::clean_slots()
 {
  pbuf.detach();

  while( done_count )
    {
     slots[done_ind].pbuf.detach();

     done_ind=NextInd(done_ind);
     done_count--;
    }

  while( op_count )
    {
     slots[op_ind].pbuf.detach();

     op_ind=NextInd(op_ind);
     op_count--;
    }
 }

auto PumpAsyncFileEngine::getFreeSlot() -> Slot *
 {
  Mutex::Lock lock(mutex);

  if( free_count==0 ) return 0;

  Slot *ret=slots+free_ind;

  free_ind=NextInd(free_ind);
  free_count--;
  op_count++;

  ret->done=false;

  return ret;
 }

void PumpAsyncFileEngine::backFreeSlot()
 {
  Mutex::Lock lock(mutex);

  free_ind=PrevInd(free_ind);
  free_count++;
  op_count--;
 }

void PumpAsyncFileEngine::setError(FileError error_)
 {
  Mutex::Lock lock(mutex);

  if( !error ) error=error_;
 }

FileError PumpAsyncFileEngine::getError()
 {
  Mutex::Lock lock(mutex);

  return error;
 }

void PumpAsyncFileEngine::complete(Slot *slot)
 {
  ulen ind=Dist(slots,slot);
  ulen delta=0;

  {
   Mutex::Lock lock(mutex);

   slot->done=true;

   if( ind==op_ind )
     {
      while( op_count && slots[op_ind].done )
        {
         op_ind=NextInd(op_ind);
         op_count--;
         done_count++;
         delta++;
        }
     }
  }

  sem.give_many(delta);
 }

void PumpAsyncFileEngine::complete_read(PacketHeader *packet_)
 {
  Packet<uint8,Slot *,Sys::AsyncFile::ReadBufExt> packet=packet_;

  auto *ext=packet.getExt();
  Slot *slot=*packet.getDeepExt<1>();

  FileError error=ext->error;

  slot->error=error;

  if( error )
    {
     setError(error);
    }
  else
    {
     packet.detach(slot->pbuf);

     slot->data=ext->data;
     slot->off=ext->off;
     slot->len=ext->len;
    }

  complete(slot);

  packet.popExt().popExt().complete();
 }

bool PumpAsyncFileEngine::add_read(TimeScope time_scope)
 {
  if( file_pos>=file_len ) return false;

  Slot *slot=getFreeSlot();

  if( !slot ) return false;

  Packet<uint8> packet=pset.try_get();
  bool ret=true;

  if( !packet )
    {
     packet=pset.get(time_scope);

     if( !packet )
       {
        backFreeSlot();

        return false;
       }

     ret=false;
    }

  ulen len=(ulen)Min<FilePosType>(file_len-file_pos,max_read_len);

  FilePosType off=file_pos;

  file_pos+=len;

  Packet<uint8,Slot *,Sys::AsyncFile::ReadBufExt> packet2=packet.pushExt<Slot *>(slot).pushExt<Sys::AsyncFile::ReadBufExt>(off,len);

  packet2.pushCompleteFunction(function_complete_read());

  file.read(packet2.forgetExt<1>());

  return ret;
 }

void PumpAsyncFileEngine::pump_read(TimeScope time_scope)
 {
  for(ulen cap=MaxSlots; cap && add_read(time_scope) ;cap--);
 }

void PumpAsyncFileEngine::complete_open(PacketHeader *packet_)
 {
  Packet<uint8,Sys::AsyncFile::OpenExt> packet=packet_;

  auto ext=packet.getExt();

  error=ext->error;

  if( !error )
    {
     is_opened=true;

     file_pos=0;
     file_len=ext->file_len;
     max_read_len=Min(MaxReadLen,ext->max_read_len);

     remaining_len=file_len;
    }

  packet.popExt().complete();
 }

void PumpAsyncFileEngine::complete_close(PacketHeader *packet_)
 {
  Packet<uint8,Sys::AsyncFile::CloseExt> packet=packet_;

  auto ext=packet.getExt();

  error=ext->error;

  packet.popExt().complete();
 }

PumpAsyncFileEngine::PumpAsyncFileEngine(MSec timeout_,ulen max_packets)
 : pset("PumpAsyncFileEngine.pset",max_packets),
   timeout(timeout_),
   final_timeout(3*timeout_),
   mutex("PumpAsyncFileEngine"),
   sem("PumpAsyncFileEngine")
 {
  init_slots();
 }

PumpAsyncFileEngine::PumpAsyncFileEngine(StrLen file_name,MSec timeout,ulen max_packets)
 : PumpAsyncFileEngine(timeout,max_packets)
 {
  open(file_name);
 }

PumpAsyncFileEngine::PumpAsyncFileEngine(StrLen dev_name,StrLen dev_file_name,MSec timeout,ulen max_packets)
 : PumpAsyncFileEngine(timeout,max_packets)
 {
  open(dev_name,dev_file_name);
 }

PumpAsyncFileEngine::~PumpAsyncFileEngine()
 {
  if( is_opened )
    {
     FileMultiError errout;

     close(errout);

     if( +errout )
       {
        Printf(NoException,"CCore::PumpAsyncFileEngine::~PumpAsyncFileEngine() : #;",errout);
       }
    }
 }

void PumpAsyncFileEngine::open(StrLen file_name)
 {
  if( is_opened )
    {
     Printf(Exception,"CCore::PumpAsyncFileEngine::open(...) : file is already opened");
    }

  TimeScope time_scope(timeout);

  Packet<uint8> packet=pset.get(time_scope);

  if( !packet )
    {
     Printf(Exception,"CCore::PumpAsyncFileEngine::open(...) : no packet");
    }

  Packet<uint8,Sys::AsyncFile::OpenExt> packet2=packet.pushExt<Sys::AsyncFile::OpenExt>();

  packet2.pushCompleteFunction(function_complete_open());

  file.open(packet2,file_name,Open_ToRead);

  pset.wait(time_scope);

  if( !is_opened )
    {
     file.closeState();

     Printf(Exception,"CCore::PumpAsyncFileEngine::open(#.q;) : #;",file_name,error);
    }

  init_slots();
 }

void PumpAsyncFileEngine::open(StrLen dev_name,StrLen dev_file_name)
 {
  if( is_opened )
    {
     Printf(Exception,"CCore::PumpAsyncFileEngine::open(...) : file is already opened");
    }

  TimeScope time_scope(timeout);

  Packet<uint8> packet=pset.get(time_scope);

  if( !packet )
    {
     Printf(Exception,"CCore::PumpAsyncFileEngine::open(...) : no packet");
    }

  Packet<uint8,Sys::AsyncFile::OpenExt> packet2=packet.pushExt<Sys::AsyncFile::OpenExt>();

  packet2.pushCompleteFunction(function_complete_open());

  file.open(packet2,dev_name,dev_file_name,Open_ToRead);

  pset.wait(time_scope);

  if( !is_opened )
    {
     file.closeState();

     Printf(Exception,"CCore::PumpAsyncFileEngine::open(#.q;,#.q;) : #;",dev_name,dev_file_name,error);
    }

  init_slots();
 }

void PumpAsyncFileEngine::close(FileMultiError &errout)
 {
  if( !is_opened )
    {
     errout.add(FileError_NoMethod);

     return;
    }

  is_opened=false;

  pset.wait(final_timeout);

  errout.add(error);

  clean_slots();

  Packet<uint8> packet=pset.get(timeout);

  if( !packet )
    {
     errout.add(FileError_SysOverload);

     file.closeState();

     return;
    }

  Packet<uint8,Sys::AsyncFile::CloseExt> packet2=packet.pushExt<Sys::AsyncFile::CloseExt>();

  packet2.pushCompleteFunction(function_complete_close());

  file.close(packet2);

  pset.wait(timeout);

  file.closeState();

  errout.add(error);
 }

void PumpAsyncFileEngine::close()
 {
  FileMultiError errout;

  close(errout);

  if( +errout )
    {
     Printf(Exception,"CCore::PumpAsyncFileEngine::close() : #;",errout);
    }
 }

PtrLen<const uint8> PumpAsyncFileEngine::pump()
 {
  if( !is_opened )
    {
     Printf(Exception,"CCore::PumpAsyncFileEngine::pump() : file is not opened");

     return Nothing;
    }

  if( !remaining_len ) return Nothing;

  TimeScope time_scope(timeout);

  pump_read(time_scope);

  if( sem.take(time_scope) )
    {
     Mutex::Lock lock(mutex);

     Slot *slot=slots+done_ind;

     done_ind=NextInd(done_ind);
     done_count--;
     free_count++;

     if( !slot->error )
       {
        pbuf=std::move(slot->pbuf);

        remaining_len-=slot->len;

        return slot->getRange();
       }
    }
  else
    {
     setError(FileError_ReadFault);
    }

  pset.cancel_and_wait();

  Printf(Exception,"CCore::PumpAsyncFileEngine::pump() : #;",getError());

  return Nothing;
 }

/* class ScanAsyncFile */

PtrLen<const char> ScanAsyncFile::underflow()
 {
  try
    {
     return Mutate<const char>(engine.pump());
    }
  catch(...)
    {
     fail();

     throw;
    }
 }

 // constructors

ScanAsyncFile::ScanAsyncFile(MSec timeout,ulen max_packets)
 : engine(timeout,max_packets)
 {
 }

ScanAsyncFile::ScanAsyncFile(StrLen file_name,MSec timeout,ulen max_packets)
 : engine(file_name,timeout,max_packets)
 {
  pump();
 }

ScanAsyncFile::ScanAsyncFile(StrLen dev_name,StrLen dev_file_name,MSec timeout,ulen max_packets)
 : engine(dev_name,dev_file_name,timeout,max_packets)
 {
  pump();
 }

ScanAsyncFile::~ScanAsyncFile()
 {
 }

 // methods

void ScanAsyncFile::open(StrLen file_name)
 {
  engine.open(file_name);

  reset();
  pump();
 }

void ScanAsyncFile::open(StrLen dev_name,StrLen dev_file_name)
 {
  engine.open(dev_name,dev_file_name);

  reset();
  pump();
 }

void ScanAsyncFile::close(FileMultiError &errout)
 {
  engine.close(errout);

  reset();
 }

void ScanAsyncFile::close()
 {
  FileMultiError errout;

  close(errout);

  if( +errout )
    {
     Printf(Exception,"CCore::ScanAsyncFile::close() : #;",errout);
    }
 }

} // namespace CCore


