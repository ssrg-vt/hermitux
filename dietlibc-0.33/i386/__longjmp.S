#include <setjmp.h>

.text
.global __longjmp
.type __longjmp,@function
__longjmp:
	movl 4(%esp), %ecx	/* User's jmp_buf in %ecx.  */
	movl 8(%esp), %eax	/* Second argument is return value.  */
	/* Save the return address now.  */
	movl (JB_PC*4)(%ecx), %edx
	/* Restore registers.  */
	movl (JB_BX*4)(%ecx), %ebx
	movl (JB_SI*4)(%ecx), %esi
	movl (JB_DI*4)(%ecx), %edi
	movl (JB_BP*4)(%ecx), %ebp
	movl (JB_SP*4)(%ecx), %esp
	/* Jump to saved PC.  */
	jmp *%edx
.size __longjmp,.-__longjmp;
