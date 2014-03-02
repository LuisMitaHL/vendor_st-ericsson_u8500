/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the PL022 (HSI)
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------
 * Includes							       
 *---------------------------------------------------------------------------*/
#include "hsi_p.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#ifdef __DEBUG
PRIVATE t_dbg_level myDebugLevel_HSI = DEBUG_LEVEL0;
PRIVATE t_dbg_id    myDebugID_HSI = HSI_HCL_DBG_ID;
#endif
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_HSI
#define MY_DEBUG_ID             myDebugID_HSI

/*--------------------------------------------
	Global variables
----------------------------------------------*/
PRIVATE t_hsi_system_context    g_hsi_system_context;

/**********************************************************************************/
/* NAME:	 hsip_InterruptTransmitter()	       							      */
/*--------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to get the Tx Interrupt           */
/* PARAMETERS    :															      */
/* IN            : t_hsi_filter_mode temp_filter: The value of the temp filter    */
/* INOUT         : None														      */
/* OUT           : None														      */
/* RETURN VALUE  : t_hsi_irq_src_id : The Value of the interrupt    		      */
/*--------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												      */

/**********************************************************************************/
PRIVATE t_hsi_irq_src_id hsip_InterruptTransmitter(t_hsi_filter_mode temp_filter)
{
    t_hsi_irq_src_id    irq_src_id = (t_hsi_irq_src_id) NULL;

    if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ0)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ0);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ1)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ1);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ2)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ2);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ3)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ3);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ4)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ4);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ5)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ5);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ6)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ6);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_TX_IRQ7)
    {
        irq_src_id = (HSI_IRQ_SRC_TX_IRQ7);
    }

    return(irq_src_id);
}

/**********************************************************************************/
/* NAME:	 hsip_InterruptReciever()	        							      */
/*--------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to get the Rx Interrupt           */
/* PARAMETERS    :															      */
/* IN            : t_hsi_filter_mode temp_filter: The value of the temp filter    */
/* INOUT         : None														      */
/* OUT           : None														      */
/* RETURN VALUE  : t_hsi_irq_src_id : The Value of the interrupt    		      */
/*--------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												      */

/**********************************************************************************/
PRIVATE t_hsi_irq_src_id hsip_InterruptReciever(t_hsi_filter_mode temp_filter)
{
    t_hsi_irq_src_id    irq_src_id = (t_hsi_irq_src_id) NULL;

    if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ0)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ0);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ1)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ1);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ2)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ2);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ3)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ3);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ4)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ4);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ5)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ5);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ6)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ6);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_IRQ7)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_IRQ7);
    }
    else
    {
    	irq_src_id = hsip_InterruptTransmitter(temp_filter);
    }

    return(irq_src_id);
}


/**********************************************************************************/
/* NAME:	 hsip_InterruptOverrunException()								      */
/*--------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to get the Rx Overrun Interrupt   */
/* PARAMETERS    :															      */
/* IN            : t_hsi_filter_mode temp_filter: The value of the temp filter    */
/* INOUT         : None														      */
/* OUT           : None														      */
/* RETURN VALUE  : t_hsi_irq_src_id : The Value of the interrupt    		      */
/*--------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												      */

/**********************************************************************************/
PRIVATE t_hsi_irq_src_id hsip_InterruptOverrunException(t_hsi_filter_mode temp_filter)
{
    t_hsi_irq_src_id    irq_src_id = (t_hsi_irq_src_id) NULL;

    if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV0)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV0);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV1)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV1);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV2)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV2);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV3)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV3);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV4)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV4);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV5)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV5);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV6)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV6);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OV7)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OV7);
    }
    else
    {
    	irq_src_id = hsip_InterruptReciever(temp_filter);
    }

    return(irq_src_id);
}

/**********************************************************************************/
/* NAME:	 hsip_InterruptException()										      */
/*--------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to get the Rx Exception Interrupt */
/* PARAMETERS    :															      */
/* IN            : t_hsi_filter_mode temp_filter: The value of the temp filter    */
/* INOUT         : None														      */
/* OUT           : None														      */
/* RETURN VALUE  : t_hsi_irq_src_id : The Value of the interrupt    		      */
/*--------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												      */

/**********************************************************************************/
PRIVATE t_hsi_irq_src_id hsip_InterruptException(t_hsi_filter_mode temp_filter)
{
    t_hsi_irq_src_id    irq_src_id = (t_hsi_irq_src_id) NULL;

    if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_TIMEOUT)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_TIMEOUT);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_BREAK)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_BREAK);
    }
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_OVERRUN)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_OVERRUN);
    }
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    else if (temp_filter & (t_uint32) HSI_IRQ_SRC_RX_EX_PARITY)
    {
        irq_src_id = (HSI_IRQ_SRC_RX_EX_PARITY);
    }
#endif
    else
    {
    	irq_src_id = hsip_InterruptOverrunException(temp_filter);
    }
    

    return(irq_src_id);
}

/************************************************************************************/
/* NAME:	 hsip_InterruptRecieverTimeout()       							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Timeout Interrupt */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE void hsip_InterruptRecieverTimeout(t_hsi_event *p_event)
{
    
    t_uint8             count;
    volatile t_hsi_rx_exception  rx_exp_status;
        
    rx_exp_status.ex_time_out = (t_bool) HSI_RX_GET_EX_TIMEOUT(g_hsi_system_context.p_hsi_rx->hsir_excepmis);

    if ((t_uint32) (rx_exp_status.ex_time_out) != NULL)
    {
        (g_hsi_system_context.hsi_event.rx_ex_event) |= (t_uint32) HSI_EXCEP_EVENT_RX_TIMEOUT;

        /*asinging the event to the global variable*/
        p_event->rx_ex_event = (t_uint32) HSI_EXCEP_EVENT_RX_TIMEOUT;
        HSI_SET_RX_EX_TIMEOUT(g_hsi_system_context.p_hsi_rx->hsir_acknowladge);
    }

    for (count = NULL; count < g_hsi_system_context.p_hsi_rx->hsir_channel; count++)
    {
        g_hsi_system_context.hsi_rx_data_buffer[count].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
    }
}

/************************************************************************************/
/* NAME:	 hsip_InterruptRecieverOverrun()       							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Overrun Interrupt */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE void hsip_InterruptRecieverOverrun(t_hsi_event *p_event)
{
    volatile t_hsi_rx_exception  rx_exp_status;
    t_uint8             count;

    rx_exp_status.ex_overrun = (t_bool) HSI_GET_RX_EX_OVR(g_hsi_system_context.p_hsi_rx->hsir_excepmis);
    if ((t_uint32) (rx_exp_status.ex_overrun) != NULL)
    {
        (g_hsi_system_context.hsi_event.rx_ex_event) |= (t_uint32) HSI_EXCEP_EVENT_RX_OVERRUN;

        /*asinging the event to the global variable*/
        p_event->rx_ex_event = (t_uint32) HSI_EXCEP_EVENT_RX_OVERRUN;
        HSI_SET_RX_EX_OVR(g_hsi_system_context.p_hsi_rx->hsir_acknowladge);
    }

    for (count = NULL; count < g_hsi_system_context.p_hsi_rx->hsir_channel; count++)
    {
        g_hsi_system_context.hsi_rx_data_buffer[count].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
    }
}

/************************************************************************************/
/* NAME:	 hsip_InterruptRecieverBreak()       							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Break Interrupt   */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE void hsip_InterruptRecieverBreak(t_hsi_event *p_event)
{
    volatile t_hsi_rx_exception  rx_exp_status;
    t_uint8             count;

    rx_exp_status.ex_brk = (t_bool) HSI_GET_RX_EX_BREAK(g_hsi_system_context.p_hsi_rx->hsir_excepmis);
    if ((t_uint32) (rx_exp_status.ex_brk) != NULL)
    {
        (g_hsi_system_context.hsi_event.rx_ex_event) |= (t_uint32) HSI_EXCEP_EVENT_RX_BREAK;

        /*asinging the event to the global variable*/
        p_event->rx_ex_event = (t_uint32) HSI_EXCEP_EVENT_RX_BREAK;
        HSI_SET_RX_EX_BREAK(g_hsi_system_context.p_hsi_rx->hsir_acknowladge);
    }

    for (count = NULL; count < g_hsi_system_context.p_hsi_rx->hsir_channel; count++)
    {
        g_hsi_system_context.hsi_rx_data_buffer[count].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
    }
}
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
/************************************************************************************/
/* NAME:	 hsip_InterruptRecieverParity()       							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Parity Interrupt  */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE void hsip_InterruptRecieverParity(t_hsi_event *p_event)
{
    volatile t_hsi_rx_exception  rx_exp_status;
    t_uint8             count;
    
    rx_exp_status.ex_parity = (t_bool) HSI_GET_RX_EX_PARITY(g_hsi_system_context.p_hsi_rx->hsir_excepmis);
    if ((t_uint32) (rx_exp_status.ex_parity) != NULL)
    {
        (g_hsi_system_context.hsi_event.rx_ex_event) |= (t_uint32) HSI_EXCEP_EVENT_RX_PARITY;

        /*asinging the event to the global variable*/
        p_event->rx_ex_event = (t_uint32) HSI_EXCEP_EVENT_RX_PARITY;
        HSI_SET_RX_EX_PARITY(g_hsi_system_context.p_hsi_rx->hsir_acknowladge);
    }

    for (count = NULL; count < g_hsi_system_context.p_hsi_rx->hsir_channel; count++)
    {
        g_hsi_system_context.hsi_rx_data_buffer[count].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
    }
}
#endif
/************************************************************************************/
/* NAME:	 hsip_InterruptOverrun()            							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Overrun Interrupt */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE void hsip_InterruptOverrun(t_hsi_event *p_event)
{
    volatile t_hsi_rx_exception  rx_exp_status;
    t_uint8             count;

    rx_exp_status.overrun[g_hsi_system_context.hsi_rx_data_buffer->ch_no] = (t_bool) HSI_GET_RX_OVERRUNMISx
        (
            g_hsi_system_context.p_hsi_rx->hsir_overrunmis,
            g_hsi_system_context.hsi_rx_data_buffer->ch_no
        );
    if ((t_uint32) (rx_exp_status.overrun[g_hsi_system_context.hsi_rx_data_buffer->ch_no]) != NULL)
    {
        (g_hsi_system_context.hsi_event.rx_ex_event) |= (t_uint32) (MASK_BIT0 << g_hsi_system_context.hsi_rx_data_buffer->ch_no);

        /*asinging the event to the global variable*/
        p_event->rx_ex_event = (t_uint32) (MASK_BIT0 << g_hsi_system_context.hsi_rx_data_buffer->ch_no);
        HSI_SET_RX_OVERRUNACK_CHx
        (
            g_hsi_system_context.p_hsi_rx->hsir_overrunack,
            g_hsi_system_context.hsi_rx_data_buffer->ch_no
        );
    }

    for (count = NULL; count < g_hsi_system_context.p_hsi_rx->hsir_channel; count++)
    {
        g_hsi_system_context.hsi_rx_data_buffer[count].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
    }
}



/************************************************************************************/
/* NAME:	 hsip_InterruptGetRecieverData()       							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Interrupt         */
/* PARAMETERS    :															        */
/* IN            : t_uint32 temp_event: The value of the event generated            */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : t_hsi_error                                                      */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE t_hsi_error hsip_InterruptGetRecieverData(t_uint32 temp_event)
{
    t_uint32            count, watermark_index, local_count;
    t_uint32            single_intr_no = NULL;
    volatile t_uint32   *p_fifo_reg = NULL;
    volatile t_uint32   *p_data_word_out = NULL;
    t_hsi_error         error_status;

    /*------------------------------------------------------
			Find out which FIFO has caused the interrupt
	------------------------------------------------------*/
    for (count = NULL; count <= g_hsi_system_context.p_hsi_rx->hsir_channel; count++)
    {
        /*SHIFT BYTE1 so that we get same value as the FIFO event mask*/
        if (temp_event & (t_uint32) (MASK_BIT0 << (count + SHIFT_BYTE1)))
        {
            single_intr_no = count;
            break;
        }
    }

    p_fifo_reg = &(g_hsi_system_context.p_hsi_rx->hsir_buffer[single_intr_no]);

    switch (g_hsi_system_context.hsi_rx_frm_size[single_intr_no])
    {
        case (HSI_FRAME_SIZE_BYTE):
            error_status = HSI_REQUEST_NOT_APPLICABLE;
            break;

        case (HSI_FRAME_SIZE_HALF_WORD):
            error_status = HSI_REQUEST_NOT_APPLICABLE;
            break;

        case (HSI_FRAME_SIZE_WORD):
            if (NULL == g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].size)
            {
                g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
                HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, single_intr_no);
                error_status = HSI_REMAINING_PENDING_EVENTS;
            }
            else
            {
                p_data_word_out = (t_uint32 *) g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].data;

                /*---------------
								Recieve the data
								-----------------*/
                if
                (
                    g_hsi_system_context.p_hsi_rx->hsir_watermark[single_intr_no] < g_hsi_system_context.
                        hsi_rx_data_buffer[single_intr_no].size
                )
                {
                    for
                    (
                        watermark_index = NULL;
                        watermark_index < (g_hsi_system_context.p_hsi_rx->hsir_watermark[single_intr_no] + 1);
                        watermark_index++
                    )
                    {
                        *(p_data_word_out++) = (t_uint32) HSI_GET_BUFFER(*p_fifo_reg);
                    }

                    g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].size -=
                        (
                            g_hsi_system_context.p_hsi_rx->hsir_watermark[single_intr_no] +
                            1
                        );

                    if (NULL == g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].size)
                    {
                        g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
                        HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                        DBGEXIT0(error_status);
                        return(error_status);
                    }
                    else
                    {
                        g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].data = (t_uint32 *) p_data_word_out;
                        HSI_SET_WMARKICx(g_hsi_system_context.p_hsi_rx->hsir_wmarkic, single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                    }
                }
                else
                {
                    local_count = g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].size;
                    for (watermark_index = NULL; watermark_index < local_count; watermark_index++)
                    {
                        *(p_data_word_out++) = (t_uint32) HSI_GET_BUFFER(*p_fifo_reg);
                        g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].size--;
                    }

                    if (NULL == g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].size)
                    {
                        g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
                        HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                        DBGEXIT0(error_status);
                        return(error_status);
                    }
                    else
                    {
                        g_hsi_system_context.hsi_rx_data_buffer[single_intr_no].data = (t_uint32 *) p_data_word_out;
                        HSI_SET_WMARKICx(g_hsi_system_context.p_hsi_rx->hsir_wmarkic, single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                    }
                }
            }
            break;

        default:
            error_status = HSI_REMAINING_PENDING_EVENTS;
            return(error_status);
    }   /*switch corresponding to frmsize*/

    return(error_status);
}

