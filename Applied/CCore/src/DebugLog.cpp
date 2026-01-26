/* DebugLog.cpp */ 
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
//  Copyright (c) 2024 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------
 
#include <CCore/inc/DebugLog.h>
 
namespace CCore {

/* class DebugLog */

DebugLog::DebugLog(StrLen file_name)
 : out(file_name)
 {
 }

DebugLog::~DebugLog()
 {
 }

/* global Log */ 

DebugLog Log;

} // namespace CCore

