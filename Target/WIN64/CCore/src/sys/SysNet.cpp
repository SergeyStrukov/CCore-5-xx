/* SysNet.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN64
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/sys/SysNet.h>
#include <CCore/inc/sys/SysWaitInternal.h>

#include <CCore/inc/Exception.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* GetPlanInitNode_...() */

namespace Private_SysNet {

/* class WSAInitExit */

class WSAInitExit : NoCopy
 {
  public:

   WSAInitExit()
    {
     WinNN::WSAInfo info;

     AbortIf( WinNN::WSAStartup(WinNN::WSAVersion_2_00,&info) ,"WinNN::WSAStartup()");
    }

   ~WSAInitExit()
    {
     AbortIf( WinNN::WSACleanup() ,"WinNN::WSACleanup()");
    }

   static const char * GetTag() { return "SysNet"; }
 };

PlanInitObject<WSAInitExit> Object CCORE_INITPRI_1 ;

/* WSAError() */

ErrorType WSAError()
 {
  if( auto ret=WinNN::WSAGetLastError() ) return ErrorType(ret);

  return Error_SysErrorFault;
 }

/* class Address */

class Address : NoCopy
 {
   static constexpr WinNN::ushortlen_t Len = 16 ;

  private:

   alignas(8) uint8 data[Len];

  private:

   void setFamily(uint16 address_family=WinNN::WSA_IPv4);

   void setPort(Net::UDPort udport);

   void setAddress(Net::IPAddress address);

  public:

   Address() {}

   explicit Address(Net::UDPort udport);

   explicit Address(Net::UDPoint udpoint);

   // get

   void get(Net::UDPoint &udpoint);

   // raw

   uint8 * getPtr() { return data; }

   const uint8 * getPtr() const { return data; }

   WinNN::ushortlen_t getLen() const { return Len; }
 };

void Address::setFamily(uint16 address_family)
 {
  BufPutDev dev(data);

  dev.use<LeOrder>(address_family);
 }

void Address::setPort(Net::UDPort udport)
 {
  BufPutDev dev(data+2);

  dev.use<BeOrder>(udport);
 }

void Address::setAddress(Net::IPAddress address)
 {
  BufPutDev dev(data+4);

  dev(address);
 }

Address::Address(Net::UDPort udport)
 : data()
 {
  setFamily();
  setPort(udport);
 }

Address::Address(Net::UDPoint udpoint)
 : data()
 {
  setFamily();
  setPort(udpoint.port);
  setAddress(udpoint.address);
 }

void Address::get(Net::UDPoint &udpoint)
 {
  BufGetDev dev(data+2);

  dev.use<BeOrder>(udpoint.port,udpoint.address);
 }

/* struct Timeout */

struct Timeout : WinNN::WSATimeout
 {
  explicit Timeout(MSec timeout)
   {
    auto val=+timeout;

    sec=val/1000;
    usec=(val%1000)*1000;
   }
 };

} // namespace Private_SysNet

using namespace Private_SysNet;

PlanInitNode * GetPlanInitNode_SysNet() { return &Object; }

/* struct UDPSocket */

auto UDPSocket::Open(Net::UDPort udport) noexcept -> OpenType
 {
  OpenType ret;

  ret.sockid=WinNN::WSASocketW(WinNN::WSA_IPv4,WinNN::WSA_Datagram,WinNN::WSA_UDP,0,0,WinNN::WSA_AsyncIO);

  if( ret.sockid==WinNN::InvalidSocket )
    {
     ret.error=WSAError();

     return ret;
    }

  Address address(udport);

  if( WinNN::bind(ret.sockid,address.getPtr(),address.getLen()) )
    {
     ret.error=WSAError();

     WinNN::closesocket(ret.sockid); // ignore unprobable error

     ret.sockid=WinNN::InvalidSocket;

     return ret;
    }

  ret.error=NoError;

  return ret;
 }

ErrorType UDPSocket::Close(Type sockid) noexcept
 {
  if( WinNN::closesocket(sockid) ) return WSAError();

  return NoError;
 }

