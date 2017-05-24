enum { __FILE_NUM__ = 0 };
/*
#include "blueapi_types.h"
#include <blueapi.h>
#include <string.h>
#include "ota_api.h"
#include "dlps_platform.h"
#include "trace.h"
#include "portable.h"
#include "simple_ble_broadcaster_api.h"
*/

#include "gap.h"

bool broadcaster_StartBtStack()
{
    return GAP_StartBtStack();
}

