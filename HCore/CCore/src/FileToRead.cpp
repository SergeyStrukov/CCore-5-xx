/* FileToRead.cpp */
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

#include <CCore/inc/FileToRead.h>

#include <CCore/inc/RawFileToRead.h>
#include <CCore/inc/OptMember.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class DefAbstFileToRead::Base */

class DefAbstFileToRead::Base : public AbstFileToReadBase
 {
   OptMember<AltFileToRead> file;

  public:

   Base() {}

   ~Base() {}

   virtual void open(StrLen file_name,FileOpenFlags oflags)
    {
     if( is_opened )
       {
        Printf(Exception,"CCore::DefAbstFileToRead::Base::open(#.q;,#;) : file is already opened",file_name,oflags);
       }
     else
       {
        file.create(file_name,oflags);

        is_opened=true;

        file_len=file.getPtr_unsafe()->getLen();
       }
    }

   virtual void close() noexcept
    {
     if( is_opened )
       {
        reset();

        file.destroy();
       }
     else
       {
        Printf(NoException,"CCore::DefAbstFileToRead::Base::close(...) : file is not opened");
       }
    }

   virtual ulen read(FilePosType off,uint8 *buf,ulen len)
    {
     if( !is_opened )
       {
        Printf(Exception,"CCore::DefAbstFileToRead::Base::read(...) : file is not opened");
       }

     return file.getPtr_unsafe()->read(off,buf,len);
    }

   virtual void read_all(FilePosType off,uint8 *buf,ulen len)
    {
     if( !is_opened )
       {
        Printf(Exception,"CCore::DefAbstFileToRead::Base::read_all(...) : file is not opened");
       }

     file.getPtr_unsafe()->read_all(off,buf,len);
    }
 };

/* class DefAbstFileToRead */

DefAbstFileToRead::DefAbstFileToRead()
 : AbstFileToRead(new Base())
 {
 }

DefAbstFileToRead::~DefAbstFileToRead()
 {
 }

/* class VolumeAbstFileToRead::Base */

class VolumeAbstFileToRead::Base : public AbstFileToReadBase
 {
   Volume<AltFileToRead> vol;

   OptMember< VolumeFile<AltFileToRead> > file;

  public:

   Base(StrLen file_name,FileOpenFlags oflags) : vol(file_name,oflags) {}

   ~Base() {}

   virtual void open(StrLen file_name,FileOpenFlags oflags)
    {
     if( is_opened )
       {
        Printf(Exception,"CCore::VolumeAbstFileToRead::Base::open(#.q;,#;) : file is already opened",file_name,oflags);
       }
     else
       {
        file.create(vol,file_name);

        is_opened=true;

        file_len=file.getPtr_unsafe()->getLen();
       }
    }

   virtual void close() noexcept
    {
     if( is_opened )
       {
        reset();

        file.destroy();
       }
     else
       {
        Printf(NoException,"CCore::VolumeAbstFileToRead::Base::close(...) : file is not opened");
       }
    }

   virtual ulen read(FilePosType off,uint8 *buf,ulen len)
    {
     if( !is_opened )
       {
        Printf(Exception,"CCore::VolumeAbstFileToRead::Base::read(...) : file is not opened");
       }

     return file.getPtr_unsafe()->read(off,buf,len);
    }

   virtual void read_all(FilePosType off,uint8 *buf,ulen len)
    {
     if( !is_opened )
       {
        Printf(Exception,"CCore::VolumeAbstFileToRead::Base::read_all(...) : file is not opened");
       }

     file.getPtr_unsafe()->read_all(off,buf,len);
    }
 };

/* class VolumeAbstFileToRead */

VolumeAbstFileToRead::VolumeAbstFileToRead(StrLen file_name,FileOpenFlags oflags)
 : AbstFileToRead(new Base(file_name,oflags))
 {
 }

VolumeAbstFileToRead::~VolumeAbstFileToRead()
 {
 }

} // namespace CCore

