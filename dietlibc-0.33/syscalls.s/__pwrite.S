#include "syscalls.h"

#ifdef __NR_pwrite64
syscall_weak(pwrite64,pwrite64,__libc_pwrite64)
#else
syscall_weak(pwrite,pwrite64,__libc_pwrite64)
#endif
