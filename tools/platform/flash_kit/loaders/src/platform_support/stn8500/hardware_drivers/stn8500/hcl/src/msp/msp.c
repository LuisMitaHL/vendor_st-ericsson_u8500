/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the MSP
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "msp_irqp.h"
#include "msp_p.h"
/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE volatile t_uint32       dummyForWait;


/*--------------------------------------------------------------------------*
 * Local Functions															*
 *--------------------------------------------------------------------------*/
PRIVATE t_msp_fifo_status       msp_GetRxFifoStatus(IN t_msp_device_id msp_device_id);

PRIVATE t_msp_fifo_status       msp_GetTxFifoStatus(IN t_msp_device_id msp_device_id);

PRIVATE void                    msp_ConfigureProtocol(IN t_msp_device_id msp_device_id, IN t_msp_protocol_descriptor const *const);
PRIVATE void                    msp_ConfigureClock(IN t_msp_device_id msp_device_id, IN t_msp_protocol_descriptor const *const);
PRIVATE void                    msp_LocalEnableIrqSource(IN t_msp_irq_src it_to_be_enabled);
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
PRIVATE t_uint32 				msp_GetRxFifoWmrkLevel(IN t_msp_device_id msp_device_id);
PRIVATE t_uint32 				msp_GetTxFifoWmrkLevel(IN t_msp_device_id msp_device_id);
#endif

/*--------------------------------------------------------------------------*
 * debug stuff                                                              *
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_MSP
#define MY_DEBUG_ID             myDebugID_MSP
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = MSP_HCL_DBG_ID;
#endif

#define MSP_ENDAD_COUNTER       500000

/*--------------------------------------------------------------------------*
 * Global data for interrupt mode management                                *
 *--------------------------------------------------------------------------*/
PRIVATE t_msp_system_context    g_msp_system_context[MSP_INSTANCES];

/*--------------------------------------------------------------------------*
 * Private functions                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_msp_error msp_InsertDelay(IN t_msp_device_id msp_device_id)
{
    t_uint32                delay;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;

    delay = ((p_msp_device_context->serial_clock_divisor * 2000000) / p_msp_device_context->msp_in_clock_frequency + 1) + 1;

    if (delay > 1000)
    {
        delay = 1000;
    }

    return(MSP_Delay(msp_device_id, delay));
}

/****************************************************************************/
/* NAME:    DelayForBoard                                                	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Provides delay during enabling / disabling the MSP          */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :                                                    			        */
/*            none											                */
/* OUT: none                                                         		*/
/*                                                                          */
/* RETURN: none															    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: No Issue                                               		*/

/****************************************************************************/
PRIVATE void msp_DelayForBoard(IN t_msp_device_id msp_device_id)
{
    t_uint32        i;
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    /* dummyForWait = dummyForWait; */

    for (i = 0; i < 10; i++)
    {
        dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);
        dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);
    }
}

/****************************************************************************/
/* NAME:    msp_CalculateTransferWidthSize                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Maps Transfer width to its size.                   			*/
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :  TransferWidth: Access width used for writing/reading data register.*/
/*                                                                          */
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:                                                                  */
/*              Size of access width                        			    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: No Issue                                               		*/

/****************************************************************************/
PRIVATE t_uint8 msp_CalculateTransferWidthSize(IN t_msp_data_transfer_width msp_data_transfer_width)
{
    /* By default size equivalent to transfer width is set to 4
	---------------------------------------------------*/
    t_uint8 transfer_width_size = (t_uint8)sizeof(t_uint32);

    /* add transfer width to size conversions here 
	--------------------------------------------------*/
    switch (msp_data_transfer_width)
    {
        case MSP_DATA_TRANSFER_WIDTH_BYTE:
            transfer_width_size = (t_uint8)sizeof(t_uint8);
            break;

        case MSP_DATA_TRANSFER_WIDTH_HALFWORD:
            transfer_width_size = (t_uint8)sizeof(t_uint16);
            break;

        case MSP_DATA_TRANSFER_WIDTH_WORD:
            transfer_width_size = (t_uint8)sizeof(t_uint32);
            break;
    }

    return(transfer_width_size);
}           

PRIVATE t_msp_error msp_FilterProcessIRQSrcProcessRX(IN t_msp_irq_status *p_status_structure, OUT t_msp_event *p_event_id, IN t_msp_filter_mode filter_mode, IN t_msp_filter_mode l_filter)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_device_id         msp_device_id = (t_msp_device_id) ((p_status_structure->pending_irq) >> MSP_SHIFT_BY_TWELVE);
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    volatile t_msp_event    *p_l_event;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    t_uint8     nb_of_bytes_to_transfer; 
    t_uint32    no_byte_transfer; 
    
    p_status_structure->interrupt_state = MSP_IRQ_STATE_OLD;
    p_l_event = &p_msp_system_context->msp_event_status; 
    
    nb_of_bytes_to_transfer = (t_uint8)
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
			(msp_GetRxFifoWmrkLevel(msp_device_id) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width));
#else
            (MSP_FIFO_WATERMARK_LEVEL * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width));
#endif
    no_byte_transfer = p_msp_system_context->rx_status.it_bytes_nb;


        msp_error = MSP_ReadBuffer(msp_device_id, (t_uint8 *) p_msp_system_context->rx_status.p_it_data_flow, &no_byte_transfer);
        p_msp_system_context->rx_status.it_bytes_nb -= no_byte_transfer;
        p_msp_system_context->rx_status.p_it_data_flow += no_byte_transfer;

        if ((l_filter & ((t_uint32) MSP0_IRQ_SRC_RX_SERVICE)))
        {
            p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_RX_SERVICE)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*passing information to user abt the info of interrupt
			that has been serviced
			-------------------------------------------------------*/
            *p_event_id = ((t_uint32) MSP0_IRQ_SRC_RX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*modifying global event
			---------------------------*/
            (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_RX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            if (MSP_FIFO_ENABLE == p_msp_system_context->msp_device_context.general_config.rx_fifo_config)
            {
                if (!(p_msp_system_context->rx_status.it_bytes_nb >= nb_of_bytes_to_transfer))  /* check if <=4 elements to be received now remaining */
                {
                    /* enable Rx FIFO not empty interrupt here to receive elements one by one. */

                    MSP_SET_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_FIFO_NOT_EMPTY_INT_BIT);
                    MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_SERVICE_INTERRUPT_BIT);

                }
            }
        }
        else
        {
            p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_RX_FIFO_NOT_EMPTY)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*passing information to user abt the info of interrupt
			that has been serviced
			-------------------------------------------------------*/
            *p_event_id = ((t_uint32) MSP0_IRQ_SRC_RX_FIFO_NOT_EMPTY) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*modifying global event
			---------------------------*/
            (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_RX_FIFO_NOT_EMPTY) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        }
        
        DBGEXIT(msp_error);
        return(msp_error);        
}

PRIVATE t_msp_error msp_FilterProcessIRQSrcProcessIt(IN t_msp_irq_status *p_status_structure, OUT t_msp_event *p_event_id, IN t_msp_filter_mode filter_mode, IN t_msp_filter_mode l_filter)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_device_id         msp_device_id = (t_msp_device_id) ((p_status_structure->pending_irq) >> MSP_SHIFT_BY_TWELVE);
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    volatile t_msp_event    *p_l_event;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    
    p_status_structure->interrupt_state = MSP_IRQ_STATE_OLD;
    p_l_event = &p_msp_system_context->msp_event_status;

    if ((l_filter & ((t_uint32) MSP0_IRQ_SRC_RX_SERVICE)) || (l_filter & ((t_uint32) MSP0_IRQ_SRC_RX_FIFO_NOT_EMPTY)))
    {
        

        msp_error = msp_FilterProcessIRQSrcProcessRX(p_status_structure, p_event_id, filter_mode, l_filter);
        if(MSP_OK != msp_error)
        {
    	    DBGEXIT(msp_error);
            return(msp_error);
        }

        if (0 == p_msp_system_context->rx_status.it_bytes_nb)   /* All data transfered*/
        {
            /*masking Rx interrupt after data Xmission*/
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_SERVICE_INTERRUPT_BIT);
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_FIFO_NOT_EMPTY_INT_BIT);
            p_msp_system_context->flag_for_receive_interrupt_mode = 0;
        }
    }
    else if (l_filter & ((t_uint32) MSP0_IRQ_SRC_RX_OVERRUN_ERROR))
    {
        p_msp_system_context->rx_status.flow_error_nb++;
        p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_RX_OVERRUN_ERROR)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        *p_event_id = ((t_uint32) MSP0_IRQ_SRC_RX_OVERRUN_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*modifying global event 
		-------------------------*/
        (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_RX_OVERRUN_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_OVERRUN_ERROR_BIT);
    }
    else if (l_filter & ((t_uint32) MSP0_IRQ_SRC_RX_FRAME_ERROR))
    {
        p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_RX_FRAME_ERROR)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*passing information to user abt the info of interrupt 
		that has been serviced
		------------------------------------------------------*/
        *p_event_id = ((t_uint32) MSP0_IRQ_SRC_RX_FRAME_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*modifying global event
		---------------------------*/
        (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_RX_FRAME_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_FRAME_SYNC_ERR_INT_BIT);
    }
    else if (l_filter & ((t_uint32) MSP0_IRQ_SRC_RX_FRAME_SYNC))
    {
        p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_RX_FRAME_SYNC)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*passing information to user abt the info of
		interrupt that has been serviced
		---------------------------------------------*/
        *p_event_id = ((t_uint32) MSP0_IRQ_SRC_RX_FRAME_SYNC) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*modifying global event
		--------------------------*/
        (*p_l_event) |= (((t_uint32) MSP0_IRQ_SRC_RX_FRAME_SYNC) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
        MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_FRAME_SYNC_INT_BIT);
    }
    else if ((l_filter & ((t_uint32) MSP0_IRQ_SRC_TX_SERVICE)) || (l_filter & ((t_uint32) MSP0_IRQ_SRC_TX_FIFO_NOT_FULL)))
    {
        t_uint8     nb_of_bytes_to_transfer = (t_uint8)
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
			(msp_GetTxFifoWmrkLevel(msp_device_id) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width));
#else
            (MSP_FIFO_WATERMARK_LEVEL * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width));
#endif
        t_uint32    no_byte_transfer = p_msp_system_context->tx_status.it_bytes_nb;
        msp_error = MSP_WriteBuffer(msp_device_id, (t_uint8 *) p_msp_system_context->tx_status.p_it_data_flow, &no_byte_transfer);
        p_msp_system_context->tx_status.it_bytes_nb -= no_byte_transfer;
        p_msp_system_context->tx_status.p_it_data_flow += no_byte_transfer;

        if ((l_filter & ((t_uint32) MSP0_IRQ_SRC_TX_SERVICE)))
        {
            if (MSP_FIFO_ENABLE == p_msp_system_context->msp_device_context.general_config.tx_fifo_config)
            {
                if (!(p_msp_system_context->tx_status.it_bytes_nb >= nb_of_bytes_to_transfer))  /* check if <=4 elements to be received now remaining */
                {
                    /* enable Rx FIFO not empty interrupt here to receive elements one by one. */
                    MSP_SET_BIT(p_msp_register->msp_imsc, (t_uint32) TRANSMIT_FIFO_NOT_FULL_INT_BIT);
                    MSP_CLEAR_BIT(p_msp_register->msp_imsc, TRANSMIT_INTERRUPT_SERVICE_BIT);
                }
            }

            p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_TX_SERVICE)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*passing information to user abt the info of interrupt
			that has been serviced
			-------------------------------------------------------*/
            *p_event_id = ((t_uint32) MSP0_IRQ_SRC_TX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*modifying global event
			-------------------------*/
            (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_TX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        }
        else
        {
            p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_TX_FIFO_NOT_FULL)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*passing information to user abt the info of interrupt
			that has been serviced
			-------------------------------------------------------*/
            *p_event_id = ((t_uint32) MSP0_IRQ_SRC_TX_FIFO_NOT_FULL) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

            /*modifying global event
			-------------------------*/
            (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_TX_FIFO_NOT_FULL) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        }

        if (0 == p_msp_system_context->tx_status.it_bytes_nb)
        {
            p_msp_system_context->flag_for_transmit_interrupt_mode = 1;

            /*masking Tx interrupt after data Xmission
			-------------------------------------------*/
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, TRANSMIT_INTERRUPT_SERVICE_BIT);
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) TRANSMIT_FIFO_NOT_FULL_INT_BIT);
        }
    }
    else if (l_filter & (t_uint32) MSP0_IRQ_SRC_TX_UNDERRUN_ERROR)
    {
        p_msp_system_context->tx_status.flow_error_nb++;
        p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_TX_UNDERRUN_ERROR)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*passing information to user abt the info of interrupt that has been serviced
		-----------------------------------------------------------------------------*/
        *p_event_id = ((t_uint32) MSP0_IRQ_SRC_TX_UNDERRUN_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*modifying global event 
		-----------------------*/
        (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_TX_UNDERRUN_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_UNDERRUN_ERR_INT_BIT);
    }
    else if (l_filter & ((t_uint32) MSP0_IRQ_SRC_TX_FRAME_ERROR))
    {
        p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_TX_FRAME_ERROR)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*passing information to user abt the info of interrupt 
		that has been serviced
		-------------------------------------------------------*/
        *p_event_id = ((t_uint32) MSP0_IRQ_SRC_TX_FRAME_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*modifying global event 
		-------------------------*/
        (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_TX_FRAME_ERROR) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        MSP_SET_BIT(p_msp_register->msp_icr, (t_uint32) TRANSMIT_FRAME_SYNC_ERR_INT_BIT);
    }
    else if (l_filter & ((t_uint32) MSP0_IRQ_SRC_TX_FRAME_SYNC))
    {
        p_status_structure->pending_irq |= (((t_uint32) MSP0_IRQ_SRC_TX_FRAME_SYNC)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*passing information to user abt the info of interrupt 
		that has been serviced
		----------------------------------------------------------*/
        *p_event_id = ((t_uint32) MSP0_IRQ_SRC_TX_FRAME_SYNC) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

        /*modifying global event 
		---------------------------*/
        (*p_l_event) |= ((t_uint32) MSP0_IRQ_SRC_TX_FRAME_SYNC) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_FRAME_SYNC_INT_BIT);
    }
    
    DBGEXIT(msp_error);
    return(msp_error);
}

