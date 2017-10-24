#include <signal.h>

int __rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, long nr, void* restorer);

int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  return __rt_sigaction(signum, act, oldact, _NSIG/8, 0);
}
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) 
__attribute__((weak,alias("__libc_sigaction")));
