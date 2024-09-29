/* InternalHost.cpp */
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

#include <CCore/inc/video/InternalHost.h>

#include <CCore/inc/sys/SysUtf8.h>

#include <CCore/inc/sys/SysFileInternal.h>

#include <CCore/inc/Exception.h>

namespace CCore {
namespace Video {
namespace Internal {

/* class WindowClass */

void WindowClass::regClass()
 {
  WinNN::HModule hModule=(WinNN::HModule)WinNN::GetModuleHandleW(0);

  if( hModule==0 )
    {
     error=Sys::NonNullError();

     return;
    }

  WinNN::WindowClass wndcls;

  wndcls.cb=sizeof wndcls;
  wndcls.class_extra=0;
  wndcls.window_extra=sizeof (WinNN::UPtrType);

  wndcls.hModule=hModule;

  wndcls.style=WinNN::ClassStyle_VRedraw|WinNN::ClassStyle_HRedraw|WinNN::ClassStyle_DClick;

  wndcls.hCursor=WindowsHost::GetCursor(Mouse_Arrow);

  wndcls.hBrush=0;
  wndcls.hIcon=hIcon;
  wndcls.hIconSm=hIconSm;
  wndcls.menu_res=0;

  Sys::WCharString<> temp("9613CA28BE7A78F0-2DD3FC07C7330F49-WindowsHost"_c);

  wndcls.class_name=temp;
  wndcls.wnd_proc=WindowsHost::WndProc;

  atom=WinNN::RegisterClassExW(&wndcls);

  if( atom==0 )
    {
     error=Sys::NonNullError();
    }
  else
    {
     error=Sys::NoError;
    }
 }

WinNN::HIcon WindowClass::CreateIcon(Picture pict,Coord dx,Coord dy)
 {
  const char *format="CCore::Video::Internal::WindowsHost::WindowClass::CreateIcon(...) : #;";

  WinNN::IconInfo info{};

  WindowBitmap mask_bmp({dx,dy});
  WindowBitmap bmp({dx,dy});
  DesktopColor ground=pict->defaultGround();

  DrawBuf color_buf(bmp.getPlane());

  color_buf.erase(ground);

  pict->draw(color_buf,dx,dy);

  DrawBuf mask_buf(mask_bmp.getPlane());

  mask_buf.erase(Black);

  info.icon_flag=true;
  info.bmpMask=mask_bmp.getHandle();
  info.bmpColor=bmp.getHandle();

  auto ret=WinNN::CreateIconIndirect(&info);

  SysGuard(format,ret!=0);

  return ret;
 }

WindowClass::WindowClass()
 : atom(0),error(Sys::NoError),hIcon(0),hIconSm(0)
 {
 }

WindowClass::~WindowClass()
 {
  if( hIcon ) WinNN::DestroyIcon(hIcon);
  if( hIconSm ) WinNN::DestroyIcon(hIconSm);
 }

void WindowClass::setIcon(Picture pict)
 {
  if( !hIcon )
    {
     Coord dx=Coord(WinNN::GetSystemMetrics(WinNN::SysMetric_DXIcon));
     Coord dy=Coord(WinNN::GetSystemMetrics(WinNN::SysMetric_DYIcon));

     hIcon=CreateIcon(pict,dx,dy);
    }

  if( !hIconSm )
    {
     Coord dx=Coord(WinNN::GetSystemMetrics(WinNN::SysMetric_DXIconSm));
     Coord dy=Coord(WinNN::GetSystemMetrics(WinNN::SysMetric_DYIconSm));

     hIconSm=CreateIcon(pict,dx,dy);
    }
 }

WinNN::Atom WindowClass::getAtom(const char *format)
 {
  if( error ) SysGuardFailed(format,error);

  if( atom!=0 ) return atom;

  regClass();

  if( error ) SysGuardFailed(format,error);

  return atom;
 }

/* class WindowPaint */

WindowPaint::WindowPaint(WinNN::HWindow hWnd_)
 {
  const char *format="CCore::Video::Internal::WindowsHost::WindowPaint::WindowPaint(...) : #;";

  if( hWnd_==0 )
    {
     Printf(Exception,format,"null hWnd");
    }

  hWnd=hWnd_;

  hGD=WinNN::BeginPaint(hWnd_,&pd);

  SysGuard(format,hGD!=0);
 }

WindowPaint::~WindowPaint()
 {
  WinNN::EndPaint(hWnd,&pd);
 }

/* class WindowsHost */

WinNN::HCursor WindowsHost::CursorTable[]=
 {
  0,
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_Arrow)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_IBeam)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_Wait)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_Cross)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_Hand)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_No)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_Help)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_SizeLeftRight)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_SizeUpDown)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_SizeUpLeft)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_SizeUpRight)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_SizeAll)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_UpArrow)),
  WinNN::LoadCursorW(0,WinNN::MakeIntResource(WinNN::SysCursor_AppStarting))
 };

