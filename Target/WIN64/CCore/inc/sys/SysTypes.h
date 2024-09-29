/* SysTypes.h */
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

#ifndef CCore_inc_sys_SysTypes_h
#define CCore_inc_sys_SysTypes_h

#include <CCore/inc/win64/Win64Types.h>

namespace CCore {
namespace Sys {

/* namespace WinNN */

namespace WinNN = Win64 ;

/* types */

using WChar = WinNN::wchar ;

using unicode_t = unsigned int ;

using handle_t = WinNN::handle_t ;

using index_t = WinNN::index_t ;

using flags_t = WinNN::flags_t ;

} // namespace Sys
} // namespace CCore

#endif

