/**
****************************************************************************
* @file    mxd2660_iic_slave.h
* @brief   
* @author  wanli.han
* @version Initial Draft
* @date    2017/01/04
* @history 
* @note    
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/


#ifndef __MXD2660_IIC_SLAVE_H__
#define __MXD2660_IIC_SLAVE_H__

#include "mxd2660_iic.h"
#ifdef _cplusplus
	extern "C" {
#endif


typedef enum
{
	SLAVE_NOT_INIT,
	SLAVE_INIT,
}I2C_SLVAE_STATE;

typedef void (*_i2c_slave_write_cb)(uint16 reg_addr);
typedef void (*_i2c_slave_read_cb)(void);
typedef struct
{
	uint8 *buf;
	uint16 size;
	uint16 write_pos;
	uint16 read_pos;
	uint16 addr;
	I2C_DEV_MODE addr_mode;
	I2C_REG_MODE reg_mode;
	I2C_SLVAE_STATE status;
	_i2c_slave_read_cb read_cb;
	_i2c_slave_write_cb write_cb;
}I2C_SLAVE_CONFIG;


/**
* @brief  initialize IIC module work in slave mode and config some parameter
* @param  mode 7BIT mode or 10BIT mode, see I2C_DEV_MODE
* @param  buf trans fifo pointer
* @param  size trans fifo size
* @param  addr slave device address
* @return none
*/	
extern void i2c0_slave_init(I2C_DEV_MODE mode, uint8 *buf,uint16 size, uint16 addr);
extern void i2c1_slave_init(I2C_DEV_MODE mode, uint8 *buf,uint16 size, uint16 addr);
/**
* @brief  config read and write operation callback
* @param  write_cb when write operation happend, call this function to do something
* @param  read_cb when read operation happend, call this function to do something
* @return none
*/
extern void i2c0_set_trx_callback(_i2c_slave_write_cb write_cb, _i2c_slave_read_cb read_cb);
extern void i2c1_set_trx_callback(_i2c_slave_write_cb write_cb, _i2c_slave_read_cb read_cb);
/**
* @brief  release iic module
* @param  none
* @return none
*/
extern void i2c0_slave_release(void);
extern void i2c1_slave_release(void);
/**
* @brief  set iic slave trans data pos
* @param  pos trans data start position at trans buffer
* @return none
*/
extern void i2c0_set_op_pos(uint16 pos);
extern void i2c1_set_op_pos(uint16 pos);


#ifdef _cplusplus
	}
#endif




#endif
