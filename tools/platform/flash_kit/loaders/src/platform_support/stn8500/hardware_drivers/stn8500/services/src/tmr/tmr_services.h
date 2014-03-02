
/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file TIMER Services
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _TMR_SERVICES
#define _TMR_SERVICES

#include "hcl_defs.h"
#include "memory_mapping.h"
#include "tmr.h"
#include "tmr_irq.h"
#include "services.h"

/* DEFINES */

#define TMR_UNIT_0_ADDR MTU_0_REG_BASE_ADDR
#define TMR_UNIT_1_ADDR MTU_1_REG_BASE_ADDR


#if defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)|| defined(ST_8500V2) || defined(ST_HREFV2)

#define PRCC_KERNEL_CLK_EN_OFFSET 0x8

#define MTU0_AMBA_CLK_EN_VAL      0x00000080

#define MTU1_AMBA_CLK_EN_VAL      0x00000100



#endif



#define NUM_TIMER       8
/* Typedefs */
typedef struct
{
    t_tmr_id    timer_id;
} t_ser_tmr_param;

/* Function Prototypes */
PUBLIC t_tmr_id     SER_TMR_RequestPeriodicCallback(t_uint32, t_callback_fct, void *, t_tmr_clk_prescaler);
PUBLIC t_tmr_id     SER_TMR_RequestOneShotCallback(t_uint32, t_callback_fct, void *, t_tmr_clk_prescaler);
PUBLIC t_ser_error  SER_TMR_ReleaseOneShotCallback(t_tmr_req_id);
PUBLIC void         SER_TMR_Init(t_uint8 default_ser_mask);
PUBLIC void         SER_TMR_Close(void);
PUBLIC t_sint32     SER_TMR_ConnectonTimer(t_tmr_id, t_callback_fct, void *);
PUBLIC void         SER_TMR_InstallDefaultHandler(void);
PUBLIC void         SER_TMR_WaitEnd(t_tmr_id tmrid);
#endif /* _TMR_SERVICES */

