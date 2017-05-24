enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     simple_ble_service.c
* @brief    simple BLE profile source file.
* @details  Demonstration of how to implement a self-definition profile.
* @author
* @date     2015-5-10
* @version  v0.1
*********************************************************************************************************
*/
#include "trace.h"
#include "endialig.h"
#include <string.h>
#include "FreeRTOS.h"
#include "gatt.h"
#include "simple_ble_service.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Value of simple read characteristic. */
static uint8_t simpleCharReadValue = 0xf0;
/**<  Value of simple control point characteristic. */
static uint8_t simpleCharCtlPntValue[3] = {0};
/**<  simple BLE profile control point indication enable flag. */
static uint8_t simpleCharCtlPntIndFlag = GATT_CLIENT_CHAR_CONFIG_DEFAULT;
/**<  Function pointer used to send event to application from simple profile. Initiated in SimpBleService_AddService. */
static pfnAPPHandleInfoCB_t pfnSimpBleServiceCB = NULL;
/**<  128bit UUID of simple BLE service. */
const uint8_t GATT_UUID128_SIMPLE_PROFILE[16] = { 0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xA0, 0x0A, 0x00, 0x00};

bool SimpBleService_V5Indicate( uint8_t ServiceId, uint8_t OpCode, uint8_t RspCode );

/**< @brief  profile/service definition.  */
const TAttribAppl simple_ble_service_tbl[] =
{
    /* <<Primary Service>>, .. */
#ifdef SIMP_USE_UUID128
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),        /* wFlags     */
        {
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),     /* bTypeValue */
        },
        UUID_128BIT_SIZE,                           /* bValueLen     */
        (void*)GATT_UUID128_SIMPLE_PROFILE,         /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },
#else
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_SIMPLE_PROFILE),      /* service UUID */
            HI_WORD(GATT_UUID_SIMPLE_PROFILE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },
#endif

    /* <<Characteristic>> demo for read */
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
#ifdef SIMP_USE_UUID128
    {
        ATTRIB_FLAG_VALUE_APPL|ATTRIB_FLAG_UUID_128BIT, /* wFlags */
        {
            GATT_UUID128_CHAR_SIMPLE_V1_READ
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
#else
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SIMPLE_V1_READ),
            HI_WORD(GATT_UUID_CHAR_SIMPLE_V1_READ)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
#endif
    /* <<Characteristic>> demo for write */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE)  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SIMPLE_V2_WRITE),
            HI_WORD(GATT_UUID_CHAR_SIMPLE_V2_WRITE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE                             /* wPermissions */
    },
    /* <<Characteristic>>, demo for notify */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY)                 /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SIMPLE_V3_NOTIFY),
            HI_WORD(GATT_UUID_CHAR_SIMPLE_V3_NOTIFY)
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
    },
    /* <<Characteristic>> demo for indicate */
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

    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SIMPLE_V4_INDICATE),
            HI_WORD(GATT_UUID_CHAR_SIMPLE_V4_INDICATE)
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
    },

    /* <<Characteristic>>, demo for Control Point */
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
    /*--- Simple Control Point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SIMPLE_V5_CTL_PNT),
            HI_WORD(GATT_UUID_CHAR_SIMPLE_V5_CTL_PNT)
        },
        0,                                          /* bValueLen, 0 : variable length */
        NULL,
        GATT_PERM_WRITE                             /* wPermissions */
    },
    /* client characteristic configuration */
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

			/* <<Characteristic>> demo for write Long*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE)  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SIMPLE_V6_WRITE_LONG),
            HI_WORD(GATT_UUID_CHAR_SIMPLE_V6_WRITE_LONG)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE                             /* wPermissions */
    },
    
#ifdef GATT_SERVICE_INCLUDED
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_GATT),      /* service UUID */
            HI_WORD(GATT_UUID_GATT)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

  /* <<Characteristic>> demo for indicate */
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
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SERVICE_CHANGED),
            HI_WORD(GATT_UUID_CHAR_SERVICE_CHANGED)
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
    },
    
