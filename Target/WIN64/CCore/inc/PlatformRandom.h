/* PlatformRandom.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN64
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_PlatformRandom_h
#define CCore_inc_PlatformRandom_h

#include <CCore/inc/crypton/IntelRandom.h>

namespace CCore {

/* type PlatformRandom */

using PlatformRandom = Crypton::IntelRandom ;

} // namespace CCore

#endif


