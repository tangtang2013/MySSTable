#include "debug.h"
#include "hashtable.h"
#include "PMurHash.h"
#include "xmalloc.h"
#include <sys/stat.h>

void* hashtable_new( int id )
{
	hashtable_t* htable = (hashtable_t*)xmalloc(sizeof(hashtable_t));
	memset(htable,0,sizeof(hashtable_t));
	htable->id = id;
	htable->bucket_szie = 1024*4;
	htable->max = 1024*64;

	memset(htable->filename,0,128);
	sprintf(htable->filename,"sst%06d.data",htable->id); 

	htable->buf = buffer_new(1024*2);

	htable->key_num = -1;	//represent the data structure varie is in not initialized status

	return htable;   
}

void hashtable_open( hashtable_t* htable )
{
	int ret;
	int id;
	int i;
	int filterlen = 0;
	int key_num = 0;

	struct _stat info;
	_stat(htable->filename, &info);
	if(info.st_size < 16)
	{
		htable->key_num = 0;
		__INFO("file content error:%s",htable->filename);
	}
	else
	{
		htable->file = fopen(htable->filename,"rb");
		ret = fread(buffer_detach(htable->buf),16,1,htable->file);
		htable->buf->NUL = 20;
		id = buffer_getint(htable->buf);
		key_num = buffer_getint(htable->buf);
		htable->max = buffer_getint(htable->buf);
		htable->bucket_szie = buffer_getint(htable->buf);

		if(htable->id != id)
		{
			__INFO("file content id:%d not equal read id:%d",htable->id,id);
		}
		if(info.st_size - 16 > htable->buf->buflen)
		{
			buffer_free(htable->buf);
			htable->buf = buffer_new(info.st_size - 12);
		}

		buffer_clear(htable->buf);
		buffer_seekfirst(htable->buf);
		fseek(htable->file,16+filterlen,SEEK_SET);
		ret = fread(buffer_detach(htable->buf),1,info.st_size - 12 - filterlen,htable->file);

		i = htable->key_num;
		htable->buckets = (data_t*)xmalloc(htable->bucket_szie * sizeof(data_t*));
		memset(htable->buckets,0,htable->bucket_szie * sizeof(data_t*));
		for (i=0; i<key_num; i++)
		{
			hashtable_put(htable,buffer_getdata(htable->buf));
		}

		//htable->bigest_key = htable->keys[sstdata->key_num-1];
		//htable->smallest_key = htable->keys[0];
	}
}

void hashtable_free( hashtable_t* htable )
{
	if(htable->file)
		fclose(htable->file);

	if(htable->buf)
		buffer_free(htable->buf);
}

void hashtable_build( hashtable_t* htable )
{
	__INFO("hashtable build : %s",htable->filename);
	htable->file = fopen(htable->filename,"wb+");
	if(!htable->file)
		__PANIC("file open failed -- exiting:%s",htable->filename);

	htable->key_num = 0;
	htable->bucket_szie = 1024*4;

	htable->buckets = (data_t*)xmalloc(htable->bucket_szie * sizeof(data_t*));
	memset(htable->buckets,0,(htable->bucket_szie * sizeof(data_t*)));
}

void hashtable_flush( hashtable_t* htable )
{
	//write sstable head in file
	hashtable_writehead(htable);

	//write sstable data in file 
	hashtable_writedata(htable);

	fflush(htable->file);
}

int hashtable_put( hashtable_t* htable,data_t* data )
{
	data_t* p = htable->buckets[data->hash_value % htable->bucket_szie];
	data_t* q = NULL;

	if (htable->key_num >= htable->max)
	{
		return 1;	//hashtable is full
	}

	while(p && CmpKey(p->key, p->key_len, data->key, data->key_len))
	{
		q = p;
		p = p->next;
	}

	if (p)
	{
		if (q)
		{
			q->next = data;
		} 
		else
		{
			htable->buckets[data->hash_value % htable->bucket_szie] = data;
		}
		data->next = p->next;
	} 
	else
	{
		data->next = htable->buckets[data->hash_value % htable->bucket_szie];
		htable->buckets[data->hash_value % htable->bucket_szie] = data;
		htable->key_num++;
	}

	return 0;
}

data_t* hashtable_get( hashtable_t* htable,const char* key )
{
	size_t hashValue;
	data_t* pBucket;
	if (htable->key_num <= 0)
	{
		return NULL;
	}

	hashValue = PMurHash32(0,key,strlen(key));

	pBucket = htable->buckets[hashValue % htable->bucket_szie];

	while(pBucket && 0 != CmpKey(pBucket->key, pBucket->key_len, key, strlen(key)))
	{
		pBucket = pBucket->next;
	}

	if (!pBucket)
	{
		return NULL;
	}

	return pBucket;

}

void hashtable_relasedata( hashtable_t* htable )
{
	int ret,i;
	data_t* data,*temp;

	for (i=0; i<htable->bucket_szie; i++)
	{
		data = htable->buckets[i];
		while(data)
		{
			temp = data->next;
			xfree(data);
			data = temp;
		}
	}
}

void hashtable_writehead( hashtable_t* htable )
{
	int ret;

	buffer_clear(htable->buf);
	buffer_putint(htable->buf,htable->id);
	buffer_putint(htable->buf,htable->key_num);
	buffer_putint(htable->buf,htable->max);
	buffer_putint(htable->buf,htable->bucket_szie);

	__INFO("buffer NUL:%d\n",htable->buf->NUL);

	fseek(htable->file,0,SEEK_SET);
	ret = fwrite(buffer_detach(htable->buf),1,htable->buf->NUL,htable->file);
	fflush(htable->file);

	__INFO("sstdata:flush data head flush : %s, NUL:%d, ret:%d ",htable->filename,htable->buf->NUL,ret);

	buffer_clear(htable->buf);
}

void hashtable_writedata( hashtable_t* htable )
{
	int ret,i;
	data_t* data;

	for (i=0; i<htable->bucket_szie; i++)
	{
		data = htable->buckets[i];
		while(data)
		{
			buffer_putdata(htable->buf,data);
			if (htable->buf->NUL > 8192)
			{
				ret = fwrite(buffer_detach(htable->buf),htable->buf->NUL,1,htable->file);
				buffer_clear(htable->buf);
			}
			data = data->next;
		}
	}

	ret = fwrite(buffer_detach(htable->buf),htable->buf->NUL,1,htable->file);
	fflush(htable->file);
}

data_t* hashtable_nextdata( hashtable_t* htable )
{
	data_t* data;
	if (htable->next_num >= htable->key_num)
	{
		return NULL;
	}
	while (htable->nextdata == NULL)
	{
		htable->nextdata = htable->buckets[htable->index];

		if (htable->index >= htable->bucket_szie)
		{
			return NULL;
		}
	}
	data = htable->nextdata;
	htable->nextdata = htable->nextdata->next;
	while (htable->nextdata == NULL)
	{
		if (htable->index + 1 >= htable->bucket_szie)
		{
			break;
		} 
		else
		{
			htable->index++;
			htable->nextdata = htable->buckets[htable->index];
		}
	}
	if (data)
	{
		htable->next_num++;
	}
	return data;
}

int hashtable_compactput( hashtable_t* htable,data_t* data )
{
	int ret;
	if(htable->max > htable->key_num)
	{
		buffer_clear(htable->buf);
		buffer_putdata(htable->buf,data);
		ret = fwrite(buffer_detach(htable->buf),htable->buf->NUL,1,htable->file);
		htable->key_num++;
		return 0;
	}
	else
	{
		__INFO("file is full");
		return 1;
	}
}