#endif
            
};

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref SIMP_Application_Parameters
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool SimpBleService_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;

    switch (param_type)
    {
    default:
        ret = FALSE;
        break;
    case SIMPLE_BLE_SERVICE_PARAM_V1_READ_CHAR_VAL:
        simpleCharReadValue = *(uint8_t *)value_ptr;
        break;
    case SIMPLE_BLE_SERVICE_PARAM_CTL_PNT_PROG_CLR:
        simpleCharCtlPntValue[0] = 0;
        break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleService_SetParameter failed\n", 0 );

    return ret;
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
TProfileResult  SimpBleServiceAttrReadCb( uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;

    switch ( iAttribIndex )
    {
    default:
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- SimpBleServiceAttrReadCb, Attr not found, index=%d",
                   1, iAttribIndex);
        wCause  = ProfileResult_AttrNotFound;
        break;
    case SIMPLE_BLE_SERVICE_CHAR_V1_READ_INDEX:
        {
            TSIMP_CALLBACK_DATA callback_data;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            callback_data.msg_data.read_value_index = SIMP_READ_V1;
            pfnSimpBleServiceCB(ServiceId, (void*)&callback_data);            
            *ppValue    = &simpleCharReadValue;
            *piLength = sizeof(simpleCharReadValue);
        }
        break;
    }

    return ( wCause );
}

/**
 * @brief  set GATT_UUID_CHAR_SIMPLE_V4_INDICATE characteristic's value,
 *              and then send set result to client.
 * @param serviceId
 * @return none
 * @retval  void
*/
static void SimpBleService_CtlPntSetIndVal( uint8_t serviceID )
{
    TSIMP_CALLBACK_DATA callback_data;
    
    uint8_t              op_code = SIMPLE_CP_OPCODE_SET_CHAR_INDICATE_VAL;
    uint8_t              rsp_code = SIMPLE_CP_RSPCODE_SUCCESS;

    /* Notify Application. */
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.msg_data.write.opcode = SIMP_WRITE_V5_OPCODE_SET_CHAR_INDICATE_VAL;
    callback_data.msg_data.write.value = simpleCharCtlPntValue[1];
    if (pfnSimpBleServiceCB)
    {
        pfnSimpBleServiceCB(serviceID, (void*)&callback_data);
    }

    /* Send indication to client. */
    SimpBleService_V5Indicate( serviceID, op_code, rsp_code );
}

