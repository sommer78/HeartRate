#include "rtl876x_flash_storage.h"

#define flash_ioctl_app_range_check_read_erase 0x7
#define flash_ioctl_app_range_check_write      0x8
#define flash_ioctl_get_unique_uuid_for_app    0xA
#define flash_ioctl_ctl_hw_timer_0             0xB
#define flash_ioctl_rpt_bt_link_info           0xC
#define flash_ioctl_get_vendor_ID_AB_key       0xD

uint32_t Save_To_Storage(void* pdata, uint16_t offset, uint16_t size);
uint32_t Load_From_Storage(void* pdata, uint16_t offset, uint16_t size);
uint32_t FMC_ioctl(uint32_t cmd, uint32_t p1, uint32_t p2);

/**
  * @brief  save vendor_data into storage
  * @details Save vendor data into the 2048 bytes logic Flash area
  * @param  pdata: Vendor Data 
  * @param  data_size: Vendor Data Size(4 bytes align)
  *     @arg Min: 4
  *     @arg Max: 2048
  * @param  addr_offset: The offset of in the logic area(4 bytes align)
  *	    @arg Min: 0
  *     @arg Max: 2044
  * @retval save result
  *     @arg 0: save success
  *     @arg 1: data_size our of range
  *     @arg 2: data_size or addr_offset is not 4 bytes align
  */
uint32_t fs_save_vendor_data(void* pdata, uint16_t data_size, uint16_t addr_offset)
{
    if( (data_size&0x3) || (addr_offset&0x3))
    {
        return 2; // not 4byte align
    }

    if ( 4<= (addr_offset+data_size) && (addr_offset+data_size) <=2048 )
    {
        return Save_To_Storage(pdata, addr_offset+1024, data_size);
    }
    
    return 1; // out of range
}

/**
  * @brief  load vendor_data into storage
  * @details Load vendor data from the 2048 bytes logic Flash area
  * @param  pdata: the buffer for storing Vendor data
  * @param  data_size: Vendor data size(4 bytes align)
  *     @arg Min: 4
  *     @arg Max: 2048
  * @param  addr_offset: The first byte of Vendor data(4 bytes align)
  *	    @arg Min: 0
  *     @arg Max: 2044
  * @retval load result
  *     @arg 0: load success
  *     @arg 1: @ref data_size out of range
  *     @arg 2: @ref data_size or @ref addr_offset is not 4 bytes align
  */
uint32_t fs_load_vendor_data(void* pdata, uint16_t data_size, uint16_t addr_offset)
{
    if( (data_size&0x3) || (addr_offset&0x3))
    {
        return 2;
    }

    if ( 4<= (addr_offset+data_size) && (addr_offset+data_size) <=2048 )
    {
        return Load_From_Storage(pdata, addr_offset+1024, data_size);
    }
    
    return 1; // out of range
}
 
/**
  * @brief  flash read 
  * @param  address must 4byte alignment, the valid range is from 0x50A000 to 0x50A000+100K-4, beware the space is shared with app image space, pls use it carefully ... 
  * @retval 0 means success, or invalid address will return 0xdeadbeef
  */ 
uint32_t Flash_Read(uint32_t u32Addr)
{
    return FMC_ioctl(flash_ioctl_app_range_check_read_erase, u32Addr, 0);
}

/**
  * @brief  flash write 
  * @param  address must 4byte alignment, the valid range is from 0x50A000 to 0x50A000+100K-4, beware the space is shared with app image space, pls use it carefully ... 
  * @param  data
  * @retval 0 means success, or invalid address will return 0xdeadbeef
  */  
uint32_t Flash_Write(uint32_t u32Addr, uint32_t u32Data)
{
    return FMC_ioctl(flash_ioctl_app_range_check_write, u32Addr, u32Data);
}
 
/**
  * @brief  flash erase page  
  * @param  address must 2kbyte alignment, the valid range is from 0x50A000 to 0x50A000+100K-2k, beware the space is shared with app image space, pls use it carefully ... 
  * @retval 0 means success, or invalid address will return 0xdeadbeef
  */  
uint32_t Flash_Erase_Page(uint32_t u32PageAddr)
{
    return FMC_ioctl(flash_ioctl_app_range_check_read_erase, u32PageAddr, 1);    
}

/**
  * @brief  get IC UUID  
  * @param  UUID[]: the buffer for storing UUID
  * @param  length: UUID size(max size is 13)
  * @retval 0 means success, other value means fail: 13 means UUID buffer is NULL, other value means length is larger then 13. 
  */
uint32_t Get_IC_UUID(uint8_t UUID[], uint32_t length)
{
    return FMC_ioctl(flash_ioctl_get_unique_uuid_for_app, (uint32_t)UUID, length);
}

/**
  * @brief  enable timer0(timer0 is used for DLPS timer tick, don't use this function if DLPS shoule be supported)
  * @retval 0 means success, other value means fail. 
  */
uint32_t Enable_Timer0()
{
    uint8_t on[1] = {1};
    return FMC_ioctl(flash_ioctl_ctl_hw_timer_0, 1, (uint32_t)on);
}

/**
  * @brief  disable timer0(timer0 is used for DLPS timer tick, don't use this function if DLPS shoule be supported)
  * @retval 0 means success, other value means fail. 
  */
uint32_t Disable_Timer0()
{
    uint8_t off[1] = {0};
    return FMC_ioctl(flash_ioctl_ctl_hw_timer_0, 1, (uint32_t)off);
}

/**
  * @brief  Get BT Link Information(support single link)
  * @param  pLinkInfo: the buffer for storing BT Link information
  *     @arg pLinkInfo[0]: the instantaneous value of native clock
  *     @arg pLinkInfo[1]: connection handle
  *     @arg pLinkInfo[2]: anchor point get by ce_early_interrupt
  *     @arg pLinkInfo[3]: connection event interval(unit:1.25ms)
  *     @arg pLinkInfo[4]: slave latency
  * @retval 0 means success, other value means fail. 
  */
uint32_t Get_BT_Link_Info(uint32_t *pLinkInfo)
{
    return FMC_ioctl(flash_ioctl_rpt_bt_link_info, (uint32_t)pLinkInfo, 5);
}

/**
  * @brief  get public key  
  * @param  key[]: the buffer for storing public key(key size is 6).
  * @retval 0 means success, other value means fail. 
  */
uint32_t Get_Public_Key(uint8_t key[])
{
    return FMC_ioctl(flash_ioctl_get_vendor_ID_AB_key, 1, (uint32_t)key);
}

/**
  * @brief  get private key  
  * @param  key[]: the buffer for storing private key(key size is 6).
  * @retval 0 means success, other value means fail. 
  */
uint32_t Get_Private_Key(uint8_t key[])
{
    return FMC_ioctl(flash_ioctl_get_vendor_ID_AB_key, 2, (uint32_t)key);
}

