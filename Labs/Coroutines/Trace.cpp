/* Trace.cpp */
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

#include "Trace.h"

#include <CCore/inc/Print.h>

namespace App {

/* functions */

void ShowStack()
 {
  int unused;

  Printf(Con,"stack = #.h;\n",(uptr)&unused);
 }

void TraceBeg()
 {
  Printf(Con,"beg\n");
 }

void TraceEnd()
 {
  Printf(Con,"end\n");
 }

void TraceValue(int value)
 {
  Printf(Con,"#;\n",value);
 }

void TraceException(int ex)
 {
  Printf(Con,"exception #;\n",ex);
 }

} // namespace App
