enum { __FILE_NUM__ = 0 };

#include <FreeRTOS.h>
#include "blueapi.h"
#include "profileApi.h"
#include "hids_mouse.h"
#include <endialig.h>
#include <string.h>
#include "section_config.h"

#define ATTRIB_FLAG_CCCD_NO_FILTER  0x0020
pfnAPPHandleInfoCB_t pfnHIDsCB = NULL;

const uint8_t gHIDReportDescriptor[] =
{
    0x05, 0x01,     /* USAGE_PAGE       (Generic Desktop) */
    0x09, 0x02,     /* USAGE            (Mouse) */
    0xa1, 0x01,     /* COLLECTION       (Application) */
    0x85, HOGP_MOUSE_REPORT_ID, /* REPORT_ID        (3) */
    0x09, 0x01,     /* USAGE            (Pointer) */
    0xa1, 0x00,     /* COLLECTION       (Physical) */
    0x05, 0x09,     /* USAGE_PAGE       (Button) */
    0x19, 0x01,     /* USAGE_MINIMUM    (Button 1) */
#ifdef FIVE_KEY_MOUSE
	0x29, 0x05,     /* USAGE_MAXIMUM    (Button 5) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM  (1) */
    0x95, 0x05,     /* REPORT_COUNT     (5) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x81, 0x02,     /* INPUT            (Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT     (1) */
    0x75, 0x03,     /* REPORT_SIZE      (3) */
#else
	0x29, 0x03,     /* USAGE_MAXIMUM    (Button 3) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM  (1) */
    0x95, 0x03,     /* REPORT_COUNT     (3) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x81, 0x02,     /* INPUT            (Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT     (1) */
    0x75, 0x05,     /* REPORT_SIZE      (5) */
#endif
    
    0x81, 0x01,     /* INPUT            (Cnst,Var,Abs) */
    0x05, 0x01,     /* USAGE_PAGE       (Generic Desktop) */
    0x09, 0x30,     /* USAGE            (X) */
    0x09, 0x31,     /* USAGE            (Y) */
    0x09, 0x38,     /* USAGE            (Wheel) */
    0x15, 0x81,     /* LOGICAL_MINIMUM  (-127) */
    0x25, 0x7f,     /* LOGICAL_MAXIMUM  (127) */
    0x75, 0x08,     /* REPORT_SIZE      (8) */
    0x95, 0x03,     /* REPORT_COUNT     (3) */
    0x81, 0x06,     /* INPUT            (Data,Var,Rel) */
    0xc0,           /* END_COLLECTION */
    0xc0,           /* END_COLLECTION */

};

// for index end

uint8_t gHIDProtocolMode = REPORT_PROCOCOL_MODE;
uint8_t gHIDSuspandMode = 0;

HID_INFO_ATTRB cHidInfo = {0x0101, 0, 0x03};

const TAttribAppl HIDSAttrTbl[] =
{
    /*--------------------------HID Service ---------------------------*/
    /* <<Primary Service>>, .. 0*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_HID),               /* service UUID */
            HI_WORD(GATT_UUID_HID)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },
    /* <<Characteristic>>, .. 1*/
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

    /*  HID Information  characteristic value 2*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_INFO),
            HI_WORD(GATT_UUID_CHAR_HID_INFO)
        },
        4,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* <<Characteristic>>, .. 3*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP,                      /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID controlPoint  characteristic value 4*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT),
            HI_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT)
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    },

    /* <<Characteristic>>, .. 5*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP,                    /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID Protocol Mode  characteristic value 6*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_PROTOCOL_MODE),
            HI_WORD(GATT_UUID_CHAR_PROTOCOL_MODE)
        },
        1,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                        /* wPermissions */
    },

    /* <<Characteristic>>, .. 7*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                  /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID Report Map  characteristic value 8*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_MAP),
            HI_WORD(GATT_UUID_CHAR_REPORT_MAP)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    }
    ,
     /* <<Characteristic>>, .. 9*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_WRITE,               /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID boot mouse input  characteristic value 10*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BOOT_MOUSE_IN_REPORT),
            HI_WORD(GATT_UUID_CHAR_BOOT_MOUSE_IN_REPORT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                            /* wPermissions */
    },

    /* client characteristic configuration 11*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL|ATTRIB_FLAG_CCCD_NO_FILTER),
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

    /* <<Characteristic>>, .. 12*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_NOTIFY,           /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value ,pointer Input 13*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE /* wPermissions */
    },

    /* client characteristic configuration 14*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL|ATTRIB_FLAG_CCCD_NO_FILTER),
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

    /*report ID map reference descriptor 15*/
    {
        ATTRIB_FLAG_VALUE_INCL,                 /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HOGP_MOUSE_REPORT_ID, /* client char. config. bit field */
            HID_INPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,//(void*)&cPointerInputReportIdMap,
        (GATT_PERM_READ)        /* wPermissions */
    }  
};