PRIVATE void msp_ProcessIRQSrcServiceIt(IN t_msp_irq_status *p_status_structure)
{    
    t_msp_device_id         msp_device_id = (t_msp_device_id) ((p_status_structure->pending_irq) >> MSP_SHIFT_BY_TWELVE);
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    
    /*Servicing Tx Frame Sync Interrupt 
	-------------------------------------*/
    if (((t_uint32) MSP0_IRQ_SRC_TX_FRAME_SYNC & p_status_structure->pending_irq) != 0)
    {
        MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_FRAME_SYNC_INT_BIT);
    }

    /*Servicing Overrun Interrupt 
	------------------------------*/
    if (((t_uint32) MSP0_IRQ_SRC_RX_OVERRUN_ERROR & p_status_structure->pending_irq) != 0)
    {
        p_msp_system_context->rx_status.flow_error_nb++;
        MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_OVERRUN_ERROR_BIT);
    }

    /*Servicing Rx Frame error Interrupt 
	-----------------------------------*/
    if (((t_uint32) MSP0_IRQ_SRC_RX_FRAME_ERROR & p_status_structure->pending_irq) != 0)
    {
        MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_FRAME_SYNC_ERR_INT_BIT);
    }

    /*Servicing Tx UnderRun Interrupt */
    if (((t_uint32) MSP0_IRQ_SRC_TX_UNDERRUN_ERROR & p_status_structure->pending_irq) != 0)
    {
        p_msp_system_context->tx_status.flow_error_nb++;
        MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_UNDERRUN_ERR_INT_BIT);
    }

    /*Servicing Tx Frame Error Interrupt
	------------------------------------- */
    if (((t_uint32) MSP0_IRQ_SRC_TX_FRAME_ERROR & p_status_structure->pending_irq) != 0)
    {
        MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_FRAME_SYNC_ERR_INT_BIT);
    }        
}

PRIVATE t_msp_error msp_EnableErrorCheck(IN t_msp_device_id msp_device_id)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (FALSE == p_msp_device_context->msp_general_configuration_done || FALSE == p_msp_device_context->msp_protocol_descriptor_set)
    {
        msp_error = MSP_NOT_CONFIGURED;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (MSP_DIRECT_COMPANDING_MODE_ENABLE != p_msp_device_context->general_config.msp_direct_companding_mode)
    {
        if (FALSE == p_msp_device_context->serial_clock_divisor_set)    /* test call to Serial clock devisor if not configured in DCM*/
        {
            msp_error = MSP_NOT_CONFIGURED;
            DBGEXIT(msp_error);
            return(msp_error);
        }
    }
    
    DBGEXIT(msp_error);
    return(msp_error);
}

PRIVATE void msp_LocalEnableIrqSource(IN t_msp_irq_src it_to_be_enabled)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) (it_to_be_enabled >> MSP_SHIFT_BY_TWELVE);
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    p_msp_register->msp_imsc |= (it_to_be_enabled & MSP_MASK_INTERRUPT_BITS);

    /*delay after enabling the interrupt
        -------------------------------------*/
    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);
}

PRIVATE void msp_ConfigureClock(IN t_msp_device_id msp_device_id, IN t_msp_protocol_descriptor const *const p_protocol_desc)
{
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    /* Before configuring, clocks must be stopped
	----------------------------------------------*/
    t_uint32        fr_period = p_protocol_desc->frame_period;
    t_uint32        fr_width = p_protocol_desc->frame_width;

    MSP_WRITE_BITS(p_msp_register->msp_srg, fr_period, THIRTEEN_BITS, FRPER_BIT);
    MSP_WRITE_BITS(p_msp_register->msp_srg, fr_width, SIX_BITS, FRWID_BIT);

    /* To ensure correct initialization, there is a need to wait two cycles
	------------------------------------------------------------------------*/
    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);

    /*  wait two cycles for the MSP to re-intialize and then configure
	SPI clock modes
	------------------------------------------------------------------------*/
    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_gcr, ((t_uint32) p_protocol_desc->spi_clk_mode), TWO_BITS, SPICKM_BIT);

    dummyForWait = MSP_READ_BITS(p_msp_register->msp_srg, TEN_BITS, FRWID_BIT);


    MSP_WRITE_BITS(p_msp_register->msp_gcr, ((t_uint32) p_protocol_desc->spi_burst_mode), ONE_BIT, SPIBURST_BIT);

}

PRIVATE void msp_ConfigureProtocol(IN t_msp_device_id msp_device_id, IN t_msp_protocol_descriptor const *const p_msp_protocol_descriptor)
{
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    p_msp_system_context->rx_status.phase_mode = p_msp_protocol_descriptor->rx_phase_mode;  /* Phase modes recorded */
    p_msp_system_context->tx_status.phase_mode = p_msp_protocol_descriptor->tx_phase_mode;

    /* Receive mode
	-------------------*/
    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) (p_msp_protocol_descriptor->rx_phase_mode)), ONE_BIT, P2EN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) (p_msp_protocol_descriptor->rx_phase2_start_mode)), ONE_BIT, P2SM_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) (p_msp_protocol_descriptor->rx_frame_length_1)), SEVEN_BITS, P1FLEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) (p_msp_protocol_descriptor->rx_frame_length_2)), SEVEN_BITS, P2FLEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) (p_msp_protocol_descriptor->rx_element_length_1)), THREE_BITS, P1ELEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) (p_msp_protocol_descriptor->rx_element_length_2)), THREE_BITS, P2ELEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) (p_msp_protocol_descriptor->rx_data_delay)), TWO_BITS, DDLY_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_gcr, ((t_uint32) (!p_msp_protocol_descriptor->rx_clock_pol)), ONE_BIT, RCKPOL_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_gcr, ((t_uint32) (p_msp_protocol_descriptor->tx_clock_pol)), ONE_BIT, TCKPOL_BIT);

    /* Transmit mode
	------------------*/
    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) (p_msp_protocol_descriptor->tx_phase_mode)), ONE_BIT, P2EN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) (p_msp_protocol_descriptor->tx_phase2_start_mode)), ONE_BIT, P2SM_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) (p_msp_protocol_descriptor->tx_frame_length_1)), SEVEN_BITS, P1FLEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) (p_msp_protocol_descriptor->tx_frame_length_2)), SEVEN_BITS, P2FLEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) (p_msp_protocol_descriptor->tx_element_length_1)), THREE_BITS, P1ELEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) (p_msp_protocol_descriptor->tx_element_length_2)), THREE_BITS, P2ELEN_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) (p_msp_protocol_descriptor->tx_data_delay)), TWO_BITS, DDLY_BIT);

    MSP_WRITE_BITS(p_msp_register->msp_gcr, ((t_uint32) (p_msp_protocol_descriptor->tx_clock_pol)), ONE_BIT, TCKPOL_BIT);

    /* Receive/Transmit endianess
	------------------------------*/
    MSP_WRITE_BITS(p_msp_register->msp_tcf, (t_uint32) p_msp_protocol_descriptor->tx_bit_transfer_format, ONE_BIT, ENDN_BIT);
    MSP_WRITE_BITS(p_msp_register->msp_rcf, (t_uint32) p_msp_protocol_descriptor->rx_bit_transfer_format, ONE_BIT, ENDN_BIT);

    /* Receive/Transmit frame sync polarity
	 ----------------------------------------*/
    MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_protocol_descriptor->tx_frame_sync_pol, ONE_BIT, TFSPOL_BIT);
    MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_protocol_descriptor->rx_frame_sync_pol, ONE_BIT, RFSPOL_BIT);


    /* Byte swap features are available for these platform*/
    MSP_WRITE_BITS(p_msp_register->msp_tcf, (t_uint32) p_msp_protocol_descriptor->tx_half_word_swap, TWO_BITS, TBSWAP_BIT);
    MSP_WRITE_BITS(p_msp_register->msp_rcf, (t_uint32) p_msp_protocol_descriptor->rx_half_word_swap, TWO_BITS, RBSWAP_BIT);

    /* Companding modes programming
    -------------------------------*/
    MSP_WRITE_BITS(p_msp_register->msp_tcf, ((t_uint32) p_msp_protocol_descriptor->compression_mode), TWO_BITS, DTYP_BIT);
    MSP_WRITE_BITS(p_msp_register->msp_rcf, ((t_uint32) p_msp_protocol_descriptor->expansion_mode), TWO_BITS, DTYP_BIT);
}

PRIVATE t_msp_fifo_status msp_GetRxFifoStatus(IN t_msp_device_id msp_device_id)
{
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return(MSP_FIFO_FULL);
    }

    if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, RFE_BIT)) >> RFE_BIT)))
    {
        return(MSP_FIFO_EMPTY);
    }
    else if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, RFU_BIT)) >> RFU_BIT)))
    {
        return(MSP_FIFO_FULL);
    }
    else
    {
        return(MSP_FIFO_PARTIALLY_FILLED);
    }
}


PRIVATE t_msp_fifo_status msp_GetTxFifoStatus(IN t_msp_device_id msp_device_id)
{
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return(MSP_FIFO_EMPTY);
    }

    if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, TFE_BIT)) >> TFE_BIT)))
    {
        return(MSP_FIFO_EMPTY);
    }
    else if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, TFU_BIT)) >> TFU_BIT)))
    {
        return(MSP_FIFO_FULL);
    }
    else
    {
        return(MSP_FIFO_PARTIALLY_FILLED);
    }
}
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
PRIVATE t_uint32 msp_GetRxFifoWmrkLevel(IN t_msp_device_id msp_device_id)
{
	t_msp_register *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
	t_msp_wmrk_level msp_wmrk_level;
	t_uint32 wmrk_level=0;
	
	if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return((t_uint32)DEFAULT_WMRK_LEVEL);
    }
	msp_wmrk_level = (t_msp_wmrk_level)(p_msp_register->msp_wmrk & RWMRK_MASK);
	switch(msp_wmrk_level)
	{
		case MSP_TWO_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_2;
			break;
			
		case MSP_FOUR_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_4;
			break;
		
		case MSP_EIGHT_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_8;
			break;
		
		case MSP_SIXTEEN_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_16;
			break;
	}
	return(wmrk_level);
}

PRIVATE t_uint32 msp_GetTxFifoWmrkLevel(IN t_msp_device_id msp_device_id)
{
	t_msp_register *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
	t_msp_wmrk_level msp_wmrk_level; 
	t_uint32 wmrk_level=0;
	
	if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return((t_uint32)DEFAULT_WMRK_LEVEL);
    }
	msp_wmrk_level = (t_msp_wmrk_level)(p_msp_register->msp_wmrk & TWMRK_MASK);
	switch(msp_wmrk_level)
	{
		case MSP_TWO_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_2;
			break;
			
		case MSP_FOUR_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_4;
			break;
		
		case MSP_EIGHT_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_8;
			break;
		
		case MSP_SIXTEEN_ELEMENTS:
			wmrk_level = (t_uint32)ELEMENTS_16;
			break;			
	}
	return(wmrk_level);
}
#endif 

/*--------------------------------------------------------------------------*
 * Public functions                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:  	t_msp_error MSP_Init()                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine initializes MSP register base address.		*/
/*					It also checks for peripheral Ids and PCell Ids.		*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:	msp_base_address     :  MSP registers base address      */
/* 		OUT 	:	none                                                    */
/*                                                                          */
/* RETURN		:	MSP_UNSUPPORTED_HW   :  This is returned  if peripheral	*/
/*                       				    ids don't match                	*/
/*					MSP_INVALID_PARAMETER:	This values is returned if base	*/
/*                             			    address passed is zero			*/
/*					MSP_OK               :	Returns this if no error        */
/*                                          deteceted	                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY 	    : 	Non Re-entrant                                      */
/* REENTRANCY ISSUES:														*/
/*			1)		Global variable p_msp_register(register base address)	*/
/*					is being modified										*/

/****************************************************************************/
PUBLIC t_msp_error MSP_Init(IN t_msp_device_id msp_device_id, IN t_logical_address msp_base_address)
{
    DBGENTER2("(%x %lx)", msp_device_id, msp_base_address);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        DBGEXIT0(MSP_INVALID_PARAMETER);
        return(MSP_INVALID_PARAMETER);
    }

    if (NULL != msp_base_address)
    {
        t_msp_register  *p_msp_register;

        /* Initializing the registers structure
        ---------------------------------------*/
        p_msp_register = g_msp_system_context[msp_device_id].p_msp_register = (t_msp_register *) msp_base_address;

        /* Checking Peripheral Ids *
         *-------------------------*/
        if
        (
            (MSP_P_ID0 == p_msp_register->msp_periphid0) &&
            (MSP_P_ID1 == p_msp_register->msp_periphid1) &&
            (MSP_P_ID2 == p_msp_register->msp_periphid2) &&
            (MSP_P_ID3 == p_msp_register->msp_periphid3) &&
            (MSP_CELL_ID0 == p_msp_register->msp_cellid0) &&
            (MSP_CELL_ID1 == p_msp_register->msp_cellid1) &&
            (MSP_CELL_ID2 == p_msp_register->msp_cellid2) &&
            (MSP_CELL_ID3 == p_msp_register->msp_cellid3)
        )
        {
            DBGEXIT0(MSP_OK);
            return(MSP_OK);
        }
        else
        {
            DBGEXIT0(MSP_UNSUPPORTED_HW);
            return(MSP_UNSUPPORTED_HW);
        }
    }   /* end if */

    DBGEXIT0(MSP_INVALID_PARAMETER);
    return(MSP_INVALID_PARAMETER);
}

