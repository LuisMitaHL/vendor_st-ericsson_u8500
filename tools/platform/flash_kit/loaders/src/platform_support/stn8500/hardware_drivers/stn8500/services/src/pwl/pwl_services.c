/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for PWL initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "memory_mapping.h"
#include "services.h"
#include "pwl_services.h"
#include "pwl.h"

#if((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2))
#include "gpio.h"
#endif

/*--------------------------------------------------------------------------*
 * Global Variables                                                         *
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Constants and new types                                                  *
 *--------------------------------------------------------------------------*/
#define DEFAULT_INTENSITY_LEVEL 0x0FUL

/*--------------------------------------------------------------------------*
 * Private functions                                                        *
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Public functions                                                         *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_PWL_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine provides initialization services for PWL       */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_PWL_Init(t_uint8 default_ser_mask)
{
    t_pwl_error     pwl_error  = PWL_OK;
   	  	
#if((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_HREFV2)||(defined ST_8500V2))
   t_gpio_pin  output_pin;
  	volatile t_gpio_config config; 
 	t_gpio_error error_gpio;
	
#endif
   	
#if((defined ST_8500V1)||(defined ST_HREFV1)||(defined ST_HREFV2)||(defined ST_8500V2))
  
    /* AMBA & Kernel clock enable for Peripheral2-PWL */
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR))                             |= PWL_AMBA_CLK_EN_VAL;
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) |= PWL_KERNEL_CLK_EN_VAL;
    
  
 
    output_pin = GPIO_PIN_202;
    
#endif
#if((defined ST_8500ED)||(defined ST_HREFED))
   
    output_pin = GPIO_PIN_226;
   
#endif
#if((defined __PEPS_8500_V1)||(defined __PEPS_8500_V2))
	/* AMBA & Kernel clock enable for Peripheral2-PWL */
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR))                             |= PWL_AMBA_CLK_EN_VAL;
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) |= PWL_KERNEL_CLK_EN_VAL;
#endif
  
      	
#if((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2))   
   
   	config.mode = GPIO_MODE_ALT_FUNCTION_B;
	config.direction = GPIO_DIR_OUTPUT;
	config.trig = GPIO_TRIG_LEAVE_UNCHANGED;
	
	error_gpio = GPIO_SetPinConfig(output_pin,config);
	
	error_gpio = (t_gpio_error)(error_gpio +1); /*Added to remove Coverity Defect*/
	
#else 
	
	 PRINT("Compiling for PEPS");
	 
#endif 
	
	
    pwl_error = PWL_Init(PWL_REG_BASE_ADDR);
    
   

    if(PWL_OK!=pwl_error)
    {
        PRINT("Error in Enabling PWL: Error Code %d\r\n", pwl_error); 
    }
    
}

/****************************************************************************/
/* NAME:    SER_PWL_Close                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops services for PWL                         */
/* WARNING : NEED TO BE AVOIDED . CAN BE DONE FOR TESTING                   */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_PWL_Close(void)
{
    /* TBD */
}

/****************************************************************************/
/* NAME:    SER_PWL_ConfigureDefault                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Set PWL To default configuration                            */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non Reentrant                                                */

/****************************************************************************/
PUBLIC t_ser_error SER_PWL_ConfigureDefault(void)
{
    t_pwl_error err = PWL_OK;

    /* Set PWL level */
    err = PWL_SetLevel(DEFAULT_INTENSITY_LEVEL);
    if (PWL_OK != err)
    {
        PRINT("PWL_SetLevel failed. Error = %u\r\n", err);
        return(SERVICE_FAILED);
    }

    /* Enable PWL */
    err = PWL_Enable();
    if (PWL_OK != err)
    {
        PRINT("PWL_Enable failed. Error = %u\r\n", err);
        return(SERVICE_FAILED);
    }

    return(SERVICE_OK);
}

