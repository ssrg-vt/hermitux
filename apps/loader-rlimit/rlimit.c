#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char **argv) {
	struct rlimit rlim;

	if(getrlimit(RLIMIT_FSIZE, &rlim)) {
		perror("getrlimit");
		return -1;
	}

	printf("rlimit fsize cur: %ld, max: %ld\n", rlim.rlim_cur, rlim.rlim_max);

	return 0;
}
