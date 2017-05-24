enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     lns.c
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
#include "lns.h"


/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
uint32_t gLns_LnFeatureSupport = 0;

LNS_NOTIFY_INDICATE_FLAG gLns_NotifyIndicateFlag = {0};
TLNSControlPoint gLns_CtlPnt = {0};

POSITION_QUALITY_VALUE gLns_PositionQualityValue = {0};
uint8_t gLns_PQValueForReadCfm[LNS_POSITION_AND_QUALITY_VALUE_MAX_LEN] = {0};
uint8_t gLns_PQValueLengthForReadCfm = 0;

LOCATION_AND_SPEED_VALUE gLns_LocationAndSpeedValue = {0};
NAVIGATION_VALUE gLns_NavigationValue = {0};
uint16_t gLns_CP_NumberOfRoute = 0;
uint8_t gLns_CP_NameOfRoute[LNS_CP_PARA_NAME_OF_ROUTE_MAX_LEN] = {0};
uint8_t gLns_CP_NameOfRouteLen = 0;

/**<  Function pointer used to send event to application from location and navigation profile. */

static pfnAPPHandleInfoCB_t pfnLnsAppCB = NULL;

/**< @brief  profile/service definition.  */
static const TAttribAppl LnsAttrTbl[] =
{
    /*----------------- Health Temperature Service -------------------*/
    /* <<Primary Service>>, .. 0,*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR),   /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_SERVICE_LOCATION_AND_NAVIGATION),              /* service UUID */
            HI_WORD(GATT_UUID_SERVICE_LOCATION_AND_NAVIGATION)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, .. 1, LN Feature*/
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
    /* Temperature Measurement value 2,*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_LNS_LN_FEATURE),
            HI_WORD(GATT_UUID_CHAR_LNS_LN_FEATURE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* <<Characteristic>>, ..Location and Speed */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY                     /* characteristic properties */
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
            LO_WORD(GATT_UUID_CHAR_LNS_LOCATION_AND_SPEED),
            HI_WORD(GATT_UUID_CHAR_LNS_LOCATION_AND_SPEED)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
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
    },

#ifdef LNS_POSITION_QUALITY_CHAR_SUPPORT
    /* <<Characteristic>>, .. Position Quality*/
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
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_LNS_POSITION_QUALITY),
            HI_WORD(GATT_UUID_CHAR_LNS_POSITION_QUALITY)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
#endif

#ifdef LNS_LN_CONTROL_POINT_CHAR_SUPPORT
    /* <<Characteristic>>, .. LN Control Point*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_INDICATE /* characteristic properties */
            )
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Measurement Interval value*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_LNS_LN_CP),
            HI_WORD(GATT_UUID_CHAR_LNS_LN_CP)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE/* wPermissions */
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
    },
#endif

