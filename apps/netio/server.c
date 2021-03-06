#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include "measure_time.h"
//4, 16, 64, 256, 1K, 4K, 16K, 64K, 256K, and 512K bytes.

int main(int argc, char *argv[])
{
  uint64_t cpuid = 3;

    int listenfd = 0, connfd = 0;
    int buff_length, numBytes=0;
    struct sockaddr_in serv_addr; 
    int eval_type;
    uint64_t begin, end;
    struct timespec begints, endts;
    int port_number;
//    if(argc!=5)
//      {
//	printf("%d Incorrect usage: %s -s <Buffer length> <Eval type>: 1 for RDTSCP\n", argc, argv[0]);
//	return 1;
//      }
    buff_length = 40960;
    char sendBuff[buff_length];
    char recvBuff_server[buff_length];
    char ack_recieve_buffer[1];
    eval_type = 1;
    port_number = 8000;

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//will it call TCP?
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 
    memset(ack_recieve_buffer, '0', sizeof(ack_recieve_buffer));

    sendBuff[buff_length-3] = 'a';
    sendBuff[buff_length-2] = 'b';
    sendBuff[buff_length-1]='\0';
    sendBuff[0]='c';
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port_number);
    int flag = 1;
    int result = setsockopt(listenfd,            /* socket affected */
			    IPPROTO_TCP,     /* set option at TCP level */
			    TCP_NODELAY,     /* name of option */
			    (char *) &flag,  /* the cast is historical cruft */
			    sizeof(int));    /* length of option value */
    int c = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //    printf("result = %d\n",c);
      listen(listenfd, 10);
      

      connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
      int result1 = setsockopt(connfd,            /* socket affected */
			       IPPROTO_TCP,     /* set option at TCP level */
			       TCP_NODELAY,     /* name of option */
			       (char *) &flag,  /* the cast is historical cruft */
			       sizeof(int));    /* length of option value */
      //Handle Nagling
      
      //    if (result==-1)
      //      {
      //	printf("error in disabling nagling]n");
      //	exit(1);
      //      }
      //
      int i=0;
      srand(100);
      for(i=0;i<buff_length;i++)
	{
	  sendBuff[i] = (char)(rand()%256);
	}

	  int n;
	  begin = RDTSCP();
	  for(int iter=0; iter<ITER; iter++)
	  {
	    numBytes = 0;
	    while(numBytes < buff_length)
            {
                if ((n = write(connfd, sendBuff, buff_length-numBytes)) > 0)
			numBytes+=n;
	    }
	  
	    //printf("Server Bytes written: %d\n",numBytes);
	  }

	  end = RDTSCP();
          double time = ((double)(end-begin))/((double)FREQ);
          printf("\nRDTSCP: %f",time);
          printf("\nTx Bandwidth: %f MBytes/s\n", ITER*buff_length/(1024.0*1024.0*time));

	  shutdown(connfd,1);
	  
	  begin = RDTSCP();
	  for(int iter=0; iter<ITER; iter++)
	  { 
	    numBytes = 0;
	    while (numBytes < buff_length)
	    {
		if ((n = read(connfd, recvBuff_server, buff_length-numBytes)) > 0)
			numBytes+=n;
	    }

	    //printf("Server Bytes read: %d\n",numBytes);
          }
	  end = RDTSCP();
	  time = ((double)(end-begin))/((double)FREQ);
	  printf("\nRDTSCP: %f",time);
	  printf("\nRx Bandwidth: %f MBytes/s\n", ITER*buff_length/(1024.0*1024.0*time));
      
    close(connfd);
    close(listenfd);
    return 0;
}

