#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "measure_time.h"

int client_main(int argc, char *argv[])
{
  uint64_t cpu_id = 2;

  int sockfd = 0, n = 0, numBytes=0;
    struct sockaddr_in serv_addr; 
    int buff_length;
    int port_number;
    if(argc != 4)
    {
        printf("\n Usage: %s <ip of server> <buffer length> <port number>\n",argv[0]);
        return 1;
    } 
    buff_length = atoi(argv[2]);
    port_number = atoi(argv[3]);
    char recvBuff[buff_length-1];
    char sendBuff_client[buff_length-1];
    char ack_buf[1];

    printf("bufffer length %d\n", buff_length);

    memset(ack_buf, '0',sizeof(ack_buf));
    memset(recvBuff, '0',sizeof(recvBuff));
    memset(sendBuff_client, '0',sizeof(sendBuff_client));

    if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    // Handle nagling
    int flag = 1;
    int result = setsockopt(sockfd,            /* socket affected */
			    IPPROTO_TCP,     /* set option at TCP level */
			    TCP_NODELAY,     /* name of option */
			    (char *) &flag,  /* the cast is historical cruft */
			    sizeof(int));    /* length of option value */
    if (result==-1)
      {
	printf("error in disabling nagling");
	exit(1);
      }
    //
    int i;
    srand(90);
    for(i=0;i<buff_length;i++)
      {
	sendBuff_client[i] = (char)(rand()%256);
      }
    int err;
    if (err = (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0))
    {
      printf("\n Error : Connect Failed %s\n",strerror(err));
       return 1;
    } 

    numBytes=0;
    while (numBytes < buff_length)
    {
	if ((n = read(sockfd, recvBuff, buff_length-numBytes)) > 0)
		numBytes+=n;
    }

    printf("Client Bytes received: %d\n", numBytes);
    shutdown(sockfd,0);

    printf("now writing\n");

    //	sendBuff_client[buff_length-3] = 'e';
    //	sendBuff_client[buff_length-2] = 'f';
    //	sendBuff_client[buff_length-1] = '\0';
    //	sendBuff_client[0]='g';
     numBytes=0;
     while (numBytes < buff_length)
     {
	if ((n = write(sockfd, recvBuff, buff_length-numBytes)) > 0)
		numBytes+=n;
     }
     printf("Client bytes sent:%d\n",buff_length);
     close(sockfd);
    return 0;
}

