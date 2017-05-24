enum { __FILE_NUM__ = 0 };

#include <FreeRTOS.h>
#include "blueapi.h"
#include "profileApi.h"
#include "hids_keyboard.h"
#include <endialig.h>
#include <string.h>
#include "section_config.h"

pfnAPPHandleInfoCB_t pfnHIDsCB = NULL;

const uint8_t gHIDReportDescriptor[] =
{
    0x05, 0x01,     /* USAGE_PAGE       (Generic Desktop) */
    0x09, 0x06,     /* USAGE            (Keyboard) */
    0xa1, 0x01,     /* COLLECTION       (Application) */
    0x85, HOGP_KB_REPORT_ID,    /* REPORT_ID        (3) */
    0x05, 0x07,     /* USAGE_PAGE       (Keyboard) */
    0x19, 0xe0,     /* USAGE_MINIMUM    (Keyboard Left Control) */
    0x29, 0xe7,     /* USAGE_MAXIMUM    (Keyboard Right GUI) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM  (1) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x95, 0x08,     /* REPORT_COUNT     (8) */
    0x81, 0x02,     /* INPUT            (Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT     (1) */
    0x75, 0x08,     /* REPORT_SIZE      (8) */
    0x81, 0x01,     /* INPUT            (Cnst,Var,Abs) */
    0x95, 0x05,     /* REPORT_COUNT     (5) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x05, 0x08,     /* USAGE_PAGE       (LEDs) */
    0x19, 0x01,     /* USAGE_MINIMUM    (Num Lock) */
    0x29, 0x05,     /* USAGE_MAXIMUM    (Kana) */
    0x91, 0x02,     /* OUTPUT           (Data,Var,Abs) */
    0x95, 0x01,     /* REPORT_COUNT     (1) */
    0x75, 0x03,     /* REPORT_SIZE      (3) */
    0x91, 0x01,     /* OUTPUT           (Cnst,Var,Abs) */
    0x95, 0x06,     /* REPORT_COUNT     (6) */
    0x75, 0x08,     /* REPORT_SIZE      (8) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0xa4,     /* LOGICAL_MAXIMUM  (164) */    /* Can be 255 */
    0x05, 0x07,     /* USAGE_PAGE       (Keyboard) */
    0x19, 0x00,     /* USAGE_MINIMUM    (Reserved-no event indicated) */
    0x29, 0xa4,     /* USAGE_MAXIMUM    (Keyboard Application) */       /* Can be 255 */
    0x81, 0x00,     /* INPUT            (Data,Ary,Abs) */
    0xc0,           /* END_COLLECTION */
#ifdef MULTIMEDIA_KEYBOARD
    0x05, 0x0c,     /* USAGE_PAGE       (Consumer) */
    0x09, 0x01,     /* USAGE            (Consumer Control) */
    0xa1, 0x01,     /* COLLECTION       (Application) */
    0x85, HOGP_MM_KB_REPORT_ID,     /* REPORT_ID        (4) */
    0x15, 0x00,     /* LOGICAL_MINIMUM  (0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM  (1) */
    0x75, 0x01,     /* REPORT_SIZE      (1) */
    0x95, 0x18,     /* REPORT_COUNT     (24) */
    0x09, 0xb5,     /* USAGE            (Scan Next Track) */
    0x09, 0xb6,     /* USAGE            (Scan Previous Track) */
    0x09, 0xb7,     /* USAGE            (Stop) */
    0x09, 0xcd,     /* USAGE            (Play/Pause) */
    0x09, 0xe2,     /* USAGE            (Mute) */
    0x09, 0xe5,     /* USAGE            (Bass Boost) */
    0x09, 0xe7,     /* USAGE            (Loudness) */
    0x09, 0xe9,     /* USAGE            (Volume Increment) */
    0x09, 0xea,     /* USAGE            (Volume Decrement) */
    0x0a, 0x52, 0x01,     /* USAGE      (Bass Increment) */
    0x0a, 0x53, 0x01,     /* USAGE      (Bass Decrement) */
    0x0a, 0x54, 0x01,     /* USAGE      (Treble Increment) */
    0x0a, 0x55, 0x01,     /* USAGE      (Treble Decrement) */
    0x0a, 0x83, 0x01,     /* USAGE      (AL Consumer Control Configuration) */
    0x0a, 0x8a, 0x01,     /* USAGE      (AL Email Reader) */
    0x0a, 0x92, 0x01,     /* USAGE      (AL Calculator) */
    0x0a, 0x94, 0x01,     /* USAGE      (AL Local Machine Browser) */
    0x0a, 0x21, 0x02,     /* USAGE      (AC Search) */
    0x0a, 0x23, 0x02,     /* USAGE      (AC Home) */
    0x0a, 0x24, 0x02,     /* USAGE      (AC Back) */
    0x0a, 0x25, 0x02,     /* USAGE      (AC Forward) */
    0x0a, 0x26, 0x02,     /* USAGE      (AC Stop) */
    0x0a, 0x27, 0x02,     /* USAGE      (AC Refresh) */
    0x0a, 0x2a, 0x02,     /* USAGE      (AC Bookmarks) */
    0x81, 0x02,     /* INPUT            (Data,Var,Abs) */
    0xc0            /* END_COLLECTION */
#endif
};

