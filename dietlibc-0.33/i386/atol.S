/*
  Copyright (C) 2001, 2002 Thomas M. Ogrisegg

  This is free software. You can redistribute and
  modify it under the terms of the GNU General Public
  Public License.

  atol.S
    i386 assembler implementation of atoi(3) & atol(3)
*/

.text
.globl atoi
.globl atol
.type atoi,@function
.type atol,@function

atoi:
atol:
	pushl %esi
	movl 0x8(%esp), %esi
	xorl %eax, %eax
	xorl %edx, %edx
	xorb %cl, %cl
	cld
.LSpace:
	lodsb
	or %al, %al
	jz .Lreturn
	cmpb $0x20, %al
	jng .LSpace
	cmpb $'+', %al
	jz .Lfe5
	cmpb $'-', %al
	jnz .Lfe3
	inc %cl
	jmp .Lfe5
.Lfe2:
	imul $0xa, %edx
	addl %eax, %edx
.Lfe5:
	lodsb
.Lfe3:
	cmpb $47, %al
	jng .Lout
	subb $('0'), %al
	cmpb $9, %al
	jng .Lfe2
.Lout:
	movl %edx, %eax
	or %cl, %cl
	jz .Lreturn
	negl %eax
.Lreturn:
	popl %esi
	ret
.size	atoi, . - atoi
.size	atol, . - atol
