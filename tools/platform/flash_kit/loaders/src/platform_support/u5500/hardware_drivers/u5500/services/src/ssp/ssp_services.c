/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This provides services for SSP initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/


#include "ssp_services.h"

volatile ser_ssp_context    g_ser_ssp_context[SER_NUM_SSP_INSTANCES];

t_uint32                    *gp_dest_buffer, *gp_src_buffer;

t_callback                  g_callback_ssp = { 0, 0 };


/****************************************************************************/
/* NAME:    SER_SSP_InterruptHandler                              	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for SSP   	          	*/
/* 																            */
/* PARAMETERS: 		t_uint32 irq		                                    */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_SSP_InterruptHandler(t_uint32 irq)
{
    t_uint32        int_status = 0;
    t_uint32        index = 0;
    t_ser_ssp_param ssp_param;
    t_ssp_error     error_ssp = SSP_OK;
    t_ssp_device_id ssp_device_id;
    

    int_status = SSP_GetIRQSrc(SSP_DEVICE_ID_0);
    if(0 == int_status)
    {
    	int_status = SSP_GetIRQSrc(SSP_DEVICE_ID_1);
    	if(0 == int_status)
    	{
    		PRINT("\nSSP Interrupt Glitch");
    		return;
    	}
    	else
    	{
    		ssp_device_id = SSP_DEVICE_ID_1;
        }
    }
    else
    {
    	ssp_device_id = SSP_DEVICE_ID_0;
    }

    if (SSP_IsPendingIRQSrc(ssp_device_id, SSP_IRQ_SRC_RECEIVE)) 
    {
        g_ser_ssp_context[ssp_device_id].rcv_flag = TRUE; 

		int_status |= SSP_IRQ_SRC_RECEIVE;

        SSP_DisableIRQSrc(ssp_device_id, SSP_IRQ_SRC_RECEIVE); 

        if (TRUE == g_ser_ssp_context[ssp_device_id].receive_it_mode)
        {
            while((index < g_ser_ssp_context[ssp_device_id].rx_trig_level) &&  (error_ssp == SSP_OK)
            &&  (g_ser_ssp_context[ssp_device_id].rx_index < g_ser_ssp_context[ssp_device_id].receive_size))
            {
                error_ssp = SSP_GetData(ssp_device_id,(t_uint32 *)g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow); 
                g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow++;
                if (SSP_OK == error_ssp)
                {
                    g_ser_ssp_context[ssp_device_id].rx_index++;
                    index++;
                }
            }
        }
    }

    if (SSP_IsPendingIRQSrc(ssp_device_id, SSP_IRQ_SRC_TRANSMIT))
    {
        g_ser_ssp_context[ssp_device_id].trans_flag = TRUE;
        
        int_status |= SSP_IRQ_SRC_TRANSMIT;

        SSP_DisableIRQSrc(ssp_device_id, SSP_IRQ_SRC_TRANSMIT);

        if (TRUE == g_ser_ssp_context[ssp_device_id].transmit_it_mode)
        {
            while
            (
                (index < g_ser_ssp_context[ssp_device_id].tx_trig_level)
            &&  (error_ssp == SSP_OK)
            &&  (g_ser_ssp_context[ssp_device_id].tx_index < g_ser_ssp_context[ssp_device_id].transmit_size)
            )
            {
                error_ssp = SSP_SetData(ssp_device_id,*g_ser_ssp_context[ssp_device_id].p_tx_it_data_flow); 
                g_ser_ssp_context[ssp_device_id].p_tx_it_data_flow++;
                if (SSP_OK == error_ssp)
                {
                    g_ser_ssp_context[ssp_device_id].tx_index++;
                    index++;
                }
            }
        }
    }

    if (SSP_IsPendingIRQSrc(ssp_device_id, SSP_IRQ_SRC_RECEIVE_TIMEOUT))
    {
        g_ser_ssp_context[ssp_device_id].rcv_timeout = TRUE;
        
        int_status |= SSP_IRQ_SRC_RECEIVE_TIMEOUT;

        SSP_DisableIRQSrc(ssp_device_id, SSP_IRQ_SRC_RECEIVE_TIMEOUT);

        if
        (
            ((g_ser_ssp_context[ssp_device_id].receive_size - g_ser_ssp_context[ssp_device_id].rx_index) < g_ser_ssp_context[ssp_device_id].rx_trig_level)
        &&  (TRUE == g_ser_ssp_context[ssp_device_id].receive_it_mode)
        )
        {
            while ((error_ssp == SSP_OK) && (g_ser_ssp_context[ssp_device_id].rx_index < g_ser_ssp_context[ssp_device_id].receive_size))
            {
                error_ssp = SSP_GetData(ssp_device_id,(t_uint32 *)g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow); 
                g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow++;
                if (SSP_OK == error_ssp)
                {
                    g_ser_ssp_context[ssp_device_id].rx_index++;
                    index++;
                }
            }
        }
    }

    if (SSP_IsPendingIRQSrc(ssp_device_id, SSP_IRQ_SRC_RECEIVE_OVERRUN))
    {
        g_ser_ssp_context[ssp_device_id].rcv_overrun = TRUE;
        
        int_status |= SSP_IRQ_SRC_RECEIVE_OVERRUN;
        
        PRINT("SSP_IRQ_SRC_RECEIVE_OVERRUN\n");

        SSP_DisableIRQSrc(ssp_device_id, SSP_IRQ_SRC_RECEIVE_OVERRUN);
        
       }

    if (g_callback_ssp.fct != 0)
    {
        ssp_param.irq_src = int_status;
        g_callback_ssp.fct(g_callback_ssp.param, &ssp_param);
    }
	
    /* If using Service Receive and Transmit API's */
    if ((TRUE == g_ser_ssp_context[ssp_device_id].transmit_it_mode) || (TRUE == g_ser_ssp_context[ssp_device_id].receive_it_mode))
    {
        if
        (
            (g_ser_ssp_context[ssp_device_id].rx_index == g_ser_ssp_context[ssp_device_id].receive_size)
        &&  (g_ser_ssp_context[ssp_device_id].tx_index == g_ser_ssp_context[ssp_device_id].transmit_size)
        )
        {
            SSP_DisableIRQSrc(ssp_device_id, 0x0F);
            g_ser_ssp_context[ssp_device_id].receive_it_mode = FALSE;
            g_ser_ssp_context[ssp_device_id].transmit_it_mode = FALSE;
            g_ser_ssp_context[ssp_device_id].rx_index = 0;
            g_ser_ssp_context[ssp_device_id].tx_index = 0;
            g_ser_ssp_context[ssp_device_id].rx_trig_level = 1;
            g_ser_ssp_context[ssp_device_id].tx_trig_level = 1;
            g_ser_ssp_context[ssp_device_id].transmit_size = 1;
            g_ser_ssp_context[ssp_device_id].receive_size = 1;

        }
        else
        {
            SSP_EnableIRQSrc(ssp_device_id, int_status);
        }
    }

	if(SSP_DEVICE_ID_0 == ssp_device_id)
		GIC_EnableItLine(GIC_SSP_0_LINE);
	else if(SSP_DEVICE_ID_1 == ssp_device_id)
		GIC_EnableItLine(GIC_SSP_1_LINE);
	
}