#ifdef LNS_LN_NAVIGATION_CHAR_SUPPORT
    /* <<Characteristic>>, .. Navigation*/
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
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_LNS_NAVIGATION),
            HI_WORD(GATT_UUID_CHAR_LNS_NAVIGATION)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND_AUTHEN_REQ/* wPermissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }
#endif

};

/**< @brief  Health Temperature service size definition.  */
const uint16_t iLnsAttrTblSize = sizeof(LnsAttrTbl);

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref LNS_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool LNS_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        {
            ret = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "LNS_SetParameter failed\n", 0 );
        }
        break;

    case LNS_PARAM_LN_FEATURE_SUPPORT:
      {
            if (len != sizeof(uint32_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LnFeatureSupport, value_ptr, len);
            }
        }
        break;      

    //Location And Speed Value
    case LNS_LAS_PARAM_INC_FLAG:
        {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.flag, value_ptr, len);
            }
        }
        break;

    case LNS_LAS_PARAM_INSTANTANEOUS_SPEED:
        {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.Instantaneous_Speed, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_TOTAL_DISTANCE:
        {
            if (len != 3)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.Total_Distance, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_LOCATION_LATITUDE:
        {
            if (len != 3)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.Location_Latitude, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_LOCATION_LONGITUDE:
        {
            if (len != 3)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.Location_Latitude, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_ELEVATION:
        {
            if (len != 3)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.Elevation, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_HEADING:
        {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.Heading, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_ROLLING_TIME:
        {
            if (len != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.Rolling_Time, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_UTC_TIME:
        {
            if (len != 7)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_LocationAndSpeedValue.UTC_Time, value_ptr, len);

            }
        }
        break;

    case LNS_LAS_PARAM_POSITION_STATUS:
        {
            gLns_LocationAndSpeedValue.flag.Position_Status =  *(uint8_t*)value_ptr;
        }
        break;

    case LNS_LAS_PARAM_SPEED_AND_DISTANCE_FORMAT:
        {
            gLns_LocationAndSpeedValue.flag.Speed_and_Distance_format =  *(uint8_t*)value_ptr;
        }
        break;

    case LNS_LAS_PARAM_ELEVATION_SOURCE:
        {
            gLns_LocationAndSpeedValue.flag.Elevation_Source =  *(uint8_t*)value_ptr;
        }
        break;

    case LNS_LAS_PARAM_HEADING_SOURCE:
        {
            gLns_LocationAndSpeedValue.flag.Heading_Source =  *(uint8_t*)value_ptr;
        }
        break;

    //Navigation value
    case LNS_NAVIGATION_PARAM_INC_FLAG:
        {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_NavigationValue.flag, value_ptr, len);
            }
        }
        break;

    case LNS_NAVIGATION_PARAM_BEARING:
        {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_NavigationValue.Bearing, value_ptr, len);

            }
        }
        break;

    case LNS_NAVIGATION_PARAM_HEADING:
        {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_NavigationValue.Heading, value_ptr, len);

            }
        }
        break;

    case LNS_NAVIGATION_PARAM_REMAINING_DISTANCE:
        {
            if (len != 3)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_NavigationValue.Reamining_Distance, value_ptr, len);

            }
        }
        break;

    case LNS_NAVIGATION_PARAM_REMAINING_VERTICAL_DISTANCE:
        {
            if (len != 3)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_NavigationValue.Reamining_Vertical_Distance, value_ptr, len);

            }
        }
        break;

    case LNS_NAVIGATION_PARAM_ESTIMATED_TIME_OF_ARRIVAL:
        {
            if (len != 7)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_NavigationValue.Estimated_Time_of_Arrival, value_ptr, len);

            }
        }
        break;

    case LNS_NAVIGATION_PARAM_POSITION_STATUS:
        {
            gLns_NavigationValue.flag.Position_Status =  *(uint8_t*)value_ptr;
        }
        break;
    case LNS_NAVIGATION_PARAM_HEADING_SOURCE:
        {
            gLns_NavigationValue.flag.Heading_Source =  *(uint8_t*)value_ptr;
        }
        break;

    case LNS_NAVIGATION_PARAM_NAVIGATION_INDICATOR_TYPE:
        {
            gLns_NavigationValue.flag.Navigation_Indicator_Type =  *(uint8_t*)value_ptr;
        }
        break;

    case LNS_NAVIGATION_PARAM_WAYPOINT_REACHED:
        {
            gLns_NavigationValue.flag.Waypoint_Reached =  *(uint8_t*)value_ptr;
        }
        break;

    case LNS_NAVIGATION_PARAM_DESTINATION_REACHED:
        {
            gLns_NavigationValue.flag.Destination_Reached =  *(uint8_t*)value_ptr;
        }
        break;

    case LNS_PQ_PARAM_INC_FLAG:
      {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.flag, value_ptr, len);
            }
        }
        break;

    case LNS_PQ_PARAM_NUMBER_OF_BEACONS_IN_SOLUTION:
      {
            if (len != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.Number_of_Beacons_in_Solution, value_ptr, len);
            }
        }
        break;

    case LNS_PQ_PARAM_NUMBER_OF_BEACONS_IN_VIEW:
      {
            if (len != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.Number_of_Beacons_in_View, value_ptr, len);
            }
        }
        break;


    case LNS_PQ_PARAM_TIME_TO_FIRST_FIX:
      {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.Time_to_First_Fix, value_ptr, len);
            }
        }
        break;

    case LNS_PQ_PARAM_EHPE:
      {
            if (len != sizeof(uint32_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.EHPE, value_ptr, len);
            }
        }
        break;

    case LNS_PQ_PARAM_EVPE:
      {
            if (len != sizeof(uint32_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.EVPE, value_ptr, len);
            }
        }
        break;
    case LNS_PQ_PARAM_HDOP:
      {
            if (len != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.HDOP, value_ptr, len);
            }
        }
        break;

    case LNS_PQ_PARAM_VDOP:
      {
            if (len != sizeof(uint8_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_PositionQualityValue.VDOP, value_ptr, len);
            }
        }
        break;

    case LNS_CP_PARA_NUMBER_OF_ROUTE:
      {
            if (len != sizeof(uint16_t))
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_CP_NumberOfRoute, value_ptr, len);
            }
        }
        break;

    case LNS_CP_PARA_NAME_OF_ROUTE:
      {
            if (len > LNS_CP_PARA_NAME_OF_ROUTE_MAX_LEN)
            {
                ret = FALSE;
            }
            else
            {
                memcpy(&gLns_CP_NameOfRoute, value_ptr, len);
                gLns_CP_NameOfRouteLen = len;
            }
        }
        break;
    case LNS_PARAM_CTL_PNT_PROG_CLR:
        {
            if (len != 0)
            {
                ret = FALSE;
            }
            else
            {
                gLns_CtlPnt.value[0] = LN_CP_OPCODE_RESERVED;
            }
        }
        break;

    }

    return ret;
}