WaitResult UDPSocket::Wait_in(Type sockid,MSec timeout_) noexcept
 {
  WinNN::WSASockSet set;

  set.count=1;
  set.set[0]=sockid;

  Timeout timeout(timeout_);

  switch( WinNN::select(0,&set,0,0,&timeout) )
    {
     case 0 : return Wait_timeout;
     case 1 : return WaitResult(0);

     default: return Wait_error;
    }
 }

ErrorType UDPSocket::Outbound(Type sockid,Net::UDPoint dst,PtrLen<const uint8> data) noexcept
 {
  if( data.len>=WinNN::InvalidUShortLen ) return ErrorType(WinNN::ErrorInvalidParameter);

  Address address(dst);

  auto result=WinNN::sendto(sockid,data.ptr,(WinNN::ushortlen_t)data.len,0,address.getPtr(),address.getLen());

  if( result==WinNN::InvalidUShortLen ) return WSAError();

  if( result!=data.len ) return Error_Socket;

  return NoError;
 }

auto UDPSocket::Inbound(Type sockid,PtrLen<uint8> buf) noexcept -> InResult
 {
  InResult ret;

  Address address;
  WinNN::ushortlen_t address_len=address.getLen();

  WinNN::ushortlen_t len=WinNN::CapLen(buf.len);

  auto result=WinNN::recvfrom(sockid,buf.ptr,len,0,address.getPtr(),&address_len);

  if( result==WinNN::InvalidUShortLen )
    {
     ret.len=0;
     ret.error=WSAError();
    }
  else if( address_len!=address.getLen() || result>len )
    {
     ret.len=0;
     ret.error=Error_Socket;
    }
  else
    {
     address.get(ret.src);

     ret.len=result;
     ret.error=NoError;
    }

  return ret;
 }

/* struct AsyncUDPSocket */

struct AsyncUDPSocket::AsyncState
 {
  WinNN::WSAOverlapped olap;
  WinNN::WSABuf buf;

  Address address;
  WinNN::ushortlen_t address_len;
 };

auto AsyncUDPSocket::Open(Net::UDPort udport) noexcept -> OpenType
 {
  OpenType ret;

  ret.sockid=WinNN::WSASocketW(WinNN::WSA_IPv4,WinNN::WSA_Datagram,WinNN::WSA_UDP,0,0,WinNN::WSA_AsyncIO);

  if( ret.sockid==WinNN::InvalidSocket )
    {
     ret.error=WSAError();

     return ret;
    }

  Address address(udport);

  if( WinNN::bind(ret.sockid,address.getPtr(),address.getLen()) )
    {
     ret.error=WSAError();

     WinNN::closesocket(ret.sockid); // ignore unprobable error

     ret.sockid=WinNN::InvalidSocket;

     return ret;
    }

  ret.error=NoError;

  return ret;
 }

ErrorType AsyncUDPSocket::Close(Type sockid) noexcept
 {
  if( WinNN::closesocket(sockid) ) return WSAError();

  return NoError;
 }

auto AsyncUDPSocket::StartOutbound(Type sockid,Async async,Net::UDPoint dst,PtrLen<const uint8> data) noexcept -> OutResult
 {
  OutResult ret;

  if( data.len>=WinNN::InvalidUShortLen )
    {
     ret.pending=false;
     ret.error=ErrorType(WinNN::ErrorInvalidParameter);

     return ret;
    }

  async->olap.internal=0;
  async->olap.internal_hi=0;
  async->olap.offset=0;

  async->buf.ptr=const_cast<uint8 *>(data.ptr);
  async->buf.len=(WinNN::ushortlen_t)data.len;

  WinNN::ushortlen_t ret_len;

  Address address(dst);

  if( WinNN::WSASendTo(sockid,&async->buf,1,&ret_len,0,address.getPtr(),address.getLen(),&async->olap,0) )
    {
     auto error=WSAError();

     if( error==WinNN::WSAErrorIOPending )
       {
        ret.pending=true;
        ret.error=NoError;
       }
     else
       {
        ret.pending=false;
        ret.error=error;
       }
    }
  else
    {
     ret.pending=false;
     ret.error=(ret_len!=data.len)?Error_Socket:NoError;
    }

  return ret;
 }