/****************************************************************************/
/* NAME:    MSP_Configure                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Configures the MSP										*/
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :       msp_device_id:        Device Id for Identification		    */
/*   msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 	*/
/*   p_msp_general_config: Pointer to general purpose structure.            */
/*	   (protocol independent structure)                                     */
/*   p_msp_protocol_desc: Pointer to protocol descriptor structure          */
/*	   (protocol dependent structure).                                      */
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:    MSP_INVALID_PARAMETER: returned if all pointers are NULL. 	*/
/*                or if msp_device_id is invalid  or Value of 				*/
/* p_msp_gereral_config->srg_clock_sel must be MSP_SRG_IN_CLOCK_APB  in case*/
/*	msp_device_id is MSP_DEVICE_ID_2.  										*/
/*			MSP_REQUEST_NOT_APPLICABLE: if MSP is already enabled			*/
/*          MSP_OK             : if everything goes fine.                 	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: ReEntrant                                                    */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC t_msp_error MSP_Configure(IN t_msp_device_id msp_device_id, IN t_msp_general_config const *const p_msp_general_config, t_msp_protocol_descriptor const *const p_msp_protocol_descriptor)
{
    t_msp_error             msp_error = MSP_OK;
    t_uint16                total_clocks_for_one_frame;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;

    /*t_msp_system_context* p_msp_system_context = g_msp_system_context + msp_device_id;*/
    DBGENTER3(" (%x %lx %lx)", msp_device_id, p_msp_general_config, p_msp_protocol_descriptor);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL == p_msp_protocol_descriptor && NULL == p_msp_general_config)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL != p_msp_protocol_descriptor)
    {
        total_clocks_for_one_frame = p_msp_protocol_descriptor->total_clocks_for_one_frame;

        if (0 == total_clocks_for_one_frame)
        {
            msp_error = MSP_INVALID_PARAMETER;
            DBGEXIT(msp_error);
            return(msp_error);
        }
    }

    if (TRUE == p_msp_device_context->rx_enable || TRUE == p_msp_device_context->tx_enable)
    {
        msp_error = MSP_REQUEST_NOT_APPLICABLE;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL != p_msp_general_config)
    {
        if (MSP_DEVICE_ID_2 == msp_device_id && MSP_SRG_IN_CLOCK_APB != p_msp_general_config->srg_clock_sel)
        {
            msp_error = MSP_INVALID_PARAMETER;
            DBGEXIT(msp_error);
            return(msp_error);
        }

        /* Receive/Transmit clock configuration 
		(from an external source or generated by the SRG)
		--------------------------------------------------*/
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->tx_clock_sel, ONE_BIT, TCKSEL_BIT);
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->rx_clock_sel, ONE_BIT, RCKSEL_BIT);

        /* Unexpected frame sync in receive/transmit mode
	        --------------------------------------------------*/
        MSP_WRITE_BITS(p_msp_register->msp_tcf, (t_uint32) p_msp_general_config->tx_unexpect_frame_sync, ONE_BIT, FSIG_BIT);
        MSP_WRITE_BITS(p_msp_register->msp_rcf, (t_uint32) p_msp_general_config->rx_unexpect_frame_sync, ONE_BIT, FSIG_BIT);

        /* Receive/Transmit frame sync selection
		----------------------------------------*/
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->tx_frame_sync_sel, TWO_BITS, TFSSEL_BIT);
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->rx_frame_sync_sel, ONE_BIT, RFSSEL_BIT);

        /* Receive/Transmit FIFO enable
		--------------------------------*/
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->tx_fifo_config, ONE_BIT, TFFEN_BIT);
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->rx_fifo_config, ONE_BIT, RFFEN_BIT);            

        /* Sample rate generator clock selection
		-------------------------------------*/
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->srg_clock_sel, TWO_BITS, SCKSEL_BIT);

        /* Co-processor, loop back mode
		-------------------------------*/
        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->msp_direct_companding_mode, ONE_BIT, DCM_BIT);

        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->msp_loopback_mode, ONE_BIT, LBM_BIT);

        MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_general_config->tx_extra_delay, ONE_BIT, TXDDL_BIT);

        /* Enable Rx-DMA interface
		----------------------------*/
        if (MSP_MODE_DMA == p_msp_general_config->rx_msp_mode)
        {
            MSP_SET_BIT(p_msp_register->msp_dmacr, RDMAE_BIT);
        }
        else
        {
            MSP_CLEAR_BIT(p_msp_register->msp_dmacr, RDMAE_BIT);
        }

        /* Enable Tx-DMA interface
		----------------------------*/
        if (MSP_MODE_DMA == p_msp_general_config->tx_msp_mode)
        {
            MSP_SET_BIT(p_msp_register->msp_dmacr, TDMAE_BIT);
        }
        else
        {
            MSP_CLEAR_BIT(p_msp_register->msp_dmacr, TDMAE_BIT);
        }

        p_msp_device_context->general_config = *p_msp_general_config;

        p_msp_device_context->msp_general_configuration_done = TRUE;
    }

    if (NULL != p_msp_protocol_descriptor)
    {
        /* Clock configuration
		------------------------*/
        msp_ConfigureClock(msp_device_id, p_msp_protocol_descriptor);
        msp_DelayForBoard(msp_device_id);

        /* MSP configured with protocol dependent settings
		----------------------------------------------------*/
        msp_ConfigureProtocol(msp_device_id, p_msp_protocol_descriptor);

        p_msp_device_context->protocol_descriptor = *p_msp_protocol_descriptor;
        p_msp_device_context->msp_protocol_descriptor_set = TRUE;
    }

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_ConfigureMultiChannel                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Configures the multichannel mode.                        */
/*            If the protocol chosen is not a single phase one, the T/R     */
/*                part can not be enabled.                                  */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :       msp_device_id:        Device Id for Identification		    */
/*        p_multichannel_config:    Configuration of the multichannel mode. */
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:    MSP_NON_AUTHORIZED_MODE is returned when function is called 	*/
/*			with phase mode is dual phase    								*/
/*            MSP_INVALID_PARAMETER:    if msp_device_id is incorrect		*/
/*					            or if invalid pointer is passed    			*/
/*			  MSP_REQUEST_NOT_APPLICABLE: if MSP is already enable.			*/
/*            MSP_OK :             returned if everything goes fine.        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  Re-Entrant                                                  */
/* REENTRANCY ISSUES:    No Issues                                          */

/****************************************************************************/
PUBLIC t_msp_error MSP_ConfigureMultiChannel(IN t_msp_device_id msp_device_id, IN t_msp_multichannel_config const *const p_multichannel_config)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;

    DBGENTER2(" (%x %lx)", msp_device_id, p_multichannel_config);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return(MSP_INVALID_PARAMETER);
    }

    if (TRUE == p_msp_device_context->rx_enable || TRUE == p_msp_device_context->tx_enable)
    {
        msp_error = MSP_REQUEST_NOT_APPLICABLE;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL != p_multichannel_config)
    {
        /* Multichannel mode enabling. Chks that the mode prev sel is authorized.
    ----------------------------------------------------------------------*/
        if (MSP_PHASE_MODE_SINGLE == p_msp_system_context->tx_status.phase_mode)
        {
            MSP_WRITE_BITS(p_msp_register->msp_mcr, ((t_uint32) p_multichannel_config->tx_multichannel_enable), ONE_BIT, TMCEN_BIT);

            /* Channel enabling for Transmit mode.
				---------------------------------------*/
            p_msp_register->msp_tce0 = p_multichannel_config->tx_channel_0_enable;
            p_msp_register->msp_tce1 = p_multichannel_config->tx_channel_1_enable;
            p_msp_register->msp_tce2 = p_multichannel_config->tx_channel_2_enable;
            p_msp_register->msp_tce3 = p_multichannel_config->tx_channel_3_enable;
        }
        else
        {
            DBGPRINT(DBGL_ERROR, " Transmit multichannel mode not authorized");
            msp_error = MSP_NON_AUTHORIZED_MODE;
        }

        if (MSP_PHASE_MODE_SINGLE == p_msp_system_context->rx_status.phase_mode)
        {
            MSP_WRITE_BITS(p_msp_register->msp_mcr, ((t_uint32) p_multichannel_config->rx_multichannel_enable), ONE_BIT, RMCEN_BIT);

            /* Channel enabling for Receive mode.
	            ------------------------------------*/
            p_msp_register->msp_rce0 = p_multichannel_config->rx_channel_0_enable;
            p_msp_register->msp_rce1 = p_multichannel_config->rx_channel_1_enable;
            p_msp_register->msp_rce2 = p_multichannel_config->rx_channel_2_enable;
            p_msp_register->msp_rce3 = p_multichannel_config->rx_channel_3_enable;
        }
        else
        {
            DBGPRINT(DBGL_ERROR, " Receive multichannel mode not authorized");
            msp_error = MSP_NON_AUTHORIZED_MODE;
        }

        /* Comparison configuration
		----------------------------*/
        MSP_WRITE_BITS(p_msp_register->msp_mcr, ((t_uint32) p_multichannel_config->rx_comparison_enable_mode), TWO_BITS, RCMPM_BIT);

        p_msp_register->msp_rcm = p_multichannel_config->comparison_mask;
        p_msp_register->msp_rcv = p_multichannel_config->comparison_value;

        p_msp_device_context->multichannel_config = *p_multichannel_config;
        p_msp_device_context->multichannel_configured = TRUE;
    }
    else
    {
        msp_error = MSP_INVALID_PARAMETER;
    }

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_SetSampleFrequency                                      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:																*/
/* Set sample frequency. This API is exclusive to MSP_SetSerialClockDevisor.*/
/* This API calculates appropriate serial clock devisor for required sample */
/* frequency.																*/
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/* msp_in_clock_frequency: Input clock frequency to MSP                     */
/* sample_frequency: required sample frequency                              */
/* 										                                    */
/* OUT:																		*/
/* p_effective_sample_frequency: pointer to location which receives         */
/* effective clock frequency possible by supplied msp_in_clock_frequency    */
/* and protocol selected.		                                            */
/*																			*/
/* RETURN:                                                                  */
/* MSP_INVALID_PARAMETER: if msp_device_id is not valid. or if 				*/
/* sample_frequency	or msp_in_clock_frequency is 0							*/
/* MSP_NOT_CONFIGURED: if reffered MSP is not configured					*/
/* MSP_OK: if everything goes fine.                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: No Issue		                                                */

