/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_io_dlps.c
* @brief    This file provides all the IO DLPS control firmware functions.
* @details
* @author   tifnan_ge
* @date     2015-05-18
* @version  v0.1
*********************************************************************************************************
*/
#include "rtl876x.h"
#include "rtl_types.h"
#include "rtl876x_pinmux.h"
#include "legacyperiphapi.h"
#include "rtl876x_rcc.h"
#include "dlps_platform.h"
#include "trace.h"
#include "rtl876x_io_dlps.h"
#include "board.h"

/********************************************** ********************************************************/
/**************************************** [CPU & PINMUX DLPS]*******************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_IO_DRIVER_DLPS

__STATIC_INLINE void CPU_DLPS_Enter(void);
__STATIC_INLINE void CPU_DLPS_Exit(void);
__STATIC_INLINE void Pinmux_DLPS_Enter(void);
__STATIC_INLINE void Pinmux_DLPS_Exit(void);

uint32_t CPU_StoreReg[10];         /*  This array should be placed in RAM ON/Buffer ON.    */
uint32_t Pinmux_StoreReg[10];      /*  This array should be placed in RAM ON/Buffer ON.    */
uint32_t Periph_StoreReg[2];       /*  This array should be placed in RAM ON/Buffer ON.    */

#endif 

/********************************************** ********************************************************/
/**************************************** [GPIO DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_GPIO_DLPS

__STATIC_INLINE void GPIO_DLPS_Enter(void);
__STATIC_INLINE void GPIO_DLPS_Exit(void);

uint32_t GPIO_StoreReg[8];         /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  GPIO enter dlps callback function(Save GPIO register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void GPIO_DLPS_Enter(void)
{
    GPIO_StoreReg[0] = GPIO->DATAOUT;
    GPIO_StoreReg[1] = GPIO->DATADIR;
    GPIO_StoreReg[2] = GPIO->DATASRC;
    GPIO_StoreReg[3] = GPIO->INTEN;
    GPIO_StoreReg[4] = GPIO->INTMASK;
    GPIO_StoreReg[5] = GPIO->INTTYPE;
    GPIO_StoreReg[6] = GPIO->INTPOLARITY;
    GPIO_StoreReg[7] = GPIO->DEBOUNCE;
    
    return;
}

/**
  * @brief  GPIO exit dlps callback function(Resume GPIO register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void GPIO_DLPS_Exit(void)
{
    
    PERIPH->PERI_FUNC1_EN |= BIT_PERI_GPIO_EN;
    PERIPH->PERI_CLK_CTRL |= (BIT_SOC_ACTCK_GPIO_EN | SYSBLK_SLPCK_GPIO_EN_Msk);
    
    GPIO->DATADIR       = GPIO_StoreReg[1];
    GPIO->DATASRC       = GPIO_StoreReg[2];
    GPIO->INTMASK       = GPIO_StoreReg[4];
    GPIO->INTTYPE       = GPIO_StoreReg[5];
    GPIO->INTPOLARITY   = GPIO_StoreReg[6];
    GPIO->DEBOUNCE      = GPIO_StoreReg[7];
    GPIO->DATAOUT       = GPIO_StoreReg[0];
    GPIO->INTEN         = GPIO_StoreReg[3];
    
    return;
}
#endif  /* USE_GPIO_DLPS */


/********************************************** ********************************************************/
/**************************************** [KEYSCAN DLPS] ***********************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_KEYSCAN_DLPS
__STATIC_INLINE void KeyScan_DLPS_Enter(void);
__STATIC_INLINE void KeyScan_DLPS_Exit(void); 

uint32_t KeyScan_StoreReg[4];      /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  KEYSCAN enter dlps callback function(Save KEYSCAN register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void KeyScan_DLPS_Enter(void)
{ 
    KeyScan_StoreReg[0] = KEYSCAN->CR0;
    KeyScan_StoreReg[1] = KEYSCAN->CR1;
    KeyScan_StoreReg[2] = KEYSCAN->CR2;
    KeyScan_StoreReg[3] = KEYSCAN->INTCR;
    
    return;
}

/**
  * @brief  KEYSCAN exit dlps callback function(Resume KEYSCAN register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void KeyScan_DLPS_Exit(void)
{
    PERIPH->PERI_FUNC0_EN |= BIT_PERI_KEYSCAN_EN;
    PERIPH->PERI_CLK_CTRL1 |= (BIT_SOC_ACTCK_KEYSCAN_EN | SYSBLK_SLPCK_KEYSCAN_EN_Msk);
#if 1
    KEYSCAN->CR0 = KeyScan_StoreReg[0]&(~KEYSCAN_CR0_CLKEN_Msk);
    KEYSCAN->CR1 = KeyScan_StoreReg[1];
    KEYSCAN->CR2 = KeyScan_StoreReg[2];
    
    KEYSCAN->CR2 |= KEYSCAN_CR2_DEBEN_Msk;
    KEYSCAN->CR2 = (KEYSCAN->CR2&(~KEYSCAN_CR2_DEBTIME_Msk)) | (KeyScan_StoreReg[2]& KEYSCAN_CR2_DEBTIME_Msk);
    KEYSCAN->CR2 &= ~KEYSCAN_CR2_DEBEN_Msk;

    KEYSCAN->INTCR = KeyScan_StoreReg[3];

    KEYSCAN->CR2 |= KEYSCAN_CR2_FIFOCLEAR_Msk;
    KEYSCAN->CR0 = KeyScan_StoreReg[0]|KEYSCAN_CR0_CLKEN_Msk;
#else
	// Disable keyscan
	KEYSCAN->CR0 &=	(~KEYSCAN_CR0_CLKEN_Msk);

	//Mask all interrupt
    KEYSCAN->INTCR |= (1 << 15);
	KEYSCAN->CR1 =  KeyScan_StoreReg[1];

	// Enable program debounce time
	KEYSCAN->CR2 |= KEYSCAN_CR2_DEBEN_Msk;
	// Mask debounce time
	KEYSCAN->CR2 &= ~(KEYSCAN_CR2_DEBTIME_Msk);
	// Configure debounce time
	KEYSCAN->CR2 = (KeyScan_StoreReg[2]& KEYSCAN_CR2_DEBTIME_Msk);
	// Disable program debounce time
	KEYSCAN->CR2 &= ~KEYSCAN_CR2_DEBEN_Msk;
	
	//Configure row size and  column size 
	KEYSCAN->CR0 =	KeyScan_StoreReg[0]&(~KEYSCAN_CR0_CLKEN_Msk);
	
	// Configure detection mode and FIFO control 
	KEYSCAN->CR2 =  KeyScan_StoreReg[2];
	
	//Clear Keyscan FIFO and all flag
	KEYSCAN->CR2 |=  (0x0F << 12);
	//Configure keyscan interrupt and clear all interrupt status and mask interrupt
	KEYSCAN->INTCR = KeyScan_StoreReg[3] | (0x0F << 12); 
	
	//Unmask all interrupt
	KEYSCAN->INTCR &= ~(1 << 15);

	// Enable Keyscan clock or not
    KEYSCAN->CR0 |= KeyScan_StoreReg[0] & KEYSCAN_CR0_CLKEN_Msk;

#endif
    
    return;
}
#endif /* USE_KEYSCAN_DLPS */


