#define _GNU_SOURCE
#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

int main(void)
{
  char file[] = "/tmp/dietlibc-fadvise-test.XXXXXX";
  int fd;

  fd = mkstemp(file);
  unlink(file);

  assert(posix_fadvise(fd, 23, 42, POSIX_FADV_RANDOM) == 0);
  close(fd);

  return EXIT_SUCCESS;
}
