#include "client.h"

stClient* CreateClient(char* strIP, int nPort)
{
	stClient* pClient = (stClient*)malloc(sizeof(stClient));
	pClient->pClientBase = CreateClientBase(strIP, nPort);

	return pClient;
}

void InitClient(stClient* pClient)
{
	InitClientBase(pClient->pClientBase);
	Connect(pClient->pClientBase);
}

void DeleteClient(stClient* pClient)
{
	DisConnect(pClient->pClientBase);
	DestroyClientBase(pClient->pClientBase);
	free(pClient);
}

int Put(stClient* pClient, char* pKey, int nKeySize, char* pValue, int nValueSize)
{
	int nRet;
	stMsgPutRequest* pMsgRequest;
	stMsgPutReply* pMsgReply;
	uv_buf_t* uvBuf;

	pMsgRequest = CreateMsgPutRequestSt(pKey, nKeySize, pValue, nValueSize);
	uvBuf = CreateMsgPutRequestBuf(pMsgRequest);

	SendBase(pClient->pClientBase, uvBuf->base, uvBuf->len);

	free(uvBuf->base);
	uvBuf->base = pClient->pClientBase->pBuffer;
	uvBuf->len = pClient->pClientBase->nBufferSize;

	pMsgReply = ParseMsgPutReplyBuf(uvBuf);

	if(pMsgReply)
		nRet = pMsgReply->nRet;
	else
		nRet = -1;	//

	free(uvBuf);
	free(pMsgReply);
	free(pMsgRequest);

	return nRet;
}

data_t* Get(stClient* pClient, char* pKey, int nKeySize)
{
	data_t* pRet;
	stMsgGetRequest* pMsgRequest;
	stMsgGetReply* pMsgReply;
	uv_buf_t* uvBuf;

	pMsgRequest = CreateMsgGetRequestSt(pKey, nKeySize);
	uvBuf = CreateMsgGetRequestBuf(pMsgRequest);

	SendBase(pClient->pClientBase, uvBuf->base, uvBuf->len);

	free(uvBuf->base);
	uvBuf->base = pClient->pClientBase->pBuffer;
	uvBuf->len = pClient->pClientBase->nBufferSize;

	pMsgReply = ParseMsgGetReplyBuf(uvBuf);
	//pRet = pMsgReply->pData;
	pRet = clone_data(pMsgReply->pData);

	free(uvBuf);
	free(pMsgReply);
	free(pMsgRequest);

	return pRet;
}

int Delete(stClient* pClient, char* pKey, int nKeySize)
{
	return -1;
}