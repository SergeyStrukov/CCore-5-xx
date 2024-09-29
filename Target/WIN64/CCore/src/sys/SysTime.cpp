/* SysTime.cpp */
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

#include <CCore/inc/sys/SysTime.h>
#include <CCore/inc/sys/SysTypes.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* functions */

MSecTimeType GetMSecTime() noexcept
 {
  return WinNN::GetTickCount();
 }

SecTimeType GetSecTime() noexcept
 {
  WinNN::file_time_t sys_time;

  WinNN::GetSystemTimeAsFileTime(&sys_time);

  return SecTimeType( sys_time/10'000'000-11'644'473'600ul );
 }

#if 0

 // Fake code

ClockTimeType GetClockTime() noexcept
 {
  return 0;
 }

#endif

} // namespace Sys
} // namespace CCore


