#include "syscalls.h"

#ifdef __NR_recvmsg
syscall_weak(recvmsg,recvmsg,__libc_recvmsg)
#endif
