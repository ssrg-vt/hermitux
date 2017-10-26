#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FILE_NAME "test.txt"

int main (void) {
	int fd, ret;
	struct flock lock;

	printf ("opening %s\n", FILE_NAME);
	 /* Open a file descriptor to the file. */
	fd = open(FILE_NAME, O_RDWR | O_CREAT, 0600);
	if(!fd) {
		printf("Error opening file\n");
		return -1;
	}

	printf ("locking\n");
	/* Initialize the flock structure. */
	memset (&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;
	/* Place a write lock on the file. */
	ret = fcntl (fd, F_SETLKW, &lock);
	if(ret == -1) {
		printf("error locking\n");
		goto out_close;
	}

	printf ("locked\n");

	printf ("unlocking\n");
	/* Release the lock. */
	lock.l_type = F_UNLCK;
	ret = fcntl (fd, F_SETLKW, &lock);
	if(ret == -1)
		printf("error unlocking\n");

out_close:
	close (fd);
	return 0;
}
