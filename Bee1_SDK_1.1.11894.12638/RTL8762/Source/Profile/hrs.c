enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hrs.c
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
#include "hrs.h"


/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

HRS_NOTIFY_INDICATE_FLAG gHrs_NotifyIndicateFlag = {0};


HEART_RATE_MEASUREMENT_VALUE gHrs_HeartRateMeasurementValue = {0};
uint8_t gHrs_BodySensorLocationValue = 0;
THRSControlPoint gHrs_CtlPnt = {0};


/**<  Function pointer used to send event to application from location and navigation profile. */

static pfnAPPHandleInfoCB_t pfnHrsAppCB = NULL;

/**< @brief  profile/service definition.  */
static const TAttribAppl HrsAttrTbl[] =
{
    /*----------------- Heart Rate Service -------------------*/
    /* <<Primary Service>>, .. 0,*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR),   /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_SERVICE_HEART_RATE),              /* service UUID */
            HI_WORD(GATT_UUID_SERVICE_HEART_RATE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, Heart Rate Measurement*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY/* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Temperature Measurement value 2,*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HRS_HEART_RATE_MEASUREMENT),
            HI_WORD(GATT_UUID_CHAR_HRS_HEART_RATE_MEASUREMENT)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND/* wPermissions */
    },

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
#ifdef HRS_BODY_SENSOR_LOCATION_CHAR_SUPPORT
    ,
    /* <<Characteristic>>Body Sensor Location*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ/* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HRS_BODY_SENSOR_LOCATION),
            HI_WORD(GATT_UUID_CHAR_HRS_BODY_SENSOR_LOCATION)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef HRS_ENERGY_EXPENDED_FEATURE_SUPPORT
    ,
    /* <<Characteristic>> Heart Rate Control Point*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE/* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HRS_HEART_RATE_CP),
            HI_WORD(GATT_UUID_CHAR_HRS_HEART_RATE_CP)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE/* wPermissions */
    }
#endif
};

/**< @brief  Heart Rate service size definition.  */
const uint16_t iHrsAttrTblSize = sizeof(HrsAttrTbl);

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref HRS_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool HRS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        {
            ret = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HRS_SetParameter failed\n", 0 );
        }
        break;

    case HRS_HEART_RATE_MEASUREMENT_PARAM_FLAG:
        {
            if (len != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gHrs_HeartRateMeasurementValue.flag, value_ptr, len);
            }
        }
        break;

    case HRS_HEART_RATE_MEASUREMENT_PARAM_MEASUREMENT_VALUE:
        {
            if (len != sizeof(uint16_t) && len != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gHrs_HeartRateMeasurementValue.HeartRateMeasurementValue, value_ptr, len);

            }
        }
        break;

    case HRS_HEART_RATE_MEASUREMENT_PARAM_ENERGY_EXPENDED:
        {
            if (len != sizeof(uint16_t) )
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gHrs_HeartRateMeasurementValue.Energy_Expended, value_ptr, len);

            }
        }
        break;

    case HRS_HEART_RATE_MEASUREMENT_PARAM_RR_INTERVAL:
        {
            if (len != sizeof(uint16_t) )
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gHrs_HeartRateMeasurementValue.RR_Interval, value_ptr, len);

            }
        }
        break;

    case HRS_BODY_SENSOR_LOCATION_PARAM_VALUE:
        {
            if (len != sizeof(uint8_t) )
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gHrs_BodySensorLocationValue, value_ptr, len);
            }
        }
        break;

    }

    return ret;
}

/**
  * @brief send notification of heart rate measurement value.
  *
  * @param[in] ServiceId         service ID of service.
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool HRS_HeartRateMeasurementValueNotify( uint8_t ServiceId )
{
    uint8_t heart_rate_measurement_value[HRS_HEART_RATE_MEASUREMENT_VALUE_MAX_LEN];
    uint8_t iCurOffset = 0;

    memcpy(&heart_rate_measurement_value[iCurOffset], &gHrs_HeartRateMeasurementValue.flag, 1);
    iCurOffset += 1;

    if (gHrs_HeartRateMeasurementValue.flag.Heart_Rate_Value_Format_bit == Heart_Rate_Value_Format_UINT8)
    {
        memcpy(&heart_rate_measurement_value[iCurOffset], &gHrs_HeartRateMeasurementValue.HeartRateMeasurementValue, 1);
        iCurOffset += 1;
    }
    else
    {
        memcpy(&heart_rate_measurement_value[iCurOffset], &gHrs_HeartRateMeasurementValue.HeartRateMeasurementValue, 2);
        iCurOffset += 2;
    }

    if (gHrs_HeartRateMeasurementValue.flag.Energy_Expended_Status_bit)
    {
        memcpy(&heart_rate_measurement_value[iCurOffset], &gHrs_HeartRateMeasurementValue.Energy_Expended, 2);
        iCurOffset += 2;
    }

    if (gHrs_HeartRateMeasurementValue.flag.RR_Interval_bit)
    {
        memcpy(&heart_rate_measurement_value[iCurOffset], &gHrs_HeartRateMeasurementValue.RR_Interval, 2);
        iCurOffset += 2;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HRS_HeartRateMeasurementValueNotify", 0 );
    return ProfileAPI_SendData(ServiceId, HRS_HEART_RATE_MEASUREMENT_VALUE_INDEX, heart_rate_measurement_value, iCurOffset);
}

/**
 * @brief  handle control point write (request).
 *
 * @param serviceID
 * @param write_length      write request data length.
 * @param value_ptr         pointer to write request data.
 * @return none
 * @retval  void
*/
static TProfileResult  HRS_Hanlde_CtlPntProc( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    THRS_CALLBACK_DATA callback_data;
    TProfileResult  wCause  = ProfileResult_Success;
    uint16_t parameter_length = 0;
    memcpy( gHrs_CtlPnt.value, value_ptr, write_length );
    if(write_length>=1)
    {
        parameter_length = write_length - 1;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "    HRS_Hanlde_CtlPntProc request: OpCode=0x%x", 1,
               gHrs_CtlPnt.value[0] );

    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.msg_data.write.opcode = (THRS_HEART_RATE_CP_OPCODE)gHrs_CtlPnt.value[0];


    switch ( gHrs_CtlPnt.value[0] )
    {
    case HRS_HEART_RATE_CP_OPCODE_RESET_ENERGY_EXPENDED:
        {
            if (!gHrs_NotifyIndicateFlag.heart_rate_measurement_notify_enable)
            {
                wCause = ProfileResult_AppErr_CccdImproperlyConfigured;
            }
           else
            if(parameter_length != 0)
            {
                wCause = ProfileResult_InvalidParameter;
            }
        }
        break;


    default:
        {
            wCause = ProfileResult_AppErr;
        }
        break;
    }
    
    pfnHrsAppCB(serviceID, (void*)&callback_data);
    return wCause;
}


