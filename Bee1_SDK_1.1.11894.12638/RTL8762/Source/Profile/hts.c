enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hts.c
* @brief    Health Thermometer service source file.
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
#include "hts.h"


/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Temperature Type value. */
static HTS_TEMPERATURE_TYPE gTemperatureType = HTS_TEMPERATURE_TYPE_ARMPIT;
static uint16_t gHtsMeasurementInterval = 50;
static THTS_MEASUREMENT_INTERVAL_VALID_RANGE gHtsMeasurementIntervalValidRange = {0};

static pfnAPPHandleInfoCB_t pfnHtsCB = NULL;


static HTS_MESAURMENT gHtsMeasurement = {0};
static uint8_t gHtsMeasurementValueForNotifyIndicate[HTS_MEASUREMENT_VALUE_MAX_LEN];
static uint8_t  gHtsMeasurementValueActualLength = 0;

/**< @brief  profile/service definition.  */
static const TAttribAppl HtsAttrTbl[] =
{
    /*----------------- Health Temperature Service -------------------*/
    /* <<Primary Service>>, .. 0,*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_HELTH_TEMPERATURE_SERVICE),              /* service UUID */
            HI_WORD(GATT_UUID_HELTH_TEMPERATURE_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },
    /* <<Characteristic>>, .. 1,*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_INDICATE                     /* characteristic properties */
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
            LO_WORD(GATT_UUID_CHAR_HTS_TEMPERATURE_MEASUREMENT),
            HI_WORD(GATT_UUID_CHAR_HTS_TEMPERATURE_MEASUREMENT)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration 3*/
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
            
#ifdef HTS_TEMPERATURE_TYPE_CHAR_SUPPORT
    ,
    /* <<Characteristic>>, .. 4*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ                     /* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Temperature Type value 5*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HTS_TEMPERATURE_TYPE),
            HI_WORD(GATT_UUID_CHAR_HTS_TEMPERATURE_TYPE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef HTS_INTERMEDIATE_TEMPERATURE_CHAR_SUPPORT
    ,
    /* <<Characteristic>>, .. 6*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY                   /* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Intermediate Temperature value,7*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HTS_INTERMEDIATE_TMPERATURE),
            HI_WORD(GATT_UUID_CHAR_HTS_INTERMEDIATE_TMPERATURE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration ,8*/
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

#ifdef HTS_MEASUREMENT_INTERVAL_CHAR_SUPPORT
    ,
    /* <<Characteristic>>, .. ,9*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ                     /* characteristic properties */
#ifdef HTS_MEASUREMENT_INTERVAL_CHAR_INDICATE_SUPPORT
            |GATT_CHAR_PROP_INDICATE
#endif
#ifdef HTS_MEASUREMENT_INTERVAL_CHAR_WRITE_SUPPORT
            |GATT_CHAR_PROP_WRITE
#endif
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Measurement Interval value, 10*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HTS_MEASUREMENT_INTERVAL),
            HI_WORD(GATT_UUID_CHAR_HTS_MEASUREMENT_INTERVAL)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ|GATT_PERM_WRITE)/* wPermissions */
    }
    
#ifdef HTS_MEASUREMENT_INTERVAL_CHAR_INDICATE_SUPPORT
    ,
    /* client characteristic configuration, 11*/
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

#ifdef HTS_MEASUREMENT_INTERVAL_CHAR_WRITE_SUPPORT
    ,
    /* valid range descriptor ,12*/
    {
        ATTRIB_FLAG_VALUE_APPL,
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_VALID_RANGE),
            HI_WORD(GATT_UUID_CHAR_VALID_RANGE),
         
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ)          /* wPermissions */
    }