/****************************************************************************/
PUBLIC t_msp_error MSP_SetSampleFrequency
(
    IN t_msp_device_id  msp_device_id,
    IN t_uint32         msp_in_clock_frequency /* 48000 = 48MHz */,
    t_uint32            sample_frequency,
    OUT t_uint32        *p_effective_sample_frequency
)
{
    t_msp_error             msp_error = MSP_OK;
    t_uint32                serial_clock_devisor = 1;

    /*t_msp_system_context* p_msp_system_context = g_msp_system_context + msp_device_id;*/
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;

    /*t_msp_register* p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;*/
    DBGENTER4(" (%x %lx %lx %lx)", msp_device_id, msp_in_clock_frequency, sample_frequency, p_effective_sample_frequency);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (0 == sample_frequency || 0 == msp_in_clock_frequency)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (TRUE != p_msp_device_context->msp_protocol_descriptor_set)
    {
        msp_error = MSP_NOT_CONFIGURED;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    /* TBD - increase accuracy*/
    serial_clock_devisor = (msp_in_clock_frequency / (sample_frequency * p_msp_device_context->protocol_descriptor.total_clocks_for_one_frame));

    if (NULL != p_effective_sample_frequency)
    {
        if (0 == serial_clock_devisor)
        {
            *p_effective_sample_frequency = msp_in_clock_frequency / ((p_msp_device_context->protocol_descriptor.total_clocks_for_one_frame));
        }
        else
        {
            *p_effective_sample_frequency = msp_in_clock_frequency / (serial_clock_devisor * (p_msp_device_context->protocol_descriptor.total_clocks_for_one_frame));
        }
    }

    msp_error = MSP_SetSerialClockdevisor(msp_device_id, msp_in_clock_frequency, serial_clock_devisor);

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_SetSerialClockdevisor                                      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:																*/
/* To set serial clock devisor. This API is exclusive to 					*/
/* MSP_SetSampleFrequency. This API directly programs the serial clock      */
/* devisor to hardware                                                      */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/* msp_in_clock_frequency: Input clock frequency to MSP                     */
/* serial_clock_devisor: value to set serial clock devisor. (0 to 1023)     */
/* 										                                    */
/* OUT:																		*/
/*	None																	*/
/*																			*/
/* RETURN:                                                                  */
/* MSP_INVALID_PARAMETER: returned if null pointer is passed. Serial clock  */
/* devisor is incorrect. Or if msp_device_id is not valid.                  */
/* MSP_OK: if everything goes fine.                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: No Issue		                                                */

/****************************************************************************/
PUBLIC t_msp_error MSP_SetSerialClockdevisor(IN t_msp_device_id msp_device_id, IN t_uint32 msp_in_clock_frequency, IN t_uint32 serial_clock_devisor)
{
    t_msp_error             msp_error = MSP_OK;
    t_uint32                sck_div = 0;

    /*t_msp_system_context	*p_msp_system_context = g_msp_system_context + msp_device_id;*/
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    DBGENTER3(" (%x %lx %lx)", msp_device_id, msp_in_clock_frequency, serial_clock_devisor);

    p_msp_device_context->msp_in_clock_frequency = msp_in_clock_frequency;

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (serial_clock_devisor > 1024)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (0 == msp_in_clock_frequency)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (0 == serial_clock_devisor)
    {
        sck_div = serial_clock_devisor;
    }
    else
    {
        sck_div = serial_clock_devisor - 1;
    }

 
    p_msp_device_context->serial_clock_divisor = serial_clock_devisor;

    /*
	msp_error = msp_InsertDelay(msp_device_id);
	if (MSP_OK != msp_error)
	{
        return msp_error;
	}
	*/
    MSP_WRITE_BITS(p_msp_register->msp_srg, sck_div, TEN_BITS, SCKDIV_BIT);

    p_msp_device_context->serial_clock_divisor_set = TRUE;

    /*
	msp_error = msp_InsertDelay(msp_device_id);
	*/
    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_Enable                                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: 															*/
/* Enables MSP in specified direction.			                            */
/*																			*/
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/* msp_direction: Part of the MSP to be enabled (receive, transmit or both).*/
/*																			*/
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:                                                                  */
/* MSP_INVALID_PARAMETER: if msp_device_id is not valid.					*/
/* MSP_NOT_CONFIGURED: MSP is not configured.                               */
/* MSP_OK: if successful.                                                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                               	*/
/* REENTRANCY ISSUES:   No issue											*/

/****************************************************************************/
PUBLIC t_msp_error MSP_Enable(IN t_msp_device_id msp_device_id, IN t_msp_direction msp_direction)
{
    t_msp_error             msp_error = MSP_OK;
    t_bool                  tx_to_enable = FALSE;
    t_bool                  rx_to_enable = FALSE;
    t_bool                  enable_sgen_and_fgen = FALSE;

    /*t_msp_system_context* p_msp_system_context = g_msp_system_context + msp_device_id;*/
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;

    DBGENTER2(" (%x %d)", msp_device_id, msp_direction);

    msp_error = msp_EnableErrorCheck(msp_device_id);
    if (MSP_OK != msp_error)
    {
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    switch (msp_direction)
    {
        case MSP_DIRECTION_TRANSMIT:
            tx_to_enable = TRUE;
            break;

        case MSP_DIRECTION_RECEIVE:
            rx_to_enable = TRUE;
            break;

        case MSP_DIRECTION_TRANSMIT_AND_RECEIVE:
            rx_to_enable = TRUE;
            tx_to_enable = TRUE;
            break;
    }

    if (!(TRUE == tx_to_enable && FALSE == p_msp_device_context->tx_enable))
    {
        tx_to_enable = FALSE;
    }

    if (!(TRUE == rx_to_enable && FALSE == p_msp_device_context->rx_enable))
    {
        rx_to_enable = FALSE;
    }

    if (TRUE == tx_to_enable || TRUE == rx_to_enable)
    {
        enable_sgen_and_fgen = TRUE;
    }

    if (TRUE == enable_sgen_and_fgen)
    {
        /* Clock enabled
		--------------------*/
        MSP_SET_BIT(p_msp_register->msp_gcr, SGEN_BIT);

        msp_error = msp_InsertDelay(msp_device_id);
    }

    if (TRUE == tx_to_enable)
    {
        if (MSP_DIRECT_COMPANDING_MODE_ENABLE != p_msp_device_context->general_config.msp_direct_companding_mode)
        {
            /* Tx part enable*/
            MSP_SET_BIT(p_msp_register->msp_gcr, TXEN_BIT);
        }

        p_msp_device_context->tx_enable = TRUE;
    }

    if (TRUE == rx_to_enable)
    {
        if (MSP_DIRECT_COMPANDING_MODE_ENABLE != p_msp_device_context->general_config.msp_direct_companding_mode)
        {
            /* Tx part enable*/
            MSP_SET_BIT(p_msp_register->msp_gcr, RXEN_BIT);
        }

        p_msp_device_context->rx_enable = TRUE;
    }

    if (TRUE == enable_sgen_and_fgen)
    {
        /* enabling frame generation logic
		-------------------------------*/
        MSP_SET_BIT(p_msp_register->msp_gcr, FGEN_BIT);
    }

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_Disable                                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Disables the MSP.                                        */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/* msp_direction: Part of the MSP to be disabled (receive, transmit or both)*/
/*																			*/
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:																	*/
/* MSP_INVALID_PARAMETER: if msp_device_id is not valid.					*/
/* MSP_OK: if successful.                                                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                               	*/
/* REENTRANCY ISSUES:No Issue												*/

/****************************************************************************/
PUBLIC t_msp_error MSP_Disable(IN t_msp_device_id msp_device_id, IN t_msp_direction msp_direction)
{
    t_msp_error             msp_error = MSP_OK;
    t_bool                  tx_to_disable = FALSE;
    t_bool                  rx_to_disable = FALSE;
    t_bool                  disable_sgen_and_fgen = FALSE;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    /*t_msp_system_context* p_msp_system_context = g_msp_system_context + msp_device_id;*/
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;
    DBGENTER2(" (%x %lx)", msp_device_id, (t_uint32) msp_direction);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    switch (msp_direction)
    {
        case MSP_DIRECTION_TRANSMIT:
            tx_to_disable = TRUE;
            break;

        case MSP_DIRECTION_RECEIVE:
            rx_to_disable = TRUE;
            break;

        case MSP_DIRECTION_TRANSMIT_AND_RECEIVE:
            rx_to_disable = TRUE;
            tx_to_disable = TRUE;
            break;
    }

    if ((TRUE == tx_to_disable || FALSE == p_msp_device_context->tx_enable) && (TRUE == rx_to_disable || FALSE == p_msp_device_context->rx_enable))
    {
        disable_sgen_and_fgen = TRUE;
    }

    if (TRUE == disable_sgen_and_fgen)
    {
        /* Clock enabled
		--------------------*/
        MSP_CLEAR_BIT(p_msp_register->msp_gcr, SGEN_BIT);
    }

    if (TRUE == tx_to_disable)
    {
        /* Tx part enable*/
        MSP_CLEAR_BIT(p_msp_register->msp_gcr, TXEN_BIT);

        if (MSP_MODE_IT == p_msp_device_context->general_config.tx_msp_mode)
        {
            MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_UNDERRUN_ERR_INT_BIT);
            MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_FRAME_SYNC_ERR_INT_BIT);
            MSP_SET_BIT(p_msp_register->msp_icr, TRANSMIT_FRAME_SYNC_INT_BIT);

            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) TRANSMIT_INTERRUPT_SERVICE_BIT);
        }

        p_msp_device_context->tx_enable = FALSE;
    }

    if (TRUE == rx_to_disable)
    {
        /* Rx part enable*/
        MSP_CLEAR_BIT(p_msp_register->msp_gcr, RXEN_BIT);

        if (MSP_MODE_IT == p_msp_device_context->general_config.rx_msp_mode)
        {
            MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_OVERRUN_ERROR_BIT);
            MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_FRAME_SYNC_ERR_INT_BIT);
            MSP_SET_BIT(p_msp_register->msp_icr, RECEIVE_FRAME_SYNC_INT_BIT);

            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_SERVICE_INTERRUPT_BIT);
        }

        p_msp_device_context->rx_enable = FALSE;
    }

    if (TRUE == disable_sgen_and_fgen)
    {
        /* enabling frame generation logic
		-------------------------------*/
        MSP_CLEAR_BIT(p_msp_register->msp_gcr, FGEN_BIT);
    }

    /*
	msp_error = msp_InsertDelay(msp_device_id);
	*/
    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_SetDbgLevel                                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the debug level for the MSP HCL.          */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:    t_dbg_level        dbg_level from DEBUG_LEVEL0 to DEBUG_LEVEL4.  	*/
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:        MSP_OK                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_msp_error MSP_SetDbgLevel(IN t_dbg_level dbg_level)
{
    DBGENTER1(" (%d)", dbg_level);
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = dbg_level;
#endif
    DBGEXIT(MSP_OK);
    return(MSP_OK);
}

/****************************************************************************/
/* NAME:    MSP_GetDbgLevel                                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the debug level for the MSP HCL.          */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:  none															    */
/* OUT: p_dbg_level from DEBUG_LEVEL0 to DEBUG_LEVEL4. 						*/
/*                                                                          */
/* RETURN:				                                                    */
/* MSP_INVALID_PARAMETER: if p_dbg_level is null.							*/
/* MSP_OK: if successful                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetDbgLevel(OUT t_dbg_level *p_dbg_level)
{
    DBGENTER1(" (%lx)", p_dbg_level);

    if (NULL == p_dbg_level)
    {
        DBGEXIT(MSP_INVALID_PARAMETER);
        return(MSP_INVALID_PARAMETER);
    }

#ifdef __DEBUG
    * p_dbg_level = MY_DEBUG_LEVEL_VAR_NAME;
#endif
    DBGEXIT(MSP_OK);
    return(MSP_OK);
}

/****************************************************************************/
/* NAME:    MSP_GetTransmitSubframeId                                      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    															*/
/* Gets the subrame id of currently being transmitted subframe.				*/
/*																			*/
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/*                                                                          */
/* OUT:                                                                     */
/* None                                                                     */
/*                                                                          */
/* RETURN:                                                                  */
/* Returns currently transmitting subframe                                  */
/* The value returned may be undefined if multichannel mode is not          */
/* 	effective.                                                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  ReEntrant                                                   */
/* REENTRANCY  No Issues                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetTransmitSubframeId(IN t_msp_device_id msp_device_id, OUT t_msp_multichannel_subframe_id *p_msp_multichannel_subframe_id)
{
    t_msp_error     msp_error = MSP_OK;
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    DBGENTER2(" (%x %lx)", msp_device_id, p_msp_multichannel_subframe_id);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL == p_msp_multichannel_subframe_id)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    *p_msp_multichannel_subframe_id = (t_msp_multichannel_subframe_id) ((MSP_READ_BITS(p_msp_register->msp_mcr, TWO_BITS, TNCSF_BIT)) >> TNCSF_BIT);

    DBGEXIT0(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_GetReceiveSubframeId 	                                  	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    															*/
/* Gets the subrame id of currently being received subframe.				*/
/*																			*/
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/*                                                                          */
/* OUT:                                                                     */
/* None                                                                     */
/*                                                                          */
/* RETURN:                                                                  */
/* Returns currently being received subframe								*/
/* The value returned may be undefined if multichannel mode is not effective*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  ReEntrant                                                   */
/* REENTRANCY  No Issues                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetReceiveSubframeId(IN t_msp_device_id msp_device_id, OUT t_msp_multichannel_subframe_id *p_msp_multichannel_subframe_id)
{
    t_msp_error     msp_error = MSP_OK;
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    DBGENTER2(" (%x %lx)", msp_device_id, p_msp_multichannel_subframe_id);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL == p_msp_multichannel_subframe_id)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    *p_msp_multichannel_subframe_id = (t_msp_multichannel_subframe_id) ((MSP_READ_BITS(p_msp_register->msp_mcr, TWO_BITS, RMCSF_BIT)) >> RMCSF_BIT);

    DBGEXIT0(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_GetTransmissionStatus                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Returns the status whether interrupt mode transmission is*/
/*                over.		                                                */
/*                                                                          */
/* PARAMETERS:                                                              */
/*   IN:																	*/
/*   msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 	*/
/*   OUT:                                                                   */
/*   p_underrun_error_count:                                                */
/*	   location of variable to store under-run error count.                 */
/*   p_bytes_transmitted:                                                   */
/*	   location of variable to store number of bytes transmitted.           */
/*   p_is_transmition_completed:                                            */
/*		   TRUE if transfer completed, FALSE otherwise.                     */
/*                                                                          */
/*   RETURN:                                                                */
/*   MSP_INVALID_PARAMETER: 	returned if all pointers are NULL           */
/*							or if msp_device_id is invalid.                 */
/*   MSP_OK: if successful.                                                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:Re-entrant                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetTransmissionStatus
(
    IN t_msp_device_id  msp_device_id,
    OUT t_uint32        *p_underrun_error_count,
    OUT t_uint32        *p_bytes_pending_to_transmitted,
    OUT t_bool          *p_is_transmition_completed
)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    DBGENTER4(" (%x %lx %lx %lx)", msp_device_id, p_underrun_error_count, p_bytes_pending_to_transmitted, p_is_transmition_completed);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL == p_underrun_error_count && NULL == p_bytes_pending_to_transmitted && NULL == p_is_transmition_completed)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL != p_bytes_pending_to_transmitted)
    {
        if (MSP_MODE_IT == p_msp_system_context->msp_device_context.general_config.tx_msp_mode)
        {
            *p_bytes_pending_to_transmitted = p_msp_system_context->tx_status.it_bytes_nb;
        }
        else
        {
            *p_bytes_pending_to_transmitted = 0;
        }
    }

    if (NULL != p_underrun_error_count)
    {
        *p_underrun_error_count = p_msp_system_context->tx_status.flow_error_nb;
    }

    if (NULL != p_is_transmition_completed)
    {
        if (MSP_MODE_IT == p_msp_system_context->msp_device_context.general_config.tx_msp_mode)
        {
            if (1 == p_msp_system_context->flag_for_transmit_interrupt_mode)
            {
                *p_is_transmition_completed = TRUE;
            }
            else
            {
                *p_is_transmition_completed = FALSE;
            }
        }
        else
        {
            *p_is_transmition_completed = TRUE;
        }
    }

    msp_error = MSP_OK;
    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_GetReceptionStatus                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Returns the status whether interrupt mode reception is	*/
/*                over    .                                                 */
/*                                                                          */
/* PARAMETERS:                                                              */
/*   IN:																	*/
/*   msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 	*/
/*   OUT:                                                                   */
/*   p_overrun_error_count:                                                 */
/*	   location of variable to store over-run error count.                  */
/*   p_bytes_transmitted:                                                   */
/*	   location of variable to store number of bytes received.              */
/*   p_is_transmition_completed:                                            */
/*		   TRUE if transfer completed, FALSE otherwise.                     */
/*                                                                          */
/*   RETURN:                                                                */
/*   MSP_INVALID_PARAMETER: 	returned if all pointers are NULL           */
/*							or if msp_device_id is invalid.                 */
/*   MSP_OK: if successful.                                                 */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:Re-entrant                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetReceptionStatus(IN t_msp_device_id msp_device_id, OUT t_uint32 *p_overrun_error_count, OUT t_uint32 *p_bytes_pending_to_be_received, OUT t_bool *p_is_reception_completed)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    DBGENTER4(" (%x %lx %lx %lx)", msp_device_id, p_overrun_error_count, p_bytes_pending_to_be_received, p_is_reception_completed);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL == p_overrun_error_count && NULL == p_bytes_pending_to_be_received && NULL == p_is_reception_completed)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL != p_bytes_pending_to_be_received)
    {
        if (MSP_MODE_IT == p_msp_system_context->msp_device_context.general_config.rx_msp_mode)
        {
            *p_bytes_pending_to_be_received = p_msp_system_context->rx_status.it_bytes_nb;
        }
        else
        {
            *p_bytes_pending_to_be_received = 0;
        }
    }

    if (NULL != p_overrun_error_count)
    {
        *p_overrun_error_count = p_msp_system_context->rx_status.flow_error_nb;
    }

    if (MSP_MODE_IT == p_msp_system_context->msp_device_context.general_config.rx_msp_mode)
    {
        if (0 == p_msp_system_context->flag_for_receive_interrupt_mode)
        {
            *p_is_reception_completed = TRUE;
        }
        else
        {
            *p_is_reception_completed = FALSE;
        }
    }
    else
    {
        *p_is_reception_completed = TRUE;
    }

    msp_error = MSP_OK;
    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_GetRxFIFOStatus 	                                       	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    															*/