/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_SSP_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize ssp		                            */
/*                                                                          */
/* PARAMETERS: mask: For initializing a particular SSP instance             */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_SSP_Init(t_uint8 mask)
{
    t_gic_error    gic_error;
 #if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
    t_gpio_error   gpio_error;
 #endif
    t_gic_func_ptr old_datum;
    t_version           version;
    t_ssp_device_id ssp_device_id = SSP_DEVICE_ID_0;
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
    t_gpio_alt_function ssp_gpio_alt_func;
#ifndef ST_8500_ED
    /* Enable SSP0 and SSP1 */
    (*(t_uint32 volatile *)(PRCC_3_CTRL_REG_BASE_ADDR)) |= SER_SSP_PRCC_ENABLE; 
    (*(t_uint32 volatile *)(PRCC_3_CTRL_REG_BASE_ADDR + SER_SSP_PRCC_KCKEN_OFFSET)) |= SER_SSP_PRCC_ENABLE;
#endif
#endif

    

	switch(mask)
	{
		case INIT_SSP0:
		
			ssp_device_id = SSP_DEVICE_ID_0;
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
			ssp_gpio_alt_func = GPIO_ALT_SSP0;
#endif
            	if (SSP_OK == SSP_GetVersion(&version))
                {
                    PRINT(" Version %d.%d.%d ", version.version, version.major, version.minor);
                }
                else
                {
                    PRINT("\nError <E> SER_SSP:SER_SSP_Init:from SSP_GetVersion\n");
                    return;
                }
		    SSP_SetBaseAddress(SSP_DEVICE_ID_0, SSP_0_REG_BASE_ADDR);
		    
		    if(SSP_Init(SSP_DEVICE_ID_0, SSP_0_REG_BASE_ADDR) != SSP_OK) 
		    {
		    	PRINT("register map failed. ");
		    }
            /* This has to be done after GIC initialization */
            gic_error = GIC_ChangeDatum(GIC_SSP_0_LINE,SER_SSP_InterruptHandler, &old_datum);
            if (gic_error != GIC_OK)
            {
                PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
            }

            gic_error = GIC_EnableItLine(GIC_SSP_0_LINE);
            if (gic_error != GIC_OK)
            {
                PRINT("GIC Enabling SSP0 Line error - %d\n", gic_error);
            }
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
            gpio_error = GPIO_EnableAltFunction(ssp_gpio_alt_func);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
#endif
            
		break;

	
		case INIT_SSP1:
			ssp_device_id = SSP_DEVICE_ID_1;
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
			ssp_gpio_alt_func = GPIO_ALT_SSP1;
#endif
            if (SSP_OK == SSP_GetVersion(&version))
                {
                    PRINT(" Version %d.%d.%d ", version.version, version.major, version.minor);
                }
                else
                {
                    PRINT("\nError <E> SER_SSP:SER_SSP_Init:from SSP_GetVersion\n");
                    return;
                }

		    SSP_SetBaseAddress(SSP_DEVICE_ID_1, SSP_1_REG_BASE_ADDR);
		    
		    if(SSP_Init(SSP_DEVICE_ID_1, SSP_1_REG_BASE_ADDR) != SSP_OK) 
		    {
		    	PRINT("register map failed. ");
		    }
			/* This has to be done after GIC initialization */
            gic_error = GIC_ChangeDatum(GIC_SSP_1_LINE,SER_SSP_InterruptHandler, &old_datum);
            if (gic_error != GIC_OK)
            {
                PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
            }

            gic_error = GIC_EnableItLine(GIC_SSP_1_LINE);
            if (gic_error != GIC_OK)
            {
                PRINT("GIC Enabling SSP1 Line error - %d\n", gic_error);
            }
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
            gpio_error = GPIO_EnableAltFunction(ssp_gpio_alt_func);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
#endif
            
		break;
	}

    

    g_ser_ssp_context[ssp_device_id].receive_it_mode = FALSE;
    g_ser_ssp_context[ssp_device_id].transmit_it_mode = FALSE;
    g_ser_ssp_context[ssp_device_id].rx_index = 0;
    g_ser_ssp_context[ssp_device_id].tx_index = 0;
    g_ser_ssp_context[ssp_device_id].rx_trig_level = 1;
    g_ser_ssp_context[ssp_device_id].tx_trig_level = 1;
    g_ser_ssp_context[ssp_device_id].transmit_size = 1;
    g_ser_ssp_context[ssp_device_id].receive_size = 1;
}

