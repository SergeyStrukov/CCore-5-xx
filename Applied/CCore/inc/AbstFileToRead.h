/* AbstFileToRead.h */
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

#ifndef CCore_inc_AbstFileToRead_h
#define CCore_inc_AbstFileToRead_h

#include <CCore/inc/GenFile.h>
#include <CCore/inc/RefObjectBase.h>

namespace CCore {

/* classes */

class AbstractFileToRead;

/* class AbstractFileToRead */

class AbstractFileToRead : NoCopy
 {
  protected:

   bool is_opened = false ;
   FilePosType file_len = 0 ;

  protected:

   void reset()
    {
     is_opened=false;
     file_len=0;
    }

  public:

   virtual void open(StrLen file_name,FileOpenFlags oflags=Open_Read)=0;

   virtual void close() noexcept =0;

   bool isOpened() const { return is_opened; }

   FilePosType getLen() const { return file_len; }

   virtual ulen read(FilePosType off,uint8 *buf,ulen len)=0;

   virtual void read_all(FilePosType off,uint8 *buf,ulen len)=0;
 };

/* type AbstFileToReadBase */

using AbstFileToReadBase = RefObjectBase<AbstractFileToRead> ;

/* GetNullAbstFileToReadPtr() */

AbstFileToReadBase * GetNullAbstFileToReadPtr();

/* type AbstFileToRead */

using AbstFileToRead = RefObjectHook<AbstFileToReadBase,AbstractFileToRead,GetNullAbstFileToReadPtr> ;

} // namespace CCore

#endif

