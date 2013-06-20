#ifndef __SSTABLE_SSTMANAGER_H_
#define __SSTABLE_SSTMANAGER_H_

#include "sst.h"
#include "threadpool.h"
#include "common.h"

#define SSTABLE_NUM 128
#define SSTABLE_MAX 128



#define BUFFER_MAX_SIZE 1024

//this structure is a manager
typedef struct sstmanager 
{
	int last_id;			//last sstable id
	int current_id;			//current work sstable(not only write)
	int sst_num;			//the number of sstable
	int max;				//the max number of sstable

	int compact_num;		//the number of compact sstables
	int uncompact_num;		//the number of uncompact sstables;

	int start_id;			//start id of sstables
	int end_id;				//end id of sstables;

	sstable_t* head;		//the head of sstable list
	sstable_t* curtable;	//the current work sstable
	sstable_t* tail;		//the tail of sstable list
	sstable_t* compact;		//the compact sstable
	sstable_t* uncompact;		//the not compact sstable
	sstable_t* writetable;	//the writing sstable(only one in list)

	threadPool_t* pool;		//thread pool
	HANDLE lock;			//lock
	HANDLE compact_thread;	//the handle of compact thread
	HANDLE compact_semaphore;//the semaphore of compact
	BOOL compact_run;		//the flag of compact

	FILE* file;				//the file handle
	char filename[FILE_MAX_PATH];//file name
	buffer_t* buf;			//buffer use to serialize
}sstmanager_t;
/*
 * Manifest structure
 * file[last_id,sst_num,max] [startid,endid]
 */

/************************************
 * @name:	sstmanager_new
 * @func:	new a sstable manager
 * @param:	VOID
 * @ret:	VOID
 ************************************/
void* sstmanager_new();

/************************************
* @name:	sstmanager_open
* @func:	open a sstable manager
* @param:	sstmanager_t*
* @ret:		VOID
*************************************/
void sstmanager_open(sstmanager_t* manager);

/************************************
* @name:	sstmanager_flush
* @func:	flush a sstable manager
* @param:	sstmanager_t*
* @ret:		VOID
*************************************/
void sstmanager_flush(sstmanager_t* manager);

/************************************
* @name:	sstmanager_close
* @func:	close a sstable manager
* @param:	sstmanager_t*
* @ret:		VOID
*************************************/
void sstmanager_close(sstmanager_t* manager);

void sstmanager_createsst(sstmanager_t* manager,sst_status status);
void sstmanager_addsst(sstmanager_t* manager,sstable_t* sst);
void sstmanager_rmsst(sstmanager_t* manager,int start,int end);

/************************************
* @name:	sstmanager_put
* @func:	put data in a sstable manager
* @param:	sstmanager_t*,data_t*
* @ret:		int
*************************************/
int sstmanager_put(sstmanager_t* manager,data_t* data);

/************************************
* @name:	sstmanager_get
* @func:	get data in a sstable manager
* @param:	sstmanager_t*,data_t*
* @ret:		int
*************************************/
data_t* sstmanager_get(sstmanager_t* manager,const char* key,int keySize);

/************************************
* @name:	sstmanager_compact
* @func:	compact file in a sstable manager
* @param:	sstmanager_t*,int,int
* @ret:		VOID
*************************************/
void sstmanager_compact(sstmanager_t* manager,int begin,int end);

void sstmanager_checkcompact(void* arg);

#endif