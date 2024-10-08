/* AppState.h */
//----------------------------------------------------------------------------------------
//
//  Project: Aspect 1.01
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef AppState_h
#define AppState_h

#include <CCore/inc/Array.h>
#include <CCore/inc/String.h>

#include <inc/App.h>

namespace App {

/* classes */

struct AppState;

/* struct AppState */

struct AppState
 {
  FramePlace place;

  DynArray<String> recent_files;

  // methods

  static StrLen File();

  static StrLen Pretext();

  AppState();

  ~AppState();

  bool load(StrLen file_name);

  void save(StrLen file_name) const;

  bool load() noexcept;

  void save() const noexcept;
 };

} // namespace App

#endif


