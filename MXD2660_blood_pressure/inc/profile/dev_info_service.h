/**
****************************************************************************
* @file     dev_info_service.h
* @brief    device information service
* @author   wenjie/yanping
* @version  v1.0.0
* @date     2016/11/14
* @history  xiaoyu modify the coding style and also added read call back function
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

/** @defgroup  profile  device information service
* @{
*/


#ifndef DEVINFOSERVICE_H
#define DEVINFOSERVICE_H
#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Device Information Service Parameters
#define DEVINFO_SYSTEM_ID                     0
#define DEVINFO_MODEL_NUMBER              1
#define DEVINFO_SERIAL_NUMBER             2
#define DEVINFO_FIRMWARE_REV              3
#define DEVINFO_HARDWARE_REV              4
#define DEVINFO_SOFTWARE_REV              5
#define DEVINFO_MANUFACTURER_NAME         6
#define DEVINFO_11073_CERT_DATA           7
#define DEVINFO_PNP_ID                    8

// IEEE 11073 authoritative body values
#define DEVINFO_11073_BODY_EMPTY          0
#define DEVINFO_11073_BODY_IEEE           1
#define DEVINFO_11073_BODY_CONTINUA       2
#define DEVINFO_11073_BODY_EXP            254

// System ID length
#define DEVINFO_SYSTEM_ID_LEN             8

// PnP ID length
#define DEVINFO_PNP_ID_LEN                7


enum
{
    DEVINFO_SYSTEM_ID_IDX = 0,
    DEVINFO_MODEL_NUMBER_IDX ,
    DEVINFO_SERIAL_NUMBER_IDX ,
    DEVINFO_FIRMWARE_REV_IDX  ,
    DEVINFO_HARDWARE_REV_IDX ,
    DEVINFO_SOFTWARE_REV_IDX  ,
    DEVINFO_MANUFACTURER_NAME_IDX  ,
    DEVINFO_11073_CERT_DATA_IDX      ,
    DEVINFO_PNP_ID_IDX                  ,
};


extern uint32 dev_info_init_service( void );

#ifdef __cplusplus
}
#endif

#endif /* DEVINFOSERVICE_H */

/** @} */ // 模块结尾,自行拷贝到末尾




