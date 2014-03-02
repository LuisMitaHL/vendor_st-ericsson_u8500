/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides API routines for the I2C Controller
* \author  ST-Ericsson
*/
/*****************************************************************************/


#include "i2c.h"
#include "i2c_p.h"
#include "i2c_irqp.h"



/*---------------------------------------------------------------------------
 * defines
 *---------------------------------------------------------------------------*/
#ifdef __DEBUG
PRIVATE t_dbg_level myDebugLevel_I2C = DEBUG_LEVEL0;
PRIVATE t_dbg_id    myDebugID_I2C = I2C_HCL_DBG_ID;
#endif
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_I2C
#define MY_DEBUG_ID             myDebugID_I2C

#define I2C_ENDAD_COUNTER       25000
#define I2C_FIFO_FLUSH_COUNTER  10

/*-----------------------------------------------------------------------------
Global variables
-----------------------------------------------------------------------------*/

#if((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
    /* 8500 has 4 I2C Controllers */
    PRIVATE volatile t_i2c_system_context   g_i2c_system_context[4];
#else
    /* 8500 V1 has 5 I2C Controllers */
    PRIVATE volatile t_i2c_system_context   g_i2c_system_context[5];
#endif

/*-----------------------------------------------------------------------------
	Configuration functions
-----------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	I2C_Init												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Initialize the given I2C controller by specifying the   */
/*                  base logical address.                                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_logical_address : The controller's logical address      */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                	    */
/*                                                                          */
/* RETURN		: t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are invalid    */
/*                I2C_UNSUPPORTED_HW    if peripheral ids are not matched   */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_Init(IN t_i2c_device_id id, IN t_logical_address address)
{
    t_i2c_registers *p_i2c_registers;
    DBGENTER2("Id is %d, Address is %p", id, (t_uint32 *) address);

    /* 
	Check if the controller id is valid.
	*/
	if((NULL == address) ||(id > (t_i2c_device_id) I2C_MAX_ID))
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
	g_i2c_system_context[id].p_i2c_registers = (t_i2c_registers *) address;
	
    p_i2c_registers = (t_i2c_registers *) address;

    if
    (
        (I2C_P_ID_0 != p_i2c_registers->periph_id_0)
    ||  (I2C_P_ID_1 != p_i2c_registers->periph_id_1)
    ||  (I2C_P_ID_2 != p_i2c_registers->periph_id_2)
    ||  (I2C_P_ID_3 != p_i2c_registers->periph_id_3)
    ||  (I2C_CELL_ID_0 != p_i2c_registers->cell_id_0)
    ||  (I2C_CELL_ID_1 != p_i2c_registers->cell_id_1)
    ||  (I2C_CELL_ID_2 != p_i2c_registers->cell_id_2)
    ||  (I2C_CELL_ID_3 != p_i2c_registers->cell_id_3)
    )
    {
        return(I2C_UNSUPPORTED_HW);
    }        

    /*
	Initialize the right structure and save the base address.
	*/
   
    g_i2c_system_context[id].freq_scl = 0;
    g_i2c_system_context[id].freq_input = 0;
    g_i2c_system_context[id].mode = I2C_FREQ_MODE_STANDARD;
    g_i2c_system_context[id].own_address = 0;
    g_i2c_system_context[id].enabled = FALSE;
    g_i2c_system_context[id].slave_address = 0;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].data = 0;
    g_i2c_system_context[id].databuffer = NULL;
    g_i2c_system_context[id].count_data = 0;
    g_i2c_system_context[id].register_index = 0;
    g_i2c_system_context[id].operation = (t_i2c_operation) I2C_NO_OPERATION;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = FALSE;

    /*   g_i2c_system_context[id].i2c_device_context... to be initialized*/
    g_i2c_system_context[id].digital_filter_control = I2C_DIGITAL_FILTERS_OFF;
    g_i2c_system_context[id].dma_sync_logic_control = I2C_DISABLE;
    g_i2c_system_context[id].start_byte_procedure = I2C_DISABLE;
    g_i2c_system_context[id].slave_data_setup_time = 0; /* TBD */
    g_i2c_system_context[id].high_speed_master_code = 0;
    g_i2c_system_context[id].bus_control_mode = I2C_BUS_SLAVE_MODE;
    g_i2c_system_context[id].i2c_loopback_mode = I2C_DISABLE;
    g_i2c_system_context[id].general_call_mode_handling = I2C_NO_GENERAL_CALL_HANDLING;

    g_i2c_system_context[id].index_transfer_mode = I2C_TRANSFER_MODE_POLLING;
    g_i2c_system_context[id].data_transfer_mode = I2C_TRANSFER_MODE_POLLING;
    g_i2c_system_context[id].i2c_transmit_interrupt_threshold = 1;
    g_i2c_system_context[id].i2c_receive_interrupt_threshold = 1;
    g_i2c_system_context[id].transmit_burst_length = 0;
    g_i2c_system_context[id].receive_burst_length = 0;
    g_i2c_system_context[id].index_format = I2C_NO_INDEX;
    g_i2c_system_context[id].current_bus_config = I2C_CURRENT_BUS_SLAVE_TRANSMITTER;
    g_i2c_system_context[id].std                = FALSE;

    
    /*Disable the interrupts */
    HCL_WRITE_REG(p_i2c_registers->imscr,I2C_CLEAR);
    /* Disable the controller */

    HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_PE);    /* Hardware change in MOP500 V1. Clearing PE bit resets all registers acting as a sw reset */   
	
	DBGEXIT0(I2C_OK);
    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_SetDbgLevel								     		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Set the debug level for the HCL                         */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_dbg_level: The debug level to be configured for the HCL */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error											   	*/
/*                I2C_OK                 if it is ok                        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_SetDbgLevel(IN t_dbg_level debug_level)
{
    DBGENTER1("Setting Debug Level to %d", debug_level);

#ifdef __DEBUG
    myDebugLevel_I2C = debug_level;
    myDebugID_I2C = I2C_HCL_DBG_ID;
#endif
    DBGEXIT0(I2C_OK);
    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetVersion								     		*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Return the version of the current I2C HCL API           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: tNone                                                     */
/*     InOut    :  None                                                     */
/* 		OUT 	:  It will contain the current version of API          	    */
/*                                                                          */
/* RETURN		:	t_i2c_error											   	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetVersion(OUT t_version *p_version)
{
    if (NULL == p_version)
    {
        DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
    }

    p_version->version = I2C_HCL_VERSION_ID;
    p_version->major   = I2C_MAJOR_VERSION;
    p_version->minor   = I2C_MINOR_VERSION;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_SetDeviceConfiguration                  			*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Configure the given I2C controller, by clearing         */
/*                   registers, setting the input clock. The controller and */
/*                   interrupts are disabled after this routine             */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_i2c_device_config : pointer to the structer containg    */
/*                                      the configuration                   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not invalid*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_SetDeviceConfiguration(IN t_i2c_device_id id, IN t_i2c_device_config *p_device_config)
{
    t_i2c_error     error_status = I2C_OK;
    t_i2c_registers *p_i2c_registers;
	t_uint16 address; 
	t_uint32 cr = I2C_CLEAR; /*local copy of the register*/

    DBGENTER2("Id is %d, @ of device configuration is %p", id, (void *) p_device_config);

    /* Check if parameters are valid.*/
	if((NULL == p_device_config) ||(id > (t_i2c_device_id) I2C_MAX_ID))
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}

	/*Set Own Address*/
	address = p_device_config->controller_i2c_address;
	if (!i2c_AddressIsValid(address))
    {
        return(I2C_SLAVE_ADDRESS_NOT_VALID);
    }
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

 
    /* Save the value.*/
    g_i2c_system_context[id].enabled = FALSE;

    /* Now save the input parameters.*/
    g_i2c_system_context[id].digital_filter_control = p_device_config->i2c_digital_filter_control;
    g_i2c_system_context[id].slave_data_setup_time = p_device_config->slave_data_setup_time;
    g_i2c_system_context[id].dma_sync_logic_control = p_device_config->i2c_dma_sync_logic_control;
    g_i2c_system_context[id].start_byte_procedure = p_device_config->i2c_start_byte_procedure;
    g_i2c_system_context[id].high_speed_master_code = p_device_config->i2c_high_speed_master_code;
    g_i2c_system_context[id].freq_input = p_device_config->input_frequency;
    g_i2c_system_context[id].own_address = p_device_config->controller_i2c_address;

    /* Clear registers.*/
    HCL_WRITE_REG(p_i2c_registers->cr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->scr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->hsmcr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->tftr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->rftr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->dmar, I2C_CLEAR);
    
    if (address <= I2C_MAX_10_BIT_ADDRESS && address > I2C_MAX_7_BIT_ADDRESS)
    {
        /* Set Slave address mode to 10 bit addressing mode */
        HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_SAM);
    }
    else
    {
        /* Set the slave address mode to 7 bit addressing mode */
        HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_SAM);
    }
	
	/* Set the Address */ 
	I2C_WRITE_FIELD(p_i2c_registers->scr, I2C_SCR_ADDR, I2C_SCR_SHIFT_ADDR, address);

    /* set the  digital filter   */
    I2C_WRITE_FIELD
    (
        cr,
        I2C_CR_FON,
        I2C_CR_SHIFT_FON,
        (t_uint32) g_i2c_system_context[id].digital_filter_control
    );

    /* Set the DMA sync logic */
    I2C_WRITE_FIELD
    (
        cr,
        I2C_CR_DMA_SLE,
        I2C_CR_SHIFT_DMA_SLE,
        (t_uint32) g_i2c_system_context[id].dma_sync_logic_control
    );

    /* Set the Slave Data Set up Time */
    I2C_WRITE_FIELD
    (
        p_i2c_registers->scr,
        I2C_SCR_DATA_SETUP_TIME,
        I2C_SCR_SHIFT_DATA_SETUP_TIME,
        g_i2c_system_context[id].slave_data_setup_time
    );

    /* Disable generation of interrupts.*/
    I2C_DisableIRQSrc((t_i2c_irq_src_id) (((t_uint32) id << (t_uint32) I2CID_SHIFT) | (t_uint32) I2C_IRQ_SRC_ALL)); /* 
	                                            Single Interrupt source matches
	                                            the sequence of device ids they
	                                            are used interchangeably.
	                                            */
	
	p_i2c_registers->cr |= cr; 		 
    /* Enable the I2C Controller */
    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
        
    DBGEXIT0(error_status);
    return(error_status);

}

/****************************************************************************/
/* NAME			:	I2C_SetTransferConfiguration                  			*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Configure the given I2C controller for transfer mode,   */
/*                  baud rate, general call handling and bus access mode.   */
/*                  Additionally fifo levels, loopback control and DMA      */
/*                  burst length are also configured.           */
/*                  This routine enable the I2C controller                  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_i2c_transfer_config :pointer to the structure containing */
/*                                      the configuration                   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_UNSUPPORTED_FEATURE if required index and data        */
/*                  transfer modes are not supported.                       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_SetTransferConfiguration(IN t_i2c_device_id id, IN t_i2c_transfer_config *p_transfer_config)
{
    t_i2c_error     error_status = I2C_OK;
    t_i2c_registers *p_i2c_registers;

    DBGENTER2("Id is %d, config is %lx", id, (t_uint32) p_transfer_config);

    /* Check if parameters are valid.*/
	if((NULL == p_transfer_config) ||(id > (t_i2c_device_id) I2C_MAX_ID))
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}        

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;


    /*
    Error handling for unsopported features according to the platform
    */
    if
    (
        (
            I2C_TRANSFER_MODE_POLLING != p_transfer_config->index_transfer_mode
        &&  I2C_TRANSFER_MODE_POLLING == p_transfer_config->data_transfer_mode
        )
    ||  (    (I2C_TRANSFER_MODE_INTERRUPT == p_transfer_config->index_transfer_mode) 
          && (I2C_TRANSFER_MODE_DMA == p_transfer_config->data_transfer_mode)
        )
        
    ||  (I2C_TRANSFER_MODE_DMA == p_transfer_config->index_transfer_mode)
    )
    {
        DBGEXIT0(I2C_UNSUPPORTED_FEATURE);
        return(I2C_UNSUPPORTED_FEATURE);
    }

    /* 
        Clear all the existing state of the controller by clearing PE bit
    */

    /* Now save the input parameters.*/
    g_i2c_system_context[id].i2c_loopback_mode = p_transfer_config->i2c_loopback_mode;
    g_i2c_system_context[id].general_call_mode_handling = p_transfer_config->i2c_slave_general_call_mode;
    g_i2c_system_context[id].index_transfer_mode = p_transfer_config->index_transfer_mode;

    /*Index transfer mode is still relevant even if I2C_NO_INDEX is 
      used since then this mode is used for the address transmission */
      
    g_i2c_system_context[id].data_transfer_mode = p_transfer_config->data_transfer_mode;
    g_i2c_system_context[id].i2c_transmit_interrupt_threshold = p_transfer_config->i2c_transmit_interrupt_threshold;
    g_i2c_system_context[id].i2c_receive_interrupt_threshold = p_transfer_config->i2c_receive_interrupt_threshold;
    g_i2c_system_context[id].transmit_burst_length = p_transfer_config->transmit_burst_length;
    g_i2c_system_context[id].receive_burst_length = p_transfer_config->receive_burst_length;
    g_i2c_system_context[id].freq_scl = p_transfer_config->i2c_transfer_frequency;
    g_i2c_system_context[id].bus_control_mode = p_transfer_config->bus_control_mode;

    g_i2c_system_context[id].multi_operation = FALSE;
    g_i2c_system_context[id].register_index = 0;    /* The index of the slave's registers*/
    g_i2c_system_context[id].index_format = I2C_NO_INDEX;


    /* Set the SCL bus clock frequency. -> transfer frequency*/
    error_status = i2c_SetBusClock(id, g_i2c_system_context[id].freq_scl, g_i2c_system_context[id].freq_input);
    if (I2C_OK != error_status)
    {
        DBGEXIT0(error_status);
        return(error_status);
    }

    /*Set the loop back mode */
    I2C_WRITE_FIELD
    (
        p_i2c_registers->cr,
        I2C_CR_LM,
        I2C_CR_SHIFT_LM,
        (t_uint32) g_i2c_system_context[id].i2c_loopback_mode
    );

    /*  Enable the general call handing in the controller*/
    /*  Only possible general call handing in this controller is 
	    in the software mode.
	*/
	/*Setting the general call handling*/
    HCL_WRITE_BITS(p_i2c_registers->cr,(g_i2c_system_context[id].general_call_mode_handling >> 1),I2C_CR_SGCM);
    /* Disable the Tx DMA */
    HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_DMA_TX_EN);

    /* Disable the Rx DMA */
    HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_DMA_RX_EN);

    /* configure the Tx DMA burst size */
    if (g_i2c_system_context[id].transmit_burst_length >= 1)
    {
        /* set the DMA Tx request mode to Burst */
        HCL_SET_BITS(p_i2c_registers->dmar, I2C_DMA_BURST_TX);

        /* Set the Destination Burst Size */
        I2C_WRITE_FIELD
        (
            p_i2c_registers->dmar,
            I2C_DMA_DBSIZE_TX,
            I2C_DMA_SHIFT_DBSIZE_TX,
            g_i2c_system_context[id].transmit_burst_length
        );
    }
    else
    {
        /* Set the DMA Tx Request mode to Single */
        HCL_CLEAR_BITS(p_i2c_registers->dmar, I2C_DMA_BURST_TX);
    }

    /* configure the Rx DMA burst size */
    if (g_i2c_system_context[id].receive_burst_length >= 1)
    {
        /* set the DMA Rx request mode to Burst */
        HCL_SET_BITS(p_i2c_registers->dmar, I2C_DMA_BURST_RX);

        /* Set the source burst size */
        I2C_WRITE_FIELD
        (
            p_i2c_registers->dmar,
            I2C_DMA_SBSIZE_RX,
            I2C_DMA_SHIFT_SBSIZE_RX,
            g_i2c_system_context[id].receive_burst_length
        );
    }
    else
    {
        /* Set the DMA Rx Request mode to Single */
        HCL_CLEAR_BITS(p_i2c_registers->dmar, I2C_DMA_BURST_RX);
    }

    /* Set the Bus control mode */
    I2C_WRITE_FIELD
    (
        p_i2c_registers->cr,
        I2C_CR_OM,
        I2C_CR_SHIFT_OM,
        (t_uint32) g_i2c_system_context[id].bus_control_mode
    );

    /* Set the Transmit Fifo threshold value */
    p_i2c_registers->tftr = g_i2c_system_context[id].i2c_transmit_interrupt_threshold;

    /* Set the Receive Fifo Threshold value */
    p_i2c_registers->rftr = g_i2c_system_context[id].i2c_receive_interrupt_threshold;

    /*Disable the interrupts if index transfer mode is polling */
    if (I2C_TRANSFER_MODE_POLLING == g_i2c_system_context[id].index_transfer_mode)
    {
        I2C_DisableIRQSrc((t_i2c_irq_src_id) (((t_uint32) id << (t_uint32) I2CID_SHIFT) | (t_uint32) I2C_IRQ_SRC_ALL));
    }

    /* Now restore CR register with the values it has before it was disabled.*/
    /*Enable the I2C Controller and set the enable status in the global structure */
    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
    g_i2c_system_context[id].enabled = TRUE;
        
    DBGEXIT0(error_status);
    return(error_status);

}

/****************************************************************************/
/* NAME			:	I2C_SetTransferMode                  			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Set the transfer modes for the index and data transfer  */
/*                   on the given I2C controller                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_i2c_transfer_mode : Index transfer mode                 */
/*                t_i2c_transfer_mode : data  transger mode                 */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_UNSUPPORTED_FEATURE if required index and data        */
/*                  transfer modes are not supported.                       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_SetTransferMode
(
    IN t_i2c_device_id      id,
    IN t_i2c_transfer_mode  index_transfer_mode,
    IN t_i2c_transfer_mode  data_transfer_mode
)
{
    DBGENTER3
    (
        "Id is %d, index  and data transfer modes are %lx and %lx",
        id,
        (t_uint32) index_transfer_mode,
        (t_uint32) data_transfer_mode
    );
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}        

    if
    (
        (I2C_TRANSFER_MODE_POLLING != index_transfer_mode && I2C_TRANSFER_MODE_POLLING == data_transfer_mode)
    ||  (    (I2C_TRANSFER_MODE_INTERRUPT == index_transfer_mode) 
          && (I2C_TRANSFER_MODE_DMA == data_transfer_mode)
        )

    ||  (I2C_TRANSFER_MODE_DMA == index_transfer_mode)
    )
    {
        DBGEXIT0(I2C_UNSUPPORTED_FEATURE);
        return(I2C_UNSUPPORTED_FEATURE);
    }


    /* Check if parameters are valid.*/
       
    g_i2c_system_context[id].index_transfer_mode = index_transfer_mode;

    /*Index transfer mode is still relevant even if I2C_NO_INDEX is 
      used since then this mode is used for the addres transmission */
      
    g_i2c_system_context[id].data_transfer_mode = data_transfer_mode;

    /*Disable the interrupts if index tranfer mode is polling  */
    if (I2C_TRANSFER_MODE_POLLING == index_transfer_mode)
    {

        I2C_DisableIRQSrc((t_i2c_irq_src_id) (((t_uint32) id << (t_uint32) I2CID_SHIFT) | (t_uint32) I2C_IRQ_SRC_ALL));

    }
        

    DBGEXIT0(I2C_OK);
    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_SetBusControlMode                  			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Set the bus control mode for the data transfer on the   */
/*                given I2C controller.                                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_i2c_bus_control_mode : The mode in which I2C bus        */
/*                                         is accessed                      */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_SetBusControlMode(IN t_i2c_device_id id, IN t_i2c_bus_control_mode bus_control_mode)
{


    t_i2c_registers *p_i2c_registers;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    DBGENTER2("Id is %d, bus control mode is %lx", id, (t_uint32) bus_control_mode);

    /* Check if parameters are valid.*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
		return(I2C_INVALID_PARAMETER);
	}   
    g_i2c_system_context[id].bus_control_mode = bus_control_mode;

    /* Set the Bus control mode */
    I2C_WRITE_FIELD
    (
        p_i2c_registers->cr,
        I2C_CR_OM,
        I2C_CR_SHIFT_OM,
        (t_uint32) g_i2c_system_context[id].bus_control_mode
    );

    /* Enable the I2C controller */
    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
        
    DBGEXIT0(I2C_OK);
    return(I2C_OK);
}

