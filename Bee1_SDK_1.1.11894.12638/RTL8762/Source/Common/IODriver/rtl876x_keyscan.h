/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_keyscan.h
* @brief     header file of keyscan driver.
* @details   
* @author    tifnan_ge
* @date      2015-04-29
* @version   v0.1
* *********************************************************************************************************
*/


#ifndef _RTL876X_KEYSCAN_H_
#define _RTL876X_KEYSCAN_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
     
/** @addtogroup RTK_Periph_Driver
  * @{
  */

/** @addtogroup KeyScan KeyScan
  * @brief Keyscan driver module
  * @{
  */

/** @defgroup KeyScan_Exported_Types KeyScan Exported Types
  * @{
  */

/**
 * @brief keyscan initialize parameters
 *
 * keyscan initialize parameters
 */
typedef struct
{
    uint16_t        rowSize;
    uint16_t        colSize;
    uint32_t        detectPeriod;       //ms
    uint16_t        timeout;            //sw interrupt timeout  (s)
    uint16_t        scanInterval;       //ms
    uint16_t        debounceEn;         //debounce enable or disable
    uint16_t        debounceTime;       //useful when debounceEn == KeyScan_Debounce_Enable. really debounce time = 31.25us * debounceTime
    uint16_t        detectMode;
    uint16_t        fifoOvrCtrl;
    uint16_t        maxScanData;        //max scan data allowable in each scan
}KEYSCAN_InitTypeDef, *PKEYSCAN_InitTypeDef;

/**
  * @}
  */

/** @defgroup KeyScan_Exported_Constants KeyScan Exported Constants
  * @{
  */

#define IS_KeyScan_PERIPH(PERIPH) ((PERIPH) == KEYSCAN)

/** @defgroup Keyscan_Press_Detect_Mode Keyscan Press Detect Mode 
  * @{
  */

#define KeyScan_Detect_Mode_Edge        ((uint16_t)(0 << 7))   
#define KeyScan_Detect_Mode_Level       ((uint16_t)(1 << 7)) 

#define IS_KEYSCAN_DETECT_MODE(MODE)    (((MODE) == KeyScan_Detect_Mode_Edge) || ((MODE) == KeyScan_Detect_Mode_Level))

/**
  * @}
  */

/** @defgroup Keyscan_Fifo_Overflow_Control Keyscan Fifo Overflow Control 
  * @{
  */

#define KeyScan_FIFO_OVR_CTRL_DIS_ALL   ((uint16_t)(0 << 6))   //discard all the new scan data when FIFO is full
#define KeyScan_FIFO_OVR_CTRL_DIS_LAST  ((uint16_t)(1 << 6))   //discard the last scan data when FIFO is full

#define IS_KEYSCAN_FIFO_OVR_CTRL(CTRL)  (((CTRL) == KeyScan_FIFO_OVR_CTRL_DIS_ALL) || ((CTRL) == KeyScan_FIFO_OVR_CTRL_DIS_LAST))

/**
  * @}
  */


/** @defgroup Keyscan_Interrupt_Definition Keyscan Interrupt Definition 
  * @{
  */

#define KEYSCAN_INT_TIMEOUT                    ((uint16_t)(1 << 2))    //int3
#define KEYSCAN_INT_OVER_THRESHOLD             ((uint16_t)(1 << 1))    //int2
#define KEYSCAN_INT_SCAN_END                   ((uint16_t)(1 << 0))    //int1  priority: int1 > int2 > int3

#define IS_KEYSCAN_CONFIG_IT(IT) ((((IT) & (uint32_t)0xFFF8) == 0x00) && ((IT) != 0x00))

/**
  * @}
  */

/** 
  * @defgroup  Keyscan_Flags Keyscan Flags
  * @{
  */

#define KEYSCAN_FLAG_REJECT         ((uint16_t)(1 << 7))
#define KEYSCAN_FLAG_OVR            ((uint16_t)(1 << 6))
#define KEYSCAN_FLAG_FULL           ((uint16_t)(1 << 5))
#define KEYSCAN_FLAG_EMPTY          ((uint16_t)(1 << 4))

