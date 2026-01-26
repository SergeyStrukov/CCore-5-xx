/* test7101.UserPref.cpp */ 
//----------------------------------------------------------------------------------------
//
//  Project: CCore 5.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003 
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2025 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/video/UserPreference.h>

namespace App {

/* Testit<7101> */ 

template<>
const char *const Testit<7101>::Name="Test7101 UserPref";

template<>
bool Testit<7101>::Main() 
 {
  using namespace Video;

  UserPreference user_pref;

  user_pref.sync();

  Printf(Con,"#;\n",user_pref.get().text_Ok);

  return true;
 }
 
} // namespace App

