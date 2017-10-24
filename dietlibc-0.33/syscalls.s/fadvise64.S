#ifndef __arm__
/* ARM needs special treatment for fadvise */

#include "syscalls.h"

#ifndef __NR_fadvise64_64
.globl posix_fadvise
.type posix_fadvise,@function
posix_fadvise:
#endif

#ifdef __NR_fadvise64
syscall(fadvise64,fadvise64)

#endif

#endif