#define IS_KEYSCAN_FLAG(FLAG)       ((((FLAG) & (uint32_t)0x01FF) == 0x00) && ((FLAG) != (uint32_t)0x00))
#define IS_KEYSCAN_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0x00C0) == 0x00) && ((FLAG) != (uint32_t)0x00))

/**
  * @}
  */

/** 
  * @defgroup  Keyscan_Scan_Status_Flags Keyscan Scan Status Flags
  * @{
  */

#define KEYSCAN_SCAN_ST_IDLE        ((uint16_t)0x00)
#define KEYSCAN_SCAN_ST_INIT        ((uint16_t)0x01)
#define KEYSCAN_SCAN_ST_SCAN        ((uint16_t)0x02)
#define KEYSCAN_SCAN_ST_SCAN_END    ((uint16_t)0x03)
#define KEYSCAN_SCAN_ST_WAIT        ((uint16_t)0x04)
#define KEYSCAN_SCAN_ST_TIMEOUT     ((uint16_t)0x05)

#define IS_KEYSCAN_SCAN_STATE(ST)   (((ST) == KEYSCAN_SCAN_ST_IDLE) || ((ST) == KEYSCAN_SCAN_ST_INIT)||\
                                     ((ST) == KEYSCAN_SCAN_ST_SCAN) || ((ST) == KEYSCAN_SCAN_ST_SCAN_END)||\
                                     ((ST) == KEYSCAN_SCAN_ST_WAIT) || ((ST) == KEYSCAN_SCAN_ST_TIMEOUT))
 
/**
  * @}
  */

/** @defgroup Keyscan_Row_Number Keyscan Row Number
  * @{
  */

#define IS_KEYSCAN_ROW_NUM(ROW) ((ROW) <= 8)
/**
  * @}
  */

/** @defgroup Keyscan_Column_Number Keyscan Column Number
  * @{
  */

#define IS_KEYSCAN_COL_NUM(COL) ((COL) <= 20)
/**
  * @}
  */

/** @defgroup Keyscan_Detect_Period(ms) Keyscan Detect Period(ms)
  * @{
  */
#define KeyScan_Detect_Period_Disable           ((uint32_t)(7 << 8))
#define KeyScan_Detect_Period_5ms               ((uint32_t)(0 << 8))
#define KeyScan_Detect_Period_10ms              ((uint32_t)(1 << 8))
#define KeyScan_Detect_Period_20ms              ((uint32_t)(2 << 8))
#define KeyScan_Detect_Period_40ms              ((uint32_t)(3 << 8))
#define KeyScan_Detect_Period_80ms              ((uint32_t)(4 << 8))
#define KeyScan_Detect_Period_160ms             ((uint32_t)(5 << 8))
#define KeyScan_Detect_Period_320ms             ((uint32_t)(6 << 8))

#define IS_KEYSCAN_DETECT_PERIOD(DET_PER) (((DET_PER) == KeyScan_Detect_Period_Disable) || ((DET_PER) == KeyScan_Detect_Period_5ms)\
                                            || ((DET_PER) == KeyScan_Detect_Period_10ms) || ((DET_PER) == KeyScan_Detect_Period_20ms)\
                                            || ((DET_PER) == KeyScan_Detect_Period_40ms) || ((DET_PER) == KeyScan_Detect_Period_80ms)\
                                            || ((DET_PER) == KeyScan_Detect_Period_160ms) || ((DET_PER) == KeyScan_Detect_Period_320ms))
/**
  * @}
  */

/** @defgroup Keyscan_Interrupt_Timeout(ms) Keyscan Interrupt Timeout(ms)
  * @{
  */
#define KeyScan_Timeout_1s                      ((uint16_t)(0 << 2))
#define KeyScan_Timeout_2s                      ((uint16_t)(1 << 2))
#define KeyScan_Timeout_4s                      ((uint16_t)(2 << 2))
#define KeyScan_Timeout_8s                      ((uint16_t)(3 << 2))

#define IS_KEYSCAN_TIMEOUT(TIMEOUT) (((TIMEOUT) == KeyScan_Timeout_1s)\
                                            || ((TIMEOUT) == KeyScan_Timeout_2s)\
                                            || ((TIMEOUT) == KeyScan_Timeout_4s)\
                                            || ((TIMEOUT) == KeyScan_Timeout_8s))  //s
