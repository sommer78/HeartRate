/**
****************************************************************************
* @file     system_config.h
* @brief    all system related configuration declaration here
* @author
* @version  Initial Draft
* @date     2016/12/14
* @history  xiaoyu added through MXD c comment style
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/


/** @defgroup  system_config all system related configuration declaration here
* @{
*/



#ifndef _SYSTEM_CONFIG_H_
#define _SYSTEM_CONFIG_H_

#include <stdbool.h>      // boolean definition
#include <stdio.h>
#include "mxdaqua.h"
#include "core_cm0plus.h"
#include "cmsis_os.h"
#include "device_cfg.h"
#include "log_descriptor.h"

/** @name TYPE DEFINITION
* @{
*/

/**< exact-width signed integer types */
typedef   signed          char int8;
typedef   signed short     int int16;
typedef   signed           int int32;
typedef   signed       __int64 int64;

/**< exact-width unsigned integer types */
typedef unsigned          char uint8;
typedef unsigned short     int uint16;
typedef unsigned           int uint32;
typedef unsigned       __int64 uint64;

#define _ABS(a) ( (a)<0?((a)*(-1)):(a))

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif


/** @} */


/**<the header structure of user image*/

typedef struct
{
    uint16 project_id;          /**< each project has its unique id */
    uint8 root_version;	        /**<secondary boot update*/
	uint8 stack_version;        /**<stack update*/	
    uint32 app_version;         /**<app update*/
    uint32 code_size;           /**< updated code size*/
	uint32 crc32;               /**<crc32*/
	
	
}UPDATE_HEADER;




/** @name HCI COMMAND DEFINITION
* @{
*/

/******************************************
HCI_Version
0   Bluetooth Core Specification 1.0b
1   Bluetooth Core Specification 1.1
2   Bluetooth Core Specification 1.2
3   Bluetooth Core Specification 2.0 + EDR
4   Bluetooth Core Specification 2.1 + EDR
5   Bluetooth Core Specification 3.0 + HS
6   Bluetooth Core Specification 4.0
7   Bluetooth Core Specification 4.1
8   Bluetooth Core Specification 4.2
9 - 255 Reserved
********************************************/
#define MXD_VER 8           
#define MXD_COMPID 0x03BA
#define MXD_SUBVER  0x2203

/** @} */



extern void * memcpy(void *, const void *, uint32 length);
extern void * memset(void *, int32 value, uint32 length);
extern uint8 memcmp2(const uint8 *buf1, const uint8 *buf2, uint16 count);

/** @} */


/** @name DEBUG DEFINITION
* @{
*/

#define _TRUNK_DEBUG__
#ifdef _TRUNK_DEBUG__
    #define LOG(address, data) log_output_cb(address, data)
#else
    #define LOG(address, data)
#endif

#ifdef _TRUNK_DEBUG__
typedef struct
{
    uint16  wr;
    uint16  rd;
} t_log_ctrl;

extern t_log_ctrl  g_log_ctrl;
extern uint8  g_log_pattern;

extern void uart1_Log_output(uint16 address, uint16 data);
extern void uart1_Log_print(void);
extern bool is_log_canbe_sleep(void);
extern void sys_debug_init(void);
extern void sys_debug_en(uint8 debug_en);
extern void log_output_cb(uint16 address, uint16 data);

#endif


/** @} */


#endif

/** @} */ // ?¡ê?¨¦?¨¢?2,¡Á?DD??¡À¡ä¦Ì????2