#endif
#endif
};
/**< @brief  Health Temperature service size definition.  */
const uint16_t iHtsAttrTblSize = sizeof(HtsAttrTbl);

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref HTS_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool HTS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        {
            ret = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HTS_SetParameter failed\n", 0 );
        }
        break;
        case HTS_PARAM_MEASUREMENT_TEMPPARAM_FLAG:
        {
            memcpy(&gHtsMeasurement.flag,value_ptr, 1);
        }
        break;
        case HTS_PARAM_MEASUREMENT_TEMPERATUER_VALUE:
        {
            if(len != 4)
            {		
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HTS_SetParameter Temperature value \n", 0 );
            break;
            }
            if(gHtsMeasurement.flag.Temp_value_units_bit == 0)
            {
                memcpy(&gHtsMeasurement.TempCelsiusValue,value_ptr,len);
            }
            else
            {
                memcpy(&gHtsMeasurement.TempFahrenheitValue,value_ptr,len);
            }            
        }
        break;
        case HTS_PARAM_MEASUREMENT_TIME_STAMP:
        {
            if(len != sizeof(TIME_STAMP))
            {		
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HTS_SetParameter Temperature value \n", 0 );
                break;
            }
            memcpy(&gHtsMeasurement.TimeStamp,value_ptr,len);		
        }
        break;
        case HTS_PARAM_MEASUREMENT_TEMPERATURE_TYPE:
        {
            gHtsMeasurement.TemperNextDesc = *(uint8_t*)value_ptr;
        }
        break;

        case HTS_PARAM_MEASUREMENT_INTERVAL:
           memcpy(&gHtsMeasurementInterval, value_ptr, len);		 
        break;
        
        case HTS_PARAM_VALID_RANGE:
            memcpy(&gHtsMeasurementIntervalValidRange, value_ptr, len);		
        break;
            
    
    }

    return ret;
}


void HTS_FormatMeasurementValue()
{
    uint8_t iCurOffset = 0;

    memcpy(&gHtsMeasurementValueForNotifyIndicate[iCurOffset], &gHtsMeasurement.flag, 1);
    iCurOffset += 1;

    if (gHtsMeasurement.flag.Temp_value_units_bit == 0)
    {
        memcpy(&gHtsMeasurementValueForNotifyIndicate[iCurOffset], &gHtsMeasurement.TempCelsiusValue, 4);
   }
    else
    {
        memcpy(&gHtsMeasurementValueForNotifyIndicate[iCurOffset], &gHtsMeasurement.TempFahrenheitValue, 4);
    }
    iCurOffset += 4;

    if (gHtsMeasurement.flag.Temp_time_stamp_present_bit)
    {
        memcpy(&gHtsMeasurementValueForNotifyIndicate[iCurOffset], &gHtsMeasurement.TimeStamp, 7);
        iCurOffset += 7;
    }

    if (gHtsMeasurement.flag.Temp_type_present_bit)
    {
        memcpy(&gHtsMeasurementValueForNotifyIndicate[iCurOffset], &gHtsMeasurement.TemperNextDesc, 1);
        iCurOffset += 1;
    }

  

    gHtsMeasurementValueActualLength = iCurOffset;

}

