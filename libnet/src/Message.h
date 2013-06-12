#ifndef _LIBNET_MESSAGE_H_
#define _LIBNET_MESSAGE_H_

#include "common.h"

//enum for massage type
typedef enum msg_type
{
	CONNECTING,
	CLOSECONNECT,
	PUT_REQUEST,
	PUT_REPLY,
	GET_REQUEST,
	GET_REPLY,
	DEL
}eMsgType;

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
stMsgPutRequest* ParseMsgPutRequestBuf(char* pBuf);

//put...reply
stMsgPutReply* CreateMsgPutReplySt(int nRet);
void* CreateMsgPutReplyBuf(stMsgPutReply* pMsgPut);
stMsgPutReply* ParseMsgPutReplyBuf(char* pBuf);

typedef struct msg_get_request
{
	eMsgType type;
	int nKeySize;
	char* pKey;
	int nValueSize;
	char* pValue;
	char buf[1];
}stMsgGetRequest;

typedef struct msg_get_reply
{
	eMsgType type;
	int nRet;
}stMsgGetReply;

//get...request
stMsgPutRequest* CreateMsgGetRequestSt(char* pKey, int nKeySize);
void* CreateMsgGetRequestBuf(stMsgPutRequest* pMsgPut);
stMsgPutRequest* ParseMsgGetRequestBuf(char* pBuf);

//get...reply
stMsgPutReply* CreateMsgGetReplySt(int nRet);
void* CreateMsgGetReplyBuf(stMsgPutReply* pMsgPut);
stMsgPutReply* ParseMsgGetReplyBuf(char* pBuf);

#endif