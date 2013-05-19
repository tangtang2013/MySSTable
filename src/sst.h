#ifndef __SSTABLE_SST_H_
#define __SSTABLE_SST_H_

#include "buffer.h"
#include "data.h"
#include <stdio.h>

typedef enum esst_status
{
	SNULL,
	UNOPEN,
	WRITE,
	READ,
	WFULL,
	COMPACT
}sst_status;

typedef struct sstable
{
    int id;
    sst_data_t* sstdata;
	sst_status status;
    
    char filename[128];
    FILE *file;
    buffer_t* buf;

	struct sstable* prev;
	struct sstable* next;
}sstable_t;

void* sst_new(int id);
void sst_open(sstable_t* sst);
void sst_close(sstable_t* sst);
void sst_build(sstable_t* sst);
void sst_flush(sstable_t* sst);

int sst_put(sstable_t* sst,data_t* data);
data_t* sst_get(sstable_t* sst,const char* key);

#endif