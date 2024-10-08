/* Application.h */
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

  String title = "Aspect"_str ;

  String text_Path   = "Path"_str ;
  String text_Aspect = "Aspect"_str ;

  String text_Nothing = "Nothing to save!"_str ;
  String text_Save    = "Save"_str ;

  // menu

  String text_SelectPath = "Select a project path"_str ;

  String menu_File    = "@File"_str ;
  String menu_Actions = "@Actions"_str ;
  String menu_Options = "@Options"_str ;
  String menu_Recent  = "@Recent"_str ;

  String menu_New     = "@New ..."_str ;
  String menu_Open    = "@Open ..."_str ;
  String menu_Save    = "@Save"_str ;
  String menu_SaveAs  = "Save @as ..."_str ;
  String menu_Exit    = "E@xit"_str ;
  String menu_Collect = "@Collect"_str ;
  String menu_Global  = "@Global ..."_str ;
  String menu_App     = "@Application ..."_str ;

  // hide

  Coord status_dxy = 30 ;

  String text_Hide    = "Hide"_str ;
  String text_ShowAll = "Show all"_str ;

  // count

  Coord count_status_dxy = 15 ;

  // item

  Coord item_dxy = 30 ;
  Coord item_rxy = 20 ;
  Coord item_rin =  2 ;
  Coord item_kxy = 20 ;

  VColor item_text = Black ;

  FontCouple item_font;

  VColor item_face1 = Black ;
  VColor item_face2 =  Gray ;
  VColor item_line  =  Navy ;

  // status

  VColor status_New    = SkyBlue ;
  VColor status_Ignore = Gray ;
  VColor status_Red    = Red ;
  VColor status_Yellow = Yellow ;
  VColor status_Green  = Green ;

  // constructors

  AppPreferenceBag() noexcept {}

  // methods

  template <class Ptr,class Func>
  static void Members(Ptr ptr,Func func);

  void bindItems(ConfigItemBind &binder);
 };

template <class Ptr,class Func>
void AppPreferenceBag::Members(Ptr ptr,Func func)
 {
  func("title"_c,ptr->title);

  func("text_Path"_c,ptr->text_Path);
  func("text_Aspect"_c,ptr->text_Aspect);

  func("text_Nothing"_c,ptr->text_Nothing);
  func("text_Save"_c,ptr->text_Save);

  func("text_SelectPath"_c,ptr->text_SelectPath);

  func("menu_File"_c,ptr->menu_File);
  func("menu_Actions"_c,ptr->menu_Actions);
  func("menu_Options"_c,ptr->menu_Options);
  func("menu_Recent"_c,ptr->menu_Recent);

  func("menu_New"_c,ptr->menu_New);
  func("menu_Open"_c,ptr->menu_Open);
  func("menu_Save"_c,ptr->menu_Save);
  func("menu_SaveAs"_c,ptr->menu_SaveAs);
  func("menu_Exit"_c,ptr->menu_Exit);
  func("menu_Collect"_c,ptr->menu_Collect);
  func("menu_Global"_c,ptr->menu_Global);
  func("menu_App"_c,ptr->menu_App);

  func("status_dxy"_c,ptr->status_dxy);
  func("text_Hide"_c,ptr->text_Hide);
  func("text_ShowAll"_c,ptr->text_ShowAll);

  func("count_status_dxy"_c,ptr->count_status_dxy);

  func("item_dxy"_c,ptr->item_dxy);
  func("item_rxy"_c,ptr->item_rxy);
  func("item_rin"_c,ptr->item_rin);
  func("item_kxy"_c,ptr->item_kxy);

  func("item_text"_c,ptr->item_text);

  func("item_font"_c,ptr->item_font);

  func("item_face1"_c,ptr->item_face1);
  func("item_face2"_c,ptr->item_face2);
  func("item_line"_c,ptr->item_line);

  func("status_New"_c,ptr->status_New);
  func("status_Ignore"_c,ptr->status_Ignore);
  func("status_Red"_c,ptr->status_Red);
  func("status_Yellow"_c,ptr->status_Yellow);
  func("status_Green"_c,ptr->status_Green);
 }

/* struct AppProp */

struct AppProp
 {
  static StrLen Key() { return AppKey(); }

  static Picture Icon() { return DefaultAppIcon(); }

  using PreferenceBag = AppPreferenceBag ;

  using ClientWindow = App::ClientWindow ;

  using Opt = OptFileName ;

  using Persist = AppState ;

  static constexpr PrepareOpt Prepare = PreparePersist ;
 };

} // namespace App

#endif

