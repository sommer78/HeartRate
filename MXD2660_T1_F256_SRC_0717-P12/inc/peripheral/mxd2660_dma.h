/**
****************************************************************************
* @file  mxd2660_dma.h
* @brief
* @author maxscend\yanping.wang
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

#ifndef __MXD2660_DMA_H__
#define __MXD2660_DMA_H__

#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "system_config.h"
#include "mxdaqua.h"


#define dmair_dma_int_status 0x00

#define dmair_dma_int_mask 0x01

#define dmair_dma_int_clear 0x02

#define dma_0_config 0x03
#define dma_0_config_0_en_bit ((uint32)1<<31)
#define dma_0_config_0_pause_bit (1<<30)
#define dma_0_config_0_src_inc_bit (1<<29)
#define dma_0_config_0_src_size_shift 27
#define dma_0_config_0_src_size_mask ((1<<2)-1)
#define dma_0_config_0_dst_inc_bit (1<<26)
#define dma_0_config_0_dst_size_shift 24
#define dma_0_config_0_dst_size_mask ((1<<2)-1)
#define dma_0_config_0_mem_to_mem_bit (1<<23)
#define dma_0_config_0_mem_to_peri_bit (1<<22)
#define dma_0_config_0_peri_sel_shift 18
#define dma_0_config_0_peri_sel_mask ((1<<4)-1)
#define dma_0_config_0_pri_bit (1<<17)
#define dma_0_config_0_circ_bit (1<<16)
#define dma_0_config_0_cfg_cnt_shift 0
#define dma_0_config_0_cfg_cnt_mask ((1<<16)-1)

#define dma_0_src_addr 0x04

#define dma_0_dst_addr 0x05

#define dma_0_tcnt 0x06

#define dma_1_config 0x07
#define dma_1_config_1_en_bit ((uint32)1<<31)
#define dma_1_config_1_pause_bit (1<<30)
#define dma_1_config_1_src_inc_bit (1<<29)
#define dma_1_config_1_src_size_shift 27
#define dma_1_config_1_src_size_mask ((1<<2)-1)
#define dma_1_config_1_dst_inc_bit (1<<26)
#define dma_1_config_1_dst_size_shift 24
#define dma_1_config_1_dst_size_mask ((1<<2)-1)
#define dma_1_config_1_mem_to_mem_bit (1<<23)
#define dma_1_config_1_mem_to_peri_bit (1<<22)
#define dma_1_config_1_peri_sel_shift 18
#define dma_1_config_1_peri_sel_mask ((1<<4)-1)
#define dma_1_config_1_pri_bit (1<<17)
#define dma_1_config_1_circ_bit (1<<16)
#define dma_1_config_1_cfg_cnt_shift 0
#define dma_1_config_1_cfg_cnt_mask ((1<<16)-1)

#define dma_1_src_addr 0x08

#define dma_1_dst_addr 0x09

#define dma_1_tcnt 0x0a

#define dma_2_config 0x0b
#define dma_2_config_2_en_bit ((uint32)1<<31)
#define dma_2_config_2_pause_bit (1<<30)
#define dma_2_config_2_src_inc_bit (1<<29)
#define dma_2_config_2_src_size_shift 27
#define dma_2_config_2_src_size_mask ((1<<2)-1)
#define dma_2_config_2_dst_inc_bit (1<<26)
#define dma_2_config_2_dst_size_shift 24
#define dma_2_config_2_dst_size_mask ((1<<2)-1)
#define dma_2_config_2_mem_to_mem_bit (1<<23)
#define dma_2_config_2_mem_to_peri_bit (1<<22)
#define dma_2_config_2_peri_sel_shift 18
#define dma_2_config_2_peri_sel_mask ((1<<4)-1)
#define dma_2_config_2_pri_bit (1<<17)
#define dma_2_config_2_circ_bit (1<<16)
#define dma_2_config_2_cfg_cnt_shift 0
#define dma_2_config_2_cfg_cnt_mask ((1<<16)-1)

#define dma_2_src_addr 0x0c

#define dma_2_dst_addr 0x0d

#define dma_2_tcnt 0x0e

#define dma_3_config 0x0f
#define dma_3_config_3_en_bit ((uint32)1<<31)
#define dma_3_config_3_pause_bit (1<<30)
#define dma_3_config_3_src_inc_bit (1<<29)
#define dma_3_config_3_src_size_shift 27
#define dma_3_config_3_src_size_mask ((1<<2)-1)
#define dma_3_config_3_dst_inc_bit (1<<26)
#define dma_3_config_3_dst_size_shift 24
#define dma_3_config_3_dst_size_mask ((1<<2)-1)
#define dma_3_config_3_mem_to_mem_bit (1<<23)
#define dma_3_config_3_mem_to_peri_bit (1<<22)
#define dma_3_config_3_peri_sel_shift 18
#define dma_3_config_3_peri_sel_mask ((1<<4)-1)
#define dma_3_config_3_pri_bit (1<<17)
#define dma_3_config_3_circ_bit (1<<16)
#define dma_3_config_3_cfg_cnt_shift 0
#define dma_3_config_3_cfg_cnt_mask ((1<<16)-1)

#define dma_3_src_addr 0x10

#define dma_3_dst_addr 0x11

#define dma_3_tcnt 0x12

#define dma_4_config 0x13
#define dma_4_config_4_en_bit ((uint32)1<<31)
#define dma_4_config_4_pause_bit (1<<30)
#define dma_4_config_4_src_inc_bit (1<<29)
#define dma_4_config_4_src_size_shift 27
#define dma_4_config_4_src_size_mask ((1<<2)-1)
#define dma_4_config_4_dst_inc_bit (1<<26)
#define dma_4_config_4_dst_size_shift 24
#define dma_4_config_4_dst_size_mask ((1<<2)-1)
#define dma_4_config_4_mem_to_mem_bit (1<<23)
#define dma_4_config_4_mem_to_peri_bit (1<<22)
#define dma_4_config_4_peri_sel_shift 18
#define dma_4_config_4_peri_sel_mask ((1<<4)-1)
#define dma_4_config_4_pri_bit (1<<17)
#define dma_4_config_4_circ_bit (1<<16)
#define dma_4_config_4_cfg_cnt_shift 0
#define dma_4_config_4_cfg_cnt_mask ((1<<16)-1)

#define dma_4_src_addr 0x14

#define dma_4_dst_addr 0x15

#define dma_4_tcnt 0x16

#define dma_int_sel 0x17

#define dma_to_thld 0x18


/******************************************************************/
typedef enum DMA_ISR
{
    DMA_1_4_DONE = (1 << 4),       /****/

    DMA_2_4_DONE = (1 << 3),       /****/

    DMA_3_4_DONE = (1 << 2),       /****/

    DMA_4_4_DONE = (1 << 1),       /****/

    DMA_TIME_OUT = (1 << 0),       /****/

} DMA_ISR_ENUM;


