/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This provides services for SPI initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/

#include "spi_services.h"


/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/

/* Defines for instances */
#define SER_NUM_SPI_INSTANCES	4

volatile ser_spi_context    g_ser_spi_context[SER_NUM_SPI_INSTANCES];

t_uint32                    *gp_dest_buffer, *gp_src_buffer;

t_callback                  g_callback_spi = { 0, 0 };


PUBLIC void SER_SPI0_InterruptHandler(IN t_uint32 not_used)
{
    SER_SPI_InterruptHandler(SPI_DEVICE_ID_0);
}

PUBLIC void SER_SPI1_InterruptHandler(IN t_uint32 not_used)
{
    SER_SPI_InterruptHandler(SPI_DEVICE_ID_1);
}

PUBLIC void SER_SPI2_InterruptHandler(IN t_uint32 not_used)
{
    SER_SPI_InterruptHandler(SPI_DEVICE_ID_2);
}

PUBLIC void SER_SPI3_InterruptHandler(IN t_uint32 not_used)
{
    SER_SPI_InterruptHandler(SPI_DEVICE_ID_3);
}


/****************************************************************************/
/* NAME:    SER_SPI_InterruptHandler                              	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for SPI   	          	*/
/* 																            */
/* PARAMETERS: 		t_uint32 irq		                                    */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
//RSU: Changed for supporting two instances
/****************************************************************************/
PUBLIC void SER_SPI_InterruptHandler(t_spi_device_id  spi_device_id)
{
    t_spi_irq_status irq_status = {SPI_IRQ_STATE_NEW,0,0};
    t_spi_irq_src    irq_src=0x00;
    t_spi_event      current_event;
    t_ser_spi_param  ser_spi_param;
    t_gic_error       gic_error;
    t_gic_line        gic_line = GIC_SPI_0_LINE;
    t_spi_error       spi_error; 
    t_bool              done = FALSE;

    volatile t_callback *p_callback_spi;    /*ptr to call back stucture*/

    switch(spi_device_id)
    {
        case SPI_DEVICE_ID_0:
            p_callback_spi = &g_ser_spi_context[spi_device_id].callback_spi;
            gic_line = GIC_SPI_0_LINE;
            irq_src = SPI_INTERRUPT_ALL;
		break;
	
		case SPI_DEVICE_ID_1:
			p_callback_spi = &g_ser_spi_context[spi_device_id].callback_spi;
            gic_line = GIC_SPI_1_LINE;
            irq_src = SPI_INTERRUPT_ALL;
		break;

        case SPI_DEVICE_ID_2:
			p_callback_spi = &g_ser_spi_context[spi_device_id].callback_spi;
            gic_line = GIC_SPI_2_LINE;
            irq_src = SPI_INTERRUPT_ALL;
		break;

        case SPI_DEVICE_ID_3:
			p_callback_spi = &g_ser_spi_context[spi_device_id].callback_spi;
            gic_line = GIC_SPI_3_LINE;
            irq_src = SPI_INTERRUPT_ALL;
 		break;
 		
 		default:
 			p_callback_spi = NULL;
 		break;
 			

    }
    
	  //p_callback_spi = p_callback_spi;
	  	    
      p_callback_spi = &g_ser_spi_context[spi_device_id].callback_spi;

      SPI_GetIRQSrcStatus(spi_device_id,irq_src, &irq_status);

      if(irq_status.pending_irq & 0x01)
      {
          g_ser_spi_context[spi_device_id].rcv_overrun = TRUE;
      }
      if(irq_status.pending_irq & 0x02)
      {
          g_ser_spi_context[spi_device_id].rcv_timeout = TRUE;
      }

      gic_error = GIC_DisableItLine(gic_line);
      if (GIC_OK != gic_error)
      {
          PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
          return;
      }

        
    do
    {
        spi_error = SPI_FilterProcessIRQSrc(spi_device_id,&irq_status, &current_event, SPI_NO_FILTER_MODE);
        switch (spi_error)
        {
            case SPI_NO_PENDING_EVENT_ERROR:
            case SPI_NO_MORE_PENDING_EVENT:
            case SPI_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case SPI_INTERNAL_EVENT:
                break;

            case SPI_REMAINING_PENDING_EVENTS:
                SPI_AcknowledgeEvent(spi_device_id,&current_event);
                if (p_callback_spi->fct != NULL)
                {
                    ser_spi_param.irq_status = irq_status;
                    ser_spi_param.current_event = current_event;
                    ser_spi_param.spi_error = spi_error;
                    p_callback_spi->fct(p_callback_spi->param, &ser_spi_param);
                }
                break;
          
        }
    } while (done != TRUE);

   	gic_error = GIC_EnableItLine(gic_line);
   	if (GIC_OK != gic_error)
      {
          PRINT("\nGIC error after GIC_AcknowledgeItLine  :: %d", gic_error);
          return;
      }
    
}


