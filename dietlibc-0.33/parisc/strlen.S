/* strlen -- strlen(3) -- determine length of a string

  Copyright (C) 2002 Thomas M. Ogrisegg

  Created:              09/22/02
  Last updated:         09/23/02

  Remarks:
      Highperformance uxor-based strlen implementation. Even faster
      than the HP-UX libc implementation, which is also uxor-based.
*/

.LEVEL  1.1

.text
.globl strlen

strlen:
        .PROC
        .CALLINFO
        .ENTRY
        xor %r28, %r28, %r28
        ldi 3, %r19
        and,<> %r26, %r19, %r20
        b,n .Lstrlen17
        sub %r19, %r20, %r20
        addi 1, %r20, %r20
.Lstrlen23:
        ldb,ma 1(%r26), %r19
        cmpb,=,n %r0, %r19, .Lstrlen42
        addi 1, %r28, %r28
        addi,= -1, %r20, %r20
        b,n .Lstrlen23
.Lstrlen17:
        ldw,ma 4(%r26), %r19
        uxor,sbz %r19, %r0, %r0
        b .Lstrlen17
        addi 4, %r28, %r28
        addi -5, %r28, %r28
.Lstrlen72:
        extrs %r19, 7, 8, %r20
        addi 1, %r28, %r28
        cmpb,<> %r0, %r20, .Lstrlen72
        depw %r19, 23, 24, %r19
.Lstrlen42:
        bv,n %r0(%rp)
        .EXIT
        .PROCEND
        .EXPORT strlen

