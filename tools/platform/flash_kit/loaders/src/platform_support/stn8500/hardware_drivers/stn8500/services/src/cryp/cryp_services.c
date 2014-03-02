//*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides CRYP Initialization services
*  Specification release related to this implementation: A_V2.2
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#include "cryp_services.h"
#include <stdlib.h>

t_callback g_cryp_callback;


/****************************************************************************/
/* NAME:    SER_CRYP_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This provides interrupt services for CRYP, interrupt        */
/*                processing is done by the callback function.				*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_CRYP0_InterruptHandler(t_uint32 t_param)
{
    t_gic_error         gic_error;
    t_ser_cryp_param    ser_cryp_param;
        
    gic_error = GIC_DisableItLine(GIC_CRYP_0_LINE);	
	if (GIC_OK != gic_error)
	{
	    PRINT("GIC_DisableItLine::Error in Procesing Interrupt for CRYPT0");
	    exit(-1);
	} 

    if (NULL != g_cryp_callback.fct)
    {                
        ser_cryp_param.irq_src = CRYP_GetIRQSrc(CRYP_DEVICE_ID_0);
        ser_cryp_param.cryp_status = CRYP_GetStatus(CRYP_DEVICE_ID_0);
        ser_cryp_param.cryp_device_id = CRYP_DEVICE_ID_0;
        
        g_cryp_callback.fct((void *)g_cryp_callback.param,(void *)&ser_cryp_param);
    }
    else
    {   
        PRINT("!!NO callback register hence no processing of the interrupt!!!");
    }

    gic_error = GIC_EnableItLine(GIC_CRYP_0_LINE);	
	if (GIC_OK != gic_error)
	{
	    PRINT("GIC_EnableItLine::Error in Procesing Interrupt for CRYPT0");
	    exit(-1);
	} 
    
}

/****************************************************************************/
/* NAME:    SER_CRYP_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This provides interrupt services for CRYP, interrupt        */
/*                processing is done by the callback function.				*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_CRYP1_InterruptHandler(t_uint32 t_param)
{
    t_gic_error         gic_error;
    t_ser_cryp_param    ser_cryp_param;


    gic_error = GIC_DisableItLine(GIC_CRYP_1_LINE);	
	if (GIC_OK != gic_error)
	{
	    PRINT("GIC_DisableItLine::Error in Procesing Interrupt for CRYPT1");
	    exit(-1);
	} 


    if (NULL != g_cryp_callback.fct)
    {                
        ser_cryp_param.irq_src = CRYP_GetIRQSrc(CRYP_DEVICE_ID_1);
        ser_cryp_param.cryp_status = CRYP_GetStatus(CRYP_DEVICE_ID_1);
        ser_cryp_param.cryp_device_id = CRYP_DEVICE_ID_1;
        
        g_cryp_callback.fct((void *)g_cryp_callback.param,(void *)&ser_cryp_param);
    }
    else
    {   
        PRINT("!!NO callback register hence no processing of the interrupt!!!");
    }     


    gic_error = GIC_EnableItLine(GIC_CRYP_1_LINE);	
	if (GIC_OK != gic_error)
	{
	    PRINT("GIC_EnableItLine::Error in Procesing Interrupt for CRYPT0");
	    exit(-1);
	} 

    
}

/****************************************************************************/
/* NAME:    SER_CRYP_Init                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This provides initialization services for CRYP				*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/

PUBLIC void SER_CRYP_Init(t_uint8 cryp_ser_mask) 
{	
	t_cryp_error 	error;
	t_gic_error     error_gic;
	t_gic_func_ptr   p_old_handler;    

    if(INIT_CRYP0 == cryp_ser_mask)
    {
#if ((defined ST_8500V1) || (defined ST_HREFV1) || (defined ST_8500V2) || (defined ST_HREFV2))
    	*((volatile t_uint32 *)PRCC_6_CTRL_REG_BASE_ADDR) |= 0x00000002;
        *((volatile t_uint32 *)(PRCC_6_CTRL_REG_BASE_ADDR + 0x08)) |= 0x00000002;
#endif
    	/* Set base address for the CRYP  */
		error = CRYP_Init(CRYP_DEVICE_ID_0,CRYP_0_REG_BASE_ADDR);
		if(CRYP_OK != error)
		{
			PRINT("CRYP0 INIT FAILED");
		}
		else
		{
	    	CRYP_SetBaseAddress(CRYP_DEVICE_ID_0,CRYP_0_REG_BASE_ADDR);
		}

		error_gic = GIC_DisableItLine(GIC_CRYP_0_LINE);
	    if (GIC_OK != error_gic)
	    {
	    	PRINT("GIC_DisableItLine::Error in Disabling Interrupt for CRYPT0");
	    	exit(-1);
		}
		
	    error_gic = GIC_ChangeDatum(GIC_CRYP_0_LINE , SER_CRYP0_InterruptHandler, &p_old_handler);
		if (GIC_OK != error_gic)
		{
	    	PRINT("GIC_ChangeDatum::Error in Binding Interrupt for CRYPT0");
	    	exit(-1);
		} 

	    error_gic = GIC_EnableItLine(GIC_CRYP_0_LINE);
	    if (GIC_OK != error_gic)
		{
		   PRINT("GIC_EnableItLine::Error in Enabling Interrupt for CRYPT0");
		   exit(-1);
		}
    }
    else if(INIT_CRYP1 == cryp_ser_mask)
    {
    	/* Set base address for the CRYP  */
		error = CRYP_Init(CRYP_DEVICE_ID_1,CRYP_1_REG_BASE_ADDR);
		if(CRYP_OK != error)
		{
			PRINT("CRYP1 INIT FAILED");
		}
		else
		{
	    	CRYP_SetBaseAddress(CRYP_DEVICE_ID_1,CRYP_1_REG_BASE_ADDR);
		}

		error_gic = GIC_DisableItLine(GIC_CRYP_1_LINE);
	    if (GIC_OK != error_gic)
	    {
	    	PRINT("GIC_DisableItLine::Error in Disabling Interrupt for CRYPT1");
	    	exit(-1);
		}
		
	    error_gic = GIC_ChangeDatum(GIC_CRYP_1_LINE , SER_CRYP1_InterruptHandler, &p_old_handler);
		if (GIC_OK != error_gic)
		{
	    	PRINT("GIC_ChangeDatum::Error in Binding Interrupt for CRYPT1");
	    	exit(-1);
		} 

	    error_gic = GIC_EnableItLine(GIC_CRYP_1_LINE);
	    if (GIC_OK != error_gic)
		{
		   PRINT("GIC_EnableItLine::Error in Enabling Interrupt for CRYPT1");
		   exit(-1);
		}
    }     

}

/****************************************************************************/
/* NAME:    SER_CRYP_Close                                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This provides resets services for CRYP						*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              							    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/

PUBLIC void SER_CRYP_Close(t_uint8 cryp_device_mask) 
{	


}
/****************************************************************************/
/* NAME:    SER_CRYP_RegisterCallback                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This function is for call back purpose		 				*/
/*																			*/
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC int SER_CRYP_RegisterCallback (IN t_callback_fct callback_fct, IN void* param)
{

    g_cryp_callback.fct   = callback_fct;
    g_cryp_callback.param = param;
    return HCL_OK;
}

