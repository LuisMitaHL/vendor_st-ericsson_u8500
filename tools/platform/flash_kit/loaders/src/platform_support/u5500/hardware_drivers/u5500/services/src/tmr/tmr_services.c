
/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides initialization services for timer
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include <stdlib.h>

#include "services.h"

#include "tmr_services.h"
#include "gic.h"
#include "tmr.h"
#include "tmr_irq.h"
#include "memory_mapping.h"

/*--------------------------------------------------------------------------*
 *  GLOBAL VARIABLES                                                        *
 *--------------------------------------------------------------------------*/
/* Total No of Timers */

t_callback          g_timer_callback[NUM_TIMER] =
{
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 }
};
t_bool            g_timer_expired[NUM_TIMER] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};


/*--------------------------------------------------------------------------*
 * PRIVATE FUNCTION DECLARATION                                             *
 *--------------------------------------------------------------------------*/
PRIVATE t_uint32    ser_tmr_GetIndex(t_tmr_id tmr_id);
PRIVATE void        SER_TMR0_Handler(t_uint32 irq);
PRIVATE void        SER_TMR1_Handler(t_uint32 irq);

/*--------------------------------------------------------------------------*
 * PRIVATE FUNCTIONS                                                        *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_TMR0_Handler                              	                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for DTU0   	          	*/
/* 																            */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE void ser_tmr_InterruptHandler(t_tmr_device_id tmr_device_id)
{
    t_tmr_error         tmr_error;
    t_tmr_irq_status    tmr_irq_status;
    t_tmr_irq_src       tmr_irq_src;
    t_tmr_event         tmr_event;
    t_bool              done;
    t_bool              reenable = FALSE;
    t_gic_error         gic_error;
    t_ser_tmr_param     ser_tmr_param;

    t_gic_line          gic_line_for_tmr;

    switch (tmr_device_id)
    {
        case TMR_DEVICE_ID_0:
        default:
            gic_line_for_tmr = GIC_TIMER_0_LINE;
            break;

        case TMR_DEVICE_ID_1:
            gic_line_for_tmr = GIC_TIMER_1_LINE;
            break;

    };

    tmr_irq_src = TMR_GetIRQSrc(tmr_device_id);
    TMR_GetIRQSrcStatus(tmr_irq_src, &tmr_irq_status);

    gic_error = GIC_DisableItLine(gic_line_for_tmr);
    if (GIC_OK != gic_error)
    {
        PRINT
        (
            "GIC_DisableItLine failed for GIC_TIMER_%d_LINE, error [%d]",
            gic_line_for_tmr == GIC_TIMER_0_LINE ? 0 : 1,
            gic_error
        );
        exit(-1);
    }

  /*  gic_error = GIC_AcknowledgeItLine(gic_line_for_tmr);
    if (GIC_OK != gic_error)
    {
        PRINT
        (
            "GIC_AcknowledgeItLine failed for GIC_TIMER_%d_LINE, error [%d]",
            gic_line_for_tmr == GIC_TIMER_0_LINE ? 0 : 1,
            gic_error
        );
        exit(-1);
    } */

    do
    {
        tmr_error = TMR_FilterProcessIRQSrc(&tmr_irq_status, &tmr_event, TMR_NO_FILTER_MODE);
        done = FALSE;
        switch (tmr_error)
        {
            case TMR_NO_PENDING_EVENT_ERROR:
                done = TRUE;
                break;

            case TMR_NO_MORE_PENDING_EVENT:
                reenable = TRUE;
                done = TRUE;
				break;
				
				
            case TMR_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case TMR_REMAINING_PENDING_EVENTS:
                if (NULL != g_timer_callback[ser_tmr_GetIndex((t_tmr_id) tmr_event)].fct)
                {
                    ser_tmr_param.timer_id = (t_tmr_id) tmr_event;
                    g_timer_callback[ser_tmr_GetIndex((t_tmr_id) tmr_event)].fct
                        (
                            g_timer_callback[ser_tmr_GetIndex((t_tmr_id) tmr_event)].param,
                            &ser_tmr_param
                        );
                }

                TMR_AcknowledgeEvent(&tmr_event);
                g_timer_expired[ser_tmr_GetIndex((t_tmr_id) tmr_event)] = TRUE;
                break;

            case TMR_INTERNAL_EVENT:
                break;

            default:
                break;
        }
    } while (!done);

    if (reenable)
    {
        gic_error = GIC_EnableItLine(gic_line_for_tmr);
        if (GIC_OK != gic_error)
        {
            PRINT
            (
                "GIC_EnableItLine failed for GIC_TIMER_%d_LINE, error [%d]",
                gic_line_for_tmr == GIC_TIMER_0_LINE ? 0 : 1,
                gic_error
            );
            exit(-1);
        }
    }
}

