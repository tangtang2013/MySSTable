#ifndef _LIBNET_CLIENTBASE_H_
#define _LIBNET_CLIENTBASE_H_

#include <stdio.h>
#include <common.h>
#include <message.h>

typedef struct client_base
{
	uv_loop_t* uvLoop;
	uv_tcp_t uvSocket;
	uv_connect_t uvConnect;

	int nPort;
	char strIP[32];

	char* pBuffer;
	int nBufferSize;
	int nBufferMaxSize;

	void* pHandler;
}stClientBase;

stClientBase* CreateClientBase(char* strIP, int nPort);
void InitClientBase(stClientBase* pClient);
void Connect(stClientBase* pClient);
void DisConnect(stClientBase* pClient);
void DestroyClientBase(stClientBase* pClient);
void SendBase(stClientBase* pClient, char* pBuffer, int nSize);

#endif