WindowClass WindowsHost::WindowClassObject CCORE_INITPRI_3 ;

WinNN::HWindow WindowsHost::HMainWindow=0;

PlatformRandom WindowsHost::Random CCORE_INITPRI_3 ;

void WindowsHost::guardAlive(const char *format)
 {
  if( hWnd==0 )
    {
     Printf(Exception,format,"window is dead");
    }
 }

void WindowsHost::guardDead(const char *format)
 {
  if( hWnd!=0 )
    {
     Printf(Exception,format,"window is alive");
    }
 }

void WindowsHost::GuardMaxSize(const char *format,Point max_size)
 {
  if( max_size<=Null )
    {
     Printf(Exception,format,"bad max_size");
    }
 }

void WindowsHost::GuardPane(const char *format,Pane pane,Point max_size)
 {
  if( !(pane.getSize()<=max_size) )
    {
     Printf(Exception,format,"bad pane size");
    }
 }

void WindowsHost::guardCreate(const char *format,Point max_size)
 {
  guardDead(format);

  GuardMaxSize(format,max_size);
 }

void WindowsHost::guardCreate(const char *format,Pane pane,Point max_size)
 {
  guardDead(format);

  GuardMaxSize(format,max_size);

  GuardPane(format,pane,max_size);
 }

VKey WindowsHost::ToVKey(WinNN::MsgWParam wParam)
 {
  switch( wParam )
    {
     case WinNN::VK_Escape    : return VKey_Esc;
     case WinNN::VK_F1        : return VKey_F1;
     case WinNN::VK_F2        : return VKey_F2;
     case WinNN::VK_F3        : return VKey_F3;
     case WinNN::VK_F4        : return VKey_F4;
     case WinNN::VK_F5        : return VKey_F5;
     case WinNN::VK_F6        : return VKey_F6;
     case WinNN::VK_F7        : return VKey_F7;
     case WinNN::VK_F8        : return VKey_F8;
     case WinNN::VK_F9        : return VKey_F9;
     case WinNN::VK_F10       : return VKey_F10;
     case WinNN::VK_F11       : return VKey_F11;
     case WinNN::VK_F12       : return VKey_F12;

     case WinNN::VK_Tilda     : return VKey_Tilda;

     case '1'                 : return VKey_1;
     case '2'                 : return VKey_2;
     case '3'                 : return VKey_3;
     case '4'                 : return VKey_4;
     case '5'                 : return VKey_5;
     case '6'                 : return VKey_6;
     case '7'                 : return VKey_7;
     case '8'                 : return VKey_8;
     case '9'                 : return VKey_9;
     case '0'                 : return VKey_0;

     case WinNN::VK_Minus     : return VKey_Minus;
     case WinNN::VK_Plus      : return VKey_Equal;
     case WinNN::VK_BackSlash : return VKey_BackSlash;
     case WinNN::VK_Back      : return VKey_BackSpace;
     case WinNN::VK_Tab       : return VKey_Tab;

     case 'Q'                 : return VKey_q;
     case 'W'                 : return VKey_w;
     case 'E'                 : return VKey_e;
     case 'R'                 : return VKey_r;
     case 'T'                 : return VKey_t;
     case 'Y'                 : return VKey_y;
     case 'U'                 : return VKey_u;
     case 'I'                 : return VKey_i;
     case 'O'                 : return VKey_o;
     case 'P'                 : return VKey_p;

     case WinNN::VK_OBracket  : return VKey_OpenBracket;
     case WinNN::VK_CBracket  : return VKey_CloseBracket;
     case WinNN::VK_Return    : return VKey_Enter;

     case WinNN::VK_CapsLock  : return VKey_CapsLock;

     case 'A'                 : return VKey_a;
     case 'S'                 : return VKey_s;
     case 'D'                 : return VKey_d;
     case 'F'                 : return VKey_f;
     case 'G'                 : return VKey_g;
     case 'H'                 : return VKey_h;
     case 'J'                 : return VKey_j;
     case 'K'                 : return VKey_k;
     case 'L'                 : return VKey_l;

     case WinNN::VK_Colon     : return VKey_Semicolon;
     case WinNN::VK_Quote     : return VKey_Quote;

     case WinNN::VK_Shift     : return VKey_Shift;

     case 'Z'                 : return VKey_z;
     case 'X'                 : return VKey_x;
     case 'C'                 : return VKey_c;
     case 'V'                 : return VKey_v;
     case 'B'                 : return VKey_b;
     case 'N'                 : return VKey_n;
     case 'M'                 : return VKey_m;

     case WinNN::VK_Comma     : return VKey_Comma;
     case WinNN::VK_Period    : return VKey_Period;
     case WinNN::VK_Slash     : return VKey_Slash;

     case WinNN::VK_Control   : return VKey_Ctrl;
     case WinNN::VK_Alt       : return VKey_Alt;

     case WinNN::VK_Space     : return VKey_Space;

     case WinNN::VK_Scroll    : return VKey_ScrollLock;
     case WinNN::VK_Pause     : return VKey_Pause;

     case WinNN::VK_Insert    : return VKey_Insert;
     case WinNN::VK_Home      : return VKey_Home;
     case WinNN::VK_PgUp      : return VKey_PageUp;
     case WinNN::VK_Delete    : return VKey_Delete;
     case WinNN::VK_End       : return VKey_End;
     case WinNN::VK_PgDown    : return VKey_PageDown;

     case WinNN::VK_Up        : return VKey_Up;
     case WinNN::VK_Left      : return VKey_Left;
     case WinNN::VK_Down      : return VKey_Down;
     case WinNN::VK_Right     : return VKey_Right;

     case WinNN::VK_NumLock   : return VKey_NumLock;
     case WinNN::VK_Div       : return VKey_NumSlash;
     case WinNN::VK_Mul       : return VKey_NumAsterisk;
     case WinNN::VK_Sub       : return VKey_NumMinus;
     case WinNN::VK_Add       : return VKey_NumPlus;

     case WinNN::VK_Numpad7   : return VKey_Num7;
     case WinNN::VK_Numpad8   : return VKey_Num8;
     case WinNN::VK_Numpad9   : return VKey_Num9;
     case WinNN::VK_Numpad4   : return VKey_Num4;
     case WinNN::VK_Numpad5   : return VKey_Num5;
     case WinNN::VK_Numpad6   : return VKey_Num6;
     case WinNN::VK_Numpad1   : return VKey_Num1;
     case WinNN::VK_Numpad2   : return VKey_Num2;
     case WinNN::VK_Numpad3   : return VKey_Num3;

     case WinNN::VK_Numpad0   : return VKey_NumIns;
     case WinNN::VK_Decimal   : return VKey_NumDel;

     default: return VKey_Null;
    }
 }