/*--------------------------------------------------------------------------*
 * PUBLIC FUNCTIONS                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_TMR0_Handler                              	                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for MTU0   	          	*/
/* 																            */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE void SER_TMR0_Handler(t_uint32 irq)
{
    ser_tmr_InterruptHandler(TMR_DEVICE_ID_0);
}

/****************************************************************************/
/* NAME:    SER_TMR1_Handler                              	                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for MTU1   	          	*/
/* 																            */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE void SER_TMR1_Handler(t_uint32 irq)
{
    ser_tmr_InterruptHandler(TMR_DEVICE_ID_1);
}

/****************************************************************************/
/* NAME:    ser_tmr_GetIndex                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is a hash function ,it converts timer id to array index*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE t_uint32 ser_tmr_GetIndex(t_tmr_id tmr_id)
{
    t_uint32    i;
    t_uint32    value = TMR_ID_0;

    for (i = 0; value != tmr_id; i++)
    {
        value <<= 1;
    }

    return(i);
}

/*--------------------------------------------------------------------------*
 * PUBLIC FUNCTIONS                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_TMR_RequestPeriodicCallback                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures an available timer to call			*/
/* periodically the fct callback function with the given parameter 			*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: t_ser_error		                                                */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_tmr_id SER_TMR_RequestPeriodicCallback
(
    t_uint32            period,
    t_callback_fct      fct,
    void                *param,
    t_tmr_clk_prescaler tmr_clk_prescaler
)
{
    t_tmr_error tmr_error;
    t_tmr_id    timer_id = TMR_ID_0;

    /* Allocate The Timer If Available */
    tmr_error = TMR_AllocTimer(&timer_id);
    if (TMR_OK == tmr_error)
    {
        /* Initialize Periodic Timer */
        tmr_error = TMR_Configure(timer_id, TMR_MODE_PERIODIC, period, tmr_clk_prescaler);

        /* Bind the callback function */
        SER_TMR_ConnectonTimer(timer_id, fct, param);
        tmr_error = TMR_StartTimer(timer_id);
        if (TMR_OK != tmr_error)
        {
            PRINT("TMR_StartTimer failed for periodic timer, error [%d]", tmr_error);
            exit(-1);
        }
    }
    else
    {
        PRINT("TMR_AllocTimer failed for periodic timer, error [%d]", tmr_error);
        exit(-1);
    }

    return(timer_id);
}

/****************************************************************************/
/* NAME:    SER_TMR_RequestOneShotCallback                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures an available timer to call			*/
/* at one shot the fct callback function with the given parameter 			*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: t_ser_error		                                                */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Required                                                     */

