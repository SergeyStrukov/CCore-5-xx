/* ConfigBinder.cpp */
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

#include <CCore/inc/video/ConfigBinder.h>

#include <CCore/inc/video/HomeFile.h>

namespace CCore {
namespace Video {

/* struct HomeSyncBase */

bool HomeSyncBase::syncHome(StrLen home_dir,StrLen cfg_file) noexcept
 {
  bool ret=false;

  try
    {
     HomeFile home_file(home_dir,cfg_file);

     if( home_file.exist() )
       {
        ConfigMap map;

        ret=map.loadDDL_safe(home_file.get());

        syncMap(map);

        if( map.isModified() )
          {
           map.saveDDL(home_file.get());
          }
       }
     else
       {
        ConfigMap map;

        updateMap(map);

        home_file.createDir();

        map.saveDDL(home_file.get());
       }
    }
  catch(...)
    {
    }

  return ret;
 }

void HomeSyncBase::updateHome(StrLen home_dir,StrLen cfg_file) const noexcept
 {
  try
    {
     HomeFile home_file(home_dir,cfg_file);

     ConfigMap map;

     if( home_file.exist() ) map.loadDDL_safe(home_file.get());

     updateMap(map);

     home_file.createDir();

     map.saveDDL(home_file.get());
    }
  catch(...)
    {
    }
 }

} // namespace Video
} // namespace CCore

