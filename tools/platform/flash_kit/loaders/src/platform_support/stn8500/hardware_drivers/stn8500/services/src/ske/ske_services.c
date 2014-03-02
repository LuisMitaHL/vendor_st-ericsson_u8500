/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides the Services API definitions for SKE(Scroll Key and Keypad Encoder)
*  Specification release related to this implementation: A_V2.2
*  Reference : Software Code Development, C Coding Rules, Guidelines 
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#include "memory_mapping.h"
#include "ske_services.h"
#include "gic.h"

/*#include "ske_hwp.h"*/


#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_HREFV2)||(defined ST_8500V2))

#include "gpio.h"

#define GPIO_PIN_KPI_0  GPIO_PIN_0
#define GPIO_PIN_KPO_0  GPIO_PIN_25

#endif

PRIVATE t_callback              g_ske_callback_desc[SKE_MAX_DEVICE] = { { 0, 0 }, { 0, 0 }, { 0, 0 } };

PRIVATE t_ske_callback_param    g_ske_callback_param =
{
    SKE_DEVICE_INVALID,
    SKE_KEYPAD_COLUMN_INVALID,
    SKE_KEYPAD_ROW_INVALID,
};


PRIVATE t_ske_output_drive g_ske_output_drive[SKE_KEYPAD_COLUMN_MAX] =
        {
            {TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE},
            {FALSE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE},
            {FALSE,FALSE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE},
            {FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE,FALSE},
            {FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE},
            {FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE},
            {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE},
            {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE}
        };


PRIVATE t_uint8                 g_keypad_max_columns = SKE_KEYPAD_COLUMN_MAX;
PRIVATE t_uint8                 g_keypad_max_rows = SKE_KEYPAD_ROW_MAX;
PRIVATE t_ske_output_drive 		g_ske_output_drive[SKE_KEYPAD_COLUMN_MAX];


/*extern PUBLIC t_ske_register  *gp_ske_register;*/
/* SKE Integration Test Input register */
#define SKE_ITIP   * ((volatile t_uint32 *) 0x80006084)

/* Configuration register for keypad and SKE CR_REG2 */
#define CR_REG2  * ((volatile t_uint32 *) 0xa03d8008)       

/*******************************************************************************************/
/* NAME:  SER_SKE_Init                                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for Initialisation of SKE environment              */
/* PARAMETERS:                                                                             */
/* IN:    mask(for future purpose if required)                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SER_SKE_Init(IN t_uint8 mask)
{
    t_gic_func_ptr old_datum;

#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_HREFV2)||(defined ST_8500V2))  
    t_uint8     count;
    t_gpio_pin  gpio_keypad_row_pin = GPIO_PIN_KPI_0;
	t_gpio_pin  gpio_keypad_col_pin = GPIO_PIN_KPO_0;
#endif

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    *((volatile t_uint32 *)(PRCC_3_CTRL_REG_BASE_ADDR)) = SKE_AMBA_CLK_EN_VAL;
    *((volatile t_uint32 *)(PRCC_3_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) = SKE_KERNEL_CLK_EN_VAL;
#endif 	

    if (SKE_OK != SKE_Init(SKE_REG_BASE_ADDR))
    {
        PRINT("ERROR! in SKE Initialisation");
        return;
    }

    SKE_SetBaseAddress(SKE_REG_BASE_ADDR);

#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2)) /* __PEPS_8500 */

    /* Enabling Alternate GPIO Functions for SKE */
    /*if (GPIO_OK != GPIO_EnableAltFunction(GPIO_ALT_SCROLL_KEY_1))
    {
        PRINT("ERROR! in Enabling ALT Function in GPIO for ScrollKey");
        return;
    }*/
    /***********************************************************************************/
    if (GPIO_OK != GPIO_EnableAltFunction(GPIO_ALT_KEYPAD))
    {
        PRINT("ERROR! in Enabling ALT Function in GPIO for Keypad");
        return;
    }
    /************************************************************************************/
