/* HFSBase.cpp */
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

#include <CCore/inc/net/HFSBase.h>

#include <CCore/inc/net/PTPBase.h>

namespace CCore {
namespace Net {
namespace HFS {

static_assert( MaxReadLen==MaxInfoLen-DeltaReadLen ,"CCore::Net::HFS::MaxReadLen is broken");

static_assert( MaxWriteLen==MaxInfoLen-DeltaWriteLen ,"CCore::Net::HFS::MaxWriteLen is broken");

static_assert( DeltaReadLen==SaveLenCounter<Result>::SaveLoadLen+SaveLenCounter<ReadOutput>::SaveLoadLen ,"CCore::Net::HFS::DeltaReadLen is broken");

static_assert( DeltaWriteLen==SaveLenCounter<ServiceFunction>::SaveLoadLen+SaveLenCounter<WriteInput>::SaveLoadLen ,"CCore::Net::HFS::DeltaWriteLen is broken");

} // namespace HFS
} // namespace Net
} // namespace CCore