/*-----------------------------------------------------------------------------
	    Configuration functions
-----------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	I2C_FlushFifo                  			                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Flush the transmit or receive FIFO                      */ 
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_i2c_fifo :        FIFO to be flused                     */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*               = I2C_INVALID_PARAMETER  - if input id is wrong            */
/*               = I2C_HW_FAILED          - if FIFO flush bit is not reset  */
/*                            itself after setting. This could be happen if */
/*                            i2c clock frequency is not set                */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_FlushFifo(IN t_i2c_device_id id, IN t_i2c_fifo fifo)
{

    t_uint32        loop_counter;
    t_i2c_registers *p_i2c_registers;

    /* Check parameters valid */
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}   

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    switch (fifo)
    {
        case I2C_TRANSMIT_FIFO:
            HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FTX);   /* Flush the Tx Fifo */

            /*Wait till for the Tx Flush bit to reset */
            loop_counter = 0;
            while
            (
                I2C_READ_FIELD(p_i2c_registers->cr, I2C_CR_FTX, I2C_CR_SHIFT_FTX)
            &&  loop_counter < I2C_FIFO_FLUSH_COUNTER
            )
            {
                loop_counter++;
            }
            if (loop_counter >= I2C_FIFO_FLUSH_COUNTER)
            {
                return(I2C_HW_FAILED);
            }
            break;

        case I2C_RECEIVE_FIFO:
            HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FRX);   /* Flush the Rx Fifo */

            /* Wait till Rx flush bit to reset */
            loop_counter = 0;
            while
            (
                I2C_READ_FIELD(p_i2c_registers->cr, I2C_CR_FRX, I2C_CR_SHIFT_FRX)
            &&  loop_counter < I2C_FIFO_FLUSH_COUNTER
            )
            {
                loop_counter++;
            }
            if (loop_counter >= I2C_FIFO_FLUSH_COUNTER)
            {
                return(I2C_HW_FAILED);
            }
            break;
    }

    return(I2C_OK);

}

