/**
****************************************************************************
* @file     hid_service.h
* @brief    hid service profile related definition
* @author   wenjie/xiaoyu/yanping
* @version  V1.0.0
* @date     2016/11/14
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

/** @defgroup  profile hid service
* @{
*/



#ifndef __HID_SERVICE_H__
#define __HID_SERVICE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "system_config.h"


/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
#define HID_NUM_REPORTS          8

// HID Report IDs for the service
#define HID_RPT_ID_MOUSE_IN      1  // Mouse input report ID
#define HID_RPT_ID_KEY_IN        2  // Keyboard input report ID
#define HID_RPT_ID_CC_IN         3  // Consumer Control input report ID
#define HID_RPT_ID_LED_OUT       0  // LED output report ID
#define HID_RPT_ID_FEATURE       0  // Feature report ID
// HID feature flags
//#define HID_KBD_FLAGS             HID_FLAGS_REMOTE_WAKE|HID_FLAGS_NORMALLY_CONNECTABLE
#define HID_KBD_FLAGS             HID_FLAGS_REMOTE_WAKE

/* Attribute value lengths */
#define HID_PROTOCOL_MODE_LEN       1    // HID Protocol Mode
#define HID_INFORMATION_LEN         4    // HID Information
#define HID_REPORT_REF_LEN          2    // HID Report Reference Descriptor
#define HID_EXT_REPORT_REF_LEN      2    // External Report Reference Descriptor
#define HID_INCLUDE_LEN      6    // incclude

/* HID protocol mode values */
#define HID_PROTOCOL_MODE_BOOT      0x00 // Boot Protocol Mode
#define HID_PROTOCOL_MODE_REPORT    0x01 // Report Protocol Mode

/* Control point commands */
#define HID_CMD_SUSPEND             0x00 // Suspend
#define HID_CMD_EXIT_SUSPEND        0x01 // Exit Suspend

/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE           0x01 // RemoteWake
#define HID_FLAGS_NORMALLY_CONNECTABLE  0x02 // NormallyConnectable

/* HID Report type */
#define HID_REPORT_TYPE_INPUT       1
#define HID_REPORT_TYPE_OUTPUT      2
#define HID_REPORT_TYPE_FEATURE     3

// HID callback events
#define HID_DEV_SUSPEND_EVT         0  // HID suspend
#define HID_DEV_EXIT_SUSPEND_EVT    1  // HID exit suspend
#define HID_DEV_SET_BOOT_EVT        2  // HID set boot mode
#define HID_DEV_SET_REPORT_EVT      3  // HID set report mode

#define HID_LED_OUT_RPT_LEN         1

// HID read/write operation
#define HID_DEV_OPER_WRITE          0  // Write operation
#define HID_DEV_OPER_READ           1  // Read operation
#define HID_DEV_OPER_ENABLE         2  // Notification enabled for report ID
#define HID_DEV_OPER_DISABLE        3  // Notifications disabled for report ID


// Attribute index enumeration-- these indexes match array elements above
enum
{
    HID_SERVICE_IDX = 0,              // HID Service
    HID_INCLUDED_SERVICE_IDX = 1,       // Included Service
//  HID_INFO_DECL_IDX,              // HID Information characteristic declaration
    HID_INFO_IDX = 2,                   // HID Information characteristic
    HID_CONTROL_POINT_IDX = 3,          // HID Control Point characteristic

    HID_PROTOCOL_MODE_IDX = 4,          // HID Protocol Mode characteristic
    HID_REPORT_MAP_IDX = 5,             // HID Report Map characteristic
    HID_REPORT_EXT_REF_IDX = 6,   //HID Report external ref characterstic

    HID_REPORT_MOUSE_IN_IDX = 7,          // HID Report characteristic, mouse input
    HID_REPORT_MOUSE_IN_CCCD_IDX = 8,     // HID Report characteristic client characteristic configuration

    HID_REPORT_KEY_IN_IDX = 9,          // HID Report characteristic, key input
    HID_REPORT_KEY_IN_CCCD_IDX = 10,     // HID Report characteristic client characteristic configuration

    HID_REPORT_CC_IN_IDX = 11,       // HID Report characteristic, consumer control input
    HID_REPORT_CC_IN_CCCD_IDX = 12,     // HID Report characteristic client characteristic configuration


    HID_REPORT_LED_OUT_IDX = 13,         // HID Report characteristic, LED output
//  HID_REPORT_REF_LED_OUT_IDX,     // HID Report Reference characteristic descriptor, LED output
    HID_BOOT_KEY_IN_IDX = 14,            // HID Boot Keyboard Input Report
    HID_BOOT_KEY_IN_CCCD_IDX = 15,       // HID Boot Keyboard Input Report characteristic client characteristic configuration

    HID_BOOT_KEY_OUT_IDX = 16,           // HID Boot Keyboard Output Report

    HID_BOOT_MOUSE_IN_IDX = 17,          // HID Boot Mouse Input Report
    HID_BOOT_MOUSE_IN_CCCD_IDX = 18,     // HID Boot Mouse Input Report characteristic client characteristic configuration

    HID_FEATURE_IDX = 19,                // Feature Report
    HID_MAX_IDX = 20
};

// HID report mapping table
typedef struct
{
    uint16        att_handle;       // Handle of report characteristic
    uint16        uuid;
    uint8         operation;
    uint8         id;           // Report ID
    uint8         type;         // Report type
    uint8         length;
    void          *value_ptr;
} HID_RPT_MAP;


/**
 * global variable
 */
extern uint16 hid_server_hdl[24];
extern HID_RPT_MAP hid_rpt_map[HID_NUM_REPORTS];





/** @name GLOBAL API
* @{
*/



/**
* @brief  Initializes the HID service for keyboard by registering GATT attributes with the GATT server.
* @param  none
* @return Success or Failure
*/
extern uint32 hid_init_service(void);

/**
* @brief  get report index from attribute handler
* @param  attribute_handle
* @return index
*/
extern uint8 hid_get_rpt_idx(uint16 attribute_handle);

/**
* @brief  get handle index from attribute handler
* @param  id
* @param  type
* @return index
*/
extern uint16 hid_get_rpt_att_handle(uint8 id, uint8 type);




/*********************************************************************
 * @fn      HidKbd_GetParameter
 *
 * @brief   Get a HID Kbd parameter.
 *
 * @param   id     - HID report ID.
 * @param   type   - HID report type.
 * @param   uuid   - attribute uuid.
 * @param   pLen   - length of data to be read.
 * @param   pValue - pointer to data to get.  This is dependent on
 *          the input parameters and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 *
 * @return  GATT status code.
 */
extern uint8 HidKbd_GetParameter(uint8 id, uint8 type, uint16 uuid, uint8 *pLen,
                                 void *pValue);

extern uint16  HidDev_GetAttrLenCB(uint16 attribute_handle );
extern uint16  HidDev_ReadAttrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size);

extern uint32 HidDev_WriteAttrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size);

/** @} */




#ifdef __cplusplus
}
#endif

#endif


/** @} */ // 模块结尾,自行拷贝到末尾



