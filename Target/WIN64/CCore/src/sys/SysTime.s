# SysTime.s
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

        .global _ZN5CCore3Sys12GetClockTimeEv

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Sys::GetClockTime

_ZN5CCore3Sys12GetClockTimeEv:

        rdtsc

        shlq   $32, %rdx
        orq    %rdx, %rax

        ret

#-----------------------------------------------------------------------------------------
