#ifndef __SSTABLE_DATA_H_
#define __SSTABLE_DATA_H_

#include "buffer.h"
#include "common.h"
#include "tinybloom.h"
#include <stdio.h>

#define FILE_MAX (10*1024*1024)

typedef struct sst_data
{
    int id;
    int key_num;
    data_t** keys;
    
    int max;
    
    char filename[128];
    FILE *file;
    buffer_t* buf;
	bloom_filter* bfilter;
}sst_data_t;

void* sstdata_new(int id);
void sstdata_free(sst_data_t* sstdata);
void sstdata_open(sst_data_t* sstdata);
void sstdata_flush(sst_data_t* sstdata);
void sstdata_build(sst_data_t* sstdata);
int sstdata_put(sst_data_t* sstdata,data_t* data);
data_t* sstdata_get(sst_data_t* sstdata,const char* key);

#endif