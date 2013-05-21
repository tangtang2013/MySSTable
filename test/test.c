#include <stdio.h>
#include "debug.h"
#include "sstmanager.h"

#define BUFFER_SIZE 1024

int main()
{
	sstmanager_t* manager;
    data_t* data;
    int i = 0;
	int count = 0;

	time_t now,start;
	
    char key[16];
    char value[32];
	buffer_t* buf = buffer_new(1024);

	manager = sstmanager_new();
	sstmanager_open(manager);

	start = time(NULL);
    
	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		sstmanager_put(manager,data);
	}

	now = time(NULL);
	printf("use : %d\n",now - start);

	for(i=0; i<200000; i++)
	{
		sprintf(key,"key%06d",i);
		data = sstmanager_get(manager,key);
		if (data)
		{
			count++;
		}
	}

	now = time(NULL);
	printf("success %d\n",count);
	printf("use : %d\n",now - start);
	
	sstmanager_compact(manager,0,2);
	now = time(NULL);
	printf("use : %d\n",now - start);

	sstmanager_flush(manager);
	sstmanager_close(manager);
}