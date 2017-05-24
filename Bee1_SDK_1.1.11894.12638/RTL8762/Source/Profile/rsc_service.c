enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rsc_service.c
* @brief    RSC service source file.
* @details  Interfaces to get and put rsc characteristics value and rsc control point procedure.
* @author   ethan_su
* @date     2014-10-10
* @version  v0.1
*********************************************************************************************************
*/
#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "profileApi.h"
#include "rsc_service.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

typedef struct _RSC_NOTIFY_INDICATE_FLAG
{
    uint8_t rsc_measurement_notify_enable: 1;
    uint8_t sc_cp_indicate_enable: 1;
    uint8_t rfu: 6;
} RSC_NOTIFY_INDICATE_FLAG;

/**<  RSC measurement data. */
static TRSCMeasurement RSC_Measurement;
/**<  RSC feature that RSC service supported, can be configured by user. */
static uint16_t RSC_Feature;
/**<  RSC current sensor location, where the RSC sensor located. */
static uint8_t RSC_CurSensLocation;
/**<  RSC supported sensor location list, can be configured by user. */
static uint16_t RSC_SensLocList;
/**<  RSC control point data. */
static TRSCControlPoint RSC_ControlPoint = {0};
/**<  RSC control point indication enable flag. */
static RSC_NOTIFY_INDICATE_FLAG gRsc_NotifyIndicateFlag = {0};
/**<  Function pointer used to send event to application from RSC profile. Initiated in RSC_AddService. */
static pfnAPPHandleInfoCB_t pfnRscCB = NULL;

/**< @brief  profile/service definition.  */
const TAttribAppl RscAttrTbl[] =
{
    /*-------------------------- RSC Service ---------------------------*/
    /* <<Primary Service>>, ..  Index 0 */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_RSC),               /* service UUID */
            HI_WORD(GATT_UUID_RSC)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, ..  Index 1 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
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
    /*--- RSC Measurement characteristic value --- Index 2 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_RSC_MEASUREMENT),
            HI_WORD(GATT_UUID_CHAR_RSC_MEASUREMENT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_NONE                              /* wPermissions */
    },
    /* client characteristic configuration  Index 3 */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
        {   /* bTypeValue */
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
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                       /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*--- RSC Features characteristic value --- Index 5 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_RSC_FEATURE),
            HI_WORD(GATT_UUID_CHAR_RSC_FEATURE),
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    }

#ifdef RSC_MULTIPLE_SENSOR_LOCATIONS_SUPPORT
    ,
    /* <<Characteristic>>, ..  Index 6 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
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
    /*--- RSC sensor location characteristic value --- Index 7 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SENSOR_LOCATION),
            HI_WORD(GATT_UUID_CHAR_SENSOR_LOCATION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef RSC_SC_CONTROL_POINT_SUPPORT
    ,
    /* <<Characteristic>>, ..  Index 8 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
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
    /*--- RSC SC Control Point value --- Index 9 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
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
        {   /* bTypeValue */
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
/**< @brief  RSC service size definition.  */
const int32_t iRscAttrTblSize = sizeof(RscAttrTbl);

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref RSC_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool RSC_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;
    uint8_t inc_flag = RSC_Measurement.value[0];

    switch (param_type)
    {
    default:
        ret = FALSE;
        break;
    case RSC_PARAM_CSC_FEATURE:
        memcpy(&RSC_Feature, value_ptr, 2);
        break;
    case RSC_PARAM_INC_FLAG:
        RSC_Measurement.value[0] = *(uint8_t *)value_ptr;
        inc_flag = RSC_Measurement.value[0];
        RSC_Measurement.cur_length = 4;
        if ( inc_flag & RSC_INC_STRIDE_LENGTH_MASK )
            RSC_Measurement.cur_length += 2;
        if ( inc_flag & RSC_INC_TOTAL_DISTANCE_MASK )
            RSC_Measurement.cur_length += 4;
        break;
    case RSC_PARAM_SPEED:
        LE_WORD2EXTRN(RSC_Measurement.value + 1, *(uint16_t *)value_ptr);
        break;
    case RSC_PARAM_CADENCE:
        RSC_Measurement.value[3] = *(uint8_t *)value_ptr;
        break;
    case RSC_PARAM_STRIDE_LENGTH:
        if ( inc_flag & RSC_INC_STRIDE_LENGTH_MASK )
        {
            LE_WORD2EXTRN(RSC_Measurement.value + 4, *(uint16_t *)value_ptr) ;
        }
        else
        {
            ret = FALSE;
        }
        break;
    case RSC_PARAM_TOTALDISTANCE:
        if ( inc_flag & RSC_INC_TOTAL_DISTANCE_MASK )
        {
            if ( inc_flag & RSC_INC_STRIDE_LENGTH_MASK )
            {
                LE_DWORD2EXTRN(RSC_Measurement.value + 6, *(uint32_t *)value_ptr);
            }
            else
            {
                LE_DWORD2EXTRN(RSC_Measurement.value + 4, *(uint32_t *)value_ptr);
            }
        }
        else
        {
            ret = FALSE;
        }
        break;
    case RSC_PARAM_SENSOR_LOC:
        RSC_CurSensLocation = *(uint8_t *)value_ptr;
        break;
    case RSC_PARAM_CTL_PNT_PROG_CLR:
        RSC_ControlPoint.value[0] = RSC_CP_OPCODE_RESERVED;
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSC parameter set failed\n", 0 );

    return ret;
}