typedef enum DMA_SELECT
{

    DMA_0 = 0,

    DMA_1 = 1,

    DMA_2 = 2,

    DMA_3 = 3,

    DMA_4 = 4

} DMA_SELECT_ENUM;

typedef enum DMA_ENABLE
{
    DMA_DIS = 0,

    DMA_EN = 1

} DMA_ENABLE_ENUM;

typedef enum DMA_SRC_INC
{
    SRC_NOT_INC = 0,      /**source address not increment  ***/

    SRC_INC = 1           /**source address increment  ******/

} DMA_SRC_INC_ENUM;


typedef enum DMA_SRC_SIZE
{
    SRC_SIZE_BYTE = 0,       /***source size 8 bit****/

    SRC_SIZE_HALFWORD = 1,   /***source size 16 bit***/

    SRC_SIZE_WORD = 2        /***source size 32 bit***/

} DMA_SRC_SIZE_ENUM;

typedef enum DMA_DST_INC
{
    DST_NOT_INC = 0,      /**destination address not increment  ***/

    DST_INC = 1           /**destination address increment  ******/

} DMA_DST_INC_ENUM;

typedef enum DMA_DST_SIZE
{
    DST_SIZE_BYTE = 0,       /***destination size 8 bit****/

    DST_SIZE_HALFWORD = 1,   /***destination size 16 bit***/

    DST_SIZE_WORD = 2        /***destination size 32 bit***/

} DMA_DST_SIZE_ENUM;

typedef enum DMA_WORK_MODE
{
    PERI_TO_MEM = 0,        /*** dma work mode peripheral to memory  ****/

    MEM_TO_PERI = 1,        /*** dma work mode memory to peripheral  ****/

    MEM_TO_MEM = 2          /*** dma work mode memory to memory    ****/

} DMA_WORK_MODE_ENUM;

