enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     sps.c
* @brief    SPS service source file.
* @details  Interfaces to access SPS service.
* @author   Lilly_he
* @date     2015-5-12
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl_types.h"
#include "stdint.h"
#include "stddef.h"
#include "gatt.h"
#include "profileApi.h"
#include <string.h>
#include "trace.h"
#include "sps.h"
#include "sps_config.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  SPS Refresh Value. */
static uint8_t spsRefreshValue __attribute__((unused)) = 0;
/**<  Function pointer used to send event to application from sps profile. */
/**<  Initiated in SPS_AddService. */
static pfnAPPHandleInfoCB_t pfnSpsCB = NULL;

/**< @brief  profile/service definition.  */
const TAttribAppl SpsAttrTbl[] =
{
    /*--------------------------ScanParameters Service------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR),   /* wFlags     */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_SPS),              /* service UUID */
            HI_WORD(GATT_UUID_SPS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE_NO_RSP                  /* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Scan Interval Window Char value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SCAN_INTERVAL_WINDOW),
            HI_WORD(GATT_UUID_CHAR_SCAN_INTERVAL_WINDOW)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY                    /* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#ifdef SPS_CHAR_SCAN_REFRESH_SUPPORT
     ,
    /* Scan Refresh Char value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SCAN_REFRESH),
            HI_WORD(GATT_UUID_CHAR_SCAN_REFRESH)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* wFlags */
        {                                         /* bTypeValue */
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
/**< @brief  SPS service size definition.  */
const uint16_t iSpsAttrTblSize = sizeof(SpsAttrTbl);

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref SPS_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool SPS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        /* invalid param to set. */
        ret = FALSE;
        break;
    case SPS_PARAM_SCAN_REFRESH:
        spsRefreshValue = *((uint8_t*)value_ptr);
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "SPS parameter set failed\n", 0 );

    return ret;
}


/**
  * @brief send notification.
  *
  * @param[in] ServiceId            service ID of service.
  * @param[in] scanIntervalWindow   characteristic value to notify
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool SPS_ScanIntervalWindowValueNotify( uint8_t ServiceId, TSPS_SCAN_INTERVAL_WINDOW scanIntervalWindow )
{
    return ProfileAPI_SendData(ServiceId, GATT_SVC_SPS_SCAN_REFRESH_INDEX, (uint8_t*)&scanIntervalWindow, sizeof(scanIntervalWindow));
}


/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param iAttribIndex          Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue            value to be written.
 * @return Profile procedure result
*/
TProfileResult SpsAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex,
                              uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc)
{

    TProfileResult  wCause = ProfileResult_Success;
    if ((GATT_SVC_SPS_SCAN_INTERVAL_INDEX == iAttribIndex)
            && (wLength == 4) && pValue)
    {
        uint16_t ScanInterval = ((uint16_t)pValue[1] << 8) && (0xff00) + pValue[0];
        uint16_t ScanWindow = ((uint16_t)pValue[2] << 8) && (0xff00) + pValue[3];

        TSPS_CALLBACK_DATA callback_data;
        callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
        callback_data.msg_data.write.write_type = SPS_WRITE_SCAN_INTERVAL_WINDOW;
        callback_data.msg_data.write.write_parameter.scan.scan_interval = ScanInterval;
        callback_data.msg_data.write.write_parameter.scan.scan_interval = ScanWindow;
        /* Notify Application. */
        if (pfnSpsCB)
        {
            pfnSpsCB(ServiceId, (void*)&callback_data);
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> SpsAttrWrite Error  iAttribIndex = 0x%x wLength=%d",
                   2,
                   iAttribIndex,
                   wLength);
        wCause = ProfileResult_AttrNotFound;
    }
    return wCause;

}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void SpsCccdUpdateCb(uint8_t serviceId, uint16_t Index, uint16_t wCCCBits)
{
    TSPS_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    bool bHandle = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SpsCccdUpdate  Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
    case GATT_SVC_SPS_SCAN_REFRESH_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = SPS_NOTIFY_INDICATE_SCAN_REFRESH_ENABLE;
            }
            else
            {
                callback_data.msg_data.notification_indification_index = SPS_NOTIFY_INDICATE_SCAN_REFRESH_DISABLE;
            }
            bHandle =  TRUE;
            break;
        }
    default:
        break;
    }
    /* Notify Application. */
    if (pfnSpsCB && (bHandle == TRUE))
    {
        pfnSpsCB(serviceId, (void*)&callback_data);
    }

    return;
}

/**
 * @brief SPS Service Callbacks.
*/
CONST gattServiceCBs_t spsCBs =
{
    NULL,  // Read callback function pointer
    SpsAttrWriteCb, // Write callback function pointer
    SpsCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add scan parameters service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t SPS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)SpsAttrTbl,
                                       iSpsAttrTblSize,
                                       spsCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "SPS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }
    pfnSpsCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

