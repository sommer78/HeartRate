enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     dis.c
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
#include "dis.h"
#include "dis_config.h"

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/

static pfnAPPHandleInfoCB_t pfnDisCB = NULL;


#ifdef DIS_CHAR_SYSTEM_ID_SUPPORT
/**<  DIS service System ID. */
static uint8_t DISSystemID[DIS_SYSTEM_ID_LENGTH] = {0, 1, 2, 0, 0, 3, 4, 5};
#endif


#ifdef DIS_CHAR_PNP_ID_SUPPORT
/**<  DIS service PnP ID. */
static uint8_t DISPnpID[DIS_PNP_ID_LENGTH] = 
{
    1,                                  // Vendor ID source (1=Bluetooth SIG)
    LO_WORD(0x005D), HI_WORD(0x005D),   // Vendor ID (Realtek Semiconductor Corporation)
    LO_WORD(0x0000), HI_WORD(0x0000),   // Product ID (vendor-specific)
    LO_WORD(0x0100), HI_WORD(0x0100)    // Product version (vendor-assigned-JJ.M.N)
};
#endif

#ifdef DIS_CHAR_MANUFACTURER_NAME_SUPPORT
/**<  DIS service Manufacturer Name. A null-terminated byte is reserved*/
static uint8_t DISManufacturerName[DIS_CHAR_MANUFACTURER_NAME_STR_MAX_LENGTH + 1] = "";
uint8_t DISManufacturerNameLen = 0;
#endif

#ifdef DIS_CHAR_MODEL_NUMBER_SUPPORT
/**<  DIS service Model Number. A null-terminated byte is reserved*/
static uint8_t DISModelNumber[DIS_CHAR_MODEL_NUMBER_STR_MAX_LENGTH + 1] = "";
uint8_t DISModelNumberLen = 0;
#endif

#ifdef DIS_CHAR_SERIAL_NUMBER_SUPPORT
/**<  DIS service Serial Number. A null-terminated byte is reserved*/
static uint8_t DISSerialNumber[DIS_CHAR_SERIAL_NUMBER_STR_MAX_LENGTH + 1] = "";
uint8_t DISSerialNumberLen = 0;
#endif

#ifdef DIS_CHAR_HARDWARE_REVISION_SUPPORT
/**<  DIS service Hardware Revision. A null-terminated byte is reserved*/
static uint8_t DISHardwareRev[DIS_CHAR_HARDWARE_REVISION_STR_MAX_LENGTH + 1] = "";
uint8_t DISHardwareRevLen = 0;
#endif

#ifdef DIS_CHAR_FIRMWARE_REVISION_SUPPORT
/**<  DIS service Firmware Revision. A null-terminated byte is reserved*/
static uint8_t DISFirmwareRev[DIS_CHAR_FIRMWARE_REVISION_STR_MAX_LENGTH + 1] = "";
uint8_t DISFirmwareRevLen = 0;
#endif

#ifdef DIS_CHAR_SOFTWARE_REVISION_SUPPORT
/**<  DIS service Software Revision. A null-terminated byte is reserved*/
static uint8_t DISSoftwareRev[DIS_CHAR_SOFTWARE_REVISION_STR_MAX_LENGTH + 1] = "";
uint8_t DISSoftwareRevLen = 0;
#endif

#ifdef DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
/**<  DIS service IEEE 11073-20601 Regulatory Certification Data List. */
static uint8_t DISIEEEDataList[DIS_CHAR_IEEE_CERTIF_DATA_LIST_MAX_LENGTH] = 
{
    DIS_IEEE_11073_BODY_EXP,      // Authoritative body type
    0x00,                         // Authoritative body structure type
                                // Authoritative body data follows below:
    'e', 'x', 'p', 'e', 'r', 'i', 'm', 'e', 'n', 't', 'a', 'l'
};
uint8_t DISIEEEDataListLen = 14;
#endif

/**< @brief  profile/service definition.  */
static const TAttribAppl DisAttrTbl[] =
{
    /*----------------- Device Information Service -------------------*/
    /* <<Primary Service>> */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE | ATTRIB_FLAG_BR_EDR), /* wFlags     */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE),  /* service UUID */
            HI_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE)
        },
        UUID_16BIT_SIZE,                              /* bValueLen     */
        NULL,                                     /* pValueContext */
        GATT_PERM_READ                            /* wPermissions  */
    }

