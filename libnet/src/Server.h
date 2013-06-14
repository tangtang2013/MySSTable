#ifndef _LIBNET_SERVER_H_
#define _LIBNET_SERVER_H_

#include <uv.h>

typedef struct ln_Server
{
//	uv_loop_t* loop;
	uv_tcp_t server;
	struct sockaddr_in bind_addr;

	char strIp[32];
	int	nPort;
}stServer;

stServer* ln_ServerCreate(char* strIP, int nPort);
int ln_ServerInit(stServer* pServer);
void ln_ServerStart(stServer* pServer);
void ln_ServerStop(stServer* pServer);
void ln_ServerDestroy(stServer* pServer);

#endif