uint16_t HIDSAttrTblLen = sizeof(HIDSAttrTbl);

uint8_t* pMouseData = NULL;
uint16_t MouseDataLen = 0;

/** @addtogroup HIDS_Mouse_Exported_Functions HIDS Mouse Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set, ref: @ref HIDS_Mouse_Application_Parameters
  * @param[in] length                value length to be set.
  * @param[in] value_ptr             value to set.
  * @return parameter set result.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool HIDS_SetParameter( uint8_t param_type, uint8_t length, void *value_ptr )
{
	bool ret = TRUE;
	
	switch(param_type)
	{
	case HIDS_PARAM_PROTOCOL_MODE:
		{
			gHIDProtocolMode = *((uint8_t*)value_ptr);
		}
		break;
	case HIDS_PARAM_SUSPEND_MODE:
		{
			gHIDSuspandMode = *((uint8_t*)value_ptr);
		}
		break;
	case HIDS_PARAM_HID_INFO:
		{
			memcpy((void*)&cHidInfo,value_ptr,length);
		}
		break;
	case HIDS_PARAM_REPORT:
		{
			MouseDataLen = length;
			pMouseData = value_ptr;
		}
		break;		
	default:
		ret = false;
		break;
		
	}
	return ret;
}

/** @} End of HIDS_Mouse_Exported_Functions */

/*----------------------------------------------------------------------------
 * get HID value
 * --------------------------------------------------------------------------*/

TProfileResult  HidsAttrReadCb(uint8_t ServiceID, uint16_t iAttribIndex,
                             uint16_t iOffset, uint16_t * piLength, uint8_t **ppValue)
{
    uint8_t * pValue = NULL;
    TProfileResult  wCause  = ProfileResult_Success;
    THID_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
    *piLength = 0;

    switch (iAttribIndex)
    {
    default:
        wCause = ProfileResult_AttrNotFound;
        break;
	case GATT_SRV_HID_MOUSE_BOOT_INPUT_INDEX:
	case GATT_SRV_HID_MOUSE_INPUT_INDEX:
		{		
		    callback_data.msg_data.read_value_index = HIDS_READ_PARAM_REPORT;
			if (pfnHIDsCB)
			{
				pfnHIDsCB(ServiceID, (void*)&callback_data);
			}
			pValue = (uint8_t *)pMouseData;
            *piLength = MouseDataLen;
            if(*piLength == 0)
            {
                  uint8_t  keyboard[1]= {0};
                  pValue = keyboard;
                  *piLength = sizeof(keyboard);
             }
		}
	break;
    case GATT_SVC_HID_INFO_INDEX:
        {		
            pValue = (uint8_t *)&cHidInfo;
            *piLength = sizeof(cHidInfo);
        }
        break;
    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        {			
			callback_data.msg_data.read_value_index = HIDS_READ_SUSPEND_MODE;
			if (pfnHIDsCB)
			{
				pfnHIDsCB(ServiceID, (void*)&callback_data);
			}
            pValue = &gHIDSuspandMode;
            *piLength = 1;
        }
        break;
    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        {			
			callback_data.msg_data.read_value_index = HIDS_READ_PROTOCOL_MODE;
			if (pfnHIDsCB)
			{
				pfnHIDsCB(ServiceID, (void*)&callback_data);
			}
            pValue = &gHIDProtocolMode;
            *piLength = sizeof(gHIDProtocolMode);
        }
        break;
    case GATT_SVC_HID_REPORT_MAP_INDEX:
        pValue = (uint8_t*)gHIDReportDescriptor;
        *piLength = sizeof(gHIDReportDescriptor);
        break;

    }
	


    if (wCause == ProfileResult_Success)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- GetHIDSValue piLength=%d  iAttribIndex=%d pValue=%p",
                   3, *piLength, iAttribIndex, pValue);
        *ppValue = pValue;
    }
    return ( wCause );
}

/*----------------------------------------------------------------------------
 * put/write (local) attribute value
 * --------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * put/write (local) attribute value
 * --------------------------------------------------------------------------*/
