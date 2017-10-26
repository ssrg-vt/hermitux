#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(void) {
	printf("prio: %d\n", getpriority(PRIO_PROCESS, 0));
	return 0;
}
