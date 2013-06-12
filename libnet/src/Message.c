#include <assert.h>
#include <string.h>
#include "Message.h"

stMsgPutRequest* CreateMsgPutRequestSt(char* pKey, int nKeySize, char* pValue, int nValueSize)
{
	stMsgPutRequest* pMsg = (stMsgPutRequest*)malloc(sizeof(stMsgPutRequest) + nKeySize + nValueSize);

	pMsg->type = PUT_REQUEST;
	pMsg->nKeySize = nKeySize;
	pMsg->nValueSize = nValueSize;

	memcpy(pMsg->buf+1, pKey, nKeySize);
	memcpy(pMsg->buf+1+nKeySize, pValue, nValueSize);

	pMsg->pKey = pMsg->buf+1;
	pMsg->pValue = pMsg->buf+1+nKeySize;

	return pMsg;
}

void* CreateMsgPutRequestBuf(stMsgPutRequest* pMsgPut)
{
	int nBufferSize;
	char* pBuffer;
	int offset = 0;
	int len = 0;
	nBufferSize = 3 * sizeof(int) + pMsgPut->nKeySize + pMsgPut->nValueSize;
	
	pBuffer = malloc(nBufferSize);
	memset(pBuffer,0,nBufferSize);
	
	len = sizeof(pMsgPut->type);
	memcpy(pBuffer + offset, &pMsgPut->type, len);
	offset += len;

	len = sizeof(int);
	memcpy(pBuffer + offset, &pMsgPut->nKeySize, len);
	offset += len;

	len = sizeof(int);
	memcpy(pBuffer + offset, &pMsgPut->nValueSize, len);
	offset += len;

	memcpy(pBuffer + offset, pMsgPut->pKey, pMsgPut->nKeySize);
	offset += pMsgPut->nKeySize;

	memcpy(pBuffer + offset, pMsgPut->pValue, pMsgPut->nValueSize);
	offset += pMsgPut->nValueSize;

	assert(offset == nBufferSize);
	return pBuffer;
}

stMsgPutRequest* ParseMsgPutRequestBuf(char* pBuf)
{
	int offset = 0;
	int nKeySize = 0;
	int nValueSize = 0;
	eMsgType type;
	stMsgPutRequest* pMsgPut;

	memcpy(&type, pBuf + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == PUT_REQUEST);

	memcpy(&nKeySize, pBuf + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&nValueSize, pBuf + offset, sizeof(int));
	offset += sizeof(int);

	pMsgPut = malloc(sizeof(stMsgPutRequest) + nKeySize + nValueSize);
	pMsgPut->type = type;
	pMsgPut->nKeySize = nKeySize;
	pMsgPut->nValueSize = nValueSize;

	memcpy(pMsgPut->buf + 1, pBuf + offset, pMsgPut->nKeySize);
	offset += pMsgPut->nKeySize;

	memcpy(pMsgPut->buf + 1 + pMsgPut->nKeySize, pBuf + offset, pMsgPut->nValueSize);
	offset += pMsgPut->nValueSize;

	pMsgPut->pKey = pMsgPut->buf + 1;
	pMsgPut->pValue = pMsgPut->buf + 1 + pMsgPut->nKeySize;

	return pMsgPut;
}

stMsgPutReply* CreateMsgPutReplySt( int nRet )
{
	stMsgPutReply *pMsg = (stMsgPutReply*)malloc(sizeof(stMsgPutReply));

	pMsg->type = PUT_REPLY;
	pMsg->nRet = nRet;

	return pMsg;
}

void* CreateMsgPutReplyBuf( stMsgPutReply* pMsgPut )
{
	int nBufferSize;
	char* pBuffer;
	int offset = 0;
	int len = 0;
	nBufferSize = 2 * sizeof(int);

	pBuffer = malloc(nBufferSize);
	memset(pBuffer,0,nBufferSize);

	len = sizeof(pMsgPut->type);
	memcpy(pBuffer + offset, &pMsgPut->type, len);
	offset += len;

	len = sizeof(int);
	memcpy(pBuffer + offset, &pMsgPut->nRet, len);
	offset += len;

	assert(offset == nBufferSize);
	return pBuffer;
}

