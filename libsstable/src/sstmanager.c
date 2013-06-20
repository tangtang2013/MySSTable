#include "sstmanager.h"
#include "debug.h"
#include <stdio.h>

void* sstmanager_new()
{
	sstmanager_t* manager = (sstmanager_t*)xmalloc(sizeof(sstmanager_t));
	memset(manager,0,sizeof(sstmanager_t));

	memset(manager->filename, 0, FILE_MAX_PATH);
	sprintf(manager->filename, "Manifest");

	manager->max = SSTABLE_MAX;
	manager->head = NULL;

	manager->buf = buffer_new(BUFFER_MAX_SIZE);

	manager->pool = threadPool_new(2);
	manager->lock = CreateMutex(0,0,0);
	manager->compact_semaphore = CreateSemaphore(NULL,0,1,NULL);

	return manager;
}

void sstmanager_open(sstmanager_t* manager)
{
	int ret,i,id;
	sstable_t* sst = NULL;
	
	//open file of "Manifest",if it exist read file content for get status,if not, init with first status
	manager->file = fopen(manager->filename,"rb");
	if (!manager->file)
	{
		manager->last_id = -1;
		manager->current_id = 0;
		manager->sst_num = 0;
		__INFO("sstmanager:open Manifaest no exist");
	}
	else
	{
		manager->file = fopen(manager->filename,"rb");
		ret = fread(buffer_detach(manager->buf),20,1,manager->file);
		manager->buf->NUL = 20;
		manager->last_id = buffer_getint(manager->buf);
		manager->sst_num = buffer_getint(manager->buf);
		manager->max = buffer_getint(manager->buf);

		manager->uncompact_num = buffer_getint(manager->buf);
		manager->compact_num = buffer_getint(manager->buf);

		//sstable id is the last run program's last id
		manager->current_id = manager->last_id;

		ret = fread(buffer_detach(manager->buf),(manager->uncompact_num + manager->compact_num) * sizeof(int),1,manager->file);
		buffer_seekfirst(manager->buf);

		//read uncompact sstable info
		i = manager->uncompact_num;
		while (i--)
		{
			id = buffer_getint(manager->buf);
			sst = sst_new(id);
			if (manager->uncompact == NULL)
			{
				manager->uncompact = sst;
			}
			else
			{
				manager->uncompact->next = sst;
			}
		}

		//read compact sstable info
		i = manager->compact_num;
		while (i--)
		{
			id = buffer_getint(manager->buf);
			sst = sst_new(id);
			if (manager->compact == NULL)
			{
				manager->compact = sst;
			}
			else
			{
				manager->compact->next = sst;
			}
		}
	}
	threadPool_init(manager->pool);
	manager->compact_run = TRUE;
//	manager->compact_thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)sstmanager_checkcompact,manager,0,NULL);
}

void sstmanager_flush( sstmanager_t* manager )
{
	int i,ret;
	sstable_t* sst;

	//write sstamanager info to file, current id is last id
	buffer_clear(manager->buf);
	buffer_seekfirst(manager->buf);
	buffer_putint(manager->buf,manager->current_id);
	buffer_putint(manager->buf,manager->sst_num);
	buffer_putint(manager->buf,manager->max);

	buffer_putint(manager->buf,manager->uncompact_num);
	buffer_putint(manager->buf,manager->compact_num);

	//write uncompact sstable info
	sst = manager->uncompact;
	while (sst)
	{
		buffer_putint(manager->buf,sst->id);
		sst = sst->next;
	}

	//write compact sstable info
	sst = manager->compact;
	while (sst)
	{
		buffer_putint(manager->buf,sst->id);
		sst = sst->next;
	}

	manager->file = fopen(manager->filename,"wb+");//wb+
	ret = fwrite(buffer_detach(manager->buf),1,manager->buf->NUL,manager->file);
	fclose(manager->file);
	
	//only flush the sstable in WRITE and COMPACT status,because other status ssstable no change
	sst = manager->uncompact;
	while (sst && sst->status != SNULL)
	{
		if (sst->status == WRITE || sst->status == COMPACT)
		{
			sst_flush(sst);
		}
		sst = sst->next;
	}

	sst = manager->compact;
	while (sst && sst->status != SNULL)
	{
		if (sst->status == WRITE || sst->status == COMPACT)
		{
			sst_flush(sst);
		}
		sst = sst->next;
	}
}

