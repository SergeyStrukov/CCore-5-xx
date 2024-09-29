# Quick.s
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

        .global _ZN5CCore5Quick9ScanLSBitEm
        .global _ZN5CCore5Quick9ScanMSBitEm
        .global _ZN5CCore5Quick10ByteSwap16Et
        .global _ZN5CCore5Quick10ByteSwap32Ej
        .global _ZN5CCore5Quick10ByteSwap64Em

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::ScanLSBit

_ZN5CCore5Quick9ScanLSBitEm:

        bsfq    %rcx, %rax

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::ScanMSBit

_ZN5CCore5Quick9ScanMSBitEm:

        bsrq    %rcx, %rax

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::ByteSwap16

_ZN5CCore5Quick10ByteSwap16Et:

        movzwl  %cx, %eax
        xchgb   %al, %ah

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::ByteSwap32

_ZN5CCore5Quick10ByteSwap32Ej:

        movl    %ecx, %eax
        bswap   %eax

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::ByteSwap64

_ZN5CCore5Quick10ByteSwap64Em:

        movq    %rcx, %rax
        bswap   %rax

        ret

#-----------------------------------------------------------------------------------------

        .global _ZN5CCore5Quick11UIntMulFuncImE3MulC1Emm
        .global _ZN5CCore5Quick11UIntMulFuncImE3DivEmmm
        .global _ZN5CCore5Quick11UIntMulFuncImE3ModEmmm
        .global _ZN5CCore5Quick11UIntMulFuncImE6MulDivEmmm
        .global _ZN5CCore5Quick11UIntMulFuncImE6DivModC1Emmm
        .global _ZN5CCore5Quick11UIntMulFuncImE6ModMulEmmm
        .global _ZN5CCore5Quick11UIntMulFuncImE6ModMacEmmmm

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::UIntMulFunc<uint64>::Mul::Mul

_ZN5CCore5Quick11UIntMulFuncImE3MulC1Emm:

        movq    %rdx, %rax
        mulq    %r8

        movq    %rdx, (%rcx)
        movq    %rax, 8(%rcx)

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::UIntMulFunc<uint64>::Div

_ZN5CCore5Quick11UIntMulFuncImE3DivEmmm:

        movq    %rdx, %rax
        movq    %rcx, %rdx
        divq    %r8

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::UIntMulFunc<uint64>::Mod

_ZN5CCore5Quick11UIntMulFuncImE3ModEmmm:

        movq    %rdx, %rax
        movq    %rcx, %rdx
        divq    %r8

        movq    %rdx, %rax

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::UIntMulFunc<uint64>::MulDiv

_ZN5CCore5Quick11UIntMulFuncImE6MulDivEmmm:

        movq    %rcx, %rax
        mulq    %rdx
        divq    %r8

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::UIntMulFunc<uint64>::DivMod::DivMod

_ZN5CCore5Quick11UIntMulFuncImE6DivModC1Emmm:

        movq    %r8, %rax
        divq    %r9

        movq    %rax, (%rcx)
        movq    %rdx, 8(%rcx)

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::UIntMulFunc<uint64>::ModMul

_ZN5CCore5Quick11UIntMulFuncImE6ModMulEmmm:

        movq    %rdx, %rax
        mulq    %rcx
        divq    %r8

        movq    %rdx, %rax

        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Quick::UIntMulFunc<uint64>::ModMac

_ZN5CCore5Quick11UIntMulFuncImE6ModMacEmmmm:

        movq    %rdx, %rax
        mulq    %r8

        addq    %rcx, %rax
        adcq    $0, %rdx

        divq    %r9

        movq    %rdx, %rax

        ret

#-----------------------------------------------------------------------------------------

