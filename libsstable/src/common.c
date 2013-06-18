#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "PMurHash.h"

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
	data_t* clone;
	if (data == NULL)
	{
		return NULL;
	}
	clone = (data_t*)xmalloc(sizeof(data_t) + data->key_len + data->value_len + 2);
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

int CmpKey(const char* pKey1,int nKeySize1, const char* pKey2,int nKeySize2)
{
	int minlen = (nKeySize1 < nKeySize2) ? nKeySize1 : nKeySize2;
	int ret = memcmp(pKey1, pKey2, minlen);

	if (ret == 0)
	{
		if (nKeySize1 < nKeySize2)
		{
			ret = -1;
		}
		else if (nKeySize1 > nKeySize2)
		{
			ret = +1;
		}
	}

	return ret;
}

int Comparator( const data_t first, const data_t second )
{
	int ret;

	if (first.hash_value > second.hash_value)
	{
		ret = 1;
	} 
	else if (first.hash_value == second.hash_value)
	{
		ret = CmpKey(first.key, first.key_len, second.key, second.key_len);
	}
	else
	{
		ret = -1;
	}

	return ret;
}

int ComparatorB( unsigned long firsthash, const char* firstkey, unsigned long secondhash, const char* secondkey )
{
	int ret;

	if (firsthash > secondhash)
	{
		ret = 1;
	} 
	else if (firsthash == secondhash)
	{
		ret = CmpKey(firstkey, strlen(firstkey), secondkey, strlen(secondkey));
	}
	else
	{
		ret = -1;
	}

	return ret;
}

/*********************************************************
 * @name:	ComparatorC
 * @func:	use to find a smaller data in compact...
 * @in:		const data_t*, const data_t*
 * @ret:	int
 *********************************************************/
int ComparatorC( const data_t* first, const data_t* second )
{
	int ret;

	if (first == NULL && second == NULL)
	{
		return 0;
	}
	else if(first == NULL && second != NULL)//in my opinion : NULL data -the data is smaller
	{
		return 1;
	}
	else if(first != NULL && second == NULL)
	{
		return -1;
	}

	if (first->hash_value > second->hash_value)
	{
		ret = 1;
	} 
	else if (first->hash_value == second->hash_value)
	{
		ret = CmpKey(first->key, first->key_len, second->key, second->key_len);
	}
	else
	{
		ret = -1;
	}

	return ret;
}

void TakeLock(HANDLE lck)
{
	DWORD ret = WaitForSingleObject(lck, INFINITE);
	if(ret != WAIT_OBJECT_0) {
		assert(0);
	}
}

void unTakeLock(HANDLE lck)
{
	if(ReleaseMutex(lck) == 0) {
		assert(0);
	}
}

int tryTakeLock( HANDLE lck )
{
	switch(WaitForSingleObject(lck, 0)) {
		case WAIT_OBJECT_0:
			return 1;
		case WAIT_TIMEOUT:
			return 0;
		default:
			break;
	}
	assert(0);
}