/**
****************************************************************************
* @file     version.h
* @brief    here is very important for SDK version management
* @author
* @version  Initial Draft
* @date     2017/05/08
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/

#ifndef __VERSION_H__
#define __VERSION_H__

#define PROJECT_ID      0x2A4B      /**<each project has its own ID*/
#define BOOT_VERSION    0x0        /**<BOOT LOADER version*/
#define STACK_VERSION   0xA       /**<stack version*/
#define USER_APP_VERSION 0x0002
#define APP_VERSION     ((BOOT_VERSION << 24) | (STACK_VERSION << 16) | USER_APP_VERSION)  /**<app code version, only low uin16 can be set by user*/



#endif