/********************************************** ********************************************************/
/**************************************** [QDEC DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_QDECODER_DLPS
__STATIC_INLINE void QuadDecoder_DLPS_Enter(void);
__STATIC_INLINE void QuadDecoder_DLPS_Exit(void);

uint32_t QuadDecoder_StoreReg[3];  /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  QDEC enter dlps callback function(Save QDEC register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void QuadDecoder_DLPS_Enter(void)
{
    QuadDecoder_StoreReg[0] = QDEC->REG_CR0;
    QuadDecoder_StoreReg[1] = QDEC->REG_CR1;
    QuadDecoder_StoreReg[2] = QDEC->REG_DIVNUM;
    
    return;
}

/**
  * @brief  QDEC exit dlps callback function(Resume QDEC register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void QuadDecoder_DLPS_Exit(void)
{
    SYSBLKCTRL->PERI_FUNC0_EN |= SYSBLK_QDECODE_EN_Msk;
    SYSBLKCTRL->PERI_CLK_CTRL1 |= (SYSBLK_ACTCK_QDECODE_EN_Msk | SYSBLK_SLPCK_QDECODE_EN_Msk);

    //clear flags
    QDEC->REG_CR0 = QuadDecoder_StoreReg[0];
    QDEC->REG_CR1 = (QuadDecoder_StoreReg[1] | (0x3f << 12));  //mask interrupt
    QDEC->REG_DIVNUM = QuadDecoder_StoreReg[2];
    
    QDEC->REG_CR2 = 0xFFFFFFFF;
    
    return;
}
#endif

/********************************************** ********************************************************/
/**************************************** [SPI0 DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_SPI0_DLPS
__STATIC_INLINE void SPI0_DLPS_Enter(void);
__STATIC_INLINE void SPI0_DLPS_Exit(void);

uint32_t SPI0_StoreReg[13];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  SPI0 enter dlps callback function(Save SPI0 register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void SPI0_DLPS_Enter(void)
{
    SPI0_StoreReg[0] = SPI0->CTRLR0;
    SPI0_StoreReg[1] = SPI0->CTRLR1;
    SPI0_StoreReg[2] = SPI0->SSIENR;
    SPI0_StoreReg[3] = SPI0->SER;
    SPI0_StoreReg[4] = SPI0->BAUDR;
    SPI0_StoreReg[5] = SPI0->TXFTLR;
    SPI0_StoreReg[6] = SPI0->RXFTLR;
    SPI0_StoreReg[7] = SPI0->IMR;
    SPI0_StoreReg[8] = SPI0->DMACR;
    SPI0_StoreReg[9] = SPI0->DMATDLR;
    SPI0_StoreReg[10] = SPI0->DMARDLR;
    SPI0_StoreReg[11] = SPI0->RX_SAMPLE_DLY;
    SPI0_StoreReg[12] = *(volatile uint32_t *)0x40000308;
}

/**
  * @brief  SPI0 exit dlps callback function(Resume SPI0 register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void SPI0_DLPS_Exit(void)
{
    *(volatile uint32_t *)0x40000308 = SPI0_StoreReg[12];
    SYSBLKCTRL->PERI_FUNC0_EN |=  SYSBLK_SPI0_EN_Msk;
    SYSBLKCTRL->PERI_CLK_CTRL0 |= (SYSBLK_ACTCK_SPI0_EN_Msk | SYSBLK_SLPCK_SPI0_EN_Msk);
  
    SPI0->CTRLR0 = SPI0_StoreReg[0];
    SPI0->CTRLR1 = SPI0_StoreReg[1];   
    SPI0->SER = SPI0_StoreReg[3];
    SPI0->BAUDR = SPI0_StoreReg[4];
    SPI0->TXFTLR = SPI0_StoreReg[5];
    SPI0->RXFTLR = SPI0_StoreReg[6];
    SPI0->IMR = SPI0_StoreReg[7];
    SPI0->DMACR = SPI0_StoreReg[8];
    SPI0->DMATDLR = SPI0_StoreReg[9];
    SPI0->DMARDLR = SPI0_StoreReg[10];
    SPI0->RX_SAMPLE_DLY = SPI0_StoreReg[11];

    /* Enable the selected SPI peripheral */
    SPI0->SSIENR = SPI0_StoreReg[2];    
}
#endif


/********************************************** ********************************************************/
/**************************************** [SPI1 DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_SPI1_DLPS
__STATIC_INLINE void SPI1_DLPS_Enter(void);
__STATIC_INLINE void SPI1_DLPS_Exit(void);

uint32_t SPI1_StoreReg[13];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  SPI1 enter dlps callback function(Save SPI1 register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void SPI1_DLPS_Enter(void)
{
    SPI1_StoreReg[0] = SPI1->CTRLR0;
    SPI1_StoreReg[1] = SPI1->CTRLR1;
    SPI1_StoreReg[2] = SPI1->SSIENR;
    SPI1_StoreReg[3] = SPI1->SER;
    SPI1_StoreReg[4] = SPI1->BAUDR;
    SPI1_StoreReg[5] = SPI1->TXFTLR;
    SPI1_StoreReg[6] = SPI1->RXFTLR;
    SPI1_StoreReg[7] = SPI1->IMR;
    SPI1_StoreReg[8] = SPI1->DMACR;
    SPI1_StoreReg[9] = SPI1->DMATDLR;
    SPI1_StoreReg[10] = SPI1->DMARDLR;
    SPI1_StoreReg[11] = SPI1->RX_SAMPLE_DLY;
    SPI1_StoreReg[12] = *(volatile uint32_t *)0x40000308;
}

/**
  * @brief  SPI1 exit dlps callback function(Resume SPI1 register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void SPI1_DLPS_Exit(void)
{  
    *(volatile uint32_t *)0x40000308 = SPI1_StoreReg[12];
    SYSBLKCTRL->PERI_FUNC0_EN |=  SYSBLK_SPI1_EN_Msk;
    SYSBLKCTRL->PERI_CLK_CTRL0 |= (SYSBLK_ACTCK_SPI1_EN_Msk | SYSBLK_SLPCK_SPI1_EN_Msk);

    SPI1->CTRLR0 = SPI1_StoreReg[0];
    SPI1->CTRLR1 = SPI1_StoreReg[1];
    SPI1->SER = SPI1_StoreReg[3];
    SPI1->BAUDR = SPI1_StoreReg[4];
    SPI1->TXFTLR = SPI1_StoreReg[5];
    SPI1->RXFTLR = SPI1_StoreReg[6];
    SPI1->IMR = SPI1_StoreReg[7];
    SPI1->DMACR = SPI1_StoreReg[8];
    SPI1->DMATDLR = SPI1_StoreReg[9];
    SPI1->DMARDLR = SPI1_StoreReg[10];
    SPI1->RX_SAMPLE_DLY = SPI1_StoreReg[11];

    /* Enable the selected SPI peripheral */
    SPI1->SSIENR = SPI1_StoreReg[2];
}
#endif

