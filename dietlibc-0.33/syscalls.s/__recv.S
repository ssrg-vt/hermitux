#include "syscalls.h"

#ifdef __NR_recv
syscall_weak(recv,recv,__libc_recv)
#endif
