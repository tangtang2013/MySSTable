#include "sst.h"
#include "debug.h"

void* sst_new(int id)
{
    sstable_t* sst = (sstable_t*)xmalloc(sizeof(sstable_t));
    memset(sst,0,sizeof(sstable_t));

	sst->status = SNULL;
	sst->id = id;
  
    return sst;
}

void sst_open(sstable_t* sst)
{
	//create index in open type
	sst->sstdata = (sst_data_t*)sstdata_new(sst->id);
	sstdata_open(sst->sstdata);
	sst->status = READ;
}

void sst_build(sstable_t* sst)
{
    sst->sstdata = (sst_data_t*)sstdata_new(sst->id);

    sstdata_build(sst->sstdata);
	sst->status = WRITE;
}

void sst_flush(sstable_t* sst)
{
    sstdata_flush(sst->sstdata);    
}

void sst_close(sstable_t* sst)
{
	if (sst->status != SNULL)
	{
		sstdata_free(sst->sstdata);
	}
    
	xfree(sst);
}

int sst_put(sstable_t* sst,data_t data)
{
    return sstdata_put(sst->sstdata,data);
}

data_t* sst_get(sstable_t* sst,const char* key)
{
    return sstdata_get(sst->sstdata,key);
}

