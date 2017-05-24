enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     tps.c
* @brief    Tx Power service source file.
* @details  Interfaces to access Tx Power service.
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
#include "tps.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static uint8_t TxpowerValue = 0;
/**<  Function pointer used to send event to application from tx power service. */
/**<  Initiated in TPS_AddService. */
static pfnAPPHandleInfoCB_t pfnTPSCB = NULL;

/**< @brief  profile/service definition.  */
const TAttribAppl TPSAttrTbl[] =
{
     /*----------------- TX Power Service -------------------*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_TX_POWER_SERVICE),    /* service UUID */
            HI_WORD(GATT_UUID_TX_POWER_SERVICE)
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
            GATT_CHAR_PROP_READ,           /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Alert Level Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_TX_LEVEL),
            HI_WORD(GATT_UUID_CHAR_TX_LEVEL)
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ                             /* wPermissions */
    }
};
/**< @brief  PXP profile related services size definition.  */
const static int iTPSAttrTblSize = sizeof(TPSAttrTbl);

/**
  * @brief Set TPS parameters.
  *
  * @param[in] param_type            type of parameter to be set: @ref TPS_Application_Parameters
  * @param[in] length                length of parameter value.
  * @param[in] value_ptr             pointer to the parameter value.
  * @return bool value
  * @retval TRUE success
  * @retval FALSE fail
  */
bool TPS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        /* invalid param to set. */
        ret = FALSE;
        break;

    case TPS_PARAM_TX_POWER:
        TxpowerValue = *(uint8_t *)value_ptr;
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
TProfileResult  TpsAttrReadCb(uint8_t ServiceId , uint16_t attrib_index, uint16_t offset, uint16_t * length_ptr , uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;
    *length_ptr = 0;
    TTPS_CALLBACK_DATA callback_data;
    switch ( attrib_index )
    {
    default:
        /* invalid attribute index. */
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "PXP_AttrRead default:attrib_index=%d\n", 1, attrib_index );
        wCause = ProfileResult_AttrNotFound;
        break;

    case TPS_TX_POWER_VALUE_INDEX:

        callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
        callback_data.msg_data.read_value_index = TPS_READ_TX_POWER_VALUE;
        pfnTPSCB(ServiceId, (void*)&callback_data);

        *ppValue    = (uint8_t *)&TxpowerValue;
        *length_ptr = sizeof(TxpowerValue);
        break;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- gatt get PXP attribute value: AttribIndex--%d, length--%d",
               2,
               attrib_index,
               *length_ptr);
    return ( wCause );
}


/*********************************************************************
 * SERVICE CALLBACKS
 */
// TPS related Service Callbacks
CONST gattServiceCBs_t tpsCBs =
{
    TpsAttrReadCb,  // Read callback function pointer
    NULL, // Write callback function pointer
    NULL  // CCCD update callback function pointer
};

/**
  * @brief add TPS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval  ServiceId
  */
uint8_t TPS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)TPSAttrTbl,
                                       iTPSAttrTblSize,
                                       tpsCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "TPS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }

    pfnTPSCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

