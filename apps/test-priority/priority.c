#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/resource.h>

#define PRIO_TO_SET		-20

int main(int argc, char **argv) {

	int prio = getpriority(PRIO_PROCESS, 0);
	printf("prio is: %d\n", prio);

	setpriority(PRIO_PROCESS, 0, PRIO_TO_SET);

	prio = getpriority(PRIO_PROCESS, 0);
	printf("prio is: %d\n", prio);

	return 0;
}
