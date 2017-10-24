#include <sys/mount.h>
#include "syscalls.h"

#ifndef __NR_umount
int umount(const char *target)
{
       return umount2(target, 0);
}
#endif

