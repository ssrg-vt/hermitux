#include "syscalls.h"

#ifdef __NR_ugetrlimit

syscall(ugetrlimit,getrlimit)

#elif defined(__NR_getrlimit)

syscall(getrlimit,getrlimit)

#endif
