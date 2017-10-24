/*****************************************************************************
* Copyright (C) 2002 Andre McCurdy  <armccurdy@yahoo.co.uk>
*
* "derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm"
*
* This program is free software. you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY, without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
******************************************************************************
*
* History:
*
* 2002/02/12:  Andre McCurdy <armccurdy@yahoo.co.uk>
*   - Original created.
*
* 2002/02/13:  Andre McCurdy <armccurdy@yahoo.co.uk>
*   - Rewrote MD5Update in assembler (saves 52 bytes).
*   - Added guard so we don't compile anything for Big-Endian targets.
*
* 2002/02/22:  Andre McCurdy <armccurdy@yahoo.co.uk>
*   - Added new function MD5Update (a wrapper for the original version) which
*     checks for correct 32bit alignment of the input data pointer.
*   - Original MD5Update function renamed __MD5Update.
*   - __MD5Transform symbol no longer exported.
*
* 2002/02/25:  Andre McCurdy <armccurdy@yahoo.co.uk>
*   - MD5Update re-written in assembler (saves 12 bytes + lots of cycles :-)
*
* 2002/03/07:  Andre McCurdy <armccurdy@yahoo.co.uk>
*   - _Finally_ fixed hashing from an unaligned buffer in all cases :-)
*
******************************************************************************
*
* Notes:
*
* This code is believed to be Position Independent (ie can be safely used
* as part of a shared library).
*
*****************************************************************************/

#include <endian.h>

#if (__BYTE_ORDER == __LITTLE_ENDIAN)

   .global MD5Init
   .global MD5Update

   .text
#ifdef __ARM_EABI__
   .align  4
#else		
   .align  2
#endif
	
    @ --
    @ void MD5Init (MD5_CTX* context);
    @ --

