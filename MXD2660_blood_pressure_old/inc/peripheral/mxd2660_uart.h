/**
****************************************************************************
* @file mxd2660_uart.h
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


#ifndef __MXD2660_UART_H__
#define __MXD2660_UART_H__


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "log_descriptor.h"



/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */
enum
{
    //don't care parity ,
    ERROR_DISCARD_DIS = 0,
    //if parity error, discarding the data
    ERROR_DISCARD_EN = 1
};

enum
{
    //disable hw flow control
    NO_HW_FLOW = 0,
    //enable hw flow control
    HW_FLOW = 1
};

enum
{
    //no send line break
    NO_SND_LINE_BREAK = 0,
    //send line break
    SND_LINE_BREAK = 1
};

enum
{
    /// Little endian
    UART_LITTLE = 0,
    /// big endian
    UART_BIG  = 1
};

/// Character format
enum
{
    /// char format 5
    UART_CHARFORMAT_5 = 0,
    /// char format 6
    UART_CHARFORMAT_6 = 1,
    /// char format 7
    UART_CHARFORMAT_7 = 2,
    /// char format 8
    UART_CHARFORMAT_8 = 3
};

/// Stop bit
enum
{
    /// stop bit 1
    UART_STOPBITS_1 = 0,
    /* Note: The number of stop bits is 1.5 if a
     * character format with 5 bit is chosen */
    UART_STOPBITS_1p5 = 1,
    /// stop bit 2
    UART_STOPBITS_2 = 2
};

/// Parity bit
enum
{
    /// odd parity
    UART_PARITYBIT_ODD   = 0,
    /// even parity
    UART_PARITYBIT_EVEN  = 1,
    /// stick1 parity
    UART_PARITYBIT_STICK1 = 2, // The parity bit is always 1.
    /// stick0 parity
    UART_PARITYBIT_STICK0  = 3,  // The parity bit is always 0.
    /// no parity
    UART_NO_PARITY = 4
};



/// status values
enum
{
    UART_FE_ERR     = 0,

    UART_PARITY_ERR = 1,

    UART_LB_ERR     = 2,

    UART_RX_TO      = 3,

    UART_TFIFO_UNTH = 4,

    UART_TFIFO_EMPT = 5,

    UART_RFIFO_OVTH = 6,

    UART_RFIFO_OVFL = 7,

    UART_RFIFO_EMPT_N = 8
};

typedef struct
{
    uint32 tfifo_thld_4bit : 4;
    uint32 rfifo_thld_4bit : 4;
    uint32 to_thld_8bit : 8;
    uint32 stop_bit_2bit : 2;
    uint32 byte_size_2bit : 2;
    uint32 parity_mode_3bit : 3;
    uint32 msb_1bit : 1;
    uint32 lb_snd_1bit : 1;
    uint32 flow_ctrl_en_1bit : 1;
    uint32 rts_thld_4bit : 4;
    uint32 err_discard_1bit : 1;
    uint32 Reserved_1bit : 1;
} st_uart_config_reg;

//Figure 2.13: Data channel PDU header
typedef union
{
    st_uart_config_reg  _B;
    uint32 uart_config;
} ut_uart_config;


typedef enum PERI_CTRL
{
    UART_USE_DMA = (1 << 0)

} PERI_CTRL_ENUM;

typedef void (*uart_rx_handler_t)(void);
typedef void (*uart_tx_handler_t)(void);
typedef void (*uart_irq_handler_t)(uint32);

typedef struct
{
    uart_rx_handler_t uart_receive_cb;
    uart_tx_handler_t uart_transmit_cb;
} uart_handle_CBs_t;


//baud rate
#define BAUDRATE_2400 ((0x1A09) | (0x3B7<<16))
#define BAUDRATE_9600 ((0x681) | (0xED<<16))
#define BAUDRATE_14400 ((0x456) | (0x9E<<16))
#define BAUDRATE_19200 ((0x340) | (0x76<<16))
#define BAUDRATE_38400 ((0x19F) | (0x3A<<16))
#define BAUDRATE_57600 ((0x114) | (0x27<<16))
#define BAUDRATE_115200 ((0x89) | (0x13<<16))
#define BAUDRATE_256000 ((0x3D) | (0x8<<16))
#define BAUDRATE_230400 ((0x44) | (0x9<<16))
#define BAUDRATE_460800 ((0x22) | (0x4<<16))
#define BAUDRATE_921600 ((0x10) | (0x2<<16))
#define BAUDRATE_2250000 ((0x6) | (0x0<<16))
#define BAUDRATE_2000000 ((0x7) | (0x1<<16))
#define BAUDRATE_1600000 ((0x9) | (0x1<<16))


