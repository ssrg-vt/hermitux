#include "syscalls.h"

#ifdef __NR_accept
syscall_weak(accept,accept,__libc_accept)
#endif
