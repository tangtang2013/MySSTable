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

