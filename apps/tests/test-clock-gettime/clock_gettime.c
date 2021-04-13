#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#define SLEEP_TIME 2

int main(int argc, char** argv)
{
	struct timespec res;

	clock_gettime(CLOCK_REALTIME, &res);
	printf("res.tv_sec = %ld\nres.tv_nsec = %ld\n", res.tv_sec, res.tv_nsec);

	sleep(SLEEP_TIME);

	clock_gettime(CLOCK_REALTIME, &res);
	printf("res.tv_sec = %ld\nres.tv_nsec = %ld\n", res.tv_sec, res.tv_nsec);

	clock_getres(CLOCK_REALTIME, &res);
	printf("resolution.tv_sec = %ld\nresolution.tv_nsec = %ld\n", res.tv_sec, res.tv_nsec);

	return 0;
}
