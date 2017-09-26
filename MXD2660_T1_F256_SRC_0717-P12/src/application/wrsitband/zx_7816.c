
#include "system_config.h"
#include "log_descriptor.h"
#include "zx_7816.h"
#include "mxd2660_7816.h"
#include "mem.h"
#include "app_message.h"
uint8 zx_7816_reset_data[64] = {0};


#define MXD7816_FIFO_SIZE  (128)


void zx_7816_init(void)
{
	_7816_init_default();
}

uint32 wristband_7816_cmd_proc(void *buffer)
{
	uint8 *ptr = ((APP_PERIPHERAL_MSG *)buffer)->p;
	switch(ptr[0])
	{
		case _7816_RESET_COLD:
		{
			_7816_reset(_7816_RESET_COLD);
			break;
		}
		case _7816_RESET_WARM:
		{
			_7816_reset(_7816_RESET_WARM);
			break;
		}
		default:
		{
			
		}
	}
	return 0;
}
