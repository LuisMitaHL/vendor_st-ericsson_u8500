/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the PL022 (SPI)
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "spi_p.h"

/*--------------------------------------------------------------------------*
 * debug stuff																*
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_SPI
#define MY_DEBUG_ID             myDebugID_SPI
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = SPI_HCL_DBG_ID;
#endif

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PUBLIC t_spi_system_context    g_spi_system_context[NUM_SPI_INSTANCES];

/****************************************************************************/
/* NAME:    spi_CalculateTransferWidthSize                                  */
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
PRIVATE t_spi_transfer_data_size spi_CalculateTransferWidthSize(IN t_spi_data_size spi_data_transfer_width)
{
    /* By default size equivalent to transfer width is set to 4
	---------------------------------------------------*/
    t_spi_transfer_data_size    transfer_width_size = (t_spi_transfer_data_size) sizeof(t_uint32);

    /* add transfer width to size conversions here 
	--------------------------------------------------*/
    if ((SPI_DATA_BITS_4 <= spi_data_transfer_width) && (SPI_DATA_BITS_9 > spi_data_transfer_width))
    {
        transfer_width_size = (t_spi_transfer_data_size) sizeof(t_uint8);
    }
    else if ((SPI_DATA_BITS_9 <= spi_data_transfer_width) && (SPI_DATA_BITS_17 > spi_data_transfer_width))
    {
        transfer_width_size = (t_spi_transfer_data_size) sizeof(t_uint16);
    }
    else
    {
        transfer_width_size = (t_spi_transfer_data_size) sizeof(t_uint32);
    }

    return(transfer_width_size);
}

/**********************************************************************************************/
/* NAME:	t_bool spi_IsSpiMaster()										     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns TRUE if SPI is master o.w. it returns FALSE	  */
/* PARAMETERS:																                  */
/* IN : 		NONE														  	          	  */
/* OUT:			NONE																		  */
/* RETURN:		t_bool :	TRUE, if SPI is master otherwise FALSE				          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_bool spi_IsSpiMaster(IN t_spi_device_id spi_device_id)
{
    DBGENTER0();
    if (g_spi_system_context[spi_device_id].p_spi_register->spi_cr1 & SPI_CR1_MASK_MS)
    {
        DBGEXIT0(SPI_OK);
        return(FALSE);
    }
    else
    {
        DBGEXIT0(SPI_OK);
        return(TRUE);
    }
}

/**********************************************************************************************/
/* NAME:	t_spi_error spi_checkevent()    								     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns TRUE if SPI is master o.w. it returns FALSE	  */
/* PARAMETERS:																                  */
/* IN : 		NONE														  	          	  */
/* OUT:			NONE																		  */
/* RETURN:		t_bool :	TRUE, if SPI is master otherwise FALSE				          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_spi_error spi_checkevent(IN t_spi_event *p_local_event, IN t_spi_filter_mode filter_mode)
{
    /*No filter event
			-------------------*/
    if (NULL == ((*p_local_event) & filter_mode & SPI_ENABLE_ALL_INTERRUPT))
    {
        if (NULL == ((*p_local_event) & SPI_ENABLE_ALL_INTERRUPT))
        /*No active event
				------------------*/
        {
            DBGEXIT0(SPI_NO_MORE_PENDING_EVENT);
            return(SPI_NO_MORE_PENDING_EVENT);
        }
        else
        {
            DBGEXIT0(SPI_NO_MORE_FILTER_PENDING_EVENT);
            return(SPI_NO_MORE_FILTER_PENDING_EVENT);
        }
    }
    else
    {
        DBGEXIT0(SPI_INTERNAL_EVENT);
        return(SPI_INTERNAL_EVENT);
    }
}

/**********************************************************************************************/
/* NAME:	t_watermark_level spi_setrxlowerwatermark()									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns the current watermark level                	  */
/* PARAMETERS:																                  */
/* IN : 		watermark_level : watermark level of SPI					  	          	  */
/* OUT:			NONE																		  */
/* RETURN:		transfer_level :  The value for the current watermark level		          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_watermark_level spi_setrxlowerwatermark(IN t_spi_rx_level_trig watermark_level)
{
    t_watermark_level   transfer_level = NULL;

    DBGENTER0();

    switch (watermark_level)
    {
        case SPI_RX_1_OR_MORE_ELEM:
            transfer_level = MASK_BIT0;
            break;

        case SPI_RX_4_OR_MORE_ELEM:
            transfer_level = MASK_BIT2;
            break;

        default:
            transfer_level = spi_setrxupperwatermark(watermark_level);
    }

    return(transfer_level);
}

/**********************************************************************************************/
/* NAME:	t_watermark_level spi_setrxupperwatermark()						     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns the current watermark level                	  */
/* PARAMETERS:																                  */
/* IN : 		watermark_level : watermark level of SPI					  	          	  */
/* OUT:			NONE																		  */
/* RETURN:		transfer_level :  The value for the current watermark level		          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_watermark_level spi_setrxupperwatermark(IN t_spi_rx_level_trig watermark_level)
{
    t_watermark_level   transfer_level = NULL;

    DBGENTER0();

    switch (watermark_level)
    {
        case SPI_RX_8_OR_MORE_ELEM:
            transfer_level = MASK_BIT3;
            break;

        case SPI_RX_16_OR_MORE_ELEM:
            transfer_level = MASK_BIT4;
            break;

        case SPI_RX_32_OR_MORE_ELEM:
            transfer_level = MASK_BIT5;
            break;
    }

    return(transfer_level);
}

/**********************************************************************************************/
/* NAME:	t_watermark_level spi_settxlowerwatermark()									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns the current watermark level                	  */
/* PARAMETERS:																                  */
/* IN : 		watermark_level : watermark level of SPI					  	          	  */
/* OUT:			NONE																		  */
/* RETURN:		transfer_level :  The value for the current watermark level		          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_watermark_level spi_settxlowerwatermark(IN t_spi_tx_level_trig watermark_level)
{
    t_watermark_level   transfer_level = NULL;

    DBGENTER0();

    switch (watermark_level)
    {
        case SPI_TX_1_OR_MORE_EMPTY_LOC:
            transfer_level = MASK_BIT0;
            break;

        case SPI_TX_4_OR_MORE_EMPTY_LOC:
            transfer_level = MASK_BIT2;
            break;

        default:
            transfer_level = spi_settxupperwatermark(watermark_level);
    }

    return(transfer_level);
}

/**********************************************************************************************/
/* NAME:	t_watermark_level spi_settxupperwatermark()						     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns the current watermark level                	  */
/* PARAMETERS:																                  */
/* IN : 		watermark_level : watermark level of SPI					  	          	  */
/* OUT:			NONE																		  */
/* RETURN:		transfer_level :  The value for the current watermark level		          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_watermark_level spi_settxupperwatermark(IN t_spi_tx_level_trig watermark_level)
{
    t_watermark_level   transfer_level = NULL;

    DBGENTER0();

    switch (watermark_level)
    {
        case SPI_TX_8_OR_MORE_EMPTY_LOC:
            transfer_level = MASK_BIT3;
            break;

        case SPI_TX_16_OR_MORE_EMPTY_LOC:
            transfer_level = MASK_BIT4;
            break;

        case SPI_TX_32_OR_MORE_EMPTY_LOC:
            transfer_level = MASK_BIT5;
            break;
    }

    return(transfer_level);
}

/**********************************************************************************************/
/* NAME:	t_watermark_level spi_ConvertRxWaterMarkLevel()   				     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns the number of elements that should be taken	  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_rx_level_trig watermark_level:Defines the watermark level given in SPI  */
/* OUT:			NONE                                                                          */
/* RETURN:		t_watermark_level transfer_level:Defines the number of elements to be taken   */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_watermark_level spi_ConvertRxWaterMarkLevel(IN t_spi_rx_level_trig watermark_level)
{
    t_watermark_level   transfer_level = NULL;

    DBGENTER0();

    transfer_level = spi_setrxlowerwatermark(watermark_level);
	/*coverity[self_assign]*/
    transfer_level = transfer_level;

    DBGEXIT0(SPI_OK);
    return(transfer_level);
}

