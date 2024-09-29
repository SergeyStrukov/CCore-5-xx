/* PTPConBase.cpp */
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

#include <CCore/inc/net/PTPConBase.h>

#include <CCore/inc/net/PTPBase.h>

namespace CCore {
namespace Net {
namespace PTPCon {

static_assert( MaxReadDataLen==MaxInfoLen-DeltaReadLen ,"CCore::Net::PTPCon::MaxReadDataLen is broken");

static_assert( MaxWriteDataLen==MaxInfoLen-DeltaWriteLen ,"CCore::Net::PTPCon::MaxWriteDataLen is broken");

static_assert( DeltaReadLen==SaveLenCounter<Result,ReadOutput>::SaveLoadLen ,"CCore::Net::PTPCon::DeltaReadLen is broken");

static_assert( DeltaWriteLen==SaveLenCounter<ServiceFunction,WriteInput>::SaveLoadLen ,"CCore::Net::PTPCon::DeltaWriteLen is broken");

} // namespace PTPCon
} // namespace Net
} // namespace CCore


