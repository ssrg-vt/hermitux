/*
   Copyright (C) 2002 Thomas M. Ogrisegg 

   __ltostr.S -- convert an integer into a string

 %eax = dividend
 %ebx = divisor
 %ecx = size of output-buffer
 %edi = output-buffer
 %ebp = if uppercase is set, then %ebp is 'A'-10 else %ebp is 'a'-10

*/

.text
.globl __ltostr
__ltostr:
	pushl %esi
	pushl %edi		# destination
	pushl %ebp
	pushl %ebx
	movl %esp, %eax
	movl 0x14(%eax), %edi
	movl 0x18(%eax), %ecx	# size
	movl 0x20(%eax), %ebx	# divisor
	movl 0x1c(%eax), %eax	# dividend
	decl %ecx
	movl %ecx, %esi
	movl $('A'-0xa), %ebp
	xorl %edx, %edx		# must be 0 -- used by idiv
	cmpl $0x0, 36(%esp)	# check for uppercase
	jnz .Lnext
	addl $0x20, %ebp	# set lowercase
.Lnext:
	idiv %ebx, %eax
	cmpb $0x9, %dl
	jg .Lnext2
	addb $'0', %dl
	jmp .Lstos
.Lnext2:
	addl %ebp, %edx
.Lstos:
	movb %dl, (%edi, %ecx)
	xorl %edx, %edx
	decl %ecx
	jz .Lout
	orl %eax, %eax
	jnz .Lnext
.Lout:
	cld
	movl %esi, %ebx
	leal 1(%edi, %ecx), %esi
	subl %ebx, %ecx
	negl %ecx
	movl %ecx, %eax
	repnz movsb
	movb $0x0, (%edi)
	popl %ebx
	popl %ebp
	popl %edi
	popl %esi
	ret
.size __ltostr, . - __ltostr