/**
  * @}
  */

/** @defgroup Keyscan_Scan_Interval(ms) Keyscan Scan Interval(ms)
  * @{
  */
#define KeyScan_Scan_Interval_12_5ms            ((uint16_t)(0 << 0))
#define KeyScan_Scan_Interval_25ms              ((uint16_t)(1 << 0))
#define KeyScan_Scan_Interval_50ms              ((uint16_t)(2 << 0))
#define KeyScan_Scan_Interval_100ms             ((uint16_t)(3 << 0))

#define IS_KEYSCAN_INTERVAL(INTERVAL) (((INTERVAL) == KeyScan_Scan_Interval_12_5ms)\
                                        || ((INTERVAL) == KeyScan_Scan_Interval_25ms)\
                                        || ((INTERVAL) == KeyScan_Scan_Interval_50ms)\
                                        || ((INTERVAL) == KeyScan_Scan_Interval_100ms))    //ms
/**
  * @}
  */

/** @defgroup Keyscan_Debounce_Time(ms) Keyscan Debounce Time(ms)
  * @{
  */

#define IS_KEYSCAN_DEBOUNCE_TIME(DEBOUNCE_TIME) (((DEBOUNCE_TIME) >= 1) && ((DEBOUNCE_TIME) <= 0x7FFF))    // max: 0x7fff * 31.25us /1000 = 64ms
/**
  * @}
  */

/** @defgroup Keyscan_DebounceTime(ms) Keyscan DebounceTime(ms)
  * @{
  */
  
#define IS_KEYSCAN_MAX_SCAN_DATA(DATA_NUM) ((DATA_NUM) <= 26)    //0 means no limit
/**
  * @}
  */

/** @defgroup Keyscan_Debounce_Config Keyscan Debounce Config
  * @{
  */
  
#define KeyScan_Debounce_Enable              ((uint16_t)(1 << 7))
#define KeyScan_Debounce_Disable             ((uint16_t)(0 << 7)) 
#define IS_KEYSCAN_DEBOUNCE_EN(EN) (((EN) == KeyScan_Debounce_Enable) || ((EN) == KeyScan_Debounce_Disable))    //0 means no limit
/**
  * @}
  */

/** @cond private
  * @defgroup Keyscan_FIFO_AVALIABLE_MASK
  * @{
  */
  
#define STATUS_FIFO_DATA_NUM_MASK           ((uint32_t)(0x1F << 8))

/**
  * @}
  * @endcond
  */

/** @cond private
  * @defgroup Keyscan_CURRENT_STATUS_MASK
  * @{
  */
#define STATUS_CURRENT_STATUS_MASK          ((uint32_t)(0x07 << 0))

/**
  * @}
  * @endcond
  */

/**
  * @}
  */

/** @defgroup Keyscan_Exported_Variables
  * @{
  */

/**
  * @}
  */

/** @defgroup Keyscan_Exported_Functions Keyscan Exported Functions
  * @{
  */

extern void KeyScan_Init(KEYSCAN_TypeDef* KeyScan, KEYSCAN_InitTypeDef* KeyScan_InitStruct);
extern void KeyScan_DeInit(KEYSCAN_TypeDef* KeyScan);
extern void KeyScan_StructInit(KEYSCAN_TypeDef* KeyScan, KEYSCAN_InitTypeDef* KeyScan_InitStruct);
extern void KeyScan_INTConfig(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_IT, FunctionalState newState);
extern void KeyScan_INTMask(KEYSCAN_TypeDef* KeyScan, FunctionalState newState);
extern void KeyScan_Read(KEYSCAN_TypeDef* KeyScan, uint8_t* outBuf, uint16_t count);
extern void KeyScan_Cmd(KEYSCAN_TypeDef* KeyScan, FunctionalState NewState);
__STATIC_INLINE uint16_t KeyScan_GetFifoDataNum(KEYSCAN_TypeDef* KeyScan);
__STATIC_INLINE void KeyScan_ClearINTPendingBit(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_IT);
__STATIC_INLINE void KeyScan_ClearFlags(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_FLAG);
__STATIC_INLINE FlagStatus KeyScan_GetFlagState(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_FLAG);
__STATIC_INLINE uint16_t KeyScan_GetCurState(KEYSCAN_TypeDef* KeyScan);

