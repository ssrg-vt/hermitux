/*----------------------------------
File     : data.cpp
Contents : data set management
----------------------------------*/

#include "data.h"
#include "common.h"

Data::Data(char *filename)
{
#ifndef BINARY
  in = fopen(filename,"r+t");
#else
  in = fopen(filename, "r+b");
#endif
}

Data::~Data()
{
  if(in) fclose(in);
}

int Data::isOpen()
{
  if(in) return 1;
  else return 0;
}

int *Data::parseDataFile(MapFile *mapfile)
{
	int *counts;
	MapFileNode *current_mapfilenode, *new_mapfilenode;
	current_mapfilenode = (MapFileNode *)database_buf->newbuf(1, sizeof(MapFileNode));
	current_mapfilenode->init(100000, 1);
	current_mapfilenode->next = mapfile->first;
	mapfile->first = current_mapfilenode;
	mapfile->tablesize ++;
	int *TransactionContent = current_mapfilenode->TransactionContent;
	int *newTransactionContent;
	int length = 0;
	int size = current_mapfilenode->size;
	int net_itemno=0;
	char c;
	int item, pos;
	int j;
	TRANSACTION_NO = 0;
	
	counts= new int[ITEM_NO];
	
	for(int i=0; i<ITEM_NO; i++)
		counts[i] = 0;

	while (1) {
		item=0;
		pos=0;
		c = getc(in);
		while((c >= '0') && (c <= '9')) {
			item *=10;
			item += c-'0';
			c = getc(in);
			pos=1;
		}
		if (pos) {
			if (length == size) {
				new_mapfilenode = (MapFileNode *)database_buf->newbuf(1, sizeof(MapFileNode));
				new_mapfilenode->init(100000, 1);
				new_mapfilenode->next = current_mapfilenode;		
				mapfile->first = new_mapfilenode;
				mapfile->tablesize ++;
				newTransactionContent = new_mapfilenode->TransactionContent;
				newTransactionContent[0] = -1;
				j = 1;
				length --;
				while (TransactionContent[length] != -1) {
					newTransactionContent[j] = TransactionContent[length];
					j ++;
					length --;
				}
				current_mapfilenode->top = length + 1;
				length = j;
				current_mapfilenode = new_mapfilenode;
				TransactionContent = newTransactionContent;
			}
			TransactionContent[length] = item;
			length ++;
			if(item>=ITEM_NO)
			{
				ITEM_NO = 2*item;
				int* temp = new int[ITEM_NO];
				for(j=0; j<=net_itemno; j++)
					temp[j] = counts[j];
				for(; j<ITEM_NO; j++)
					temp[j] = 0;
				delete []counts;
				counts=temp;
				net_itemno=item;
			}else
				if(net_itemno<item)
					net_itemno = item;
			counts[item]++;
		}
		if (c == '\n') {
			if (length == size) {
				current_mapfilenode->top = length;
				current_mapfilenode = (MapFileNode *)database_buf->newbuf(1, sizeof(MapFileNode));
				current_mapfilenode->init(100000, 1);
				current_mapfilenode->next = mapfile->first;		
				mapfile->first = current_mapfilenode;
				mapfile->tablesize ++;
				TransactionContent = current_mapfilenode->TransactionContent;
				length = 0;
			}
			TransactionContent[length] = -1;
			length ++;
			TRANSACTION_NO ++;
		}
		else if(feof(in)){
			rewind(in);
			printf("transaction number is %d\n", TRANSACTION_NO);
			current_mapfilenode->top = length;
			ITEM_NO = net_itemno+1;
			return counts;
		}			 
	};
}

void MapFileNode::init(int SIZE, int mul)
{
	size = SIZE;
	TransactionContent = (int *) new int [size];
	size *= mul;
	top = 0;
}

void MapFileNode::finalize()
{
	delete [] TransactionContent;
}

void MapFile::init()
{
	first = NULL;
	tablesize = 0;
}

void MapFile::transform_list_table()
{
	MapFileNode * current_node;
	table = (MapFileNode **)database_buf->newbuf(1, tablesize * sizeof(int*));
	int i = 0;
	current_node = first;
	while (current_node) {
		table[i] = current_node;
		i ++;
		current_node = current_node->next;
	}
}

void MapFile::alloc_work_between_threads(int workingthread)
{
	int *content;
	int sumwork;
	int averwork;
	int i, j, pos1, pos2, pos3;
	int *current_top = new int [tablesize];
	nodebegin = (int *)database_buf->newbuf(1, (workingthread << 4) + tablesize);
	nodeend = nodebegin + workingthread;
	posbegin = nodeend + workingthread;
	posend = posbegin + workingthread;
	shareinfomap = (char *) (posend + workingthread);
	sumwork = 0;
	for (i = 0; i < tablesize; i ++) {
		sumwork += table[i]->top;
		current_top[i] = table[i]->top;
		shareinfomap[i] = 0;
	}
	averwork = sumwork / workingthread;
	sumwork = 0;
	j = 0;
	posbegin[0] = table[0]->top - 1;
	nodebegin[0] = 0;
	for (i = 0; i < tablesize; i ++) {
		if (sumwork + current_top[i] > averwork) {
			pos1 = current_top[i] - (averwork - sumwork);
			content = table[i]->TransactionContent;
			pos2 = pos1;
			while (pos2 <= current_top[i] && content[pos2] != -1)
				pos2 ++;
			pos3 = pos1;
			while (pos3 > -1 && content[pos3] != -1)
				pos3 --;
			if (pos2 - pos1 > pos1 - pos3)
				pos1 = pos3;
			else pos1 = pos2;
			pos2 = pos1 - 1;
			if (content[pos1] == -1)
				pos1 ++;
			if (pos1 >= table[i]->top) {
				nodeend[j] = i - 1;
				posend[j] = 0;
			}
			else {
				nodeend[j] = i;
				posend[j] = pos1;
			}
			j ++;
			nodebegin[j] = i;
			posbegin[j] = pos2;
			shareinfomap[i] = 1;
			if (pos2 > averwork) {
				current_top[i] = pos2;
				sumwork = 0;
				i --;
			} else sumwork = pos2 + 1;
		}
		else sumwork += table[i]->top;
		if (j == workingthread - 1) {
			posend[j] = 0;
			nodeend[j] = tablesize - 1;
			break;
		}
	}
	delete [] current_top;
}


