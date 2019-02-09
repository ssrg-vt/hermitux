#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv) {
	struct stat res;
	int fd, ret;

	fd = open(argv[0], O_RDONLY, 0x0);
	if(fd == -1) {
		fprintf(stderr, "cannot open test.dat");
		return -1;
	}
	
	ret = fstat(fd, &res);
	if(ret == -1) {
		fprintf(stderr, "Error running fstat");
		close(fd);
		return -1;
	}

	printf("fstat size: %llu blocks\n", res.st_size);

	close(fd);

	memset(&res, 0x0, sizeof(struct stat));
	ret = stat(argv[0], &res);
	if(ret == -1) {
		fprintf(stderr, "Error running stat\n");
		return -1;
	}

	printf("stat size: %llu blocks\n", res.st_size);

	memset(&res, 0x0, sizeof(struct stat));
	ret = lstat(argv[0], &res);
	if(ret == -1) {
		fprintf(stderr, "Error running lstat\n");
		return -1;
	}

	printf("lstat size: %llu blocks\n", res.st_size);

	return 0;
}
