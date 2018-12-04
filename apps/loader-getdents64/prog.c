#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

struct linux_dirent64 {
   uint64_t        d_ino;    /* 64-bit inode number */
   uint64_t        d_off;    /* 64-bit offset to next structure */
   unsigned short d_reclen; /* Size of this dirent */
   unsigned char  d_type;   /* File type */
   char           d_name[]; /* Filename (null-terminated) */
};

#define BUF_SIZE	1024

int main(int argc, char **argv) {
	int fd;
	int bpos;
	char buf[BUF_SIZE];

	fd = open(".", O_RDONLY | O_DIRECTORY, 0x0);
	if(fd == -1) {
		perror("open");
		return -1;
	}

	int ret = syscall(SYS_getdents64, fd, buf, BUF_SIZE);

	if(ret < 0)
		perror("getdents64");

	printf("getdents64 returned %d\n", ret);
	for(bpos = 0; bpos < ret;) {
		struct linux_dirent64 *s = (struct linux_dirent64 *) (buf + bpos);
		printf("- %s (%llu)\n", s->d_name, s->d_ino);
		bpos += s->d_reclen;
	}

	return 0;
}
