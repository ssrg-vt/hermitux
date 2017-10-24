#include "syscalls.h"

syscall_weak(osf_sigprocmask,__old_sigprocmask,__n_sigprocmask)