/*-----------------------------------------------------------------------------
	    Status functions
-----------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	I2C_GetInfo                  			                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Get information related to the current settings         */
/*                     of the given controller                              */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query               */
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_info: It will contain the current settings of       */
/*                             the controller                         	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetInfo(IN t_i2c_device_id id, OUT t_i2c_info *p_info)
{
    /* Check if parameters are valid.*/
	if((NULL == p_info)|| id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 

    p_info->base_address = (t_logical_address)g_i2c_system_context[id].p_i2c_registers;
    p_info->id = id;
    p_info->freq_mode = g_i2c_system_context[id].mode;
    p_info->is_enabled = g_i2c_system_context[id].enabled;
    p_info->bus_control_mode = g_i2c_system_context[id].bus_control_mode;
    p_info->current_bus_config = g_i2c_system_context[id].current_bus_config;
    p_info->general_call_handling = g_i2c_system_context[id].general_call_mode_handling;
    p_info->freq_scl = g_i2c_system_context[id].freq_scl;
    p_info->freq_input = g_i2c_system_context[id].freq_input;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetInputClock                  		     	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Get the current input clock frequency of the given      */
/*                  controller (Hz).                                        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query         		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_uint32 * : It will contain the current input            */
/*                            clock frequency (in Hz)                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetInputClock(IN t_i2c_device_id id, OUT t_uint32 *p_freq_input)
{
    /* Check if parameters are valid.*/
	if((NULL == p_freq_input) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
		return(I2C_INVALID_PARAMETER);
	} 
    *p_freq_input = (t_uint32) g_i2c_system_context[id].freq_input;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetBusClock                  		     	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Get the current bus clock frequency of the given        */
/*                  controller (Hz).                                        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query        		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_uint32 * : It will contain the current bus              */
/*                            clock frequency (in Hz)                	    */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetBusClock(IN t_i2c_device_id id, OUT t_uint32 *p_freq_scl)
{
    /* Check if parameters are valid.*/
	if((NULL == p_freq_scl) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 

    *p_freq_scl = g_i2c_system_context[id].freq_scl;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetEnabled                  		     	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Query the given I2C controller to check if it is enabled.  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query            	*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_bool * : It will contain TRUE if the controller is      */
/*                           enabled, FALSE otherwise                       */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetEnabled(IN t_i2c_device_id id, OUT t_bool *p_status)
{


    t_i2c_registers *p_i2c_registers;

    /* Check if parameters are valid.*/
	if( (NULL == p_status) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    *p_status = (t_bool) I2C_READ_FIELD(p_i2c_registers->cr, I2C_CR_PE, I2C_CR_SHIFT_PE);

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetDeviceConfiguration                  		    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Query the given I2C controller to check the device         */
/*               configuration.                                             */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query        		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_device_config * : The device configuration settings */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetDeviceConfiguration(IN t_i2c_device_id id, OUT t_i2c_device_config *p_device_config)
{
    /* Check if parameters are valid.*/
	if((NULL == p_device_config) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
    p_device_config->controller_i2c_address = g_i2c_system_context[id].own_address;
    p_device_config->input_frequency = g_i2c_system_context[id].freq_input;
    p_device_config->i2c_digital_filter_control = g_i2c_system_context[id].digital_filter_control;
    p_device_config->i2c_dma_sync_logic_control = g_i2c_system_context[id].dma_sync_logic_control;
    p_device_config->i2c_start_byte_procedure = g_i2c_system_context[id].start_byte_procedure;
    p_device_config->i2c_high_speed_master_code = g_i2c_system_context[id].high_speed_master_code;
    p_device_config->slave_data_setup_time = g_i2c_system_context[id].slave_data_setup_time;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetTransferConfiguration                  		    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Query the given I2C controller to check the Transfer       */
/*               configuration.                                             */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query      		    */
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_transfer_config * : The transfer configuration      */
/*                                          settings                        */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetTransferConfiguration(IN t_i2c_device_id id, OUT t_i2c_transfer_config *p_transfer_config)
{
    /* Check if parameters are valid.*/
	if((NULL == p_transfer_config) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
    p_transfer_config->i2c_loopback_mode = g_i2c_system_context[id].i2c_loopback_mode;
    p_transfer_config->i2c_slave_general_call_mode = g_i2c_system_context[id].general_call_mode_handling;
    p_transfer_config->index_transfer_mode = g_i2c_system_context[id].index_transfer_mode;
    p_transfer_config->data_transfer_mode = g_i2c_system_context[id].data_transfer_mode;
    p_transfer_config->bus_control_mode = g_i2c_system_context[id].bus_control_mode;
    p_transfer_config->i2c_transmit_interrupt_threshold = g_i2c_system_context[id].i2c_transmit_interrupt_threshold;
    p_transfer_config->i2c_receive_interrupt_threshold = g_i2c_system_context[id].i2c_receive_interrupt_threshold;
    p_transfer_config->transmit_burst_length = g_i2c_system_context[id].transmit_burst_length;
    p_transfer_config->receive_burst_length = g_i2c_system_context[id].receive_burst_length;
    p_transfer_config->i2c_transfer_frequency = g_i2c_system_context[id].freq_scl;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetTransferMode                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Query the mode in which index and data is transferred.     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query       		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_transfer_mode  * : The mode used to transfer the    */
/*                              index in case of indexed operations         */
/*                t_i2c_transfer_mode  * : The mode used to transfer rest   */
/*                                         of the data                      */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetTransferMode
(
    IN t_i2c_device_id      id,
    OUT t_i2c_transfer_mode *p_index_transfer_mode,
    OUT t_i2c_transfer_mode *p_data_transfer_mode
)
{
    /* Check if parameters are valid.*/
	if((NULL == p_index_transfer_mode)||(NULL == p_data_transfer_mode) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 

    *p_index_transfer_mode = g_i2c_system_context[id].index_transfer_mode;
    *p_data_transfer_mode = g_i2c_system_context[id].data_transfer_mode;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetBusControlMode                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Query the bus control mode for the data transfer on        */
/*                the given I2C controller.                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to query	            */
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_current_bus_configuration  * : The mode in which    */
/*            the current transfer is being carried on the controller       */
/*                t_i2c_bus_control_mode  * : The mode configured for       */
/*                                        accessing the I2C bus             */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetBusControlMode
(
    IN t_i2c_device_id                  id,
    OUT t_i2c_current_bus_configuration *p_i2c_current_transfer_mode,
    OUT t_i2c_bus_control_mode          *p_bus_control_mode
)
{
    /* Check if parameters are valid.*/
	if((NULL == p_i2c_current_transfer_mode)||(NULL == p_bus_control_mode)||id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
    *p_i2c_current_transfer_mode = g_i2c_system_context[id].current_bus_config;
    *p_bus_control_mode = g_i2c_system_context[id].bus_control_mode;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_SendCommand                  		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine send the command to I2C controller. This     */
/*                routine set the appropriate bits based on the input      */
/*                command                                                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_i2c_command   : The command to be sent to the controller*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_UNSUPPORTED_FEATURE if requested command is not       */
/*                  supported by the platform.                              */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_SendCommand(IN t_i2c_device_id id, IN t_i2c_command command)
{

    t_i2c_registers *p_i2c_registers;

	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
	switch (command)
    {
        case I2C_COMMAND_SET_TRANSMIT_DMA:
        case I2C_COMMAND_CLEAR_TRANSMIT_DMA:
            HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_DMA_TX_EN);
            break;

        case I2C_COMMAND_SET_RECEIVE_DMA:
        case I2C_COMMAND_CLEAR_RECEIVE_DMA:
            HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_DMA_RX_EN);
            break;

        default:
			DBGEXIT0(I2C_UNSUPPORTED_FEATURE);
			return(I2C_UNSUPPORTED_FEATURE);
    }

    return(I2C_OK);

}

/****************************************************************************/
/* NAME			:	I2C_Enable                  		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Enable the given I2C controller.                           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_Enable(IN t_i2c_device_id id)
{
    t_i2c_registers *p_i2c_registers;

    /* Check if parameters are valid.*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
        
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);

    g_i2c_system_context[id].enabled = TRUE;
        
    
    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_Disable                  		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Disble the given I2C controller.                           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to disabled       		*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_Disable(IN t_i2c_device_id id)
{
    t_i2c_registers *p_i2c_registers;
	
    /* Check if parameters are valid.*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
        
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_PE);
	
	g_i2c_system_context[id].enabled = FALSE;
        
    return(I2C_OK);

}

#ifdef __I2C_ELEMENTARY
/*-----------------------------------------------------------------------------
        Operative functions
-----------------------------------------------------------------------------*/
/*ONLY ELEMENTARY LAYER*/

/****************************************************************************/
/* NAME			:	I2C_GetDeviceStatus                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Query the bus control mode for the data transfer on        */
/*                the given I2C controller.                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller id             		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_device_status  * : The mode in which                */
/*            the current transfer is being carried on the controller       */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_GetDeviceStatus(IN t_i2c_device_id id, OUT t_i2c_device_status *p_device_status)
{
    t_i2c_registers *p_i2c_registers;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    /* Check if parameters are valid.*/
	if((NULL == p_device_status) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}    

    *p_device_status = (t_i2c_device_status) I2C_READ_FIELD(p_i2c_registers->sr, I2C_SR_OP, I2C_SR_SHIFT_OP);


    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_WriteData                  		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:To write to the data register of the I2C controller.       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller id                  	*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_uint8 data:  The value to be written in the             */
/*                               I2C Data Register.                         */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_WriteData(IN t_i2c_device_id id, IN t_uint8 data)
{
    t_i2c_registers *p_i2c_registers;

    /* Check if parameters are valid.*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    p_i2c_registers->tfr = data;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_ReadData                  		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:To read from the data register of the I2C controller.      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller id             		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_uint8 *p_data:The Data read from the I2C Data register  */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_ReadData(IN t_i2c_device_id id, OUT t_uint8 *p_data)
{
    t_i2c_registers *p_i2c_registers;

    /* Check if parameters are valid.*/
	if((NULL == p_data) || id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    *p_data = (t_uint8) p_i2c_registers->rfr;

    return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_ConfigureSlaveAddress                  		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Set the slave address used to access the slave while       */
/*               in master mode                                             */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The address of the slave to be accessed      */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                                                                          */ 
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_ConfigureSlaveAddress(IN t_i2c_device_id id, IN t_uint16 i2c_slave_address)
{

    t_i2c_registers *p_i2c_registers;

    /* Check if parameters are valid.*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    I2C_WRITE_FIELD(p_i2c_registers->mcr, I2C_MCR_A10, I2C_MCR_SHIFT_A10, i2c_slave_address);

    /* Enable the I2C Controller */
    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
    return(I2C_OK);

}

/****************************************************************************/
/* NAME			:	I2C_ConfigureFrameLength                  		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Set the number of bytes to be transmitted through          */
/*               I2C  Controller                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The frame lenght                             */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_ConfigureFrameLength(IN t_i2c_device_id id, IN t_uint16 i2c_frame_length)
{
    t_i2c_registers *p_i2c_registers;

    /* Check if parameters are valid.*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    I2C_WRITE_FIELD(p_i2c_registers->mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, i2c_frame_length);

    /* Enable the I2C controller */
    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
    return(I2C_OK);

}

#elif (defined __I2C_ENHANCED)

/****************************************************************************/
/* NAME			:	I2C_WriteSingleData                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to write a single data byte to        */
/*                a receiver. Writing can be done to a slave device by      */
/*               using the indexed modes.                                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The address of the slave to be accessed      */
/*                t_i2c_index_format: The index of the register on the      */
/*                             receiver to which data is written            */
/*                t_unit16   :The format of the index on receiver side      */
/*                t_uint8 : The data byte to be written to the slave device */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_SLAVE_ADDRESS_NOT_VALID  If requested slave address   */
/*                                      is not valid                        */
/*                I2C_CONTROLLER_BUSY    if I2C controller is busy          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_WriteSingleData
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              data
)
{

/*
Steps:
    - Check Mode
        - Polling
        - Interrupt
        - DMA
*/

    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 			count = 1;  /*For single Data byte Transmission*/
	t_uint32 id_identifier = 0x00000000;
	t_i2c_irq_src_id irq_write;
		
    DBGENTER5
    (
        "Id is %d, Address is %x, Index format is %d and value is %d, Data is %d",
        id,
        slave_address,
        index_format,
        index_value,
        data
    );
        
    /* Check if parameters are valid.*/
    error_status = i2c_RWDataErrorCheck(id, slave_address, index_format);
    if (error_status != I2C_OK)
    {
        return(error_status);
    }
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    /* Save parameters.*/
    g_i2c_system_context[id].slave_address = slave_address;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].register_index = index_value;
    g_i2c_system_context[id].index_format = index_format;
    g_i2c_system_context[id].data = data;
    g_i2c_system_context[id].databuffer = NULL;
    g_i2c_system_context[id].count_data = 1;
    g_i2c_system_context[id].operation = I2C_WRITE;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = FALSE;
	
	/*Enable the Interrupts for Writing the Data*/
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	irq_write = (t_i2c_irq_src_id) (I2C_MULTIPLE_WRITE | id_identifier | I2C_IRQ_SRC_READ_FROM_SLAVE_EMPTY);
		
    /* Disable all the interrupts to remove previously garbage interrupts */
    i2c_RWDataDisableIRQ(id);
    
    
/*    I2C_DisableIRQSrc((I2C0 == id) ? I2C0_IRQ_SRC_ALL : I2C1_IRQ_SRC_ALL);*/

    /* Check if I2C controller is Master */
    i2c_WriteDataCheckMode(id, slave_address, index_format,count);
    

    switch (g_i2c_system_context[id].index_transfer_mode)
    {
        case I2C_TRANSFER_MODE_POLLING:
            /*  	
          Index Transfer
          */
            if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
            {
                error_status = i2c_SlaveIndexReceive(id);
                if (I2C_OK != error_status)
                {
                    DBGEXIT0(error_status);
                    return(error_status);
                }
            }
            else
            {
                error_status = i2c_MasterIndexTransmit(id);
                if (I2C_OK != error_status)
                {
                    DBGEXIT0(error_status);
                    return(error_status);
                }
            }

            /*
          Data Transfer
          */
            switch (g_i2c_system_context[id].data_transfer_mode)
            {
                case I2C_TRANSFER_MODE_POLLING:
                    error_status = i2c_TransmitDataPollingSingle(id, (t_uint8 *) &g_i2c_system_context[id].data);
                    if (I2C_OK != error_status)
                    {
                        DBGEXIT0(error_status);
                        return(error_status);
                    }

                    /* Stop Signal to be sent/received for transfer completion*/
                    break;

                case I2C_TRANSFER_MODE_INTERRUPT:
                    /* Enable the interrupts */
                    I2C_EnableIRQSrc(irq_write);
                    break;

                case I2C_TRANSFER_MODE_DMA:
                    /* Configure the DMA */
                    /* enable the DMA Tx interface */
                    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_DMA_TX_EN);
                    g_i2c_system_context[id].operation = I2C_NO_OPERATION;
                    break;

                default:
                    break;
            }
            break;

        case I2C_TRANSFER_MODE_INTERRUPT:
            /* enable the interrupts */
            i2c_WriteDataEnableIRQ(id);
            break;

        case I2C_TRANSFER_MODE_DMA:
        default:
            DBGEXIT0(I2C_INVALID_PARAMETER);
            return(I2C_INVALID_PARAMETER);
    }        

    DBGEXIT0(error_status);
    return(error_status);

}

/****************************************************************************/
/* NAME			:	I2C_WriteMultipleData                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to write a multiple data byte to      */
/*                a receiver. Writing can be done to a slave device by      */
/*               using the indexed modes.                                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The address of the slave to be accessed      */
/*                t_i2c_index_format: The index of the register on the      */
/*                             receiver to which data is written            */
/*                t_unit16   :The format of the index on receiver side      */
/*                t_uint8*   : The data buffer to be written to the         */
/*                           slave device                                   */
/*                t_unit32   : no of bytes to be transfered                 */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_SLAVE_ADDRESS_NOT_VALID  If requested slave address   */
/*                                      is not valid                        */
/*                I2C_CONTROLLER_BUSY    if I2C controller is busy          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_WriteMultipleData
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              *p_data,
    IN t_uint32             count
)
{

/*
Steps:
    - Check Mode
        - Polling
        - Interrupt
        - DMA
*/
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 id_identifier = 0x00000000;
	t_i2c_transfer_data data_type = I2C_MULTIPLE_BYTE_TRANSFER;
	t_i2c_irq_src_id irq_write;
	
    DBGENTER6
    (
        "Id is %d, Address is %x, Index format is %d and value is %d, Data count is %d and @ is %p",
        id,
        slave_address,
        index_format,
        index_value,
        count,
        (void *) p_data
    );    
	if(NULL == p_data)
	{
		error_status = I2C_INVALID_PARAMETER;
		DBGEXIT0(error_status);
        return(error_status);
	}

    /* Check if parameters are valid.*/
    error_status = i2c_RWDataErrorCheck(id, slave_address, index_format);
    if (error_status != I2C_OK)
    {
        return(error_status);
    }
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    /* Save parameters.*/
    g_i2c_system_context[id].slave_address = slave_address;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].register_index = index_value;
    g_i2c_system_context[id].index_format = index_format;
    g_i2c_system_context[id].data = 0;
    g_i2c_system_context[id].databuffer = p_data;
    g_i2c_system_context[id].count_data = count;
    g_i2c_system_context[id].operation = I2C_WRITE;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = TRUE;
	
	/*Enable the Interrupts for Writing the Data*/
	id_identifier = ((id_identifier | id) << I2CID_SHIFT); 
	irq_write = (t_i2c_irq_src_id) (I2C_MULTIPLE_WRITE | id_identifier );
	
    /* Disable all the interrupts to remove previously garbage interrupts */
    i2c_RWDataDisableIRQ(id);
    
    
    /*I2C_DisableIRQSrc((I2C0 == id) ? I2C0_IRQ_SRC_ALL : I2C1_IRQ_SRC_ALL);*/

    /* Check if I2C controller is Master */
    i2c_WriteDataCheckMode(id, slave_address, index_format, count);
    

    switch (g_i2c_system_context[id].index_transfer_mode)
    {
        case I2C_TRANSFER_MODE_POLLING:
            /*  	
         Index Transfer
         */
            if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
            {
                error_status = i2c_SlaveIndexReceive(id);                
            }
            else
            {
                error_status = i2c_MasterIndexTransmit(id);                
            }
            
            if (I2C_OK != error_status)
            {
                DBGEXIT0(error_status);
                return(error_status);
            }

            /*
         Data Transfer
         */
            switch (g_i2c_system_context[id].data_transfer_mode)
            {
                case I2C_TRANSFER_MODE_POLLING:
                    error_status = i2c_TransmitDataPolling(id, g_i2c_system_context[id].databuffer,data_type);
                    if (I2C_OK != error_status)
                    {
                        DBGEXIT0(error_status);
                        return(error_status);
                    }
                    break;

                case I2C_TRANSFER_MODE_INTERRUPT:
                    /* Enable the interrupts */
					I2C_EnableIRQSrc(irq_write);
                    break;

                case I2C_TRANSFER_MODE_DMA:
                    /* Configure the DMA */
                    /* enable the DMA Tx interface */
                    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_DMA_TX_EN);
                    g_i2c_system_context[id].operation = I2C_NO_OPERATION;
                    break;

                default:
                    break;
            }
            break;

        case I2C_TRANSFER_MODE_INTERRUPT:
            /* Enable the interrupts */
            i2c_WriteDataEnableIRQ(id);
            break;

        case I2C_TRANSFER_MODE_DMA:
        default:
            DBGEXIT0(I2C_INVALID_PARAMETER);
            return(I2C_INVALID_PARAMETER);
    }
        
    DBGEXIT0(error_status);
    return(error_status);

}

/****************************************************************************/
/* NAME			:	I2C_ReadSingleData                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to read a single  data byte from      */
/*                a transmitter. Read can be done from  a slave device by   */
/*               using the indexed modes.                                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The address of the slave to be accessed      */
/*                t_uint16   : The index of the register on the transmitter */
/*                              from which data is read                     */
/*                t_i2c_index_format :The format of the index on tranmitter */ 
/*									side									*/
/*                t_uint8    : The data  to be read from the tranmitter     */
/*                t_unit32   : no of bytes to be transfered                 */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_SLAVE_ADDRESS_NOT_VALID  If requested slave address   */
/*                                      is not valid                        */
/*                I2C_CONTROLLER_BUSY    if I2C controller is busy          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_ReadSingleData
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              *p_data
)
{

/*
Steps:
    - Check Mode
        - Polling
        - Interrupt
        - DMA
*/
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 count = 1; 		/*indicates a single byte transfer*/
	t_uint32 id_identifier = 0x00000000;
	t_i2c_irq_src_id irq_read;
    DBGENTER5
    (
        "Id is %d, Address is %x, Index format is %d and value is %d, Data count is %d and @ is %p",
        id,
        slave_address,
        index_format,
        index_value,
        (void *) p_data
    );
	if(NULL == p_data)
	{
		error_status = I2C_INVALID_PARAMETER;
		DBGEXIT0(error_status);
        return(error_status);
	}
    /* Check if parameters are valid.*/
    error_status = i2c_RWDataErrorCheck(id, slave_address, index_format);
    if (error_status != I2C_OK)
    {
        return(error_status);
    }
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    /* Save parameters.*/
    g_i2c_system_context[id].slave_address = slave_address;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].register_index = index_value;
    g_i2c_system_context[id].index_format = index_format;
    g_i2c_system_context[id].data = 0;
    g_i2c_system_context[id].databuffer = p_data;
    g_i2c_system_context[id].count_data = 1;
    g_i2c_system_context[id].operation = I2C_READ;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = FALSE;
	
	/*Interrupt Enabling for Read contains only values*/
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	irq_read = (t_i2c_irq_src_id) (I2C_MULTIPLE_READ | id_identifier );
	
    /* Disable all the interrupts to remove previously garbage interrupts */
    i2c_RWDataDisableIRQ(id);
    
    /* Check if I2C controller is Master */
    i2c_ReadDataCheckMode(id, slave_address, index_format,count);

    switch (g_i2c_system_context[id].index_transfer_mode)
    {
        case I2C_TRANSFER_MODE_POLLING:
            /*  	
         Index Transfer
         */
            if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
            {
                error_status = i2c_SlaveIndexReceive(id);                
            }
            else
            {
                error_status = i2c_MasterIndexTransmit(id);                
            }
            
            
            if (I2C_OK != error_status)
            {
                DBGEXIT0(error_status);
                return(error_status);
            }

            /*
         Data Transfer
         */
            switch (g_i2c_system_context[id].data_transfer_mode)
            {
                case I2C_TRANSFER_MODE_POLLING:
                    error_status = i2c_ReceiveDataPolling(id, g_i2c_system_context[id].databuffer);
                    if (I2C_OK != error_status)
                    {
                        DBGEXIT0(error_status);
                        return(error_status);
                    }

                    /* Stop Signal to be sent/received for transfer completion*/
                    break;

                case I2C_TRANSFER_MODE_INTERRUPT:
                    /* enable the interrupts */
					I2C_EnableIRQSrc(irq_read);
                    break;

                case I2C_TRANSFER_MODE_DMA:
                    /* Configure the DMA */
                    /* enable the DMA Rx interface */
                    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_DMA_RX_EN);
                    g_i2c_system_context[id].operation = I2C_NO_OPERATION;
                    break;

                default:
                    break;
            }
            break;

        case I2C_TRANSFER_MODE_INTERRUPT:
            /* Enable the interrupts */
            i2c_ReadDataEnableIRQ(id, index_format);
            break;

        case I2C_TRANSFER_MODE_DMA:
        default:
            DBGEXIT0(I2C_INVALID_PARAMETER);
            return(I2C_INVALID_PARAMETER);
    }
        
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME			:	I2C_ReadMultipleData                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to read a multiple  data byte from    */
/*                a transmitter. Read can be done from  a slave device by   */
/*               using the indexed modes.                                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The address of the slave to be accessed      */
/*                t_i2c_index_format  : The index of the register on the  	*/
/*                              transmitter from which data is read         */
/*                t_unit16   :The format of the index on tranmitter side    */
/*                t_uint8*   : The data buffer to be written to the         */
/*                           slave device                                   */
/*                t_unit32   : no of bytes to be transfered                 */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_SLAVE_ADDRESS_NOT_VALID  If requested slave address   */
/*                                      is not valid                        */
/*                I2C_CONTROLLER_BUSY    if I2C controller is busy          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_ReadMultipleData
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              *p_data,
    IN t_uint32             count
)
{

/*
Steps:
    - Check Mode
        - Polling
        - Interrupt
        - DMA
*/
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 id_identifier = 0x00000000;
	t_i2c_irq_src_id irq_read;
	
    DBGENTER6
    (
        "Id is %d, Address is %x, Index format is %d and value is %d, Data count is %d and @ is %p",
        id,
        slave_address,
        index_format,
        index_value,
        count,
        (void *) p_data
    );
        
	if(NULL == p_data)
	{
		error_status = I2C_INVALID_PARAMETER;
		DBGEXIT0(error_status);
        return(error_status);
	}
    /* Check if parameters are valid.*/
    error_status = i2c_RWDataErrorCheck(id, slave_address, index_format);
    if (error_status != I2C_OK)
    {
        return(error_status);
    }    
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    /* Save parameters.*/
    g_i2c_system_context[id].slave_address = slave_address;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].register_index = index_value;
    g_i2c_system_context[id].index_format = index_format;
    g_i2c_system_context[id].data = 0;
    g_i2c_system_context[id].databuffer = p_data;
    g_i2c_system_context[id].count_data = count;
    g_i2c_system_context[id].operation = I2C_READ;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = TRUE;
	
	/*Interrupt Enabling for Read contains only values*/
	id_identifier = ((id_identifier | id) << I2CID_SHIFT); 
	irq_read = (t_i2c_irq_src_id) (I2C_MULTIPLE_READ | id_identifier );

    /* Disable all the interrupts to remove previously garbage interrupts */
    i2c_RWDataDisableIRQ(id);
    
    /* Check if I2C controller is Master */
    i2c_ReadDataCheckMode(id, slave_address, index_format, count);

    switch (g_i2c_system_context[id].index_transfer_mode)
    {
        case I2C_TRANSFER_MODE_POLLING:
            /*  	
          Index Transfer
          */
            if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
            {
                error_status = i2c_SlaveIndexReceive(id);                
            }
            else
            {
                error_status = i2c_MasterIndexTransmit(id);                
            }
            
            
            if (I2C_OK != error_status)
            {
                DBGEXIT0(error_status);
                return(error_status);
            }

            /*
          Data Transfer
          */
            switch (g_i2c_system_context[id].data_transfer_mode)
            {
                case I2C_TRANSFER_MODE_POLLING:
                    error_status = i2c_ReceiveDataPolling(id, g_i2c_system_context[id].databuffer);
                    if (I2C_OK != error_status)
                    {
                        DBGEXIT0(error_status);
                        return(error_status);
                    }
                    break;

                case I2C_TRANSFER_MODE_INTERRUPT:
                    /* enable the interrupts */
					I2C_EnableIRQSrc(irq_read);
                    break;
                    
                case I2C_TRANSFER_MODE_DMA:
                    /* Configure the DMA */
                    /* enable the DMA Rx interface */
                    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_DMA_RX_EN);
                    g_i2c_system_context[id].operation = I2C_NO_OPERATION;
                    break;

                default:
                    break;
            }
            break;

        case I2C_TRANSFER_MODE_INTERRUPT:
            /* Enable the interrupts */
            i2c_ReadDataEnableIRQ(id, index_format);
            break;

        case I2C_TRANSFER_MODE_DMA:
        default:
            DBGEXIT0(I2C_INVALID_PARAMETER);
            return(I2C_INVALID_PARAMETER);
    }
        

    DBGEXIT0(error_status);
    return(error_status);

}

/****************************************************************************/
/* NAME			:	I2C_Cancel                  			                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine is used to cancel the current transfer     */
/*                  operation, if any.                                      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be canceled  		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_active_event: It will contain the result of         */
/*                                    the operation                         */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_Cancel(IN t_i2c_device_id id, INOUT t_i2c_active_event *event)   /*Only IT mode*/
{
    /* Check if parameters are valid.*/

	if((NULL == event)||(id > (t_i2c_device_id) I2C_MAX_ID))
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 

    if (I2C_NO_EVENT == g_i2c_system_context[id].active_event)
    {
        event->type = I2C_NO_EVENT;
        event->transfer_data = 0;
        event->id = id;
    }
    else
    {
        event->type = I2C_CANCEL_EVENT;
        event->transfer_data = g_i2c_system_context[id].transfer_data;
        event->id = id;
        g_i2c_system_context[id].active_event = I2C_CANCEL_EVENT;
		i2c_Abort(id);
    }
    
	return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_GetIRQSrcStatus                  			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Get the interrupt status associated with the given      */
/*                  interrupt source identifier.                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_src_id 	: Interrupt source identifier 		    */
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_irq_status: Status of the given interrupt source    */
/*                                  identifier                              */
/*                                                                          */
/* RETURN		:	None								    	            */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void I2C_GetIRQSrcStatus(IN t_i2c_irq_src_id id, OUT t_i2c_irq_status *status)
{
    /* Only to be compliant with HCL interrupt handling directives.*/

    * status = GETDEVICE((t_uint32) id);

}

/****************************************************************************/
/* NAME			:	I2C_ProcessIRQSrc                  			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine is used to process all the given           */
/*                  interrupt sources identified. It also updates the       */
/*                  interrupt status for the pending interrupts.            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: None                                          		    */
/*     InOut    :  None                                                     */
/* 		OUT 	: t_i2c_irq_status: The status of the interrupt source to   */
/*                                  be processed                            */
/*                                                                          */
/* RETURN		:	t_i2c_error								    	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_ProcessIRQSrc(INOUT t_i2c_irq_status *status)
{
    t_i2c_error         errStatus;
    t_i2c_active_event  idEvent;

    if (NULL == status)
    {
        DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
    }
        
    errStatus = i2c_ProcessIt(status, &idEvent);        
    
    return(errStatus);
}

/****************************************************************************/
/* NAME			:	I2C_FilterProcessIRQSrc                  		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine is used to process a single interrupt        */
/*                source identified and generate the associated event.      */
/*                It also updates the interrupt status for the pending      */
/*                interrupts.                                               */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: None                                          		    */
/*     InOut    : t_i2c_irq_status * : The status of interrupt source       */
/*                                     to be processed                      */
/* 		OUT 	: t_i2c_active_event* : Generated Event                     */
/*                                                                          */
/* RETURN		:	t_i2c_error : I2C error status            	            */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_FilterProcessIRQSrc(INOUT t_i2c_irq_status *status, OUT t_i2c_active_event *event)
{
    t_i2c_error errStatus;
    if ((NULL == status) || (NULL == event))
    {
        DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
    }
        
    errStatus = i2c_ProcessIt(status, event);        
    
    return(errStatus);
}

/****************************************************************************/
/* NAME			:	I2C_IsEventActive                  		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to determine if the given event       */
/*                is still active.                                          */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_active_event: the Event to be checked      		    */
/*     InOut    : None                                                      */
/* 		OUT 	: None                                                      */
/*                                                                          */
/* RETURN		:	t_bool  :   TRUE or FALSE               	            */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_bool I2C_IsEventActive(IN t_i2c_active_event *event)
{
    if (event->type == g_i2c_system_context[event->id].active_event)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/****************************************************************************/
/* NAME			:	I2C_AcknowledgeEvent                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to acknowledge that the given event   */
/*               has been processed.                                        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_active_event* : Generated Event            		    */
/*     InOut    : None                                                      */
/* 		OUT 	: None                                                      */
/*                                                                          */
/* RETURN		:	t_bool  :   TRUE or FALSE       	    	            */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_bool I2C_AcknowledgeEvent(IN t_i2c_active_event *event)
{
    if (event->type == g_i2c_system_context[event->id].active_event)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}
#endif

/*-----------------------------------------------------------------------------
		Power Management functions
-----------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	I2C_SaveDeviceContext                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Save the device context, by saving all configuration       */
/*               for a given controller to private data-structures          */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The ID of the controller for storing  */
/*                                     configuration       		            */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error  									    	*/
/*                  return  I2C_INVALID_PARAMETER  if id is not correct     */
/*                  else    I2C_OK									    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_SaveDeviceContext(IN t_i2c_device_id id)
{
	    
	t_i2c_registers *p_i2c_registers;
    
    if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}     
	p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	
    /* save the enabled inerrupts in the system context */
    g_i2c_system_context[id].i2c_device_context[0] = (t_uint32) p_i2c_registers->imscr;        
	return(I2C_OK);
}

/*-----------------------------------------------------------------------------
		Power Management functions
-----------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	I2C_RestoreDeviceContext              		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Restore the device context from the saved private          */
/*               data-structures for the given I2C controller               */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The ID of the controller for restoring*/
/*                                     configuration       		            */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error  									    	*/
/*                  return  I2C_INVALID_PARAMETER  if id is not correct     */
/*                  else    I2C_OK											*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error I2C_RestoreDeviceContext(IN t_i2c_device_id id)
{

    t_i2c_registers *p_i2c_registers;
    t_i2c_error     errStatus = I2C_OK;
    t_bool          prev_enabled = FALSE; 
	t_uint16 address =  (t_uint16) g_i2c_system_context[id].own_address;
	/*Check for the id*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	} 
	
	/* Set own address.*/
    if (!i2c_AddressIsValid(address))
    {
        return(I2C_SLAVE_ADDRESS_NOT_VALID);
    }
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    prev_enabled = g_i2c_system_context[id].enabled;

    
    /* Save the value.*/
    g_i2c_system_context[id].enabled = FALSE;

    /* Clear registers.*/
    HCL_WRITE_REG(p_i2c_registers->cr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->scr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->hsmcr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->mcr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->tftr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->rftr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->dmar, I2C_CLEAR);

    

    if (address < 1024 && address > 127)
    {
        /* Set Slave address mode to 10 bit addressing mode */
        HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_SAM);
        I2C_WRITE_FIELD(p_i2c_registers->scr, I2C_SCR_ADDR, I2C_SCR_SHIFT_ADDR, address);
    }
    else
    {
        /* Set the slave address mode to 7 bit addressing mode */
        HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_SAM);
        I2C_WRITE_FIELD(p_i2c_registers->scr, I2C_SCR_ADDR, I2C_SCR_SHIFT_ADDR, address);
    }
	
    /* Set the SCL bus clock frequency.*/
    errStatus = i2c_SetBusClock(id, g_i2c_system_context[id].freq_scl, g_i2c_system_context[id].freq_input);
    if (errStatus != I2C_OK)
    {
        DBGEXIT0(errStatus);
        return(errStatus);
    }

    if (prev_enabled)
    {
        /* set the  digital filter   */
        I2C_WRITE_FIELD
        (
            p_i2c_registers->cr,
            I2C_CR_FON,
            I2C_CR_SHIFT_FON,
            (t_uint32) g_i2c_system_context[id].digital_filter_control
        );

        /* set the Slave data setup time */
        I2C_WRITE_FIELD
        (
            p_i2c_registers->scr,
            I2C_SCR_DATA_SETUP_TIME,
            I2C_SCR_SHIFT_DATA_SETUP_TIME,
            g_i2c_system_context[id].slave_data_setup_time
        );

        /* Set the DMA sync logic */
        I2C_WRITE_FIELD
        (
            p_i2c_registers->cr,
            I2C_CR_DMA_SLE,
            I2C_CR_SHIFT_DMA_SLE,
            (t_uint32) g_i2c_system_context[id].dma_sync_logic_control
        );

        /*Set the loop back mode */
        I2C_WRITE_FIELD
        (
            p_i2c_registers->cr,
            I2C_CR_LM,
            I2C_CR_SHIFT_LM,
            (t_uint32) g_i2c_system_context[id].i2c_loopback_mode
        );

        if (I2C_HARDWARE_GENERAL_CALL_HANDLING == g_i2c_system_context[id].general_call_mode_handling)
        {
            HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_SGCM);
        }
        else
        {
            HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_SGCM);
        }

        /* Disable the Tx DMA */
        HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_DMA_TX_EN);

        /* Diable the Rx DMA */
        HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_DMA_RX_EN);

        /* Flush the Tx Fifo */
        HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FTX);

        /* Flush the Rx Fifo */
        HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FRX);

        /* Set the Bus control mode */
        I2C_WRITE_FIELD
        (
            p_i2c_registers->cr,
            I2C_CR_OM,
            I2C_CR_SHIFT_OM,
            (t_uint32) g_i2c_system_context[id].bus_control_mode
        );

        /* Set the Transmit Fifo threshold value */
        p_i2c_registers->tftr = g_i2c_system_context[id].i2c_transmit_interrupt_threshold;

        /* Set the Receive Fifo Threshold value */
        p_i2c_registers->rftr = g_i2c_system_context[id].i2c_receive_interrupt_threshold;

        /* Now restore CR register with the values it has before it was disabled.*/

        /*  If the controller was enabled before then enable again else let it be activated 
          and not enabled.
        */
        /*Enable the interrupts */
        p_i2c_registers->imscr = (t_uint32) g_i2c_system_context[id].i2c_device_context[0];

        HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
        g_i2c_system_context[id].enabled = TRUE;
    }        
	return(I2C_OK);
}

/****************************************************************************/
/* NAME			:	I2C_Reset                           		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:Reset the I2C Registers for given I2C controller           */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The ID of the controller for reset    */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error  									    	*/
/*                  return  I2C_INVALID_PARAMETER  if id is not correct     */
/*                  else    I2C_OK                                          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_Reset(IN t_i2c_device_id id)
{
    t_i2c_registers *p_i2c_registers;

    /* Check if parameters are valid.*/
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

#if ((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
    /* Clear registers.*/
    HCL_WRITE_REG(p_i2c_registers->cr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->scr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->hsmcr, I2C_CLEAR);    
    HCL_WRITE_REG(p_i2c_registers->tftr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->rftr, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->dmar, I2C_CLEAR);
    HCL_WRITE_REG(p_i2c_registers->icr, 0x131F0008);
    HCL_WRITE_REG(p_i2c_registers->imscr,I2C_CLEAR);
