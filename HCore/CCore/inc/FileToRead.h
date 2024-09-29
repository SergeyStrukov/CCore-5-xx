/* FileToRead.h */
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

#ifndef CCore_inc_FileToRead_h
#define CCore_inc_FileToRead_h

#include <CCore/inc/AbstFileToRead.h>
#include <CCore/inc/Volume.h>

namespace CCore {

/* classes */

class DefAbstFileToRead;

class VolumeAbstFileToRead;

/* class DefAbstFileToRead */

class DefAbstFileToRead : public AbstFileToRead
 {
   class Base;

  public:

   DefAbstFileToRead();

   ~DefAbstFileToRead();
 };

/* class VolumeAbstFileToRead */

class VolumeAbstFileToRead : public AbstFileToRead
 {
   class Base;

  public:

   explicit VolumeAbstFileToRead(StrLen file_name,FileOpenFlags oflags=Open_Read);

   ~VolumeAbstFileToRead();
 };

} // namespace CCore

#endif

