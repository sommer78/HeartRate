enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     bas.c
* @brief    Battery service source file.
* @details  Interfaces to access Battery service.
* @author   Lilly_he
* @date     2015-5-12
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl_types.h"
#include "stdint.h"
#include "stddef.h"
#include "gatt.h"
#include <string.h>
#include "trace.h"
#include "profileApi.h"
#include "bas.h"
#include "bas_config.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  BatteryLevel value. */
static uint8_t gBatteryLevel = 0;
static uint8_t gBasReadBatteryLevelPending = FALSE;
/**<  Function pointer used to send event to application from pxp profile. */
/**<  Initiated in PXP_AddService. */
static pfnAPPHandleInfoCB_t pfnBasCB = NULL;

/**< @brief  profile/service definition.  */
static const TAttribAppl BasAttrTbl[] =
{
    /*----------------- Battery Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR),   /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_BATTERY),              /* service UUID */
            HI_WORD(GATT_UUID_BATTERY)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
#ifdef BAS_BATTERY_LEVEL_NOTIFY_SUPPORT            
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
            GATT_CHAR_PROP_NOTIFY)
#else
            GATT_CHAR_PROP_READ
#endif
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Battery Level value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BAS_LEVEL),
            HI_WORD(GATT_UUID_CHAR_BAS_LEVEL)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#ifdef BAS_BATTERY_LEVEL_NOTIFY_SUPPORT   
    ,
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }
#endif
};
/**< @brief  Battery service size definition.  */
const static uint16_t iBasAttrTblSize = sizeof(BasAttrTbl);

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref BAS_Application_Parameters
  * @param[in] length                value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool BAS_SetParameter( uint8_t param_type, uint8_t length, uint8_t *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        {
            ret = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "BAS_SetParameter failed\n", 0 );
        }
        break;

    case BAS_PARAM_BATTERY_LEVEL:
        {
            if (length != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                gBatteryLevel = value_ptr[0];
            }
        }
        break;
    }

    return ret;
}

/**
 * @brief Notify battery level value to client.
 *
 * @param[in] ServiceId         Service ID to notify.
 * @param[in] BatteryLevel      The value of battery level to notify.
 * @return Result of notify.
 * @retval 0 FALSE
 * @retval 1 TRUE
*/
bool BAS_BatteryLevelValueNotify( uint8_t ServiceId, uint8_t BatteryLevel )
{
    return ProfileAPI_SendData(ServiceId, GATT_SVC_BAS_BATTERY_LEVEL_INDEX, &BatteryLevel, sizeof(BatteryLevel));
}

/**
 * @brief Response battery level value to client.
 *
 * @param[in] ServiceId         Service ID to response.
 * @param[in] BatteryLevel      The value of battery level to response.
 * @return Result of Read confirmation
 * @retval 0 FALSE
 * @retval 1 TRUE
*/
bool BAS_BatteryLevelValueReadConfirm( uint8_t ServiceId, uint8_t BatteryLevel )
{
    if(gBasReadBatteryLevelPending == TRUE)
    {
        gBasReadBatteryLevelPending = FALSE;
        return  ProfileAPI_AttribValueReadConf(ServiceId, GATT_SVC_BAS_BATTERY_LEVEL_INDEX, &BatteryLevel, sizeof(BatteryLevel));
    }
    else
    {
        return FALSE;
    }
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
TProfileResult  BasAttrReadCb(uint8_t ServiceID, uint16_t iAttribIndex,
                              uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue)
{
    TAppResult appResult = AppResult_Success;
    TProfileResult  wCause  = ProfileResult_Success;
    *piLength = 0;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "BasAttrReadCb  iAttribIndex = %d iOffset %x", 2, iAttribIndex, iOffset);

    switch (iAttribIndex)
    {
    default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "BasAttrReadCb  iAttribIndex = %d not found", 1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
        }
        break;

    case GATT_SVC_BAS_BATTERY_LEVEL_INDEX:
        {
            TBAS_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = BAS_READ_BATTERY_LEVEL;
            appResult = pfnBasCB(ServiceID, (void*)&callback_data);
            if(appResult == AppResult_Pending)
            {
                gBasReadBatteryLevelPending = TRUE;
            }
            wCause = ProfileAPI_GetProfileResult(appResult);

            *ppValue = &gBatteryLevel;
            *piLength = sizeof(gBatteryLevel);
        }
        break;
    }
    return ( wCause );
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void BasCccdUpdateCb(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
    TBAS_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    bool bHandle = TRUE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "BasCccdUpdateCb  Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
    case GATT_SVC_BAS_CHAR_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = BAS_NOTIFY_BATTERY_LEVEL_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_index = BAS_NOTIFY_BATTERY_LEVEL_DISABLE;
            }
            break;
        }
    default:
        {
            bHandle = FALSE;
			break;
        }

    }
    /* Notify Application. */
    if (pfnBasCB && (bHandle == TRUE))
    {
        pfnBasCB(ServiceId, (void*)&callback_data);
    }

    return;
}

/**
 * @brief BAS Service Callbacks.
*/
CONST gattServiceCBs_t basCBs =
{
    BasAttrReadCb,  // Read callback function pointer
    NULL, // Write callback function pointer
    BasCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add BAS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval  ServiceId
  */
uint8_t BAS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)BasAttrTbl,
                                       iBasAttrTblSize,
                                       basCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "BAS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }
    pfnBasCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

