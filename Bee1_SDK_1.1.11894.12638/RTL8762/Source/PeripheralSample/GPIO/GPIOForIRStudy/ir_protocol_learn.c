/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file      ir_protocol_learn.c
* @brief    This file provides ir protocol learn driver by GPIO and timer.
* @details
* @author elliot_chen
* @date     2016-03-18
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "ir_protocol_learn.h"

/* Globals------------------------------------------------------------------- */
IR_PacketTypeDef IR_Packet;
uint32_t LoopQueue[LOOP_QUEUE_MAX_SIZE] SRAM_OFF_BD_DATA_SECTION;

/**
  * @brief  Enables or disables the timer 0 interrupt of lower stack.
  * @param  bEnable: new state of the timer 0 interrupt state. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval execution state.
  * @retval TRUE successfully
  * @retval FALSE failed
  */
bool blueAPI_IOCTL_LowerStackTimer0Enable(uint8_t bEnable)
{
    TApiBufUserDefined ApiBufUserDefined;
    ApiBufUserDefined.Type = API_TYPE_FMC_IOCTL;
    ApiBufUserDefined.p.fmcIoctl.cmd = 0xB;
    ApiBufUserDefined.p.fmcIoctl.p1 = 1;
    ApiBufUserDefined.p.fmcIoctl.p2 = (uint32_t)&bEnable;
    return blueAPI_UserDefined(&ApiBufUserDefined);     
}

/**
  * @brief  Initializes loop queue to their default reset values.
  * @param  pIR_Packet: point to IR packet struct.
  * @retval None
  */
void LoopQueueInit(IR_PacketTypeDef *pIR_Packet)
{
    uint8_t i = 0;

    pIR_Packet->ReadIndex      = 0;                         /**< index of read queue */
    pIR_Packet->WriteIndex     = 0;                         /**< index of write queue */

    pIR_Packet->SquareWaveNum  = 0;                         /**< record square wave number in high voltage level */
    pIR_Packet->IsHighLevel    = false;                     /**< record decode voltage level */
    pIR_Packet->OverFLow       = false;                     /**< record loop queue over folw or not */
    pIR_Packet->DetectTimeNum  = 0;                         /**< record IR data number */
    pIR_Packet->frequencyNum   = 0;
	pIR_Packet->rxEndFlag      = false;
	pIR_Packet->learn_time_out = false;
	pIR_Packet->rising_status  = true;

    for (i = 0; i < DETECT_WAVE_MAX_SIZE; i++)
    {
        pIR_Packet->DetectTime[i] = 0;                      /**< record total time initialization */
    }

    for (i = 0; i < DETECT_WAVE_MAX_SIZE / 2 + 1; i++)
    {
#if CALCULATE_FREQUENCY
        pIR_Packet->wave_frequency[i] = 0;                  /**< record IR frequency initialization */
#else
        pIR_Packet->high_wave_time[i] = 0;
		pIR_Packet->high_wave_count[i] = 0;  
#endif
    }
}

#if 0
/**
  * @brief  check loop queue if full or not.
  * @param  pIR_Packet: point to IR packet struct.
  * @retval The new state of loop queue (full:true, not full:false).
  */
__STATIC_INLINE BOOL LoopQueueIsFull(IR_PacketTypeDef *pIR_Packet)
{
    if (((pIR_Packet->WriteIndex + 1)&QUEUE_CAPABILITY) == pIR_Packet->ReadIndex)
    {
#if PRINT_LOG
        DBG_BUFFER(MODULE_DRIVERTASK, LEVEL_ERROR, "LoopQueue is overflow!", 0);
#endif
        return true;
    }
    return false;
}
#endif

/**
  * @brief  check loop queue if empty or not.
  * @param  pIR_Packet: point to IR packet struct.
  * @retval The new state of loop queue (empty:true, not empty:false).
  */
BOOL LoopQueueIsEmpty(IR_PacketTypeDef *pIR_Packet)
{
    return (pIR_Packet->WriteIndex == pIR_Packet->ReadIndex);
}

/**
  * @brief  learing a specific IR waveform.
  * @param  pIR_Packet: point to IR packet struct.
  * @retval The decoded data number of IR learned waveform.
  */