/********************************************** ********************************************************/
/**************************************** [SPI2W DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_SPI2W_DLPS
__STATIC_INLINE void SPI2W_DLPS_Enter(void);
__STATIC_INLINE void SPI2W_DLPS_Exit(void);

uint32_t SPI2W_StoreReg[2];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  SPI2W enter dlps callback function(Save SPI2W register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void SPI2W_DLPS_Enter(void)
{
    SPI2W_StoreReg[0] = SPI2WIRE->CFGR;
	SPI2W_StoreReg[1] = SPI2WIRE->INTCR;
}

/**
  * @brief  SPI2W exit dlps callback function(Resume SPI2W register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void SPI2W_DLPS_Exit(void)
{
    SYSBLKCTRL->PERI_FUNC0_EN |=  SYSBLK_SPI2W_EN_Msk;
    SYSBLKCTRL->PERI_CLK_CTRL1 |= (SYSBLK_ACTCK_SPI2WIRE_EN_Msk | SYSBLK_SLPCK_SPI2WIRE_EN_Msk);
  
    SPI2WIRE->CFGR = SPI2W_StoreReg[0];
	SPI2WIRE->INTCR = SPI2W_StoreReg[1];   
}
#endif

/********************************************** ********************************************************/
/**************************************** [I2C0 DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_I2C0_DLPS
__STATIC_INLINE void I2C0_DLPS_Enter(void);
__STATIC_INLINE void I2C0_DLPS_Exit(void);

uint32_t I2C0_StoreReg[19];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  I2C0 enter dlps callback function(Save I2C0 register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void I2C0_DLPS_Enter(void)
{
    I2C0_StoreReg[0] = I2C0->IC_CON;
    I2C0_StoreReg[1] = I2C0->IC_TAR;
    I2C0_StoreReg[2] = I2C0->IC_SAR;

    I2C0_StoreReg[3] = I2C0->IC_SS_SCL_HCNT;
    I2C0_StoreReg[4] = I2C0->IC_SS_SCL_LCNT;
    I2C0_StoreReg[5] = I2C0->IC_FS_SCL_HCNT;
    I2C0_StoreReg[6] = I2C0->IC_FS_SCL_LCNT;
    I2C0_StoreReg[7] = I2C0->IC_HS_SCL_HCNT;
    I2C0_StoreReg[8] = I2C0->IC_HS_SCL_LCNT;

    I2C0_StoreReg[9] = I2C0->IC_INTR_MASK;
    I2C0_StoreReg[10] = I2C0->IC_RX_TL;
    I2C0_StoreReg[11] = I2C0->IC_TX_TL;
    I2C0_StoreReg[12] = I2C0->IC_ENABLE;
    I2C0_StoreReg[13] = I2C0->IC_SDA_HOLD;
    I2C0_StoreReg[14] = I2C0->IC_SLV_DATA_NACK_ONLY;
    I2C0_StoreReg[15] = I2C0->IC_DMA_CR;
    I2C0_StoreReg[16] = I2C0->IC_DMA_TDLR;
    I2C0_StoreReg[17] = I2C0->IC_DMA_RDLR;
    
    I2C0_StoreReg[18] = I2C0->IC_SDA_SETUP;
}

/**
  * @brief  I2C0 exit dlps callback function(Resume I2C0 register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void I2C0_DLPS_Exit(void)
{
    PERIPH->PERI_CLK_CTRL1 |= (SYSBLK_ACTCK_I2C0_EN_Msk | SYSBLK_SLPCK_I2C0_EN_Msk);
    PERIPH->PERI_FUNC0_EN |= SYSBLK_I2C0_EN_Msk;  

    I2C0->IC_CON = I2C0_StoreReg[0];
    I2C0->IC_TAR = I2C0_StoreReg[1];
    I2C0->IC_SAR = I2C0_StoreReg[2];

    I2C0->IC_SS_SCL_HCNT =I2C0_StoreReg[3];
    I2C0->IC_SS_SCL_LCNT = I2C0_StoreReg[4];
    I2C0->IC_FS_SCL_HCNT = I2C0_StoreReg[5];
    I2C0->IC_FS_SCL_LCNT = I2C0_StoreReg[6];
    I2C0->IC_HS_SCL_HCNT = I2C0_StoreReg[7];
    I2C0->IC_HS_SCL_LCNT = I2C0_StoreReg[8];

    I2C0->IC_INTR_MASK = I2C0_StoreReg[9];
    I2C0->IC_RX_TL = I2C0_StoreReg[10];
    I2C0->IC_TX_TL = I2C0_StoreReg[11];
    I2C0->IC_SDA_HOLD = I2C0_StoreReg[13];
    I2C0->IC_SLV_DATA_NACK_ONLY = I2C0_StoreReg[14];
    I2C0->IC_DMA_CR = I2C0_StoreReg[15];
    I2C0->IC_DMA_TDLR = I2C0_StoreReg[16];
    I2C0->IC_DMA_RDLR = I2C0_StoreReg[17];
    I2C0->IC_SDA_SETUP = I2C0_StoreReg[18];

    I2C0->IC_ENABLE = I2C0_StoreReg[12];
}
#endif

/********************************************** ********************************************************/
/**************************************** [I2C1 DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_I2C1_DLPS
__STATIC_INLINE void I2C1_DLPS_Enter(void);
__STATIC_INLINE void I2C1_DLPS_Exit(void);

uint32_t I2C1_StoreReg[19];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  I2C1 enter dlps callback function(Save I2C1 register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void I2C1_DLPS_Enter(void)
{
    I2C1_StoreReg[0] = I2C1->IC_CON;
    I2C1_StoreReg[1] = I2C1->IC_TAR;
    I2C1_StoreReg[2] = I2C1->IC_SAR;

    I2C1_StoreReg[3] = I2C1->IC_SS_SCL_HCNT;
    I2C1_StoreReg[4] = I2C1->IC_SS_SCL_LCNT;
    I2C1_StoreReg[5] = I2C1->IC_FS_SCL_HCNT;
    I2C1_StoreReg[6] = I2C1->IC_FS_SCL_LCNT;
    I2C1_StoreReg[7] = I2C1->IC_HS_SCL_HCNT;
    I2C1_StoreReg[8] = I2C1->IC_HS_SCL_LCNT;

    I2C1_StoreReg[9] = I2C1->IC_INTR_MASK;
    I2C1_StoreReg[10] = I2C1->IC_RX_TL;
    I2C1_StoreReg[11] = I2C1->IC_TX_TL;
    I2C1_StoreReg[12] = I2C1->IC_ENABLE;
    I2C1_StoreReg[13] = I2C1->IC_SDA_HOLD;
    I2C1_StoreReg[14] = I2C1->IC_SLV_DATA_NACK_ONLY;
    I2C1_StoreReg[15] = I2C1->IC_DMA_CR;
    I2C1_StoreReg[16] = I2C1->IC_DMA_TDLR;
    I2C1_StoreReg[17] = I2C1->IC_DMA_RDLR;
    
    I2C1_StoreReg[18] = I2C1->IC_SDA_SETUP;
}

/**
  * @brief  I2C1 exit dlps callback function(Resume I2C1 register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void I2C1_DLPS_Exit(void)
{
    PERIPH->PERI_CLK_CTRL1 |= (SYSBLK_ACTCK_I2C1_EN_Msk | SYSBLK_SLPCK_I2C1_EN_Msk);
    PERIPH->PERI_FUNC0_EN |= SYSBLK_I2C1_EN_Msk;  

    I2C1->IC_CON = I2C1_StoreReg[0];
    I2C1->IC_TAR = I2C1_StoreReg[1];
    I2C1->IC_SAR = I2C1_StoreReg[2];

    I2C1->IC_SS_SCL_HCNT =I2C1_StoreReg[3];
    I2C1->IC_SS_SCL_LCNT = I2C1_StoreReg[4];
    I2C1->IC_FS_SCL_HCNT = I2C1_StoreReg[5];
    I2C1->IC_FS_SCL_LCNT = I2C1_StoreReg[6];
    I2C1->IC_HS_SCL_HCNT = I2C1_StoreReg[7];
    I2C1->IC_HS_SCL_LCNT = I2C1_StoreReg[8];

    I2C1->IC_INTR_MASK = I2C1_StoreReg[9];
    I2C1->IC_RX_TL = I2C1_StoreReg[10];
    I2C1->IC_TX_TL = I2C1_StoreReg[11];
    I2C1->IC_SDA_HOLD = I2C1_StoreReg[13];
    I2C1->IC_SLV_DATA_NACK_ONLY = I2C1_StoreReg[14];
    I2C1->IC_DMA_CR = I2C1_StoreReg[15];
    I2C1->IC_DMA_TDLR = I2C1_StoreReg[16];
    I2C1->IC_DMA_RDLR = I2C1_StoreReg[17];
    I2C1->IC_SDA_SETUP = I2C1_StoreReg[18];

    I2C1->IC_ENABLE = I2C1_StoreReg[12];
}
#endif

/********************************************** ********************************************************/
/**************************************** [Timer & PWM DLPS] *******************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_TIM_DLPS
__STATIC_INLINE void TIM_DLPS_Enter(void);
__STATIC_INLINE void TIM_DLPS_Exit(void); 

uint32_t TIM_StoreReg[18];         /*  This array should be placed in RAM ON/Buffer ON.    */

