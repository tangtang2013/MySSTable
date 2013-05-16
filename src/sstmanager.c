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

	manager->sstables = xmalloc(sizeof(sstable_t*) * SSTABLE_NUM);	//if no enough, need expend
	memset(manager->sstables,0,sizeof(sstable_t*) * SSTABLE_NUM);

	manager->buf = buffer_new(1024);
	return manager;
}

void sstmanager_open(sstmanager_t* manager)
{
	int ret,i;
	sstable_t** ssts = NULL;
	
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
		if (manager->sst_num > manager->max)
		{
			ssts = xrealloc(manager->sstables, sizeof(sstable_t*) * (manager->sst_num + 10));
			manager->sstables = ssts;
			manager->max += 10;
		}

		//read sstable...not open
		for (i=0; i<manager->sst_num; i++)
		{
			manager->sstables[i] = sst_new(i);
		}
	}
}

void sstmanager_flush( sstmanager_t* manager )
{
	int i,ret;
	buffer_clear(manager->buf);
	buffer_seekfirst(manager->buf);
	buffer_putint(manager->buf,manager->current_id);
	buffer_putint(manager->buf,manager->sst_num);
	buffer_putint(manager->buf,manager->max);

	manager->file = fopen(manager->filename,"wb+");//wb+
	ret = fwrite(buffer_detach(manager->buf),1,manager->buf->NUL,manager->file);
	fclose(manager->file);
	
	for (i=0; i<manager->sst_num; i++)
	{
		if (manager->sstables[i]->status == WRITE)
		{
			sst_flush(manager->sstables[i]);
		}
	}
}

void sstmanager_close( sstmanager_t* manager )
{
	int i;

	for (i=0; i<manager->sst_num; i++)
	{
		sst_close(manager->sstables[i]);
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

int _readid( sstmanager_t* manager )
{
	int ret;
	if(manager->file = fopen(manager->filename,"rb+"))
	{
		//read file name to init sstable
		ret = fread(buffer_detach(manager->buf),4,1,manager->file);
		manager->buf->NUL = 4;
		ret = buffer_getint(manager->buf);
		__INFO("sst last id : %d",ret);

		fclose(manager->file);       
	}
	else
	{
		__INFO("sst:open error...%s not exist",manager->filename);

		ret = -1;
	}

	return ret;
}

void sstmanager_addsst( sstmanager_t* manager,sstable_t* sst )
{
	sstable_t** ssts = NULL;

	if (manager->sst_num + 1 > manager->max)
	{
		ssts = xrealloc(manager->sstables, sizeof(sstable_t*) * (manager->sst_num + 10));
		manager->sstables = ssts;
		manager->max += 10;
	}

	manager->sstables[manager->current_id] = sst;
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

int sstmanager_put( sstmanager_t* manager,data_t data )
{
	int ret;
	if (manager->sst_num == 0 || manager->curtable == NULL)
	{
		sstmanager_createsst(manager);
		manager->curtable = manager->sstables[manager->current_id - 1];
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
		manager->curtable = manager->sstables[manager->current_id - 1];
		ret = sst_put(manager->curtable,data); 
		return ret;
	}
}

data_t* sstmanager_get( sstmanager_t* manager,const char* key )
{
	int i = manager->sst_num - 1;
	sstable_t* pos;
	data_t* data = NULL;
	pos = manager->sstables[i];
	while (pos && i >= 0)
	{
		if (pos->status == SNULL)
		{
			sst_open(pos);
		}
		data = sst_get(pos,key);
		if (data)
		{
			break;
		}
		i--;
		pos = manager->sstables[i];
	}
	return data;
}