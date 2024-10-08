/* LangStateMachine.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CondLangLR1 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include "inc/process/LangStateMachine.h"

#include <CCore/inc/Exception.h>

namespace App {

/* functions */

void GuardStateCapReached()
 {
  Printf(Exception,"App::LangStateMachine<Estimate,Context>::LangStateMachine(...) : state cap reached");
 }

} // namespace App


