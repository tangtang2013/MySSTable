#include "sst.h"
#include "debug.h"

void* sst_new(int id)
{
    sstable_t* sst = (sstable_t*)xmalloc(sizeof(sstable_t));
    memset(sst,0,sizeof(sstable_t));

	sst->status = UNOPEN;	//if a sstable have been new, status is UNOPEN,initial stats is SNULL 
	sst->id = id;
  
    return sst;
}

void sst_open(sstable_t* sst,sst_status status)
{
	//create sstable in type
	sst->sstdata = (sst_data_t*)sstdata_new(sst->id);
	if (status == READ)
	{
		sstdata_open(sst->sstdata);
		sst->status = READ;
	} 
	else if (status == WRITE)
	{
		sstdata_build(sst->sstdata);
		sst->status = WRITE;
	}
	else if (status == COMPACT)				//COMPACT sstable can not get...
	{
		sstdata_build(sst->sstdata);
		sst->status = COMPACT;
		sstdata_writehead(sst->sstdata);	//why I write in this? because I want to keep a clear architecture
	}
}

void sst_flush(sstable_t* sst)
{
	if (sst->status == COMPACT)
	{
		sstdata_writehead(sst->sstdata);
		sst->status = COMPACTED;
	} 
	else
	{
		sstdata_writehead(sst->sstdata);
		sstdata_writedata(sst->sstdata);
		sst->status = READ;
	}
}

void sst_close(sstable_t* sst)
{
	if (sst->status == COMPACT || sst->status == COMPACTED || sst->status !=UNOPEN)
	{
			sstdata_free(sst->sstdata);
	}
	else if (sst->status != SNULL)
	{
		sstdata_relasedata(sst->sstdata);
		sstdata_free(sst->sstdata);
	}
    
	xfree(sst);
}

int sst_put(sstable_t* sst,data_t* data)
{
    return sstdata_put(sst->sstdata,data);
}

data_t* sst_get(sstable_t* sst,const char* key)
{
	if (sst->status == COMPACT || sst->status == COMPACTED)
	{
		return NULL;
	}
	else if (sst->status == UNOPEN)
	{
		sst_open(sst,READ);
	}
    return sstdata_get(sst->sstdata,key);
}

int sst_compactput(sstable_t* sst,data_t* data)
{
	if (sst->status == COMPACT)
	{
		return sstdata_compactput(sst->sstdata,data);
	}
}