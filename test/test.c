#include <stdio.h>
#include "debug.h"
#include "sstmanager.h"

#define BUFFER_SIZE 1024

int main()
{
	sstmanager_t* manager;
    data_t* data;
    int i = 0;
    char key[16];
    char value[32];
	buffer_t* buf = buffer_new(1024);

	manager = sstmanager_new();
	sstmanager_open(manager);
    
	for(i=0; i<2000; i++)
	{
		sprintf(key,"key%06d",i);
		data = create_data(key,strlen(key),key,strlen(key),'s');
		sstmanager_put(manager,*data);
		__INFO("put:%s",key);
		xfree(data); 
	}

    for(i=0; i<2000; i++)
    {
        sprintf(key,"key%06d",i);
        data = sstmanager_get(manager,key);
        if(data)
        __INFO("key:%s value:%s type:%c version:%lu\n",data->key,data->value,data->type,data->version); 
    }
    
	sstmanager_flush(manager);
	sstmanager_close(manager);
}