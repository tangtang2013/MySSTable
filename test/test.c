#include <stdio.h>
#include "debug.h"
#include "sstmanager.h"

#define BUFFER_SIZE 1024

DWORD WINAPI thread_put0(void* arg)
{
	int i;
	data_t* data;
	char key[16];
	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		sstmanager_put((sstmanager_t*)arg,data);
	}
}

DWORD WINAPI thread_put1(void* arg)
{
	int i;
	data_t* data;
	char key[16];
	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		sstmanager_put((sstmanager_t*)arg,data);
	}
}

DWORD WINAPI thread_put2(void* arg)
{
	int i;
	data_t* data;
	char key[16];
	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		sstmanager_put((sstmanager_t*)arg,data);
	}
}

DWORD WINAPI thread_get0(void* arg)
{
	int i,count = 0;
	data_t* data;
	char key[16];
	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = sstmanager_get((sstmanager_t*)arg,key);
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
	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = sstmanager_get((sstmanager_t*)arg,key);
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
	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = sstmanager_get((sstmanager_t*)arg,key);
		if (data && strcmp(key,data->key) == 0)
		{
			count++;
		}
	}
	printf("thread02-get : %d\n",count);
}

int main()
{
	sstmanager_t* manager;
    data_t* data;
    int i = 0;
	int count = 0;

	time_t now,start;

	const int n = 200000; //loop num
	
    char key[16];
    char value[32];
	buffer_t* buf = buffer_new(1024);
	
	HANDLE handle[3];

	manager = sstmanager_new();
	sstmanager_open(manager);

	start = time(NULL);
    
	for(i=0; i < n; i++)
	{
		sprintf(key,"key%06d",i);
		data = (data_t*)create_data(key,strlen(key),key,strlen(key),'s');
		sstmanager_put(manager,data);
		if (data != NULL)
		{
			count++;
		}
	}
	//put thread...
// 	handle[0] = CreateThread(NULL,0,thread_put0,manager,0,NULL);
// 	handle[1] = CreateThread(NULL,0,thread_put1,manager,0,NULL);
// 	handle[2] = CreateThread(NULL,0,thread_put2,manager,0,NULL);

// 	WaitForMultipleObjects(3,handle,1,INFINITE);
// 
// 	CloseHandle(handle[0]);
// 	CloseHandle(handle[1]);
// 	CloseHandle(handle[2]);

	now = time(NULL);
	printf("success put %d\n",count);
	printf("use : %d\n",now - start);


	count = 0;

	for(i=0; i < n; i++)
	{
		sprintf(key,"key%06d",i);
		data = sstmanager_get(manager,key);
		if (data)
		{
			count++;
		}
	}

	now = time(NULL);
	printf("success get %d\n",count);
	printf("use : %d\n",now - start);
	
//	sstmanager_compact(manager,0,2);
	now = time(NULL);
	printf("use : %d\n",now - start);

	sstmanager_flush(manager);
	sstmanager_close(manager);
}