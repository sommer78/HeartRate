/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    wss_config.h
  * @brief   weight scale service configuration file.
  * @details This file includes common constants or types for Weight Scale service. 
  *          And some optional feature may be defined in this file.
  * @author  Vera
  * @date    20-October-2015
  * @version v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion **/
#ifndef _WSS_CONFIG_H_
#define _WSS_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup WSS
  * @{
  */
	 
/** @defgroup WSS_UUIDs WSS UUIDs
  * @brief Weight Scale Service UUID definitions
  * @{
  */
/* Weight scale service UUID */
#define GATT_UUID_WEIGHT_SCALE_SERVICE              0x181D
/* Weight scale feature characteristic UUID*/
#define GATT_UUID_CHAR_WEIGHT_SCALE_FEATURE_READ    0x2A9E
/* Weight measurement characteristic UUID*/
#define GATT_UUID_CHAR_WEIGHT_MEASUREMENT_INDICATE  0x2A9D

/** @} End of WSS_UUIDs */

/** @defgroup WSS_Feature_Supported WSS Feature Supported
  * @brief Weight Scale Service Supported Feature
  * @{
  */
#define WSS_FEATURE_TIMESTAMP_SUPPORTED            1
#define WSS_FEATURE_MULTIUSERS_SUPPORTED           1
#define WSS_FEATURE_BMI_SUPPORTED                  1
#define WSS_FEATURE_WEIGHTMEASUREMENT_RESOLUTION   1
#define WSS_FEATURE_HEIGHTMEASUREMENT_RESOLUTION   1
/** @} End of WSS_Feature_Supported */

/** @} End of WSS */

#ifdef __cplusplus
}
#endif

#endif
