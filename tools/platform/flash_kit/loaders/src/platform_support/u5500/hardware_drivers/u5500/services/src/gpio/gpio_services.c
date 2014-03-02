/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides GPIO Initialization services
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/

#include "memory_mapping.h"
#include "gpio_services.h"
#include "gic.h"

#define GPIO_MAXCALLBACKS 8

t_ser_gpio_param  g_gpio_param;
volatile t_callback g_gpio_callback[GPIO_MAXCALLBACKS]={0,0,0,0,0,0,0,0};

/****************************************************************************/
/* NAME:    SER_GPIO0_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO0 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO0_IntHandler(t_uint32 irq) 
{
	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_0_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_0_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_0 , &(g_gpio_param.gpio_irq_param));
	

	if(g_gpio_callback[0].fct != NULL)
	{
		g_gpio_callback[0].fct(g_gpio_callback[0].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_0; irq_src <= (t_uint32)GPIO_IRQ_SRC_31 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_0_LINE);
		if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_0_LINE \n");
        return;
    }

}

/****************************************************************************/
/* NAME:    SER_GPIO1_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO1 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO1_IntHandler(t_uint32 irq) 
{
	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_1_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_1_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_1 , &(g_gpio_param.gpio_irq_param));

	if(g_gpio_callback[1].fct != NULL)
	{
		g_gpio_callback[1].fct(g_gpio_callback[1].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_32; irq_src <= (t_uint32)GPIO_IRQ_SRC_63 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_1_LINE);
	if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_1_LINE \n");
        return;
    }
 	
}

/****************************************************************************/
/* NAME:    SER_GPIO2_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO2 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO2_IntHandler(t_uint32 irq) 
{
	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_2_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_2_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_2 , &(g_gpio_param.gpio_irq_param));
	

	if(g_gpio_callback[2].fct != NULL)
	{
		g_gpio_callback[2].fct(g_gpio_callback[2].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_64; irq_src <= (t_uint32)GPIO_IRQ_SRC_95 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_2_LINE);
	if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_2_LINE \n");
        return;
    }
 	
}

/****************************************************************************/
/* NAME:    SER_GPIO3_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO3 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO3_IntHandler(t_uint32 irq) 
{
	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_3_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_3_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_3 , &(g_gpio_param.gpio_irq_param));
	
	if(g_gpio_callback[3].fct != NULL)
	{
		g_gpio_callback[3].fct(g_gpio_callback[3].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_96; irq_src <= (t_uint32)GPIO_IRQ_SRC_127 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_3_LINE);
	if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_3_LINE \n");
        return;
    }
 	
}

/****************************************************************************/
/* NAME:    SER_GPIO4_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO4 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO4_IntHandler(t_uint32 irq) 
{
	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_4_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_4_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_4 , &(g_gpio_param.gpio_irq_param));
	
	if(g_gpio_callback[4].fct != NULL)
	{
		g_gpio_callback[4].fct(g_gpio_callback[4].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_128; irq_src <= (t_uint32)GPIO_IRQ_SRC_159 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_4_LINE);
	if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_4_LINE \n");
        return;
    }
 	
}

/****************************************************************************/
/* NAME:    SER_GPIO5_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO5 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO5_IntHandler(t_uint32 irq) 
{
	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_5_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_4_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_5 , &(g_gpio_param.gpio_irq_param));


	if(g_gpio_callback[5].fct != NULL)
	{
		g_gpio_callback[5].fct(g_gpio_callback[5].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_160; irq_src <= (t_uint32)GPIO_IRQ_SRC_191 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_5_LINE);
	if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_5_LINE \n");
        return;
    }
 	
}

/****************************************************************************/
/* NAME:    SER_GPIO6_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO6 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO6_IntHandler(t_uint32 irq) 
{

	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_6_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_6_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_6 , &(g_gpio_param.gpio_irq_param));
	

	if(g_gpio_callback[6].fct != NULL)
	{
		g_gpio_callback[6].fct(g_gpio_callback[6].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_192; irq_src <= (t_uint32)GPIO_IRQ_SRC_223 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_6_LINE);
	if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_6_LINE \n");
        return;
    }
 	
}

/****************************************************************************/
/* NAME:    SER_GPIO7_IntHandler	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This is interrupt service routine for GPIO7 Block	     	*/
/* 																            */
/* PARAMETERS: unsigned int irq                                             */
/*                                                                          */
/* RETURN: None                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_GPIO7_IntHandler(t_uint32 irq) 
{
	t_uint32 irq_src;
	t_gic_error         gic_error;

	GPIO_DisableIRQSrc((t_gpio_irq_src_id)irq);
 	gic_error = GIC_DisableItLine(GIC_GPIO_7_LINE);
 	if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_GPIO_7_LINE \n");
        return;
    }
 	
 
	GPIO_GetMulptipleIRQStatus((t_uint32)GPIO_DEVICE_ID_7 , &(g_gpio_param.gpio_irq_param));

	if(g_gpio_callback[7].fct != NULL)
	{
		g_gpio_callback[7].fct(g_gpio_callback[7].param, &g_gpio_param );
				
	}


	for(irq_src = (t_uint32)GPIO_IRQ_SRC_224; irq_src <= (t_uint32)GPIO_IRQ_SRC_227 ;irq_src++)
	{
	    if (g_gpio_param.gpio_irq_param.gpio[irq_src] == TRUE)
	        GPIO_ClearIRQSrc((t_gpio_irq_src_id)irq_src);
	}

	
	gic_error = GIC_EnableItLine(GIC_GPIO_7_LINE);
	if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_GPIO_7_LINE \n");
        return;
    }
 	
}
/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/


/****************************************************************************/
/* NAME:    SER_GPIO_Init                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the gpio services                   */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/


