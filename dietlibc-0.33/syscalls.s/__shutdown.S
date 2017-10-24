#include "syscalls.h"

#ifdef __NR_shutdown
syscall_weak(shutdown,shutdown,__libc_shutdown)
#endif