/************************************************************************************/
/* NAME:    SER_SSP_RegisterCallback                                          		*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine binds an call back for extended interrupt processing   */
/*                                                                          		*/
/* PARAMETERS:  t_callback_fct fct : Address of the call back                 		*/
/*                                                                          		*/
/* RETURN: void                                                             		*/
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                   		*/

/************************************************************************************/
PUBLIC void SER_SSP_RegisterCallback(t_callback_fct fct, void *param)
{
    g_callback_ssp.fct = fct;
    g_callback_ssp.param = param;
}

/************************************************************************************/
/* NAME:    SER_SSP_WaitEnd	                                            	    	*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine polls on the passed interrupt source					*/
/* PARAMETERS: t_ssp_irq_src_id: Interrupt Sources ORed                            	*/
/* RETURN: t_uint32: Active Interrupt source from the passed interrupt sources      */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                   		*/
/************************************************************************************/
PUBLIC t_uint32 SER_SSP_WaitEnd(t_ssp_device_id ssp_device_id, t_uint32 irq_src)
{
    t_uint32    active_irq_src = 0x0;

    while
    (
        (
            ((irq_src & SSP_IRQ_SRC_RECEIVE) && (g_ser_ssp_context[ssp_device_id].rcv_flag))
        ||  ((irq_src & SSP_IRQ_SRC_TRANSMIT) && (g_ser_ssp_context[ssp_device_id].trans_flag))
        ||  ((irq_src & SSP_IRQ_SRC_RECEIVE_TIMEOUT) && (g_ser_ssp_context[ssp_device_id].rcv_timeout))
        ||  ((irq_src & SSP_IRQ_SRC_RECEIVE_OVERRUN) && (g_ser_ssp_context[ssp_device_id].rcv_overrun))
        ) == FALSE
    )
        ;

    if (g_ser_ssp_context[ssp_device_id].rcv_flag)
    {
        active_irq_src |= SSP_IRQ_SRC_RECEIVE;
        g_ser_ssp_context[ssp_device_id].rcv_flag = FALSE;
    }

    if (g_ser_ssp_context[ssp_device_id].trans_flag)
    {
        active_irq_src |= SSP_IRQ_SRC_TRANSMIT;
        g_ser_ssp_context[ssp_device_id].trans_flag = FALSE;
    }

    if (g_ser_ssp_context[ssp_device_id].rcv_timeout)
    {
        active_irq_src |= SSP_IRQ_SRC_RECEIVE_TIMEOUT;
        g_ser_ssp_context[ssp_device_id].rcv_timeout = FALSE;
    }

    if (g_ser_ssp_context[ssp_device_id].rcv_overrun)
    {
        active_irq_src |= SSP_IRQ_SRC_RECEIVE_OVERRUN;
        g_ser_ssp_context[ssp_device_id].rcv_overrun = FALSE;
    }

    return(active_irq_src);
}