PUBLIC void SER_GPIO_Init( t_uint8 default_ser_mask ) 
{
	t_uint32 index = 0;
	t_gic_func_ptr old_datum;
		
	/*GIC_GPIO_0_LINE */
	
	/* Enable PRCC Clock */

    *(volatile t_uint32* )(PRCC_1_CTRL_REG_BASE_ADDR) = SER_GPIO_PRCC_CLK_ENABLE_1;
    *(volatile t_uint32* )(PRCC_2_CTRL_REG_BASE_ADDR) = SER_GPIO_PRCC_CLK_ENABLE_2;	
    *(volatile t_uint32* )(PRCC_3_CTRL_REG_BASE_ADDR) = SER_GPIO_PRCC_CLK_ENABLE_3;	
    *(volatile t_uint32* )(PRCC_5_CTRL_REG_BASE_ADDR) = SER_GPIO_PRCC_CLK_ENABLE_5;		
   
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_0_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_0_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_0_LINE, SER_GPIO0_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_0_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_0_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_0_LINE\n");
	}

	/*GIC_GPIO_1_LINE */
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_1_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_1_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_1_LINE, SER_GPIO1_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_1_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_1_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_1_LINE\n");
	}

	/*GIC_GPIO_2_LINE */
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_2_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_2_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_2_LINE, SER_GPIO2_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_2_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_2_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_2_LINE\n");
	}

	/*GIC_GPIO_3_LINE */
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_3_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_3_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_3_LINE, SER_GPIO3_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_3_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_3_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_3_LINE\n");
	}
	
	/*GIC_GPIO_4_LINE */
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_4_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_4_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_4_LINE, SER_GPIO4_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_4_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_4_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_4_LINE\n");
	}

	/*GIC_GPIO_5_LINE */
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_5_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_5_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_5_LINE, SER_GPIO5_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_5_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_5_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_5_LINE\n");
	}

	/*GIC_GPIO_6_LINE */
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_6_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_6_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_6_LINE, SER_GPIO6_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_6_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_6_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_6_LINE\n");
	}

	/*GIC_GPIO_7_LINE */
	
	/* DISABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_DisableItLine(GIC_GPIO_7_LINE))
	{
		PRINT("error GIC_DisableItLine GIC_GPIO_7_LINE\n");
	}

	if(GIC_OK != GIC_ChangeDatum(GIC_GPIO_7_LINE, SER_GPIO7_IntHandler,&old_datum))
	{
		PRINT("error GIC_ChangeDatum GIC_GPIO_7_LINE \n");
	}
	
	/* ENABLE INTERRUPT LINE */
	if(GIC_OK!=GIC_EnableItLine(GIC_GPIO_7_LINE))
	{
		PRINT("error GIC_EnableItLine GIC_GPIO_7_LINE\n");
	}

  	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_0,GPIO_0_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 0 \n");
	}
		
	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_1,GPIO_1_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 1 \n");
	}

	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_2,GPIO_2_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 2 \n");
	}
	
	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_3,GPIO_3_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 3 \n");
	}

	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_4,GPIO_4_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 4 \n");
	}

	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_5,GPIO_5_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 5 \n");
	}
	
	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_6,GPIO_6_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 6 \n");
	}
	
	if(GPIO_OK != GPIO_Init(GPIO_CTRL_ID_7,GPIO_7_REG_BASE_ADDR))
	{
		PRINT("error GPIO_Init for ctrl id 7 \n");
	}

	GPIO_SetBaseAddress(GPIO_CTRL_ID_0,GPIO_0_REG_BASE_ADDR);
	GPIO_SetBaseAddress(GPIO_CTRL_ID_1,GPIO_1_REG_BASE_ADDR);
	GPIO_SetBaseAddress(GPIO_CTRL_ID_2,GPIO_2_REG_BASE_ADDR);
	GPIO_SetBaseAddress(GPIO_CTRL_ID_3,GPIO_3_REG_BASE_ADDR);
	GPIO_SetBaseAddress(GPIO_CTRL_ID_4,GPIO_4_REG_BASE_ADDR);
	GPIO_SetBaseAddress(GPIO_CTRL_ID_5,GPIO_5_REG_BASE_ADDR);
	GPIO_SetBaseAddress(GPIO_CTRL_ID_6,GPIO_6_REG_BASE_ADDR);
	GPIO_SetBaseAddress(GPIO_CTRL_ID_7,GPIO_7_REG_BASE_ADDR);
	
	for (index = 0; index <= 7; index++)
	{ 
	    g_gpio_callback[index].fct = NULL;
	    g_gpio_callback[index].param = NULL;
	}
	
	return  ;	
}