/************************************************************************************/
/* NAME:	 hsip_InterruptGetTransmitterData()    							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Tx Interrupt         */
/* PARAMETERS    :															        */
/* IN            : t_uint32 temp_event: The value of the event generated            */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : t_hsi_error                                                      */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE t_hsi_error hsip_InterruptGetTransmitterData(t_uint32 temp_event)
{
    t_uint32            count, watermark_index, local_count;
    t_uint32            single_intr_no = NULL;
    volatile t_uint32   *p_fifo_reg = NULL;
    volatile t_uint32   *p_data_word_in = NULL;
    t_hsi_error         error_status;

    /*Find out which FIFO has caused the interrupt*/
    for (count = NULL; count <= g_hsi_system_context.p_hsi_tx->hsit_channel; count++)
    {
        if (temp_event & (t_uint32) (MASK_BIT0 << (count)))
        {
            single_intr_no = count;
            break;
        }
    }

    p_fifo_reg = &(g_hsi_system_context.p_hsi_tx->hsit_buffer[single_intr_no]);

    switch (g_hsi_system_context.hsi_tx_frm_size[single_intr_no])
    {
        case (HSI_FRAME_SIZE_BYTE):
            error_status = HSI_REQUEST_NOT_APPLICABLE;
            break;

        case (HSI_FRAME_SIZE_HALF_WORD):
            error_status = HSI_REQUEST_NOT_APPLICABLE;
            break;

        case (HSI_FRAME_SIZE_WORD):
            if (NULL == g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].size)
            {
                g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
                HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, single_intr_no);
                error_status = HSI_REMAINING_PENDING_EVENTS;
                return(error_status);
            }
            else
            {
                p_data_word_in = (t_uint32 *) g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].data;

                /*---------------
							Transfer the data
							-----------------*/
                if
                (
                    g_hsi_system_context.p_hsi_tx->hsit_watermark[single_intr_no] < g_hsi_system_context.
                        hsi_tx_data_buffer[single_intr_no].size
                )
                {
                    for
                    (
                        watermark_index = NULL;
                        watermark_index < (g_hsi_system_context.p_hsi_tx->hsit_watermark[single_intr_no] + 1);
                        watermark_index++
                    )
                    {
                        HSI_SET_BUFFER(*p_fifo_reg, (t_uint32) (*(p_data_word_in++)));
                    }

                    g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].size -=
                        (
                            g_hsi_system_context.p_hsi_tx->hsit_watermark[single_intr_no] +
                            1
                        );
                    if (NULL == g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].size)
                    {
                        g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
                        HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                        return(error_status);
                    }
                    else
                    {
                        g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].data = (t_uint32 *) p_data_word_in;

                        //  HSI_SET_WMARKICx(g_hsi_system_context.p_hsi_tx->hsit_wmarkic,single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                    }
                }
                else
                {
                    local_count = g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].size;
                    for (watermark_index = NULL; watermark_index < local_count; watermark_index++)
                    {
                        HSI_SET_BUFFER(*p_fifo_reg, (t_uint32) (*(p_data_word_in++)));
                        g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].size--;
                    }

                    if (NULL == g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].size)
                    {
                        g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFER_ENDED;
                        HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                        return(error_status);
                    }
                    else
                    {
                        g_hsi_system_context.hsi_tx_data_buffer[single_intr_no].data = (t_uint32 *) p_data_word_in;

                        //  HSI_SET_WMARKICx(g_hsi_system_context.p_hsi_tx->hsit_wmarkic,single_intr_no);
                        error_status = HSI_REMAINING_PENDING_EVENTS;
                    }
                }
            }
            break;

        default:
            error_status = HSI_REMAINING_PENDING_EVENTS;
            return(error_status);
    }   /*end of switch corresponding to frmsz*/

    return(error_status);
}

/************************************************************************************/
/* NAME:	 hsip_InterruptGetTransmitterSource()   	         			        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to get the Tx Interrupt Source      */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/*                 t_hsi_irq_src_id irq_src_id: Interrupt Src Id                    */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE t_hsi_error hsip_InterruptGetTransmitterSource
(
    t_hsi_irq_src_id    irq_src_id,
    t_hsi_event         *p_event,
    t_hsi_event         *lp_event
)
{
    t_hsi_error error_status = HSI_OK;
    t_uint32    temp_event = NULL;

    switch (irq_src_id)
    {
        /*----------------------------------------
		Transmitter interrupt processing
		1. Log the event
		2. Find the FiFo causing the interrupt
		3. Fifo empty interrupt-> fill the channel
			with the data....
		4. No data...for channel...i.e FIFO reset 
		   interrupt just clr the interrupt and say
		   event pending.	
		----------------------------------------*/
        case HSI_IRQ_SRC_TX_IRQ0:
        case HSI_IRQ_SRC_TX_IRQ1:
        case HSI_IRQ_SRC_TX_IRQ2:
        case HSI_IRQ_SRC_TX_IRQ3:
        case HSI_IRQ_SRC_TX_IRQ4:
        case HSI_IRQ_SRC_TX_IRQ5:
        case HSI_IRQ_SRC_TX_IRQ6:
        case HSI_IRQ_SRC_TX_IRQ7:
            /*------------------------------------------------------
			TX interrupt source is mapped with the event id
			i.e for HSI_IRQ_SRC_TX_IRQ0  the event id is 
					HSI_FIFO_EVENT_ID_TX0 == HSI_IRQ_SRC_TX_IRQ0 
			------------------------------------------------------*/
            g_hsi_system_context.hsi_event.fifo_event |= (t_uint32) irq_src_id;
            p_event->fifo_event = lp_event->fifo_event;
            temp_event = (t_uint32) irq_src_id;

            error_status = hsip_InterruptGetTransmitterData(temp_event);
    }

    return(error_status);
}

/************************************************************************************/
/* NAME:	 hsip_InterruptGetRecieverSource()    		         			        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to get the Rx Interrupt Source      */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/*                 t_hsi_irq_src_id irq_src_id: Interrupt Src Id                    */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE t_hsi_error hsip_InterruptGetRecieverSource
(
    t_hsi_irq_src_id    irq_src_id,
    t_hsi_event         *p_event,
    t_hsi_event         *lp_event
)
{
    t_hsi_error error_status = HSI_OK;
    t_uint32    temp_event = NULL;

    switch (irq_src_id)
    {
        case HSI_IRQ_SRC_RX_IRQ0:
        case HSI_IRQ_SRC_RX_IRQ1:
        case HSI_IRQ_SRC_RX_IRQ2:
        case HSI_IRQ_SRC_RX_IRQ3:
        case HSI_IRQ_SRC_RX_IRQ4:
        case HSI_IRQ_SRC_RX_IRQ5:
        case HSI_IRQ_SRC_RX_IRQ6:
        case HSI_IRQ_SRC_RX_IRQ7:
            /*------------------------------------------------------
			RX interrupt source is mapped with the event id
			i.e for HSI_IRQ_SRC_RX_IRQ0  the event id is 
					HSI_FIFO_EVENT_ID_RX0 == HSI_IRQ_SRC_RX_IRQ0
			------------------------------------------------------*/
            lp_event->fifo_event |= (t_uint32) irq_src_id;
            p_event->fifo_event = lp_event->fifo_event;
            temp_event = (t_uint32) irq_src_id;

            error_status = hsip_InterruptGetRecieverData(temp_event);

            break;

        default:
            error_status = hsip_InterruptGetTransmitterSource(irq_src_id, p_event, lp_event);
            
    }

    return(error_status);
}

/************************************************************************************/
/* NAME:	 hsip_InterruptGetOverrunExceptionSource()    					        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Exception         */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/*                 t_hsi_irq_src_id irq_src_id: Interrupt Src Id                    */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE t_hsi_error hsip_InterruptGetOverrunExceptionSource(t_hsi_irq_src_id irq_src_id, t_hsi_event *p_event, t_hsi_event *lp_event)
{
    t_hsi_error error_status = HSI_OK;

    switch (irq_src_id)
    {
        case HSI_IRQ_SRC_RX_EX_OV0:
        case HSI_IRQ_SRC_RX_EX_OV1:
        case HSI_IRQ_SRC_RX_EX_OV2:
        case HSI_IRQ_SRC_RX_EX_OV3:
        case HSI_IRQ_SRC_RX_EX_OV4:
        case HSI_IRQ_SRC_RX_EX_OV5:
        case HSI_IRQ_SRC_RX_EX_OV6:
        case HSI_IRQ_SRC_RX_EX_OV7:
            hsip_InterruptOverrun(p_event);
            error_status = HSI_REMAINING_PENDING_EVENTS;
            break;

        default:
            error_status = hsip_InterruptGetRecieverSource(irq_src_id, p_event, lp_event);
            
    }

    return(error_status);
}

/************************************************************************************/
/* NAME:	 hsip_InterruptGetExceptionSource()    							        */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION   : This Private routine is used to service the Rx Exception         */
/* PARAMETERS    :															        */
/* IN            : t_hsi_event p_event: The value of the event generated            */
/*                 t_hsi_irq_src_id irq_src_id: Interrupt Src Id                    */
/* INOUT         : None														        */
/* OUT           : None														        */
/* RETURN VALUE  : None                                                             */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												        */

/************************************************************************************/
PRIVATE t_hsi_error hsip_InterruptGetExceptionSource(t_hsi_irq_src_id irq_src_id, t_hsi_event *p_event, t_hsi_event *lp_event)
{
    t_hsi_error error_status = HSI_OK;
    switch (irq_src_id)
    {
        case HSI_IRQ_SRC_RX_EX_TIMEOUT:
            hsip_InterruptRecieverTimeout(p_event);
            error_status = HSI_REMAINING_PENDING_EVENTS;
            break;

        case HSI_IRQ_SRC_RX_EX_OVERRUN:
            hsip_InterruptRecieverOverrun(p_event);
            error_status = HSI_REMAINING_PENDING_EVENTS;

            break;

        case HSI_IRQ_SRC_RX_EX_BREAK:
            hsip_InterruptRecieverBreak(p_event);
            error_status = HSI_REMAINING_PENDING_EVENTS;

            break;
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
        case HSI_IRQ_SRC_RX_EX_PARITY:
            hsip_InterruptRecieverParity(p_event);
            error_status = HSI_REMAINING_PENDING_EVENTS;
            break;
#endif
        default:
            error_status = hsip_InterruptGetOverrunExceptionSource(irq_src_id, p_event, lp_event);
            
    }

    return(error_status);
}


/***************************************************************************/
/* NAME:	 HSI_Init()												       */
/*-------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine initializes the HSI registers and check that*/
/*				  base address is valid                                    */
/* PARAMETERS    :											               */
/* IN            :id_device -- Transmitter/Receiver device.                */
/*				  base_address -- Base Address of the device               */
/* INOUT         :None                                                     */
/* OUT           :None                                                     */
/* RETURN VALUE  :HSI_UNSUPPORTED_HW: If the peripheral id or cell id does */
/*										not match 						   */
/*				 :HSI_OK: If base_address is correct					   */
/* TYPE          :Public                                                   */
/*-------------------------------------------------------------------------*/
/* REENTRANCY: NA														   */

