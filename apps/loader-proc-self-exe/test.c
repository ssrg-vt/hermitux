#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	char buf[128];
	int bytes = readlink("/proc/self/exe", buf, 128);

	if(bytes == 128) {
		perror("readlink");
		return -1;
	}

	printf("readlink on /proc/self/exe returns %s\n", buf);

	return 0;
}
