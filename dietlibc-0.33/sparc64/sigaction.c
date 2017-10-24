#include <signal.h>
#include "syscalls.h"

static void
__rt_sigreturn_stub (void)
{
  __asm__ ("mov %0, %%g1\n\t"
           "ta  0x6d\n\t"
           : /* no outputs */
           : "i" (__NR_rt_sigreturn));
}

int __rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, void* restorer, long nr);

int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  return __rt_sigaction(signum, act, oldact, (void*)(&__rt_sigreturn_stub) - 8, _NSIG/8);
}
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
__attribute__((weak,alias("__libc_sigaction")));