//threshold
#define DMA_FIFO_THRD 1
#define DMA_FIFO_THRD 1

#define RX_FIFO_THRD 6
#define TX_FIFO_THRD 6
#define HW_FC_THRD 5

//rx timeout
#define RX_TIMEOUT 0xFF  //32 us
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
extern void UART0_IRQHandler(void);


/**
* @brief  uart0 init default config
* @param uart baudrate
* @return N/A
*/
extern void uart0_init(uint32 uart_baud);
/**
* @brief  uart1 init default config
* @param uart baudrate
* @return N/A
*/
extern void uart1_init(uint32 uart_baud);
/**
* @brief  uart0 set user-defined config
* @param uart_config
* @return N/A
*/
extern void uart0_set_config(uint32 uart_config);
/**
* @brief  uart1 set user-defined config
* @param uart_config
* @return N/A
*/
extern void uart1_set_config(uint32 uart_config);
/**
* @brief  uart0 set user-defined interrupt mask
* @param uart_mask
* @return N/A
*/
extern void uart0_set_mask(uint32 uart_mask);
/**
* @brief  uart1 set user-defined interrupt mask
* @param uart_mask
* @return N/A
*/
extern void uart1_set_mask(uint32 uart_mask);
/**
* @brief  uart0 get char
* @param  NONE
* @return DATA
*/
extern uint8 uart0_get(void);
/**
* @brief  uart1 get char
* @param  *data  store data
* @return true or false
*/
extern bool uart1_get(uint8 *data);
/**
* @brief  uart0 put char
* @param  *data  store data
* @return true or false
*/
extern void uart0_put(uint8 ch);
/**
* @brief  uart1 put char
* @param  *data  store data
* @return true or false
*/
extern bool uart1_put(uint8 ch);

/**
* @brief config uart0 threshold,include receive over threshold and trans under threshold
* @param rx_over_th receive fifo over threshold, 0~8
* @param tx_under_th trans fifo under threshold, 0~8
* @return N/A
*/
extern void uart0_set_threshold(uint8 rx_over_th, uint8 tx_under_th);
extern void uart1_set_threshold(uint8 rx_over_th, uint8 tx_under_th);

/**
* @brief uart0 set user-defined baudrate
* @param baudrate
* @return N/A
*/
void uart0_set_baudrate(uint32 uart_baud);
void uart1_set_baudrate(uint32 uart_baud);


/**
* @brief get receive fifo data length
* @param N/A
* @return N/A
*/
extern uint8 uart0_get_rfifo_cnt(void);
extern uint8 uart1_get_rfifo_cnt(void);

/**
* @brief get trans fifo data length
* @param N/A
* @return N/A
*/
extern uint8 uart0_get_tfifo_cnt(void);
extern uint8 uart1_get_tfifo_cnt(void);


extern void uart0_send(uint8 *buf, uint32 len);
extern void uart1_send(uint8 *buf, uint32 len);

extern void uart1_Log_output(uint16 address, uint16 data);

/** Ã¶¾Ù³£Á¿ */
typedef enum UART_LOG
{
    UART_BASE =  LOG_UART_DEBUG_BASE,
    UART_DMA_RX_ISR_STATUS = LOG_UART_DEBUG_BASE + 1,
    UART_DMA_PERI_RD = LOG_UART_DEBUG_BASE + 2,
    UART_DMA_PERI_WR = LOG_UART_DEBUG_BASE + 3,
    UART_DMA_TX_ISR_STATUS = LOG_UART_DEBUG_BASE + 4,
    UART_DMA_EVENT_RD = LOG_UART_DEBUG_BASE + 5,
    UART_DMA_EVENT_WR = LOG_UART_DEBUG_BASE + 6,
    UART_DMA_TX_SEND_CNT = LOG_UART_DEBUG_BASE + 7,
    UART_DMA_PERI_RD_ISR = LOG_UART_DEBUG_BASE + 8,
    UART_DMA_PERI_WR_ISR = LOG_UART_DEBUG_BASE + 9,
    UART_DMA_EVENT_RD_ISR = LOG_UART_DEBUG_BASE + 10,
} UART_LOG_ENUM;




#endif /* _UART_H_ */
