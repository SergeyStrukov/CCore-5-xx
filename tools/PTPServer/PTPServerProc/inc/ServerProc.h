/* ServerProc.h */
//----------------------------------------------------------------------------------------
//
//  Project: PTPServerProc 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2021 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef PTPServer_ServerProc_h
#define PTPServer_ServerProc_h

#include <CCore/inc/ReadCon.h>

#include "SupportProc.h"
#include "PTPConProc.h"
#include "HFSProc.h"

namespace PTPServerProc {

//
//   Net::PTP::ServerProc is called in the single task context
//
//   PacketMultipointDevice::InboundProc is called in the single task context
//
//   UDPMultipointDevice task
//
//   get, put is called in the main task context
//

/* classes */

class ServerProc;

/* class ServerProc */

class ServerProc : public Net::PTP::ServerProc , ServerProcBase
 {
   // UDPMultipointDevice task

   SupportProc support_proc;

   PTPConProc con_proc;

   HFSProc hfs_proc;

   // main task

   ReadCon con;

   const char *ins_text;

  private:

   // ServerProcBase

   virtual void session(XPoint point);

   virtual bool exist(ServiceIdType service_id,FunctionIdType function_id);

   // Net::PTP::ServerProc

   virtual void inbound(XPoint point,TransIndex idx,Packet<uint8> packet,PtrLen<const uint8> client_info);

   virtual void tick();

  public:

   ServerProc(StrLen ptp_server_name,bool exit_on_close);

   ~ServerProc();

   // main task

   Symbol get();

   bool try_get(Symbol &ret) { return con.try_get(ret); }

   bool get(MSec timeout,Symbol &ret) { return con.get(timeout,ret); }

   bool get(TimeScope time_scope,Symbol &ret) { return con.get(time_scope,ret); }

   void put(OneOfTypes<char,Symbol> auto ch) { con.put(ch); }

   void put(const char *str,ulen len) { con.put(str,len); }

   void put(StrLen str) { con.put(str); }
 };

} // namespace PTPServerProc

#endif


