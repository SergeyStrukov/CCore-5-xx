/* Trace.h */
//----------------------------------------------------------------------------------------
//
//  Project: Labs/Coroutines
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef App_Trace_h
#define App_Trace_h

#include <CCore/inc/Gadget.h>

namespace App {

/* using */

using namespace CCore;

/* functions */

void ShowStack();

void TraceBeg();

void TraceEnd();

void TraceValue(int value);

void TraceException(int ex);

} // namespace App

#endif