void LNS_FormatPQValueForRead()
{
    uint8_t iCurOffset = 0;

    memcpy(&gLns_PQValueForReadCfm[iCurOffset], &gLns_PositionQualityValue.flag, 2);
    iCurOffset += 2;

    if (gLns_PositionQualityValue.flag.Number_of_Beacons_in_Solution_Present)
    {
        memcpy(&gLns_PQValueForReadCfm[iCurOffset], &gLns_PositionQualityValue.Number_of_Beacons_in_Solution, 1);
        iCurOffset += 1;
    }

    if (gLns_PositionQualityValue.flag.Number_of_Beacons_in_View_Present)
    {
        memcpy(&gLns_PQValueForReadCfm[iCurOffset], &gLns_PositionQualityValue.Number_of_Beacons_in_View, 1);
        iCurOffset += 1;
    }

    if (gLns_PositionQualityValue.flag.Time_to_First_Fix_Present)
    {
        memcpy(&gLns_PQValueForReadCfm[iCurOffset], &gLns_PositionQualityValue.Time_to_First_Fix, 2);
        iCurOffset += 2;
    }

    if (gLns_PositionQualityValue.flag.EHPE_Present)
    {
        memcpy(&gLns_PQValueForReadCfm[iCurOffset], &gLns_PositionQualityValue.EHPE, 4);
        iCurOffset += 4;
    }

    if (gLns_PositionQualityValue.flag.EVPE_Present)
    {
        memcpy(&gLns_PQValueForReadCfm[iCurOffset], &gLns_PositionQualityValue.EVPE, 4);
        iCurOffset += 4;
    }

    if (gLns_PositionQualityValue.flag.HDOP_Present)
    {
        memcpy(&gLns_PQValueForReadCfm[iCurOffset], &gLns_PositionQualityValue.HDOP, 1);
        iCurOffset += 1;
    }

    if (gLns_PositionQualityValue.flag.VDOP_Present)
    {
        memcpy(&gLns_PQValueForReadCfm[iCurOffset],
               &gLns_PositionQualityValue.VDOP, 1);
        iCurOffset += 1;
    }

    gLns_PQValueLengthForReadCfm = iCurOffset;

}

uint8_t LNS_GetFisrtValueOffsetToNotify()
{
   
    uint8_t iCurOffset = 2;

    if (gLns_LocationAndSpeedValue.flag.Instantaneous_Speed_Present)
    {
        iCurOffset += 2;
    }

    if (gLns_LocationAndSpeedValue.flag.Total_Distance_Present)
    {
        iCurOffset += 3;
    }

    if (gLns_LocationAndSpeedValue.flag.Location_Present)
    {
        iCurOffset += 4;
        iCurOffset += 4;
    }

    if (gLns_LocationAndSpeedValue.flag.Elevation_Present)
    {
        iCurOffset += 3;
    }

    if (gLns_LocationAndSpeedValue.flag.Heading_Present)
    {
        iCurOffset += 2;
    }


    if (gLns_LocationAndSpeedValue.flag.Rolling_Time_Present)
    {
        iCurOffset += 1;
    }

    if(iCurOffset > 20)
    {
        return iCurOffset-1;
    }


    if (gLns_LocationAndSpeedValue.flag.UTC_Time_Present)
    {
       iCurOffset +=7;
    }

    if(iCurOffset > 20)
    {
        return iCurOffset-7;
    }

    return iCurOffset;
    
}

