/* SysTime.h */
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

#ifndef CCore_inc_sys_SysTime_h
#define CCore_inc_sys_SysTime_h

#include <CCore/inc/Gadget.h>

namespace CCore {
namespace Sys {

/* types */

using MSecTimeType  = uint32 ; // unsigned integral type

using SecTimeType   = uint32 ; // unsigned integral type

using ClockTimeType = uint64 ; // unsigned integral type

/* functions */

MSecTimeType GetMSecTime() noexcept;

SecTimeType GetSecTime() noexcept;

ClockTimeType GetClockTime() noexcept;

} // namespace Sys
} // namespace CCore

#endif


