/* PTPBootBase.cpp */
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

#include <CCore/inc/net/PTPBootBase.h>

#include <CCore/inc/net/PTPBase.h>

namespace CCore {
namespace Net {
namespace PTPBoot {

static_assert( MaxWriteDataLen==MaxInfoLen-DeltaWriteLen ,"CCore::Net::PTPBoot::MaxWriteDataLen is broken");

static_assert( DeltaWriteLen==SaveLenCounter<ServiceFunction,WriteInput>::SaveLoadLen ,"CCore::Net::PTPBoot::DeltaWriteLen is broken");

} // namespace PTPBoot
} // namespace Net
} // namespace CCore