/**
  * @brief send notification of Location And Speed Value.
  *
  * @param[in] ServiceId         service ID of service.
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool LNS_LocationAndSpeedValueNotify( uint8_t ServiceId )
{
    bool bRet= TRUE;

    LOCATION_AND_SPEED_VALUE_FLAG flag = {0};    
    uint8_t location_and_speed_value[20];

    
    uint8_t iCurOffset = 2;
    uint8_t iFirstNotifyValueOffset = LNS_GetFisrtValueOffsetToNotify();
    
    if (gLns_LocationAndSpeedValue.flag.Instantaneous_Speed_Present)
    {
        flag.Instantaneous_Speed_Present = 1;
        memcpy(&location_and_speed_value[iCurOffset], &gLns_LocationAndSpeedValue.Instantaneous_Speed, 2);
        iCurOffset += 2;
    }

    if (gLns_LocationAndSpeedValue.flag.Total_Distance_Present)
    {
        flag.Total_Distance_Present = 1;
        memcpy(&location_and_speed_value[iCurOffset], &gLns_LocationAndSpeedValue.Total_Distance, 3);
        iCurOffset += 3;
    }

    if (gLns_LocationAndSpeedValue.flag.Location_Present)
    {
        flag.Location_Present = 1;
        memcpy(&location_and_speed_value[iCurOffset], &gLns_LocationAndSpeedValue.Location_Latitude, 4);
        iCurOffset += 4;
        memcpy(&location_and_speed_value[iCurOffset], &gLns_LocationAndSpeedValue.Location_Longitute, 4);
        iCurOffset += 4;
    }

    if (gLns_LocationAndSpeedValue.flag.Elevation_Present)
    {
        flag.Elevation_Present = 1;    
        memcpy(&location_and_speed_value[iCurOffset], &gLns_LocationAndSpeedValue.Elevation, 3);
        iCurOffset += 3;
    }

    if (gLns_LocationAndSpeedValue.flag.Heading_Present)
    {
        flag.Heading_Present = 1;  
        memcpy(&location_and_speed_value[iCurOffset],
               &gLns_LocationAndSpeedValue.Heading, 2);
        iCurOffset += 2;
    }

    if(iCurOffset == iFirstNotifyValueOffset)
    {
        //copy flag
        memcpy(&location_and_speed_value[0], &flag, 2);
        //send first notification here
        ProfileAPI_SendData(ServiceId, LNS_LN_LOCATION_AND_SPEED_VALUE_INDEX, location_and_speed_value, iCurOffset);

        //send remain data
        memset(&flag, 0, 2);
        iCurOffset = 2;
        if (gLns_LocationAndSpeedValue.flag.Rolling_Time_Present)
        {
            memcpy(&location_and_speed_value[iCurOffset],
               &gLns_LocationAndSpeedValue.Rolling_Time, 1);
            flag.Rolling_Time_Present = 1;
            iCurOffset += 1;
        }

        if (gLns_LocationAndSpeedValue.flag.UTC_Time_Present)
        {

            memcpy(&location_and_speed_value[iCurOffset],
               &gLns_LocationAndSpeedValue.UTC_Time, 7);
            flag.UTC_Time_Present = 1;
            iCurOffset += 7;
        }
        memcpy(&location_and_speed_value[0], &flag, 2);
        //send second notification
        ProfileAPI_SendData(ServiceId, LNS_LN_LOCATION_AND_SPEED_VALUE_INDEX, location_and_speed_value, iCurOffset);
        
        return bRet;        
    }

    if (gLns_LocationAndSpeedValue.flag.Rolling_Time_Present)
    {
        flag.Rolling_Time_Present = 1;
        memcpy(&location_and_speed_value[iCurOffset],
               &gLns_LocationAndSpeedValue.Rolling_Time, 1);        
        iCurOffset += 1;
    }
    
    if(iCurOffset == iFirstNotifyValueOffset)
    {
        //copy flag        
        memcpy(&location_and_speed_value[0], &flag, 2);
        //send first notification here
        ProfileAPI_SendData(ServiceId, LNS_LN_LOCATION_AND_SPEED_VALUE_INDEX, location_and_speed_value, iCurOffset);

        
        //send remain data  
        memset(&flag, 0, 2);
        iCurOffset = 2;
        if (gLns_LocationAndSpeedValue.flag.UTC_Time_Present)
        {
            memcpy(&location_and_speed_value[iCurOffset],
               &gLns_LocationAndSpeedValue.UTC_Time, 7);
            flag.UTC_Time_Present = 1;
            iCurOffset += 7;
        }
        memcpy(&location_and_speed_value[0], &gLns_LocationAndSpeedValue.flag, 2);
        //send second notification
        ProfileAPI_SendData(ServiceId, LNS_LN_LOCATION_AND_SPEED_VALUE_INDEX, location_and_speed_value, iCurOffset);

        return bRet;        
    }

    if (gLns_LocationAndSpeedValue.flag.UTC_Time_Present)
    {
        memcpy(&location_and_speed_value[iCurOffset],
           &gLns_LocationAndSpeedValue.UTC_Time, 7);    
        flag.UTC_Time_Present = 1;
        iCurOffset += 7;
    }
    memcpy(&location_and_speed_value[0], &gLns_LocationAndSpeedValue.flag, 2);
    //send nortification    
    ProfileAPI_SendData(ServiceId, LNS_LN_LOCATION_AND_SPEED_VALUE_INDEX, location_and_speed_value, iCurOffset);



    return bRet;
}

/**
  * @brief send notification of Navigation Value.
  *
  * @param[in] ServiceId         service ID of service.
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool LNS_NavigationValueNotify( uint8_t ServiceId )
{
    uint8_t navigation_value[LNS_NAVIGATION_VALUE_MAX_LEN];
    uint8_t iCurOffset = 0;

    memcpy(&navigation_value[iCurOffset], &gLns_NavigationValue.flag, 2);
    iCurOffset += 2;
    memcpy(&navigation_value[iCurOffset], &gLns_NavigationValue.Bearing, 2);
    iCurOffset += 2;
    memcpy(&navigation_value[iCurOffset], &gLns_NavigationValue.Heading, 2);
    iCurOffset += 2;

    if (gLns_NavigationValue.flag.Remaining_Distance_Present)
    {
        memcpy(&navigation_value[iCurOffset], &gLns_NavigationValue.Reamining_Distance, 3);
        iCurOffset += 3;
    }

    if (gLns_NavigationValue.flag.Remaining_Vertical_Distance_Present)
    {
        memcpy(&navigation_value[iCurOffset], &gLns_NavigationValue.Reamining_Vertical_Distance, 3);
        iCurOffset += 3;
    }

    if (gLns_NavigationValue.flag.Estimated_Time_of_Arrival_Present)
    {
        memcpy(&navigation_value[iCurOffset], &gLns_NavigationValue.Estimated_Time_of_Arrival, 7);
        iCurOffset += 7;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- LNS_NavigationValueNotify", 0 );
    return ProfileAPI_SendData(ServiceId, LNS_LN_NAVIGATION_VALUE_INDEX, navigation_value, iCurOffset);
}

bool LNS_LNCtlPntValueIndicate( uint8_t ServiceId, uint8_t OpCode, uint8_t RspCode, 
    uint8_t *pParameter, uint8_t para_len)
{
    uint16_t attrib_index = LNS_LN_CP_VALUE_INDEX;

    uint8_t *pData;
    uint16_t dataLen;

    gLns_CtlPnt.value[0] = LN_CP_OPCODE_RESPONSE_CODE;
    gLns_CtlPnt.value[1] = OpCode;    /* Control Point request opcode. */
    gLns_CtlPnt.value[2] = RspCode;

    if(pParameter!= NULL && para_len!=0)
    {
        memcpy(&gLns_CtlPnt.value[3] , pParameter, para_len);
    }

    gLns_CtlPnt.cur_length = 3 * sizeof(uint8_t) + para_len;


    pData = gLns_CtlPnt.value;
    dataLen = gLns_CtlPnt.cur_length;

    // send indication to client
    return ProfileAPI_SendData(ServiceId, attrib_index, pData, dataLen);
}




