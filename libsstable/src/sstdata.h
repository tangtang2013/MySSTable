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

/************************************
* @name:	sstdata_new
* @func:	new a sstable data
* @param:	int
* @ret:		VOID
************************************/
void* sstdata_new(int id);

/************************************
* @name:	sstdata_free
* @func:	free a sstable data
* @param:	sst_data_t*
* @ret:		VOID
************************************/
void sstdata_free(sst_data_t* sstdata);

/************************************
* @name:	sstdata_open
* @func:	open a sstable data
* @param:	sst_data_t*
* @ret:		VOID
************************************/
void sstdata_open(sst_data_t* sstdata);

/************************************
* @name:	sstdata_flush
* @func:	flush a sstable data into file
* @param:	sst_data_t*
* @ret:		VOID
************************************/
void sstdata_flush(sst_data_t* sstdata);

/************************************
* @name:	sstdata_build
* @func:	build a sstable data
* @param:	sst_data_t*
* @ret:		VOID
************************************/
void sstdata_build(sst_data_t* sstdata);

/************************************
* @name:	sstdata_put
* @func:	build a sstable data
* @param:	sst_data_t*, data_t*
* @ret:		int
************************************/
int sstdata_put(sst_data_t* sstdata,data_t* data);

/************************************
* @name:	sstdata_put
* @func:	build a sstable data
* @param:	sst_data_t*, const char*
* @ret:		data_t*
************************************/
data_t* sstdata_get(sst_data_t* sstdata,const char* key);

/************************************
* @name:	sstdata_relasedata
* @func:	relase data in sstdata
* @param:	sst_data_t*
* @ret:		VOID
************************************/
void sstdata_relasedata(sst_data_t* sstdata);

/************************************
* @name:	sstdata_compactput
* @func:	compact sstable data use it put data
* @param:	sst_data_t*, data_t*
* @ret:		int
************************************/
int sstdata_compactput(sst_data_t* sstdata,data_t* data);

/************************************
* @name:	sstdata_writehead
* @func:	write head of sstdata in file
* @param:	sst_data_t*
* @ret:		VOID
************************************/
void sstdata_writehead(sst_data_t* sstdata);

/************************************
* @name:	sstdata_writedata
* @func:	write data of sstdata in file
* @param:	sst_data_t*
* @ret:		VOID
************************************/
void sstdata_writedata(sst_data_t* sstdata);

#endif