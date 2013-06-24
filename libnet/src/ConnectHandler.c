#include <stdio.h>
#include "ConnectHandler.h"

write_req_t* MsgHandler_Default(char* pInBuffer, int nInBufferSize);

stConnectHandler* InitConnectHandler(int nThreadNum, stServer* server, MsgHandler_cb handler_cb)
{
	stConnectHandler* pConnectHandler = (stConnectHandler*)malloc(sizeof(stConnectHandler));
	
	pConnectHandler->nThreadNum = nThreadNum;
	pConnectHandler->pServer = server;

	if(handler_cb != NULL)
		pConnectHandler->funcMsgHandler = handler_cb;
	else
		pConnectHandler->funcMsgHandler = MsgHandler_Default;

	pConnectHandler->pHandlers = (stHandler*)malloc(nThreadNum * sizeof(stHandler));
	memset(pConnectHandler->pHandlers, 0, nThreadNum * sizeof(stHandler));

	pConnectHandler->pWorkList = NULL;
	pConnectHandler->nWorkNum = 0;

	pConnectHandler->loop = uv_loop_new();
	uv_mutex_init(&pConnectHandler->uvMutex);
	uv_cond_init(&pConnectHandler->uvCond);

	return pConnectHandler;
}

void StartHandler(stConnectHandler* pConnectHandler)
{
	int i;

	uv_mutex_lock(&pConnectHandler->uvMutex);
	pConnectHandler->bIsRunning = TRUE;
	uv_mutex_unlock(&pConnectHandler->uvMutex);

	for(i=0; i<pConnectHandler->nThreadNum; i++)
	{
		uv_thread_create(&pConnectHandler->pHandlers[i].thread,ThreadFunc,pConnectHandler);
	}
	uv_run(pConnectHandler->loop, UV_RUN_DEFAULT);
}

void StopHandler(stConnectHandler* pConnectHandler)
{
	uv_mutex_lock(&pConnectHandler->uvMutex);
	pConnectHandler->bIsRunning = FALSE;
	uv_mutex_unlock(&pConnectHandler->uvMutex);
}

void DestroyHandler(stConnectHandler* pConnectHandler)
{
	int i;
	uv_cond_broadcast(&pConnectHandler->uvCond);

	for(i=0; i<pConnectHandler->nThreadNum; i++)
	{
		uv_thread_join(&pConnectHandler->pHandlers[i].thread);
	}
	//TODO
	DeleteWorkList(pConnectHandler);
	uv_mutex_destroy(&pConnectHandler->uvMutex);
	uv_cond_destroy(&pConnectHandler->uvCond);
}

void AddWork(stConnectHandler* pConnectHandler, uv_stream_t* client, char* pBuffer, int nBufferLength)
{
	stWork* pWork;
	stWork* pHead;
	pWork = (stWork*)malloc(sizeof(stWork) + nBufferLength + 1);
	memset(pWork, 0, sizeof(stWork) + nBufferLength);
	
	pWork->client = client;
	pWork->nBufferLen = nBufferLength;
	pWork->pBuffer = pWork->addr + 1;
	pWork->pNext = NULL;
	memcpy(pWork->addr + 1, pBuffer, nBufferLength);

	uv_mutex_lock(&pConnectHandler->uvMutex);
	if(pConnectHandler->pWorkList != NULL || pConnectHandler->nWorkNum > 0)
	{
		pHead = pConnectHandler->pWorkList;
		while(pHead->pNext != NULL)
		{
			pHead = pHead->pNext;
		}
		pHead->pNext = pWork;
		//pWork = gpWorkList;
		pConnectHandler->nWorkNum++;
		//gpWorkList = gpWorkList->pNext;
	}
	else
	{
		pConnectHandler->pWorkList = pWork;
		pConnectHandler->nWorkNum++;
	}
	uv_mutex_unlock(&pConnectHandler->uvMutex);
	uv_cond_signal(&pConnectHandler->uvCond);
}

stWork* GetWork(stConnectHandler* pConnectHandler)
{
	stWork* pWork = NULL;
	//uv_mutex_lock(&gMutex);
	//TODO
	if(pConnectHandler->pWorkList != NULL || pConnectHandler->nWorkNum > 0)
	{
		pWork = pConnectHandler->pWorkList;
		pConnectHandler->nWorkNum--;
		pConnectHandler->pWorkList = pConnectHandler->pWorkList->pNext;
	}
	//uv_mutex_unlock(&gMutex);

	return pWork;
}