/* PWM, use with timer */
#if USE_PWM0_DLPS
uint32_t PWM0_StoreReg[2];         /*  This array should be placed in RAM ON/Buffer ON.    */
#endif
#if USE_PWM1_DLPS
uint32_t PWM1_StoreReg[2];         /*  This array should be placed in RAM ON/Buffer ON.    */
#endif
#if USE_PWM2_DLPS
uint32_t PWM2_StoreReg[2];         /*  This array should be placed in RAM ON/Buffer ON.    */
#endif
#if USE_PWM3_DLPS
uint32_t PWM3_StoreReg[2];         /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

/**
  * @brief  TIMER enter dlps callback function(Save TIMER register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void TIM_DLPS_Enter(void)
{
    TIM_StoreReg[0] = TIM0->LoadCount;
    TIM_StoreReg[1] = TIM0->ControlReg;
    
    TIM_StoreReg[2] = TIM1->LoadCount;
    TIM_StoreReg[3] = TIM1->ControlReg;

    TIM_StoreReg[4] = TIM2->LoadCount;
    TIM_StoreReg[5] = TIM2->ControlReg;

    TIM_StoreReg[6] = TIM3->LoadCount;
    TIM_StoreReg[7] = TIM3->ControlReg;

    TIM_StoreReg[8] = TIM4->LoadCount;
    TIM_StoreReg[9] = TIM4->ControlReg;

    TIM_StoreReg[10] = TIM5->LoadCount;
    TIM_StoreReg[11] = TIM5->ControlReg;

    TIM_StoreReg[12] = TIM6->LoadCount;
    TIM_StoreReg[13] = TIM6->ControlReg;

    TIM_StoreReg[14] = TIM7->LoadCount;
    TIM_StoreReg[15] = TIM7->ControlReg;

    TIM_StoreReg[16] = *((volatile uint32_t *)0x40000360UL);
    TIM_StoreReg[17] = *((volatile uint32_t *)0x40000384UL);
#if USE_PWM0_DLPS
    PWM0_StoreReg[0] = PWM0->CR0;
    PWM0_StoreReg[1] = PWM0->CR1;
#endif

#if USE_PWM1_DLPS
    PWM1_StoreReg[0] = PWM1->CR0;
    PWM1_StoreReg[1] = PWM1->CR1;
#endif

#if USE_PWM2_DLPS
    PWM2_StoreReg[0] = PWM2->CR0;
    PWM2_StoreReg[1] = PWM2->CR1;
#endif

#if USE_PWM3_DLPS
    PWM3_StoreReg[0] = PWM3->CR0;
    PWM3_StoreReg[1] = PWM3->CR1;
#endif
}

/**
  * @brief  TIMER exit dlps callback function(Resume TIMER register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void TIM_DLPS_Exit(void)
{
    /* Enable timer IP clock and function */
    SYSBLKCTRL->SOC_FUNC_EN |= BIT(16);
    SYSBLKCTRL->CLK_CTRL |= (SYSBLK_ACTCK_TIMER_EN_Msk | SYSBLK_SLPCK_TIMER_EN_Msk);

    *((volatile uint32_t *)0x40000360UL) = TIM_StoreReg[16]; 
    *((volatile uint32_t *)0x40000384UL) = TIM_StoreReg[17]; 

    TIM0->LoadCount = TIM_StoreReg[0];
    TIM0->ControlReg = TIM_StoreReg[1];
    
    TIM1->LoadCount = TIM_StoreReg[2];
    TIM1->ControlReg = TIM_StoreReg[3];

    TIM2->LoadCount = TIM_StoreReg[4];
    TIM2->ControlReg = TIM_StoreReg[5];

    TIM3->LoadCount = TIM_StoreReg[6];
    TIM3->ControlReg = TIM_StoreReg[7];

    TIM4->LoadCount = TIM_StoreReg[8];
    TIM4->ControlReg = TIM_StoreReg[9];

    TIM5->LoadCount = TIM_StoreReg[10];
    TIM5->ControlReg = TIM_StoreReg[11];

    TIM6->LoadCount = TIM_StoreReg[12];
    TIM6->ControlReg = TIM_StoreReg[13];

    TIM7->LoadCount = TIM_StoreReg[14];
    TIM7->ControlReg = TIM_StoreReg[15];

#if USE_PWM0_DLPS
    PWM0->CR0 &= ~(BIT(0));
    PWM0->CR1 = PWM0_StoreReg[1];
    PWM0->CR0 = PWM0_StoreReg[0];
#endif

#if USE_PWM1_DLPS
    PWM1->CR0 &= ~(BIT(0));
    PWM1->CR1 = PWM1_StoreReg[1];
    PWM1->CR0 = PWM1_StoreReg[0];
#endif

#if USE_PWM2_DLPS
    PWM2->CR0 &= ~(BIT(0));
    PWM2->CR1 = PWM2_StoreReg[1];
    PWM2->CR0 = PWM2_StoreReg[0];
#endif

#if USE_PWM3_DLPS
    PWM3->CR0 &= ~(BIT(0));
    PWM3->CR1 = PWM3_StoreReg[1];
    PWM3->CR0 = PWM3_StoreReg[0];
#endif

}
#endif  /* USE_TIM_DLPS */


