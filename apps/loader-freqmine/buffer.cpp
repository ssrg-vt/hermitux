/*
   Author:  Jianfei Zhu  
            Concordia University
   Date:    Sep. 26, 2003

Copyright (c) 2003, Concordia University, Montreal, Canada
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

   - Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
   - Neither the name of Concordia University nor the names of its
       contributors may be used to endorse or promote products derived from
       this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "common.h"
#include <malloc.h>

memory::memory()
{
	BUFPOS=20;
//	BUFS_BIG=5122880L;
//	BUFS_SMALL=1024576L;
	BUFS_BIG=1024576L;
	BUFS_SMALL=4096L;
	BUFSBSWITCH=2;
	init();
}

memory::memory(int bufpos, long bufs_small, long bufs_big, int bufsbswitch)
{
	BUFPOS=bufpos;
	BUFS_BIG=bufs_big;
	BUFS_SMALL=bufs_small;
	BUFSBSWITCH=bufsbswitch;
	init();
}

void memory::init()
{ 
	buffer = new char*[BUFPOS];
	start = new char*[BUFPOS];
	rest = new unsigned int[BUFPOS];        /* number of free positions */
	restsize = new unsigned int[BUFPOS];    /* max. size of buffer */

	buffer[0] = new char[BUFS_SMALL];
//	buffer[0] = new char[BUFS_BIG];
	if (buffer[0] == NULL) {
#ifdef DEBUG
		perror("initbuf 2"); 
#endif
		printf("INIT: not enough memory to run this program\n");
		exit(0);
	}
	start[0] = buffer[0];
	markbuf = buffer[0];
	markcount = 0;
//	markrest = BUFS_BIG;
//	rest[0] = BUFS_BIG;
//	restsize[0] = BUFS_BIG;
	markrest = BUFS_SMALL;
	rest[0] = BUFS_SMALL;
	restsize[0] = BUFS_SMALL;
	bufcount = 0;
}

memory::~memory()
{
	int i;
	for(i=0; i <= bufcount; i++)delete []buffer[i];

	delete []buffer;
	delete []start;
	delete []rest;
	delete []restsize;
}

char * memory::newbuf(unsigned int num,unsigned int size)
//	get num times size bytes from the buffer (if possible) 
//  RETURN: pointer to the first element
{ 
	register unsigned int i;
  	register int pos;
  	char *hlp;			// save current position in hlp 

	size += (i=(size & L2BITS))? MULTOF - i : 0;     // size must be a multiple of MULTOF
	i = num * size;
	for(pos=markcount; pos < bufcount; pos++)
		if (rest[pos] >= i) break;
	
	if (rest[pos] < i) pos = switchbuf(i);
	hlp = start[pos];
	start[pos] += i; // adjust start and rest
	rest[pos] -= i;

	return hlp;
}

int memory::switchbuf(unsigned int i)  // creates a new buffer with size >= i 
{
	bufcount++;		// the new buffer has number bufcount
	if (bufcount == BUFPOS) 
	{ 
		buffree(); bufcount--; 
		printf("The blocks are used up.\n");
		delete this;
		exit(0);

  //		freebuf(); 
	}
	if (bufcount < BUFSBSWITCH) rest[bufcount] = BUFS_SMALL;
	else{
		int j = BUFPOS/(BUFPOS-bufcount);
		if(j<12)
			rest[bufcount] = power2[j-1]*BUFS_BIG;
		else 
			rest[bufcount] = power2[11]*BUFS_BIG;
	}

	if (rest[bufcount] < i) rest[bufcount] = i;
	restsize[bufcount] = rest[bufcount];

	buffer[bufcount] = new char[rest[bufcount]];
	if (buffer[bufcount] == NULL) {
#ifdef DEBUG
		perror("switchbuf 1");
#endif
		buffree(); 
		printf("Not enough memory!\n");
		delete this;
//		freebuf(); 
	}
	start[bufcount] = buffer[bufcount]; 
	return bufcount;
} // switchbuf 

char* memory::bufmark(unsigned int*MR, int* MC)		// set a mark in the buffer for freebuf
{	
	markbuf = start[bufcount]; markcount = bufcount;
	// markrest = restsize[bufcount] - rest[bufcount];
    markrest = rest[bufcount];
	*MR=markrest; *MC = markcount;
	return markbuf;
}

/*void memory::bufmark()		// set a mark in the buffer for freebuf
{	
	markbuf = start[bufcount]; markcount = bufcount;
	// markrest = restsize[bufcount] - rest[bufcount];
    markrest = rest[bufcount]; 
}*/
void memory::freebuf(unsigned int MR, int MC, char* MB)	//clear the buffer above the mark 
{	
	int i;
//	register char *pts;
//	for(i=markcount+1; i <= bufcount; i++) 
int freesize = 0;
	for(i=MC+1; i <= bufcount; i++) 
	{
//		dispose(buffer[i]);
		delete []buffer[i];
		freesize += start[i] - buffer[i];
		rest[i] = 0; 
		restsize[i] = 0;
		start[i] = NULL; 
		buffer[i] = NULL;
	}
    bufcount=MC;

	// clear area: positions markbuf to start-1 are used
//	for(pts = start[MC]; pts != MB; *(--pts) = 0);
	freesize+= MR - rest[bufcount];
	start[bufcount] = MB; rest[bufcount] = MR;
}


void memory::buffree()   // delete the mark
{	
	markbuf = buffer[0]; markcount = 0; markrest = BUFS_SMALL;
}
