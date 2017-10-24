#include "syscalls.h"

#ifdef __NR_send
syscall_weak(send,send,__libc_send)
#endif
