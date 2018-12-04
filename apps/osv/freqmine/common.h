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

#include <time.h>
#include "fp_tree.h"

//#define BINARY               // ASCI file or binary file

#define SUDDEN 5
#define SWITCH 15

const int power2[29] = {1, 2, 4, 8,
			16, 32, 64, 128, 256,
			512, 1024, 2048, 4096, 
			8192, 16384, 32768, 65536, 
			131072, 262144, 524288,1048576, 
			2097152, 4194304, 8388608,16777216, 
			33554432, 67108864, 134217728, 268435456};	

extern int TRANSACTION_NO; 
extern int ITEM_NO;
extern int THRESHOLD;
extern int* order_item;		// given order i, order_item[i] gives itemname
extern int* item_order;		// given item i, item_order[i] gives its new order 

extern memory** fp_buf;
extern memory** fp_tree_buf;
extern memory* database_buf;
extern memory* fp_node_sub_buf;
extern int **new_data_num; 
extern int** compact;
extern int** supp;

extern int **ITlen;
extern int** bran;
extern int** prefix;
 
extern stack** list;




