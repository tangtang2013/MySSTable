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
		ret = (int)fread(buffer_detach(htable->buf),16,1,htable->file);
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
		ret = (int)fread(buffer_detach(htable->buf),1,info.st_size - 12 - filterlen,htable->file);

		htable->bucket_locks = xmalloc(htable->bucket_szie * sizeof(HANDLE));
		for (i=0; i<htable->bucket_szie; i++)
		{
			htable->bucket_locks[i] = CreateMutex(0, 0, 0);
		}
		htable->num_lock = CreateMutex(0,0,0);
		
		htable->key_num = 0;
		htable->buckets = (data_t**)xmalloc(htable->bucket_szie * sizeof(data_t*));
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
	
	if (htable->buckets)
	{
		xfree(htable->buckets);
	}

	if (htable->datas)
	{
		xfree(htable->datas);
	}

	if(htable->buf)
		buffer_free(htable->buf);

	if (htable->bucket_locks)
	{
		xfree(htable->bucket_locks);
	}
}

void hashtable_build( hashtable_t* htable )
{
	int i;
	__INFO("hashtable build : %s",htable->filename);
	htable->file = fopen(htable->filename,"wb+");
	if(!htable->file)
		__PANIC("file open failed -- exiting:%s",htable->filename);

	htable->key_num = 0;
	htable->bucket_szie = 1024*4;

	htable->buckets = (data_t**)xmalloc(htable->bucket_szie * sizeof(data_t*));
	memset(htable->buckets,0,(htable->bucket_szie * sizeof(data_t*)));

	htable->bucket_locks = xmalloc(htable->bucket_szie * sizeof(HANDLE));
	for (i=0; i<htable->bucket_szie; i++)
	{
		htable->bucket_locks[i] = CreateMutex(0, 0, 0);
	}
	htable->num_lock = CreateMutex(0,0,0);
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
	data_t* p;
	data_t* q = NULL;
	HANDLE hashlock = htable->bucket_locks[data->hash_value % htable->bucket_szie];
	p = htable->buckets[data->hash_value % htable->bucket_szie];

// 	TakeLock(htable->num_lock);	//LOCK number
// 	if (htable->key_num >= htable->max)
// 	{
// 		unTakeLock(htable->num_lock);
// 		return 1;	//hashtable is full
// 	}
// 	unTakeLock(htable->num_lock);

	TakeLock(hashlock);	//LOCK buckets
	while(p && Comparator(*p, *data))	//if P is NULL or p equal with data break this (while)
	{									//
		q = p;
		p = p->next;
	}

	if (p)			//find a data in bucket equal with the value I want to insert
	{				//so replace it
		if (q)
		{
			q->next = data;
		} 
		else
		{
			htable->buckets[data->hash_value % htable->bucket_szie] = data;
		}
		data->next = p->next;
		xfree(p);	//release the data
	} 
	else
	{	//if canot find the data, put data in the head of bucket
		TakeLock(htable->num_lock);	//LOCK number
		if (htable->key_num >= htable->max)
		{
			unTakeLock(htable->num_lock);
			unTakeLock(hashlock);	//UnLock
			return 1;	//hashtable is full
		}
		htable->key_num++;
		unTakeLock(htable->num_lock);

		data->next = htable->buckets[data->hash_value % htable->bucket_szie];
		htable->buckets[data->hash_value % htable->bucket_szie] = data;
	}
	unTakeLock(hashlock);	//UnLock

	return 0;
}

data_t* hashtable_get( hashtable_t* htable,const char* key,int keySize )
{
	size_t hashValue;
	data_t* pBucket;
	HANDLE hashlock;

	if (htable->key_num <= 0)
	{
		return NULL;
	}

	hashValue = PMurHash32(0,key,keySize);
	hashlock = htable->bucket_locks[hashValue % htable->bucket_szie];

	TakeLock(hashlock);	//Lock
	pBucket = htable->buckets[hashValue % htable->bucket_szie];


	while(pBucket && 0 != CmpKey(pBucket->key, pBucket->key_len, key, keySize))
	{
		pBucket = pBucket->next;
	}
	unTakeLock(hashlock);	//UnLock

	if (!pBucket)
	{
		return NULL;
	}

	return pBucket;

}

void hashtable_relasedata( hashtable_t* htable )
{
	int i,count = 0;
	data_t* data,*temp;

	for (i=0; i<htable->bucket_szie; i++)
	{
		data = htable->buckets[i];
		while(data)
		{
			temp = data->next;
			xfree(data);
			data = temp;
			count++;
		}
	}
	printf("release data : %d\n",count);
}

