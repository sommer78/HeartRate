/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_flash_storage.h
* @brief     header file of flash storage interface.
* @details   
* @author    raven_su
* @date      2015-05-08
* @version   v0.1
* *********************************************************************************************************
*/
#ifndef rtl876x_flash_storage_h
#define rtl876x_flash_storage_h

#include <stdint.h>

/////////////////////////////////////////////////////////////////////

/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup Flash_Storage Flash Storage
  * @brief Flash Storage modules
  * @details This module contains a group of high-level Flash load/save API.\n
  *    For storing some data less than 2048 bytes, there are two generic load/save functions:\n 
  *      @ref fs_load_vendor_data\n
  *      @ref fs_save_vendor_data\n
  *    There is a 2048 bytes logic area in Flash, both @ref fs_load_vendor_data and
  *      @ref fs_save_vendor_data use this area.
  * @{
  */

/** @defgroup Flash_Storage_Exported_Types Flash Storage Exported Types
  * @{
  */

/**
 * @brief remote_BD_struct
 *
 * remote_BD_struct
 */
typedef struct
{
    uint8_t  addr[6];
    uint8_t  remote_bd_type;    
    uint8_t  padding;
}remote_BD_struct;

/**
 * @brief LTK_struct
 *
 * LTK_struct 
 */
typedef struct
{
    uint8_t linkKeyLength;
    uint8_t padding[3];    
    uint8_t linkKey[28];
}LTK_struct;

/**
 * @brief remLTK_struct
 *
 * remLTK_struct 
 */
typedef struct
{
    uint8_t linkKeyLength;
    uint8_t padding[3];        
    uint8_t linkKey[28];
}remLTK_struct;

/**
 * @brief IRK_struct
 *  
 * IRK_struct
 */
typedef struct
{
    uint8_t linkKeyLength;
    uint8_t padding[3];
    uint8_t linkKey[28];
}IRK_struct;

/**
 * @brief Local_name_struct
 *
 * Local_name_struct
 */
typedef struct
{
    uint8_t Local_name[64];   
}Local_name_struct;

/**
 * @brief cccData_struct
 *
 * cccData_struct
 */
typedef struct
{
    uint8_t data_length;    
    uint8_t padding[3];
    
    uint8_t val[32];   
}cccData_struct;


/**
  * @}
  */

  
/** @defgroup Flash_Storage_Exported_Functions IO Flash Storage Functions
  * @{
  */  
  
/**
  * @brief  save remote_BD_struct into storage
  * @param  remote_BD_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_save_remote_BD_struct(remote_BD_struct* pdata);

/**
  * @brief  load remote_BD_struct into storage
  * @param  remote_BD_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_load_remote_BD_struct(remote_BD_struct* pdata);

/**
  * @brief  save LTK_struct into storage
  * @param  LTK_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_save_LTK_struct(LTK_struct* pdata);

/**
  * @brief  load LTK_struct into storage
  * @param  LTK_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_load_LTK_struct(LTK_struct* pdata);

/**
  * @brief  save remLTK_struct into storage
  * @param  remLTK_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_save_remLTK_struct(remLTK_struct* pdata);

/**
  * @brief  load remLTK_struct into storage
  * @param  remLTK_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_load_remLTK_struct(remLTK_struct* pdata);

/**
  * @brief  save IRK_struct into storage
  * @param  IRK_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_save_IRK_struct(IRK_struct* pdata);
/**
  * @brief  load IRK_struct into storage
  * @param  IRK_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_load_IRK_struct(IRK_struct* pdata);

/**
  * @brief  save Local_name_struct into storage
  * @param  Local_name_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_save_Local_name_struct(Local_name_struct* pdata);
/**
  * @brief  load Local_name_struct into storage
  * @param  Local_name_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_load_Local_name_struct(Local_name_struct* pdata);

/**
  * @brief  save cccData_struct into storage
  * @param  cccData_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_save_cccData_struct(cccData_struct* pdata);
/**
  * @brief  load cccData_struct into storage
  * @param  cccData_struct pointer
  * @retval 0 means success, or means fail
  */
uint32_t fs_load_cccData_struct(cccData_struct* pdata);


uint32_t fs_save_vendor_data(void* pdata, uint16_t data_size, uint16_t addr_offset);
uint32_t fs_load_vendor_data(void* pdata, uint16_t data_size, uint16_t addr_offset);

/**
  * @brief  flash read 
  * @param  address must 4byte alignment, the valid range is from 0x50A000 to 0x50A000+100K-4, beware the space is shared with app image space, pls use it carefully ... 
  * @retval 0 means success, or invalid address will return 0xdeadbeef
  */ 
uint32_t Flash_Read(uint32_t u32Addr);

/**
  * @brief  flash write 
  * @param  address must 4byte alignment, the valid range is from 0x50A000 to 0x50A000+100K-4, beware the space is shared with app image space, pls use it carefully ... 
  * @param  data
  * @retval 0 means success, or invalid address will return 0xdeadbeef
  */ 
uint32_t Flash_Write(uint32_t u32Addr, uint32_t u32Data);

/**
  * @brief  flash erase page  
  * @param  address must 2kbyte alignment, the valid range is from 0x50A000 to 0x50A000+100K-2k, beware the space is shared with app image space, pls use it carefully ... 
  * @retval 0 means success, or invalid address will return 0xdeadbeef
  */ 
uint32_t Flash_Erase_Page(uint32_t u32PageAddr);

/////////////////////////////////////////////////////////////////////

  
/**
  * @}
  */  
  
/**
  * @}
  */ 

/**
  * @}
  */   


#endif // rtl876x_flash_storage_h

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor *****END OF FILE****/