KeyMod WindowsHost::GetKeyMod(bool ext)
 {
  KeyMod kmod=KeyMod_Null;

  if( ext ) kmod|=KeyMod_Ext;

  if( WinNN::GetKeyState(WinNN::VK_Shift)&WinNN::KeyStateDown ) kmod|=KeyMod_Shift;

  if( WinNN::GetKeyState(WinNN::VK_Control)&WinNN::KeyStateDown ) kmod|=KeyMod_Ctrl;

  if( WinNN::GetKeyState(WinNN::VK_CapsLock)&WinNN::KeyStateToggle ) kmod|=KeyMod_CapsLock;

  if( WinNN::GetKeyState(WinNN::VK_NumLock)&WinNN::KeyStateToggle ) kmod|=KeyMod_NumLock;

  if( WinNN::GetKeyState(WinNN::VK_Scroll)&WinNN::KeyStateToggle ) kmod|=KeyMod_ScrollLock;

  return kmod;
 }

KeyMod WindowsHost::GetKeyMod()
 {
  KeyMod kmod=KeyMod_Null;

  if( WinNN::GetKeyState(WinNN::VK_Shift)&WinNN::KeyStateDown ) kmod|=KeyMod_Shift;

  if( WinNN::GetKeyState(WinNN::VK_Control)&WinNN::KeyStateDown ) kmod|=KeyMod_Ctrl;

  if( WinNN::GetKeyState(WinNN::VK_Alt)&WinNN::KeyStateDown ) kmod|=KeyMod_Alt;

  if( WinNN::GetKeyState(WinNN::VK_CapsLock)&WinNN::KeyStateToggle ) kmod|=KeyMod_CapsLock;

  if( WinNN::GetKeyState(WinNN::VK_NumLock)&WinNN::KeyStateToggle ) kmod|=KeyMod_NumLock;

  if( WinNN::GetKeyState(WinNN::VK_Scroll)&WinNN::KeyStateToggle ) kmod|=KeyMod_ScrollLock;

  return kmod;
 }

MouseKey WindowsHost::ToMouseKey(WinNN::MsgWParam wParam)
 {
  MouseKey mkey=MouseKey_Null;

  if( wParam&WinNN::MouseKey_Left ) mkey|=MouseKey_Left;

  if( wParam&WinNN::MouseKey_Right ) mkey|=MouseKey_Right;

  if( wParam&WinNN::MouseKey_Shift ) mkey|=MouseKey_Shift;

  if( wParam&WinNN::MouseKey_Control ) mkey|=MouseKey_Ctrl;

  if( WinNN::GetKeyState(WinNN::VK_Alt)&WinNN::KeyStateDown ) mkey|=MouseKey_Alt;

  return mkey;
 }