#else
	HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_PE);   /*Clearing the PE bit has the same effect of a SW Reset in V1.0*/
	HCL_WRITE_REG(p_i2c_registers->icr, 0x131F0008);
#endif

    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FTX);       /* Flush the Tx Fifo */
    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FRX);       /* Flush the Rx Fifo */

    /*
	Initialize the right structure to default state 
	*/
    g_i2c_system_context[id].freq_scl = 0;
    g_i2c_system_context[id].freq_input = 0;
    g_i2c_system_context[id].mode = I2C_FREQ_MODE_STANDARD;
    g_i2c_system_context[id].own_address = 0;
    g_i2c_system_context[id].enabled = FALSE;
    g_i2c_system_context[id].slave_address = 0;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].data = 0;
    g_i2c_system_context[id].databuffer = NULL;
    g_i2c_system_context[id].count_data = 0;
    g_i2c_system_context[id].register_index = 0;
    g_i2c_system_context[id].operation = (t_i2c_operation) I2C_NO_OPERATION;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = FALSE;

    /*   g_i2c_system_context[id].i2c_device_context... to be initialized*/
    g_i2c_system_context[id].digital_filter_control = I2C_DIGITAL_FILTERS_OFF;
    g_i2c_system_context[id].dma_sync_logic_control = I2C_DISABLE;
    g_i2c_system_context[id].start_byte_procedure = I2C_DISABLE;
    g_i2c_system_context[id].slave_data_setup_time = 0; /* TBD */
    g_i2c_system_context[id].high_speed_master_code = 0;
    g_i2c_system_context[id].bus_control_mode = I2C_BUS_SLAVE_MODE;
    g_i2c_system_context[id].i2c_loopback_mode = I2C_DISABLE;
    g_i2c_system_context[id].general_call_mode_handling = I2C_NO_GENERAL_CALL_HANDLING;

    g_i2c_system_context[id].index_transfer_mode = I2C_TRANSFER_MODE_POLLING;
    g_i2c_system_context[id].data_transfer_mode = I2C_TRANSFER_MODE_POLLING;
    g_i2c_system_context[id].i2c_transmit_interrupt_threshold = 1;
    g_i2c_system_context[id].i2c_receive_interrupt_threshold = 1;
    g_i2c_system_context[id].transmit_burst_length = 0;
    g_i2c_system_context[id].receive_burst_length = 0;
    g_i2c_system_context[id].index_format = I2C_NO_INDEX;
    g_i2c_system_context[id].current_bus_config = I2C_CURRENT_BUS_SLAVE_TRANSMITTER;
    g_i2c_system_context[id].std =FALSE;
        

    return(I2C_OK);
}
/*----------------------------------------------------------------------------
		Setup Time And Hold Time Related API's 
-----------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	I2C_SetDataHoldTime                   		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:  Sets the Data Hold time for the data                     */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	  : The ID of the controller for reset  */
/*              : t_uint8             : Hold time required in units of ns   */
/*              : t_uint8             : Pad Delay in units of ns            */
/*              : t_i2c_time_period   : I2C clock time period               */  
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error  									    	*/
/*                  return  I2C_INVALID_PARAMETER  if id is not correct     */
/*                  else    I2C_OK                                          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_SetDataHoldTime(IN t_i2c_device_id id, IN t_uint32 hold_time, IN t_uint32 pad_delay, IN t_i2c_time_period period )
{
    
    t_uint8 hold_time_count = 0x0; /*set the count to 0*/   
	t_i2c_registers *p_i2c_registers;
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    if((id > (t_i2c_device_id) I2C_MAX_ID)|| hold_time < I2C_MIN_DATA_HOLD_TIME)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
	
	
	switch(g_i2c_system_context[id].mode)
	{
         case I2C_FREQ_MODE_STANDARD:
         case I2C_FREQ_MODE_FAST:
         case I2C_FREQ_MODE_FAST_PLUS:
              hold_time_count = ((hold_time - pad_delay)/(t_uint8)period);
              break;
         case I2C_FREQ_MODE_HIGH_SPEED:
              hold_time_count = (hold_time/(t_uint8)period);
              break;
         default:
                 break;
    }
    I2C_WRITE_FIELD(p_i2c_registers->thddat, I2C_THDDAT_MASK, I2C_THDDAT_SHIFT, hold_time_count);
        
    return (I2C_OK);            
}
/****************************************************************************/
/* NAME			:	I2C_SetHoldTimeStart                   		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:  Sets the Hold Time for the Start Condition               */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	  : The ID of the controller for reset  */
/*              : t_uint8             : Hold time required in units of ns   */
/*              : t_i2c_time_period   : I2C clock time period               */  
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error  									    	*/
/*                  return  I2C_INVALID_PARAMETER  if id is not correct     */
/*                  else    I2C_OK                                          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/  
PUBLIC t_i2c_error I2C_SetHoldTimeStart(IN t_i2c_device_id id, IN t_uint32 start_hold_time,IN t_i2c_time_period period)
{
	t_uint8 hold_time_count = 0x0; /*set the count to 0*/ 
	t_i2c_registers *p_i2c_registers;
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
	switch(g_i2c_system_context[id].mode)
	{
		case I2C_FREQ_MODE_STANDARD:
			
			if(start_hold_time < I2C_MIN_HOLD_TIME_START_STD )
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_hold_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->thdsta_fst_std, I2C_THDSTA_STD_MASK, I2C_THDSTA_STD_SHIFT, hold_time_count);			
			break;
		
		case I2C_FREQ_MODE_FAST:
			
			if(start_hold_time < I2C_MIN_HOLD_TIME_START_FST)
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_hold_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->thdsta_fst_std, I2C_THDSTA_FST_MASK, I2C_THDSTA_FST_SHIFT, hold_time_count);			
			break;
		
		case I2C_FREQ_MODE_FAST_PLUS:
			
			if(start_hold_time < I2C_MIN_HOLD_TIME_START_FMP)
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_hold_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->thdsta_fmp_hs, I2C_THDSTA_FMP_MASK, I2C_THDSTA_FMP_SHIFT, hold_time_count);			
			break;
		
		case I2C_FREQ_MODE_HIGH_SPEED:
			
			if(start_hold_time < I2C_MIN_HOLD_TIME_START_HS)
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_hold_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->thdsta_fmp_hs, I2C_THDSTA_HS_MASK, I2C_THDSTA_HS_SHIFT, hold_time_count);			
			break;
		
		default:
			break;
		
	}
	return (I2C_OK);   
}  
/****************************************************************************/
/* NAME			:	I2C_SetSetupTimeStart                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:  Sets the Setup Time for the Start Condition              */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	  : The ID of the controller for reset  */
/*              : t_uint8             : Hold time required in units of ns   */
/*              : t_i2c_time_period   : I2C clock time period               */  
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error  									    	*/
/*                  return  I2C_INVALID_PARAMETER  if id is not correct     */
/*                  else    I2C_OK                                          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/  
PUBLIC t_i2c_error I2C_SetSetupTimeStart(IN t_i2c_device_id id, IN t_uint32 start_setup_time,IN t_i2c_time_period period)
{
	t_uint8 hold_time_count = 0x0; /*set the count to 0*/
	t_i2c_registers *p_i2c_registers;
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
	switch(g_i2c_system_context[id].mode)
	{
		case I2C_FREQ_MODE_STANDARD:
			
			if(start_setup_time < I2C_MIN_SETUP_TIME_START_STD )
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_setup_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->tsusta_fst_std, I2C_TSUSTA_STD_MASK, I2C_TSUSTA_STD_SHIFT, hold_time_count);			
			break;
		
		case I2C_FREQ_MODE_FAST:
			
			if(start_setup_time < I2C_MIN_SETUP_TIME_START_FST)
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_setup_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->tsusta_fst_std, I2C_TSUSTA_FST_MASK, I2C_TSUSTA_FST_SHIFT, hold_time_count);			
			break;
		
		case I2C_FREQ_MODE_FAST_PLUS:
			
			if(start_setup_time < I2C_MIN_SETUP_TIME_START_FMP)
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_setup_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->tsusta_fmp_hs, I2C_TSUSTA_FMP_MASK, I2C_TSUSTA_FMP_SHIFT, hold_time_count);			
			break;
		
		case I2C_FREQ_MODE_HIGH_SPEED:
			
			if(start_setup_time < I2C_MIN_SETUP_TIME_START_HS)
			{
				DBGEXIT0(I2C_INVALID_PARAMETER);
				return(I2C_INVALID_PARAMETER);
			}
			
			hold_time_count = (start_setup_time/(t_uint8)period);
			I2C_WRITE_FIELD(p_i2c_registers->tsusta_fmp_hs, I2C_TSUSTA_HS_MASK, I2C_TSUSTA_HS_SHIFT, hold_time_count);			
			break;
		
		default:
			break;		
	}
	return (I2C_OK);   
}
/*-----------------------------------------------------------------------------
		Private functions
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
Name		: i2c_SetBusClock
Description	: Set the I2C bus clock for the given controller.
In			: t_i2c_registers* p_i2c_registers	: pointer to the controller's 
											  registers.
			  t_uint32 freq_scl					: the I2C bus frequency freq_scl (Hz).
			  t_uint32 freq_input					: the input clock frequency (Hz).
InOut		: None
Out			: None
Return value: I2C_OK						: no error.
			  I2C_INVALID_PARAMETER			: wrong id parameter.
			  I2C_freq_scl_NOT_SUPPORTED		: freq_scl is not supported.
Type		: Private
Comments	: The freq_input parameter is only necessary to calculate the I2C bus 
			  frequency and is not used for other purposes.
			  It is not necessary to save the freq_scl as it has been already 
			  saved by I2C_Config().
	
-----------------------------------------------------------------------------*/
t_i2c_error i2c_SetBusClock(t_i2c_device_id id, t_uint32 freq_scl, t_uint32 freq_input)
{
    /* To be defined */
    t_uint32        value, foncycle=0;    
    t_i2c_registers *p_i2c_registers;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	
	switch(g_i2c_system_context[id].digital_filter_control)
	{
		case(I2C_DIGITAL_FILTERS_OFF):
			foncycle = 1;
			break; 
		case(I2C_DIGITAL_FILTERS_1_CLK_SPIKES):
			foncycle = 3;
			break;
		case(I2C_DIGITAL_FILTERS_2_CLK_SPIKES):
			foncycle = 4;
			break;
		case(I2C_DIGITAL_FILTERS_4_CLK_SPIKES):
			foncycle = 6;
			break;
		default:
			return(I2C_INVALID_PARAMETER);
	}
	if (freq_scl <= (t_uint32) I2C_MAX_STANDARD_SCL)
    {    
    
        value = (t_uint32) (((freq_input / freq_scl) - foncycle)/2);
     /*Update the Frequency mode in the global strcture */
        g_i2c_system_context[id].mode = I2C_FREQ_MODE_STANDARD;
	}
	else
	if (freq_scl <= (t_uint32) I2C_MAX_FAST_SCL)
	{
	#if((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
        value = (t_uint32) ((((freq_input / freq_scl) - foncycle)*2)/3);
    #else
        value = (t_uint32) (((freq_input / freq_scl) - foncycle)/3);
    #endif
	/*Update the Frequency mode in the global strcture */
        g_i2c_system_context[id].mode = I2C_FREQ_MODE_FAST;
	}
	else
	if(freq_scl <= (t_uint32) I2C_MAX_FAST_PLUS_SCL)
	{
	#if((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
        value = (t_uint32) ((((freq_input / freq_scl) - foncycle)*2)/3);
    #else
        value = (t_uint32) (((freq_input / freq_scl) - foncycle)/3);
    #endif
	/*Update the Frequency mode in the global strcture */
        g_i2c_system_context[id].mode = I2C_FREQ_MODE_FAST_PLUS;
	}
	else
	if(freq_scl <= (t_uint32) I2C_MAX_HIGH_SPEED_SCL)
	{
	#if((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
        value = (t_uint32) ((((freq_input / freq_scl) - foncycle)*2)/3);
    #else
        value = (t_uint32) (freq_input / ((freq_scl * 3)));
    #endif
	g_i2c_system_context[id].mode = I2C_FREQ_MODE_HIGH_SPEED;
	}
	else
	{
		return(I2C_LINE_FREQ_NOT_SUPPORTED);
	}
	/*Set the Fast Plus mode in the control register  */
	I2C_WRITE_FIELD(p_i2c_registers->cr, I2C_CR_SM, I2C_CR_SHIFT_SM, g_i2c_system_context[id].mode);
	
	if(g_i2c_system_context[id].mode != I2C_FREQ_MODE_HIGH_SPEED)
	{
	/* set the Baud rate counter 2 value  */
	I2C_WRITE_FIELD(p_i2c_registers->brcr, I2C_BRCR_BRCNT2, I2C_BRCR_SHIFT_BRCNT2, value);

	/* Make ensure that BRCNT value set to be zero */
	I2C_WRITE_FIELD(p_i2c_registers->brcr, I2C_BRCR_BRCNT1, I2C_BRCR_SHIFT_BRCNT1, 0);
	}
	else
	{
		/* set the Baud rate counter 1 value for High Speed Mode  */
		I2C_WRITE_FIELD(p_i2c_registers->brcr, I2C_BRCR_BRCNT1, I2C_BRCR_SHIFT_BRCNT1, value);

		/* set the Baud rate counter 2 value  */
        I2C_WRITE_FIELD(p_i2c_registers->brcr, I2C_BRCR_BRCNT2, I2C_BRCR_SHIFT_BRCNT2, I2C_FAST_MODE_BRCR_VAL);

	}
	return(I2C_OK);
}

/*-----------------------------------------------------------------------------
Name		: i2c_AddressIsValid
Description	: Check if the given address is valid.
In			: t_uint16 address	: the slave address to be checked.
InOut		: None
Out			: None
Return value: TRUE				: address is valid.
			  FALSE				: address is not valid.
Type		: Private
Comments	: Note that the least-significant bit of the address parameter 
			  is not relevant for the addressing of the slave device, for 
			  example 0xE2 and 0xE3 will address the same slave device.


  Reserved addresses:
	SLAVE ADDRESS	R/W BIT		RANGE		DESCRIPTION
	0000 000		0			0			General call address
	0000 000		1			1			START byte(1)
	0000 001		X			2-3			CBUS address(2)
	0000 010		X			4-5			Reserved for different bus format(3)
	0000 011		X			6-7			Reserved for future purposes
	0000 1XX		X			8-15		Hs-mode master code
	
	1111 1XX		X			248-255		Reserved for future purposes
	1111 0XX		X			240-247		10-bit slave addressing

  Note that with 7-bit address:
	0000xxxx and 1111xxxx are reserved.  
-----------------------------------------------------------------------------*/
PRIVATE t_bool i2c_AddressIsValid(t_uint16 address)
{
    /* Check if more than 10 bits are needed.*/
    if (address > I2C_MAX_10_BIT_ADDRESS)
    {
        return(FALSE);
    }

    /* 7-bit address. LSB is not considered.*/
    /*Address 0x4 is enabled to support the ST Pepper pot camera  */
    if (address <= I2C_MAX_7_BIT_ADDRESS && !(address == 0 || address == 0x4))
    {
        if ((address < I2C_RESERVED_ADDRESS) || (address > I2C_FUTURE_RESERVED_ADDRESS))
        {
            return(FALSE);
        }
    }

    /* CM: add here the 10-bit check.*/
    return(TRUE);
}


/*-----------------------------------------------------------------------------
Name		: i2c_Abort
Description	: Abort the current transfer operation of the given controller.
In			: t_i2c_device_id id	: the controller to be aborted.
InOut		: None
Out			: None
Return		: I2C_OK		: always no error.
Type		: Private
Comments	: This is called when an unexpected event happens (internal error). 
-----------------------------------------------------------------------------*/
PRIVATE void i2c_Abort(t_i2c_device_id id)
{

    t_i2c_registers *p_i2c_registers;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    /*Disable the interrupts  */
    HCL_WRITE_REG(p_i2c_registers->imscr,I2C_CLEAR);

    /*Disable and SW Reset the Controller  */
    HCL_CLEAR_BITS(p_i2c_registers->cr, I2C_CR_PE); /* Hardware change in MOP500 V1. Clearing PE bit resets all registers */

    /*Set the I2C operation to No operation  */
    g_i2c_system_context[id].operation = (t_i2c_operation) I2C_NO_OPERATION;
}


#if ((defined __I2C_ENHANCED) || (defined __I2C_GENERIC))    /*Enhanced layer private functions   */

/****************************************************************************/
/* NAME			:	i2c_ProcessIt                  		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: This routine is used to process a single interrupt        */
/*                source identified and generate the associated event.      */
/*                It also updates the interrupt status for the pending      */
/*                interrupts.                                               */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: None                                          		    */
/*     InOut    : t_i2c_irq_status * : The status of interrupt source       */
/*                                     to be processed                      */
/* 		OUT 	: t_i2c_active_event* : Generated Event                     */
/*                                                                          */
/* RETURN		:t_i2c_error : I2C error status            	                */
/*               I2C_NO_MORE_PENDING_EVENT                                  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PRIVATE t_i2c_error i2c_ProcessIt(t_i2c_irq_status *status, t_i2c_active_event *event)
{

    /* Interrupt processing for I2C HS controller */    
    t_i2c_error         error_status = I2C_OK;
    t_i2c_device_id     id;
	t_i2c_interrupt     interrupt_source = (t_i2c_interrupt) 0;
		
    id = (t_i2c_device_id) * status;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;                           

    interrupt_source = i2c_ProcessItTestIntBit(status);
	
    /* Processing interrupt */
    
    if((I2C_IT_TXFE == interrupt_source || I2C_IT_TXFNE == interrupt_source || I2C_IT_TXFF == interrupt_source || I2C_IT_TXOVR == interrupt_source))
    {
    	i2c_ProcessItProcessTX(interrupt_source, status);
    }
    else if((I2C_IT_RXFE == interrupt_source || I2C_IT_RXFNF == interrupt_source || I2C_IT_RXFF == interrupt_source || I2C_IT_RFSR == interrupt_source || I2C_IT_RFSE == interrupt_source))
    {
    	error_status = i2c_ProcessItProcessRX(interrupt_source, status);
    	if (error_status != I2C_OK)
        {
            return(error_status);
        }
    }
    else
    {
    	i2c_ProcessItProcessMisc(interrupt_source, status);
    }		
	    
    /* Clear status.*/
    *status = (t_i2c_irq_status) 0;
    /* Fill event.*/
    event->id = id;
    event->type = g_i2c_system_context[id].active_event;
    event->transfer_data = g_i2c_system_context[id].transfer_data;
    /**/
    /* If the operation is terminated (positively or with error),
     allow new operation.*/
    if (I2C_TRANSFER_OK_EVENT == g_i2c_system_context[id].active_event)
    {
        g_i2c_system_context[id].operation = I2C_NO_OPERATION;
    }

    return(I2C_NO_MORE_PENDING_EVENT);

}

