/**
****************************************************************************
* @file     interrupt.h
* @brief    all verctor table definition here
* @author
* @version  Initial Draft
* @date     2017/05/18
* @note
* detailed description:

******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2017 maxscend </center></h2>
*/


#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

typedef void (*NMI_FAULT_IRQ_FUNC)(uint32 , uint32 , uint8 );
typedef void (*SYS_IRQ_FUNC)(void);
typedef void (*IRQ_FUNC)(void);

extern IRQ_FUNC user_irq_func_table[32];
extern SYS_IRQ_FUNC sys_irq_func_table[4];

#endif