Point WindowsHost::ToPoint(WinNN::MsgLParam lParam)
 {
  return Point((Coord)WinNN::SLoWord(lParam),(Coord)WinNN::SHiWord(lParam));
 }

Point WindowsHost::ToSize(WinNN::MsgLParam lParam)
 {
  return Point((Coord)WinNN::LoWord(lParam),(Coord)WinNN::HiWord(lParam));
 }

Coord WindowsHost::RandomLen(Coord len)
 {
  int percent=Random.select(128);

  return Coord( (len/2)+((len/4)*percent)/128 );
 }

Coord WindowsHost::RandomPos(Coord len)
 {
  int percent=Random.select(128);

  return Coord( (len*percent)/128 );
 }

void WindowsHost::setTrack()
 {
  const char *format="CCore::Video::Internal::WindowsHost::setTrack() : #;";

  WinNN::TrackMouseDesc desc;

  desc.cb=sizeof desc;
  desc.flags=track_flags;
  desc.hWnd=hWnd;
  desc.hover_time=hover_time;

  SysGuard(format, WinNN::TrackMouseEvent(&desc) );
 }

void WindowsHost::mouseShape(VKey vkey,KeyMod kmod)
 {
  if( vkey==VKey_Shift || vkey==VKey_Ctrl || vkey==VKey_Alt )
    {
     frame->setMouseShape(getMousePos(),kmod);
    }
 }

