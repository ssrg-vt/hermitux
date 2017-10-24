#include "syscalls.h"

#ifdef __NR_sendmsg
syscall_weak(sendmsg,sendmsg,__libc_sendmsg)
#endif
