/* DDLMapBase.h */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_ddl_DDLMapBase_h
#define CCore_inc_ddl_DDLMapBase_h

#include <CCore/inc/MemAllocGuard.h>

namespace CCore {
namespace DDL {

/* classes */

class MapBase;

/* class MapBase */

class MapBase : NoCopy
 {
   MemAllocGuard guard;

  public:

   template <class Map>
   explicit MapBase(Map &map)
    : guard(map.getLen())
    {
     map(guard);
    }

   ~MapBase()
    {
    }
 };

} // namespace DDL
} // namespace CCore

#endif


