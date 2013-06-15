#include <stdio.h>
#include "ConnectHandler.h"

void InitConnectHandler(int nThreadNum, stServer* server)
{
	gnThreadNum = nThreadNum;
	gServer = server;

	gpHandlers = (stHandler*)malloc(nThreadNum * sizeof(stHandler));
	memset(gpHandlers, 0, nThreadNum * sizeof(stHandler));

	gpWorkList = NULL;
	gnWorkNum = 0;
	uv_mutex_init(&gMutex);
	uv_cond_init(&gCond);
}

void StartHandler()
{
	int i;

	uv_mutex_lock(&gMutex);
	bIsRunning = TRUE;
	uv_mutex_unlock(&gMutex);

	for(i=0; i<gnThreadNum; i++)
	{
		uv_thread_create(&gpHandlers[i].thread,ThreadFunc,gpWorkList);
	}
}

void StopHandler()
{
	int i;
	
	uv_mutex_lock(&gMutex);
	bIsRunning = FALSE;
	uv_mutex_unlock(&gMutex);
	uv_cond_broadcast(&gCond);

	for(i=0; i<gnThreadNum; i++)
	{
		uv_thread_join(&gpHandlers[i].thread);
	}
}

void DestroyHandler()
{
	//TODO
	DeleteWorkList();
	uv_mutex_destroy(&gMutex);
	uv_cond_destroy(&gCond);
}

void AddWork(uv_stream_t* client, char* pBuffer, int nBufferLength)
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

	uv_mutex_lock(&gMutex);
	if(gpWorkList != NULL || gnWorkNum > 0)
	{
		pHead = gpWorkList;
		while(pHead->pNext != NULL)
		{
			pHead = pHead->pNext;
		}
		pHead->pNext = pWork;
		pWork = gpWorkList;
		gnWorkNum++;
		gpWorkList = gpWorkList->pNext;
	}
	else
	{
		gpWorkList = pWork;
		gnWorkNum++;
	}
	uv_mutex_unlock(&gMutex);
	uv_cond_signal(&gCond);
}

stWork* GetWork()
{
	stWork* pWork = NULL;
	//uv_mutex_lock(&gMutex);
	//TODO
	if(gpWorkList != NULL || gnWorkNum > 0)
	{
		pWork = gpWorkList;
		gnWorkNum--;
		gpWorkList = gpWorkList->pNext;
	}
	//uv_mutex_unlock(&gMutex);

	return pWork;
}

void DeleteWorkList()
{
	stWork* pWork = NULL;
	uv_mutex_lock(&gMutex);
	//TODO
	while(gpWorkList != NULL || gnWorkNum > 0)
	{
		pWork = gpWorkList;
		gnWorkNum--;
		free(pWork);
		pWork = NULL;
		gpWorkList = gpWorkList->pNext;
	}
	uv_mutex_unlock(&gMutex);
}


void ThreadFunc(void* pParam)
{
	stWork* pWork;

	while(bIsRunning)
	{
		//TODO
		uv_mutex_lock(&gMutex);
		
		if(!bIsRunning)
		{
			uv_mutex_unlock(&gMutex);
			return;//(pthread_exit())
		}
		while(gpWorkList == NULL || gnWorkNum == 0)
		{
			uv_cond_wait(&gCond, &gMutex);
			//uv_mutex_unlock(&gMutex);
			//uv_mutex_lock(&gMutex);
		}
		pWork = GetWork();
		uv_mutex_unlock(&gMutex);

		//TODO(work handler)
		fprintf(stderr,"-%s\n",pWork->pBuffer);
		free(pWork);
		pWork = NULL;

		uv_mutex_unlock(&gMutex);
	}
}