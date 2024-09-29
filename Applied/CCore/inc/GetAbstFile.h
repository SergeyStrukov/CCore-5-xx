/* GetAbstFile.h */
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

#ifndef CCore_inc_GetAbstFile_h
#define CCore_inc_GetAbstFile_h

#include <CCore/inc/AbstFileToRead.h>
#include <CCore/inc/SaveLoad.h>
#include <CCore/inc/Array.h>

namespace CCore {

/* classes */

class GetAbstFile;

/* class GetAbstFile */

class GetAbstFile : public NoCopyBase< GetDevBase<GetAbstFile> >
 {
   static constexpr ulen BufLen = 64_KByte ;

   AbstFileToRead file;
   SimpleArray<uint8> buf;
   FilePosType off = 0 ;

   PtrLen<const uint8> cur;

  private:

   void underflow();

   bool underflow_eof();

  public:

   // constructors

   GetAbstFile(AbstFileToRead file,StrLen file_name);

   ~GetAbstFile();

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



