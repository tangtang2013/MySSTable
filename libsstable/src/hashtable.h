#ifndef __SSTABLE_HASHTABLE_H_
#define __SSTABLE_HASHTABLE_H_

#include "buffer.h"
#include "common.h"
#include <stdio.h>

typedef struct hashtable
{
	int id;
	int bucket_szie;
	data_t** buckets;		//use to get and insert
	data_t** datas;			//use to compact
	int key_num;

	HANDLE* bucket_locks;	//buckets lock
	HANDLE* num_lock;		//number lock

	data_t* bigest_key;		//the biggest data
	data_t* smallest_key;	//the smallest data
	
	data_t* nextdata;		//use to iterator point in compact
	int index;				//
	int next_num;			//

	int max;				//limit data max number

	char filename[FILE_MAX_PATH];		//file name
	FILE *file;				//file handle
	buffer_t* buf;			//buffer use to serialize
}hashtable_t;

void* hashtable_new(int id);
void hashtable_open(hashtable_t* htable);
void hashtable_free(hashtable_t* htable);
void hashtable_build(hashtable_t* htable);
void hashtable_flush(hashtable_t* htable);
int hashtable_put(hashtable_t* htable,data_t* data);
data_t* hashtable_get(hashtable_t* htable,const char* key,int keySize);
int hashtable_del(hashtable_t* htable,const char* key,int keySize);

int hashtable_compactput(hashtable_t* htable,data_t* data);
void hashtable_relasedata(hashtable_t* htable);
void hashtable_writehead(hashtable_t* htable);
void hashtable_writedata(hashtable_t* htable);

data_t* hashtable_nextdata(hashtable_t* htable);
void hashtable_sort(hashtable_t* htable);

#endif