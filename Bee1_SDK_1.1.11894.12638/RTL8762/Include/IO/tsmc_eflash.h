// RL6402-EFlash Controller
// Digital Designer: CC Lin
// SD: Raven Su
// Date: 2014
// Version: 0.0.2
//   support NEW_VERSION_EFLASH_CONTROLLER_140915
// Version: 0.0.1

#ifndef tsmc_eflash_h
#define tsmc_eflash_h

#include "platform_autoconf.h"
#include <stdint.h>

/////////////////////////////////////////////////////////////////
//
// [Config]
//

#define ENABLE_EFLASH_FAKE_IN_FPGA_EXTRA_RAM 1

/////////////////////////////////////////////////////////////////

#define NEW_VERSION_EFLASH_CONTROLLER_140915_EXT_APPEND_1020_SPEC 1
#define NEW_VERSION_EFLASH_CONTROLLER_140915_EXT_TIMING_PARAMETER 1
#define NEW_VERSION_EFLASH_CONTROLLER_140915 1
#define HW_EFLASH_POWER_SEQUENCE_141008 1

/////////////////////////////////////////////////////////////////

#ifdef CONFIG_EFLASH_BOARD_EXIST
#undef EFLASH_NO_PREFETCH
#undef EFLASH_FAKE_IN_FPGA_EXTRA_RAM
#else
#define EFLASH_NO_PREFETCH 1
#define EFLASH_FAKE_IN_FPGA_EXTRA_RAM 1
#endif

#ifndef ENABLE_EFLASH_FAKE_IN_FPGA_EXTRA_RAM
#undef EFLASH_FAKE_IN_FPGA_EXTRA_RAM
#endif

/////////////////////////////////////////////////////////////////

#define eflash_dbg_rw_log (0x1)
#define eflash_dbg_ftl_log (0x2)
#define eflash_dbg_ota_log (0x4)
extern uint32_t g_eflash_dbg;

/////////////////////////////////////////////////////////////////

#if NEW_VERSION_EFLASH_CONTROLLER_140915_EXT_TIMING_PARAMETER


#if EFLASH_NO_PREFETCH

#define FMC_MAIN       (0x00400000)  // main blocks, 128 pages
#define FMC_INFO       (0x00480000)  // information blocks, 1 pages
#define FMC_TRIM       (0x00490000)
#define FMC_REDU       (0x004A0000)  // redundant

#else // CONFIG_FPGA

// for prefetch mode
#define FMC_MAIN       (0x00500000)  // main blocks, 128 pages
#define FMC_INFO       (0x00580000)  // information blocks, 1 pages
#define FMC_TRIM       (0x00590000)
#define FMC_REDU       (0x005A0000)  // redundant

#endif// !CONFIG_FPGA

#define FMC_REG        (0x004F0000)  // FMC register base address 

#else // NEW_VERSION_EFLASH_CONTROLLER_140915_EXT_TIMING_PARAMETER

#define FMC_MAIN       (0x30000000)
#define FMC_INFO       (0x30080000)
#define FMC_TRIM       (0x30090000)
#define FMC_REDU       (0x300A0000)  // redundant
#define FMC_REG        (0x300F0000)  // FMC register base address 

#endif

#if EFLASH_FAKE_IN_FPGA_EXTRA_RAM
#undef FMC_MAIN
#undef FMC_INFO
#undef FMC_TRIM
#undef FMC_REDU
#define FMC_MAIN       (0x20010000)
#define FMC_INFO       (0x20010000+256*1024)
#define FMC_REDU       (0x20010000+256*1024+2*1024)
#define FMC_TRIM       (0x20010000+256*1024+2*1024+2*1024)
#endif

#define FMC_MAIN_SIZE  (0x40000)
#define FMC_PAGE_SIZE  (0x800)
#define FMC_INFO_SIZE  (0x800)

#define FMC_ERASE_PATTERN  (0xffffffff)

/////////////////////////////////////////////////////////////////

#define  FMC_SUCCESS 0x0