static void LNS_CtlPntSetCumulativeValue( uint8_t serviceID , uint8_t rsp_code)
{
    uint8_t              op_code = LN_CP_OPCODE_SET_CUMULATIVE_VALUE;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, NULL, 0);
}

static void LNS_CtlPntMaskLocationSpeedCharContent( uint8_t serviceID  , uint8_t rsp_code)
{
    uint8_t              op_code = LN_CP_OPCODE_MASK_LOCATION_AND_SPEED_CHAR_CONTENT;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, NULL, 0);
}

static void LNS_CtlPntNavigationControl( uint8_t serviceID , uint8_t rsp_code)
{
    uint8_t              op_code = LN_CP_OPCODE_NAVIGATION_CONTROL;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, NULL, 0);
}

static void LNS_CtlPntRequestNumberOfRoutes( uint8_t serviceID , uint8_t rsp_code, uint16_t number_of_route)
{
    uint8_t              op_code = LN_CP_OPCODE_REQUEST_NUMBER_OF_ROUTES;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, (uint8_t*)&number_of_route, 2);
}

static void LNS_CtlPntRequestNameOfRoute( uint8_t serviceID,  uint8_t rsp_code, uint8_t*pNameOfRoute, uint8_t len)
{
    uint8_t              op_code = LN_CP_OPCODE_REQUEST_NAME_OF_ROUTE;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, pNameOfRoute, len);
}