/****************************************************************************/
/* NAME:    SER_SPI_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize spi		                            */
/* PARAMETERS: mask: For initializing a particular SPI instance             */
/* RETURN: void                                                             */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_SPI_Init(t_uint8 mask)
{
    t_gic_error         gic_error;
    
    t_gic_func_ptr      old_datum;
    t_gic_func_ptr      interrupt_handler = NULL;
    t_version           version;
    t_logical_address   spi_base_address = NULL;
    t_spi_device_id     spi_device_id = SPI_DEVICE_ID_0;
    t_gic_line          gic_line = GIC_SPI_0_LINE;
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
    t_gpio_error        gpio_error;
    t_gpio_config 		gpio_config = 
    {
    	(t_gpio_mode)0,(t_gpio_direction)0,(t_gpio_trig)0,(t_gpio_debounce)0,(t_gpio_level)0
    };
    t_gpio_pin          gpio_pin;
#ifndef ST_8500_ED    
    /* Enable SPI3 */
    (*(t_uint32 volatile *)(PRCC_1_CTRL_REG_BASE_ADDR)) |= SER_SPI_3_PRCC_ENABLE; 
    (*(t_uint32 volatile *)(PRCC_1_CTRL_REG_BASE_ADDR + SER_SPI_PRCC_KCKEN_OFFSET)) |= SER_SPI_3_PRCC_ENABLE;
    
    /* Enable SPI0,SPI1 and SPI2 */
    (*(t_uint32 volatile *)(PRCC_2_CTRL_REG_BASE_ADDR)) |= SER_SPI_0_PRCC_ENABLE; 
    (*(t_uint32 volatile *)(PRCC_2_CTRL_REG_BASE_ADDR + SER_SPI_PRCC_KCKEN_OFFSET)) |= SER_SPI_0_PRCC_ENABLE;
#endif
#endif

	if (SPI_OK == SPI_GetVersion(&version))
    {
        PRINT(" Version %d.%d.%d ", version.version, version.major, version.minor);
    }
    else
    {
        PRINT("\nError <E> SER_SPI:SER_SPI_Init:from SPI_GetVersion\n");
        return;
    }

	switch(mask)
	{
		case INIT_SPI0:
			spi_device_id = SPI_DEVICE_ID_0;
            spi_base_address = SPI_0_REG_BASE_ADDR;
            gic_line = GIC_SPI_0_LINE;
            interrupt_handler = SER_SPI0_InterruptHandler;
#if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
            gpio_config.mode =  GPIO_MODE_ALT_FUNCTION_C;
            gpio_pin = GPIO_PIN_220;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_223;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_224;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_225;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            #endif
            
		break;
	
		case INIT_SPI1:
			spi_device_id = SPI_DEVICE_ID_1;
            spi_base_address = SPI_1_REG_BASE_ADDR;
            gic_line = GIC_SPI_1_LINE;
            interrupt_handler = SER_SPI1_InterruptHandler;
            #if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
            gpio_config.mode =  GPIO_MODE_ALT_FUNCTION_B;
            gpio_pin = GPIO_PIN_209;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_212;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_213;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_214;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            #endif
		break;

        case INIT_SPI2:
			spi_device_id = SPI_DEVICE_ID_2;
            spi_base_address = SPI_2_REG_BASE_ADDR;
            gic_line = GIC_SPI_2_LINE;
            interrupt_handler = SER_SPI2_InterruptHandler;
            #if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
            *(volatile t_uint32 *)(PRCMU_REG_BASE_ADDR + 0x138) |= 0x800000;
            gpio_config.mode =  GPIO_MODE_ALT_FUNCTION_C;
            gpio_pin = GPIO_PIN_215;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_216;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_217;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_218;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            #endif
		break;

        case INIT_SPI3:
			spi_device_id = SPI_DEVICE_ID_3;
            spi_base_address = SPI_3_REG_BASE_ADDR;
            gic_line = GIC_SPI_3_LINE;
            interrupt_handler = SER_SPI3_InterruptHandler;
            #if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
            gpio_config.mode =  GPIO_MODE_ALT_FUNCTION_B;
            gpio_pin = GPIO_PIN_29;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_30;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_31;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_32;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            #endif
 		break;
        default:
            spi_device_id = SPI_DEVICE_ID_0;
            spi_base_address = SPI_0_REG_BASE_ADDR;
            gic_line = GIC_SPI_0_LINE;
            interrupt_handler = SER_SPI0_InterruptHandler;
            #if ((!defined (__PEPS_8500)) && (!defined (__PEPS_8500_V1)) && (!defined (__PEPS_8500_V2)))
            gpio_config.mode =  GPIO_MODE_ALT_FUNCTION_C;
            gpio_pin = GPIO_PIN_220;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_223;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_224;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            gpio_pin = GPIO_PIN_225;
            gpio_error = GPIO_SetPinConfig(gpio_pin,gpio_config);
            if(gpio_error != GPIO_OK)PRINT("Error in GPIO Configuration");
            #endif
		break;


	}


        if (GIC_DisableItLine(gic_line) != GIC_OK)
        {
              PRINT("<E> SER_SPI:SER_SPI_Init:from GIC_DisableItLine\n");
              return;
        }

        SPI_SetBaseAddress(spi_device_id, spi_base_address);
		    
		if(SPI_Init(spi_device_id, spi_base_address) != SPI_OK) 
		{
		   	PRINT("register map failed. ");
		}
		/* This has to be done after GIC initialization */
        gic_error = GIC_ChangeDatum(gic_line,interrupt_handler,&old_datum);
        if (gic_error != GIC_OK)
        {
            PRINT("GIC Interrupt Handler Binding error - %d\n", gic_error);
        }

        gic_error = GIC_EnableItLine(gic_line);
        if (gic_error != GIC_OK)
        {
             PRINT("GIC Enabling SPI1 Line error - %d\n", gic_error);
        }

    

    g_ser_spi_context[spi_device_id].receive_it_mode = FALSE;
    g_ser_spi_context[spi_device_id].transmit_it_mode = FALSE;
    g_ser_spi_context[spi_device_id].rx_index = 0;
    g_ser_spi_context[spi_device_id].tx_index = 0;
    g_ser_spi_context[spi_device_id].rx_trig_level = 1;
    g_ser_spi_context[spi_device_id].tx_trig_level = 1;
    g_ser_spi_context[spi_device_id].transmit_size = 1;
    g_ser_spi_context[spi_device_id].receive_size = 1;
}

