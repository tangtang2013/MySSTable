#include "sstmanager.h"
#include "debug.h"
#include <stdio.h>

void* sstmanager_new()
{
	sstmanager_t* manager = (sstmanager_t*)xmalloc(sizeof(sstmanager_t));
	memset(manager,0,sizeof(sstmanager_t));

	memset(manager->filename,0,128);
	sprintf(manager->filename,"Manifest");

	manager->max = SSTABLE_MAX;
	manager->head = NULL;

	manager->buf = buffer_new(1024);
	return manager;
}

void sstmanager_open(sstmanager_t* manager)
{
	int ret,i;
	sstable_t* sst = NULL;
	
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
		ret = fread(buffer_detach(manager->buf),12,1,manager->file);
		manager->buf->NUL = 12;
		manager->last_id = buffer_getint(manager->buf);
		manager->sst_num = buffer_getint(manager->buf);
		manager->max = buffer_getint(manager->buf);

		manager->current_id = manager->last_id;

		//read sstable...not open
		for (i=manager->last_id-manager->sst_num; i<manager->last_id; i++)
		{
			sst = sst_new(i);
			sst->next = manager->head;
			manager->head = sst;
		}
	}
}

void sstmanager_flush( sstmanager_t* manager )
{
	int i,ret;
	sstable_t* sst;
	buffer_clear(manager->buf);
	buffer_seekfirst(manager->buf);
	buffer_putint(manager->buf,manager->current_id);
	buffer_putint(manager->buf,manager->sst_num);
	buffer_putint(manager->buf,manager->max);

	manager->file = fopen(manager->filename,"wb+");//wb+
	ret = fwrite(buffer_detach(manager->buf),1,manager->buf->NUL,manager->file);
	fclose(manager->file);
	
	//only flush the sstable in WRITE and COMPACT status,because other status ssstable no change
	sst = manager->head;
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

	//close all the sstables
	sst = manager->head;
	while (sst)
	{
		manager->head = sst->next;
		sst->next = NULL;
		sst_close(sst);
		sst = manager->head;
	}

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
	sst->next = manager->head;
	manager->head = sst;

	manager->current_id++;
	manager->sst_num++;
}

void sstmanager_rmsst(sstmanager_t* manager,int start,int end)
{
	int i;
	char* filename;
	sstable_t* rmsst;
	sstable_t* psst = manager->head;
	sstable_t** ssts;
	ssts = xmalloc(sizeof(sstable_t*) * manager->sst_num);

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
	for (i=start; i<=end; i++)
	{
		rmsst = ssts[i];
		ssts[i+1]->next = ssts[i]->next;		//delete sstable from sstable list
		manager->sst_num--;						//sstable list size decrease 1
		filename = rmsst->sstdata->filename;
		sst_close(rmsst);
		remove(filename);
	}

CLEAR:
	xfree(ssts);
}

void sstmanager_createsst(sstmanager_t* manager,sst_status status)
{
	sstable_t* sst = sst_new(manager->current_id);
	sstmanager_addsst(manager,sst);
	sst_open(sst,status);
}

int sstmanager_put( sstmanager_t* manager,data_t* data )
{
	int ret;
	if (manager->sst_num == 0 || manager->curtable == NULL)
	{
		sstmanager_createsst(manager,WRITE);	//create WRITE sstable
		manager->curtable = manager->head;
	}
	ret = sst_put(manager->curtable,data);

	//ret = -1 :represent sst full
	if (ret == 0)
	{
		return ret;
	} 
	else
	{
		sst_flush(manager->curtable);
		manager->curtable->status = WFULL;		//think??? this code is good?
		sstmanager_createsst(manager,WRITE);	//create WRITE sstable
		manager->curtable = manager->head;
		ret = sst_put(manager->curtable,data); 
		return ret;
	}
}

data_t* sstmanager_get( sstmanager_t* manager,const char* key )
{
	int i = manager->sst_num - 1;
	sstable_t* pos;
	data_t* data = NULL;
	pos = manager->head;
	while (pos && pos->status != SNULL && i >= 0)
	{
		data = sst_get(pos,key);
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
	int i;
	int index= -1;
	data_t* mindata;//data[....][tail] tail is always null
	datas[number-1] = NULL;
	mindata = datas[number-1];//data[....][tail] tail is always null 
	for (i=0; i<number-1; i++)
	{
		if (point[i]<ssts[i]->sstdata->key_num)
		{
			datas[i] = ssts[i]->sstdata->keys[point[i]];
		}
		else
		{
			datas[i] = NULL;
		}
		if (ComparatorC(datas[i],mindata) == 1)
		{
//			mindata = datas[(i+number-1)%number];
//			index = (i+number-1)%number;
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
	point = xmalloc(sizeof(int) * (end - begin + 1));
	datas = xmalloc(sizeof(data_t*) * (num + 1));
	ssts = xmalloc(sizeof(sstable_t*) * manager->sst_num);

	//copy ssts...
	for (i=manager->sst_num-1; i>=0; i--)
	{
		ssts[i] = psst;
		psst = psst->next;
		if (ssts[i]->status == WRITE && ssts[i]->status == WFULL)
		{
			printf("I refuse do compact for sstable in writing\n");
			goto CLEAR;
		}
	}
	//init the data in point with zero
	memset(point,0,sizeof(int) * (end-begin+1));

	sstmanager_createsst(manager,COMPACT);	//create COMPACT sstable
	manager->compact = manager->head;

	while ((getdata=_sstmanager_findsmallest(ssts,datas,num+1,begin,point)) != NULL)
	{
		//printf("%s %s %u\n",getdata->key,getdata->value,getdata->hash_value);
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
	sst_flush(manager->compact);
	//after compact remove sstable
	sstmanager_rmsst(manager,begin,end);


CLEAR:
	xfree(point);
	xfree(datas);
	xfree(ssts);
}
