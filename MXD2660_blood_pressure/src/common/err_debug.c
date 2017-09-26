/**
 *****************************************************************************
 * @file     err_debug.c
 * @author   Richard
 * @version  V1.0.0
 * @date     07-Aug-2017
 * @brief    print err info
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
 */
 
#include "system_config.h"
#include "mxd2660_flash.h"

#define NVR3_HARDFAULT_INFO_STORE             (0x180)
#define ERR_BUFFER_SIZE         13
void HardFault_IRQ_Handler(void)
{
    uint32 buffer[ERR_BUFFER_SIZE];

    user_flash_nvr_read(NVR3_HARDFAULT_INFO_STORE, ERR_BUFFER_SIZE, buffer);
    APP_DBG(("hard fault occurs\n"));

    APP_DBG(("R0:      0x%08x\n", buffer[0]));
    APP_DBG(("R1:      0x%08x\n", buffer[1]));
    APP_DBG(("R2:      0x%08x\n", buffer[2]));
    APP_DBG(("R3:      0x%08x\n", buffer[3]));
    APP_DBG(("R12:     0x%08x\n", buffer[4]));
    APP_DBG(("LR:      0x%08x\n", buffer[5]));
    APP_DBG(("EXC_RET: 0x%08x\n", buffer[6]));
    APP_DBG(("xPSR:    0x%08x\n", buffer[7]));
    APP_DBG(("SP_OLD:  0x%08x\n", buffer[8]));
    APP_DBG(("SP_OLD:  0x%08x\n", buffer[9]));
    APP_DBG(("LR:      0x%08x\n", buffer[10]));
    APP_DBG(("ERR_TYPE:0x%08x\n", buffer[11]));
    APP_DBG(("SP:      0x%08x\n", buffer[12]));
    
//    LOG(0xDFFD,  buffer[0]);
//    LOG(0xDFFD,  buffer[1]);
//    LOG(0xDFFD,  buffer[0]);
//    LOG(0xDFFD,  buffer[1]);
//    LOG(0xDFFD,  buffer[0]);
//    LOG(0xDFFD,  buffer[1]);
//    LOG(0xDFFD,  buffer[0]);
//    LOG(0xDFFD,  buffer[1]);
//    LOG(0xDFFD,  buffer[0]);
//    LOG(0xDFFD,  buffer[1]);
    while(1);
}

void os_stack_overflow(void)
{
    uint32 buffer[ERR_BUFFER_SIZE];

    user_flash_nvr_read(NVR3_HARDFAULT_INFO_STORE, 2, buffer);
    APP_DBG(("os_stack_overflow occurs\n"));

    APP_DBG(("adr:      0x%08x\n", buffer[0]));
    APP_DBG(("err_code: 0x%08x\n", buffer[1]));
    
//    LOG(0xDFFE,  buffer[0]);
//    LOG(0xDFFE,  buffer[1]);
    while(1);
}

void system_assert_debug(uint16 adr, uint32 info)
{
    APP_DBG(("adr:      0x%08x\n", adr));
    APP_DBG(("info: 0x%08x\n", info));
    LOG(0xDFFF, adr);
    LOG(0xDFFF, info);
    LOG(0xDFFF, info >> 16);
}

