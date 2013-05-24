#ifndef __SSTABLE_COMMON_H_
#define __SSTABLE_COMMON_H_

#include <Windows.h>

#define FILE_MAX_PATH 260

typedef struct data_item
{
	int key_len;
	unsigned long hash_value;		//the hash value of key
	char* key;

	int value_len;
	char* value;
	
	struct data_item* next;			//point 
	char type;  // 's' and 'd':d is invalid data, s is valid data
	unsigned long long version;		//data version
	char addr[1];
}data_t;

void* create_data(const char* key, int key_len, const char* value, int value_len, char type);
void* clone_data(data_t* data);

int CmpKey(const char* pKey1,int nKeySize1, const char* pKey2,int nKeySize2);
int Comparator( const data_t first, const data_t second );
int ComparatorB( unsigned long firsthash, const char* firstkey, unsigned long secondhash, const char* secondkey );
int ComparatorC( const data_t* first, const data_t* second );

int SMakeDir(const char* path);

void TakeLock(HANDLE lck);
void unTakeLock(HANDLE lck);
int tryTakeLock(HANDLE lck);

#endif