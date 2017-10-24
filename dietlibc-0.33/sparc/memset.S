/*
    Copyright (C) 2003 Thomas M. Ogrisegg <tom@hi-tek.fnord.at>

    This is free software. You can redistribute and
    modify it under the terms of the GNU General Public
    Public License.

    memset.S
      sparc assembler implementation of memset(3)
*/

.text
.globl memset
memset:
	subcc %o2, 1, %o2
	bge,a memset
	stb %o1, [%o0+%o2]
	retl
	nop