#if ((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
    /* GPIO2 AFSELA */
     * ((volatile t_uint32 *) 0x8000E020)= 0x00000000;
    /* GPIO2 AFSELB */
     * ((volatile t_uint32 *) 0x8000E024)= 0x00003FFF;
    /* GPIO4 AFSELA */
     * ((volatile t_uint32 *) 0x8000E120)= 0x00001800;
    /* GPIO4 AFSELB */
     * ((volatile t_uint32 *) 0x8000E124)= 0x00001800;
    /* GPIO2 PDIS */
    * ((volatile t_uint32 *) 0x8000E00C)= 0x000003D3;
    /* GPIO4 PDIS */
    * ((volatile t_uint32 *) 0x8000E10C)= 0x00000800;
    /* GPIO2 DAT */
    * ((volatile t_uint32 *) 0x8000E000)= 0x00003C2C;
    /* GPIO4 DAT */
    * ((volatile t_uint32 *) 0x8000E100)= 0x00001000;
#endif 

    /*---------------------------------------------------------------------------------*/

    /* Currently GPIO HCL does not allow to configure GPIO_PDIS register for Alternate Function */
    /* Pull resitor for Keypad row input lines should be enabled */
    /* By GPIO Specification it is found that Reset value is 0x0(i.e enabled) */
    /* Assuming nobody disables it at least for KPI lines */
    /* Ensuring Keypad row input lines are pulled up */

	for (count = 0; count < SKE_KEYPAD_ROW_MAX; count++)
    {
		if (GPIO_OK != GPIO_SetGpioPin(gpio_keypad_row_pin))
        {
            PRINT("ERROR! in SKE IRQ Handler..GPIO Set Pin(Pull-up) for Keypad");
            return;
        }
		gpio_keypad_row_pin++;

		if (GPIO_OK != GPIO_SetGpioPin(gpio_keypad_col_pin))
        {
            PRINT("ERROR! in SKE IRQ Handler..GPIO Set Pin(Pull-up) for Keypad");
            return;
        }
		gpio_keypad_col_pin++;
    }

#endif /* __PEPS_8500 */

    
	/* Initialize for Interrupts */
    if (GIC_OK != GIC_DisableItLine(GIC_SKE_LINE))
    {
        PRINT("ERROR! in SKE Initialisation..GIC EnableItLine for ScrollKey");
        return;
    }

    if (GIC_OK != GIC_ChangeDatum(GIC_SKE_LINE, (t_gic_func_ptr)SER_SKE_InterruptHandler_ScrollKey, &old_datum))
    {
        PRINT("ERROR! in SKE Initialisation..GIC ChangeDatum for ScrollKey");
        return;
    }

    if (GIC_OK != GIC_EnableItLine(GIC_SKE_LINE))
    {
        PRINT("ERROR! in SKE Initialisation..GIC EnableItLine for ScrollKey");
        return;
    }

    if (GIC_OK != GIC_DisableItLine(GIC_KB_LINE))
    {
        PRINT("ERROR! in SKE Initialisation..GIC EnableItLine for Keypad");
        return;
    }

    if (GIC_OK != GIC_ChangeDatum(GIC_KB_LINE, (t_gic_func_ptr)SER_SKE_InterruptHandler_Keypad, &old_datum))
    {
        PRINT("ERROR! in SKE Initialisation..GIC ChangeDatum for Keypad");
        return;
    }

    if (GIC_OK != GIC_EnableItLine(GIC_KB_LINE))
    {
        PRINT("ERROR! in SKE Initialisation..GIC EnableItLine for Keypad");
        return;
    }        
    
}