/* This routine returns the status of Rx FIFO & Rx busy Flag.				*/
/*																			*/
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2       */
/*                                                                          */
/* OUT:                                                                     */
/* p_msp_fifo_status: status of FIFO                                        */
/* p_rx_busy_flag: status of Rx busy flag                                   */
/*                                                                          */
/* RETURN:                                                                  */
/* MSP_INVALID_PARAMETER: returned if all pointers are NULL or              */
/* 	if msp_device_id is invalid.                                            */
/* MSP_OK: if successful.                                                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  ReEntrant                                                   */
/* REENTRANCY  No Issues                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetRxFIFOStatus(IN t_msp_device_id msp_device_id, OUT t_msp_fifo_status *p_msp_fifo_status, OUT t_bool *p_rx_busy_flag)
{
    t_msp_error     msp_error = MSP_OK;
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    DBGENTER3(" (%x %lx %lx)", msp_device_id, p_msp_fifo_status, p_rx_busy_flag);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL == p_msp_fifo_status && NULL == p_rx_busy_flag)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL != p_msp_fifo_status)
    {
        if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, RFE_BIT)) >> RFE_BIT)))
        {
            *p_msp_fifo_status = MSP_FIFO_EMPTY;
        }
        else if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, RFU_BIT)) >> RFU_BIT)))
        {
            *p_msp_fifo_status = MSP_FIFO_FULL;
        }
        else
        {
            *p_msp_fifo_status = MSP_FIFO_PARTIALLY_FILLED;
        }
    }

    if (NULL != p_rx_busy_flag)
    {
        if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, RBUSY_BIT)) >> RBUSY_BIT)))
        {
            *p_rx_busy_flag = TRUE;
        }
        else
        {
            *p_rx_busy_flag = FALSE;
        }
    }

    DBGEXIT0(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_GetTxFIFOStatus 	                                       	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    															*/
/* This routine returns the status of Tx FIFO & Tx busy Flag.				*/
/*																			*/
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2       */
/*                                                                          */
/* OUT:                                                                     */
/* p_msp_fifo_status: status of FIFO                                        */
/* p_tx_busy_flag: status of Tx busy flag                                   */
/*                                                                          */
/* RETURN:                                                                  */
/* MSP_INVALID_PARAMETER: returned if all pointers are NULL or              */
/* 	if msp_device_id is invalid.                                            */
/* MSP_OK: if successful.                                                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  ReEntrant                                                   */
/* REENTRANCY  No Issues                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetTxFIFOStatus(IN t_msp_device_id msp_device_id, OUT t_msp_fifo_status *p_msp_fifo_status, OUT t_bool *p_tx_busy_flag)
{
    t_msp_error     msp_error = MSP_OK;
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    DBGENTER3(" (%x %lx %lx)", msp_device_id, p_msp_fifo_status, p_tx_busy_flag);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL == p_msp_fifo_status && NULL == p_tx_busy_flag)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (NULL != p_msp_fifo_status)
    {
        if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, TFE_BIT)) >> TFE_BIT)))
        {
            *p_msp_fifo_status = MSP_FIFO_EMPTY;
        }
        else if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, TFU_BIT)) >> TFU_BIT)))
        {
            *p_msp_fifo_status = MSP_FIFO_FULL;
        }
        else
        {
            *p_msp_fifo_status = MSP_FIFO_PARTIALLY_FILLED;
        }
    }

    if (NULL != p_tx_busy_flag)
    {
        if (TRUE == ((t_bool) ((MSP_READ_BITS(p_msp_register->msp_flr, ONE_BIT, TBUSY_BIT)) >> TBUSY_BIT)))
        {
            *p_tx_busy_flag = TRUE;
        }
        else
        {
            *p_tx_busy_flag = FALSE;
        }
    }

    DBGEXIT0(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_TransmitData                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:Copies from the address given in parameter several elements  */
/*                to be transmitted to the MSP.                             */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         msp_device_id:        Device Id for Identification		    */
/*    data_origin_address:    Address from where transmitted data is copied.*/
/*    bytes_to_be_transfered:        Number of bytes to be transmitted.     */
/*    						the number is rounded to highest possible value */
/*				below the supplied value in multiple of tx transfer width	*/
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:                                                                  */
/*    MSP_NOT_CONFIGURED:                                                 	*/
/*                      if the transmit part of the MSP was not             */
/*                    enabled before.                                       */
/*    MSP_NON_AUTHORIZED_MODE:                                              */
/*                      if the MSP's transmit part is configured in         */
/*                    DMA mode.                                             */
/*    MSP_INVALID_PARAMETER:			    if msp_device_id is invalid		*/
/*    MSP_OK  :       If everything goes fine.                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                */
/* REENTRANCY ISSUES:Modifying                                              */
/* p_msp_system_context->tx_status.p_it_data_flow(buffer addr for mono data)*/
/* p_msp_system_context->tx_status.it_bytes_nb(No of bytes to be transmitted*/

/****************************************************************************/
PUBLIC t_msp_error MSP_TransmitData(IN t_msp_device_id msp_device_id, IN t_logical_address data_origin_address, IN t_uint32 bytes_to_be_transfered)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;
    t_uint32 loop_counter = 0;

    /*t_msp_register* p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;*/
    DBGENTER3(" (%x %lx %lx)", msp_device_id, data_origin_address, bytes_to_be_transfered);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        return(msp_error);
    }
    
    /* rounding bytes_to_be_transfered to highest possible value that is below supplied bytes_to_be_transfered and multiple of transfer width of Tx*/
    bytes_to_be_transfered = (bytes_to_be_transfered / msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width)) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width);

    switch (p_msp_system_context->msp_device_context.general_config.tx_msp_mode)
    {
        case (MSP_MODE_DMA):
            {
                /* In DMA mode this function should not be called
			-------------------------------------------------*/
                DBGPRINT(DBGL_ERROR, " Function not authorized in DMA mode");
                msp_error = MSP_NON_AUTHORIZED_MODE;
                DBGEXIT(msp_error);
                break;
            }

        case (MSP_MODE_IT):
            {               
                /*t_msp_system_context* p_msp_system_context = g_msp_system_context + msp_device_id;*/
                t_uint8     nb_of_bytes_to_transfer = (t_uint8)
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
					(msp_GetTxFifoWmrkLevel(msp_device_id) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width));
#else
                    (MSP_FIFO_WATERMARK_LEVEL * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width));                
#endif
                p_msp_system_context->flag_for_transmit_interrupt_mode = 0;

                p_msp_system_context->tx_status.p_it_data_flow = (t_uint8 *) data_origin_address;

                p_msp_system_context->tx_status.it_bytes_nb = bytes_to_be_transfered;               

                if (0 == p_msp_system_context->tx_status.it_bytes_nb)
                {
                    /*p_msp_system_context->flag_for_transmit_interrupt_mode = 1;*/
                    p_msp_system_context->tx_status.it_bytes_nb = 0;
                }

                /*else*/
                {
                    if (MSP_FIFO_ENABLE == p_msp_system_context->msp_device_context.general_config.tx_fifo_config)
                    {
                        if (p_msp_system_context->tx_status.it_bytes_nb >= nb_of_bytes_to_transfer)
                        {
                            msp_LocalEnableIrqSource(((t_uint32) MSP0_IRQ_SRC_TX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
                        }
                        else
                        {
    
                            msp_LocalEnableIrqSource(((t_uint32) MSP0_IRQ_SRC_TX_FIFO_NOT_FULL) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
                        }
                    }
                    else
                    {
                        msp_LocalEnableIrqSource(((t_uint32) MSP0_IRQ_SRC_TX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
                    }
                }            
                break;
            }

        case (MSP_MODE_POLLING):
            {
                if (FALSE == p_msp_device_context->tx_enable)
                {
                    DBGPRINT(DBGL_ERROR, "Transmit part not configured");
                    msp_error = MSP_NOT_CONFIGURED;
                    DBGEXIT(msp_error);
                    break;
                }

                do
                {
                    t_uint32    no_byte_transfer;
                    no_byte_transfer = bytes_to_be_transfered;
                    msp_error = MSP_WriteBuffer(msp_device_id, (t_uint8 *) data_origin_address, &no_byte_transfer);
                    bytes_to_be_transfered -= no_byte_transfer;
                    data_origin_address += no_byte_transfer;
                    loop_counter++;
                } while ((bytes_to_be_transfered != 0) && (loop_counter < MSP_ENDAD_COUNTER));
                break;
            }

        default:
            {
                DBGPRINT(DBGL_ERROR, " Invalid mode !!");
                msp_error = MSP_NON_AUTHORIZED_MODE;
                break;
            }
    }   /*end of switch*/
    
    if (loop_counter >= MSP_ENDAD_COUNTER)
    {
        msp_error = MSP_ERROR;
    }

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_ReceiveData                                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Copies to the address given in parameter some elements   */
/*                received from the MSP.                                    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         msp_device_id:        Device Id for Identification		    */
/*            data_dest_address:    Address where received data are copied. */
/*            bytes_to_be_retrieved:    Number of bytes to be received.     */
/*    						the number is rounded to highest possible value */
/*				below the supplied value in multiple of tx transfer width	*/
/* OUT:        none                                                         */
/*                                                                          */
/* RETURN:                                                                  */
/*        MSP_NOT_CONFIGURED:                                         	    */
/*         if the receive part of the MSP has not been                      */
/*          enabled before.                                                 */
/*        MSP_NON_AUTHORIZED_MODE:                                          */
/*          if the MSP's receive part is configured in                      */
/*          DMA mode.                                                       */
/*        MSP_INVALID_PARAMETER: if msp_device_id is invalid				*/
/*        MSP_OK:    if everything goes fine.							    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                */
/* REENTRANCY ISSUES:Modifying                                              */
/* p_msp_system_context->rx_status.p_it_data_flow(buffer addr for mono data)*/
/* p_msp_system_context->rx_status.it_bytes_nb(No of bytes to be received)  */

/****************************************************************************/
PUBLIC t_msp_error MSP_ReceiveData(IN t_msp_device_id msp_device_id, IN t_logical_address data_dest_address, IN t_uint32 bytes_to_be_retrieved)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;
    t_uint32 loop_counter = 0;

    /*t_msp_register* p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;*/
    DBGENTER3(" (%x %lx %lx)", msp_device_id, data_dest_address, bytes_to_be_retrieved);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        return(msp_error);
    }
    
    /* rounding bytes_to_be_retrieved to highest possible value that is below supplied bytes_to_be_retrieved and multiple of transfer width of Rx*/
    bytes_to_be_retrieved = (bytes_to_be_retrieved / msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width)) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width);

    switch (p_msp_system_context->msp_device_context.general_config.rx_msp_mode)
    {
        case (MSP_MODE_DMA):
            {
                /* Nothing to be done. Everything is performed by hardware.
			 ---------------------------------------------------------*/
                DBGPRINT(DBGL_ERROR, " Function not authorized in DMA mode");
                msp_error = MSP_NON_AUTHORIZED_MODE;
                DBGEXIT(msp_error);
                break;
            }

        case (MSP_MODE_IT):
            {            
                t_uint8 nb_of_bytes_to_transfer = (t_uint8)
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
					(msp_GetRxFifoWmrkLevel(msp_device_id) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width));
#else
                    (MSP_FIFO_WATERMARK_LEVEL * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width));