/**********************************************************************************************/
/* NAME:	t_watermark_level spi_ConvertTxWaterMarkLevel()   				     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns the number of elements that should be taken	  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_tx_level_trig watermark_level:Defines the watermark level given in SPI  */
/* OUT:			NONE                                                                          */
/* RETURN:		t_watermark_level transfer_level:Defines the number of elements to be taken   */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PRIVATE t_watermark_level spi_ConvertTxWaterMarkLevel(IN t_spi_tx_level_trig watermark_level)
{
    t_watermark_level   transfer_level = NULL;

    DBGENTER0();

    transfer_level = spi_settxlowerwatermark(watermark_level);
	/*coverity[self_assign]*/
    transfer_level = transfer_level;

    DBGEXIT0(SPI_OK);
    return(transfer_level);
}

/****************************************************************************/
/* NAME:	t_spi_error spip_InterruptRecieveData ()						*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used inside the Filterprocess to recieve 	*/
/*              data                                                        */
/* PARAMETERS:																*/
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	    */
/* OUT: none 																*/
/* RETURN: t_spi_error 														*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PRIVATE t_spi_error spip_InterruptRecieveData(t_spi_device_id spi_device_id)
{
    t_uint8     watermark_level;
    t_uint8     current_routine = (t_uint8) SPI_IRQ_SRC_RECEIVE;
    t_spi_error spi_error = SPI_OK;

    /* Disable the Rx Interrupt */
    SPI_SET_RXIM(g_spi_system_context[spi_device_id].p_spi_register->spi_imsc, SPI_DISABLE);

    /* Convert the watermark for the recieve FIFO */
    watermark_level = spi_ConvertRxWaterMarkLevel(g_spi_system_context[spi_device_id].p_spi_config->rx_lev_trig);

    if (g_spi_system_context[spi_device_id].rx_status.it_element_num > watermark_level)
    {
        while (watermark_level)
        {
            if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
            {
                switch (g_spi_system_context[spi_device_id].transfer_data_size)
                {
                    case SPI_DATA_SIZE_BYTE:
                        *((t_uint8 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint8) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_HALFWORD:
                        *((t_uint16 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint16) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_WORD:
                        *((t_uint32 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint32) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                        spi_error = SPI_OK;
                        break;

                    default:
                        spi_error = SPI_INVALID_PARAMETER;
                        break;
                }
            }
            else
            {
                return(SPI_RECEIVE_ERROR);
            }

            g_spi_system_context[spi_device_id].rx_status.it_element_num--;
            g_spi_system_context[spi_device_id].rx_status.p_it_data_flow += (t_uint8) g_spi_system_context[spi_device_id].transfer_data_size;
            watermark_level--;
        }

        /* Enable the Rx Interrupt */
        SPI_SET_RXIM(g_spi_system_context[spi_device_id].p_spi_register->spi_imsc, SPI_ENABLE);

        /* Convert the watermark for the recieve FIFO */
        watermark_level = spi_ConvertRxWaterMarkLevel(g_spi_system_context[spi_device_id].p_spi_config->rx_lev_trig);

        if (g_spi_system_context[spi_device_id].rx_status.it_element_num < watermark_level)
        {
            /* Set the Rx watermark Level to zero */
            SPI_SET_RXIFLSEL
            (
                g_spi_system_context[spi_device_id].p_spi_register->spi_cr1,
                (t_uint32) SPI_RX_1_OR_MORE_ELEM
            );
        }
    }
    else
    {
        while (g_spi_system_context[spi_device_id].rx_status.it_element_num)
        {
            if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
            {
                switch (g_spi_system_context[spi_device_id].transfer_data_size)
                {
                    case SPI_DATA_SIZE_BYTE:
                        *((t_uint8 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint8) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_HALFWORD:
                        *((t_uint16 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint16) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_WORD:
                        *((t_uint32 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint32) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                        spi_error = SPI_OK;
                        break;

                    default:
                        spi_error = SPI_INVALID_PARAMETER;
                        break;
                }
            }
            else
            {
                return(SPI_RECEIVE_ERROR);
            }

            g_spi_system_context[spi_device_id].rx_status.it_element_num--;
            g_spi_system_context[spi_device_id].rx_status.p_it_data_flow += (t_uint8) g_spi_system_context[spi_device_id].transfer_data_size;
        }
    }

    if (NULL == g_spi_system_context[spi_device_id].rx_status.it_element_num)
    {
        g_spi_system_context[spi_device_id].p_spi_register->spi_imsc &= ~current_routine;
    }

    return(spi_error);
}

/****************************************************************************/
/* NAME:	t_spi_error spip_InterruptRecieverTimeout ()    				*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used inside the Filterprocess to check  	*/
/*              the Reciever Timeout Error                                  */
/* PARAMETERS:																*/
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	    */
/* OUT: none 																*/
/* RETURN: t_spi_error 														*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PRIVATE void spip_InterruptRecieverTimeout(t_spi_device_id spi_device_id)
{
    if (g_spi_system_context[spi_device_id].rx_status.it_element_num)
    {
        while (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
        {
            switch (g_spi_system_context[spi_device_id].transfer_data_size)
            {
                case SPI_DATA_SIZE_BYTE:
                    *((t_uint8 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint8) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;

                    break;

                case SPI_DATA_SIZE_HALFWORD:
                    *((t_uint16 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint16) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;

                    break;

                case SPI_DATA_SIZE_WORD:
                    *((t_uint32 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint32) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;

                    break;

                default:
                    break;
            }

            g_spi_system_context[spi_device_id].rx_status.it_element_num--;
            g_spi_system_context[spi_device_id].rx_status.p_it_data_flow += (t_uint8) g_spi_system_context[spi_device_id].transfer_data_size;
        }
    }

    g_spi_system_context[spi_device_id].rx_status.flow_error_nb = SPI_ERROR_TIMEOUT;

    /* Clearing the Reciever Over run Interrupt */
    SPI_SET_ICR_RTIC(g_spi_system_context[spi_device_id].p_spi_register->spi_icr, SPI_ENABLE);
}

/****************************************************************************/
/* NAME:	t_spi_error spip_InterruptTransmitData ()						*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used inside the Filterprocess to transmit 	*/
/*              data                                                        */
/* PARAMETERS:																*/
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	    */
/* OUT: none 																*/
/* RETURN: t_spi_error 														*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PRIVATE t_spi_error spip_InterruptTransmitData(t_spi_device_id spi_device_id)
{
    t_uint8     watermark_level;
    t_uint8     current_routine = (t_uint8) SPI_IRQ_SRC_TRANSMIT;
    t_spi_error spi_error = SPI_OK;

    /* Disable the Tx Interrupt */
    SPI_SET_TXIM(g_spi_system_context[spi_device_id].p_spi_register->spi_imsc, SPI_DISABLE);

    watermark_level = spi_ConvertTxWaterMarkLevel(g_spi_system_context[spi_device_id].p_spi_config->tx_lev_trig);

    if (g_spi_system_context[spi_device_id].tx_status.it_element_num > watermark_level)
    {
        while (watermark_level)
        {
            if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_TNF)
            {
                switch (g_spi_system_context[spi_device_id].transfer_data_size)
                {
                    case SPI_DATA_SIZE_BYTE:
                        g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint8 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_HALFWORD:
                        g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint16 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_WORD:
                        g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint32 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                        spi_error = SPI_OK;
                        break;

                    default:
                        spi_error = SPI_INVALID_PARAMETER;
                        break;
                }
            }
            else
            {
                DBGEXIT0(SPI_TRANSMIT_ERROR);
                return(SPI_TRANSMIT_ERROR);
            }

            g_spi_system_context[spi_device_id].tx_status.it_element_num--;
            g_spi_system_context[spi_device_id].tx_status.p_it_data_flow += (t_uint8) g_spi_system_context[spi_device_id].transfer_data_size;
            watermark_level--;
        }

        /* Enable the Tx Interrupt */
        SPI_SET_TXIM(g_spi_system_context[spi_device_id].p_spi_register->spi_imsc, SPI_ENABLE);
    }
    else
    {
        while (g_spi_system_context[spi_device_id].tx_status.it_element_num)
        {
            if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_TNF)
            {
                switch (g_spi_system_context[spi_device_id].transfer_data_size)
                {
                    case SPI_DATA_SIZE_BYTE:
                        g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint8 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_HALFWORD:
                        g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint16 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                        spi_error = SPI_OK;
                        break;

                    case SPI_DATA_SIZE_WORD:
                        g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint32 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                        spi_error = SPI_OK;
                        break;

                    default:
                        spi_error = SPI_INVALID_PARAMETER;
                        break;
                }
            }
            else
            {
                DBGEXIT0(SPI_TRANSMIT_ERROR);
                return(SPI_TRANSMIT_ERROR);
            }

            g_spi_system_context[spi_device_id].tx_status.it_element_num--;
            g_spi_system_context[spi_device_id].tx_status.p_it_data_flow += (t_uint8) g_spi_system_context[spi_device_id].transfer_data_size;
        }
    }

    if (NULL == g_spi_system_context[spi_device_id].tx_status.it_element_num)
    {
        g_spi_system_context[spi_device_id].p_spi_register->spi_imsc &= ~current_routine;
    }

    return(spi_error);
}

/****************************************************************************/
/* NAME:	t_spi_error SPI_Init ();										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the SPI registers.					*/
/* PARAMETERS:																*/
/* IN : t_logical_address spi_base_address:	SPI registers base address 	    */
/* OUT: none 																*/
/* RETURN: t_spi_error 														*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant												*/

/****************************************************************************/
PUBLIC t_spi_error SPI_Init(IN t_spi_device_id spi_device_id, IN t_logical_address spi_base_address)
{
    t_spi_error spi_error;
    DBGENTER0();
    if (NULL == spi_base_address)
    {
        spi_error = SPI_UNSUPPORTED_HW;
        DBGEXIT0(spi_error);
        return(spi_error);
    }

    /* Initialize the SPI registers structure */
    g_spi_system_context[spi_device_id].p_spi_register = (t_spi_register *) spi_base_address;

    /* Check Peripheral and Pcell Id Register for SPI */
    if
    (
        (SPI_PERIPHID0 == g_spi_system_context[spi_device_id].p_spi_register->spi_periphid0)
    &&  (SPI_PERIPHID1 == g_spi_system_context[spi_device_id].p_spi_register->spi_periphid1)
    &&  (SPI_PERIPHID2 == g_spi_system_context[spi_device_id].p_spi_register->spi_periphid2)
    &&  (SPI_PERIPHID3 == g_spi_system_context[spi_device_id].p_spi_register->spi_periphid3)
    &&  (SPI_PCELLID0 == g_spi_system_context[spi_device_id].p_spi_register->spi_pcellid0)
    &&  (SPI_PCELLID1 == g_spi_system_context[spi_device_id].p_spi_register->spi_pcellid1)
    &&  (SPI_PCELLID2 == g_spi_system_context[spi_device_id].p_spi_register->spi_pcellid2)
    &&  (SPI_PCELLID3 == g_spi_system_context[spi_device_id].p_spi_register->spi_pcellid3)
    )
    {
        spi_error = SPI_OK;
    }
    else
    {
        spi_error = SPI_INVALID_PARAMETER;
    }

    DBGEXIT0(spi_error);
    return(spi_error);
}

/**********************************************************************************************/
/* NAME:	t_spi_error SPI_GetVersion()									     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns version for SPI HCL.							          */
/* PARAMETERS:																                  */
/* IN : 		NONE																		  */
/* OUT : 		t_version*   : p_version	Version for SPI HCL								  */
/* 																			                  */
/* RETURN:		t_spi_error :          SPI error code							          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant																	  */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_GetVersion(OUT t_version *p_version)
{
    DBGENTER0();
    if (NULL == p_version)
    {
        DBGEXIT0(SPI_INVALID_PARAMETER);
        return(SPI_INVALID_PARAMETER);
    }

    p_version->minor = SPI_HCL_MINOR_ID;
    p_version->major = SPI_HCL_MAJOR_ID;
    p_version->version = SPI_HCL_VERSION_ID;

    DBGEXIT0(SPI_OK);
    return(SPI_OK);
}

/**********************************************************************************************/
/* NAME:	t_spi_error SPI_SetDbgLevel()									     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug comments levels        */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_dbg_level :		    DebugLevel from DEBUG_LEVEL0 to DEBUG_LEVEL4.      	  */
/* OUT: 		none																		  */
/* 																			                  */
/* RETURN:		t_spi_error :			SPI error code							          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non-reentrant												                  */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_SetDbgLevel(IN t_dbg_level spi_dbg_level)
{
    DBGENTER1(" (%x)", spi_dbg_level);
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = spi_dbg_level;
#endif
    DBGEXIT0(SPI_OK);
    return(SPI_OK);
}

/**********************************************************************************************/
/* NAME:	t_spi_error SPI_Enable()										     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables Tx-Rx of SPI.							      */
/* PARAMETERS:																                  */
/* IN : 		t_spi_enable rx_tx_enable:		    Enable/Disable SPI Rx, Tx  	          	  */
/* OUT:         none																		  */
/* RETURN:		t_spi_error :						SPI error code				          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_Enable(IN t_spi_device_id spi_device_id, IN t_spi_enable rx_tx_enable)
{
    t_bool  rx_enable = FALSE;
    t_bool  tx_enable = FALSE;

    DBGENTER1(" (%x)", rx_tx_enable);

    if (SPI_DISABLE_RX_TX != rx_tx_enable)
    {
        /* Set SSE bit in SPI_CR1 */
        SPI_SET_SSE(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_ENABLE);

        if (SPI_ENABLE_RX_TX == rx_tx_enable)
        {
            rx_enable = TRUE;
            tx_enable = TRUE;
        }
        else
        {
            if (SPI_ENABLE_RX_DISABLE_TX == rx_tx_enable)
            {
                rx_enable = TRUE;
            }

            if (SPI_DISABLE_RX_ENABLE_TX == rx_tx_enable)
            {
                tx_enable = TRUE;
            }
        }
    }
    else
    {   /* Clear SSE bit in SPI_CR1 */
        SPI_SET_SSE(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_DISABLE);
    }

    /* Set DMACR register values */
    if (SPI_DMA_MODE == g_spi_system_context[spi_device_id].tx_com_mode)
    {
        if (tx_enable)
        {
            g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr |= SPI_DMACR_MASK_TXDMAE;     /* tx is through dma  and tx is enabled.*/
        }
        else
        {
            g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr &= ~SPI_DMACR_MASK_TXDMAE;    /* tx is through dma  and tx is disabled.*/
        }
    }
    else
    {
        g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr &= ~SPI_DMACR_MASK_TXDMAE;        /*  tx is not through dma mode.*/
    }

    if (SPI_DMA_MODE == g_spi_system_context[spi_device_id].rx_com_mode)
    {
        if (rx_enable)
        {
            g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr |= SPI_DMACR_MASK_RXDMAE;     /* rx is through dma  and rx is enabled.*/
        }
        else
        {
            g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr &= ~SPI_DMACR_MASK_RXDMAE;    /* rx is through dma  and rx is disabled.*/
        }
    }
    else
    {
        g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr &= ~SPI_DMACR_MASK_RXDMAE;        /* rx is not through dma mode.*/
    }

    DBGEXIT0(SPI_OK);
    return(SPI_OK);
}

