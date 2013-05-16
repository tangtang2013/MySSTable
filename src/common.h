#ifndef __SSTABLE_COMMON_H_
#define __SSTABLE_COMMON_H_

typedef struct data_item
{
	int key_len;
	unsigned long hash_value;
	char* key;

	int value_len;
	char* value;

	char type;  // 's' and 'd'    
	unsigned long long version;
	char addr[1];
}data_t;

void* create_data(const char* key, int key_len, const char* value, int value_len, char type);
void* clone_data(data_t* data);

int SMakeDir(const char* path);


#endif