/*-----------------------------------------------------------------------------
Name		: i2c_SlaveIndexReceive
Description	: 
In			: t_i2c_id              : I2C Controller id 

InOut		: None
Out			: t_i2c_error          error status
Return value: I2C_OK						: no error
			  I2C_INVALID_PARAMETER			: wrong id parameter.

Type		: Private
Comments	: This  function  perform the operations, when 
              I2C controller addressed as a slave 
-----------------------------------------------------------------------------*/
PRIVATE t_i2c_error i2c_SlaveIndexReceive(t_i2c_device_id id)
{

    t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    if (I2C_WRITE == g_i2c_system_context[id].operation)
    {
        /* SLAVE TRANSMITTER  */
        /* Waiting for the Read from slave request */
        
        while
        (
            (!I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_RFSR, I2C_INT_SHIFT_RFSR))
        &&  loop_counter < I2C_ENDAD_COUNTER
        )
        {
            loop_counter++;
        }
        if (loop_counter >= I2C_ENDAD_COUNTER)
        {
            i2c_Abort(id);
            return(I2C_ADDRESS_MATCH_FAILED);
        }

        /* Acknowledge the Read from slave request */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_RFSR);

        /* Read from slave request recieved */
        /* Flush the Tx Fifo */
        HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FTX);

        /*Wait till for the Tx Flush bit to reset */
        loop_counter = 0;
        while
        (
            I2C_READ_FIELD(p_i2c_registers->cr, I2C_CR_FTX, I2C_CR_SHIFT_FTX)
        &&  loop_counter < I2C_FIFO_FLUSH_COUNTER
        )
        {
            loop_counter++;
        }
        if (loop_counter >= I2C_FIFO_FLUSH_COUNTER)
        {
            return(I2C_HW_FAILED);
        }

        /* update the status */
        g_i2c_system_context[id].status = I2C_STATUS_SLAVE_TRANSMITTER_MODE;
        g_i2c_system_context[id].active_event = I2C_READ_FROM_SLAVE_REQUEST_EVENT;
        g_i2c_system_context[id].current_bus_config = I2C_CURRENT_BUS_SLAVE_TRANSMITTER;


    }
    else
    {
        /* SLAVE RECEIVER  */
        /* Waiting for the Write to slave request */
       
        while
        (
            (!I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_WTSR, I2C_INT_SHIFT_WTSR))
        &&  loop_counter < I2C_ENDAD_COUNTER
        )
        {
            loop_counter++;
        }
        if (loop_counter >= I2C_ENDAD_COUNTER)
        {
            i2c_Abort(id);
            return(I2C_ADDRESS_MATCH_FAILED);
        }

        /* Acknowledge the Write to slave request */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_WTSR);

        /* update the status */
        g_i2c_system_context[id].status = I2C_STATUS_SLAVE_TRANSMITTER_MODE;
        g_i2c_system_context[id].active_event = I2C_WRITE_TO_SLAVE_REQUEST_EVENT;
        g_i2c_system_context[id].current_bus_config = I2C_CURRENT_BUS_SLAVE_RECEIVER;


    }

    /* Update the status of the I2C controller  */
    if (I2C_READ == g_i2c_system_context[id].operation)
    {
        g_i2c_system_context[id].status = I2C_STATUS_SLAVE_RECEIVER_MODE;
    }
    else
    {
        g_i2c_system_context[id].status = I2C_STATUS_SLAVE_TRANSMITTER_MODE;
    }

    return(I2C_OK);
}

/*-----------------------------------------------------------------------------
Name		: i2c_TransmitDataPolling
Description	: Transmit the data in the polling mode 
In			: t_i2c_id              : I2C Controller id 

InOut		: None
Out			: t_i2c_error          error status
Return value: I2C_OK						: no error
			  I2C_INVALID_PARAMETER			: wrong id parameter.

Type		: Private
Comments	: 
-----------------------------------------------------------------------------*/
PRIVATE t_i2c_error i2c_TransmitDataPolling(t_i2c_device_id id, volatile t_uint8 *p_data,t_i2c_transfer_data data_type) 
{

    t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;
    t_i2c_error     error_status;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
    {
        /* Slave tranmitter */
        while (g_i2c_system_context[id].count_data != 0)
        {
            /* Check for Tx Fifo not full */
            error_status = i2c_TransmitDataPollingCheckTxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            p_i2c_registers->tfr = *p_data;
            g_i2c_system_context[id].transfer_data++;
            g_i2c_system_context[id].count_data--;
			if (I2C_MULTIPLE_BYTE_TRANSFER == data_type)
			{
				p_data++;
			}
			g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
        }

        /* End of Data transfer */
        /* Check for the Slave tranaction done */
        error_status = i2c_TransmitDataPollingCheckForSTD(id);
        if (error_status != I2C_OK)
        {
            return(error_status);
        }

        /* Slave Transaction has been done */
        /* Acknowledge the Slave Transaction done */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_STD);


        g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
        return(I2C_OK);
    }
    else
    {
        /* Master Transmitter */
        while (g_i2c_system_context[id].count_data != 0)
        {
            /* Check for Tx Fifo not full */
            error_status = i2c_TransmitDataPollingCheckTxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            p_i2c_registers->tfr = *p_data;


            g_i2c_system_context[id].transfer_data++;
            g_i2c_system_context[id].count_data--;
			if(I2C_MULTIPLE_BYTE_TRANSFER == data_type)
			{
				p_data++;
			}
			g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
        }

        /* End of Data transfer */
        
        loop_counter = 0;
        /* Check whether the Stop bit has been programmed or not */
        if(I2C_READ_FIELD(p_i2c_registers->mcr, I2C_MCR_STOP, I2C_MCR_SHIFT_STOP))
        {
            /* Check for the Master transaction Done */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTD, I2C_INT_SHIFT_MTD)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        else
        {
        	/* Check for the Master transaction Done Without Stop */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTDWS, I2C_INT_SHIFT_MTDWS)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        
        
        if (loop_counter >= I2C_ENDAD_COUNTER)
        {
            error_status = i2c_GetAbortCause(id);
            if (error_status != I2C_OK)
            {
                i2c_Abort(id);
                return(error_status);
            }
            else
            {
                i2c_Abort(id);
            }

            return(I2C_INTERNAL_ERROR);
        }
        
        
        /* Master Transaction has been done */
        /* Acknowledge the Master Transaction Done */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTD);
        
        /* Master Transaction Without Stop has been done */
        /* Acknowledge the Master Transaction Done Without Stop */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTDWS);
        
        
        g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;


        g_i2c_system_context[id].operation = I2C_NO_OPERATION;
        return(I2C_OK);
    }

}
/*-----------------------------------------------------------------------------
Name		: i2c_TransmitDataPollingSingle
Description	: Transmit the data in the polling mode for a Single Byte 
In			: t_i2c_id              : I2C Controller id 

InOut		: None
Out			: t_i2c_error          error status
Return value: I2C_OK						: no error
			  I2C_INVALID_PARAMETER			: wrong id parameter.

Type		: Private
Comments	: 
-----------------------------------------------------------------------------*/
PRIVATE t_i2c_error i2c_TransmitDataPollingSingle(t_i2c_device_id id, volatile t_uint8 *p_data) 
{

    t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;
    t_i2c_error     error_status;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
    {
        /* Slave tranmitter */
        while (g_i2c_system_context[id].count_data != 0)
        {
            /* Check for Tx Fifo not full */
            error_status = i2c_TransmitDataPollingCheckTxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            p_i2c_registers->tfr = *p_data;
            g_i2c_system_context[id].transfer_data++;
            g_i2c_system_context[id].count_data--;
			g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
        }

        /* End of Data transfer */
        /* Check for the Slave tranaction done */
        error_status = i2c_TransmitDataPollingCheckForSTD(id);
        if (error_status != I2C_OK)
        {
            return(error_status);
        }

        /* Slave Transaction has been done */
        /* Acknowledge the Slave Transaction done */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_STD);


        g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
        return(I2C_OK);
    }
    else
    {
        /* Master Transmitter */
        while (g_i2c_system_context[id].count_data != 0)
        {
            /* Check for Tx Fifo not full */
            error_status = i2c_TransmitDataPollingCheckTxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            p_i2c_registers->tfr = *p_data;


            g_i2c_system_context[id].transfer_data++;
            g_i2c_system_context[id].count_data--;
			g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
        }

        /* End of Data transfer */
        
        loop_counter = 0;
        /* Check whether the Stop bit has been programmed or not */
        if(I2C_READ_FIELD(p_i2c_registers->mcr, I2C_MCR_STOP, I2C_MCR_SHIFT_STOP))
        {
            /* Check for the Master transaction Done */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTD, I2C_INT_SHIFT_MTD)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        else
        {
        	/* Check for the Master transaction Done Without Stop */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTDWS, I2C_INT_SHIFT_MTDWS)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        
        
        if (loop_counter >= I2C_ENDAD_COUNTER)
        {
            error_status = i2c_GetAbortCause(id);
            if (error_status != I2C_OK)
            {
                i2c_Abort(id);
                return(error_status);
            }
            else
            {
                i2c_Abort(id);
            }

            return(I2C_INTERNAL_ERROR);
        }
        
        
        /* Master Transaction has been done */
        /* Acknowledge the Master Transaction Done */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTD);
        
        /* Master Transaction Without Stop has been done */
        /* Acknowledge the Master Transaction Done Without Stop */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTDWS);
        
        
        g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;


        g_i2c_system_context[id].operation = I2C_NO_OPERATION;
        return(I2C_OK);
    }

}


/*-----------------------------------------------------------------------------
Name		: i2c_ReceiveDataPolling
Description	: Receiving the data in polling mode
In			: t_i2c_id              : I2C Controller id 

InOut		: None
Out			: t_i2c_error          error status
Return value: I2C_OK						: no error
			  I2C_WRONG_PARAMETER			: wrong id parameter.

Type		: Private
Comments	: 
-----------------------------------------------------------------------------*/
PRIVATE t_i2c_error i2c_ReceiveDataPolling(t_i2c_device_id id, t_uint8 *p_data)
{

    t_uint32        loop_counter = 0;
    t_i2c_error     error_status;
    t_i2c_registers *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
    {
        /* Slave Receiver */
        while (g_i2c_system_context[id].count_data != 0)
        {
            /* Wait for the Rx Fifo  empty */
            error_status = i2c_ReceiveDataPollingCheckRxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            /* Read the data byte from Rx Fifo */
            *p_data = (t_uint8) p_i2c_registers->rfr;


            g_i2c_system_context[id].transfer_data++;
            g_i2c_system_context[id].count_data--;
            p_data++;
            g_i2c_system_context[id].active_event = I2C_DATA_RX_EVENT;
        }   /* Data Reception has been completed */

        /* Check for the slave transaction done */
        error_status = i2c_ReceiveDataPollingCheckForSTD(id);
        if (error_status != I2C_OK)
        {
            return(error_status);
        }

        /* Slave Transaction has been done */
        /* Acknowledge the Slave Transaction Done */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_STD);
        g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
        g_i2c_system_context[id].operation = I2C_NO_OPERATION;


        return(I2C_OK);
    }
    else
    {
        /* Master Receiver */
        while (g_i2c_system_context[id].count_data != 0)
        {
            /* Wait for the Rx Fifo  empty */
            error_status = i2c_ReceiveDataPollingCheckRxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            /* Read the data byte from Rx Fifo */
            *p_data = (t_uint8) p_i2c_registers->rfr;


            g_i2c_system_context[id].transfer_data++;
            g_i2c_system_context[id].count_data--;
            p_data++;
            g_i2c_system_context[id].active_event = I2C_DATA_RX_EVENT;
        }   /* Data reception  has been completed */

        loop_counter = 0;
        /* Check whether the Stop bit has been programmed or not */
        if(I2C_READ_FIELD(p_i2c_registers->mcr, I2C_MCR_STOP, I2C_MCR_SHIFT_STOP))
        {
            /* Check for the Master transaction Done */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTD, I2C_INT_SHIFT_MTD)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        else
        {
        	/* Check for the Master transaction Done Without Stop */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTDWS, I2C_INT_SHIFT_MTDWS)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        if (loop_counter >= I2C_ENDAD_COUNTER)
        {
            error_status = i2c_GetAbortCause(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }
            else
            {
                i2c_Abort(id);
            }

            return(I2C_INTERNAL_ERROR);
        }

        /* Master Transaction has been done */
        /* Acknowledge the Master Transaction Done */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTD);
        
        /* Master Transaction Without Stop has been done */
        /* Acknowledge the Master Transaction Done Without Stop */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTDWS);

        g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
        g_i2c_system_context[id].operation = I2C_NO_OPERATION;


    }

    return(I2C_OK);
}

/*-----------------------------------------------------------------------------
Name		: i2c_MasterIndexTransmit
Description	: Transmits the index to slave 
In			: t_i2c_id              : I2C Controller id 

InOut		: None
Out			: t_i2c_error          error status
Return value: I2C_OK						: no error
			  I2C_WRONG_PARAMETER			: wrong id parameter.

Type		: Private
Comments	: 
-----------------------------------------------------------------------------*/
PRIVATE t_i2c_error i2c_MasterIndexTransmit(t_i2c_device_id id)
{

    volatile t_uint32   mcr = 0;
    t_uint32            loop_counter = 0;
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    error_status = i2c_MasterIndexTransmitCheckIndex(id);
    if (error_status != I2C_OK)
    {
        return(error_status);
    }

    if (g_i2c_system_context[id].operation == I2C_READ)
    {
        
        /* Check whether the Stop bit has been programmed or not */
        if(I2C_READ_FIELD(p_i2c_registers->mcr, I2C_MCR_STOP, I2C_MCR_SHIFT_STOP))
        {
            /* Check for the Master transaction Done */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTD, I2C_INT_SHIFT_MTD)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        else
        {
        	/* Check for the Master transaction Done Without Stop */
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTDWS, I2C_INT_SHIFT_MTDWS)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
        }
        
        if (loop_counter >= I2C_ENDAD_COUNTER)
        {
            error_status = i2c_GetAbortCause(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }
            else
            {
                i2c_Abort(id);
            }

            return(I2C_INTERNAL_ERROR);
        }

        /* Master Transaction has been done */
        /* Acknowledge the Master Transaction Done */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTD);
        
        /* Master Transaction Without Stop has been done */
        /* Acknowledge the Master Transaction Done Without Stop */
        HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTDWS);

        /* Master control configuration for read operation  */
        HCL_SET_BITS(mcr, I2C_MCR_OP);

        /* Configure the STOP condition*/
        HCL_SET_BITS(mcr, I2C_MCR_STOP);

        /* Configuring the Frame length */
        I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, g_i2c_system_context[id].count_data);
        
        I2C_WRITE_FIELD(p_i2c_registers->mcr,I2C_MCR_LENGTH_STOP_OP,I2C_MCR_SHIFT_LENGTH_STOP_OP,mcr);        

    }

    /* Update the status of the I2C controller  */
    if (I2C_READ == g_i2c_system_context[id].operation)
    {
        g_i2c_system_context[id].status = I2C_STATUS_MASTER_RECEIVER_MODE;
    }
    else
    {
        g_i2c_system_context[id].status = I2C_STATUS_MASTER_TRANSMITTER_MODE;
    }

    return(error_status);
}
#endif /* End of Enhanced layer private functions */

 
/* Private function valid for HS controller */


/****************************************************************************/
/* NAME			:	i2c_GetAbortCause                  		  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Get the abort Cause                                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller that aborted   		*/
/*     InOut    :  None                                                     */
/* 		OUT 	: None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_error i2c_GetAbortCause(t_i2c_device_id id)
{
    t_uint8         abort_cause;
    t_i2c_error     error_status;

    t_i2c_registers *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    if (I2C_READ_FIELD(p_i2c_registers->sr, I2C_SR_STATUS, I2C_SR_SHIFT_STATUS) == 3)
    {
        abort_cause = (t_uint8) I2C_READ_FIELD(p_i2c_registers->sr, I2C_SR_CAUSE, I2C_SR_SHIFT_CAUSE);

        switch (abort_cause)
        {
            case 0:
                error_status = I2C_ACK_FAIL_ON_ADDRESS;
                break;

            case 1:
                error_status = I2C_ACK_FAIL_ON_DATA;
                break;

            case 2:
                error_status = I2C_ACK_IN_HS_MODE;
                break;

            case 3:
                error_status = I2C_ARBITRATION_LOST;
                break;

            case 4:
                error_status = I2C_BUS_ERROR_DETECTED_START;
                break;

            case 5:
                error_status = I2C_BUS_ERROR_DETECTED_STOP;
                break;

            case 6:
                error_status = I2C_OVERFLOW;
                break;

            default:
                error_status = I2C_INTERNAL_ERROR;
                break;
        }

        return(error_status);
    }

    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItTestIntBit               		  	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Get the interrupt source                                */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_interrupt									    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_interrupt i2c_ProcessItTestIntBit(t_i2c_irq_status *status)
{

    t_uint32            misr;
    t_i2c_device_id     id;
    t_i2c_interrupt     interrupt_source = (t_i2c_interrupt) 0;
    t_i2c_registers     *p_i2c_registers;
	
    id = (t_i2c_device_id) * status;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers; 
	misr = p_i2c_registers->misr;
    
	if (HCL_READ_BITS(misr, I2C_INT_MTDWS))
    {
        interrupt_source = I2C_IT_MTDWS;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_BERR))
    {
        interrupt_source = I2C_IT_BERR;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_MAL))
    {
        interrupt_source = I2C_IT_MAL;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_STD))
    {
        interrupt_source = I2C_IT_STD;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_MTD))
    {
        interrupt_source = I2C_IT_MTD;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_WTSR))
    {
        interrupt_source = I2C_IT_WTSR;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_RFSR))
    {
        interrupt_source = I2C_IT_RFSR;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_RFSE))
    {
        interrupt_source = I2C_IT_RFSE;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_TXFE))
    {
        interrupt_source = I2C_IT_TXFE;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_TXFNE))
    {
        interrupt_source = I2C_IT_TXFNE;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_TXFF))
    {
        interrupt_source = I2C_IT_TXFF;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_TXFOVR))
    {
        interrupt_source = I2C_IT_TXOVR;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_RXFE))
    {
        interrupt_source = I2C_IT_RXFE;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_RXFNF))
    {
        interrupt_source = I2C_IT_RXFNF;
    }
    else if (HCL_READ_BITS(misr, I2C_INT_RXFF))
    {
        interrupt_source = I2C_IT_RXFF;
    }

    return(interrupt_source);
 }
/****************************************************************************/
/* NAME			:	i2c_ProcessItDisableIRQSrc               		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Disables the interrupt source of the controller         */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItDisableIRQSrc(t_i2c_irq_status *status)
{

	t_i2c_device_id     id;
	t_uint32 id_identifier = 0x00000000;
	
	id = (t_i2c_device_id) * status;
	
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	
	/*Disable the IRQ Source*/
	I2C_DisableIRQSrc((t_i2c_irq_src_id)(id_identifier|I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY));
    	
}

/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessTXFNE               		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing of the TXFNE interrupt				        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessTXFNE(t_i2c_irq_status *status)
{

	t_uint32            txthreshold, maxthreshold;
    t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;
    volatile t_uint32   count;
    
    id = (t_i2c_device_id) * status;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    txthreshold = p_i2c_registers->tftr;
    maxthreshold = 15;
    

            if
            (
                (I2C_BUS_MASTER_MODE == g_i2c_system_context[id].bus_control_mode)
            &&  (g_i2c_system_context[id].index_format > I2C_NO_INDEX)
            )
            {
                switch (g_i2c_system_context[id].index_format)
                {
                    case I2C_BYTE_INDEX:
        
                        p_i2c_registers->tfr = (0xFF & g_i2c_system_context[id].register_index);
                        g_i2c_system_context[id].index_format = I2C_NO_INDEX;
                        g_i2c_system_context[id].active_event = I2C_INDEX_TX_EVENT;
                        break;

                    case I2C_HALF_WORD_LITTLE_ENDIAN:
        
                        p_i2c_registers->tfr = (0xFF & g_i2c_system_context[id].register_index);

                        p_i2c_registers->tfr = (g_i2c_system_context[id].register_index >> SHIFT_BYTE1);
                        g_i2c_system_context[id].index_format = I2C_NO_INDEX;
                        g_i2c_system_context[id].active_event = I2C_INDEX_TX_EVENT;
                        break;

                    case I2C_HALF_WORD_BIG_ENDIAN:
        
                        p_i2c_registers->tfr = (g_i2c_system_context[id].register_index >> SHIFT_BYTE1);
        
                        p_i2c_registers->tfr = (0xFF & g_i2c_system_context[id].register_index);
                        g_i2c_system_context[id].index_format = I2C_NO_INDEX;
                        g_i2c_system_context[id].active_event = I2C_INDEX_TX_EVENT;
                        break;

                    default:
                        break;
                }

                if ( I2C_READ == g_i2c_system_context[id].operation)
                {   
				    
				    i2c_ProcessItDisableIRQSrc(status);
				    
/*                     I2C_DisableIRQSrc((I2C0 == id) ? I2C0_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY : I2C1_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY);*/
                }

                if (I2C_WRITE == g_i2c_system_context[id].operation)
                {
                    if (FALSE == g_i2c_system_context[id].multi_operation)
                    {
                            /* Tranmit single data byte */
                            p_i2c_registers->tfr = g_i2c_system_context[id].data;
    
                            g_i2c_system_context[id].transfer_data++;
                            g_i2c_system_context[id].count_data--;
                    }
                    else
                    {
                        for
                        (
                            count = (maxthreshold - txthreshold - 2);
                            count > 0 && (0 != g_i2c_system_context[id].count_data);
                            count--
                        )
                        {
                            /* Write multiple bytes into FIFO */
                            p_i2c_registers->tfr = *g_i2c_system_context[id].databuffer;

                            g_i2c_system_context[id].databuffer++;
                            g_i2c_system_context[id].count_data--;
                            g_i2c_system_context[id].transfer_data++;
                        }
                    }
                    
                    if ( 0 == g_i2c_system_context[id].count_data)
                    {
					   
				        i2c_ProcessItDisableIRQSrc(status);
                        /*I2C_DisableIRQSrc((I2C0 == id) ? I2C0_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY : I2C1_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY);*/
                    }
                    
                    g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
                }
            }
            else
            {
                if (FALSE == g_i2c_system_context[id].multi_operation)
                {
                        /* Transmit single data byte */
                        p_i2c_registers->tfr = g_i2c_system_context[id].data;
    
                        g_i2c_system_context[id].transfer_data++;
                        g_i2c_system_context[id].count_data--;
                }
                else
                {
                    for
                    (
                        count = (maxthreshold - txthreshold);
                        count > 0 && (0 != g_i2c_system_context[id].count_data);
                        count--
                    )
                    {
                        /* Write multiple bytes into FIFO */
                        p_i2c_registers->tfr = *g_i2c_system_context[id].databuffer;
    
                        g_i2c_system_context[id].databuffer++;
                        g_i2c_system_context[id].count_data--;
                        g_i2c_system_context[id].transfer_data++;
                    }
                }
                
                if ( 0 == g_i2c_system_context[id].count_data)
                {

				    i2c_ProcessItDisableIRQSrc(status);
				                
/*                   I2C_DisableIRQSrc((I2C0 == id) ? I2C0_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY : I2C1_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY);*/
                }

                g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
            }
            

}