/**********************************************************************************************/
/* NAME:	void SPI_Reset()													     		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This Private routine returns TRUE if SPI is Reset.o.W.it returns FALSE	 	  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		NONE														  	          	  */
/* OUT:			NONE																		  */
/* 																			                  */
/* RETURN:		void :															          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC void SPI_Reset(IN t_spi_device_id spi_device_id)
{
    t_uint8     rx_element;
    t_uint32    rx_fifo_not_empty = 0x4;
    t_uint32    fifo_status;
    t_spi_error spi_error;

    g_spi_system_context[spi_device_id].p_spi_register->spi_imsc &= ~(SPI_RIS_MASK_RORRIS | SPI_RIS_MASK_RTRIS | SPI_RIS_MASK_RXRIS | SPI_RIS_MASK_TXRIS);

    g_spi_system_context[spi_device_id].p_spi_register->spi_icr |= (SPI_ICR_MASK_RORIC | SPI_ICR_MASK_RTIC);

    g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr = SPI_ALLZERO;
    g_spi_system_context[spi_device_id].p_spi_register->spi_cr0 = SPI_ALLZERO;
    g_spi_system_context[spi_device_id].p_spi_register->spi_cr1 = SPI_ALLZERO;

    DBGENTER0();

    /* flush receive fifo */
    do
    {
        if ((spi_error = SPI_GetFIFOStatus(spi_device_id, &fifo_status)) != SPI_OK)
        {
            DBGEXIT0(spi_error);
            return;
        }

        if (fifo_status & rx_fifo_not_empty)
        {
            if ((spi_error = SPI_GetData(spi_device_id, &rx_element)) != SPI_OK)
            {
                DBGEXIT0(spi_error);
                return;
            }
        }
    } while (fifo_status & rx_fifo_not_empty);

    if (spi_error != SPI_OK)
    {
        DBGEXIT0(spi_error);
    }
    else
    {
        DBGEXIT0(SPI_OK);
    }

    return;
}

