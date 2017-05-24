/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     beacon_service.c
* @brief
* @details
* @author   hunter_shuai
* @date     23-June-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#include "trace.h"
#include "endialig.h"
#include "FreeRTOS.h"
#include "gatt.h"
#include "profileApi.h"
#include "beacon_service.h"
#include <string.h>
#include "gap.h"
#include "version.h"

extern uint8_t gBeaconServiceId;
extern uint8_t gOTAServiceId;

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Function pointer used to send event to application from beacon extended profile. */
/**<  Initiated in BeaconExtended_AddService. */
pfnAPPHandleInfoCB_t pfnBEACONExtendedCB = NULL;


const uint8_t GATT_UUID_BEACON_SERVICE[16] = {0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0x02, 0x00, 0x00};




/**< @brief  profile/service definition.
*   here is an example of beacon service table
*   including Write
*/
const TAttribAppl gattExtendedBeaconServiceTable[] =
{
    /*--------------------------Beacon Service ---------------------------*/
    /* <<Primary Service>>, .. 0*/
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),  /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_128BIT_SIZE,                           /* bValueLen     */
        (void *)GATT_UUID_BEACON_SERVICE,           /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, .. 1*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_READ,                  /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  Beacon UUID characteristic value 2*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BEACON_UUID),
            HI_WORD(GATT_UUID_CHAR_BEACON_UUID),
        },
        2,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },

    /* <<Characteristic>>, .. 3*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_READ,                  /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  Major id characteristic value 4*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BEACON_MAJORID),
            HI_WORD(GATT_UUID_CHAR_BEACON_MAJORID),
        },
        2,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },

    /* <<Characteristic>>, .. 5*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_READ,                  /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  Minor id characteristic value 6*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BEACON_MINORID),
            HI_WORD(GATT_UUID_CHAR_BEACON_MINORID),
        },
        2,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },

    /* <<Characteristic>>, .. 7*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_READ,                  /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  measured power characteristic value 8*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BEACON_MEASURED_POWER),
            HI_WORD(GATT_UUID_CHAR_BEACON_MEASURED_POWER),
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },

    /* <<Characteristic>>, ...11*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_READ,          /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  advertising interval characteristic value 11*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BEACON_ADV_INTERVAL),
            HI_WORD(GATT_UUID_CHAR_BEACON_ADV_INTERVAL),
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },
};




/**@brief Beacon read value */
static uint8_t Beacon_UUID[UUID_LENGTH];
static uint8_t Beacon_MajorID[MAJORID_LENGTH];
static uint8_t Beacon_MinorID[MINORID_LENGTH];
static uint8_t Beacon_MesuredPower[MEASURED_POWER_LENGTH];
uint16_t adv_interval;


/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set:
  * @param[in] len                   value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool BeaconBleService_SetParameter( uint8_t param_type, uint8_t len, void *value_ptr)
{
    bool ret = TRUE;

    switch (param_type)
    {
        default:
            ret = FALSE;
            break;
        case BEACON_READ_UUID_CHAR_VAL:
            if (value_ptr)
            {
                uint8_t *value = (uint8_t *)value_ptr;
                for (uint8_t i = 0; i < sizeof(Beacon_UUID); i++)
                {
                    Beacon_UUID[i] = value[i];
                }
            }
            break;
        case BEACON_READ_MAJORID_CHAR_VAL:
            if (value_ptr)
            {
                uint8_t *value = (uint8_t *)value_ptr;
                for (uint8_t i = 0; i < sizeof(Beacon_MajorID); i++)
                {
                    Beacon_MajorID[i] = value[i];
                }
            }
            break;
        case BEACON_READ_MINORID_CHAR_VAL:
            if (value_ptr)
            {
                uint8_t *value = (uint8_t *)value_ptr;

                for (uint8_t i = 0; i < sizeof(Beacon_MinorID); i++)
                {
                    Beacon_MinorID[i] = value[i];
                }
            }
            break;

        case BEACON_READ_MEASURED_POWER_CHAR_VAL:
            if (value_ptr)
            {
                uint8_t *value = (uint8_t *)value_ptr;

                for (uint8_t i = 0; i < sizeof(Beacon_MesuredPower); i++)
                {
                    Beacon_MesuredPower[i] = value[i];
                }
            }
            break;
        case BEACON_READ_ADV_INTERVAL_CHAR_VAL:
            if (value_ptr)
            {
                uint16_t *value = (uint16_t *)value_ptr;
                adv_interval = value[0];
            }

            break;
    }
    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "Beacon read parameter set failed\n", 0 );

    return ret;
}


