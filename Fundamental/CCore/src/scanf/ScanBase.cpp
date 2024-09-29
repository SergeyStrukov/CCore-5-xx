/* ScanBase.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/scanf/ScanBase.h>

namespace CCore {

/* class ScanBase */

void ScanBase::operator ++ ()
 {
  char ch=*buf;

  ++buf;

  text_pos.put(ch);

  if( !buf ) pump();
 }

} // namespace CCore


