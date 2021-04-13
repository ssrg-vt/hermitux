#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void) {
		int fd;
		fd = open("non-existent-file", O_RDONLY);
		if(fd == -1) {
			perror("open");
			printf("errno: %d\n", errno);
		} else
		{
			printf("fd successful! %d\n", fd);
		}

		return 0;
}