static void LNS_CtlPntSelectRoute( uint8_t serviceID, uint8_t rsp_code)
{
    uint8_t              op_code = LN_CP_OPCODE_SELECT_ROUTE;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, NULL, 0);
}

static void LNS_CtlPntSetFixRate( uint8_t serviceID , uint8_t rsp_code)
{
    uint8_t              op_code = LN_CP_OPCODE_SET_FIX_RATE;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, NULL, 0);
}

static void LNS_CtlPntSetElevation( uint8_t serviceID, uint8_t rsp_code)
{
    uint8_t              op_code = LN_CP_OPCODE_SET_ELEVATION;
    LNS_LNCtlPntValueIndicate( serviceID, op_code, rsp_code, NULL, 0);
}


static uint8_t  LNS_Hanlde_CtlPntProc2( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    TLNS_CALLBACK_DATA callback_data;
    uint8_t RespCode  = LN_CP_RSPCODE_SUCCESS;
    uint16_t parameter_length = 0;
    memcpy( gLns_CtlPnt.value, value_ptr, write_length );
    if(write_length>=1)
    {
        parameter_length = write_length - 1;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "    LNS_Hanlde_CtlPntProc request: OpCode=0x%x", 1,
               gLns_CtlPnt.value[0] );


    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.msg_data.write.opcode = (TLN_CP_OPCODE)gLns_CtlPnt.value[0];
        
    switch ( gLns_CtlPnt.value[0] )
    {
    case LN_CP_OPCODE_NAVIGATION_CONTROL:
        {
            if(parameter_length == 1)
            {
                memcpy(&callback_data.msg_data.write.cp_parameter.navigation_control, 
                            &gLns_CtlPnt.value[1], 1);
            }
            else
            {
                RespCode = LN_CP_RSPCODE_INVALID_PARAMETER;
            }
        }
        
        break;

    case LN_CP_OPCODE_SET_CUMULATIVE_VALUE:
    {
        if(parameter_length == 3)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.cumulative_total_distance, 
                        &gLns_CtlPnt.value[1], 3);
        }
        else
        {
            RespCode = LN_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;
    
    case LN_CP_OPCODE_MASK_LOCATION_AND_SPEED_CHAR_CONTENT:
    {
        if(parameter_length == 2)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.mask_location_and_speed, 
                        &gLns_CtlPnt.value[1], 2);
        }
        else
        {
            RespCode = LN_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;       
    
    case LN_CP_OPCODE_REQUEST_NUMBER_OF_ROUTES:
    {
        //no parameter
    }
    break;        
    case LN_CP_OPCODE_REQUEST_NAME_OF_ROUTE:
    {
        if(parameter_length == 2)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.number_of_desired_route, 
                        &gLns_CtlPnt.value[1], 1);
        }
        else
        {
            RespCode = LN_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;        
    
    case LN_CP_OPCODE_SELECT_ROUTE:
    {
        if(parameter_length == 2)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.select_route_desired_route_number, 
                        &gLns_CtlPnt.value[1], 2);
        }
        else
        {
            RespCode = LN_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;    

    case LN_CP_OPCODE_SET_FIX_RATE:
    {
        if(parameter_length == 1)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.set_fix_rate, 
                        &gLns_CtlPnt.value[1], 1);
        }
        else
        {
            RespCode = LN_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;

    case LN_CP_OPCODE_SET_ELEVATION:
    {
        if(parameter_length == 3)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.set_elevation, 
                        &gLns_CtlPnt.value[1], 3);
        }
        else
        {
            RespCode = LN_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;

    default:
    {
            RespCode = LN_CP_RSPCODE_OPCODE_UNSUPPORT;        
    }
    break;
    }

    if(RespCode == LN_CP_RSPCODE_SUCCESS)
    {
        pfnLnsAppCB(serviceID, (void*)&callback_data);
    }
    return RespCode;
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
static TProfileResult  LNS_Hanlde_CtlPntProc( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    TProfileResult  wCause  = ProfileResult_Success;
    memcpy( gLns_CtlPnt.value, value_ptr, write_length );

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "LNS_Hanlde_CtlPntProc request: OpCode=0x%x", 1,
               gLns_CtlPnt.value[0] );
        
    switch ( gLns_CtlPnt.value[0] )
    {
    case LN_CP_OPCODE_NAVIGATION_CONTROL:
        {
            if (!gLns_NotifyIndicateFlag.navigation_enable)
            {
                wCause = ProfileResult_AppErr_CccdImproperlyConfigured;
            }
        }
        break;

    default:
    {
    }
    break;
    }
    return wCause;
}