WinNN::MsgResult WindowsHost::msgProc(WinNN::HWindow hWnd_,WinNN::MsgCode message,WinNN::MsgWParam wParam,WinNN::MsgLParam lParam)
 {
  switch( message )
    {
     case WinNN::WM_GetMinMaxInfo :
      {
       WinNN::MinMaxInfo *info=(WinNN::MinMaxInfo *)lParam;

       Replace_min<int>(info->max_size.x,max_size.x);
       Replace_min<int>(info->max_size.y,max_size.y);
      }
     return 0;

     case WinNN::WM_NcCreate :
      {
       hWnd=hWnd_;
       is_alive=true;

       if( is_main ) HMainWindow=hWnd_;

       auto ret=WinNN::DefWindowProcW(hWnd_,message,wParam,lParam);

       try { frame->alive(); } catch(...) {}

       return ret;
      }

     case WinNN::WM_NcDestroy :
      {
       try { frame->dead(); } catch(...) {}

       hWnd=0;
       is_alive=false;

       if( is_main )
         {
          HMainWindow=0;

          WinNN::PostQuitMessage(0);
         }

       return WinNN::DefWindowProcW(hWnd_,message,wParam,lParam);
      }

     case WinNN::WM_Paint :
      {
       WindowPaint wp(hWnd);

       if( !buf_dirty ) buf.draw(wp.getGD(),wp.getPane());

       frame->paintDone(Replace_null(token));
      }
     return 0;

     case WinNN::WM_Move :
      {
       origin=ToPoint(lParam);
      }
     return 0;

     case WinNN::WM_Size :
      {
       frame->setSize(ToSize(lParam),buf_dirty);
      }
     return 0;

     case WinNN::WM_SetFocus :
      {
       frame->gainFocus();
      }
     return 0;

     case WinNN::WM_KillFocus :
      {
       frame->looseFocus();
      }
     return 0;

     case WinNN::WM_CaptureChanged :
      {
       frame->looseCapture();
      }
     return 0;

     case WinNN::WM_KeyDown :
      {
       unsigned repeat=lParam&0xFFFFu;
       bool ext=(lParam>>24)&1u;

       VKey vkey=ToVKey(wParam);
       KeyMod kmod=GetKeyMod(ext);

       mouseShape(vkey,kmod);

       if( repeat>1 )
         frame->put_Key(vkey,kmod,repeat);
       else
         frame->put_Key(vkey,kmod);
      }
     return 0;

     case WinNN::WM_SysKeyDown :
      {
       unsigned repeat=lParam&0xFFFFu;
       bool ext=(lParam>>24)&1u;
       bool alt=(lParam>>29)&1u;

       VKey vkey=ToVKey(wParam);
       KeyMod kmod=GetKeyMod(ext);

       if( alt ) kmod|=KeyMod_Alt;

       mouseShape(vkey,kmod);

       if( repeat>1 )
         frame->put_Key(vkey,kmod,repeat);
       else
         frame->put_Key(vkey,kmod);
      }
     return 0;

     case WinNN::WM_KeyUp :
      {
       unsigned repeat=lParam&0xFFFFu;
       bool ext=(lParam>>24)&1u;

       VKey vkey=ToVKey(wParam);
       KeyMod kmod=GetKeyMod(ext);

       mouseShape(vkey,kmod);

       if( repeat>1 )
         frame->put_KeyUp(vkey,kmod,repeat);
       else
         frame->put_KeyUp(vkey,kmod);
      }
     return 0;

     case WinNN::WM_SysKeyUp :
      {
       unsigned repeat=lParam&0xFFFFu;
       bool ext=(lParam>>24)&1u;
       bool alt=(lParam>>29)&1u;

       VKey vkey=ToVKey(wParam);
       KeyMod kmod=GetKeyMod(ext);

       if( alt ) kmod|=KeyMod_Alt;

       mouseShape(vkey,kmod);

       if( repeat>1 )
         frame->put_KeyUp(vkey,kmod,repeat);
       else
         frame->put_KeyUp(vkey,kmod);
      }
     return 0;

     case WinNN::WM_Char :
      {
       Sys::WChar ch=(Sys::WChar)wParam;

       if( hi )
         {
          if( Sys::IsLoSurrogate(ch) )
            {
             Unicode uch=Sys::Surrogate(Replace_null(hi),ch);
             unsigned repeat=lParam&0xFFFFu;

             if( repeat>1 )
               frame->put_Char(uch,repeat);
             else
               frame->put_Char(uch);
            }
          else if( Sys::IsHiSurrogate(ch) )
            {
             // broken, skip

             hi=ch;
            }
          else
            {
             // broken, skip

             hi=0;
            }
         }
       else
         {
          if( Sys::IsHiSurrogate(ch) )
            {
             hi=ch;
            }
          else if( Sys::IsLoSurrogate(ch) )
            {
             // broken, skip
            }
          else
            {
             unsigned repeat=lParam&0xFFFFu;

             if( repeat>1 )
               frame->put_Char(ch,repeat);
             else
               frame->put_Char(ch);
            }
         }
      }
     return 0;

     case WinNN::WM_SysChar :
      {
       Sys::WChar ch=(Sys::WChar)wParam;

       if( syshi )
         {
          if( Sys::IsLoSurrogate(ch) )
            {
             Unicode uch=Sys::Surrogate(Replace_null(syshi),ch);
             unsigned repeat=lParam&0xFFFFu;

             if( repeat>1 )
               frame->put_AltChar(uch,repeat);
             else
               frame->put_AltChar(uch);
            }
          else if( Sys::IsHiSurrogate(ch) )
            {
             // broken, skip

             syshi=ch;
            }
          else
            {
             // broken, skip

             syshi=0;
            }
         }
       else
         {
          if( Sys::IsHiSurrogate(ch) )
            {
             syshi=ch;
            }
          else if( Sys::IsLoSurrogate(ch) )
            {
             // broken, skip
            }
          else
            {
             unsigned repeat=lParam&0xFFFFu;

             if( repeat>1 )
               frame->put_AltChar(ch,repeat);
             else
               frame->put_AltChar(ch);
            }
         }
      }
     return 0;

     case WinNN::WM_LButtonDown :
      {
       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       frame->put_LeftClick(point,mkey);
      }
     return 0;

     case WinNN::WM_LButtonUp :
      {
       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       frame->put_LeftUp(point,mkey);
      }
     return 0;

     case WinNN::WM_LButtonDClick :
      {
       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       frame->put_LeftDClick(point,mkey);
      }
     return 0;

     case WinNN::WM_RButtonDown :
      {
       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       frame->put_RightClick(point,mkey);
      }
     return 0;

     case WinNN::WM_RButtonUp :
      {
       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       frame->put_RightUp(point,mkey);
      }
     return 0;

     case WinNN::WM_RButtonDClick :
      {
       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       frame->put_RightDClick(point,mkey);
      }
     return 0;

     case WinNN::WM_MouseMove :
      {
       if( track_flags )
         {
          if( !track_on )
            {
             setTrack();

             track_on=true;
            }
         }

       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       frame->put_Move(point,mkey);
      }
     return 0;

     case WinNN::WM_MouseWheel :
      {
       MouseKey mkey=ToMouseKey(WinNN::LoWord(wParam));
       Coord delta=WinNN::SHiWord(wParam)/WinNN::WheelDelta;
       Point point=ToPoint(lParam)-origin;

       frame->put_Wheel(point,mkey,delta);
      }
     return 0;

     case WinNN::WM_MouseHover :
      {
       MouseKey mkey=ToMouseKey(wParam);
       Point point=ToPoint(lParam);

       track_on=false;

       frame->put_Hover(point,mkey);
      }
     return 0;

     case WinNN::WM_MouseLeave :
      {
       track_on=false;

       frame->put_Leave();
      }
     return 0;

     case WinNN::WM_NcHitTest :
      {
       Point point=ToPoint(lParam);

       frame->setMouseShape(point-origin,GetKeyMod());
      }
     return WinNN::HitCode_Client;

     case WinNN::WM_SetCursor :
      {
       // do nothing
      }
     return true;

     case WinNN::WM_Close :
      {
       frame->askClose();
      }
     return 0;

     default: return WinNN::DefWindowProcW(hWnd_,message,wParam,lParam);
    }
 }