/************************************************************************************/
/* NAME:    SER_SPI_RegisterCallback                                          		*/
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
PUBLIC void SER_SPI_RegisterCallback(t_callback_fct fct, void *param)
{
    g_callback_spi.fct = fct;
    g_callback_spi.param = param;
}

/************************************************************************************/
/* NAME:    SER_SPI_WaitEnd	                                            		*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine polls on the passed interrupt source					*/
/*                                                                          		*/
/* PARAMETERS: t_spi_irq_src_id: Interrupt Sources ORed                            	*/
/*                                                                          		*/
/* RETURN: t_uint32: Active Interrupt source from the passed interrupt sources      */
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                   		*/
//RSU: Support for 2 instances provided, device ID argument added
/************************************************************************************/
PUBLIC t_uint32 SER_SPI_WaitEnd(t_spi_device_id spi_device_id, t_uint32 irq_src)
{
    t_uint32    active_irq_src = NULL;

    while
    (
        (
            ((irq_src & SPI_IRQ_SRC_RECEIVE) && (g_ser_spi_context[spi_device_id].rcv_flag))
        ||  ((irq_src & SPI_IRQ_SRC_TRANSMIT) && (g_ser_spi_context[spi_device_id].trans_flag))
        ||  ((irq_src & SPI_IRQ_SRC_RECEIVE_TIMEOUT) && (g_ser_spi_context[spi_device_id].rcv_timeout))
        ||  ((irq_src & SPI_IRQ_SRC_RECEIVE_OVERRUN) && (g_ser_spi_context[spi_device_id].rcv_overrun))
        ) == FALSE
    )
        ;

    if (g_ser_spi_context[spi_device_id].rcv_flag)
    {
        active_irq_src |= SPI_IRQ_SRC_RECEIVE;
        g_ser_spi_context[spi_device_id].rcv_flag = FALSE;
    }

    if (g_ser_spi_context[spi_device_id].trans_flag)
    {
        active_irq_src |= SPI_IRQ_SRC_TRANSMIT;
        g_ser_spi_context[spi_device_id].trans_flag = FALSE;
    }

    if (g_ser_spi_context[spi_device_id].rcv_timeout)
    {
        active_irq_src |= SPI_IRQ_SRC_RECEIVE_TIMEOUT;
        g_ser_spi_context[spi_device_id].rcv_timeout = FALSE;
    }

    if (g_ser_spi_context[spi_device_id].rcv_overrun)
    {
        active_irq_src |= SPI_IRQ_SRC_RECEIVE_OVERRUN;
        g_ser_spi_context[spi_device_id].rcv_overrun = FALSE;
    }

    return(active_irq_src);
}



