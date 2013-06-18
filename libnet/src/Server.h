#ifndef _LIBNET_SERVER_H_
#define _LIBNET_SERVER_H_

#include <common.h>

stServer* ln_ServerCreate(char* strIP, int nPort);
int ln_ServerInit(stServer* pServer, MsgHandler_cb handler_cb);
void ln_ServerStart(stServer* pServer);
void ln_ServerStop(stServer* pServer);
void ln_ServerDestroy(stServer* pServer);

void on_file_write(uv_write_t *req, int status);
void write_data(uv_stream_t *dest, size_t size, uv_buf_t buf, uv_write_cb callback);

#endif