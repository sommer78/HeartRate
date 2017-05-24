/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     module_param_config.h
* @brief
* @details
* @author       jane
* @date     2015-03-19
* @version  v0.1
*********************************************************************************************************
*/

#ifndef _MUDULE_PARAM_CONFIG_H_
#define  _MUDULE_PARAM_CONFIG_H_

#include "rtl876x_flash_storage.h"
#include "rtl_types.h"
#include "freeRTOS.h"
#include "timers.h"


#define DEVICE_NAME_MAX_LENGTH 16
#define PIN_LENGTH 6

typedef struct
{
    uint32_t  baudrate;
} baudrate_struct;


typedef struct
{
    uint16_t  stopbit;
    uint8_t   padding[2];
} stopbit_struct;

typedef struct
{
    uint16_t  paribit;
    uint8_t   padding[2];
} paribit_struct;



typedef enum
{
    PERIPHERAL_ROLE,
    CENTRAL_ROLE
} GapRoleType;


typedef struct
{
    uint16_t  gaprole;
    uint8_t   padding[2];
} gaprole_struct;



typedef struct
{
    uint16_t  fc_enable;
    uint8_t   padding[2];
} fc_struct;



typedef struct
{
    uint32_t pin;
} pin_struct;


typedef struct
{
    uint32_t is_enter_sleep;
} is_enter_sleep_struct;


typedef struct
{
    uint16_t  local_name_length;
    uint8_t   padding[2];
} local_name_length_struct;


typedef struct
{
    uint16_t  adv_interval;
    uint8_t   padding[2];
} adv_interval_struct;

typedef struct
{
    uint8_t data_length;
    uint8_t padding[3];
    uint8_t adv_data[16];
} adv_data_struct;




typedef struct
{
    Local_name_struct localName;
    local_name_length_struct localNameLength;
    bool is_conn_update;
    uint16_t conn_interval;
    uint16_t conn_interval_update;
    baudrate_struct baudrate;
    stopbit_struct stopbit;
    paribit_struct paribit;
    pin_struct pin;
    is_enter_sleep_struct is_enter_sleep;
    adv_interval_struct advInterval;
    adv_data_struct advData;
    gaprole_struct  gaprole;
    fc_struct fc_enable;
    bool  isSleeping;
    bool  isTimerDLPSActive;
    bool  isDataAvailable;
    bool  isChanMapSet;
    bool  isConnectionFirstEstablished;
    uint8_t chanMap[5];
} ConfigParamStruct;



extern ConfigParamStruct *gConfigParam;


extern const char defaultName[];
extern const uint16_t defaultNameLengeh;
extern const uint32_t defaultPin;
extern const uint32_t defaultbaudrate;
extern const uint16_t defaultstopbit;
extern const uint16_t defaultparibit;
extern const uint32_t default_is_enter_sleep;
extern const uint16_t defaultgaprole;
extern const uint16_t defaultfc;


uint32_t fs_save_baudrate_struct(baudrate_struct *pdata);
uint32_t fs_load_baudrate_struct(baudrate_struct *pdata);
uint32_t fs_save_adv_interval_struct(adv_interval_struct *pdata);
uint32_t fs_load_adv_interval_struct(adv_interval_struct *pdata);
uint32_t fs_save_adv_data_struct(adv_data_struct *pdata);
uint32_t fs_load_adv_data_struct(adv_data_struct *pdata);
uint32_t fs_save_pin_struct(pin_struct *pdata);
uint32_t fs_load_pin_struct(pin_struct *pdata);
uint32_t fs_save_stopbit_struct(stopbit_struct *pdata);
uint32_t fs_load_stopbit_struct(stopbit_struct *pdata);
uint32_t fs_save_paribit_struct(paribit_struct *pdata);
uint32_t fs_load_paribit_struct(paribit_struct *pdata);
uint32_t fs_save_is_enter_sleep_struct(is_enter_sleep_struct *pdata);
uint32_t fs_load_is_enter_sleep_struct(is_enter_sleep_struct *pdata);
uint32_t fs_save_gaprole_struct(gaprole_struct *pdata);
uint32_t fs_load_gaprole_struct(gaprole_struct *pdata);
uint32_t fs_save_fc_struct(fc_struct *pdata);
uint32_t fs_load_fc_struct(fc_struct *pdata);





void moduleParam_LoadFromFlash(void);
void moduleParam_InitAdvAndScanRspData(void);
bool moduleParam_SetConnInterval(uint16_t param);
bool moduleParam_SetModuleName(char *p_adv, uint16_t len);
bool moduleParam_SetModulePin(char *p_adv);
bool moduleParam_SetBaudrate(int param);
uint8_t moduleParam_GetBaudrateIndex(void);
bool moduleParam_SetStopBit(uint16_t param);
bool moduleParam_SetGapRole(uint16_t param);
bool moduleParam_SetFc(uint16_t param);
bool moduleParam_SetPariBit(uint16_t param);
bool moduleParam_SetIsEnterSleep(uint32_t param);
bool moduleParam_SetChanClass(char *p_adv);
void moduleParam_SetSystemReset(void);


#endif
