#include "dietfeatures.h"
.text
.global memchr
.type memchr,@function
memchr:	/* rdi=ptr, esi=ch, rdx=len */
#ifdef WANT_VALGRIND_SUPPORT
#ifdef __DYN_LIB
  movq	__valgrind@GOTPCREL(%rip), %rax
  movq	(%rax), %rax
  test	%rax, %rax
#else
  xor %eax,%eax
  or (__valgrind),%eax
#endif
  jnz .Lvalgrind
#endif
  and $0xff,%esi
  test %rdx,%rdx
  jz .Lnull		/* if (!len) return 0 */
  imul $0x1010101,%esi	/* esi = ch x 4 */

  mov %rdi,%rcx
  and $15,%ecx
  movd %esi,%xmm0
  lea (%rdi,%rdx),%rsi	/* rsi = max ptr */
  and $~15,%rdi		/* 16-align rdi, diff in cl */
  or $-1,%eax
  shl %cl,%eax

  pshufd $0,%xmm0,%xmm0	/* xmm0 = ch x 16 */
1:
  movdqa (%rdi),%xmm1
  pcmpeqb %xmm0,%xmm1
  pmovmskb %xmm1,%ecx
  and %eax,%ecx
  jnz .Lfound	/* found something */
  lea 16(%rdi),%rdi
  or $-1,%eax
  cmp %rsi,%rdi
  jb 1b
  jmp .Lnull
.Lfound:
  bsf %ecx,%eax
  add %rdi,%rax
  cmp %rsi,%rax
  jae .Lnull
  ret
.Lnull:
  xor %eax,%eax
  ret

#ifdef WANT_VALGRIND_SUPPORT
/* rdi=ptr, esi=ch, rdx=len */
.Lvalgrind:
.Lloop:
  test %rdx,%rdx
  jz .Lnull
  movzbl (%rdi),%eax
  cmp %esi,%eax
  jz .Lfound2
  inc %rdi
  dec %rdx
  jmp .Lloop
.Lfound2:
  mov %rdi,%rax
  ret
#endif
.Lhere:
.size memchr,.Lhere-memchr
