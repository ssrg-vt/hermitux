#include "syscalls.h"

#ifdef __NR_bind
syscall_weak(bind,bind,__libc_bind)
#endif