void hashtable_writehead( hashtable_t* htable )
{
	int ret;

	buffer_clear(htable->buf);
	buffer_putint(htable->buf,htable->id);
	buffer_putint(htable->buf,htable->key_num);
	buffer_putint(htable->buf,htable->max);
	buffer_putint(htable->buf,htable->bucket_szie);

	__INFO("buffer NUL:%d",htable->buf->NUL);

	fseek(htable->file,0,SEEK_SET);
	ret = (int)fwrite(buffer_detach(htable->buf),1,htable->buf->NUL,htable->file);
	fflush(htable->file);

	__INFO("hashtable:flush hashtable head : %s, NUL:%d, ret:%d ",htable->filename,htable->buf->NUL,ret);

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
				ret = (int)fwrite(buffer_detach(htable->buf),htable->buf->NUL,1,htable->file);
				buffer_clear(htable->buf);
			}
			data = data->next;
		}
	}

	ret = (int)fwrite(buffer_detach(htable->buf),htable->buf->NUL,1,htable->file);
	fflush(htable->file);
	__INFO("hashtable:flush data : %s, NUL:%d, number:%d ",htable->filename,htable->buf->NUL,htable->key_num);
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

		if (htable->index + 1>= htable->bucket_szie)
		{
			return NULL;
		}
		htable->index++;
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
	size_t ret;
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

void merge_no_sentinel(data_t** sstDataTable, int iStartKeyNo, int iMiddleKeyNo, int iEndKeyNo)
{
	int iLeftKeyLoop, iRightKeyLoop, iSourceKeyLoop;

	int iLeftLength = iMiddleKeyNo - iStartKeyNo + 1;
	int iRightLength = iEndKeyNo - iMiddleKeyNo;
	data_t** oData_LeftKeys = (data_t**)xmalloc(sizeof(data_t*)*(iLeftLength));
	data_t** oData_RightKeys = (data_t**)xmalloc(sizeof(data_t*)*(iRightLength));
	//将即将要比较的两部分分别存放到oData_LeftKeys和oData_RightKeys中
	for (iLeftKeyLoop = 0 ; iLeftKeyLoop < iLeftLength ;iLeftKeyLoop ++)
	{
		oData_LeftKeys[iLeftKeyLoop] = sstDataTable[iStartKeyNo + iLeftKeyLoop] ;
	}

	for (iRightKeyLoop = 0 ; iRightKeyLoop < iRightLength ; iRightKeyLoop ++)
	{
		oData_RightKeys[iRightKeyLoop] = sstDataTable[iMiddleKeyNo + 1 + iRightKeyLoop];
	}

	iLeftKeyLoop = iRightKeyLoop = 0;
	iSourceKeyLoop = iStartKeyNo;

	while(iLeftKeyLoop < iLeftLength  && iRightKeyLoop < iRightLength )
	{
		if(Comparator(*oData_LeftKeys[iLeftKeyLoop], *oData_RightKeys[iRightKeyLoop]) < 1)//compare  LeftData with  RightData unfinished
		{
			sstDataTable[iStartKeyNo++] = oData_LeftKeys[iLeftKeyLoop++];//将比较小的存到源数据中
		}
		else
		{
			sstDataTable[iStartKeyNo++] = oData_RightKeys[iRightKeyLoop++];
		}
	}

	if (iLeftKeyLoop < iLeftLength)//将左边或者右边的剩余有序部分以此拷贝到源数据中
	{
		for(; iLeftKeyLoop < iLeftLength ; iLeftKeyLoop++)
		{
			sstDataTable[iStartKeyNo++] = oData_LeftKeys[iLeftKeyLoop];
		}
	}else{
		for(; iRightKeyLoop < iRightLength ; iRightKeyLoop++)
		{
			sstDataTable[iStartKeyNo++] = oData_RightKeys[iRightKeyLoop];
		}
	}
	free(oData_LeftKeys);//释放资源
	free(oData_RightKeys);
	oData_LeftKeys = NULL;
	oData_RightKeys = NULL;
}

void merge_sort(data_t** sstDataTable,int iStartKeyNo,int iEndKeyNo)
{
	int iMiddleKeyNo=-1;
	if( iStartKeyNo < iEndKeyNo )
	{
		iMiddleKeyNo = (int)((iStartKeyNo+iEndKeyNo)/2);
		merge_sort(sstDataTable, iStartKeyNo, iMiddleKeyNo);
		merge_sort(sstDataTable, iMiddleKeyNo+1, iEndKeyNo);
		merge_no_sentinel(sstDataTable,iStartKeyNo, iMiddleKeyNo, iEndKeyNo);
	}
}

void hashtable_sort( hashtable_t* htable )
{
	int i;
	int count = 0;
	data_t* data;

	htable->datas = (data_t**)xmalloc(htable->key_num * sizeof(data_t*));
	memset(htable->datas,0,htable->key_num * sizeof(data_t*));

	for (i=0; i<htable->key_num; i++)
	{
		data = hashtable_nextdata(htable);
		if(data)
			htable->datas[count++] = data;
	}
	merge_sort(htable->datas,0,count-1);
}