/**********************************************************************************************/
/* NAME:	t_spi_error SPI_ResolveClockFrequency()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine calculates the value of effective frequency.				      */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_uint32 spi_freq:		    Frequency of the clock that feeds SPI HW IP    	  */
/*				t_uint32 target_freq:		Target frequency when SPI is master				  */
/* OUT:			t_uint32 *p_effective_freq	Closest frequency that SPI can effectively achieve*/
/*				t_spi_clock_params *p_clock_parameters									      */
/*											Clock parameters corresponding to effectiveFreq   */
/* 																			                  */
/* RETURN:		t_spi_error :						SPI error code				          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_ResolveClockFrequency
(
    IN t_uint32             spi_freq,
    IN t_uint32             target_freq,
    OUT t_uint32            *p_effective_freq,
    OUT t_spi_clock_params  *p_clock_parameters
)
{
    t_spi_error status = SPI_OK;
    t_uint32    cpsdvsr = SPI_MIN_CPSDVR;
    t_uint32    scr = SPI_MIN_SCR;
    t_bool      freq_found = FALSE;
    t_uint32    max_tclk;
    t_uint32    min_tclk;
    t_uint32    temp_effective_freq_lower, temp_effective_freq_higher;

    DBGENTER2(" (%ld %ld) ", spi_freq, target_freq);

    /* Check for null pointers */
    if ((NULL == p_effective_freq) || (NULL == p_clock_parameters))
    {
        status = SPI_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    max_tclk = (spi_freq / (SPI_MIN_CPSDVR * (SPI_CLOCK_INCREMENT + SPI_MIN_SCR))); /* cpsdvscr = 2 & scr 0 */
    min_tclk = (spi_freq / (SPI_MAX_CPSDVR * (SPI_CLOCK_INCREMENT + SPI_MAX_SCR))); /* cpsdvsr = 254 & scr = 255 */

    if ((target_freq <= max_tclk) && (target_freq >= min_tclk))
    {
        while (cpsdvsr <= SPI_MAX_CPSDVR && !freq_found)
        {
            while (scr <= SPI_MAX_SCR && !freq_found)
            {
                if ((spi_freq / (cpsdvsr * (SPI_CLOCK_INCREMENT + scr))) > target_freq)
                {
                    scr += SPI_CLOCK_INCREMENT;
                }
                else
                {
                    /* This bool is made TRUE when effective frequency >= target frequency is found*/
                    freq_found = TRUE;
                    if ((spi_freq / (cpsdvsr * (SPI_CLOCK_INCREMENT + scr))) != target_freq)
                    {
                        /* Compare the Effective Frequency to get the Closet to the Desired Frequency */
                        temp_effective_freq_lower = spi_freq / (cpsdvsr * (SPI_CLOCK_INCREMENT + scr));
                        temp_effective_freq_higher = spi_freq / (cpsdvsr * (SPI_CLOCK_INCREMENT + (scr - SPI_CLOCK_INCREMENT)));

                        /* Compare both the Frequencies with the desired frequency */
                        /* Calculate the Delta between the Higher Calculated Freq and Traget Freq */
                        temp_effective_freq_higher -= target_freq;

                        /* Calculate the Delta between the Lower Calculated Freq and Traget Freq */
                        temp_effective_freq_lower = target_freq - temp_effective_freq_lower;

                        if (temp_effective_freq_higher < temp_effective_freq_lower)
                        {
                            if (SPI_MIN_SCR == scr)
                            {
                                cpsdvsr -= SPI_MIN_CPSDVR;
                                scr = SPI_MAX_SCR;
                            }
                            else
                            {
                                scr -= SPI_CLOCK_INCREMENT;
                            }
                        }
                    }   /* end of if loop */
                }       /* end of else loop */
            }           /* end of inner while loop */

            if (!freq_found)
            {
                cpsdvsr += SPI_MIN_CPSDVR;
                scr = SPI_MIN_SCR;
            }
        }               /* end of outer while */

        if (cpsdvsr != NULL)
        {
            *p_effective_freq = spi_freq / (cpsdvsr * (SPI_CLOCK_INCREMENT + scr));
            p_clock_parameters->cpsdvsr = (t_uint8) (cpsdvsr & SPI_LAST8BITS);
            p_clock_parameters->scr = (t_uint8) (scr & SPI_LAST8BITS);
        }
    }
    else
    {
        status = SPI_INVALID_PARAMETER;
        *p_effective_freq = SPI_MIN_SCR;
        DBGEXIT0(status);
        return(status);
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	 t_spi_error SPI_CheckConfiguration()									          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine checks the configuration parameters.							  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_config  *p_Config:   structure holding the config parameters			  */
/* OUT: 		none														                  */
/* 																			                  */
/* RETURN:		t_spi_error :            SPI error code										  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_CheckConfiguration(IN t_spi_config *p_config)
{
    t_spi_error status = SPI_OK;
    DBGENTER0();

    /*Check for null pointer. */
    if (NULL == p_config)
    {
        status = SPI_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	 t_spi_error SPI_SetConfiguration()									              */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures SPI registers.										  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_config  *p_config  pointer to the structure							  */
/*										 holding the config parameters.						  */
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	                      */
/* OUT : 		NONE														                  */
/* 																			                  */
/* RETURN:		t_spi_error :            SPI error code										  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant														          */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_SetConfiguration(IN t_spi_device_id spi_device_id, IN t_spi_config *p_config)
{
    t_spi_error status = SPI_OK;
    t_uint32    prescale_dvsr;

    DBGENTER0();

    /*Check for null pointer. */
    if (NULL == p_config)
    {
        status = SPI_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    /* Set MS bit to master/slave hierarchy */
    SPI_SET_MS(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, (t_uint32) p_config->hierarchy);

    /* Set SCR and CPSDVSR bits to clock parameters */
    if (spi_IsSpiMaster(spi_device_id))
    {
        prescale_dvsr = (t_uint32) p_config->clk_freq.cpsdvsr;
        if ((prescale_dvsr % SPI_MIN_CPSDVR) != SPI_LOW)
        {
            prescale_dvsr = prescale_dvsr - SPI_CLOCK_INCREMENT;    /* make it even */
        }

        /* The Last bit in CPDVSR is always zero */
        SPI_SET_CPSDVSR(g_spi_system_context[spi_device_id].p_spi_register->spi_cpsr, prescale_dvsr);
        SPI_SET_SCR(g_spi_system_context[spi_device_id].p_spi_register->spi_cr0, (t_uint32) p_config->clk_freq.scr);
    }

    /* Set Endianness */
    switch (p_config->endian)
    {
        case SPI_RX_MSB_TX_MSB:
            SPI_SET_RENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_DISABLE);
            SPI_SET_TENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_DISABLE);
            break;

        case SPI_RX_MSB_TX_LSB:
            SPI_SET_RENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_DISABLE);
            SPI_SET_TENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_ENABLE);
            break;

        case SPI_RX_LSB_TX_MSB:
            SPI_SET_RENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_ENABLE);
            SPI_SET_TENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_DISABLE);
            break;

        case SPI_RX_LSB_TX_LSB:
            SPI_SET_RENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_ENABLE);
            SPI_SET_TENDN(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, SPI_ENABLE);
            break;

        default:
            status = SPI_INVALID_PARAMETER;
            DBGEXIT0(status);
            return(status);
    }

    SPI_SET_DSS(g_spi_system_context[spi_device_id].p_spi_register->spi_cr0, (t_uint32) (p_config->data_size));

    g_spi_system_context[spi_device_id].transfer_data_size = spi_CalculateTransferWidthSize(p_config->data_size);

    /* Set g_spi_system_context.tx_com_mode and g_spi_system_context.rx_com_mode to the communication mode */
    g_spi_system_context[spi_device_id].tx_com_mode = p_config->txcom_mode;
    g_spi_system_context[spi_device_id].rx_com_mode = p_config->rxcom_mode;

    SPI_SET_RXIFLSEL(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, (t_uint32) p_config->rx_lev_trig);
    SPI_SET_TXIFLSEL(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, (t_uint32) p_config->tx_lev_trig);

    SPI_SET_SPO(g_spi_system_context[spi_device_id].p_spi_register->spi_cr0, (t_uint32) p_config->clk_pol);
    SPI_SET_SPH(g_spi_system_context[spi_device_id].p_spi_register->spi_cr0, (t_uint32) p_config->clk_phase);

    /* Set SOD bit in slave mode*/
    if (!spi_IsSpiMaster(spi_device_id))
    {
        SPI_SET_SOD(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, (t_uint32) p_config->slave_tx_disable);
    }

    /* Clear Receive timeout and overrun Interrupts */
    g_spi_system_context[spi_device_id].p_spi_register->spi_icr = SPI_ICR_MASK_RORIC | SPI_ICR_MASK_RTIC;

    /* Disable all Interrupts */
    g_spi_system_context[spi_device_id].p_spi_register->spi_imsc = SPI_DISABLE_ALL_INTERRUPT;

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	 t_spi_error SPI_SetWatermarkLevel()								              */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures SPI watermark level.								  */
/* PARAMETERS:																                  */
/* IN : 		t_spi_rx_level_trig  rx_watermark : The watermark level for Rx				  */
/*              t_spi_tx_level_trig  tx_watermark : The watermark level for Tx				  */
/* IN :         t_spi_device_id spi_device_id:	SPI Device Id          	                      */
/* OUT : 		NONE														                  */
/* 																			                  */
/* RETURN:		t_spi_error :            SPI error code										  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-reentrant														          */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_SetWatermarkLevel
(
    t_spi_device_id         spi_device_id,
    IN t_spi_tx_level_trig  tx_watermark,
    t_spi_rx_level_trig     rx_watermark
)
{
    /* Set the Tx Watermark Level */
    SPI_SET_TXIFLSEL(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, (t_uint32) tx_watermark);

    /* Set the Rx Watermark Level */
    SPI_SET_RXIFLSEL(g_spi_system_context[spi_device_id].p_spi_register->spi_cr1, (t_uint32) rx_watermark);

    DBGEXIT0(SPI_OK);
    return(SPI_OK);
}

