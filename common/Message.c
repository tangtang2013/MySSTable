#include <assert.h>
#include <string.h>
//#include "Server.h"
#include "Message.h"

//Put---***Request
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
	uv_buf_t* uvBuf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	int offset = 0;
	int len = 0;
	uvBuf->len = 3 * sizeof(int) + pMsgPut->nKeySize + pMsgPut->nValueSize;
	
	uvBuf->base = malloc(uvBuf->len);
	memset(uvBuf->base,0,uvBuf->len);
	
	len = sizeof(pMsgPut->type);
	memcpy(uvBuf->base + offset, &pMsgPut->type, len);
	offset += len;

	len = sizeof(int);
	memcpy(uvBuf->base + offset, &pMsgPut->nKeySize, len);
	offset += len;

	len = sizeof(int);
	memcpy(uvBuf->base + offset, &pMsgPut->nValueSize, len);
	offset += len;

	memcpy(uvBuf->base + offset, pMsgPut->pKey, pMsgPut->nKeySize);
	offset += pMsgPut->nKeySize;

	memcpy(uvBuf->base + offset, pMsgPut->pValue, pMsgPut->nValueSize);
	offset += pMsgPut->nValueSize;

	assert(offset == uvBuf->len);
	return uvBuf;
}

stMsgPutRequest* ParseMsgPutRequestBuf(uv_buf_t* uvBuf)
{
	int offset = 0;
	int nKeySize = 0;
	int nValueSize = 0;
	eMsgType type;
	stMsgPutRequest* pMsgPut;

	memcpy(&type, uvBuf->base + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == PUT_REQUEST);

	memcpy(&nKeySize, uvBuf->base + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&nValueSize, uvBuf->base + offset, sizeof(int));
	offset += sizeof(int);

	pMsgPut = malloc(sizeof(stMsgPutRequest) + nKeySize + nValueSize);
	pMsgPut->type = type;
	pMsgPut->nKeySize = nKeySize;
	pMsgPut->nValueSize = nValueSize;

	memcpy(pMsgPut->buf + 1, uvBuf->base + offset, pMsgPut->nKeySize);
	offset += pMsgPut->nKeySize;

	memcpy(pMsgPut->buf + 1 + pMsgPut->nKeySize, uvBuf->base + offset, pMsgPut->nValueSize);
	offset += pMsgPut->nValueSize;

	pMsgPut->pKey = pMsgPut->buf + 1;
	pMsgPut->pValue = pMsgPut->buf + 1 + pMsgPut->nKeySize;

	assert(offset == uvBuf->len);

	return pMsgPut;
}

//Put---***Reply
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
	uv_buf_t* uvBuf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	int offset = 0;
	int len = 0;
	nBufferSize = 2 * sizeof(int);

	uvBuf->len = nBufferSize;
	uvBuf->base = malloc(nBufferSize);
	memset(uvBuf->base,0,uvBuf->len);

	len = sizeof(pMsgPut->type);
	memcpy(uvBuf->base + offset, &pMsgPut->type, len);
	offset += len;

	len = sizeof(int);
	memcpy(uvBuf->base + offset, &pMsgPut->nRet, len);
	offset += len;

	assert(offset == uvBuf->len);
	return uvBuf;
}

stMsgPutReply* ParseMsgPutReplyBuf( uv_buf_t* uvBuf )
{
	int offset = 0;
	int nRet = 0;
	eMsgType type;
	stMsgPutReply* pMsgPut;

	if(uvBuf->len <= 0)
	{
		return NULL;
	}

	pMsgPut = malloc(sizeof(stMsgPutReply));

	memcpy(&type, uvBuf->base + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == PUT_REPLY);

	memcpy(&nRet, uvBuf->base + offset, sizeof(int));
	offset += sizeof(int);

	pMsgPut->type = type;
	pMsgPut->nRet = nRet;

	assert(offset == uvBuf->len);

	return pMsgPut;
}

//Get---***Request
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
	uv_buf_t* uvBuf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	int offset = 0;
	int len = 0;
	uvBuf->len = 2 * sizeof(int) + pMsgGet->nKeySize;

	uvBuf->base= malloc(uvBuf->len);
	memset(uvBuf->base,0,uvBuf->len);

	len = sizeof(pMsgGet->type);
	memcpy(uvBuf->base + offset, &pMsgGet->type, len);
	offset += len;

	len = sizeof(int);
	memcpy(uvBuf->base + offset, &pMsgGet->nKeySize, len);
	offset += len;

	memcpy(uvBuf->base + offset, pMsgGet->pKey, pMsgGet->nKeySize);
	offset += pMsgGet->nKeySize;

	assert(offset == uvBuf->len);
	return uvBuf;
}

