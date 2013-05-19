#include "data.h"
#include "debug.h"
#include <sys\stat.h>

void* sstdata_new(int id)
{
    sst_data_t* sstdata = (sst_data_t*)xmalloc(sizeof(sst_data_t));
    memset(sstdata,0,sizeof(sst_data_t));
    sstdata->id = id;
	sstdata->max = 1024*64;

    memset(sstdata->filename,0,128);
    sprintf(sstdata->filename,"sst%06d.data",sstdata->id); 

    sstdata->buf = buffer_new(1024*2);

	sstdata->key_num = -1;	//represent the data structure varie is in not initialized status
    
    return sstdata;   
}

void sstdata_open(sst_data_t* sstdata)
{
    int ret;
    int id;
	int i;
	int filterlen = 0;

    struct _stat info;
    _stat(sstdata->filename, &info);
    if(info.st_size <12)
    {
        sstdata->key_num = 0;
        __INFO("file content error:%s",sstdata->filename);
    }
    else
    {
        sstdata->file = fopen(sstdata->filename,"rb");
        ret = fread(buffer_detach(sstdata->buf),12,1,sstdata->file);
        sstdata->buf->NUL = 16;
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

		buffer_clear(sstdata->buf);
		buffer_seekfirst(sstdata->buf);
		fseek(sstdata->file,12+filterlen,SEEK_SET);
		ret = fread(buffer_detach(sstdata->buf),1,info.st_size - 12 - filterlen,sstdata->file);
		
		i = sstdata->key_num;
		sstdata->keys = (data_t*)xmalloc(sstdata->key_num * sizeof(data_t*));
		for (i=0; i<sstdata->key_num; i++)
		{
			sstdata->keys[i] = buffer_getdata(sstdata->buf);
		}

		sstdata->bigest_key = sstdata->keys[sstdata->key_num-1];
		sstdata->smallest_key = sstdata->keys[0];
    }
}

void sstdata_build(sst_data_t* sstdata)
{
    __INFO("index build : %s",sstdata->filename);
    sstdata->file = fopen(sstdata->filename,"wb+");
    if(!sstdata->file)
        __PANIC("file open failed -- exiting:%s",sstdata->filename);
        
    sstdata->key_num = 0;

    sstdata->keys = (data_t*)xmalloc(sstdata->max * sizeof(data_t*));
}

void sstdata_flush(sst_data_t* sstdata)
{
    int ret,i,len;
	
    buffer_putint(sstdata->buf,sstdata->id);
    buffer_putint(sstdata->buf,sstdata->key_num);
    buffer_putint(sstdata->buf,sstdata->max);
    __INFO("buffer NUL:%d\n",sstdata->buf->NUL);
    ret = fwrite(buffer_detach(sstdata->buf),1,sstdata->buf->NUL,sstdata->file);
    __INFO("sstdata:flush data head flush : %s, NUL:%d, ret:%d ",sstdata->filename,sstdata->buf->NUL,ret);
	
	buffer_clear(sstdata->buf);
	for (i=0; i<sstdata->key_num; i++)
	{
		buffer_putdata(sstdata->buf,sstdata->keys[i]);
		if (sstdata->buf->NUL > 8192)
		{
			ret = fwrite(buffer_detach(sstdata->buf),sstdata->buf->NUL,1,sstdata->file);
			buffer_clear(sstdata->buf);
		}
	}
	ret = fwrite(buffer_detach(sstdata->buf),sstdata->buf->NUL,1,sstdata->file);
//	__INFO("index data flush : %s, NUL:%d, ret:%d ",sstdata->filename,sstdata->buf->NUL,ret);

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

void _sstdata_sort(sst_data_t* sstdata)
{
	int i;
	int j;
	data_t* index;
	for (i=1; i<sstdata->key_num; i++)
	{
		j = i;
		index = sstdata->keys[i];
		while (j>0 && Comparator(*index,*sstdata->keys[j-1]) == -1)
		{
			sstdata->keys[j] = sstdata->keys[j-1];
			j--;
		}
		sstdata->keys[j] = index;
	}
}

data_t* _sstdata_binarysearch(sst_data_t* sstdata, unsigned int hashvalue, const char* key)
{
	int left, right, middle;

	left = 0, right = sstdata->key_num - 1;

	while (left <= right)
	{
		middle = (left + right) / 2;
		if (ComparatorB(sstdata->keys[middle]->hash_value,sstdata->keys[middle]->key,hashvalue,key) == 1)
		{
			right = middle - 1;
		}
		else if (ComparatorB(sstdata->keys[middle]->hash_value,sstdata->keys[middle]->key,hashvalue,key) == -1)
		{
			left = middle + 1;
		}
		else
		{
			return sstdata->keys[middle];
		}
	}
	return NULL;
}

int _sstdata_binaryinsert(sst_data_t* sstdata, data_t* data)
{
	int left, right, middle;
	int i,j;
	if (sstdata->key_num == 0)
	{
		sstdata->keys[0] = data;
		return 1;
	}

	left = 0, right = sstdata->key_num - 1;

	while (left <= right)
	{
		middle = (left + right) / 2;
		if (Comparator(*sstdata->keys[middle], *data) == 1)
		{
			right = middle - 1;
		}
		else if (Comparator(*sstdata->keys[middle], *data) == -1)
		{
			left = middle + 1;
		}
		else if (Comparator(*sstdata->keys[middle], *data) == 0)
		{
			xfree(sstdata->keys[middle]);	//if the key is exist,replace
			sstdata->keys[middle] = data;
			return 0;
		}
	}

	i = middle;
	if (Comparator(*sstdata->keys[middle], *data) > 0)
	{
		i = middle;
	}
	else
	{
		i = middle + 1;
	}
	j = sstdata->key_num - 1;
	for (; j>=i; j--)
	{
		sstdata->keys[j+1] = sstdata->keys[j];
	}

	sstdata->keys[i] = data;
	
	return 1;
}

int sstdata_put(sst_data_t* sstdata,data_t* data)
{
    if(sstdata->max > sstdata->key_num)
    {
		sstdata->keys[sstdata->key_num] = data;
		_sstdata_binaryinsert(sstdata, data);
		sstdata->key_num++;
//		_sstdata_sort(sstdata);
		sstdata->bigest_key = sstdata->keys[sstdata->key_num-1];
		sstdata->smallest_key = sstdata->keys[0];
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
	unsigned long hash_value;
	hash_value = PMurHash32(0,key,strlen(key));
	if (hash_value <= sstdata->smallest_key->hash_value && hash_value >= sstdata->bigest_key->hash_value)
	{
		return NULL;
	}
    
	return _sstdata_binarysearch(sstdata,hash_value,key);
}
