/**
****************************************************************************
* @file		battery_service.h
* @brief	
* @author	wenjie
* @version	v1.0.1
* @date		2016/11/14
* @history	modify the coding style 
* @note		
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/


/** @defgroup  profile battery service
* @{
*/
 


#ifndef BATTSERVICE_H
#define BATTSERVICE_H



#ifdef __cplusplus
extern "C"
{
#endif

/** @name MACRO DEFINITION
* @{
*/


// Battery Service Get/Set Parameters
#define BATT_PARAM_LEVEL                0
#define BATT_PARAM_CRITICAL_LEVEL       1
#define BATT_PARAM_SERVICE_HANDLE       2
#define BATT_PARAM_BATT_LEVEL_IN_REPORT 3

// Callback events
#define BATT_LEVEL_NOTI_ENABLED         1
#define BATT_LEVEL_NOTI_DISABLED        2

// HID Report IDs for the service
#define HID_RPT_ID_BATT_LEVEL_IN        4  // Battery Level input report ID


// ADC voltage levels
#define BATT_ADC_LEVEL_3V           409
#define BATT_ADC_LEVEL_2V           273

 
#define BATT_LEVEL_VALUE_IDX  				0  // Position of battery level in attribute array
#define BATT_LEVEL_VALUE_CLIENT_CFG_IDX		1  // Position of battery level CCCD in attribute array


#define BATT_LEVEL_VALUE_LEN        1

/**
 * GATT Characteristic Descriptions
 */
#define GATT_DESC_LENGTH_UUID            0x3111 // Used with Unit percent

/** @} */


/** @name GLOBAL FUNCTION DECLARATION
* @{
*/




/**
 * @brief   Initializes the Battery service by registering
 *          GATT attributes with the GATT server.
 * @param	none 
 * @return  Success or Failure
 */
extern uint32 batt_init_service( void );



/**
* @brief	Measure the battery level and update the battery
*              level value in the service characteristics.  If
*              the battery level-state characteristic is configured
*              for notification and the battery level has changed
*              since the last measurement, then a notification
*              will be sent.
* @param	none 
* @return	success or failure 
*/

extern uint16 batt_level_measured( void );


extern uint16  batt_read_attrCB(uint16 attribute_handle, uint16 offset, uint8  * buffer, uint16  buffer_size);


extern uint32 batt_write_attrCB(uint16 attribute_handle, uint16  transaction_mode, uint16 offset, uint8  *buffer, uint16 buffer_size);


/** @} */



#ifdef __cplusplus
}
#endif

#endif /* BATTSERVICE_H */

/** @} */ // 模块结尾,自行拷贝到末尾



