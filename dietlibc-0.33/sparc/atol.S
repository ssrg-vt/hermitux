/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    atol.S
      sparc assembler implementation of atol(3)
*/

.text
.globl atol
.globl atoi
atoi:
atol:
	clr %o3
1:
	ldub [%o0], %o1
	tst %o1
	be .Lret
	cmp %o1, 0x20
	ble,a 1b
	inc %o0
	cmp %o1, '-'
	be,a .Lload
	mov 1, %o5
	clr %o5
2:
	subcc %o1, '0', %o1
	bneg .Lret
	cmp %o1, 0x9
	bg .Lret
	add %o3, %o3, %o4
	sll %o3, 3, %o3
	add %o3, %o4, %o3
	add %o1, %o3, %o3
.Lload:
	inc %o0
	b 2b
	ldub [%o0], %o1
.Lret:
	tst %o5
	bne,a .Lretl
	neg %o3
.Lretl:
	retl
	mov %o3, %o0