/****************************************************************************/
/* NAME:    SER_SPI_Close               	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine close services for spi			                */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                */
//RSU: Support for 2 instances provided, closes both instances of SPI
//RSU: Should it close both the instances? Yes
/****************************************************************************/
PUBLIC void SER_SPI_Close(void)
{
	t_uint8 idx;
	for(idx = NULL; idx < SER_NUM_SPI_INSTANCES; idx++)
	{
	    g_ser_spi_context[idx].rcv_flag = FALSE;
	    g_ser_spi_context[idx].trans_flag = FALSE;
	    g_ser_spi_context[idx].rcv_timeout = FALSE;
	    g_ser_spi_context[idx].rcv_overrun = FALSE;
	    g_ser_spi_context[idx].receive_it_mode = FALSE;
	    g_ser_spi_context[idx].transmit_it_mode = FALSE;
	    g_ser_spi_context[idx].rx_index = 0;
	    g_ser_spi_context[idx].tx_index = 0;
	    g_ser_spi_context[idx].rx_trig_level = 1;
	    g_ser_spi_context[idx].tx_trig_level = 1;
	    g_ser_spi_context[idx].transmit_size = 1;
	    g_ser_spi_context[idx].receive_size = 1;
	}
    SPI_Reset(SPI_DEVICE_ID_0);
    SPI_Reset(SPI_DEVICE_ID_1);
    SPI_Reset(SPI_DEVICE_ID_2);
    SPI_Reset(SPI_DEVICE_ID_3);
}


