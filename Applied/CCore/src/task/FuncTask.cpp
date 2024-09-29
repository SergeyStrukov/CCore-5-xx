/* FuncTask.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/task/FuncTask.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* GuardRunFuncTaskFailed() */

void GuardRunFuncTaskFailed()
 {
  Printf(Exception,"CCore::RunFuncTask<FuncInit>(...) : failed");
 }

} // namespace CCore


