#include <stdio.h>
#include "debug.h"
#include "sstmanager.h"

#define BUFFER_SIZE 1024

int main()
{
	bloom_filter* bfilter,*filter;
	sstmanager_t* manager;
    data_t* data;
    int i = 0;
	int filter_present = 0;
	
    char key[16];
    char value[32];
	buffer_t* buf = buffer_new(1024);
	int test[1024];
	int f[1024];
/*bfilter = create_bfilter(1024*4);
	srand(time(NULL));
	for(i = 0; i < 1024; i++)
	{
		f[i] = rand();
		bfilter_add(bfilter, &f[i]);

		// Fill our test array with a 50/50 mix of numbers that have been entered into the filter, and random numbers
		if(rand() & 1)
			test[i] = f[i];
		else
			test[i] = rand();
	}

	for(i = 0; i < 1024; i++)
	{
		filter_present += bfilter_check(bfilter, &test[i]);
	}
	printf("%d\n",filter_present);

	buffer_putfilter(buf,bfilter);

	destroy_bfilter(bfilter);
	

	filter = buffer_getfilter(buf);

	for(i = 0; i < 1024; i++)
	{
		filter_present += bfilter_check(bfilter, &test[i]);
	}
	printf("%d\n",filter_present);

	return 0;*/

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