#ifndef __APP_IR_H_
#define __APP_IR_H_
#include "mxd2660_timer.h"
#include "system_config.h"
#include "log_descriptor.h"

#define LETV_IR_REMOTER  0

#if (IR_REMOTER_SUPPORT==LETV_IR_REMOTER)
	#include "letv_ir_key.h"
//#elseif

#endif
void app_ir_msg_proc(uint8 ir_key);
void app_ir_init(void);

#if IR_LOG_EN
	#define ir_log(address, data) uart0_Log_output(UART1, address, data)
#else
	#define ir_log(address, data) 
#endif

#if FUN_IR_EN

#endif






#endif