WinNN::MsgResult WindowsHost::ObjWndProc(WindowsHost *obj,WinNN::HWindow hWnd,WinNN::MsgCode message,WinNN::MsgWParam wParam,WinNN::MsgLParam lParam)
 {
  try
    {
     return obj->msgProc(hWnd,message,wParam,lParam);
    }
  catch(...)
    {
     return 0;
    }
 }

WinNN::MsgResult WindowsHost::WndProc(WinNN::HWindow hWnd,WinNN::MsgCode message,WinNN::MsgWParam wParam,WinNN::MsgLParam lParam)
 {
  TaskEventHost.add<MsgEvent>(message,MsgEvent::Entry);

  if( message==WinNN::WM_NcCreate )
    {
     WinNN::CreateData *ctx=(WinNN::CreateData *)lParam;

     WinNN::SetWindowLongW(hWnd,0,(WinNN::UPtrType)(ctx->arg));
    }

  void *arg=(void *)WinNN::GetWindowLongW(hWnd,0);

  WinNN::MsgResult ret;

  if( !arg )
    {
     // WM_GetMinMaxInfo comes before WM_NcCreate

     ret=WinNN::DefWindowProcW(hWnd,message,wParam,lParam);
    }
  else
    {
     ret=ObjWndProc(static_cast<WindowsHost *>(arg),hWnd,message,wParam,lParam);
    }

  TaskEventHost.add<MsgEvent>(message,MsgEvent::Leave);

  return ret;
 }

void WindowsHost::reset()
 {
  origin=Null;
  buf_dirty=true;
  track_flags=0;
  track_on=false;
  max_flag=false;
  hi=0;
  syshi=0;
 }

void WindowsHost::do_move(Pane pane)
 {
  const char *format="CCore::Video::Internal::WindowsHost::do_move(...) : #;";

  guardAlive(format);

  GuardPane(format,pane,max_size);

  unsigned pos_flags=WinNN::WindowPos_NoZOrder|WinNN::WindowPos_NoCopyBits|WinNN::WindowPos_NoRedraw|WinNN::WindowPos_DeferErase;

  SysGuard(format, WinNN::SetWindowPos(hWnd,(WinNN::HWindow)0,pane.x,pane.y,pane.dx,pane.dy,pos_flags) );
 }

WindowsHost::WindowsHost()
 {
 }

WindowsHost::~WindowsHost()
 {
  if( is_alive )
    {
     WinNN::DestroyWindow(hWnd);
    }
 }

 // msg boxes

void WindowsHost::AbortMsgBox(StrLen text)
 {
  ErrorMsgBox(text,"Abort"_c);
 }

void WindowsHost::ErrorMsgBox(StrLen text,StrLen title)
 {
  Sys::WCharString<> cap(text);
  Sys::WCharString<> cap_title(title);

  WinNN::MessageBoxW(HMainWindow,cap,cap_title,WinNN::MessageBox_Ok|WinNN::MessageBox_IconError);
 }

 // icons

void WindowsHost::SetAppIcon(Picture pict)
 {
  WindowClassObject.setIcon(pict);
 }

 // WinControl
 // create/destroy

void WindowsHost::createMain(Point max_size)
 {
  Pane work=GetWorkPane();

  Point size;

  size.x=RandomLen(work.dx);
  size.y=RandomLen(work.dy);

  size=Inf(size,max_size);

  Point base;

  base.x=RandomPos(work.dx-size.x);
  base.y=RandomPos(work.dy-size.y);

  Pane pane=Pane(work.getBase()+base,size);

  createMain(pane,max_size);
 }

void WindowsHost::createMain(Pane pane,Point max_size_)
 {
  const char *format="CCore::Video::Internal::WindowsHost::createMain(...) : #;";

  guardCreate(format,pane,max_size_);

  if( HMainWindow!=0 )
    {
     Printf(Exception,format,"main window is already created");
    }

  is_main=true;
  max_size=max_size_;

  reset();

  buf.setSize(max_size_);

  Sys::WCharString<16> temp(""_c);

  WinNN::HWindow hWnd=WinNN::CreateWindowExW(0,
                                             WinNN::MakeIntAtom(WindowClassObject.getAtom(format)),
                                             temp,
                                             WinNN::WindowStyle_Popup,
                                             pane.x,pane.y,pane.dx,pane.dy,
                                             0,0,0,
                                             (void *)this);

  SysGuard(format,hWnd!=0);
 }

