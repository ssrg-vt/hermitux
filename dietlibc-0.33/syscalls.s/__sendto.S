#include "syscalls.h"

#ifdef __NR_sendto
syscall_weak(sendto,sendto,__libc_sendto)
#endif
