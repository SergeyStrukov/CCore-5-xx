/* GetBinaryFile.h */
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

#ifndef CCore_inc_GetBinaryFile_h
#define CCore_inc_GetBinaryFile_h

#include <CCore/inc/SaveLoad.h>
#include <CCore/inc/Array.h>

#include <CCore/inc/scan/ScanFile.h>

namespace CCore {

/* classes */

class GetBinaryFile;

/* class GetBinaryFile */

class GetBinaryFile : public NoCopyBase< GetDevBase<GetBinaryFile> >
 {
   static constexpr ulen BufLen = 64_KByte ;

   RawFileToScan file;
   SimpleArray<uint8> buf;

   PtrLen<const uint8> cur;

  private:

   void reset() { cur=Null; }

   void underflow();

   bool underflow_eof();

  public:

   // constructors

   GetBinaryFile() noexcept;

   explicit GetBinaryFile(StrLen file_name);

   ~GetBinaryFile();

   // methods

   bool isOpened() const { return file.isOpened(); }

   void open(StrLen file_name);

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

#endif