/********************************************** ********************************************************/
/**************************************** [UART DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_UART_DLPS
#include "rtl876x_uart.h"

__STATIC_INLINE void UART_DLPS_Enter(void);
__STATIC_INLINE void UART_DLPS_Exit(void);

uint32_t UART_StoreReg[8];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  UART enter dlps callback function(Save UART register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void UART_DLPS_Enter(void)
{  
    //access DLH and DLL
    UART->LCR |= (1 << 7);
    UART_StoreReg[0] = UART->DLL;
    UART_StoreReg[1] = UART->DLH_INTCR;
    UART->LCR &= (~(1 << 7));

    //save other registers
    UART_StoreReg[2] = UART->DLH_INTCR;
    //UartFcrRegVal is saved in uart inti, because FCR is write-only register
    UART_StoreReg[3] = UartFcrRegVal;
    UART_StoreReg[4] = UART->LCR;
    UART_StoreReg[5] = UART->MCR;
    UART_StoreReg[6] = UART->SPR;
    UART_StoreReg[7] = UART->STSR;

    return;
}

/**
  * @brief  UART exit dlps callback function(Resume UART register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void UART_DLPS_Exit(void)
{
    PERIPH->PERI_FUNC0_EN |= (1 << 0);
    PERIPH->PERI_CLK_CTRL0 |= (SYSBLK_ACTCK_UART0DATA_EN_Msk | SYSBLK_SLPCK_UART0DATA_EN_Msk);
    PERIPH->PERI_BD_FUNC0_EN |= (1 << 0);
    
    //access DLH and DLL
    UART->LCR |= (1 << 7);
    UART->DLL = UART_StoreReg[0];
    UART->DLH_INTCR =  UART_StoreReg[1];
    UART->LCR &= (~(1 << 7));

    //access other registers
    UART->INTID_FCR = UART_StoreReg[3];
    UART->LCR = UART_StoreReg[4];
    UART->MCR = UART_StoreReg[5];  
    UART->SPR = UART_StoreReg[6];
    UART->STSR = UART_StoreReg[7];
    UART->DLH_INTCR = UART_StoreReg[2];
    
    return;
}
#endif

/********************************************** ********************************************************/
/**************************************** [ADC DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

#if USE_ADC_DLPS
__STATIC_INLINE void ADC_DLPS_Enter(void);
__STATIC_INLINE void ADC_DLPS_Exit(void);

uint32_t ADC_StoreReg[4];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  ADC enter dlps callback function(Save ADC register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void ADC_DLPS_Enter(void)
{
    ADC_StoreReg[0] = ADC->CR;
    ADC_StoreReg[1] = ADC->INTCR;
    ADC_StoreReg[2] = ADC->PWR;
    ADC_StoreReg[3] = ADC->ANP0;
    return;
}

/**
  * @brief  ADC exit dlps callback function(Resume ADC register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void ADC_DLPS_Exit(void)
{
    PERIPH->PERI_FUNC1_EN |= (1 << 0);
    PERIPH->PERI_CLK_CTRL1 |= (SYSBLK_ACTCK_ADC_EN_Msk | SYSBLK_SLPCK_ADC_EN_Msk);

    ADC->CR     = ADC_StoreReg[0];
    ADC->PWR    = ADC_StoreReg[2]; 
    ADC->ANP0   = ADC_StoreReg[3]; 
    ADC->ANP1  |= (0x010 << 6); 
    ADC->ANP6  |= (1 << 12);
    ADC->INTCR  = ADC_StoreReg[1];
    
    return;
}

#endif

/********************************************** ********************************************************/
/**************************************** [IR DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

#if USE_IR_DLPS
__STATIC_INLINE void IR_DLPS_Enter(void);
__STATIC_INLINE void IR_DLPS_Exit(void);

uint32_t IR_StoreReg[5];        /*  This array should be placed in RAM ON/Buffer ON.    */

/**
  * @brief  IR enter dlps callback function(Save IR register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void IR_DLPS_Enter(void)
{
    IR_StoreReg[0] = IR->MFCR;
    IR_StoreReg[1] = IR->INTCR;;
    IR_StoreReg[2] = IR->CSR1;
    IR_StoreReg[3] = IR->CSR2;
    IR_StoreReg[4] = IR->CR;
    return;
}

/**
  * @brief  IR exit dlps callback function(Resume IR register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void IR_DLPS_Exit(void)
{
    PERIPH->PERI_FUNC0_EN |= (1 << 10);
    PERIPH->PERI_CLK_CTRL0 |= (SYSBLK_ACTCK_IR_EN_Msk | SYSBLK_SLPCK_IR_EN_Msk);

    IR->MFCR  = IR_StoreReg[0];
    IR->CSR1  = IR_StoreReg[2];
    IR->CSR2  = IR_StoreReg[3];
    IR->CR    = IR_StoreReg[4];
    IR->INTCR = IR_StoreReg[1];
    
    return;
}

#endif

/********************************************** ********************************************************/
/**************************************** [RTC DLPS] ***************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_RTC_DLPS

#define SYSON_BASE_ADDRESS 0x40000000
#define SYSON_READ(Offset) \
((UINT32)*((volatile UINT32*)(SYSON_BASE_ADDRESS+(Offset))))
#define SYSON_WRITE(Offset, Value) \
{ \
       ((*((volatile UINT32*)(SYSON_BASE_ADDRESS + (Offset)))) = (Value)); \
       SYSON_READ(0); /* dummy read */ \
}

__STATIC_INLINE void RTC_DLPS_Exit(void);

/**
  * @brief  RTC exit dlps callback function(Resume RTC register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void RTC_DLPS_Exit(void)
{
    uint32_t aon_rtc_regValue_dlps = 0;
 
    // Enable RTC power cut
         // Read power cut's status, if power cut not enable enable it again.
     if (!(SYSON_READ(0x14) & BIT(9)))
     {
               aon_rtc_regValue_dlps = (SYSON_READ(0x14) | BIT(9));
               SYSON_WRITE(0x14, aon_rtc_regValue_dlps);
     }

     // Release RTC ISO by BTAON fast access
     aon_rtc_regValue_dlps = (bee_aon_gp_read( 0x1D) & ~BIT(4));
     bee_aon_gp_write(0x1D, aon_rtc_regValue_dlps);

    return;
}

#endif

/********************************************** ********************************************************/
/**************************************** [GDMA DLPS] **************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

#if USE_GDMA_DLPS

__STATIC_INLINE void GDMA_DLPS_Enter(void);
__STATIC_INLINE void GDMA_DLPS_Exit(void);
uint32_t GDMA_StoreReg[7];         /*  This array should be placed in RAM ON/Buffer ON.    */

#endif

#if USE_GDMACHANNEL0_DLPS
uint32_t GDMAChannel0_StoreReg[6];        /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

#if USE_GDMACHANNEL1_DLPS
uint32_t GDMAChannel1_StoreReg[6];        /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

#if USE_GDMACHANNEL2_DLPS
uint32_t GDMAChannel2_StoreReg[6];        /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

#if USE_GDMACHANNEL3_DLPS
uint32_t GDMAChannel3_StoreReg[6];        /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

#if USE_GDMACHANNEL4_DLPS
uint32_t GDMAChannel4_StoreReg[6];        /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

#if USE_GDMACHANNEL5_DLPS
uint32_t GDMAChannel5_StoreReg[6];        /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

#if USE_GDMACHANNEL6_DLPS
uint32_t GDMAChannel6_StoreReg[6];        /*  This array should be placed in RAM ON/Buffer ON.    */
#endif