/**
  * @brief send indication of measurement value.
  *
  * @param[in] ServiceId         service ID of service.
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool HTS_MeasurementValueIndicate( uint8_t ServiceId )
{
    HTS_FormatMeasurementValue();
    return ProfileAPI_SendData(ServiceId, HTS_TEMPERATURE_MEASUREMENT_INDEX, (uint8_t*)&gHtsMeasurementValueForNotifyIndicate, gHtsMeasurementValueActualLength);
}

/**
  * @brief send notification of intermediate temperature value.
  *
  * @param[in] ServiceId         service ID of service.
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool HTS_IntermediateTemperatureValueNotify( uint8_t ServiceId )
{
    HTS_FormatMeasurementValue();
    return ProfileAPI_SendData(ServiceId, HTS_INTERMEDIATE_TEMPERATURE_INDEX, (uint8_t*)&gHtsMeasurementValueForNotifyIndicate, gHtsMeasurementValueActualLength);
}

/**
  * @brief send notification of measurement interval.
  *
  * @param[in] ServiceId         service ID of service.
  * @param[in] seconds           measurement interval value
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool HTS_MeasurementIntervalNotify( uint8_t ServiceId, uint16_t seconds)
{
    return ProfileAPI_SendData(ServiceId, HTS_MEASUREMENT_INTERVAL_INDEX,  (uint8_t*)&seconds, sizeof(seconds));
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
TProfileResult  HtsAttrReadCb(uint8_t ServiceID, uint16_t iAttribIndex,
                            uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue)
{
    TProfileResult  wCause  = ProfileResult_Success;
    *piLength = 0;
    THTS_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
        
    if (HTS_MEASUREMENT_INTERVAL_INDEX == iAttribIndex)
    {
        callback_data.msg_data.read_value_index = HTS_READ_MEASUREMENT_INTERVAL_VALUE;
        pfnHtsCB(ServiceID, (void*)&callback_data);        
        *ppValue = (uint8_t *)&gHtsMeasurementInterval;
        *piLength = sizeof(gHtsMeasurementInterval);
    }
    else if (HTS_TEMPERATURE_TYPE_INDEX == iAttribIndex)
    {
        callback_data.msg_data.read_value_index = HTS_READ_TEMPERATURE_TYPE_VALUE;
        pfnHtsCB(ServiceID, (void*)&callback_data);          
        *ppValue = &gTemperatureType;
        *piLength = 1;
    }
    else if (HTS_MEASUREMENT_INTERVAL_VALID_RANGE_INDEX == iAttribIndex)
    {
        callback_data.msg_data.read_value_index = HTS_READ_VALID_RANGE_VALUE;
        pfnHtsCB(ServiceID, (void*)&callback_data);          
        *ppValue = (uint8_t*)&gHtsMeasurementIntervalValidRange;
        *piLength = sizeof(gHtsMeasurementIntervalValidRange);
    }

    else
    {
        wCause  = ProfileResult_AttrNotFound;
    }
	
    return ( wCause );
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
TProfileResult HtsAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex,
                            uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HtsAttrWriteCb  iAttribIndex = %d wLength %x", 2, iAttribIndex, wLength);

    switch (iAttribIndex)
    {
    case HTS_MEASUREMENT_INTERVAL_INDEX:
        {
            THTS_CALLBACK_DATA callback_data;      
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_index = HTS_WRITE_MEASUREMENT_INTERVAL_VALUE;
            memcpy(&callback_data.msg_data.write.measurement_interval, pValue, 2);
            pfnHtsCB(ServiceId, (void*)&callback_data);
        }
        break;

    default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HtsAttrWriteCb  iAttribIndex = %d not found", 1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
        }
        break;
    }

    return wCause;}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void HtsCccdUpdateCb(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
    THTS_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    bool bHandle = TRUE;
    
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HtsCccdUpdate  Index = %d wCCCDBits %x", 2, Index, wCCCBits);

    switch (Index)
    {
    case HTS_TEMPERATURE_MEASUREMENT_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_INDICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = HTS_NOTIFY_INDICATE_TEMPERATURE_MEASUREMENT_VALUE_ENABLE;

            }
            else
            {
                callback_data.msg_data.notification_indification_index = HTS_NOTIFY_INDICATE_TEMPERATURE_MEASUREMENT_VALUE_DISABLE;
            }
        }
        break;
    case HTS_INTERMEDIATE_TEMPERATURE_CCCD_INDEX:
		 if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = HTS_NOTIFY_INDICATE_INTERMEDIATE_TEMPERATURE_VALUE_ENABLE;

            }
            else
            {
                callback_data.msg_data.notification_indification_index = HTS_NOTIFY_INDICATE_INTERMEDIATE_TEMPERATURE_VALUE_DISABLE;
            }
        break;
    case HTS_MEASUREMENT_INTERVAL_CCCD_INDEX:
            if (wCCCBits & GATT_CCCD_INDICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = HTS_NOTIFY_INDICATE_MEASUREMENT_INTERVAL_VALUE_ENABLE;

            }
            else
            {
                callback_data.msg_data.notification_indification_index = HTS_NOTIFY_INDICATE_MEASUREMENT_INTERVAL_VALUE_DISABLE;
            }
        break;
        
    default:
        bHandle = FALSE;
        break;

    }
    /* Notify Application. */
    if (pfnHtsCB && (bHandle == TRUE))
    {
        pfnHtsCB(ServiceId, (void*)&callback_data);
    }

    return;
}

/**
 * @brief BAS Service Callbacks.
*/
CONST gattServiceCBs_t htsCBs =
{
    HtsAttrReadCb,  // Read callback function pointer
    HtsAttrWriteCb, // Write callback function pointer
    HtsCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add hts service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t HTS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)HtsAttrTbl,
                                       iHtsAttrTblSize,
                                       htsCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HTS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }
    pfnHtsCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;

}