/**
 * @brief  set GATT_UUID_CHAR_SIMPLE_V3_NOTIFY characteristic's value,
 *              and then send set result to client.
 *
 * @param serviceID
 * @return none
 * @retval  void
*/
static void SimpBleService_CtlPntSetNotVal( uint8_t serviceID )
{
    TSIMP_CALLBACK_DATA callback_data;
    uint8_t              op_code = SIMPLE_CP_OPCODE_SET_CHAR_NOTIFY_VAL;
    uint8_t              rsp_code = SIMPLE_CP_RSPCODE_SUCCESS;

    /* Notify Application. */
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.msg_data.write.opcode = SIMP_WRITE_V5_OPCODE_SET_CHAR_NOTIFY_VAL;
    callback_data.msg_data.write.value = simpleCharCtlPntValue[1];
    if (pfnSimpBleServiceCB)
    {
        pfnSimpBleServiceCB(serviceID, (void*)&callback_data);
    }

    /* Send indication to client. */
    SimpBleService_V5Indicate( serviceID, op_code, rsp_code );
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
static void  SimpBleService_CtlPntHandleReq( uint8_t serviceID, uint16_t write_length, uint8_t * value_ptr )
{
    BOOL opcode_supp = TRUE;

    memcpy( simpleCharCtlPntValue, value_ptr, write_length );
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleService_CtlPntHandleReq: OpCode=0x%x", 1,
               simpleCharCtlPntValue[0] );

    switch ( simpleCharCtlPntValue[0] )
    {
    default:
        opcode_supp = FALSE;
        break;
    case SIMPLE_CP_OPCODE_SET_CHAR_INDICATE_VAL:
        SimpBleService_CtlPntSetIndVal(serviceID);
        break;
    case SIMPLE_CP_OPCODE_SET_CHAR_NOTIFY_VAL:
        SimpBleService_CtlPntSetNotVal(serviceID);
        break;
    }

    if (!opcode_supp)
    {
        /* Send indication to client when request opcode not supported. */
        SimpBleService_V5Indicate(serviceID, simpleCharCtlPntValue[0], SIMPLE_CP_RSPCODE_OPCODE_UNSUPPORT);
    }
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
TProfileResult SimpBleServiceAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t wLength, uint8_t * pValue, TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TSIMP_CALLBACK_DATA callback_data;
    TProfileResult  wCause = ProfileResult_Success;
    if (SIMPLE_BLE_SERVICE_CHAR_V2_WRITE_INDEX == iAttribIndex)
    {
        /* Make sure written value size is valid. */
        if ( (wLength > sizeof(uint8_t)) || (pValue == NULL) )
        {
            wCause  = ProfileResult_InvalidValueSize;
        }
        else
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.opcode = SIMP_WRITE_V2;
            callback_data.msg_data.write.value = pValue[0];
            
            if (pfnSimpBleServiceCB)
            {
                pfnSimpBleServiceCB(ServiceId, (void*)&callback_data);
            }
        }
    }
    else if (SIMPLE_BLE_SERVICE_CHAR_V5_CTL_PNT_INDEX == iAttribIndex)
    {
        /* Make sure written value size is valid. */
        if ( (wLength > sizeof(simpleCharCtlPntValue)) || (pValue == NULL) )
        {
            wCause  = ProfileResult_InvalidValueSize;
        }
        /* Make sure Control Point is not "Process already in progress". */
        else if ( SIMPLE_BLE_SERVICE_CTL_PNT_OPERATE_ACTIVE(simpleCharCtlPntValue[0]) )
        {
            wCause  = ProfileResult_AppErr_ProcAlreadyInProgress;
        }
        /* Make sure Control Point is configured indication enable. */
        else if (simpleCharCtlPntIndFlag != GATT_CLIENT_CHAR_CONFIG_INDICATE)
        {
            wCause = ProfileResult_AppErr_CccdImproperlyConfigured;
        }
        else
        {
            *pWriteIndPostProc = SimpBleService_CtlPntHandleReq;
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> SimpBleServiceAttrWriteCb Error  iAttribIndex = 0x%x wLength=%d",
                   2,
                   iAttribIndex,
                   wLength);
        wCause = ProfileResult_AttrNotFound;
    }
    return wCause;
}


/**
 * @brief write Long characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param iAttribIndex          Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue            value to be written.
 * @return Profile procedure result
*/
TProfileResult SimpBleServiceAttrPreWriteCb(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t wLength, uint8_t * pValue, TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TSIMP_CALLBACK_DATA callback_data;
    TProfileResult  wCause = ProfileResult_Success;
    if (SIMPLE_BLE_SERVICE_CHAR_V6_WRITE_INDEX == iAttribIndex)
    {
        /* Make sure written value size is valid. */
        if ( (wLength > sizeof(uint8_t)) || (pValue == NULL) )
        {
            wCause  = ProfileResult_InvalidValueSize;
        }
        else
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
			callback_data.msg_data.pre_write.opcode = SIMP_PRE_WRITE_V6;
			callback_data.msg_data.pre_write.pValue = pValue;
			callback_data.msg_data.pre_write.Len = wLength;
            
            if (pfnSimpBleServiceCB)
            {
                pfnSimpBleServiceCB(ServiceId, (void*)&callback_data);
            }
        }
    }
    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> SimpBleServiceAttrPreWriteCb Error  iAttribIndex = 0x%x wLength=%d",
                   2,
                   iAttribIndex,
                   wLength);
        wCause = ProfileResult_AttrNotFound;
    }
    return wCause;
}

/**
  * @brief send notification of simple notify characteristic value.
  *
  * @param[in] ServiceId         service ID of service.
  * @param[in] value             characteristic value to notify
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool SimpBleService_SimpleV3Notify( uint8_t ServiceId, uint8_t value)
{
    uint8_t *pData = (uint8_t *)&value;
    uint16_t dataLen = sizeof(value);

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- SimpBleService_SimpleV3Notify", 0 );
    // send notification to client
    return ProfileAPI_SendData(ServiceId, SIMPLE_BLE_SERVICE_CHAR_V3_NOTIFY_INDEX, pData, dataLen);
}

/**
  * @brief send indication of simple indicate characteristic value.
  *
  * @param[in] ServiceId         service ID of service.
  * @param[in] value             characteristic value to indicate
  * @return notification action result
  * @retval 1 TRUE
  * @retval 0 FALSE
  */
