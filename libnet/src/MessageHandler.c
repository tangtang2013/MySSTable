#include <stdio.h>
#include <assert.h>
#include "MessageHandler.h"
#include "Message.h"

void InitMsgHandler()
{
	pManager = sstmanager_new();
	sstmanager_open(pManager);
}

void CloseMsgHandler()
{
	sstmanager_flush(pManager);
	sstmanager_close(pManager);
}

void* MsgHandler_callback(char* pInBuffer, int nInBufferSize)
{
	eMsgType eType;

	uv_buf_t* uvTmp;
	uv_buf_t* uvBuf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));

	uvBuf->base = pInBuffer;
	uvBuf->len = nInBufferSize;

	memcpy(&eType, pInBuffer, sizeof(eMsgType));
	switch(eType)
	{
	case PUT_REQUEST:
		uvTmp = PutRequestHandler(uvBuf);
		break;
	case GET_REQUEST:
		uvTmp = GetRequestHandler(uvBuf);
		break;
	default:
		fprintf(stderr,"error type\n");
		uvTmp = malloc(sizeof(uv_buf_t));
		uvTmp->base = (char*) malloc(11);
		uvTmp->len = 11;
		memcpy(uvTmp->base, "hello kitty", uvTmp->len);
		break;
	}

	//req->buf = uv_buf_init((char*) malloc(nOutBufferSize), nOutBufferSize);
	//memcpy(req->buf.base, "hello kitty", nOutBufferSize);
	req->buf.base = uvTmp->base;
	req->buf.len = uvTmp->len;

	free(uvBuf);
	free(uvTmp);
	
	return req;
}

void* PutRequestHandler(uv_buf_t* uvBuf)
{
	int nRet;
	uv_buf_t* uvRetBuf;
	data_t* pData;
	stMsgPutRequest* pRequest = ParseMsgPutRequestBuf(uvBuf);
	stMsgPutReply * pReply;

	assert(PUT_REQUEST == pRequest->type);

	pData = create_data(pRequest->pKey,pRequest->nKeySize,pRequest->pValue,pRequest->nValueSize,pRequest->type);
	nRet = sstmanager_put(pManager, pData);

	pReply = CreateMsgPutReplySt(nRet);
	uvRetBuf = CreateMsgPutReplyBuf(pReply);

	free(pReply);
	free(pRequest);

	return uvRetBuf;
}

void* GetRequestHandler(uv_buf_t* uvBuf)
{
	int nRet;
	char* pRetBuffer;
	data_t* pData;
	stMsgGetRequest* pRequest = ParseMsgGetRequestBuf(uvBuf);
	stMsgGetReply * pReply;

	assert(GET_REQUEST == pRequest->type);

	pData = sstmanager_get(pManager, pRequest->pKey, pRequest->nKeySize);

	pReply = CreateMsgGetReplySt(0, pData);			//test ...
	pRetBuffer = CreateMsgGetReplyBuf(pReply);

//	free(pData);
	free(pReply);
	free(pRequest);

	return pRetBuffer;
}