/****************************************************************************/
/* NAME:    SER_SSP_Close               	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine close services for ssp			                */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                */
/****************************************************************************/
PUBLIC void SER_SSP_Close(void)
{
	t_uint8 idx;
	for(idx = 0; idx < 2; idx++)
	{
	    g_ser_ssp_context[idx].rcv_flag = FALSE;
	    g_ser_ssp_context[idx].trans_flag = FALSE;
	    g_ser_ssp_context[idx].rcv_timeout = FALSE;
	    g_ser_ssp_context[idx].rcv_overrun = FALSE;
	    g_ser_ssp_context[idx].receive_it_mode = FALSE;
	    g_ser_ssp_context[idx].transmit_it_mode = FALSE;
	    g_ser_ssp_context[idx].rx_index = 0;
	    g_ser_ssp_context[idx].tx_index = 0;
	    g_ser_ssp_context[idx].rx_trig_level = 1;
	    g_ser_ssp_context[idx].tx_trig_level = 1;
	    g_ser_ssp_context[idx].transmit_size = 1;
	    g_ser_ssp_context[idx].receive_size = 1;
	}
    SSP_Reset(SSP_DEVICE_ID_0);
    SSP_Reset(SSP_DEVICE_ID_1);
}


/**************************************************************************************************/
/* NAME:    SER_SSP_TransmitData                                                                  */
/*------------------------------------------------------------------------------------------------*/
/* DESCRIPTION:Copies from the address given in parameter several elements                        */
/*                to be transmitted to the SSP.                                                   */
/* PARAMETERS:                                                                                    */
/* IN :  ssp_device_id: Device Id for Identification		                                      */
/*       data_origin_address: Address from where transmitted data is copied.                      */
/*       elements_to_be_transfered: Number of elements to be transmitted.                         */
/* OUT:        none                                                                               */
/* RETURN:                                                                                        */
/*    SSP_NON_AUTHORIZED_MODE:if the SSP's transmit part is                                       */
/*                            configured in DMA mode.                                             */
/*    SSP_INVALID_PARAMETER: if ssp_device_id is invalid		                                  */
/*    SSP_OK  :       If everything goes fine.                                                    */
/*------------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                                      */
/**************************************************************************************************/
PUBLIC t_ssp_error SER_SSP_TransmitData(IN t_ssp_device_id      ssp_device_id,
                                        IN t_logical_address    data_origin_address,
                                        IN t_uint32             elements_to_be_transfered
                                        )
{
    t_ssp_error ssp_error = SSP_OK;
    t_ssp_mode  ssp_mode = SSP_UNINITIALZED_MODE;
    t_uint32    number_of_elements = elements_to_be_transfered;

    if ((t_uint32) ssp_device_id >= NUM_SSP_INSTANCES)
    {
        ssp_error = SSP_INVALID_PARAMETER;
        return(ssp_error);
    }

    if (NULL == data_origin_address)
    {
        ssp_error = SSP_INVALID_PARAMETER;
        
        return(ssp_error);
    }

	
    ssp_error = SSP_GetTxMode(ssp_device_id,&ssp_mode);
    if(SSP_OK != ssp_error)
    {
        
        return(ssp_error);
    }

    switch (ssp_mode)
    {
        case (SSP_DMA_MODE):
            {
                /* In DMA mode this function should not be called
			-------------------------------------------------*/
                ssp_error = SSP_NON_AUTHORIZED_MODE;
                
                break;
            }

        case (SSP_IT_MODE):
            {
                if (NULL == number_of_elements)
                {
                    g_ser_ssp_context[ssp_device_id].transmit_size = number_of_elements;
                }
                else
                {
                    g_ser_ssp_context[ssp_device_id].transmit_it_mode = TRUE;
                    g_ser_ssp_context[ssp_device_id].transmit_size = number_of_elements;
                    g_ser_ssp_context[ssp_device_id].p_tx_it_data_flow = (t_uint32 *) data_origin_address;
                    SSP_EnableIRQSrc(ssp_device_id, SSP_IRQ_SRC_TRANSMIT);
                }
                break;
            }

        case (SSP_POLLING_MODE):
            {       
                g_ser_ssp_context[ssp_device_id].p_tx_it_data_flow = ((t_uint32 *) data_origin_address);
                g_ser_ssp_context[ssp_device_id].transmit_size = number_of_elements;
                do
                {
                   ssp_error = SSP_SetData(ssp_device_id,*g_ser_ssp_context[ssp_device_id].p_tx_it_data_flow);
                   if(ssp_error != SSP_OK)
                   {
                       
                       return(ssp_error);
                   }
                   g_ser_ssp_context[ssp_device_id].transmit_size--;
                   g_ser_ssp_context[ssp_device_id].p_tx_it_data_flow++;

                } while((g_ser_ssp_context[ssp_device_id].transmit_size != NULL));

                g_ser_ssp_context[ssp_device_id].p_tx_it_data_flow = ((t_uint32 *) data_origin_address);
                break;
            }
        default:
            {
                ssp_error = SSP_NON_AUTHORIZED_MODE;
                break;
            }
    }   /*end of switch*/

    
    return(ssp_error);
}

