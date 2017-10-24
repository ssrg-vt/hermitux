#define _REENTRANT
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>

int nice(int incr) {
  int prio;
  int res;
  errno=0;
  prio = getpriority(PRIO_PROCESS,0) + incr;
  if (prio < PRIO_MIN) prio=PRIO_MIN;
  if (prio >= PRIO_MAX) prio=PRIO_MAX-1;
  if (setpriority (PRIO_PROCESS, 0, prio)==-1)
    return -1;
  else
    return getpriority(PRIO_PROCESS, 0);
}
