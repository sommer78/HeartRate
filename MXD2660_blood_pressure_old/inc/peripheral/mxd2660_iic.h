/**
****************************************************************************
* @file mxd2660_iic.h
* @brief
* @author wanli.han
* @version Initial Draft
* @date 2016/11/18
* @history 
* @note	 
* detailed description 

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2016 maxscend </center></h2>
*/
#ifndef __MXD2660_IIC_H__
#define __MXD2660_IIC_H__

#ifdef __cplusplus
 extern "C" {
#endif 

	 
#include "system_config.h"
#include "mxdaqua.h"
#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition



//i2cir_tfifo_wr_data:i2c_tfifo:0x0000  

//i2cir_rfifo_rd_data:i2c_rfifo:0x0004

//i2cir_int_status:INT_STATUS:0x0008
#define IIC_SLAVE_ADDR_MATCH_BIT        (11)
#define IIC_SLAVE_BUS_ERROR_BIT         (10)
#define IIC_MASTER_TRANS_COMP_BIT       (9)
#define IIC_RECV_OVER_RD_BIT            (8)
#define IIC_RECV_NOT_EMPTY_BIT          (7)
#define IIC_RECV_FULL_BIT               (6)
#define IIC_RECV_OVER_THRESHOLD_BIT     (5)
#define IIC_TRANS_OVER_WR_BIT           (4)
#define IIC_TRANS_OVER_RD_BIT           (3)
#define IIC_TRANS_EMPTY_BIT             (2)
#define IIC_TRANS_UNDER_THRESHOLD_BIT   (1)
#define IIC_RECV_TMOUT_BIT              (0)
#define IIC_READ_MASK                   (0x3E1)
#define IIC_WRITE_MASK                  (0x3FF)



#define I2C_DEV_TOTEL_NUM	(8)
//speed calculating formula: 16MHz/(5*nKHz)-1 = 63
#define I2C_CALC_PRER(speed) (16000/(5*speed)-1)
//#define I2C_CALC_PRER(speed) (63)
#define I2C_MAX_WRITE_LENTGH (253)
#define IIC_TX_THRESHOLD (4)
#define IIC_RX_THRESHOLD (4)
#define IIC_LOG_EN 0

#if IIC_LOG_EN
	#define IIC_LOG(addr, data) LOG(address, data)
#else
	#define IIC_LOG(addr, data) 
#endif



typedef enum
{
    I2C_DEV_7BIT = 0,      /**< 7bit addr */
	I2C_DEV_10BIT = 1      /**< 10bit addr */        
}I2C_DEV_MODE;

typedef enum
{
    I2C_REG_BYTE = 1,      /**< storage space is 1byte. */
	I2C_REG_WORD = 2       /**< srotare space is 2byte. */        
}I2C_REG_MODE;


typedef struct
{
	uint8 channel;            /**< I2C controller channel:0-I2C0,1-I2C1 */
	I2C_DEV_MODE dev_mode;    /**< device address mode: value range is enum I2C_DEV_ADDR_MODE */
	I2C_REG_MODE reg_mode;    /**< device storage space address mode:range is enum I2C_SUB_ADDR_MODE */
	uint16 dev_addr;          /**< deviec address */
	uint16 speed;             /**< CLK frequency:KHz, range:100--400 */
}I2C_DEV_CONFIG;

typedef struct
{
	uint8 ch;                 /**<iic select£º0--IIC0, 1--IIC1 */
	I2C_REG_MODE reg_mode;    /**<iic register address lenght */
   	uint32 mcfg;              /**<i2c_i2cm_cfg, include clock prescale[23:8],and address mode[6] */
	uint32 maddr;             /**<i2c_i2cm_tar, with the device address value */
}I2C_MASTER_CONFIG;

typedef struct
{
    uint32 i2c_error;        /**< return value, app should check it, now it is for debuf */
	uint16 reg_addr;         /**< data address in the device */
	uint8 length;            /**< data trans length, not include reg_addr size,maxinum value is 253 */
	uint8 *buf;				 /**< data buf */
}I2C_OPER_CONFIG_STRU;




/**
* @brief  register device.before use operate iic device,please registerit and get id
* @param  dev_conf device config information
* @return positive number is device di
*         others is failed
*/
extern uint8 i2c_register_device(I2C_DEV_CONFIG *dev_conf);
/**
* @brief  deregister device.
* @param  dev_id device id you want to release
* @return none
*/
extern void i2c_deregister_device(uint16 dev_addr);
/**
* @brief  read data from device use iic0
* @param  buf read data save fifo
* @param  len read data length
* @param  reg_addr register address
* @param  dev_addr i2c device address
* @return read length
*/
extern uint8 i2c0_read(uint8 *buf, uint8 len, uint16 reg_addr, uint16 dev_addr);
/**
* @brief  write data to device use iic0
* @param  buf write data fifo
* @param  len write data length
* @param  reg_addr register address
* @param  dev_addr i2c device address
* @return write length
*/
extern uint8 i2c0_write(uint8 *buf, uint8 len, uint16 reg_addr, uint16 dev_addr);
/**
* @brief  read data from device use iic0
* @param  buf read data save fifo
* @param  len read data length
* @param  reg_addr register address
* @param  dev_addr i2c device address
* @return read length
*/
extern uint8 i2c1_read(uint8 *buf, uint8 len, uint16 reg_addr, uint16 dev_addr);
/**
* @brief  write data to device use iic1
* @param  buf write data fifo
* @param  len write data length
* @param  reg_addr register address
* @param  dev_addr i2c device address
* @return write length
*/
extern uint8 i2c1_write(uint8 *buf, uint8 len, uint16 reg_addr, uint16 dev_addr);

/**
* @brief  read data from device use iic0
* @param  buf read data save fifo
* @param  len read data length
* @param  reg_addr register address
* @param  dev_addr i2c device address
* @return read length
*/
extern uint8 i2c0_read_direct(uint8 *buf, uint8 len, uint16 dev_addr);



#define I2C_DEBUG_EN 1
#if I2C_DEBUG_EN
	#define i2c_log(addr,value)   LOG(addr,value)
#else
	#define i2c_log(addr,value)   
#endif



#ifdef __cplusplus
}
#endif

#endif /* _i2c_reg_h_ */
