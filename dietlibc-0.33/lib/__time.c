#include <sys/time.h>
#include <time.h>
#include <syscalls.h>

#ifndef __NR_time
time_t time(time_t *t)
{
  struct timeval tv;
  if (__unlikely(gettimeofday(&tv, NULL) < 0))
    tv.tv_sec = -1;
  if (t)
    *t = tv.tv_sec;
  return tv.tv_sec;
}
#endif