/*******************************************************************************************/
/* NAME:  SER_SKE_RegisterCallback                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for registering a callback function which will be  */
/*              called by the respective interrupt handler                                 */
/* PARAMETERS:                                                                             */
/* IN:    ske_device :SKE device for which callback is to be registered                    */
/*        callback_func :Callback function which need to be called on                      */
/*                       occurence of interrupt.Its type is                                */
/*                         void (func_name)(void *, void *) [Declared in services.h]       */
/*        callback_param: This is user defined parameter which will be passed as           */
/*                        second argument in the callback function when it is called       */
/*                          in the interrupt handler(of SKE service)                       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: HCL error. HCL_OK - If no errors,                                               */
/*                    HCL_INVALID_PARAMETER - If wrong value for SKE device is passed or   */
/*                                            If no callback function is passed            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC int SER_SKE_RegisterCallback
(
    IN t_ske_device     ske_device,
    IN t_callback_fct   callback_func,
    IN void             *callback_param
)
{
    if ((0 != callback_func) && (SKE_MAX_DEVICE > ske_device))
    {
        g_ske_callback_desc[ske_device].fct = callback_func;
        g_ske_callback_desc[ske_device].param = callback_param;
        return(HCL_OK);
    }
    else
    {
        return(HCL_INVALID_PARAMETER);
    }
}

/*******************************************************************************************/
/* NAME:  SER_SKE_InterruptHandler_ScrollKey                                               */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is Interrupt Handler for Scroll Key Device                   */
/* PARAMETERS:                                                                             */
/* IN:     irq : Currently not used (for future purpose if required)                       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
void SER_SKE_InterruptHandler_ScrollKey(IN unsigned int irq)
{
    t_ske_irq_src   ske_irq_src;
    if (GIC_OK != GIC_DisableItLine(GIC_SKE_LINE))
    {
        PRINT("ERROR! in SKE IRQ Handler..GIC EnableItLine for ScrollKey");
        return;
    }

 /*   if (GIC_OK != GIC_AcknowledgeItLine(GIC_SKE_LINE))
    {
        PRINT("ERROR! in SKE IRQ Handler..GIC AcknowledgeItLine for ScrollKey");
        return;
    } */

    ske_irq_src = SKE_GetIRQSrc();
    if (ske_irq_src & SKE_IRQ_SCROLL_DEVICE_0)
    {
        SKE_ClearIRQSrc(SKE_IRQ_SCROLL_DEVICE_0);
        if (0 != g_ske_callback_desc[SKE_DEVICE_SCROLL_KEY_0].fct)
        {
            g_ske_callback_param.ske_device = SKE_DEVICE_SCROLL_KEY_0;
            g_ske_callback_desc[SKE_DEVICE_SCROLL_KEY_0].fct
                (
                    &g_ske_callback_param,
                    g_ske_callback_desc[SKE_DEVICE_SCROLL_KEY_0].param
                );
        }
        else
        {
            PRINT("WARNING! in SKE IRQ Handler..No callback registered for Scroll Key 0");
        }
    }

    if (ske_irq_src & SKE_IRQ_SCROLL_DEVICE_1)
    {
        SKE_ClearIRQSrc(SKE_IRQ_SCROLL_DEVICE_1);
        if (0 != g_ske_callback_desc[SKE_DEVICE_SCROLL_KEY_1].fct)
        {
            g_ske_callback_param.ske_device = SKE_DEVICE_SCROLL_KEY_1;
            g_ske_callback_desc[SKE_DEVICE_SCROLL_KEY_1].fct
                (
                    &g_ske_callback_param,
                    g_ske_callback_desc[SKE_DEVICE_SCROLL_KEY_1].param
                );
        }
        else
        {
            PRINT("WARNING! in SKE IRQ Handler..No callback registered for Scroll Key 1");
        }
    }

    if (GIC_OK != GIC_EnableItLine(GIC_SKE_LINE))
    {
        PRINT("ERROR! in SKE IRQ Handler..GIC EnableItLine for ScrollKey");
        return;
    }
}

/*******************************************************************************************/
/* NAME:  SER_SKE_InterruptHandler_Keypad                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is Interrupt Handler for Keypad Device                       */
/* PARAMETERS:                                                                             */
/* IN:     irq : Currently not used (for future purpose if required)                       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
void SER_SKE_InterruptHandler_Keypad(IN unsigned int irq)
{
    //t_ske_error         error;
    t_ske_keypad_column ske_keypad_column = SKE_KEYPAD_COLUMN_0;
    t_ske_keypad_row    ske_keypad_row = SKE_KEYPAD_ROW_INVALID;
    t_ske_row_result    ske_row_result;
    t_uint8             count;
    t_uint16            wait;
    t_uint8             ske_keypad_debounce;
    
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_HREFV2)||(defined ST_8500V2)) /* __PEP_8500 */    
    t_gpio_pin          gpio_keypad_row_pin;
    t_gpio_data         gpio_keypad_row_value;
