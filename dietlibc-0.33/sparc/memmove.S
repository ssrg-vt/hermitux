/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    memmove.S
      sparc assembler implementation of memmove(3)
*/

.text
.globl memmove
memmove:
	mov %o0, %o5
	mov -1, %o4
	cmp %o0, %o1
	bg,a .Lcopy
	add %o4, %o2, %o4
	inc %o4
.Lcopy:
	subcc %o2, 1, %o2
	bl .Lret
	cmp %o0, %o1
	ldub [%o1+%o4], %o3
	stb %o3, [%o0+%o4]
	bg,a .Lnext
	sub %o4, 2, %o4
.Lnext:
	b .Lcopy
	inc %o4
.Lret:
	retl
	mov %o5, %o0
