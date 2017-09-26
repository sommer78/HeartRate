#ifndef _LL_SECURITY_
#define _LL_SECURITY_
#include "stack_ctrl.h"

// connection info available as long as connection exists
extern void ll_start_encryption_set_sk(uint8 idx);

extern uint8 get_mic_status(void);
#endif

