#ifndef __SSTABLE_SSTMANAGER_H_
#define __SSTABLE_SSTMANAGER_H_

#include "sst.h"

#define SSTABLE_NUM 128
#define SSTABLE_MAX 128

typedef struct sstmanager 
{
	int last_id;
	int current_id;
	int sst_num;
	int max;
	sstable_t* head;
	sstable_t* curtable;
	sstable_t* tail;

	FILE* file;
	char filename[128];
	buffer_t* buf;
}sstmanager_t;
/*
 * Manifest structure
 * file[last_id,sst_num,max]
 */
void* sstmanager_new();
void sstmanager_open(sstmanager_t* manager);
void sstmanager_flush(sstmanager_t* manager);
void sstmanager_close(sstmanager_t* manager);

void sstmanager_createsst(sstmanager_t* manager);
void sstmanager_addsst(sstmanager_t* manager,sstable_t* sst);
void sstmanager_rmsst(sstmanager_t* manager,int id);

int sstmanager_put(sstmanager_t* manager,data_t* data);
data_t* sstmanager_get(sstmanager_t* manager,const char* key);

#endif