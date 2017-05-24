/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_i2c.c
* @brief    This file provides all the I2C firmware functions.
* @details
* @author   elliot chen
* @date     2015-04-29
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_i2c.h"
#include "rtl876x_rcc.h"

#define USE_TIMEOUT_MECHANISM		1

/**
  * @brief  Deinitializes the I2Cx peripheral registers to their default reset values.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @retval None
  */
void I2C_DeInit(I2C_TypeDef* I2Cx)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));

    /*Disable I2C IP*/
    if(I2Cx == I2C0)
    {
        RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, DISABLE);
    }
    else
    {
        RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, DISABLE);
    }
}

/**
  * @brief  Initializes the I2Cx peripheral according to the specified 
  *   parameters in the I2C_InitStruct.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  I2C_InitStruct: pointer to a I2C_InitTypeDef structure that
  *   contains the configuration information for the specified I2C peripheral.
  * @retval None
  */
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(IS_I2C_CLOCK_SPEED(I2C_InitStruct->I2C_ClockSpeed));

    /* Disable I2C device before change configuration */
    I2Cx->IC_ENABLE &= ~0x0001; 

    /* ------------------------------ Initialize I2C device ------------------------------*/
    if(I2C_DeviveMode_Master == I2C_InitStruct->I2C_DeviveMode)
    {
        /*configure I2C device mode which can be selected for master or slave*/
        I2Cx->IC_CON = I2C_InitStruct->I2C_DeviveMode |(I2C_InitStruct->I2C_AddressMode << 4)| BIT(5);
        
        /*set target address*/
        I2Cx->IC_TAR = (I2C_InitStruct->I2C_SlaveAddress & 0x3ff)
                        |(I2C_InitStruct->I2C_AddressMode << 12);
        /*set SDA hold time in master mode*/
        I2Cx->IC_SDA_HOLD = 0x01;
        
        #if 1
        /*set Tx empty level*/
        I2Cx->IC_TX_TL = 0;
        /*set Rx full level*/
        I2Cx->IC_RX_TL = 0;
        #endif
    }
    else
    {
		/* set to slave mode */
		I2Cx->IC_CON = (I2C_InitStruct->I2C_DeviveMode) | (I2C_InitStruct->I2C_AddressMode << 3);
        /* set Ack in slave mode */
        I2Cx->IC_ACK_GENERAL_CALL &= I2C_InitStruct->I2C_Ack;
        /* set slave address */
        I2Cx->IC_SAR = I2C_InitStruct->I2C_SlaveAddress;      
        /* set SDA hold time in slave mode */
        I2Cx->IC_SDA_HOLD = 0x07;
        /* set SDA setup time delay only in slave transmitter mode(greater than 2) ,delay time:[(IC_SDA_SETUP - 1) * (ic_clk_period)]*/
        I2Cx->IC_SDA_SETUP = 0x02;
    }    

    /*------------------------------ configure I2C speed ------------------------------*/
    /*Configure I2C speed in standard mode*/
    if (I2C_InitStruct->I2C_ClockSpeed <= 100000)
    {
        I2Cx->IC_CON &= 0xfffb;
        /*configure I2C speed*/
        I2Cx->IC_SS_SCL_HCNT = 20 + (4000*(SystemClock/10000)) / (I2C_InitStruct->I2C_ClockSpeed);
        I2Cx->IC_SS_SCL_LCNT = 20 + (4700*(SystemClock/10000)) / (I2C_InitStruct->I2C_ClockSpeed);
    }
    /*Configure I2C speed in fast mode*/
    else if(I2C_InitStruct->I2C_ClockSpeed <= 400000)
    {
        
        I2Cx->IC_CON &= 0xfffd;
        if(I2C_InitStruct->I2C_ClockSpeed == 200000)
        {
            /*configure I2C speed*/
            I2Cx->IC_FS_SCL_HCNT = 32 + (600*(SystemClock/10000)*4) / (I2C_InitStruct->I2C_ClockSpeed);
            I2Cx->IC_FS_SCL_LCNT = (1300*(SystemClock/10000)*4) / (I2C_InitStruct->I2C_ClockSpeed);
        }
        else
        {
            if(I2C_InitStruct->I2C_ClockSpeed == 400000)
            {
                /*configure I2C speed*/
                I2Cx->IC_FS_SCL_HCNT = 8 + (600*(SystemClock/10000)*4) / (I2C_InitStruct->I2C_ClockSpeed);
                I2Cx->IC_FS_SCL_LCNT = 1 + (1300*(SystemClock/10000)*4) / (I2C_InitStruct->I2C_ClockSpeed);
            }
        }
    }
    /*Configure I2C speed in high mode*/
    else
    {
        if(I2C_InitStruct->I2C_ClockSpeed <= 3400000)
        {
            /*configure I2C speed*/
            I2Cx->IC_HS_SCL_HCNT = 8 + (60*(SystemClock/10000)*30) / (I2C_InitStruct->I2C_ClockSpeed);
            I2Cx->IC_HS_SCL_LCNT = 1 + (120*(SystemClock/10000)*30) / (I2C_InitStruct->I2C_ClockSpeed);
        }
    }   
}

