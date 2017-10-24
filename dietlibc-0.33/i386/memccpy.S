/*
  Copyright (C) 2002 Thomas M. Ogrisegg

  This is free software. You can redistribute and
  modify it under the terms of the GNU General Public
  Public License.

  memccpy.S
    i386 assembler implementation of memccpy(3)
*/

.text
.global memccpy
.type   memccpy,@function
memccpy:
		pushl %esi
		pushl %edi

		movl %esp, %ecx
		movl 0x0c(%ecx), %edi
		movl 0x10(%ecx), %esi
		movl 0x14(%ecx), %edx
		movl 0x18(%ecx), %ecx
		cld
		jecxz	.Lerr
.Lloop:
		lodsb
		stosb
		cmp %al, %dl
		jz .Lout
		decl %ecx
		jnz .Lloop
.Lerr:
		xorl %edi, %edi
.Lout:
		movl %edi, %eax
		popl %edi
		popl %esi
		ret
.Lende:
.size    memccpy,.Lende-memccpy
