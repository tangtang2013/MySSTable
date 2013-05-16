#include "data.h"
#include "debug.h"
#include <sys\stat.h>

void* sstdata_new(int id)
{
    sst_data_t* sstdata = (sst_data_t*)xmalloc(sizeof(sst_data_t));
    memset(sstdata,0,sizeof(sst_data_t));
    sstdata->id = id;

    memset(sstdata->filename,0,128);
    sprintf(sstdata->filename,"sst%06d.data",sstdata->id); 
    sstdata->buf = buffer_new(1024);

	sstdata->key_num = -1;	//represent the data structure varie is in not initialized status
    
    return sstdata;   
}

void sstdata_open(sst_data_t* sstdata)
{
    int ret;
    int id;
	int i;

    struct _stat info;
    _stat(sstdata->filename, &info);
    if(info.st_size <12)
    {
        sstdata->key_num = 0;
        sstdata->max = 1024 * 1024;
        __INFO("file content error:%s",sstdata->filename);
    }
    else
    {
        sstdata->file = fopen(sstdata->filename,"rb");
        ret = fread(buffer_detach(sstdata->buf),12,1,sstdata->file);
        sstdata->buf->NUL = 12;
        id = buffer_getint(sstdata->buf);
        sstdata->key_num = buffer_getint(sstdata->buf);
        sstdata->max = buffer_getint(sstdata->buf);
        
        if(sstdata->id != id)
        {
            __INFO("file content id:%d not equal read id:%d",sstdata->id,id);
        }
		if(info.st_size - 12 > sstdata->buf->buflen)
		{
			buffer_free(sstdata->buf);
			sstdata->buf = buffer_new(info.st_size - 12);
		}

		buffer_seekfirst(sstdata->buf);
		fread(buffer_detach(sstdata->buf),info.st_size - 12,1,sstdata->file);
		
		i = sstdata->key_num;
		sstdata->keys = (data_t*)xmalloc(sstdata->key_num * sizeof(data_t*));
		for (i=0; i<sstdata->key_num; i++)
		{
			sstdata->keys[i] = buffer_getdata(sstdata->buf);
		}
    }
}

void sstdata_build(sst_data_t* sstdata)
{
    __INFO("index build : %s",sstdata->filename);
    sstdata->file = fopen(sstdata->filename,"wb+");
    if(!sstdata->file)
        __PANIC("file open failed -- exiting:%s",sstdata->filename);
        
    sstdata->key_num = 0;
    sstdata->max = 1024;
    sstdata->keys = (data_t*)xmalloc(1024 * sizeof(data_t*));
}

void sstdata_flush(sst_data_t* sstdata)
{
    int ret,i,len;
	
    buffer_putint(sstdata->buf,sstdata->id);
    buffer_putint(sstdata->buf,sstdata->key_num);
    buffer_putint(sstdata->buf,sstdata->max);
    __INFO("buffer NUL:%d\n",sstdata->buf->NUL);
    ret = fwrite(buffer_detach(sstdata->buf),1,sstdata->buf->NUL,sstdata->file);
    __INFO("index flush : %s, NUL:%d, ret:%d ",sstdata->filename,sstdata->buf->NUL,ret);
	
	buffer_clear(sstdata->buf);
	for (i=0; i<sstdata->key_num; i++)
	{
		buffer_putdata(sstdata->buf,sstdata->keys[i]);
	}
	ret = fwrite(buffer_detach(sstdata->buf),1,sstdata->buf->NUL,sstdata->file);
	__INFO("index data flush : %s, NUL:%d, ret:%d ",sstdata->filename,sstdata->buf->NUL,ret);

	fflush(sstdata->file);
}

void sstdata_free(sst_data_t* sstdata)
{
    int i;
    for(i=0; i<sstdata->key_num; i++)
    {
        xfree(sstdata->keys[i]);
    }

	if(sstdata->file)
		fclose(sstdata->file);

	if(sstdata->buf)
		buffer_free(sstdata->buf);

	if(sstdata->keys)
		xfree(sstdata->keys);
}

int sstdata_put(sst_data_t* sstdata,data_t data)
{
    data_t* temp;
    if(sstdata->max > sstdata->key_num)
    {
        temp = clone_data(&data);
        sstdata->keys[sstdata->key_num] = temp;
        sstdata->key_num++;
        return 0;
    }
    else
    {
        __INFO("file is full");
        return 1;
    }
}

data_t* sstdata_get(sst_data_t* sstdata,const char* key)
{
    int i;
    
    for(i=0; i<sstdata->key_num; i++)
    {
        if(strcmp(key,sstdata->keys[i]->key) == 0)
        {
            return sstdata->keys[i];
        }
    }
    return NULL;
}
