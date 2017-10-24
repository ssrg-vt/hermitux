#include "syscalls.h"

#ifdef __NR_recvfrom
syscall_weak(recvfrom,recvfrom,__libc_recvfrom)
#endif
