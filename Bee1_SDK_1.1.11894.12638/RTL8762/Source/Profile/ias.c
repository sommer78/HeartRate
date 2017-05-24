enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ias.c
* @brief    immediate alert level service source file.
* @details  Interfaces to access immediate alert level service.
* @author
* @date     2015-5-12
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl_types.h"
#include <string.h>
#include "trace.h"
#include "endialig.h"
#include "gatt.h"
#include "profileApi.h"
#include "ias.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/


static pfnAPPHandleInfoCB_t pfnIASCB = NULL;

/**< @brief  profile/service definition.  */
const TAttribAppl IASAttrTbl[] =
{
    /*----------------- Immediate Alert Service -------------------*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_IMMEDIATE_ALERT_SERVICE),    /* service UUID */
            HI_WORD(GATT_UUID_IMMEDIATE_ALERT_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* Alert Level Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP,              /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Alert Level Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_ALERT_LEVEL),
            HI_WORD(GATT_UUID_CHAR_ALERT_LEVEL)
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_WRITE | GATT_PERM_READ                           /* wPermissions */
    }  
};
/**< @brief  IAS service size definition.  */
const int iIASAttrTblSize = sizeof(IASAttrTbl);

/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param attrib_index          Attribute index of characteristic.
 * @param wLength            length of writing characteristic data.
 * @param pValue            pointer to characteristic data.
 * @param pWriteIndPostProc      function pointer after IasAttrWriteCb.
 * @return TProfileResult
*/
TProfileResult IasAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex,
                              uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    TIAS_CALLBACK_DATA callback_data;
    if (!pValue)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> IasAttrWriteCb pValue %p wLength= 0x%x",
                   2,
                   pValue,
                   wLength);
        wCause = ProfileResult_InvalidParameter;
        return wCause;
    }

    switch (iAttribIndex)
    {
    default:
        wCause = ProfileResult_AttrNotFound;
        break;
 
    case GATT_SVC_PXP_IMMEDIATE_AlERT_VALUE_INDEX:
        if (wLength != sizeof(UINT8))
        {
            wCause = ProfileResult_InvalidValueSize;
        }
        else
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write_alert_level = pValue[0];
            
        }
        break;
    }
    if (pfnIASCB && (wCause == ProfileResult_Success))
    {
        pfnIASCB(ServiceId, (void*)&callback_data);
    }

    return wCause;

}

/*********************************************************************
 * SERVICE CALLBACKS
 */
// IAS related Service Callbacks
CONST gattServiceCBs_t
iasCBs =
{
    NULL,  // Read callback function pointer
    IasAttrWriteCb, // Write callback function pointer
    NULL  // CCCD update callback function pointer
};

/**
  * @brief add IAS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval  ServiceId
  */
uint8_t IAS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)IASAttrTbl,
                                       iIASAttrTblSize,
                                       iasCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "IAS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }

    pfnIASCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