ErrorType AsyncUDPSocket::CompleteOutbound(Type sockid,Async async) noexcept
 {
  WinNN::ushortlen_t ret_len;
  WinNN::flags_t flags;

  if( WinNN::WSAGetOverlappedResult(sockid,&async->olap,&ret_len,false,&flags) )
    {
     return (ret_len!=async->buf.len)?Error_Socket:NoError;
    }
  else
    {
     return WSAError();
    }
 }

auto AsyncUDPSocket::StartInbound(Type sockid,Async async,PtrLen<uint8> buf) noexcept -> StartInResult
 {
  StartInResult ret;

  async->olap.internal=0;
  async->olap.internal_hi=0;
  async->olap.offset=0;

  async->buf.ptr=buf.ptr;
  async->buf.len=WinNN::CapLen(buf.len);

  async->address_len=async->address.getLen();

  WinNN::ushortlen_t ret_len;
  WinNN::flags_t flags=0;

  if( WinNN::WSARecvFrom(sockid,&async->buf,1,&ret_len,&flags,async->address.getPtr(),&async->address_len,&async->olap,0) )
    {
     auto error=WSAError();

     if( error==WinNN::WSAErrorIOPending )
       {
        ret.pending=true;

        ret.result.src=Net::UDPoint();
        ret.result.len=0;
        ret.result.error=NoError;
       }
     else
       {
        ret.pending=false;

        ret.result.src=Net::UDPoint();
        ret.result.len=0;
        ret.result.error=error;
       }
    }
  else
    {
     ret.pending=false;

     if( ret_len>async->buf.len || async->address_len!=async->address.getLen() )
       {
        ret.result.src=Net::UDPoint();
        ret.result.len=0;
        ret.result.error=Error_Socket;
       }
     else
       {
        async->address.get(ret.result.src);

        ret.result.len=ret_len;
        ret.result.error=NoError;
       }
    }

  return ret;
 }

auto AsyncUDPSocket::CompleteInbound(Type sockid,Async async) noexcept -> InResult
 {
  InResult ret;

  WinNN::ushortlen_t ret_len;
  WinNN::flags_t flags;

  if( WinNN::WSAGetOverlappedResult(sockid,&async->olap,&ret_len,false,&flags) )
    {
     if( ret_len>async->buf.len || async->address_len!=async->address.getLen() )
       {
        ret.src=Net::UDPoint();
        ret.len=0;
        ret.error=Error_Socket;
       }
     else
       {
        async->address.get(ret.src);

        ret.len=ret_len;
        ret.error=NoError;
       }
    }
  else
    {
     ret.src=Net::UDPoint();
     ret.len=0;
     ret.error=WSAError();
    }

  return ret;
 }

/* class AsyncUDPSocketWait::Engine */

class AsyncUDPSocketWait::Engine : public WaitBase<AsyncUDPSocket::AsyncState,WaitNet>
 {
  public:

   explicit Engine(ulen async_count) : WaitBase<AsyncUDPSocket::AsyncState,WaitNet>(async_count) {}

   ~Engine() {}
 };

/* struct AsyncUDPSocketWait */

ErrorType AsyncUDPSocketWait::init(ulen async_count) noexcept
 {
  if( async_count>MaxAsyncs ) return ErrorType(WinNN::ErrorInvalidParameter);

  SilentReportException report;

  try
    {
     obj=new Engine(async_count);

     asyncs=obj->getAsyncs();

     return NoError;
    }
  catch(CatchType)
    {
     return ErrorType(WinNN::ErrorNotEnoughMemory);
    }
 }

void AsyncUDPSocketWait::exit() noexcept
 {
  delete Replace_null(obj);

  asyncs=0;
 }

bool AsyncUDPSocketWait::addWait(ulen index) noexcept
 {
  return obj->addWait(index);
 }

bool AsyncUDPSocketWait::delWait(ulen index) noexcept
 {
  return obj->delWait(index);
 }

WaitResult AsyncUDPSocketWait::wait(MSec timeout) noexcept
 {
  return obj->wait(timeout);
 }

void AsyncUDPSocketWait::interrupt() noexcept
 {
  obj->interrupt();
 }

WaitResult AsyncUDPSocketWait::waitAll(MSec timeout) noexcept
 {
  return obj->waitAll(timeout);
 }

} // namespace Sys
} // namespace CCore


