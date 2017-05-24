/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      pxp_ext.c
* @brief
* @details
* @author
* @date
* @version
* *********************************************************************************************************
*/
#include "rtl876x.h"
#include "pxp_ext.h"
#include <FreeRTOS.h>
#include "blueapi.h"
#include "profileApi.h"
#include <endialig.h>
#include "trace.h"

/* Temp added here for testing pxp functions. */
extern uint8_t gPXPEXTServiceId;

/**<  Function pointer used to send event to application from pxp extended profile. */
/**<  Initiated in PXPExtended_AddService. */
pfnAPPHandleInfoCB_t pfnPxpExtendedCB = NULL;

UINT32 gTimeParaValue = 20;
UINT8 KEY_SEND_FLAG = 0;
const uint8_t   GATT_UUID128_PXPEXT_SERVICE[16] = {0xA6, 0xF6, 0xF6, 0x07, 0x4D, 0xC4, 0x9D, 0x98, 0x6D, 0x45, 0x29, 0xBB, 0xD0, 0xFF, 0x00, 0x00};
#if 0
static const TAttribAppl gattExtendedServiceTable[] =
{
    /*----------------- simple key Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR),   /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_EXS),              /* service UUID */
            HI_WORD(GATT_UUID_EXS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* Set para Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,            /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Set para Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_PARAM),
            HI_WORD(GATT_UUID_CHAR_PARAM)
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                           /* wPermissions */
    },

    /* Key  <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (                /* characteristic properties */
                GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* simple key value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_KEY),
            HI_WORD(GATT_UUID_CHAR_KEY)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
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
};
#else
static const TAttribAppl gattExtendedServiceTable[] =
{
    /*----------------- simple key Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR),   /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_128BIT_SIZE,                            /* bValueLen     */
        (void *)GATT_UUID128_PXPEXT_SERVICE,        /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* Set para Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,            /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Set para Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* wFlags */
        {   /* bTypeValue */
            GATT_UUID128_CHAR_PARAM
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                           /* wPermissions */
    },

    /* Key  <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (                /* characteristic properties */
                GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* simple key value */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* wFlags */
        {   /* bTypeValue */
            GATT_UUID128_CHAR_KEY
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
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
};
#endif

/**< @brief  PXP Extended service size definition.  */
const uint16_t iExtendedServiceTableSize = sizeof(gattExtendedServiceTable);


UINT8 PTSData[8] = {0};
UINT32 PTSDateLen = 0;
bool bPTSFlag = FALSE;

/**
 * @brief read characteristic data from stack.
 *
 * @param ServiceId          ServiceId generated when register to upper stack.
 * @param attrib_index          Attribute index of getting characteristic data.
 * @param offset                Used for Blob Read.
 * @param length_ptr            length of getting characteristic data.
 * @param ppValue            pointer of getting characteristic data.
 * @return TProfileResult   profile procedure results.
*/
TProfileResult  PxpExtended_AttrRead(uint8_t ServiceId , uint16_t attrib_index, uint16_t offset, uint16_t *length_ptr , uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;
    *length_ptr = 0;

    switch ( attrib_index )
    {
        default:
            /* invalid attribute index. */
            DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "PxpExtended_AttrRead default:attrib_index=%d\n", 1, attrib_index );
            wCause = ProfileResult_AttrNotFound;
            break;
        case SIMPLE_PARAM_VALUE_INDEX:
            *ppValue    = (uint8_t *)&gTimeParaValue;
            *length_ptr = sizeof(gTimeParaValue);
            break;
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- gatt get PxpExtended attribute value: AttribIndex--%d, length--%d",
               2,
               attrib_index,
               *length_ptr);
    return ( wCause );
}

/**
 * @brief write characteristic data from stack.
 *
 * @param ServiceId          ServiceId generated when register to upper stack.
 * @param iAttribIndex          Attribute index of getting characteristic data.
 * @param wLength                length of data to be written.
 * @param pValue            pointer of data to be written.
 * @return TProfileResult   profile procedure results.
*/
TProfileResult PxpExtended_AttrWrite(uint8_t ServiceID, uint16_t iAttribIndex,
                                     uint16_t wLength, uint8_t *pValue, TGATTDWriteIndPostProc *pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    TPXP_EXTENDED_PARA_CBs_t data = {0};

    if (!pValue)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> PxpExtended_AttrWrite   pValue %p wLength= 0x%x",
                   2,
                   pValue,
                   wLength);
        wCause = ProfileResult_InvalidParameter;
        return wCause;
    }

    switch (iAttribIndex)
    {
        default:
            wCause = ProfileResult_AttrNotFound;
            break;
        case SIMPLE_PARAM_VALUE_INDEX:
            if (wLength != sizeof(UINT32))
            {
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_ERROR, "Err wLength = %d\r\n", 1, wLength);
                wCause = ProfileResult_InvalidValueSize;
            }
            else
            {
                gTimeParaValue = *(pValue + 3);
                gTimeParaValue <<= 8;
                gTimeParaValue |= *(pValue + 2);
                gTimeParaValue <<= 8;
                gTimeParaValue |= *(pValue + 1);
                gTimeParaValue <<= 8;
                gTimeParaValue |= *pValue;
                DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "gTimeParaValue = %d ", 1, gTimeParaValue);
            }
            break;
    }
    /* Notify to application. */
    if (pfnPxpExtendedCB && (wCause == ProfileResult_Success))
    {
        pfnPxpExtendedCB(ServiceID, (void *)&data);
    }

    return wCause;

}

/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          ServiceId generated when register to upper stack.
 * @param Index          Attribute index of getting characteristic data.
 * @param wCCCBits                CCCD bits to be updated.
 * @return none.
*/
void PxpExtended_CccdUpdate(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
    TPXP_EXTENDED_PARA_CBs_t cBData = {0};
    bool bHandle = TRUE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "PxpExtended_CccdUpdate Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
        case (SIMPLE_SKEY_VALUE_INDEX + 1):
            {
                if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
                {
                    // enable notification
                    KEY_SEND_FLAG = 1;
                    uint8_t value_to_send = 0;
                    ProfileAPI_SendData(gPXPEXTServiceId, SIMPLE_SKEY_VALUE_INDEX, &value_to_send, sizeof(uint8_t));
                }
                else
                {
                    // disable notification.
                    KEY_SEND_FLAG = 0;
                }
            }
            break;
        default:
            bHandle = false;
            break;
    }
    /* Notify to application. */
    if (pfnPxpExtendedCB && (bHandle == TRUE))
    {
        pfnPxpExtendedCB(ServiceId, (void *)&cBData);
    }

    return;
}

/**< @brief  PXP Extended Service Callbacks, will be registered to stack.  */
CONST gattServiceCBs_t
pxpExtendedCBs =
{
    PxpExtended_AttrRead,  // Read callback function pointer
    PxpExtended_AttrWrite, // Write callback function pointer
    PxpExtended_CccdUpdate  // CCCD update callback function pointer
};

/**
 * @brief Add PXP extended service to stack/profile.
 *
 * @param pFunc          pointer of function to send data to application from PXP extended service.
 * @return ServiceId.   the service ID auto generated by stack when register this service to stack
*/
uint8_t PXPExtended_AddService(void *pFunc)
{
    uint8_t ServiceId;
    /* register service callbacks to stack/profile. */
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t *)gattExtendedServiceTable,
                                       iExtendedServiceTableSize,
                                       pxpExtendedCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "PXPExtended_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }

    pfnPxpExtendedCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

