#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define TESTFILE	"testfile"
#define FILE_SIZE	1024
#define TRUNC_LEN1	256
#define TRUNC_LEN2	512

static int fd;

static struct tcase {
	off_t trunc_len;
	off_t read_off;
	off_t read_count;
	char exp_char;
} tcases[] = {
	{TRUNC_LEN1, 0, TRUNC_LEN1, 'a'},
	{TRUNC_LEN2, TRUNC_LEN1, TRUNC_LEN1, '\0'},
};

int tst_fill_fd(int fd, char pattern, size_t bs, size_t bcount)
{
	size_t i;
	char *buf;

	/* Filling a memory buffer with provided pattern */
	buf = malloc(bs);
	if (buf == NULL)
		return -1;

	for (i = 0; i < bs; i++)
		buf[i] = pattern;

	/* Filling the file */
	for (i = 0; i < bcount; i++) {
		if (write(fd, buf, bs) != (ssize_t)bs) {
			free(buf);
			return -1;
		}
	}

	free(buf);

	return 0;
}

static int verify_truncate(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	struct stat stat_buf;
	char read_buf[tc->read_count];
	int i;

	memset(read_buf, 'b', tc->read_count);

	int ret = truncate(TESTFILE, tc->trunc_len);
	if (ret != 0) {
		printf("ERROR: truncate(%s, %ld) failed (returned: %d, errno: %d)\n",
			TESTFILE, tc->trunc_len, ret, errno);
		return -1;
	}

	stat(TESTFILE, &stat_buf);
	if (stat_buf.st_size != tc->trunc_len) {
		printf("ERROR: %s: Incorrect file size %ld, expected %ld\n",
			TESTFILE, stat_buf.st_size, tc->trunc_len);
		return -1;
	}

	if (lseek(fd, 0, SEEK_CUR)) {
		printf("ERROR: truncate(%s, %ld) changes offset\n", TESTFILE,
			tc->trunc_len);
		return -1;
	}

	pread(fd, read_buf, tc->read_count, tc->read_off);
	for (i = 0; i < tc->read_count; i++) {
		if (read_buf[i] != tc->exp_char) {
			printf("ERROR: %s: wrong content %c, expected %c\n",	TESTFILE,
				read_buf[i], tc->exp_char);
			return -1;
		}
	}

	printf("SUCCESS: truncate(%s, %ld) succeeded\n", TESTFILE, tc->trunc_len);
	return 0;
}

static void setup(void)
{
	fd = open(TESTFILE, O_RDWR | O_CREAT, 0644);

	tst_fill_fd(fd, 'a', FILE_SIZE, 1);

	lseek(fd, 0, SEEK_SET);
}

static void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

int main(int argc, char **argv) {
	setup();

	if(verify_truncate(0) || verify_truncate(1))
		printf("Test failed\n");
	else
		printf("All tests succeeded\n");

	cleanup();
}