enum { __FILE_NUM__ = 0 };

/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     module_param_config.c
* @brief
* @details
* @author       jane
* @date     2015-03-19
* @version  v0.1
*********************************************************************************************************
*/


#include "module_param_config.h"
#include "flash_translation_layer.h"
#include "rtl876x_flash_storage.h"
#include <string.h>
#include "trace.h"
#include "rtl876x.h"

#include "core_cm0.h"
#include "dataTrans_application.h"


#define IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(type,offset_value)\
    enum { type##_SIZE   = sizeof(type)};\
    enum { type##_OFFSET = offset_value};\
    uint32_t imp_fs_save_##type(type* pdata)\
    {\
        return fs_save_vendor_data(pdata, type##_SIZE, type##_OFFSET);\
    }\
    uint32_t imp_fs_load_##type(type* pdata)\
    {\
        uint32_t has_error =  fs_load_vendor_data(pdata, type##_SIZE, type##_OFFSET);\
        if(has_error) memset(pdata, 0, sizeof(type));\
        return has_error; \
    }


IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(baudrate_struct, 4);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(adv_interval_struct, 8);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(adv_data_struct, 12);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(pin_struct, 32);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(stopbit_struct, 36);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(paribit_struct, 40);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(local_name_length_struct, 44);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(is_enter_sleep_struct, 48);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(gaprole_struct, 0);
IMPLEMENT_SAVE_LOAD_TYPE_OFFSET(fc_struct, 52);




ConfigParamStruct ConfigParam;
ConfigParamStruct *gConfigParam = &ConfigParam;



const char defaultName[] = "Realtek";
const uint16_t defaultNameLengeh = 7;
const uint32_t defaultPin = 0;
const uint32_t defaultbaudrate = 9600;
const uint16_t defaultstopbit = 1;
const uint16_t defaultparibit = 0;
const uint16_t defaultgaprole = 0;
const uint32_t default_is_enter_sleep = 1;
const uint16_t defaultfc = 0;


uint32_t fs_save_baudrate_struct(baudrate_struct *pdata)
{


    return imp_fs_save_baudrate_struct(pdata);
}


uint32_t fs_load_baudrate_struct(baudrate_struct *pdata)
{


    return imp_fs_load_baudrate_struct(pdata);
}


uint32_t fs_save_adv_interval_struct(adv_interval_struct *pdata)
{


    return imp_fs_save_adv_interval_struct(pdata);
}

uint32_t fs_load_adv_interval_struct(adv_interval_struct *pdata)
{


    return imp_fs_load_adv_interval_struct(pdata);
}

uint32_t fs_save_adv_data_struct(adv_data_struct *pdata)
{


    return imp_fs_save_adv_data_struct(pdata);
}

uint32_t fs_load_adv_data_struct(adv_data_struct *pdata)
{


    return imp_fs_load_adv_data_struct(pdata);
}


uint32_t fs_save_pin_struct(pin_struct *pdata)
{


    return imp_fs_save_pin_struct(pdata);
}


uint32_t fs_load_pin_struct(pin_struct *pdata)
{


    return imp_fs_load_pin_struct(pdata);
}



uint32_t fs_save_stopbit_struct(stopbit_struct *pdata)
{


    return imp_fs_save_stopbit_struct(pdata);
}


uint32_t fs_load_stopbit_struct(stopbit_struct *pdata)
{


    return imp_fs_load_stopbit_struct(pdata);
}


uint32_t fs_save_paribit_struct(paribit_struct *pdata)
{


    return imp_fs_save_paribit_struct(pdata);
}


uint32_t fs_load_paribit_struct(paribit_struct *pdata)
{


    return imp_fs_load_paribit_struct(pdata);
}


uint32_t fs_save_gaprole_struct(gaprole_struct *pdata)
{


    return imp_fs_save_gaprole_struct(pdata);
}


uint32_t fs_load_gaprole_struct(gaprole_struct *pdata)
{


    return imp_fs_load_gaprole_struct(pdata);
}




uint32_t fs_save_fc_struct(fc_struct *pdata)
{


    return imp_fs_save_fc_struct(pdata);
}


uint32_t fs_load_fc_struct(fc_struct *pdata)
{


    return imp_fs_load_fc_struct(pdata);
}





uint32_t fs_save_local_name_length_struct(local_name_length_struct *pdata)
{


    return imp_fs_save_local_name_length_struct(pdata);
}


uint32_t fs_load_local_name_length_struct(local_name_length_struct *pdata)
{


    return imp_fs_load_local_name_length_struct(pdata);
}




uint32_t fs_save_is_enter_sleep_struct(is_enter_sleep_struct *pdata)
{


    return imp_fs_save_is_enter_sleep_struct(pdata);
}


uint32_t fs_load_is_enter_sleep_struct(is_enter_sleep_struct *pdata)
{


    return imp_fs_load_is_enter_sleep_struct(pdata);
}



#define  ADV_LENGTH_INDEX  21
#define  ADV_REMOTE_NAME_INDEX 23
#define  ADV_DATA_FIX_DATA_LENGTH  23
#define SCAN_RSP_LENGTH_START_INDEX 0
#define SCAN_RSP_LOCAL_NAME_START_INDEX 2
#define SCAN_DATA_FIX_DATA_LENGTH 2


static uint16_t AdvDataLength;
//GAP- Advertisment data
static uint8_t AdvData[] =
{
    0x02,                       /* length */
    0x01,               /* Flags: Value = 0x01 */
    0x06,             /* type="Flags", data="bit 1: LE General Discoverable Mode"  bit2: BR/EDR Not Supported   */

    0x11,           /* length */
    0x07,           /* Service: Value = 0x02 (16-bit Service UUIDs)" */
    0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0xE0, 0x00, 0x00,      /* Service UUID */

    0x08,                                           /* length */
    0x09,                                           /* Local name: Value=0x09(Complete local name)*/
    'R', 'e', 'a', 'l', 't', 'e', 'k'   /* Default local name */
};



static uint16_t scanRspDataLength;
// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[31] =
{
    0x08,                                           /* length */
    0x09,                                           /* Local name: Value=0x09(Complete local name)*/
    'R', 'e', 'a', 'l', 't', 'e', 'k' /* Default local name */
};




void moduleParam_LoadFromFlash()
{

    if ( fs_load_Local_name_struct(&gConfigParam->localName) )
    {
        memcpy(gConfigParam->localName.Local_name,  defaultName , strlen( defaultName) + 1);
        fs_save_Local_name_struct(&gConfigParam->localName);
    }


    if ( fs_load_local_name_length_struct(&gConfigParam->localNameLength) )
    {
        gConfigParam->localNameLength.local_name_length = defaultNameLengeh;
        fs_save_local_name_length_struct(&gConfigParam->localNameLength);
    }


    if ( fs_load_baudrate_struct(&gConfigParam->baudrate) )
    {
        gConfigParam->baudrate.baudrate = defaultbaudrate;
        fs_save_baudrate_struct(&gConfigParam->baudrate);
    }


    if (  fs_load_is_enter_sleep_struct(&gConfigParam->is_enter_sleep) )
    {
        gConfigParam->is_enter_sleep.is_enter_sleep = default_is_enter_sleep;
        fs_save_is_enter_sleep_struct(&gConfigParam->is_enter_sleep);
    }

    if ( fs_load_pin_struct(&gConfigParam->pin) )
    {
        gConfigParam->pin.pin = defaultPin ;
        fs_save_pin_struct(&gConfigParam->pin);
    }

    if ( fs_load_stopbit_struct(&gConfigParam->stopbit) )
    {
        gConfigParam->stopbit.stopbit = defaultstopbit ;
        fs_save_stopbit_struct(&gConfigParam->stopbit);
    }

    if ( fs_load_paribit_struct(&gConfigParam->paribit) )
    {
        gConfigParam->paribit.paribit = defaultparibit ;
        fs_save_paribit_struct(&gConfigParam->paribit);
    }


    if ( fs_load_gaprole_struct(&gConfigParam->gaprole) )
    {
        gConfigParam->gaprole.gaprole = defaultgaprole ;
        fs_save_gaprole_struct(&gConfigParam->gaprole);
    }


    if ( fs_load_fc_struct(&gConfigParam->fc_enable) )
    {
        gConfigParam->fc_enable.fc_enable = defaultfc ;
        fs_save_fc_struct(&gConfigParam->fc_enable);
    }


    gConfigParam->conn_interval = 0;
    gConfigParam->conn_interval_update = 20;
    gConfigParam->isSleeping = FALSE;
    gConfigParam->isTimerDLPSActive = FALSE;
    gConfigParam-> isDataAvailable = FALSE;
    gConfigParam-> isChanMapSet = FALSE;
    gConfigParam->isConnectionFirstEstablished = FALSE;

    for (uint8_t i = 0; i < 5; i++)
    {
        gConfigParam->chanMap[i] = 0;
    }
}





void moduleParam_InitAdvAndScanRspData(void)
{
    /* set adv data*/
    AdvData[ADV_LENGTH_INDEX] = 1 + gConfigParam->localNameLength.local_name_length;
    memcpy(AdvData + ADV_REMOTE_NAME_INDEX, gConfigParam->localName.Local_name, gConfigParam->localNameLength.local_name_length);
    AdvDataLength = ADV_DATA_FIX_DATA_LENGTH + gConfigParam->localNameLength.local_name_length;
    peripheralSetGapParameter( GAPPRRA_ADVERT_DATA, AdvDataLength , AdvData );
    /* set scan response data */
    scanRspData[SCAN_RSP_LENGTH_START_INDEX] = 1 + gConfigParam->localNameLength.local_name_length;
    memcpy(scanRspData + SCAN_RSP_LOCAL_NAME_START_INDEX, gConfigParam->localName.Local_name, gConfigParam->localNameLength.local_name_length);
    scanRspDataLength = SCAN_DATA_FIX_DATA_LENGTH + gConfigParam->localNameLength.local_name_length;
    peripheralSetGapParameter( GAPPRRA_SCAN_RSP_DATA, scanRspDataLength, scanRspData );
}



bool moduleParam_SetConnInterval(uint16_t param)
{
    uint16_t desired_min_interval;
    uint16_t desired_max_interval;
    uint16_t desired_slave_latency;
    uint16_t desired_conn_timeout;

//#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetConnInterval: %d", 1, param);
//#endif

    if ((g_AppCB->gapProfileState != GAPSTATE_CONNECTED) && (g_AppCB->gapProfileState != GAPSTATE_CONNECTED_ADV))
    {
        return FALSE;
    }

#if 1
    if ((param == 10)  || (param == 20) || (param == 50) || (param == 100) || (param == 200) || (param == 300) || (param == 400) || (param == 500) || (param == 1000) || (param == 1500) || (param == 2000))
    {
        gConfigParam->conn_interval_update = param;
        if (param == 20)
        {
            desired_max_interval = 0x11;//0x10;
            desired_min_interval = 0x08;//0x08;
        }
        else if (param == 10)
        {
            desired_max_interval = 0x08;
            desired_min_interval = 0x08;
        }
        else
        {
            desired_max_interval = gConfigParam->conn_interval_update * 4 / 5;
            desired_min_interval = desired_max_interval - 10;
        }
        desired_slave_latency = 0;
        desired_conn_timeout = 500;

        peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
        peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
        peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
        peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );
        peripheral_SendUpdateParam();

        return TRUE;

    }
    else
    {
        return FALSE;
    }
#endif
#if 0
    desired_max_interval = param;
    desired_min_interval = param;
    desired_slave_latency = 0;
    desired_conn_timeout = 1000;




    peripheralSetGapParameter( GAPPRRA_MIN_CONN_INTERVAL, sizeof( uint16_t ), &desired_min_interval );
    peripheralSetGapParameter( GAPPRRA_MAX_CONN_INTERVAL, sizeof( uint16_t ), &desired_max_interval );
    peripheralSetGapParameter( GAPPRRA_SLAVE_LATENCY, sizeof( uint16_t ), &desired_slave_latency );
    peripheralSetGapParameter( GAPPRRA_TIMEOUT_MULTIPLIER, sizeof( uint16_t ), &desired_conn_timeout );
    peripheral_SendUpdateParam();

    return TRUE;
#endif

}



bool moduleParam_SetModuleName(char *p_adv, uint16_t len)
{
    Local_name_struct local_name;
    local_name_length_struct local_name_length;
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetModuleName: len=%d", 1, len);
#endif
    if (len > DEVICE_NAME_MAX_LENGTH)
    {
        return FALSE;
    }
    if (p_adv[len] != '\0')
    {
        p_adv[len] = '\0';
    }
    if (memcmp(gConfigParam->localName.Local_name, p_adv, len) != 0)
    {
        memcpy(gConfigParam->localName.Local_name, p_adv,len + 1);
        memcpy(local_name.Local_name, p_adv, len + 1);
        fs_save_Local_name_struct(&local_name);
        local_name_length.local_name_length = len;
        fs_save_local_name_length_struct(&local_name_length);

    }
    return TRUE;
}


bool moduleParam_SetModulePin(char *p_adv)
{
    pin_struct  newpin;
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetModulePin", 0);
#endif

    if (p_adv[6] != '\0')
    {
        p_adv[6] = '\0';
    }

    newpin.pin = Atoi( (const char *)(p_adv) ) ;


    if (gConfigParam->pin.pin != newpin.pin)
    {
        fs_save_pin_struct(&newpin);
    }
    return TRUE;
}


bool moduleParam_SetBaudrate(int param)
{
    uint32_t  baud;
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetBaudrate: %d", 1, param);
#endif
    if ( (param == 1) || (param == 2) || (param == 3) || (param == 4) || (param == 5) || (param == 6) || (param == 7) || (param == 8) || (param == 9))
    {
        switch (param)
        {
            case 1:
                baud = 1200;
                break;
            case 2:
                baud = 2400;
                break;
            case 3:
                baud = 4800;
                break;
            case 4:
                baud = 9600;
                break;
            case 5:
                baud = 19200;
                break;
            case 6:
                baud = 38400;
                break;
            case 7:
                baud = 57600;
                break;
            case 8:
                baud = 115200;
                break;
            case 9:
                baud = 230400;
                break;
            default:
                break;
        }
        if (gConfigParam->baudrate.baudrate != baud)
        {
            gConfigParam->baudrate.baudrate = baud;
            fs_save_baudrate_struct(&gConfigParam->baudrate);
            xTimerStart(TimersUartBaudrateChange, 0);
        }

    }
    else
    {
        return FALSE;
    }
    return TRUE;

}


uint8_t moduleParam_GetBaudrateIndex(void)
{
    uint8_t param = 0;

    switch (gConfigParam->baudrate.baudrate)
    {
        case 1200:
            param = 1;
            break;
        case 2400:
            param = 2;
            break;
        case 4800:
            param = 3;
            break;
        case 9600:
            param = 4;
            break;
        case 19200:
            param = 5;
            break;
        case 38400:
            param = 6;
            break;
        case 57600:
            param = 7;
            break;
        case 115200:
            param = 8;
            break;
        case 230400:
            param = 9;
            break;
        default:
            param = 4;
            break;

    }

    return param;
}


bool moduleParam_SetStopBit(uint16_t param)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetStopBit: %d", 1, param);
#endif
    if ( (param == 1) || (param == 2) )
    {

        if (gConfigParam->stopbit.stopbit != param)
        {
            gConfigParam-> stopbit.stopbit = param;
            fs_save_stopbit_struct(&gConfigParam->stopbit);
            xTimerStart(TimersUartStopBitChange, 0);
        }

    }
    else
    {
        return FALSE;
    }
    return TRUE;
}








bool moduleParam_SetGapRole(uint16_t param)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetGapRole: %d", 1, param);
#endif
    if ( (param == 0) || (param == 1) )
    {

        if (gConfigParam->gaprole.gaprole != param)
        {
            gConfigParam-> gaprole.gaprole = param;
            fs_save_gaprole_struct(&gConfigParam->gaprole);
        }

    }
    else
    {
        return FALSE;
    }
    return TRUE;
}




bool moduleParam_SetFc(uint16_t param)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetFc: %d", 1, param);
#endif
    if ( (param == 0) || (param == 1) )
    {

        if (gConfigParam->fc_enable.fc_enable != param)
        {
            gConfigParam-> fc_enable.fc_enable = param;
            fs_save_fc_struct(&gConfigParam->fc_enable);
            xTimerStart(TimersUartFcEnChange, 0);
        }

    }
    else
    {
        return FALSE;
    }
    return TRUE;
}





