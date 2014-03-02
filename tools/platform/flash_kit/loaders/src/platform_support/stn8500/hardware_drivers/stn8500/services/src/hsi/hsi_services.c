/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the test environement	
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "memory_mapping.h"
#include "hsi_services.h"
#include "hsi.h"

/********* GLOBAL VARIABLES **************/
t_callback      g_hsi_callback[SER_NO_OF_CONTROLLERS] =
{
    { 0, 0 },
    { 0, 0 }
};


PRIVATE t_uint32 g_exception_status;

/**************************************************
*  PRIVATE FUNCTIONS DECLARATION                  *
**************************************************/
PRIVATE void SER_HSI_IntHandlerTx(t_uint32 irq);
PRIVATE void SER_HSI_IntHandlerRx(t_uint32 irq);

/****************************************************************************/
/* NAME:    SER_HSI_IntHandlerTx                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for HSI Transmitter     	*/
/* 																            */
/* PARAMETERS: t_uint32 irq                                                 */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PRIVATE void SER_HSI_IntHandlerTx(IN t_uint32 irq)
{
    t_hsi_error             hsi_error;
    t_hsi_irq_status		hsi_status;
    t_bool                  reenable = FALSE;
    t_hsi_event 			hsi_event;
    t_bool                  done = FALSE;
    t_hsi_irq_src	        tx_interrupt;
    t_gic_error				gic_error;
    t_ser_hsi_param			hsi_param;
    
    tx_interrupt = HSI_GetIRQSrc(HSI_DEVICE_ID_TX);

    HSI_GetIRQSrcStatus(tx_interrupt, &hsi_status);

    gic_error = GIC_DisableItLine(SER_HSI_TX_LINE);
    if (GIC_OK != gic_error)
    {
    	PRINT("GIC_DisableItLine failed for SER_HSI_TX_LINE, error = 0x%x\n", gic_error);
    	return;
    }
    
    
    do
    {
        hsi_error = HSI_FilterProcessIRQSrc(&hsi_status, &hsi_event, HSI_NO_FILTER_MODE);

        switch (hsi_error)
        {
				case HSI_NO_PENDING_EVENT_ERROR:
				    done = TRUE;
				    break;

				case HSI_NO_MORE_PENDING_EVENT:
				    reenable = TRUE;
				    done = TRUE;
				    break;	

				case HSI_NO_MORE_FILTER_PENDING_EVENT:
				    done = TRUE;
				    break;

				case HSI_REMAINING_PENDING_EVENTS:

					if (g_hsi_callback[SER_HSI_TX_INDEX].fct != NULL)
					{
					    hsi_param.event = hsi_event;
					    g_hsi_callback[SER_HSI_TX_INDEX].fct(g_hsi_callback[SER_HSI_TX_INDEX].param, &hsi_param);
					}

					HSI_AcknowledgeEvent(&hsi_event);
					break;

				case HSI_INTERNAL_EVENT:
				    break;
                case HSI_REQUEST_NOT_APPLICABLE:
                    done = TRUE;
                    break;
				default :
					break;
        }
    } while (!done);
    
    if (reenable)
    {
        gic_error = GIC_EnableItLine(SER_HSI_TX_LINE);
        if (GIC_OK != gic_error)
	    {
    		PRINT("GIC_EnableItLine failed for SER_HSI_TX_LINE, error = 0x%x\n", gic_error);
    		return;
	    }
	}
}

