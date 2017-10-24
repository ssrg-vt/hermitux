#include <endian.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef __NO_STAT64
extern size_t __pread(int fd, void *buf, size_t count, int dummy, off_t a, off_t b);

size_t __libc_pread64(int fd, void *buf, size_t count, off64_t offset);
size_t __libc_pread64(int fd, void *buf, size_t count, off64_t offset) {
  return __pread(fd,buf,count,0,__LONG_LONG_PAIR( (off_t)(offset>>32),(off_t)(offset&0xffffffff) ));
}

int pread64(int fd, void *buf, size_t count, off_t offset) __attribute__((weak,alias("__libc_pread64")));
#endif
