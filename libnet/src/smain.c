#include <stdio.h>
#include <signal.h>
#include <uv.h>
#include "Server.h"
#include "TMessageHandler.h"

stServer* pServer;

void SignalFunc(int nSignal)
{
	fprintf(stderr,"exit...\n");
	ln_ServerStop(pServer);
	ln_ServerDestroy(pServer);
	exit(-1);
}

int main(int argc, char** argv)
{
	signal(SIGINT,SignalFunc);
	signal(SIGTERM,SignalFunc);

	pServer = ln_ServerCreate("0.0.0.0",7000);
	ln_ServerInit(pServer,MsgHandler_callback);
	ln_ServerStart(pServer);
}