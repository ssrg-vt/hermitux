#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILENAME_1	"file1.txt"
#define FILENAME_2	"file2.txt"
#define BUF_SIZE	128
#define READ_SIZE	16

int main(int argc, char *argv[])
{
	int fd1, fd2;
	char buffer[BUF_SIZE];

	fd1 = open(FILENAME_1, O_RDONLY, NULL);
	if(fd1 != -1)
		if(read(fd1, buffer, READ_SIZE) != -1)
			printf("read: %s\n", buffer);

	fd2 = open(FILENAME_2, O_RDONLY, NULL);
	if(fd2 != -1)
		if(read(fd2, buffer, READ_SIZE) != -1)
			printf("read: %s\n", buffer);

	if(fd1 != -1) close(fd1);
	if(fd2 != -1) close(fd2);

	return 0;
}
