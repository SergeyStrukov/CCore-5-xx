/* Client.h */
//----------------------------------------------------------------------------------------
//
//  Project: ContourLab 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef Client_h
#define Client_h

#include <CCore/inc/video/AppOpt.h>
#include <CCore/inc/video/BookFrame.h>

#include <inc/Editor.h>

namespace App {

/* classes */

class ClientWindow;

/* class ClientWindow */

class ClientWindow : public ComboWindow , public AliveControl
 {
  public:

   struct Config
    {
     // user

     RefVal<String> text_LoadFile = "Select a file to load from"_str ;
     RefVal<String> text_SaveFile = "Select a file to save to"_str ;

     RefVal<String> text_Alert   = "Alert"_str ;
     RefVal<String> text_AskSave = "Save modifications?"_str ;

     RefVal<String> text_Yes    = "Yes"_str ;
     RefVal<String> text_No     = "No"_str ;
     RefVal<String> text_Cancel = "Cancel"_str ;

     CtorRefVal<FileFrame::ConfigType> file_cfg;
     CtorRefVal<MessageFrame::AlertConfigType> msg_cfg;
     CtorRefVal<SimpleTopMenuWindow::ConfigType> menu_cfg;
     CtorRefVal<SimpleCascadeMenu::ConfigType> cascade_menu_cfg;

     // app

     RefVal<String> menu_File    = "@File"_str ;
     RefVal<String> menu_Options = "@Options"_str ;
     RefVal<String> menu_Help    = "@Help"_str ;
     RefVal<String> menu_New     = "@New"_str ;
     RefVal<String> menu_Open    = "@Open ..."_str ;
     RefVal<String> menu_Save    = "@Save"_str ;
     RefVal<String> menu_SaveAs  = "Save @as ..."_str ;
     RefVal<String> menu_Exit    = "E@xit"_str ;
     RefVal<String> menu_Global  = "@Global ..."_str ;
     RefVal<String> menu_App     = "@Application ..."_str ;
     RefVal<String> menu_Book    = "@Book ..."_str ;
     RefVal<String> menu_Manual  = "@Manual ..."_str ;

     RefVal<String> title_Manual  = "Book: ContourLab manual"_str ;

     EditorWindow::ConfigType editor_cfg;

     BookFrame::ConfigType book_cfg;

     template <class AppPref,class BookPref>
     Config(const UserPreference &user_pref,const AppPref &app_pref,const BookPref &book_pref) noexcept
      : editor_cfg(user_pref,app_pref),
        book_cfg(user_pref,book_pref)
      {
       bindUser(user_pref.get(),user_pref.getSmartConfig());
       bindApp(app_pref.get());
      }

     template <class Bag,class Proxy>
     void bindUser(const Bag &bag,Proxy proxy)
      {
       text_LoadFile.bind(bag.text_LoadFile);
       text_SaveFile.bind(bag.text_SaveFile);

       text_Alert.bind(bag.text_Alert);
       text_AskSave.bind(bag.text_AskSave);

       text_Yes.bind(bag.text_Yes);
       text_No.bind(bag.text_No);
       text_Cancel.bind(bag.text_Cancel);

       file_cfg.bind(proxy);
       msg_cfg.bind(proxy);
       menu_cfg.bind(proxy);
       cascade_menu_cfg.bind(proxy);
      }

     template <class Bag>
     void bindApp(const Bag &bag)
      {
       menu_File.bind(bag.menu_File);
       menu_Options.bind(bag.menu_Options);
       menu_Help.bind(bag.menu_Help);
       menu_New.bind(bag.menu_New);
       menu_Open.bind(bag.menu_Open);
       menu_Save.bind(bag.menu_Save);
       menu_SaveAs.bind(bag.menu_SaveAs);
       menu_Exit.bind(bag.menu_Exit);
       menu_Global.bind(bag.menu_Global);
       menu_App.bind(bag.menu_App);
       menu_Book.bind(bag.menu_Book);
       menu_Manual.bind(bag.menu_Manual);
       title_Manual.bind(bag.title_Manual);
      }
    };

   using ConfigType = Config ;

  private:

   const Config &cfg;

   // menu

   MenuData menu_data;

   MenuData menu_file_data;
   MenuData menu_opt_data;
   MenuData menu_help_data;

   SimpleTopMenuWindow menu;
   SimpleCascadeMenu cascade_menu;

   // editor

   EditorWindow editor;

   // frames

   FileFrame file_frame;
   MessageFrame msg_frame;
   BookFrame book_frame;

   // continuation

   enum Continue
    {
     ContinueNone = 0,

     ContinueNew,
     ContinueOpen,
     ContinueStartOpen,
     ContinueSave,
     ContinueSaveAs,
     ContinueExit
    };

   Continue cont = ContinueNone ;
   Point file_point;

   Point action_base = Point(10,10) ;

  private:

   void menuOff();

   void fileOff();

   void msgOff();

   void askSave(Continue cont);

   void startOpen(Point point);

   void startSave(Point point);

   enum MenuId
    {
     MenuFile = 1,

     MenuFileNew    = 101,
     MenuFileOpen   = 102,
     MenuFileSave   = 103,
     MenuFileSaveAs = 104,
     MenuFileExit   = 110,

     MenuOptions = 2,

     MenuOptionsUserPref = 201,
     MenuOptionsAppPref  = 202,
     MenuOptionsBookPref = 203,

     MenuHelp = 3,

     MenuManual = 301
    };

   void menuAction(int id,Point point);

   void menuAction(int id);

  private:

   void menu_selected(int id,Point point);

   void cascade_menu_selected(int id,Point point);

   void cascade_menu_pressed(VKey vkey,KeyMod kmod);

   void file_destroyed();

   void msg_destroyed();

   SignalConnector<ClientWindow,int,Point> connector_menu_selected;
   SignalConnector<ClientWindow,int,Point> connector_cascade_menu_selected;
   SignalConnector<ClientWindow,VKey,KeyMod> connector_cascade_menu_pressed;
   SignalConnector<ClientWindow> connector_file_destroyed;
   SignalConnector<ClientWindow> connector_msg_destroyed;

   void book_doUserPref(Point point);

   void book_doBookPref(Point point);

   SignalConnector<ClientWindow,Point> connector_book_doUserPref;
   SignalConnector<ClientWindow,Point> connector_book_doBookPref;

  public:

   ClientWindow(SubWindowHost &host,const Config &cfg,OptNone opt,Signal<> &update);

   virtual ~ClientWindow();

   // base

   virtual void open();

   // drawing

   virtual void layout();

   // user input

   virtual void react(UserAction action);

   void react_Key(VKey vkey,KeyMod kmod);

   void react_LeftClick(Point point,MouseKey mkey);

   void react_RightClick(Point point,MouseKey mkey);

   void react_other(UserAction action);

   // AliveControl

   virtual bool askDestroy();

   // signals

   Signal<Point> doUserPref;
   Signal<Point> doAppPref;
   Signal<Point> doBookPref;
 };

} // namespace App

#endif

