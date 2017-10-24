#include "dietdirent.h"
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

static struct dirent tmp;

struct dirent* readdir(DIR *d) {
  struct dirent* ld;
  if (readdir_r(d,&tmp,&ld)) return 0;
  return ld;
}
