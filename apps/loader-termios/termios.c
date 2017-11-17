#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

int main() {
	int i;
	struct termios t;
	ioctl(0, TCGETS, &t);

	printf("c_iflags: %#x\n", t.c_iflag);
	printf("c_oflags: %#x\n", t.c_oflag);
	printf("c_cflags: %#x\n", t.c_cflag);
	printf("c_lflags: %#x\n", t.c_lflag);

	printf("c_line: %#x\n", t.c_line);
	
	for(i=0; i<NCCS; i++)
		printf("c_cc[%d]: %#x\n", i, t.c_cc[i]);


	return 0;
}
