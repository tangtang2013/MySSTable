#include <stdio.h>
#include <common.h>
#include "clientbase.h"

int main()
{
	int ret;
	uv_tcp_t socket;
	uv_connect_t connect;
	struct sockaddr_in dest;

	stClientBase* pClient = CreateClientBase("127.0.0.1", 7000);
	InitClientBase(pClient);
	Connect(pClient);
	SendBase(pClient, "juntang", 7);
	DisConnect(pClient);

	//SOCKET client;
	//struct sockaddr_in addr;
	//int ret;
	//char buf[64];
	//data_t *pData;
	//stMsgPutRequest* pMsg;
	//stMsgPutReply* reply;

	//stMsgGetRequest* GRequest;
	//stMsgGetReply* GReply;

	//uv_buf_t* uvBuf;
	//WSADATA data;
	//ret = WSAStartup(MAKEWORD(2,2),&data);

	//client = socket(AF_INET,SOCK_STREAM,0);

	//addr.sin_family = AF_INET;
	//addr.sin_port = htons(7000);
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//if( (ret = connect(client,(struct sockaddr*)&addr,sizeof(struct sockaddr))) == -1)
	//{
	//	fprintf(stderr,"connect error\n");
	//	return 1;
	//}

	//pMsg = CreateMsgPutRequestSt("huang", 5, "juntang", 7);
	//uvBuf = CreateMsgPutRequestBuf(pMsg);

	//ret = send(client,uvBuf->base,uvBuf->len,0);
	//fprintf(stderr,"send : %d\n",ret);
	//memset(buf,0,32);
	//ret = recv(client,buf,32,0);
	//
	//fprintf(stderr,"recv : %s\n",buf);
	//uvBuf->base = buf;
	//uvBuf->len = ret;
	//reply = ParseMsgPutReplyBuf( uvBuf );

	////Get
	//GRequest = CreateMsgGetRequestSt("huang", 5);
	//uvBuf = CreateMsgGetRequestBuf(GRequest);

	//ret = send(client,uvBuf->base,uvBuf->len,0);
	//fprintf(stderr,"send : %d\n",ret);
	//memset(buf,0,64);
	//ret = recv(client,buf,64,0);
	//	
	//fprintf(stderr,"recv : %s\n",buf);
	//uvBuf->base = buf;
	//uvBuf->len = ret;
	//GReply = ParseMsgGetReplyBuf( uvBuf );
}