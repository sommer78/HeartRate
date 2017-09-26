/**
 *****************************************************************************
 * @file     wristband_application.h
 * @author   Richard
 * @version  V1.0.0
 * @date     21-Dec-2016
 * @brief    lis3dh
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */
 
#ifndef __WRISTBAND_APPLICATION_H__
#define __WRISTBAND_APPLICATION_H__

#define WRISTBAND_POOL_SIZE 20

/**
 * @brief  handle wristband peripheral msg
 * @param  msg msg from peripheral
 * @return none
 */
void wristband_msg_handler(APP_PERIPHERAL_MSG *msg);

/**
 * @brief  app layer send msg to ble
 * @param  command 
 * @return none
 */
void wristband_msg_to_ble_handler(uint8 command);

#endif