void WindowsHost::create(Pane pane,Point max_size)
 {
  create(0,pane,max_size);
 }

void WindowsHost::create(WindowHost *parent,Pane pane,Point max_size_)
 {
  const char *format="CCore::Video::Internal::WindowsHost::create(...) : #;";

  guardCreate(format,pane,max_size_);

  is_main=false;
  max_size=max_size_;

  reset();

  buf.setSize(max_size_);

  WinNN::HWindow hParent;

  if( parent )
    {
     WindowsHost *ptr=dynamic_cast<WindowsHost *>(parent);

     if( !ptr )
       {
        Printf(Exception,format,"bad parent");
       }

     hParent=ptr->hWnd;

     if( hParent==0 )
       {
        Printf(Exception,format,"dead parent");
       }
    }
  else
    {
     hParent=HMainWindow;
    }

  Sys::WCharString<16> temp(""_c);

  WinNN::HWindow hWnd=WinNN::CreateWindowExW(0,
                                             WinNN::MakeIntAtom(WindowClassObject.getAtom(format)),
                                             temp,
                                             WinNN::WindowStyle_Popup,
                                             pane.x,pane.y,pane.dx,pane.dy,
                                             hParent,0,0,
                                             (void *)this);

  SysGuard(format,hWnd!=0);
 }

void WindowsHost::destroy()
 {
  const char *format="CCore::Video::Internal::WindowsHost::destroy() : #;";

  guardAlive(format);

  SysGuard(format, WinNN::DestroyWindow(hWnd) );
 }

 // operations

void WindowsHost::setTitle(StrLen title)
 {
  Sys::WCharString<> cap(title);

  const char *format="CCore::Video::Internal::WindowsHost::setTitle(...) : #;";

  guardAlive(format);

  if( is_main )
    SysGuard(format, WinNN::SetWindowTextW(hWnd,cap) );
 }

void WindowsHost::setMaxSize(Point max_size_)
 {
  const char *format="CCore::Video::Internal::WindowsHost::setMaxSize(...) : #;";

  GuardMaxSize(format,max_size_);

  buf.setSize(max_size_);

  max_size=max_size_;

  buf_dirty=true;
 }

bool WindowsHost::enableUserInput(bool en)
 {
  const char *format="CCore::Video::Internal::WindowsHost::enableUserInput(...) : #;";

  guardAlive(format);

  return !WinNN::EnableWindow(hWnd,en);
 }

void WindowsHost::display(CmdDisplay cmd_display)
 {
  const char *format="CCore::Video::Internal::WindowsHost::display(...) : #;";

  guardAlive(format);

  switch( cmd_display )
    {
     default:
     case CmdDisplay_Normal :
      {
       WinNN::ShowWindow(hWnd,WinNN::CmdShow_Normal); // error unavailable
      }
     break;

     case CmdDisplay_Minimized :
      {
       if( is_main )
         WinNN::ShowWindow(hWnd,WinNN::CmdShow_Minimized); // error unavailable
      }
     break;

     case CmdDisplay_Maximized :
      {
       if( !max_flag )
         {
          max_flag=true;

          restore=getPlace();

          Pane pane=GetWorkPane(restore);

          Replace_min(pane.dx,max_size.x);
          Replace_min(pane.dy,max_size.y);

          do_move(pane);

          WinNN::InvalidateRect(hWnd,0,true); // ignore error
         }

       WinNN::ShowWindow(hWnd,WinNN::CmdShow_Normal); // error unavailable
      }
     break;

     case CmdDisplay_Restore :
      {
       if( max_flag )
         {
          max_flag=false;

          do_move(restore);

          WinNN::InvalidateRect(hWnd,0,true); // ignore error
         }

       WinNN::ShowWindow(hWnd,WinNN::CmdShow_Normal); // error unavailable
      }
     break;
    }
 }

void WindowsHost::show()
 {
  const char *format="CCore::Video::Internal::WindowsHost::show() : #;";

  guardAlive(format);

  WinNN::ShowWindow(hWnd,WinNN::CmdShow_Show); // error unavailable
 }

void WindowsHost::hide()
 {
  const char *format="CCore::Video::Internal::WindowsHost::hide() : #;";

  guardAlive(format);

  WinNN::ShowWindow(hWnd,WinNN::CmdShow_Hide); // error unavailable
 }

 // drawing

void WindowsHost::update()
 {
  const char *format="CCore::Video::Internal::WindowsHost::update() : #;";

  guardAlive(format);

  SysGuard(format, WinNN::UpdateWindow(hWnd) );

  buf_dirty=false;
 }