#endif
                p_msp_system_context->flag_for_receive_interrupt_mode = 1;

                p_msp_system_context->rx_status.p_it_data_flow = (t_uint8 *) data_dest_address;

                p_msp_system_context->rx_status.it_bytes_nb = bytes_to_be_retrieved;

                if (0 == bytes_to_be_retrieved)
                {
                    p_msp_system_context->flag_for_receive_interrupt_mode = 0;
                    p_msp_system_context->rx_status.it_bytes_nb = 0;
                }
                else
                {
                    if (bytes_to_be_retrieved >= nb_of_bytes_to_transfer)
                    {
                        msp_LocalEnableIrqSource(((t_uint32) MSP0_IRQ_SRC_RX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
                    }
                    else
                    {
                        if (MSP_FIFO_ENABLE == p_msp_system_context->msp_device_context.general_config.rx_fifo_config)
                        {
                            msp_LocalEnableIrqSource(((t_uint32) MSP0_IRQ_SRC_RX_FIFO_NOT_EMPTY) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
                        }
                        else
                        {
                            msp_LocalEnableIrqSource(((t_uint32) MSP0_IRQ_SRC_RX_SERVICE) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE));
                        }
                    }
                }            
                break;
            }

        case (MSP_MODE_POLLING):
            {
                if (FALSE == p_msp_device_context->rx_enable)
                {
                    DBGPRINT(DBGL_ERROR, "Receive part not configured");
                    msp_error = MSP_NOT_CONFIGURED;
                    DBGEXIT(msp_error);
                    break;
                }

                do
                {
                    t_uint32    no_byte_transfer;
                    no_byte_transfer = bytes_to_be_retrieved;
                    msp_error = MSP_ReadBuffer(msp_device_id, (t_uint8 *) data_dest_address, &no_byte_transfer);
                    bytes_to_be_retrieved -= no_byte_transfer;
                    data_dest_address += no_byte_transfer;
                    loop_counter++;
                } while ((bytes_to_be_retrieved != 0) && (loop_counter < MSP_ENDAD_COUNTER));
                break;
            }

        default:
            {
                /* by default DMA mode is assumed
			---------------------------------*/
                DBGPRINT(DBGL_ERROR, " Invalid mode !!");
                msp_error = MSP_NON_AUTHORIZED_MODE;
                DBGEXIT(msp_error);
                break;
            }
    }
    
    if (loop_counter >= MSP_ENDAD_COUNTER)
    {
        msp_error = MSP_ERROR;
    }

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_WriteBuffer                                            		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:																*/
/* Transfer data to Tx FIFO and return immediately when FIFO is full.       */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/* p_data_src_address: pointer to data buffer to be transmitted.            */
/*                                                                          */
/* OUT:                                                                     */
/* None                                                                     */
/*                                                                          */
/* INOUT:                                                                   */
/*                                                                          */
/* p_bytes_count: pointer to location which contains requested number of    */
/* bytes to be transmitted through MSP. This is an in-out type of parameter.*/
/* After call to this API, the location pointed by the p_byte_count is      */
/* filled by  total number of bytes actually                                */
/* transmitted (written to MSP FIFO).                                       */
/*                                                                          */
/* RETURN:                                                                  */
/* MSP_INVALID_PARAMETER: if p_data_src_address is NULL. 					*/
/* Or if msp_device_id is not valid.										*/
/* MSP_OK: if successful.													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_msp_error MSP_WriteBuffer(IN t_msp_device_id msp_device_id, IN t_uint8 *p_data_src_address, INOUT t_uint32 *p_bytes_count)
{
    t_msp_error             msp_error = MSP_OK;
    t_uint8                 transfer_width_size;
    t_sint32                total_no_of_bytes = 0;    
    t_uint8                 *p_data_src;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;    
    

    DBGENTER3(" (%x %lx %lx)", msp_device_id, p_data_src_address, p_bytes_count);
/*
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL == p_data_src_address)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    total_no_of_bytes = *p_bytes_count;

    if (total_no_of_bytes < 0)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }
*/  

    total_no_of_bytes = *p_bytes_count;

    if (total_no_of_bytes < 0)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    transfer_width_size = msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width);

    p_data_src = (t_uint8 *) p_data_src_address;

    while (1)
    {
        total_no_of_bytes -= transfer_width_size;
      
        if (total_no_of_bytes < 0 || MSP_FIFO_FULL == msp_GetTxFifoStatus(msp_device_id))
        {
            total_no_of_bytes += transfer_width_size;
            *p_bytes_count = *p_bytes_count - total_no_of_bytes;
            break;
        }
  
        switch (p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width)
        {
            case MSP_DATA_TRANSFER_WIDTH_BYTE:
                p_msp_register->msp_dr = *((t_uint8 *) p_data_src);
                break;

            case MSP_DATA_TRANSFER_WIDTH_HALFWORD:
                p_msp_register->msp_dr = *((t_uint16 *) p_data_src);
                break;

            case MSP_DATA_TRANSFER_WIDTH_WORD:
                p_msp_register->msp_dr = *((t_uint32 *) p_data_src);
                break;

            default:
                return(MSP_INVALID_PARAMETER);
        };

        p_data_src += transfer_width_size;  /* Advance to one element */
    }   /* end of while*/
	

    msp_error = MSP_OK;	

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_ReadBuffer                                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:																*/
/* Transfer data from Rx FIFO and return immediately when FIFO is Empty.    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:																		*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2       */
/*                                                                          */
/* p_data_dest_address: Address of data buffer in which data is             */
/* to be received.                                                          */
/* OUT:                                                                     */
/* None                                                                     */
/*                                                                          */
/* INOUT:                                                                   */
/* p_bytes_count: pointer to location which contains requested number       */
/* of bytes to be received from MSP. This is an in-out type of parameter.   */
/* After call to this API, the location pointed by the p_byte_count is      */
/* filled by total number of bytes actually received (read from MSP FIFO).  */
/*                                                                          */
/* RETURN:                                                                  */
/* MSP_INVALID_PARAMETER, if p_data_dest_address is NULL. 					*/
/* Or if msp_device_id is not valid.                                        */
/* MSP_OK: if successful.                                                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_msp_error MSP_ReadBuffer(IN t_msp_device_id msp_device_id, IN t_uint8 *p_data_dest_address, INOUT t_uint32 *p_bytes_count)
{
    t_msp_error             msp_error = MSP_OK;
    t_uint8                 transfer_width_size;
    t_sint32                total_no_of_bytes = 0;    
    t_uint8                 *p_data_dest;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;   
    
    DBGENTER3(" (%x %lx %lx)", msp_device_id, p_data_dest_address, p_bytes_count);

/*
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (NULL == p_data_dest_address)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    total_no_of_bytes = *p_bytes_count;

    if (total_no_of_bytes < 0)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }
*/

    total_no_of_bytes = *p_bytes_count;

    if (total_no_of_bytes < 0)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    transfer_width_size = msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width);

    p_data_dest = (t_uint8 *) p_data_dest_address;

    while (1)
    {
        total_no_of_bytes -= transfer_width_size;
        if (total_no_of_bytes < 0 || MSP_FIFO_EMPTY == msp_GetRxFifoStatus(msp_device_id))
        {
            total_no_of_bytes += transfer_width_size;
            *p_bytes_count = *p_bytes_count - total_no_of_bytes;
            break;
        }

        switch (p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width)
        {
            case MSP_DATA_TRANSFER_WIDTH_BYTE:
                *((t_uint8 *) p_data_dest) = (t_uint8) p_msp_register->msp_dr;
                break;

            case MSP_DATA_TRANSFER_WIDTH_HALFWORD:
                *((t_uint16 *) p_data_dest) = (t_uint16) p_msp_register->msp_dr;
                break;

            case MSP_DATA_TRANSFER_WIDTH_WORD:
                *((t_uint32 *) p_data_dest) = (t_uint32) p_msp_register->msp_dr;
                break;

            default:
                return(MSP_INVALID_PARAMETER);
        };

        p_data_dest += transfer_width_size; /* Advance to one element */
    }   /* end of while*/

    msp_error = MSP_OK;
    
    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:        MSP_GetIRQSrcStatus                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Updates the interrupt status variable.                   */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:            t_msp_irq_src: Id of the interrupt for which the          */
/*                p_status_structure is to be updated.                      */
/*                														    */
/* OUT: 	p_status_structure: Interrupt status structure                  */
/*                                                                          */
/* REMARK:                                                                  */
/*         This function do nothing and immediately returns if Invalid 		*/
/*			device id is found. 											*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC void MSP_GetIRQSrcStatus(IN t_msp_irq_src msp_irq_src, OUT t_msp_irq_status *p_status_structure)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) (msp_irq_src >> MSP_SHIFT_BY_TWELVE);
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    DBGENTER4(" (%x %lx %x %lx)", msp_device_id, msp_irq_src, p_status_structure->interrupt_state, p_status_structure->pending_irq);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        return;
    }

    if (((t_uint32) MSP0_IRQ_SRC_ALL) == (msp_irq_src & MSP_MASK_INTERRUPT_BITS))
    {
        p_status_structure->pending_irq = (p_msp_register->msp_mis) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        p_status_structure->initial_irq = p_status_structure->pending_irq;
    }
    else
    {
        p_status_structure->pending_irq = ((p_msp_register->msp_mis) & (msp_irq_src & MSP_MASK_INTERRUPT_BITS)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        p_status_structure->initial_irq = p_status_structure->pending_irq;
    }

    p_status_structure->interrupt_state = MSP_IRQ_STATE_NEW;
    DBGEXIT(MSP_OK);
}

/****************************************************************************/
/* NAME:        MSP_FilterProcessIRQSrc                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:this iterative routine allows to process the current xxx     */
/*            device interrupt sources identified through the               */
/*            p_status_structure .it processes the interrupt sources one by */
/*            one,updates the p_status_structure to keep an internal history*/
/*            and generates the events. This function must be called until  */
/*              all processing is not done or all events are not generated. */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :        t_msp_irq_status *p_status_structure  status of the interrupt*/
/* OUT :       t_msp_event   *p_event_id:current event that is handled by 	*/
/*                                the routine                               */
/*             t_msp_filter_mode filter_mode filter to selectively process  */
/*                the interrupt                                             */
/*                                                                          */
/* RETURN:        MSP_INTERNAL_EVENT(positive):It indicates that all the    */
/*            processing associated with p_status_structure has been        */
/*            done all the events are not yet processed  i.e all the        */
/*            events have not been acknowledged).                           */
/*                                                                          */
/*            MSP_NO_MORE_PENDING_EVENT(positive value):Indicate that       */
/*            all processing associated with the pstructure                 */
/*            (without filter option) is done .The interrupt source can be  */
/*                reenabled.                                                */
/*                                                                          */
/*              MSP_NO_MORE_FILTER_PENDING_EVENT(positive value) indicates  */
/*            that all the processing associated with p_status_structure    */
/*              with the filter option is done                              */
/*                                                                          */
/*            MSP_NO_PENDING_EVENT_ERROR :when there is no interrupt        */
/*                to process                                                */
/*            MSP_INVALID_PARAMETER: is returned if invalid device id is 	*/
/*				found. 														*/
/*                                                                          */
/*                                                                          */
/*NOTE         :Befor using this function ,it is necessasry to call         */
/*            MSP_GetIRQSrcStatus or MSP_IsIrqSrcActive functions to        */
/*            intialize p_status_structure structure.                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Re-Entrant                                               */
/* REENTRANCY ISSUES:Modifying global p_msp_system_context->msp_event_status*/
/* variable  																*/

/****************************************************************************/
PUBLIC t_msp_error MSP_FilterProcessIRQSrc(IN t_msp_irq_status *p_status_structure, OUT t_msp_event *p_event_id, IN t_msp_filter_mode filter_mode)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_device_id         msp_device_id = (t_msp_device_id) ((p_status_structure->pending_irq) >> MSP_SHIFT_BY_TWELVE);
 
    volatile t_msp_event    *p_l_event;
    t_msp_filter_mode       l_filter;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;
    DBGENTER2(" (%x %lx)", msp_device_id, filter_mode);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    if (MSP_IRQ_STATE_NEW == p_status_structure->interrupt_state && 0 == (p_status_structure->pending_irq & MSP_MASK_INTERRUPT_BITS))
    {
        DBGEXIT0(MSP_NO_PENDING_EVENT_ERROR);
        return(MSP_NO_PENDING_EVENT_ERROR);
    }

    p_status_structure->interrupt_state = MSP_IRQ_STATE_OLD;
    p_l_event = &p_msp_system_context->msp_event_status;

    /*No filter mode means only one interrupt is to be served
	-------------------------------------------------------*/
    if (MSP_NO_FILTER_MODE == filter_mode)
    {
        l_filter = p_status_structure->pending_irq;
        if (0 == (l_filter & MSP_MASK_INTERRUPT_BITS))  /*if LFilter is not Zero then process interrupt*/
        {
            if (0 == ((*p_l_event) & MSP_MASK_INTERRUPT_BITS))
            {
                DBGEXIT0(MSP_NO_MORE_PENDING_EVENT);
                return(MSP_NO_MORE_PENDING_EVENT);
            }
            else
            {
                DBGEXIT0(MSP_INTERNAL_EVENT);
                return(MSP_INTERNAL_EVENT);
            }
        }
    }
    else
    {
        /*Filter mode
		--------------*/
        l_filter = (p_status_structure->pending_irq & filter_mode) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
        if (0 == (l_filter & MSP_MASK_INTERRUPT_BITS))
        {
            /*No filter event
			-------------------*/
            if (0 == ((*p_l_event) & filter_mode & MSP_MASK_INTERRUPT_BITS))
            {
                if (0 == ((*p_l_event) & MSP_MASK_INTERRUPT_BITS))
                /*No active event
				------------------*/
                {
                    DBGEXIT0(MSP_NO_MORE_PENDING_EVENT);
                    return(MSP_NO_MORE_PENDING_EVENT);
                }
                else
                {
                    DBGEXIT0(MSP_NO_MORE_FILTER_PENDING_EVENT);
                    return(MSP_NO_MORE_FILTER_PENDING_EVENT);
                }
            }
            else
            {
                DBGEXIT0(MSP_INTERNAL_EVENT);
                return(MSP_INTERNAL_EVENT);
            }
        }
    }

    msp_error = msp_FilterProcessIRQSrcProcessIt(p_status_structure, p_event_id, filter_mode, l_filter);
    if(MSP_OK != msp_error)
    {
    	DBGEXIT(msp_error);
        return(msp_error);

    }

    DBGEXIT0(MSP_REMAINING_PENDING_EVENTS);
    return(MSP_REMAINING_PENDING_EVENTS);
}

/****************************************************************************/
/* NAME:        MSP_ProcessIRQSrc                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine allows to process the interrupt sources     */
/*                identified through the p_status_structure structure.      */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:            p_status_structure:                                       */
/* OUT:            None                                                     */
/*                                                                          */
/* RETURN:    MSP_NO_MORE_PENDING_EVENT(positive value):Indicate that all   */
/*        processing associated with the pstructure                         */
/*        (without filter option) is done .The inteupt source can be        */
/*                reenabled.                                                */
/*                                                                          */
/*        MSP_NO_PENDING_EVENT_ERROR :when there is no interrupt            */
/*                to process                                                */
/*            MSP_INVALID_PARAMETER: is returned if invalid device id is 	*/
/* 			found. 															*/
/*                                                                          */
/*NOTE         :Befor using this function ,it is necessasry to call         */
/*            MSP_GetIRQSrcStatus or MSP_IsIrqSrcActive functions to        */
/*            intialize p_status_structure structure.                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Re-Entrant                                               */
/* REENTRANCY ISSUES: p_msp_system_context->rx_status.flow_error_nb and    	*/
/*					  p_msp_system_context->tx_status.flow_error_nb,     	*/
/*                    are  being modified.                                  */

/****************************************************************************/
PUBLIC t_msp_error MSP_ProcessIRQSrc(IN t_msp_irq_status *p_status_structure)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_device_id         msp_device_id = (t_msp_device_id) ((p_status_structure->pending_irq) >> MSP_SHIFT_BY_TWELVE);
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        return(msp_error);
    }

    if (MSP_IRQ_STATE_OLD == p_status_structure->interrupt_state || 0 == (p_status_structure->pending_irq & MSP_MASK_INTERRUPT_BITS))
    {
        msp_error = MSP_NO_PENDING_EVENT_ERROR;
        return(msp_error);
    }

    p_status_structure->interrupt_state = MSP_IRQ_STATE_OLD;
    if ((p_status_structure->pending_irq & (t_uint32) MSP0_IRQ_SRC_RX_SERVICE) != 0 || (p_status_structure->pending_irq & (t_uint32) MSP0_IRQ_SRC_RX_FIFO_NOT_EMPTY) != 0)
    {
        t_uint8     nb_of_bytes_to_transfer = (t_uint8)
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
			(msp_GetRxFifoWmrkLevel(msp_device_id) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width));
