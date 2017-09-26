/**
****************************************************************************
* @file     dev_info_service.c
* @brief    device information service
* @author   wenjie/yanping
* @version  v1.0.0
* @date     2016/11/14
* @history  xiaoyu modify the coding style and also added read call back function
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

#include "gatts_api.h"
#include "dev_info_service.h"



// Device Info Service Callbacks
const gatt_serviceCBs_t devInfoCBs =
{
    NULL,  //devInfo_ReadAttrCB, // Read callback function pointer
    NULL,               // Write callback function pointer
    NULL                // Authorization callback function pointer
};

/**
* @brief  Initializes the Device Information service by registering GATT attributes with the GATT server.
* @param  none
* @return Success or Failure
*/
uint32 dev_info_init_service( void )
{
    uint8 devInfoMfrName[] = {0x59, 0x4b, 0x46, 0x33, 0x34, 0x32, 0x2d, 0x32, 0x30, 0x32, 0x42};

    uint8 softWareRevision[] = {0x4d, 0x69, 0x54, 0x56, 0x2d, 0x31, 0x2e, 0x31, 0x2e, 0x32, 0x2e, 0x32};
    uint8 devInfoPnpId[DEVINFO_PNP_ID_LEN] =
    {
        1,                                      // Vendor ID source (1=Bluetooth SIG)
        LO_UINT16(0x2717), HI_UINT16(0x2717),   // Vendor ID (Texas Instruments)
        LO_UINT16(0x3200), HI_UINT16(0x3200),   // Product ID (vendor-specific)
        LO_UINT16(0x0110), HI_UINT16(0x0110)    // Product version (JJ.M.N)
    };
    uint8  manufactureid[] = {0x20, 0x21};

    uint16 hdl = 0;


    hdl = GATTS_addServiceStart(0, DEVINFO_SERV_UUID, NULL, NULL);
    if (hdl == 0)
    {
        return RTN_FAIL;
    }
    hdl = GATTS_addChar(0, SERIAL_NUMBER_UUID, NULL, ATT_PROPERTY_READ, devInfoMfrName, sizeof(devInfoMfrName));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }


    hdl = GATTS_addChar(0, SOFTWARE_REV_UUID, NULL, ATT_PROPERTY_READ, softWareRevision, sizeof(softWareRevision));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }


    hdl = GATTS_addChar(0, PNP_ID_UUID, NULL, ATT_PROPERTY_READ, devInfoPnpId, sizeof(devInfoPnpId));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    hdl = GATTS_addChar(0, MANUFACTURER_NAME_UUID, NULL, ATT_PROPERTY_READ, manufactureid, sizeof(manufactureid));
    if (hdl == 0)
    {
        return RTN_FAIL;
    }

    GATTS_addServiceEnd();
    return  0;
}






