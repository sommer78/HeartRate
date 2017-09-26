#include "gatts_api.h"
#include "scan_para_service.h"

uint32 scan_para_init_service( void )
{
    uint16 hdl = 0;
    hdl = GATTS_addServiceStart(0, SCAN_PARAM_SERV_UUID, NULL, NULL);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hdl = GATTS_addChar(0, SCAN_INTERVAL_WINDOW_UUID, NULL, ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, SCAN_REFRESH_UUID, NULL, ATT_PROPERTY_NOTIFY, NULL, 0);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    
    hdl = GATTS_addCharDescrip_client_config();
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    GATTS_addServiceEnd();
    return  0;
}


/*********************************************************************
*********************************************************************/

