/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_pinmux.c
* @brief    This file provides all the PINMUX firmware functions.
* @details
* @author   Chuanguo Xue
* @date     2015-03-27
* @version  v0.1
*********************************************************************************************************
*/

#include "rtl876x.h"
#include "rtl_types.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "legacyperiphapi.h"
#include "dlps_platform.h"
#include "trace.h"
#include "rtl876x_uart.h"

/**
  * @brief  Config pin to its corresponding IO function.
  * @param  Pin_Num: pin number. 
  *     This parameter is from P0_0 to P4_5, please refer to rtl876x_pinmux.h "Pin_Number" part.
  * @param  Pin_Func: mean one IO function, please refer to rtl876x_pinmux.h "Pin_Function_Number" part.
  * @retval None
  */
void Pinmux_Config(uint8_t Pin_Num, uint8_t Pin_Func)
{
    uint8_t pinmux_reg_num;
    uint8_t reg_offset;

    pinmux_reg_num = Pin_Num>>2;
    reg_offset = (Pin_Num&0x03)<<3;

    PINMUX->CFG[pinmux_reg_num] = (PINMUX->CFG[pinmux_reg_num]& ~(0xFF<<reg_offset))
                                    | Pin_Func<<reg_offset;
    
    return;
}

/**
  * @brief  Deinit the IO function of one pin.
  * @param  Pin_Num: pin number. 
  *     This parameter is from P0_0 to P4_5, please refer to rtl876x_pinmux.h "Pin_Number" part.
  * @retval None
  */
void Pinmux_Deinit(uint8_t Pin_Num)
{
    uint8_t pinmux_reg_num;

    pinmux_reg_num = Pin_Num>>2; 
    PINMUX->CFG[pinmux_reg_num] &= ~(0xff << ((Pin_Num%4)<<3));
    return;
}

/**
  * @brief  Reset all pin to default value.
  * @param  None. 
  * @note: two SWD pins will also be reset. Please use this function carefully.
  * @retval None
  */
void Pinmux_Reset(void)
{
    uint8_t i;
    
    for(i = 0; i < 10; i++)
    {
        PINMUX->CFG[i] = 0x00;
    }
    
    return;
}

#if defined (RTL8762AX_VB)
/**
  * @brief  config the corresponding pad.
  * @param  Pin_Num: pin number. 
  *     This parameter is from P0_0 to P4_5, please refer to rtl876x_pinmux.h "Pin_Number" part.
  * @param  AON_PAD_MODE: use software mode or pinmux mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_SW_MODE: use software mode.
  *     @arg PAD_PINMUX_MODE: use pinmux mode.
  * @param  AON_PAD_PwrOn: config power of pad.
  *     This parameter can be one of the following values:
  *     @arg PAD_NOT_PWRON: shutdown power of pad.
  *     @arg PAD_IS_PWRON: enable power of pad.
  * @param  AON_PAD_Pull: config pad pull mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_PULL_NONE: no pull.
  *     @arg PAD_PULL_UP: pull this pin up.
  *     @arg PAD_PULL_DOWN: pull thi pin down.
  * @param  AON_PAD_E: config pad out put function.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_DISABLE: disable pin output.
  *     @arg PAD_OUT_ENABLE: enable pad output.
  * @param  AON_PAD_O: config pin output level.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_LOW: pad output low.
  *     @arg PAD_OUT_HIGH: pad output high.
  * @retval None
  */
