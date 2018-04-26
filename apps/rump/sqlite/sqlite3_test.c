#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#include "sqlite3.h"

#define RECORD_NUM		200000
#define TABLE_NAME		"test_table"
#define STR_FIELD_SIZE	128

static inline int execute_query(const char *query, sqlite3 *db) {
	int ret;
	char *err_msg = 0;

	ret = sqlite3_exec(db, query, 0, 0, &err_msg);
	
	if(ret != SQLITE_OK) {
		fprintf(stderr, "Cannot execute sql query: %s\n", err_msg);
		return -1;
	}

	return 0;
}

int create_table(sqlite3 *db) {
	char q[128];

	sprintf(q, "CREATE TABLE %s(Id INT, Str TEXT, Float FLOAT);", TABLE_NAME);
	return execute_query(q, db);
}

static inline int insert_records(uint32_t record_num, sqlite3 *db) {
	uint32_t i;
	char str[STR_FIELD_SIZE];
	float f = (float)rand()/(float)(RAND_MAX/5.0);
	char q[256];

	str[STR_FIELD_SIZE-1] = '\0';

	for(i=0; i<record_num; i++) {
		sprintf(q, "INSERT INTO %s VALUES(%u, '%s', %lf);", TABLE_NAME, i, str, f);
		if(execute_query(q, db))
			return -1;
	}

	return 0;
}

static inline int retrieve_records(sqlite3 *db) {
	char q[128];

	sprintf(q, "SELECT * FROM %s;", TABLE_NAME);

	return execute_query(q, db);
}

int main(int argc, char *argv[])
{
	sqlite3 *db;
	int ret;
	struct timeval start, stop, res;

	ret = sqlite3_open(":memory:", &db);
	if(ret != SQLITE_OK) {
		fprintf(stderr, "Error opening sqlite3 handler\n");
		return -1;
	}

	if(create_table(db))
		return -1;

	gettimeofday(&start, NULL);
	if(insert_records(RECORD_NUM, db))
		return -1;
	gettimeofday(&stop, NULL);

	timersub(&stop, &start, &res);
	printf("Insertion took: %ld.%06lds\n", res.tv_sec, res.tv_usec);

	gettimeofday(&start, NULL);
	if(retrieve_records(db))
		return -1;
	gettimeofday(&stop, NULL);

	timersub(&stop, &start, &res);
	printf("Retrieval took: %ld.%06lds\n", res.tv_sec, res.tv_usec);

	sqlite3_close(db);

	return 0;
}