/****************************************************************************/
PUBLIC t_tmr_id SER_TMR_RequestOneShotCallback
(
    t_uint32            period,
    t_callback_fct      fct,
    void                *param,
    t_tmr_clk_prescaler tmr_clk_prescaler
)
{
    t_tmr_error tmr_error;
    t_tmr_id    timer_id = TMR_ID_0;

    /* Allocate The Timer If Available */
    tmr_error = TMR_AllocTimer(&timer_id);
    if (TMR_OK == tmr_error)
    {
        /* Initialize Periodic Timer */
        tmr_error = TMR_Configure(timer_id, TMR_MODE_ONE_SHOT, period, tmr_clk_prescaler);

        /* Bind the callback function */
        SER_TMR_ConnectonTimer(timer_id, fct, param);
        tmr_error = TMR_StartTimer(timer_id);
        if (TMR_OK != tmr_error)
        {
            PRINT("TMR_StartTimer failed for one shot timer, error [%d]", tmr_error);
            exit(-1);
        }
    }
    else
    {
        PRINT("TMR_AllocTimer failed for one shot timer, error [%d]", tmr_error);
        exit(-1);
    }

    return(timer_id);
}

/****************************************************************************/
/* NAME:    SER_TMR_ReleaseOneShotCallback                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine release a previously requested 				*/
/* 				one shot callback not already elapsed.						*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: t_ser_error		                                                */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Required                                                     */

/****************************************************************************/
PUBLIC t_ser_error SER_TMR_ReleaseOneShotCallback(t_tmr_req_id request_id)
{
    t_tmr_error tmr_error;
    t_uint32    current_value = 0;

    tmr_error = TMR_GetCounterValue((t_tmr_id) request_id, &current_value);
    if (current_value != 0)
    {
        /* Release the callback function */
        SER_TMR_ConnectonTimer((t_tmr_id) request_id, 0, 0);
    }
   /*coverity[self_assign]*/
    tmr_error = tmr_error;

    return(SERVICE_OK);
}

/****************************************************************************/
/* NAME:    SER_TMR_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializa the timer services and binds ISRs	*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_TMR_Init(t_uint8 default_ser_mask)
{
    t_tmr_error tmr_error;
    t_gic_error gic_error;
    t_gic_func_ptr old_datum;

    /* Set the base address for both the timer units */
    TMR_SetBaseAddress(TMR_DEVICE_ID_0, TMR_UNIT_0_ADDR);
    TMR_SetBaseAddress(TMR_DEVICE_ID_1, TMR_UNIT_1_ADDR);
    
 
    
/* AMBA clock enable for Peripheral6-MTU0/1 */

	(*((volatile t_uint32 *)(PRCC_6_CTRL_REG_BASE_ADDR)))   |= MTU0_AMBA_CLK_EN_VAL;
    (*((volatile t_uint32 *)(PRCC_6_CTRL_REG_BASE_ADDR)))   |= MTU1_AMBA_CLK_EN_VAL;
    
	
	
	tmr_error = TMR_Init(TMR_DEVICE_ID_0, TMR_UNIT_0_ADDR);
    if (TMR_OK != tmr_error)
    {
        PRINT("TMR_Init failed for TMR_DEVICE_ID_0, error [%d]", tmr_error);
        exit(-1);
    }

    tmr_error = TMR_Init(TMR_DEVICE_ID_1, TMR_UNIT_1_ADDR);
    if (TMR_OK != tmr_error)
    {
        PRINT("TMR_Init failed for TMR_DEVICE_ID_1, error [%d]", tmr_error);
        exit(-1);
    }


  	/* Attach the ISRs and enable the interrupt line for both the timers */
    gic_error = GIC_ChangeDatum(GIC_TIMER_0_LINE, SER_TMR0_Handler, &old_datum);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_ChangeDatum failed for GIC_TIMER_0_LINE, error [%d]", tmr_error);
        exit(-1);
    }

    gic_error = GIC_EnableItLine(GIC_TIMER_0_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_EnableItLine failed for GIC_TIMER_0_LINE, error [%d]", tmr_error);
        exit(-1);
    }

    gic_error = GIC_ChangeDatum(GIC_TIMER_1_LINE, SER_TMR1_Handler, &old_datum);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_ChangeDatum failed for GIC_TIMER_1_LINE, error [%d]", tmr_error);
        exit(-1);
    }

    gic_error = GIC_EnableItLine(GIC_TIMER_1_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_EnableItLine failed for GIC_TIMER_1_LINE, error [%d]", tmr_error);
        exit(-1);
    }


    /* Initialise both the timers */
    tmr_error = TMR_Reset(TMR_DEVICE_ID_0);
    if (TMR_OK != tmr_error)
    {
        PRINT("TMR_Reset failed for GIC_TIMER_0_LINE, error [%d]", tmr_error);
        exit(-1);
    }

    tmr_error = TMR_Reset(TMR_DEVICE_ID_1);
    if (TMR_OK != tmr_error)
    {
        PRINT("TMR_Reset failed for GIC_TIMER_1_LINE, error [%d]", tmr_error);
        exit(-1);
    }
    

}

