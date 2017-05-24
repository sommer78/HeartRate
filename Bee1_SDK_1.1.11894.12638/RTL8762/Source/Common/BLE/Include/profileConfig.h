/**
********************************************************************************************************
Copyright (c) 2014, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file       profileConfig.h
* @brief     This is a brief description.
* @details   Here typically goes a more extensive explanation of
*                what this file defines.
* @author   lilly_he
* @date      2014-06-26
* @version  v1.0
*/
#ifndef __PROFILE_CONFIG_H
#define __PROFILE_CONFIG_H
    
#ifdef __cplusplus
    extern "C"  {
#endif      /* __cplusplus */

/****************************************************************************************************************
* macros that other .c files may use all defined here
****************************************************************************************************************/
/** @brief  Number of services used in application. You must modify this value according the services number used in your application. */
#define PROFILE_SUPPORT_SERVICE_NUM         6
#ifndef CODE_SIZE_32K_DEBUG
#define PROFILE_DEBUG
#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined (__PROFILE_CONFIG_H) */
