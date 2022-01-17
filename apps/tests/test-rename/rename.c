#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syscall.h>

#define SRC_FILE "src-file.dat"
#define DST_FILE "dst-file.dat"
#define FILE_SZ  4096

int main(void) {
    int fd=0;

    /* delete any existing file */
    remove(SRC_FILE);
    remove(DST_FILE);

    /* create the source file */
    fd = open(SRC_FILE, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
    if(fd == -1) {
        perror("open");
        exit(-1);
    }

    for(int i=0; i<FILE_SZ; i++)
        if(write(fd, "@", 1) != 1) {
            perror("write");
            exit(-1);
        }

    close(fd);

    /* rename */
    if(rename(SRC_FILE, DST_FILE)) {
        perror("rename");
        exit(-1);
    }

    /* check destination file */
    fd = open(DST_FILE, O_RDONLY, NULL);
    if(fd == -1) {
        perror("open");
        exit(-1);
    }

    for(int i=0; i<FILE_SZ; i++) {
        char c;
        if(read(fd, &c, 1) != 1) {
            perror("wrread");
            exit(-1);
        }

        if(c != '@') {
            fprintf(stderr, "destination file content check failed\n");
            exit(-1);
        }
    }

    close(fd);

    printf("Test succeeds!\n");

    return 0;
}
