/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     adns3000.c
* @brief
* @details
* @author   Chuanguo Xue
* @date     2015-04-20
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "board.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_spi.h"
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"

#include "trace.h"
#include "os.h"
#include "bee_message.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"

extern BOOL allowedSensor;
BEE_IO_MSG mouse_xy_msg;

/* adns3000 register address */
#define REG_PROD_ID             0x00    /* R, product id */
#define REG_REV_ID              0x01    /* R revision id */
#define REG_MOTION_STA          0x02    /* R, motion status */
#define REG_DELTA_X             0x03    /* R, lower byte of Delta_X */
#define REG_DELTA_Y             0x04    /* R, lower byte of Delta_Y */
#define REG_SQUAL               0x05    /* R, squal quality */
#define REG_SHUT_HI             0x06    /* R, shutter open time, high byte */
#define REG_SHUT_LO             0x07    /* R, shutter open time, lower byte */
#define REG_PIX_MAX             0x08    /* R, maximum pixel value */
#define REG_PIX_AVR             0x09    /* R, average pixel value */
#define REG_PIX_MIN             0x0a    /* R, minimum pixel value */
#define REG_PIX_GRAB            0x0b    /* RW, pixel grabber*/
#define REG_DELTA_XY_HIGH       0x0c    /* R, Upper 4 bits of Delta X and Y displacement */
#define REG_MOSUE_CTRL          0x0d    /* RW, mosue control */
#define REG_RUN_DOWN_SHIFT      0x0e    /* RW, run to rest1 time */

#define REG_REST1_PERIOD        0x0f    /* RW, rest1 period */
#define REG_REST1_DOWN_SHIFT    0x10    /* RW, rest1 to rest2 time */

#define REG_REST2_PERIOD        0x11    /* RW, rest2 period */
#define REG_REST2_DOWN_SHIFT    0x12    /* RW, rest2 to rest3 time */

#define REG_REST3_PERIOD        0x13    /* RW, rest3 period */
#define REG_PERFORMANCE         0x22    /* RW, performance */
#define REG_RESET               0x3a    /* W, reset */
#define REG_NOT_REV_ID          0x3f    /* R, Inverted Revision ID */
#define REG_LED_CTRL            0x40    /* RW, led control */
#define REG_MOTION_CTRL         0x41    /* RW, motion ctrl */
#define REG_BURST_READ_FIRST    0x42    /* RW, burst read start register address */
#define REG_REST_MODE_CONFIG    0x45    /* RW, rest mode configuration */
#define REG_MOTION_BURST        0x63    /* R, enable burst read, read from this register to start burst read */

/* motion interrupt type */
#define EDGE_LOW_ACTIVE             0x01    /* edge interrupt, low active */
#define EDGE_HIGH_ACTIVE            0x02    /* edge interrupt, high active */
#define LEVEL_LOW_ACTIVE            0x03    /* level interrupt, low active */
#define LEVEL_HIGH_ACTIVE           0x04    /* level interrupt, high active */

/* dpi */
#define DPI_250                     (0x01 << 2)
#define DPI_500                     (0x02 << 2)
#define DPI_1000                    (0x00 << 2)
#define DPI_1250                    (0x03 << 2)
#define DPI_1500                    (0x04 << 2)
#define DPI_1750                    (0x05 << 2)
#define DPI_2000                    (0x06 << 2)

/* motion report length */
#define MOT_REPORT_LEN_8BIT         (0x00 << 7) /* use delta x and delta y*/
#define MOT_REPORT_LEN_12BIT        (0x01 << 7) /* use delta x and delta y and delta xy */

/**
 * @brief  spi chip select.
 * @param  on_off, 1--chip select, 0-- do not select chip.
 * @return none.
 * @retval void.
*/
static void MouseChipSelect(uint8_t on_off)
{
    /* chip select */
    if (1 == on_off)
    {
        GPIO_ResetBits(GPIO_GetPin(SENSOR_SPI_CS));
    }

    /* chip disable */
    if (0 == on_off)
    {
        GPIO_SetBits(GPIO_GetPin(SENSOR_SPI_CS));
    }
    return;
}


