#include <unistd.h>
#include <signal.h>

int pause(void)
{
  sigset_t set;
  sigemptyset(&set);
  sigprocmask(SIG_BLOCK, NULL, &set);
  return sigsuspend(&set);
}

