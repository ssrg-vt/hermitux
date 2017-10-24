#include <sys/types.h>
#include <unistd.h>

#undef seteuid
int seteuid(uid_t euid);
int seteuid(uid_t euid) {
  return setreuid(-1,euid);
}
