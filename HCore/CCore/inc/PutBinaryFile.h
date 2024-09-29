/* PutBinaryFile.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_PutBinaryFile_h
#define CCore_inc_PutBinaryFile_h

#include <CCore/inc/SaveLoad.h>
#include <CCore/inc/Array.h>

#include <CCore/inc/print/PrintFile.h>

namespace CCore {

/* classes */

class PutBinaryFile;

/* class BinaryFile */

class PutBinaryFile : public NoCopyBase< PutDevBase<PutBinaryFile> >
 {
   static constexpr ulen BufLen = 64_KByte ;

   RawFileToPrint file;
   SimpleArray<uint8> buf;
   bool no_flush_exception = false ;
   FileError flush_error;

   PtrLen<uint8> out;
   bool has_data = false ;

  private:

   void provide();

  public:

   // constructors

   PutBinaryFile() noexcept;

   explicit PutBinaryFile(StrLen file_name,FileOpenFlags oflags=Open_ToWrite);

   ~PutBinaryFile();

   // methods

   bool isOpened() const { return file.isOpened(); }

   void open(StrLen file_name,FileOpenFlags oflags=Open_ToWrite);

   void disableExceptions();

   void close(FileMultiError &errout);

   void close();

   void preserveFile() { file.preserveFile(); }

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


