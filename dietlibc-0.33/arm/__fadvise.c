#include <fcntl.h>
#include "syscalls.h"

#ifndef __NR_fadvise64
long fadvise64_64(int fd, off64_t offset, off64_t len, int advice)
{
  extern long __arm_fadvise64_64(int fd, int advice, off64_t offset, off64_t len);

  return __arm_fadvise64_64(fd, advice, offset, len);
}

int posix_fadvise(int fd, off64_t offset, off64_t len, int advise)
  __attribute__((__alias__("fadvise64_64")));
#endif