#ifdef DIS_CHAR_MANUFACTURER_NAME_SUPPORT
    ,
    /* <<Characteristic>> */
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
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_MANUFACTURER_NAME),
            HI_WORD(GATT_UUID_CHAR_MANUFACTURER_NAME)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef DIS_CHAR_MODEL_NUMBER_SUPPORT
    ,
    /* <<Characteristic>> */
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
    /* Model Number characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_MODEL_NUMBER),
            HI_WORD(GATT_UUID_CHAR_MODEL_NUMBER)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif       

#ifdef DIS_CHAR_SERIAL_NUMBER_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Serial Number String String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SERIAL_NUMBER),
            HI_WORD(GATT_UUID_CHAR_SERIAL_NUMBER)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif            

#ifdef DIS_CHAR_HARDWARE_REVISION_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HARDWARE_REVISION),
            HI_WORD(GATT_UUID_CHAR_HARDWARE_REVISION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif            

#ifdef DIS_CHAR_FIRMWARE_REVISION_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
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
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_FIRMWARE_REVISION),
            HI_WORD(GATT_UUID_CHAR_FIRMWARE_REVISION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif            

#ifdef DIS_CHAR_SOFTWARE_REVISION_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SOFTWARE_REVISION),
            HI_WORD(GATT_UUID_CHAR_SOFTWARE_REVISION)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef DIS_CHAR_SYSTEM_ID_SUPPORT
    ,
    /* <<Characteristic>> */
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
    /* System ID String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_SYSTEM_ID),
            HI_WORD(GATT_UUID_CHAR_SYSTEM_ID)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
     ,

    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_IEEE_CERTIF_DATA_LIST),
            HI_WORD(GATT_UUID_CHAR_IEEE_CERTIF_DATA_LIST)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif

#ifdef DIS_CHAR_PNP_ID_SUPPORT
    ,
    /* <<Characteristic>> */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Manufacturer Name String characteristic value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_PNP_ID),
            HI_WORD(GATT_UUID_CHAR_PNP_ID)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
#endif            
};
/**< @brief  DIS service size definition.  */
const uint16_t iDisAttrTblSize = sizeof(DisAttrTbl);

/**
  * @brief  set Service related data from application.
  *
  * @param[in] param_type            parameter type to set: @ref DIS_Application_Parameters
  * @param[in] length                length of value, not include null-terminated byte.
  * @param[in] value_ptr             value to set
  * @return parameter set result
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
        
#ifdef DIS_CHAR_MANUFACTURER_NAME_SUPPORT        
    case DIS_PARAM_MANUFACTURER_NAME:
        if(length > DIS_CHAR_MANUFACTURER_NAME_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_MANUFACTURER_NAME_STR_MAX_LENGTH;
        }
        DISManufacturerNameLen = length + 1;//Add one byte for  null-terminated byte
        memcpy(DISManufacturerName, value_ptr, length);
        break;
#endif

#ifdef DIS_CHAR_MODEL_NUMBER_SUPPORT
    case DIS_PARAM_MODEL_NUMBER:
        if(length > DIS_CHAR_MODEL_NUMBER_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_MODEL_NUMBER_STR_MAX_LENGTH;
        }
        DISModelNumberLen = length + 1;//Add one byte for  null-terminated byte        
        memcpy(DISModelNumber, value_ptr, length);
        break;
#endif

#ifdef DIS_CHAR_SERIAL_NUMBER_SUPPORT
    case DIS_PARAM_SERIAL_NUMBER:
        if(length > DIS_CHAR_SERIAL_NUMBER_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_SERIAL_NUMBER_STR_MAX_LENGTH;
        } 
        DISSerialNumberLen = length + 1;//Add one byte for  null-terminated byte
        memcpy(DISSerialNumber, value_ptr, length);
        
        break;
#endif

#ifdef DIS_CHAR_HARDWARE_REVISION_SUPPORT
    case DIS_PARAM_HARDWARE_REVISION:
        if(length > DIS_CHAR_HARDWARE_REVISION_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_HARDWARE_REVISION_STR_MAX_LENGTH;
        }
        DISHardwareRevLen = length + 1;//Add one byte for  null-terminated byte
        memcpy(DISHardwareRev, value_ptr, length);
        break;
#endif        

#ifdef DIS_CHAR_FIRMWARE_REVISION_SUPPORT
    case DIS_PARAM_FIRMWARE_REVISION:
        if(length > DIS_CHAR_FIRMWARE_REVISION_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_FIRMWARE_REVISION_STR_MAX_LENGTH;
        }
        DISFirmwareRevLen = length + 1;//Add one byte for  null-terminated byte
        memcpy(DISFirmwareRev, value_ptr, length);
        break;
#endif

#ifdef DIS_CHAR_SOFTWARE_REVISION_SUPPORT
    case DIS_PARAM_SOFTWARE_REVISION:
        if(length > DIS_CHAR_SOFTWARE_REVISION_STR_MAX_LENGTH)
        {
            length = DIS_CHAR_SOFTWARE_REVISION_STR_MAX_LENGTH;
        }
        DISSoftwareRevLen = length + 1;//Add one byte for  null-terminated byte
        memcpy(DISSoftwareRev, value_ptr, length);
        break;
#endif

#ifdef DIS_CHAR_SYSTEM_ID_SUPPORT
    case DIS_PARAM_SYSTEM_ID:
        memcpy(DISSystemID, value_ptr, length);
        break;
#endif

#ifdef DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
    case DIS_PARAM_IEEE_DATA_LIST:
        if(length > DIS_CHAR_IEEE_CERTIF_DATA_LIST_MAX_LENGTH)
        {
            length = DIS_CHAR_IEEE_CERTIF_DATA_LIST_MAX_LENGTH;
        }
        DISIEEEDataListLen = length;
        memcpy(DISIEEEDataList, value_ptr, length);
        break;
#endif

#ifdef DIS_CHAR_PNP_ID_SUPPORT
    case DIS_PARAM_PNP_ID:
        memcpy(DISPnpID, value_ptr, length);
        break;
#endif        
    }

    if ( !ret )
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "DIS parameter set failed\n", 0 );

    return ret;
}



/**
 * @brief read characteristic data from service.
 *
 * @param ServiceID          ServiceID to be read. 
 * @param attrib_index          Attribute index of getting characteristic data.
 * @param offset                offset of characteritic to be read.
 * @param length_ptr            length of getting characteristic data.
 * @param ppValue            pointer to pointer of characteristic value to be read.
 * @return TProfileResult
*/
TProfileResult  DisAttrReadCb(uint8_t ServiceId , uint16_t attrib_index, uint16_t offset, uint16_t * length_ptr , uint8_t **ppValue )
{
    TDIS_CALLBACK_DATA callback_data;
    TProfileResult  wCause  = ProfileResult_Success;
    *length_ptr = 0;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
    
    switch ( attrib_index )
    {
    default:
        /* invalid attribute index. */
        DBG_BUFFER(MODULE_PROFILE, LEVEL_WARN, "DisAttrRead default:attrib_index=%d\n", 1, attrib_index );
        wCause = ProfileResult_AttrNotFound;
        break;
        
#ifdef DIS_CHAR_SYSTEM_ID_SUPPORT
    case GATT_SVC_DIS_SYSTEM_ID_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_SYSTEM_ID_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);
        
        *ppValue    = (uint8_t *)&DISSystemID;
        *length_ptr = sizeof(DISSystemID);
        break;
#endif

#ifdef DIS_CHAR_MANUFACTURER_NAME_SUPPORT        
    case GATT_SVC_DIS_MANU_NAME_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_MANU_NAME_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);
        
        *ppValue    = (uint8_t *)&DISManufacturerName;
        *length_ptr = DISManufacturerNameLen;
        break;
