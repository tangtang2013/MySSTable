#include "common.h"
#include "PMurHash.h"
#include <stdio.h>

void* create_data(const char* key, int key_len, const char* value, int value_len, char type)
{
	data_t* data = (data_t*)xmalloc(sizeof(data_t) + key_len + value_len + 2);
	memset(data,0,sizeof(*data));
	data->type = type;
	data->key_len = key_len;
	data->value_len = value_len;

	data->key = data->addr + 1;
	data->value = data->addr + 2 + data->key_len;
	memcpy(data->key,key,data->key_len);
	memcpy(data->value,value,data->value_len);
	data->addr[data->key_len + 1] = 0;
	data->addr[data->key_len + data->value_len + 2] = 0;
	data->version = _time64(NULL);

	data->hash_value = PMurHash32(0,data->key,data->key_len);

	return data;   
}

void* clone_data(data_t* data)
{
	data_t* clone = (data_t*)xmalloc(sizeof(data_t) + data->key_len + data->value_len + 2);
	//	memcpy(clone,data,sizeof(data_t) + data->key_len + data->value_len + 2);
	memset(clone,0,sizeof(data_t) + data->key_len + data->value_len + 2);

	clone->type = data->type;
	clone->key_len = data->key_len;
	clone->value_len = data->value_len;
	clone->version = data->version;
	clone->hash_value = data->hash_value;

	clone->key = clone->addr + 1;
	clone->value = clone->addr + 2 + clone->key_len;
	memcpy(clone->key,data->key,data->key_len);
	memcpy(clone->value,data->value,data->value_len);
	clone->addr[clone->key_len + 1] = 0;
	clone->addr[clone->key_len + clone->value_len + 2] = 0;

	return clone;    
}