/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      keyscan.h
* @brief
* @details
* @author    Chuanguo Xue
* @date      2015-3-27
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _KEYSCAN_H
#define _KEYSCAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x.h"
#include "rtl876x_bitfields.h"

typedef struct _KEYSCAN_DATA_STRUCT_
{
    uint32_t Length;            /**< Keyscan state register        */
    struct
    {
        uint8_t column: 5;      /**< Keyscan raw buffer data       */
        uint8_t row: 3;      /**< Keyscan raw buffer data       */
    } key[26];
} KEYSCAN_DATA_STRUCT, *PKEYSCAN_DATA_STRUCT;


/**
 * @brief Keyscan event definition
 *
 * Keyscan event definition
 */
typedef enum _MSG_TYPE_KEYSCAN
{
    MSG_KEYSCAN_RX_PKT        = 1, /**< Keyscan RX data event */ // request to send a SDIO RX packet to the host side
    MSG_KEYSCAN_MAX           = 2, /**<  */
    MSG_KEYSCAN_ALLKEYRELEASE = 3, /**< All keys are released event */
} MSG_TYPE_KEYSCAN;

extern uint8_t RepeatReport;

extern void KeyPad_Init(void);//(KEYSCAN_CONFIG_STRUCT * Keyscan_Param);

#ifdef __cplusplus
}
#endif

#endif /* _KEYSCAN_H */

