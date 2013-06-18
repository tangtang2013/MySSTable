#include "hashtable.h"
#include <stdio.h>

DWORD WINAPI thread_put0(void* arg)
{
	int i;
	data_t* data;
	char key[16];
	for(i=0; i<65536; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		hashtable_put((hashtable_t*)arg,data);
	}
}

DWORD WINAPI thread_put1(void* arg)
{
	int i;
	data_t* data;
	char key[16];
	for(i=0; i<65536; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		hashtable_put((hashtable_t*)arg,data);
	}
}

DWORD WINAPI thread_put2(void* arg)
{
	int i;
	data_t* data;
	char key[16];
	for(i=0; i<65536; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		hashtable_put((hashtable_t*)arg,data);
	}
}

DWORD WINAPI thread_get0(void* arg)
{
	int i,count = 0;
	data_t* data;
	char key[16];
	for(i=0; i<65536; i++)
	{
		sprintf(key,"key%06d",i);
		data = hashtable_get((hashtable_t*)arg,key);
		if (data && strcmp(key,data->key) == 0)
		{
			count++;
		}
	}
	printf("thread00-get : %d\n",count);
}

DWORD WINAPI thread_get1(void* arg)
{
	int i,count = 0;
	data_t* data;
	char key[16];
	for(i=0; i<65536; i++)
	{
		sprintf(key,"key%06d",i);
		data = hashtable_get((hashtable_t*)arg,key);
		if (data && strcmp(key,data->key) == 0)
		{
			count++;
		}
	}
	printf("thread01-get : %d\n",count);
}

DWORD WINAPI thread_get2(void* arg)
{
	int i,count = 0;
	data_t* data;
	char key[16];
	for(i=0; i<65536; i++)
	{
		sprintf(key,"key%06d",i);
		data = hashtable_get((hashtable_t*)arg,key);
		if (data && strcmp(key,data->key) == 0)
		{
			count++;
		}
	}
	printf("thread02-get : %d\n",count);
}

int main()
{
	data_t* data;
	int i = 0;
	int count = 0;

	time_t now,start;

	char key[16];
	char value[32];

	HANDLE handle[3];

	hashtable_t* htable = hashtable_new(1);
//	hashtable_build(htable);
	hashtable_open(htable);

	start = time(NULL);

	//put thread...
// 	handle[0] = CreateThread(NULL,0,thread_put0,htable,0,NULL);
// 	handle[1] = CreateThread(NULL,0,thread_put1,htable,0,NULL);
// 	handle[2] = CreateThread(NULL,0,thread_put2,htable,0,NULL);
// 
// 	WaitForMultipleObjects(3,handle,1,INFINITE);
// 
// 	CloseHandle(handle[0]);
// 	CloseHandle(handle[1]);
// 	CloseHandle(handle[2]);
// 
// 	now = time(NULL);
// 	printf("use : %d\n",now - start);
// 
 	//get thread...
	handle[0] = CreateThread(NULL,0,thread_get0,htable,0,NULL);
	handle[1] = CreateThread(NULL,0,thread_get1,htable,0,NULL);
	handle[2] = CreateThread(NULL,0,thread_get2,htable,0,NULL);

	WaitForMultipleObjects(3,handle,1,INFINITE);

	CloseHandle(handle[0]);
	CloseHandle(handle[1]);
	CloseHandle(handle[2]);

	now = time(NULL);
	printf("use : %d\n",now - start);
	printf("success %d\n",count);

	hashtable_get(htable,"key068590");
	
	count = 0;
	hashtable_sort(htable);

	for (i=0; i<htable->bucket_szie;i++)
	{
	//	printf("%s %u\n",htable->datas[i]->key,htable->datas[i]->hash_value);
		data = htable->buckets[i];
		while(data)
		{
			printf("%s %u\n",data->key,data->hash_value);
			data = data->next;
		}
	}

	printf("use : %d\n",now - start);
	printf("success %d\n",count);

	hashtable_flush(htable);
	hashtable_relasedata(htable);
	hashtable_free(htable);
}