/****************************************************************************/
/* NAME:    SER_HSI_IntHandlerRx                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for HSI Receiver	     	*/
/* 																            */
/* PARAMETERS: t_uint32 irq                                                 */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PRIVATE void SER_HSI_IntHandlerRx(IN t_uint32 irq)
{
    t_hsi_error             hsi_error;
    t_hsi_irq_status  		hsi_status;
    t_bool                  reenable = FALSE;
    t_hsi_event				hsi_event;
    t_bool                  done = FALSE;
    t_hsi_irq_src	        rx_interrupt;
    t_gic_error				gic_error;
    t_ser_hsi_param			hsi_param;
    
    rx_interrupt = HSI_GetIRQSrc(HSI_DEVICE_ID_RX);
    HSI_GetIRQSrcStatus(rx_interrupt, &hsi_status);

    gic_error = GIC_DisableItLine(SER_HSI_RX_LINE);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_EnableItLine failed for SER_HSI_RX_LINE, error = 0x%x\n", gic_error);
    	return;
    }
    
    do
    {
        hsi_error = HSI_FilterProcessIRQSrc(&hsi_status, &hsi_event, HSI_NO_FILTER_MODE);

        switch (hsi_error)
        {
            case HSI_NO_PENDING_EVENT_ERROR:
                done = TRUE;
                break;

            case HSI_NO_MORE_PENDING_EVENT:
                reenable = TRUE;
				done = TRUE;
				break;

            case HSI_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case HSI_REMAINING_PENDING_EVENTS:

				switch (hsi_event.rx_ex_event)
				{
					case  HSI_EXCEP_EVENT_RX_OVR0:
					case  HSI_EXCEP_EVENT_RX_OVR1:
					case  HSI_EXCEP_EVENT_RX_OVR2:
					case  HSI_EXCEP_EVENT_RX_OVR3:
					case  HSI_EXCEP_EVENT_RX_OVR4:
					case  HSI_EXCEP_EVENT_RX_OVR5:
					case  HSI_EXCEP_EVENT_RX_OVR6:
					case  HSI_EXCEP_EVENT_RX_OVR7:
					case  HSI_EXCEP_EVENT_RX_BREAK: 
					case  HSI_EXCEP_EVENT_RX_OVERRUN:
					case  HSI_EXCEP_EVENT_RX_TIMEOUT:
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined __PEPS_8500)||(defined __PEPS_8500_V1))
					case HSI_EXCEP_EVENT_RX_PARITY:
#endif					
						g_exception_status = (t_uint32) hsi_event.rx_ex_event;
						break;
					default:
						break;
				} 

				if (g_hsi_callback[SER_HSI_RX_INDEX].fct != NULL)
				{
					hsi_param.event = hsi_event;
					g_hsi_callback[SER_HSI_RX_INDEX].fct(g_hsi_callback[SER_HSI_RX_INDEX].param, &hsi_param);
				}

				HSI_AcknowledgeEvent(&hsi_event);
				break;

            case HSI_INTERNAL_EVENT:
                break;
            case HSI_REQUEST_NOT_APPLICABLE:
                done = TRUE;
                break;
            default:
            	break;
        }
    } while (!done);
    
    if (reenable)
    {
        gic_error = GIC_EnableItLine(SER_HSI_RX_LINE);
        if (GIC_OK != gic_error)
        {
	        PRINT("GIC_EnableItLine failed for SER_HSI_RX_LINE, error = 0x%x\n", gic_error);
    		return;
        }
    }
}

/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_HSI_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize  hsi                             	*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                              */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_HSI_Init(IN t_uint8 default_ser_mask)
{
	t_hsi_error		hsi_error;
	t_gic_error		gic_error;
	t_gic_func_ptr  old_datum;
	

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	 
	/* Enable the PRCMU Clk for HSIT and HSIR */
	(*(t_uint32 volatile *)(PRCMU_REG_BASE_ADDR + SER_HSIT_PRCMU_OFFSET)) |= SER_HSIT_PRCMU_ENABLE; 
	(*(t_uint32 volatile *)(PRCMU_REG_BASE_ADDR + SER_HSIR_PRCMU_OFFSET)) |= SER_HSIR_PRCMU_ENABLE;
	
	
    (*(t_uint32 volatile *)(PRCC_2_CTRL_REG_BASE_ADDR)) |= SER_HSI_PRCC_PCKEN_ENABLE; 
    (*(t_uint32 volatile *)(PRCC_2_CTRL_REG_BASE_ADDR + SER_HSI_PRCC_KCKEN_OFFSET)) |= SER_HSI_PRCC_KCKEN_ENABLE;
    
#endif
	/* Set base address for transmitter*/
	hsi_error = HSI_Init(HSI_DEVICE_ID_TX, HSI_TX_REG_BASE_ADDR);
	if (HSI_OK != hsi_error)
	{
		PRINT("HSI_Init failed for transmitter, error = 0x%x", hsi_error);
		return;
	}
	/* Set base address for receiver*/
	hsi_error =	HSI_Init(HSI_DEVICE_ID_RX,HSI_RX_REG_BASE_ADDR);
	if (HSI_OK != hsi_error)
	{
		PRINT("HSI_Init failed for receiver, error = 0x%x", hsi_error);
		return;
	}

    HSI_SetBaseAddress(HSI_DEVICE_ID_RX,HSI_RX_REG_BASE_ADDR);
	HSI_SetBaseAddress(HSI_DEVICE_ID_TX,HSI_TX_REG_BASE_ADDR);

    gic_error = GIC_DisableItLine(SER_HSI_TX_LINE);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_DisableItLine failed for SER_HSI_TX_LINE, error = 0x%x\n", gic_error);
    	return;
    }
    
    /* Connect the transmit interrupt handler and enable the interrupt line */
    gic_error = GIC_ChangeDatum(SER_HSI_TX_LINE,SER_HSI_IntHandlerTx,&old_datum);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_ChangeDatum failed for SER_HSI_TX_LINE, error = 0x%x\n", gic_error);
    	return;
    }
    
    gic_error = GIC_EnableItLine(SER_HSI_TX_LINE);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_EnableItLine failed for SER_HSI_TX_LINE, error = 0x%x\n", gic_error);
    	return;
    }

    gic_error = GIC_DisableItLine(SER_HSI_RX_LINE);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_DisableItLine failed for SER_HSI_RX_LINE, error = 0x%x\n", gic_error);
    	return;
    }
    
    /* Connect the receive interrupt handler and enable the interrupt line */
    gic_error = GIC_ChangeDatum(SER_HSI_RX_LINE,SER_HSI_IntHandlerRx,&old_datum);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_ChangeDatum failed for SER_HSI_RX_LINE, error = 0x%x\n", gic_error);
    	return;
    }
    
    gic_error = GIC_EnableItLine(SER_HSI_RX_LINE);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_EnableItLine failed for SER_HSI_RX_LINE, error = 0x%x\n", gic_error);
    	return;
    }
}