/**
  * @brief  Enables or disables the specified I2C peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  NewState: new state of the I2Cx peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    I2Cx->IC_INTR_MASK = 0;
  
    if (NewState != DISABLE)
    {
        /* Enable the selected I2C peripheral */
        I2Cx->IC_ENABLE |= 0x0001;
    }
    else
    {
        /* Disable the selected I2C peripheral */
        I2Cx->IC_ENABLE &= ~0x0001;
    }
}

#if USE_TIMEOUT_MECHANISM
/**
  * @brief  Send data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
void I2C_MasterWrite(I2C_TypeDef* I2Cx, uint8_t* pBuf, uint16_t len)
{
    uint16_t cnt = 0;
	uint32_t time_out = 0xfffff;
	
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
    /* Write in the DR register the data to be sent */
    for(cnt = 0; cnt < len; cnt++)
    {
        if(cnt >= len - 1)
        {
            /*generate stop signal*/
            I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = *pBuf++;
        }
		/* wait for flag of I2C_FLAG_TFNF */
		time_out = 0xfffff;
        while(((I2Cx->IC_STATUS & (1 << 1)) == 0) &&(time_out!=0))
       	{
			time_out--;
		}
    }   
}
#else
/**
  * @brief  Send data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
void I2C_MasterWrite(I2C_TypeDef* I2Cx, uint8_t* pBuf, uint16_t len)
{
    uint16_t cnt = 0;
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
    /* Write in the DR register the data to be sent */
    for(cnt = 0; cnt < len; cnt++)
    {
        if(cnt >= len - 1)
        {
            /*generate stop signal*/
            I2Cx->IC_DATA_CMD = (*pBuf++) | (1 << 9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = *pBuf++;
        }
        /* Wait if Tx FIFO is full */
        while((I2Cx->IC_STATUS & BIT(1)) == 0);
    }   
}
#endif

#if USE_TIMEOUT_MECHANISM
/**
  * @brief  Read data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
void I2C_MasterRead(I2C_TypeDef* I2Cx, uint8_t* pBuf, uint16_t len)
{
    uint16_t cnt = 0;
    uint32_t reg_value = 0;
	uint32_t time_out = 0xfffff;
    
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
    /* read in the DR register the data to be sent */
    for(cnt = 0; cnt < len; cnt++)
    {
        if(cnt >= len - 1)
        {
            /* generate stop singal */
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0003 << 8);
        }
        else
        {
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0001 << 8);
        }
        
        /* read data */
        if(cnt > 0)
        {
            /* wait for I2C_FLAG_RFNE flag */
			time_out = 0xfffff;
            while(((I2Cx->IC_STATUS & (1 << 3)) == 0) && (time_out!=0))
			{
				time_out--;
            }

			*pBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
        }
    }

    /* wait for I2C_FLAG_RFNE flag */
	time_out = 0xfffff;
    while(((I2Cx->IC_STATUS & (1 << 3)) == 0) && (time_out!=0))
	{
		time_out--;
    }

	*pBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;   
}
#else
/**
  * @brief  Read data in master mode through the I2Cx peripheral.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
void I2C_MasterRead(I2C_TypeDef* I2Cx, uint8_t* pBuf, uint16_t len)
{
    uint16_t cnt = 0;
    uint32_t reg_value = 0;
    
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
    /* read in the DR register the data to be sent */
    for(cnt = 0; cnt < len; cnt++)
    {
        if(cnt >= len - 1)
        {
            /* generate stop singal */
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0003 << 8);
        }
        else
        {
            I2Cx->IC_DATA_CMD = (reg_value) | (0x0001 << 8);
        }
        
        /* read data */
        if(cnt > 0)
        {
            /* wait for I2C_FLAG_RFNE flag */
            while((I2Cx->IC_STATUS & (1 << 3)) == 0);
            *pBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
        }
    }

    /* wait for I2C_FLAG_RFNE flag */
    while((I2Cx->IC_STATUS & (1 << 3)) == 0);
    *pBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;   
}
#endif

