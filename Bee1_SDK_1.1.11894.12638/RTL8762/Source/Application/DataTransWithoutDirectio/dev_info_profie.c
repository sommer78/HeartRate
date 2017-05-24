enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dev_info_profie.c
* @brief    Device Information service source file.
* @details  Interfaces to access Device Information service.
* @author
* @date     2015-5-12
* @version  v0.1
*********************************************************************************************************
*/
#include "stdint.h"
#include "stddef.h"
#include "rtl_types.h"
#include <string.h>
#include "trace.h"
#include "endialig.h"
#include "gatt.h"
#include "profileApi.h"
#include "dev_info_profile.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  DIS service System ID. */
static  uint8_t DISSystemID[DIS_SYSTEM_ID_LENGTH] = {0, 1, 2, 0, 0, 3, 4, 5};
/**<  DIS service Firmware Revision. */
static const uint8_t DISFirmwareRev[] = "v1.0";

/**< @brief  profile/service definition.  */
static const TAttribAppl DisAttrTbl[] =
{
    /*----------------- Device Information Service -------------------*/
    /* <<Primary Service>> */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR), /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE),  /* service UUID */
            HI_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE)
        },
        UUID_16BIT_SIZE,                              /* bValueLen     */
        NULL,                                     /* pValueContext */
        GATT_PERM_READ                            /* wPermissions  */
    },
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Firmware revision String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_FIRMWARE_REVISION),
            HI_WORD(GATT_UUID_CHAR_FIRMWARE_REVISION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* <<Characteristic>> */
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
    /* System ID String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SYSTEM_ID),
            HI_WORD(GATT_UUID_CHAR_SYSTEM_ID)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
};
/**< @brief  DIS service size definition.  */
const uint16_t iDisAttrTblSize = sizeof(DisAttrTbl);

/**
 * @brief  set Service related data from application.
 *
 * @param param_type        parameter type to set.
 * @param value_ptr             value to set.
 * @return parameter set result.
 * @retval 0 FALSE
 * @retval 1 TRUE
*/
bool DIS_SetParameter( uint8_t param_type, uint8_t length, void *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
        default:
            /* invalid param to set. */
            ret = FALSE;
            break;
        case DIS_PARAM_SYSTEM_ID:
            memcpy(DISSystemID, value_ptr, length);
            break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "DIS parameter set failed\n", 0 );

    return ret;
}

/**
 * @brief  get Service related data from application.
 *
 * @param param_type        parameter type to get.
 * @param value_ptr             value to get.
 * @return parameter get result.
 * @retval 0 FALSE
 * @retval 1 TRUE
*/
bool DIS_GetParameter( uint8_t param_type, void *value_ptr )
{
    bool ret = TRUE;

    switch (param_type)
    {
        default:
            ret = FALSE;
            break;
        case DIS_PARAM_FIRMWARE_REVISION:
            memcpy(value_ptr, DISFirmwareRev, sizeof(DISFirmwareRev));
            break;
        case DIS_PARAM_SYSTEM_ID:
            memcpy(value_ptr, DISSystemID, sizeof(DISSystemID));
            break;
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "DIS parameter get failed\n", 0 );

    return ret;
}

/**
 * @brief read characteristic data from service.
 *
 * @param attrib_index          Attribute index of getting characteristic data.
 * @param offset                Used for Blob Read.
 * @param length_ptr            length of getting characteristic data.
 * @return pointer of getting characteristic data
 * @retval  value_ptr
*/

TProfileResult  DisAttrRead(uint8_t ServiceId , uint16_t attrib_index, uint16_t offset, uint16_t *length_ptr , uint8_t **ppValue )
{
    TProfileResult  wCause  = ProfileResult_Success;
    *length_ptr = 0;

    switch ( attrib_index )
    {
        default:
            /* invalid attribute index. */
            DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "DisAttrRead default:attrib_index=%d\n", 1, attrib_index );
            wCause = ProfileResult_AttrNotFound;
            break;
        case GATT_SVC_DIS_SYSTEM_ID_INDEX:
            *ppValue    = (uint8_t *)&DISSystemID;
            *length_ptr = sizeof(DISSystemID);
            break;
        case GATT_SVC_DIS_FIRMWARE_REV_INDEX:
            *ppValue    = (uint8_t *)&DISFirmwareRev;
            *length_ptr = sizeof(DISFirmwareRev);
            break;
    }
#ifdef PROFILE_DEBUG
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- gatt get DIS attribute value: AttribIndex--%d, length--%d",
               2,
               attrib_index,
               *length_ptr);
#endif
    return ( wCause );
}

CONST gattServiceCBs_t disCBs =
{
    DisAttrRead,  // Read callback function pointer
    NULL, // Write callback function pointer
    NULL  // Authorization callback function pointer
};
uint8_t DIS_AddService(void *pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t *)DisAttrTbl,
                                       iDisAttrTblSize,
                                       disCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "DIS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }
    return ServiceId;
}