/****************************************************************************/
/* NAME:    SER_HSI_RegisterCallback                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine registers the callback for HSIT or HSIR       	*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: HCL_OK                                                           */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC t_ser_error SER_HSI_RegisterCallback (IN t_hsi_device_id device_id, IN t_callback_fct callback_fct, IN void *param)
{
	if (NULL == param || NULL == callback_fct)
	{
		PRINT("NULL CALLBACK FUNCTION or void Param pointer\n");
		PRINT("Error HSI_SER: From SER_HSI_RegisterCallback\n");
		return SERVICE_FAILED;
	}
	
	g_hsi_callback[device_id].fct = callback_fct;
	g_hsi_callback[device_id].param = param;
    
    return SERVICE_OK ;
}

/****************************************************************************/
/* NAME:    SER_HSI_InstallDefaultHandler                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine installs the default IRQ handler for HSIT or   */
/*              HSIR        	                                            */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_HSI_InstallDefaultHandler(IN t_hsi_device_id device_id)
{
    t_gic_error 	gic_error;
    t_gic_line		irq_line;
    t_gic_func_ptr  old_datum;

    if (HSI_DEVICE_ID_INVALID == device_id)
    {
    	PRINT("Invalid HSI device id. Interrupt Hnadlers not reset to default\n");
    	return;
    }

    if (HSI_DEVICE_ID_TX == device_id)
    {
    	irq_line = SER_HSI_TX_LINE;
		gic_error = GIC_ChangeDatum(irq_line, SER_HSI_IntHandlerTx,&old_datum);
		PRINT("Change Gic Datum %d",gic_error);
    }
    else
    {
    	irq_line = SER_HSI_RX_LINE;
	    gic_error = GIC_ChangeDatum(irq_line, SER_HSI_IntHandlerRx,&old_datum);
	    PRINT("Change Gic Datum %d",gic_error);
    }

    gic_error = GIC_DisableItLine(GIC_ALL_LINES);   
    
    if (GIC_OK != gic_error)
    {
    	PRINT("GIC_ChangeDatum failed for 0x%x, error 0x%x\n", irq_line, gic_error);
    	return;
    }

    gic_error = GIC_EnableItLine(irq_line);
    if (GIC_OK != gic_error)
    {
	    PRINT("GIC_EnableItLine failed for 0x%x, error 0x%x\n", irq_line, gic_error);
	    return;
    }
}

/******************************************************************************/
/* NAME:    SER_HSI_WaitEnd                                                   */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION: This routine waits for completion of transmission or reception*/
/*                                                                            */
/* PARAMETERS: t_uint32                                                       */
/*                                                                            */
/* RETURN: void                                                               */
/*              															  */
/*----------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                             */
/******************************************************************************/

PUBLIC void SER_HSI_WaitEnd(IN t_hsi_device_id device_id, IN t_uint32 channel_no)
{
	t_hsi_error error =  HSI_OK;
	t_hsi_channel_status rx_ch_status = HSI_CHANNEL_STATUS_NO_TRANSFER;
	t_hsi_channel_status tx_ch_status = HSI_CHANNEL_STATUS_NO_TRANSFER;
	
	switch (device_id)
	{
		case HSI_DEVICE_ID_TX:
			while (HSI_CHANNEL_STATUS_TRANSFER_ENDED != tx_ch_status)
			{
				error = HSI_GetTxChannelStatus (channel_no, &tx_ch_status);
				if(HSI_OK != error)
				{
					return;
				}
			}
			break;

		case HSI_DEVICE_ID_RX:
			while (HSI_CHANNEL_STATUS_TRANSFER_ENDED != rx_ch_status)
			{
				error = HSI_GetRxChannelStatus(channel_no, &rx_ch_status);
		
				if (0 != g_exception_status)
				{
					PRINT("Exception Occured, Ex Register value %lx \n", g_exception_status);
					g_exception_status = 0;
					break;
				}
				if(HSI_OK != error)
				{
					return;
				}
			}
			break;
	 }
	 
	 PRINT("The Error is %d",error);
}
/****************************************************************************/
/* NAME:    SER_HSI_Close                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine resets  hsi                             		*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_HSI_Close(void)
{
    /* T0 Be Developed */
}