/**
 * @brief read characteristic data from service.
 *
 * @param ServiceId          ServiceID of characteristic data.
 * @param iAttribIndex       Attribute index of getting characteristic data.
 * @param iOffset            Used for Blob Read.
 * @param piLength           length of getting characteristic data.
 * @param ppValue            data got from service.
 * @return Profile procedure result
*/
TProfileResult   BeaconServiceAttrReadCb( uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t *piLength, uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;
    TBEACON_CALLBACK_DATA callback_data;

    if (gBeaconServiceId == ServiceId)
    {
        switch ( iAttribIndex )
        {
            default:
                DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- Beacon_AttrRead, Attr not found, index=%d",
                           1, iAttribIndex);
                wCause  = ProfileResult_AttrNotFound;
                break;
            case BEACON_BLE_SERVICE_CHAR_UUID_INDEX:
                {
                    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
                    callback_data.msg_data.read_value_index = BEACON_READ_UUID_CHAR_VAL;
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                    *ppValue    = Beacon_UUID;
                    *piLength = sizeof(Beacon_UUID);
                }
                break;
            case BEACON_BLE_SERVICE_CHAR_MAJORID_INDEX:
                {
                    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
                    callback_data.msg_data.read_value_index = BEACON_READ_MAJORID_CHAR_VAL;
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                    *ppValue    = Beacon_MajorID;
                    *piLength = sizeof(Beacon_MajorID);
                }
                break;
            case BEACON_BLE_SERVICE_CHAR_MINORID_INDEX:
                {
                    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
                    callback_data.msg_data.read_value_index = BEACON_READ_MINORID_CHAR_VAL;
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                    *ppValue    = Beacon_MinorID;
                    *piLength = sizeof(Beacon_MinorID);
                }
                break;
            case BEACON_BLE_SERVICE_CHAR_MEASURED_POWER_INDEX:
                {
                    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
                    callback_data.msg_data.read_value_index = BEACON_READ_MEASURED_POWER_CHAR_VAL;
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                    *ppValue    = Beacon_MesuredPower;
                    *piLength = sizeof(Beacon_MesuredPower);
                }
                break;

            case BEACON_BLE_SERVICE_CHAR_ADV_INTERVAL_INDEX:
                {
                    uint16_t temp1, temp2;
                    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
                    callback_data.msg_data.read_value_index = BEACON_READ_ADV_INTERVAL_CHAR_VAL;
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                    adv_interval = (adv_interval * 10 / 16);
                    temp1 = adv_interval & 0x00ff;
                    temp2 = adv_interval & 0xff00;
                    adv_interval = temp1 << 8 | temp2 >> 8;
                    *ppValue    = (uint8_t *)&adv_interval;
                    *piLength = sizeof(adv_interval);
                }
                break;
        }

    }

    return ( wCause );
}


/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param iAttribIndex       Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue             value to be written.
 * @return Profile procedure result