uint8_t IR_DecodePacket(IR_PacketTypeDef *pIR_Packet)
{
    uint32_t current_time = 0;
    uint32_t last_time = 0;
    uint32_t time_interval = 0;
    uint16_t idx = 0;

    /* Wait for first valid data */
    while (1)
    {
        if (pIR_Packet->WriteIndex - pIR_Packet->ReadIndex >= 2)
        {
		    /* Start to calculate timeout period */
			TIM_TIME_OUT->ControlReg |= BIT(0);

			/* Enable interrupt */
			TIM_TIME_OUT->ControlReg &= ~(BIT(2));
			*((volatile uint32_t *)0x4000800CUL) |= 0x03E0;	

            /* read the second data */
            last_time = LoopQueue[pIR_Packet->ReadIndex++];
            break;
        }
    }

    /* Begin to learn waveform */
    while (1)
    {
        if (pIR_Packet->ReadIndex != pIR_Packet->WriteIndex)
        {
            current_time = LoopQueue[pIR_Packet->ReadIndex++];
            pIR_Packet->ReadIndex &= QUEUE_CAPABILITY;

			/* Data process */
            if (current_time <= last_time)
            {
                time_interval = last_time - current_time;
            }
            else
            {
                /* If timer is overflow */
                time_interval = 0xffffffff - current_time;
                time_interval += (1 + last_time);
            }

            /* Record total time of high wave */
            if ((time_interval > TIME_LOWEST_VALUE) && (time_interval < TIME_HIGHEST_VALUE))
            {
                pIR_Packet->DetectTime[idx] += time_interval;
                pIR_Packet->IsHighLevel = true;
                pIR_Packet->SquareWaveNum++;

                /* Record IR data number */
                pIR_Packet->DetectTimeNum = idx + 1;
            }
			
            /* Wave from high to low */
            else if(time_interval > TIME_LOW_VOLTAGE_VALUE)
            {    
                if (pIR_Packet->IsHighLevel == true)
                {
                	/* Calculate IR frequency based on time value of high waveform */
#if CALCULATE_FREQUENCY
					
#if SWITCH_EDGE_POLARITY
					pIR_Packet->wave_frequency[pIR_Packet->frequencyNum++] = (pIR_Packet->SquareWaveNum * 5000.0) / pIR_Packet->DetectTime[idx];
#else
                	pIR_Packet->wave_frequency[pIR_Packet->frequencyNum++] = (pIR_Packet->SquareWaveNum * 10000.0) / pIR_Packet->DetectTime[idx];
#endif

#else
					pIR_Packet->high_wave_count[pIR_Packet->frequencyNum] =	pIR_Packet->SquareWaveNum;
					pIR_Packet->high_wave_time[pIR_Packet->frequencyNum++] = pIR_Packet->DetectTime[idx];
#endif

                    pIR_Packet->SquareWaveNum = 0;

                    /* pointer to next buffer to store data */
                    idx++;
					
					/* Record IR data number */
	                pIR_Packet->DetectTimeNum = idx + 1;
					/* Store value of low waveform */
	                pIR_Packet->DetectTime[idx++] = time_interval;
	                pIR_Packet->IsHighLevel = false;
                }
				else
				{
#if PRINT_LOG
					DBG_BUFFER(MODULE_APP,LEVEL_INFO,"Warning: Pelase consider resetting the trigger threshold!", 0);
#endif
				} 
            }
			
            /*record the lastest time for next waveform decoder */
            last_time = current_time;
        }

		/* Check the maximum number of learning data */
		if(idx >= DETECT_WAVE_MAX_SIZE)
		{
			break;
		}
		
        /* Stop IR learning */
		if(pIR_Packet->learn_time_out == true)
		{
			if (pIR_Packet->ReadIndex == pIR_Packet->WriteIndex)
            {
            	pIR_Packet->learn_time_out = false;
                break;
            }
		}
    }
	
    return pIR_Packet->DetectTimeNum;
}

/**
  * @brief  learing a specific IR waveform frequency.
  * @param  pIR_Packet: point to IR packet struct.
  * @retval The decoded waveform frequency of IR learned waveform.
  */
