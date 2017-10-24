#include "syscalls.h"

#ifdef __NR_setsockopt
syscall_weak(setsockopt,setsockopt,__libc_setsockopt)
#endif
