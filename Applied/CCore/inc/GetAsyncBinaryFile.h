/* GetAsyncBinaryFile.h */
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

#include <CCore/inc/SaveLoad.h>

#include <CCore/inc/scan/ScanAsyncFile.h>

namespace CCore {

/* classes */

class GetAsyncBinaryFile;

/* class GetAsyncBinaryFile */

class GetAsyncBinaryFile : public NoCopyBase< GetDevBase<GetAsyncBinaryFile> >
 {
   PumpAsyncFileEngine engine;

   PtrLen<const uint8> cur;

  private:

   void reset() { cur=Null; }

   void underflow();

   bool underflow_eof();

  public:

   // constructors

   explicit GetAsyncBinaryFile(MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   explicit GetAsyncBinaryFile(StrLen file_name,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   GetAsyncBinaryFile(StrLen dev_name,StrLen dev_file_name,MSec timeout=DefaultTimeout,ulen max_packets=DefaultMaxPackets);

   ~GetAsyncBinaryFile();

   // methods

   bool isOpened() const { return engine.isOpened(); }

   void setFinalTimeout(MSec t) { engine.setFinalTimeout(t); }

   void open(StrLen file_name);

   void open(StrLen dev_name,StrLen dev_file_name);

   void close(FileMultiError &errout);

   void close();

   // pump raw data

   bool more() { return +cur || underflow_eof() ; }

   PtrLen<const uint8> pump() { return Replace_null(cur); }

   // get

   uint8 do_get()
    {
     if( !cur ) underflow();

     uint8 ret=*cur;

     ++cur;

     return ret;
    }

   void do_get(uint8 *ptr,ulen len);
 };

} // namespace CCore