bool moduleParam_SetPariBit(uint16_t param)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetPariBit: %d", 1, param);
#endif
    if ( (param == 0) || (param == 1) || (param == 2))
    {

        if (gConfigParam->paribit.paribit != param)
        {
            gConfigParam-> paribit.paribit = param;
            fs_save_paribit_struct(&gConfigParam->paribit);
            xTimerStart(TimersUartPariBitChange, 0);
        }

    }
    else
    {
        return FALSE;
    }
    return TRUE;
}


bool moduleParam_SetIsEnterSleep(uint32_t param)
{
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetIsEnterSleep: %d", 1, param);
#endif
    if ( (param == 0) || (param == 1) )
    {

        if (gConfigParam->is_enter_sleep.is_enter_sleep != param)
        {
            gConfigParam->is_enter_sleep.is_enter_sleep = param;
            fs_save_is_enter_sleep_struct(&gConfigParam->is_enter_sleep);
        }

    }
    else
    {
        return FALSE;
    }
    return TRUE;
}





bool moduleParam_SetChanClass(char *p_adv)
{
    uint8_t i = 0;
#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "moduleParam_SetChanClass", 0);
#endif
    gConfigParam->isChanMapSet = TRUE;


    while (i <= 8)
    {
        if (p_adv[i] <= '9' && p_adv[i] >= '0')
        {
            gConfigParam->chanMap[i / 2] = 16 * (p_adv[i] - '0');
        }
        else if (p_adv[i] <= 'F' && p_adv[i] >= 'A')
        {
            gConfigParam->chanMap[i / 2] = 16 * (p_adv[i] - 'A' + 10);
        }
        if (p_adv[i + 1] <= '9' && p_adv[i + 1] >= '0')
        {
            gConfigParam->chanMap[i / 2] += (p_adv[i + 1] - '0');
        }
        else if (p_adv[i + 1] <= 'F' && p_adv[i + 1] >= 'A')
        {
            gConfigParam->chanMap[i / 2] += (p_adv[i + 1] - 'A' + 10);
        }

        i = i + 2;
    }
    gConfigParam->chanMap[4] = gConfigParam->chanMap[4] & 0x1F;

#ifndef CODE_SIZE_32K_DEBUG
    DBG_BUFFER(MODULE_APP, LEVEL_INFO, "gConfigParam->chanMap[0]:%02X, [1]:%02X, [2]:%02X, [3]:%02X, [4]: %02X ", 5, gConfigParam->chanMap[0], gConfigParam->chanMap[1], gConfigParam->chanMap[2], gConfigParam->chanMap[3], gConfigParam->chanMap[4]);
#endif

    return TRUE;

}






void moduleParam_SetSystemReset(void)
{
    NVIC_SystemReset();
}