/**
 * @brief read characteristic data from service.
 *
 * @param ServiceId          ServiceID of characteristic data.
 * @param iAttribIndex          Attribute index of getting characteristic data.
 * @param iOffset                Used for Blob Read.
 * @param piLength            length of getting characteristic data.
 * @param ppValue            data got from service.
 * @return Profile procedure result
*/
TProfileResult  HrsAttrReadCb(uint8_t ServiceID, uint16_t iAttribIndex,
                              uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue)
{
    TProfileResult  wCause  = ProfileResult_Success;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HrsAttrReadCb  iAttribIndex = %d iOffset %x", 2, iAttribIndex, iOffset);

    *piLength = 0;

    switch (iAttribIndex)
    {
    case HRS_BODY_SENSOR_LOCATION_VALUE_INDEX:
        {
            THRS_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = HRS_READ_BODY_SENSOR_LOCATION_VALUE;            
            pfnHrsAppCB(ServiceID, (void*)&callback_data);

            *piLength = sizeof(gHrs_BodySensorLocationValue);
            memcpy(ppValue, &gHrs_BodySensorLocationValue, *piLength);
        }
        break;

    default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HrsAttrReadCb  iAttribIndex = %d not found", 1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
        }
        break;
    }

    return wCause;
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
TProfileResult HrsAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex,
                              uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HrsAttrWriteCb  iAttribIndex = %d wLength %x", 2, iAttribIndex, wLength);

    switch (iAttribIndex)
    {
    case HRS_HEART_RATE_CP_VALUE_INDEX:
        {
            /* Attribute value has variable size, make sure written value size is valid. */
            if ( (wLength > HRS_MAX_CTL_PNT_VALUE) || (pValue == NULL) )
            {
                wCause  = ProfileResult_InvalidValueSize;
            }
            /* Make sure Control Point is not "Process already in progress". */
            else if ( HRS_CTL_PNT_OPERATE_ACTIVE( gHrs_CtlPnt.value[0] ))
            {
                wCause  = ProfileResult_AppErr_ProcAlreadyInProgress;
            }
            /* Make sure Control Point is configured indication enable. */
            else if (!gHrs_NotifyIndicateFlag.heart_rate_measurement_notify_enable)
            {
                wCause = ProfileResult_AppErr_CccdImproperlyConfigured;
            }
            else
            {
                wCause = HRS_Hanlde_CtlPntProc(ServiceId, wLength, pValue);
            }

        }
        break;

    default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HrsAttrWriteCb  iAttribIndex = %d not found", 1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
        }
        break;
    }
    gHrs_CtlPnt.value[0] = HRS_HEART_RATE_CP_OPCODE_RESERVED;

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
void HrsCccdUpdateCb(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
    THRS_CALLBACK_DATA pHrsCallbackData;
    bool bHandle = TRUE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HrsCccdUpdateCb  Index = %d wCCCDBits %x", 2, Index, wCCCBits);

    switch (Index)
    {
    case HRS_HEART_RATE_MEASUREMENT_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                gHrs_NotifyIndicateFlag.heart_rate_measurement_notify_enable = 1;
                pHrsCallbackData.msg_data.notification_indification_index = HRS_NOTIFY_INDICATE_MEASUREMENT_VALUE_ENABLE;
            }
            else
            {
                gHrs_NotifyIndicateFlag.heart_rate_measurement_notify_enable = 0;
                pHrsCallbackData.msg_data.notification_indification_index = HRS_NOTIFY_INDICATE_MEASUREMENT_VALUE_DISABLE;
            }
        }
        break;


    default:
        {
            bHandle = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HrsCccdUpdateCb  Index = %d not found", 1, Index);
        }
        break;

    }
    /* Notify Application. */
    if (pfnHrsAppCB && (bHandle == TRUE))
    {
        pfnHrsAppCB(ServiceId, (void*)&pHrsCallbackData);
    }

    return;
}

/**
 * @brief Heart Rate Service Callbacks.
*/
CONST gattServiceCBs_t hrsCBs =
{
    HrsAttrReadCb,  // Read callback function pointer
    HrsAttrWriteCb, // Write callback function pointer
    HrsCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add HRS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t HRS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)HrsAttrTbl,
                                       iHrsAttrTblSize,
                                       hrsCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HRS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }
    pfnHrsAppCB = (pfnAPPHandleInfoCB_t)pFunc;



    return ServiceId;

}


