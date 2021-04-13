#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	char buf[128];
	int bytes = readlink("/proc/self/exe", buf, 128);

	if(bytes == 128) {
		perror("readlink");
		return -1;
	}

	buf[bytes] = '\0';

	printf("readlink on /proc/self/exe returns %d (%s)\n", bytes, buf);

	return 0;
}
