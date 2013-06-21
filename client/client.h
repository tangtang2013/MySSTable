#ifndef _LIBNET_CLIENT_H_
#define _LIBNET_CLIENT_H_

#include <stdio.h>
#include <common.h>
#include "clientbase.h"

typedef struct client_t
{
	stClientBase* pClientBase;
}stClient;

stClient* CreateClient(char* strIP, int nPort);
void InitClient(stClient* pClient);
//void Connect(stClient* pClient);
//void DisConnect(stClient* pClient);
void DeleteClient(stClient* pClient);

int Put(stClient* pClient, char* pKey, int nKeySize, char* pValue, int nValueSize);
data_t* Get(stClient* pClient, char* pKey, int nKeySize);
int Delete(stClient* pClient, char* pKey, int nKeySize);

#endif