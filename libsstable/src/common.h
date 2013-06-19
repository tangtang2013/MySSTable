#ifndef __SSTABLE_COMMON_H_
#define __SSTABLE_COMMON_H_

#include <uv.h>

#define FILE_MAX_PATH 260

//*****************************  Net *******************************//

//enum for massage type(socket)
typedef enum msg_type
{
	CONNECTING,
	CLOSECONNECT,
	PUT_REQUEST,
	PUT_REPLY,
	GET_REQUEST,
	GET_REPLY,
	DEL
}eMsgType;

//write structure
typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

//Server structure
typedef struct ln_Server
{
//	uv_loop_t* loop;
	uv_tcp_t server;
	struct sockaddr_in bind_addr;

	void* pConnhandler;	//(runtime cast...)

	char strIp[32];
	int	nPort;
}stServer;

//call back
typedef write_req_t* (*MsgHandler_cb)(char* pInBuffer, int nInBufferSize);

//*****************************  Net *******************************//

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
	unsigned long long expireTime;
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