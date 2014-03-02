/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Private Header file of SKE
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __SKE_IRQ_HEADER
#define __SKE_IRQ_HEADER

#ifdef __cplusplus
extern "C"
{
#endif


#include "hcl_defs.h"

typedef enum
{
    SKE_IRQ_SCROLL_DEVICE_0 = 0x1,
    SKE_IRQ_SCROLL_DEVICE_1 = 0x2,
    SKE_IRQ_KEYPAD_AUTOSCAN = 0x4,
    SKE_IRQ_KEYPAD_SOFTSCAN = 0x8  
}t_ske_irq_src_id;

/* For SKE interrupt source it can be either 
   directly the ske_irq_src_id or combined(by ORing required ske_irq_src_ids)*/
typedef t_uint32 t_ske_irq_src;


/* ----- Interrupt Management M0 functions ------- */

PUBLIC void SKE_SetBaseAddress(IN t_logical_address ske_base_address);

/* For ske_irq_src(as input), either individual ske_irq_src_id or 
   combined interrupt source(by bitwise ORing with required ske_irq_src_id)
   can be given */
   
PUBLIC void SKE_EnableIRQSrc(IN t_ske_irq_src ske_irq_src);
PUBLIC void SKE_DisableIRQSrc(IN t_ske_irq_src ske_irq_src);

/* SKE_GetIRQSrc returns combined interrupt source,
   return should be bitwise ANDed with required ske_irq_src_id and checked if not zero */
PUBLIC t_ske_irq_src SKE_GetIRQSrc(void);
PUBLIC void SKE_ClearIRQSrc(IN t_ske_irq_src ske_irq_src);
PUBLIC t_bool SKE_IsPendingIRQSrc(IN t_ske_irq_src ske_irq_src);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif /* __SKE_IRQ_HEADER */


