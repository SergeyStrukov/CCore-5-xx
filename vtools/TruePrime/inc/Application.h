/* Application.h */
//----------------------------------------------------------------------------------------
//
//  Project: TruePrime 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef Application_h
#define Application_h

#include <CCore/inc/video/AppMain.h>

#include <inc/Client.h>

namespace App {

/* classes */

struct AppPreferenceBag;

struct AppProp;

/* struct AppPreferenceBag */

struct AppPreferenceBag
 {
  // common

  String title = "TruePrime"_str ;

  // menu

  String menu_File    = "@File"_str ;
  String menu_Options = "@Options"_str ;
  String menu_Exit    = "E@xit"_str ;
  String menu_Global  = "@Global"_str ;
  String menu_App     = "@Application"_str ;

  // number

  VColor number_text = Black ;

  Point number_space = Point(8,8) ;

  FontCouple number_font;

  // constructors

  AppPreferenceBag() noexcept {}

  // methods

  template <class Ptr,class Func>
  static void Members(Ptr ptr,Func func);

  virtual void bindItems(ConfigItemBind &binder);

  void findFonts();
 };

template <class Ptr,class Func>
void AppPreferenceBag::Members(Ptr ptr,Func func)
 {
  func("title"_c,ptr->title);
  func("menu_File"_c,ptr->menu_File);
  func("menu_Options"_c,ptr->menu_Options);
  func("menu_Exit"_c,ptr->menu_Exit);
  func("menu_Global"_c,ptr->menu_Global);
  func("menu_App"_c,ptr->menu_App);

  func("number_text"_c,ptr->number_text);
  func("number_space"_c,ptr->number_space);
  func("number_font"_c,ptr->number_font);
 }

/* struct AppProp */

struct AppProp
 {
  static StrLen Key() { return AppKey(); }

  static Picture Icon() { return DefaultAppIcon(); }

  using PreferenceBag = AppPreferenceBag ;

  using ClientWindow = App::ClientWindow ;

  using Opt = OptNone ;

  static constexpr PrepareOpt Prepare = PrepareCenter ;
 };

} // namespace App

#endif

