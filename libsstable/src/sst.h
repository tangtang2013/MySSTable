#ifndef __SSTABLE_SST_H_
#define __SSTABLE_SST_H_

#include "buffer.h"
#include "sstdata.h"
#include "hashtable.h"
#include <stdio.h>

//this enum is sstable status
typedef enum esst_status
{
	SNULL,		//sstable is NULL
	UNOPEN,		//sstable is unopen
	WRITE,		//sstable is in wirting
	READ,		//sstable is in reading
	WFULL,		//sstable is writing full
	FLUSH,		//sstable is in flushing
	COMPACT,	//sstable is in compacting
	COMPACTED,	//sstable is in compacted
}sst_status;

//this a is structure for sstable
typedef struct sstable
{
    int id;					//id
    sst_data_t* sstdata;	//store data
	hashtable_t* htable;	//store data(insert quickly)
	sst_status status;		//sstable status

	HANDLE lock;			//sstable lock

	struct sstable* prev;	//prev point in sstable list
	struct sstable* next;	//next point in sstable list
}sstable_t;

void* sst_new(int id);
void sst_open(sstable_t* sst,sst_status status);
void sst_close(sstable_t* sst);

void sst_flush(sstable_t* sst);

int sst_put(sstable_t* sst,data_t* data);
data_t* sst_get(sstable_t* sst,const char* key,int keySize);

void sst_precompact(sstable_t* sst);
int sst_compactput(sstable_t* sst,data_t* data);

#endif