/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessRFSE	               		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing of the RFSE interrupt				        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessRFSE(t_i2c_irq_status *status)
{
	t_uint32            txthreshold;
    t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;
    volatile t_uint32   count;
	t_uint32 id_identifier = 0x00000000;
    
    id = (t_i2c_device_id) * status;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    txthreshold = p_i2c_registers->tftr;
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);

			
                if (FALSE == g_i2c_system_context[id].multi_operation)
                {
                    if (1 == g_i2c_system_context[id].count_data)
                    {
                        /* Transmit single data byte */
                        p_i2c_registers->tfr = g_i2c_system_context[id].data;

                        g_i2c_system_context[id].transfer_data++;
                        g_i2c_system_context[id].count_data--;
                        
						/*Disable the IRQ Source*/
						I2C_DisableIRQSrc((t_i2c_irq_src_id)(id_identifier|(t_uint32)I2C_IRQ_SRC_READ_FROM_SLAVE_EMPTY));
					}
                }
                else
                {
                    for (count = txthreshold; count > 0 && (0 != g_i2c_system_context[id].count_data); count--)
                    {
                        /* Write multiple bytes into FIFO */
                        p_i2c_registers->tfr = *g_i2c_system_context[id].databuffer;
    
                        g_i2c_system_context[id].databuffer++;
                        g_i2c_system_context[id].count_data--;
                        g_i2c_system_context[id].transfer_data++;
                    }
                }

            /*Enable the Tx nearly empty interrupt to send the remaining data    */
            if (g_i2c_system_context[id].count_data > 0)
            {
				I2C_EnableIRQSrc((t_i2c_irq_src_id)(id_identifier|I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY));
				I2C_DisableIRQSrc((t_i2c_irq_src_id)(id_identifier|I2C_IRQ_SRC_READ_FROM_SLAVE_EMPTY));
						                    
            }

            g_i2c_system_context[id].active_event = I2C_READ_FROM_SLAVE_EMPTY_EVENT;
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessMTD	               		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing of the Master Transaction done interrupt	    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessMTD(t_i2c_irq_status *status)
{
	
	volatile t_uint32   mcr = 0;
	t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 id_identifier = 0x00000000;
    
    id = (t_i2c_device_id) * status;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);

	if (I2C_READ == g_i2c_system_context[id].operation)
	{
		/* read the pending data in the Rx Fifo  */
		while (!HCL_READ_BITS(p_i2c_registers->risr, I2C_INT_RXFE))
		{
			if(0 == g_i2c_system_context[id].count_data)
			{
			   break;
			}
			*g_i2c_system_context[id].databuffer = (t_uint8)p_i2c_registers->rfr;


			g_i2c_system_context[id].databuffer++;
			g_i2c_system_context[id].count_data--;
			g_i2c_system_context[id].transfer_data++;
		}
	}

	/* Acknowledge the Master Transaction done interrupt */
	HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTD);

	if ((g_i2c_system_context[id].operation == I2C_READ) && (g_i2c_system_context[id].count_data > 0))
	{
		mcr |= (I2C_MCR_OP|I2C_MCR_STOP); /*Set the read Operation and Transaction is terminated by STOP*/
		
		I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, g_i2c_system_context[id].count_data);
		
		g_i2c_system_context[id].index_format = I2C_NO_INDEX;
		I2C_WRITE_FIELD(p_i2c_registers->mcr,I2C_MCR_LENGTH_STOP_OP,I2C_MCR_SHIFT_LENGTH_STOP_OP,mcr);        
		
		/*Disable the Interrupts*/
		I2C_DisableIRQSrc((t_i2c_irq_src_id)(I2C_MTD_INTR|id_identifier));
					  
		g_i2c_system_context[id].active_event = I2C_DATA_RX_EVENT;                
	}
	else
	{
   
		g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
		I2C_DisableIRQSrc((t_i2c_irq_src_id)(I2C_MTD_INTR|id_identifier|
							(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_EMPTY |
							(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_NEARLY_FULL |
							(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_FULL));                
	}
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessSTD	               		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing of the Slave Transaction done interrupt	    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessSTD(t_i2c_irq_status *status)
{
	t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 id_identifier = 0x00000000;
    
    id = (t_i2c_device_id) * status;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;	
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
			
	if (I2C_READ == g_i2c_system_context[id].operation)
	{
		/* read the pending data in the Rx Fifo  */
		while (!HCL_READ_BITS(p_i2c_registers->risr, I2C_INT_RXFE))
		{
			*g_i2c_system_context[id].databuffer = (t_uint8)p_i2c_registers->rfr;
			g_i2c_system_context[id].databuffer++;
			g_i2c_system_context[id].count_data--;
			g_i2c_system_context[id].transfer_data++;
		}
	}

	HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_STD);
	
	if(    (g_i2c_system_context[id].own_address > I2C_MAX_7_BIT_ADDRESS)
		&& (I2C_WRITE == g_i2c_system_context[id].operation) 
		&& (FALSE == g_i2c_system_context[id].std ))
	{
		g_i2c_system_context[id].active_event =I2C_SLAVE_TRANSACTION_DONE_EVENT;
		g_i2c_system_context[id].std = TRUE;
	}
	else
	{
		g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
		g_i2c_system_context[id].std =FALSE;
	}
	
	I2C_DisableIRQSrc((t_i2c_irq_src_id)(I2C_STD_INTR|id_identifier));
	I2C_ClearIRQSrc((t_i2c_irq_src_id)(I2C_IRQ_SRC_ALL|id_identifier));            
            
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessBERR               		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing of the Bus Error interrupt	    			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessBERR(t_i2c_irq_status *status)
{
	t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;
    t_uint32            bus_error_cause;
    
    id = (t_i2c_device_id) * status;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

            if (I2C_BUS_ERROR_STATUS == I2C_READ_FIELD(p_i2c_registers->sr, I2C_SR_STATUS, I2C_SR_SHIFT_STATUS))
            {
                /*Bus error */
    
                i2c_Abort(id);

                /*Read the Abort cause and set the active event*/
                bus_error_cause = (t_uint32) I2C_READ_FIELD(p_i2c_registers->sr, I2C_SR_CAUSE, I2C_SR_SHIFT_CAUSE);
                switch (bus_error_cause)
                {
                    case I2C_NACK_ADDR:
                        g_i2c_system_context[id].active_event = I2C_ABORT_NACK_ON_ADDRESS_EVENT;
                        break;

                    case I2C_NACK_DATA:
                        g_i2c_system_context[id].active_event = I2C_ABORT_NACK_ON_DATA_EVENT;
                        break;

                    case I2C_ACK_MCODE:
                        g_i2c_system_context[id].active_event = I2C_ABORT_ACK_ON_MASTER_CODE_EVENT;
                        break;

                    case I2C_ARB_LOST:
                        g_i2c_system_context[id].active_event = I2C_ARBITRATION_LOST_ERROR_EVENT;
                        break;

                    case I2C_BERR_START:
                        g_i2c_system_context[id].active_event = I2C_BUS_ERROR_DETECTED_START_EVENT;
                        break;

                    case I2C_BERR_STOP:
                        g_i2c_system_context[id].active_event = I2C_BUS_ERROR_DETECTED_STOP_EVENT;
                        break;

                    case I2C_OVFL:
                        g_i2c_system_context[id].active_event = I2C_OVERFLOW_EVENT;
                        break;

                    default:
                        g_i2c_system_context[id].active_event = I2C_BUS_ERROR_EVENT;
                        break;
                }
            }
            else
            {
                if (I2C_WRITE == g_i2c_system_context[id].operation)
                {
                    g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
                }
                else
                {
                    g_i2c_system_context[id].active_event = I2C_DATA_RX_EVENT;
                }
            }
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessMTDWS              		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing of the MTD without Stop Interrupt   			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessMTDWS(t_i2c_irq_status *status)
{
	
	volatile t_uint32   mcr = 0;
	t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 id_identifier = 0x00000000;
    
    id = (t_i2c_device_id) * status;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);

	if (I2C_READ == g_i2c_system_context[id].operation)
	{
		/* read the pending data in the Rx Fifo  */
		while (!HCL_READ_BITS(p_i2c_registers->risr, I2C_INT_RXFE))
		{
			if(0 == g_i2c_system_context[id].count_data)
			{
			   break;
			}
			*g_i2c_system_context[id].databuffer = (t_uint8)p_i2c_registers->rfr;


			g_i2c_system_context[id].databuffer++;
			g_i2c_system_context[id].count_data--;
			g_i2c_system_context[id].transfer_data++;
		}
	}

	/* Acknowledge the Master Transaction done interrupt */
	HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTDWS);

	if ((g_i2c_system_context[id].operation == I2C_READ) && (g_i2c_system_context[id].count_data > 0))
	{
		mcr |= (I2C_MCR_OP|I2C_MCR_STOP); /* Set the read operation and Transaction is terminated by STOP */ 
		
		I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, g_i2c_system_context[id].count_data);
		
		g_i2c_system_context[id].index_format = I2C_NO_INDEX;
		I2C_WRITE_FIELD(p_i2c_registers->mcr,I2C_MCR_LENGTH_STOP_OP,I2C_MCR_SHIFT_LENGTH_STOP_OP,mcr);        
		
		/*Disable the Interrupts*/
		I2C_DisableIRQSrc((t_i2c_irq_src_id)(I2C_MTDWS_INTR|id_identifier));
					  
		g_i2c_system_context[id].active_event = I2C_DATA_RX_EVENT;                
	}
	else
	{
   
		g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
		I2C_DisableIRQSrc((t_i2c_irq_src_id)(I2C_MTDWS_INTR|id_identifier|
							(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_EMPTY |
							(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_NEARLY_FULL |
							(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_FULL));                
	}
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessTX              		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing Trasmission Related Interrupts    			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessTX(t_i2c_interrupt interrupt_source, t_i2c_irq_status *status)
{

	/* Interrupt processing for I2C HS controller */
    t_i2c_device_id     id;

	id = (t_i2c_device_id) * status;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
	
    switch (interrupt_source)
    {
        case I2C_IT_TXFE:
        case I2C_IT_TXFNE:
        
        	i2c_ProcessItProcessTXFNE(status);
            
            break;

        case I2C_IT_TXFF:
            g_i2c_system_context[id].active_event = I2C_TRANSMIT_FIFO_FULL_EVENT;
    
            break;

        case I2C_IT_TXOVR:
            /* Tx FIFO over flow interrupt */
            /* Tx FIFO content is corrupted and at least one word has been lost */
            /* Flush the Tx fifo */
    
            g_i2c_system_context[id].active_event = I2C_TRANSMIT_FIFO_OVERRUN_EVENT;
            break;
            
        default:
        	break;
            
    }
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessRX              		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing Receiving Related Interrupts    				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE t_i2c_error i2c_ProcessItProcessRX(t_i2c_interrupt interrupt_source, t_i2c_irq_status *status)
{

    /* Interrupt processing for I2C HS controller */
    t_uint32            rxthreshold, maxthreshold, loop_counter;
    t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;
    volatile t_uint32   count;

    id = (t_i2c_device_id) * status;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    rxthreshold = p_i2c_registers->rftr;
    maxthreshold = I2C_MAX_THRESHOLD_VALUE;
    
    switch (interrupt_source)
    {
        case I2C_IT_RXFE:
            /* Rx FIFO empty */
            /* Updating the active event status */
            g_i2c_system_context[id].active_event = I2C_RECEIVE_FIFO_EMPTY_EVENT;

            break;

        case I2C_IT_RXFNF:
            /* Read the RxThreshold number of bytes data from Rx Fifo */
            for (count = rxthreshold; count > 0; count--)
            {
                *g_i2c_system_context[id].databuffer = (t_uint8)p_i2c_registers->rfr;

                g_i2c_system_context[id].databuffer++;
            }

            g_i2c_system_context[id].count_data -= rxthreshold;
            g_i2c_system_context[id].transfer_data += rxthreshold;

            /* Up dating the Event status */
            g_i2c_system_context[id].active_event = I2C_RECEIVE_FIFO_NEARLY_FULL_EVENT;

            break;

        case I2C_IT_RXFF:
            /* Rx FIFO is FULL */
            /* read the all bytes from Rx Fifo */
            for (count = maxthreshold; count > 0; count--)
            {
                *g_i2c_system_context[id].databuffer = (t_uint8)p_i2c_registers->rfr;
    
                g_i2c_system_context[id].databuffer++;
            }

            g_i2c_system_context[id].count_data -= maxthreshold;
            g_i2c_system_context[id].transfer_data += maxthreshold;

            /* Up dating the Event status */
            g_i2c_system_context[id].active_event = I2C_RECEIVE_FIFO_FULL_EVENT;
            break;

        case I2C_IT_RFSR:
            /* Slave Transmitter */
            /* Flush the Tx Fifo */
            HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_FTX);

            /*Wait till for the Tx Flush bit to reset */
            loop_counter = 0;
            while
            (
                I2C_READ_FIELD(p_i2c_registers->cr, I2C_CR_FTX, I2C_CR_SHIFT_FTX)
            &&  loop_counter < I2C_FIFO_FLUSH_COUNTER
            )
            {
                loop_counter++;
            }
            if (loop_counter >= I2C_FIFO_FLUSH_COUNTER)
            {
                return(I2C_HW_FAILED);
            }

            /* update the status */
            g_i2c_system_context[id].status = I2C_STATUS_SLAVE_TRANSMITTER_MODE;
            g_i2c_system_context[id].current_bus_config = I2C_CURRENT_BUS_SLAVE_TRANSMITTER;
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_RFSR);

            g_i2c_system_context[id].active_event = I2C_READ_FROM_SLAVE_REQUEST_EVENT;
            break;

        case I2C_IT_RFSE:
            /* Read from Slave Empty */
            
            if (0 == g_i2c_system_context[id].count_data)
            {
                DBGEXIT0(I2C_INTERNAL_ERROR);
                i2c_Abort(id);
                return(I2C_INTERNAL_ERROR);
            }
            
            else
            {
            	i2c_ProcessItProcessRFSE(status);
            }
    
            break;
            
        default:
        	break;
            
    }
    
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_ProcessItProcessMisc              		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Processing Miscellaneous Interrupts	    				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_irq_status 	: The interrupt request source status   */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE void i2c_ProcessItProcessMisc(t_i2c_interrupt interrupt_source, t_i2c_irq_status *status)
{

	/* Interrupt processing for I2C HS controller */    
    t_i2c_device_id     id;
    t_i2c_registers     *p_i2c_registers;    

    id = (t_i2c_device_id) * status;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    switch (interrupt_source)
    {
        case I2C_IT_WTSR:
            /* Slave Receiver */
            g_i2c_system_context[id].status = I2C_STATUS_SLAVE_RECEIVER_MODE;
            g_i2c_system_context[id].current_bus_config = I2C_CURRENT_BUS_SLAVE_RECEIVER;

    
            /*Acknowledge the write to slave request interrupt */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_WTSR);
            g_i2c_system_context[id].active_event = I2C_WRITE_TO_SLAVE_REQUEST_EVENT;

            break;

        case I2C_IT_MTD:
            /* Master Transaction has been done */

    		i2c_ProcessItProcessMTD(status);
            
            break;

        case I2C_IT_STD:
            /* Slave Transaction has been done */
            /* Acknowledge the Slave Transaction done */

    		i2c_ProcessItProcessSTD(status);        

            break;

        case I2C_IT_MAL:
            /*Master Arbitration lost error */
    
            i2c_Abort(id);

            /* Acknowledge the Master Arbitration lost interrupt */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MAL);
            g_i2c_system_context[id].active_event = I2C_ARBITRATION_LOST_ERROR_EVENT;

            break;

        case I2C_IT_BERR:

            i2c_ProcessItProcessBERR(status);

            /* Acknowledge the interrupt */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_BERR);
            break;
            
        case I2C_IT_MTDWS:
            /* Master Transaction Without Stop has been done */

    		i2c_ProcessItProcessMTDWS(status);
            
            break;

        default:
            break;
            
    }
}
/****************************************************************************/
/* NAME			:	i2c_MasterIndexTransmitCheckTxFifo     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Used to check the Transmit fifo during index tx			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/ 
PRIVATE t_i2c_error i2c_MasterIndexTransmitCheckTxFifo(t_i2c_device_id id)
{
	t_uint32            loop_counter = 0;
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

	
	while
	(
		I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_TXFF, I2C_INT_SHIFT_TXFF)
	&&  loop_counter < I2C_ENDAD_COUNTER
	)
	{
		loop_counter++;
	}
	if (loop_counter >= I2C_ENDAD_COUNTER)
	{
		error_status = i2c_GetAbortCause(id);
		if (error_status != I2C_OK)
		{
			return(error_status);
		}
		else
		{
			i2c_Abort(id);
		}

		return(I2C_TRANSMIT_FIFO_FULL);
	}
            
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_MasterIndexTransmitCheckIndex     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Used to determine the Index while transmission			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_error i2c_MasterIndexTransmitCheckIndex(t_i2c_device_id id)
{
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 tfr = 0x00000000;
	
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	 
    switch (g_i2c_system_context[id].index_format)
    {
        case I2C_NO_INDEX:
            if (g_i2c_system_context[id].slave_address <= I2C_MAX_7_BIT_ADDRESS)
            {  
               return(I2C_OK);
            }
            
            break;

        case I2C_BYTE_INDEX:
            /* Checking for the Tx fifo not full */
            error_status = i2c_MasterIndexTransmitCheckTxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            p_i2c_registers->tfr = (0xFF & g_i2c_system_context[id].register_index);

    
            g_i2c_system_context[id].active_event = I2C_INDEX_TX_EVENT;
            g_i2c_system_context[id].index_format = I2C_NO_INDEX;

            break;

        case I2C_HALF_WORD_LITTLE_ENDIAN:
            /* Checking for the Tx Fifo not full  */
            error_status = i2c_MasterIndexTransmitCheckTxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            tfr = (0xFF & (t_uint32) g_i2c_system_context[id].register_index);
    		tfr = (g_i2c_system_context[id].register_index >> 8);
			p_i2c_registers->tfr = tfr; 
    
            g_i2c_system_context[id].index_format = I2C_NO_INDEX;
            g_i2c_system_context[id].active_event = I2C_INDEX_TX_EVENT;
            break;

        case I2C_HALF_WORD_BIG_ENDIAN:
            /* Cheking for the Tx Fifo full */
            error_status = i2c_MasterIndexTransmitCheckTxFifo(id);
            if (error_status != I2C_OK)
            {
                return(error_status);
            }

            tfr = (g_i2c_system_context[id].register_index >> 8);
            tfr = (0xFF & g_i2c_system_context[id].register_index);
			p_i2c_registers->tfr = tfr;

    
            g_i2c_system_context[id].index_format = I2C_NO_INDEX;
            g_i2c_system_context[id].active_event = I2C_INDEX_TX_EVENT;
            break;

        default:
            break;
    }
    
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_ReceiveDataPollingCheckRxFifo     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Check the Rx Fifo while polling							*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_error i2c_ReceiveDataPollingCheckRxFifo(t_i2c_device_id id)
{
	t_uint32        loop_counter = 0;
    t_i2c_error     error_status;
    t_i2c_registers *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

	
	while
	(
		I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_RXFE, I2C_INT_SHIFT_RXFE)
	&&  loop_counter < I2C_ENDAD_COUNTER
	)
	{
		loop_counter++;
	}
	if (loop_counter >= I2C_ENDAD_COUNTER)
	{
		error_status = i2c_GetAbortCause(id);
		if (error_status != I2C_OK)
		{
			return(error_status);
		}
		else
		{
			i2c_Abort(id);
		}

		return(I2C_RECEIVE_FIFO_EMPTY);
	}
            
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_ReceiveDataPollingCheckForSTD     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Check whether the Slave Transaction Done while 			*/
/*					receiving data polling									*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_error i2c_ReceiveDataPollingCheckForSTD(t_i2c_device_id id)
{
	t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;
    t_i2c_error     error_status;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

	
	while
	(
		!I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_STD, I2C_INT_SHIFT_STD)
	&&  loop_counter < I2C_ENDAD_COUNTER
	)
	{
		loop_counter++;
	}
	if (loop_counter >= I2C_ENDAD_COUNTER)
	{
		error_status = i2c_GetAbortCause(id);
		if (error_status != I2C_OK)
		{
			return(error_status);
		}
		else
		{
			i2c_Abort(id);
		}

		return(I2C_INTERNAL_ERROR);
	}
            
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_TransmitDataPollingCheckTxFifo     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Check for the Tx Fifo while Transmitting in Polling mode*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_error i2c_TransmitDataPollingCheckTxFifo(t_i2c_device_id id)
{
	t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;
    t_i2c_error     error_status;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

	while
	(
		I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_TXFF, I2C_INT_SHIFT_TXFF)
	&&  loop_counter < I2C_ENDAD_COUNTER
	)
	{
		loop_counter++;
	}
	if (loop_counter >= I2C_ENDAD_COUNTER)
	{
		error_status = i2c_GetAbortCause(id);
		if (error_status != I2C_OK)
		{
			return(error_status);
		}
		else
		{
			return(I2C_TRANSMIT_FIFO_FULL);
		}
	}
            
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_TransmitDataPollingCheckForSTD     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Check for the STD while Transmitting in Polling mode	*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_error i2c_TransmitDataPollingCheckForSTD(t_i2c_device_id id)
{
	t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;
    t_i2c_error     error_status;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

	while
	(
		!I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_STD, I2C_INT_SHIFT_STD)
	&&  loop_counter < I2C_ENDAD_COUNTER
	)
	{
		loop_counter++;
	}
	if (loop_counter >= I2C_ENDAD_COUNTER)
	{
		error_status = i2c_GetAbortCause(id);
		if (error_status != I2C_OK)
		{
			return(error_status);
		}
		else
		{
			i2c_Abort(id);
		}

		return(I2C_INTERNAL_ERROR);
	}
            
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_RWDataErrorCheck			     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Data Error Check while reading or writing Data			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*				: t_uint16			: Slave Address							*/
/*				: t_i2c_index_format : Index Format							*/ 
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_i2c_error i2c_RWDataErrorCheck(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format)
{	

	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}

    if (!i2c_AddressIsValid(slave_address))
    {
        DBGEXIT0(I2C_SLAVE_ADDRESS_NOT_VALID);
        return(I2C_SLAVE_ADDRESS_NOT_VALID);
    }

    /* Index transfers are only valid in case the Bus Control Mode is not slave*/
    if ((I2C_BUS_MASTER_MODE != g_i2c_system_context[id].bus_control_mode) && (I2C_NO_INDEX != index_format))
    {
        DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
    }    

    /* Check if not busy.*/
    if ((g_i2c_system_context[id].operation != I2C_NO_OPERATION))
    {
        DBGEXIT0(I2C_CONTROLLER_BUSY);
        return(I2C_CONTROLLER_BUSY);
    }
            
    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_RWDataDisableIRQ			     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Disabling the Interrupts while read or write			*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE void i2c_RWDataDisableIRQ(t_i2c_device_id id)
{	
	t_uint32 id_identifier = 0x00000000;
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	/*Disable the IRQ Source Lines*/
	I2C_DisableIRQSrc((t_i2c_irq_src_id)(id_identifier|I2C_IRQ_SRC_ALL));
}
/****************************************************************************/
/* NAME			:	i2c_WriteDataCheckMode			     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Checks the Validity of Data while writing				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*				: t_uint16			: Slave Address							*/
/*				: t_i2c_index_format : Index Format							*/
/*				: t_uint32			: Count of Data							*/ 
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void										    		*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE void i2c_WriteDataCheckMode(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format,t_uint32 count)
{	
    volatile t_uint32   mcr = 0;    
    t_i2c_registers     *p_i2c_registers;
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    if (I2C_BUS_MASTER_MODE == g_i2c_system_context[id].bus_control_mode)
    {
        /* Master control configuration  */

        /* Set the Master write operation */
        HCL_CLEAR_BITS(mcr, I2C_MCR_OP);

        /*  start byte procedure configuration */
        I2C_WRITE_FIELD(mcr, I2C_MCR_SB, I2C_MCR_SHIFT_SB, (t_uint32) g_i2c_system_context[id].start_byte_procedure);

        /* Check the General call handling */
        if (g_i2c_system_context[id].general_call_mode_handling != I2C_NO_GENERAL_CALL_HANDLING)
        {
            /* The Transaction is intiated by a general call command */
            I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 0);
        }
        else
        {
            /* Check if Slave address is 10 bit */
            if (g_i2c_system_context[id].slave_address <= I2C_MAX_10_BIT_ADDRESS && g_i2c_system_context[id].slave_address > I2C_MAX_7_BIT_ADDRESS)
            {
                /* Set the Address mode to 10 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 2);
            }
            else
            {
                /* Set the Address mode to 7 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 1);
            }
        }

        /* Store the HS master code */
        if (I2C_FREQ_MODE_HIGH_SPEED == g_i2c_system_context[id].mode)
        {
            p_i2c_registers->hsmcr = g_i2c_system_context[id].high_speed_master_code;
        }

        /* Store  the Slave addres in the Master control register */
        I2C_WRITE_FIELD(mcr, I2C_MCR_A10, I2C_MCR_SHIFT_A10, slave_address);

        /* Configure the STOP condition*/
        /* Current transaction is terminated by STOP condition */
        HCL_SET_BITS(mcr, I2C_MCR_STOP);

        /* Configuring the Frame length */
        switch (index_format)
        {
            case I2C_NO_INDEX:
                I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, count);
                break;

            case I2C_BYTE_INDEX:
                I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, (count + 1));
                break;

            case I2C_HALF_WORD_LITTLE_ENDIAN:
            case I2C_HALF_WORD_BIG_ENDIAN:
                I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, (count + 2));
                break;

            default:
                break;
        }

        /*Write the MCR register  */
        p_i2c_registers->mcr = mcr;

    }

}
/****************************************************************************/
/* NAME			:	i2c_WriteDataEnableIRQ			     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Enabling of interrupt Request while writing				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE void i2c_WriteDataEnableIRQ(t_i2c_device_id id)
{
	
	t_uint32 id_identifier = 0x00000000;
	t_i2c_irq_src_id irq_write;
	/* Write the Device Id to Mask with the Interrupts */
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	/*Enable the Interrupts*/
	irq_write = (t_i2c_irq_src_id) (I2C_MULTIPLE_WRITE | id_identifier | I2C_IRQ_SRC_READ_FROM_SLAVE_EMPTY);
	
	if(I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
	{
		HCL_CLEAR_BITS(irq_write,I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY);
	}
	I2C_EnableIRQSrc(irq_write);
    	
}
/****************************************************************************/
/* NAME			:	i2c_ReadDataCheckMode			     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Checking the Validity of data Read						*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE void i2c_ReadDataCheckMode(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format, t_uint32 count)
{	
    volatile t_uint32   mcr = 0;    
    t_i2c_registers     *p_i2c_registers;
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    if (I2C_BUS_MASTER_MODE == g_i2c_system_context[id].bus_control_mode)
    {

        /*  start byte procedure configuration */
        I2C_WRITE_FIELD(mcr, I2C_MCR_SB, I2C_MCR_SHIFT_SB, (t_uint32) g_i2c_system_context[id].start_byte_procedure);

        /* Check the General call handling */
        if (g_i2c_system_context[id].general_call_mode_handling != I2C_NO_GENERAL_CALL_HANDLING)
        {
            /* The Transaction is intiated by a general call command */
            I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 0);
        }
        else
        {
            /* Check if Slave address is 10 bit */
            if (g_i2c_system_context[id].slave_address <= I2C_MAX_10_BIT_ADDRESS && g_i2c_system_context[id].slave_address > I2C_MAX_7_BIT_ADDRESS)
            {
                /* Set the Address mode to 10 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 2);
            }
            else
            {
                /* Set the Address mode to 7 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 1);
            }
        }

        /* Store the HS master code */
        if (I2C_FREQ_MODE_HIGH_SPEED == g_i2c_system_context[id].mode)
        {
            p_i2c_registers->hsmcr = g_i2c_system_context[id].high_speed_master_code;
        }

        /* Store  the Slave addres in the Master control register */
        I2C_WRITE_FIELD(mcr, I2C_MCR_A10, I2C_MCR_SHIFT_A10, slave_address);

        if
        (
            (g_i2c_system_context[id].slave_address <= I2C_MAX_10_BIT_ADDRESS && g_i2c_system_context[id].slave_address > I2C_MAX_7_BIT_ADDRESS)
        &&  (I2C_NO_INDEX == index_format)
        )
        {
            /* Set the Master write operation */
            HCL_CLEAR_BITS(mcr, I2C_MCR_OP);

            I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, 0);

            /* Current transaction is not terminated by STOP condition,
                 a repeated start operation will be fallowed */
            HCL_CLEAR_BITS(mcr, I2C_MCR_STOP);

            /*Write MCR register   */
            p_i2c_registers->mcr = mcr;

            /* Enable the I2C controller */
            HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
        }
        else
        {
            /* Master control configuration  */
            if (I2C_NO_INDEX != index_format)
            {
                /* Set the Master write operation */
                HCL_CLEAR_BITS(mcr, I2C_MCR_OP);
            }
            else
            {
                /* Set the Master read operation */
                HCL_SET_BITS(mcr, I2C_MCR_OP);
            }

            /* Configuring the Frame length */
            switch (index_format)
            {
                case I2C_NO_INDEX:
                    I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, count);

                    /* Current transaction is terminated by STOP condition */
                    HCL_SET_BITS(mcr, I2C_MCR_STOP);
                    break;

                case I2C_BYTE_INDEX:
                    I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, 1);

                    /* Current transaction is not terminated by STOP condition,
	                 a repeated start operation will be fallowed */
                    HCL_CLEAR_BITS(mcr, I2C_MCR_STOP);
                    break;

                case I2C_HALF_WORD_LITTLE_ENDIAN:
                case I2C_HALF_WORD_BIG_ENDIAN:
                    I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, 2);

                    /* Current transaction is not terminated by STOP condition,
	                a repeated start operation will be fallowed */
                    HCL_CLEAR_BITS(mcr, I2C_MCR_STOP);
                    break;

                default:
                    break;
            }

            /*Write MCR register   */
            p_i2c_registers->mcr = mcr;

        }
    }

}
/****************************************************************************/
/* NAME			:	i2c_ReadDataEnableIRQ			     		        	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	Enabling the interrupts while reading data				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The i2c controller in use				*/
/*				: t_i2c_index_format : The index format						*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None                                                     */
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/

