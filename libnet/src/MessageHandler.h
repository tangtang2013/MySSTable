#ifndef _LIBNET_MESSAGEHANDLER_H_
#define _LIBNET_MESSAGEHANDLER_H_

#include "Message.h"

//call back
//typedef void (*MsgHandler_cb)(char* pInBuffer, int nInBufferSize, char* pOutBuffer, int nOutBufferSize);
//MsgHandler_cb funcMsgHandler;
void* MsgHandler_callback(char* pInBuffer, int nInBufferSize);



#endif