stMsgPutReply* ParseMsgPutReplyBuf( char* pBuf )
{
	int offset = 0;
	int nRet = 0;
	eMsgType type;
	stMsgPutReply* pMsgPut = malloc(sizeof(stMsgPutReply));

	memcpy(&type, pBuf + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == PUT_REPLY);

	memcpy(&nRet, pBuf + offset, sizeof(int));
	offset += sizeof(int);

	pMsgPut->type = type;
	pMsgPut->nRet = nRet;

	return pMsgPut;
}

stMsgGetRequest* CreateMsgGetRequestSt( char* pKey, int nKeySize )
{
	stMsgGetRequest* pMsg = (stMsgGetRequest*)malloc(sizeof(stMsgPutRequest) + nKeySize);

	pMsg->type = GET_REQUEST;
	pMsg->nKeySize = nKeySize;

	memcpy(pMsg->buf+1, pKey, nKeySize);

	pMsg->pKey = pMsg->buf+1;

	return pMsg;
}

void* CreateMsgGetRequestBuf( stMsgGetRequest* pMsgGet )
{
	int nBufferSize;
	char* pBuffer;
	int offset = 0;
	int len = 0;
	nBufferSize = 3 * sizeof(int) + pMsgGet->nKeySize;

	pBuffer = malloc(nBufferSize);
	memset(pBuffer,0,nBufferSize);

	len = sizeof(pMsgGet->type);
	memcpy(pBuffer + offset, &pMsgGet->type, len);
	offset += len;

	len = sizeof(int);
	memcpy(pBuffer + offset, &pMsgGet->nKeySize, len);
	offset += len;

	memcpy(pBuffer + offset, pMsgGet->pKey, pMsgGet->nKeySize);
	offset += pMsgGet->nKeySize;

	assert(offset == nBufferSize);
	return pBuffer;
}

stMsgGetRequest* ParseMsgGetRequestBuf( char* pBuf )
{
	int offset = 0;
	int nKeySize = 0;
	eMsgType type;
	stMsgGetRequest* pMsgGet;

	memcpy(&type, pBuf + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == GET_REQUEST);

	memcpy(&nKeySize, pBuf + offset, sizeof(int));
	offset += sizeof(int);

	pMsgGet = malloc(sizeof(stMsgGetRequest) + nKeySize);
	pMsgGet->type = type;
	pMsgGet->nKeySize = nKeySize;

	memcpy(pMsgGet->buf + 1, pBuf + offset, pMsgGet->nKeySize);
	offset += pMsgGet->nKeySize;

	pMsgGet->pKey = pMsgGet->buf + 1;

	return pMsgGet;
}

stMsgGetReply* CreateMsgGetReplySt( int nRet, data_t* pData )
{
	stMsgGetReply* pMsg = NULL;
	if (pData != NULL)
	{
		pMsg= (stMsgGetReply*)malloc(sizeof(stMsgGetReply) + sizeof(data_t) + pData->key_len + pData->value_len);
		pMsg->type = GET_REPLY;
		pMsg->nRet = nRet;
		//TODO
		pMsg->pData = pMsg + sizeof(stMsgGetReply);

		pMsg->pData->type = pData->type;
		pMsg->pData->key_len = pData->key_len;
		pMsg->pData->value_len = pData->value_len;
		pMsg->pData->version = pData->version;
		pMsg->pData->hash_value = pData->hash_value;

		pMsg->pData->key = pMsg->pData->addr + 1;
		pMsg->pData->value = pMsg->pData->addr + 1 + pMsg->pData->key_len;
		memcpy(pMsg->pData->key,pData->key,pData->key_len);
		memcpy(pMsg->pData->value,pData->value,pData->value_len);
	} 
	else
	{
		pMsg= (stMsgGetReply*)malloc(sizeof(stMsgGetReply));
		pMsg->type = GET_REPLY;
		pMsg->nRet = nRet;
		pMsg->pData = NULL;
	}
	return pMsg;
}

void* CreateMsgGetReplyBuf( stMsgGetReply* pMsgPut )
{

}

stMsgGetReply* ParseMsgGetReplyBuf( char* pBuf )
{

}
