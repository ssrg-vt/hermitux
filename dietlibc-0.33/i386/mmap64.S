#include <dietfeatures.h>
#include <syscalls.h>
#include <errno.h>

#ifdef __DYN_LIB
#ifndef WANT_THREAD_SAFE
#define WANT_THREAD_SAFE
#endif
#endif

.text
.global mmap64
.type mmap64,@function
mmap64:
/* man is this ugly! */
  push	%ebp
  push	%edi
  push	%esi
  push	%ebx
  movl	%esp, %edi
  movl	0x28(%edi), %eax
  movl	0x2c(%edi), %edx
  testl	$0xfff, %eax	/* offset in pages */
  jnz	.Leinval
  shrdl	$12, %edx, %eax
  shrl	$12, %edx
  jnz	.Leinval
  movl	%eax, %ebp
  movl	0x14(%edi),%ebx
  movl	0x18(%edi),%ecx
  movl	0x1c(%edi),%edx
  movl	0x20(%edi),%esi
  movl	0x24(%edi),%edi
  movl	$__NR_mmap2,%eax
  int	$0x80
  cmp	$-124,%eax
  jbe	.Lnoerror
  neg	%eax
.Lerror:
#ifdef WANT_THREAD_SAFE
  movl	%eax,%ebx
#ifdef __DYN_LIB
  call	__errno_location@PLT
#else
  call	__errno_location
#endif
  movl	%ebx,(%eax)
  orl	$-1,%eax
#else
  mov	%eax,errno
  sbb	%eax,%eax               # eax = eax - eax - CY = -1
#endif
.Lnoerror:
  pop	%ebx
  pop	%esi
  pop	%edi
  pop	%ebp
  ret
.Leinval:
  movl $EINVAL,%eax
  jmp .Lerror
.Lende2:
.size mmap64,.Lende2-mmap64
