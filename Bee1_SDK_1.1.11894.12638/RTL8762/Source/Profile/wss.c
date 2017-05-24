enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     wss.c
* @brief    weight scale service source file.
* @details  Interface to access the weight scale service.
* @author   Vera
* @date     2015-10-20
* @version  v0.1
*********************************************************************************************************
*/
#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "wss.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Value of weight scale feature read characteristic. */
static WEIGHT_SCALE_FEATURE_VALUE weightscaleCharReadValue =
{ 
     WSS_FEATURE_TIMESTAMP_SUPPORTED,
		 WSS_FEATURE_MULTIUSERS_SUPPORTED,
		 WSS_FEATURE_BMI_SUPPORTED,
		 WSS_FEATURE_WEIGHTMEASUREMENT_RESOLUTION,
		 WSS_FEATURE_HEIGHTMEASUREMENT_RESOLUTION,
		 0
};      

/**<  Function pointer used to send event to application from Weight Scale Service. Initiated in Weight_Scale_Service_AddService. */
static pfnAPPHandleInfoCB_t pfnWeightScaleServiceCB = NULL;
/**<  Measurement Related Value. */
static WSS_MEASUREMENT gWssMeasurement = {0};
static uint8_t gWssMeasurementValueForIndicate[WSS_MEASUREMENT_VALUE_MAX_LEN];
static uint8_t  gWssMeasurementValueActualLength = 0;


/**< @brief  service definition.  */
/**<  Attribute Table */
static const TAttribAppl weight_scale_service_tbl[] =
{
    /* <<Primary Service>>, .. 0*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_WEIGHT_SCALE_SERVICE),      /* service UUID */
            HI_WORD(GATT_UUID_WEIGHT_SCALE_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },
    /* <<Characteristic>> for read , .. 1*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Weight Scale Feature value , .. 2 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_WEIGHT_SCALE_FEATURE_READ),
            HI_WORD(GATT_UUID_CHAR_WEIGHT_SCALE_FEATURE_READ)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    
    /* <<Characteristic>> for indicate , .. 3 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                          /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_INDICATE)               /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Weight Measurement value , .. 4 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_WEIGHT_MEASUREMENT_INDICATE),
            HI_WORD(GATT_UUID_CHAR_WEIGHT_MEASUREMENT_INDICATE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration , .. 5 */
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
    },

};


/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref WSS_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool Weight_Scale_Service_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;

    switch (param_type)
    {
		default:
				{
						ret = FALSE;
						DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter failed\n", 0 );
		}
				break;
		case WSS_PARAM_MEASUREMENT_WEIGHTPARAM_FLAG:
				{
						if(len != 1)
						{
								DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter Measurement WeightParam Flag \n", 0 );
						}
						memcpy(&gWssMeasurement.flag,value_ptr, len);
				}
				break;
		case WSS_PARAM_MEASUREMENT_WEIGHT_VALUE:
				{
            if(len != 2)
            {		
								DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter Measurement Weight value \n", 0 );
								break;
            }
            if(gWssMeasurement.flag.MEASUREMENT_UNITS == 0)
            {
                memcpy(&gWssMeasurement.WeightSIValue,value_ptr,len);
            }
            else
            {
                memcpy(&gWssMeasurement.WeightImperialValue,value_ptr,len);
            }            
				}
        break;
		case WSS_PARAM_MEASUREMENT_TIME_STAMP:
				{
            if(len != sizeof(WSS_TIME_STAMP))
            {		
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter Weight Measurement Time Stamp \n", 0 );
                break;
            }
			
            memcpy(&gWssMeasurement.TimeStamp,value_ptr,len);		
			
        }
        break;
		case WSS_PARAM_MEASUREMENT_USERID:
        {
            if(len != 1)
						{
								DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter Weight Measurement UserID \n", 0 );
								break;
						}
						
						memcpy(&gWssMeasurement.UserID,value_ptr,len);
        }
        break;
		case WSS_PARAM_MEASUREMENT_BMI:
        {
            if(len != 2)
						{
								DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter Weight Measurement BMI \n", 0 );
								break;
						}
						
						memcpy(&gWssMeasurement.BMI, value_ptr,len);
						
        }
        break;
		case WSS_PARAM_MEASUREMENT_HEIGHT_VALUE:
        {
            if(len != 2)
            {		
								DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter Height value \n", 0 );
								break;
            }
            if(gWssMeasurement.flag.MEASUREMENT_UNITS == 0)
            {
               memcpy(&gWssMeasurement.HeightSIValue,value_ptr,len);
            }
            else
            {
               memcpy(&gWssMeasurement.HeightImperialValue,value_ptr,len);
            }  
					  						
        }
        break;
		case WSS_PARAM_FEATURE_READ_CHAR_VAL:
		{
			if(len != 4 )
			{
			DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "WSS_SetParameter Feature value \n", 0 );
            break;
			}
			memcpy(&weightscaleCharReadValue,value_ptr,len);
			
		}
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Weight_Scale_Service_SetParameter failed\n", 0 );
		if ( ret )
				DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Weight_Scale_Service_SetParameter success\n", 0 );

    return ret;
}


