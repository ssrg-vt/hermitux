#include <signal.h>

int __rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, void* restorer, long nr);

int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  return __rt_sigaction(signum, act, oldact, 0, _NSIG/8);
}
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
__attribute__((weak,alias("__libc_sigaction")));