float IR_DecodeFrequency(IR_PacketTypeDef *pIR_Packet)
{
    uint8_t i = 0;
    float frequency = 0;

#if CALCULATE_FREQUENCY
    /* frequency calculation*/
    for (i = 0; i < (pIR_Packet->frequencyNum); i++)
    {
        frequency += pIR_Packet->wave_frequency[i];
    }
#else
	for (i = 0; i < (pIR_Packet->frequencyNum); i++)
	{
#if SWITCH_EDGE_POLARITY
		frequency += (pIR_Packet->high_wave_count[i] * 5000.0) / pIR_Packet->high_wave_time[i];
#else

		frequency += (pIR_Packet->high_wave_count[i] * 10000.0) / pIR_Packet->high_wave_time[i];
#endif
	}
#endif

    frequency = frequency / pIR_Packet->frequencyNum;

#if CORRECT_FREQ_BY_SOFTWARE
    /* frequency correction by software */
    if (ABS_FREQUENCY(frequency, 38.0) <= 1.0)
    {
        frequency = 38.0;
    }
    else if (ABS_FREQUENCY(frequency, 36.0) <= 1.0)
    {
        frequency = 36.0;
    }
    else if (ABS_FREQUENCY(frequency, 40.0) <= 1.0)
    {
        frequency = 40.0;
    }
    else
    {
        if (ABS_FREQUENCY(frequency, 56.0) <= 1.0)
        {
            frequency = 56.0;
        }
    }
#endif
    return frequency;
}

/**
  * @brief close the IR learn function.
  * @param  none.
  * @retval None
  */
void IR_Protocol_Learn_DeInit(void)
{
    TIM_Cmd(TIM_LEARN_HIGH, DISABLE);
	TIM_Cmd(TIM_TIME_OUT, DISABLE);
}

/**
  * @brief  Initializes some peripheral which used to learn waveform.
  * @param  none.
  * @retval None
  */
