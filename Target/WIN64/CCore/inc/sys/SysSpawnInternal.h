/* SysSpawnInternal.h */
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

#ifndef CCore_inc_sys_SysSpawnInternal_h
#define CCore_inc_sys_SysSpawnInternal_h

#include <CCore/inc/sys/SysUtf8.h>

#include <CCore/inc/Array.h>

namespace CCore {
namespace Sys {

/* classes */

class ProcessSetup;

/* class ProcessSetup */

class ProcessSetup : NoCopy
 {
   DynArray<WChar> dir;
   DynArray<WChar> cmdline;
   DynArray<WChar> envblock;

   bool has_dir;
   ErrorType error;

  private:

   class BuildStr;

   void makeDir(char *wdir);

   void makeCmdline(char *path,char **argv);

   void makeEnvblock(char **envp);

  public:

   ProcessSetup(char *wdir,char *path,char **argv,char **envp) noexcept; // path!=0 , argv!=0 , envp!=0

   ~ProcessSetup();

   ErrorType create(handle_t &handle) noexcept;
 };

} // namespace Sys
} // namespace CCore

#endif