#if USE_TIMEOUT_MECHANISM
/**
  * @brief  Sends data and read data in master mode through the I2Cx peripheral.Attention:Read data with time out mechanism.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval Actual length of read data 
  */
uint16_t I2C_RepeatRead(I2C_TypeDef* I2Cx, uint8_t* pWriteBuf, uint16_t Writelen, uint8_t* pReadBuf, uint16_t Readlen)
{
    uint16_t cnt = 0;
    uint32_t reg_value = 0;
    uint16_t rev_len = 0;
    uint32_t time_out = 0xfffff;
    
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
    /*------------------------------ write data section ------------------------------*/
    /* write data in the IC_DATA_CMD register */
    for(cnt = 0; cnt < Writelen; cnt++)
    { 
        I2Cx->IC_DATA_CMD = *pWriteBuf++;
		
        /*wait for I2C_FLAG_TFNF flag that Tx FIFO is not full*/
		time_out = 0xfffff;
        while(((I2Cx->IC_STATUS & BIT(1)) == 0) && (time_out!=0))
       	{
			time_out--;
		}
    }
    
    /*------------------------------ read data section ------------------------------*/
    for(cnt = 0; cnt < Readlen; cnt++)
    {
        if(cnt >= Readlen - 1)
        {
            /*generate stop singal in last byte which to be sent*/
            I2Cx->IC_DATA_CMD = reg_value | BIT(8) |BIT(9);
        }
        else
        {
            I2Cx->IC_DATA_CMD = reg_value | BIT(8);      
        }
        
        /*read data */
        if(cnt > 0)
        {
            /*wait for I2C_FLAG_RFNE flag*/
			time_out = 0xfffff;
            while(((I2Cx->IC_STATUS & BIT(3))==0) && (time_out!=0))
            {
                time_out--;
            }
            if(time_out > 0)
            {
                *pReadBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
                rev_len++;
            }
        }
    }

    /*read data*/
    time_out = 0xfffff;
    while(((I2Cx->IC_STATUS & BIT(3))==0) && (time_out!=0))
    {
        time_out--;
    }
    if(time_out > 0)
    {
        *pReadBuf = (uint8_t)I2Cx->IC_DATA_CMD;
        rev_len++;
    }
    
    return rev_len;
}
#else
/**
  * @brief  Sends data and read data in master mode through the I2Cx peripheral. Attention:Read data without time out mechanism.
  * @param  I2Cx: where x can be 0 or 1 to select the I2C peripheral.
  * @param  Data: Byte to be transmitted..
  * @retval None
  */
uint16_t I2C_RepeatRead(I2C_TypeDef* I2Cx, uint8_t* pWriteBuf, uint16_t Writelen, uint8_t* pReadBuf, uint16_t Readlen)
{
    uint16_t cnt = 0;
    uint32_t reg_value = 0;
	uint16_t rev_len = 0;
    
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
  
    /*------------------------------ write data section ------------------------------*/
    /* write data in the IC_DATA_CMD register */
    for(cnt = 0; cnt < Writelen; cnt++)
    {
        I2Cx->IC_DATA_CMD = *pWriteBuf++;
        /*wait for I2C_FLAG_TFNF flag that Tx FIFO is not full*/
        while((I2Cx->IC_STATUS & BIT(1)) == 0);
    }
    
    /*------------------------------ read data section ------------------------------*/
    for(cnt = 0; cnt < Readlen; cnt++)
    {
        if(cnt >= Readlen - 1)
        {
            /*generate stop singal in last byte which to be sent*/
            I2Cx->IC_DATA_CMD = reg_value | BIT(8) |BIT(9);
        }
        else
        {			
            I2Cx->IC_DATA_CMD = reg_value | BIT(8);      
        }
        
        /*read data */
        if(cnt > 0)
        {
            /*wait for I2C_FLAG_RFNE flag*/
            while((I2Cx->IC_STATUS & BIT(3)) == 0);
            *pReadBuf++ = (uint8_t)I2Cx->IC_DATA_CMD;
			rev_len++;
        }
    }

    /*read data*/
    while((I2Cx->IC_STATUS & BIT(3)) == 0);
    *pReadBuf = (uint8_t)I2Cx->IC_DATA_CMD; 
	rev_len++;
    
    return rev_len;
}
#endif