void Pad_Config(uint8_t Pin_Num,
                BOOL AON_PAD_Mode,
                BOOL AON_PAD_PwrOn,
                BOOL AON_PAD_Pull,
                BOOL AON_PAD_E,
                BOOL AON_PAD_O)
{
    uint8_t addr_offset;
    uint8_t bit_pos;
    uint8_t bit_pos_comp;
    uint8_t value8;

    addr_offset = Pin_Num >> 3;
    bit_pos = BIT(Pin_Num & 0x07);
    bit_pos_comp = ~bit_pos;

    value8 = bee_aon_gp_read(REG_AON_PAD_S_ADDRESS + addr_offset);
    value8 &= bit_pos_comp;
    if (AON_PAD_Mode == PAD_PINMUX_MODE)
        value8 |= bit_pos;
    bee_aon_gp_write(REG_AON_PAD_S_ADDRESS + addr_offset, value8);

    value8 = bee_aon_gp_read(REG_AON_PAD_PWRON_ADDRESS + addr_offset);
    value8 &= bit_pos_comp;
    if (AON_PAD_PwrOn == PAD_IS_PWRON)
        value8 |= bit_pos;
    bee_aon_gp_write(REG_AON_PAD_PWRON_ADDRESS + addr_offset, value8);

    if (AON_PAD_Pull == PAD_PULL_DOWN)
    {
        if((Pin_Num == 8) || (Pin_Num == 9))
        {
            value8 = bee_aon_gp_read(REG_AON_PAD_PU_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + addr_offset, value8);

            value8 = bee_aon_gp_read(REG_AON_PAD_PD_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            value8 |= bit_pos;
            bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + addr_offset, value8);
        }
        else
        {
            value8 = bee_aon_gp_read(REG_AON_PAD_PD_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + addr_offset, value8);

            value8 = bee_aon_gp_read(REG_AON_PAD_PU_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            value8 |= bit_pos;
            bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + addr_offset, value8);
        }
        
    }
    else if (AON_PAD_Pull == PAD_PULL_UP)
    {
        if((Pin_Num == 8) || (Pin_Num == 9))
        {
            value8 = bee_aon_gp_read(REG_AON_PAD_PD_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + addr_offset, value8);

            value8 = bee_aon_gp_read(REG_AON_PAD_PU_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            value8 |= bit_pos;
            bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + addr_offset, value8);
        }
        else
        {
            value8 = bee_aon_gp_read(REG_AON_PAD_PU_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + addr_offset, value8);

            value8 = bee_aon_gp_read(REG_AON_PAD_PD_ADDRESS + addr_offset);
            value8 &= bit_pos_comp;
            value8 |= bit_pos;
            bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + addr_offset, value8);
        }
    }
    else
    {
        value8 = bee_aon_gp_read(REG_AON_PAD_PD_ADDRESS + addr_offset);
        value8 &= bit_pos_comp;
        bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + addr_offset, value8);

        value8 = bee_aon_gp_read(REG_AON_PAD_PU_ADDRESS + addr_offset);
        value8 &= bit_pos_comp;
        bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + addr_offset, value8);
    }

    value8 = bee_aon_gp_read(REG_AON_PAD_E_ADDRESS + addr_offset);
    value8 &= bit_pos_comp;
    if (AON_PAD_E == PAD_OUT_ENABLE)
        value8 |= bit_pos;
    bee_aon_gp_write(REG_AON_PAD_E_ADDRESS + addr_offset, value8);

    value8 = bee_aon_gp_read(REG_PAD_O_ADDRESS + addr_offset);
    value8 &= bit_pos_comp;
    if (AON_PAD_O == PAD_OUT_HIGH)
        value8 |= bit_pos;
    bee_aon_gp_write(REG_PAD_O_ADDRESS + addr_offset, value8); 
}


/**
  * @brief  Set all pins to the default state.
  * @param  void. 
  * @retval void.
  */
