/**
 *****************************************************************************
 * @file     key_remap.c
 * @author   Richard
 * @version  V1.0.0
 * @date     09-Oct-2016
 * @brief    key function remap definition
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
 */
 
#include "system_config.h"
#include "app_ir.h"
#include "app_key_remap.h"


#if (IR_REMOTER_SUPPORT == LETV_IR_REMOTER)
	#include "letv_ir_key.h"
	uint8 gkey_ir_remap[] = 
	{
		LETV_RELEASE,
		LETV_POWER,         LETV_UP,    LETV_VOL_PLUS,     LETV_HOME,
		LETV_RIGHT,         LETV_OK,    LETV_LEFT,         LETV_SHOW, 
		LETV_RETURN,        LETV_DOWN,  LETV_VOL_MINUS,    LETV_MUTE,
		LETV_CHANNEL_MINUS, LETV_MENU,  LETV_CHANNEL_PLUS, LETV_LIST,
		LETV_REPEAT
	};

#elif (IR_REMOTER_SUPPORT == MI_IR_REMOTER)
	#include "mi_ir_key.h"
	uint8 gkey_ir_remap[] =
	{
		MI_IR_RELEASE,  MI_IR_POWER,  MI_IR_UP,       MI_IR_RIGHT,
		MI_IR_OK,       MI_IR_LEFT,   MI_IR_DOWN,     MI_IR_MENU,
		MI_IR_RETURN,   MI_IR_HOME,   MI_IR_VOL_PLUS, MI_IR_VOL_MINS,
		MI_IR_REPEAT
	};
#endif


