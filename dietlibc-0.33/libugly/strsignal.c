#define _GNU_SOURCE
#include <string.h>
#include <signal.h>

char* strsignal(int sig) {
  if ((unsigned int)sig<=SIGRTMAX)
    return (char*)sys_siglist[sig];
  else
    return (char*)"(unknown signal)";
}
