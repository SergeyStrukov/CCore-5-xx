/* DebugLog.h */ 
//----------------------------------------------------------------------------------------
//
//  Project: CCore 5.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003 
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2025 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_DebugLog_h
#define CCore_inc_DebugLog_h

#include <CCore/inc/Print.h>
 
namespace CCore {

/* classes */    

class DebugLog;

/* class DebugLog */

class DebugLog : NoCopy
 {
   PrintFile out;
   
  public:
  
   explicit DebugLog(StrLen file_name="out/log.txt"_c);

   ~DebugLog();

   template <class ... TT>
   void operator () (const char *format,const TT && ... tt)
    {
     Printf(out,format,tt...);   

     out.flush();
    }
 };

/* global Log */ 

extern DebugLog Log;

} // namespace CCore
 
#endif