#if USE_GDMA_DLPS
/**
  * @brief  GDMA enter dlps callback function(Save GDMA register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void GDMA_DLPS_Enter(void)
{
    GDMA_StoreReg[0] = GDMA_BASE->DmaCfgReg;
    GDMA_StoreReg[1] = GDMA_BASE->ChEnReg;  
    GDMA_StoreReg[2] = GDMA_BASE->MASK_TFR;
    GDMA_StoreReg[3] = GDMA_BASE->MASK_BLOCK;
    GDMA_StoreReg[4] = GDMA_BASE->MASK_SRC_TRAN;
    GDMA_StoreReg[5] = GDMA_BASE->MASK_DST_TRAN;
    GDMA_StoreReg[6] = GDMA_BASE->MASK_ERR;

    PeriphSelectFlag |= ((USE_GDMACHANNEL0_DLPS << 25)
                        |(USE_GDMACHANNEL1_DLPS << 26)
                        |(USE_GDMACHANNEL2_DLPS << 27)
                        |(USE_GDMACHANNEL3_DLPS << 28)
                        |(USE_GDMACHANNEL4_DLPS << 29)
                        |(USE_GDMACHANNEL5_DLPS << 30)
                        |((uint32_t)USE_GDMACHANNEL6_DLPS << 31));
#if USE_GDMACHANNEL0_DLPS 
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH0)
    {
        GDMAChannel0_StoreReg[0] = GDMA_Channel0->SAR;
        GDMAChannel0_StoreReg[1] = GDMA_Channel0->DAR;
        GDMAChannel0_StoreReg[2] = GDMA_Channel0->CTL_LOW;
        GDMAChannel0_StoreReg[3] = GDMA_Channel0->CTL_HIGH;
        GDMAChannel0_StoreReg[4] = GDMA_Channel0->CFG_LOW;
        GDMAChannel0_StoreReg[5] = GDMA_Channel0->CFG_HIGH; 
    }
#endif
#if USE_GDMACHANNEL1_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH1)
    {       
        GDMAChannel1_StoreReg[0] = GDMA_Channel1->SAR;
        GDMAChannel1_StoreReg[1] = GDMA_Channel1->DAR;
        GDMAChannel1_StoreReg[2] = GDMA_Channel1->CTL_LOW;
        GDMAChannel1_StoreReg[3] = GDMA_Channel1->CTL_HIGH;
        GDMAChannel1_StoreReg[4] = GDMA_Channel1->CFG_LOW;
        GDMAChannel1_StoreReg[5] = GDMA_Channel1->CFG_HIGH;
    }
#endif
#if USE_GDMACHANNEL2_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH2)
    {       
        GDMAChannel2_StoreReg[0] = GDMA_Channel2->SAR;
        GDMAChannel2_StoreReg[1] = GDMA_Channel2->DAR;
        GDMAChannel2_StoreReg[2] = GDMA_Channel2->CTL_LOW;
        GDMAChannel2_StoreReg[3] = GDMA_Channel2->CTL_HIGH;
        GDMAChannel2_StoreReg[4] = GDMA_Channel2->CFG_LOW;
        GDMAChannel2_StoreReg[5] = GDMA_Channel2->CFG_HIGH;
    }
#endif
#if USE_GDMACHANNEL3_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH3)
    {
        GDMAChannel3_StoreReg[0] = GDMA_Channel3->SAR;
        GDMAChannel3_StoreReg[1] = GDMA_Channel3->DAR;
        GDMAChannel3_StoreReg[2] = GDMA_Channel3->CTL_LOW;
        GDMAChannel3_StoreReg[3] = GDMA_Channel3->CTL_HIGH;
        GDMAChannel3_StoreReg[4] = GDMA_Channel3->CFG_LOW;
        GDMAChannel3_StoreReg[5] = GDMA_Channel3->CFG_HIGH;
    }
#endif
#if USE_GDMACHANNEL4_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH4)
    {
        GDMAChannel4_StoreReg[0] = GDMA_Channel4->SAR;
        GDMAChannel4_StoreReg[1] = GDMA_Channel4->DAR;
        GDMAChannel4_StoreReg[2] = GDMA_Channel4->CTL_LOW;
        GDMAChannel4_StoreReg[3] = GDMA_Channel4->CTL_HIGH;
        GDMAChannel4_StoreReg[4] = GDMA_Channel4->CFG_LOW;
        GDMAChannel4_StoreReg[5] = GDMA_Channel4->CFG_HIGH;
    }
#endif
#if USE_GDMACHANNEL5_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH5)
    {
        GDMAChannel5_StoreReg[0] = GDMA_Channel5->SAR;
        GDMAChannel5_StoreReg[1] = GDMA_Channel5->DAR;
        GDMAChannel5_StoreReg[2] = GDMA_Channel5->CTL_LOW;
        GDMAChannel5_StoreReg[3] = GDMA_Channel5->CTL_HIGH;
        GDMAChannel5_StoreReg[4] = GDMA_Channel5->CFG_LOW;
        GDMAChannel5_StoreReg[5] = GDMA_Channel5->CFG_HIGH;
    }
#endif
#if USE_GDMACHANNEL6_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH6)
    {
        GDMAChannel6_StoreReg[0] = GDMA_Channel6->SAR;
        GDMAChannel6_StoreReg[1] = GDMA_Channel6->DAR;
        GDMAChannel6_StoreReg[2] = GDMA_Channel6->CTL_LOW;
        GDMAChannel6_StoreReg[3] = GDMA_Channel6->CTL_HIGH;
        GDMAChannel6_StoreReg[4] = GDMA_Channel6->CFG_LOW;
        GDMAChannel6_StoreReg[5] = GDMA_Channel6->CFG_HIGH;
    }
#endif
}

/**
  * @brief  GDMA exit dlps callback function(Resume GDMA register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void GDMA_DLPS_Exit(void)
{
    SYSBLKCTRL->SOC_FUNC_EN |= SYSBLK_GDMA0_EN_Msk;
    PERIPH->PERI_CLK_CTRL |= (SYSBLK_ACTCK_GDMA0_EN_Msk | SYSBLK_SLPCK_GDMA0_EN_Msk);

    /* Enable GDMA in DmaCfgReg*/
    GDMA_BASE->DmaCfgReg = GDMA_StoreReg[0];
    
    /*----------clear pending all interrupts of GDMA channel-----------*/
    GDMA_BASE->CLEAR_TFR = 0x3f;
    GDMA_BASE->CLEAR_BLOCK = 0x3f;
    GDMA_BASE->CLEAR_DST_TRAN = 0x3f;
    GDMA_BASE->CLEAR_SRC_TRAN = 0x3f;
    GDMA_BASE->CLEAR_ERR = 0x3f;

    /*--------------------mask interrupt-------- -----------------*/
    GDMA_BASE->MASK_TFR = GDMA_StoreReg[2];
    GDMA_BASE->MASK_BLOCK = GDMA_StoreReg[3];
    GDMA_BASE->MASK_SRC_TRAN = GDMA_StoreReg[4];
    GDMA_BASE->MASK_DST_TRAN = GDMA_StoreReg[5];
    GDMA_BASE->MASK_ERR = GDMA_StoreReg[6];

#if USE_GDMACHANNEL0_DLPS 
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH0)
    {
        GDMA_Channel0->SAR = GDMAChannel0_StoreReg[0];
        GDMA_Channel0->DAR = GDMAChannel0_StoreReg[1];
        GDMA_Channel0->CTL_LOW = GDMAChannel0_StoreReg[2];
        GDMA_Channel0->CTL_HIGH = GDMAChannel0_StoreReg[3];
        GDMA_Channel0->CFG_LOW = GDMAChannel0_StoreReg[4];
        GDMA_Channel0->CFG_HIGH = GDMAChannel0_StoreReg[5];
    }
#endif
#if USE_GDMACHANNEL1_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH1)
    {       
        GDMA_Channel1->SAR = GDMAChannel1_StoreReg[0];
        GDMA_Channel1->DAR = GDMAChannel1_StoreReg[1];
        GDMA_Channel1->CTL_LOW = GDMAChannel1_StoreReg[2];
        GDMA_Channel1->CTL_HIGH = GDMAChannel1_StoreReg[3];
        GDMA_Channel1->CFG_LOW = GDMAChannel1_StoreReg[4];
        GDMA_Channel1->CFG_HIGH = GDMAChannel1_StoreReg[5];
    }
#endif
#if USE_GDMACHANNEL2_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH2)
    {       
        GDMA_Channel2->SAR = GDMAChannel2_StoreReg[0];
        GDMA_Channel2->DAR = GDMAChannel2_StoreReg[1];
        GDMA_Channel2->CTL_LOW = GDMAChannel2_StoreReg[2];
        GDMA_Channel2->CTL_HIGH = GDMAChannel2_StoreReg[3];
        GDMA_Channel2->CFG_LOW = GDMAChannel2_StoreReg[4];
        GDMA_Channel2->CFG_HIGH = GDMAChannel2_StoreReg[5];
    }
