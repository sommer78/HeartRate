/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ota_service.c
* @brief
* @details
* @author   hunter_shuai
* @date     14-July-2015
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
#include "ota_service.h"
#include "gap.h"
#include "string.h"
#include "version.h"


/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Function pointer used to send event to application from BWPS extended profile. */
/**<  Initiated in BWPSExtended_AddService. */
pfnAPPHandleInfoCB_t pfnOTAExtendedCB = NULL;
uint8_t mac_addr[6];
uint16_t patch_version = 0;
uint16_t app_version = 0;
const uint8_t GATT_UUID_OTA_SERVICE[16] = { 0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0xD0, 0x00, 0x00};


/**< @brief  profile/service definition.
*   here is an example of OTA service table
*   including Write
*/
const TAttribAppl gattExtendedServiceTable[] =
{
    /*--------------------------OTA Service ---------------------------*/
    /* <<Primary Service>>, .. 0*/
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),  /* wFlags     */
        {
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),     /* bTypeValue */
        },
        UUID_128BIT_SIZE,                     /* bValueLen     */
        (void *)GATT_UUID_OTA_SERVICE,        /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic1>>, .. 1*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP,                    /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /*  OTA characteristic value 2*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_OTA),
            HI_WORD(GATT_UUID_CHAR_OTA),
        },
        2,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },

    /* <<Characteristic2>>, .. 3, MAC Address*/
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
    /*  OTA characteristic value 4*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_MAC),
            HI_WORD(GATT_UUID_CHAR_MAC),
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ            /* wPermissions */
    },

    /* <<Characteristic3>>, .. 5, Patch version*/
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
    /*  OTA characteristic value 6*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_PATCH),
            HI_WORD(GATT_UUID_CHAR_PATCH),
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ            /* wPermissions */
    },
    /* <<Characteristic4>>, .. 7 App version*/
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
    /*  OTA characteristic value 8*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_APP_VERSION),
            HI_WORD(GATT_UUID_CHAR_APP_VERSION),
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ            /* wPermissions */
    },
};




/**
 * @brief write characteristic data from service.
 *
 * @param ServiceID          ServiceID to be written.
 * @param iAttribIndex       Attribute index of characteristic.
 * @param wLength            length of value to be written.
 * @param pValue             value to be written.
 * @return Profile procedure result
*/
TProfileResult  OTAServiceAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t wLength, uint8_t *pValue, TGATTDWriteIndPostProc *pWriteIndPostProc)
{
    TOTA_CALLBACK_DATA callback_data;
    TProfileResult  wCause = ProfileResult_Success;

    if (BLE_SERVICE_CHAR_OTA_INDEX == iAttribIndex)
    {
        /* Make sure written value size is valid. */
        if ( (wLength != sizeof(uint8_t)) || (pValue == NULL) )
        {
            wCause  = ProfileResult_InvalidValueSize;
        }
        else
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.opcode = OTA_WRITE_CHAR_VAL;
            callback_data.msg_data.write.value = pValue[0];

            if (pfnOTAExtendedCB)
            {
                pfnOTAExtendedCB(ServiceId, (void *)&callback_data);
            }
        }
    }

    else
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> OTA_AttrWrite Error  iAttribIndex = 0x%x wLength=%d",
                   2,
                   iAttribIndex,
                   wLength);
        wCause = ProfileResult_AttrNotFound;
    }
    return wCause;

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
TProfileResult   OTAServiceAttrReadCb( uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t *piLength, uint8_t **ppValue )
{
    TProfileResult  wCause = ProfileResult_Success;

    switch ( iAttribIndex )
    {
        default:
            DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- OTA_AttrRead, Attr not found, index=%d",
                       1, iAttribIndex);
            wCause  = ProfileResult_AttrNotFound;
            break;
        case BLE_SERVICE_CHAR_MAC_ADDRESS_INDEX:
            {
                GAP_GetParameter( GAPPARA_BD_ADDR, mac_addr );
                uint8_t addr[6];
                for (int i = 0; i < 6; i++)
                {
                    addr[i] = mac_addr[5 - i];
                }
                memcpy(mac_addr, addr, 6);
                *ppValue  = (uint8_t *)mac_addr;
                *piLength = sizeof(mac_addr);
            }
            break;
        case BLE_SERVICE_CHAR_PATCH_INDEX:
            {
                blueAPI_ReadPatchVersion(&patch_version);
                DBG_BUFFER(MODULE_APP, LEVEL_INFO, "patch_version = %d", 1, patch_version);
                *ppValue  = (uint8_t *)&patch_version;
                *piLength = sizeof(patch_version);
            }
            break;

        case BLE_SERVICE_CHAR_APP_VERSION_INDEX:
            {
                app_version = VERSION_BUILD;
                *ppValue  = (uint8_t *)&app_version;
                *piLength = sizeof(app_version);
            }
            break;

    }
    return ( wCause );
}


/**
 * @brief OTA ble Service Callbacks.
*/
CONST gattServiceCBs_t OTAServiceCBs =
{
    OTAServiceAttrReadCb,   // Read callback function pointer
    OTAServiceAttrWriteCb,  // Write callback function pointer
    NULL                    // CCCD update callback function pointer
};

/**
 * @brief  add OTA ble service to application.
 *
 * @param  pFunc          pointer of app callback function called by profile.
 * @return service ID auto generated by profile layer.
 * @retval ServiceId
*/
uint8_t OTAService_AddService(void *pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t *)gattExtendedServiceTable,
                                       sizeof(gattExtendedServiceTable),
                                       OTAServiceCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "OTAService_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
        return ServiceId;
    }
    pfnOTAExtendedCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

