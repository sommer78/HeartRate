enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     lls.c
* @brief    link loss service source file.
* @details  Interfaces to access link loss service.
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
#include "lls.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

static pfnAPPHandleInfoCB_t pfnLLSCB = NULL;
static uint8_t gLls_LinkLossAlertLevel = 0;

/**< @brief  profile/service definition.  */
const TAttribAppl LLSAttrTbl[] =
{
    /*----------------- Link Loss Service -------------------*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_LINK_LOSS_SERVICE),    /* service UUID */
            HI_WORD(GATT_UUID_LINK_LOSS_SERVICE)
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
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,            /* characteristic properties */
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
        GATT_PERM_READ | GATT_PERM_WRITE                           /* wPermissions */
    }
};
/**< @brief  LLS service size definition.  */
const int iLLSAttrTblSize = sizeof(LLSAttrTbl);


/**
  * @brief set Service related data from application.
  *
  * @param[in] param_type            type of parameter to be set: @ref LLS_Application_Parameters
  * @param[in] length                length of parameter value.
  * @param[in] value_ptr             pointer to the parameter value.
  * @return bool
  * @retval TRUE success
  * @retval FALSE fail
  */
bool LLS_SetParameter( uint8_t param_type, uint8_t length, void *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        /* invalid param to set. */
        ret = FALSE;
        break;
    case LLS_PARAM_LINK_LOSS_ALERT_LEVEL:
        gLls_LinkLossAlertLevel = *(uint8_t *)value_ptr;
        break;
  
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "PXP parameter set failed\n", 0 );

    return ret;
}


/**
 * @brief read characteristic data from service.
 *
 * @param ServiceID          ServiceID to be read. 
 * @param attrib_index          Attribute index of getting characteristic data.
 * @param offset                offset of characteritic to be read.
 * @param length_ptr            length of getting characteristic data.
 * @param ppValue            pointer to pointer of characteristic value to be read.
 * @return TProfileResult
*/
TProfileResult  LlsAttrReadCb(uint8_t ServiceId , uint16_t attrib_index, uint16_t offset, uint16_t * length_ptr , uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;
    *length_ptr = 0;
    TLLS_CALLBACK_DATA callback_data;

    switch ( attrib_index )
    {
    default:
        /* invalid attribute index. */
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "LLS_AttrReadCb default:attrib_index=%d\n", 1, attrib_index );
        wCause = ProfileResult_AttrNotFound;
        break;
 
    case GATT_SVC_PXP_LINK_LOSS_ALERT_LEVEL_INDEX:
        callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
        callback_data.msg_data.read_value_index = LLS_READ_ALERT_LEVEL;
        pfnLLSCB(ServiceId, (void*)&callback_data);
        *ppValue    = (uint8_t *)&gLls_LinkLossAlertLevel;
        *length_ptr = sizeof(gLls_LinkLossAlertLevel);
        break;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "LLS_AttrReadCb: AttribIndex--%d, length--%d",
               2,
               attrib_index,
               *length_ptr);
    return ( wCause );
}

/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param attrib_index          Attribute index of characteristic.
 * @param wLength            length of writing characteristic data.
 * @param pValue            pointer to characteristic data.
 * @param pWriteIndPostProc      function pointer called after LlsAttrWriteCb.
 * @return TProfileResult
*/
TProfileResult LlsAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex,
                              uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    TLLS_CALLBACK_DATA callback_data;
    if (!pValue)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> PXP_AttrWrite pValue %p wLength= 0x%x",
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
    case GATT_SVC_PXP_LINK_LOSS_ALERT_LEVEL_INDEX:
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
    if (pfnLLSCB && (wCause == ProfileResult_Success))
    {
        pfnLLSCB(ServiceId, (void*)&callback_data);
    }

    return wCause;

}

/*********************************************************************
 * SERVICE CALLBACKS
 */
// LLS related Service Callbacks
CONST gattServiceCBs_t llsCBs =
{
    LlsAttrReadCb,  // Read callback function pointer
    LlsAttrWriteCb, // Write callback function pointer
    NULL  // CCCD update callback function pointer
};

/**
  * @brief add LLS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval  ServiceId
  */
uint8_t LLS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)LLSAttrTbl,
                                       iLLSAttrTblSize,
                                       llsCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "LLS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }

    pfnLLSCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

