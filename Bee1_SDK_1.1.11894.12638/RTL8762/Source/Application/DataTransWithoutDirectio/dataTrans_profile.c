enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dataTrans_profile.c
* @brief
* @details
* @author   jane
* @date     2015-03-29
* @version  v0.2
*********************************************************************************************************
*/

#include <string.h>
#include "trace.h"
#include "dataTrans_profile.h"
#include "app_queue.h"
#include "dataTrans_application.h"
#include "profile.h"
#include "profileApi.h"
#include "datatrans_client.h"
#include "module_param_config.h"
#include "dataTrans_uart.h"
#include "rtl876x_uart.h"

uint8_t gDataTransServiceId = 0xff;
const uint8_t GATT_UUID_DATATRANSFER_SERVICE[16] = { 0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0xE0, 0x00, 0x00};


static const TAttribAppl GattDataTransmitProfileTable[] =
{
    /*----------------- uart data Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),   /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_128BIT_SIZE,                            /* bValueLen     */
        (void *)GATT_UUID_DATATRANSFER_SERVICE,                                      /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* Data TX Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,              /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Data TX Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_DATA),
            HI_WORD(GATT_UUID_CHAR_DATA)
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_NOTIF_IND |   GATT_PERM_WRITE                      /* wPermissions    */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }

};




TProfileResult ProfileHandleDataRx(uint16_t wLength, uint8_t *pValue)
{
    TProfileResult wCause = ProfileResult_Success;
    PTxData pTxData = NULL;


    if (wLength > gProfileData.wMTUSize - 3)
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "ProfileHandleDataRx:len too long wLength=%d, MTU= %d\n", 2, wLength, gProfileData.wMTUSize);
#endif
        wCause = ProfileResult_InvalidValueSize;
        return wCause;
    }

    pTxData = AppQueueOut(&g_AppCB->txUartDataQueueFree);
    if (pTxData != NULL)
    {
        memcpy(pTxData->tx_buffer, pValue, wLength);
        pTxData->length = wLength;
        if (xQueueSend(g_AppCB->QueueHandleTxData, &pTxData, 0) == errQUEUE_FULL)
        {
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "ProfileHandleDataRx:send data failed\n", 0);
#endif
            AppQueueIn(&g_AppCB->txUartDataQueueFree, pTxData);
            wCause = ProfileResult_AppErr;
        }
    }
    else
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "ProfileHandleDataRx: queue is full\n", 0);
#endif
        wCause = ProfileResult_AppErr;
    }

    return wCause;
}





void Profile_DataTransSendData()
{
    PTxAppData pTxData = NULL;
    bool  status;


    if (g_AppCB->txAppDataQueue.ElementCount == 0)
    {
        return;
    }


    for (; g_AppCB->txAppDataQueue.ElementCount != 0;)
    {
        if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
        {
            if (g_AppCB->wDsCredits == 0)
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Profile_DataTransSendData: credit is 0\n", 0);
#endif
                return;
            }
#ifndef CODE_SIZE_32K_DEBUG
            //DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Profile_DataTransSendData: credit = %d\n", 1, g_AppCB->wDsCredits);
#endif
        }



        pTxData = AppQueueOut(&g_AppCB->txAppDataQueue);
        if (pTxData != NULL)
        {

            if (gConfigParam->gaprole.gaprole == CENTRAL_ROLE)
            {
                status = DtClient_V1WriteChar(pTxData->send_buffer, pTxData->length);
            }
            else if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
            {
                if (!g_AppCB->tx_service_cccd_enable)
                {
#ifndef CODE_SIZE_32K_DEBUG
                    DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "Profile_DataTransSendData: cccd is not config\n", 0);
#endif
                    AppQueueIn(&g_AppCB->txAppDataQueueFree, pTxData);
                    return;
                }
                status = ProfileAPI_SendData(gDataTransServiceId, DATA_VALUE_INDEX, pTxData->send_buffer,  pTxData->length);
            }


            if (status)
            {
                AppQueueIn(&g_AppCB->txAppDataQueueFree, pTxData);
                if (gConfigParam->gaprole.gaprole == PERIPHERAL_ROLE)
                {
                    g_AppCB->wDsCredits--;
                }

            }
            else
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "Profile_DataTransSendData: ProfileAPI_SendData failed\n", 0);
#endif
                AppQueueInsert(&g_AppCB->txAppDataQueue, NULL, pTxData);
                return;
            }
        }
    }
}



TProfileResult DataTransAttrWrite(uint8_t ServiceID, uint16_t iAttribIndex,
                                  uint16_t wLength, uint8_t *pValue, TGATTDWriteIndPostProc *pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;

    if (!pValue)
    {
//#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> DataTransAttrWrite   pValue %p wLength= 0x%x",
                   2,
                   pValue,
                   wLength);
//#endif

        wCause = ProfileResult_InvalidParameter;
        return wCause;
    }

    if (gConfigParam->isSleeping == TRUE)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RX/TX is forbidden in DLPS!!", 0);
        wCause = ProfileResult_InvalidParameter;
        return wCause;
    }

    switch (iAttribIndex)
    {
        case DATA_VALUE_INDEX:
            if (gConfigParam->isTimerDLPSActive == TRUE)
            {
#ifndef CODE_SIZE_32K_DEBUG
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "Data to WRITE before time out, Stop TimersEnterDLPSAfterConnected and Reset con-interval 18.75ms", 0);
#endif
                xTimerStop(TimersEnterDLPSAfterConnected, 0);
                gConfigParam->isTimerDLPSActive = FALSE;
                gConfigParam-> isDataAvailable = TRUE;
            }
            wCause = ProfileHandleDataRx(wLength, pValue);
            break;

        default:
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "DataTransAttrWrite not handle iAttribIndex 0x%x\n", 1, iAttribIndex);
#endif
            break;
    }


    return wCause;

}

void DataTransCccdUpdate(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
    if (Index == (DATA_VALUE_INDEX + 1))
    {
        if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
        {   /*  enable notification */
            g_AppCB->tx_service_cccd_enable = TRUE;
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Enter DataTransCccdUpdate():   enable notification", 0);
#endif
        }
        else
        {   /*  disable notification    */
            g_AppCB->tx_service_cccd_enable = FALSE;
#ifndef CODE_SIZE_32K_DEBUG
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Enter DataTransCccdUpdate():   disable notification", 0);
#endif
        }

    }
}
/*********************************************************************
 * PROFILE CALLBACKS
 */
CONST gattServiceCBs_t
dataTransCBs =
{
    NULL,                                       /* Read callback function pointer   */
    DataTransAttrWrite,             /* Write callback function pointer  */
    DataTransCccdUpdate             /* Authorization callback function pointer  */
};

void DataTrans_AddService(void *pFunc)
{
    if (FALSE == ProfileAPI_AddService(&gDataTransServiceId,
                                       (uint8_t *)GattDataTransmitProfileTable,
                                       sizeof(GattDataTransmitProfileTable),
                                       dataTransCBs))
    {
#ifndef CODE_SIZE_32K_DEBUG
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "DataTrans_AddService: ServiceId %d", 1, gDataTransServiceId);
#endif
        gDataTransServiceId = 0xff;
    }
}
