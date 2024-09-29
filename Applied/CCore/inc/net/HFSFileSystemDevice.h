/* HFSFileSystemDevice.h */
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

#ifndef CCore_inc_net_HFSFileSystemDevice_h
#define CCore_inc_net_HFSFileSystemDevice_h

#include <CCore/inc/net/HFSClientDevice.h>

#include <CCore/inc/sys/SysAsyncFile.h>
#include <CCore/inc/sys/SysAsyncFileSystem.h>

namespace CCore {
namespace Net {
namespace HFS {

/* classes */

class FileSystemDevice;

/* class FileSystemDevice */

class FileSystemDevice : public ObjBase , public FunchorDock<FileSystemDevice> , public Sys::AsyncFileDevice , public Sys::AsyncFileSystemDevice
 {
   ObjHook hook;

   ClientDevice *dev;

  private:

   bool attach_state(ObjHook &hook,Packet<uint8,Sys::AsyncFileSystem::FileExt> packet);

   static void CompleteFile(Sys::AsyncFileSystem::FileExt *ext2,const FileId &file_id,FileLenType file_len);

   static void CompleteFile(Sys::AsyncFileSystem::FileExt *ext2,FileError fe);

   template <PTP::ExtType Ext>
   static FileError MakeError(Ext *ext,FileError fe);

   void complete_getFileType(PacketHeader *packet);

   void complete_getFileUpdateTime(PacketHeader *packet);

   void complete_getFileList(PacketHeader *packet);

   void complete_exec2(PacketHeader *packet);

   template <PTP::ExtType Ext>
   void complete_ext(PacketHeader *packet);

   template <PTP::ExtType Ext>
   PacketFunction function_complete_ext() { return functionOf< &FileSystemDevice::complete_ext<Ext> >(); }

   class FileState;

  public:

   explicit FileSystemDevice(StrLen hfs_dev_name);

   virtual ~FileSystemDevice();

   // Sys::AsyncFileDevice

   virtual Sys::AsyncFileState * createState(const ObjHook &hook) noexcept ;

   // Sys::AsyncFileSystemDevice

   virtual void getFileType(Packet<uint8,Sys::AsyncFileSystem::FileTypeExt> packet,StrLen path);

   virtual void getFileUpdateTime(Packet<uint8,Sys::AsyncFileSystem::CmpFileTimeExt> packet,StrLen path);

   virtual void getFileList(ObjHook &hook,Packet<uint8,Sys::AsyncFileSystem::FileExt> packet,StrLen path);

   virtual void createFile(Packet<uint8,Sys::AsyncFileSystem::ErrorExt> packet,StrLen file_name);

   virtual void deleteFile(Packet<uint8,Sys::AsyncFileSystem::ErrorExt> packet,StrLen file_name);

   virtual void createDir(Packet<uint8,Sys::AsyncFileSystem::ErrorExt> packet,StrLen dir_name);

   virtual void deleteDir(Packet<uint8,Sys::AsyncFileSystem::ErrorExt> packet,StrLen dir_name,bool recursive);

   virtual void rename(Packet<uint8,Sys::AsyncFileSystem::ErrorExt> packet,StrLen old_path,StrLen new_path,bool allow_overwrite);

   virtual void remove(Packet<uint8,Sys::AsyncFileSystem::ErrorExt> packet,StrLen path);

   virtual void exec(Packet<uint8,Sys::AsyncFileSystem::ErrorExt> packet,StrLen dir,StrLen program,StrLen arg);

   virtual void exec2(ObjHook &hook,Packet<uint8,Sys::AsyncFileSystem::FileExt> packet,StrLen dir,StrLen program);
 };

/* class FileSystemDevice::FileState */

class FileSystemDevice::FileState : public Sys::AsyncFileState , public FunchorDock<FileState>
 {
   ClientDevice *dev;
   FileId file_id;

   friend class FileSystemDevice;

  private:

   void complete_open(PacketHeader *packet);

   void complete_close(PacketHeader *packet);

   void complete_write(PacketHeader *packet);

   void complete_read(PacketHeader *packet);

   void complete_readbuf(PacketHeader *packet);

  public:

   FileState(const ObjHook &hook,ClientDevice *dev);

   virtual ~FileState();

   // Sys::AsyncFileState

   virtual void open(Packet<uint8,Sys::AsyncFile::OpenExt> packet,StrLen file_name,FileOpenFlags oflags);

   virtual void close(Packet<uint8,Sys::AsyncFile::CloseExt> packet,bool preserve_file);

   virtual void write(Packet<uint8,Sys::AsyncFile::WriteExt> packet,FilePosType off);

   virtual void read(Packet<uint8,Sys::AsyncFile::ReadExt> packet);

   virtual void read(Packet<uint8,Sys::AsyncFile::ReadBufExt> packet);
 };

} // namespace HFS
} // namespace Net
} // namespace CCore

#endif


