#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024

/**
 * Test that dup2 will successfully create a copy of oldfd
 * using an unused newfd
 */
static int test_newfd(void)
{

    fprintf(stderr, "----- test_newfd -----\n");
    int oldfd = open("test.dat", O_RDONLY | O_CREAT);
    if (oldfd == -1) {
        fprintf(stderr, "cannot open test.dat\n");
        return -1;
    }
    fprintf(stdout, "oldfd: %d\n", oldfd);

    int newfd = oldfd + 1;
    int ret = 0;
    while (ret != -1) {
        ret = fcntl(newfd, F_GETFL);
        newfd++;
    }

    ret = dup2(oldfd, newfd);
    fprintf(stdout, "newfd: %d\ndup2 returns: %d\n", newfd, ret);
    if (newfd != ret) {
        return -1;
    }

    return 0;
}


/**
 * Test that dup2 will successfully create a copy of oldfd
 * using the used newfd, silently closing newfd before reusing. 
 */
static int test_usedfd()
{

    fprintf(stderr, "----- test_usedfd -----\n");
    char buf_used[BUF_SIZE * sizeof(char)];
    char buf_new[BUF_SIZE * sizeof(char)];

    int oldfd = open("test.dat", O_RDONLY | O_CREAT);
    if (oldfd == -1) {
        fprintf(stderr, "cannot open test.dat\n");
        return oldfd;
    }
    fprintf(stdout, "oldfd: %d\n", oldfd);

    int usedfd = open("usedfd.dat", O_RDONLY | O_CREAT);
    if (usedfd == -1) {
        fprintf(stderr, "cannot open usedfd.dat\n");
        return usedfd;
    }

    int ret = read(usedfd, buf_used, BUF_SIZE);
    if (ret == -1) {
        fprintf(stderr, "Error reading from file descriptor usedfd\n");
        return ret;
    }

    int newfd = dup2(oldfd, usedfd);
    fprintf(stdout, "usedfd: %d\ndup2 returns: %d\n", usedfd, newfd);
    if (newfd != usedfd) {
        return -1;
    }

    ret = read(newfd, buf_new, BUF_SIZE);
    if (ret == -1) {
        fprintf(stderr, "Error reading from file descriptor newfd\n");
        return ret;
    }

    fprintf(stdout, "compare_contents: From usedfd:\n%s\n", buf_used);
    fprintf(stdout, "compare_contents: From newfd:\n%s\n", buf_new);
    return 0;
}


/**
 * ssize_t sys_read(int fd, char* buf, size_t len); 
 */
int main(int argc, char **argv)
{
    if (test_newfd() == -1) {
        fprintf(stderr, "test_newfd failed\n");
    }
    if (test_usedfd() == -1) {
        fprintf(stderr, "test_usedfd failed\n");
    }
}