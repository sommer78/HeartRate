/**
************************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     rtl876x_lib_ota.h
* @brief    Versions are stored in the OTA registers which will not be updated automatically, 
*           so they should be updated after firmware update successfully.
* @details  
* @author   
* @date     2016-04-22
* @version  
*************************************************************************************************************
*/

#ifndef __RTL876X_LIB_OTA_H
#define __RTL876X_LIB_OTA_H

#include "rtl_types.h"

/**
 * @brief  read Patch version from OTA Registers
 * @param  void
 * @return  patch version
*/
uint16_t otaGetPatchVersion(void);

/**
 * @brief  write Patch version into OTA Registers
 * @param  patch version
 * @return  void
*/
void otaSetPatchVersion(uint16_t ver);

/**
 * @brief  read App version from OTA Registers
 * @param  void
 * @return  App version
*/
uint16_t otaGetAppVersion(void);

/**
 * @brief  write App version into OTA Registers
 * @param  App version
 * @return  void
*/
void otaSetAppVersion(uint16_t ver);

/* 
 * Example usage:
 * Before App firmware update, write the old version into OTA version Register;
 * After App firmware update, read the old version from OTA version Register, and compare it with the current version:
 * if the versions are not the same, firmware update is success, or fail.
 * 
    bool IsPatchUpdateSuccess(void)
    {
        //Make sure otaSetAppVersion() is called before otaGetAppVersion().
        uint16_t app_old_version = otaGetAppVersion();
        uint16_t app_curr_version = VERSION_BUILD;

        if(app_curr_version == app_old_version)
        {
            return false;
        }
        else 
        {
            otaSetAppVersion(app_curr_version);
            return true;
        }
    }
 *
 */
 
 #endif /*__RTL876X_LIB_OTA_H*/