#endif    
    
    if (GIC_OK != GIC_DisableItLine(GIC_KB_LINE))
    {
        PRINT("ERROR! in SKE IRQ Handler..GIC EnableItLine for Keypad");
        return;
    }

/*    if (GIC_OK != GIC_AcknowledgeItLine(GIC_KB_LINE))
    {
        PRINT("ERROR! in SKE IRQ Handler..GIC AcknowledgeItLine for Keypad");
        return;
    } */


    if (TRUE == (SKE_IsKeypadAutoScanEnabled()))    /* Automatic Matrix Scan */
    {
        /* Wrisky to wait indefinitely in ISR for ASON flag to clear.
	     The SKE Hardware must ensure that it is cleared by this time */
        while (TRUE == SKE_IsAutoScanOngoing())
            ;
        SKE_ClearIRQSrc(SKE_IRQ_KEYPAD_AUTOSCAN);
        
        /* Disable KeyPad AutoScan */
        SKE_DisableIRQSrc((t_ske_irq_src) SKE_IRQ_KEYPAD_AUTOSCAN);
        
        do
        {
            SKE_GetAutoScanResult(ske_keypad_column, &ske_row_result);
            for (count = 0; count < g_keypad_max_rows; count++)
            {
                if (TRUE == ske_row_result.keypad_row_set[count])
                {
                    ske_keypad_row = (t_ske_keypad_row) count;
                    break;
                }
            }

            if (SKE_KEYPAD_ROW_INVALID != ske_keypad_row)
            {
                break;
            }

            ske_keypad_column = (t_ske_keypad_column) ((t_uint8) ske_keypad_column + 1);
        } while (g_keypad_max_columns != ske_keypad_column);
        
        /* Wait for key to be released */
        while (TRUE == SKE_IsAutoKeyPressed())
            ;
        
        /* Enable the AutoScan Interrupt */
        SKE_EnableIRQSrc((t_ske_irq_src) SKE_IRQ_KEYPAD_AUTOSCAN);    
        
    }
    else    /* Manual Software Matix Scan */
    {
        /* Disable KeyPad SoftScan */
        SKE_DisableIRQSrc((t_ske_irq_src) SKE_IRQ_KEYPAD_SOFTSCAN);
        
        /* Clear Keypad Soft Scan Interrupt */
        SKE_ClearIRQSrc(SKE_IRQ_KEYPAD_SOFTSCAN);      
        
        do
        {
            SKE_SetKeypadOutputDriveBulk(&g_ske_output_drive[ske_keypad_column]);

#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_HREFV2)||(defined ST_8500V2))           
            gpio_keypad_row_pin = GPIO_PIN_KPI_0;
#endif 
            
            //for (count = 0; count < g_keypad_max_rows; count++)SKE_KEYPAD_ROW_MAX
            for (count = 0; count < SKE_KEYPAD_ROW_MAX; count++)
            {            
#if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2))
                if (GPIO_OK != GPIO_ReadGpioPin(gpio_keypad_row_pin, &gpio_keypad_row_value))
                {
                    PRINT("ERROR! in SKE IRQ Handler..GPIO Read Pin for Keypad");
                    return;
                }

                if (GPIO_DATA_LOW == gpio_keypad_row_value)
                {
                    ske_keypad_row = (t_ske_keypad_row) count;                    
                }
                
                if(ske_keypad_row != SKE_KEYPAD_ROW_INVALID)
                {
                	
                	/* Get the Keypad Debounce Value */
                	if(SKE_OK != SKE_GetKeypadDebounce(&ske_keypad_debounce))
                    {
                        PRINT("ERROR! in SKE_GetKeypadDebounce");
                    }
                	
                	/* Wait for one Debouncing Period */
                	for (wait = 0; wait < ske_keypad_debounce; wait++)
                	{
                		;
                	}
                	
                	if (GPIO_OK != GPIO_ReadGpioPin(gpio_keypad_row_pin, &gpio_keypad_row_value))
                    {
                        PRINT("ERROR! in SKE IRQ Handler..GPIO Read Pin for Keypad");
                        return;
                    }

                    if (GPIO_DATA_LOW == gpio_keypad_row_value)
                    {  
                        switch(count)
                        {
                        	case 0:
                        	  count = count + 4;
                        	  break;                        	                          	
                        	  
                        	case 1:
                        	  count = count + 6;
                        	  break;
                        	
                        	case 4:
                        	  count = count - 4;
                        	  break;
                        	  
                        	case 7:
                        	  count = count - 6;
                        	  break;
                        	  
                        	default:
                        	  break;
                        }                        
                        
                        if(count < g_keypad_max_rows)
                        {
                        	ske_keypad_row = (t_ske_keypad_row) count;                                                                                            
                            break;
                        }
                        else
                        {
                        	ske_keypad_row = SKE_KEYPAD_ROW_INVALID;
                        }
                        
                    }
                    else
                    {
                    	ske_keypad_row = SKE_KEYPAD_ROW_INVALID;
                    }
                }
                
                gpio_keypad_row_pin++;

#else                                              
                if ((~(SKE_ITIP)) & (MASK_BIT0 << count))
                {
                    ske_keypad_row = (t_ske_keypad_row) count;                    
                }
                
                if(ske_keypad_row != SKE_KEYPAD_ROW_INVALID)
                {
                	
                	/* Get the Keypad Debounce Value */
                	if(SKE_OK != SKE_GetKeypadDebounce(&ske_keypad_debounce))
                    {
                        PRINT("ERROR! in SKE_GetKeypadDebounce");
                    }
                	
                	/* Wait for one Debouncing Period */
                	for (wait = 0; wait < 100; wait++)
                	{
                		;
                	}
                	
                	if ((~(SKE_ITIP)) & (MASK_BIT0 << count))
                    {
                       ske_keypad_row = (t_ske_keypad_row) count;
                       break;
                    }
                    else
                    {
                    	ske_keypad_row = SKE_KEYPAD_ROW_INVALID;
                    }
                }
                	                
#endif
            }

            if (SKE_KEYPAD_ROW_INVALID != ske_keypad_row)
            {
                break;
            }

            ske_keypad_column = (t_ske_keypad_column) ((t_uint8) ske_keypad_column + 1);            
            
        } while (g_keypad_max_columns != ske_keypad_column);
        
        /* Driving All the Keypad Output */       
        SKE_SetAllKeypadOutputDrive(TRUE);
        
        /* Wait for key to be released */
        while (TRUE == SKE_IsSoftKeyPressed())
            ;
        
        /* Enable the SoftScan Interrupt */
        SKE_EnableIRQSrc((t_ske_irq_src) SKE_IRQ_KEYPAD_SOFTSCAN);    
    }        

    if ((g_keypad_max_columns != ske_keypad_column) && (SKE_KEYPAD_ROW_INVALID != ske_keypad_row))
    {
        if (0 != g_ske_callback_desc[SKE_DEVICE_KEYPAD].fct)
        {
            g_ske_callback_param.ske_device = SKE_DEVICE_KEYPAD;
            g_ske_callback_param.ske_keypressed_column = ske_keypad_column;
            g_ske_callback_param.ske_keypressed_row = ske_keypad_row;
            g_ske_callback_desc[SKE_DEVICE_KEYPAD].fct
                (
                    &g_ske_callback_param,
                    g_ske_callback_desc[SKE_DEVICE_KEYPAD].param
                );
        }
        else
        {
            PRINT("WARNING! in SKE IRQ Handler..No callback registered for Keypad");
        }

        if (TRUE == (SKE_IsKeypadAutoScanEnabled()))
        {
            ske_row_result.keypad_row_set[ske_keypad_row] = FALSE;
            SKE_SetAutoScanResult(ske_keypad_column, &ske_row_result);
        }
    }

    if (GIC_OK != GIC_EnableItLine(GIC_KB_LINE))
    {
        PRINT("ERROR! in SKE IRQ Handler..GIC EnableItLine for Keypad");
        return;
    }

}

