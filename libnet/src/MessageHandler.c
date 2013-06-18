#include <stdio.h>
#include "MessageHandler.h"
#include "Message.h"

void* MsgHandler_callback(char* pInBuffer, int nInBufferSize)
{
	eMsgType eType;
	int nOutBufferSize = 11;
	uv_buf_t* uvBuf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));

	uvBuf->base = pInBuffer;
	uvBuf->len = nInBufferSize;

	memcpy(&eType, pInBuffer, sizeof(eMsgType));
	switch(eType)
	{
	case PUT_REQUEST:
		ParseMsgPutRequestBuf(uvBuf);
		break;
	case GET_REQUEST:
		ParseMsgGetRequestBuf(uvBuf);
		break;
	}

	req->buf = uv_buf_init((char*) malloc(nOutBufferSize), nOutBufferSize);
	memcpy(req->buf.base, "hello kitty", nOutBufferSize);
	
	return req;
}