*/
TProfileResult  BeaconServiceAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t wLength, uint8_t *pValue, TGATTDWriteIndPostProc *pWriteIndPostProc)
{
    TBEACON_CALLBACK_DATA callback_data;
    TProfileResult  wCause = ProfileResult_Success;

    if (gBeaconServiceId == ServiceId)
    {
        if (BEACON_BLE_SERVICE_CHAR_UUID_INDEX == iAttribIndex)
        {
            /* Make sure written value size is valid. */
            if ( (wLength != UUID_LENGTH * sizeof(uint8_t)) || (pValue == NULL) )
            {
                wCause  = ProfileResult_InvalidValueSize;
            }
            else
            {
                /* Notify Application. */
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
                callback_data.msg_data.write.opcode = BEACON_WRITE_UUID_CHAR_VAL;
                callback_data.msg_data.write.value = pValue;

                if (pfnBEACONExtendedCB)
                {
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                }
            }
        }
        else if (BEACON_BLE_SERVICE_CHAR_MAJORID_INDEX == iAttribIndex)
        {
            /* Make sure written value size is valid. */
            if ( (wLength != MAJORID_LENGTH * sizeof(uint8_t)) || (pValue == NULL) )
            {
                wCause  = ProfileResult_InvalidValueSize;
            }
            else
            {
                /* Notify Application. */
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
                callback_data.msg_data.write.opcode = BEACON_WRITE_MAJORID_CHAR_VAL;
                callback_data.msg_data.write.value = pValue;

                if (pfnBEACONExtendedCB)
                {
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                }
            }
        }
        else if (BEACON_BLE_SERVICE_CHAR_MINORID_INDEX == iAttribIndex)
        {
            /* Make sure written value size is valid. */
            if ( (wLength != MINORID_LENGTH * sizeof(uint8_t)) || (pValue == NULL) )
            {
                wCause  = ProfileResult_InvalidValueSize;
            }
            else
            {
                /* Notify Application. */
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
                callback_data.msg_data.write.opcode = BEACON_WRITE_MINORID_CHAR_VAL;
                callback_data.msg_data.write.value = pValue;

                if (pfnBEACONExtendedCB)
                {
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                }
            }
        }
        else if (BEACON_BLE_SERVICE_CHAR_MEASURED_POWER_INDEX == iAttribIndex)
        {
            /* Make sure written value size is valid. */
            if ( (wLength != MEASURED_POWER_LENGTH * sizeof(uint8_t)) || (pValue == NULL) )
            {
                wCause  = ProfileResult_InvalidValueSize;
            }
            else
            {
                /* Notify Application. */
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
                callback_data.msg_data.write.opcode = BEACON_WRITE_MEASURED_POWER_CHAR_VAL;
                callback_data.msg_data.write.value = pValue;

                if (pfnBEACONExtendedCB)
                {
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                }
            }
        }

        else if (BEACON_BLE_SERVICE_CHAR_ADV_INTERVAL_INDEX == iAttribIndex)
        {
            /* Make sure written value size is valid. */
            if ( (wLength != 2 * sizeof(uint8_t)) || (pValue == NULL) )
            {
                wCause  = ProfileResult_InvalidValueSize;
            }
            else
            {
                /* Notify Application. */
                callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
                callback_data.msg_data.write.opcode = BEACON_WRITE_ADV_INTERVAL_CHAR_VAL;
                callback_data.msg_data.write.value = pValue;

                if (pfnBEACONExtendedCB)
                {
                    pfnBEACONExtendedCB(ServiceId, (void *)&callback_data);
                }
            }
        }
        else
        {
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> Beacon_AttrWrite Error  iAttribIndex = 0x%x wLength=%d",
                       2,
                       iAttribIndex,
                       wLength);
            wCause = ProfileResult_AttrNotFound;
        }
    }

    return wCause;
}


/**
 * @brief  update CCCD bits from stack.
 *
 * @param  ServiceId      Service ID.
 * @param  Index          Attribute index of characteristic data.
 * @param  wCCCBits       CCCD bits from stack.
 * @return None
*/
void BeaconServiceCccdUpdateCb(uint8_t serviceId, uint16_t Index, uint16_t wCCCBits)
{
    //handle Beacon CCCD message here...
    return;
}

/**
 * @brief Beacon ble Service Callbacks.
*/
CONST gattServiceCBs_t BeaconServiceCBs =
{
    BeaconServiceAttrReadCb,   // Read callback function pointer
    BeaconServiceAttrWriteCb,  // Write callback function pointer
    BeaconServiceCccdUpdateCb  // CCCD update callback function pointer
};

/**
 * @brief  add Beacon ble service to application.
 *
 * @param  pFunc          pointer of app callback function called by profile.
 * @return service ID auto generated by profile layer.
 * @retval ServiceId
*/
uint8_t BeaconService_AddService(void *pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t *)gattExtendedBeaconServiceTable,
                                       sizeof(gattExtendedBeaconServiceTable),
                                       BeaconServiceCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "BeaconService_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }
    pfnBEACONExtendedCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}


