#include <utime.h>
#include <syscalls.h>

#ifndef __NR_utime
int utime(const char *filename, const struct utimbuf *times)
{
  if (times == NULL)
    return utimes(filename, NULL);
  else {
    struct timeval tvs[2];
    tvs[0].tv_sec  = times->actime;
    tvs[0].tv_usec = 0;
    tvs[1].tv_sec  = times->modtime;
    tvs[1].tv_usec = 0;
    return utimes(filename, tvs);
  }
}
#endif
