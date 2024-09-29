/* AbstFileToMem.h */
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

#ifndef CCore_inc_AbstFileToMem_h
#define CCore_inc_AbstFileToMem_h

#include <CCore/inc/ToMemBase.h>
#include <CCore/inc/Array.h>
#include <CCore/inc/AbstFileToRead.h>

namespace CCore {

/* classes */

class AbstFileToMem;

class PartAbstFileToMem;

/* class AbstFileToMem */

class AbstFileToMem : public ToMemBase
 {
  public:

   AbstFileToMem() noexcept {}

   AbstFileToMem(AbstFileToRead file,StrLen file_name,ulen max_len=MaxULen);

   ~AbstFileToMem() {}

   // std move

   AbstFileToMem(AbstFileToMem &&obj) noexcept = default ;

   AbstFileToMem & operator = (AbstFileToMem &&obj) noexcept = default ;

   // swap/move objects

   void objSwap(AbstFileToMem &obj) noexcept
    {
     Swap<ToMemBase>(*this,obj);
    }

   explicit AbstFileToMem(ToMoveCtor<AbstFileToMem> obj) noexcept
    : ToMemBase(obj.template cast<ToMemBase>())
    {}
 };

/* class PartAbstFileToMem */

class PartAbstFileToMem : NoCopy
 {
   AbstFileToRead file;
   SimpleArray<uint8> buf;
   FilePosType off;
   FilePosType file_len;

  public:

   static constexpr ulen DefaultBufLen = 64_KByte ;

   explicit PartAbstFileToMem(AbstFileToRead file,StrLen file_name,ulen buf_len=DefaultBufLen);

   ~PartAbstFileToMem();

   FilePosType getFileLen() const { return file_len; }

   FilePosType getCurPos() const { return off; }

   FilePosType getRestLen() const { return file_len-off; }

   // pump raw data

   bool more() const { return off<file_len; }

   PtrLen<const uint8> pump();
 };

} // namespace CCore

#endif