PRIVATE void i2c_ReadDataEnableIRQ(t_i2c_device_id id, t_i2c_index_format index_format)
{	
	
	t_uint32 id_identifier = 0x00000000;
	t_i2c_irq_src_id irq_read;
	/*Write the Id value in the bits 7,8,9*/
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	/*Interrupt Enabling for Read contains only values*/
	irq_read = (t_i2c_irq_src_id) (I2C_MULTIPLE_READ | id_identifier );
	if (index_format > I2C_NO_INDEX)
	{
		
		irq_read |= (t_i2c_irq_src_id)((t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY |
											   (t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_OVERRUN);                                                
	}
	I2C_EnableIRQSrc(irq_read);	   
}

#if (defined __I2C_GENERIC)

/****************************************************************************/
/* NAME			:	I2C_StartWriteRequest                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to configure mcr register for starting*/
/*               write request                                              */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The address of the slave to be accessed      */
/*                t_unit32   : no of bytes to be transfered                 */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_SLAVE_ADDRESS_NOT_VALID  If requested slave address   */
/*                                      is not valid                        */
/*                I2C_CONTROLLER_BUSY    if I2C controller is busy          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_StartWriteRequest
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,   
    IN t_uint32             count
)
{
	volatile t_uint32   mcr = 0;
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
	t_uint32 id_identifier = 0x00000000;
    DBGENTER3
    (
        "Id is %d, Address is %x, Data count is %d",
        id,
        slave_address,        
        count        
    );

    /* Check if parameters are valid.*/
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}

    /* Index transfers are only valid in case the Bus Control Mode is not slave*/
    if ((I2C_BUS_MASTER_MODE != g_i2c_system_context[id].bus_control_mode))
    {
        DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
    }

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

    /* Check if not busy.*/
    if ((g_i2c_system_context[id].operation != I2C_NO_OPERATION))
    {
        DBGEXIT0(I2C_CONTROLLER_BUSY);
        return(I2C_CONTROLLER_BUSY);
    }

    /* Save parameters.*/
    g_i2c_system_context[id].slave_address = slave_address;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].count_data = count;
    g_i2c_system_context[id].index_format = I2C_NO_INDEX;
    g_i2c_system_context[id].operation = I2C_WRITE;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = TRUE;

	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
    /* Disable all the interrupts to remove previously garbage interrupts */
	I2C_DisableIRQSrc((t_i2c_irq_src_id)(id_identifier|I2C_IRQ_SRC_ALL));
    
    /* Check if I2C controller is Master */
    if (I2C_BUS_MASTER_MODE == g_i2c_system_context[id].bus_control_mode)
    {
        /* Master control configuration  */

        /* Set the Master write operation */
        HCL_CLEAR_BITS(mcr, I2C_MCR_OP);

        /*  start byte procedure configuration */
        I2C_WRITE_FIELD(mcr, I2C_MCR_SB, I2C_MCR_SHIFT_SB, (t_uint32) g_i2c_system_context[id].start_byte_procedure);

        /* Check the General call handling */
        if (g_i2c_system_context[id].general_call_mode_handling != I2C_NO_GENERAL_CALL_HANDLING)
        {
            /* The Transaction is intiated by a general call command */
            I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 0);
        }
        else
        {
            /* Check if Slave address is 10 bit */
            if (g_i2c_system_context[id].slave_address <= I2C_MAX_10_BIT_ADDRESS && g_i2c_system_context[id].slave_address > I2C_MAX_7_BIT_ADDRESS)
            {
                /* Set the Address mode to 10 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 2);
            }
            else
            {
                /* Set the Address mode to 7 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 1);
            }
        }

        /* Store the HS master code */
        if (I2C_FREQ_MODE_HIGH_SPEED == g_i2c_system_context[id].mode)
        {
            p_i2c_registers->hsmcr = g_i2c_system_context[id].high_speed_master_code;
        }

        /* Store  the Slave addres in the Master control register */
        I2C_WRITE_FIELD(mcr, I2C_MCR_A10, I2C_MCR_SHIFT_A10, slave_address);

        /* Configure the STOP condition*/
        /* Current transaction is terminated by STOP condition */
        HCL_SET_BITS(mcr, I2C_MCR_STOP);

        /* Configuring the Frame length */
        I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, count);        

        p_i2c_registers->mcr = mcr;
    }
    
    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME			:	I2C_TransmitBuffer                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to write a multiple data byte to      */
/*                a receiver.                                               */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint8*   : The data buffer to be written to the         */
/*                             slave device                                 */
/*                t_unit32   : no of bytes to be transfered                 */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_TransmitBuffer
(
    IN t_i2c_device_id      id,    
    IN t_uint8              *p_data,
    IN t_uint32             count
)
{
	
    t_i2c_error         error_status = I2C_OK;
    DBGENTER3
    (
        "Id is %d, Data count is %d and @ is %p",
        id,        
        count,
        (void *) p_data
    );

    /* Check if parameters are valid.*/

	if((id > (t_i2c_device_id) I2C_MAX_ID) || (NULL == p_data))
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}

    /* Save parameters.*/
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].databuffer = p_data;
    g_i2c_system_context[id].count_data = count;
    g_i2c_system_context[id].operation = I2C_WRITE;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = TRUE;

   
    switch (g_i2c_system_context[id].index_transfer_mode)
    {
        case I2C_TRANSFER_MODE_POLLING:
            
            if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
            {
                error_status = i2c_SlaveIndexReceive(id);
                if (I2C_OK != error_status)
                {
                    DBGEXIT0(error_status);
                    return(error_status);
                }
            }
            
            /*
         Buffer Transfer
         */
            error_status = i2c_TransmitBufferDataTransfer(id);
            if (I2C_OK != error_status)
            {
                DBGEXIT0(error_status);
                return(error_status);
            }
            break;

        case I2C_TRANSFER_MODE_INTERRUPT:
            /* Enable the interrupts */
            i2c_WriteDataEnableIRQ(id);
            break;

        case I2C_TRANSFER_MODE_DMA:
        default:
            DBGEXIT0(I2C_INVALID_PARAMETER);
            return(I2C_INVALID_PARAMETER);
    }

    DBGEXIT0(error_status);
    return(error_status);

}

/****************************************************************************/
/* NAME			:	I2C_EndWriteRequest                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to end write request                  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INTERNAL_ERROR     if STD/MTD not triggered           */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_EndWriteRequest
(
    IN t_i2c_device_id      id    
)
{
    t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;
    t_i2c_error     error_status;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
    
    if((I2C_TRANSFER_MODE_POLLING == g_i2c_system_context[id].index_transfer_mode) || (I2C_TRANSFER_MODE_POLLING == g_i2c_system_context[id].data_transfer_mode))
    {
    	if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
        {        
            /* Check for the Slave tranaction done */        
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_STD, I2C_INT_SHIFT_STD)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
            if (loop_counter >= I2C_ENDAD_COUNTER)
            {
                error_status = i2c_GetAbortCause(id);
                if (error_status != I2C_OK)
                {
                    return(error_status);
                }
                else
                {
                    i2c_Abort(id);
                }

                return(I2C_INTERNAL_ERROR);
            }

            /* Slave Transaction has been done */
            /* Acknowledge the Slave Transaction done */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_STD);

            g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
            return(I2C_OK);
        }
        else
        {        
            /* Check whether the Stop bit has been programmed or not */
            if(I2C_READ_FIELD(p_i2c_registers->mcr, I2C_MCR_STOP, I2C_MCR_SHIFT_STOP))
            {
                /* Check for the Master transaction Done */
                while
                (
                    !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTD, I2C_INT_SHIFT_MTD)
                &&  loop_counter < I2C_ENDAD_COUNTER
                )
                {
                    loop_counter++;
                }
            }
            else
            {
        	    /* Check for the Master transaction Done Without Stop */
                while
                (
                    !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTDWS, I2C_INT_SHIFT_MTDWS)
                &&  loop_counter < I2C_ENDAD_COUNTER
                )
                {
                    loop_counter++;
                }
            }
        
        
            if (loop_counter >= I2C_ENDAD_COUNTER)
            {
                error_status = i2c_GetAbortCause(id);
                if (error_status != I2C_OK)
                {
                    i2c_Abort(id);
                    return(error_status);
                }
                else
                {
                    i2c_Abort(id);
                }

                return(I2C_INTERNAL_ERROR);
            }
        
        
            /* Master Transaction has been done */
            /* Acknowledge the Master Transaction Done */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTD);
        
            /* Master Transaction Without Stop has been done */
            /* Acknowledge the Master Transaction Done Without Stop */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTDWS);
        
        
            g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;

            g_i2c_system_context[id].operation = I2C_NO_OPERATION;
            return(I2C_OK);
        }
    }

    return(I2C_OK);

}

