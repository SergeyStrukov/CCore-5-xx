/* Integer.cpp */
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

#include <CCore/inc/math/Integer.h>

#include <CCore/inc/Exception.h>

namespace CCore {
namespace Math {

/* guards */

void GuardBitsOfOverflow()
 {
  Printf(Exception,"CCore::Math::Integer<...>::BitsOf::total(...) : overflow");
 }

} // namespace Math
} // namespace CCore