MD5Init:

    adr     r1, 1f                          @ r1 = base address of MD5InitData array
    ldmia   r1, { r1 - r3, r12 }            @ load 4 elements from MD5InitData array
    stmia   r0, { r1 - r3, r12 }            @ store into MD5 context->state[0..3]
    mov     r1, #0
    str     r1, [r0, #0x10]                 @ initial count[0] = 0
    str     r1, [r0, #0x14]                 @ initial count[1] = 0
    mov     pc, lr                          @ return

1: .word    0x67452301                      @ initial MD5 context->state[0]
   .word    0xefcdab89                      @ initial MD5 context->state[1]
   .word    0x98badcfe                      @ initial MD5 context->state[2]
   .word    0x10325476                      @ initial MD5 context->state[3]


    @ --
    @ void MD5Update (MD5_CTX* context, const uint8_t* buf, signed int len);
    @ --

MD5Update:

    stmdb   sp!, { r4 - r8, lr }
    add     r4, r0, #(6 * 4)                @ r4 = &context->buffer[0]
    ldmdb   r4, { r0, r3 }                  @ r0 = count[0], r3 = count[1]
    mov     r5, r1                          @ r5 = input
    mov     r6, r0, lsr #3                  @ r6 = (context->count[0] >> 3)
    and     r6, r6, #0x3f                   @ r6 = byteIndex = ((context->count[0] >> 3) & 0x3f)
    adds    r0, r0, r2, lsl #3
    adc     r3, r3, r2, lsr #29
    stmdb   r4, { r0, r3 }                  @ context->count += (inputBytes * 8)
    rsb     r7, r6, #64                     @ r7 = (64 - byteIndex) == partLen
    subs    r8, r2, r7                      @ r8 = (inputBytes - partLen)
    add     r0, r4, r6
    bcc     2f
    mov     r2, r7
    bl      memcpy                          @ memcpy (&context->buffer[byteIndex], input, partLen);
    sub     r0, r4, #(6 * 4)
    mov     r1, r4
    mov     r2, #1
    bl      __MD5Transform                  @ __MD5Transform (context->state, context->buffer, 1);
    sub     r0, r4, #(6 * 4)
    add     r1, r5, r7
    mov     r2, r8, lsr #6                  @ r2 = len = ((inputBytes - partLen) / 64)
    tst     r1, #3                          @ if (input & 0x03)
    adr     lr, 1f
    beq     __MD5Transform                  @ __MD5Transform (context->state, &input[partLen], len);
    bne     __UnalignedMD5Transform         @ else __UnalignedMD5Transform (context->state, &input[partLen], len);
1:  mov     r0, r4
    bic     r2, r8, #0x3f
    add     r1, r2, r7
    add     r1, r1, r5
    sub     r2, r8, r2
2:  ldmia   sp!, { r4 - r8, lr }
    b       memcpy                          @ classic tail-call optimisation...


    @ --
    @ static void __MD5Transform (uint32_t *buf, const uint32_t *in, int repeat);
    @ --

MD5MagicData:

1: .word     0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee
   .word     0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, ( 3f- 4f-4)
   .word     0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, ( 3f- 4f-4)
   .word     0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, ( 3f- 4f-4)
   .word     0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, ( 4f- 4f-4)
   .word     0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, ( 5f- 9f-4)
   .word     0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, ( 6f- 9f-4)
   .word     0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, ( 7f- 9f-4)
   .word     0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, ( 9f- 9f-4)
   .word     0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, (10f-14f-4)
   .word     0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, (11f-14f-4)
   .word     0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, (12f-14f-4)
   .word     0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, (14f-14f-4)
   .word     0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, (15f-19f-4)
   .word     0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, (16f-19f-4)
   .word     0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391, (17f-19f-4)
   .word     0x6e4120A9, 0x20657264, 0x7543634d, 0x00796472, (19f-19f-4)

__MD5Transform:

    cmp     r2, #0
    moveq   pc, lr                          @ return if (repeat == 0)
    stmdb   sp!, { r0, r2, r4 - r11, lr }   @ stack 'buf', 'repeat' and all callee saved registers
    ldmia   r0,  { r2 - r5 }                @ r2 = a, r3 = b, r4 = c, r5 = d
2:  adr     r0, 1b                          @ r0 = base address of MD5MagicData
    ldmia   r0!, { r6 - r9 }                @ load first 4 elements from MD5MagicData
3:  ldmia   r1!, { r10 - r12, lr }          @ r10 = in[i], r11 = in[i+1], r12 = in[i+2], lr = in[i+3]

    add     r10, r10, r6                    @ r10 = in[i] + MD5MagicData[i]
    eor     r6, r4, r5                      @ r6  =   (c ^ d)
    and     r6, r6, r3                      @ r6  =  ((c ^ d) & b)
    eor     r6, r6, r5                      @ r6  = (((c ^ d) & b) ^ d)
    add     r2, r2, r6                      @ a   = (((c ^ d) & b) ^ d) + a
    add     r2, r2, r10                     @ a   = (((c ^ d) & b) ^ d) + a + (in[i] + MD5MagicData[i])
    add     r2, r3, r2, ROR #(32 -  7)      @ a   = b + (a rotated left by  7 places)

    add     r11, r11, r7                    @ r11 = in[i+1] + MD5MagicData[i+1]
    eor     r7, r3, r4                      @ r7  =   (b ^ c)
    and     r7, r7, r2                      @ r7  =  ((b ^ c) & a)
    eor     r7, r7, r4                      @ r7  = (((b ^ c) & a) ^ c)
    add     r5, r5, r7                      @ d   = (((b ^ c) & a) ^ c) + d
    add     r5, r5, r11                     @ d   = (((b ^ c) & a) ^ c) + d + (in[i+1] + MD5MagicData[i+1])
    add     r5, r2, r5, ROR #(32 - 12)      @ d   = a + (d rotated left by 12 places)

    add     r12, r12, r8                    @ r12 = in[i+2] + MD5MagicData[i+2]
    eor     r8, r2, r3                      @ r8  =   (a ^ b)
    and     r8, r8, r5                      @ r8  =  ((a ^ b) & d)
    eor     r8, r8, r3                      @ r8  = (((a ^ b) & d) ^ b)
    add     r4, r4, r8                      @ c   = (((a ^ b) & d) ^ b) + c
    add     r4, r4, r12                     @ c   = (((a ^ b) & d) ^ b) + c + (in[i+2] + MD5MagicData[i+2])
    add     r4, r5, r4, ROR #(32 - 17)      @ c   = d + (c rotated left by 17 places)

    add     lr, lr, r9                      @ lr  = in[i+3] + MD5MagicData[i+3]
    eor     r9, r5, r2                      @ r9  =   (d ^ a)
    and     r9, r9, r4                      @ r9  =  ((d ^ a) & c)
    eor     r9, r9, r2                      @ r9  = (((d ^ a) & c) ^ a)
    add     r3, r3, r9                      @ b   = (((d ^ a) & c) ^ a) + b
    add     r3, r3, lr                      @ b   = (((d ^ a) & c) ^ a) + b + (in[i+3] + MD5MagicData[i+3])
    add     r3, r4, r3, ROR #(32 - 22)      @ b   = c + (b rotated left by 22 places)

    ldmia   r0!, { r6 - r10 }               @ load next 4 elements from MD5MagicData
    add     pc, pc, r10                     @ and branch to next state

4:  ldr     r10, [r1, #-(15*4)]             @ r10 = in[ 1]
    ldr     r11, [r1, #-(10*4)]             @ r11 = in[ 6]
    ldr     r12, [r1, #-( 5*4)]             @ r12 = in[11]
    ldr     lr,  [r1, #-(16*4)]             @ lr  = in[ 0]
    b       8f
5:  ldr     r10, [r1, #-(11*4)]             @ r10 = in[ 5]
    ldr     r11, [r1, #-( 6*4)]             @ r11 = in[10]
    ldr     r12, [r1, #-( 1*4)]             @ r12 = in[15]
    ldr     lr,  [r1, #-(12*4)]             @ lr  = in[ 4]
    b       8f
6:  ldr     r10, [r1, #-( 7*4)]             @ r10 = in[ 9]
    ldr     r11, [r1, #-( 2*4)]             @ r11 = in[14]
    ldr     r12, [r1, #-(13*4)]             @ r12 = in[ 3]
    ldr     lr,  [r1, #-( 8*4)]             @ lr  = in[ 8]
    b       8f
7:  ldr     r10, [r1, #-( 3*4)]             @ r10 = in[13]
    ldr     r11, [r1, #-(14*4)]             @ r11 = in[ 2]
    ldr     r12, [r1, #-( 9*4)]             @ r12 = in[ 7]
    ldr     lr,  [r1, #-( 4*4)]             @ lr  = in[12]

8:  add     r10, r10, r6                    @ r10 = in[x] + MD5MagicData[i]
    eor     r6, r3, r4                      @ r6  =   (b ^ c)
    and     r6, r6, r5                      @ r6  =  ((b ^ c) & d)
    eor     r6, r6, r4                      @ r6  = (((b ^ c) & d) ^ c)
    add     r2, r2, r6                      @ a   = (((b ^ c) & d) ^ c) + a
    add     r2, r2, r10                     @ a   = (((b ^ c) & d) ^ c) + a + (in[x] + MD5MagicData[i])
    add     r2, r3, r2, ROR #(32 -  5)      @ a   = b + (a rotated left by  5 places)

    add     r11, r11, r7                    @ r11 = in[x] + MD5MagicData[i+1]
    eor     r7, r2, r3                      @ r7  =   (a ^ b)
    and     r7, r7, r4                      @ r7  =  ((a ^ b) & c)
    eor     r7, r7, r3                      @ r7  = (((a ^ b) & c) ^ b)
    add     r5, r5, r7                      @ d   = (((a ^ b) & c) ^ b) + d
    add     r5, r5, r11                     @ d   = (((a ^ b) & c) ^ b) + d + (in[x] + MD5MagicData[i+1])
    add     r5, r2, r5, ROR #(32 -  9)      @ d   = a + (d rotated left by  9 places)

    add     r12, r12, r8                    @ r12 = in[x] + MD5MagicData[i+2]
    eor     r8, r5, r2                      @ r8  =   (d ^ a)
    and     r8, r8, r3                      @ r8  =  ((d ^ a) & b)
    eor     r8, r8, r2                      @ r8  = (((d ^ a) & b) ^ a)
    add     r4, r4, r8                      @ c   = (((d ^ a) & b) ^ a) + c
    add     r4, r4, r12                     @ c   = (((d ^ a) & b) ^ a) + c + (in[x] + MD5MagicData[i+2])
    add     r4, r5, r4, ROR #(32 - 14)      @ c   = d + (c rotated left by 14 places)

    add     lr, lr, r9                      @ lr  = in[x] + MD5MagicData[i+3]
    eor     r9, r4, r5                      @ r9  =   (c ^ d)
    and     r9, r9, r2                      @ r9  =  ((c ^ d) & a)
    eor     r9, r9, r5                      @ r9  = (((c ^ d) & a) ^ d)
    add     r3, r3, r9                      @ b   = (((c ^ d) & a) ^ d) + b
    add     r3, r3, lr                      @ b   = (((c ^ d) & a) ^ d) + b + (in[x] + MD5MagicData[i+3])
    add     r3, r4, r3, ROR #(32 - 20)      @ b   = c + (b rotated left by 20 places)

    ldmia   r0!, { r6 - r10 }               @ load next 4 elements from MD5MagicData
    add     pc, pc, r10                     @ and branch to next state

9:  ldr     r10, [r1, #-(11*4)]             @ r10 = in[ 5]
    ldr     r11, [r1, #-( 8*4)]             @ r11 = in[ 8]
    ldr     r12, [r1, #-( 5*4)]             @ r12 = in[11]
    ldr     lr,  [r1, #-( 2*4)]             @ lr  = in[14]
    b       13f
10: ldr     r10, [r1, #-(15*4)]             @ r10 = in[ 1]
    ldr     r11, [r1, #-(12*4)]             @ r11 = in[ 4]
    ldr     r12, [r1, #-( 9*4)]             @ r12 = in[ 7]
    ldr     lr,  [r1, #-( 6*4)]             @ lr  = in[10]
    b       13f
11: ldr     r10, [r1, #-( 3*4)]             @ r10 = in[13]
    ldr     r11, [r1, #-(16*4)]             @ r11 = in[ 0]
    ldr     r12, [r1, #-(13*4)]             @ r12 = in[ 3]
    ldr     lr,  [r1, #-(10*4)]             @ lr  = in[ 6]
    b       13f
12: ldr     r10, [r1, #-( 7*4)]             @ r10 = in[ 9]
    ldr     r11, [r1, #-( 4*4)]             @ r11 = in[12]
    ldr     r12, [r1, #-( 1*4)]             @ r12 = in[15]
    ldr     lr,  [r1, #-(14*4)]             @ lr  = in[ 2]

13: add     r10, r10, r6                    @ r10 = in[x] + MD5MagicData[i]
    add     r2, r2, r10                     @ a  += in[x] + MD5MagicData[i]
    eor     r6, r3, r4                      @ r6  = (b ^ c)
    eor     r10, r6, r5                     @ r10 = (b ^ c ^ d)
    add     r2, r2, r10                     @ a  += (b ^ c ^ d)
    add     r2, r3, r2, ROR #(32 -  4)      @ a   = b + (a rotated left by  4 places)

    add     r11, r11, r7                    @ r11 = in[x] + MD5MagicData[i+1]
    eor     r7, r2, r6                      @ r7  = (a ^ b ^ c)
    add     r5, r5, r7                      @ d   = (a ^ b ^ c) + d
    add     r5, r5, r11                     @ d   = (a ^ b ^ c) + d + (in[x] + MD5MagicData[i+1])
    add     r5, r2, r5, ROR #(32 - 11)      @ d   = a + (d rotated left by 11 places)

    add     r12, r12, r8                    @ r12 = in[x] + MD5MagicData[i+2]
    add     r4, r4, r12                     @ c  += in[x] + MD5MagicData[i+2]
    eor     r8, r5, r2                      @ r8  = (d ^ a)
    eor     r12, r8, r3                     @ r12 = (d ^ a ^ b)
    add     r4, r4, r12                     @ c  += (d ^ a ^ b)
    add     r4, r5, r4, ROR #(32 - 16)      @ c   = d + (c rotated left by 16 places)

    add     lr, lr, r9                      @ lr  = in[x] + MD5MagicData[i+3]
    eor     r9, r4, r8                      @ r9  = (c ^ d ^ a)
    add     r3, r3, r9                      @ b   = (c ^ d ^ a) + b
    add     r3, r3, lr                      @ b   = (c ^ d ^ a) + b + (in[x] + MD5MagicData[i+3])
    add     r3, r4, r3, ROR #(32 - 23)      @ b   = c + (b rotated left by 23 places)

    ldmia   r0!, { r6 - r10 }               @ load next 4 elements from MD5MagicData
    add     pc, pc, r10                     @ and branch to next state

14: ldr     r10, [r1, #-(16*4)]             @ r10 = in[ 0]
    ldr     r11, [r1, #-( 9*4)]             @ r11 = in[ 7]
    ldr     r12, [r1, #-( 2*4)]             @ r12 = in[14]
    ldr     lr,  [r1, #-(11*4)]             @ lr  = in[ 5]
    b       18f
15: ldr     r10, [r1, #-( 4*4)]             @ r10 = in[12]
    ldr     r11, [r1, #-(13*4)]             @ r11 = in[ 3]
    ldr     r12, [r1, #-( 6*4)]             @ r12 = in[10]
    ldr     lr,  [r1, #-(15*4)]             @ lr  = in[ 1]
    b       18f
16: ldr     r10, [r1, #-( 8*4)]             @ r10 = in[ 8]
    ldr     r11, [r1, #-( 1*4)]             @ r11 = in[15]
    ldr     r12, [r1, #-(10*4)]             @ r12 = in[ 6]
    ldr     lr,  [r1, #-( 3*4)]             @ lr  = in[13]
    b       18f
17: ldr     r10, [r1, #-(12*4)]             @ r10 = in[ 4]
    ldr     r11, [r1, #-( 5*4)]             @ r11 = in[11]
    ldr     r12, [r1, #-(14*4)]             @ r12 = in[ 2]
    ldr     lr,  [r1, #-( 7*4)]             @ lr  = in[ 9]

18: add     r10, r10, r6                    @ r10 = in[x] + MD5MagicData[i]
    add     r2, r2, r10                     @ a  += in[x] + MD5MagicData[i]
    mvn     r6, r5                          @ r6  =      (    ~d)
    orr     r6, r6, r3                      @ r6  =      (b | ~d)
    eor     r6, r6, r4                      @ r6  = (c ^ (b | ~d))
    add     r2, r2, r6                      @ a  += (c ^ (b | ~d))
    add     r2, r3, r2, ROR #(32 -  6)      @ a   = b + (a rotated left by  6 places)

    add     r11, r11, r7                    @ r11 = in[x] + MD5MagicData[i]
    add     r5, r5, r11                     @ d  += in[x] + MD5MagicData[i]
    mvn     r7, r4                          @ r7  =      (    ~c)
    orr     r7, r7, r2                      @ r7  =      (a | ~c)
    eor     r7, r7, r3                      @ r7  = (b ^ (a | ~c))
    add     r5, r5, r7                      @ d  += (b ^ (a | ~c))
    add     r5, r2, r5, ROR #(32 - 10)      @ d   = a + (d rotated left by 10 places)

    add     r12, r12, r8                    @ r12 = in[x] + MD5MagicData[i]
    add     r4, r4, r12                     @ c  += in[x] + MD5MagicData[i]
    mvn     r8, r3                          @ r8  =      (    ~b)
    orr     r8, r8, r5                      @ r8  =      (d | ~b)
    eor     r8, r8, r2                      @ r8  = (a ^ (d | ~b))
    add     r4, r4, r8                      @ c  += (a ^ (d | ~b))
    add     r4, r5, r4, ROR #(32 - 15)      @ c   = d + (c rotated left by 15 places)

    add     lr, lr, r9                      @ lr  = in[x] + MD5MagicData[i]
    add     r3, r3, lr                      @ b  += in[x] + MD5MagicData[i]
    mvn     r9, r2                          @ r9  =      (    ~a)
    orr     r9, r9, r4                      @ r9  =      (c | ~a)
    eor     r9, r9, r5                      @ r9  = (d ^ (c | ~a))
    add     r3, r3, r9                      @ b  += (d ^ (c | ~a))
    add     r3, r4, r3, ROR #(32 - 21)      @ b   = c + (b rotated left by 21 places)

    ldmia   r0!, { r6 - r10 }               @ load next 4 elements from MD5MagicData
    add     pc, pc, r10                     @ and branch to next state

19: ldmia   sp, { r0, r10 }                 @ fetch 'buf' and 'repeat' from the stack
    ldmia   r0, { r6 - r9 }                 @ r6 = buf[0], r7 = buf[1], r8 = buf[2], r9 = buf[3] (original values..)
    subs    r10, r10, #1
    strne   r10, [sp, #4]
    add     r2, r6, r2
    add     r3, r7, r3
    add     r4, r8, r4
    add     r5, r9, r5
    stmia   r0, { r2 - r5 }
    bne     2b
    ldmia   sp!, { r0, r2, r4 - r11, pc }   @ restore all callee saved registers and return


    @ --
    @ static void __UnalignedMD5Transform (uint32_t *buf, const uint8_t *in, int repeat);
    @ --

__UnalignedMD5Transform:

    stmdb   sp!, { r4 - r7, lr }
    movs    r4, r2, lsl #6                  @ r4 = (repeat * 64)
    ldmeqia sp!, { r4 - r7, pc }            @ if  ((repeat * 64) == 0) return;
    mov     r5, r0                          @ r5 = state
    mov     r6, r1                          @ r6 = in
    sub     sp, sp, #256                    @ create 256 byte temp buffer
1:  cmp     r4, #256
    mov     r0, sp
    movcc   r7, r4
    movcs   r7, #256                        @ r7 = (blockCount * 64) = min((repeat * 64), (4 * 64))
    mov     r1, r6
    mov     r2, r7
    bl      memcpy                          @ memcpy (blockBuffer, in, (blockCount * 64));
    mov     r0, r5
    mov     r1, sp
    mov     r2, r7, lsr #6
    bl      __MD5Transform                  @ __MD5Transform (state, blockBuffer, blockCount);
    subs    r4, r4, #256                    @ (repeat * 64) -= (4 * 64);
    add     r6, r6, #256                    @ in += (4 * 64);
    bgt     1b                              @ loop while (repeat > 0)
    add     sp, sp, #256                    @ return temp buffer to stack
    ldmia   sp!, { r4 - r7, pc }            @ return


#endif   /* __BYTE_ORDER == __LITTLE_ENDIAN */