/**************************************************************************************************/
/* NAME:    SER_SPI_TransmitData                                                                  */
/*------------------------------------------------------------------------------------------------*/
/* DESCRIPTION:Copies from the address given in parameter several elements                        */
/*                to be transmitted to the SPI.                                                   */
/* PARAMETERS:                                                                                    */
/* IN :  spi_device_id: Device Id for Identification		                                      */
/*       data_origin_address: Address from where transmitted data is copied.                      */
/*       elements_to_be_transfered: Number of elements to be transmitted.                         */
/* OUT:        none                                                                               */
/* RETURN:                                                                                        */
/*    SPI_NON_AUTHORIZED_MODE:if the SPI's transmit part is                                       */
/*                            configured in DMA mode.                                             */
/*    SPI_INVALID_PARAMETER: if spi_device_id is invalid		                                  */
/*    SPI_OK  :       If everything goes fine.                                                    */
/*------------------------------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                                      */
/**************************************************************************************************/
PUBLIC t_spi_error SER_SPI_TransmitData(IN t_spi_device_id      spi_device_id,
                                        IN t_uint8              *data_origin_address,
                                        IN t_uint32             elements_to_be_transfered,
                                        IN t_spi_config         *tx_spi_config
                                        )
{
    t_spi_error spi_error = SPI_OK;
    t_uint32    number_of_elements = elements_to_be_transfered;

    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        spi_error = SPI_INVALID_PARAMETER;
        return(spi_error);
    }

    if (NULL == data_origin_address)
    {
        spi_error = SPI_INVALID_PARAMETER;
        return(spi_error);
    }

    g_ser_spi_context[spi_device_id].transmit_it_mode = TRUE;
    g_ser_spi_context[spi_device_id].trans_flag = TRUE;
    g_ser_spi_context[spi_device_id].transmit_size = number_of_elements;
    spi_error = SPI_TransmitData(spi_device_id,data_origin_address,elements_to_be_transfered,tx_spi_config);


    return(spi_error);
}

/************************************************************************************/
/* NAME:    SER_SPI_ReceiveData                                                     */
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION:    Copies to the address given in parameter some elements           */
/*                received from the SPI.                                            */
/*                                                                                  */
/* PARAMETERS:                                                                      */
/* IN :         spi_device_id:        Device Id for Identification		            */
/*            data_dest_address:    Address where received data are copied.         */
/*            elements_to_be_retrieved: Number of elements to be received.          */
/* OUT:        none                                                                 */
/* RETURN:                                                                          */
/*    SPI_NON_AUTHORIZED_MODE:if the SPI's transmit part is                         */
/*                            configured in DMA mode.                               */
/*    SPI_INVALID_PARAMETER: if spi_device_id is invalid		                    */
/*    SPI_OK  :       If everything goes fine.                                      */
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: Non-ReEntrant                                                        */
/************************************************************************************/
PUBLIC t_spi_error SER_SPI_ReceiveData(IN t_spi_device_id      spi_device_id,
                                       IN t_uint8              *data_dest_address,
                                       IN t_uint32             elements_to_be_retrieved,
                                       IN t_spi_config         *rx_spi_config
                                       )
{
    t_spi_error spi_error = SPI_OK;
    
    
    if ((t_uint32) spi_device_id >= NUM_SPI_INSTANCES)
    {
        spi_error = SPI_INVALID_PARAMETER;
        return(spi_error);
    }

    if (NULL == data_dest_address)
    {
        spi_error = SPI_INVALID_PARAMETER;
        return(spi_error);
    }

    g_ser_spi_context[spi_device_id].receive_it_mode = TRUE;
    g_ser_spi_context[spi_device_id].rcv_flag = TRUE;
    g_ser_spi_context[spi_device_id].receive_size = elements_to_be_retrieved;

    spi_error = SPI_ReceiveData(spi_device_id,data_dest_address,elements_to_be_retrieved,rx_spi_config);

    return(spi_error);
}