/**********************************************************************************************/
/* NAME:	t_spi_error		SPI_GetData()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets data from the receive FIFO								  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	                      */
/* OUT : 		t_uint8    *p_dataelement: represents the data element in the Rx FIFO		  */
/*              																			  */
/* 																			                  */
/* RETURN:		t_spi_error :              SPI error code									  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_GetData(IN t_spi_device_id spi_device_id, OUT t_uint8 *p_dataelement)
{
    t_spi_error status = SPI_OK;
    DBGENTER0();

    /*Check for null pointer. */
    if (NULL == p_dataelement)
    {
        status = SPI_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
    {
        switch (g_spi_system_context[spi_device_id].transfer_data_size)
        {
            case SPI_DATA_SIZE_BYTE:
                *((t_uint8 *) p_dataelement) = (t_uint8) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                status = SPI_OK;
                break;

            case SPI_DATA_SIZE_HALFWORD:
                *((t_uint16 *) p_dataelement) = (t_uint16) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                status = SPI_OK;
                break;

            case SPI_DATA_SIZE_WORD:
                *((t_uint32 *) p_dataelement) = (t_uint32) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                status = SPI_OK;
                break;

            default:
                status = SPI_INVALID_PARAMETER;
                break;
        }
    }
    else
    {
        status = SPI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	t_spi_error		SPI_SetData()													  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine puts data in the transmit FIFO									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_uint8    *data				: the data to be put in TX fifo				  */
/*              t_spi_device_id spi_device_id:	SPI Device Id          	                      */
/* OUT: 		none																		  */
/* 																			                  */
/* RETURN:		t_spi_error :            SPI error code										  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_SetData(IN t_spi_device_id spi_device_id, IN t_uint8 *p_dataoriginaddress)
{
    t_spi_error status = SPI_OK;
    DBGENTER0();

    if (NULL == p_dataoriginaddress)
    {
        status = SPI_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_TNF)
    {
        switch (g_spi_system_context[spi_device_id].transfer_data_size)
        {
            case SPI_DATA_SIZE_BYTE:
                g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint8 *) p_dataoriginaddress);
                status = SPI_OK;
                break;

            case SPI_DATA_SIZE_HALFWORD:
                g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint16 *) p_dataoriginaddress);
                status = SPI_OK;
                break;

            case SPI_DATA_SIZE_WORD:
                g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint32 *) p_dataoriginaddress);
                status = SPI_OK;
                break;

            default:
                status = SPI_INVALID_PARAMETER;
                break;
        }
    }
    else
    {
        status = SPI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	t_spi_error		SPI_GetFIFOstatus()												  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to get the value of status register					  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	                      */
/* OUT : 		t_uint32			*p_dataelement: value of status register				  */
/*              																			  */
/* 																			                  */
/* RETURN:		t_spi_error :       SPI error code											  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_GetFIFOStatus(IN t_spi_device_id spi_device_id, OUT t_uint32 *p_dataelement)
{
    t_spi_error status;
    DBGENTER0();

    /*Check for null pointer. */
    if (NULL == p_dataelement)
    {
        status = SPI_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    *p_dataelement = g_spi_system_context[spi_device_id].p_spi_register->spi_sr;
    if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
    {
        status = SPI_OK;
    }
    else
    {
        status = SPI_REQUEST_NOT_APPLICABLE;
    }

    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	t_spi_error		SPI_FlushFifo()		    										  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to flush the FIFO register         					  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	                      */
/* OUT : 		                                                                              */
/*              																			  */
/* 																			                  */
/* RETURN:		t_spi_error :       SPI error code											  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant															          */

/**********************************************************************************************/
PUBLIC t_spi_error SPI_FlushFifo(IN t_spi_device_id spi_device_id)
{
    t_spi_error status;
    t_uint32    dataelement = NULL;

    DBGENTER0();

    if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
    {
        while (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
        {
            /* This read is necessary to flush the SPI FIFO */
            dataelement = g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
        }

        status = SPI_OK;
    }
    else
    {
        status = SPI_REQUEST_NOT_APPLICABLE;
    }
	/*coverity[self_assign]*/
    dataelement = dataelement; /* intentional to remove compiler warning */
    DBGEXIT0(status);
    return(status);
}

/**********************************************************************************************/
/* NAME:	void SPI_SaveDeviceContext()										     	 	  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine saves current of SPI hardware for power management.			  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : t_spi_device_id spi_device_id:	SPI Device Id                 	                      */
/* OUT			NONE																		  */
/* RETURN:		void																		  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												                  */
/* Global array g_spi_system_context.device_context[9] is being modified					  */

/**********************************************************************************************/
PUBLIC void SPI_SaveDeviceContext(IN t_spi_device_id spi_device_id)
{
    DBGENTER0();

    g_spi_system_context[spi_device_id].device_context[0] = g_spi_system_context[spi_device_id].p_spi_register->spi_cr0;
    g_spi_system_context[spi_device_id].device_context[1] = g_spi_system_context[spi_device_id].p_spi_register->spi_cr1;
    g_spi_system_context[spi_device_id].device_context[2] = g_spi_system_context[spi_device_id].p_spi_register->spi_cpsr;
    g_spi_system_context[spi_device_id].device_context[3] = g_spi_system_context[spi_device_id].p_spi_register->spi_imsc;
    g_spi_system_context[spi_device_id].device_context[4] = g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr;

    DBGEXIT0(SPI_OK);
}

/**********************************************************************************************/
/* NAME:	void SPI_RestoreDeviceContext()										     	 	  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine restore current of SPI hardware.								  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN			NONE											 	  					  	  */
/* OUT			NONE																		  */
/* RETURN:		void																		  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY:Reentrant														                  */

/**********************************************************************************************/
PUBLIC void SPI_RestoreDeviceContext(IN t_spi_device_id spi_device_id)
{
    DBGENTER0();

    g_spi_system_context[spi_device_id].p_spi_register->spi_cr0 = g_spi_system_context[spi_device_id].device_context[0];
    g_spi_system_context[spi_device_id].p_spi_register->spi_cpsr = g_spi_system_context[spi_device_id].device_context[2];
    g_spi_system_context[spi_device_id].p_spi_register->spi_cr1 = g_spi_system_context[spi_device_id].device_context[1];
    g_spi_system_context[spi_device_id].p_spi_register->spi_dmacr = g_spi_system_context[spi_device_id].device_context[4];
    g_spi_system_context[spi_device_id].p_spi_register->spi_imsc = g_spi_system_context[spi_device_id].device_context[3];

    DBGEXIT0(SPI_OK);
}

/****************************************************************************/
/* NAME:	 SPI_GetIRQSrcStatus()  										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :This function updates a <t_spi_interrupt_status> structure*/
/*				  according to the <t_spi_idIRQSrc> parameter. It allows to */
/* 				  store the reason of the interrupt within the 				*/
/* PARAMETERS																*/
/* IN            :irq_src  : irqsource number								*/
/*				  p_status :pointer to the structure to be updated			*/
/* INOUT         : None														*/
/* OUT           : None														*/
/* RETURN VALUE  : SPI_INVALID_PARAMETER: If any parameter is INVLAID		*/
/*				   SPI_OK: Otherwise										*/
/* TYPE          : Public													*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_spi_error SPI_GetIRQSrcStatus
(
    IN t_spi_device_id      spi_device_id,
    IN t_spi_irq_src        irq_src,
    OUT t_spi_irq_status    *p_status
)
{
    t_uint32    temp = NULL;

    DBGENTER3
    (
        "(Interrput Request: %lx, interrupt State: %x, Pending interrupt: %lx)",
        irq_src,
        p_status->irq_state,
        p_status->pending_irq
    );

    if (NULL == p_status)
    {
        DBGEXIT0(SPI_INVALID_PARAMETER);
        return(SPI_INVALID_PARAMETER);
    }

    /* To read the recieve over run interrupt */
    temp = ((g_spi_system_context[spi_device_id].p_spi_register->spi_mis) & SPI_MIS_MASK_RORMIS);

    /* To read the recieve time out interrupt */
    temp |= ((g_spi_system_context[spi_device_id].p_spi_register->spi_mis) & SPI_MIS_MASK_RTMIS);

    /* Receiver interrupt */
    temp |= ((g_spi_system_context[spi_device_id].p_spi_register->spi_mis) & SPI_MIS_MASK_RXMIS);

    /* Transmitter interrupt */
    temp |= (g_spi_system_context[spi_device_id].p_spi_register->spi_mis) & SPI_MIS_MASK_TXMIS;

    p_status->pending_irq = temp & irq_src;
    p_status->irq_state = SPI_IRQ_STATE_NEW;

    DBGEXIT0(SPI_OK);
    return(SPI_OK);
}

/****************************************************************************/
/* NAME:    SPI_TransmitData                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:Copies from the address given in parameter several elements  */
/*                to be transmitted to the SPI.                             */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :  spi_device_id: Device Id for Identification		                */
/*       data_origin_address: Address from where transmitted data is copied.*/
/*       elements_to_be_transfered: Number of elements to be transmitted.   */
/* OUT:        none                                                         */
/* RETURN:                                                                  */
/*    SPI_NON_AUTHORIZED_MODE:if the SPI's transmit part is                 */
/*                            configured in DMA mode.                       */
/*    SPI_INVALID_PARAMETER: if spi_device_id is invalid		            */
/*    SPI_OK  :       If everything goes fine.                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                */

/****************************************************************************/
PUBLIC t_spi_error SPI_TransmitData
(
    IN t_spi_device_id  spi_device_id,
    IN t_uint8          *data_origin_address,
    IN t_uint32         elements_to_be_transfered,
    IN t_spi_config     *p_tx_config_spi
)
{
    t_spi_error spi_error = SPI_OK;
    t_uint32    number_of_elements = elements_to_be_transfered;

    DBGENTER3(" (%x %lx %lx)", spi_device_id, data_origin_address, elements_to_be_transfered);
    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        spi_error = SPI_INVALID_PARAMETER;
        return(spi_error);
    }

    if (NULL == data_origin_address)
    {
        spi_error = SPI_INVALID_PARAMETER;
        DBGEXIT(spi_error);
        return(spi_error);
    }

    g_spi_system_context[spi_device_id].p_spi_config = p_tx_config_spi;

    switch (g_spi_system_context[spi_device_id].tx_com_mode)
    {
        case (SPI_DMA_MODE):
            {
                /* In DMA mode this function should not be called
			-------------------------------------------------*/
                DBGPRINT(DBGL_ERROR, " Function not authorized in DMA mode");
                spi_error = SPI_NON_AUTHORIZED_MODE;
                DBGEXIT(spi_error);
                break;
            }

        case (SPI_IT_MODE):
            {
                g_spi_system_context[spi_device_id].tx_status.p_it_data_flow = (t_uint8 *) data_origin_address;

                g_spi_system_context[spi_device_id].tx_status.it_element_num = number_of_elements;

                if (NULL == number_of_elements)
                {
                    g_spi_system_context[spi_device_id].tx_status.it_element_num = number_of_elements;
                }
                else
                {
                    g_spi_system_context[spi_device_id].p_spi_register->spi_imsc |= SPI_IMSC_MASK_TXIM;
                }
                break;
            }

        case (SPI_POLLING_MODE):
            g_spi_system_context[spi_device_id].tx_status.p_it_data_flow = ((t_uint8 *) data_origin_address);
            g_spi_system_context[spi_device_id].tx_status.it_element_num = number_of_elements;
            do
            {
                if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_TNF)
                {
                    switch (g_spi_system_context[spi_device_id].transfer_data_size)
                    {
                        case SPI_DATA_SIZE_BYTE:
                            g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint8 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                            spi_error = SPI_OK;
                            break;

                        case SPI_DATA_SIZE_HALFWORD:
                            g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint16 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                            spi_error = SPI_OK;
                            break;

                        case SPI_DATA_SIZE_WORD:
                            g_spi_system_context[spi_device_id].p_spi_register->spi_dr = *((t_uint32 *) g_spi_system_context[spi_device_id].tx_status.p_it_data_flow);
                            spi_error = SPI_OK;
                            break;

                        default:
                            spi_error = SPI_INVALID_PARAMETER;
                            break;
                    }
                }

                g_spi_system_context[spi_device_id].tx_status.it_element_num--;
                g_spi_system_context[spi_device_id].tx_status.p_it_data_flow += (t_uint8) g_spi_system_context[spi_device_id].transfer_data_size;
            } while
            (
                (g_spi_system_context[spi_device_id].tx_status.it_element_num != NULL)
            &&  (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_TNF)
            );

            break;

        default:
            {
                DBGPRINT(DBGL_ERROR, " Invalid mode !!");
                spi_error = SPI_NON_AUTHORIZED_MODE;
                break;
            }
    }   /*end of switch*/

    DBGEXIT(spi_error);
    return(spi_error);
}

/****************************************************************************/
/* NAME:    SPI_ReceiveData                                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Copies to the address given in parameter some elements   */
/*                received from the SPI.                                    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :         spi_device_id:        Device Id for Identification		    */
/*            data_dest_address:    Address where received data are copied. */
/*            elements_to_be_retrieved: Number of elements to be received.  */
/* OUT:        none                                                         */
/* RETURN:                                                                  */
/*    SPI_NON_AUTHORIZED_MODE:if the SPI's transmit part is                 */
/*                            configured in DMA mode.                       */
/*    SPI_INVALID_PARAMETER: if spi_device_id is invalid		            */
/*    SPI_OK  :       If everything goes fine.                              */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                */

/****************************************************************************/
PUBLIC t_spi_error SPI_ReceiveData
(
    IN t_spi_device_id  spi_device_id,
    IN t_uint8          *data_dest_address,
    IN t_uint32         elements_to_be_retrieved,
    IN t_spi_config     *p_rx_config_spi
)
{
    t_spi_error spi_error = SPI_OK;

    DBGENTER3(" (%x %lx %lx)", spi_device_id, data_dest_address, elements_to_be_retrieved);

    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        spi_error = SPI_INVALID_PARAMETER;
        return(spi_error);
    }

    if (NULL == data_dest_address)
    {
        spi_error = SPI_INVALID_PARAMETER;
        DBGEXIT(spi_error);
        return(spi_error);
    }

    g_spi_system_context[spi_device_id].p_spi_config = p_rx_config_spi;

    switch (g_spi_system_context[spi_device_id].rx_com_mode)
    {
        case (SPI_DMA_MODE):
            {
                /* Nothing to be done. Everything is performed by hardware.
			 ---------------------------------------------------------*/
                DBGPRINT(DBGL_ERROR, " Function not authorized in DMA mode");
                spi_error = SPI_NON_AUTHORIZED_MODE;
                DBGEXIT(spi_error);
                break;
            }

        case (SPI_IT_MODE):
            g_spi_system_context[spi_device_id].rx_status.p_it_data_flow = (t_uint8 *) data_dest_address;

            g_spi_system_context[spi_device_id].rx_status.it_element_num = elements_to_be_retrieved;

            if (NULL == elements_to_be_retrieved)
            {
                g_spi_system_context[spi_device_id].rx_status.it_element_num = elements_to_be_retrieved;
            }
            else
            {
                g_spi_system_context[spi_device_id].p_spi_register->spi_imsc |= SPI_IMSC_MASK_RXIM;
            }
            break;

        case (SPI_POLLING_MODE):
            g_spi_system_context[spi_device_id].rx_status.p_it_data_flow = ((t_uint8 *) data_dest_address);
            g_spi_system_context[spi_device_id].rx_status.it_element_num = elements_to_be_retrieved;

            do
            {
                if (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
                {
                    switch (g_spi_system_context[spi_device_id].transfer_data_size)
                    {
                        case SPI_DATA_SIZE_BYTE:
                            *((t_uint8 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint8) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                            spi_error = SPI_OK;
                            break;

                        case SPI_DATA_SIZE_HALFWORD:
                            *((t_uint16 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint16) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                            spi_error = SPI_OK;
                            break;

                        case SPI_DATA_SIZE_WORD:
                            *((t_uint32 *) g_spi_system_context[spi_device_id].rx_status.p_it_data_flow) = (t_uint32) g_spi_system_context[spi_device_id].p_spi_register->spi_dr;
                            spi_error = SPI_OK;
                            break;

                        default:
                            spi_error = SPI_INVALID_PARAMETER;
                            break;
                    }
                }

                g_spi_system_context[spi_device_id].rx_status.it_element_num--;
                g_spi_system_context[spi_device_id].rx_status.p_it_data_flow += (t_uint8) g_spi_system_context[spi_device_id].transfer_data_size;
            } while
            (
                (g_spi_system_context[spi_device_id].rx_status.it_element_num != NULL)
            &&  (g_spi_system_context[spi_device_id].p_spi_register->spi_sr & SPI_SR_MASK_RNE)
            );

            break;

        default:
            {
                /* by default DMA mode is assumed
			---------------------------------*/
                DBGPRINT(DBGL_ERROR, " Invalid mode !!");
                spi_error = SPI_NON_AUTHORIZED_MODE;
                DBGEXIT(spi_error);
                break;
            }
    }

    DBGEXIT(spi_error);
    return(spi_error);
}

/****************************************************************************/
/* NAME:        SPI_FilterProcessIRQSrc                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:this iterative routine allows to process the current xxx     */
/*            device interrupt sources identified through the               */
/*            p_status_structure .it processes the interrupt sources one by */
/*            one,updates the p_status_structure to keep an internal history*/
/*            and generates the events. This function must be called until  */
/*              all processing is not done or all events are not generated. */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :        t_spi_irq_status *p_status_structure  status of the interrupt*/
/* OUT :       t_spi_event   *p_event_id:current event that is handled by 	*/
/*                                the routine                               */
/*             t_spi_filter_mode filter_mode filter to selectively process  */
/*                the interrupt                                             */
/*                                                                          */
/* RETURN:        SPI_INTERNAL_EVENT(positive):It indicates that all the    */
/*            processing associated with p_status_structure has been        */
/*            done all the events are not yet processed  i.e all the        */
/*            events have not been acknowledged).                           */
/*                                                                          */
/*            SPI_NO_MORE_PENDING_EVENT(positive value):Indicate that       */
/*            all processing associated with the pstructure                 */
/*            (without filter option) is done .The inteupt source can be    */
/*                reenabled.                                                */
/*                                                                          */
/*              SPI_NO_MORE_FILTER_PENDING_EVENT(positive value) indicates  */
/*            that all the processing associated with p_status_structure    */
/*              with the filter option is done                              */
/*                                                                          */
/*            SPI_NO_PENDING_EVENT_ERROR :when there is no interrupt        */
/*                to process                                                */
/*            SPI_INVALID_PARAMETER: is returned if invalid device id is 	*/
/*				found. 														*/
/*                                                                          */
/*                                                                          */
/*NOTE         :Befor using this function ,it is necessasry to call         */
/*            SPI_GetIRQSrcStatus or SPI_IsIrqSrcActive functions to        */
/*            intialize p_status_structure structure.                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Re-Entrant                                               */
/* REENTRANCY ISSUES:Modifying global p_spi_system_context->spi_event_status*/
/* variable  																*/

/****************************************************************************/
PUBLIC t_spi_error SPI_FilterProcessIRQSrc
(
    IN t_spi_device_id      spi_device_id,
    IN t_spi_irq_status     *p_status_structure,
    OUT t_spi_event         *p_event_id,
    IN t_spi_filter_mode    filter_mode
)
{
    t_spi_error         spi_error = SPI_OK;
    t_spi_event         *p_local_event;
    t_spi_filter_mode   local_filter;

    DBGENTER2(" (%x %lx)", spi_device_id, filter_mode);
    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        spi_error = SPI_INVALID_PARAMETER;
        DBGEXIT(spi_error);
        return(spi_error);
    }

    if
    (
        SPI_IRQ_STATE_NEW == p_status_structure->irq_state
    &&  NULL == (p_status_structure->pending_irq & SPI_ENABLE_ALL_INTERRUPT)
    )
    {
        DBGEXIT0(SPI_NO_PENDING_EVENT_ERROR);
        return(SPI_NO_PENDING_EVENT_ERROR);
    }

    p_status_structure->irq_state = SPI_IRQ_STATE_OLD;
    p_local_event = &g_spi_system_context[spi_device_id].spi_event_status;

    /*No filter mode means only one interrupt is to be served
	-------------------------------------------------------*/
    if (SPI_NO_FILTER_MODE == filter_mode)
    {
        local_filter = p_status_structure->pending_irq;
        if (NULL == (local_filter & SPI_ENABLE_ALL_INTERRUPT))  /*if LFilter is not Zero then process interrupt*/
        {
            if (NULL == ((*p_local_event) & SPI_ENABLE_ALL_INTERRUPT))
            {
                DBGEXIT0(SPI_NO_MORE_PENDING_EVENT);
                return(SPI_NO_MORE_PENDING_EVENT);
            }
            else
            {
                DBGEXIT0(SPI_INTERNAL_EVENT);
                return(SPI_INTERNAL_EVENT);
            }
        }
    }
    else
    {
        /*Filter mode
		--------------*/
        local_filter = (p_status_structure->pending_irq & filter_mode);
        if (NULL == (local_filter & SPI_ENABLE_ALL_INTERRUPT))
        {
            spi_error = spi_checkevent(p_local_event, local_filter);
            if (SPI_OK != spi_error)
            {
                DBGEXIT0(spi_error);
                return(spi_error);
            }
        }
    }

    if ((local_filter & ((t_uint32) SPI_IRQ_SRC_RECEIVE)))
    {
        spi_error = spip_InterruptRecieveData(spi_device_id);
        if (SPI_OK != spi_error)
        {
            DBGEXIT0(spi_error);
            return(spi_error);
        }

        p_status_structure->pending_irq &= ~((t_uint32) SPI_IRQ_SRC_RECEIVE);

        /*passing information to user abt the info of interrupt
			that has been serviced
			-------------------------------------------------------*/
        *p_event_id = ((t_uint32) SPI_IRQ_SRC_RECEIVE);

        /*modifying global event
			---------------------------*/
        (*p_local_event) |= ((t_uint32) SPI_IRQ_SRC_RECEIVE);
    }
    else if (local_filter & ((t_uint32) SPI_IRQ_SRC_RECEIVE_OVERRUN))
    {
        /* The number of error contd. */
        g_spi_system_context[spi_device_id].rx_status.flow_error_nb = SPI_ERROR_OVERRUN;

        /* Clearing the serviced interrupt from the pending interrupts */
        p_status_structure->pending_irq &= (~((t_uint32) SPI_IRQ_SRC_RECEIVE_OVERRUN));

        /* Notifying the user of the current interrupt serviced */
        *p_event_id = ((t_uint32) SPI_IRQ_SRC_RECEIVE_OVERRUN);

        /* modifying global event */
        (*p_local_event) |= ((t_uint32) SPI_IRQ_SRC_RECEIVE_OVERRUN);

        /* Clearing the Reciever Over run Interrupt */
        SPI_SET_ICR_RORIC(g_spi_system_context[spi_device_id].p_spi_register->spi_icr, SPI_ENABLE);
    }
    else if (local_filter & ((t_uint32) SPI_IRQ_SRC_RECEIVE_TIMEOUT))
    {
        p_status_structure->pending_irq &= (~((t_uint32) SPI_IRQ_SRC_RECEIVE_TIMEOUT));

        /*passing information to user abt the info of interrupt 
		that has been serviced
		------------------------------------------------------*/
        *p_event_id = ((t_uint32) SPI_IRQ_SRC_RECEIVE_TIMEOUT);

        /*modifying global event
		---------------------------*/
        (*p_local_event) |= ((t_uint32) SPI_IRQ_SRC_RECEIVE_TIMEOUT);

        spip_InterruptRecieverTimeout(spi_device_id);
    }
    else if (local_filter & ((t_uint32) SPI_IRQ_SRC_TRANSMIT))
    {
        spi_error = spip_InterruptTransmitData(spi_device_id);
        if (SPI_OK != spi_error)
        {
            DBGEXIT0(spi_error);
            return(spi_error);
        }

        p_status_structure->pending_irq &= ~((t_uint32) SPI_IRQ_SRC_TRANSMIT);

        /*passing information to user abt the info of interrupt
			that has been serviced
			-------------------------------------------------------*/
        *p_event_id = ((t_uint32) SPI_IRQ_SRC_TRANSMIT);

        /*modifying global event
			---------------------------*/
        (*p_local_event) |= ((t_uint32) SPI_IRQ_SRC_TRANSMIT);
    }

    DBGEXIT0(SPI_REMAINING_PENDING_EVENTS);
    return(SPI_REMAINING_PENDING_EVENTS);
}

/****************************************************************************/
/* NAME:        SPI_IsIRQSrcActive                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    Determines whether the IRQ source in argument is active. */
/*                Updates too the interrupt status variable.                */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:            spi_irq_src:Identification of the event to be checked.    */
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
PUBLIC t_bool SPI_IsIRQSrcActive
(
    IN t_spi_device_id      spi_device_id,
    IN t_spi_irq_src        spi_irq_src,
    OUT t_spi_irq_status    *p_status_structure
)
{
    t_uint32    temp = NULL;
    t_uint32    temp_irq;

    DBGENTER3(" (%x %lx %lx)", spi_device_id, spi_irq_src, (t_uint32) p_status_structure);

    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        DBGEXIT(FALSE);
        return(FALSE);
    }

    /*if interrupt is pending
	-------------------------*/
    if
    (
        (
            spi_irq_src & (t_spi_irq_src)
                (
                    (t_uint32) SPI_IRQ_SRC_TRANSMIT |
                    (t_uint32) SPI_IRQ_SRC_RECEIVE |
                    (t_uint32) SPI_IRQ_SRC_RECEIVE_TIMEOUT |
                    (t_uint32) SPI_IRQ_SRC_RECEIVE_OVERRUN
                )
        ) != 0
    )
    {
        temp = SPI_GET_INTR(g_spi_system_context[spi_device_id].p_spi_register->spi_mis);
        temp &= spi_irq_src;
        temp_irq = p_status_structure->pending_irq;
        p_status_structure->pending_irq &= (~spi_irq_src);
        p_status_structure->pending_irq |= temp;

        if (temp_irq != p_status_structure->pending_irq)
        {
            p_status_structure->irq_state = SPI_IRQ_STATE_NEW;
        }
    }

    if (p_status_structure->pending_irq & spi_irq_src)
    {
        DBGEXIT2(SPI_OK, "(%s %lx )", "TRUE", spi_irq_src);
        return(TRUE);
    }
    else
    {
        DBGEXIT2(SPI_OK, "(%s %lx )", "FALSE", spi_irq_src);
        return(FALSE);
    }
}

/****************************************************************************/
/* NAME:        SPI_AcknowledgeEvent                                        */
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
/*     p_spi_system_context->spi_event_status global event status is being 	*/
/*		modified    														*/

/****************************************************************************/
PUBLIC void SPI_AcknowledgeEvent(IN t_spi_device_id spi_device_id, IN const t_spi_event *p_event_id)
{
    DBGENTER2(" (%x %lx)", spi_device_id, p_event_id);
    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        DBGEXIT(SPI_INVALID_PARAMETER);
        return;
    }

    g_spi_system_context[spi_device_id].spi_event_status = (g_spi_system_context[spi_device_id].spi_event_status &~((*p_event_id) & SPI_ENABLE_ALL_INTERRUPT));

    DBGEXIT0(SPI_OK);
}

