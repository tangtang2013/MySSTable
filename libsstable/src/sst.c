#include "sst.h"
#include "debug.h"

void* sst_new(int id)
{
    sstable_t* sst = (sstable_t*)xmalloc(sizeof(sstable_t));
    memset(sst,0,sizeof(sstable_t));

	sst->status = UNOPEN;	//if a sstable have been new, status is UNOPEN,initial stats is SNULL 
	sst->id = id;

	sst->lock = CreateMutex(0, 0, 0);
  
    return sst;
}

void sst_open(sstable_t* sst,sst_status status)
{
	//create sstable in type
	sst->htable = (hashtable_t*)hashtable_new(sst->id);
	if (status == READ)
	{
		hashtable_open(sst->htable);
		sst->status = READ;
	} 
	else if (status == WRITE)
	{
		hashtable_build(sst->htable);
		sst->status = WRITE;
	}
	else if (status == COMPACT)				//COMPACT sstable can not get...
	{
		hashtable_build(sst->htable);
		sst->status = COMPACT;
		hashtable_writehead(sst->htable);	//why I write in this? because I want to keep a clear architecture
	}
}

void sst_flush(sstable_t* sst)
{
	if (sst->status == COMPACT)
	{
		hashtable_writehead(sst->htable);
		sst->status = COMPACTED;
	}
	else if (sst->status == WFULL)
	{
		hashtable_writehead(sst->htable);
		hashtable_writedata(sst->htable);
		sst->status = FLUSH;
	}
	else
	{
		hashtable_writehead(sst->htable);
		hashtable_writedata(sst->htable);
		sst->status = READ;
	}
}

void sst_close(sstable_t* sst)
{
	if (sst->status ==UNOPEN)
	{
		//TODO
	}
	else if (sst->status == COMPACT || sst->status == COMPACTED)
	{
		hashtable_free(sst->htable);
	}
	else if (sst->status != SNULL)
	{
		hashtable_relasedata(sst->htable);
		hashtable_free(sst->htable);
	}

	CloseHandle(sst->lock);
    
	xfree(sst);
}

int sst_put(sstable_t* sst,data_t* data)
{
	int ret = 2;	//do nothing
	if (sst->status == WRITE)
	{
		ret = hashtable_put(sst->htable,data);
	}

	if (ret == 1)			//change the sstable's status
	{
//		TakeLock(sst->lock);
//		printf("[%d]sst : %d status : %d\n",GetCurrentThreadId(),sst->id,sst->status);
//		sst->status = WFULL;
//		printf("[%d]sst : %d status : %d\n",GetCurrentThreadId(),sst->id,sst->status);
//		unTakeLock(sst->lock);
	}

	return ret;
}

data_t* sst_get(sstable_t* sst,const char* key,int keySize)
{
	if (sst->status == COMPACT || sst->status == COMPACTED)
	{
		return NULL;
	}
	else if (sst->status == UNOPEN)
	{
		sst_open(sst,READ);
	}
    return hashtable_get(sst->htable,key,keySize);
}

int sst_compactput(sstable_t* sst,data_t* data)
{
	if (sst->status == COMPACT)
	{
		return hashtable_compactput(sst->htable,data);
	}
}

void sst_precompact( sstable_t* sst )
{
	if (sst->status == UNOPEN)
	{
		sst_open(sst,READ);
	}
	hashtable_sort(sst->htable);
}