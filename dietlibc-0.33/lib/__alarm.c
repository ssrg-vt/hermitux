#include <unistd.h>
#include <sys/time.h>
#include "syscalls.h"

#ifndef __NR_alarm
unsigned int alarm(unsigned int seconds) {
  struct itimerval old, new;
  unsigned int ret;
  new.it_interval.tv_usec=0;
  new.it_interval.tv_sec=0;
  new.it_value.tv_usec	=0;
  new.it_value.tv_sec	=(long)seconds;
  if (setitimer(ITIMER_REAL,&new,&old)==-1) return 0;
  return old.it_value.tv_sec+(old.it_value.tv_usec?1:0);
}
#endif
