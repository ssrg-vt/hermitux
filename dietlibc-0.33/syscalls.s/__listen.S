#include "syscalls.h"

#ifdef __NR_listen
syscall_weak(listen,listen,__libc_listen)
#endif
