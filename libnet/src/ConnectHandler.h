#ifndef _LIBNET_CONNECTHANDLER_H_
#define _LIBNET_CONNECTHANDLER_H_

#include <common.h>
//#include "Server.h"

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

//---
typedef struct ln_ConnectHandler
{
	uv_loop_t* loop;
	int nThreadNum;
	stHandler* pHandlers;

	stWork* pWorkList;
	int nWorkNum;

	stServer* pServer;
	BOOL bIsRunning;

	MsgHandler_cb funcMsgHandler;

	//Work mutex lock
	uv_mutex_t uvMutex;
	//Work Condition
	uv_cond_t uvCond;
}stConnectHandler;

//Handler
stConnectHandler* InitConnectHandler(int nThreadNum, stServer* server, MsgHandler_cb handler_cb);
void StartHandler(stConnectHandler* pConnectHandler);
void StopHandler(stConnectHandler* pConnectHandler);
void DestroyHandler(stConnectHandler* pConnectHandler);

//Worker
void AddWork(stConnectHandler* pConnectHandler, uv_stream_t* client, char* pBuffer, int nBufferLength);
stWork* GetWork(stConnectHandler* pConnectHandler);
void DeleteWork(stWork* pWork);
void DeleteWorkList(stConnectHandler* pConnectHandler);

void ThreadFunc(void* pParam);
void Handler(stConnectHandler* pConnectHandler, stWork* pWork);

#endif