/**
 * @brief  spi register write.
 * @param  reg_addr, --register address.
 *          val, --the value to be written.
 * @return none.
 * @retval void.
*/
void Mouse_SPI_SingleWrite(uint8_t reg_addr, uint8_t val)
{
    /* 1. write register address */
    SPI_SendData(SPI0, (uint16_t)(reg_addr | 0x80));
    /*Waiting for SPI data transfer to end*/
    while (SPI_GetFlagState(SPI0, SPI_FLAG_RFNE) != SET);

    /* dump read */
    SPI_ReceiveData(SPI0);

    /* 2. write byte */
    SPI_SendData(SPI0, (uint16_t)val);
    while (SPI_GetFlagState(SPI0, SPI_FLAG_RFNE) != SET);
    SPI_ReceiveData(SPI0);

    return;
}


/**
 * @brief  read register.
 * @param  reg_addr, --register address.
 * @return the byte read.
*/
static uint8_t Mouse_SPI_SingleRead(uint8_t reg_addr)
{
    uint8_t read_byte = 0x00;

    /* 1. write register address */
    SPI_SendData(SPI0, (uint16_t)(reg_addr & 0x7f));
    /*Waiting for SPI data transfer to end*/
    while (SPI_GetFlagState(SPI0, SPI_FLAG_RFNE) != SET);
    /* dump read */
    SPI_ReceiveData(SPI0);
    /* 2. should delay minimum 4us here */
    /* 3. read byte */
    SPI_SendData(SPI0, 0xff);
    /*Waiting for SPI data transfer to end*/
    while (SPI_GetFlagState(SPI0, SPI_FLAG_RFNE) != SET);
    read_byte =  (uint8_t)SPI_ReceiveData(SPI0);

    return read_byte;
}

/**
 * @brief  set mouse motion interrupt type.
 * @param  motion_int_type, motion interrupt type.
 * @return none.
 * @retval void.
*/
void MouseSetMotionInt(uint8_t motion_int_type)
{
    MouseChipSelect(1);

    switch (motion_int_type)
    {
        case EDGE_LOW_ACTIVE:
            Mouse_SPI_SingleWrite(REG_MOTION_CTRL, 0x40);
            break;

        case EDGE_HIGH_ACTIVE:
            Mouse_SPI_SingleWrite(REG_MOTION_CTRL, 0xc0);
            break;

        case LEVEL_LOW_ACTIVE:
            Mouse_SPI_SingleWrite(REG_MOTION_CTRL, 0x00);
            break;

        case LEVEL_HIGH_ACTIVE:
            Mouse_SPI_SingleWrite(REG_MOTION_CTRL, 0x80);
            break;
    }

    MouseChipSelect(0);

    return;
}

/**
 * @brief set mouse dpi and report length.
 * @param  dpi, mosue dpi value
            report_len report data length.
 * @return none.
 * @retval void.
*/
void MouseSetDpiAndReportLen(uint8_t dpi, uint8_t report_len)
{
    MouseChipSelect(1);
    Mouse_SPI_SingleWrite(REG_MOSUE_CTRL, dpi | report_len | (1 << 5));
    MouseChipSelect(0);

    return;
}

/**
 * @brief  reset chip, not necessary.
 * @param  none.
 * @return none.
 * @retval void.
*/
void MouseReset(void)
{
    MouseChipSelect(1);
    Mouse_SPI_SingleWrite(REG_RESET, 0x5a);
    MouseChipSelect(0);
    return;
}


/**
 * @brief  get production id and version id.
 * @param  p_id, --pointer to production/version id buffer.
 * @return none.
 * @retval void.
*/
void MouseGetProdRevId(uint8_t *p_id)
{
    MouseChipSelect(1);
    *p_id++ = Mouse_SPI_SingleRead(REG_PROD_ID);
    MouseChipSelect(0);
    /* may delay some time here */
    MouseChipSelect(1);
    *p_id++ = Mouse_SPI_SingleRead(REG_REV_ID);
    MouseChipSelect(0);

    return;
}

