#define _GNU_SOURCE
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

char* get_current_dir_name(void) {
  char* pwd = getenv("PWD");
  char tmp[PATH_MAX];
  struct stat a,b;
  if (pwd && !stat(".",&a) && !stat(pwd,&b) &&
      a.st_dev==b.st_dev && a.st_ino==b.st_ino)
    return strdup(pwd);
  if (getcwd(tmp,sizeof(tmp)))
    return strdup(tmp);
  return 0;
}
