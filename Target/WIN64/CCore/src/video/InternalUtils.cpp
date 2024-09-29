/* InternalUtils.cpp */
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

#include <CCore/inc/video/InternalUtils.h>

#include <CCore/inc/Exception.h>
#include <CCore/inc/PrintError.h>

#include <cstdlib>

namespace CCore {
namespace Video {
namespace Internal {

/* SysGuard() */

void SysGuardFailed(const char *format,Sys::ErrorType error)
 {
  Printf(Exception,format,PrintError(error));
 }

/* GetWorkPane() */

Pane GetWorkPane(Pane pane)
 {
  WinNN::Rectangle rect;

  rect.left=pane.x;
  rect.top=pane.y;
  rect.right=pane.x+pane.dx;
  rect.bottom=pane.y+pane.dy;

  SysGuard("CCore::Video::Internal::GetWorkPane() : #;", WinNN::SystemParametersInfoW(WinNN::SPA_getWorkArea,0,&rect,0) );

  return ToPane(rect);
 }

/* struct MsgEvent */

void MsgEvent::Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc)
 {
  auto id_Code=info.addEnum_uint32("WinMsgCode"_str)
                   .addValueName(WinNN::WM_Create,"WM_Create"_str)
                   .addValueName(WinNN::WM_Destroy,"WM_Destroy"_str)
                   .addValueName(WinNN::WM_Move,"WM_Move"_str)
                   .addValueName(WinNN::WM_Size,"WM_Size"_str)
                   .addValueName(WinNN::WM_Activate,"WM_Activate"_str)
                   .addValueName(WinNN::WM_SetFocus,"WM_SetFocus"_str)
                   .addValueName(WinNN::WM_KillFocus,"WM_KillFocus"_str)
                   .addValueName(WinNN::WM_Enable,"WM_Enable"_str)
                   .addValueName(WinNN::WM_Paint,"WM_Paint"_str)
                   .addValueName(WinNN::WM_Close,"WM_Close"_str)
                   .addValueName(WinNN::WM_Quit,"WM_Quit"_str)
                   .addValueName(WinNN::WM_QueryOpen,"WM_QueryOpen"_str)
                   .addValueName(WinNN::WM_EraseBackground,"WM_EraseBackground"_str)
                   .addValueName(WinNN::WM_EndSession,"WM_EndSession"_str)
                   .addValueName(WinNN::WM_ShowWindow,"WM_ShowWindow"_str)
                   .addValueName(WinNN::WM_ActivateApp,"WM_ActivateApp"_str)
                   .addValueName(WinNN::WM_CancelMode,"WM_CancelMode"_str)
                   .addValueName(WinNN::WM_SetCursor,"WM_SetCursor"_str)
                   .addValueName(WinNN::WM_MouseActivate,"WM_MouseActivate"_str)
                   .addValueName(WinNN::WM_GetMinMaxInfo,"WM_GetMinMaxInfo"_str)
                   .addValueName(WinNN::WM_WindowPosChanging,"WM_WindowPosChanging"_str)
                   .addValueName(WinNN::WM_WindowPosChanged,"WM_WindowPosChanged"_str)
                   .addValueName(WinNN::WM_ContextMenu,"WM_ContextMenu"_str)
                   .addValueName(WinNN::WM_NcCreate,"WM_NcCreate"_str)
                   .addValueName(WinNN::WM_NcDestroy,"WM_NcDestroy"_str)
                   .addValueName(WinNN::WM_NcHitTest,"WM_NcHitTest"_str)
                   .addValueName(WinNN::WM_NcPaint,"WM_NcPaint"_str)
                   .addValueName(WinNN::WM_NcActivate,"WM_NcActivate"_str)
                   .addValueName(WinNN::WM_SyncPaint,"WM_SyncPaint"_str)
                   .addValueName(WinNN::WM_NcMouseMove,"WM_NcMouseMove"_str)
                   .addValueName(WinNN::WM_NcLButtonDown,"WM_NcLButtonDown"_str)
                   .addValueName(WinNN::WM_NcLButtonUp,"WM_NcLButtonUp"_str)
                   .addValueName(WinNN::WM_NcLButtonDClick,"WM_NcLButtonDClick"_str)
                   .addValueName(WinNN::WM_NcRButtonDown,"WM_NcRButtonDown"_str)
                   .addValueName(WinNN::WM_NcRButtonUp,"WM_NcRButtonUp"_str)
                   .addValueName(WinNN::WM_NcRButtonDClick,"WM_NcRButtonDClick"_str)
                   .addValueName(WinNN::WM_NcMButtonDown,"WM_NcMButtonDown"_str)
                   .addValueName(WinNN::WM_NcMButtonUp,"WM_NcMButtonUp"_str)
                   .addValueName(WinNN::WM_NcMButtonDClick,"WM_NcMButtonDClick"_str)
                   .addValueName(WinNN::WM_KeyDown,"WM_KeyDown"_str)
                   .addValueName(WinNN::WM_KeyUp,"WM_KeyUp"_str)
                   .addValueName(WinNN::WM_Char,"WM_Char"_str)
                   .addValueName(WinNN::WM_DeadChar,"WM_DeadChar"_str)
                   .addValueName(WinNN::WM_SysKeyDown,"WM_SysKeyDown"_str)
                   .addValueName(WinNN::WM_SysKeyUp,"WM_SysKeyUp"_str)
                   .addValueName(WinNN::WM_SysChar,"WM_SysChar"_str)
                   .addValueName(WinNN::WM_SysDeadChar,"WM_SysDeadChar"_str)
                   .addValueName(WinNN::WM_InitDialog,"WM_InitDialog"_str)
                   .addValueName(WinNN::WM_Command,"WM_Command"_str)
                   .addValueName(WinNN::WM_SysCommand,"WM_SysCommand"_str)
                   .addValueName(WinNN::WM_InitMenu,"WM_InitMenu"_str)
                   .addValueName(WinNN::WM_InitMenuPopup,"WM_InitMenuPopup"_str)
                   .addValueName(WinNN::WM_MenuSelect,"WM_MenuSelect"_str)
                   .addValueName(WinNN::WM_EnterIdle,"WM_EnterIdle"_str)
                   .addValueName(WinNN::WM_UninitMenuPopup,"WM_UninitMenuPopup"_str)
                   .addValueName(WinNN::WM_MouseMove,"WM_MouseMove"_str)
                   .addValueName(WinNN::WM_LButtonDown,"WM_LButtonDown"_str)
                   .addValueName(WinNN::WM_LButtonUp,"WM_LButtonUp"_str)
                   .addValueName(WinNN::WM_LButtonDClick,"WM_LButtonDClick"_str)
                   .addValueName(WinNN::WM_RButtonDown,"WM_RButtonDown"_str)
                   .addValueName(WinNN::WM_RButtonUp,"WM_RButtonUp"_str)
                   .addValueName(WinNN::WM_RButtonDClick,"WM_RButtonDClick"_str)
                   .addValueName(WinNN::WM_MButtonDown,"WM_MButtonDown"_str)
                   .addValueName(WinNN::WM_MButtonUp,"WM_MButtonUp"_str)
                   .addValueName(WinNN::WM_MButtonDClick,"WM_MButtonDClick"_str)
                   .addValueName(WinNN::WM_MouseWheel,"WM_MouseWheel"_str)
                   .addValueName(WinNN::WM_EnterMenuLoop,"WM_EnterMenuLoop"_str)
                   .addValueName(WinNN::WM_ExitMenuLoop,"WM_ExitMenuLoop"_str)
                   .addValueName(WinNN::WM_Sizing,"WM_Sizing"_str)
                   .addValueName(WinNN::WM_CaptureChanged,"WM_CaptureChanged"_str)
                   .addValueName(WinNN::WM_Moving,"WM_Moving"_str)
                   .addValueName(WinNN::WM_EnterSizeMove,"WM_EnterSizeMove"_str)
                   .addValueName(WinNN::WM_ExitSizeMove,"WM_ExitSizeMove"_str)
                   .addValueName(WinNN::WM_NcMouseHover,"WM_NcMouseHover"_str)
                   .addValueName(WinNN::WM_MouseHover,"WM_MouseHover"_str)
                   .addValueName(WinNN::WM_NcMouseLeave,"WM_NcMouseLeave"_str)
                   .addValueName(WinNN::WM_MouseLeave,"WM_MouseLeave"_str)
                   .getId();

  auto id_Flag=info.addEnum_uint8("WinMsgFlag"_str)
                   .addValueName(Entry,"Entry"_str,EventMarker_Up)
                   .addValueName(Leave,"Leave"_str,EventMarker_Down)
                   .addValueName(Stop,"Stop"_str,EventMarker_Stop)
                   .getId();

  auto id=info.addStruct("WinMsgEvent"_str)
              .addField_uint32("time"_str,Offset_time)
              .addField_uint16("id"_str,Offset_id)
              .addField_enum_uint32(id_Code,"code"_str,Offset_code)
              .addField_enum_uint8(id_Flag,"flag"_str,Offset_flag)
              .getId();

  desc.setStructId(info,id);
 }

/* struct TickEvent */

void TickEvent::Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc)
 {
  auto id_Flag=info.addEnum_uint8("WinTickFlag"_str)
                   .addValueName(Entry,"Entry"_str,EventMarker_Up)
                   .addValueName(Leave,"Leave"_str,EventMarker_Down)
                   .getId();

  auto id=info.addStruct("WinTickEvent"_str)
              .addField_uint32("time"_str,Offset_time)
              .addField_uint16("id"_str,Offset_id)
              .addField_enum_uint8(id_Flag,"flag"_str,Offset_flag)
              .getId();

  desc.setStructId(info,id);
 }

/* class WindowBitmap */

WindowBitmap::WindowBitmap(Point size_)
 : size(size_)
 {
  const char *format="CCore::Video::Internal::WindowBitmap::WindowBitmap(...) : #;";

  if( size<=Null )
    {
     Printf(Exception,format,"bad size");
    }

  WinNN::BitmapInfo info;

  info.header.cb=sizeof (WinNN::BitmapInfoHeader);
  info.header.dx=size.x;
  info.header.dy=-size.y;
  info.header.planes=1;
  info.header.bpp=32;
  info.header.compression=WinNN::Bitmap_RGB;

  info.header.image_size=0;
  info.header.x_ppm=0;
  info.header.y_ppm=0;
  info.header.colors_used=1;
  info.header.colors_important=0;

  info.colors[0].blue=0;
  info.colors[0].green=0;
  info.colors[0].red=0;
  info.colors[0].alpha=0;

  hBitmap=WinNN::CreateDIBSection(0,&info,WinNN::DIB_RGBColors,&mem,0,0);

  SysGuard(format,hBitmap!=0);
 }

WindowBitmap::~WindowBitmap()
 {
  if( hBitmap ) WinNN::DeleteObject(WinNN::ToGDObject(hBitmap));
 }

/* class WindowBuf */

void WindowBuf::setSize(Point new_size,bool first_time)
 {
  const char *format="CCore::Video::Internal::WindowBuf::setSize(...) : #;";

  if( new_size<=Null )
    {
     Printf(Exception,format,"bad size");
    }

  WinNN::BitmapInfo info;

  info.header.cb=sizeof (WinNN::BitmapInfoHeader);
  info.header.dx=new_size.x;
  info.header.dy=-new_size.y;
  info.header.planes=1;
  info.header.bpp=32;
  info.header.compression=WinNN::Bitmap_RGB;

  info.header.image_size=0;
  info.header.x_ppm=0;
  info.header.y_ppm=0;
  info.header.colors_used=1;
  info.header.colors_important=0;

  info.colors[0].blue=0;
  info.colors[0].green=0;
  info.colors[0].red=0;
  info.colors[0].alpha=0;

  void *new_mem;

  WinNN::HGDObject new_bmp=WinNN::ToGDObject(WinNN::CreateDIBSection(0,&info,WinNN::DIB_RGBColors,&new_mem,0,0));

  SysGuard(format,new_bmp!=0);

  if( first_time )
    {
     old_bmp=WinNN::SelectObject(hGD,new_bmp);
    }
  else
    {
     WinNN::SelectObject(hGD,new_bmp);

     WinNN::DeleteObject(bmp);
    }

  bmp=new_bmp;
  mem=new_mem;
  size=new_size;
 }

WindowBuf::WindowBuf()
 {
  const char *format="CCore::Video::Internal::WindowBuf::WindowBuf(...) : #;";

  hGD=WinNN::CreateCompatibleDC(0);

  SysGuard(format,hGD!=0);
 }

WindowBuf::~WindowBuf()
 {
  if( mem )
    {
     WinNN::SelectObject(hGD,old_bmp);

     WinNN::DeleteObject(bmp);
    }

  WinNN::DeleteDC(hGD);
 }

void WindowBuf::setSize(Point new_size)
 {
  if( mem )
    {
     if( new_size!=size ) setSize(new_size,false);
    }
  else
    {
     setSize(new_size,true);
    }
 }

void WindowBuf::draw(WinNN::HGDevice dstGD,Pane pane)
 {
  const char *format="CCore::Video::Internal::WindowBuf::draw(...) : #;";

  if( !mem )
    {
     Printf(Exception,format,"no buf");
    }

  SysGuard(format, WinNN::BitBlt(dstGD,pane.x,pane.y,pane.dx,pane.dy,hGD,pane.x,pane.y,WinNN::RasterOp_Copy) );
 }

/* class Clipboard */

Clipboard::Clipboard(WinNN::HWindow hWnd)
 {
  const char *format="CCore::Video::Internal::Clipboard::Clipboard(...) : #;";

  SysGuard(format, WinNN::OpenClipboard(hWnd) );
 }

Clipboard::~Clipboard()
 {
  WinNN::CloseClipboard();
 }

/* class PutToClipboard */

PutToClipboard::PutToClipboard(ulen len)
 {
  const char *format="CCore::Video::Internal::PutToClipboard::PutToClipboard(...) : #;";

  SysGuard(format, WinNN::EmptyClipboard() );

  h_mem=WinNN::GlobalAlloc(WinNN::GMemMovable,len);

  SysGuard(format,h_mem!=0);

  mem=WinNN::GlobalLock(h_mem);
 }

PutToClipboard::~PutToClipboard()
 {
  if( h_mem )
    {
     WinNN::GlobalUnlock(h_mem);

     WinNN::GlobalFree(h_mem);
    }
 }

void PutToClipboard::commit(unsigned cbd_format)
 {
  const char *format="CCore::Video::Internal::PutToClipboard::commit(...) : #;";

  if( h_mem )
    {
     WinNN::GlobalUnlock(h_mem);

     mem=0;

     if( !WinNN::SetClipboardData(cbd_format,h_mem) )
       {
        Sys::ErrorType error=Sys::NonNullError();

        mem=WinNN::GlobalLock(h_mem);

        SysGuardFailed(format,error);
       }

     h_mem=0;
    }
 }

/* class GetFromClipboard */

GetFromClipboard::GetFromClipboard(unsigned cbd_format)
 {
  const char *format="CCore::Video::Internal::GetFromClipboard::GetFromClipboard(...) : #;";

  h_mem=WinNN::GetClipboardData(cbd_format);

  SysGuard(format,h_mem!=0);

  mem=WinNN::GlobalLock(h_mem);
  len=WinNN::GlobalSize(h_mem);
 }

GetFromClipboard::~GetFromClipboard()
 {
  WinNN::GlobalUnlock(h_mem);
 }

/* class TextToClipboard */

void TextToClipboard::Feed(PtrLen<const Char> text,FuncArgType<Sys::WChar> auto func)
 {
  while( +text )
    {
     Char ch=*text;

     if( ch=='\r' || ch=='\n' )
       {
        if( text.len>=2 && ch=='\r' && text[1]=='\n' )
          {
           text+=2;
          }
        else
          {
           ++text;
          }

        if( +text )
          {
           func('\r');
           func('\n');
          }
       }
     else
       {
        ++text;

        if( Sys::IsSurrogate(ch) )
          {
           Sys::SurrogateCouple couple(ch);

           func(couple.hi);
           func(couple.lo);
          }
        else
          {
           func((Sys::WChar)ch);
          }
       }
    }

  func(0);
 }

ulen TextToClipboard::getLen() const
 {
  ULenSat len;

  Feed(text, [&len] (Sys::WChar) { len+=sizeof (Sys::WChar); } );

  if( !len )
    {
     Printf(Exception,"CCore::Video::Internal::TextToClipboard::getLen() : overflow");
    }

  return len.value;
 }

void TextToClipboard::fill(void *mem) const
 {
  Sys::WChar *dst=static_cast<Sys::WChar *>(mem);

  Feed(text, [&dst] (Sys::WChar ch) { *(dst++)=ch; } );
 }

} // namespace Internal
} // namespace Video
} // namespace CCore