void IR_Protocol_Learn_Init(void)
{
    /* Turn on GPIO and TIM clock */
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);

    /* Configure to GPIO function */
    Pinmux_Config(IR_LEARN_PIN, GPIO_FUN);
    Pad_Config(IR_LEARN_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* TIM which for learning IR wave */
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_InitStruct.TIM_Period = 0xffffffff;
    TIM_TimeBaseInit(TIM_LEARN_HIGH, &TIM_InitStruct);
    TIM_Cmd(TIM_LEARN_HIGH, ENABLE);

	/* TIM which for calculating time for jumping out of IR learn mode */
    TIM_InitStruct.TIM_ClockSrc = TIM_CLOCK_10MHZ;
    TIM_InitStruct.TIM_Mode = TIM_Mode_UserDefine;
	TIM_InitStruct.TIM_Period = IR_LEARN_TIME_OUT;
    TIM_TimeBaseInit(TIM_TIME_OUT, &TIM_InitStruct);

	/* GPIO init */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin  = GPIO_IR_LEARN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_ITCmd = ENABLE;
    GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
    GPIO_InitStruct.GPIO_ITPolarity = GPIO_IR_INT_POLARITY;
    GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;  
    GPIO_Init(&GPIO_InitStruct);
	/* Enable IR learn interrupt */
	GPIO_INTConfig(GPIO_IR_LEARN, ENABLE);

	/* Configure NVIC of GPIO */
	NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GPIO_IR_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

	/* Configure TIM NVIC */	
	NVIC_InitStruct.NVIC_IRQChannel = IR_TIME_OUT_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/**
  * @brief  store learn waveform to a memory.
  * @param  pstore_address: store address.
  * @param  len: store data length.
  * @retval execute result.
  */
BOOL IR_ReturnLearnWaveForm(uint16_t *pstore_address, uint8_t len)
{
    uint16_t index = 0;
    float learn_frequency = 0;

    /* IR learned data process */
    learn_frequency = IR_DecodeFrequency(&IR_Packet);

    for (index = 0; index < len; index++)
    {
        if (index % 2 == 0)
        {
            *(pstore_address + index) = (IR_Packet.DetectTime[index] * (learn_frequency / 10000.0)) + 0x8000;
        }
        else
        {
            *(pstore_address + index) = IR_Packet.DetectTime[index] * (learn_frequency / 10000.0);
        }
    }

    return true;
}

#if ENABLE_TX_TEST
/**
  * @brief  send a specific IR waveform.
  * @param  pIR_Packet: point to IR packet struct.
  * @retval The new state of send result.
  */
BOOL IR_SendLearnWaveForm(IR_PacketTypeDef *pIR_Packet)
{
    uint16_t sendBuf[DETECT_WAVE_MAX_SIZE] = {0};
    uint16_t index = 0;
    uint16_t txCount = 0;
    float frequency = 0;
   
    frequency = IR_DecodeFrequency(pIR_Packet);

	/* IR learned data process */
    for (index = 0; index < pIR_Packet->DetectTimeNum; index++)
    {
        if (index % 2 == 0)
        {
            sendBuf[index] = (pIR_Packet->DetectTime[index] * (frequency / 10000)) + 0x8000;
        }
        else
        {
            sendBuf[index] = pIR_Packet->DetectTime[index] * (frequency / 10000);
        }
    }

    /* send a learned IR waveform */
    while (txCount < pIR_Packet->DetectTimeNum)
    {
        if ((pIR_Packet->DetectTimeNum - txCount) >= IR_TX_FIFO_SIZE)
        {
            IR_SendData(IR, sendBuf + txCount, 31);
            /* wait tx end */
            while (IR_GetFlagState(IR, IR_FLAG_TX_FIFO_EMPTY) != SET);
            txCount += 31;
        }
        else
        {
            IR_SendData(IR, sendBuf + txCount, pIR_Packet->DetectTimeNum - txCount);
            /* wait tx end */
            while (IR_GetFlagState(IR, IR_FLAG_TX_FIFO_EMPTY) != SET);
            txCount += (pIR_Packet->DetectTimeNum - txCount);
        }
    }
    /* stop tx */
    IR_StopTx(IR);

#if PRINT_LOG
	/* Print IR learn data */
	DBG_BUFFER(MODULE_APP, LEVEL_INFO, "Print IR learn data: ", 0);
	for (index = 0; index < pIR_Packet->DetectTimeNum; index++)
	{
		DBG_BUFFER(MODULE_APP, LEVEL_INFO, "%d: %d", 2, index, pIR_Packet->DetectTime[index]);
	}
#endif
    return true;
}

/**
  * @brief  Initializes some peripheral which used to send learned waveform.
  * @param  none.
  * @retval None
  */
void IR_SendInit(uint16_t frequency)
{
#if ENABLE_TX_TEST
    /* Pinmux and PAD config */
    Pinmux_Config(IR_TX_PIN, IRDA_TX);
    Pad_Config(IR_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif
    IR_InitTypeDef irInitStruct;
    IR_StructInit(&irInitStruct);
    irInitStruct.frequency = frequency;
    IR_Init(IR, &irInitStruct);

    IR_Cmd(IR, IR_MODE_TX, ENABLE);
    /* set tx threshold */
    IR_SetTxThreshold(IR, 31);
}
#endif

/**
  * @brief  this API is for user to see IR learn procedures, user can Refer to the following procedures.
  * @param  none.
  * @retval None
  */
void IR_User_Sample_Code(void)
{
    uint16_t sendBuf[DETECT_WAVE_MAX_SIZE] = {0};
    uint8_t learned_data_size = 0;
    /*set expected learned IR data size.For NEC protocol,this value is 66 */
    const uint8_t expect_data_size = 66;
	/* Record frequency of waveform */
	uint16_t frequency = 0;

    //while (1)
    {
    	blueAPI_IOCTL_LowerStackTimer0Enable(DISABLE);
		
        /* Data struct initialization */
        LoopQueueInit(&IR_Packet);

        /* Peripheral initialization */
        IR_Protocol_Learn_Init();

        /* Learn IR waveform, return actual learned data number */
        learned_data_size = IR_DecodePacket(&IR_Packet);

		/* Calculate frequency */
		frequency = IR_DecodeFrequency(&IR_Packet);
#if PRINT_LOG
		DBG_BUFFER(MODULE_APP,LEVEL_INFO,"frequency = %d!", 1, frequency);
#endif


        /* Store learned result */
        if (learned_data_size >= expect_data_size)
        {
            /* Store IR learned data */
            IR_ReturnLearnWaveForm(sendBuf, learned_data_size);

            //user code, call flash store API to store IR data
        }
        else
        {
            /* error handle code */
            //have not study expected data size
        }

		/* close IR learn function */
		IR_Protocol_Learn_DeInit();

		blueAPI_IOCTL_LowerStackTimer0Enable(ENABLE);
    }
}

/**
  * @brief  this API is for user to test IR learned result.
  * @param  none.
  * @retval None
  */
void IR_Waveform_Learn_Test_Code(void)
{
    uint16_t detectTimeNum = 0;
    uint16_t frequency = 0;

    uint32_t i = 0;

    /* loop to open IR learn function and send IR learned data */
    //while (1)
    {   
    	blueAPI_IOCTL_LowerStackTimer0Enable(DISABLE);
        LoopQueueInit(&IR_Packet);
        IR_Protocol_Learn_Init();
			
#if PRINT_LOG
		DBG_BUFFER(MODULE_APP,LEVEL_INFO,"Start IR learn!", 0);
#endif
        /* learn IR waveform, return actual learned data number */
        detectTimeNum = IR_DecodePacket(&IR_Packet);

		/* Calculate frequency */
		frequency = IR_DecodeFrequency(&IR_Packet);

#if PRINT_LOG
		DBG_BUFFER(MODULE_APP,LEVEL_INFO,"Read IR data size = %d, frequency = %d, OverFlow = %d!", 3, detectTimeNum, frequency, IR_Packet.OverFLow);
#endif
		
#if PRINT_LOG
		for (i = 0; i < IR_Packet.DetectTimeNum; i++)
		{
			DBG_BUFFER(MODULE_APP, LEVEL_INFO, "%d: %d", 2, i, IR_Packet.DetectTime[i]);
		}
#endif

#if ENABLE_TX_TEST
		/* delay some time to send IR learned waveform */
        for (i = 0; i < 0xfffff; i++){;}
		
        /* begin to send learned IR data */
        IR_SendInit(frequency);

        /* send IR data */
        IR_SendLearnWaveForm(&IR_Packet);
#endif

        /* close IR learn function */
        IR_Protocol_Learn_DeInit();
		blueAPI_IOCTL_LowerStackTimer0Enable(ENABLE);
    }
}

void GPIO_IR_IntrHandler(void)
{
	uint32_t value = TIM_LEARN_HIGH->CurrentValue;

	/* Mask GPIO interrupt */
	GPIO->INTMASK |= GPIO_IR_LEARN;
    
    /* Check loop queue is full or not */
    if(((IR_Packet.WriteIndex + 1) & QUEUE_CAPABILITY) != IR_Packet.ReadIndex)
    {
        /* Store current timer value */
        LoopQueue[IR_Packet.WriteIndex++] = value;
        IR_Packet.WriteIndex &= QUEUE_CAPABILITY;
    }
    else
    {
        /* Loop queue overflow*/
        IR_Packet.OverFLow = true;
    }
	
#if SWITCH_EDGE_POLARITY	
	/* Change GPIO Interrupt Polarity */
	GPIO->INTPOLARITY &= ~(GPIO_IR_LEARN);

    if(IR_Packet.rising_status)
    {
        IR_Packet.rising_status = FALSE;
    }
    else
    {
        /* Configure rising trigger */
		GPIO->INTPOLARITY |= GPIO_IR_LEARN;
        IR_Packet.rising_status = TRUE;
    }
#endif

	/* Clear interrupt */
	GPIO->INTCLR = GPIO_IR_LEARN; 
	/* Unmask interrupt */
    GPIO->INTMASK &= ~(GPIO_IR_LEARN);
}

/**
  * @brief  time interrupt handle which control to quit IR learn mode.
  * @param  None.
  * @retval None.
  */
void TIME_OUT_IntrHandler(void)
{
	/* Stop to calculate timeout period */
	TIM_Cmd(TIM_TIME_OUT, DISABLE);
	TIM_INTConfig(TIM_TIME_OUT,DISABLE);

	/*  Set time out flag */
	IR_Packet.learn_time_out = true;

	/* Disable GPIO interrupt */
	GPIO_INTConfig(GPIO_IR_LEARN, DISABLE);
	/* Enable NVIC */
	NVIC_DisableIRQ(GPIO_IR_IRQ);
	
	TIM_ClearINT(TIM_TIME_OUT);
}

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/