/**
 * @brief  get delta x(8bit) and delta y(8bit).
 * @param  p_delta_xy, --pointer to buffer to save delta x and delta y value.
 * @return none.
 * @retval void.
*/
void MouseGet8bitDeltaXY(uint8_t *p_delta_xy)
{
    MouseChipSelect(1);
    *p_delta_xy++ = Mouse_SPI_SingleRead(REG_DELTA_X);
    MouseChipSelect(0);
    /* may delay some time here */
    MouseChipSelect(1);
    *p_delta_xy++ = Mouse_SPI_SingleRead(REG_DELTA_Y);
    MouseChipSelect(0);

    return;
}



/**
 * @brief  clear motion status.
 * @param  none.
 * @return none.
 * @retval void.
*/
void MouseClearMotionStatus(void)
{
    /* write any value to clear motion status register */
    MouseChipSelect(1);
    Mouse_SPI_SingleWrite(REG_MOTION_STA, 0x00);
    MouseChipSelect(0);

    return;
}

/**
 * @brief  Mouse sensor initialize.
 * @param  void.
 * @return void.
 * @retval void.
*/
void Mouse_Sensor_Init(void)
{
    uint8_t id[2] = {0xff, 0xff};
    MouseReset();

    /* check sensor id */
    MouseGetProdRevId(&id[0]);
    if (0x2a != id[0] || 0x00 != id[1])
    {
        DBG_BUFFER(MODULE_FRAMEWORK, LEVEL_INFO,  "Read mouse sensor id error! \n", 0);
        return;
    }

    /* change to level trigger, H->L */
    MouseSetMotionInt(LEVEL_LOW_ACTIVE);

    /* set dpi and report length */
    MouseSetDpiAndReportLen(DPI_2000, MOT_REPORT_LEN_8BIT);

//    /* Enable Interrupt (Peripheral, CPU NVIC) */
//    {
//        GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), DISABLE);
//      GPIO_MaskINTConfig(GPIO_GetPin(SENSOR_MOTION_INT), ENABLE);//mask sensor motion interrupt

//        /*  GPIO2~GPIO31 IRQ    */
//        NVIC_ClearPendingIRQ(PERIPHERAL_IRQ);
//        NVIC_SetPriority(PERIPHERAL_IRQ, 0);
//        NVIC_EnableIRQ(PERIPHERAL_IRQ);
//    }
}

/**
 * @brief get mouse motion register status.
 * @param  void.
 * @return motion status of mouse.
*/
uint8_t MouseGetMotionStatus(void)
{
    return Mouse_SPI_SingleRead(REG_MOTION_STA);
}

/**
 * @brief  Mouse motion interrupt. Get sensor data and send to app task.
 * @param  void.
 * @return void.
 * @retval void.
*/
void MouseMotionIntrHandler(void)
{
    //BEE_IO_MSG mouse_xy_msg;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), DISABLE);

    allowedSensor = false;

    uint8_t mosue_xy[2] = {0};
    uint8_t motion_status __attribute__((unused)) = 0;

    /* 1. read motion status */
    motion_status = MouseGetMotionStatus();
    //DBG_BUFFER(MODULE_APP, LEVEL_INFO, " motion_status = 0x%x!", 1, motion_status);
    /* 2. get delta x and delta y */
    MouseGet8bitDeltaXY(&mosue_xy[0]);

    //DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_INFO, "x: 0x%x,  Y:0x%x", 2, (int)mosue_xy[0], (int)mosue_xy[1]);

    mouse_xy_msg.IoType = MOUSE_SENSOR_MSG;
    mouse_xy_msg.parm = (mosue_xy[0] << 8) | (mosue_xy[1]);

    //if (motion_status & 0x80)
    {
        SendMessageFromISR(&mouse_xy_msg, &xHigherPriorityTaskWoken);
    }


    /* 3. clear motion status */
    MouseClearMotionStatus();

    GPIO_ClearINTPendingBit(GPIO_GetPin(SENSOR_MOTION_INT));
    GPIO_INTConfig(GPIO_GetPin(SENSOR_MOTION_INT), ENABLE);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    return;
}