/**
  * @brief  get service related data by application.
  *
  * @param[in] param_type            parameter type to set: @ref RSC_Application_Parameters
  * @param[out] value_ptr            value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool RSC_GetParameter( uint8_t param_type, void *value_ptr )
{
    bool ret = TRUE;
    uint8_t inc_flag = RSC_Measurement.value[0];

    switch (param_type)
    {
    default:
        ret = FALSE;
        break;
    case RSC_PARAM_INC_FLAG:
        *(uint8_t *)value_ptr = RSC_Measurement.value[0];
        break;
    case RSC_PARAM_SPEED:
        *(uint16_t *)value_ptr = LE_EXTRN2WORD(RSC_Measurement.value + 1);
        break;
    case RSC_PARAM_CADENCE:
        *(uint8_t *)value_ptr = RSC_Measurement.value[3];
        break;
    case RSC_PARAM_STRIDE_LENGTH:
        if ( inc_flag & RSC_INC_STRIDE_LENGTH_MASK )
        {
            *(uint16_t *)value_ptr = LE_EXTRN2WORD(RSC_Measurement.value + 4);
        }
        else
        {
            ret = FALSE;
        }
        break;
    case RSC_PARAM_TOTALDISTANCE:
        if ( inc_flag & RSC_INC_TOTAL_DISTANCE_MASK )
        {
            if ( inc_flag & RSC_INC_STRIDE_LENGTH_MASK )
            {
                *(uint32_t *)value_ptr = LE_EXTRN2DWORD(RSC_Measurement.value + 6);
            }
            else
            {
                *(uint32_t *)value_ptr = LE_EXTRN2DWORD(RSC_Measurement.value + 4);
            }
        }
        else
        {
            ret = FALSE;
        }
        break;
    case RSC_PARAM_SENSOR_LOC:
        *(uint8_t *)value_ptr = RSC_CurSensLocation;
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSC parameter get failed\n", 0 );

    return ret;
}

/**
 * @brief  display control point response.
 *
 * @param RSC_ctl_pnt_ptr       pointer to RSC control point data.
 * @return none
 * @retval  void
*/
static void  RSC_CtlPntDisplayRsp( PRSCControlPoint RSC_ctl_pnt_ptr )
{
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSCCP response: ReqOpCode=0x%x", 1,
               RSC_ctl_pnt_ptr->value[1] );
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "                RspCode=0x%x", 1,
               RSC_ctl_pnt_ptr->value[2] );
}

/**
 * @brief  whether the sensor location supported.
 *
 * @param sens_location     sensor location to be judged(RSC sensor support or not).
 * @return support result.
 * @retval  1   support
 * @retval  0   not support
*/
static bool RSC_SensLocSupported( uint8_t sens_location )
{
    uint8_t     result = FALSE;
    if ( (RSC_SensLocList >> sens_location) & 0x0001 )
    {
        result = TRUE;
    }
    return (result);
}

