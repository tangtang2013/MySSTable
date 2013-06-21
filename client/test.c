#include <stdio.h>
#include <common.h>
#include "client.h"

int main()
{
	int i,count=0;
	int nRet;
	char Buf[32];
	data_t* pData;
	stClient* pClient = CreateClient("127.0.0.1", 7000);
	InitClient(pClient);

	for(i=0; i<10000; i++)
	{
		sprintf(Buf,"%08d",i);
		nRet = Put(pClient,Buf,8,Buf,8);
		if(nRet == 0)
			count++;
	}
	fprintf(stderr,"put : %d\n",count);

	count = 0;
	for(i=0; i<10000; i++)
	{
		sprintf(Buf,"%08d",i);
		pData = Get(pClient,Buf,8);
		if(pData)
			count++;
	}
	fprintf(stderr,"get : %d\n",count);
}