bool SimpBleService_SimpleV4Indicate( uint8_t ServiceId, uint8_t value)
{
    uint8_t *pData = (uint8_t *)&value;
    uint16_t dataLen = sizeof(value);

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- SimpBleService_SimpleV4Indicate", 0 );
    // send indication to client
    return ProfileAPI_SendData(ServiceId, SIMPLE_BLE_SERVICE_CHAR_V4_INDICATE_INDEX, pData, dataLen);
}

/**
 * @brief  send indication to client of simple indicate characteristic value.
 *
 * @param ServiceId         service ID of service.
 * @param OpCode            control point request opcode type.
 * @param RspCode           control point response code to indicate to client.
 * @return indication action result
 * @retval  1 TRUE
 * @retval  0 FALSE
*/
bool SimpBleService_V5Indicate( uint8_t ServiceId, uint8_t OpCode, uint8_t RspCode )
{
    uint8_t *pData;
    uint16_t dataLen;

    simpleCharCtlPntValue[0] = SIMPLE_CP_OPCODE_RSP_CODE;
    simpleCharCtlPntValue[1] = OpCode;
    simpleCharCtlPntValue[2] = RspCode;
    pData = (uint8_t *)simpleCharCtlPntValue;
    dataLen = sizeof(simpleCharCtlPntValue);

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- SimpBleService_V5Indicate", 0 );
    // send indication to client
    return ProfileAPI_SendData(ServiceId, SIMPLE_BLE_SERVICE_CHAR_V5_CTL_PNT_INDEX, pData, dataLen);
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index          Attribute index of characteristic data.
 * @param wCCCBits         CCCD bits from stack.
 * @return None
*/
void SimpBleServiceCccdUpdateCb(uint8_t serviceId, uint16_t Index, uint16_t wCCCBits)
{
    TSIMP_CALLBACK_DATA callback_data;
    bool bHandle = FALSE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "SimpBleServiceCccdUpdateCb  Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
    case SIMPLE_BLE_SERVICE_CHAR_NOTIFY_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V3_ENABLE;
            }
            else
            {
                // Disable Notification
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V3_DISABLE;
            }
            bHandle =  TRUE;
        }
        break;
    case SIMPLE_BLE_SERVICE_CHAR_INDICATE_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_INDICATION_BIT)
            {
                // Enable Indication
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V4_ENABLE;
            }
            else
            {
                // Disable Indication
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V4_DISABLE;
             }
            bHandle =  TRUE;
        }
        break;
    case SIMPLE_BLE_SERVICE_CHAR_CTL_PNT_CCCD_INDEX:
        {
            simpleCharCtlPntIndFlag = wCCCBits;
            if (wCCCBits & GATT_CCCD_INDICATION_BIT)
            {
                // Enable Indication
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V5_ENABLE;
            }
            else
            {
                // Disable Indication
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V5_DISABLE;
            }
            bHandle =  TRUE;
        }
        break;
    default:
        break;
    }
    /* Notify Application. */
    if (pfnSimpBleServiceCB && (bHandle == TRUE))
    {
        pfnSimpBleServiceCB(serviceId, (void*)&callback_data);
    }
}

/**
 * @brief Simple ble Service Callbacks.
*/
CONST gattServiceCBs_t simpBleServiceCBs =
{
    SimpBleServiceAttrReadCb,  // Read callback function pointer
    SimpBleServiceAttrWriteCb, // Write callback function pointer
    SimpBleServiceCccdUpdateCb  // CCCD update callback function pointer
};

/**
  * @brief add Simple BLE service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t SimpBleService_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)simple_ble_service_tbl,
                                       sizeof(simple_ble_service_tbl),
                                       simpBleServiceCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "SimpBleService_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }
    pfnSimpBleServiceCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

