#include <assert.h>
#include <stdlib.h>
#include <utime.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
  char file[] = "/tmp/utime-test.XXXXXX";
  int tmp_fd;
  struct utimbuf utm = {
    .actime = 23,
    .modtime = 42,
  };
  struct stat st;
  time_t now;

  tmp_fd = mkstemp(file);
  close(tmp_fd);

  assert(utime(file, &utm) == 0);
  assert(stat(file, &st) == 0);
  assert(st.st_atime == utm.actime);
  assert(st.st_mtime == utm.modtime);

  now = time(NULL);
  assert(utime(file, NULL) == 0);
  assert(stat(file, &st) == 0);

  assert(st.st_atime == st.st_mtime);
  assert(st.st_atime >= now);
  assert(st.st_atime - now < 10);

  unlink(file);

  return EXIT_SUCCESS;
}

