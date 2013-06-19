#ifndef _LIBNET_MESSAGEHANDLER_H_
#define _LIBNET_MESSAGEHANDLER_H_

#include "Message.h"
#include <sstmanager.h>

sstmanager_t* pManager;

void InitMsgHandler();

void CloseMsgHandler();
//call back
//typedef void (*MsgHandler_cb)(char* pInBuffer, int nInBufferSize, char* pOutBuffer, int nOutBufferSize);
//MsgHandler_cb funcMsgHandler;
void* MsgHandler_callback(char* pInBuffer, int nInBufferSize);

void* PutRequestHandler(uv_buf_t* uvBuf);

void* GetRequestHandler(uv_buf_t* uvBuf);

#endif