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

#ifndef _FP_TREE_CLASS
#define _FP_TREE_CLASS

#include <fstream>
#include "buffer.h"
#include "data.h"
#include "fp_node.h"
#include "fsout.h"

#define SORTHRESH 9

  
class FP_tree {
public:
	int itemno;		//Header_table
	int *order;		//Header_table
	int *table;		//orders[table[i]]=i; table[orders[i]]=i;
	Fnode* Root;

	int* count;		//Header_table

	int **NodeArrayList;
	int *ItemArray;
	int *nodenum;

	int MC_tree;			//markcount for memory
	unsigned int MR_tree;	//markrest for memory
	char* MB_tree;
	int* MC_nodes;
	unsigned int *MR_nodes;
	char** MB_nodes;
	int num_hot_item;
private:
	void scan1_DB(int,FP_tree*, int);			//build header_table
	int cal_level_25(int);
	void powerset(int*, int, int*, int, int, FSout*, int)const;
	void release_node_array_after_mining(int sequence, int thread, int workingthread);
	void release_node_array_before_mining(int sequence, int thread, int workingthread);
	void database_tiling(int workingthread);
public:
	void init(int Itemno, int new_item_no, int thread);
	~FP_tree(){/*delete root;	delete []order;	delete []table;*/};

	void scan1_DB(Data*);		//find the count of all nodes from origional DB
	void scan2_DB(int workingthread);
	bool Single_path(int)const;   //Is it a single path?
	void generate_all(int,int,FSout*)const;
	int FP_growth_first(FSout* fout);
	int FP_growth(int thread, FSout* fout);
	void fill_count(int, int);
	void insert(int* compact, int counts, int current, int ntype, int thread);
};



class stack{
public:
	int top;
	int* FS;
	int* counts;
public:
	stack(int);
	~stack();
	void insert(FP_tree* fptree);
};

#endif