/**
  * @brief mask the specified I2C interrupt.
  * @param  I2Cx: where x can be 0 or 1
  * @param  I2C_INT
  * This parameter can be one of the following values:
  *     @arg I2C_INT_GEN_CALL: Set only when a General Call address is received and it is acknowledged.                                  
  *     @arg I2C_INT_START_DET: Indicates whether a START or RESTART condition has occurred on the I2C
                              interface regardless of whether I2C is operating in slave or master mode.                              
  *     @arg I2C_INT_STOP_DET:  Indicates whether a STOP condition has occurred on the I2C interface regardless
                              of whether I2C is operating in slave or master mode                                  
  *     @arg I2C_INT_ACTIVITY:  This bit captures I2C activity and stays set until it is cleared.                                
  *     @arg I2C_INT_RX_DONE:   When the I2C is acting as a slave-transmitter, this bit is set to 1 if the
                              master does not acknowledge a transmitted byte. This occurs on the last byte of
                              the transmission, indicating that the transmission is done.                                
  *     @arg I2C_INT_TX_ABRT:   This bit indicates if I2C as an I2C transmitter, is unable to complete the
                              intended actions on the contents of the transmit FIFO.                                   
  *     @arg I2C_INT_RD_REQ:    This bit is set to 1 when acting as a slave and another I2C master
                              is attempting to read data.                                  
  *     @arg I2C_INT_TX_EMPTY:  This bit is set to 1 when the transmit buffer is at or below the threshold value set
                              in the IC_TX_TL register.                                  
  *     @arg I2C_INT_TX_OVER:   Set during transmit if the transmit buffer is filled to IC_TX_BUFFER_DEPTH and
                              the processor attempts to issue another I2C command.                                  
  *     @arg I2C_INT_RX_FULL:   Set when the receive buffer reaches or goes above the RX_TL threshold in the
                              IC_RX_TL register                                  
  *     @arg I2C_INT_RX_OVER:   Set if the receive buffer is completely filled to IC_RX_BUFFER_DEPTH and an
                               additional byte is received from an external I2C device.                                 
  *     @arg I2C_INT_RX_UNDER:   Set if the processor attempts to read the receive buffer when it is empty by reading.
  * @retval None.
  */
void I2C_INTConfig(I2C_TypeDef* I2Cx, uint16_t I2C_INT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
	assert_param(I2C_GET_INT(I2C_INT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    
    if (NewState != DISABLE)
    {
        /* Enable the selected I2C interrupts */
        I2Cx->IC_INTR_MASK |= I2C_INT;
    }
    else
    {
        /* Disable the selected I2C interrupts */
        I2Cx->IC_INTR_MASK &= (uint16_t)~I2C_INT;
    }
}

/**
  * @brief clear the specified I2C interrupt.
  * @param  I2Cx: where x can be 0 or 1
  * @retval None.
  */
void I2C_ClearINTPendingBit(I2C_TypeDef* I2Cx, uint16_t I2C_IT)
{   
    /* Check the parameters */
    assert_param(IS_I2C_ALL_PERIPH(I2Cx));
    assert_param(I2C_GET_INT(I2C_IT));

    switch (I2C_IT)
    {
        case I2C_INT_RX_UNDER:
            I2Cx->IC_CLR_RX_UNDER;
            break;
        case I2C_INT_RX_OVER:
            I2Cx->IC_CLR_RX_OVER;
            break;
        case I2C_INT_TX_OVER:
            I2Cx->IC_CLR_TX_OVER;
            break;
        case I2C_INT_RD_REQ:
            I2Cx->IC_CLR_RD_REQ;
            break;
        case I2C_INT_TX_ABRT:
            I2Cx->IC_CLR_TX_ABRT;
            break;
        case I2C_INT_RX_DONE:
            I2Cx->IC_CLR_RX_DONE;
            break;
        case I2C_INT_ACTIVITY:
            I2Cx->IC_CLR_ACTIVITY;
            break;
        case I2C_INT_STOP_DET:
            I2Cx->IC_CLR_STOP_DET;
            break;
        case I2C_INT_START_DET:
            I2Cx->IC_CLR_START_DET;
            break;
        case I2C_INT_GEN_CALL:
            I2Cx->IC_CLR_GEN_CALL;
            break;
        default:
            break;
    }
}

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

