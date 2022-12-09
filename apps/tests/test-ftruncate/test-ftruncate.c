#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define TESTFILE	"testfile"

#define TRUNC_LEN1	256
#define TRUNC_LEN2	512
#define FILE_SIZE	1024

static int fd;

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

int tst_fill_file(const char *path, char pattern, size_t bs, size_t bcount)
{
	int fd;

	fd = open(path, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
	if (fd < 0)
		return -1;

	if (tst_fill_fd(fd, pattern, bs, bcount)) {
		close(fd);
		unlink(path);
		return -1;
	}

	if (close(fd) < 0) {
		unlink(path);

		return -1;
	}

	return 0;
}

static int check_and_report(off_t offset, char data, off_t trunc_len)
{
	int i, file_length;
	char buf[FILE_SIZE];
	struct stat stat_buf;

	memset(buf, '*', sizeof(buf));

	fstat(fd, &stat_buf);
	file_length = stat_buf.st_size;

	if (file_length != trunc_len) {
		printf("ERROR: ftruncate() got incorrected size: %d\n", file_length);
		return -1;
	}

	lseek(fd, offset, SEEK_SET);
	read(fd, buf, sizeof(buf));

	for (i = 0; i < TRUNC_LEN1; i++) {
		if (buf[i] != data) {
			printf("ERROR: ftruncate() got incorrect data %i, expected %i\n",
				buf[i], data);
			return -1;
		}
	}

	printf("SUCCESS: ftruncate() succeeded\n");
}

static int verify_ftruncate(void)
{
	int ret = ftruncate(fd, TRUNC_LEN1);
	if (ret == -1) {
		printf("ERROR: ftruncate() failed, retcode %d, errno %d\n", ret, errno);
		return -1;
	}

	check_and_report(0, 'a', TRUNC_LEN1);

	ret = ftruncate(fd, TRUNC_LEN2);
	if (ret == -1) {
		printf("ERROR: ftruncate() failed, retcode %d, errno %d\n", ret, errno);
		return -1;
	}

	check_and_report(TRUNC_LEN1, 0, TRUNC_LEN2);
}

static int setup(void)
{
	if (tst_fill_file(TESTFILE, 'a', FILE_SIZE, 1)) {
		printf("ERROR: Failed to create test file");
		return -1;
	}

	fd = open(TESTFILE, O_RDWR);
	if(fd == -1) {
		printf("ERROR: can't open test file\n");
		return -1;
	}

	return 0;
}

static void cleanup(void)
{
	if (fd > 0)
		close(fd);
}

int main(int argc, char **argv) {
	if(setup()) {
		printf("Test failed\n");
		return -1;
	}

	if(verify_ftruncate())
		printf("Test succeeded\n");
	else {
		printf("Test failed\n");
		return -1;
	}

	cleanup();
}
