/**
 *****************************************************************************
 * @file     call_back.h
 * @author   Richard
 * @version  V1.0.0
 * @date     28-June-2017
 * @brief    call back function defintion
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
 */
 
#ifndef __CALL_BACK_H__
#define __CALL_BACK_H__

typedef void (*CALL_BACK_FUNC)(void);
typedef uint32 (*decrypt_callback_t)(uint32 addr, uint32 code, uint32 key);
typedef bool (*CALL_BACK_CHECK_FUNC)(void);
typedef void (*SYSTEM_ASSERT_FUNC_CB)(uint16 addr, uint32 info);

extern SYSTEM_ASSERT_FUNC_CB system_assert_cb[2];
extern CALL_BACK_CHECK_FUNC call_back_check_table[4];
extern decrypt_callback_t fp_decrypt_func;
extern CALL_BACK_FUNC call_back_func_table[2];
extern uint32 g_image_key;

extern void main_idle_process(void);
extern bool hci_sleep_check(void);
extern bool before_sleep_check(void);
extern bool after_wkup_check(void);
extern bool log_sleep_check(void);
extern void log_output_cb(uint16 address, uint16 data);
extern void os_error_handler(uint32 adr, uint32 info);

#endif


