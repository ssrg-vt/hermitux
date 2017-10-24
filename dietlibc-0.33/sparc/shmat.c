#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

extern void* __ipc();

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

void* shmat(int shmid,const void* shmaddr,int shmflg) {
  void* raddr;
  register void* result;
  result=__ipc(SHMAT,shmid,shmflg,&raddr,shmaddr);
  if ((unsigned long)result <= -(unsigned long)PAGE_SIZE)
    result=raddr;
  return result;
}
