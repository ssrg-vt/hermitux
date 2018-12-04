/*
   Author:  Jianfei Zhu  
            Concordia University
   Date:    Feb. 10, 2004

Copyright (c) 2004, Concordia University, Montreal, Canada
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

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <sched.h>  
#include <sys/syscall.h>
#include "buffer.h"
#include "common.h"
#include "wtime.h"

#ifdef _OPENMP
#include <omp.h>
#else
static int omp_get_max_threads() {return 1;}
static int omp_get_thread_num() {return 0;}
#endif //_OPENMP

#define fast_rightsib_table_size 16
int ***currentnodeiter;
Fnode ***nodestack;
int **itemstack;
int** origin; 
Fnode ***hashtable;
int **hot_node_count;
int *hot_node_depth;
int *hot_node_index;
Fnode ****fast_rightsib_table;
Fnode ****rightsib_backpatch_stack;
int **global_count_array;
int **global_table_array;
int **global_temp_order_array;
int **global_order_array;
int **rightsib_backpatch_count;
int **sum_item_num;
int **global_nodenum;
int lowerbound_array[3] = {0x10000, 0x100, 0};
int **ntypearray;
int *thread_finish_status;
int *thread_begin_status;
int released_pos;
int* first_MC_tree;
unsigned int * first_MR_tree;
char** first_MB_tree;


#define hot_item_num	16
#define hot_item_num1	6
#define hot_node_num	(1<<hot_item_num)
#define _MM_HINT_T0     1
#define _MM_HINT_T1     2
#define _MM_HINT_T2     3
#define _MM_HINT_NTA    0

MapFile *mapfile;
MapFile **thread_mapfile;
int sumntype[hot_node_num];
int ntypehashtable[hot_node_num];
int ntypeidarray[hot_node_num];
int mergedworkbase[hot_node_num];
int mergedworkend[hot_node_num];
int mergedworknum;
unsigned short *threadtranscontent;
int **threadntypeoffsetiter;
int *ntypeoffsetbase;
int *ntypeoffsetend;
int numusefulntype;
int **threadworkload;
int *threadworkloadnum;


template <class T> void transform_FPTree_into_FPArray(FP_tree *fptree, int thread, T mark)
{
	Fnode* temp;
	int i;
	memory *local_buf = fp_buf[thread];
	int **local_currentnodeiter = currentnodeiter[thread];
	Fnode **local_nodestack = nodestack[thread];
	int *local_itemstack = itemstack[thread];

	T *ItemArray;
	fptree->NodeArrayList = (int**)local_buf->newbuf(1, fptree->itemno * (sizeof(int*) * 2 + sizeof(int) * 2));
	fptree->MB_nodes = (char**)(fptree->NodeArrayList + fptree->itemno);
	fptree->MC_nodes = (int*)(fptree->MB_nodes + fptree->itemno);
	fptree->MR_nodes = (unsigned int*)fptree->MC_nodes + fptree->itemno;
	new_data_num[thread][0] ++;
	ItemArray = (T*)local_buf->newbuf(1, new_data_num[thread][0] * sizeof(T));
	for (i = 0; i < fptree->itemno; i ++) {
		fptree->MB_nodes[i]=local_buf->bufmark(&fptree->MR_nodes[i], &fptree->MC_nodes[i]);
		local_currentnodeiter[i] = fptree->NodeArrayList[i] = (int*)local_buf->newbuf(1, fptree->nodenum[i] * 2 * sizeof(int));
	}
	int itemiter = new_data_num[thread][0] - 1;

	fptree->Root->count = 0;
	local_nodestack[0] = fptree->Root->leftchild;
	int stacktop = 0;
	int kept_itemiter = new_data_num[thread][0];
	bool	first = true;
	while (stacktop != -1) {
		temp = local_nodestack[stacktop];
		stacktop --;
		if (temp) {
			if (!first && temp->leftchild == NULL) {
				stacktop ++;
				local_nodestack[stacktop] = temp->rightsibling;
				int itemname = temp->itemname;
				int itemcount = temp->count;
				int *nodeiter = local_currentnodeiter[itemname];
				local_currentnodeiter[itemname] += 2;
				nodeiter[0] = kept_itemiter;
				nodeiter[1] = itemcount;	
				kept_itemiter --;
			}
			else {
				first = false;
				ItemArray[itemiter] = mark;
				itemiter --;
				for (i = 0; i <= stacktop; i ++) {
					ItemArray[itemiter] = (T)local_itemstack[i];
					itemiter --;
				}
				for (; temp != NULL; temp = temp->leftchild) {
					stacktop ++;
					local_nodestack[stacktop] = temp->rightsibling;
					int itemname = temp->itemname;
					int itemcount = temp->count;
					local_itemstack[stacktop] = itemname;
					ItemArray[itemiter] = (T) itemname;
					itemiter --;
					int *nodeiter = local_currentnodeiter[itemname];
					local_currentnodeiter[itemname] += 2;
					nodeiter[0] = (itemiter + 2);
					nodeiter[1] = itemcount;
				}
				kept_itemiter = itemiter + 1;
				itemiter ++;
			}
		}
		kept_itemiter ++;
	}
	fptree->ItemArray = (int *) ItemArray;
}

template <class T> void first_transform_FPTree_into_FPArray(FP_tree *fptree, T mark)
{
	int j;
	memory *local_buf = fp_buf[0];
	int sum_new_data_num;
	sum_new_data_num = 0;
	fptree->MB_nodes = (char**) local_buf->newbuf(1, fptree->itemno * (sizeof(int*) + sizeof(int) * 2));
	fptree->MC_nodes = (int*)(fptree->MB_nodes + fptree->itemno);
	fptree->MR_nodes = (unsigned int*)fptree->MC_nodes + fptree->itemno;
	int workingthread = omp_get_max_threads();	
	int *content_offset_array = new int [workingthread];

	for (j = 0; j < workingthread; j ++) {
		new_data_num[j][0] ++;
		sum_new_data_num += new_data_num[j][0];
		content_offset_array[j] = sum_new_data_num;
	}
	int **node_offset_array = new int *[workingthread + 1];
	node_offset_array[0] = new int [(workingthread + 1) * fptree->itemno];
	for (j = 1; j <= workingthread; j ++)
		node_offset_array[j] = node_offset_array[j - 1] + fptree->itemno;
	for (j = 0; j < fptree->itemno; j ++)
		node_offset_array[0][j] = 0;
	for (j = 1; j <= workingthread; j ++)
		for (int i = 0; i < fptree->itemno; i ++)
			node_offset_array[j][i] = node_offset_array[j - 1][i] + global_nodenum[j - 1][i];
	for (j = 0; j < fptree->itemno; j ++) {
		fptree->MB_nodes[j]=fp_node_sub_buf->bufmark(&fptree->MR_nodes[j], &fptree->MC_nodes[j]);
		fptree->nodenum[j] = node_offset_array[workingthread][j];
		fptree->NodeArrayList[j] = (int*)fp_node_sub_buf->newbuf(1, node_offset_array[workingthread][j]* 2 * sizeof(int));
	}
	new_data_num[0][0] = sum_new_data_num;
	T *ItemArray = (T *)local_buf->newbuf(1, new_data_num[0][0] * sizeof(T));
#pragma omp parallel for
	for (j = 0; j < workingthread; j ++) {
		int kept_itemiter;
		int itemiter = content_offset_array[j] - 1;
		int stacktop;
		int shift_bit;
		int i, k;
		Fnode **local_nodestack = nodestack[j];
		int *local_itemstack = itemstack[j];
		int **local_currentnodeiter = currentnodeiter[j];
		Fnode* temp;
		for (i = 0; i < fptree->itemno; i ++)
			local_currentnodeiter[i] = fptree->NodeArrayList[i] + 2 * node_offset_array[j][i];
		for (k = 0; k < threadworkloadnum[j]; k ++) {
			int ntype = threadworkload[j][k];
			bool first = true;
			stacktop = 0;
			kept_itemiter = itemiter + 1;
			if (hashtable[0][ntype] == fptree->Root)
				local_nodestack[0] = fptree->Root->leftchild;
			else {
				for (i = 0, shift_bit = 1; i < hot_item_num; i ++, shift_bit <<=1) {
					if ((shift_bit & ntype) != 0) {
						local_itemstack[stacktop] = i;
						local_nodestack[stacktop] = NULL;
						stacktop ++;
					}
				}
				stacktop --;
				local_nodestack[stacktop] = hashtable[0][ntype];
			}
			while (stacktop != -1) {
				temp = local_nodestack[stacktop];
				stacktop --;
				if (temp) {
					if (temp->leftchild ==NULL && !first) {
						stacktop ++;
						local_nodestack[stacktop] = temp->rightsibling;
						int itemname = temp->itemname;
						int itemcount = temp->count;
						int *nodeiter = local_currentnodeiter[itemname];
						local_currentnodeiter[itemname] += 2;
						nodeiter[0] = kept_itemiter;
						nodeiter[1] = itemcount;
						kept_itemiter --;
					}
					else {
						first = false;
						ItemArray[itemiter] = mark;
						itemiter --;
						for (i = 0; i <= stacktop; i ++) {
							ItemArray[itemiter] = (T)local_itemstack[i];
							itemiter --;
						}
						for (; temp != NULL; temp = temp->leftchild) {
							stacktop ++;
							local_nodestack[stacktop] = temp->rightsibling;
							int itemname = temp->itemname;
							int itemcount = temp->count;
							local_itemstack[stacktop] = itemname;
							ItemArray[itemiter] = (T) itemname;
							itemiter --;
							int *nodeiter = local_currentnodeiter[itemname];
							local_currentnodeiter[itemname] += 2;
							nodeiter[0] = (itemiter + 2);
							nodeiter[1] = itemcount;
						}
						kept_itemiter = itemiter + 1;
						itemiter ++;
					}
				}
				kept_itemiter ++;
			}
		}
	}
	fptree->ItemArray = (int *) ItemArray;
}

template <class T> int FPArray_conditional_pattern_base(FP_tree *fptree, int itemname, int thread, T mark)
{
	int i, j;
	int numnode = fptree->nodenum[itemname];
	int* nodearray = fptree->NodeArrayList[itemname];
	T* Trans;
	T * ItemArray = (T *) fptree->ItemArray;
	int local_sum_item_num = 0;
	int *local_supp = supp[thread];
	int *local_global_table_array = global_table_array[thread];
	int *local_global_count_array = global_count_array[thread];
	int *local_global_temp_order_array = global_temp_order_array[thread];
	
	for (i = 0; i < numnode; i ++) {
		#ifdef ENABLE_PREFETCHING
		_mm_prefetch(ItemArray + nodearray[8], _MM_HINT_T0);
		#endif
		int begin = nodearray[0];
		int tempcount = nodearray[1];
		nodearray += 2;
		Trans = ItemArray + begin;
		for (j = 0; Trans[j] != mark; j ++) {
			local_supp[Trans[j]]+= tempcount;
		}
		local_sum_item_num += j;
	}
	sum_item_num[thread][0] = local_sum_item_num;		
	j = 0;
	for(i=0; i<itemname; i++)
	{
		if(local_supp[i]>=THRESHOLD)
		{
			local_global_table_array[j] = fptree->table[i];
			local_global_count_array[j] = local_supp[i];
			j++;
		}else if (local_supp[i] > 0) {
			local_global_temp_order_array[fptree->table[i]] = -1;
		}
		local_supp[i] = 0;
	}
			
	return j;

}

template <class T> void FPArray_scan2_DB(FP_tree*fptree, FP_tree* old_tree, int itemname, int thread, T mark)
{
	int i, has;
	int *local_origin = origin[thread];
	int *local_hot_node_count = hot_node_count[thread];
	Fnode **local_hashtable = hashtable[thread];
	memory *local_fp_tree_buf = fp_tree_buf[thread];
	int *	local_compact = compact[thread];

	int numnode = old_tree->nodenum[itemname];
	int* nodearray = old_tree->NodeArrayList[itemname];
	T* Trans;
	T* ItemArray = (T *)old_tree->ItemArray;
	int ntype;
	int max_itemno;
	int *local_global_order_array = global_order_array[thread];
	int num_hot_item = fptree->num_hot_item;
	for (i = 0; i < numnode; i ++) {
		#ifdef ENABLE_PREFETCHING
		_mm_prefetch(ItemArray + nodearray[6], _MM_HINT_NTA);
		#endif
		int begin = nodearray[0];
		int tempcount = nodearray[1];
		nodearray += 2;
		Trans = ItemArray + begin;
		has = 0;
		ntype = 0;
		max_itemno = 0;
		for (int j = 0; Trans[j] != mark; j ++) {
			int item = local_global_order_array[Trans[j]];
			if (item < num_hot_item) {
				if (item != -1) 
					ntype |= (1 << item);
			} else {	
				local_origin[item]=1;
				has++;
				if (item > max_itemno)
						max_itemno = item;
			}
		}
		local_hot_node_count[ntype] += tempcount;
		if (local_hashtable[ntype] == NULL) {
			local_hashtable[ntype] = (Fnode*)local_fp_tree_buf->newbuf(1, sizeof(Fnode));
			local_hashtable[ntype]->init(hot_node_index[ntype], 0);
		}
		if(has)
		{
			fptree->fill_count(max_itemno, thread);
			fptree->insert(local_compact, tempcount, has, ntype, thread);
		}
	}
	int local_new_data_num = new_data_num[thread][0];
	local_new_data_num ++;
	int step, num_hot_node, parent;
	Fnode *current_node, *parent_node;
	num_hot_node = 1<<num_hot_item;
	for (i=num_hot_node-1;i>0;i--) {
		if (local_hashtable[i] == NULL) 
			continue;
		step = hot_node_index[i];
		parent = i ^ (1 << step);
		local_hot_node_count[parent] += local_hot_node_count[i];
		parent_node = local_hashtable[parent];
		if (parent_node == NULL) {
			parent_node = (Fnode*)local_fp_tree_buf->newbuf(1, sizeof(Fnode));
			parent_node->itemname = hot_node_index[parent];
			parent_node->rightsibling = NULL;
			parent_node->leftchild = NULL;
			local_hashtable[parent] = parent_node;
		}
		if (parent_node->leftchild == NULL)
			local_new_data_num ++;
		else local_new_data_num += hot_node_depth[i];
		current_node = local_hashtable[i];
		current_node->rightsibling = parent_node->leftchild;
		parent_node->leftchild = current_node;
		current_node->count = local_hot_node_count[i];
		local_hashtable[i] = NULL;
		local_hot_node_count[i] = 0;
		fptree->nodenum[step] ++;
	}
	new_data_num[thread][0] = local_new_data_num; 
	int local_rightsib_backpatch_count = rightsib_backpatch_count[thread][0];
	Fnode ***local_rightsib_backpatch_stack = rightsib_backpatch_stack[thread];
	for (i = 0; i < local_rightsib_backpatch_count; i ++)
		*local_rightsib_backpatch_stack[i] = NULL;
}

template <class T1, class T2> void transform_FPArray(T1 *oldItemArray, T2 mark, int size)
{
	T2 * newItemArray = (T2 *) oldItemArray;
	for (int i = 0; i < size; i ++) 
		newItemArray[i] = oldItemArray[i] & mark;
}

template <class T> void swap(T* k, T* j)
{
	T temp;
	temp = *j;
	*j = *k;
	*k = temp;
}

int findpivot(const int& i, const int& j) {return (i+j)/2;}

int partition(int* array, int* temp, int l, int r, int pivot)
{
	do {
		while (array[++l] > pivot);
		while (r && array[--r] < pivot);
		swap(array+l, array+r);
		swap(temp+l, temp+r);
	}while (l < r);
	swap(array + l, array + r);
	swap(temp + l, temp + r);
	return l;
}

void inssort(int* array, int* temp, int i, int j)
{
	for (int k=i+1; k<=j; k++)
		for (int m=k; (m>i) && (array[m] > array[m-1]); m--)
		{
			swap(array+m, array+m-1);
		    swap(temp+m, temp+m-1);
		}
}

void sort(int *array, int* temp, int i, int j)
{
	if(j-i < SORTHRESH) inssort(array, temp, i, j);
	else
	{
		int pivotindex = findpivot(i, j);
		swap(array+pivotindex, array+j);
		swap(temp+pivotindex, temp+j);
		int k = partition(array, temp, i-1, j, array[j]);
		swap(array+k, array+j);
		swap(temp+k, temp+j);
		if((k-i)>1) sort(array, temp, i, k-1);
		if((j-k)>1) sort(array, temp, k+1, j);
	}
}

stack::stack(int length)
{
	top= 0; 
	FS = new int[length];
	counts = NULL;
}

stack::~stack()
{
	delete []FS;
}

void stack::insert(FP_tree* fptree)
{
	for(Fnode* node=fptree->Root->leftchild; node!=NULL; node=node->leftchild)
	{
		FS[top]=node->itemname; 
		top++;
	}
}

void FP_tree::init(int old_itemno, int new_itemno, int thread)
{
	int i;
	Root = (Fnode*)fp_buf[thread]->newbuf(1, sizeof(Fnode));
	Root->init(-1, 0);
	if(old_itemno!=-1)
	{
		count = (int*)fp_buf[thread]->newbuf(1,new_itemno * 2 * sizeof(int));
		table = count + new_itemno;
		for (i=0; i<new_itemno; i++)
		{
			count[i] = 0;
			table[i] = i;
		}
		new_data_num[thread][0] = 0;
	}
	itemno = new_itemno;
}

void FP_tree::database_tiling(int workingthread)
{
	int i;
	int *thread_pos = new int [workingthread];
	int local_num_hot_item = num_hot_item;
	int local_itemno = itemno;

	for (i = 0; i < workingthread; i ++) {
		thread_pos[i] = 0;
		int j;
		for (j = 0; j < hot_node_num; j ++)
			ntypearray[i][j] = 0;
		for (j = local_num_hot_item; j < local_itemno; j ++)
			origin[i][j] = 1;
	}
#pragma omp parallel for schedule(dynamic,1)
	for (i = 0; i < mapfile->tablesize; i ++) {
		int k, l;
		int *content;
		MapFileNode *currentnode;
		MapFileNode *newnode;
		int size;
		unsigned short *newcontent;
		int currentpos;
		int thread = omp_get_thread_num();
		int *local_origin = origin[thread];
		int *local_ntype = ntypearray[thread];
		int ntype;
		int item;
		int has;
		int *local_hot_node_count = hot_node_count[thread];
		newnode = thread_mapfile[thread]->first;
		size = newnode->size;
		newcontent = (unsigned short *) newnode->TransactionContent;
		currentpos = thread_pos[thread];
		int max_item = 0;
		int min_item = local_itemno;
			currentnode = mapfile->table[i];
			ntype = 0;
			content = currentnode->TransactionContent;
			has = 0;
			for (k = currentnode->top - 1; k >= 0; k --) {
				if (content[k] == -1) {
					ntype &= 0xffff;
					if (has > 0) {					
						if (size - currentpos < has + 1) {
							newnode->top = currentpos;
							newnode = (MapFileNode *)fp_tree_buf[thread]->newbuf(1, sizeof(MapFileNode));
							newnode->init(5000000, 2);
							newnode->next = thread_mapfile[thread]->first;
							thread_mapfile[thread]->first = newnode;
							newcontent = (unsigned short *) (newnode->TransactionContent);
							currentpos = 0;
						}
						newcontent[currentpos ++] = ntype;
						newcontent[currentpos ++] = has;
						local_ntype[ntype] += has + 1;
						for (l = min_item; l <= max_item; l ++)
							if (local_origin[l] != 1) {
								newcontent[currentpos ++] = l;
								local_origin[l] = 1;
							}
						has = 0;
						max_item = 0;
						min_item = local_itemno;
					}
					local_hot_node_count[ntype] ++;
					ntype = 0;
				}
				else {
					item = item_order[content[k]];
					if (item < local_num_hot_item) {
						ntype |= (1 << item);
					} else
					{
						has += local_origin[item];
						local_origin[item] = 0;
						if (item > max_item)
							max_item = item;
						if (item < min_item)
							min_item = item;
					}
				}
			}
			ntype &= 0xffff;
			if (has > 0) {
				if (size - currentpos < has + 1) {
					newnode->top = currentpos;
					newnode = (MapFileNode *)fp_tree_buf[i]->newbuf(1, sizeof(MapFileNode));
					newnode->init(5000000, 2);
					newnode->next = thread_mapfile[i]->first;
					thread_mapfile[i]->first = newnode;
					newcontent = (unsigned short *) (newnode->TransactionContent);
					currentpos = 0;
				}
				newcontent[currentpos ++] = ntype;
				newcontent[currentpos ++] = has;
				local_ntype[ntype] += has + 1;
				for (l = min_item; l <= max_item; l ++)
					if (local_origin[l] != 1) {
						newcontent[currentpos ++] = l;
						local_origin[l] = 1;
					}
			}
			local_hot_node_count[ntype] ++;
			newnode->top = currentpos;
			currentnode->finalize();
			thread_pos[thread] = currentpos;
	}
	
	for (i = 0; i < workingthread; i ++) {
		thread_pos[i] = 0;
		int j;
		for (j = local_num_hot_item; j < local_itemno; j ++)
			origin[i][j] = 0;
	}

	int sumworkload = 0;
	numusefulntype = 0;
	int averworkload = 0;
	ntypeoffsetbase = new int [hot_node_num];
	ntypeoffsetend = new int [hot_node_num];
	int *tempntypeoffsetbase = new int [hot_node_num];
	int tempworkload;
	for (i = 0; i < hot_node_num; i ++) {
		sumntype[i] = 0;
		for (int j = 0; j < workingthread; j ++)
			sumntype[i] += ntypearray[j][i];
		if (sumntype[i] > 0)
			numusefulntype ++;
		tempntypeoffsetbase[i] = ntypeoffsetbase[i] = sumworkload;
		sumworkload += sumntype[i];
		ntypeoffsetend[i] = sumworkload;
	}
	int num_hot_node = 1<<local_num_hot_item;
	int j, step, parent;
	Fnode **local_hashtable = hashtable[0];
	int *local_hot_node_count = hot_node_count[0];
	Fnode *current_node;
	threadworkload = new int * [workingthread];
	threadworkloadnum = new int [workingthread];
	for (j = 0; j < workingthread; j ++) {
		threadworkloadnum[j] = 0;
		threadworkload[j] = new int [hot_node_num];
	}
	for (i=num_hot_node-1;i>0;i--) {
		for (j = 1; j < workingthread; j ++) {
			local_hot_node_count[i] += hot_node_count[j][i];
			hot_node_count[j][i] = 0;
		}
		if (local_hot_node_count[i] == 0) 
			continue;
		current_node = (Fnode*)fp_tree_buf[0]->newbuf(1, sizeof(Fnode));
		current_node->itemname = hot_node_index[i];
		current_node->rightsibling = NULL;
		current_node->leftchild = NULL;
		current_node->count = local_hot_node_count[i];
		local_hashtable[i] = current_node;
		step = hot_node_index[i];
		parent = i ^ (1 << step);
		local_hot_node_count[parent] += local_hot_node_count[i];
		local_hot_node_count[i] = 0;
		if (sumntype[i] == 0) {
			threadworkload[0][threadworkloadnum[0]] = i;
			threadworkloadnum[0] ++;
			global_nodenum[0][step] ++;
			new_data_num[0][0] += hot_node_depth[i] + 1;
		}
	}	
	threadtranscontent = (unsigned short *) database_buf->newbuf(1, sumworkload * sizeof(short));
	sort(sumntype, ntypeidarray, 0, hot_node_num - 1);
	averworkload = sumworkload / 512;
	tempworkload = 0;
	mergedworkbase[0] = 0;
	mergedworknum = 0;
	for (i = 0; i < numusefulntype; i ++) {
		tempworkload += sumntype[i];
		if (tempworkload >= averworkload) {
			mergedworkend[mergedworknum] = i + 1;
			mergedworknum ++;
			mergedworkbase[mergedworknum] = i + 1;
			tempworkload = 0;
		}
	}
	mergedworkend[mergedworknum] = i;
	mergedworknum ++;

	averworkload = 0;
	averworkload = sumworkload / workingthread;
	j = 0;

	for (i = 0; i < hot_node_num; i ++)
		for (j = 0; j < workingthread; j ++) {
			if (ntypearray[j][i] > 0) {
				threadntypeoffsetiter[j][i] = tempntypeoffsetbase[i];
				tempntypeoffsetbase[i] += ntypearray[j][i];
			}
		}

#pragma omp parallel for
	for (i = 0; i < workingthread; i ++) {
		MapFileNode *current_mapfilenode;
		unsigned short * content;
		int k, size, current_pos, ntype, has;
		unsigned short *new_content;
		int *local_threadntypeoffsetiter = threadntypeoffsetiter[i];
		current_mapfilenode = thread_mapfile[i]->first;
		while (current_mapfilenode) {
			size = current_mapfilenode->top;
			current_pos = 0;
			content = (unsigned short *)current_mapfilenode->TransactionContent;
			while (current_pos < size) {
				ntype = content[current_pos];
				current_pos ++;
				has = content[current_pos];
				new_content = threadtranscontent + local_threadntypeoffsetiter[ntype];
				local_threadntypeoffsetiter[ntype] += has + 1;
				for (k = 0; k < has + 1; k ++)
					new_content[k] = content[current_pos ++];
			}
			current_mapfilenode->finalize();
			current_mapfilenode = current_mapfilenode->next;
		}
	}
	delete [] tempntypeoffsetbase;
	delete [] thread_pos;
}


void FP_tree::scan1_DB(Data* fdat)
{
	int i,j;
	int *counts;
	int thread = omp_get_thread_num();

	mapfile = (MapFile*)database_buf->newbuf(1, sizeof(MapFile));
	mapfile->first = NULL;
	
	counts = fdat->parseDataFile(mapfile);

	order = (int*)fp_buf[thread]->newbuf(1, ITEM_NO * 3 * sizeof(int));
	table = order + ITEM_NO;
	count = table + ITEM_NO;
	for (i=0; i<ITEM_NO; i++)
	{
		order[i]=-1;
		count[i] = counts[i];
		table[i] = i;
	}

	sort(count, table, 0, ITEM_NO-1);
	
	for (i =0; i<ITEM_NO&&count[i] >= THRESHOLD; i++);

	itemno = i;

	for (j=0; j<itemno; j++)
	{
		count[j]=counts[table[j]];  
		order[table[j]]=j;
	}

	order_item = new int[itemno];
	item_order = new int[ITEM_NO];
	for(i=0; i<itemno; i++)
	{
		order_item[i]=table[i];
		table[i]=i;
		item_order[i] = order[i];
		order[i]=i;
	}
	for(;i<ITEM_NO; i++)
	{
		item_order[i] = order[i];
		order[i]=-1;
	}
	ITEM_NO = itemno;
	
	delete []counts;
	MC_tree = 0;
	MR_tree = 0;
	MB_tree=fp_tree_buf[thread]->bufmark(&MR_tree, &MC_tree);
	num_hot_item = hot_item_num;
	if (num_hot_item > itemno)
		num_hot_item = itemno;
	int num_hot_node = 1 << hot_item_num;
	int workingthread=omp_get_max_threads();

	thread_mapfile = (MapFile **)database_buf->newbuf(1, workingthread*3*sizeof(int*));
	ntypearray = (int **) (thread_mapfile + workingthread);
	threadntypeoffsetiter = (int **) (ntypearray + workingthread);
	first_MC_tree = (int *) fp_tree_buf[0]->newbuf(1, workingthread*(2*sizeof(int) + sizeof(int*)));
	first_MB_tree = (char **) (first_MC_tree + workingthread);
	first_MR_tree = (unsigned int *) (first_MB_tree + workingthread);
	for (i = 0; i < workingthread; i ++) {
		first_MB_tree[i] = fp_tree_buf[i]->bufmark(&first_MR_tree[i], &first_MC_tree[i]);
		thread_mapfile[i] = (MapFile *)database_buf->newbuf(1, sizeof(MapFile) + sizeof(MapFileNode) + hot_node_num * 2 * sizeof(int));
		thread_mapfile[i]->init();
		thread_mapfile[i]->first = (MapFileNode *) (thread_mapfile[i] + 1);
		thread_mapfile[i]->first->init(2000000, 2);
		ntypearray[i] = (int *) (thread_mapfile[i]->first + 1);
		threadntypeoffsetiter[i] = (int *) (ntypearray[i] + hot_node_num);
	}
	{
		currentnodeiter = (int***)fp_buf[thread]->newbuf(1, workingthread * 25 * sizeof(int*) + itemno * 2 * sizeof(int*) + num_hot_node * 2 * sizeof(int*));
		itemstack = (int**) (currentnodeiter + workingthread);
		origin = itemstack + workingthread;
		global_count_array = origin + workingthread;
		global_temp_order_array = global_count_array + workingthread;
		global_order_array = global_temp_order_array + workingthread;
		global_table_array = global_order_array + workingthread;
		hot_node_count = global_table_array + workingthread;
		supp = hot_node_count + workingthread;
		ITlen = supp + workingthread;
		bran = ITlen + workingthread;
		compact = bran + workingthread;
		prefix = compact + workingthread;
		rightsib_backpatch_count = prefix + workingthread;
		sum_item_num = rightsib_backpatch_count + workingthread;
		new_data_num = sum_item_num  + workingthread;
		list = (stack **) (new_data_num + workingthread);
		hashtable = (Fnode ***) (list + workingthread);
		nodestack = hashtable + workingthread;
		fast_rightsib_table = (Fnode ****) (nodestack + workingthread);
		rightsib_backpatch_stack = fast_rightsib_table + workingthread;
		nodenum = (int *) (rightsib_backpatch_stack + workingthread);
		NodeArrayList = (int **) (nodenum + itemno);
		thread_finish_status = (int *) (NodeArrayList + itemno);
		thread_begin_status = thread_finish_status + workingthread;
		hot_node_depth = thread_begin_status + workingthread;
		hot_node_index = hot_node_depth + num_hot_node;
		global_nodenum = (int **) (hot_node_index + num_hot_node);
		for (i = 0; i < workingthread; i ++) {
			list[i] = new stack(itemno);
			thread_finish_status[i] = itemno;
			thread_begin_status[i] = itemno - 1;
		}
		released_pos = itemno;
		for (i = 0; i < itemno; i ++)
			nodenum[i] = 0;
	}
	for (i = 1; i < num_hot_node; i ++) {
		hot_node_depth[i] = 0;
		for (j = 1<<(num_hot_item - 1); j != 0; j >>= 1)
			if ((j & i) != 0) {
				hot_node_depth[i] ++;
			}
		for (j = num_hot_item - 1; ((1 << j) & i) == 0; j --);
			hot_node_index[i] = j;
	}
	hot_node_depth[0] = 0;
	#pragma omp parallel for
	for (int k = 0; k < workingthread; k ++) {
		int i;
#ifdef __linux__
#ifdef CPU_SETSIZE
		cpu_set_t cpu_mask; 
		CPU_ZERO(&cpu_mask);
		CPU_SET(k,&cpu_mask);
		sched_setaffinity(k,sizeof(cpu_set_t), &cpu_mask); 
#else
		unsigned long cpu_mask = (unsigned long) 1 << MyRank;
		printf("NOT CPU_SETSIZE cpu_mask:%d\n", cpu_mask);
		sched_setaffinity(k, sizeof(unsigned long), &cpu_mask);
#endif
#endif
		currentnodeiter[k] = (int**)fp_buf[k]->newbuf(1, itemno * (14 + fast_rightsib_table_size) * sizeof(int *) + num_hot_node * 2 * sizeof(int *)  + (fast_rightsib_table_size * itemno) * sizeof(int *) + fast_rightsib_table_size + 3 * sizeof(int*));
		nodestack[k] = (Fnode**)(currentnodeiter[k] + itemno);
		itemstack[k] = (int*)(nodestack[k] + itemno);
		global_count_array[k] = itemstack[k] + itemno;
		global_table_array[k] = global_count_array[k] + itemno;
		global_temp_order_array[k] = global_table_array[k] + itemno;
		global_order_array[k] = global_temp_order_array[k] + itemno;
		supp[k] = global_order_array[k] + itemno;
		ITlen[k] = supp[k] + itemno;
		bran[k] = ITlen[k] + itemno;
		compact[k] = bran[k] + itemno;
		prefix[k] = compact[k] + itemno;
		hashtable[k] = (Fnode**) (prefix[k] + itemno);
		origin[k] = (int *) (hashtable[k] + num_hot_node);
		hot_node_count[k] = (int *) (origin[k] + itemno);
		fast_rightsib_table[k] = (Fnode ***) (hot_node_count[k] + num_hot_node);
		fast_rightsib_table[k][0] = (Fnode **) (fast_rightsib_table[k] + fast_rightsib_table_size);
		for (i = 1; i < fast_rightsib_table_size; i ++)
			fast_rightsib_table[k][i] = fast_rightsib_table[k][i - 1] + itemno;
		global_nodenum[k] = (int *)fp_tree_buf[k]->newbuf(1, itemno*sizeof(int));
		for (i = 0; i < itemno; i ++)
			global_nodenum[k][i] = 0;
		new_data_num[k] = (int *) (fast_rightsib_table[k][fast_rightsib_table_size - 1] + itemno);
		new_data_num[k][0] = 0;
		rightsib_backpatch_count[k] = new_data_num[k] + 1;
		sum_item_num[k] = rightsib_backpatch_count[k] + 1;
		rightsib_backpatch_stack[k] = (Fnode ***) (sum_item_num[k] + 1);
		rightsib_backpatch_count[k][0] = 0;
		for (i = 0; i < itemno * fast_rightsib_table_size; i ++)
			fast_rightsib_table[k][0][i] = NULL;
		for (i = 1; i < num_hot_node; i ++) {
			hot_node_count[k][i] = 0;
			hashtable[k][i] = NULL; 
		}
		hashtable[k][0] = Root;
		for (i = 0; i < itemno; i ++) {
			origin[k][i] = 0;
			supp[k][i] = 0;
			ITlen[k][i] = 0;
			bran[k][i] = 0;
		}
	}
	mapfile->transform_list_table();
	for (i = 0; i < hot_node_num; i ++)
		ntypeidarray[i] = i;
}
	
void FP_tree::insert(int* compact, int counts, int current, int ntype, int thread)
{
	Fnode* child;
	Fnode* temp, *temp2;
	Fnode** backpatch_node = NULL;
	int i=0, k;
	child = hashtable[thread][ntype];
	int *local_bran = bran[thread];
	if (ntype < fast_rightsib_table_size) {
		temp = fast_rightsib_table[thread][ntype][compact[i]];
		if (temp == NULL) {
			backpatch_node = &(fast_rightsib_table[thread][ntype][compact[i]]);
			goto OUT;
		}
		temp->count+=counts;
		child=temp;
		i++;
	}

	while(i<current)
	{
		int itemname = compact[i];
		temp=child->leftchild;
		if (temp == NULL)
			break;
		if(temp->itemname!=itemname) {
			temp = temp->rightsibling;
			while (1) {
				if (temp == NULL)
					goto OUT;
				if(temp->itemname==itemname)break;
				temp = temp->rightsibling;
			}
		}
		temp->count+=counts;
		child=temp;
		i++;
	}
OUT:
	k = current - i;
	if (k > 0) {
		temp = (Fnode*)fp_tree_buf[thread]->newbuf(1, sizeof(Fnode) * k);
		if (backpatch_node) {
			*backpatch_node = temp;
			rightsib_backpatch_stack[thread][rightsib_backpatch_count[thread][0]] = backpatch_node;
			rightsib_backpatch_count[thread][0] ++;
		}
		nodenum[compact[i]] ++;
		local_bran[i]++;
		temp->itemname = compact[i];
		temp->count = counts;
		if (child->leftchild == NULL) {
			temp->rightsibling = child->leftchild;
			child->leftchild=temp;
			new_data_num[thread][0] += k;
		} else {
			temp->rightsibling = child->leftchild->rightsibling;
			child->leftchild->rightsibling = temp;
			new_data_num[thread][0] += current + hot_node_depth[ntype];
		}
		temp2 = temp;
		temp ++;
		i++;
		while(i<current)
		{   
			nodenum[compact[i]] ++;
			local_bran[i]++;
			temp->itemname = compact[i];
			temp->rightsibling = NULL;
			temp->count = counts;
			temp2->leftchild=temp;
			temp2 = temp;
			temp ++;
			i++;
		}
		temp --;
		temp->leftchild = NULL;
	}
}

int FP_tree::cal_level_25(int thread)
{
	int i, total_25=0, total_50=0, total_bran=0, maxlen=0;
	int *local_bran = bran[thread];

	for(i=0; i<this->itemno && local_bran[i]!=0; i++);
	maxlen =i;
	for(i=0; i<int(maxlen*0.25); i++)
		total_25 +=local_bran[i];
	total_50 = total_25;
	for(i=int(maxlen*0.25); i<this->itemno*0.5; i++)
		total_50 +=local_bran[i];
	for(i=0; i<this->itemno && local_bran[i]!=0; i++)
	{
		total_bran+=local_bran[i];
		local_bran[i]=0;
	}
    return total_bran;
}

void FP_tree::fill_count(int max_itemno, int thread)
{
	int i, j=0;
	int *local_origin = origin[thread];
	int *local_compact = compact[thread];
	for(i=num_hot_item; i<= max_itemno; i++)
	{
		if (local_origin[i] != 0) {
			local_compact[j++]=i;
			local_origin[i] = 0;
		}
	}
}


void FP_tree::scan2_DB(int workingthread)
{
    double tstart, tend;
	int j;
	wtime(&tstart);
	database_tiling(workingthread);
	Fnode **local_hashtable = hashtable[0];
#pragma omp parallel for schedule(dynamic,1)
	for (j = 0; j < mergedworknum; j ++) {
		int thread = omp_get_thread_num();
		int localthreadworkloadnum = threadworkloadnum[thread];
		int *localthreadworkload = threadworkload[thread];
		int has, ntype;
		unsigned short *content = threadtranscontent;
		int *local_nodenum = global_nodenum[thread];
		memory *local_fp_tree_buf = fp_tree_buf[thread];
		int *local_bran = bran[thread];
		unsigned short* compact;
		Fnode ***local_rightsib_backpatch_stack = rightsib_backpatch_stack[thread];
		int local_rightsib_backpatch_count = rightsib_backpatch_count[thread][0];
		for (int t = mergedworkbase[j]; t < mergedworkend[j]; t ++) {
			ntype = ntypeidarray[t];
			localthreadworkload[localthreadworkloadnum] = ntype;
			localthreadworkloadnum ++;
			int size = ntypeoffsetend[ntype];
			int current_pos = ntypeoffsetbase[ntype];
			Fnode **local_fast_rightsib_table = fast_rightsib_table[thread][ntype];
			Fnode *current_root = local_hashtable[ntype];
			int current_new_data_num = 0;
			int current_hot_node_depth = hot_node_depth[ntype];
			if (ntype != 0)
				local_nodenum[hot_node_index[ntype]] ++;
			while (current_pos < size) {
				has = content[current_pos];
				current_pos += 1;
				compact = content + current_pos;
				{
					Fnode* child;
					Fnode* temp, *temp2;
					Fnode** backpatch_node = NULL;
					int i=0, k;
					child = current_root;
					if (ntype < fast_rightsib_table_size) {
						temp = local_fast_rightsib_table[compact[i]];
						if (temp == NULL) {
							backpatch_node = &(local_fast_rightsib_table[compact[i]]);
						}
						else {
							temp->count+=1;
							child=temp;
							i++;
						}
					}
					if (temp != NULL)
					while(i<has)
					{
						for(temp=child->leftchild; temp!=NULL; temp = temp->rightsibling)
						{
							if(temp->itemname==table[compact[i]])break;
						}
						if(!temp)break;
						temp->count+=1;
						child=temp;
						i++;
					}
					k = has - i;
					if (k > 0) {
						temp = (Fnode*)local_fp_tree_buf->newbuf(1, sizeof(Fnode) * k);
						if (backpatch_node) {
							*backpatch_node = temp;
							local_rightsib_backpatch_stack[local_rightsib_backpatch_count] = backpatch_node;
							local_rightsib_backpatch_count ++;
						}
						local_nodenum[compact[i]] ++;
						local_bran[i]++;
						temp->itemname = compact[i];
						temp->count = 1;
						if (child->leftchild == NULL) {
							current_new_data_num += k;
							temp->rightsibling = child->leftchild;
							child->leftchild=temp;
						} else {
							temp->rightsibling = child->leftchild->rightsibling;
							child->leftchild->rightsibling = temp;
							current_new_data_num += has + current_hot_node_depth;
						}
						temp2 = temp;
						temp ++;
						i++;
						while(i<has)
						{	
							local_nodenum[compact[i]] ++;
							local_bran[i]++;
							temp->itemname = compact[i];
							temp->rightsibling = NULL;
							temp->count = 1;
							temp2->leftchild=temp;
							temp2 = temp;
							temp ++;
							i++;
						}
						temp --;
						temp->leftchild = NULL;
					}
				}
				current_pos += has;
			}
			new_data_num[thread][0] += current_new_data_num + current_hot_node_depth + 1;
		}
		rightsib_backpatch_count[thread][0] = local_rightsib_backpatch_count;
		threadworkloadnum[thread] = localthreadworkloadnum;
	}
	delete database_buf;
	
	for (int i = 0; i < workingthread; i ++) {
		int temp = 0;
		for (j = 0; j  < itemno; j ++)
			temp += global_nodenum[i][j];
	}
	int totalnodes = cal_level_25(0);
	
#pragma omp parallel for
	for (j = 0; j < workingthread; j ++) {
		int local_rightsib_backpatch_count = rightsib_backpatch_count[j][0];
		Fnode ***local_rightsib_backpatch_stack = rightsib_backpatch_stack[j];
		for (int i = 0; i < local_rightsib_backpatch_count; i ++)
			*local_rightsib_backpatch_stack[i] = NULL;
	}
	wtime(&tend);
//	printf("Creating the first tree from source file cost %f seconds\n", tend - tstart);
//       printf("we have %d nodes in the initial FP tree\n", totalnodes);
}

void FP_tree::scan1_DB(int thread, FP_tree* old_tree, int item)
{
	int i;
	int *local_global_count_array = global_count_array[thread];
	int *local_global_table_array = global_table_array[thread];
	int *local_global_temp_order_array = global_temp_order_array[thread];
	int *local_global_order_array = global_order_array[thread];
	int *old_table = old_tree->table;
	for(i=0; i< itemno; i++)
	{
		count[i]=local_global_count_array[i];
		table[i]=local_global_table_array[i];
	}

	sort(count, table, 0, itemno-1);

	for(i=0; i<itemno; i++)
	{
		local_global_temp_order_array[table[i]]=i; 
	}
	for (i = 0; i < item; i ++)
		local_global_order_array[i] = local_global_temp_order_array[old_table[i]];
	
	nodenum = (int*)fp_buf[thread]->newbuf(1, itemno * sizeof(int));
	for (i = 0; i < itemno; i ++)
		nodenum[i] = 0;

	if (itemno > hot_item_num1 * 2)
		num_hot_item = hot_item_num1;
	else
		num_hot_item = itemno / 2;
	if ((1<<num_hot_item) > (sum_item_num[thread][0] / 8))
		num_hot_item = 0;
	hashtable[thread][0] = Root;
	rightsib_backpatch_count[thread][0] = 0;
}

void FP_tree::powerset(int*prefix, int prefixlen, int* items, int current, int itlen, FSout* fout, int thread )const
{
	if(current==itlen)
	{
		if(prefixlen!=0)
		{	
				fout->printset(list[thread]->top, list[thread]->FS);
				fout->printSet(prefixlen, prefix, count[global_temp_order_array[thread][prefix[prefixlen-1]]]);
		}
	}else{
		current++;
		powerset(prefix, prefixlen, items, current, itlen, fout, thread);
		current--;
		prefix[prefixlen++]=items[current++];
		powerset(prefix, prefixlen, items, current, itlen, fout, thread);
	}
}	

void FP_tree::generate_all(int new_item_no, int thread, FSout* fout)const
{ 
	powerset(prefix[thread], 0, list[thread]->FS, list[thread]->top, list[thread]->top+new_item_no, fout, thread); 
}

bool FP_tree::Single_path(int thread)const
{
	Fnode* node;
	for(node=Root->leftchild; node!=NULL; node=node->leftchild)
		if(node->rightsibling!=NULL)return false;

	return true;
}

void FP_tree::release_node_array_after_mining(int sequence, int thread, int workingthread)
{
	int current, i;
	thread_finish_status[thread] = sequence;
	current = 0;
	for (i = 0; i < workingthread; i ++) {
		if (current < thread_finish_status[i])
			current = thread_finish_status[i];
	}
{
#pragma omp critical
	{
		if (current < released_pos) {
			released_pos = current;
			fp_node_sub_buf->freebuf(MR_nodes[current], MC_nodes[current], MB_nodes[current]);
		}
	}
}

}

void FP_tree::release_node_array_before_mining(int sequence, int thread, int workingthread)
{
	int current, i;
	thread_begin_status[thread] = sequence;
	current = 0;
	for (i = 0; i < workingthread; i ++) {
		if (current < thread_begin_status[i])
			current = thread_begin_status[i];
	}
	current ++;
{
#pragma omp critical
	{
		if (current < released_pos) {
			released_pos = current;
			fp_node_sub_buf->freebuf(MR_nodes[current], MC_nodes[current], MB_nodes[current]);
		}
	}
}

}

int FP_tree::FP_growth_first(FSout* fout)
{
	int sequence;
	double tstart, tend, temp_time;
	int function_type;
	int workingthread = omp_get_max_threads();

	wtime(&tstart);
	fp_node_sub_buf = new memory(80, 131072, 2097152 * 4, 2);
	if (itemno <= 0x100) {
		function_type = 0;
		first_transform_FPTree_into_FPArray(this, (unsigned char) 0xff);
	}
	else if (itemno  < 0x10000) {
		function_type = 1;
		first_transform_FPTree_into_FPArray(this, (unsigned short) 0xffff);
	}
	else {
		function_type = 2;
		first_transform_FPTree_into_FPArray(this, (unsigned int) 0xffffffff);
	}
	for (int k = 0; k < hot_node_num; k ++)
		hashtable[0][k] = NULL;
	int first_data_num = new_data_num[0][0];
	wtime(&temp_time);
	for (int i = 0; i < workingthread; i ++)
		fp_tree_buf[i]->freebuf(first_MR_tree[i], first_MC_tree[i], first_MB_tree[i]);
	fp_tree_buf[0]->freebuf(MR_tree, MC_tree, MB_tree);
	int lowerbound = 0x7fffffff;
	int upperbound;
	if (lowerbound > itemno)
		lowerbound = itemno;
	for (int t = 0; t < 3; t ++) {
		upperbound = lowerbound;
		if (upperbound > itemno)
			upperbound = itemno;
		lowerbound = lowerbound_array[t];

		if (upperbound > lowerbound) {
			int new_function_type = 2;
			if (upperbound - 1 < 0x10000)
				new_function_type = 1;
			if (upperbound - 1 < 0x100)
				new_function_type = 0;
			if (new_function_type != function_type) {
				if (t == 1)
					transform_FPArray(ItemArray, (unsigned short) 0xffff, first_data_num);
				if (t == 2) 
					transform_FPArray((unsigned short *) ItemArray, (unsigned char) 0xff, first_data_num);
				function_type = new_function_type;
			}
		}

		#pragma omp parallel for schedule(dynamic,1)
		for(sequence=upperbound - 1; sequence>=lowerbound; sequence--)
		{	int current, new_item_no, listlen;
			int MC2=0;			
			unsigned int MR2=0;	
			char* MB2;		
			int thread = omp_get_thread_num();
			//release_node_array_before_mining(sequence, thread, workingthread); remove due to data race
			memory *local_fp_tree_buf = fp_tree_buf[thread];
			memory *local_fp_buf = fp_buf[thread];
			stack *local_list = list[thread];
			int *local_ITlen = ITlen[thread];
			int *local_global_table_array = global_table_array[thread];
			int *local_global_count_array = global_count_array[thread];
			current=table[sequence];
			local_list->FS[local_list->top++]=current;
			listlen = local_list->top;
			local_ITlen[local_list->top-1]++;
			if(fout)
				fout->printSet(local_list->top, local_list->FS, count[sequence]);
			if(sequence !=0) {
				if (function_type == 0)
					new_item_no=FPArray_conditional_pattern_base(this, sequence, thread, (unsigned char) 0xff);
				else if (function_type == 1)
					new_item_no=FPArray_conditional_pattern_base(this, sequence, thread, (unsigned short) 0xffff);
				else new_item_no=FPArray_conditional_pattern_base(this, sequence, thread, (unsigned int) 0xffffffff);
			}
			else
				new_item_no = 0;
			
			if(new_item_no==0 || new_item_no == 1)
			{
				if(new_item_no==1)
				{
					local_list->FS[local_list->top++] = local_global_table_array[0];
					local_ITlen[local_list->top-1]++;
					if(fout)
						fout->printSet(local_list->top, local_list->FS, local_global_count_array[0]);
				}
				local_list->top=listlen-1;
				release_node_array_after_mining(sequence, thread, workingthread);
				continue;
			}

			FP_tree *fptree;
			fptree = (FP_tree*)local_fp_buf->newbuf(1, sizeof(FP_tree));
			fptree->init(this->itemno, new_item_no, thread);

			MB2=local_fp_tree_buf->bufmark(&MR2, &MC2);
			fptree->MB_tree = MB2;
			fptree->MR_tree = MR2;
			fptree->MC_tree = MC2;

			fptree->scan1_DB(thread, this, sequence);
			if (function_type == 0)
				FPArray_scan2_DB(fptree, this, sequence, thread, (unsigned char)0xff);
			else if (function_type == 1)
				FPArray_scan2_DB(fptree, this, sequence, thread, (unsigned short)0xffff);
			else FPArray_scan2_DB(fptree, this, sequence, thread, (unsigned int)0xffffffff);
			local_list->top=listlen;							
			if(fptree->Single_path(thread))
			{
	                        Fnode* node;
	                        for(node=fptree->Root->leftchild; node!=NULL; node=node->leftchild)
	                                local_list->FS[local_list->top++] = fptree->table[node->itemname];
	                        local_list->top = listlen;
				int i1, i2;
				int temp = 1;
				for (i1 = 1, i2 = new_item_no; i1 <= new_item_no; i1 ++, i2 --) {
					temp = (temp * i2) / i1;
					local_ITlen[local_list->top+i1-1] += temp;
				}
				if (fout)
					fptree->generate_all(new_item_no, thread, fout);
				local_list->top--;
				local_fp_tree_buf->freebuf(fptree->MR_tree, fptree->MC_tree, fptree->MB_tree);
			}else{             
				fptree->FP_growth(thread, fout);
				local_list->top = listlen-1;
			}
			release_node_array_after_mining(sequence, thread, workingthread);
		}
	}
	 wtime(&tend);
//	 printf("the major FP_growth cost %f vs %f seconds\n", tend - tstart, temp_time - tstart);
	return 0;
}

int FP_tree::FP_growth(int thread, FSout* fout)
{
	int sequence, current, new_item_no, listlen;
	int MC2=0;			
	unsigned int MR2=0;	
	char* MB2;			
	int function_type;
	memory *local_fp_tree_buf = fp_tree_buf[thread];
	memory *local_fp_buf = fp_buf[thread];
	stack *local_list = list[thread];
	int *local_ITlen = ITlen[thread];
	int *local_global_table_array = global_table_array[thread];
	int *local_global_count_array = global_count_array[thread];
	if (itemno <= 0x100) {
		function_type = 0;
		transform_FPTree_into_FPArray(this, thread, (unsigned char) 0xff);
	}
	else if (itemno  <= 0x10000) {
		function_type = 1;
		transform_FPTree_into_FPArray(this, thread, (unsigned short) 0xffff);
	}
	else {
		function_type = 2;
		transform_FPTree_into_FPArray(this, thread, (unsigned int) 0xffffffff);
	}
	local_fp_tree_buf->freebuf(MR_tree, MC_tree, MB_tree);
	for(sequence=itemno - 1; sequence>=0; sequence--)
	{
		current=table[sequence];
		local_list->FS[local_list->top++]=current;
		listlen = local_list->top;

		local_ITlen[local_list->top-1]++;
		if(fout)
			fout->printSet(local_list->top, local_list->FS, count[sequence]);
		if(sequence !=0) {
			if (function_type == 0)
				new_item_no=FPArray_conditional_pattern_base(this, sequence, thread, (unsigned char) 0xff);
			else if (function_type == 1)
				new_item_no=FPArray_conditional_pattern_base(this, sequence, thread, (unsigned short) 0xffff);
			else new_item_no=FPArray_conditional_pattern_base(this, sequence, thread, (unsigned int) 0xffffffff);
		}
		else
			new_item_no = 0;
		
		if(new_item_no==0 || new_item_no == 1)
		{
			if(new_item_no==1)
			{
				local_list->FS[local_list->top++] = local_global_table_array[0];
				local_ITlen[local_list->top-1]++;
				if(fout)
					fout->printSet(local_list->top, local_list->FS, local_global_count_array[0]);
			}
			local_list->top=listlen-1;
			continue;
		}

		FP_tree *fptree;
		fptree = (FP_tree*)local_fp_buf->newbuf(1, sizeof(FP_tree));
		fptree->init(this->itemno, new_item_no, thread);

		MB2=local_fp_tree_buf->bufmark(&MR2, &MC2);
		fptree->MB_tree = MB2;
		fptree->MR_tree = MR2;
		fptree->MC_tree = MC2;

		fptree->scan1_DB(thread, this, sequence);
		if (function_type == 0)
			FPArray_scan2_DB(fptree, this, sequence, thread, (unsigned char)0xff);
		else if (function_type == 1)
			FPArray_scan2_DB(fptree, this, sequence, thread, (unsigned short)0xffff);
		else FPArray_scan2_DB(fptree, this, sequence, thread, (unsigned int)0xffffffff);
		local_list->top=listlen;							
		if(fptree->Single_path(thread))
		{
                        Fnode* node;
                        for(node=fptree->Root->leftchild; node!=NULL; node=node->leftchild)
                                local_list->FS[local_list->top++] = fptree->table[node->itemname];
                        local_list->top = listlen;
			int i1, i2;
			int temp = 1;
			for (i1 = 1, i2 = new_item_no; i1 <= new_item_no; i1 ++, i2 --) {
				temp = (temp * i2) / i1;
				local_ITlen[local_list->top+i1-1] += temp;
			}
			if (fout)
				fptree->generate_all(new_item_no, thread, fout);
			local_list->top--;
			local_fp_tree_buf->freebuf(fptree->MR_tree, fptree->MC_tree, fptree->MB_tree);
		}else{             
			fptree->FP_growth(thread, fout);
			local_list->top = listlen-1;
		}
		local_fp_buf->freebuf(MR_nodes[sequence], MC_nodes[sequence], MB_nodes[sequence]);
	}
	return 0;
}

