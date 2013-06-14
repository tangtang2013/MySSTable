#include <stdio.h>
#include <signal.h>
#include <uv.h>
#include "Server.h"

stServer* pServer;

void SignalFunc(int nSignal)
{
	ln_ServerStop(pServer);
	fprintf(stderr,"exit...\n");
	ln_ServerDestroy(pServer);
	exit(-1);
}

int main(int argc, char** argv)
{
	signal(SIGINT,SignalFunc);
	signal(SIGTERM,SignalFunc);

	pServer = ln_ServerCreate("0.0.0.0",7000);
	ln_ServerInit(pServer);
	ln_ServerStart(pServer);
}