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

#ifndef BUFFERCLASS
#define BUFFERCLASS
#define L2BITS	        0x3   // L2BITS: the two least sig. bits

#define MULTOF		8		// MULTOF: addresses should start at numbers which are divisible by MULTOF 

class memory{   
private:
	int BUFPOS;				//default: 40   number of tries to fetch memory from the OS
	long BUFS_BIG;			//default: 6291456 buffer size
	long BUFS_SMALL;		//default: 2097152 buffer size 
	int BUFSBSWITCH; 		//default:2 switch from small to big */
//	char *buffer[BUFPOS];
	char **buffer;
	int bufcount;               /* marks current buffer position */
//	char *start[BUFPOS];        /* pt to the next free position */
	char **start;        /* pt to the next free position */
//	unsigned int rest[BUFPOS];        /* number of free positions */
	unsigned int *rest;        /* number of free positions */
//	unsigned int restsize[BUFPOS];    /* max. size of buffer */
	unsigned int *restsize;    /* max. size of buffer */
	char *markbuf;              /* mark for freebuf */
	int markcount;
	unsigned int markrest;
private:
	int switchbuf(unsigned int i);
	void init();
public:
	memory();
	memory(int bufpos, long bufs_small, long bufs_big, int bufsbswitch);
	~memory();		
	void freebuf(unsigned int MR, int MC, char* MB);   //clear the buffer above the mark 
	char * newbuf(unsigned int num,unsigned int size);
//	void bufmark();
	char* bufmark(unsigned int*, int*);
	void buffree();
	bool half(){return BUFPOS/(BUFPOS-bufcount) > 2;}
};
#endif