// for index end
uint8_t gHIDProtocolMode = REPORT_PROCOCOL_MODE;
uint8_t gHIDSuspandMode = 0;
HID_INFO_ATTRB cHidInfo = {0, 0, 0x0100};

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
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions  */
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
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },

    /*  HID Information  characteristic value 2*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_INFO),
            HI_WORD(GATT_UUID_CHAR_HID_INFO)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
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
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },

    /*  HID controlPoint  characteristic value 4*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT),
            HI_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ            /* wPermissions */
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
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },

    /*  HID Protocol Mode  characteristic value 6*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_PROTOCOL_MODE),
            HI_WORD(GATT_UUID_CHAR_PROTOCOL_MODE)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                        /* wPermissions */
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
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
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
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },
 
    /* <<Characteristic>>, .. 9*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_WRITE,               /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },

    /*  HID boot keyboard input  characteristic value  10*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BOOT_KB_IN_REPORT),
            HI_WORD(GATT_UUID_CHAR_BOOT_KB_IN_REPORT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ /* wPermissions */
    },

    /* client characteristic configuration 11*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
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
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* wPermissions */
    },

    /* <<Characteristic>>, .. 12*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP | GATT_CHAR_PROP_WRITE,                 /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                              /* wPermissions */
    },

    /*  HID boot keyboard output  characteristic value  13*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_BOOT_KB_OUT_REPORT),
            HI_WORD(GATT_UUID_CHAR_BOOT_KB_OUT_REPORT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                        /* wPermissions */
    },

    /* <<Characteristic>>, .. 14*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,           /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                            /* wPermissions */
    },

    /*  HID Report characteristic value ,keyboard Input 15*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* wPermissions */
    },

    /* client characteristic configuration 16*/
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
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* wPermissions */
    },

    /*report ID map reference descriptor 17*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HOGP_KB_REPORT_ID, /* client char. config. bit field */
            HID_INPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* wPermissions */
    },
    /* <<Characteristic>>, .. 18*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,             /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                            /* wPermissions */
    },

    /*  HID Report characteristic value ,keyboard Output,19*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* wPermissions */
    },

    /*report ID map reference descriptor 20*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HOGP_KB_REPORT_ID, /* client char. config. bit field */
            HID_OUTPUT_TYPE
        },
        2,                                          /* bValueLen */
        (void*)NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* wPermissions */
    },

#ifdef MULTIMEDIA_KEYBOARD
    /* <<Characteristic>>, .. 21*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE,           /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                            /* wPermissions */
    },

    /*  HID Report characteristic value ,multimedia keyboard Input 22*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        3,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ                          /* wPermissions */
    },

    /* client characteristic configuration 23*/
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
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* wPermissions */
    },

    /*report ID map reference descriptor 24*/
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
        ATTRIB_FLAG_CCCD_APPL),
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HOGP_MM_KB_REPORT_ID, /* client char. config. bit field */
            HID_INPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ)          /* wPermissions */
    }
#endif
};



uint16_t HIDSAttrTblLen = sizeof(HIDSAttrTbl);

uint8_t* pKeyboardData = NULL;
uint16_t KeyboardDataLen = 0;

/** @addtogroup HIDS_Keyboard_Exported_Functions HIDS Keyboard Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */

