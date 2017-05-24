/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      pxp_ext.h
* @brief
* @details
* @author    Chuanguo Xue
* @date      2015-3-27
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef _PXP_EXT_H
#define _PXP_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"

/****************************************************************************************************************
* macros that other .c files may use all defined here
****************************************************************************************************************/
/** @brief  GATT UUID definitions. */
#define GATT_UUID_EXS  0xFFD0
#define GATT_UUID_CHAR_PARAM 0xFFD1
#define GATT_UUID_CHAR_KEY  0xFFD2
#define GATT_UUID128_EXS    0xA6, 0xF6, 0xF6, 0x07, 0x4D, 0xC4, 0x9D, 0x98, 0x6D, 0x45, 0x29, 0xBB, 0xD0, 0xFF, 0x00, 0x00,
#define GATT_UUID128_CHAR_PARAM   0xA6, 0xF6, 0xF6, 0x07, 0x4D, 0xC4, 0x9D, 0x98, 0x6D, 0x45, 0x29, 0xBB, 0xD1, 0xFF, 0x00, 0x00,
#define GATT_UUID128_CHAR_KEY     0xA6, 0xF6, 0xF6, 0x07, 0x4D, 0xC4, 0x9D, 0x98, 0x6D, 0x45, 0x29, 0xBB, 0xD2, 0xFF, 0x00, 0x00,

/** @brief  PXP Extended characteristics index definitions. */
#define SIMPLE_PARAM_VALUE_INDEX  2
#define SIMPLE_SKEY_VALUE_INDEX  4

//#define blueAPI_ServiceExs     20

/** @brief  Max number of parameters PXP Extended service can inform application. */
#define PXP_EXTENDED_PARAMETERS_MAX     1

/**
 * @brief PXP extended data struct for notification data to application.
 *
 * PXP extended service data to inform application.
*/
typedef struct _TPXP_EXTENDED_PARA_CBs_t
{
    uint8_t     eventId;                    /**<  @brief EventId defined upper */
    uint16_t    sParas[PXP_EXTENDED_PARAMETERS_MAX];
} TPXP_EXTENDED_PARA_CBs_t;

/* Add PXP Extended service to your application. */
uint8_t PXPExtended_AddService(void *pFunc);

#ifdef __cplusplus
}
#endif

#endif /* __HIDS_H */
