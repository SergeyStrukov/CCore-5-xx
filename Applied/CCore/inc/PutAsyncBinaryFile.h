/* PutAsyncBinaryFile.h */
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

#ifndef CCore_inc_PutAsyncBinaryFile_h
#define CCore_inc_PutAsyncBinaryFile_h

#include <CCore/inc/SaveLoad.h>

#include <CCore/inc/AsyncFile.h>

namespace CCore {

/* classes */

class PutAsyncBinaryFile;

/* class PutAsyncBinaryFile */

class PutAsyncBinaryFile : public NoCopyBase< PutDevBase<PutAsyncBinaryFile> >
 {
   AsyncFile file;

   Packet<uint8> packet;
   FilePosType file_pos;

   PtrLen<uint8> out;
   ulen buf_len;

  private:

   void provide();

  public:

   // constructors

   explicit PutAsyncBinaryFile(MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   explicit PutAsyncBinaryFile(StrLen file_name,FileOpenFlags oflags=Open_ToWrite,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   PutAsyncBinaryFile(StrLen dev_name,StrLen dev_file_name,FileOpenFlags oflags=Open_ToWrite,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   ~PutAsyncBinaryFile();

   // methods

   bool isOpened() const { return file.isOpened(); }

   void setFinalTimeout(MSec t) { file.setFinalTimeout(t); }

   void open(StrLen file_name,FileOpenFlags oflags=Open_ToWrite);

   void open(StrLen dev_name,StrLen dev_file_name,FileOpenFlags oflags=Open_ToWrite);

   void close(FileMultiError &errout);

   void close();

   void preserveFile() { file.preserveFile(); }

   void setLargeWriteLen(ulen large_write_len) { file.setLargeWriteLen(large_write_len); }

   void wait() { file.wait(); }

   void wait_final() { file.wait_final(); }

   void wait(MSec timeout) { file.wait(timeout); }

   void cancel_and_wait() { file.cancel_and_wait(); }

   // put

   void do_put(uint8 value)
    {
     if( !out ) provide();

     *out=value;

     ++out;
    }

   void do_put(const uint8 *ptr,ulen len);

   void flush();
 };

} // namespace CCore

#endif


