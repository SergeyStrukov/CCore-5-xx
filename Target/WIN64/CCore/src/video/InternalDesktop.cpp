/* InternalDesktop.cpp */
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

#include <CCore/inc/video/InternalDesktop.h>

namespace CCore {
namespace Video {
namespace Internal {

/* class WindowsDesktop */

bool WindowsDesktop::TestMsg()
 {
  WinNN::Msg msg;

  return WinNN::PeekMessageW(&msg,0,0,0,WinNN::PeekMessage_NoRemove);
 }

WindowsDesktop::WindowsDesktop()
 {
  h_event=WinNN::CreateEventW(0,false,false,0);
 }

WindowsDesktop::~WindowsDesktop()
 {
  if( h_event ) WinNN::CloseHandle(h_event);
 }

 // Desktop

WindowHost * WindowsDesktop::createHost()
 {
  return new WindowsHost();
 }

Point WindowsDesktop::getScreenSize()
 {
  return Point((Coord)WinNN::GetSystemMetrics(WinNN::SysMetric_DXScreen),(Coord)WinNN::GetSystemMetrics(WinNN::SysMetric_DYScreen));
 }

Pane WindowsDesktop::getMaxPane()
 {
  return GetWorkPane();
 }

bool WindowsDesktop::pump(unsigned lim)
 {
  for(WinNN::Msg msg; lim && WinNN::PeekMessageW(&msg,0,0,0,WinNN::PeekMessage_Remove) ;lim--)
    {
     if( msg.message==WinNN::WM_Quit )
       {
        TaskEventHost.add<MsgEvent>(msg.message,MsgEvent::Stop);

        return false;
       }

     WinNN::TranslateMessage(&msg);
     WinNN::DispatchMessageW(&msg);
    }

  return true;
 }

void WindowsDesktop::wait(TimeScope time_scope)
 {
  if( auto t=+time_scope.get() )
    {
     if( TestMsg() ) return;

     if( h_event )
       WinNN::MsgWaitForMultipleObjects(1,&h_event,false,t,WinNN::Wake_AllInput);
     else
       WinNN::MsgWaitForMultipleObjects(0,0,false,t,WinNN::Wake_AllInput);
    }
 }

void WindowsDesktop::interrupt()
 {
  if( h_event ) WinNN::SetEvent(h_event);
 }

} // namespace Internal
} // namespace Video
} // namespace CCore

