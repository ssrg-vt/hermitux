#include "syscalls.h"

#ifdef __NR_socket
syscall_weak(socket,socket,__libc_socket)
#endif
