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

	manager->head = NULL;//xmalloc(sizeof(sstable_t));	//if no enough, need expend
	//memset(manager->head,0,sizeof(sstable_t));

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

		//sstable size not enough.... try expend

		//read sstable...not open
		for (i=0; i<manager->sst_num; i++)
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
	
	sst = manager->head;
	while (sst && sst->status == WRITE)
	{
		sst_flush(sst);
		sst = sst->next;
	}
// 	for (i=0; i<manager->sst_num; i++)
// 	{
// 		if (manager->sstable_list[i]->status == WRITE)
// 		{
// 			sst_flush(manager->sstable_list[i]);
// 		}
// 	}
}

void sstmanager_close( sstmanager_t* manager )
{
	int i;
	sstable_t* sst;

	sst = manager->head;
	while (sst)
	{
		manager->head = sst->next;
		sst->next = NULL;
		sst_close(sst);
		sst = manager->head;
	}

// 	for (i=0; i<manager->sst_num; i++)
// 	{
// 		sst_close(manager->sstable_list[i]);
// 	}
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

void sstmanager_rmsst( sstmanager_t* manager,int id )
{
	
}

void sstmanager_createsst(sstmanager_t* manager)
{
	sstable_t* sst = sst_new(manager->current_id);
	sstmanager_addsst(manager,sst);
	sst_build(sst);
}

int sstmanager_put( sstmanager_t* manager,data_t* data )
{
	int ret;
	if (manager->sst_num == 0 || manager->curtable == NULL)
	{
		sstmanager_createsst(manager);
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
		manager->curtable->status = WFULL;
		sstmanager_createsst(manager);
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
		if (pos->status == UNOPEN)
		{
			sst_open(pos);
		}
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