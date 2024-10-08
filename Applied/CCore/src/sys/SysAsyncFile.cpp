/* SysAsyncFile.cpp */
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

#include <CCore/inc/sys/SysAsyncFile.h>

#include <CCore/inc/Path.h>

namespace CCore {
namespace Sys {

/* struct AsyncFile */

void AsyncFile::open(Packet<uint8,OpenExt> packet,StrLen file_name,FileOpenFlags oflags_)
 {
  SplitDevName split(file_name);

  if( !split )
    {
     file_state=0;
     oflags=oflags_;

     return CompletePacket(packet,FileError_BadName);
    }

  open(packet,split.dev_name,split.dev_path,oflags_);
 }

void AsyncFile::open(Packet<uint8,OpenExt> packet,StrLen dev_name,StrLen dev_file_name,FileOpenFlags oflags_)
 {
  file_state=0;
  oflags=oflags_;

  ObjHook hook(JustTry,dev_name);

  AsyncFileDevice *dev=hook.cast<AsyncFileDevice>();

  if( !dev ) return CompletePacket(packet,FileError_NoDevice);

  file_state=dev->createState(hook);

  if( !file_state ) return CompletePacket(packet,FileError_SysOverload);

  auto ext=packet.getExt();

  ext->write_format=file_state->getWriteFormat();
  ext->max_read_len=file_state->getMaxReadLen();

  file_state->open(packet,dev_file_name,oflags_);
 }

void AsyncFile::close(Packet<uint8,CloseExt> packet,bool preserve_file)
 {
  file_state->close(packet,preserve_file);
 }

void AsyncFile::closeState()
 {
  delete Replace_null(file_state);
 }

void AsyncFile::write(Packet<uint8,WriteExt> packet,FilePosType off)
 {
  if( oflags&Open_Write )
    {
     file_state->write(packet,off);
    }
  else
    {
     CompletePacket(packet,FileError_NoMethod);
    }
 }

void AsyncFile::read(Packet<uint8,ReadExt> packet)
 {
  if( oflags&Open_Read )
    {
     file_state->read(packet);
    }
  else
    {
     CompletePacket(packet,FileError_NoMethod);
    }
 }

void AsyncFile::read(Packet<uint8,ReadBufExt> packet)
 {
  if( oflags&Open_Read )
    {
     file_state->read(packet);
    }
  else
    {
     CompletePacket(packet,FileError_NoMethod);
    }
 }

/* struct AsyncFileDevice */

const Unid AsyncFileDevice::TypeUnid={{0x10B0'F786,0x6E64'273A,0x05CB'649B,0x7877'706D,0x4020'9E95,0xA71A'E25B,0x69DE'F63E,0x631D'2801}};

} // namespace Sys
} // namespace CCore


