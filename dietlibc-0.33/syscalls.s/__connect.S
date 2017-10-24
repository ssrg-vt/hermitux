#include "syscalls.h"

#ifdef __NR_connect
syscall_weak(connect,connect,__libc_connect)
#endif
