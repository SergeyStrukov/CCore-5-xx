/* SysAbort.h */
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

#ifndef CCore_inc_sys_SysAbort_h
#define CCore_inc_sys_SysAbort_h

#include <CCore/inc/sys/SysError.h>

namespace CCore {
namespace Sys {

/* private functions */

void AbortOnError(const char *text,ErrorType error);

inline void AbortIf(bool nok,const char *text) { if( nok ) AbortOnError(text,NonNullError()); }

/* functions */

void AbortProcess();

} // namespace Sys
} // namespace CCore

#endif


