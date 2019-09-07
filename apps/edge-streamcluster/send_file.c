#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"

int sendfile(FILE *fp, int sockfd);
ssize_t total=0;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        perror("usage:send_file filepath <IPaddress>");
        exit(1);
    }

    while(1) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Can't allocate sockfd");
            exit(1);
        }

        struct sockaddr_in serveraddr;
        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(SERVERPORT);
        if (inet_pton(AF_INET, argv[2], &serveraddr.sin_addr) < 0) {
            perror("IPaddress Convert Error");
            exit(1);
        }

        /* Keep trying to send the file */
        while(1) {
            if (connect(sockfd, (const struct sockaddr *) &serveraddr,
                        sizeof(serveraddr)) < 0) {
                usleep(100 * 1000);
            } else
                break;
        }

        char *filename = basename(argv[1]);
        if (filename == NULL) {
            perror("Can't get filename");
            exit(1);
        }

        char buff[BUFFSIZE] = {0};
        FILE *fp = fopen(argv[1], "rb");
        if (fp == NULL) {
            perror("Can't open file");
            exit(1);
        }

        int bytes_sent = sendfile(fp, sockfd);
        printf("Send Success, NumBytes = %ld\n", bytes_sent);
        fclose(fp);
        close(sockfd);

        sleep(1);
    }

    return 0;
}

int sendfile(FILE *fp, int sockfd) 
{
    int n; 
    char sendline[MAX_LINE] = {0};
    int bytes_sent = 0;

    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) {
	    total+=n;
        bytes_sent += n;

        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }
        
        int sent = send(sockfd, sendline, n, 0);
        if (sent == -1) {
            perror("Can't send file");
            exit(1);
        }

        memset(sendline, 0, MAX_LINE);
    }

    return bytes_sent;
}