TProfileResult HidsAttrWriteCb(uint8_t ServiceId, uint16_t iAttribIndex,
                              uint16_t wLength, uint8_t * pValue , TGATTDWriteIndPostProc * pWriteIndPostProc)
{
    TProfileResult  wCause  = ProfileResult_Success;
    THID_CALLBACK_DATA callback_data;
    if (!pValue)
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "--> HidsAttrWrite   pValue %p wLength= 0x%x",
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
    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        {
            gHIDProtocolMode = *pValue;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_type = HID_WRITE_PROTOCOL_MODE;
            callback_data.msg_data.write.write_parameter.protocol_mode= *pValue;

            wCause = ProfileResult_Success;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> host write gHIDProtocolMode %d ",
                       1,
                       gHIDProtocolMode);
        }
        break;
    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        {
            gHIDSuspandMode = *pValue;
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_type = HID_WRITE_SUSPEND_MODE;
            callback_data.msg_data.write.write_parameter.suspend_mode= *pValue;

            wCause = ProfileResult_Success;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> host write gHIDSuspandMode %d",
                       1,
                       gHIDSuspandMode);
        }
        break;
		case GATT_SRV_HID_MOUSE_INPUT_INDEX:// FOR bqb
		case GATT_SRV_HID_MOUSE_BOOT_INPUT_INDEX:			

		{
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_type = HID_WRITE_INPUT_REPORT;
           	callback_data.msg_data.write.write_parameter.report_data.reportLen = wLength;
			    	callback_data.msg_data.write.write_parameter.report_data.report = pValue;	
            wCause = ProfileResult_Success;
		}
			break;
    }
    if (pfnHIDsCB && (wCause == ProfileResult_Success))
    {
        pfnHIDsCB(ServiceId, (void*)&callback_data);
    }

    return wCause;

}

void HidsCccdUpdateCb(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
    THID_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    
    bool bHandle = TRUE;
    DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HidsCccdUpdate  Index = %d wCCCDBits %x", 2, Index, wCCCBits);
    switch (Index)
    {
    case GATT_SRV_HID_MOUSE_BOOT_CCCD_INDEX:
        {

        }
        break;

    case GATT_SRV_HID_MOUSE_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_MOUSE_ENABLE;

            }
            else
            {
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_MOUSE_DISABLE;
            }

            break;
        }
    default:
        bHandle = false;
        break;
    }

    if (pfnHIDsCB && (bHandle == TRUE))
    {
        pfnHIDsCB(ServiceId, (void*)&callback_data);
    }

    return;
}

/** @addtogroup HIDS_Mouse_Exported_Functions HIDS Mouse Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */

/**
  * @brief Send report to client
  *
  * @param[in] ServiceId         Service ID to notify.
  * @param[in] Index         	 Index of Attribute, ref: @ref HIDS_Mouse_Attribute_Index
  * @param[in] pData             Pointer of the data to send
  * @param[in] dataLen           Length of the data to send
  * @return Result of send report.
  * @retval 0 FALSE
  * @retval 1 TRUE
  */
bool HID_SendReport(uint8_t ServiceId, uint16_t Index, uint8_t* pData, uint16_t dataLen)
{
    return ProfileAPI_SendData(ServiceId, Index, pData, dataLen);
}

/** @} End of HIDS_Mouse_Exported_Functions */

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Battery Service Callbacks
CONST gattServiceCBs_t hidCBs =
{
    HidsAttrReadCb,  // Read callback function pointer
    HidsAttrWriteCb, // Write callback function pointer
    HidsCccdUpdateCb  // Authorization callback function pointer
};

/** @addtogroup HIDS_Mouse_Exported_Functions HIDS Mouse Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */
  
/**
  * @brief add HIDS service to application.
  *
  * @param[in] pFunc          pointer of app callback function called by profile.
  * @return service ID auto generated by profile layer.
  * @retval ServiceId
  */
uint8_t HIDS_AddService(void* pFunc)
{
    uint8_t ServiceId;
    if (FALSE == ProfileAPI_AddService(&ServiceId,
                                       (uint8_t*)HIDSAttrTbl,
                                       HIDSAttrTblLen,
                                       hidCBs))
    {
        DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "HIDS_AddService: ServiceId %d", 1, ServiceId);
        ServiceId = 0xff;
    }

    pfnHIDsCB = (pfnAPPHandleInfoCB_t)pFunc;
    return ServiceId;
}

/** @} End of HIDS_Mouse_Exported_Functions */
