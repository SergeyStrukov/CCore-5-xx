/* InternalHost.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WinNN
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_InternalHost_h
#define CCore_inc_video_InternalHost_h

#include <CCore/inc/PlatformRandom.h>

#include <CCore/inc/video/Picture.h>

#include <CCore/inc/video/InternalUtils.h>

namespace CCore {
namespace Video {
namespace Internal {

/* classes */

class WindowClass;

class WindowPaint;

class WindowsHost;

/* class WindowClass */

class WindowClass : NoCopy
 {
   WinNN::Atom atom;
   Sys::ErrorType error;

   WinNN::HIcon hIcon;
   WinNN::HIcon hIconSm;

  private:

   void regClass();

   static WinNN::HIcon CreateIcon(Picture pict,Coord dx,Coord dy);

  public:

   WindowClass();

   ~WindowClass();

   void setIcon(Picture pict);

   WinNN::Atom getAtom(const char *format);
 };

/* class WindowPaint */

class WindowPaint : NoCopy
 {
   WinNN::HWindow hWnd;
   WinNN::PaintData pd;
   WinNN::HGDevice hGD;

  public:

   explicit WindowPaint(WinNN::HWindow hWnd);

   ~WindowPaint();

   WinNN::HGDevice getGD() const { return hGD; }

   Pane getPane() const { return ToPane(pd.rect); }

   bool getEraseFlag() const { return pd.erase_flag; }
 };

/* class WindowsHost */

class WindowsHost : public WindowHost
 {
   WinNN::HWindow hWnd = 0 ;
   bool is_main = false ;

   Point origin;

   WindowBuf buf;
   bool buf_dirty = true ;

   unsigned track_flags = 0 ;
   bool track_on = false ;
   unsigned hover_time = 0 ;

   Pane restore;
   bool max_flag = false ;

   Sys::WChar hi = 0 ;
   Sys::WChar syshi = 0 ;

  private:

   void guardAlive(const char *format);

   void guardDead(const char *format);

   static void GuardMaxSize(const char *format,Point max_size);

   static void GuardPane(const char *format,Pane pane,Point max_size);

   void guardCreate(const char *format,Point max_size);

   void guardCreate(const char *format,Pane pane,Point max_size);

   static WinNN::HCursor CursorTable[];

   static WinNN::HCursor GetCursor(MouseShape mshape)
    {
     return CursorTable[mshape];
    }

   static WindowClass WindowClassObject;

   static WinNN::HWindow HMainWindow;

   static PlatformRandom Random;

  private:

   static VKey ToVKey(WinNN::MsgWParam wParam);

   static KeyMod GetKeyMod(bool ext);

   static KeyMod GetKeyMod();

   static MouseKey ToMouseKey(WinNN::MsgWParam wParam);

   static Point ToPoint(WinNN::MsgLParam lParam);

   static Point ToSize(WinNN::MsgLParam lParam);

   static Coord RandomLen(Coord len);

   static Coord RandomPos(Coord len);

   void setTrack();

   void mouseShape(VKey vkey,KeyMod kmod);

   WinNN::MsgResult msgProc(WinNN::HWindow hWnd,WinNN::MsgCode message,WinNN::MsgWParam wParam,WinNN::MsgLParam lParam);

   static WinNN::MsgResult ObjWndProc(WindowsHost *obj,WinNN::HWindow hWnd,WinNN::MsgCode message,WinNN::MsgWParam wParam,WinNN::MsgLParam lParam);

   static WinNN::MsgResult WndProc(WinNN::HWindow hWnd,WinNN::MsgCode message,WinNN::MsgWParam wParam,WinNN::MsgLParam lParam);

   void reset();

   void do_move(Pane pane);

   friend class WindowClass;

  public:

   WindowsHost();

   virtual ~WindowsHost();

   // msg boxes

   static void AbortMsgBox(StrLen text);

   static void ErrorMsgBox(StrLen text,StrLen title);

   // icons

   static void SetAppIcon(Picture pict);

   // WinControl
   // create/destroy

   virtual void createMain(Point max_size);

   virtual void createMain(Pane pane,Point max_size);

   virtual void create(Pane pane,Point max_size);

   virtual void create(WindowHost *parent,Pane pane,Point max_size);

   virtual void destroy();

   // operations

   virtual void setTitle(StrLen title);

   virtual void setMaxSize(Point max_size);

   virtual bool enableUserInput(bool en);

   virtual void display(CmdDisplay cmd_display);

   virtual void show();

   virtual void hide();

   // drawing

   virtual void update();

   virtual void invalidate(Pane pane,unsigned token);

   virtual void invalidate(unsigned token);

   virtual ColorPlane getDrawPlane();

   // keyboard

   virtual void setFocus();

   // mouse

   virtual Point getMousePos();

   virtual void captureMouse();

   virtual void releaseMouse();

   virtual void trackMouseHover();

   virtual void trackMouseHover(MSec time);

   virtual void untrackMouseHover();

   virtual void trackMouseLeave();

   virtual void untrackMouseLeave();

   virtual void setMouseShape(MouseShape mshape);

   // place

   virtual Pane getPlace();

   virtual void setPlace(Pane pane);

   // clipboard

   virtual void textToClipboard(PtrLen<const Char> text);

   virtual void textFromClipboard(Function<void (PtrLen<const Char>)> func);
 };

} // namespace Internal
} // namespace Video
} // namespace CCore

#endif

