# IntelRandom.s
#-----------------------------------------------------------------------------------------
#
#  Project: CCore 4.01
#
#  Tag: Target/WIN64
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2022 Sergey Strukov. All rights reserved.
#
#-----------------------------------------------------------------------------------------

        .text

        .global _ZN5CCore7Crypton11IntelRandom4nextEv

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # uint64 CCore::Crypton::IntelRandom::next() next

_ZN5CCore7Crypton11IntelRandom4nextEv:

L1:
        rdrand  %rax
        jnc     L1

        ret

#-----------------------------------------------------------------------------------------
