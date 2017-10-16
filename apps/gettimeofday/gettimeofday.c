
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
//#include <hermit/syscall.h>

extern int sys_msleep(long arg);

#define N	255

int main(int argc, char** argv)
{
	struct timeval res;

	gettimeofday(&res, NULL);
	printf("res.tv_sec = %ld\nres.tv_usec = %ld\n", res.tv_sec, res.tv_usec);

	sys_msleep(10000);

	gettimeofday(&res, NULL);
	printf("res.tv_sec = %ld\nres.tv_usec = %ld\n", res.tv_sec, res.tv_usec);


	return 0;
}