void All_Pad_Config_Default(void)
{
	uint8_t addr_offset;
	uint8_t value8;
	
    for (addr_offset = 0; addr_offset < 4; addr_offset++)
    {
        bee_aon_gp_write(REG_AON_PAD_S_ADDRESS + addr_offset, 0);  //PAD_SW_MODE
		bee_aon_gp_write(REG_AON_PAD_PWRON_ADDRESS + addr_offset, 0xFF);  //PAD_IS_PWRON
		bee_aon_gp_write(REG_AON_PAD_E_ADDRESS + addr_offset, 0);  //PAD_OUT_DISABLE
		bee_aon_gp_write(REG_PAD_O_ADDRESS + addr_offset, 0);  //PAD_OUT_LOW
    }
	
	value8 = bee_aon_gp_read(REG_AON_PAD_S_ADDRESS + 4);
	value8 &= 0xC0;
	bee_aon_gp_write(REG_AON_PAD_S_ADDRESS + 4, value8);  //PAD_SW_MODE
	
	value8 = bee_aon_gp_read(REG_AON_PAD_PWRON_ADDRESS + 4);
	value8 |= 0x3F;
	bee_aon_gp_write(REG_AON_PAD_PWRON_ADDRESS + 4, value8);  //PAD_IS_PWRON
	
	value8 = bee_aon_gp_read(REG_AON_PAD_E_ADDRESS + 4);
	value8 &= 0xC0;
	bee_aon_gp_write(REG_AON_PAD_E_ADDRESS + 4, value8);  //PAD_OUT_DISABLE
	
	value8 = bee_aon_gp_read(REG_PAD_O_ADDRESS + 4);
	value8 &= 0xC0;
	bee_aon_gp_write(REG_PAD_O_ADDRESS + 4, value8);  //PAD_OUT_LOW
	
    //Pull Down
	bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + 0, 0);
	bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + 1, 0x3);
	bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + 2, 0);
	bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + 3, 0);
	value8 = bee_aon_gp_read(REG_AON_PAD_PD_ADDRESS + 4);
	value8 &= 0xC0;
	bee_aon_gp_write(REG_AON_PAD_PD_ADDRESS + 4, value8);
	
	bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + 0, 0xFF);
	bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + 1, 0xFC);
	bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + 2, 0xFF);
	bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + 3, 0xFF);
	value8 = bee_aon_gp_read(REG_AON_PAD_PU_ADDRESS + 4);
	value8 |= 0x3F;
	bee_aon_gp_write(REG_AON_PAD_PU_ADDRESS + 4, value8);
	
	//Disable Every WakeUp Pin
	value8 = bee_aon_gp_read(REG_PAD_WKEN_ADDRESS + 1);
	value8 &= 0xF8;
	bee_aon_gp_write(REG_PAD_WKEN_ADDRESS + 1, value8);//GPIO_WKEN[10:8]

	value8 = bee_aon_gp_read(REG_PAD_WKEN_ADDRESS + 2);
	value8 &= 0x0C;
	bee_aon_gp_write(REG_PAD_WKEN_ADDRESS + 2, value8);//GPIO_WKEN[17:16] GPIO_WKEN[23:20]

	value8 = bee_aon_gp_read(REG_PAD_WKEN_ADDRESS + 3);
	value8 &= 0xF0;
	bee_aon_gp_write(REG_PAD_WKEN_ADDRESS + 3, value8);//GPIO_WKEN[27:24]

	value8 = bee_aon_gp_read(REG_PAD_WKEN_ADDRESS + 4);
	value8 &= 0xF7;
	bee_aon_gp_write(REG_PAD_WKEN_ADDRESS + 4, value8);//GPIO_WKEN[35]

