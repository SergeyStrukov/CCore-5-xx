/* BookClient.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_book_Client_h
#define CCore_inc_video_book_Client_h

#include <CCore/inc/video/book/BookWindow.h>

#include <CCore/inc/video/FileFrame.h>

namespace CCore {
namespace Video {
namespace Book {

/* classes */

class ClientWindow;

/* class ClientWindow */

class ClientWindow : public ComboWindow
 {
  public:

   using SubWinType = BookWindow ;

   struct ClientBag
    {
     // common

     String title = "Book"_str ;

     // menu

     String menu_File    = "@File"_str ;
     String menu_Options = "@Options"_str ;
     String menu_Empty   = "@Empty"_str ;
     String menu_Open    = "@Open ..."_str ;
     String menu_Exit    = "E@xit"_str ;
     String menu_Global  = "@Global ..."_str ;
     String menu_Book    = "@Book ..."_str ;

     // constructors

     ClientBag() noexcept {}

     // methods

     template <class Ptr,class Func>
     static void Members(Ptr ptr,Func func)
      {
       func("title"_c,ptr->title);
       func("menu_File"_c,ptr->menu_File);
       func("menu_Options"_c,ptr->menu_Options);
       func("menu_Empty"_c,ptr->menu_Empty);
       func("menu_Open"_c,ptr->menu_Open);
       func("menu_Exit"_c,ptr->menu_Exit);
       func("menu_Global"_c,ptr->menu_Global);
       func("menu_Book"_c,ptr->menu_Book);
      }

     void bindItems(ConfigItemBind &binder);

     void findFonts() {}
    };

   using AppBag = CombineBags<BookWindow::AppBag,ClientBag> ;

   struct Config
    {
     // user

     RefVal<String> text_LoadFile = "Select a file to load from"_str ;

     CtorRefVal<FileFrame::ConfigType> file_cfg;
     CtorRefVal<SimpleTopMenuWindow::ConfigType> menu_cfg;
     CtorRefVal<SimpleCascadeMenu::ConfigType> cascade_menu_cfg;

     // app

     RefVal<String> menu_File    = "@File"_str ;
     RefVal<String> menu_Options = "@Options"_str ;
     RefVal<String> menu_Empty   = "@Empty"_str ;
     RefVal<String> menu_Open    = "@Open ..."_str ;
     RefVal<String> menu_Exit    = "E@xit"_str ;
     RefVal<String> menu_Global  = "@Global ..."_str ;
     RefVal<String> menu_Book    = "@Book ..."_str ;

     SubWinType::ConfigType sub_win_cfg;

     template <class UserPref,class AppPref>
     Config(const UserPref &user_pref,const AppPref &app_pref) noexcept
      : sub_win_cfg(user_pref,app_pref)
      {
       bindUser(user_pref.get(),user_pref.getSmartConfig());
       bindApp(app_pref.get());
      }

     template <class Bag,class Proxy>
     void bindUser(const Bag &bag,Proxy proxy)
      {
       text_LoadFile.bind(bag.text_LoadFile);

       file_cfg.bind(proxy);
       menu_cfg.bind(proxy);
       cascade_menu_cfg.bind(proxy);
      }

     template <class Bag>
     void bindApp(const Bag &bag)
      {
       menu_File.bind(bag.menu_File);
       menu_Options.bind(bag.menu_Options);
       menu_Empty.bind(bag.menu_Empty);
       menu_Open.bind(bag.menu_Open);
       menu_Exit.bind(bag.menu_Exit);
       menu_Global.bind(bag.menu_Global);
       menu_Book.bind(bag.menu_Book);
      }
    };

   using ConfigType = Config ;

  private:

   const Config &cfg;

   // menu

   MenuData menu_data;

   MenuData menu_file_data;
   MenuData menu_opt_data;

   SimpleTopMenuWindow menu;
   SimpleCascadeMenu cascade_menu;

   // inner

   SubWinType sub_win;

   // frames

   FileFrame file_frame;

   // continuation

   enum Continue
    {
     ContinueNone = 0,

     ContinueOpen
    };

   Continue cont = ContinueNone ;
   Point file_point;

   Point action_base = Point(10,10) ;

  private:

   void menuOff();

   void fileOff();

   void startOpen(Point point);

   enum MenuId
    {
     MenuFile = 1,

     MenuFileEmpty  = 101,
     MenuFileOpen   = 102,
     MenuFileExit   = 110,

     MenuOptions = 2,

     MenuOptionsUserPref = 201,
     MenuOptionsBookPref = 202
    };

   void menuAction(int id,Point point);

   void menuAction(int id);

  private:

   void menu_selected(int id,Point point);

   void cascade_menu_selected(int id,Point point);

   void cascade_menu_pressed(VKey vkey,KeyMod kmod);

   void file_destroyed();

   SignalConnector<ClientWindow,int,Point> connector_menu_selected;
   SignalConnector<ClientWindow,int,Point> connector_cascade_menu_selected;
   SignalConnector<ClientWindow,VKey,KeyMod> connector_cascade_menu_pressed;
   SignalConnector<ClientWindow> connector_file_destroyed;

  public:

   ClientWindow(SubWindowHost &host,const Config &cfg,OptFileName opt,Signal<> &update);

   virtual ~ClientWindow();

   // methods

   template <class AppState>
   void prepare(const AppState &app_state) { sub_win.prepare(app_state); }

   template <class AppState>
   void save(AppState &app_state) { sub_win.save(app_state); }

   Point getMinSize() const;

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

   // signals

   Signal<Point> doUserPref;
   Signal<Point> doAppPref;
 };

} // namespace Book
} // namespace Video
} // namespace CCore

#endif