#else
            (MSP_FIFO_WATERMARK_LEVEL * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.rx_data_transfer_width));
#endif
        t_uint32    no_byte_transfer = p_msp_system_context->rx_status.it_bytes_nb;

        msp_error = MSP_ReadBuffer(msp_device_id, (t_uint8 *) p_msp_system_context->rx_status.p_it_data_flow, &no_byte_transfer);
        if(MSP_OK != msp_error)
        {
        	return(msp_error);
        }
        p_msp_system_context->rx_status.it_bytes_nb -= no_byte_transfer;
        p_msp_system_context->rx_status.p_it_data_flow += no_byte_transfer;

        if (MSP_FIFO_ENABLE == p_msp_system_context->msp_device_context.general_config.rx_fifo_config)
        {        
            if (!(p_msp_system_context->rx_status.it_bytes_nb >= nb_of_bytes_to_transfer))  /* check if <=4 elements to be received now remaining */
            {
                /* enable Rx FIFO not empty interrupt here to receive elements one by one. */
                MSP_SET_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_FIFO_NOT_EMPTY_INT_BIT);
                MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_SERVICE_INTERRUPT_BIT);
            }        
        }

        if (0 == p_msp_system_context->rx_status.it_bytes_nb)        /* All data transfered*/
        {
            /*masking Rx interrupt after data Xmission*/
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_SERVICE_INTERRUPT_BIT);
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) RECEIVE_FIFO_NOT_EMPTY_INT_BIT);
            p_msp_system_context->flag_for_receive_interrupt_mode = 0;
        }
    }   /*end of MSP_RX_SERVICE_IT*/

    if (p_status_structure->pending_irq & (t_uint32) MSP0_IRQ_SRC_RX_FRAME_SYNC)
    {
        MSP_SET_BIT(p_msp_register->msp_icr, (t_uint32) MSP0_IRQ_SRC_RX_FRAME_SYNC);
    }

    if (((t_uint32) MSP0_IRQ_SRC_TX_SERVICE & p_status_structure->pending_irq) != 0 || ((t_uint32) MSP0_IRQ_SRC_TX_FIFO_NOT_FULL & p_status_structure->pending_irq) != 0)
    {    
        t_uint8     nb_of_bytes_to_transfer = (t_uint8)
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
			(msp_GetTxFifoWmrkLevel(msp_device_id) * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width));
#else
            (MSP_FIFO_WATERMARK_LEVEL * msp_CalculateTransferWidthSize(p_msp_system_context->msp_device_context.protocol_descriptor.tx_data_transfer_width));
#endif
        t_uint32    no_byte_transfer = p_msp_system_context->tx_status.it_bytes_nb;
        
        msp_error = MSP_WriteBuffer(msp_device_id, (t_uint8 *) p_msp_system_context->tx_status.p_it_data_flow, &no_byte_transfer);
        if(MSP_OK != msp_error)
        {
        	return(msp_error);
        }
        p_msp_system_context->tx_status.it_bytes_nb -= no_byte_transfer;
        p_msp_system_context->tx_status.p_it_data_flow += no_byte_transfer;

        if (MSP_FIFO_ENABLE == p_msp_system_context->msp_device_context.general_config.tx_fifo_config)
        {        
            if (!(p_msp_system_context->tx_status.it_bytes_nb >= nb_of_bytes_to_transfer))  /* check if <=4 elements to be received now remaining */
            {
                /* enable Rx FIFO not empty interrupt here to receive elements one by one. */
                MSP_SET_BIT(p_msp_register->msp_imsc, (t_uint32) TRANSMIT_FIFO_NOT_FULL_INT_BIT);
                MSP_CLEAR_BIT(p_msp_register->msp_imsc, TRANSMIT_INTERRUPT_SERVICE_BIT);
            }        
        }

        if (0 == p_msp_system_context->tx_status.it_bytes_nb)
        {
            p_msp_system_context->flag_for_transmit_interrupt_mode = 1;

            /*masking Tx interrupt after data Xmission
			-------------------------------------------*/
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, TRANSMIT_INTERRUPT_SERVICE_BIT);
            MSP_CLEAR_BIT(p_msp_register->msp_imsc, (t_uint32) TRANSMIT_FIFO_NOT_FULL_INT_BIT);
        }
    }   /*end of Tx service it*/
    
    /* Servicing Interrupts */
    msp_ProcessIRQSrcServiceIt(p_status_structure);
    
    DBGEXIT0(MSP_NO_MORE_PENDING_EVENT);
    return(MSP_NO_MORE_PENDING_EVENT);
}

/****************************************************************************/
/* NAME:        MSP_IsIRQSrcActive                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Determines whether the IRQ source in argument is active. */
/*                Updates too the interrupt status variable.                */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:            msp_irq_src:Identification of the event to be checked.    */
/* OUT:            p_status_structure:        Interrupt status              */
/*                                                                          */
/* RETURN:                                                                  */
/*              A boolean representing "Is Irq active?"                     */
/* REMARK:                                                               	*/
/*         This function do nothing and returns FALSE if Invalid 			*/
/*			device id is found.												*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC t_bool MSP_IsIRQSrcActive(IN t_msp_irq_src msp_irq_src, OUT t_msp_irq_status *p_status_structure)
{
    t_msp_device_id msp_device_id = (t_msp_device_id) ((msp_irq_src) >> MSP_SHIFT_BY_TWELVE);
    t_msp_register  *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;

    DBGENTER3(" (%x %lx %lx)", msp_device_id, msp_irq_src, (t_uint32) p_status_structure);

    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        DBGEXIT(FALSE);
        return(FALSE);
    }

    /*if interrupt is pending
	-------------------------*/
    if ((p_msp_register->msp_mis & (MSP_MASK_INTERRUPT_BITS & msp_irq_src)) != 0)
    {
        /*if NULL reason of the interrupt is not evaluated
		--------------------------------------------------*/
        if (p_status_structure != NULL)
        {
            if ((t_uint32) MSP0_IRQ_SRC_ALL == (msp_irq_src & MSP_MASK_INTERRUPT_BITS))
            {
                p_status_structure->pending_irq = p_msp_register->msp_mis | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
                p_status_structure->initial_irq = p_status_structure->pending_irq;
            }
            else
            {
                p_status_structure->pending_irq = (((p_msp_register->msp_mis) & (msp_irq_src & MSP_MASK_INTERRUPT_BITS))) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);
                p_status_structure->initial_irq = p_status_structure->pending_irq;
            }

            p_status_structure->interrupt_state = MSP_IRQ_STATE_NEW;
        }

        DBGEXIT2(MSP_OK, "(%s %lx )", "TRUE", msp_irq_src);
        return(TRUE);
    }
    else
    {
        DBGEXIT2(MSP_OK, "(%s %lx )", "FALSE", msp_irq_src);
        return(FALSE);
    }
}

/****************************************************************************/
/* NAME:        MSP_IsEventActive                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Detects if the event in parameter is active or not.      */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:     p_event_id:    Identification of the event to be cleared    .    */
/* OUT:            None                                                     */
/*                                                                          */
/* RETURN:        A boolean representing     "Is Event active ?"            */
/*																			*/
/* REMARK:                    												*/
/*              in case of invalid device id FALSE is returned			    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                   */
/* REENTRANCY ISSUES:No Issues                                              */

/****************************************************************************/
PUBLIC t_bool MSP_IsEventActive(IN const t_msp_event *p_event_id)
{
    t_msp_device_id         msp_device_id = (t_msp_device_id) ((*p_event_id) >> MSP_SHIFT_BY_TWELVE);
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;

    DBGENTER2(" (%x %lx)", msp_device_id, *p_event_id);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        DBGEXIT(FALSE);
        return(FALSE);
    }

    if (((p_msp_system_context->msp_event_status & ((*p_event_id) & MSP_MASK_INTERRUPT_BITS)) != 0))
    {
        DBGEXIT(TRUE);
        return(TRUE);
    }
    else
    {
        DBGEXIT(FALSE);
        return(FALSE);
    }
}

/****************************************************************************/
/* NAME:        MSP_AcknowledgeEvent                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Acknowledges the event given in parameter.               */
/*            Clears the software variable that records the event state.    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:    p_event_id:        Identification of the event to be cleared.     */
/* OUT:            None                                                     */
/*                                                                          */
/* RETURN:        None                                                      */
/*                                                                          */
/* REMARK:                                                                  */
/*         This function do nothing and immediately returns if Invalid 		*/
/*			device id is found. 											*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                */
/* REENTRANCY ISSUES:			                                            */
/*     p_msp_system_context->msp_event_status global event status is being 	*/
/*		modified    														*/

/****************************************************************************/
PUBLIC void MSP_AcknowledgeEvent(IN const t_msp_event *p_event_id)
{
    t_msp_device_id         msp_device_id = (t_msp_device_id) ((*p_event_id) >> MSP_SHIFT_BY_TWELVE);
    t_msp_system_context    *p_msp_system_context = g_msp_system_context + (t_uint32) msp_device_id;

    DBGENTER2(" (%x %lx)", msp_device_id, p_event_id);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        DBGEXIT(MSP_INVALID_PARAMETER);
        return;
    }

    p_msp_system_context->msp_event_status = (p_msp_system_context->msp_event_status &~((*p_event_id) & MSP_MASK_INTERRUPT_BITS)) | (((t_uint32) msp_device_id) << MSP_SHIFT_BY_TWELVE);

    DBGEXIT0(MSP_OK);
}

/****************************************************************************/
/* NAME:    MSP_Reset                                               		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:  															*/
/* Flushes Rx and Tx FIFOs and reset MSP registers & Global Variables.		*/
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :       																*/
/* msp_device_id: MSP_DEVICE_ID_0, MSP_DEVICE_ID_1 or MSP_DEVICE_ID_2 		*/
/*                                                                          */
/* OUT:																		*/
/* None                                                             		*/
/*                                                                          */
/* RETURN:																	*/
/*  MSP_INVALID_PARAMETER: if msp_device_id is invalid						*/
/*	MSP_OK: if successfull													*/
/* NOTE:Call to this function will lead to resetting whole registers        */
/*    to there default value 0x00000000.This function sholud be called      */
/*            before MSP_Configure since it clears all settings             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  Re-Entrant                                                  */
/* REENTRANCY ISSUES:    No Issues                                          */

/****************************************************************************/
PUBLIC t_msp_error MSP_Reset(IN t_msp_device_id msp_device_id)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;
    t_msp_system_context    *p_msp_system_context = &g_msp_system_context[msp_device_id];
    DBGENTER1(" (%x)", msp_device_id);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    p_msp_register->msp_gcr = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_tcf = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_rcf = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_srg = MSP_REGISTER_RESET_VALUE;

    p_msp_register->msp_mcr = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_rcv = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_rcm = MSP_REGISTER_RESET_VALUE;

    p_msp_register->msp_tce0 = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_tce1 = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_tce2 = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_tce3 = MSP_REGISTER_RESET_VALUE;

    p_msp_register->msp_rce0 = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_rce1 = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_rce2 = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_rce3 = MSP_REGISTER_RESET_VALUE;

    p_msp_register->msp_icr |= MSP_CLEAR_ALL_INTERRUPT;
    p_msp_register->msp_dmacr = MSP_REGISTER_RESET_VALUE;
    p_msp_register->msp_imsc = MSP_REGISTER_RESET_VALUE;

    msp_error = MSP_EmptyTxFIFO(msp_device_id);
    if (MSP_OK != msp_error)
    {
        DBGEXIT(msp_error);
        return(msp_error);
    }

    msp_error = MSP_EmptyRxFIFO(msp_device_id);
    if (MSP_OK != msp_error)
    {
        DBGEXIT(msp_error);
        return(msp_error);
    }

    p_msp_device_context->general_config.srg_clock_sel = MSP_SRG_IN_CLOCK_APB;
    p_msp_device_context->general_config.sck_pol = MSP_SCK_POL_AT_RISING_EDGE;
    p_msp_device_context->general_config.msp_loopback_mode = MSP_LOOPBACK_MODE_DISABLE;
    p_msp_device_context->general_config.msp_direct_companding_mode = MSP_DIRECT_COMPANDING_MODE_DISABLE;
    p_msp_device_context->general_config.rx_clock_sel = MSP_CLOCK_SEL_EXT;
    p_msp_device_context->general_config.tx_clock_sel = MSP_CLOCK_SEL_EXT;
    p_msp_device_context->general_config.rx_msp_mode = MSP_MODE_POLLING;
    p_msp_device_context->general_config.tx_msp_mode = MSP_MODE_POLLING;
    p_msp_device_context->general_config.rx_frame_sync_sel = MSP_RX_FRAME_SYNC_EXT;
    p_msp_device_context->general_config.tx_frame_sync_sel = MSP_TX_FRAME_SYNC_EXT;
    p_msp_device_context->general_config.rx_unexpect_frame_sync = MSP_UNEXPEC_FRAME_SYNC_IGNORED;
    p_msp_device_context->general_config.tx_unexpect_frame_sync = MSP_UNEXPEC_FRAME_SYNC_IGNORED;
    p_msp_device_context->general_config.rx_fifo_config = MSP_FIFO_ENABLE;
    p_msp_device_context->general_config.tx_fifo_config = MSP_FIFO_ENABLE;

    p_msp_device_context->serial_clock_divisor = 1;

    p_msp_system_context->tx_status.flow_error_nb = 0;
    p_msp_system_context->rx_status.flow_error_nb = 0;

    p_msp_device_context->msp_general_configuration_done = FALSE;
    p_msp_device_context->msp_protocol_descriptor_set = FALSE;
    p_msp_device_context->serial_clock_divisor_set = FALSE;
    p_msp_device_context->multichannel_configured = FALSE;
    p_msp_device_context->tx_enable = FALSE;
    p_msp_device_context->rx_enable = FALSE;

    dummyForWait = p_msp_register->msp_dr;

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_EmptyRxFIFO                                               	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Flushes Rx FIFOs     		                            */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :       msp_device_id:        Device Id for Identification		    */
/*                                                                          */
/* OUT:    NONE                                                             */
/*                                                                          */
/* RETURN:                                                                  */
/*  MSP_INVALID_PARAMETER:    if msp_device_id is invalid					*/
/*	MS_OK: if successfull													*/
/* NOTE:Call to this function will lead to resetting Rx part. 				*/
/*										This function should be called      */
/*            before MSP_Configure since it clears all settings             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  Re-Entrant                                                  */
/* REENTRANCY ISSUES:    No Issues                                          */

