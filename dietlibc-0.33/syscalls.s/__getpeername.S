#include "syscalls.h"

#ifdef __NR_getpeername
syscall_weak(getpeername,getpeername,__libc_getpeername)
#endif
