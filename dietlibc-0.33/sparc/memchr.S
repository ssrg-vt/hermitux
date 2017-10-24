/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    memchr.S
      sparc assembler implementation of memchr(3)
*/

.text
.globl memchr
memchr:
	tst %o2
	be,a .Lret
	xor %o0, %o0, %o0
	ldub [%o0], %o3
	cmp %o3, %o1
	dec %o2
	bne,a memchr
	inc %o0
.Lret:
	retl
	nop
