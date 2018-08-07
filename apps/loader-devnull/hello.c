#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

char *buffer = "hello";

int main(int argc, char **argv) {

	int fd = open("/dev/null", O_WRONLY);
	if(fd == -1) {
		fprintf(stderr, "Error open\n");
		return -1;
	}

	int ret = write(fd, buffer, strlen(buffer) + 1);
	if(ret != strlen(buffer)+1) {
		fprintf(stderr, "Error write: %d\n", ret);
	}

	return 0;
}