typedef enum DMA_PERIPHERAL_SELECT
{
    UART0_TX = 0,

    UART0_RX = 1,

    UART1_TX = 2,

    UART1_RX = 3,

    SPI0_TX = 4,

    SPI0_RX = 5,

    SPI1_TX = 6,

    SPI1_RX = 7,

    I2C0_TX = 8,

    I2C0_RX = 9,

    I2C1_TX = 10,

    I2C1_RX = 11,

    _7816_TX = 12,

    _7816_RX = 13,

    I2S_RX = 14,

    Reserved = 15

} DMA_PERIPHERAL_SELECT_ENUM;

typedef enum DMA_PRIORITY_LEVEL
{
    PRI_LOW = 0,

    PRI_HIGH = 1

} DMA_PRIORITY_LEVEL_ENUM;

typedef enum DMA_CIRCULAR
{
    CIRC_NOT = 0,     /***  not enable circular  ****/

    CIRC_EN = 1       /***  circular mode enable ****/

} DMA_CIRCULAR_ENUM;

typedef enum DMA_PERI_BASE_ADDR
{
    UART0_TX_ADDR = 0x40013000,     /* uart0 tx fifo*/

    UART0_RX_ADDR = 0x40013004,     /* uart0 rx fifo*/

    UART1_TX_ADDR = 0x40014000,     /* uart1 tx fifo*/

    UART1_RX_ADDR = 0x40014004,     /* uart1 rx fifo*/

    SPI0_TX_ADDR = 0x40019000,      /* spi slave tx fifo */

    SPI0_RX_ADDR = 0x40019004,      /* spi slave rx fifo*/

    SPI1_TX_ADDR = 0x4001a000,      /* spi master tx fifo*/

    SPI1_RX_ADDR = 0x4001a004,       /* spi master rx fifo*/

    I2C0_TX_ADDR = 0x40011000,      /* i2c0 tx fifo*/

    I2C0_RX_ADDR = 0x40011004,       /* i2c0 rx fifo*/

    I2C1_TX_ADDR = 0x40012000,       /* i2c1 tx fifo*/

    I2C1_RX_ADDR = 0x40012004,       /* i2c1 rx fifo*/

    I2S_RX_ADDR = 0x40020000,        /** I2S rx fifo **/

    _7816_TX_ADDR = 0x40021000,    /** 7816 tx fifo **/

    _7816_RX_ADDR = 0x40021004,    /** 7816 rx fifo **/

} DMA_PERI_BASE_ADDR_ENUM;

#define dma_reg_write( regAddr,  regVal) \
               ((volatile uint32 *)DMA_BASE)[regAddr] = regVal

#define dma_reg_read( regAddr,  pRegVal) \
                *pRegVal = ((volatile uint32 *)DMA_BASE)[regAddr]


typedef void (*dma_handler_t)(uint8 isr_status);

typedef struct DMA_HANDLER
{
    uint8 status;
    dma_handler_t dma_isr[5];
} DMA_HANDLER_STRU;




/**
* @brief  dma init
* @param  ch_sel, dma channel 0-4
* @param  peri_sel peripheral sel
* @param  buf_size, buffer length
* @param  srcAdd,  rsource address
* @param  dstAddr,  destination address
* @return extern N/A
*/
extern void dma_init(uint8 ch_sel, uint8 peri_sel, uint16 buf_size, uint32 srcAddr, uint32 dstAddr);
/**
* @brief  dma_regsiter, get free dma channel_id, set callback function
* @param pf_handler, user regsiter callback function
* @return dma channel (0-4)
*/
extern uint8 dma_regsiter(dma_handler_t pf_handler);
/**
* @brief   dma release,  release channel id
* @param   channel id
* @return N/A
*/
extern void dma_release(uint8 ch);
/**
* @brief    set dma interrupt
* @param   ch_id, channel id.
* @param   isr_en, enable dma interrupt.
* @return extern
*/
extern void dma_set_isr(uint8 ch_id, uint8 isr_en);

extern void dma_continue(uint8 ch);
extern void dma_disable(uint8 ch);
extern void dma_enable(uint8 ch);
extern void DMA_IRQ0Handler(void);
extern void DMA_IRQ1Handler(void);
extern void dma_pause(uint8 ch);

extern DMA_HANDLER_STRU gDMA;

#endif/* _dma_reg_h_ */

