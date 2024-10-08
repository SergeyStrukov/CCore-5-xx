/* Config.h */
//----------------------------------------------------------------------------------------
//
//  Project: PTPSecureClient 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/ddl/DDLMapTypes.h>

namespace App {

/* using */

using namespace CCore;

/* TypeDef */

#include "inc/Config.type.h"

/* classes */

class Config;

/* class Config */

class Config : NoCopy
 {
   void *mem;

   TypeDef::PTPSecureConfig *config;

  private:

   static StrLen Pretext();

  public:

   explicit Config(StrLen file_name);

   ~Config();

   operator TypeDef::PTPSecureConfig * () const { return config; }

   void show();
 };

} // namespace App

