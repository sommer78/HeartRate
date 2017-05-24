/****************************************************************************************************//**
 * @file     debug_platform.h
 *
 * @brief    Provide some debug APIs.
 *
 * @version  v0.1
 * @date     2016-05-20
 *
 * @note     
 *******************************************************************************************************/
#ifndef _DEBUG_PLATFORM_H_
#define _DEBUG_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

void DebugTaskHang(void);
	
void save_log_to_flash(void);
void stop_save_log_to_flash(void);
void erase_ota_flash(void);
	
#ifdef __cplusplus
}
#endif

#endif /*_APPLICATION_H_*/

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/