void sstmanager_close( sstmanager_t* manager )
{
	int i;
	sstable_t* sst;

	if (manager->pool)
	{
		threadPool_destroy(manager->pool);
	}

	//close all the sstables
	sst = manager->uncompact;
	while (sst)
	{
		manager->uncompact = sst->next;
		sst->next = NULL;
		sst_close(sst);
		sst = manager->uncompact;
	}

	sst = manager->compact;
	while (sst)
	{
		manager->compact = sst->next;
		sst->next = NULL;
		sst_close(sst);
		sst = manager->compact;
	}

	ReleaseMutex(manager->lock);

	if (manager->buf)
	{
		buffer_free(manager->buf);
	}

	if (manager->file)
	{
		fclose(manager->file);
	}
}

void sstmanager_addsst( sstmanager_t* manager,sstable_t* sst )
{
	//add sstable to the head of sstable list
// 	sst->next = manager->head;
// 	manager->head = sst;

	manager->current_id++;
	manager->sst_num++;

	if (sst->status == COMPACT)
	{
		manager->compact_num++;
		sst->next = manager->compact;
		manager->compact = sst;
	}
	else
	{
		manager->uncompact_num++;
		sst->next = manager->uncompact;
		manager->uncompact = sst;
	}
}

void sstmanager_rmsst(sstmanager_t* manager,int start,int end)
{
	int i;
	char* filename;
	sstable_t* rmsst;
	sstable_t* psst = manager->head;
	sstable_t** ssts;
	ssts = xmalloc(sizeof(sstable_t*) * manager->sst_num);

	//get sstable
	for (i=manager->sst_num-1; i>=0; i--)
	{
		ssts[i] = psst;
		psst = psst->next;
		if (ssts[i]->status == WRITE && ssts[i]->status == WFULL)
		{
			printf("I refuse do delete for sstable in writing\n");
			goto CLEAR;
		}
	}

	//remove sstables,data,file
	for (i=start; i<=end; i++)
	{
		rmsst = ssts[i];
		ssts[i+1]->next = ssts[i]->next;		//delete sstable from sstable list
		manager->sst_num--;						//sstable list size decrease 1
		filename = rmsst->htable->filename;
		sst_close(rmsst);
		remove(filename);
	}

CLEAR:
	xfree(ssts);
}

void sstmanager_createsst(sstmanager_t* manager,sst_status status)
{
	//use id create a sstable,add to sstable list, open it with status 
	sstable_t* sst = sst_new(manager->current_id);
	sstmanager_addsst(manager,sst);
	sst_open(sst,status);
	if (status == WRITE)
	{
		manager->writetable = manager->uncompact;
	}

	if (manager->sst_num > 5)
	{
		//notify
	}
}

int sstmanager_put( sstmanager_t* manager,data_t* data )
{
	int ret;
	sstable_t* cursstable;

REPEAT:
	TakeLock(manager->lock);
	cursstable = manager->uncompact;
	//if there are nothing in sstable list, create a new sstable in list
	if (manager->sst_num == 0 || manager->uncompact == NULL || cursstable->status == UNOPEN)
	{
		sstmanager_createsst(manager,WRITE);	//create WRITE sstable
		cursstable = manager->writetable;
		printf("----%d %s\n",cursstable->id,data->key);
	}
	unTakeLock(manager->lock);

	ret = sst_put(cursstable,data);

	//ret = 1 :represent sst is full
	if (ret == 0)
	{
		//printf("ret : %d -- %s\n",ret,data->key);
		return ret;
	}
	else if (ret == 1)
	{
		TakeLock(manager->lock);
		TakeLock(cursstable->lock);
		//add sstable flush work to threadPool
		printf("[%d] ret:%d->%d %x %d %s\n",GetCurrentThreadId(),ret,cursstable->id,cursstable,cursstable->status,data->key);
		if (cursstable->status == WRITE)
		{
			cursstable->status = FLUSH;
			threadPool_addJob(manager->pool,sst_flush,cursstable);
			sstmanager_createsst(manager,WRITE);	//create WRITE sstable
			printf("[%d] ret:%d->%d %x %d %s\n",GetCurrentThreadId(),ret,cursstable->id,cursstable,cursstable->status,data->key);
		}
		unTakeLock(cursstable->lock);
		cursstable = manager->writetable;
		unTakeLock(manager->lock);

		ret = sst_put(cursstable,data); 

		if (ret == 2)
		{
			printf("[goto1] ret : %d -- %s\n",ret,data->key);
			goto REPEAT;
		}
		return ret;
	}
	else if (ret == 2)
	{
		printf("[goto2] ret : %d -- %s\n",ret,data->key);
		goto REPEAT;
	}
}