void WSS_FormatMeasurementValue()
{
    uint8_t iCurOffset = 0;

    memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.flag, 1);
    iCurOffset += 1;

    if (gWssMeasurement.flag.MEASUREMENT_UNITS == 0)
    {
        memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.WeightSIValue, 2);
    }
    else
    {
        memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.WeightImperialValue, 2);
    }
    iCurOffset += 2;

    if (gWssMeasurement.flag.Time_Stamp_PRESENT)
    {
        memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.TimeStamp, 7);
        iCurOffset += 7;
    }

    if (gWssMeasurement.flag.USER_ID_PRESENT)
    {
        memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.UserID, 1);
        iCurOffset += 1;
    }
		if (gWssMeasurement.flag.BMI_AND_HEIGHT_PRESENT)
    {
        memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.BMI, 2);
        iCurOffset += 2;
				if (gWssMeasurement.flag.MEASUREMENT_UNITS == 0)
				{
						memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.HeightSIValue, 2);
				}
				else
				{
						memcpy(&gWssMeasurementValueForIndicate[iCurOffset], &gWssMeasurement.HeightImperialValue, 2);
				}
				iCurOffset += 2;
    }

    gWssMeasurementValueActualLength = iCurOffset;

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
TProfileResult  Weight_Scale_Service_AttrReadCb( uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;

    switch ( iAttribIndex )
    {
    default:
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- WeightScaleServiceAttrReadCb, Attr not found, index=%d",
                   1, iAttribIndex);
        wCause  = ProfileResult_AttrNotFound;
        break;
    case WEIGHT_SCALE_SERVICE_CHAR_FEATURE_READ_INDEX:
        {
						DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- Weight_Scale_Feature_Read, index = %d",1, iAttribIndex);
			
            TWS_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = WEIGHT_SCALE_READ_FEATURE;
            pfnWeightScaleServiceCB(ServiceId, (void*)&callback_data);            
            *ppValue  = (uint8_t *)&weightscaleCharReadValue;
            *piLength = sizeof(weightscaleCharReadValue);
						DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO,"weightscaleCharReadValue = %x,Length = %d",2,weightscaleCharReadValue,*piLength);
        }
        break;
    }

    return ( wCause );
}



/**
  * @brief send indication of WSS indicate characteristic value.
  *
  * @param[in] ServiceId         service ID of service.
  * @param[in] value             characteristic value to indicate
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */

bool Weight_Scale_Service_Measurement_Indicate( uint8_t ServiceId)
{
		WSS_FormatMeasurementValue();

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- WeightScaleService_Measurement_Indicate", 0 );
    // send indication to client
    return ProfileAPI_SendData(ServiceId, WEIGHT_SCALE_SERVICE_CHAR_MEASUREMENT_INDICATE_INDEX, (uint8_t*)&gWssMeasurementValueForIndicate, gWssMeasurementValueActualLength);
}



/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void Weight_Scale_ServiceCccdUpdateCb(uint8_t serviceId, uint16_t Index, uint16_t wCCCBits)
{
    TWS_CALLBACK_DATA callback_data;
    bool bHandle = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "weightScaleServiceCccdUpdateCb  Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
    case WEIGHT_SCALE_SERVICE_CHAR_INDICATE_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_INDICATION_BIT)
            {
                // Enable Indication
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indication_index = WSS_INDICATE_WEIGHT_MEASUREMENT_ENABLE;
            }
            else
            {
                // Disable Indication
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indication_index = WSS_INDICATE_WEIGHT_MEASUREMENT_DISABLE;
             }
            bHandle =  TRUE;
        }
        break;
    default:
        break;
    }
	if (pfnWeightScaleServiceCB && (bHandle == TRUE))
    {
        pfnWeightScaleServiceCB(serviceId, (void*)&callback_data);
    }
}

/**
 * @brief Weight Scale Service Callbacks.
*/
CONST gattServiceCBs_t Weight_Scale_Service_CBs =
{
    Weight_Scale_Service_AttrReadCb,  // Read callback function pointer
    NULL, // Write callback function pointer
    Weight_Scale_ServiceCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add Weight scale service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t Weight_Scale_Service_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)weight_scale_service_tbl,
                                       sizeof(weight_scale_service_tbl),
                                       Weight_Scale_Service_CBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "Weight_Scale_Service_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }
		DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Weight_Scale_Service_AddService: ServiceId %d", 1, ServiceId);
    pfnWeightScaleServiceCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

