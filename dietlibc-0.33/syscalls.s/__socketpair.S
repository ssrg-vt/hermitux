#include "syscalls.h"

#ifdef __NR_socketpair
syscall_weak(socketpair,socketpair,__libc_socketpair)
#endif
