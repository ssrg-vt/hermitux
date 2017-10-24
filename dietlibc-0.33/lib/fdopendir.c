#include "dietdirent.h"
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>

DIR*  fdopendir ( int fd ) {
  DIR*  t  = NULL;

  if ( fd >= 0 ) {
    t = (DIR *) mmap (NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, 
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (t == MAP_FAILED)
lose:
      close (fd);
    else
      t->fd = fd;
  }


  return t;
}