data_t* sstmanager_get( sstmanager_t* manager,const char* key,int keySize )
{
	int i = manager->sst_num - 1;
	sstable_t* pos;
	data_t* data = NULL;
	pos = manager->uncompact;
	while (pos && pos->status != SNULL && i >= 0)
	{
		data = sst_get(pos,key,keySize);
		if (data)
		{
			break;
		}
		i--;
		pos = pos->next;
	}
	return data;
}

data_t* _sstmanager_findsmallest(sstable_t** ssts,data_t** datas,int number,int start,int* point)
{
	int i,ret;
	int index= -1;
	data_t* mindata;//data[....][tail] tail is always null
	datas[number-1] = NULL;
	mindata = datas[number-1];//data[....][tail] tail is always null 
	for (i=0; i<number-1; i++)
	{
		if (point[i]<ssts[i]->htable->key_num)
		{
			datas[i] = ssts[i]->htable->datas[point[i]];
		}
		else
		{
			datas[i] = NULL;
		}

		ret = ComparatorC(datas[i],mindata);
		if (ret == 1)
		{
			//TODO
		}
		else if (ret == 0)	//version
		{
			if (mindata == NULL)
			{
				//TODO
			}
			else if (mindata->version > datas[i]->version)
			{
				point[i]++;
			}
			else
			{
				mindata = datas[i];
				point[index]++;
				index = i;
			}
		}
		else
		{
			mindata = datas[i];
			index = i;
		}
	}
	if (index == -1)
	{
		return NULL;
	}

	point[index]++;
	
	return mindata;
}

void sstmanager_compact( sstmanager_t* manager,int begin,int end )
{
	int ret;
	int *point;
	data_t* getdata;
	data_t** datas;
	sstable_t* psst = manager->head;
	sstable_t** ssts;

	int i,count = 0;
	int num = end - begin + 1;
	if (begin == end || manager->sst_num <= end)
	{
		printf("only one sstable or the sstables number is litter than end\n");
		return;
	}

	//init the point, use the sort data
	point = (int*)xmalloc(sizeof(int) * (end - begin + 1));
	datas = (sstable_t**)xmalloc(sizeof(data_t*) * (num + 1));
	ssts = (sstable_t**)xmalloc(sizeof(sstable_t*) * manager->sst_num);

	//copy ssts...
	for (i=manager->sst_num-1; i>=0; i--)
	{
		ssts[i] = psst;
		psst = psst->next;
		//this if has a problem
		if (ssts[i]->status == WRITE && ssts[i]->status == WFULL)
		{
			printf("I refuse do compact for sstable in writing\n");
			goto CLEAR;
		}
	}
	//init the data in point with zero
	memset(point,0,sizeof(int) * (end-begin+1));

	for (i=begin; i<=end; i++)
	{
		sst_precompact(ssts[i]);
	}

	sstmanager_createsst(manager,COMPACT);	//create COMPACT sstable
	manager->compact = manager->head;

	while ((getdata=_sstmanager_findsmallest(ssts,datas,num+1,begin,point)) != NULL)
	{
		ret = sst_compactput(manager->compact,getdata);	//ret = 0 is ok,ret != 0 is sst full
		if (ret != 0)
		{
			sst_flush(manager->compact);
			sstmanager_createsst(manager,COMPACT);		//create COMPACT sstable
			manager->compact = manager->head;
			ret = sst_compactput(manager->compact,getdata);
		}
		count++;
	}

	sst_flush(manager->compact);						//flush compact sstable
	sstmanager_rmsst(manager,begin,end);				//after compact remove sstable


CLEAR:
	xfree(point);
	xfree(datas);
	xfree(ssts);
}

void sstmanager_checkcompact( void* arg )
{
	sstmanager_t* manager = (sstmanager_t*)arg;
	while (manager->compact_run)
	{
		if (manager->sst_num > 5)
		{
			sstmanager_compact(manager,manager->start_id,manager->current_id - 2);
			Sleep(30000);
		}
		else
		{
			//waiting
		}
	}
}