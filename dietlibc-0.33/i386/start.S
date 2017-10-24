#include "dietfeatures.h"
#include "PIC.h"

.text
.global _start
_start:
	popl	%ecx			/* %ecx = argc */
	movl	%esp,%eax		/* %eax = argv */
	pushl	%ecx
	leal	4(%eax,%ecx,4),%esi	/* %esi = envp = (4*ecx)+%eax+4 */

#ifdef WANT_DYNAMIC
/* in %edx we have the ld.so dynamic _fini ( register this if non null ) */
	push	%edx
#endif

	pushl	%esi
	pushl	%eax
	pushl	%ecx
	PIC_INIT			/* non-PIC: this is an empty line */
	PUT_VAR %esi, environ, %ecx	/* non-PIC: movl %esi,environ */

#ifdef WANT_SYSENTER
/* skip environment, scan for NULL */
1:
	lodsl
	testl	%eax,%eax
	jnz	1b
/* The ELF auxvec follows the environment, consists of key/value pairs.
   We are looking for key 32, which stands for the vsyscall page */
1:
	lodsl
	testl	%eax,%eax
	jz	1f
	cmpl	$32,%eax
	lodsl
	jne	1b
	PUT_VAR %eax, __vsyscall, %edx
1:
#endif

#ifdef PROFILING
	pushl	$_etext
	pushl	$.text
	call	monitor
	addl	$0x8, %esp
#endif

#ifdef WANT_DYNAMIC
	call	_dyn_start
#else
	call	CALL_IN_STARTCODE
#endif

#ifdef PROFILING
	pushl	%eax
	call	_stop_monitor
	popl	%eax
#endif
	pushl	%eax
	call	exit
	hlt	/* die now ! will ya ... */
.Lstart:
	.size	 _start,.Lstart-_start

