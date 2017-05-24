/**
************************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     test_mode.h
* @brief    test mode definition.
* @author   lory_xu
* @date     2015-07
* @version  v1.0
*************************************************************************************************************
*/

#ifndef _TEST_MODE_H_
#define _TEST_MODE_H_

#include "rtl_types.h"
#include "legacyperiphapi.h"
#include "hal_wdg.h"
#include "rtl876x_flash_storage.h"

/* ascii value of "test" */
#define TEST_MODE_FLAG_DEFAULT	0x74657374

/* General Purpose FW register */
#define AON_HCI_MODE_REG                    0x4
#define AON_HCI_MODE_REG_SWITCH_HCI_FLAG    0x1
#define AON_HCI_MODE_REG_SWITCH_HCI_BIT     BIT5

typedef union REG_AON_FW_ONREG_HCI {
    UINT8 d8;
    struct
    {
        UINT8 flags : 5;                /* bit[0:4]: reserved for ROM code */
        UINT8 hciMode : 1;              /* bit[5]: hci mode */
        UINT8 testMode : 2;             /* bit[7:6]: test mode */
    };
} REG_AON_FW_ONREG_HCI_TYPE;

typedef enum
{
    NOT_TEST_MODE       =   0,
    DIRECT_TEST_MODE    =   1,
    SINGLE_TONE_MODE    =   2,
    AUTO_PAIR_WITH_FIX_ADDR_MODE    =   3
} TEST_MODE;

extern char Load$$FLASH_TEST_MODE_FLAG$$RO$$Base[];

__STATIC_INLINE TEST_MODE GetTestMode(void)
{
    REG_AON_FW_ONREG_HCI_TYPE aon;
    aon.d8 = bee_aon_gp_read(AON_HCI_MODE_REG);
    return (TEST_MODE)(aon.testMode);
}

__STATIC_INLINE void SwitchToTestMode(TEST_MODE testMode)
{
    REG_AON_FW_ONREG_HCI_TYPE aon;
    aon.d8 = bee_aon_gp_read(AON_HCI_MODE_REG);
    aon.testMode = testMode;
    bee_aon_gp_write(AON_HCI_MODE_REG, aon.d8);

    WDG_SystemReset();
}

__STATIC_INLINE void ResetTestMode(void)
{
    REG_AON_FW_ONREG_HCI_TYPE aon;
    aon.d8 = bee_aon_gp_read(AON_HCI_MODE_REG);
    aon.testMode = NOT_TEST_MODE;
    bee_aon_gp_write(AON_HCI_MODE_REG, aon.d8);
}

__STATIC_INLINE void SwitchToHciMode(void)
{
    REG_AON_FW_ONREG_HCI_TYPE aon;
    aon.d8 = bee_aon_gp_read(AON_HCI_MODE_REG);
    aon.hciMode = AON_HCI_MODE_REG_SWITCH_HCI_FLAG;
    bee_aon_gp_write(AON_HCI_MODE_REG, aon.d8);

    WDG_SystemReset();
}

__STATIC_INLINE bool IsTestModeEnable(void)
{
    return (TEST_MODE_FLAG_DEFAULT == Flash_Read((uint32_t)Load$$FLASH_TEST_MODE_FLAG$$RO$$Base));
}

__STATIC_INLINE void TestModeDisable(void)
{
    Flash_Erase_Page((uint32_t)Load$$FLASH_TEST_MODE_FLAG$$RO$$Base);
}


#endif /* _TEST_MODE_H_ */
