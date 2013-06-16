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
	nBufferSize = 2 * sizeof(int) + pMsgGet->nKeySize;

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
	int length = 0;
	if (pData != NULL)
	{
		pMsg= (stMsgGetReply*)malloc(sizeof(stMsgGetReply) + sizeof(data_t) + pData->key_len + pData->value_len);
		pMsg->type = GET_REPLY;
		pMsg->nRet = nRet;
		//TODO
		length = sizeof(stMsgGetReply);
		pMsg->pData = (char*)pMsg + length;

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

void* CreateMsgGetReplyBuf( stMsgGetReply* pMsgGet )
{
	int nBufferSize;
	char* pBuffer;
	int offset = 0;
	int len = 0;

	if (pMsgGet->pData == NULL)
	{
		nBufferSize = 2 * sizeof(int);
		pBuffer = malloc(nBufferSize);

		len = sizeof(pMsgGet->type);
		memcpy(pBuffer + offset, &pMsgGet->type, len);
		offset += len;

		len = sizeof(int);
		memcpy(pBuffer + offset, &pMsgGet->nRet, len);
		offset += len;
	} 
	else
	{
		nBufferSize = 4 * sizeof(int) + 2 * sizeof(unsigned long long) + pMsgGet->pData->key_len + pMsgGet->pData->value_len;

		pBuffer = malloc(nBufferSize);
		memset(pBuffer,0,nBufferSize);

		len = sizeof(pMsgGet->type);								//write msg type
		memcpy(pBuffer + offset, &pMsgGet->type, len);
		offset += len;

		len = sizeof(int);											//write msg nRet
		memcpy(pBuffer + offset, &pMsgGet->nRet, len);
		offset += len;

		len = sizeof(int);											//write data info : key length
		memcpy(pBuffer + offset, &pMsgGet->pData->key_len, len);
		offset += len;

		len = sizeof(int);											//write data info : value length
		memcpy(pBuffer + offset, &pMsgGet->pData->value_len, len);
		offset += len;

		len = sizeof(unsigned long long);							//write data info : version
		memcpy(pBuffer + offset, &pMsgGet->pData->version, len);
		offset += len;

		len = sizeof(unsigned long long);							//write data info : expire time
		memcpy(pBuffer + offset, &pMsgGet->pData->expireTime, len);
		offset += len;

		memcpy(pBuffer + offset, pMsgGet->pData->key, pMsgGet->pData->key_len);
		offset += pMsgGet->pData->key_len;							//write data info : key buffer

		memcpy(pBuffer + offset, pMsgGet->pData->value, pMsgGet->pData->value_len);
		offset += pMsgGet->pData->value_len;						//write data info : value buffer

		assert(offset == nBufferSize);
	}

	return pBuffer;
}

stMsgGetReply* ParseMsgGetReplyBuf( char* pBuf )
{
	int offset = 0;
	int nRet = 0;
	int nKeySize = 0;
	int nValueSize = 0;
	eMsgType type;
	stMsgGetReply* pMsgGet;

	memcpy(&type, pBuf + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == GET_REPLY);

	memcpy(&nRet, pBuf + offset, sizeof(int));
	offset += sizeof(int);

	if (nRet == 0)
	{
		memcpy(&nKeySize, pBuf + offset, sizeof(int));
		offset += sizeof(int);

		memcpy(&nValueSize, pBuf + offset, sizeof(int));
		offset += sizeof(int);

		pMsgGet = malloc(sizeof(stMsgGetReply) + sizeof(data_t) + nKeySize + nValueSize);
		pMsgGet->type = type;
		pMsgGet->nRet = nRet;

		pMsgGet->pData = (char*)pMsgGet + sizeof(stMsgGetReply);
		pMsgGet->pData->key_len = nKeySize;
		pMsgGet->pData->value_len = nValueSize;

		memcpy(&pMsgGet->pData->version, pBuf + offset, sizeof(unsigned long long));
		offset += sizeof(unsigned long long);

		memcpy(&pMsgGet->pData->expireTime, pBuf + offset, sizeof(unsigned long long));
		offset += sizeof(unsigned long long);

		memcpy(pMsgGet->pData->addr + 1, pBuf + offset, nKeySize);
		offset += nKeySize;

		memcpy(pMsgGet->pData->addr + 1 + nKeySize, pBuf + offset, nValueSize);
		offset += nValueSize;

		pMsgGet->pData->key = pMsgGet->pData->addr + 1;
		pMsgGet->pData->value = pMsgGet->pData->addr + 1 + nKeySize;
	} 
	else
	{
		pMsgGet = malloc(sizeof(stMsgGetReply));
		pMsgGet->type = type;
		pMsgGet->nRet = nRet;
	}

	return pMsgGet;
}

write_req_t* MsgHandler(char* pInBuffer, int nInBufferSize)
{
	int nOutBufferSize = 11;
	write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));

	req->buf = uv_buf_init((char*) malloc(nOutBufferSize), nOutBufferSize);
	memcpy(req->buf.base, "hello kitty", nOutBufferSize);
	
	return req;
}