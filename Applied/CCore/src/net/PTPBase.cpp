/* PTPBase.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/net/PTPBase.h>

namespace CCore {
namespace Net {
namespace PTP {

static_assert( DeltaInfoLen==SaveLenCounter<PacketPrefix3,Packet_RECALL3>::SaveLoadLen ,"CCore::Net::PTP::DeltaInfoLen is broken");

static_assert( DeltaInfoLen==SaveLenCounter<PacketPrefix4,Packet_RET4>::SaveLoadLen ,"CCore::Net::PTP::DeltaInfoLen is broken");

} // namespace PTP
} // namespace Net
} // namespace CCore