stMsgGetRequest* ParseMsgGetRequestBuf( uv_buf_t* uvBuf )
{
	int offset = 0;
	int nKeySize = 0;
	eMsgType type;
	stMsgGetRequest* pMsgGet;

	memcpy(&type, uvBuf->base + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == GET_REQUEST);

	memcpy(&nKeySize, uvBuf->base + offset, sizeof(int));
	offset += sizeof(int);

	pMsgGet = malloc(sizeof(stMsgGetRequest) + nKeySize);
	pMsgGet->type = type;
	pMsgGet->nKeySize = nKeySize;

	memcpy(pMsgGet->buf + 1, uvBuf->base + offset, pMsgGet->nKeySize);
	offset += pMsgGet->nKeySize;

	pMsgGet->pKey = pMsgGet->buf + 1;

	assert(offset == uvBuf->len);

	return pMsgGet;
}

//Get---***Reply
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
	uv_buf_t* uvBuf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	int offset = 0;
	int len = 0;

	if (pMsgGet->pData == NULL)
	{
		uvBuf->len = 2 * sizeof(int);
		uvBuf->base = malloc(uvBuf->len);

		len = sizeof(pMsgGet->type);
		memcpy(uvBuf->base + offset, &pMsgGet->type, len);
		offset += len;

		len = sizeof(int);
		memcpy(uvBuf->base + offset, &pMsgGet->nRet, len);
		offset += len;
	} 
	else
	{
		uvBuf->len = 4 * sizeof(int) + 2 * sizeof(unsigned long long) + pMsgGet->pData->key_len + pMsgGet->pData->value_len;

		uvBuf->base = malloc(uvBuf->len);
		memset(uvBuf->base,0,uvBuf->len);

		len = sizeof(pMsgGet->type);								//write msg type
		memcpy(uvBuf->base + offset, &pMsgGet->type, len);
		offset += len;

		len = sizeof(int);											//write msg nRet
		memcpy(uvBuf->base + offset, &pMsgGet->nRet, len);
		offset += len;

		len = sizeof(int);											//write data info : key length
		memcpy(uvBuf->base + offset, &pMsgGet->pData->key_len, len);
		offset += len;

		len = sizeof(int);											//write data info : value length
		memcpy(uvBuf->base + offset, &pMsgGet->pData->value_len, len);
		offset += len;

		len = sizeof(unsigned long long);							//write data info : version
		memcpy(uvBuf->base + offset, &pMsgGet->pData->version, len);
		offset += len;

		len = sizeof(unsigned long long);							//write data info : expire time
		memcpy(uvBuf->base + offset, &pMsgGet->pData->expireTime, len);
		offset += len;

		memcpy(uvBuf->base + offset, pMsgGet->pData->key, pMsgGet->pData->key_len);
		offset += pMsgGet->pData->key_len;							//write data info : key buffer

		memcpy(uvBuf->base + offset, pMsgGet->pData->value, pMsgGet->pData->value_len);
		offset += pMsgGet->pData->value_len;						//write data info : value buffer

		assert(offset == uvBuf->len);
	}

	return uvBuf;
}

stMsgGetReply* ParseMsgGetReplyBuf( uv_buf_t* uvBuf )
{
	int offset = 0;
	int nRet = 0;
	int nKeySize = 0;
	int nValueSize = 0;
	eMsgType type;
	stMsgGetReply* pMsgGet;

	if(uvBuf->len <= 0)
	{
		return NULL;
	}

	memcpy(&type, uvBuf->base + offset, sizeof(eMsgType));
	offset += sizeof(eMsgType);
	assert(type == GET_REPLY);

	memcpy(&nRet, uvBuf->base + offset, sizeof(int));
	offset += sizeof(int);

	if (nRet == 0)
	{
		memcpy(&nKeySize, uvBuf->base + offset, sizeof(int));
		offset += sizeof(int);

		memcpy(&nValueSize, uvBuf->base + offset, sizeof(int));
		offset += sizeof(int);

		pMsgGet = malloc(sizeof(stMsgGetReply) + sizeof(data_t) + nKeySize + nValueSize);
		pMsgGet->type = type;
		pMsgGet->nRet = nRet;

		pMsgGet->pData = (char*)pMsgGet + sizeof(stMsgGetReply);
		pMsgGet->pData->key_len = nKeySize;
		pMsgGet->pData->value_len = nValueSize;

		memcpy(&pMsgGet->pData->version, uvBuf->base + offset, sizeof(unsigned long long));
		offset += sizeof(unsigned long long);

		memcpy(&pMsgGet->pData->expireTime, uvBuf->base + offset, sizeof(unsigned long long));
		offset += sizeof(unsigned long long);

		memcpy(pMsgGet->pData->addr + 1, uvBuf->base + offset, nKeySize);
		offset += nKeySize;

		memcpy(pMsgGet->pData->addr + 1 + nKeySize, uvBuf->base + offset, nValueSize);
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

	assert(offset == uvBuf->len);

	return pMsgGet;
}

void* MsgHandler(char* pInBuffer, int nInBufferSize)
{
	int nOutBufferSize = 11;
	write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));

	req->buf = uv_buf_init((char*) malloc(nOutBufferSize), nOutBufferSize);
	memcpy(req->buf.base, "hello kitty", nOutBufferSize);
	
	return req;
}