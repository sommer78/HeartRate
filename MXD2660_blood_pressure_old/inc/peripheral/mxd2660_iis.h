/*****************************************************************************
  File Name     : mxd2660_iis.h
  Version       : Initial Draft
  Author        : maxscend\yanping.wang
  Created       : 2016/5/29
  Last Modified :
  Description   : iis.c header file
  Function List :
  History       :
  1.Date        : 2016/5/29
    Author      : maxscend\yanping.wang
    Modification: Created file
  2.Date        : 2016/9/14
    Author      : maxscend\wanli.han
    Modification: define macro
******************************************************************************/

#ifndef __MXD2660_IIS_H__
#define __MXD2660_IIS_H__


#include "system_config.h"
#include "mxdaqua.h"
#include "mxd2660_dma.h"


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */



typedef enum
{
	I2S_LJF = 0,    /**<  */
	I2S_I2S = 1,
	I2S_PCMA = 2,
	I2S_PCMB = 3	
}I2S_DATA_FORMAT;

typedef enum
{
	SAMP_RATE_8K,
	SAMP_RATE_16K,
	SAMP_RATE_32K
}I2S_SAMP_RATE;

typedef enum
{
	I2S_DATA_16BIT = 0,
	I2S_DATA_20BIT = 1,
	I2S_DATA_24BIT = 2,
}I2S_DATA_LEN;

typedef struct
{
	
}I2S_CFG;

typedef void (*i2s_data_handler_t)(uint8 status);
/**
* @brief  initialize i2s dma channel
* @param  dam_ch dma channel
* @param  dma_conf dma config information
* @param  buf_addr read data first address
* @return 
*/
extern uint8 i2s_init(uint16 buf_len, void *buf_addr,i2s_data_handler_t handler);
/**
* @brief  config i2s module
* @param  conf config information
* @return none
*/
extern void i2s_conf(uint32 conf);
/**
* @brief  start i2s
* @param  none
* @return none
*/
extern void i2s_start(void);
/**
* @brief  stop i2s
* @param  none
* @return none
*/
extern void i2s_stop(void);
/**
* @brief  config sampling rate. in master mode, we can config 8K,16K,32K
* @param  samp_rate see enum I2S_SAMP_RATE
* @return none
*/
extern void i2s_config_sampling_rate(I2S_SAMP_RATE samp_rate);
/**
* @brief  config i2s data mode
* @param  
* @return none
*/
extern void i2s_config_mode(I2S_DATA_FORMAT format);
/**
* @brief  config i2s sound channel
* @param  bitmap sound channel select,bit0--left channel,bit1--right channel
* @return none
*/
extern void i2s_config_sound_channel(uint8 bitmap);
/**
* @brief  config i2s data length
* @param  bitmap sound channel select,bit0--left channel,bit1--right channel
* @return none
*/
extern void i2s_config_data_len(I2S_DATA_LEN len);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __IIS_H__ */

