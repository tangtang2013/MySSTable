#ifndef _LIBNET_CONNECTHANDLER_H_
#define _LIBNET_CONNECTHANDLER_H_

#include <common.h>
//#include "Server.h"

//Work mutex lock
uv_mutex_t gMutex;
//Work Condition
uv_cond_t gCond;

typedef struct ln_Work
{
	uv_stream_t* client;
	char* pBuffer;
	int nBufferLen;
	struct In_Work* pNext;
	char addr[1];
}stWork;

typedef struct ln_Handler
{
//	stServer* server;
	uv_thread_t thread;
	void (*func)(void*);
}stHandler;

int gnThreadNum;
stHandler* gpHandlers;

stWork* gpWorkList;
int gnWorkNum;

stServer* gServer;
BOOL bIsRunning;

MsgHandler_cb funcMsgHandler;

//Handler
void InitConnectHandler(int nThreadNum, stServer* server, MsgHandler_cb handler_cb);
void StartHandler();
void StopHandler();
void DestroyHandler();

//Worker
void AddWork(uv_stream_t* client, char* pBuffer, int nBufferLength);
stWork* GetWork();
void DeleteWork(stWork* pWork);
void DeleteWorkList();

void ThreadFunc(void* pParam);
void Handler(stWork* pWork);

#endif