void WindowsHost::invalidate(Pane pane,unsigned token_)
 {
  if( !pane ) return;

  token|=token_;

  WinNN::Rectangle rect;

  rect.left=pane.x;
  rect.right=pane.x+pane.dx;

  rect.top=pane.y;
  rect.bottom=pane.y+pane.dy;

  WinNN::InvalidateRect(hWnd,&rect,true); // ignore error

  buf_dirty=false;
 }

void WindowsHost::invalidate(unsigned token_)
 {
  token|=token_;

  WinNN::InvalidateRect(hWnd,0,true); // ignore error

  buf_dirty=false;
 }

ColorPlane WindowsHost::getDrawPlane()
 {
  return buf.getPlane();
 }

 // keyboard

void WindowsHost::setFocus()
 {
  const char *format="CCore::Video::Internal::WindowsHost::setFocus() : #;";

  guardAlive(format);

  WinNN::SetFocus(hWnd); // error unavailable
 }

 // mouse

Point WindowsHost::getMousePos()
 {
  WinNN::Point point_;

  if( WinNN::GetCursorPos(&point_) )
    {
     Point point(point_.x,point_.y);

     return point-origin;
    }

  return Null;
 }

void WindowsHost::captureMouse()
 {
  const char *format="CCore::Video::Internal::WindowsHost::captureMouse() : #;";

  guardAlive(format);

  WinNN::SetCapture(hWnd); // error unavailable
 }

void WindowsHost::releaseMouse()
 {
  const char *format="CCore::Video::Internal::WindowsHost::releaseMouse() : #;";

  guardAlive(format);

  SysGuard(format, WinNN::ReleaseCapture() );
 }

void WindowsHost::trackMouseHover()
 {
  BitSet(track_flags,WinNN::MouseTrack_Hover);

  hover_time=WinNN::HoverTimeDefault;

  setTrack();
 }

void WindowsHost::trackMouseHover(MSec time)
 {
  BitSet(track_flags,WinNN::MouseTrack_Hover);

  hover_time=+time;

  setTrack();
 }

void WindowsHost::untrackMouseHover()
 {
  BitClear(track_flags,WinNN::MouseTrack_Hover);
 }

void WindowsHost::trackMouseLeave()
 {
  BitSet(track_flags,WinNN::MouseTrack_Leave);

  setTrack();
 }

void WindowsHost::untrackMouseLeave()
 {
  BitClear(track_flags,WinNN::MouseTrack_Leave);
 }

void WindowsHost::setMouseShape(MouseShape mshape)
 {
  WinNN::SetCursor(GetCursor(mshape)); // error unavailable
 }

 // place

Pane WindowsHost::getPlace()
 {
  const char *format="CCore::Video::Internal::WindowsHost::getPlace() : #;";

  guardAlive(format);

  WinNN::Rectangle rect;

  SysGuard(format, WinNN::GetWindowRect(hWnd,&rect) );

  return ToPane(rect);
 }

void WindowsHost::setPlace(Pane pane)
 {
  max_flag=false;

  do_move(pane);
 }

 // clipboard

void WindowsHost::textToClipboard(PtrLen<const Char> text)
 {
  TextToClipboard obj(text);

  Clipboard cbd(hWnd);

  ulen len=obj.getLen();

  PutToClipboard put(len);

  obj.fill(put.getMem());

  put.commit(WinNN::ClipboardFormat_UnicodeText);
 }

void WindowsHost::textFromClipboard(Function<void (PtrLen<const Char>)> func)
 {
  Clipboard cbd(hWnd);
  GetFromClipboard get(WinNN::ClipboardFormat_UnicodeText);

  const Sys::WChar *ptr=static_cast<const Sys::WChar *>(get.getMem());
  ulen len=get.getLen()/sizeof (Sys::WChar);

  auto text=Range(ptr,len);

  for(auto r=text; +r ;++r)
    {
     if( !*r )
       {
        text=text.prefix(r);

        break;
       }
    }

  ULenSat outlen;

  Sys::FeedUnicode(text, [&outlen] (Char) { outlen+=1u; } );

  if( !outlen )
    {
     Printf(Exception,"CCore::Video::Internal::WindowsHost::textFromClipboard(...) : overflow");
    }

  Sys::TempBuf<Char> temp(outlen.value);

  Char *dst=temp;

  if( !dst )
    {
     Printf(Exception,"CCore::Video::Internal::WindowsHost::textFromClipboard(...) : no memory");
    }

  Sys::FeedUnicode(text, [&dst] (Char ch) { *(dst++)=ch; } );

  func(Range(+temp,outlen.value));
 }

} // namespace Internal
} // namespace Video
} // namespace CCore

