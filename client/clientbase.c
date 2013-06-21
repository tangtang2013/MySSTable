#include "clientbase.h"

void onConnect(uv_connect_t* req, int status);
void onWriteCallBack(uv_write_t *req, int status);
uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size);
void echo_read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf);

stClientBase* CreateClientBase(char* strIP, int nPort)
{
	stClientBase* pClient = (stClientBase*)malloc(sizeof(stClientBase));
	pClient->nPort = nPort;

	memset(pClient->strIP, 0, 32);
	memcpy(pClient->strIP, strIP, strlen(strIP));

	pClient->nBufferMaxSize = 1024;
	pClient->pBuffer = (char*)malloc(pClient->nBufferMaxSize);

	return pClient;
}

void InitClientBase(stClientBase* pClient)
{
	int nRet;
	WSADATA data;
	uv_tcp_t* pServer;
	
	nRet = WSAStartup(MAKEWORD(2,2),&data);
	
	pClient->uvLoop = uv_default_loop();
	uv_tcp_init(pClient->uvLoop, &pClient->uvSocket);

	pServer = (uv_tcp_t*)&pClient->uvConnect;
//	pServer->handler = pClient;
}

void Connect(stClientBase* pClient)
{
	int nRet;
	struct sockaddr_in dest;
	dest = uv_ip4_addr(pClient->strIP, pClient->nPort);

	nRet = uv_tcp_connect(&pClient->uvConnect, &pClient->uvSocket, dest, onConnect);
	uv_run(pClient->uvLoop,UV_RUN_DEFAULT);
}

void DisConnect(stClientBase* pClient)
{
	uv_close(pClient->uvConnect.handle, NULL);
}

void DestroyClientBase(stClientBase* pClient)
{
	if(pClient->pBuffer)
		free(pClient->pBuffer);

	uv_loop_delete(pClient->uvLoop);

	if(pClient)
		free(pClient);
}

void SendBase(stClientBase* pClient, char* pBuffer, int nSize)
{
	int ret;
	int index = 0;
	uv_tcp_t* pServer;

	fd_set rfds;
	struct timeval tv = {2,0};

    write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));

    req->buf = uv_buf_init((char*) malloc(nSize), nSize);
    memcpy(req->buf.base, pBuffer, nSize);

	ret = uv_write((uv_write_t*) req, (uv_stream_t*)pClient->uvConnect.handle, &req->buf, 1, onWriteCallBack);

	//Recv
	pServer = (uv_tcp_t*)pClient->uvConnect.handle;

	FD_ZERO(&rfds);
	FD_SET(pServer->socket,&rfds);
	if(select(pServer->socket+1, &rfds, NULL, NULL,&tv) == -1)
	{
		//NETWORK error
		fprintf(stderr,"select error code : [%d]\n",GetLastError());
	}

	while(1)
	{
		ret = recv(pServer->socket,pClient->pBuffer + index, pClient->nBufferMaxSize, 0);
		if(ret > 0)
			index += ret;
		else if(ret == 0)
			break;
		else if(ret == -1)
			break;
	}
	//fprintf(stderr,"error code : [%d]\n",GetLastError());
	pClient->nBufferSize = index;
}

void onConnect(uv_connect_t* req, int status)
{

}

void onWriteCallBack(uv_write_t *req, int status)
{
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}