/****************************************************************************/
/* NAME:    SER_TMR_ConnectonTimer											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to connect a callback function 			*/
/* for a particular timer id							                    */
/*                                                                          */
/* PARAMETERS:                                                              */
/*           - pipeId:    pipe identifier                                   */
/*           - fct: function pointer                                        */
/*           - param: parameter to use when call the previous function      */
/*                                                                          */
/* RETURN: int                                                              */
/*              		                                           			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_sint32 SER_TMR_ConnectonTimer(t_tmr_id timer_id, t_callback_fct fct, void *param)
{
    g_timer_callback[ser_tmr_GetIndex(timer_id)].fct = fct;
    g_timer_callback[ser_tmr_GetIndex(timer_id)].param = param;
    return(HCL_OK);
}

/****************************************************************************/
/* NAME:    SER_TMR_InstallDefaultHandler									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to insatll the default Interrupt handler*/
/*                                                                          */
/* PARAMETERS:                                                              */
/*                                                                          */
/* RETURN: void                                                             */
/*              		                                           			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_TMR_InstallDefaultHandler(void)
{
    t_gic_error gic_error;
    t_gic_func_ptr old_datum;

    gic_error = GIC_ChangeDatum(GIC_TIMER_0_LINE, SER_TMR0_Handler, &old_datum);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_ChangeDatum failed for GIC_TIMER_0_LINE, error [%d]", gic_error);
        exit(-1);
    }

    gic_error = GIC_EnableItLine(GIC_TIMER_0_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_EnableItLine failed for GIC_TIMER_0_LINE, error [%d]", gic_error);
        exit(-1);
    }

    gic_error = GIC_ChangeDatum(GIC_TIMER_1_LINE, SER_TMR1_Handler, &old_datum);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_ChangeDatum failed for GIC_TIMER_1_LINE, error [%d]", gic_error);
        exit(-1);
    }

    gic_error = GIC_EnableItLine(GIC_TIMER_1_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("GIC_EnableItLine failed for GIC_TIMER_1_LINE, error [%d]", gic_error);
        exit(-1);
    }
    

}

/*****************************************************************************/
/* NAME:    SER_TMR_WaitEnd										         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to wait for interrupt of particular event*/
/*                                                                           */
/* PARAMETERS:                                                               */
/*           -id :id of the event                                            */
/*                                                                           */
/* RETURN: void                                                              */
/*              		                                           			 */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC void SER_TMR_WaitEnd(t_tmr_id tmrid)
{
    /* Wait for the timer to expire. This varaible is set to TRUE in the ISR
	 *  when the timer expires 
	 */
    while (FALSE == g_timer_expired[ser_tmr_GetIndex(tmrid)])
        ;

    g_timer_expired[ser_tmr_GetIndex(tmrid)] = FALSE;
}

/****************************************************************************/
/* NAME:    SER_TMR_Close                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops timer services 							*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_TMR_Close(void)
{
    /* TBD */
}

