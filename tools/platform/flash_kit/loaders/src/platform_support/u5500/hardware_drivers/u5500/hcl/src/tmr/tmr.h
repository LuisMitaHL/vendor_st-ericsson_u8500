/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file for the TIMER(MTU)
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _TMR_H_
#define _TMR_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "debug.h"

#include "tmr_irq.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
typedef t_uint32    t_tmr_event;
typedef t_uint32    t_tmr_filter_mode;

#define TMR_NO_FILTER_MODE  0

/*------------------------------------------------------------------------
 * Data types
 *----------------------------------------------------------------------*/
typedef enum
{
    TMR_ERROR_NO_MORE_TIMER_AVAILABLE   = HCL_MAX_ERROR_VALUE - 6,
    TMR_ERROR_INVALID_TIMER_ID          = HCL_MAX_ERROR_VALUE - 5,
    TMR_ERROR_TIMER_ALREADY_STOPPED     = HCL_MAX_ERROR_VALUE - 4,
    TMR_ERROR_TIMER_ALREADY_STARTED     = HCL_MAX_ERROR_VALUE - 3,
    TMR_ERROR_MAX_RUNNING_TIMER_REACHED = HCL_MAX_ERROR_VALUE - 2,
    TMR_ERROR_INVALID_STATE             = HCL_MAX_ERROR_VALUE - 1,
    TMR_ERROR_NOT_ONE_SHOT              = HCL_MAX_ERROR_VALUE - 0,
    TMR_INTERNAL_ERROR                  = HCL_INTERNAL_ERROR,
    TMR_NOT_CONFIGURED                  = HCL_NOT_CONFIGURED,
    TMR_REQUEST_PENDING                 = HCL_REQUEST_PENDING,
    TMR_REQUEST_NOT_APPLICABLE          = HCL_REQUEST_NOT_APPLICABLE,
    TMR_INVALID_PARAMETER               = HCL_INVALID_PARAMETER,
    TMR_UNSUPPORTED_FEATURE             = HCL_UNSUPPORTED_FEATURE,
    TMR_UNSUPPORTED_HW                  = HCL_UNSUPPORTED_HW,
    TMR_ERROR                           = HCL_ERROR,
    TMR_OK                              = HCL_OK,
    TMR_INTERNAL_EVENT                  = HCL_INTERNAL_EVENT,
    TMR_REMAINING_PENDING_EVENTS        = HCL_REMAINING_PENDING_EVENTS,
    TMR_REMAINING_FILTER_PENDING_EVENTS = HCL_REMAINING_FILTER_PENDING_EVENTS,
    TMR_NO_MORE_PENDING_EVENT           = HCL_NO_MORE_PENDING_EVENT,
    TMR_NO_MORE_FILTER_PENDING_EVENT    = HCL_NO_MORE_FILTER_PENDING_EVENT,
    TMR_NO_PENDING_EVENT_ERROR          = HCL_NO_PENDING_EVENT_ERROR
} t_tmr_error;

typedef enum
{
    TMR_MODE_FREE_RUNNING               = 0,
    TMR_MODE_PERIODIC                   = 1,
    TMR_MODE_ONE_SHOT                   = 2
} t_tmr_mode;

typedef enum
{
    TMR_CLK_PRESCALER_1,
    TMR_CLK_PRESCALER_16,
    TMR_CLK_PRESCALER_256
} t_tmr_clk_prescaler;

typedef enum
{
    TMR_IRQ_STATE_NEW,
    TMR_IRQ_STATE_OLD
} t_tmr_irq_state;

typedef struct
{
    t_tmr_irq_state interrupt_state;
    t_uint8         padding[3];
    t_tmr_irq_src   initial_irq;
    t_tmr_irq_src   pending_irq;
} t_tmr_irq_status;

/*------------------------------------------------------------------------
 * Public Functions
 *----------------------------------------------------------------------*/
t_tmr_error TMR_Init(t_tmr_device_id tmr_device_id, t_logical_address tmr_base_address);
t_tmr_error TMR_Reset(t_tmr_device_id tmr_device_id);
t_tmr_error TMR_AllocTimer(t_tmr_id *p_tmr_id);

t_tmr_error TMR_Configure(t_tmr_id tmr_id, t_tmr_mode tmr_mode, t_uint32 load_value, t_tmr_clk_prescaler tmr_clk_prescaler);
t_tmr_error TMR_StartTimer(t_tmr_id tmr_id);
t_tmr_error TMR_LoadBackGround(t_tmr_id tmr_id, t_uint32 load_value);
t_tmr_error TMR_StopTimer(t_tmr_id tmr_id);
t_tmr_error TMR_ReStartTimer(t_tmr_id tmr_id, t_uint32 load_value, t_tmr_clk_prescaler tmr_clk_prescaler);
t_tmr_error TMR_GetCounterValue(t_tmr_id tmr_id, t_uint32 *p_current_value);
t_tmr_error TMR_FreeTimer(t_tmr_id tmr_id);

t_tmr_error TMR_SetDbgLevel(t_dbg_level dbg_level);
t_tmr_error TMR_GetDbgLevel(t_dbg_level *dbg_level);

void        TMR_SaveDeviceContext(void);
void        TMR_RestoreDeviceContext(void);

t_tmr_error TMR_GetVersion(t_version *p_version);

void        TMR_GetIRQSrcStatus(t_tmr_irq_src tmr_irq_src, t_tmr_irq_status *p_status);
t_tmr_error TMR_FilterProcessIRQSrc(t_tmr_irq_status *p_tmr_irq_status, t_tmr_event *p_tmr_event, t_tmr_filter_mode filter_mode);
t_bool      TMR_IsEventActive(const t_tmr_irq_src *p_event);
void        TMR_AcknowledgeEvent(const t_tmr_event *p_event);
#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* #ifndef _HCL_TMR_H_ */

/* End of file - tmr.h*/

