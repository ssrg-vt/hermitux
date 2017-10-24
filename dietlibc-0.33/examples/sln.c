#include <unistd.h>

int strlen(const char *s) {
  int len;
  for (len=0; *s; s++,len++) ;
  return len;
}

void die(char *s) {
  write(2,s,strlen(s));
  exit(111);
}

main(int argc,char *argv[]) {
  if (argc<3)
    die("usage: sln dest src\n");
  if (symlink(argv[2],argv[1]) == -1)
    die("symlink failed\n");
  return 0;
}
