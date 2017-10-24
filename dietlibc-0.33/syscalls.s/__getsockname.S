#include "syscalls.h"

#ifdef __NR_getsockname
syscall_weak(getsockname,getsockname,__libc_getsockname)
#endif
