enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     csc_service.c
* @brief    CSC profile interfaces called in profile.c.
* @details  Interfaces to get and put csc characteristics value and csc control point procedure.
* @author   ethan_su
* @date     2014-10-13
* @version  v0.1
*********************************************************************************************************
*/
#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "profileApi.h"
#include "csc_service.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
typedef struct _CSC_NOTIFY_INDICATE_FLAG
{
    uint8_t csc_measurement_notify_enable: 1;
    uint8_t sc_cp_indicate_enable: 1;
    uint8_t rfu: 6;
} CSC_NOTIFY_INDICATE_FLAG;


/**<  CSC measurement data. */
static TCSCMeasurement CSC_Measurement;
/**<  CSC feature that CSC service supported, can be configured by user. */
static uint16_t  CSC_Feature;
/**<  CSC current sensor location, where the CSC sensor located. */
static uint8_t CSC_CurSensLocation;
/**<  CSC supported sensor location list, can be configured by user. */
static uint16_t CSC_SensLocList;
/**<  CSC control point data. */
static TCSCControlPoint  CSC_ControlPoint = {0};
/**<  CSC control point indication enable flag. */
static CSC_NOTIFY_INDICATE_FLAG gCsc_NotifyIndicateFlag = {0};
/**<  Function pointer used to send event to application from CSC profile. Initiated in CSC_AddService. */
static pfnAPPHandleInfoCB_t pfnCscCB = NULL;