static void  LNS_CtlPntWriteIndPostProc( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    uint8_t   RespCode  = LN_CP_RSPCODE_SUCCESS;
    
    BOOL bError = FALSE;
    memcpy( gLns_CtlPnt.value, value_ptr, write_length );
    gLns_CtlPnt.cur_length = write_length;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "LNS_CtlPntWriteIndPostProc: OpCode=0x%x", 1,
               gLns_CtlPnt.value[0] );

    RespCode = LNS_Hanlde_CtlPntProc2(serviceID, write_length, value_ptr);

    switch ( gLns_CtlPnt.value[0] )
    {
    default:
        {
            bError = TRUE;
        }
        break;

    case LN_CP_OPCODE_SET_CUMULATIVE_VALUE:
        {
            LNS_CtlPntSetCumulativeValue(serviceID, RespCode);
        }
        break;

    case LN_CP_OPCODE_MASK_LOCATION_AND_SPEED_CHAR_CONTENT:
        {
            LNS_CtlPntMaskLocationSpeedCharContent( serviceID , RespCode);
        }
        break;

    case LN_CP_OPCODE_NAVIGATION_CONTROL:
        {

            LNS_CtlPntNavigationControl( serviceID, RespCode);
        }
        break;

    case LN_CP_OPCODE_REQUEST_NUMBER_OF_ROUTES:
        {
            LNS_CtlPntRequestNumberOfRoutes(  serviceID , RespCode, gLns_CP_NumberOfRoute);
        }
        break;
    case LN_CP_OPCODE_REQUEST_NAME_OF_ROUTE:
        {
            LNS_CtlPntRequestNameOfRoute(  serviceID, RespCode, gLns_CP_NameOfRoute, gLns_CP_NameOfRouteLen);
        }
        break;

    case LN_CP_OPCODE_SELECT_ROUTE:
        {
            LNS_CtlPntSelectRoute(  serviceID, RespCode);
        }
        break;

    case LN_CP_OPCODE_SET_FIX_RATE:
        {
            LNS_CtlPntSetFixRate(  serviceID, RespCode);
        }
        break;

    case LN_CP_OPCODE_SET_ELEVATION:
        {
            LNS_CtlPntSetElevation(  serviceID, RespCode);
        }
        break;


    }

    if (bError)
    {
        LNS_LNCtlPntValueIndicate(serviceID, gLns_CtlPnt.value[0], LN_CP_RSPCODE_OPCODE_UNSUPPORT, NULL, 0);
    }

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
TProfileResult  LnsAttrReadCb(uint8_t ServiceID, uint16_t iAttribIndex,
                              uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue)
{
    TProfileResult  wCause  = ProfileResult_Success;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "LnsAttrReadCb  iAttribIndex = %d iOffset %x", 2, iAttribIndex, iOffset);

    *piLength = 0;

    switch (iAttribIndex)
    {
    case LNS_LN_FEATURE_VALUE_INDEX:
        {
            *piLength = sizeof(gLns_LnFeatureSupport);
            *ppValue = (uint8_t *)&gLns_LnFeatureSupport;
        }
        break;

    case LNS_LN_POSITION_QUALITY_VALUE_INDEX:
        {
            TLNS_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = LNS_READ_POSITION_QUALITY_VALUE;            
            pfnLnsAppCB(ServiceID, (void*)&callback_data);
            //after app call LNS_SetParameter to set Position Quality Value, profile will response this value to remote device
            LNS_FormatPQValueForRead();
            *piLength = gLns_PQValueLengthForReadCfm;
            *ppValue = gLns_PQValueForReadCfm;
        }
        break;

    default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "LnsAttrReadCb  iAttribIndex = %d not found", 1, iAttribIndex);
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
TProfileResult LnsAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex,
                              uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "LnsAttrWriteCb  iAttribIndex = %d wLength %x", 2, iAttribIndex, wLength);

    switch (iAttribIndex)
    {
    case LNS_LN_CP_VALUE_INDEX:
        {
            /* Attribute value has variable size, make sure written value size is valid. */
            if ( (wLength > LNS_MAX_CTL_PNT_VALUE) || (pValue == NULL) )
            {
                wCause  = ProfileResult_InvalidValueSize;
            }
            /* Make sure Control Point is not "Process already in progress". */
            else if ( LN_CTL_PNT_OPERATE_ACTIVE( gLns_CtlPnt.value[0] ) )
            {
                wCause  = ProfileResult_AppErr_ProcAlreadyInProgress;
            }
            /* Make sure Control Point is configured indication enable. */
            else if (!gLns_NotifyIndicateFlag.ln_cp_indicate_enable)
            {
                wCause = ProfileResult_AppErr_CccdImproperlyConfigured;
            }
            else
            {
                wCause = LNS_Hanlde_CtlPntProc(ServiceId, wLength, pValue);
                if(wCause == ProfileResult_Success)
                {
                    *pWriteIndPostProc = LNS_CtlPntWriteIndPostProc;
                }
                else
                {
                    *pWriteIndPostProc = NULL;
                }
            }

        }
        break;

    default:
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "LnsAttrWriteCb  iAttribIndex = %d not found", 1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
        }
        break;
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
void LnsCccdUpdateCb(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
    TLNS_CALLBACK_DATA callback_data;
    bool bHandle = TRUE;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "LnsCccdUpdateCb  Index = %d wCCCDBits %x", 2, Index, wCCCBits);

    switch (Index)
    {
    case LNS_LN_LOCATION_AND_SPEED_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                gLns_NotifyIndicateFlag.location_and_speed_notify_enable = 1;
                callback_data.msg_data.notification_indification_index = LNS_NOTIFY_INDICATE_LOCATION_AND_SPEED_ENABLE;
            }
            else
            {
                gLns_NotifyIndicateFlag.location_and_speed_notify_enable = 0;
                callback_data.msg_data.notification_indification_index = LNS_NOTIFY_INDICATE_LOCATION_AND_SPEED_DISABLE;
            }
        }
        break;

    case LNS_LN_CP_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_INDICATION_BIT)
            {
                // Enable Indofication
                gLns_NotifyIndicateFlag.ln_cp_indicate_enable = 1;
                callback_data.msg_data.notification_indification_index = LNS_NOTIFY_INDICATE_CP_ENABLE;
            }
            else
            {
                gLns_NotifyIndicateFlag.ln_cp_indicate_enable = 0;
                callback_data.msg_data.notification_indification_index = LNS_NOTIFY_INDICATE_CP_DISABLE;
            }

        }
        break;

    case LNS_LN_NAVIGATION_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                gLns_NotifyIndicateFlag.navigation_enable = 1;
                callback_data.msg_data.notification_indification_index = LNS_NOTIFY_INDICATE_NAVIGATION_ENABLE;
            }
            else
            {
                gLns_NotifyIndicateFlag.navigation_enable = 0;
                callback_data.msg_data.notification_indification_index = LNS_NOTIFY_INDICATE_NAVIGATION_DISABLE;
            }
        }
        break;

    default:
        {
            bHandle = FALSE;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "LnsCccdUpdateCb  Index = %d not found", 1, Index);
        }
        break;

    }
    /* Notify Application. */
    if (pfnLnsAppCB && (bHandle == TRUE))
    {
        callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;        
        pfnLnsAppCB(ServiceId, (void*)&callback_data);
    }
    return;
}

/**
 * @brief LOCATION AND NAVIGATION Service Callbacks.
*/
CONST gattServiceCBs_t lnsCBs =
{
    LnsAttrReadCb,  // Read callback function pointer
    LnsAttrWriteCb, // Write callback function pointer
    LnsCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add lns service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t LNS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)LnsAttrTbl,
                                       iLnsAttrTblSize,
                                       lnsCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "LNS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }
    pfnLnsAppCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}


