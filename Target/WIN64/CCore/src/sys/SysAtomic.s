# SysAtomic.s
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

        .global _ZN5CCore3Sys6Atomic3SetEPVmm
        .global _ZN5CCore3Sys6Atomic3AddEPVmm
        .global _ZN5CCore3Sys6Atomic6TrySetEPVmmm

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Sys::Atomic::Set

_ZN5CCore3Sys6Atomic3SetEPVmm:

        movq    %rdx, %rax
        xchgq   %rax, (%rcx)

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Sys::Atomic::Add

_ZN5CCore3Sys6Atomic3AddEPVmm:

        movq    %rdx, %rax
   lock xaddq   %rax, (%rcx)

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Sys::Atomic::TrySet

_ZN5CCore3Sys6Atomic6TrySetEPVmmm:

        movq     %rdx, %rax
   lock cmpxchgq %r8 , (%rcx)

        ret

#-----------------------------------------------------------------------------------------



