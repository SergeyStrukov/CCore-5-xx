/* FileBoss.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_FileBoss_h
#define CCore_inc_video_FileBoss_h

#include <CCore/inc/GenFile.h>
#include <CCore/inc/RefObjectBase.h>

namespace CCore {
namespace Video {

/* classes */

struct AbstractFileBoss;

/* struct AbstractFileBoss */

struct AbstractFileBoss
 {
  virtual FileType getFileType(StrLen path)=0;

  virtual StrLen pathOf(StrLen path,char buf[MaxPathLen+1])=0;

  virtual void enumDir(StrLen dir_name,Function<void (StrLen name,FileType type)> func)=0;

  virtual void createDir(StrLen dir_name)=0;

  virtual void deleteDir(StrLen dir_name)=0;

  virtual StrLen getHitDirFile() const =0; // Null OR "/file"
 };

/* type FileBossBase */

using FileBossBase = RefObjectBase<AbstractFileBoss> ;

/* GetNullFileBossPtr() */

FileBossBase * GetNullFileBossPtr();

/* type FileBoss */

using FileBoss = RefObjectHook<FileBossBase,AbstractFileBoss,GetNullFileBossPtr> ;

} // namespace Video
} // namespace CCore

#endif

