/* AbstFileToRead.cpp */
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

#include <CCore/inc/AbstFileToRead.h>

#include <CCore/inc/Exception.h>

namespace CCore {

namespace Private_AbstFileToRead {

class NullAbstFileToRead : public AbstFileToReadBase
 {
  public:

   NullAbstFileToRead() {}

   ~NullAbstFileToRead() {}

   virtual void open(StrLen,FileOpenFlags)
    {
     Printf(Exception,"CCore::NullAbstFileToRead::open(...) : no such file");
    }

   virtual void close() noexcept
    {
     Printf(NoException,"CCore::NullAbstFileToRead::close() : file is not opened");
    }

   virtual ulen read(FilePosType,uint8 *,ulen)
    {
     Printf(Exception,"CCore::NullAbstFileToRead::read(...) : file is not opened");

     return 0;
    }

   virtual void read_all(FilePosType,uint8 *,ulen)
    {
     Printf(Exception,"CCore::NullAbstFileToRead::read_all(...) : file is not opened");
    }
 };

NullAbstFileToRead Object CCORE_INITPRI_3 ;

} // namespace Private_AbstFileToRead

using namespace Private_AbstFileToRead;

/* GetNullAbstFileToReadPtr() */

AbstFileToReadBase * GetNullAbstFileToReadPtr() { return &Object; }

} // namespace CCore

