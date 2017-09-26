/**
 *****************************************************************************
 * @file     app_config.h
 * @author   Richard
 * @version  V1.0.0
 * @date     20-Sep-2016
 * @brief    all function macro definition here 
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */

 /**
  * @addtogroup Application
  * @{
  * @defgroup app_config app_config
  * @{
  */
 
#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "system_config.h"
#include "app_task.h"

/**
 * @name Function macro definition
 * @{
 */
#define FUNC_OTA_EN                       /**<OTA scheme*/
#define FUNC_KEYBOARD_EN                  /**<Keyboard scheme*/
//#define FUNC_MOUSE_EN                   /**<Mouse scheme*/
//#define FUNC_BEACON_EN                    /**<Beacon scheme*/
//#define FUNC_WRISTBAND_EN                 /**<wristband scheme*/
//#define FUNC_PEDOMETER_EN                 /**<Pedometer scheme*/
//#define FUNC_PROXIMITY_EN                 /**<Proximity scheme*/


#ifdef FUNC_KEYBOARD_EN
	#define KEYBOARD_LOG_EN 1
    #define FUNC_IR_EN                        

    #define LETV_IR_REMOTER  0
    #define MI_IR_REMOTER    1
    #define NO_IR_REMOTER    0xFF

    #ifdef FUNC_IR_EN
        #define IR_LOG_EN 0
        #define IR_REMOTER_SUPPORT LETV_IR_REMOTER
    #else
        #define IR_LOG_EN 0
        #define IR_REMOTER_SUPPORT NO_IR_REMOTER
    #endif

#endif





/**
 * @} 
 */
 


/** 
 * @} 
 * @} //end module
 */ 

#endif