/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set, ref: @ref HIDS_Keyboard_Application_Parameters
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
			KeyboardDataLen = length;
			pKeyboardData = value_ptr;
		}
		break;
	default:
		ret = false;
		break;
		
	}
	return ret;
}

/** @} End of HIDS_Keyboard_Exported_Functions */

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
		case GATT_SRV_HID_KB_BOOT_OUTPUT_INDEX:
		case GATT_SRV_HID_KB_OUTPUT_INDEX:
			{
					callback_data.msg_data.read_value_index = HID_READ_OUTPUT_KEYBOARD;
					if (pfnHIDsCB)
					{
							pfnHIDsCB(ServiceID, (void*)&callback_data);
					}
						pValue = pKeyboardData;
						*piLength = KeyboardDataLen;
                                if(*piLength == 0)
            {
                  uint8_t  keyboard[1]= {0};
                  pValue = keyboard;
                  *piLength = sizeof(keyboard);
             }
		  }
			break;
	case GATT_SRV_HID_KB_BOOT_INPUT_INDEX:
	case GATT_SRV_HID_KB_INPUT_INDEX:
#ifdef MULTIMEDIA_KEYBOARD
	case GATT_SRV_HID_MM_KB_INPUT_INDEX:
#endif
		{		
		    callback_data.msg_data.read_value_index = HIDS_READ_PARAM_REPORT;
			if (pfnHIDsCB)
			{
				pfnHIDsCB(ServiceID, (void*)&callback_data);
			}
			pValue = (uint8_t *)pKeyboardData;
            *piLength = KeyboardDataLen;
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
			callback_data.msg_data.read_value_index = HIDS_READ_PARAM_SUSPEND_MODE;
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
			callback_data.msg_data.read_value_index = HIDS_READ_PARAM_PROTOCOL_MODE;
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
        DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "<-- GetHIDSValue piLength=%d  iAttribIndex=%d *pValue=%x",
                   3, *piLength, iAttribIndex, *pValue);
        *ppValue = pValue;
    }
    return ( wCause );
}

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
	case GATT_SRV_HID_KB_OUTPUT_INDEX:
	case GATT_SRV_HID_KB_BOOT_OUTPUT_INDEX:
		{
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.msg_data.write.write_type = HID_WRITE_OUTPUT;
            callback_data.msg_data.write.write_parameter.report_data.reportLen = wLength;
		    callback_data.msg_data.write.write_parameter.report_data.report = pValue;	
            wCause = ProfileResult_Success;
            DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "--> host write output %x",
                       1,*pValue);
        }
        break;
					case GATT_SRV_HID_KB_BOOT_INPUT_INDEX:
	        case GATT_SRV_HID_KB_INPUT_INDEX:
#ifdef MULTIMEDIA_KEYBOARD
            case GATT_SRV_HID_MM_KB_INPUT_INDEX:
#endif
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
    case GATT_SRV_HID_KB_BOOT_CCCD_INDEX:
        {

        }
        break;

    case GATT_SRV_HID_KB_CCCD_INDEX:
        {
            if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_KB_ENABLE;

            }
            else
            {
                callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_KB_DISABLE;
            }

            break;
        }
#ifdef MULTIMEDIA_KEYBOARD
    case GATT_SRV_HID_MM_KB_INPUT_CCCD_INDEX:
        if (wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
        {
            // Enable Notification
            callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_MM_KB_ENABLE;

        }
        else
        {
            callback_data.msg_data.notification_indification_index = HID_NOTIFY_INDICATE_MM_KB_DISABLE;
        }
        break;
#endif
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

/** @addtogroup HIDS_Keyboard_Exported_Functions HIDS Keyboard Exported Functions
  * @brief functions that other .c files may use all defined here
  * @{
  */

/**
  * @brief Send report to client
  *
  * @param[in] ServiceId         Service ID to notify.
  * @param[in] Index         	 Index of Attribute, ref: @ref HIDS_Keyboard_Attribute_Index
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

/** @} End of HIDS_Keyboard_Exported_Functions */

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

/** @addtogroup HIDS_Keyboard_Exported_Functions HIDS Keyboard Exported Functions
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

/** @} End of HIDS_Keyboard_Exported_Functions */