#endif

#ifdef DIS_CHAR_SERIAL_NUMBER_SUPPORT
    case GATT_SVC_DIS_SERIAL_NUM_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_SERIAL_NUM_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);        
        *ppValue    = (uint8_t *)&DISSerialNumber;
        *length_ptr = DISSerialNumberLen;
        break;
#endif

#ifdef DIS_CHAR_FIRMWARE_REVISION_SUPPORT
    case GATT_SVC_DIS_FIRMWARE_REV_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_FIRMWARE_REV_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);                
        *ppValue    = (uint8_t *)&DISFirmwareRev;
        *length_ptr = DISFirmwareRevLen;
        break;
#endif

#ifdef DIS_CHAR_HARDWARE_REVISION_SUPPORT
    case GATT_SVC_DIS_HARDWARE_REV_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_HARDWARE_REV_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);                
        *ppValue    = (uint8_t *)&DISHardwareRev;
        *length_ptr = DISHardwareRevLen;
        break;
#endif

#ifdef DIS_CHAR_SOFTWARE_REVISION_SUPPORT
    case GATT_SVC_DIS_SOFTWARE_REV_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_SOFTWARE_REV_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);                
        *ppValue    = (uint8_t *)&DISSoftwareRev;
        *length_ptr = DISSoftwareRevLen;
        break;
#endif

#ifdef DIS_CHAR_IEEE_CERTIF_DATA_LIST_SUPPORT
    case GATT_SVC_DIS_IEEE_CERT_STR_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_IEEE_CERT_STR_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);        
        *ppValue    = (uint8_t *)&DISIEEEDataList;
        *length_ptr = DISIEEEDataListLen;
        break;
#endif
        
#ifdef DIS_CHAR_PNP_ID_SUPPORT
    case GATT_SVC_DIS_PNP_ID_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_PNP_ID_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);                
        *ppValue    = (uint8_t *)&DISPnpID;
        *length_ptr = sizeof(DISPnpID);
        break;
#endif

#ifdef DIS_CHAR_MODEL_NUMBER_SUPPORT
    case GATT_SVC_DIS_MODEL_NUM_INDEX:
        callback_data.msg_data.read_value_index = DIS_READ_MODEL_NUM_INDEX;
        pfnDisCB(ServiceId, (void*)&callback_data);                
        *ppValue    = (uint8_t *)&DISModelNumber;
        *length_ptr = sizeof(DISModelNumber);
        break;
#endif
        
    }

    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- gatt get DIS attribute value: AttribIndex--%d, length--%d",
               2,
               attrib_index,
               *length_ptr);
    return ( wCause );
}

CONST gattServiceCBs_t disCBs =
{
    DisAttrReadCb,  // Read callback function pointer
    NULL, // Write callback function pointer
    NULL  // Authorization callback function pointer
};

/**
  * @brief add DIS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval  ServiceId
  */
uint8_t DIS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)DisAttrTbl,
                                       iDisAttrTblSize,
                                       disCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "DIS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }
    pfnDisCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}




