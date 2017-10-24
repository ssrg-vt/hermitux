#include <sys/resource.h>

extern int __syscall_getpriority(int which, int who);

int getpriority(int which, int who) {
  int r;

  r = __syscall_getpriority(which, who);
  if (r >= 0) r = 20 - r;
  return r;
}