/****************************************************************************/
/* NAME:    SER_GPIO_Close	                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops the gpio services                   		*/
/*                                                                          */
/* PARAMETERS: t_gpio_pin  pin                                              */
/*                                                                          */
/* RETURN: void                                                             */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_GPIO_Close(void)
{

	t_gpio_device_id device_id;
	t_uint8 count; 
	t_uint32 max_pin;
	t_uint32 pin;
	
	max_pin = 203; /*DOUBT*/

	for ( pin=(t_uint32)GPIO_IRQ_SRC_0; pin <= max_pin; pin++)
	{
		if(g_gpio_param.gpio_irq_param.gpio[pin] != FALSE)
			g_gpio_param.gpio_irq_param.gpio[pin] = FALSE;
	
		device_id = GPIO_GetDeviceID((t_gpio_irq_src_id)pin);
		
		switch(device_id)
		{
		    case GPIO_DEVICE_ID_0: 
		        count = 0;
		        break;
		 
		    case GPIO_DEVICE_ID_1: 
		        count = 1;
		        break;
		 
		    case GPIO_DEVICE_ID_2: 
		        count = 2;
		        break;
		 
		    case GPIO_DEVICE_ID_3: 
		        count = 3;
		        break;
			
			case GPIO_DEVICE_ID_4: 
		        count = 4;
		        break;
		 
		    case GPIO_DEVICE_ID_5: 
		        count = 5;
		        break;
		 
		    case GPIO_DEVICE_ID_6: 
		        count = 6;
		        break;
		        
		    case GPIO_DEVICE_ID_7: 
		        count = 7;
		        break;
		        
		         
		    default:
		        PRINT("WRONG pin passed: SER_GPIO_Close\n");
		        return ;
		}	
		
		g_gpio_callback[count].fct = NULL;
		g_gpio_callback[count].param = 0;
	
	}
	
}

/************************************************************************************/
/* NAME:    SER_GPIO_RegisterCallback                                          		*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine binds an call back for extended interrupt processing   */
/*                                                                          		*/
/* PARAMETERS: void                                                         		*/
/*                                                                          		*/
/* RETURN: void                                                             		*/
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           		*/
/************************************************************************************/

PUBLIC t_ser_error SER_GPIO_RegisterCallback(t_gpio_device_id device_id,t_callback_fct fct, void *p_param)
{

	t_uint32 index;
	if ((fct== NULL) || (p_param == NULL))
	{
		PRINT("NULL CALLBACK FUNCTION\n");
		return SERVICE_FAILED;
	}
	switch(device_id)
	{
	    case GPIO_DEVICE_ID_0: 
	        index = 0;
	        break;
	    case GPIO_DEVICE_ID_1: 
	        index = 1;
	        break;
	    case GPIO_DEVICE_ID_2: 
	        index = 2;
	        break;
		case GPIO_DEVICE_ID_3:
			index = 3;
			break;		
		case GPIO_DEVICE_ID_4: 
	        index = 4;
	        break;	        
	    case GPIO_DEVICE_ID_5: 
	        index = 5;
	        break;
	    case GPIO_DEVICE_ID_6: 
	        index = 6;
	        break;
	    case GPIO_DEVICE_ID_7: 
	        index = 7;
	        break;
	   
	    default:
	        PRINT("WRONG DEVICED ID passed: SER_GPIO_RegisterCallback\n");
	        return SERVICE_FAILED;
	}
	
	g_gpio_callback[index].fct = fct;
    g_gpio_callback[index].param = p_param;
    return SERVICE_OK;
    
}

/************************************************************************************/
/* NAME:    SER_GPIO_WaitEnd                                                   		*/
/*----------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine binds an call back for extended interrupt processing   */
/*                                                                          		*/
/* PARAMETERS: t_gpio_pin                                                      		*/
/*                                                                          		*/
/* RETURN: void                                                             		*/
/*              																	*/
/*----------------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           		*/
/************************************************************************************/
PUBLIC void SER_GPIO_WaitEnd(t_gpio_pin pin)
{
	while (TRUE != g_gpio_param.gpio_irq_param.gpio[pin] );
	
	g_gpio_param.gpio_irq_param.gpio[pin]=FALSE;
	
}

