/* TreeBase.h */
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

#ifndef CCore_inc_tree_TreeBase_h
#define CCore_inc_tree_TreeBase_h

#include <CCore/inc/Cmp.h>

namespace CCore {

/* functions */

void RadixTreeBrokenAbort();

/* concept TreeKeyTypes<K,KRef> */

template <class K,class KRef>
concept TreeKeyTypes = CmpableType<K> && NothrowCopyCtorType<KRef> && requires(const K &cobj,KRef kref)
 {
  Ground<CmpResult>( Cmp(kref,cobj) );
 } ;

} // namespace CCore

#endif