/**
  * @brief  Get KeyScan FIFO data num.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval number of data in FIFO.
  */
__STATIC_INLINE uint16_t KeyScan_GetFifoDataNum(KEYSCAN_TypeDef* KeyScan)
{
    return (uint16_t)((KeyScan->STATUS & STATUS_FIFO_DATA_NUM_MASK) >> 8);
}

/**
  * @brief  Clears the KEyScan interrupt pending bits.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_IT: specifies the interrupt pending bit to clear. 
  *   This parameter can be any combination of the following values:
  *     @arg KEYSCAN_INT_TIMEOUT: KeyScan timeout interrupt mask
  *     @arg KEYSCAN_INT_OVER_THRESHOLD: Kescan FIFO data over threshold interrupt mask
  *     @arg KEYSCAN_INT_SCAN_END: KeyScan scan end interrupt mask
  * @retval None
  */
__STATIC_INLINE void KeyScan_ClearINTPendingBit(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_IT)
{ 
    /* Check the parameters */
    assert_param(IS_KEYSCAN_CONFIG_IT(KeyScan_IT));

    KeyScan->INTCR |= (KeyScan_IT << 12);

    return;
}

/**
  * @brief  Clears KeyScan pending flags.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_FLAG: specifies the flag to clear. 
  *   This parameter can be one of the following values:
  *     @arg KEYSCAN_FLAG_REJECT: KeyScan reject status(any scan data is rejected or not)
  *     @arg KEYSCAN_FLAG_OVR: FIFO is overflow
  *
  * @note
  *   - KEYSCAN_FLAG_FULL and KEYSCAN_FLAG_EMPTY can't be cleared manually. They
  *     are cleared by hardware automatically.
  */
__STATIC_INLINE void KeyScan_ClearFlags(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_FLAG)
{ 
    /* Check the parameters */
    assert_param(IS_KEYSCAN_CLEAR_FLAG(KeyScan_FLAG));

    KeyScan->CR2|= (KeyScan_FLAG << 6);

    return;
}

/**
  * @brief  Checks whether the specified KeyScan flag is set or not.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_FLAG: specifies the flag to check. 
  *   This parameter can be one of the following values:
  *     @arg KEYSCAN_FLAG_REJECT: KeyScan reject status(any scan data is rejected or not)
  *     @arg KEYSCAN_FLAG_OVR: FIFO is overflow
  *     @arg KEYSCAN_FLAG_FULL: FIFO is full
  *     @arg KEYSCAN_FLAG_EMPTY: FIFO is empty
  * @retval The new state of KeyScan_FLAG (SET or RESET).
  */
__STATIC_INLINE FlagStatus KeyScan_GetFlagState(KEYSCAN_TypeDef* KeyScan, uint32_t KeyScan_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* Check the parameters */
    assert_param(IS_KEYSCAN_FLAG(KeyScan_FLAG));

    if((KeyScan->STATUS & KeyScan_FLAG) != 0)
    {
        bitstatus = SET;
    }

    return bitstatus;
}

/**
  * @brief  Get KeyScan current state.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval The KeyScan current state, must one of the following values: 
        @arg KEYSCAN_SCAN_ST_IDLE: KeyScan is idle
  *     @arg KEYSCAN_SCAN_ST_INIT: KeyScan is initializating
  *     @arg KEYSCAN_SCAN_ST_SCAN: Scaning now.
  *     @arg KEYSCAN_SCAN_ST_SCAN_END: Scan has end.
  *     @arg KEYSCAN_SCAN_ST_WAIT: Wait state
  *     @arg KEYSCAN_SCAN_ST_TIMEOUT: Wait timeout
  */
__STATIC_INLINE uint16_t KeyScan_GetCurState(KEYSCAN_TypeDef* KeyScan)
{
    return (uint16_t)(KeyScan->STATUS & STATUS_CURRENT_STATUS_MASK);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_KEYSCAN_H_ */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor *****END OF FILE****/


