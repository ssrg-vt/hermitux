#include <alloca.h>
#include <signal.h>
#include "i386/syscalls.h"

int __rt_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact, long nr);

void __restore_rt(void);
void __restore(void);

int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int __libc_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  struct sigaction *newact = (struct sigaction *)act;
  if (act) {
    newact = alloca(sizeof(*newact));
    newact->sa_handler = act->sa_handler;
    newact->sa_flags = act->sa_flags | SA_RESTORER;
    newact->sa_restorer = (act->sa_flags & SA_SIGINFO) ? &__restore_rt : &__restore;
    newact->sa_mask = act->sa_mask;
  }
  return __rt_sigaction(signum, newact, oldact, _NSIG/8);
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
__attribute__((weak,alias("__libc_sigaction")));

