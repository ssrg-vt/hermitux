#include <sys/types.h>
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int * wait_stat, int flags) {
  return wait4(pid, wait_stat, flags, 0);
}
