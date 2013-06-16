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

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

//call back
typedef write_req_t* (*MsgHandler_cb)(char* pInBuffer, int nInBufferSize);
MsgHandler_cb funcMsgHandler;

stServer* ln_ServerCreate(char* strIP, int nPort);
int ln_ServerInit(stServer* pServer, MsgHandler_cb handler_cb);
void ln_ServerStart(stServer* pServer);
void ln_ServerStop(stServer* pServer);
void ln_ServerDestroy(stServer* pServer);

void on_file_write(uv_write_t *req, int status);
void write_data(uv_stream_t *dest, size_t size, uv_buf_t buf, uv_write_cb callback);

#endif