/**
 * @brief  send response to client, control point indication procedure.
 *
 * @param ServiceId         service ID of service.
 * @param OpCode            control point request opcode type.
 * @param RspCode           control point response code to indicate to client.
 * @return indication action result
 * @retval  1 TRUE
 * @retval  0 FALSE
*/
bool RSC_CtlPntIndicate( uint8_t ServiceId, uint8_t OpCode, uint8_t RspCode )
{
    uint16_t attrib_index = GATT_SVC_RSC_CTL_PNT_INDEX;
    uint8_t sens_location;
    uint8_t param_offset;
    uint8_t *pData;
    uint16_t dataLen;

    RSC_ControlPoint.value[1] = RSC_ControlPoint.value[0];  /* Control Point request opcode. */
    RSC_ControlPoint.value[2] = RspCode;
    RSC_ControlPoint.value[0] = RSC_CP_OPCODE_RSP_CODE;
    RSC_ControlPoint.cur_length = 3 * sizeof(uint8_t);

    /* Diff RspType, different indication contents. */
    if (OpCode == RSC_CP_OPCODE_REQ_SENS_LOC_LIST)
    {
        /* get sensor location list */
        for (sens_location = RSC_SENSOR_LOC_OTHER, param_offset = 3; sens_location < RSC_SENSOR_LOC_MAX; sens_location++)
        {
            if (RSC_SensLocSupported(sens_location))
            {
                RSC_ControlPoint.value[param_offset] = sens_location;
                param_offset++;
            }
        }
        RSC_ControlPoint.cur_length = param_offset;
    }

    RSC_CtlPntDisplayRsp(&RSC_ControlPoint);
    pData = RSC_ControlPoint.value;
    dataLen = RSC_ControlPoint.cur_length;

    // send indication to client
    return ProfileAPI_SendData(ServiceId, attrib_index, pData, dataLen);
}


/**
 * @brief  set cumulative response.
 *
 * @param serviceId
 * @return none
 * @retval  void
*/
static void RSC_CtlPntSetCumulative( uint8_t serviceID, uint8_t rsp_code)
{
    uint8_t              op_code = RSC_CP_OPCODE_SET_CUMULATIVE;
    RSC_CtlPntIndicate( serviceID, op_code, rsp_code );
}


static void RSC_CtlPntStartCalib( uint8_t serviceID, bool result)
{
    uint8_t              op_code = RSC_CP_OPCODE_SET_CUMULATIVE;
    uint8_t rsp_code;
    if(result == TRUE)
    {
        rsp_code = RSC_CP_RSPCODE_SUCCESS;
    }
    else
    {
        rsp_code = RSC_CP_RSPCODE_OPERATION_FAILED;
    }
    RSC_CtlPntIndicate( serviceID, op_code, rsp_code );
}

/**
  * @brief send indication of result of calibration.
  *
  * @param[in] ServiceId         service ID of service.
  * @param[in] result            calibration result
  * @return none
  */
void RSC_CalibResutlIndicate( uint8_t serviceID, bool result)
{
    RSC_CtlPntStartCalib( serviceID, result);
}

/**
 * @brief  update sensor location response.
 *
 * @param serviceID
 * @return none
 * @retval  void
*/
static void RSC_CtlPntUpdateSensLoc( uint8_t serviceID, uint8_t rsp_code)
{

    uint8_t              op_code = RSC_CP_OPCODE_UPDATE_SENS_LOC;
    RSC_CtlPntIndicate( serviceID, op_code, rsp_code );
}

/**
 * @brief  supported sensor location response.
 *
 * @param serviceID
 * @return none
 * @retval  void
*/
static void RSC_CtlPntReqSensLocList( uint8_t serviceID, uint8_t rsp_code)
{
    uint8_t              op_code = RSC_CP_OPCODE_REQ_SENS_LOC_LIST;
    RSC_CtlPntIndicate( serviceID, op_code, rsp_code );
}