void DeleteWorkList(stConnectHandler* pConnectHandler)
{
	stWork* pWork = NULL;
	uv_mutex_lock(&pConnectHandler->uvMutex);
	//TODO
	while(pConnectHandler->pWorkList != NULL || pConnectHandler->nWorkNum > 0)
	{
		pWork = pConnectHandler->pWorkList;
		pConnectHandler->nWorkNum--;
		DeleteWork(pWork);
		pWork = NULL;
		pConnectHandler->pWorkList = pConnectHandler->pWorkList->pNext;
	}
	uv_mutex_unlock(&pConnectHandler->uvMutex);
}

void ThreadFunc(void* pParam)
{
	stWork* pWork;
	stConnectHandler* pConnectHandler = (stConnectHandler*)pParam;

	while(pConnectHandler->bIsRunning)
	{
		//TODO
		uv_mutex_lock(&pConnectHandler->uvMutex);
		
		if(!pConnectHandler->bIsRunning)
		{
			uv_mutex_unlock(&pConnectHandler->uvMutex);
			ExitThread(999);
			return;//(pthread_exit())
		}
		while(pConnectHandler->bIsRunning && pConnectHandler->nWorkNum == 0)
		{
			uv_cond_wait(&pConnectHandler->uvCond, &pConnectHandler->uvMutex);
		}
		if(!pConnectHandler->bIsRunning)
		{
			uv_mutex_unlock(&pConnectHandler->uvMutex);
			ExitThread(999);
			return;//(pthread_exit())
		}
		pWork = GetWork(pConnectHandler);
		uv_mutex_unlock(&pConnectHandler->uvMutex);

		//TODO(work handler)
		Handler(pConnectHandler, pWork);
		DeleteWork(pWork);
		pWork = NULL;
	}
}

void DeleteWork(stWork* pWork)
{
	//uv_close((uv_handle_t*)pWork->client, NULL);
	free(pWork);
	//fprintf(stderr,"DeleteWork\n");
}

void write_cb(uv_write_t *req, int status) {
	write_req_t *wr = (write_req_t*) req;
	free(wr->buf.base);
	free(wr);
}

void Handler(stConnectHandler* pConnectHandler, stWork* pWork)
{
	int ret;
	int len = 0;
	int pos = 0;
	int nBufferSize = 0;
	char* pBuffer = NULL;

	fd_set rfds;
	struct timeval tv = {2,0};

    write_req_t *req;
	uv_tcp_t* pClient = (uv_tcp_t*)pWork->client;

	req = (*pConnectHandler->funcMsgHandler)(pWork->pBuffer, pWork->nBufferLen);
	//fprintf(stderr,"addr: %x size[%d]\n",pWork->client,pWork->client->write_queue_size);
	//ret = uv_write((uv_write_t*)req, (uv_stream_t*)pWork->client, &req->buf, 1, write_cb);
	//on_file_write(req,1);
	//fprintf(stderr,"uv_write ret : %d\n",ret);

	len = req->buf.len;
	while(len > 0)
	{
		ret = send(pClient->socket, req->buf.base + pos, len, 0);
		if(ret > 0)
		{
			len -= ret;
			pos += ret;
		}
		else if(ret == 0)
		{
			break;
		}
		else
		{
			//TODO
			FD_ZERO(&rfds);
			FD_SET(pClient->socket,&rfds);
			if(select(pClient->socket+1, &rfds, NULL, NULL,NULL) == -1)
			{
				//NETWORK error
				fprintf(stderr,"select error code : [%d]\n",GetLastError());
			}
		}
	}

	//FREE
	free(req->buf.base);
	free(req);
}

write_req_t* MsgHandler_Default(char* pInBuffer, int nInBufferSize)
{
	int nOutBufferSize = 12;
	write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));

	req->buf = uv_buf_init((char*) malloc(nOutBufferSize), nOutBufferSize);
	memcpy(req->buf.base, "hello world", nOutBufferSize);
	
	return req;
}