/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     at_cmd.h
* @brief
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/


#ifndef _AT_CMD_H_
#define  _AT_CMD_H_

#define AT_CMD_MAX_LENGTH 28
extern uint8_t connindex;
extern bool isConnecting;


extern const char AtCmdResponseCONNTIMEOUT[];

void AtCmdSendResponse(const char *p_resp, uint16_t len);

uint32_t Atoi(const char *str);
void AtCmdParse(void);
void AtCmdSendResponseOK(void);
void AtCmdSendResponseTimeout(void);
void AtCmdSendResponseConnect(void);
void AtCmdSendResponseDisconnect(void);
void AtCmdSendResponseDisconnectTimeout(void);
void AtCmdSendResponseWake(void);
void AtCmdSendResponsePwrmConfig(void);
void AtCmdSendResponseInqEnd(void);
void AtCmdSendResponseConn(uint8_t connindex);
void AtCmdSendResponseConnEd(uint8_t connindex);
void AtCmdSendResponseDisConnED(void);
void AtCmdSendResponseInqEnd(void);

#endif

