#include "syscalls.h"

#ifdef __NR_getsockopt
syscall_weak(getsockopt,getsockopt,__libc_getsockopt)
#endif