#endif
#if USE_GDMACHANNEL3_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH3)
    {
        GDMA_Channel3->SAR = GDMAChannel3_StoreReg[0];
        GDMA_Channel3->DAR = GDMAChannel3_StoreReg[1];
        GDMA_Channel3->CTL_LOW = GDMAChannel3_StoreReg[2];
        GDMA_Channel3->CTL_HIGH = GDMAChannel3_StoreReg[3];
        GDMA_Channel3->CFG_LOW = GDMAChannel3_StoreReg[4];
        GDMA_Channel3->CFG_HIGH = GDMAChannel3_StoreReg[5];
    }
#endif
#if USE_GDMACHANNEL4_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH4)
    {
        GDMA_Channel4->SAR = GDMAChannel4_StoreReg[0];
        GDMA_Channel4->DAR = GDMAChannel4_StoreReg[1];
        GDMA_Channel4->CTL_LOW = GDMAChannel4_StoreReg[2];
        GDMA_Channel4->CTL_HIGH = GDMAChannel4_StoreReg[3];
        GDMA_Channel4->CFG_LOW = GDMAChannel4_StoreReg[4];
        GDMA_Channel4->CFG_HIGH = GDMAChannel4_StoreReg[5];
    }
#endif
#if USE_GDMACHANNEL5_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH5)
    {
        GDMA_Channel5->SAR = GDMAChannel5_StoreReg[0];
        GDMA_Channel5->DAR = GDMAChannel5_StoreReg[1];
        GDMA_Channel5->CTL_LOW = GDMAChannel5_StoreReg[2];
        GDMA_Channel5->CTL_HIGH = GDMAChannel5_StoreReg[3];
        GDMA_Channel5->CFG_LOW = GDMAChannel5_StoreReg[4];
        GDMA_Channel5->CFG_HIGH = GDMAChannel5_StoreReg[5];
    }
#endif
#if USE_GDMACHANNEL6_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA_CH6)
    {
        GDMA_Channel6->SAR = GDMAChannel6_StoreReg[0];
        GDMA_Channel6->DAR = GDMAChannel6_StoreReg[1];
        GDMA_Channel6->CTL_LOW = GDMAChannel6_StoreReg[2];
        GDMA_Channel6->CTL_HIGH = GDMAChannel6_StoreReg[3];
        GDMA_Channel6->CFG_LOW = GDMAChannel6_StoreReg[4];
        GDMA_Channel6->CFG_HIGH = GDMAChannel6_StoreReg[5];
    }
#endif
    
    /*----------clear pending all interrupts of GDMA channel before enable channel-----------*/
    GDMA_BASE->CLEAR_TFR = 0x3f;
    GDMA_BASE->CLEAR_BLOCK = 0x3f;
    GDMA_BASE->CLEAR_DST_TRAN = 0x3f;
    GDMA_BASE->CLEAR_SRC_TRAN = 0x3f;
    GDMA_BASE->CLEAR_ERR = 0x3f;
    
    /* Enable the selected DMA Channelx */
    GDMA_BASE->ChEnReg = GDMA_StoreReg[1];
}
#endif

/********************************************** ********************************************************/
/*********************************** [USER Exit DLPS CALLBACK FUCN] *****************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_USER_DEFINE_DLPS_EXIT_CB

DLPS_IO_ExitDlpsCB User_IO_ExitDlpsCB = NULL;

#endif /* USE_USER_DEFINE_DLPS_EXIT_CB */

/********************************************** ********************************************************/
/*********************************** [USER Enter DLPS CALLBACK FUCN] *****************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_USER_DEFINE_DLPS_ENTER_CB

DLPS_IO_EnterDlpsCB User_IO_EnterDlpsCB = NULL;

#endif /* USE_USER_DEFINE_DLPS_EXIT_CB */

/********************************************** ********************************************************/
/*********************************** [Enter & Exit DLPS CALLBACK FUNC] *********************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#if USE_IO_DRIVER_DLPS

/**
  * @brief  IO enter dlps callback function 
  * @param  None
  * @retval None
  */
void DLPS_IO_EnterDlpsCb(void)
{
    /* may lost*/
    //DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DLPS_IO_EnterDlpsCb", 0);

    CPU_DLPS_Enter();
   
    Pinmux_DLPS_Enter();
	
	Log_SWD_DLPS_Enter();

#if USE_USER_DEFINE_DLPS_ENTER_CB
    if(User_IO_EnterDlpsCB) 
    {
        User_IO_EnterDlpsCB();
    }
#endif

#if USE_I2C0_DLPS    
    if(PeriphSelectFlag & Bit_Periph_I2C0)
    {
        I2C0_DLPS_Enter();
    }
#endif
#if USE_I2C1_DLPS 
    if(PeriphSelectFlag & Bit_Periph_I2C1)
    {
        I2C1_DLPS_Enter();
    }
#endif
#if USE_TIM_DLPS
    if(PeriphSelectFlag & Bit_Periph_TIMER)
    {
        TIM_DLPS_Enter();
    }
#endif
#if USE_QDECODER_DLPS
    if(PeriphSelectFlag & Bit_Periph_QDEC)
    {
        QuadDecoder_DLPS_Enter();
    }
#endif
#if USE_IR_DLPS
    if(PeriphSelectFlag & Bit_Periph_IR)
    {
        IR_DLPS_Enter();
    }
#endif
#if USE_GDMA_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA)
    {
        GDMA_DLPS_Enter();
    }
#endif
#if USE_UART_DLPS
    if(PeriphSelectFlag & Bit_Periph_UART)
    {
        UART_DLPS_Enter();
    }
#endif
#if USE_SPI1_DLPS
    if(PeriphSelectFlag & Bit_Periph_SPI1)
    {
        SPI1_DLPS_Enter();
    }
#endif
#if USE_SPI0_DLPS
    if(PeriphSelectFlag & Bit_Periph_SPI0)
    {
        SPI0_DLPS_Enter();
    }
#endif
#if USE_SPI2W_DLPS
    if(PeriphSelectFlag & Bit_Periph_SPI2W)
    {
    	SPI2W_DLPS_Enter();
    }
#endif
#if USE_KEYSCAN_DLPS
    if(PeriphSelectFlag & Bit_Periph_KEYSCAN)
    {
        KeyScan_DLPS_Enter();
    }
#endif
#if 0   /* reinit when exit dlps */
    if(PeriphSelectFlag & Bit_Periph_CODEC)
    {
        
    }
#endif
#if USE_GPIO_DLPS
    if(PeriphSelectFlag & Bit_Periph_GPIO)
    {
        GPIO_DLPS_Enter();
    }
#endif
#if USE_ADC_DLPS
    if(PeriphSelectFlag & Bit_Periph_ADC)
    {
        ADC_DLPS_Enter();
    }
#endif

    //Enable GPIO Wakeup Interrupt
    System_WakeUp_Intr_Enable();
}
#endif  /* USE_IO_DRIVER_DLPS */

#if USE_IO_DRIVER_DLPS
/**
  * @brief  IO exit dlps callback function.
  * @param  None
  * @retval None
  */
