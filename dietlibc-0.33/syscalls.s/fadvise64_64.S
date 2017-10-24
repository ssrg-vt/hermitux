#include "syscalls.h"

#ifdef __NR_fadvise64_64
syscall(fadvise64_64,fadvise64_64)

.set   posix_fadvise, fadvise64_64
.globl posix_fadvise
#endif
