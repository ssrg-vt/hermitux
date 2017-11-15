#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ITERATIONS	10000

int main(void)
{
	int i, fd;
	struct winsize sz;
	char buf[64];

	fd = open("libhermit.a", O_RDONLY);
	read(fd, buf, 64);
	close(fd);

	for(i=0; i<ITERATIONS; i++) {
		ioctl(0, TIOCGWINSZ, &sz);
		(void)sz;
	}

	return 0;
}