/****************************************************************************/
PUBLIC t_msp_error MSP_EmptyRxFIFO(IN t_msp_device_id msp_device_id)
{
    t_uint32                count = 0;
    t_msp_error             msp_error = MSP_OK;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;
    DBGENTER1(" (%x)", msp_device_id);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) TRUE, ONE_BIT, RFFEN_BIT);

    while ((0 == (p_msp_register->msp_flr & MSP_RFE_FLAG_BIT)) && count < MSP_FIFO_DEPTH * 2)
    {
        /*ensure dummy read from test data register to empty the fifo*/
        dummyForWait = p_msp_register->msp_dr;
        count++;
    }

    MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_device_context->general_config.rx_fifo_config, ONE_BIT, RFFEN_BIT);

    DBGEXIT(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:    MSP_EmptyTxFIFO                                               	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Flushes Tx FIFOs     		                            */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :       msp_device_id:        Device Id for Identification		    */
/*                                                                          */
/* OUT:    NONE                                                             */
/*                                                                          */
/* RETURN:    Always MSP_OK                                                 */
/*  MSP_INVALID_PARAMETER:    if msp_device_id is invalid					*/
/*	MS_OK: if successfull													*/
/* NOTE:Call to this function will lead to resetting Tx part. 				*/
/*										This function should be called      */
/*            before MSP_Configure since it clears all settings             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:  Re-Entrant                                                  */
/* REENTRANCY ISSUES:    No Issues                                          */

/****************************************************************************/
PUBLIC t_msp_error MSP_EmptyTxFIFO(IN t_msp_device_id msp_device_id)
{
    t_uint32                count = 0;
    t_msp_error             msp_error = MSP_OK;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;

    /*t_msp_general_config 	*p_msp_general_config	= &p_msp_device_context->general_config;*/
    DBGENTER1(" (%x)", msp_device_id);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT(msp_error);
        return(msp_error);
    }

    MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) TRUE, ONE_BIT, TFFEN_BIT);

    p_msp_register->msp_tstcr = MSP_INTEGRATION_TEST_MODE;

    /*MSP_WRITE_BITS( p_msp_register->msp_gcr, (t_uint32)TRUE, ONE_BIT, DCM_BIT );*/
    while ((0 == (p_msp_register->msp_flr & MSP_TFE_FLAG_BIT)) && count < MSP_FIFO_DEPTH * 2)
    {
        /*ensure dummy read from test data register to empty the fifo*/
        dummyForWait = p_msp_register->msp_tstdr;
        count++;
    }

    MSP_WRITE_BITS(p_msp_register->msp_gcr, (t_uint32) p_msp_device_context->general_config.tx_fifo_config, ONE_BIT, TFFEN_BIT);

    /*MSP_WRITE_BITS( p_msp_register->msp_gcr, (t_uint32)p_msp_general_config->msp_direct_companding_mode, ONE_BIT, DCM_BIT );*/
    p_msp_register->msp_tstcr = MSP_REGISTER_RESET_VALUE;

    DBGEXIT(MSP_OK);
    return(MSP_OK);
}

/****************************************************************************/
/* NAME:    MSP_SaveDeviceContext                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Saves current device context.                            */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:        msp_device_id:        Device Id for Identification		    */
/* OUT:   None                                                              */
/*                                                                          */
/* RETURN:        		                                                    */
/*              MSP_INVALID_PARAMETER    if msp_device_id is invalid		*/
/*              MSP_OK otherwise                                            */
/* REENTRANCY: Non ReEntrant                                                */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY  Issues structure MspConfig[msp_device_id] and global array 	*/
/*             MspContext[msp_device_id] are being updated.   	            */

/****************************************************************************/
PUBLIC t_msp_error MSP_SaveDeviceContext(IN t_msp_device_id msp_device_id)
{
    t_msp_device_context    *p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context;
    t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
    DBGENTER1(" (%x)", msp_device_id);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        DBGEXIT(MSP_INVALID_PARAMETER);
        return(MSP_INVALID_PARAMETER);
    }

    /*saving interrupt mask status register*/
    p_msp_device_context->imsc_reg = p_msp_register->msp_imsc;

    g_msp_system_context[msp_device_id].msp_device_context_main = g_msp_system_context[msp_device_id].msp_device_context;

    DBGEXIT0(MSP_OK);
    return(MSP_OK);
}

/****************************************************************************/
/* NAME:    MSP_RestoreDeviceContext                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Restores the device context.                             */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:        MspId:        Device Id for Identification		            */
/* OUT:   None                                                              */
/*                                                                          */
/* RETURN:        		                                                    */
/*              MSP_BAD_DEVICE_ID    if MspId is invalid				    */
/*              MSP_OK otherwise                                            */
/*                                                                          */
/* REENTRANCY:  ReEntrant                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY  No Issues                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_RestoreDeviceContext(IN t_msp_device_id msp_device_id)
{
    t_msp_error             msp_error = MSP_OK;
    t_msp_direction         msp_direction = MSP_DIRECTION_TRANSMIT_AND_RECEIVE;
    t_msp_device_context    *p_msp_device_context;

    DBGENTER1(" (%x)", msp_device_id);
    if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    p_msp_device_context = &g_msp_system_context[msp_device_id].msp_device_context_main;

    msp_error = MSP_Reset(msp_device_id);
    if (MSP_OK != msp_error)
    {
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    msp_error = MSP_Disable(msp_device_id, MSP_DIRECTION_TRANSMIT_AND_RECEIVE);
    if (MSP_OK != msp_error)
    {
        DBGEXIT0(msp_error);
        return(msp_error);
    }

    if (TRUE == p_msp_device_context->msp_general_configuration_done || TRUE == p_msp_device_context->msp_protocol_descriptor_set)
    {
        msp_error = MSP_Configure(msp_device_id, &p_msp_device_context->general_config, &p_msp_device_context->protocol_descriptor);
        if (MSP_OK != msp_error)
        {
            DBGEXIT0(msp_error);
            return(msp_error);
        }
    }

    if (TRUE == p_msp_device_context->multichannel_configured)
    {
        msp_error = MSP_ConfigureMultiChannel(msp_device_id, &p_msp_device_context->multichannel_config);
        if (MSP_OK != msp_error)
        {
            DBGEXIT0(msp_error);
            return(msp_error);
        }
    }

    if (TRUE == p_msp_device_context->serial_clock_divisor_set)
    {
        msp_error = MSP_SetSerialClockdevisor(msp_device_id, p_msp_device_context->msp_in_clock_frequency, p_msp_device_context->serial_clock_divisor);
        if (MSP_OK != msp_error)
        {
            DBGEXIT0(msp_error);
            return(msp_error);
        }
    }

    /*restoring interrupt mask status register*/
    g_msp_system_context[msp_device_id].p_msp_register->msp_imsc = p_msp_device_context->imsc_reg;

    if (TRUE == p_msp_device_context->rx_enable || TRUE == p_msp_device_context->tx_enable)
    {
        /* if MSP was not enabled prevoiusly control will not reach here so no need to check enable*/
        if (TRUE == p_msp_device_context->tx_enable && TRUE == p_msp_device_context->rx_enable)
        {
            msp_direction = MSP_DIRECTION_TRANSMIT_AND_RECEIVE;
        }
        else if (TRUE == p_msp_device_context->tx_enable)
        {
            msp_direction = MSP_DIRECTION_TRANSMIT;
        }
        else if (TRUE == p_msp_device_context->rx_enable)
        {
            msp_direction = MSP_DIRECTION_RECEIVE;
        }

        msp_error = MSP_Enable(msp_device_id, msp_direction);

        if (MSP_OK != msp_error)
        {
            DBGEXIT0(msp_error);
            return(msp_error);
        }
    }

    DBGEXIT0(msp_error);
    return(msp_error);
}

/****************************************************************************/
/* NAME:        MSP_GetVersion                                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Gives the current version of the HCL                     */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:    None                                                              */
/* OUT:   p_version : structure which will consist of the version of the    */
/*        current HCL                                                       */
/*                                                                          */
/* RETURN:        		                                                    */
/* MSP_INVALID_PARAMETER: if p_version is NULL								*/
/* MSP_OK: if successfull                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: ReEntrant                                                    */
/* REENTRANCY  No Issues                                                    */

/****************************************************************************/
PUBLIC t_msp_error MSP_GetVersion(OUT t_version *p_version)
{
    DBGENTER1(" (%lx)", p_version);
    if (NULL != p_version)
    {
        p_version->minor = MSP_HCL_MINOR_ID;
        p_version->major = MSP_HCL_MAJOR_ID;
        p_version->version = MSP_HCL_VERSION_ID;
        DBGEXIT0(MSP_OK);
        return(MSP_OK);
    }
    else
    {
        DBGEXIT0(MSP_INVALID_PARAMETER);
        return(MSP_INVALID_PARAMETER);
    }
}
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
/****************************************************************************/
/* NAME:        MSP_SetTxFifoWmrkLevel                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Sets the Tx Fifo Watermark Level for the MSP Devices	    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN: 		t_msp_device_id:    MSP device ID                               */
/*			t_msp_wmrk_level:   The Watermark level for the Tx Fifo			*/
/* OUT:   																	*/
/*                                                                          */
/* RETURN:        		                                                    */
/* MSP_INVALID_PARAMETER: if id passed is wrong or the parameter			*/
/* MSP_OK: if successfull                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: ReEntrant                                                    */
/* REENTRANCY  No Issues                                                    */
/****************************************************************************/
PUBLIC t_msp_error MSP_SetTxFifoWmrkLevel(IN t_msp_device_id msp_device_id, IN t_msp_wmrk_level msp_wmrk_level)
{
	t_msp_register          *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
	t_msp_error 			msp_error = MSP_OK;
	
	DBGENTER2("(%x) (%x)", msp_device_id, msp_wmrk_level);
	 if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }
	switch(msp_device_id)
	{
		case MSP_DEVICE_ID_0:
			if(msp_wmrk_level > MSP_FOUR_ELEMENTS) 	/*Size of Tx Fifo here is 8*/
			{
				msp_error = MSP_INVALID_PARAMETER
				DBGEXIT0(msp_error);
				return(msp_error);
			}
			break;
		case MSP_DEVICE_ID_1:
			if(msp_wmrk_level > MSP_SIXTEEN_ELEMENTS)
			{
				msp_error = MSP_INVALID_PARAMETER
				DBGEXIT0(msp_error);
				return(msp_error);
			}
			break;
		case MSP_DEVICE_ID_2:
			if(msp_wmrk_level > MSP_SIXTEEN_ELEMENTS)
			{
				msp_error = MSP_INVALID_PARAMETER
				DBGEXIT0(msp_error);
				return(msp_error);
			}
			break;
		case MSP_DEVICE_ID_3:
			break; 
		
	}
	/*Set the Tx Fifo Value in the Register*/
	MSP_WRITE_BITS(p_msp_register->msp_wmrk, msp_wmrk_level,THREE_BITS,TWMRK_BIT);
	
	DBGEXIT0(msp_error);
	return(msp_error);	
}
/****************************************************************************/
/* NAME:        MSP_SetRxFifoWmrkLevel                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Sets the Rx Fifo Watermark Level for the MSP Devices	    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN: 		t_msp_device_id:    MSP device ID                               */
/*			t_msp_wmrk_level:   The Watermark level for the Rx Fifo			*/
/* OUT:   																	*/
/*                                                                          */
/* RETURN:        		                                                    */
/* MSP_INVALID_PARAMETER: if id passed is wrong or the parameter			*/
/* MSP_OK: if successfull                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: ReEntrant                                                    */
/* REENTRANCY  No Issues                                                    */
/****************************************************************************/
PUBLIC t_msp_error MSP_SetRxFifoWmrkLevel(IN t_msp_device_id msp_device_id, IN t_msp_wmrk_level msp_wmrk_level)
{
	t_msp_register *p_msp_register = g_msp_system_context[msp_device_id].p_msp_register;
	t_msp_error msp_error = MSP_OK; 
	
	DBGENTER2("(%x) (%x)", msp_device_id, msp_wmrk_level);
	 if ((t_uint32) msp_device_id >= MSP_INSTANCES)
    {
        msp_error = MSP_INVALID_PARAMETER;
        DBGEXIT0(msp_error);
        return(msp_error);
    }
	switch(msp_device_id)
	{
		case MSP_DEVICE_ID_0:
		if(msp_wmrk_level > MSP_FOUR_ELEMENTS)
		{
			msp_error = MSP_INVALID_PARAMETER;
			DBGEXIT0(msp_error);
			return(msp_error);
		}
		break; 
		
		case MSP_DEVICE_ID_1:
		break;
		
		case MSP_DEVICE_ID_2:
		if(msp_wmrk_level > MSP_TWO_ELEMENTS)
		{
			msp_error = MSP_INVALID_PARAMETER;
			DBGEXIT0(msp_error);
			return(msp_error);
		}
		break;
		
		case MSP_DEVICE_ID_3:
		if(msp_wmrk_level > MSP_SIXTEEN_ELEMENTS)
		{
			msp_error = MSP_INVALID_PARAMETER;
			DBGEXIT0(msp_error);
			return(msp_error);
		}
		break;		
	}
	/*Set the Rx Fifo Register*/
	MSP_WRITE_BITS(p_msp_register->msp_wmrk, msp_wmrk_level,THREE_BITS,RWMRK_BIT);
	
	DBGEXIT0(msp_error);
	return(msp_error);
}
#endif /*End the changes for V2.0*/

/* End of file - msp.c*/

