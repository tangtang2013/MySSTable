#ifndef _LIBNET_MESSAGE_H_
#define _LIBNET_MESSAGE_H_

#include <common.h>

typedef struct msg_put_request
{
	eMsgType type;
	int nKeySize;
	char* pKey;
	int nValueSize;
	char* pValue;
	char buf[1];
}stMsgPutRequest;

typedef struct msg_put_reply
{
	eMsgType type;
	int nRet;
}stMsgPutReply;

//put...request
stMsgPutRequest* CreateMsgPutRequestSt(char* pKey, int nKeySize, char* pValue, int nValueSize);
void* CreateMsgPutRequestBuf(stMsgPutRequest* pMsgPut);
stMsgPutRequest* ParseMsgPutRequestBuf(uv_buf_t* uvBuf);

//put...reply
stMsgPutReply* CreateMsgPutReplySt(int nRet);
void* CreateMsgPutReplyBuf(stMsgPutReply* pMsgPut);
stMsgPutReply* ParseMsgPutReplyBuf(uv_buf_t* uvBuf);

typedef struct msg_get_request
{
	eMsgType type;
	int nKeySize;
	char* pKey;

	char buf[1];
}stMsgGetRequest;

typedef struct msg_get_reply
{
	eMsgType type;
	int nRet;
	data_t* pData;
}stMsgGetReply;

//get...request
stMsgGetRequest* CreateMsgGetRequestSt(char* pKey, int nKeySize);
void* CreateMsgGetRequestBuf(stMsgGetRequest* pMsgGet);
stMsgGetRequest* ParseMsgGetRequestBuf(uv_buf_t* uvBuf);

//get...reply
stMsgGetReply* CreateMsgGetReplySt(int nRet, data_t* pData);
void* CreateMsgGetReplyBuf(stMsgGetReply* pMsgGet);
stMsgGetReply* ParseMsgGetReplyBuf(uv_buf_t* uvBuf);

//call back
//typedef void (*MsgHandler_cb)(char* pInBuffer, int nInBufferSize, char* pOutBuffer, int nOutBufferSize);
//MsgHandler_cb funcMsgHandler;
void* MsgHandler(char* pInBuffer, int nInBufferSize);

#endif