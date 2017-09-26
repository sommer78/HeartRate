/**
****************************************************************************
* @file      flash.h
* @brief     secondary boot update format definition
* @author    xiaoyu
* @version   v1.0.0
* @date      2016/11/14
* @history
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/

/**
* @addtogroup ROOT flash related definition
* @{
*/



#ifndef __FLASH_H__
#define __FLASH_H__

#include "system_config.h"


/**
 * @name External function definition
 * @{
 */


/**
 * @brief   sector erase
 * @param   sector_num which sector do you want to erase
 * @return  NONE
 */
void user_flash_sector_erase(uint32 sector_num);


/**
 * @brief  only main flash can be read,each time read one word
 * @param  addr the address you want to read
 * @param  len  length
 * @param  *buf the buffer used to save read data
 * @return NONE
 */
void user_flash_read(uint32 addr, uint32 len, uint32 *buf);

/**
 * @brief  only main flash can be written, each time only can be written one word
 * @param  addr the address you want to write,word unit
 * @param  len length of write data
 * @param  buf  write data buffer
 * @return NONE
 */
void user_flash_write(uint32 adr, uint32 len, uint32 *buf);

/**
 * @param  addr addr for 0~4KB
 * @param  len read data length
 * @param  *buf save data from read
 * @return NONE
 */
void user_flash_nvr_read(uint32 addr, uint32 len, uint32 *buf);


/**
 * @}
 */

#endif

/**
 * @}
 * @} //end module
 */