/****************************************************************************/
/* NAME			:	I2C_StartReadRequest                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to read a multiple  data byte from    */
/*                a transmitter. Read can be done from  a slave device by   */
/*               using the indexed modes.                                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint16   : The address of the slave to be accessed      */
/*                t_unit32   : no of bytes to be transfered                 */
/*                t_uint8*   : The data buffer to be written to the         */
/*                             slave device (for writing index              */
/*                t_unit8   :  count of index                               */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*                I2C_SLAVE_ADDRESS_NOT_VALID  If requested slave address   */
/*                                      is not valid                        */
/*                I2C_CONTROLLER_BUSY    if I2C controller is busy          */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_StartReadRequest
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,   
    IN t_uint32             count_totaldata,
    IN t_uint8              *p_data,
    IN t_uint8              count_index
)
{	
    
    t_i2c_error         error_status = I2C_OK;
    t_i2c_index_format  index_format;
    volatile t_uint16   index_value = 0;
    t_uint32            count;
/*    t_i2c_registers     *p_i2c_registers; */
    t_uint8             temp_data;
    
    DBGENTER5
    (
        "Id is %d, Address is %x, Index count is %d, Data count is %d and @ is %p",
        id,
        slave_address,
        count_index,      
        count_totaldata,
        (void *) p_data
    );

    /* Check if parameters are valid.*/ 
	if((id > (t_i2c_device_id) I2C_MAX_ID) || (NULL == p_data))
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}

    if (!i2c_AddressIsValid(slave_address))
    {
        DBGEXIT0(I2C_SLAVE_ADDRESS_NOT_VALID);
        return(I2C_SLAVE_ADDRESS_NOT_VALID);
    }
    
    count = count_totaldata - count_index;
    
    if(0 == count_index)
    {
    	index_format = I2C_NO_INDEX;
    }
    else if(1 == count_index)
    {
    	index_format = I2C_BYTE_INDEX;
    	index_value  = *p_data;    	
    }
    else if(2 == count_index)
    {
    	index_format = I2C_HALF_WORD_LITTLE_ENDIAN;
    	temp_data    = *p_data;
    	index_value  = (t_uint16)((temp_data << 8) | (*(p_data++)));
    }
    else
    {
    	return(I2C_INVALID_PARAMETER);
    }

    /* Index transfers are only valid in case the Bus Control Mode is not slave*/
    if ((I2C_BUS_MASTER_MODE != g_i2c_system_context[id].bus_control_mode) && (I2C_NO_INDEX != index_format))
    {
        DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
    }

/*    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers; */

    /* Check if not busy.*/
    if ((g_i2c_system_context[id].operation != I2C_NO_OPERATION))
    {
        DBGEXIT0(I2C_CONTROLLER_BUSY);
        return(I2C_CONTROLLER_BUSY);
    }

    /* Save parameters.*/
    g_i2c_system_context[id].slave_address = slave_address;
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].register_index = index_value;
    g_i2c_system_context[id].index_format = index_format;
    g_i2c_system_context[id].databuffer = p_data;
    g_i2c_system_context[id].count_data = count;
    g_i2c_system_context[id].operation = I2C_READ;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = TRUE;

    /* Disable all the interrupts to remove previously garbage interrupts */
    i2c_StartReadRequestDisableIRQ(id);

    /* Check if I2C controller is Master */
    i2c_StartReadRequestCheckMode(id, slave_address, index_format, count);
    
      /* Index Transfer */  
    switch (g_i2c_system_context[id].index_transfer_mode)
    {
        case I2C_TRANSFER_MODE_POLLING:
        
            if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
            {
                error_status = i2c_SlaveIndexReceive(id);
                if (I2C_OK != error_status)
                {
                    DBGEXIT0(error_status);
                    return(error_status);
                }
            }
            else
            { 
                error_status = i2c_MasterIndexTransmit(id);
                if (I2C_OK != error_status)
                {
                    DBGEXIT0(error_status);
                    return(error_status);
                }
            }

            break;

        case I2C_TRANSFER_MODE_INTERRUPT:
            /* enable the interrupts */
            i2c_WriteDataEnableIRQ(id);            
            break;
        
        case I2C_TRANSFER_MODE_DMA:
        default:
            DBGEXIT0(I2C_INVALID_PARAMETER);
            return(I2C_INVALID_PARAMETER);

    }
        
        DBGEXIT0(error_status);
        return(error_status);
}

/****************************************************************************/
/* NAME			:	I2C_ReceiveBuffer                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to read a multiple data byte from     */
/*                a slave.                                                  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*                t_uint8*   : The data buffer to be read from the          */
/*                             slave device                                 */
/*                t_unit32   : no of bytes to be read                       */
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INVALID_PARAMETER  if input parameters are not valid  */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_ReceiveBuffer
(
    IN t_i2c_device_id      id,    
    IN t_uint8              *p_data,
    IN t_uint32             count
)
{
	volatile t_uint32   mcr = 0;
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
    DBGENTER3
    (
        "Id is %d, Data count is %d and @ is %p",
        id,        
        count,
        (void *) p_data
    );

    /* Check if parameters are valid.*/
	if((id > (t_i2c_device_id) I2C_MAX_ID) || (NULL == p_data))
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
       
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    /* Save parameters.*/
    g_i2c_system_context[id].status = I2C_STATUS_SLAVE_MODE;
    g_i2c_system_context[id].databuffer = p_data;
    g_i2c_system_context[id].count_data = count;
    g_i2c_system_context[id].operation = I2C_READ;
    g_i2c_system_context[id].active_event = I2C_NO_EVENT;
    g_i2c_system_context[id].transfer_data = 0;
    g_i2c_system_context[id].multi_operation = TRUE;

    /*
    Data Transfer
    */
    switch (g_i2c_system_context[id].data_transfer_mode)
    {
        case I2C_TRANSFER_MODE_POLLING:
            error_status = i2c_ReceiveBufferPolling(id, g_i2c_system_context[id].databuffer);
            if (I2C_OK != error_status)
            {
                DBGEXIT0(error_status);
                return(error_status);
            }
            break;

        case I2C_TRANSFER_MODE_INTERRUPT:
            /* enable the interrupts - Already enabled in I2C_StartReadRequest API if index transfer mode is Interrupt */
            /* g_i2c_system_context[id].operation = I2C_NO_OPERATION; */
            break;

        case I2C_TRANSFER_MODE_DMA:
            /* Configure the DMA */
            /* enable the DMA Rx interface */
            HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_DMA_RX_EN);
            g_i2c_system_context[id].operation = I2C_NO_OPERATION;
            break;

        default:
            break;
    }        

    DBGEXIT0(error_status);
    return(error_status);

}

/****************************************************************************/
/* NAME			:	I2C_EndReadRequest                  		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to end read request                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*                I2C_OK                 if it is ok                        */
/*                I2C_INTERNAL_ERROR     if STD/MTD not triggered           */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PUBLIC t_i2c_error I2C_EndReadRequest
(
    IN t_i2c_device_id      id
)
{
	t_uint32        loop_counter = 0;
    t_i2c_error     error_status;
    t_i2c_registers *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	
	if(id > (t_i2c_device_id) I2C_MAX_ID)
	{
		DBGEXIT0(I2C_INVALID_PARAMETER);
        return(I2C_INVALID_PARAMETER);
	}
    
    if((I2C_TRANSFER_MODE_POLLING == g_i2c_system_context[id].index_transfer_mode) || (I2C_TRANSFER_MODE_POLLING == g_i2c_system_context[id].data_transfer_mode))
    {
    	if (I2C_BUS_SLAVE_MODE == g_i2c_system_context[id].bus_control_mode)
        {
            /* Slave Receiver */        
            /* Check for the slave transaction done */        
            while
            (
                !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_STD, I2C_INT_SHIFT_STD)
            &&  loop_counter < I2C_ENDAD_COUNTER
            )
            {
                loop_counter++;
            }
            if (loop_counter >= I2C_ENDAD_COUNTER)
            {
                error_status = i2c_GetAbortCause(id);
                if (error_status != I2C_OK)
                {
                    return(error_status);
                }
                else
                {
                    i2c_Abort(id);
                }

                return(I2C_INTERNAL_ERROR);
            }

            /* Slave Transaction has been done */
            /* Acknowledge the Slave Transaction Done */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_STD);
            g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
            g_i2c_system_context[id].operation = I2C_NO_OPERATION;

            return(I2C_OK);
        }
        else
        {
            /* Master Receiver */        
            /* Check whether the Stop bit has been programmed or not */
            if(I2C_READ_FIELD(p_i2c_registers->mcr, I2C_MCR_STOP, I2C_MCR_SHIFT_STOP))
            {
                /* Check for the Master transaction Done */
                while
                (
                    !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTD, I2C_INT_SHIFT_MTD)
                &&  loop_counter < I2C_ENDAD_COUNTER
                )
                {
                    loop_counter++;
                }
            }
            else
            {
        	    /* Check for the Master transaction Done Without Stop */
                while
                (
                    !I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_MTDWS, I2C_INT_SHIFT_MTDWS)
                &&  loop_counter < I2C_ENDAD_COUNTER
                )
                {
                    loop_counter++;
                }
            }
            if (loop_counter >= I2C_ENDAD_COUNTER)
            {
                error_status = i2c_GetAbortCause(id);
                if (error_status != I2C_OK)
                {
                    return(error_status);
                }
                else
                {
                    i2c_Abort(id);
                }

                return(I2C_INTERNAL_ERROR);
            }

            /* Master Transaction has been done */
            /* Acknowledge the Master Transaction Done */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTD);
        
            /* Master Transaction Without Stop has been done */
            /* Acknowledge the Master Transaction Done Without Stop */
            HCL_SET_BITS(p_i2c_registers->icr, I2C_INT_MTDWS);

            g_i2c_system_context[id].active_event = I2C_TRANSFER_OK_EVENT;
            g_i2c_system_context[id].operation = I2C_NO_OPERATION;
        
            return(I2C_OK);
        }
    }

    return(I2C_OK);
    
}

/****************************************************************************/
/* NAME			:	i2c_TransmitBufferPolling              		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to Transmit Buffer in Polling	        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*				  t_uint8 *			: Pointer to the Data					*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
t_i2c_error i2c_TransmitBufferPolling(t_i2c_device_id id, volatile t_uint8 *p_data)
{

    t_uint32        loop_counter = 0;
    t_i2c_registers *p_i2c_registers;
    t_i2c_error     error_status;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	while (g_i2c_system_context[id].count_data != 0)
	{
		/* Check for Tx Fifo not full */
		loop_counter = 0;
		while
		(
			I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_TXFF, I2C_INT_SHIFT_TXFF)
		&&  loop_counter < I2C_ENDAD_COUNTER
		)
		{
			loop_counter++;
		}
		if (loop_counter >= I2C_ENDAD_COUNTER)
		{
			error_status = i2c_GetAbortCause(id);
			if (error_status != I2C_OK)
			{
				return(error_status);
			}
			else
			{
				i2c_Abort(id);
			}

			return(I2C_TRANSMIT_FIFO_FULL);
		}

		p_i2c_registers->tfr = *p_data;

		g_i2c_system_context[id].transfer_data++;
		g_i2c_system_context[id].count_data--;
		p_data++;
		g_i2c_system_context[id].active_event = I2C_DATA_TX_EVENT;
	}

    return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_ReceiveBufferPolling              		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to Receive Buffer in Polling	        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*				  t_uint8 *			: Pointer to the Data					*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
t_i2c_error i2c_ReceiveBufferPolling(t_i2c_device_id id, t_uint8 *p_data)
{

    t_uint32        loop_counter = 0;
    t_i2c_error     error_status;
    t_i2c_registers *p_i2c_registers;

    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;

	while (g_i2c_system_context[id].count_data != 0)
	{
		/* Wait for the Rx Fifo  empty */
		loop_counter = 0;
		while
		(
			I2C_READ_FIELD(p_i2c_registers->risr, I2C_INT_RXFE, I2C_INT_SHIFT_RXFE)
		&&  loop_counter < I2C_ENDAD_COUNTER
		)
		{
			loop_counter++;
		}
		if (loop_counter >= I2C_ENDAD_COUNTER)
		{
			error_status = i2c_GetAbortCause(id);
			if (error_status != I2C_OK)
			{
				return(error_status);
			}
			else
			{
				i2c_Abort(id);
			}

			return(I2C_RECEIVE_FIFO_EMPTY);
		}

		/* Read the data byte from Rx Fifo */
		*p_data = (t_uint8) p_i2c_registers->rfr;

		g_i2c_system_context[id].transfer_data++;
		g_i2c_system_context[id].count_data--;
		p_data++;
		g_i2c_system_context[id].active_event = I2C_DATA_RX_EVENT;
	}   /* Data Reception has been completed */
	
	return(I2C_OK);
}
/****************************************************************************/
/* NAME			:	i2c_TransmitBufferDataTransfer             		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to Transfer Data					    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	t_i2c_error										    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PRIVATE t_i2c_error i2c_TransmitBufferDataTransfer(t_i2c_device_id id)
{
    t_i2c_error         error_status = I2C_OK;
    t_i2c_registers     *p_i2c_registers;
    t_uint32 id_identifier = 0x00000000;
	t_i2c_irq_src_id irq_write;
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
	
	
	/*Enable the Interrupts for Writing the Data*/
	
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	irq_write = (t_i2c_irq_src_id) (I2C_MULTIPLE_WRITE|id_identifier);
	
    
    switch (g_i2c_system_context[id].data_transfer_mode)
            {
                case I2C_TRANSFER_MODE_POLLING:
                    error_status = i2c_TransmitBufferPolling(id, g_i2c_system_context[id].databuffer);
                    if (I2C_OK != error_status)
                    {
                        DBGEXIT0(error_status);
                        return(error_status);
                    }
                    break;

                case I2C_TRANSFER_MODE_INTERRUPT:
                    /* Enable the interrupts */
					I2C_EnableIRQSrc(irq_write);
                    break;

                case I2C_TRANSFER_MODE_DMA:
                    /* Configure the DMA */
                    /* enable the DMA Tx interface */
                    HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_DMA_TX_EN);
                    g_i2c_system_context[id].operation = I2C_NO_OPERATION;
                    break;

                default:
                    break;
            } 
            
    return(I2C_OK);       	
}
/****************************************************************************/
/* NAME			:	i2c_StartReadRequestDisableIRQ             		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to Disable IRQ while reading data	    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PRIVATE void i2c_StartReadRequestDisableIRQ(t_i2c_device_id id)
{	
	t_uint32 id_identifier = 0x00000000;
	id_identifier = ((id_identifier | id) << I2CID_SHIFT);
	/*Disable the interrupt for the specefic Controller*/
	I2C_DisableIRQSrc((t_i2c_irq_src_id)(id_identifier|I2C_IRQ_SRC_ALL));
    
}
/****************************************************************************/
/* NAME			:	i2c_StartReadRequestCheckMode             		        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	:This routine is used to check the validity of data		    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_i2c_device_id 	: The controller to be initialized		*/
/*				: t_uint16			: Slave Address							*/
/*				: t_i2c_index_format: Index Format							*/
/*				: t_uint32			: Count of the Data being transferred	*/
/*     InOut    :  None                                                     */
/* 		OUT 	:  None														*/
/*                                                                          */
/* RETURN		:	void											    	*/
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/
/****************************************************************************/
PRIVATE void i2c_StartReadRequestCheckMode(t_i2c_device_id id, t_uint16 slave_address, t_i2c_index_format index_format, t_uint32 count)
{	
    volatile t_uint32   mcr = 0;    
    t_i2c_registers     *p_i2c_registers;
    
    p_i2c_registers = (t_i2c_registers *) g_i2c_system_context[id].p_i2c_registers;
    
    if (I2C_BUS_MASTER_MODE == g_i2c_system_context[id].bus_control_mode)
    {

        /*  start byte procedure configuration */
        I2C_WRITE_FIELD(mcr, I2C_MCR_SB, I2C_MCR_SHIFT_SB, (t_uint32) g_i2c_system_context[id].start_byte_procedure);

        /* Check the General call handling */
        if (g_i2c_system_context[id].general_call_mode_handling != I2C_NO_GENERAL_CALL_HANDLING)
        {
            /* The Transaction is intiated by a general call command */
            I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 0);
        }
        else
        {
            /* Check if Slave address is 10 bit */
            if (g_i2c_system_context[id].slave_address <= I2C_MAX_10_BIT_ADDRESS && g_i2c_system_context[id].slave_address > I2C_MAX_7_BIT_ADDRESS)
            {
                /* Set the Address mode to 10 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 2);
            }
            else
            {
                /* Set the Address mode to 7 bit */
                I2C_WRITE_FIELD(mcr, I2C_MCR_AM, I2C_MCR_SHIFT_AM, 1);
            }
        }

        /* Store the HS master code */
        if (I2C_FREQ_MODE_HIGH_SPEED == g_i2c_system_context[id].mode)
        {
            p_i2c_registers->hsmcr = g_i2c_system_context[id].high_speed_master_code;
        }

        /* Store  the Slave addres in the Master control register */
        I2C_WRITE_FIELD(mcr, I2C_MCR_A10, I2C_MCR_SHIFT_A10, slave_address);

        if
        (
            (g_i2c_system_context[id].slave_address <= I2C_MAX_10_BIT_ADDRESS && g_i2c_system_context[id].slave_address > I2C_MAX_7_BIT_ADDRESS)
        &&  (I2C_NO_INDEX == index_format)
        )
        {
            /* Set the Master write operation */
            HCL_CLEAR_BITS(mcr, I2C_MCR_OP);

            I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, 0);

            /* Current transaction is not terminated by STOP condition,
                 a repeated start operation will be fallowed */
            HCL_CLEAR_BITS(mcr, I2C_MCR_STOP);

            /*Write MCR register   */
            p_i2c_registers->mcr = mcr;

            /* Enable the I2C controller */
            HCL_SET_BITS(p_i2c_registers->cr, I2C_CR_PE);
        }
        else
        {
            /* Master control configuration  */
            if (I2C_NO_INDEX != index_format)
            {
                /* Set the Master write operation */
                HCL_CLEAR_BITS(mcr, I2C_MCR_OP);
            }
            else
            {
                /* Set the Master read operation */
                HCL_SET_BITS(mcr, I2C_MCR_OP);
            }

            /* Configuring the Frame length */
            switch (index_format)
            {
                case I2C_NO_INDEX:
                    I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, count);

                    /* Current transaction is terminated by STOP condition */
                    HCL_SET_BITS(mcr, I2C_MCR_STOP);
                    break;

                case I2C_BYTE_INDEX:
                    I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, 1);

                    /* Current transaction is not terminated by STOP condition,
	                 a repeated start operation will be fallowed */
                    HCL_CLEAR_BITS(mcr, I2C_MCR_STOP);
                    break;

                case I2C_HALF_WORD_LITTLE_ENDIAN:
                case I2C_HALF_WORD_BIG_ENDIAN:
                    I2C_WRITE_FIELD(mcr, I2C_MCR_LENGTH, I2C_MCR_SHIFT_LENGTH, 2);

                    /* Current transaction is not terminated by STOP condition,
	                a repeated start operation will be fallowed */
                    HCL_CLEAR_BITS(mcr, I2C_MCR_STOP);
                    break;

                default:
                    break;
            }

            /*Write MCR register   */
            p_i2c_registers->mcr = mcr;

        }
    }

}

#endif /* End of __I2C_GENERIC */

