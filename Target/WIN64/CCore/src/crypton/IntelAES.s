# IntelAES.s
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

#-----------------------------------------------------------------------------------------

        .global _ZN5CCore7Crypton15IntelAES128Func5IntelEPKhPh

        .p2align 4,,15

#
# void CCore::Crypton::IntelAES128Func::Intel(const uint8 *round_key,uint8 *block) noexcept
#

_ZN5CCore7Crypton15IntelAES128Func5IntelEPKhPh:

        movdqu  (%rdx), %xmm0

        movdqu      (%rcx), %xmm1
        pxor    %xmm1, %xmm0

        movdqu  1*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  2*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  3*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  4*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  5*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  6*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  7*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  8*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  9*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  10*16(%rcx), %xmm1
        aesenclast  %xmm1, %xmm0

        movdqu  %xmm0, (%rdx)

        ret

#-----------------------------------------------------------------------------------------

        .global _ZN5CCore7Crypton15IntelAES192Func5IntelEPKhPh

        .p2align 4,,15

#
# void CCore::Crypton::IntelAES192Func::Intel(const uint8 *round_key,uint8 *block) noexcept
#

_ZN5CCore7Crypton15IntelAES192Func5IntelEPKhPh:

        movdqu  (%rdx), %xmm0

        movdqu      (%rcx), %xmm1
        pxor    %xmm1, %xmm0

        movdqu  1*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  2*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  3*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  4*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  5*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  6*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  7*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  8*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  9*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  10*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  11*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  12*16(%rcx), %xmm1
        aesenclast  %xmm1, %xmm0

        movdqu  %xmm0, (%rdx)

        ret

#-----------------------------------------------------------------------------------------

        .global _ZN5CCore7Crypton15IntelAES256Func5IntelEPKhPh

        .p2align 4,,15

#
# void CCore::Crypton::IntelAES256Func::Intel(const uint8 *round_key,uint8 *block) noexcept
#

_ZN5CCore7Crypton15IntelAES256Func5IntelEPKhPh:

        movdqu  (%rdx), %xmm0

        movdqu      (%rcx), %xmm1
        pxor    %xmm1, %xmm0

        movdqu  1*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  2*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  3*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  4*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  5*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  6*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  7*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  8*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  9*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  10*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  11*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  12*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  13*16(%rcx), %xmm1
        aesenc  %xmm1, %xmm0

        movdqu  14*16(%rcx), %xmm1
        aesenclast  %xmm1, %xmm0

        movdqu  %xmm0, (%rdx)

        ret

#-----------------------------------------------------------------------------------------

        .global _ZN5CCore7Crypton22IntelAESInverse128Func5IntelEPKhPh

        .p2align 4,,15

#
# void CCore::Crypton::IntelAESInverse128Func::Intel(const uint8 *round_key,uint8 *block) noexcept
#

_ZN5CCore7Crypton22IntelAESInverse128Func5IntelEPKhPh:

        movdqu  (%rdx), %xmm0

        movdqu      (%rcx), %xmm1
        pxor    %xmm1, %xmm0

        movdqu  1*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  2*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  3*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  4*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  5*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  6*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  7*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  8*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  9*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  10*16(%rcx), %xmm1
        aesdeclast  %xmm1, %xmm0

        movdqu  %xmm0, (%rdx)

        ret

#-----------------------------------------------------------------------------------------

        .global _ZN5CCore7Crypton22IntelAESInverse192Func5IntelEPKhPh

        .p2align 4,,15

#
# void CCore::Crypton::IntelAESInverse192Func::Intel(const uint8 *round_key,uint8 *block) noexcept
#

_ZN5CCore7Crypton22IntelAESInverse192Func5IntelEPKhPh:

        movdqu  (%rdx), %xmm0

        movdqu      (%rcx), %xmm1
        pxor    %xmm1, %xmm0

        movdqu  1*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  2*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  3*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  4*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  5*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  6*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  7*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  8*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  9*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  10*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  11*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  12*16(%rcx), %xmm1
        aesdeclast  %xmm1, %xmm0

        movdqu  %xmm0, (%rdx)

        ret

#-----------------------------------------------------------------------------------------

        .global _ZN5CCore7Crypton22IntelAESInverse256Func5IntelEPKhPh

        .p2align 4,,15

#
# void CCore::Crypton::IntelAESInverse256Func::Intel(const uint8 *round_key,uint8 *block) noexcept
#

_ZN5CCore7Crypton22IntelAESInverse256Func5IntelEPKhPh:

        movdqu  (%rdx), %xmm0

        movdqu      (%rcx), %xmm1
        pxor    %xmm1, %xmm0

        movdqu  1*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  2*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  3*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  4*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  5*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  6*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  7*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  8*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  9*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  10*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  11*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  12*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  13*16(%rcx), %xmm1
        aesdec  %xmm1, %xmm0

        movdqu  14*16(%rcx), %xmm1
        aesdeclast  %xmm1, %xmm0

        movdqu  %xmm0, (%rdx)

        ret

#-----------------------------------------------------------------------------------------

