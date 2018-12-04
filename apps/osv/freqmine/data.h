/*----------------------------------------------------------------------
  File     : data.h
  Contents : data set management
----------------------------------------------------------------------*/
#ifndef _DATA_CLASS
#define _DATA_CLASS

#include <stdio.h>
#include <stdlib.h>

#define TransLen 50
class MapFile;

class Data
{
 public:
	
	Data(char *filename);
	~Data();
	int isOpen();
	void close(){if(in)fclose(in);}

	long totallength;
	
	int *parseDataFile(MapFile *mapfile);

	long currentlength(){
		return ftell(in);
	}

	void caltotallength(){
		fseek(in,0,SEEK_END);
		totallength=ftell(in);
		fseek(in,0,SEEK_SET);
	}
  
 private:
  
	FILE *in;
};

/**
 chunrong lai, 07-19-2005
**/
class MapFileNode;

class MapFileNode
{
public:
    	int *TransactionContent;
	int size;
	int top;
	unsigned int MR;
	int MC;
	char* MB;
	MapFileNode* next;
	void init(int SIZE, int mul);
	void finalize();
};

class MapFile
{
public:
	MapFileNode* first;
	MapFileNode** table;
	int* nodebegin;
	int* nodeend;
	int *posbegin;
	int *posend;
	char *shareinfomap;
	int tablesize;
	void init();
	void transform_list_table();
	void alloc_work_between_threads(int workingthread);
};

#endif

