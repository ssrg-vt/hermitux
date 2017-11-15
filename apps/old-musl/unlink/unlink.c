#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE_NAME "test.txt"

int main(void) {
	int fd, ret;

	fd = open(FILE_NAME, O_RDWR | O_CREAT, 0600);
	if(!fd) {
		printf("Error opening file\n");
		return -1;
	}

	close(fd);

	ret = unlink(FILE_NAME);

	if(ret) {
		printf("error unlink returned %d\n", ret);
		return -2;
	}

	return 0;
}
