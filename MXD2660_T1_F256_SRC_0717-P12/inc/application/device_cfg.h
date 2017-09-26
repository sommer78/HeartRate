/**
****************************************************************************
* @file     device_cfg.h
* @brief    function selection in different application 
* @author
* @version  Initial Draft
* @date     2017/05/4
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/

#ifndef __DEVICE_CFG_H__
#define __DEVICE_CFG_H__


/**<device name definition*/
#define MXD2660A_F256   //QFN 40 PINS
#define MXD2660B_F256   //QFN 48 PINS
#define MXD2660C_F256   //LGA 49 PINS

#define MXD2660A_F128
#define MXD2660B_F128
#define MXD2660C_F128

#define MXD2660A_F64
#define MXD2660B_F64
#define MXD2660C_F64


/**<system function related definition*/
//#define RELEASE_DEFAULT_VERSION 
#define  RELEASE_RAM_20KB_VERSION 		/**<release 20KB RAM version*/

#ifdef RELEASE_DEFAULT_VERSION
    #ifdef FUNC_EVALUATION_EN
        #define MULTI_LINK_FUNC_EN
        #define ANCS_FUNC_EN
        #define HCI_FUNC_EN
        #define LOG_FUNC_EN
    #endif
    
    #ifdef FUNC_WRISTBAND_EN
        #define MULTI_LINK_FUNC_EN
        #define ANCS_FUNC_EN
        #define LOG_FUNC_EN
    #endif
    
    #ifdef FUNC_KEYBOARD_EN
        #define MULTI_LINK_FUNC_EN
        #define LOG_FUNC_EN
    #endif
#endif

#ifdef RELEASE_RAM_20KB_VERSION
    #ifdef FUNC_EVALUATION_EN
        #define ANCS_FUNC_EN
//        #define LOG_FUNC_EN
    #endif
    
    #ifdef FUNC_WRISTBAND_EN
        #define ANCS_FUNC_EN
//        #define LOG_FUNC_EN
    #endif
    
    #ifdef FUNC_KEYBOARD_EN
//        #define LOG_FUNC_EN
    #endif
    
#endif


#ifdef  LOG_FUNC_EN
    #define APP_DBG(x)      printf x
#else
    #define APP_DBG(x)
#endif  //DEBUG


#endif