#ifndef RELEASE_VERSION
    //SET P0_3 as pinmux mode
    Pad_Config(P0_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
#else
    //SET P0_3 as sw mode, will not print any log
    Pad_Config(P0_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif

	if((*(uint8_t*)(0x20000134)) & BIT3)
	{
		Pad_Config(P1_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_LOW);
		Pad_Config(P1_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_LOW);
	}
}

/**
  * @brief  Set Log and SWD pins to SW mode.
  * @param  void. 
  * @retval void.
  */
void Log_SWD_DLPS_Enter(void)
{
	if((*(uint8_t*)(0x20000134)) & BIT3)
	{
		Pad_Config(P1_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
		Pad_Config(P1_1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
	}
#ifndef RELEASE_VERSION
	Pad_Config(P0_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif
}

/**
  * @brief  Set Log and SWD pins to PINMUX mode.
  * @param  void. 
  * @retval void.
  */
void Log_SWD_DLPS_Exit(void)
{
#ifndef RELEASE_VERSION
	PERIPH->PERI_CLK_CTRL |= (SYSBLK_ACTCK_LOGUART_EN_Msk | SYSBLK_SLPCK_LOGUART_EN_Msk);
	Pad_Config(P0_3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif
	if((*(uint8_t*)(0x20000134)) & BIT3)
	{
		Pad_Config(P1_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_LOW);
		Pad_Config(P1_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_LOW);
	}
}

#endif

#if defined (RTL8762AX_VB)
/**
  * @brief  Enable pin wakeup function.
  * @param  Pin_Num: pin number. 
  *     This parameter is from P0_0 to P4_5, please refer to rtl876x_pinmux.h "Pin_Number" part.
  * @param  Polarity: 1--use high level wakeup, 0-- use low level wakeup.
  * @retval None
  */
void System_WakeUp_Pin_Enable(uint8_t Pin_Num, uint8_t Polarity)
{       
    uint8_t addr_offset;
    uint8_t bit_pos;
    uint8_t bit_pos_comp;
    uint8_t value8;

    if((Pin_Num==0) ||
       (Pin_Num==1) || 
       (Pin_Num==2) || 
       //(Pin_Num==3) || 
       //(Pin_Num==10) || 
       (Pin_Num==16) || 
       (Pin_Num==17) || 
       (Pin_Num==20) || 
       (Pin_Num==21) || 
       (Pin_Num==24) || 
       (Pin_Num==25) || 
       (Pin_Num==26) ||
       (Pin_Num==32) || 
       (Pin_Num==33) || 
       (Pin_Num==34) || 
       (Pin_Num==35))
    {//wakeup pin
        if((Pin_Num==0)||(Pin_Num==1))
        {
            Pin_Num += 9;
        }
        else if((Pin_Num==2)||(Pin_Num==3))
        {
            Pin_Num += 6;
        }
        else if((Pin_Num==32)||(Pin_Num==33))
        {
            Pin_Num -= 10;
        }
        else if(Pin_Num==34)
        {
            Pin_Num -=7;
        }
        
		System_WakeUp_Intr_Disable();
		
        addr_offset = Pin_Num>>3;
        bit_pos = BIT(Pin_Num&0x07);
        bit_pos_comp = ~bit_pos;
        
        value8 = bee_aon_gp_read(REG_PAD_WKEN_ADDRESS + addr_offset);
        value8 |= bit_pos;
        bee_aon_gp_write(REG_PAD_WKEN_ADDRESS + addr_offset, value8);
        
        value8 = bee_aon_gp_read(REG_PAD_WKPOL_ADDRESS + addr_offset);
        if(Polarity)
            value8 &= bit_pos_comp;
        else
            value8 |= bit_pos;
        bee_aon_gp_write(REG_PAD_WKPOL_ADDRESS + addr_offset, value8);

    }
    else
    {
        //The pin selected can't be configured as wakeup pin.
    }
}

#endif

#if defined (RTL8762AX_VB)
/**
  * @brief  Disable pin wakeup function.
  * @param  Pin_Num: pin number. 
  *     This parameter is from P0_0 to P4_5, please refer to rtl876x_pinmux.h "Pin_Number" part.
  * @retval None
  */
void System_WakeUp_Pin_Disable(uint8_t Pin_Num)
{
    uint8_t addr_offset;
    uint8_t bit_pos;
    uint8_t bit_pos_comp;
    uint8_t value8;
    
    if((Pin_Num==0) ||
       (Pin_Num==1) || 
       (Pin_Num==2) || 
       //(Pin_Num==3) || 
       //(Pin_Num==10) || 
       (Pin_Num==16) || 
       (Pin_Num==17) || 
       (Pin_Num==20) || 
       (Pin_Num==21) || 
       (Pin_Num==24) || 
       (Pin_Num==25) || 
       (Pin_Num==26) ||
       (Pin_Num==32) || 
       (Pin_Num==33) || 
       (Pin_Num==34) || 
       (Pin_Num==35))
    {//wakeup pin
        if((Pin_Num==0)||(Pin_Num==1))
        {
            Pin_Num += 9;
        }
        else if((Pin_Num==2)||(Pin_Num==3))
        {
            Pin_Num += 6;
        }
        else if((Pin_Num==32)||(Pin_Num==33))
        {
            Pin_Num -= 10;
        }
        else if(Pin_Num==34)
        {
            Pin_Num -=7;
        }
        
        addr_offset = Pin_Num>>3;
        bit_pos = BIT(Pin_Num&0x07);
        bit_pos_comp = ~bit_pos;
        
        value8 = bee_aon_gp_read(REG_PAD_WKEN_ADDRESS + addr_offset);
        value8 &= bit_pos_comp;
        bee_aon_gp_write(REG_PAD_WKEN_ADDRESS + addr_offset, value8);
        


        value8 = bee_aon_gp_read(REG_PAD_WKPOL_ADDRESS + addr_offset);
        value8 &= bit_pos_comp;
        bee_aon_gp_write(REG_PAD_WKPOL_ADDRESS + addr_offset, value8);
        

    }
    else
    {
        //The pin selected can't be configured as wakeup pin.
    }
}

#endif
