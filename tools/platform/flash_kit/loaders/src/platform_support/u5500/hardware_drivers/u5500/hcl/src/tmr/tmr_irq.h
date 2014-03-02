/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the TIMER(MTU) 
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _TMR_IRQ_H_
#define _TMR_IRQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * New types
 *----------------------------------------------------------------------*/
typedef enum
{
    TMR_DEVICE_ID_INVALID   = -1,
    TMR_DEVICE_ID_0         = 0,
    TMR_DEVICE_ID_1         = 1
   
} t_tmr_device_id;

typedef enum
{
    TMR_ID_0                = 0x00000001,
    TMR_ID_1                = 0x00000002,
    TMR_ID_2                = 0x00000004,
    TMR_ID_3                = 0x00000008,
    TMR_ID_4                = 0x00000010,
    TMR_ID_5                = 0x00000020,
    TMR_ID_6                = 0x00000040,
    TMR_ID_7                = 0x00000080
    
} t_tmr_id;

typedef enum
{
    TMR_IRQ_SRC_ID_0        = TMR_ID_0,
    TMR_IRQ_SRC_ID_1        = TMR_ID_1,
    TMR_IRQ_SRC_ID_2        = TMR_ID_2,
    TMR_IRQ_SRC_ID_3        = TMR_ID_3,
    TMR_IRQ_SRC_ID_4        = TMR_ID_4,
    TMR_IRQ_SRC_ID_5        = TMR_ID_5,
    TMR_IRQ_SRC_ID_6        = TMR_ID_6,
    TMR_IRQ_SRC_ID_7        = TMR_ID_7
    
} t_tmr_irq_src_id;

typedef t_uint32    t_tmr_irq_src;

/*------------------------------------------------------------------------
 * Local Functions
 *----------------------------------------------------------------------*/
void                TMR_SetBaseAddress(t_tmr_device_id tmr_device_id, t_logical_address tmr_base_address);
t_tmr_irq_src       TMR_GetIRQSrc(t_tmr_device_id tmr_device_id);
void                TMR_EnableIRQSrc(t_tmr_irq_src tmr_irq_src);
void                TMR_DisableIRQSrc(t_tmr_irq_src tmr_irq_src);
void                TMR_ClearIRQSrc(t_tmr_irq_src tmr_irq_src);
t_tmr_device_id     TMR_GetDeviceID(t_tmr_irq_src tmr_irq_src);
t_bool              TMR_IsPendingIRQSrc(t_tmr_irq_src tmr_irq_src);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* #ifndef _TMR_IRQ_H_ */

/* End of file - tmr_irq.h*/