void DLPS_IO_ExitDlpsCb(void)
{

	//DBG_BUFFER(MODULE_APP, LEVEL_INFO, "DLPS_IO_ExitlpsCb",0);
	
    #if USE_READWAKEUPDATA_DLPS
    WakeupPinData_DLPS_Exit();
    #endif
    
    CPU_DLPS_Exit();
    
    Pinmux_DLPS_Exit();
	

#if USE_RTC_DLPS
    RTC_DLPS_Exit();
#endif
#if USE_I2C0_DLPS   
    if(PeriphSelectFlag & Bit_Periph_I2C0)
    {
        I2C0_DLPS_Exit();
    }
#endif
#if USE_I2C1_DLPS
    if(PeriphSelectFlag & Bit_Periph_I2C1)
    {
        I2C1_DLPS_Exit();
    }
#endif
#if USE_TIM_DLPS
    if(PeriphSelectFlag & Bit_Periph_TIMER)
    {
        TIM_DLPS_Exit();
    }
#endif
#if USE_QDECODER_DLPS
    if(PeriphSelectFlag & Bit_Periph_QDEC)
    {
        QuadDecoder_DLPS_Exit();
    }
#endif
#if USE_IR_DLPS
    if(PeriphSelectFlag & Bit_Periph_IR)
    {
        IR_DLPS_Exit();
    }
#endif
#if USE_GDMA_DLPS
    if(PeriphSelectFlag & Bit_Periph_GDMA)
    {
        GDMA_DLPS_Exit();
    }
#endif
#if USE_UART_DLPS
    if(PeriphSelectFlag & Bit_Periph_UART)
    {
        UART_DLPS_Exit();
    }
#endif
#if USE_SPI1_DLPS
    if(PeriphSelectFlag & Bit_Periph_SPI1)
    {
        SPI1_DLPS_Exit();
    }
#endif
#if USE_SPI0_DLPS
    if(PeriphSelectFlag&Bit_Periph_SPI0)
    {
        SPI0_DLPS_Exit();
    }
#endif
#if USE_SPI2W_DLPS
    if(PeriphSelectFlag & Bit_Periph_SPI2W)
   	{
		SPI2W_DLPS_Exit();
	}
#endif
#if USE_KEYSCAN_DLPS
    if(PeriphSelectFlag & Bit_Periph_KEYSCAN)
    {
        KeyScan_DLPS_Exit();
    }
#endif
#if 0   /* not use, codec will reinit when exit dlps */
    if(PeriphSelectFlag & Bit_Periph_CODEC)
        ;
#endif
#if USE_GPIO_DLPS
    if(PeriphSelectFlag & Bit_Periph_GPIO)
    {
        GPIO_DLPS_Exit();
    }
#endif
    
#if USE_ADC_DLPS
    if(PeriphSelectFlag & Bit_Periph_ADC)
    {
        ADC_DLPS_Exit();
    }
#endif

#if USE_USER_DEFINE_DLPS_EXIT_CB    
    if(User_IO_ExitDlpsCB) 
    {
        User_IO_ExitDlpsCB();
    }
#endif
	
	Log_SWD_DLPS_Exit();
	
}

/**
  * @brief  register IO DLPS callback function 
  * @param  None
  * @retval None
  */
void DLPS_IO_Register(void)
{
    DLPS_INTERRUPT_CONTROL_CB_REG(DLPS_IO_EnterDlpsCb, DLPS_ENTER);
    DLPS_INTERRUPT_CONTROL_CB_REG(DLPS_IO_ExitDlpsCb, DLPS_EXIT_ALL);

    NVIC_SetPriority(SYSTEM_ON_IRQ, 0);
    NVIC_EnableIRQ(SYSTEM_ON_IRQ);  //Enable SYSTEM_ON Interrupt

    return;
}
#endif /* USE_IO_DRIVER_DLPS */

/*********************************************************************************************/
#if USE_IO_DRIVER_DLPS
/**
  * @brief  CPU enter dlps callback function(Save CPU register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE void CPU_DLPS_Enter(void)
{
    CPU_StoreReg[0] = NVIC->ISER[0];
    CPU_StoreReg[1] = NVIC->ICER[0];
    CPU_StoreReg[2] = NVIC->IP[0];
    CPU_StoreReg[3] = NVIC->IP[1];
    CPU_StoreReg[4] = NVIC->IP[2];
    CPU_StoreReg[5] = NVIC->IP[3];
    CPU_StoreReg[6] = NVIC->IP[4];
    CPU_StoreReg[7] = NVIC->IP[5];
    CPU_StoreReg[8] = NVIC->IP[6];
    CPU_StoreReg[9] = NVIC->IP[7];

    Periph_StoreReg[0] = *((volatile uint32_t *)0x40008008);
    Periph_StoreReg[1] = *((volatile uint32_t *)0x4000800C);

    return;
}

/**
  * @brief  CPU exit dlps callback function(Resume CPU register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE  void CPU_DLPS_Exit(void)
{   
    NVIC->ISER[0]   = CPU_StoreReg[0];
    NVIC->ICER[0]   = CPU_StoreReg[1];
    NVIC->IP[0]     = CPU_StoreReg[2];
    NVIC->IP[1]     = CPU_StoreReg[3];
    NVIC->IP[2]     = CPU_StoreReg[4];
    NVIC->IP[3]     = CPU_StoreReg[5];
    NVIC->IP[4]     = CPU_StoreReg[6];
    NVIC->IP[5]     = CPU_StoreReg[7];
    NVIC->IP[6]     = CPU_StoreReg[8];
    NVIC->IP[7]     = CPU_StoreReg[9];

    *((volatile uint32_t *)0x40000230) |= BIT(6);
    
    *((volatile uint32_t *)0x40008008) = Periph_StoreReg[0];
    *((volatile uint32_t *)0x4000800C) = Periph_StoreReg[1];

    return;
}

/**
  * @brief  PINMUX enter dlps callback function(Save PINMUX register values when system enter DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE  void Pinmux_DLPS_Enter(void)
{
    uint8_t i = 0;
    
    for(i = 0; i < 10; i++)
    {
        Pinmux_StoreReg[i] = PINMUX->CFG[i];
    }

    return;
}

/**
  * @brief  PINMUX exit dlps callback function(Resume PINMUX register values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE  void Pinmux_DLPS_Exit(void)
{
    uint8_t i;
    
    for(i = 0; i < 10; i++)
    {
        PINMUX->CFG[i] = Pinmux_StoreReg[i];
    }
    
    return;
}

#if USE_READWAKEUPDATA_DLPS
/********************************************** ********************************************************/
/*********************************** [ Exit DLPS Read All Wakeup Pin level FUNC] ******************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
__STATIC_INLINE  void WakeupPinData_DLPS_Exit(void);
uint32_t WakeupPinLevel = 0;

/**
  * @brief  Exit dlps callback function(Read all wakeup GPIO values when system exit DLPS)
  * @param  None
  * @retval None
  */
__STATIC_INLINE  void WakeupPinData_DLPS_Exit(void)
{
    uint8_t pin_num = 0;
    
    #if USE_KEYSCAN_DLPS
    Pinmux_DLPS_Exit();
    KeyScan_DLPS_Exit();
    #endif
    
    for(pin_num = 8;pin_num<=10; pin_num++)
    {
        Pinmux_Config(pin_num, GPIO_FUN);
    }
    
    for(pin_num = 16;pin_num<=17; pin_num++)
    {
        Pinmux_Config(pin_num, GPIO_FUN);
    }
    
    for(pin_num = 20;pin_num<=27; pin_num++)
    {
        Pinmux_Config(pin_num, GPIO_FUN);
    }    
    Pinmux_Config(35, GPIO_FUN);
    
    WakeupPinLevel |= GPIO->DATAIN;  
}

/**
  * @brief  Get Wakeup pin GPIO status when system exit dlps.
  * @param  None
  * @retval the Wakeup pin GPIO status.
  */
__STATIC_INLINE uint32_t DLPS_ReadWakeupPinData(void)
{
    return WakeupPinLevel;
}
#endif

#endif /* USE_IO_DRIVER_DLPS */