/*******************************************************************************************/
/* NAME:  SER_SKE_SetKeypadMatrixSize                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION:This function saves keypad matrix size used (Max Column x Max Row)          */
/*             to an internal variable                                                     */
/*             NOTE: (1) This function need not be called for 8 X 8 Keypad Matrix          */
/*                        and can be called for Key Matrix of lesser order                 */
/*                   (2) If this SKE Service function is called, it not required to call   */
/*                       SKE HCL SKE_SetKeypadMatrixSize API separately                    */
/* PARAMETERS:                                                                             */
/* IN:    max_columns : Maximum Columns used by the keypad matrix &                        */
/*        max_rows    : Rows used by the keypad matrix                                     */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: HCL error can take any of following values -                                    */
/*            HCL_INVALID_PARAMETER: If rows & columns are out of range                    */
/*            HCL_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC int SER_SKE_SetKeypadMatrixSize(IN t_uint8 max_columns, IN t_uint8 max_rows)
{
    if (SKE_OK == (SKE_SetKeypadMatrixSize(max_columns, max_rows)))
    {
        g_keypad_max_columns = max_columns;
        g_keypad_max_rows = max_rows;
        return(HCL_OK);
    }
    else
    {
        return(HCL_INVALID_PARAMETER);
    }
}
/*******************************************************************************************/
/* NAME:  SER_SKE_SelectKeyPad                                                             */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is used to select the keypad by configuring the              */
/*              configuration Register                                                     */       
/* PARAMETERS:                                                                             */
/* IN:    mask(for future purpose if required)                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SER_SKE_SelectKeyPad(void)
{   
    /* KBD Signals are Selected */    
    #if ((defined ST_8500ED)||(defined ST_8500V1)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2)) 
       //CR_REG2 = 0x00076045;
        CR_REG2 &= 0x00000000;
        CR_REG2 = 0x06998968;
    #else
       //CR_REG2 |= (0x80000000);
	   CR_REG2 &=0x00000000;
	   CR_REG2 = 0x80098098;
    #endif 
}