/***************************************************************************/
PUBLIC t_hsi_error HSI_Init(IN t_hsi_device_id device, IN t_logical_address base_address)
{
    t_hsi_error         error_status = HSI_UNSUPPORTED_HW;
    t_hsi_rx_register   *p_rx_local;
    t_hsi_tx_register   *p_tx_local;

    DBGENTER2("(%lx,%lx)", device, base_address);

    if (HSI_DEVICE_ID_RX == device)
    {       /*CHECK FOR valid pointer address of receiver.*/
        p_rx_local = (t_hsi_rx_register *) base_address;

        if
        (
            (HSI_RX_PERIPHID0 == p_rx_local->hsir_periphrl_id0)
        &&  (HSI_RX_PERIPHID1 == p_rx_local->hsir_periphrl_id1)
        &&  (HSI_RX_PERIPHID2 == p_rx_local->hsir_periphrl_id2)
        &&  (HSI_RX_PERIPHID3 == p_rx_local->hsir_periphrl_id3)
        &&  (HSI_RX_PCELLID0 == p_rx_local->hsir_cell_id0)
        &&  (HSI_RX_PCELLID1 == p_rx_local->hsir_cell_id1)
        &&  (HSI_RX_PCELLID2 == p_rx_local->hsir_cell_id2)
        &&  (HSI_RX_PCELLID3 == p_rx_local->hsir_cell_id3)
        )
        {
            g_hsi_system_context.p_hsi_rx = (t_hsi_rx_register *) base_address;
            error_status = HSI_OK;
        }
        
    }
    else
    {       /*CHECK FOR valid pointer address of transmitter.*/
        p_tx_local = (t_hsi_tx_register *) base_address;
        if
        (
            (HSI_TX_PERIPHID0 == p_tx_local->hsit_periphrl_id0)
        &&  (HSI_TX_PERIPHID1 == p_tx_local->hsit_periphrl_id1)
        &&  (HSI_TX_PERIPHID2 == p_tx_local->hsit_periphrl_id2)
        &&  (HSI_TX_PERIPHID3 == p_tx_local->hsit_periphrl_id3)
        &&  (HSI_TX_PCELLID0 == p_tx_local->hsit_cell_id0)
        &&  (HSI_TX_PCELLID1 == p_tx_local->hsit_cell_id1)
        &&  (HSI_TX_PCELLID2 == p_tx_local->hsit_cell_id2)
        &&  (HSI_TX_PCELLID3 == p_tx_local->hsit_cell_id3)
        )
        {
            g_hsi_system_context.p_hsi_tx = (t_hsi_tx_register *) base_address;
            error_status = HSI_OK;
        }
        
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/***************************************************************************/
/* NAME:	 HSI_GetVersion()										       */
/*-------------------------------------------------------------------------*/
/* DESCRIPTION   :Returns the HSI HCL version information                  */
/*				                                                           */
/* PARAMETERS    :											               */
/* IN            :None                                                     */
/* INOUT         :None                                                     */
/* OUT           :p_version : For returning the version information        */
/* RETURN VALUE  :HSI_INVALID_PARAMETER: If p_version is NULL			   */
/*				 :HSI_OK : otherwise								       */
/* TYPE          :Public                                                   */
/*-------------------------------------------------------------------------*/
/* REENTRANCY: NA														   */

/***************************************************************************/
PUBLIC t_hsi_error HSI_GetVersion(OUT t_version *p_version)
{
    t_hsi_error error_status = HSI_OK;

    DBGENTER1("(%lx)", p_version);

    /*Return the version information in this pointer*/
    if (NULL == p_version)
    {
        ERROR("Invalid parameter");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    p_version->version = HSI_BUILD_VERSION;
    p_version->major = HSI_MAJOR_VERSION;
    p_version->minor = HSI_MINOR_VERSION;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_SetDbgLevel()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine configures HSI HCL in terms of debug. Debug */
/*				  level will determine the characteristics of the debug trac*/
/* PARAMETERS    :											                */
/* IN            : dbg_level-- HSI debug level                              */
/* INOUT         : None                                                     */
/* OUT           : None                                                     */
/* RETURN VALUE  : HSI_OK :Always											*/
/* TYPE          : Public                                                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_SetDbgLevel(IN t_dbg_level dbg_level)
{
    DBGENTER1("Setting Debug Level to %d", dbg_level);

#ifdef __DEBUG
    myDebugLevel_HSI = dbg_level;
    myDebugID_HSI = HSI_HCL_DBG_ID;
#endif    
    DBGEXIT0(HSI_OK);
    return(HSI_OK);
}


/****************************************************************************/
/* NAME:	 HSI_Reset()													*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine reset the hardware registers and software 	*/
/*					parameter   											*/
/* PARAMETERS    :											                */
/* IN            : device: Tranmitter or receiver                           */
/* INOUT         : None                                                     */
/* OUT           : None                                                     */
/* RETURN VALUE  :  HSI_INVALID_PARAMETER:If invalid device id				*/
/*					HSI_OK :Always											*/
/* TYPE          : Public                                                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_Reset(IN t_hsi_device_id device)
{
    t_hsi_error error_status = HSI_OK;
    t_uint32    count;

    DBGENTER1("(%d)", device);

    switch (device)
    {
        case HSI_DEVICE_ID_TX:
            /*--------------------------------------------
		Default value of the hsi transmitter  register
		----------------------------------------------*/
            g_hsi_system_context.p_hsi_tx->hsit_mode = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_tx_state = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_divisor = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_break = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_burstlen = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_dataswap = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_preamble = HSI_DEFAULT_PREAMBLE;
            g_hsi_system_context.p_hsi_tx->hsit_bufstate = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_channel = MASK_BIT0;
            g_hsi_system_context.p_hsi_tx->hsit_dmaen = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_flushbits = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_iostate = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_itcr = NULL;
            g_hsi_system_context.p_hsi_tx->hsit_itip = NULL;

            /*--------------------------------------------
		Software parameters reset
		----------------------------------------------*/
            for (count = NULL; count < HSI_MAX_CHANNEL_NUM; count++)
            {
                g_hsi_system_context.hsi_tx_data_buffer[count].status = NULL;
            }

            error_status = HSI_OK;
            break;

        case HSI_DEVICE_ID_RX:
            /*--------------------------------------------
		Default value of the hsi receiver register
		----------------------------------------------*/
            g_hsi_system_context.p_hsi_rx->hsir_mode = NULL;
            g_hsi_system_context.p_hsi_rx->hsir_rx_state = NULL;
            g_hsi_system_context.p_hsi_rx->hsir_bufstate = NULL;
            g_hsi_system_context.p_hsi_rx->hsir_channel = MASK_BIT0;
            g_hsi_system_context.p_hsi_rx->hsir_pipegauge = NULL;
            g_hsi_system_context.p_hsi_rx->hsir_excepim = NULL;
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
            g_hsi_system_context.p_hsi_rx->hsir_parity = NULL;
#endif
            g_hsi_system_context.p_hsi_rx->hsir_preamble = HSI_DEFAULT_PREAMBLE;
            g_hsi_system_context.p_hsi_rx->hsir_wmarkim = NULL;
            g_hsi_system_context.p_hsi_rx->hsir_dmaen = NULL;
            g_hsi_system_context.p_hsi_rx->hsir_itcr = NULL;
            g_hsi_system_context.p_hsi_rx->hsir_itip = NULL;

            /*--------------------------------------------
		Software parameters reset
		----------------------------------------------*/
            for (count = NULL; count < HSI_MAX_CHANNEL_NUM; count++)
            {
                g_hsi_system_context.hsi_rx_data_buffer[count].status = NULL;
            }

            error_status = HSI_OK;
            break;

        case HSI_DEVICE_ID_INVALID:
        default:
            error_status = HSI_INVALID_PARAMETER;
            break;
    }

    /*--------------------------------------------
	Software parameters reset
	----------------------------------------------*/
    g_hsi_system_context.hsi_event.fifo_event = NULL;
    g_hsi_system_context.hsi_event.rx_ex_event = NULL;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_SetTxConfiguration()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine configures the transmitter or receiver 		*/
/*					registers 												*/
/* PARAMETERS    :															*/
/* IN            : p_tx_config:Config structure for transmitter				*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: If p_tx_config is NULL, idlehold  */
/*					time is beyond limits.									*/
/*				   HSI_OPERATION_FAILED: If the transmitter is not in sleep */
/*					mode.													*/
/*				   HSI_OK : Otherwise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_SetTxConfiguration(IN t_hsi_tx_config *p_tx_config)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_tx_mode   prev_tx_mode;

    /*------------------------------
	 Parameter checking
	-------------------------------*/
    if (NULL == p_tx_config)
    {
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
	   Checking if the device is not in SLEEP mode
    ---------------------------------------------------------------*/
    prev_tx_mode = (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode);
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
	if (HSI_TX_MODE_SLEEP != prev_tx_mode)
#else    
    if (HSI_TX_MODE_INIT != prev_tx_mode)
#endif
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
	   Setting the Preamble Period for the Transmitter
    ---------------------------------------------------------------*/
    HSI_TX_SET_PREAMBLE(g_hsi_system_context.p_hsi_tx->hsit_preamble, p_tx_config->preamble);

    /*---------------------------------------------------------------
	   Setting the Bitrate Divisor for the Transmitter
    ---------------------------------------------------------------*/
    if (p_tx_config->divisor <= HSI_MAX_TX_BRD)
    {
        HSI_SET_TX_BRDEV(g_hsi_system_context.p_hsi_tx->hsit_divisor, p_tx_config->divisor);
        error_status = HSI_OK;
    }
    else
    {
        ERROR("Invalid bitrate divisor");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
       HSI Tx STATE
    ---------------------------------------------------------------*/
    HSI_SET_TX_STATE(g_hsi_system_context.p_hsi_tx->hsit_tx_state, (t_uint32) p_tx_config->txstate);

    /*---------------------------------------------------------------
    Setting the Parity for the Transmitter
	(Parity has been removed from V2.0 onwards)
    ---------------------------------------------------------------*/
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    HSI_SET_PARITY(g_hsi_system_context.p_hsi_tx->hsit_parity, (t_uint32) p_tx_config->parity);
#else
	p_tx_config->parity = (t_hsi_parity)HSI_NO_PARITY;
#endif
    HSI_SET_NOC(g_hsi_system_context.p_hsi_tx->hsit_channel, (t_uint32) p_tx_config->noc);

    /*---------------------------------------------------------------
	   Setting the Burst Length for the Transmitter
    ---------------------------------------------------------------*/
    if (p_tx_config->burstlen <= HSI_MAX_TX_BURSTLEN)
    {
        HSI_SET_TX_BURSTLEN(g_hsi_system_context.p_hsi_tx->hsit_burstlen, p_tx_config->burstlen);
    }
    else
    {
        ERROR("Invalid Burst length");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
    Setting the Mode for Transmihsion after changing all the other 
    parameters
    ---------------------------------------------------------------*/
    g_hsi_system_context.hsi_tx0_prev_mode = p_tx_config->mode;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_SetRxConfiguration()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine configures the transmitter or receiver 		*/
/*				  registers 												*/
/* PARAMETERS    :															*/
/* IN            : rx_num 	:Rx number										*/
/*		           p_rx_config:Config structure for reciever				*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: If p_rx_config is NULL			*/
/*				   HSI_OPERATION_FAILED: If the Reciever is not in sleep    */
/*					mode.													*/
/*				   HSI_OK : Otherwise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_SetRxConfiguration(IN t_hsi_rx_config *p_rx_config)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_rx_mode   prev_rx_mode;
    t_uint32        wait_counter = NULL;

    DBGENTER1("(%lx)", (t_uint32) p_rx_config);

    /*-------------------------------------
		Parameter checking
	--------------------------------------*/
    if (NULL == p_rx_config)
    {
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
	   Checking if the device is not in SLEEP mode
    ---------------------------------------------------------------*/
    prev_rx_mode = (t_hsi_rx_mode) HSI_GET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode);
    if (HSI_RX_MODE_SLEEP != prev_rx_mode)
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    HSI_SET_NOC(g_hsi_system_context.p_hsi_rx->hsir_channel, (t_uint32) p_rx_config->noc);

    /*---------------------------------------------------------------
       HSI Rx STATE
      ---------------------------------------------------------------*/
    if (HSI_RX_STATE_FLUSH == p_rx_config->rxstate)
    {
        HSI_SET_RX_STATE(g_hsi_system_context.p_hsi_rx->hsir_rx_state, (t_uint32) p_rx_config->rxstate);
        while
        (
            (HSI_RX_STATE_IDLE != ((t_hsi_rx_state) HSI_GET_RX_STATE(g_hsi_system_context.p_hsi_rx->hsir_rx_state))) &&
                (HSI_MAX_RX_TIME_OUT > wait_counter)
        )
        {
            wait_counter++;
        }

        if (wait_counter >= HSI_MAX_RX_TIME_OUT)
        {
            error_status = HSI_INTERNAL_ERROR;
            DBGEXIT0(error_status);
            return(error_status);
        }
    }
    else
    {
        HSI_SET_RX_STATE(g_hsi_system_context.p_hsi_rx->hsir_rx_state, (t_uint32) p_rx_config->rxstate);
    }

    /*---------------------------------------------------------------*/
    HSI_RX_SET_PREAMBLE(g_hsi_system_context.p_hsi_rx->hsir_preamble, p_rx_config->preamble);

#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))	
    /* HSI Rx Parity (Parity has been removed from V2.0 onwards) */
    HSI_SET_PARITY(g_hsi_system_context.p_hsi_rx->hsir_parity, (t_uint32) p_rx_config->parity);
#else
	p_rx_config->parity = (t_hsi_parity)HSI_NO_PARITY;
#endif

    /* Set the Threshold value for HSI Rx */
    HSI_SET_RX_THRESHOLD(g_hsi_system_context.p_hsi_rx->hsir_threshold, p_rx_config->threshold);

    if (HSI_RX_DETECTOR_0 == p_rx_config->detector)
    {
        HSI_CLR_RX_DETECTOR(g_hsi_system_context.p_hsi_rx->hsir_detector);
    }
    else if (HSI_RX_DETECTOR_1 == p_rx_config->detector)
    {
        HSI_SET_RX_DETECTOR(g_hsi_system_context.p_hsi_rx->hsir_detector);
    }
    else
    {
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
    Setting the Mode for Receiver after changing all the other 
    parameters
    ---------------------------------------------------------------*/
    g_hsi_system_context.hsi_rx0_prev_mode = (t_hsi_rx_mode) p_rx_config->mode;

    error_status = HSI_OK;
    DBGEXIT0(error_status);
    return(error_status);
}

/*****************************************************************************/
/* NAME:	 HSI_SetTxChannelConfig()										 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine sets up a particular Transmitter channel in th*/
/*				  specified mode (POLLED, INTERRUPT, OR DMA) along with the  */
/*				  Data Type for the channel.								 */
/* PARAMETERS    :															 */
/* IN            : t_hsi_tx_channel_config:Structure to configure chhanel    */
/* INOUT         : None														 */
/* OUT           : None														 */
/* RETURN VALUE  : HSI_INVALID_PARAMETER :If parameters are INVALID			 */
/*				   HSI_OPERATION_FAILED: If the transmitter is not in sleep */
/*					mode.													*/
/*				   HSL_OK:Otherwise											 */
/* TYPE          : Public													 */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													 */

/*****************************************************************************/
PUBLIC t_hsi_error HSI_SetTxChannelConfig(IN t_hsi_tx_channel_config tx_channel_config)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_tx_mode   prev_tx_mode;

    /*---------------------------------------------------------------
		Channel number is 0 to 7
	---------------------------------------------------------------*/
    DBGENTER2("(%d),(%d)", tx_channel_config.channel_no, tx_channel_config.mode);


    /*---------------------------------------------------------------
		Checking the channel number requested
	---------------------------------------------------------------*/
    if (HSI_MAX_CHANNEL_NUM <= tx_channel_config.channel_no)
    {
        ERROR("Invalid Channel Number Requested");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
	   Checking if the device is not in SLEEP mode
    ---------------------------------------------------------------*/
    prev_tx_mode = (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode);
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
	if (HSI_TX_MODE_SLEEP != prev_tx_mode)
#else    
    if (HSI_TX_MODE_INIT != prev_tx_mode)
#endif
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    g_hsi_system_context.hsi_tx_data_buffer[tx_channel_config.channel_no].status = (t_uint8) HSI_CHANNEL_STATUS_NO_TRANSFER;

    /*---------------------------------------------------------------
        Setting the framewidth for the chhanel     
    -----------------------------------------------------------------*/
    if (tx_channel_config.frame_len <= HSI_MAX_FRAMELEN)
    {
        HSI_SET_FRMSZ
        (
            g_hsi_system_context.p_hsi_tx->hsit_framelen[tx_channel_config.channel_no],
            tx_channel_config.frame_len
        );
        g_hsi_system_context.hsi_tx_frm_size[tx_channel_config.channel_no] = tx_channel_config.frame_len;
    }
    else
    {
        ERROR("Invalid Framewidth Requested");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*----------------------------------------------------------------
       Setting the Priority for the chhanel
    ------------------------------------------------------------------*/
    if (HSI_TX_ARB_CHANNEL_PRIORITY_0 == tx_channel_config.chhanel_priority)
    {
        HSI_CLR_TX_PRIORITY(g_hsi_system_context.p_hsi_tx->hsit_priority, (t_uint32) tx_channel_config.channel_no);
    }
    else if (HSI_TX_ARB_CHANNEL_PRIORITY_1 == tx_channel_config.chhanel_priority)
    {
        HSI_SET_TX_PRIORITY(g_hsi_system_context.p_hsi_tx->hsit_priority, (t_uint32) tx_channel_config.channel_no);
    }
    else
    {
        ERROR("Invalid Priority Level");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /* HSI Tx Channel x Base Address */
    HSI_SET_BASEx(g_hsi_system_context.p_hsi_tx->hsit_base[tx_channel_config.channel_no], tx_channel_config.base);
    g_hsi_system_context.hsi_tx_base[tx_channel_config.channel_no] = tx_channel_config.base;

    /* HSI Tx Channel x Span i.e. number of entries in the channel */
    HSI_SET_SPANx(g_hsi_system_context.p_hsi_tx->hsit_span[tx_channel_config.channel_no], tx_channel_config.span);
    g_hsi_system_context.hsi_tx_span[tx_channel_config.channel_no] = tx_channel_config.span;

    /* HSI Tx Channel x Watermark level */
    if (tx_channel_config.watermark <= HSI_MAX_FRAMELEN)
    {
        HSI_SET_WATERMARK
        (
            g_hsi_system_context.p_hsi_tx->hsit_watermark[tx_channel_config.channel_no],
            tx_channel_config.watermark
        );
    }
    else
    {
        ERROR("Invalid Watermark Level");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    switch (tx_channel_config.mode)
    {
        case HSI_MODE_IT_CPU:
            HSI_CLR_DMAE(g_hsi_system_context.p_hsi_tx->hsit_dmaen, tx_channel_config.channel_no);
            break;

        case HSI_MODE_DMA:
            HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, tx_channel_config.channel_no);
            HSI_SET_DMAE(g_hsi_system_context.p_hsi_tx->hsit_dmaen, tx_channel_config.channel_no);
            break;

        case HSI_MODE_POLLING:
            HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, tx_channel_config.channel_no);
            HSI_CLR_DMAE(g_hsi_system_context.p_hsi_tx->hsit_dmaen, tx_channel_config.channel_no);
            break;

        default:
            ERROR("Invalid Mode Requested");
            error_status = HSI_INVALID_MODE;
            DBGEXIT0(error_status);
            return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_SetRxChannelConfig()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine sets up a particular Reciever channel in the*/
/*				   specified mode (POLLED, INTERRUPT, OR DMA) along with the*/
/*				   Data Type for the channel.								*/
/* PARAMETERS    :															*/
/* IN            : channel_no: Reciever Channel to be configured			*/
/*	 			   rx_num : Receiver number									*/
/*				   mode: Transfer mode for the channel						*/
/*				   type: Data type for the channel							*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER :If parameters are INVALID			*/
/*				   HSI_OPERATION_FAILED: If the reciever is not in sleep    */
/*					mode.													*/
/*				   HSL_OK:Otherwise											*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_SetRxChannelConfig(IN t_hsi_rx_channel_config rx_channel_config)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_rx_mode   prev_rx_mode;


    /*---------------------------------------------------------------
		Checking the channel number requested
	---------------------------------------------------------------*/
    if (HSI_MAX_CHANNEL_NUM <= rx_channel_config.channel_no)
    {
        ERROR("Invalid Channel Number Requested");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*---------------------------------------------------------------
	   Checking if the device is not in SLEEP mode
    ---------------------------------------------------------------*/
    prev_rx_mode = (t_hsi_rx_mode) HSI_GET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode);
    if (HSI_RX_MODE_SLEEP != prev_rx_mode)
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    g_hsi_system_context.hsi_rx_data_buffer[rx_channel_config.channel_no].status = (t_uint8) HSI_CHANNEL_STATUS_NO_TRANSFER;

    /*---------------------------------------------------------------
        Setting the framewidth for the chhanel     
    -----------------------------------------------------------------*/
    if (rx_channel_config.frame_len <= HSI_MAX_FRAMELEN)
    {
        HSI_SET_FRMSZ
        (
            g_hsi_system_context.p_hsi_rx->hsir_framelen[rx_channel_config.channel_no],
            rx_channel_config.frame_len
        );
        g_hsi_system_context.hsi_rx_frm_size[rx_channel_config.channel_no] = rx_channel_config.frame_len;
    }
    else
    {
        ERROR("Invalid Framewidth Requested");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /* HSI Rx Channel x Base Address */
    HSI_SET_BASEx(g_hsi_system_context.p_hsi_rx->hsir_base[rx_channel_config.channel_no], rx_channel_config.base);
    g_hsi_system_context.hsi_rx_base[rx_channel_config.channel_no] = rx_channel_config.base;

    /* HSI Rx Channel x Span i.e. number of entries in the channel */
    HSI_SET_SPANx(g_hsi_system_context.p_hsi_rx->hsir_span[rx_channel_config.channel_no], rx_channel_config.span);
    g_hsi_system_context.hsi_rx_span[rx_channel_config.channel_no] = rx_channel_config.span;

    /* HSI Rx Channel x Watermark level */
    if (rx_channel_config.watermark <= HSI_MAX_FRAMELEN)
    {
        HSI_SET_WATERMARK
        (
            g_hsi_system_context.p_hsi_rx->hsir_watermark[rx_channel_config.channel_no],
            rx_channel_config.watermark
        );
    }
    else
    {
        ERROR("Invalid Watermark Level");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    switch (rx_channel_config.mode)
    {
        case HSI_MODE_IT_CPU:
            HSI_CLR_DMAE(g_hsi_system_context.p_hsi_rx->hsir_dmaen, rx_channel_config.channel_no);
            break;

        case HSI_MODE_DMA:
            HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, rx_channel_config.channel_no);
            HSI_SET_DMAE(g_hsi_system_context.p_hsi_rx->hsir_dmaen, rx_channel_config.channel_no);
            break;

        case HSI_MODE_POLLING:
            HSI_CLR_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, rx_channel_config.channel_no);
            HSI_CLR_DMAE(g_hsi_system_context.p_hsi_rx->hsir_dmaen, rx_channel_config.channel_no);
            break;

        default:
            ERROR("Invalid Mode Requested");
            error_status = HSI_INVALID_MODE;
            DBGEXIT0(error_status);
            return(error_status);
    }

    HSI_SET_RX_OVERRUNIMx(g_hsi_system_context.p_hsi_rx->hsir_overrunim, rx_channel_config.channel_no);
    HSI_SET_RX_EXCEPIM_ALL(g_hsi_system_context.p_hsi_rx->hsir_excepim, HSI_EXCEP_ALL);

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_TxInSleep()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine configures HSI Tx) in the sleep mode.	  	*/
/* PARAMETERS    :															*/
/* IN            : None                      								*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_OK:Otherwise											*/
/*                 HSI_REQUEST_NOT_APPLICABLE:HSIT already in sleep mode    */
/* TYPE          : Public													*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_TxInSleep(void)
{
    t_hsi_error error_status = HSI_OK;
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
    if ((t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode) != HSI_TX_MODE_SLEEP)
    {
        g_hsi_system_context.hsi_tx0_prev_mode = (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode);
        HSI_SET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode, (t_uint32) HSI_TX_MODE_SLEEP);
    }
#else
    if ((t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode) != HSI_TX_MODE_INIT)
    {
        g_hsi_system_context.hsi_tx0_prev_mode = (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode);
        HSI_SET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode, (t_uint32) HSI_TX_MODE_INIT);
    }
#endif    
    else
    {
        error_status = HSI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_TxOutSleep()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine takes the HSI Tx out of the sleep mode and	*/
/*				  configures it in the previously selected  mode. 			*/
/* PARAMETERS    :															*/
/* IN            : None                 									*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_OK : If no error 									*/
/*                 HSI_REQUEST_NOT_APPLICABLE:HSIT already not in sleep mode*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_TxOutSleep(void)
{
    t_hsi_error error_status = HSI_OK;
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
	if (HSI_TX_MODE_SLEEP == (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode))
#else
    if (HSI_TX_MODE_INIT == (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode))
#endif    
    {
        HSI_SET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode, (t_uint32) g_hsi_system_context.hsi_tx0_prev_mode);
        error_status = HSI_OK;
    }
    else
    {
        error_status = HSI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_RxInSleep()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine configures HSI Rx in the sleep mode.		*/
/* PARAMETERS    :															*/
/* IN            : None                 									*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_OK : Otherwise										*/
/*                 HSI_REQUEST_NOT_APPLICABLE:HSIR already in sleep mode    */
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_RxInSleep(void)
{
    t_hsi_error error_status = HSI_OK;

    if ((t_hsi_rx_mode) HSI_GET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode) != HSI_RX_MODE_SLEEP)
    {
        g_hsi_system_context.hsi_rx0_prev_mode = (t_hsi_rx_mode) HSI_GET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode);
        HSI_SET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode, (t_uint32) HSI_RX_MODE_SLEEP);
        error_status = HSI_OK;
    }
    else
    {
        error_status = HSI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_RxOutSleep()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine takes the HSI Rx out of the sleep mode and	*/
/*				   configures it in the previously selected  mode. 			*/
/* PARAMETERS    :															*/
/* IN            : None                 									*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_OK : Otherwise										*/
/*                 HSI_REQUEST_NOT_APPLICABLE:HSIR already not in sleep mode*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_RxOutSleep(void)
{
    t_hsi_error error_status = HSI_OK;

    if (HSI_RX_MODE_SLEEP == (t_hsi_rx_mode) HSI_GET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode))
    {
        HSI_SET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode, (t_uint32) g_hsi_system_context.hsi_rx0_prev_mode);
        error_status = HSI_OK;
    }
    else
    {
        error_status = HSI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_TxChangeFlushBits()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine is used to change the flush bits in HSIT	*/
/* PARAMETERS    :															*/
/* IN            : t_hsi_flushbits flushbits: The value of flush bits       */
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_OK : Otherwise  										*/
/*				   HSI_OPERATION_FAILED: If the transmitter is not in sleep */
/*					mode.													*/
/*                 HSI_INVALID_PARAMETER:If the value of flushbits is wrong */
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_TxChangeFlushBits(t_hsi_flushbits flushbits)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_tx_mode   prev_tx_mode;

    /*---------------------------------------------------------------
	   Checking if the device is not in SLEEP mode
    ---------------------------------------------------------------*/
    prev_tx_mode = (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode);
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
	if (HSI_TX_MODE_SLEEP != prev_tx_mode)
#else    
    if (HSI_TX_MODE_INIT != prev_tx_mode)
#endif    
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (HSI_MAX_FLUSHBITS <= (t_uint32) flushbits)
    {
        HSI_SET_TX_FLUSHBITS(g_hsi_system_context.p_hsi_tx->hsit_flushbits, (t_uint32) flushbits);
    }
    else
    {
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))

/****************************************************************************/
/* NAME:	 HSI_ChangeRxTimeout()											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine changes the value of Rx timeout register.	*/
/* PARAMETERS    :															*/
/* IN            :value: Value to be configured in timeout register			*/
/* INOUT         :None														*/
/* OUT           :None														*/
/* RETURN VALUE  :HSI_OPERATION_FAILED : If the RX is not in sleep mode		*/
/*				  HSI_INVALID_PARAMETER : If value or rx_num are invalid    */
/*				  HSI_OK: Otherwise											*/
/* TYPE          :Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Re-entrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_ChangeRxTimeout(IN t_uint32 value)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_rx_mode   temp_hsi_rx_mode;

    /*Checking the Receiver for Sleep Mode to make the changes*/
    temp_hsi_rx_mode = (t_hsi_rx_mode) HSI_GET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode);

    if (HSI_RX_MODE_SLEEP != temp_hsi_rx_mode)
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (value <= HSI_MAX_RX_TIME_OUT)
    {
        /*Changing the Timeout Value*/
        /* Set the Time Out Value for the HSI Reciever */
        HSI_SET_RX_TIMEOUT(g_hsi_system_context.p_hsi_rx->hsir_timeout, value);
        error_status = HSI_OK;
    }
    else
    {
        ERROR("Invalid Timeout period Value");
        error_status = HSI_INVALID_PARAMETER;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

#endif

/****************************************************************************/
/* NAME:	 HSI_ChangeTxBitrateDivisor()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine changes the value of  Tx bitrate divisor.	*/
/* PARAMETERS    :															*/
/* IN            :value: Value to be configured in divisor registe			*/
/* INOUT         :None														*/
/* OUT           :None														*/
/* RETURN VALUE  :HSI_OPERATION_FAILED : If the TX is not in sleep mode		*/
/*				  HSI_INVALID_PARAMETER : If value or tx_num are invalid    */
/*				  HSI_OK: Otherwise											*/
/* TYPE          :Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Re-entrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_ChangeTxBitrateDivisor(IN t_uint32 value)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_tx_mode   temp_hsi_mode;

    DBGENTER1("(%ld)", value);

    /*Setting the transmitter in Sleep Mode*/
    temp_hsi_mode = (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode);

#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
	if (HSI_TX_MODE_SLEEP != temp_hsi_mode)
#else
    if (HSI_TX_MODE_INIT != temp_hsi_mode)
#endif
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*
	The bit rate divisor can be changed only 
	when the HSI_TX is in SLEEP mode
	*/
    if (value <= HSI_MAX_TX_BRD)
    {
        /*Changing the Bit Rate Divisor*/
        HSI_SET_TX_BRDEV(g_hsi_system_context.p_hsi_tx->hsit_divisor, value);
        error_status = HSI_OK;
    }
    else
    {
        ERROR("Invalid Bit Rate Divisor Value");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/******************************************************************************/
/* NAME:	 HSI_WriteBreak() 												  */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine insterts a break in the transmihsion.		  */
/* PARAMETERS    :															  */
/* IN            : tx_num: Transmitter number								  */
/* INOUT         : None														  */
/* OUT           : None														  */
/* RETURN VALUE  : HSI_REQUEST_NOT_APPLICABLE: If the transmission mode is not*/
/*				   FRAME mode												  */
/*				   HSI_OK : otherwise										  */
/* TYPE          : Public													  */
/*--------------------------------------------------------------------------- */
/* REENTRANCY: Non Re-entrant												  */

/******************************************************************************/
PUBLIC t_hsi_error HSI_WriteBreak(void)
{
    t_hsi_error error_status = HSI_OK;

    if ((t_uint32) HSI_TX_MODE_FRAME == HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode))
    {
        HSI_SET_TX_BREAK(g_hsi_system_context.p_hsi_tx->hsit_break);
    }
    else
    {
        ERROR("Incompatible Mode for Break Insertion");
        error_status = HSI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_WriteSingleData()											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine writes data into a channel of tx.			*/
/* PARAMETERS    :															*/
/* IN            : channel_no: Channel number								*/
/*				   data:  Data to be written into the FIFO					*/
/*                 dataswap: Type of data swapping to be performed          */
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_OPERATION_FAILED : If FIFOs are full					*/
/*				   HSI_INVALID_PARAMETER: channel _no not configured.	    */
/*				   HSI_OK: Other wise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_WriteSingleData(IN t_hsi_channel_num channel_no, IN t_uint32 data, IN t_hsi_dataswap datatswap)
{
    t_hsi_error error_status = HSI_OK;

    DBGENTER2("(%d,%lx)", channel_no, data);

    if (HSI_MAX_CHANNEL_NUM > channel_no)
    {
        if (NULL != HSI_GET_FS(g_hsi_system_context.p_hsi_tx->hsit_bufstate, channel_no))
        {
            ERROR(" Fifo already Full");
            error_status = HSI_OPERATION_FAILED;
            DBGEXIT0(error_status);
            return(error_status);
        }

        HSI_SET_TX_DATASWAP(g_hsi_system_context.p_hsi_tx->hsit_dataswap, (t_uint32) datatswap);
        HSI_SET_BUFFER(g_hsi_system_context.p_hsi_tx->hsit_buffer[channel_no], data);
        error_status = HSI_OK;
    }
    else
    {
        ERROR("Invalid Channel Number Used");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_ReadSingleData()											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine reads data from a channel of the reciever   */
/* PARAMETERS    :															*/
/* IN            : channel_no: channel number								*/
/* INOUT         : None														*/
/* OUT           : *data : Data read from the FIFO							*/
/* RETURN VALUE  : HSI_OPERATION_FAILED : If FIFOs are full					*/
/*				   HSI_INTERNAL_ERROR: channel _no not configured.			*/
/*				   HSI_OK: Other wise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_ReadSingleData(IN t_hsi_channel_num channel_no, OUT t_uint32 *p_data)
{
    t_hsi_error error_status = HSI_OK;

    DBGENTER2("(%d,%lx)", channel_no, (t_uint32) p_data);

    if ((HSI_MAX_CHANNEL_NUM > channel_no) && (NULL != p_data))
    {
        if (NULL == HSI_GET_FS(g_hsi_system_context.p_hsi_rx->hsir_bufstate, channel_no))
        {
            ERROR(" Fifo Empty");
            error_status = HSI_OPERATION_FAILED;
            DBGEXIT0(error_status);
            return(error_status);
        }

        *p_data = HSI_GET_BUFFER(g_hsi_system_context.p_hsi_rx->hsir_buffer[channel_no]);
    }
    else
    {
        ERROR("Invalid Channel Number used");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_TxFIFOReset()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine flushes the particular FIFO of Tx or all    */
/*				   the FIFOs.                                               */
/* PARAMETERS    :				                                    		*/
/* IN            : channel_no:Channel number to be reset.					*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: if channel number is INVALID		*/
/*				   HSI_OK :Otherwise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_TxFIFOReset(IN t_hsi_channel_num channel_no)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_tx_mode   prev_tx_mode;

    DBGENTER1("(%d)", channel_no);  /*channel number is 0 to 7*/

    prev_tx_mode = (t_hsi_tx_mode) HSI_GET_TX_MODE(g_hsi_system_context.p_hsi_tx->hsit_mode);
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
	 if (HSI_TX_MODE_SLEEP != prev_tx_mode)
#else
    if (HSI_TX_MODE_INIT != prev_tx_mode)
#endif
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (channel_no < HSI_MAX_CHANNEL_NUM)
    {
        HSI_CLR_FS(g_hsi_system_context.p_hsi_tx->hsit_bufstate, channel_no);
    }
    else
    {
        ERROR("channel number should not exceeded");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_RxFIFOReset()												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine flushes the particular FIFO of Rx or all    */
/*				   the FIFOs.                                               */
/* PARAMETERS    :															*/
/* IN            : channel_no:Channel number to be reset.					*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: if channel number is INVALID		*/
/*				   HSI_OK :Otherwise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_RxFIFOReset(IN t_hsi_channel_num channel_no)
{
    t_hsi_error     error_status = HSI_OK;
    t_hsi_rx_mode   prev_rx_mode;

    DBGENTER1("(%d)", channel_no);  /*channel number is 0 to 7*/

    /*---------------------------------------------------------------
	   Checking if the device is in SLEEP mode
	   While receiver is receiving data..FIFOs can not be reset.
    ---------------------------------------------------------------*/
    prev_rx_mode = (t_hsi_rx_mode) HSI_GET_RX_MODE(g_hsi_system_context.p_hsi_rx->hsir_mode);
    if (HSI_RX_MODE_SLEEP != prev_rx_mode)
    {
        error_status = HSI_OPERATION_FAILED;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (channel_no < HSI_MAX_CHANNEL_NUM)
    {
        HSI_CLR_FS(g_hsi_system_context.p_hsi_rx->hsir_bufstate, channel_no);
        error_status = HSI_OK;
    }
    else
    {
        ERROR("channel number should not exceeded");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_WriteTxBuffer() (Asynchronous Function Call)				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine writes a given data buffer into the 			*/
/*				  transmit FIFO of a channel through interrupts				*/
/* PARAMETERS    :															*/
/* IN            :channel_no;	Channel to be written into					*/
/*				  buffer:		Pointer to the Data buffer to read from		*/
/*				  size	: 		Size of the Data to be transferred			*/
/*								(In units of Framewidth, and multiples 		*/
/*								of the watermark level)						*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: If any parameter is INVLAID		*/
/*				   HSI_REQUEST_PENDING: Otherwise							*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_WriteTxBuffer
(
    IN t_hsi_channel_num    channel_no,
    IN t_uint32             *p_buffer,
    IN t_uint32             size,
    IN t_hsi_dataswap       datatswap
)
{
    t_hsi_error error_status = HSI_OK;

    DBGENTER3("(%d,%lx,%lx)", channel_no, (t_uint32) p_buffer, size);

    if (HSI_MAX_CHANNEL_NUM <= channel_no)
    {
        ERROR("Invalid Channel Number Requested");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (NULL == p_buffer)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if ((size % 4) != 0)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    g_hsi_system_context.hsi_tx_data_buffer[channel_no].ch_no = channel_no;
    g_hsi_system_context.hsi_tx_data_buffer[channel_no].data = p_buffer;
    g_hsi_system_context.hsi_tx_data_buffer[channel_no].size = size;
    g_hsi_system_context.hsi_tx_data_buffer[channel_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFERRING;

    if (size > NULL)
    {
        HSI_CLR_DMAE(g_hsi_system_context.p_hsi_tx->hsit_dmaen, channel_no);
        HSI_SET_WMARKIMx(g_hsi_system_context.p_hsi_tx->hsit_wmarkim, channel_no);
        HSI_SET_TX_DATASWAP(g_hsi_system_context.p_hsi_tx->hsit_dataswap, (t_uint32) datatswap);
        error_status = HSI_REQUEST_PENDING;
    }
    else
    {
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_ReadRxBuffer() (Asynchronous Function Call)				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine reads into given data buffer from the 		*/
/*				  receiver FIFO of a channel through interrupts				*/
/* PARAMETERS    :															*/
/* IN            :channel_no;	Channel to be written into					*/
/*				  buffer:		Pointer to the Data buffer to read from		*/
/*				  size	: 		Size of the Data to be transferred			*/
/*								(In units of Framewidth, and multiples 		*/
/*								of the watermark level)						*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: If any parameter is INVLAID		*/
/*				   HSI_REQUEST_PENDING: Otherwise							*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_ReadRxBuffer
(
    IN t_hsi_channel_num    channel_no,
    INOUT t_uint32          *p_buffer,
    IN t_uint32             size    /*In units of framewidth
														data must be in multiples of fifo watermark*/
)
{
    t_hsi_error error_status = HSI_OK;
    DBGENTER3("(%x,%lx,%lx)", channel_no, (t_uint32) p_buffer, size);

    if (HSI_MAX_CHANNEL_NUM <= channel_no)
    {
        ERROR("Invalid Channel Number Requested");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (NULL == p_buffer)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (0 != (size % 4))
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    g_hsi_system_context.hsi_rx_data_buffer[channel_no].ch_no = channel_no;
    g_hsi_system_context.hsi_rx_data_buffer[channel_no].data = p_buffer;
    g_hsi_system_context.hsi_rx_data_buffer[channel_no].size = size;
    g_hsi_system_context.hsi_rx_data_buffer[channel_no].status = (t_uint8) HSI_CHANNEL_STATUS_TRANSFERRING;

    if (size > NULL)
    {
        HSI_CLR_DMAE(g_hsi_system_context.p_hsi_rx->hsir_dmaen, channel_no);
        HSI_SET_WMARKIMx(g_hsi_system_context.p_hsi_rx->hsir_wmarkim, channel_no);
        HSI_SET_RX_OVERRUNIMx(g_hsi_system_context.p_hsi_rx->hsir_overrunim, channel_no);
        HSI_SET_RX_EXCEPIM_ALL(g_hsi_system_context.p_hsi_rx->hsir_excepim, HSI_EXCEP_ALL);
        error_status = HSI_REQUEST_PENDING;
    }
    else
    {
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_GetIRQSrcStatus()  										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This function updates a <t_hsi_interrupt_status> structure*/
/*				  according to the <t_hsi_idIRQSrc> parameter. It allows to */
/* 				  store the reason of the interrupt within the 				*/
/* PARAMETERS																*/
/* IN            :irq_src  : irqsource number								*/
/*				  p_status :pointer to the structure to be updated			*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: If any parameter is INVLAID		*/
/*				   HSI_OK: Otherwise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC void HSI_GetIRQSrcStatus(IN t_hsi_irq_src irq_src, OUT t_hsi_irq_status *p_status)
{
    volatile t_uint32    temp = 0;

    DBGENTER3
    (
        "(Interrput Request: %lx, interrupt State: %x, Pending interrupt: %lx)",
        irq_src,
        p_status->irq_state,
        p_status->pending_irq
    );

    if (NULL == p_status)
    {
        ERROR("Invalid Parameter Passed");
        DBGEXIT0(HSI_INVALID_PARAMETER);
        return;
    }

    if (HSI_IRQ_SRC_ALL_IT == (t_hsi_irq_src_id) irq_src)
    {
        temp = ((g_hsi_system_context.p_hsi_tx->hsit_wmarkmis) & MASK_BYTE0);   /*Transmitter interrupts*/

        /*Receiver interrupts*/
        temp |= ((g_hsi_system_context.p_hsi_rx->hsir_wmarkmis) & MASK_BYTE0) << SHIFT_BYTE1;

        /*Receiver Exception*/
        temp |= (g_hsi_system_context.p_hsi_rx->hsir_exception) & HSI_RX_MASK_EXP_ALL << SHIFT_BYTE2;
        temp |= (g_hsi_system_context.p_hsi_rx->hsir_overrun) & HSI_INTR_MASK_CxIM_ALL << SHIFT_QUARTET5;
        
        p_status->pending_irq = temp & irq_src;
        p_status->irq_state = HSI_IRQ_STATE_NEW;

        return;
    }

    temp = ((g_hsi_system_context.p_hsi_tx->hsit_wmarkmis) & MASK_BYTE0);
    temp |= ((g_hsi_system_context.p_hsi_rx->hsir_wmarkmis) & MASK_BYTE0) << SHIFT_BYTE1;
    temp |= ((g_hsi_system_context.p_hsi_rx->hsir_excepmis) & HSI_RX_MASK_EXP_ALL) << SHIFT_BYTE2;
    temp |= (g_hsi_system_context.p_hsi_rx->hsir_overrun) & HSI_INTR_MASK_CxIM_ALL << SHIFT_QUARTET5;        

    p_status->pending_irq = temp & irq_src;
    p_status->irq_state = HSI_IRQ_STATE_NEW;

    DBGEXIT0(HSI_OK);
    return;
}

/*******************************************************************************/
/* NAME:	 HSI_IsIRQSrcActive() 											   */
/*---------------------------------------------------------------------------  */
/* DESCRIPTION :This function allows to know if the IRQ Source is active or not*/
/*	            It updates the interrupt structure for the relevant interrupts */
/* PARAMETERS  :															   */
/* IN          :irq_src       : irqsource number							   */
/*	            p_status :pointer to the structure to be updated			   */
/* INOUT       :None														   */
/* OUT         :None														   */
/* RETURN VALUE:void														   */
/* TYPE        :Public														   */
/*-----------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													   */

/*******************************************************************************/
PUBLIC t_bool HSI_IsIRQSrcActive(IN t_hsi_irq_src irq_src, IN t_hsi_irq_status *p_status)
{
    t_uint32    temp = 0;
    t_uint32    temp_irq;

    DBGENTER2("(Interrput Request: %lx, Pointer to Interrupt status: %p)", irq_src, (void *) p_status);
    if (NULL == p_status)
    {
        ERROR("Invalid Parameter Passed");
        DBGEXIT0(HSI_INVALID_PARAMETER);
        return(FALSE);
    }

    if
    (
        (
            irq_src & (t_hsi_irq_src)
                (
                    (t_uint32) HSI_IRQ_SRC_TX_IRQ0 | (t_uint32) HSI_IRQ_SRC_TX_IRQ1 | (t_uint32) HSI_IRQ_SRC_TX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_TX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_TX_IRQ7
                )
        ) != NULL
    )
    {
        temp |= HSI_GET_WMARKMISx_ALL(g_hsi_system_context.p_hsi_tx->hsit_wmarkmis);
        temp &= irq_src;
        temp_irq = p_status->pending_irq;
        p_status->pending_irq &= (~irq_src);
        p_status->pending_irq |= temp;

        if (temp_irq != p_status->pending_irq)
        {
            p_status->irq_state = HSI_IRQ_STATE_NEW;
        }
    }

    if
    (
        (
            irq_src &
                (
                    (t_uint32) HSI_IRQ_SRC_RX_IRQ0 | (t_uint32) HSI_IRQ_SRC_RX_IRQ1 | (t_uint32) HSI_IRQ_SRC_RX_IRQ2 |
                        (t_uint32) HSI_IRQ_SRC_RX_IRQ3 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ4 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ5 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ6 |
                            (t_uint32) HSI_IRQ_SRC_RX_IRQ7 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_TIMEOUT |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_BREAK |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OVERRUN |
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
                            (t_uint32) HSI_IRQ_SRC_RX_EX_PARITY |
#endif
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV0 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV1 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV2 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV3 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV4 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV5 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV6 |
                            (t_uint32) HSI_IRQ_SRC_RX_EX_OV7
                )
        ) != NULL
    )
    {
        temp |= (((g_hsi_system_context.p_hsi_rx->hsir_wmarkmis) & MASK_BYTE0) << SHIFT_BYTE1);
        temp |= (g_hsi_system_context.p_hsi_rx->hsir_excepmis & HSI_RX_MASK_EXP_ALL) << SHIFT_BYTE2;
        temp |= (g_hsi_system_context.p_hsi_rx->hsir_overrun) << SHIFT_QUARTET5;
        temp &= irq_src;
        temp_irq = p_status->pending_irq;
        p_status->pending_irq &= (~irq_src);
        p_status->pending_irq |= temp;

        if (temp_irq != p_status->pending_irq)
        {
            p_status->irq_state = HSI_IRQ_STATE_NEW;
        }
    }

    if (p_status->pending_irq & irq_src)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/********************************************************************************/
/* NAME:	 HSI_FilterProcessIRQSrc() 											*/
/*------------------------------------------------------------------------------*/
/* DESCRIPTION   :This iterative routine allows to process the device			*/
/*             	  interrupt sources identified through the status structure.    */
/*				  It processes the interrupt sources one by one, updates 		*/
/*				  status structure and keeps an internal history of the 		*/
/*				  events generated.												*/
/* PARAMETERS    :																*/
/* IN            :p_status 		pointer to interrupt status register			*/
/*				  p_event		pointer to store multiple events generated in   */
/*								the current iteration of HSI_FilterProcessIRQSrc*/
/*				  filter_mode   optional parameters which allows to filter 		*/
/*		                    	the procehsing and the generation of events		*/
/* INOUT         :  None														*/
/* OUT           :  None														*/
/* RETURN		 :  HSI_INTERNAL_EVENT(Positive value):	It indicates that		*/
/*            		all the processing associated with p_status has been 	    */
/*            		done all the events are not yet processed  i.e all the	    */
/*            		events have not been acknowledged.          			    */
/*                                                                              */
/*            		HSI_NO_MORE_PENDING_EVENT(Positive value): Indicate		    */
/*            		that all processing associated with the p_status		    */
/*            		(without filter option) is done .The interrupt source	    */
/*                	can be reenabled.										    */
/*                                                                              */
/*            		HSI_NO_MORE_FILTER_PENDING_EVENT(Positive value) 		    */
/*            		indicates that all the processing associated with		    */
/*             		p_status with the filter option is done 				    */
/*                                                                              */
/*            		HSI_NO_PENDING_EVENT_ERROR :when there is no interrupt	    */
/*                	to process                                   			    */
/*                                                                              */
/*                  HSI_REMAINING_PENDING_EVENTS(Positive value): Indicate      */
/*                      that the OWM_FilterProcessIRQSrc function must be       */
/*                      re-called to complete the processing. Other events      */
/*                      must be generated/processed.                            */
/*                                                                              */
/*                  HSI_INVALID_PARAMETER: This is returned  if pointer to      */
/*										   interrupt status is NULL             */
/* TYPE          :  Public														*/
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC t_hsi_error HSI_FilterProcessIRQSrc
(
    IN t_hsi_irq_status     *p_status,
    OUT t_hsi_event         *p_event,
    IN t_hsi_filter_mode    filter_mode
)
{
    t_hsi_irq_src_id    irq_src_id = HSI_IRQ_SRC_NO_IT;

    t_hsi_error         error_status;
    t_uint32            temp_filter;
    t_hsi_event         *lp_event;

    DBGENTER3
    (
        " (Pointer to Interrupt status: %p, Pointer to Event: %p, Filter mode: %lx)",
        p_status,
        p_event,
        filter_mode
    );

    /*------------------------------------------------------------------
	Parameter testing--
	p_status 	==NULL
	p_event 	==NULL
-------------------------------------------------------------------*/
    if (NULL == p_status || NULL == p_event)
    {
        DBGEXIT0(HSI_INVALID_PARAMETER);
        return(HSI_INVALID_PARAMETER);
    }

    /*------------------------------------------------------------------
	If the status is not HSI_IRQ_STATE_NEW or no pending irq
	in the irq register. 
-------------------------------------------------------------------*/
    if (HSI_IRQ_STATE_NEW == p_status->irq_state && NULL == p_status->pending_irq)
    {
        DBGEXIT0(HSI_NO_PENDING_EVENT_ERROR);
        return(HSI_NO_PENDING_EVENT_ERROR);
    }

    p_status->irq_state = HSI_IRQ_STATE_OLD;
    lp_event = &g_hsi_system_context.hsi_event;

    /*-------------------------------------------------------------------
	No filter mode means only one interrupt is to be served
-------------------------------------------------------------------*/
    if (HSI_NO_FILTER_MODE == filter_mode)
    {
        temp_filter = p_status->pending_irq;
        if (NULL == temp_filter)
        {
            if ((NULL == lp_event->fifo_event) && (NULL == lp_event->rx_ex_event))
            {
                DBGEXIT0(HSI_NO_MORE_PENDING_EVENT);
                return(HSI_NO_MORE_PENDING_EVENT);
            }
            else
            {
                DBGEXIT0(HSI_INTERNAL_EVENT);
                return(HSI_INTERNAL_EVENT);
            }
        }
    }
    else
    {
        /* Filter mode*/
        temp_filter = filter_mode & p_status->pending_irq;
        if (NULL == temp_filter)
        {
            /*No filter event*/
            if ((NULL == (lp_event->fifo_event & filter_mode) || (lp_event->rx_ex_event & filter_mode)))
            {
                if ((NULL == lp_event->fifo_event) && (NULL == lp_event->rx_ex_event))
                {
                    DBGEXIT0(HSI_NO_MORE_PENDING_EVENT);
                    return(HSI_NO_MORE_PENDING_EVENT);
                }
                else
                {
                    DBGEXIT0(HSI_NO_MORE_FILTER_PENDING_EVENT);
                    return(HSI_NO_MORE_FILTER_PENDING_EVENT);
                }
            }
            else
            {
                DBGEXIT0(HSI_INTERNAL_EVENT);
                return(HSI_INTERNAL_EVENT);
            }
        }
    }

    /*-------------------------------------------------------------------
	The irq_src_id has caused the interrupt.
	and is to be serviced according to the 
	temp_filter given by the user.
	
	Priority in which the irq id filtered 
	Highest 	:-HSI_RX (Exceptions)
	Mid Level	:-HSI_RX (Channel FIFO interrupts)
	Lowest 		:-HSI_TX (Channel FIFO interrupts)
-------------------------------------------------------------------*/
    irq_src_id = hsip_InterruptException(temp_filter);

    /*-------------------------------------------------------------------
	The irq_src_id has caused the interrupt
-------------------------------------------------------------------*/
    p_status->pending_irq &= ~((t_uint32) irq_src_id);

    /*procehsing of the interrupt*/
    error_status = hsip_InterruptGetExceptionSource(irq_src_id, p_event, lp_event);

    /*Copying the event value into the value to be returned*/
    p_event->fifo_event = lp_event->fifo_event;
    p_event->rx_ex_event = lp_event->rx_ex_event;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_IsEventActive() 											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine allows to check whether the given event is   */
/*				  active or not by checking the global event variable of    */
/*				  the HCL.													*/
/* PARAMETERS    :															*/
/* IN            :  t_hsi_idevent *p_event:the event for which its status ha*/
/*					to be checked											*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : t_bool	 TRUE if the event is active.					*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_bool HSI_IsEventActive(IN t_hsi_event *p_event)
{
    DBGENTER2("%x %x", p_event->fifo_event, p_event->rx_ex_event);

    if (NULL == p_event)
    {
        DBGEXIT1(HSI_INVALID_PARAMETER, "(%s)", "FALSE");
        return(FALSE);
    }

    if
    (
        (NULL != ((p_event->fifo_event) & g_hsi_system_context.hsi_event.fifo_event))
    ||  (NULL != ((p_event->rx_ex_event) & g_hsi_system_context.hsi_event.rx_ex_event))
    )
    {
        DBGEXIT1(HSI_OK, "(%s)", "TRUE");
        return(TRUE);
    }
    else
    {
        DBGEXIT1(HSI_OK, "(%s)", "FALSE");
        return(FALSE);
    }
}

/****************************************************************************/
/* NAME:	 HSI_AcknowledgeEvent() 										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This routine allows to acknowledge related internal event	*/
/* PARAMETERS    :															*/
/* IN            : p_event:pointer to acknowledge multiple events generated */
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : void 													*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC void HSI_AcknowledgeEvent(IN t_hsi_event *p_event)
{
    DBGENTER0();

    if (NULL == p_event)
    {
        ERROR("HSI_INVALID_PARAMETER: pointer passed for hsi_event is NULL");
        return;
    }   /* end if p_event */

    g_hsi_system_context.hsi_event.fifo_event &= (~(p_event->fifo_event));
    g_hsi_system_context.hsi_event.rx_ex_event &= (~(p_event->rx_ex_event));

    DBGEXIT0(TRUE);
}

/********************************************************************************/
/* NAME:	 HSI_GetTxChannelStatus() 											*/
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   :This routine checks the status of an single transmitter channel*/
/* PARAMETERS    :																*/
/* IN            : channel_no			Transmitter Channel to be checked		*/
/* INOUT         : p_tx_ch_status	Pointer to the transmitter channel			*/
/*									status enumeration							*/
/* OUT           : None															*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: Parameter invalid					    */
/*				   HSI_OK :Otherwise											*/
/* TYPE          : Public														*/
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC t_hsi_error HSI_GetTxChannelStatus(IN t_hsi_channel_num channel_no, OUT t_hsi_channel_status *p_tx_ch_status)
{
    t_hsi_error error_status = HSI_INTERNAL_ERROR;

    if (HSI_MAX_CHANNEL_NUM <= channel_no)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (NULL == p_tx_ch_status)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    *p_tx_ch_status = (t_hsi_channel_status) g_hsi_system_context.hsi_tx_data_buffer[channel_no].status;
    error_status = HSI_OK;
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_GetRxChannelStatus() 										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine checks the status of an single reciever chann*/
/* PARAMETERS    :															*/
/* IN            : channel_no:		Reciever Channel to be checked			*/
/* INOUT         : p_rx_ch_status	Pointer to the reciever channel			*/
/*									status enumeration						*/
/* OUT           : None														*/
/* RETURN VALUE  : t_hsi_error 												*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_GetRxChannelStatus(IN t_hsi_channel_num channel_no, OUT t_hsi_channel_status *p_rx_ch_status)
{
    t_hsi_error error_status = HSI_INTERNAL_ERROR;

    if (HSI_MAX_CHANNEL_NUM <= channel_no)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (NULL == p_rx_ch_status)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    *p_rx_ch_status = (t_hsi_channel_status) g_hsi_system_context.hsi_rx_data_buffer[channel_no].status;

    error_status = HSI_OK;
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_GetRxState() 												*/
/*------------------------------------------------------------------------- */
/* DESCRIPTION   : This routine gets the state  of receiver 			    */
/* PARAMETERS    :														    */
/* IN            : p_rx_state  (Pointer to the structure for receiving the  */
/*				  status of Rx.)										    */
/* INOUT         : None													    */
/* OUT           : None													    */
/* RETURN VALUE  : t_hsi_error												*/
/* TYPE          : Public													*/
/*-------------------------------------------------------------------------	*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_GetRxState(OUT t_hsi_rx_state *p_rx_state)
{
    t_hsi_error error_status = HSI_OK;

    DBGENTER1("(%lx)", (t_uint32) p_rx_state);

    if (NULL != p_rx_state)
    {
        *(p_rx_state) = (t_hsi_rx_state) HSI_GET_RX_STATE(g_hsi_system_context.p_hsi_rx->hsir_rx_state);
        error_status = HSI_OK;
    }
    else
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_GetTxState() 												*/
/*------------------------------------------------------------------------- */
/* DESCRIPTION   : This routine gets the state of transmitter 			    */
/* PARAMETERS    : None													    */
/* INOUT         : None													    */
/* OUT           : p_tx_state  (Pointer to the structure for receiving the  */
/*				  status of Tx.)										    */
/*				   HSI_INVALID_PARAMETER:If any pameter is INVALID			*/
/* RETURN VALUE  : HSI_OK:Otherwise											*/
/* TYPE          : Public													*/
/*-------------------------------------------------------------------------	*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_GetTxState(OUT t_hsi_tx_state *p_tx_state)
{
    t_hsi_error error_status = HSI_OK;

    DBGENTER1("(%lx)", (t_uint32) p_tx_state);

    if (NULL != p_tx_state)
    {
        *(p_tx_state) = (t_hsi_tx_state) HSI_GET_TX_STATE(g_hsi_system_context.p_hsi_tx->hsit_tx_state);
        error_status = HSI_OK;
    }
    else
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/*****************************************************************************/
/* NAME:	 HSI_GetTxFIFOStatus() 											 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine checks the FIFO status of a transmit channel.*/
/* PARAMETERS    :															 */
/* IN            : channel_no  Channel for which FIFO status is seeked		 */
/* INOUT         : *p_status : Status of FIFO								 */
/* OUT           : None														 */
/* RETURN VALUE  : HSI_INVALID_PARAMETER: IF parameters are invalid			 */
/*				   HSI_OK : Otherwise										 */
/*                 HSI_REQUEST_NOT_APPLICABLE : If channel is disabled       */
/* TYPE          : Public													 */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													 */

/*****************************************************************************/
PUBLIC t_hsi_error HSI_GetTxFIFOStatus(IN t_hsi_channel_num channel_no, OUT t_hsi_fifo_status *p_status)
{
    t_hsi_error error_status = HSI_OK;
    t_bool      fifo_full;
    t_uint8     fifo_empty;

    DBGENTER2("(%x), (%lx)", channel_no, (t_uint32) p_status);

    if (NULL == p_status)
    {
        ERROR("Invalid Status Structure");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (HSI_MAX_CHANNEL_NUM <= channel_no)
    {
        ERROR("Invalid Channel Number");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (NULL == HSI_GET_SPANx(g_hsi_system_context.p_hsi_tx->hsit_span[channel_no]))
    {
        ERROR(" Request Not Applicable");
        error_status = HSI_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error_status);
        return(error_status);
    }

    fifo_full = (t_bool) HSI_GET_FS(g_hsi_system_context.p_hsi_tx->hsit_bufstate, channel_no);
    fifo_empty = (t_uint8)g_hsi_system_context.p_hsi_tx->hsit_gauge[channel_no];

    switch (fifo_full)
    {
        case TRUE:
            *p_status = HSI_FIFO_STATUS_FULL;
            break;

        case FALSE:
            if (fifo_empty == HSI_GET_SPANx(g_hsi_system_context.p_hsi_tx->hsit_span[channel_no]))
            {
                *p_status = HSI_FIFO_STATUS_EMPTY;
            }
            else
            {
                *p_status = HSI_FIFO_STATUS_PARTIAL;
            }
            break;

        default:
            DBGEXIT0(error_status);
            return(error_status);
    }

    error_status = HSI_OK;
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_GetRxFIFOStatus() 											*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine checks the FIFO status of a  recieve channel*/
/* PARAMETERS    :															*/
/* IN            : channel_no  Channel for which FIFO status is seeked		*/
/* INOUT         : *p_status :FIFO Status									*/
/* OUT           : None														*/
/* RETURN VALUE  : HSI_INVALID_PARAMETER: IF parameters are invalid			*/
/*				   HSI_OK : Otherwise										*/
/*                 HSI_REQUEST_NOT_APPLICABLE : If channel is disabled       */
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_GetRxFIFOStatus(IN t_hsi_channel_num channel_no, OUT t_hsi_fifo_status *p_status)
{
    t_uint8     fifo_full;
    t_bool      fifo_empty;

    t_hsi_error error_status = HSI_OK;

    DBGENTER2("(%x), (%lx)", channel_no, (t_uint32) p_status);

    if (NULL == p_status)
    {
        ERROR("Invalid Status Structure");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (HSI_MAX_CHANNEL_NUM <= channel_no)
    {
        ERROR("Invalid Channel Number");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (NULL == HSI_GET_SPANx(g_hsi_system_context.p_hsi_rx->hsir_span[channel_no]))
    {
        ERROR(" Request Not Applicable");
        error_status = HSI_REQUEST_NOT_APPLICABLE;
        DBGEXIT0(error_status);
        return(error_status);
    }

    fifo_empty = (t_bool) HSI_GET_FS(g_hsi_system_context.p_hsi_rx->hsir_bufstate, channel_no);
    fifo_full = (t_uint8)g_hsi_system_context.p_hsi_rx->hsir_gauge[channel_no];

    switch (fifo_empty)
    {
        case TRUE:
            if (HSI_GET_SPANx(g_hsi_system_context.p_hsi_rx->hsir_span[channel_no]) == fifo_full)
            {
                *p_status = HSI_FIFO_STATUS_FULL;
                error_status = HSI_OK;
            }
            else
            {
                *p_status = HSI_FIFO_STATUS_PARTIAL;
                error_status = HSI_OK;
            }
            break;

        case FALSE:
            *p_status = HSI_FIFO_STATUS_EMPTY;
            error_status = HSI_OK;
            break;

        default:
            DBGEXIT0(error_status);
            return(error_status);
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/*****************************************************************************/
/* NAME:	 HSI_GetRxExceptionstatus() 									 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine gets the exception status -signal,timeout,	 */
/*					break and overrun.										 */
/* PARAMETERS    :															 */
/* IN            : exception :defines the exception type 					 */
/* INOUT         : None														 */
/* OUT           : None														 */
/* RETURN VALUE  : t_hsi_error												 */
/* TYPE          : Public													 */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													 */

/*****************************************************************************/
PUBLIC t_hsi_error HSI_GetRxExceptionstatus(OUT t_hsi_rx_exception *p_exception)
{
    t_hsi_error         error_status = HSI_OK;
    t_uint8             index;
    volatile t_uint32   *p_esr_reg = NULL;

    p_esr_reg = &g_hsi_system_context.p_hsi_rx->hsir_exception;

    DBGENTER1("(%lx)", (t_uint32) p_exception);

    if (NULL == p_exception)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    p_exception->ex_brk = (t_bool) HSI_GET_RX_EX_BREAK(*p_esr_reg);
    p_exception->ex_time_out = (t_bool) HSI_RX_GET_EX_TIMEOUT(*p_esr_reg);
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    p_exception->ex_parity = (t_bool) HSI_GET_RX_EX_PARITY(*p_esr_reg);
#endif
    p_exception->ex_overrun = (t_bool) HSI_GET_RX_EX_OVR(*p_esr_reg);

    for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
    {
        p_exception->overrun[index] = (t_bool) HSI_GET_RX_OVERRUN_CHx
            (
                g_hsi_system_context.p_hsi_rx->hsir_overrun,
                index
            );
    }

    error_status = HSI_OK;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_SetRxExceptionstatus() 									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine sets the exception status -signal,timeout,  */
/*					break and overrun.										*/
/* PARAMETERS    :															*/
/* IN            : p_exception :defines the exception type					*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : t_hsi_error												*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Re-entrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_SetRxExceptionstatus(IN t_hsi_rx_exception *p_exception)
{
    
    t_uint32            index;
    t_hsi_error         error_status = HSI_OK;

    DBGENTER1("(%lx)", (t_uint32) p_exception);

    if (NULL == p_exception)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    if (p_exception->ex_brk)
    {
         HSI_SET_RX_EX_BREAK(g_hsi_system_context.p_hsi_rx->hsir_exception);
    }

    if (p_exception->ex_time_out)
    {
        HSI_SET_RX_EX_TIMEOUT(g_hsi_system_context.p_hsi_rx->hsir_exception);
    }
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    if (p_exception->ex_parity)
    {
        HSI_SET_RX_EX_PARITY(g_hsi_system_context.p_hsi_rx->hsir_exception);
    }
#endif

    if (p_exception->ex_overrun)
    {
        HSI_SET_RX_EX_OVR(g_hsi_system_context.p_hsi_rx->hsir_exception);
    }

    for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
    {
        if (p_exception->overrun[index])
        {
            HSI_SET_RX_OVERRUNIMx(g_hsi_system_context.p_hsi_rx->hsir_overrun, index);
        }
    }

    error_status = HSI_OK;
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:	 HSI_AcknowladgeException() 									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine acknowladges the exception status           */
/*                   signal,timeout,break and overrun.			            */
/* PARAMETERS    :															*/
/* IN            : p_exception :defines the exception type					*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : t_hsi_error												*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Re-entrant												*/

/****************************************************************************/
PUBLIC t_hsi_error HSI_AcknowledgeException(IN t_hsi_rx_exception *p_exception)
{
    volatile t_uint32   *p_esr_reg = NULL;
    t_uint32            index;
    t_hsi_error         error_status = HSI_OK;

    DBGENTER1("(%lx)", (t_uint32) p_exception);

    if (NULL == p_exception)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }

    p_esr_reg = &g_hsi_system_context.p_hsi_rx->hsir_acknowladge;

    if (p_exception->ex_brk)
    {
        HSI_SET_RX_EX_BREAK(*p_esr_reg);
    }

    if (p_exception->ex_time_out)
    {
        HSI_SET_RX_EX_TIMEOUT(*p_esr_reg);
    }
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    if (p_exception->ex_parity)
    {
        HSI_SET_RX_EX_PARITY(*p_esr_reg);
    }
#endif

    if (p_exception->ex_overrun)
    {
        HSI_SET_RX_EX_OVR(*p_esr_reg);
    }

    for (index = NULL; index < HSI_MAX_CHANNEL_NUM; index++)
    {
        if (p_exception->overrun[index])
        {
            HSI_SET_RX_OVERRUNACK_CHx(g_hsi_system_context.p_hsi_rx->hsir_overrunack, index);
        }
    }

    error_status = HSI_OK;
    DBGEXIT0(error_status);
    return(error_status);
}

/************************************************************************************/
/* NAME:	t_hsi_error HSI_SetRxRealTime()  	   						     	 	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to set the real time mode of a channel	        */
/*																		            */
/* PARAMETERS:																        */
/* IN			channel_no:the channel for which real time is to be set             */
/* OUT			NONE																*/
/* RETURN:		void																*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */

/************************************************************************************/
PUBLIC t_hsi_error HSI_SetRxRealTime(IN t_hsi_channel_num channel_no)
{
    t_hsi_error error_status = HSI_OK;

    if (channel_no >= HSI_MAX_CHANNEL_NUM)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }
    else
    {
        HSI_SET_RX_REALTIME_CHx(g_hsi_system_context.p_hsi_rx->hsir_realtime, channel_no);
        error_status = HSI_OK;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/************************************************************************************/
/* NAME:	t_hsi_error HSI_GetRxRealTime()  	   						     	 	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to read weather the real time mode is set for 	*/
/*              the particular channel                                              */
/*																		            */
/* PARAMETERS:																        */
/* IN			channel_num:the channel for which the mode is to be checked.		*/
/* OUT			realtime_status:TRUE - The mode is set								*/
/*                              FALSE - The mode is not set                         */
/* RETURN:		void																*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */

/************************************************************************************/
PUBLIC t_hsi_error HSI_GetRxRealTime(IN t_hsi_channel_num channel_no, OUT t_bool *realtime_status)
{
    t_hsi_error error_status = HSI_OK;

    if (channel_no >= HSI_MAX_CHANNEL_NUM)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }
    else
    {
        if (NULL == realtime_status)
        {
            ERROR("Invalid Parameter Passed");
            error_status = HSI_INVALID_PARAMETER;
            DBGEXIT0(error_status);
            return(error_status);
        }
        else
        {
            *realtime_status = (t_bool) HSI_GET_RX_REALTIME_CHx
                (
                    g_hsi_system_context.p_hsi_rx->hsir_realtime,
                    channel_no
                );
            error_status = HSI_OK;
        }
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/************************************************************************************/
/* NAME:	t_hsi_error HSI_ClearRxRealTime()  	   						     	 	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears the realtime mode set for the particular channel*/
/*																		            */
/* PARAMETERS:																        */
/* IN			channel_num:the channel for which the mode is to be checked.		*/
/* OUT			NONE																*/
/* RETURN:		void																*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */

/************************************************************************************/
PUBLIC t_hsi_error HSI_ClearRxRealTime(IN t_hsi_channel_num channel_no)
{
    t_hsi_error error_status = HSI_OK;

    if (channel_no >= HSI_MAX_CHANNEL_NUM)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }
    else
    {
        HSI_CLEAR_RX_REALTIME_CHx(g_hsi_system_context.p_hsi_rx->hsir_realtime, channel_no);
        error_status = HSI_OK;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/************************************************************************************/
/* NAME:	t_hsi_error HSI_SetRxWaterMarkLevel()  	   						     	 	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears the realtime mode set for the particular channel*/
/*																		            */
/* PARAMETERS:																        */
/* IN			channel_num:the channel for which the mode is to be checked.		*/
/* OUT			NONE																*/
/* RETURN:		void																*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */

/************************************************************************************/
PUBLIC t_hsi_error HSI_SetRxWaterMarkLevel(IN t_uint32 watermark, IN t_hsi_channel_num channel_no)
{
    t_hsi_error error_status = HSI_OK;

    if (channel_no >= HSI_MAX_CHANNEL_NUM)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }
    else
    {
        /* HSI Rx Channel x Watermark level */
        if (watermark <= HSI_MAX_FRAMELEN)
        {
            HSI_SET_WATERMARK(g_hsi_system_context.p_hsi_rx->hsir_watermark[channel_no], watermark);
            error_status = HSI_OK;
        }
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/************************************************************************************/
/* NAME:	t_hsi_error HSI_SetTxWaterMarkLevel()  	   						      	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears the realtime mode set for the particular channel*/
/*																		            */
/* PARAMETERS:																        */
/* IN			channel_num:the channel for which the mode is to be checked.		*/
/* OUT			NONE																*/
/* RETURN:		void																*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */

/************************************************************************************/
PUBLIC t_hsi_error HSI_SetTxWaterMarkLevel(IN t_uint32 watermark, IN t_hsi_channel_num channel_no)
{
    t_hsi_error error_status = HSI_OK;

    if (channel_no >= HSI_MAX_CHANNEL_NUM)
    {
        ERROR("Invalid Parameter Passed");
        error_status = HSI_INVALID_PARAMETER;
        DBGEXIT0(error_status);
        return(error_status);
    }
    else
    {
        /* HSI Tx Channel x Watermark level */
        if (watermark <= HSI_MAX_FRAMELEN)
        {
            HSI_SET_WATERMARK(g_hsi_system_context.p_hsi_tx->hsit_watermark[channel_no], watermark);
            error_status = HSI_OK;
        }
    }

    DBGEXIT0(error_status);
    return(error_status);
}

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV1))
/************************************************************************************/
/* NAME:	t_hsi_error HSI_SetRxFrameBurstCnt()  	   						      	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the frame burst count for HSIR in PIPELINED mode  */
/*																		            */
/* PARAMETERS:																        */
/* IN			framebrstcnt:the value of the frameburst count                      */
/* OUT			NONE																*/
/* RETURN:		t_hsi_error															*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */

/************************************************************************************/
PUBLIC t_hsi_error HSI_SetRxFrameBurstCnt(IN t_uint32 framebrstcnt)
{
    t_hsi_error error_status = HSI_OK;

    if(HSI_RX_MODE_PIPELINED == (t_hsi_rx_mode)g_hsi_system_context.p_hsi_rx->hsir_mode)
    {
    	g_hsi_system_context.p_hsi_rx->hsir_frameburstcnt = framebrstcnt;
    }
    else
    {
    	error_status = HSI_INVALID_MODE;
    }

    DBGEXIT0(error_status);
    return(error_status);
}

/************************************************************************************/
/* NAME:	t_hsi_error HSI_GetRxFrameBurstCnt()  	   						      	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the frame burst count for HSIR in PIPELINED mode  */
/*																		            */
/* PARAMETERS:																        */
/* IN			framebrstcnt:the value of the frameburst count                      */
/* OUT			NONE																*/
/* RETURN:		t_hsi_error															*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */

/************************************************************************************/
PUBLIC t_hsi_error HSI_GetRxFrameBurstCnt(OUT t_uint32 *framebrstcnt)
{

	t_hsi_error error_status = HSI_OK;

    *framebrstcnt = g_hsi_system_context.p_hsi_rx->hsir_frameburstcnt;
    
    DBGEXIT0(error_status);
    return(error_status);
}


#endif

/************************************************************************************/
/* NAME:	void HSI_SaveDeviceContext()								     	 	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine saves registers of HSI hardware for power management.	*/
/*																		            */
/* PARAMETERS:																        */
/* IN			NONE											 	  				*/
/* OUT			NONE																*/
/* RETURN:		void																*/
/*																		            */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												        */
/* Global array HSI_context[9] is being modified									*/

/************************************************************************************/
PUBLIC void HSI_SaveDeviceContext(void)
{
    DBGENTER0();

    /*----------------------------------------------------------
		Store the receiver register
	-----------------------------------------------------------*/
    g_hsi_system_context.device_context.hsi_rx_dmaen = g_hsi_system_context.p_hsi_rx->hsir_dmaen;
    g_hsi_system_context.device_context.hsi_rx_id = g_hsi_system_context.p_hsi_rx->hsir_id;
    g_hsi_system_context.device_context.hsi_rx_rxstate = g_hsi_system_context.p_hsi_rx->hsir_rx_state;
    g_hsi_system_context.device_context.hsi_rx_mode = g_hsi_system_context.p_hsi_rx->hsir_mode;
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    g_hsi_system_context.device_context.hsi_rx_parity = g_hsi_system_context.p_hsi_rx->hsir_parity;
#endif
    g_hsi_system_context.device_context.hsi_rx_wmarkid = g_hsi_system_context.p_hsi_rx->hsir_wmarid;
    g_hsi_system_context.device_context.hsi_rx_wmarkmis = g_hsi_system_context.p_hsi_rx->hsir_wmarkmis;

    /*----------------------------------------------------------
		Store the transmitter  register
	-----------------------------------------------------------*/
    g_hsi_system_context.device_context.hsi_tx_dmaen = g_hsi_system_context.p_hsi_tx->hsit_dmaen;
    g_hsi_system_context.device_context.hsi_tx_id = g_hsi_system_context.p_hsi_tx->hsit_id;
    g_hsi_system_context.device_context.hsi_tx_iostate = g_hsi_system_context.p_hsi_tx->hsit_iostate;
    g_hsi_system_context.device_context.hsi_tx_mode = g_hsi_system_context.p_hsi_tx->hsit_mode;
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    g_hsi_system_context.device_context.hsi_tx_parity = g_hsi_system_context.p_hsi_tx->hsit_parity;
#endif
    g_hsi_system_context.device_context.hsi_tx_txstate = g_hsi_system_context.p_hsi_tx->hsit_tx_state;

    DBGEXIT0(HSI_OK);
}

/******************************************************************************/
/* NAME:	void HSI_RestoreDeviceContext()								 	  */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine restore registers of HSI hardware.				  */
/*																	       	  */
/* PARAMETERS:														          */
/* IN			NONE											 		  	  */
/* OUT			NONE														  */
/* RETURN:		void														  */
/*																	          */
/*----------------------------------------------------------------------------*/
/* REENTRANCY:Reentrant												          */

/******************************************************************************/
PUBLIC void HSI_RestoreDeviceContext(void)
{
    DBGENTER0();

    /*----------------------------------------------------------
		Store the receiver register
	-----------------------------------------------------------*/
    g_hsi_system_context.p_hsi_rx->hsir_dmaen = g_hsi_system_context.device_context.hsi_rx_dmaen;
    g_hsi_system_context.p_hsi_rx->hsir_id = g_hsi_system_context.device_context.hsi_rx_id;
    g_hsi_system_context.p_hsi_rx->hsir_rx_state = g_hsi_system_context.device_context.hsi_rx_rxstate;
    g_hsi_system_context.p_hsi_rx->hsir_mode = g_hsi_system_context.device_context.hsi_rx_mode;
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))	
    g_hsi_system_context.p_hsi_rx->hsir_parity = g_hsi_system_context.device_context.hsi_rx_parity;
#endif
    g_hsi_system_context.p_hsi_rx->hsir_wmarid = g_hsi_system_context.device_context.hsi_rx_wmarkid;
    g_hsi_system_context.p_hsi_rx->hsir_wmarkmis = g_hsi_system_context.device_context.hsi_rx_wmarkmis;

    /*----------------------------------------------------------
		Store the transmitter  register
	-----------------------------------------------------------*/
    g_hsi_system_context.p_hsi_tx->hsit_dmaen = g_hsi_system_context.device_context.hsi_tx_dmaen;
    g_hsi_system_context.p_hsi_tx->hsit_id = g_hsi_system_context.device_context.hsi_tx_id;
    g_hsi_system_context.p_hsi_tx->hsit_iostate = g_hsi_system_context.device_context.hsi_tx_iostate;
    g_hsi_system_context.p_hsi_tx->hsit_mode = g_hsi_system_context.device_context.hsi_tx_mode;
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
    g_hsi_system_context.p_hsi_tx->hsit_parity = g_hsi_system_context.device_context.hsi_tx_parity;
#endif
    g_hsi_system_context.p_hsi_tx->hsit_tx_state = g_hsi_system_context.device_context.hsi_tx_txstate;

    DBGEXIT0(HSI_OK);
}

