#ifndef __SSTABLE_DATA_H_
#define __SSTABLE_DATA_H_

#include "buffer.h"
#include "common.h"
#include "tinybloom.h"
#include <stdio.h>

#define FILE_MAX (10*1024*1024)

//sst_data is a structure use to store data
typedef struct sst_data
{
    int id;					//
    int key_num;			//data number
    data_t** keys;			//data point

	data_t* bigest_key;		//the biggest data
	data_t* smallest_key;	//the smallest data
    
    int max;				//limit data max number
    
    char filename[128];		//file name
    FILE *file;				//file handle
    buffer_t* buf;			//buffer use to serialize
}sst_data_t;

void* sstdata_new(int id);
void sstdata_free(sst_data_t* sstdata);
void sstdata_open(sst_data_t* sstdata);
void sstdata_flush(sst_data_t* sstdata);
void sstdata_build(sst_data_t* sstdata);
int sstdata_put(sst_data_t* sstdata,data_t* data);
data_t* sstdata_get(sst_data_t* sstdata,const char* key);

void sstdata_relasedata(sst_data_t* sstdata);

int sstdata_compactput(sst_data_t* sstdata,data_t* data);

void sstdata_writehead(sst_data_t* sstdata);
void sstdata_writedata(sst_data_t* sstdata);

#endif