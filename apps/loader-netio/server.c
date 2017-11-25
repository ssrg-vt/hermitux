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

int server_main(int argc, char *argv[])
{
  uint64_t cpuid = 3;

    int listenfd = 0, connfd = 0;
    int buff_length, numBytes=0;
    struct sockaddr_in serv_addr; 
    int eval_type;
    uint64_t begin, end;
    struct timespec begints, endts;
    int port_number;
    if(argc!=5)
      {
	printf("Incorrect usage: %s -s <Buffer length> <Eval type>: 1 for RDTSCP\n",argv[0]);
	return 1;
      }
    buff_length = atoi(argv[2]);
    char sendBuff[buff_length];
    char recvBuff_server[buff_length];
    char ack_recieve_buffer[1];
    eval_type = atoi(argv[3]);
    port_number = atoi(argv[4]);

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
     int i2=100;
    while(i2!=-1){
     i2--;
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
      if(eval_type)
	{
	  begin = RDTSCP();
	  int n;
	  numBytes = 0;
	  while(numBytes < buff_length)
          {
                if ((n = write(connfd, sendBuff, buff_length-numBytes)) > 0)
			numBytes+=n;
	  }
	  printf("Server Bytes written: %d\n",numBytes);
	  
	  shutdown(connfd,1);
	  
	  numBytes = 0;
	  while (numBytes < buff_length)
	  {
		if ((n = read(connfd, recvBuff_server, buff_length-numBytes)) > 0)
			numBytes+=n;
	    }
	  end = RDTSCP();
	  double time = ((double)(end-begin))/((double)FREQ);
	  printf("\nRDTSCP: %f",time);
	  printf("\nBandwidth: %f MBytes/s\n", buff_length/(1024.0*1024.0*time));
	}
      else
	{
	  clock_gettime(CLOCK_MONOTONIC, &begints);
	  write(connfd, sendBuff, buff_length);
	  //printf("Server Bytes written: %d\n",buff_length);
	  
	  shutdown(connfd,1);
	  
	  int n;
	  while ( (n = read(connfd, ack_recieve_buffer, 1)) > 0)
	    {
	      numBytes+=n;
	    }
	  clock_gettime(CLOCK_MONOTONIC, &endts);
	  struct timespec tmpts = TimeSpecDiff(&endts, &begints);
	  uint64_t nsecElapsed = tmpts.tv_sec * 1000000000LL + tmpts.tv_nsec;
	  float abc = buff_length/(float)nsecElapsed;
	  printf("\nclockgettime: %f",abc);
	  
	}
      
      //printf("Server Bytes received: %d\n",numBytes);
      
         }
    close(connfd);
    close(listenfd);
    return 0;
}