/**< @brief  profile/service definition.  */
const TAttribAppl CscAttrTbl[] =
{
    /*-------------------------- CSC Service ---------------------------*/
    /* <<Primary Service>>, ..  Index 0 */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_CSC),               /* service UUID */
            HI_WORD(GATT_UUID_CSC)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, ..  Index 1 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY,                    /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- CSC Measurement characteristic value --- Index 2 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CSC_MEASUREMENT),
            HI_WORD(GATT_UUID_CHAR_CSC_MEASUREMENT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration  Index 3 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
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

    /* <<Characteristic>>, ..  Index 4 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                       /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- CSC Features characteristic value --- Index 5 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CSC_FEATURE),
            HI_WORD(GATT_UUID_CHAR_CSC_FEATURE),
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    }

#ifdef CSC_MULTIPLE_SENSOR_LOCATIONS_SUPPORT
    ,
    /* <<Characteristic>>, ..  Index 6 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                    /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- CSC sensor location characteristic value --- Index 7 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SENSOR_LOCATION),
            HI_WORD(GATT_UUID_CHAR_SENSOR_LOCATION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef CSC_SC_CONTROL_POINT_SUPPORT
     ,
    /* <<Characteristic>>, ..  Index 8 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE |                   /* characteristic properties */
            GATT_CHAR_PROP_INDICATE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- CSC SC Control Point value --- Index 9 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SC_CONTROL_POINT),
            HI_WORD(GATT_UUID_CHAR_SC_CONTROL_POINT)
        },
        0,                                          /* bValueLen, 0 : variable length */
        NULL,
        GATT_PERM_WRITE                             /* wPermissions */
    },
    /* client characteristic configuration  Index 10 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
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
/**< @brief  CSC service size definition.  */
const int32_t iCscAttrTblSize = sizeof(CscAttrTbl);

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref CSC_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool CSC_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;
    uint8_t inc_flag = CSC_Measurement.value[0];

    switch (param_type)
    {
    default:
        ret = FALSE;
        break;
    case CSC_PARAM_CSC_FEATURE:
        memcpy(&CSC_Feature, value_ptr, 2);
        break;
    
    case CSC_PARAM_INC_FLAG:
        CSC_Measurement.value[0] = *(uint8_t *)value_ptr;
        inc_flag = CSC_Measurement.value[0];
        CSC_Measurement.cur_length = 1;
        if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
            CSC_Measurement.cur_length += 6;
        if ( inc_flag & CSC_INC_CRANK_REVOL_MASK )
            CSC_Measurement.cur_length += 4;
        break;
    case CSC_PARAM_WHEEL_REVOL:
        if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
        {
            LE_DWORD2EXTRN(CSC_Measurement.value + 1, *(uint32_t *)value_ptr);
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_WHEEL_EVT_TIME:
        if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
        {
            LE_WORD2EXTRN(CSC_Measurement.value + 5, *(uint16_t *)value_ptr);
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_CRANK_REVOL:
        if ( inc_flag & CSC_INC_CRANK_REVOL_MASK )
        {
            if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
            {
                LE_WORD2EXTRN(CSC_Measurement.value + 7, *(uint16_t *)value_ptr);
            }
            else
            {
                LE_WORD2EXTRN(CSC_Measurement.value + 1, *(uint16_t *)value_ptr);
            }
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_CRANK_EVT_TIME:
        if ( inc_flag & CSC_INC_CRANK_REVOL_MASK )
        {
            if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
            {
                LE_WORD2EXTRN(CSC_Measurement.value + 9, *(uint16_t *)value_ptr);
            }
            else
            {
                LE_WORD2EXTRN(CSC_Measurement.value + 3, *(uint16_t *)value_ptr);
            }
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_SENSOR_LOC:
        CSC_CurSensLocation = *(uint8_t *)value_ptr;
        break;
    case CSC_PARAM_CTL_PNT_PROG_CLR:
        CSC_ControlPoint.value[0] = CSC_CP_OPCODE_RESERVED;
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "CSC parameter set failed\n", 0 );

    return ret;
}

/**
  * @brief  Get service related data by application.
  *
  * @param[in] param_type            parameter type to set: @ref CSC_Application_Parameters
  * @param[out] value_ptr             value to set.
  * @return parameter get result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool CSC_GetParameter( uint8_t param_type, void *value_ptr)
{
    bool ret = TRUE;
    uint8_t inc_flag = CSC_Measurement.value[0];

    switch (param_type)
    {
    default:
        ret = FALSE;
        break;
    case CSC_PARAM_INC_FLAG:
        *(uint8_t *)value_ptr = CSC_Measurement.value[0];
        break;
    case CSC_PARAM_WHEEL_REVOL:
        if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
        {
            *(uint16_t *)value_ptr = LE_EXTRN2DWORD(CSC_Measurement.value + 1);
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_WHEEL_EVT_TIME:
        if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
        {
            *(uint16_t *)value_ptr = LE_EXTRN2WORD(CSC_Measurement.value + 5);
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_CRANK_REVOL:
        if ( inc_flag & CSC_INC_CRANK_REVOL_MASK )
        {
            if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
            {
                *(uint16_t *)value_ptr = LE_EXTRN2WORD(CSC_Measurement.value + 7);
            }
            else
            {
                *(uint16_t *)value_ptr = LE_EXTRN2WORD(CSC_Measurement.value + 1);
            }
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_CRANK_EVT_TIME:
        if ( inc_flag & CSC_INC_CRANK_REVOL_MASK )
        {
            if ( inc_flag & CSC_INC_WHEEL_REVOL_MASK )
            {
                *(uint16_t *)value_ptr = LE_EXTRN2WORD(CSC_Measurement.value + 9);
            }
            else
            {
                *(uint16_t *)value_ptr = LE_EXTRN2WORD(CSC_Measurement.value + 3);
            }
        }
        else
        {
            ret = FALSE;
        }
        break;
    case CSC_PARAM_SENSOR_LOC:
        *(uint8_t *)value_ptr = CSC_CurSensLocation;
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "CSC parameter get failed\n", 0 );

    return ret;
}

/**
 * @brief  display control point response.
 *
 * @param CSC_ctl_pnt_ptr       pointer to CSC control point data.
 * @return none
 * @retval  void
*/
static void  CSC_CtlPntDisplayRsp( PCSCControlPoint CSC_ctl_pnt_ptr )
{
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "CSCCP response: ReqOpCode=0x%x", 1,
               CSC_ctl_pnt_ptr->value[1] );
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "                RspCode=0x%x", 1,
               CSC_ctl_pnt_ptr->value[2] );
}

/**
 * @brief  whether the sensor location supported.
 *
 * @param sens_location     sensor location to be judged(CSC sensor support or not).
 * @return support result.
 * @retval  1   support
 * @retval  0   not support
*/
static bool CSC_SensLocSupported( uint8_t sens_location )
{
    uint8_t     result = FALSE;
    if ( (CSC_SensLocList >> sens_location) & 0x0001 )
    {
        result = TRUE;
    }

    return (result);
}

/**
 * @brief  set cumulative response.
 *
 * @param serviceID
 * @return none
 * @retval  void
*/
static void CSC_CtlPntSetCumulative( uint8_t serviceID, uint8_t rsp_code)
{
    uint8_t              op_code = CSC_CP_OPCODE_SET_CUMULATIVE;
    CSC_CtlPntIndicate( serviceID, op_code, rsp_code );
}

/**
 * @brief  update sensor location response.
 *
 * @param serviceID
 * @return none
 * @retval  void
*/
static void CSC_CtlPntUpdateSensLoc( uint8_t serviceID, uint8_t rsp_code )
{
    uint8_t              op_code = CSC_CP_OPCODE_UPDATE_SENS_LOC;
    CSC_CtlPntIndicate( serviceID, op_code, rsp_code );
}

/**
 * @brief  supported sensor location response.
 *
 * @param serviceID
 * @return none
 * @retval  void
*/
static void CSC_CtlPntReqSensLocList( uint8_t serviceID, uint8_t rsp_code)
{
    uint8_t              op_code = CSC_CP_OPCODE_REQ_SENS_LOC_LIST;
    CSC_CtlPntIndicate( serviceID, op_code, rsp_code );
}

static uint8_t  CSC_HanldeCtlPntProc2( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    TCSC_CALLBACK_DATA callback_data;
    uint8_t RespCode  = CSC_CP_RSPCODE_SUCCESS;
    uint16_t parameter_length = 0;
    memcpy( CSC_ControlPoint.value, value_ptr, write_length );
    if(write_length>=1)
    {
        parameter_length = write_length - 1;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "    CSC_Hanlde_CtlPntProc2 request: OpCode=0x%x", 1,
               CSC_ControlPoint.value[0] );


    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.msg_data.write.opcode = CSC_ControlPoint.value[0];
        
    switch ( CSC_ControlPoint.value[0] )
    {
    
    case CSC_CP_OPCODE_SET_CUMULATIVE:
        {
            if(parameter_length == 4)
            {
                memcpy(&callback_data.msg_data.write.cp_parameter.cumulative_value, 
                            &CSC_ControlPoint.value[1], 4);
            }
            else
            {
                RespCode = CSC_CP_RSPCODE_INVALID_PARAMETER;
            }
        }
        
        break;

    case CSC_CP_OPCODE_UPDATE_SENS_LOC:
    {
        if(parameter_length == 1)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.sensor_location_value, 
                        &CSC_ControlPoint.value[1], 1);
            if(callback_data.msg_data.write.cp_parameter.sensor_location_value>= CSC_SENSOR_LOC_MAX)
            {
                RespCode = CSC_CP_RSPCODE_INVALID_PARAMETER;
            }
        }
        else
        {
            RespCode = CSC_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;
    
    case CSC_CP_OPCODE_REQ_SENS_LOC_LIST:
    {
        if(parameter_length == 0)
        {
        }
        else
        {
            RespCode = CSC_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;       
    


    default:
    {
            RespCode = CSC_CP_RSPCODE_OPCODE_UNSUPPORT;        
    }
    break;
    }

    if(RespCode == CSC_CP_RSPCODE_SUCCESS)
    {
        pfnCscCB(serviceID, (void*)&callback_data);
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
static void  CSC_CtlPntHandleReq( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    uint8_t   RespCode  = CSC_CP_RSPCODE_SUCCESS;

    memcpy( CSC_ControlPoint.value, value_ptr, write_length );
    CSC_ControlPoint.cur_length = write_length;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "    CSCCP request: OpCode=0x%x", 1,
               CSC_ControlPoint.value[0] );

    RespCode = CSC_HanldeCtlPntProc2(serviceID, write_length, value_ptr);

    if(RespCode == CSC_CP_RSPCODE_SUCCESS)
    {
        switch ( CSC_ControlPoint.value[0] )
        {
        default:
            RespCode = CSC_CP_RSPCODE_OPCODE_UNSUPPORT;
            break;
        case CSC_CP_OPCODE_SET_CUMULATIVE:
            if ( (CSC_Feature & CSC_SUPPORT_WHEEL_REVOL_MASK) &&
                    (CSC_Measurement.value[0] & CSC_INC_WHEEL_REVOL_MASK) )
            {
                CSC_CtlPntSetCumulative(serviceID, RespCode);
                return;
            }
            break;
        case CSC_CP_OPCODE_UPDATE_SENS_LOC:
            if ( CSC_Feature & CSC_SUPPORT_MULTI_SENSOR_MASK )
            {
                CSC_CtlPntUpdateSensLoc(serviceID,  RespCode);
                return;
            }
            break;
        case CSC_CP_OPCODE_REQ_SENS_LOC_LIST:
            if ( CSC_Feature & CSC_SUPPORT_MULTI_SENSOR_MASK )
            {
                CSC_CtlPntReqSensLocList(serviceID,  RespCode);
                return;
            }
            break;
        }
    }

    /* Send indication to client when request opcode not supported. */
    CSC_CtlPntIndicate(serviceID, CSC_ControlPoint.value[0], RespCode);
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
TProfileResult  CSC_AttrRead( uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;

    switch ( iAttribIndex )
    {
    default:
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- CSC_AttrRead, Attr not found, index=%d", 1, iAttribIndex);
        wCause  = ProfileResult_AttrNotFound;
        break;
    case GATT_SVC_CSC_FEATURE_INDEX:
        {
            TCSC_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = CSC_READ_CSC_FEATURE;
            pfnCscCB(ServiceId, &callback_data);
                
            *ppValue = (uint8_t *)&CSC_Feature;
            *piLength = sizeof(CSC_Feature);
        }
        break;
    case GATT_SVC_CSC_SENS_LOC_INDEX:
        if ( CSC_Feature & CSC_SUPPORT_MULTI_SENSOR_MASK )
        {
            TCSC_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = CSC_READ_SENSOR_LOCATION;
            pfnCscCB(ServiceId, &callback_data);
            *ppValue = (uint8_t *)&CSC_CurSensLocation;
            *piLength = sizeof(CSC_CurSensLocation);
        }
        else
        {
            wCause  = ProfileResult_AttrNotFound;
        }
        break;
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
 * @param pWriteIndPostProc pointer of a function to handle control point write.
 * @return Profile procedure result
*/
TProfileResult CSC_AttrWrite(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t wLength, uint8_t * pValue, TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    if (GATT_SVC_CSC_CTL_PNT_INDEX == iAttribIndex)
    {
        /* Attribute value has variable size, make sure written value size is valid. */
        if ( (wLength > CSC_MAX_CTL_PNT_VALUE) || (pValue == NULL) )
        {
            wCause  = ProfileResult_InvalidValueSize;
        }
        /* Make sure Control Point is not "Process already in progress". */
        else if ( CSC_CTL_PNT_OPERATE_ACTIVE( CSC_ControlPoint.value[0] ) )
        {
            wCause  = (TProfileResult)0x80;//ProfileResult_AppErr_ProcAlreadyInProgress
        }
        /* Make sure Control Point is configured indication enable. */
        else if (FALSE == gCsc_NotifyIndicateFlag.sc_cp_indicate_enable)
        {
            wCause = (TProfileResult)0x81;//ProfileResult_AppErr_CccdImproperlyConfigured
        }
        else
        {
            /* handle SCCP request after sending write response */
            *pWriteIndPostProc = CSC_CtlPntHandleReq;
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> CSC_AttrWrite Error  iAttribIndex = 0x%x wLength=%d",
                   2, iAttribIndex, wLength);
        wCause = ProfileResult_AttrNotFound;
    }

    return wCause;
}

/**
  * @brief send notification of csc measurement value of csc server.
  *
  * @param[in] ServiceId         service ID of service.
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool CSC_MeasValueNotify( uint8_t ServiceId )
{
    uint16_t attrib_index = GATT_SVC_CSC_MEASUREMENT_INDEX;
    uint8_t *pData = CSC_Measurement.value;
    uint16_t dataLen = CSC_Measurement.cur_length;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- CSC Measurement Notification", 0 );
    return ProfileAPI_SendData(ServiceId, attrib_index, pData, dataLen);
}

/**
  * @brief  send response to client, control point indication procedure.
  *
  * @param[in] ServiceId         service ID of service.
  * @param[in] OpCode            control point request opcode type.
  * @param[in] RspCode           control point response code to indicate to client.
  * @return indication action result
  * @retval  1 TRUE
  * @retval  0 FALSE
  */
bool CSC_CtlPntIndicate( uint8_t ServiceId, uint8_t OpCode, uint8_t RspCode )
{
    uint16_t attrib_index = GATT_SVC_CSC_CTL_PNT_INDEX;
    uint8_t sens_location;
    uint8_t param_offset;
    uint8_t *pData;
    uint16_t dataLen;

    CSC_ControlPoint.value[1] = CSC_ControlPoint.value[0];  /* Control Point request opcode. */
    CSC_ControlPoint.value[2] = RspCode;
    CSC_ControlPoint.value[0] = CSC_CP_OPCODE_RSP_CODE;
    CSC_ControlPoint.cur_length = 3 * sizeof(uint8_t);

    /* Diff RspType, different indication contents. */
    if (OpCode == CSC_CP_OPCODE_REQ_SENS_LOC_LIST)
    {
        /* get sensor location list */
        for (sens_location = CSC_SENSOR_LOC_OTHER, param_offset = 3; sens_location < CSC_SENSOR_LOC_MAX; sens_location++)
        {
            if (CSC_SensLocSupported(sens_location))
            {
                CSC_ControlPoint.value[param_offset] = sens_location;
                param_offset++;
            }
        }
        CSC_ControlPoint.cur_length = param_offset;
    }

    CSC_CtlPntDisplayRsp(&CSC_ControlPoint);
    pData = CSC_ControlPoint.value;
    dataLen = CSC_ControlPoint.cur_length;

    // send indication to client
    return ProfileAPI_SendData(ServiceId, attrib_index, pData, dataLen);
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void CSC_CccdUpdate(uint8_t serviceId, uint16_t Index, uint16_t wCCCBits)
{
    TCSC_CALLBACK_DATA callback_data;
    bool bHandle = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "CSC_CccdUpdate Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
    case GATT_SVC_CSC_MEAS_CCCD_INDEX:
        if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = CSC_NOTIFY_INDICATE_MEASUREMENT_ENABLE;
            gCsc_NotifyIndicateFlag.csc_measurement_notify_enable = TRUE;
        }
        else
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = CSC_NOTIFY_INDICATE_MEASUREMENT_ENABLE;
            gCsc_NotifyIndicateFlag.csc_measurement_notify_enable = FALSE;
        }
        bHandle =  TRUE;
        break;
    case GATT_SVC_CSC_CTL_PNT_CCCD_INDEX:
        if (wCCCBits & GATT_CCCD_INDICATION_BIT)
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = CSC_NOTIFY_INDICATE_SC_CP_ENABLE;
            gCsc_NotifyIndicateFlag.sc_cp_indicate_enable = TRUE;
            
        }
        else
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = CSC_NOTIFY_INDICATE_SC_CP_DISABLE;
            gCsc_NotifyIndicateFlag.sc_cp_indicate_enable = FALSE;
        }
        bHandle =  TRUE;        
        break;
    default:
        break;
    }
    /* Notify Application. */
    if (pfnCscCB && (bHandle == TRUE))
    {
        pfnCscCB(serviceId, (void*)&callback_data);
    }
}

/**
 * @brief CSC Service Callbacks.
*/
CONST gattServiceCBs_t cscCBs =
{
    CSC_AttrRead,  // Read callback function pointer
    CSC_AttrWrite, // Write callback function pointer
    CSC_CccdUpdate  // CCCD update callback function pointer
};

/**
  * @brief add CSC service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t CSC_AddService(void* pFunc)
{
    uint8_t ServiceId;

    /* Initiate CSC service related data, modify according to user's demand. */
    CSC_Feature = CSC_ALL_FEATURE_SUPPORTED;
    CSC_Measurement.value[0] = CSC_INC_ALL_PRESENTS;
    CSC_Measurement.cur_length = CSC_MAX_MEASUREMENT_VALUE;
    CSC_SensLocList = CSC_ALL_SENS_LOC_SUPPORTED;
    CSC_CurSensLocation = CSC_SENSOR_LOC_REAL_WHEEL;

    /* register CSC service to profile layer. */
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)CscAttrTbl,
                                       iCscAttrTblSize,
                                       cscCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "CSC_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }

    /* register callback for profile to inform application that some events happened. */
    pfnCscCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}


