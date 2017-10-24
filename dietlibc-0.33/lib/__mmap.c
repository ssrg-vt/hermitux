#include <errno.h>
#include <sys/mman.h>
#include <syscalls.h>

#ifndef __NR_mmap
void*__mmap2(void*start,size_t length,int prot,int flags,int fd,off_t
pgoffset);
void *mmap(void *addr, size_t length, int prot, int flags, int fd,
          off_t offset)
{
  size_t pgsz = 4096;	/* TODO: fix for dynamic PAGESIZEs needed? */
  void *res;

  if (__unlikely(offset & (pgsz - 1))) {
    errno = -EINVAL;
    res = MAP_FAILED;
  } else
    res = __mmap2(addr, length, prot, flags, fd, offset / pgsz);
  return res;
}
#endif