/*******************************************************************************************/
/* NAME:  SER_SKE_SelectScrollKey                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is used to select the scroll key by configuring the          */
/*              configuration Register                                                     */       
/* PARAMETERS:                                                                             */
/* IN:    mask(for future purpose if required)                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SER_SKE_SelectScrollKey(void)
{	    
    /* Scroll Key Signals are Selected */    
    CR_REG2 = 0x80810810;        
}

/*******************************************************************************************/
/* NAME:  SER_SKE_SelectScrollKey0                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is used to select the scroll key by configuring the          */
/*              configuration Register                                                     */       
/* PARAMETERS:                                                                             */
/* IN:    mask(for future purpose if required)                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SER_SKE_SelectScrollKey0(void)
{	    
    /* Scroll Key Signals are Selected */        
    CR_REG2 = 0x00000000;
}

/*******************************************************************************************/
/* NAME:  SER_SKE_SelectScrollKey1                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is used to select the scroll key by configuring the          */
/*              configuration Register                                                     */       
/* PARAMETERS:                                                                             */
/* IN:    mask(for future purpose if required)                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SER_SKE_SelectScrollKey1(void)
{	    
    /* Scroll Key Signals are Selected */        
    CR_REG2 = 0x00001001;    
}

/*******************************************************************************************/
/* NAME:  SER_SKE_SelectBothScrollKeyAndKeypad                                             */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is used to select both scroll key and keypad by configuring  */
/*              the configuration Register                                                 */       
/* PARAMETERS:                                                                             */
/* IN:    mask(for future purpose if required)                                             */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: None                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void SER_SKE_SelectBothScrollKeyAndKeypad(void)
{	    
    /* Scroll Key and Key Pad Signals are Selected */    
    CR_REG2 = 0x80098098;
    //CR_REG2 = 0x80089089;
}


PUBLIC void SER_SKE_Close(void)
{
}