static uint8_t  RSC_HanldeCtlPntProc2( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    TRSC_CALLBACK_DATA callback_data;
    uint8_t RespCode  = RSC_CP_RSPCODE_SUCCESS;
    uint16_t parameter_length = 0;
    memcpy( RSC_ControlPoint.value, value_ptr, write_length );
    if(write_length>=1)
    {
        parameter_length = write_length - 1;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "    RSC_HanldeCtlPntProc2 request: OpCode=0x%x", 1,
               RSC_ControlPoint.value[0] );


    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.msg_data.write.opcode = RSC_ControlPoint.value[0];
        
    switch ( RSC_ControlPoint.value[0] )
    {
    
    case RSC_CP_OPCODE_SET_CUMULATIVE:
        {
            if(parameter_length == 4)
            {
                memcpy(&callback_data.msg_data.write.cp_parameter.cumulative_value, 
                            &RSC_ControlPoint.value[1], 4);
            }
            else
            {
                RespCode = RSC_CP_RSPCODE_INVALID_PARAMETER;
            }
        }
        
        break;

    case RSC_CP_OPCODE_START_CALIBRATION:
    {
        if(parameter_length == 0)
        {
        }
        else
        {
            RespCode = RSC_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
        break; 
    case RSC_CP_OPCODE_UPDATE_SENS_LOC:
    {
        if(parameter_length == 1)
        {
            memcpy(&callback_data.msg_data.write.cp_parameter.sensor_location_value, 
                        &RSC_ControlPoint.value[1], 1);
            if(callback_data.msg_data.write.cp_parameter.sensor_location_value>= RSC_SENSOR_LOC_MAX)
            {
                RespCode = RSC_CP_RSPCODE_INVALID_PARAMETER;
            }
        }
        else
        {
            RespCode = RSC_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;
    
    case RSC_CP_OPCODE_REQ_SENS_LOC_LIST:
    {
        if(parameter_length == 0)
        {
        }
        else
        {
            RespCode = RSC_CP_RSPCODE_INVALID_PARAMETER;
        }
    }
    break;       
    


    default:
    {
            RespCode = RSC_CP_RSPCODE_OPCODE_UNSUPPORT;        
    }
    break;
    }

    if(RespCode == RSC_CP_RSPCODE_SUCCESS)
    {
        pfnRscCB(serviceID, (void*)&callback_data);
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
static void  RSC_CtlPntHandleReq( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    uint8_t   RespCode  = RSC_CP_RSPCODE_SUCCESS;
    memcpy( RSC_ControlPoint.value, value_ptr, write_length );
    RSC_ControlPoint.cur_length = write_length;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "    RSCCP request: OpCode=0x%x", 1,
               RSC_ControlPoint.value[0] );

    RespCode = RSC_HanldeCtlPntProc2(serviceID, write_length, value_ptr);

    if(RespCode == RSC_CP_RSPCODE_SUCCESS)
    {

        switch ( RSC_ControlPoint.value[0] )
        {
        default:
            break;
        case RSC_CP_OPCODE_SET_CUMULATIVE:
            if ( (RSC_Feature & RSC_SUPPORT_TOTAL_DISTANCE_MASK) &&
                    (RSC_Measurement.value[0] & RSC_INC_TOTAL_DISTANCE_MASK) )
            {
                RSC_CtlPntSetCumulative(serviceID, RespCode);
                return;
            }
            break;
        case RSC_CP_OPCODE_START_CALIBRATION:
            if ( RSC_Feature & RSC_SUPPORT_CALIBRATE_MASK )
            {
             
                return;
            }
            break;
        case RSC_CP_OPCODE_UPDATE_SENS_LOC:
            if ( RSC_Feature & RSC_SUPPORT_MULTI_SENSOR_MASK )
            {
                RSC_CtlPntUpdateSensLoc(serviceID, RespCode);
                return;
            }
            break;
        case RSC_CP_OPCODE_REQ_SENS_LOC_LIST:
            if ( RSC_Feature & RSC_SUPPORT_MULTI_SENSOR_MASK )
            {
                RSC_CtlPntReqSensLocList(serviceID, RespCode);
                return;
            }
            break;
        }
    }

    /* Send indication to client when request opcode not supported. */
    RSC_CtlPntIndicate(serviceID, RSC_ControlPoint.value[0], RespCode);
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
TProfileResult  RscAttrReadCb( uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;

    switch ( iAttribIndex )
    {
    default:
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- RSC_AttrRead, Attr not found, index=%d", 1, iAttribIndex);
        wCause  = ProfileResult_AttrNotFound;
        break;
    case GATT_SVC_RSC_FEATURE_INDEX:
        {
            TRSC_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = RSC_READ_RSC_FEATURE;
            pfnRscCB(ServiceId, &callback_data);

            *ppValue = (uint8_t *)&RSC_Feature;
            *piLength = sizeof(RSC_Feature);

        }
        break;
    case GATT_SVC_RSC_SENS_LOC_INDEX:
        if ( RSC_Feature & RSC_SUPPORT_MULTI_SENSOR_MASK )
        {
            TRSC_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = RSC_READ_SENSOR_LOCATION;
            pfnRscCB(ServiceId, &callback_data);
            *ppValue = (uint8_t *)&RSC_CurSensLocation;
            *piLength = sizeof(RSC_CurSensLocation);
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
TProfileResult RscAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t wLength, uint8_t * pValue, TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    if (GATT_SVC_RSC_CTL_PNT_INDEX == iAttribIndex)
    {
        /* Attribute value has variable size, make sure written value size is valid. */
        if ( (wLength > RSC_MAX_CTL_PNT_VALUE) || (pValue == NULL) )
        {
            wCause  = ProfileResult_InvalidValueSize;
        }
        /* Make sure Control Point is not "Process already in progress". */
        else if ( RSC_CTL_PNT_OPERATE_ACTIVE( RSC_ControlPoint.value[0] ) )
        {
            wCause  = (TProfileResult)0x80;//ProfileResult_AppErr_ProcAlreadyInProgress
        }
        /* Make sure Control Point is configured indication enable. */
        else if (!gRsc_NotifyIndicateFlag.sc_cp_indicate_enable)
        {
            wCause = (TProfileResult)0x81;//ProfileResult_AppErr_CccdImproperlyConfigured
        }
        else
        {
            /* handle SCCP request after sending write response */
            *pWriteIndPostProc = RSC_CtlPntHandleReq;
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> RSC_AttrWrite Error  iAttribIndex = 0x%x wLength=%d",
                   2, iAttribIndex, wLength);
        wCause = ProfileResult_AttrNotFound;
    }

    return wCause;
}

/**
  * @brief notify rsc measurement value of rsc server.
  *
  * @param[in] ServiceId         service ID of service.
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool RSC_MeasValueNotify( uint8_t ServiceId )
{
    uint16_t attrib_index = GATT_SVC_RSC_MEASUREMENT_INDEX;
    uint8_t *pData = RSC_Measurement.value;
    uint16_t dataLen = RSC_Measurement.cur_length;

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- RSC Measurement Notification", 0 );
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
void RscCccdUpdateCb(uint8_t serviceId, uint16_t Index, uint16_t wCCCBits)
{
    TRSC_CALLBACK_DATA callback_data;
    bool bHandle = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "RSC_CccdUpdate Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
    case GATT_SVC_RSC_MEAS_CCCD_INDEX:
        if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = RSC_NOTIFY_INDICATE_MEASUREMENT_ENABLE;
            gRsc_NotifyIndicateFlag.rsc_measurement_notify_enable = TRUE;
        }
        else
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = RSC_NOTIFY_INDICATE_MEASUREMENT_ENABLE;
            gRsc_NotifyIndicateFlag.rsc_measurement_notify_enable = FALSE;            
        }
        bHandle =  TRUE;
        break;

    case GATT_SVC_RSC_CTL_PNT_CCCD_INDEX:
           if (wCCCBits & GATT_CCCD_INDICATION_BIT)
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = RSC_NOTIFY_INDICATE_SC_CP_ENABLE;
            gRsc_NotifyIndicateFlag.sc_cp_indicate_enable = TRUE; 
        }
        else
        {
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
            callback_data.msg_data.notification_indification_index = RSC_NOTIFY_INDICATE_SC_CP_DISABLE;
            gRsc_NotifyIndicateFlag.sc_cp_indicate_enable = FALSE;
        }
        bHandle =  TRUE;        
        break;
    default:
        break;
    }
    /* Notify Application. */
    if (pfnRscCB && (bHandle == TRUE))
    {
        pfnRscCB(serviceId, (void*)&callback_data);
    }
}

/**
 * @brief RSC Service Callbacks.
*/
CONST gattServiceCBs_t rscCBs =
{
    RscAttrReadCb,  // Read callback function pointer
    RscAttrWriteCb, // Write callback function pointer
    RscCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add RSC service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t RSC_AddService(void* pFunc)
{
    uint8_t ServiceId;

    /* Initiate RSC service related data, modify according to user's demand. */
    RSC_Feature = RSC_ALL_FEATURE_SUPPORTED;
    RSC_Measurement.value[0] = RSC_INC_ALL_PRESENTS;
    RSC_Measurement.cur_length = RSC_MAX_MEASUREMENT_VALUE;
    RSC_SensLocList = RSC_ALL_SENS_LOC_SUPPORTED;
    RSC_CurSensLocation = RSC_SENSOR_LOC_TOP_OF_SHOE;

    /* register RSC service to profile layer. */
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)RscAttrTbl,
                                       iRscAttrTblSize,
                                       rscCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "RSC_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }

    /* register callback for profile to inform application that some events happened. */
    pfnRscCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}