#ifndef ST_8500_ED
/****************************************************************************/
/* NAME:        SPI_SlaveSelect                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: The fuction is used to select the slave connected to the    */
/*              master device											    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:         spi_device_id: The Master Device Id							*/
/*             t_uint8: The Slave that is to be enabled	            		*/
/* OUT:        None                                                         */
/* RETURN:     t_spi_error                                                  */
/* REMARK:                                                                  */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                */

/****************************************************************************/
PUBLIC t_spi_error SPI_SlaveSelect(IN t_spi_device_id spi_device_id, IN t_uint8 enable_slave)
{
    DBGENTER2(" (%x %lx)", spi_device_id, enable_slave);

    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        DBGEXIT(SPI_INVALID_PARAMETER);
        return(SPI_INVALID_PARAMETER);
    }

    if (enable_slave > SPI_MAX_NUMBER_SLAVES)
    {
        DBGEXIT(SPI_INVALID_PARAMETER);
        return(SPI_INVALID_PARAMETER);
    }

    g_spi_system_context[spi_device_id].p_spi_register->spi_cr0 |=
        (
            (~enable_slave & SPI_MASK_SLAVE_ENABLE) <<
            SHIFT_QUARTET4
        );

    DBGEXIT0(SPI_OK);
    return(SPI_OK);
}

/****************************************************************************/
/* NAME:        SPI_DelayFeedbackClock                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: The fuction is used to set the value of the delay in the    */
/*              feedback clock											    */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN:         spi_device_id:  The Master Device Id							*/
/*             t_spi_fbclkdel: The amount of delay in sampling              */
/* OUT:        None                                                         */
/* RETURN:     t_spi_error                                                  */
/* REMARK:                                                                  */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                */

/****************************************************************************/
PUBLIC t_spi_error SPI_DelayFeedbackClock(IN t_spi_device_id spi_device_id, IN t_spi_fbclkdel fbclkdel)
{
    DBGENTER2(" (%x %lx)", spi_device_id, fbclkdel);

    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        DBGEXIT(SPI_INVALID_PARAMETER);
        return(SPI_INVALID_PARAMETER);
    }

    g_spi_system_context[spi_device_id].p_spi_register->spi_cr1 |= ((t_uint32)(fbclkdel) << SHIFT_QUARTET3);

    DBGEXIT0(SPI_OK);
    return(SPI_OK);
}

#endif
