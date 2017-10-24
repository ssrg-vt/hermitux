#include <endian.h>
#include <sys/types.h>

extern size_t __pwrite(int fd, void *buf, size_t count, int dummy, off_t a, off_t b);

size_t __libc_pwrite(int fd, void *buf, size_t count, off_t offset);
size_t __libc_pwrite(int fd, void *buf, size_t count, off_t offset) {
  return __pwrite(fd,buf,count,0,__LONG_LONG_PAIR(0,offset));
}

int pwrite(int fd, void *buf, size_t count, off_t offset) __attribute__((weak,alias("__libc_pwrite")));