/************************************************************************************/
/* NAME:    SER_SSP_ReceiveData                                                     */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION:    Copies to the address given in parameter some elements           */
/*                received from the SSP.                                            */
/*                                                                                  */
/* PARAMETERS:                                                                      */
/* IN :         ssp_device_id:        Device Id for Identification		            */
/*            data_dest_address:    Address where received data are copied.         */
/*            elements_to_be_retrieved: Number of elements to be received.          */
/* OUT:        none                                                                 */
/* RETURN:                                                                          */
/*    SSP_NON_AUTHORIZED_MODE:if the SSP's transmit part is                         */
/*                            configured in DMA mode.                               */
/*    SSP_INVALID_PARAMETER: if ssp_device_id is invalid		                    */
/*    SSP_OK  :       If everything goes fine.                                      */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                        */
/************************************************************************************/
PUBLIC t_ssp_error SER_SSP_ReceiveData(IN t_ssp_device_id      ssp_device_id,
                                       IN t_logical_address    data_dest_address,
                                       IN t_uint32             elements_to_be_retrieved
                                       )
{
    t_ssp_error ssp_error = SSP_OK;
    t_ssp_mode  ssp_device_mode = SSP_UNINITIALZED_MODE;
    
    
    if ((t_uint32) ssp_device_id >= NUM_SSP_INSTANCES)
    {
        ssp_error = SSP_INVALID_PARAMETER;
        return(ssp_error);
    }

    if (NULL == data_dest_address)
    {
        ssp_error = SSP_INVALID_PARAMETER;
        
        return(ssp_error);
    }

    ssp_error = SSP_GetRxMode(ssp_device_id,&ssp_device_mode);
    if(SSP_OK != ssp_error)
    {
        
        return(ssp_error);
    }

    switch (ssp_device_mode)
    {
        case (SSP_DMA_MODE):
            
                ssp_error = SSP_NON_AUTHORIZED_MODE;
                
                break;
            

        case (SSP_IT_MODE):
            
            if (NULL == elements_to_be_retrieved)
            {
                g_ser_ssp_context[ssp_device_id].receive_size = elements_to_be_retrieved;
            }
            else
            {
                g_ser_ssp_context[ssp_device_id].receive_it_mode = TRUE;
                g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow = (t_uint32 *) data_dest_address;
                g_ser_ssp_context[ssp_device_id].receive_size = elements_to_be_retrieved;
                SSP_EnableIRQSrc(ssp_device_id, SSP_IRQ_SRC_RECEIVE);
            }
            break;

        case (SSP_POLLING_MODE):

            g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow = (t_uint32 *) data_dest_address;
            g_ser_ssp_context[ssp_device_id].receive_size = elements_to_be_retrieved;

            do
            {
                ssp_error = SSP_GetData(ssp_device_id,(t_uint32 *)g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow);
                if(ssp_error != SSP_OK)
                   {
                       
                       return(ssp_error);
                   }
                             
                g_ser_ssp_context[ssp_device_id].receive_size--;
                g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow++;

            } while((g_ser_ssp_context[ssp_device_id].receive_size != NULL));

            g_ser_ssp_context[ssp_device_id].p_rx_it_data_flow = ((t_uint32 *) data_dest_address);

            break;

        default:
            {
                /* by default DMA mode is assumed
			---------------------------------*/
                ssp_error = SSP_NON_AUTHORIZED_MODE;
                
                break;
            }
    }

    
    return(ssp_error);
}

