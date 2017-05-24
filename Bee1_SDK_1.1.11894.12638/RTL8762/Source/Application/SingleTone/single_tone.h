/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      single_tone.h
* @brief     single tone test defines.
* @details   none.
* @author    Lory
* @date      2015-07-28
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _BEE_SINGLETONE_H_
#define _BEE_SINGLETONE_H_

#include "rtl_types.h"

typedef enum tagComResult
{
    comResultSuccess     = 0,             /*  0: great, lets go on  */
    comResultPending     = 1,             /*  1: request is pending */
    comResultSuccessWithBufferToRelease = 2, /*data has been send successfully,  and buffer can br releasee now, lower stack will not call hciComApiCallBack to notify*/

    /* 0 - 0x0F reserved for positiv results */

    comResultUnspecified = 0x10,            /* 10: unspecified error */
    comResultNoResources,                   /* 11: no Memory etc */
    comResultOpenfailed,                    /* 12: open of SubDevice failed */
    comResultHwProviderNotImplemented,      /* 13: the requested Hardware Provider is not implemented */
    comResultTransferModeNotImplemented,    /* 14: the requested Mode is not implemented */
    comResultInvalidHandle,                 /* 15: the handle is invalid */
    comResultInvalidLength,                 /* 16: the length is invalid */
    comResultInvalidState,                  /* 17: the given channel state is invalid */
    comResultInvalidParam,                  /* 18: the param are invalid */

    comResultPropertyNotImplemented = 0x30, /* 30: the requested Property is not implemented */
    comResultPropertyIgnored,               /* 31: the requested Property is ignored */
    comResultPropertyBadPara,               /* 32: Property parameter is bad */
    comResultPropertySetFailed,             /* 33: Setting of Property parameter failed */
    comResultNotReady,                      /* The device is not ready RNR" */
    comResultTimeout,                       /* Sync operation failed */

    comResultLast = comResultTimeout
} TComResult;

typedef enum tagComEvent
{
    comeNone,
    comeOpenCompleted,
    comeCloseCompleted,
    comeDataIndication,
    comeDataTransmitted,
    comeError
} TComEvent;

typedef struct
{
    uint16_t opcode;
    uint8_t length;
    uint8_t start;
    uint8_t channle;
    uint8_t txPower;
		uint8_t isLE;
} VEND_CMD_SINGLE_TONE;

typedef struct
{
    uint16_t opcode;
    uint8_t length;
    uint8_t txPower;
} VEND_CMD_TX_POWER;

typedef TComResult ( * PComCallBack)(
    void     *pvContext,
    TComEvent event,
    uint32_t  uStatus,
    void     *pvBuffer,
    uint32_t  uLength,
    uint8_t pType
);

extern TComResult comOpen(PComCallBack callBack, void *callBackContext);
extern TComResult comClose (void);
extern TComResult comWrite (uint8_t pkt_type, uint8_t *pvBuffer, uint32_t uBufferLength);
extern TComResult comResponse (uint8_t *pvBuffer);

/* export functions */
void SingleTone_Init(void);

#endif  /* _BEE_SINGLETONE_H_ */

