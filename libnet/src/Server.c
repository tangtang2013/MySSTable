#include <stdio.h>
#include "Server.h"
#include "ConnectHandler.h"

uv_loop_t* loop;
uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size);
void echo_read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf);
void OnConnection(uv_stream_t *server, int status);

stServer* ln_ServerCreate(char* strIP, int nPort)
{
	stServer* pServer = (stServer*)malloc(sizeof(stServer));
	memset(pServer, 0, sizeof(stServer));

	memcpy(pServer->strIp, strIP, strlen(strIP));
	pServer->nPort = nPort;

	return pServer;
}

int ln_ServerInit(stServer* pServer, MsgHandler_cb handler_cb)
{
	int ret;
	loop = uv_default_loop();
	uv_tcp_init(loop, &pServer->server);
	pServer->bind_addr = uv_ip4_addr(pServer->strIp, pServer->nPort);
	ret = uv_tcp_bind(&pServer->server, pServer->bind_addr);

	pServer->pConnhandler = InitConnectHandler(1, pServer, handler_cb);

	return ret;
}

void ln_ServerStart(stServer* pServer)
{
	int ret;
	pServer->server.handler = pServer->pConnhandler;
	ret = uv_listen((uv_stream_t*)&pServer->server,128,OnConnection);
	if(ret)
	{
		fprintf(stderr, "Listen error %s\n",uv_err_name(uv_last_error(loop)));
	}

	StartHandler(pServer->pConnhandler);

	uv_run(loop, UV_RUN_DEFAULT);
}

void ln_ServerStop(stServer* pServer)
{
	uv_stop(loop);
	StopHandler(pServer->pConnhandler);

}

void ln_ServerDestroy(stServer* pServer)
{
	if(loop)
	{
		uv_loop_delete(loop);
	}
	
	DestroyHandler(pServer->pConnhandler);

	free(pServer);
}

void OnConnection(uv_stream_t *server, int status)
{
	uv_tcp_t* client;
	uv_tcp_t* pServer = (uv_tcp_t*)server;
	if(status == -1)
	{
		//error
		fprintf(stderr, "OnConnection status error...\n");
		return;
	}

	client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	client->handler = pServer->handler;
	uv_tcp_init(loop, client);
	if(0 == uv_accept(server, (uv_stream_t*)client))
	{
		fprintf(stderr,"accept...:[%d]\n",uv_is_writable((uv_stream_t*)client));
		uv_read_start((uv_stream_t*)client,alloc_buffer,echo_read);
	}
	else
	{
		uv_close((uv_handle_t*)client, NULL);
	}
}

uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size)
{
    return uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void on_file_write(uv_write_t *req, int status) {
	write_req_t *wr = (write_req_t*) req;
	free(wr->buf.base);
	free(wr);
}

void write_data(uv_stream_t *dest, size_t size, uv_buf_t buf, uv_write_cb callback)
{
	int ret;
	write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
	req->buf = uv_buf_init((char*) malloc(size), size);
	memcpy(req->buf.base, buf.base, size);

	ret = uv_write((uv_write_t*)req, (uv_stream_t*)dest, &req->buf, 1, callback);
	fprintf(stderr,"uv_write ret : %d\n",ret);
}

void echo_read(uv_stream_t *stream, ssize_t nread, uv_buf_t buf)
{
	uv_tcp_t* pServer = (uv_tcp_t*)stream;
	stConnectHandler* pConnectHandler = pServer->handler;
    if (nread == -1) {
        if (uv_last_error(loop).code == UV_EOF)
		{
			//todo
        }
    }
    else 
	{
        if (nread > 0) 
		{
			//create worker insert into workList...
			AddWork(pConnectHandler,stream, buf.base, nread);
			//write_data(stream, nread, buf, on_file_write);
        }
    }
    if (buf.base)
        free(buf.base);
}