uint8_t  FMC_isBusy(void);

/////////////////////////////////////////////////////////////////

// polling mode (sync mode)
uint32_t FMC_Read(uint32_t u32Addr);
uint32_t FMC_Write(uint32_t u32Addr, uint32_t u32Data);
//uint32_t FMC_Write_Burst(uint32_t u32Addr, uint32_t u32Addr_src, uint32_t u32len_src); // raven.todo.need to verify
uint32_t FMC_Erase_Page(uint32_t u32PageAddr); // // Flash page address. Must be a 2048-byte aligned address.
uint32_t FMC_Erase_Mass(void);

#if ERASE_FLASH_CB
typedef void (*EraseFlashEnterCB)(void);
typedef void (*EraseFlashExitCB)(void);
	
__STATIC_INLINE void RegEraseFlashEnterCB(EraseFlashEnterCB fun)
{
	*((UINT32*)(0x20005F00)) = (UINT32)fun;
}

__STATIC_INLINE void RegEraseFlashExitCB(EraseFlashExitCB fun)
{
	*((UINT32*)(0x20005F04)) = (UINT32)fun;
}
#endif

/////////////////////////////////////////

// interrupt mode (async mode)
#define FMC_Interrupt_None         (0)
#define FMC_Interrupt_Timeout     (0x1U<<31)   // raven.todo.need to verify

#if NEW_VERSION_EFLASH_CONTROLLER_140915
#define FMC_Interrupt_EraseAll_Fail   (0x1<<11)
#define FMC_Interrupt_Erase_Fail      (0x1<<10)
#define FMC_Interrupt_WriteBurst_Fail (0x1<<9)
#define FMC_Interrupt_Write_Fail      (0x1<<8)
#else
#define FMC_Interrupt_EraseAll_Fail   (0x0)
#define FMC_Interrupt_Erase_Fail      (0x0)
#define FMC_Interrupt_WriteBurst_Fail (0x0)
#define FMC_Interrupt_Write_Fail      (0x0)
#endif

#define FMC_Interrupt_EraseAll     (0x1<<3)
#define FMC_Interrupt_Erase        (0x1<<2)
#define FMC_Interrupt_WriteBurst   (0x1<<1)
#define FMC_Interrupt_Write        (0x1<<0)

void FMC_interrupt_handler(void* data);

typedef void (*eFlash_event_handler_t) (uint32_t interrupt_state, uint32_t data);
void FMC_Write_Async(eFlash_event_handler_t handler, uint32_t data, uint32_t u32Addr, uint32_t w_data);
//void FMC_Write_Burst_Async(eFlash_event_handler_t handler,uint32_t data, uint32_t u32Addr, uint32_t u32Addr_src, uint32_t u32len_src); // raven.todo.need to verify
void FMC_Erase_Page_Async(eFlash_event_handler_t handler, uint32_t data, uint32_t u32PageAddr);
void FMC_Erase_Mass_Async(eFlash_event_handler_t handler, uint32_t data);

/////////////////////////////////////////////////////////////////

#if NEW_VERSION_EFLASH_CONTROLLER_140915
enum
{
    flash_ioctl_get_info = 0x0,
    flash_ioctl_get_page_size = 0x1,

    flash_ioctl_get_start_addr_main = 0x2,
    flash_ioctl_get_size_main = 0x3,

    flash_ioctl_get_start_addr_info = 0x4,
    flash_ioctl_get_size_info = 0x5,

    flash_ioctl_backward_compatible = 0x10,
    flash_ioctl_mass_erase = 0x20,
    flash_ioctl_debug_log = 0x30,

    flash_ioctl_read_reg = 0x40,
    flash_ioctl_write_reg = 0x41,
};
uint32_t FMC_ioctl(uint32_t ioctl, uint32_t p1, uint32_t p2);
#endif // NEW_VERSION_EFLASH_CONTROLLER_140915

/////////////////////////////////////////////////////////////////

#endif // tsmc_eflash_h
