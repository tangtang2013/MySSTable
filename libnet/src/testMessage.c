#include <stdio.h>
#include "Message.h"

void TestMsgPutReply()
{
	stMsgPutReply* pMsg;
	stMsgPutReply* pTmp;
	char* pBuf;

	pMsg = CreateMsgPutReplySt(0);
	pBuf = CreateMsgPutReplyBuf(pMsg);
	pTmp = ParseMsgPutReplyBuf(pBuf);

	free(pBuf);
	free(pMsg);
	free(pTmp);
	printf("TestMsgPutReply Successed!!! \n");
}

void TestMsgPutRequest()
{
	stMsgPutRequest* pMsg;
	stMsgPutRequest* pTmp;
	char* pBuf;

	pMsg = CreateMsgPutRequestSt("huang",5,"juntang",7);
	pBuf = CreateMsgPutRequestBuf(pMsg);
	pTmp = ParseMsgPutRequestBuf(pBuf);

	free(pBuf);
	free(pMsg);
	free(pTmp);
	printf("TestMsgPutReply Successed!!! \n");
}

void TestMsgGetReply()
{
	stMsgGetReply* pMsg;
	stMsgGetReply* pTmp;
	char* pBuf;
	int size;
	data_t* pData = create_data("huang",5,"juntang",7,'s');
	//data_t* pData = NULL;

	size = sizeof(stMsgGetReply);
	pMsg = CreateMsgGetReplySt(0,pData);
	pBuf = CreateMsgGetReplyBuf(pMsg);

	pTmp = ParseMsgGetReplyBuf(pBuf);

	free(pData);
	free(pBuf);
	free(pMsg);
	free(pTmp);
	printf("TestMsgGetReply Successed!!! \n");
}

void TestMsgGetRequest()
{
	stMsgGetRequest* pMsg;
	stMsgGetRequest* pTmp;
	char* pBuf;

	pMsg = CreateMsgGetRequestSt("huang",5);
	pBuf = CreateMsgGetRequestBuf(pMsg);
	pTmp = ParseMsgGetRequestBuf(pBuf);

	free(pBuf);
	free(pMsg);
	free(pTmp);
	printf("TestMsgGetRequest Successed!!! \n");
}

int main()
{
	TestMsgPutReply();
	TestMsgPutRequest();

	TestMsgGetReply();
	TestMsgGetRequest();
}