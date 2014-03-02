/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   C file for General Purpose Input/Output (GPIO) module.
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "gpio_p.h"

/*------------------------------------------------------------------------
 * debug stuff
 *----------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME g_mydbglevel_gpio
#define MY_DEBUG_ID             g_mydbgid_gpio
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = GPIO_HCL_DBG_ID;
#endif

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PRIVATE t_gpio_system_context   g_gpio_system_context;

PRIVATE t_gpio_error gpio_checkperipheralid(void)
{
    t_gpio_error    gpio_error;

    if
    (
        (GPIO_PERIPH_ID_0 == g_gpio_system_context.p_gpio_register[0]->gpio_periph_id_0)
    &&  (GPIO_PERIPH_ID_1 == g_gpio_system_context.p_gpio_register[0]->gpio_periph_id_1)
    &&  (GPIO_PERIPH_ID_2 == g_gpio_system_context.p_gpio_register[0]->gpio_periph_id_2)
    &&  (GPIO_PERIPH_ID_3 == g_gpio_system_context.p_gpio_register[0]->gpio_periph_id_3)
    &&  (GPIO_PCELL_ID_0 == g_gpio_system_context.p_gpio_register[0]->gpio_pcell_id_0)
    &&  (GPIO_PCELL_ID_1 == g_gpio_system_context.p_gpio_register[0]->gpio_pcell_id_1)
    &&  (GPIO_PCELL_ID_2 == g_gpio_system_context.p_gpio_register[0]->gpio_pcell_id_2)
    &&  (GPIO_PCELL_ID_3 == g_gpio_system_context.p_gpio_register[0]->gpio_pcell_id_3)
    )
    {
        gpio_error = GPIO_OK;
    }
    else
    {
        gpio_error = GPIO_UNSUPPORTED_HW;
    }

    return(gpio_error);
}

/****************************************************************************/
/* NAME:	t_gpio_error GPIO_Init()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stores base address with respect to the gpio   */
/*				block and also checks Peripheral ID and PCell ID.			*/
/* PARAMETERS:	 															*/
/* IN:		t_gpio_ctrl_id gpio_ctrl_id: GPIO device id 					*/
/* IN:		t_logical_address gpio_base_address: Logical base address of 	*/
/*												 gpio block.				*/
/* INOUT:	none															*/
/* OUT:		none															*/
/*																			*/
/* RETURN: t_gpio_error : GPIO_UNSUPPORTED_HW if defined 	 				*/
/*						  incompatible platforms							*/
/*                        GPIO_INVALID_PARAMETER if wrong parameter or NULL */
/*						  parameter is passed								*/
/*						  GPIO_OK											*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	 Non Re-entrant												*/
/* REENTRANCY ISSUE: global array p_gpio_reg is getting modified 			*/

/****************************************************************************/
PUBLIC t_gpio_error GPIO_Init(t_gpio_ctrl_id gpio_ctrl_id, t_logical_address gpio_base_address)
{
    t_gpio_error    gpio_error;
    t_uint8         array_index;
    DBGENTER2("(%x %lx)", gpio_ctrl_id, gpio_base_address);

    if ((NULL == gpio_base_address) || (GPIO_CTRL_ID_INVALID == gpio_ctrl_id))
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT(gpio_error);
        return(gpio_error);
    }

    switch (gpio_ctrl_id)
    {
        case GPIO_CTRL_ID_0:
            array_index = 0;
            break;

        case GPIO_CTRL_ID_1:
            array_index = 1;
            break;

        case GPIO_CTRL_ID_2:
            array_index = 2;
            break;

        case GPIO_CTRL_ID_3:
            array_index = 3;
            break;

        case GPIO_CTRL_ID_4:
            array_index = 4;
            break;

        case GPIO_CTRL_ID_5:
            array_index = 5;
            break;

        case GPIO_CTRL_ID_6:
            array_index = 6;
            break;

        case GPIO_CTRL_ID_7:
            array_index = 7;
            break;

        case GPIO_CTRL_ID_8:
            array_index = 8;
            break;

        case GPIO_CTRL_ID_INVALID:
        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            DBGEXIT(gpio_error);
            return(gpio_error);
    }

    /* Initializing the registers structure */
    g_gpio_system_context.p_gpio_register[array_index] = (t_gpio_register *) gpio_base_address;

    /* Check Peripheral and Pcell Id Register for given device */
    gpio_error = gpio_checkperipheralid();

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************/
/* NAME:	t_gpio_error GPIO_GetVersion ()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns version for GPIO HCL.                  */
/* PARAMETERS:	 															*/
/* IN:		none                                        					*/
/* INOUT:	none															*/
/* OUT:		t_version* p_version :Version for GPIO HCL  					*/
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if NULL parameter is 		*/
/*						  passed											*/
/*                        GPIO_OK                                           */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant	        										*/
/* REENTRANCY ISSUE: N/A                                         			*/

/****************************************************************************/
PUBLIC t_gpio_error GPIO_GetVersion(t_version *p_version)
{
    DBGENTER0();
    if (NULL == p_version)
    {
        DBGEXIT0(GPIO_INVALID_PARAMETER);
        return(GPIO_INVALID_PARAMETER);
    }

    p_version->minor = GPIO_HCL_MINOR_ID;
    p_version->major = GPIO_HCL_MAJOR_ID;
    p_version->version = GPIO_HCL_VERSION_ID;

    DBGEXIT0(GPIO_OK);
    return(GPIO_OK);
}

/****************************************************************************/
/* NAME:	t_gpio_error GPIO_SetDbgLevel()	    						    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the debug level of the GPIO HCL. It       */
/*              allows to control the nature and/or the amount of the debug */
/*              traces that will be returned.                               */
/* PARAMETERS:	 															*/
/* IN:		t_dbg_level gpio_dbg_level: GPIO debug level                    */
/* INOUT:	none															*/
/* OUT:		none                                          					*/
/* RETURN: t_gpio_error : GPIO_OK               	    					*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant	        										*/
/* REENTRANCY ISSUE: N/A                                         			*/

/****************************************************************************/
PUBLIC t_gpio_error GPIO_SetDbgLevel(t_dbg_level gpio_dbg_level)
{
    DBGENTER1(" (%x)", gpio_dbg_level);
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = gpio_dbg_level;
#endif
    DBGEXIT0(GPIO_OK);
    return(GPIO_OK);
}

/****************************************************************************/
/* NAME:	t_gpio_error GPIO_ResetGpioPin()	    					    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Resets all the GPIO internal registers for a specific pin   */
/*              to their default value.                                     */
/* PARAMETERS:	 															*/
/* IN:		t_gpio_pin pin_no: GPIO pin number to reset to its default value*/
/* INOUT:	none															*/
/* OUT:		none                                          					*/
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong parameter is      */
/*						  passed as argument								*/
/*                        GPIO_OK		                                    */
/*                                       	    							*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant       										*/
/* REENTRANCY ISSUE: H/W registers are getting modified.          			*/

/****************************************************************************/
PUBLIC t_gpio_error GPIO_ResetGpioPin(t_gpio_pin pin_no)
{
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pinno;      /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;
    t_gpio_error    gpio_error = GPIO_OK;

    DBGENTER1(" Resetting (%d) pin", pin_no);
    if (pin_no <= GPIO_PIN_2)
    {
        block_no = ((t_uint32) pin_no / GPIO_NO_OF_PINS);
        pinno = ((t_uint32) pin_no % GPIO_NO_OF_PINS);
        gpio_pin_mask = (t_uint32) (1 << pinno);

        g_gpio_system_context.p_gpio_register[block_no]->gpio_ic |= gpio_pin_mask;      /*Clear interrupt.*/
        g_gpio_system_context.p_gpio_register[block_no]->gpio_rimsc &= ~gpio_pin_mask;
        g_gpio_system_context.p_gpio_register[block_no]->gpio_fimsc &= ~gpio_pin_mask;
        g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa &= ~gpio_pin_mask;   /*set mode to gpio.*/
        g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb &= ~gpio_pin_mask;
        g_gpio_system_context.p_gpio_register[block_no]->gpio_lowemi &= ~gpio_pin_mask;
        g_gpio_system_context.p_gpio_register[block_no]->gpio_dirs = gpio_pin_mask;     /*clear data on gpio line & configure pin to input*/
        g_gpio_system_context.p_gpio_register[block_no]->gpio_datc = gpio_pin_mask;     /* As read from gpio_dirc register returns GPIO_DAT register*/
        g_gpio_system_context.p_gpio_register[block_no]->gpio_dirc = gpio_pin_mask;     /* and GPIO_DIR for gpio_dirc */
        g_gpio_system_context.p_gpio_register[block_no]->gpio_pdis &= ~gpio_pin_mask;   /*pull-up or pull-down enable.*/
        g_gpio_system_context.p_gpio_register[block_no]->gpio_slpm &= ~gpio_pin_mask;   /*gpio pin is switched to input in sleep mode.*/
        g_gpio_system_context.p_gpio_register[block_no]->gpio_rwimsc &= ~gpio_pin_mask; /*wakeup is not enabled.*/
        g_gpio_system_context.p_gpio_register[block_no]->gpio_fwimsc &= ~gpio_pin_mask;
        g_gpio_system_context.p_gpio_register[block_no]->gpio_ic &= ~gpio_pin_mask;
    }
    else
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

PRIVATE t_gpio_error gpio_enablealt(t_gpio_alt_function alt_func)
{
    t_gpio_error    gpio_error = GPIO_OK;

    switch (alt_func)
    {
        case GPIO_ALT_MSP0:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa |= GPIO_MSP0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_MSP0_MASK1;
            break;

        case GPIO_ALT_MSP1:
            g_gpio_system_context.p_gpio_register[1]->gpio_afsa |= GPIO_MSP1_MASK;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_MSP1_MASK;
            break;

        case GPIO_ALT_MSP2:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_MSP2_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_MSP2_MASK;
            break;

        case GPIO_ALT_UART_0:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa |= GPIO_UART0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_UART0_MASK1;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsa |= GPIO_UART0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb |= GPIO_UART0_MASK2;
            break;

        case GPIO_ALT_UART_1:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa |= GPIO_UART1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_UART1_MASK1;
            break;

        case GPIO_ALT_UART_2:
#ifdef STE_AP8540
            g_gpio_system_context.p_gpio_register[3]->gpio_afsa |= GPIO_UART2_MASK1;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsb &= ~GPIO_UART2_MASK1;
#else
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa |= GPIO_UART2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb |= GPIO_UART2_MASK1;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsa |= GPIO_UART2_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb |= GPIO_UART2_MASK2;
#endif
            break;

        case GPIO_ALT_LCD_PANEL:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa |= GPIO_LCD_PANEL_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_LCD_PANEL_MASK1;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_LCD_PANEL_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb |= GPIO_LCD_PANEL_MASK2;
            g_gpio_system_context.p_gpio_register[5]->gpio_afsa &= ~GPIO_LCD_PANEL_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_afsb |= GPIO_LCD_PANEL_MASK3;
            break;

        case GPIO_ALT_KEYPAD:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_KEYPAD_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_KEYPAD_MASK1;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa |= GPIO_KEYPAD_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb |= GPIO_KEYPAD_MASK2;
            g_gpio_system_context.p_gpio_register[5]->gpio_afsa |= GPIO_KEYPAD_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_afsb |= GPIO_KEYPAD_MASK3;
            break;

        case GPIO_ALT_HSIT0:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_HSIT0_MASK_1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_HSIT0_MASK_1;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsa |= GPIO_HSIT0_MASK_2;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb &= ~GPIO_HSIT0_MASK_2;

            break;

        case GPIO_ALT_HSIR0:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_HSIR0_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_HSIR0_MASK;
            break;

        case GPIO_ALT_USB_OTG:
            g_gpio_system_context.p_gpio_register[8]->gpio_afsa |= GPIO_USB_OTG_MASK;
            g_gpio_system_context.p_gpio_register[8]->gpio_afsb &= ~GPIO_USB_OTG_MASK;
            break;

        case GPIO_ALT_NAND_FLASH:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa |= GPIO_NAND_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_NAND_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_NAND_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_NAND_MASK2;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsa &= ~GPIO_NAND_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsb |= GPIO_NAND_MASK3;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_NAND_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb |= GPIO_NAND_MASK4;
            break;

        case GPIO_ALT_NOR_FLASH:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa |= GPIO_NOR_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_NOR_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_NOR_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_NOR_MASK2;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsa &= ~GPIO_NOR_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsb |= GPIO_NOR_MASK3;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_NOR_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb |= GPIO_NOR_MASK4;
            break;
            
    	case GPIO_ALT_SPI0:
    		g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_SPI0_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb |= GPIO_SPI0_MASK1;
			g_gpio_system_context.p_gpio_register[7]->gpio_afsa |= GPIO_SPI0_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb |= GPIO_SPI0_MASK2;
			break;
			
		case GPIO_ALT_SPI1:
    		g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_SPI1_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb |= GPIO_SPI1_MASK;
	        break;
	    
	    case GPIO_ALT_SPI3:
    		g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_SPI3_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb |= GPIO_SPI3_MASK1;
			g_gpio_system_context.p_gpio_register[1]->gpio_afsa &= ~GPIO_SPI3_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb |= GPIO_SPI3_MASK2;
			break;
			
    	case GPIO_ALT_STMAPE_1:
    		g_gpio_system_context.p_gpio_register[2]->gpio_afsa |= GPIO_STMAPE_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_STMAPE_MASK1;
	        break;
	        
	   	case GPIO_ALT_STMAPE_2:
    		g_gpio_system_context.p_gpio_register[4]->gpio_afsa |= GPIO_STMAPE_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb |= GPIO_STMAPE_MASK2;
	        break;
    		
        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            break;
    }

    return(gpio_error);
}

/****************************************************************************/
/* NAME:	t_gpio_error GPIO_EnableAltFunction()   					    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to enable an entire alternate function  */
/*              at once.                                                    */
/* PARAMETERS:	 															*/
/* IN:		t_gpio_alt_function alt_func: Alternate function to enable.     */
/* INOUT:	none															*/
/* OUT:		none                                          					*/
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong parameter is      */
/*						  passed as argument								*/
/*                        GPIO_UNSUPPORTED_FEATURE if the feature is not    */
/*						  supported on the given platform					*/
/*                        GPIO_OK               	    					*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant       										*/
/* REENTRANCY ISSUE: H/W registers are getting modified.          			*/

/****************************************************************************/
PUBLIC t_gpio_error GPIO_EnableAltFunction(t_gpio_alt_function alt_func)
{
    t_gpio_error    gpio_error = GPIO_OK;
    DBGENTER1(" (%d)", alt_func);

    switch (alt_func)
    {
        case GPIO_ALT_SSP0:
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa |= GPIO_SSP0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_SSP0_MASK;
            break;

        case GPIO_ALT_SSP1:
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa |= GPIO_SSP1_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_SSP1_MASK;
            break;

        case GPIO_ALT_I2C0:
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa |= GPIO_I2C0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_I2C0_MASK;
            break;

        case GPIO_ALT_I2C1_1:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb |= GPIO_I2C1_MASK1;
            break;

        case GPIO_ALT_I2C1_2:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C1_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb |= GPIO_I2C1_MASK2;
            break;

        case GPIO_ALT_I2C2_1:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb |= GPIO_I2C2_MASK1;
            break;

        case GPIO_ALT_I2C2_2:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C2_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb |= GPIO_I2C2_MASK2;
            break;

        case GPIO_ALT_I2C3_1:
            g_gpio_system_context.p_gpio_register[7]->gpio_afsa |= GPIO_I2C3_MASK1;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb |= GPIO_I2C3_MASK1;
            break;

        case GPIO_ALT_I2C3_2:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_I2C3_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb |= GPIO_I2C3_MASK1;
            break;
            
        case GPIO_ALT_I2C4:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C4_MASK;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb |= GPIO_I2C4_MASK;
            break;

        case GPIO_ALT_MM_CARD0:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa |= GPIO_MMC0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_MMC0_MASK1;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsa |= GPIO_MMC0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_MMC0_MASK2;
            break;

        case GPIO_ALT_MM_CARD1:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_MMC1_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_MMC1_MASK1;
            break;

        case GPIO_ALT_MM_CARD2:
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa |= GPIO_MMC2_MASK1;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_MMC2_MASK1;
            break;

        case GPIO_ALT_MM_CARD3:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_MMC3_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb |= GPIO_MMC3_MASK1;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsa &= ~GPIO_MMC3_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb |= GPIO_MMC3_MASK2;
            break;

        case GPIO_ALT_MM_CARD4:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_MMC4_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_MMC4_MASK1;
            break;

        case GPIO_ALT_MM_CARD5:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa |= GPIO_MMC5_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_MMC5_MASK1;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsa |= GPIO_MMC5_MASK2;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsb |= GPIO_MMC5_MASK2;
            break;

        default:
            gpio_error = gpio_enablealt(alt_func);
            break;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

PRIVATE t_gpio_error gpio_disablealt(t_gpio_alt_function alt_func)
{
    t_gpio_error    gpio_error = GPIO_OK;

    switch (alt_func)
    {
        case GPIO_ALT_MSP0:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_MSP0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_MSP0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_MSP0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_MSP0_MASK1;
            break;

        case GPIO_ALT_MSP1:
            g_gpio_system_context.p_gpio_register[1]->gpio_afsa &= ~GPIO_MSP1_MASK;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_MSP1_MASK;
            g_gpio_system_context.p_gpio_register[1]->gpio_dirc = GPIO_MSP1_MASK;
            g_gpio_system_context.p_gpio_register[1]->gpio_pdis &= ~GPIO_MSP1_MASK;
            break;

        case GPIO_ALT_MSP2:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_MSP2_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_MSP2_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_MSP2_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_MSP2_MASK;
            break;

        case GPIO_ALT_UART_0:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_UART0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_UART0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_UART0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_UART0_MASK1;

            g_gpio_system_context.p_gpio_register[1]->gpio_afsa &= ~GPIO_UART0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_UART0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_dirc = GPIO_UART0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_pdis &= ~GPIO_UART0_MASK2;
            break;

        case GPIO_ALT_UART_1:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_UART1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_UART1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_UART1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_UART1_MASK1;
            break;

        case GPIO_ALT_UART_2:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_UART2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_UART2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_UART2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_UART2_MASK1;

            g_gpio_system_context.p_gpio_register[1]->gpio_afsa &= ~GPIO_UART2_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_UART2_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_dirc = GPIO_UART2_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_pdis &= ~GPIO_UART2_MASK2;
            break;

        case GPIO_ALT_LCD_PANEL:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_LCD_PANEL_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_LCD_PANEL_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_dirc = GPIO_LCD_PANEL_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_pdis &= ~GPIO_LCD_PANEL_MASK1;

            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_LCD_PANEL_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_LCD_PANEL_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_dirc = GPIO_LCD_PANEL_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_pdis &= ~GPIO_LCD_PANEL_MASK2;

            g_gpio_system_context.p_gpio_register[5]->gpio_afsa &= ~GPIO_LCD_PANEL_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_afsb &= ~GPIO_LCD_PANEL_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_dirc = GPIO_LCD_PANEL_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_pdis &= ~GPIO_LCD_PANEL_MASK3;
            break;

        case GPIO_ALT_KEYPAD:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_KEYPAD_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_KEYPAD_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_dirc = GPIO_KEYPAD_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_pdis &= ~GPIO_KEYPAD_MASK1;

            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_KEYPAD_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_KEYPAD_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_dirc = GPIO_KEYPAD_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_pdis &= ~GPIO_KEYPAD_MASK2;

            g_gpio_system_context.p_gpio_register[5]->gpio_afsa &= ~GPIO_KEYPAD_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_afsb &= ~GPIO_KEYPAD_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_dirc = GPIO_KEYPAD_MASK3;
            g_gpio_system_context.p_gpio_register[5]->gpio_pdis &= ~GPIO_KEYPAD_MASK3;
            break;

        case GPIO_ALT_HSIT0:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_HSIT0_MASK_1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_HSIT0_MASK_1;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_HSIT0_MASK_1;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_HSIT0_MASK_1;

            g_gpio_system_context.p_gpio_register[7]->gpio_afsa &= ~GPIO_HSIT0_MASK_2;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb &= ~GPIO_HSIT0_MASK_2;
            g_gpio_system_context.p_gpio_register[7]->gpio_dirc = GPIO_HSIT0_MASK_2;
            g_gpio_system_context.p_gpio_register[7]->gpio_pdis &= ~GPIO_HSIT0_MASK_2;
            break;

        case GPIO_ALT_HSIR0:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_HSIR0_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_HSIR0_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_HSIR0_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_HSIR0_MASK;
            break;

        case GPIO_ALT_USB_OTG:
            g_gpio_system_context.p_gpio_register[8]->gpio_afsa &= ~GPIO_USB_OTG_MASK;
            g_gpio_system_context.p_gpio_register[8]->gpio_afsb &= ~GPIO_USB_OTG_MASK;
            g_gpio_system_context.p_gpio_register[8]->gpio_dirc = GPIO_USB_OTG_MASK;
            g_gpio_system_context.p_gpio_register[8]->gpio_pdis &= ~GPIO_USB_OTG_MASK;
            break;

        case GPIO_ALT_NAND_FLASH:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_NAND_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_NAND_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_dirc = GPIO_NAND_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_pdis &= ~GPIO_NAND_MASK1;

            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_NAND_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_NAND_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_dirc = GPIO_NAND_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_pdis &= ~GPIO_NAND_MASK2;

            g_gpio_system_context.p_gpio_register[3]->gpio_afsa &= ~GPIO_NAND_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsb &= ~GPIO_NAND_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_dirc = GPIO_NAND_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_pdis &= ~GPIO_NAND_MASK3;

            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_NAND_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_NAND_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_dirc = GPIO_NAND_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_pdis &= ~GPIO_NAND_MASK4;
            break;

        case GPIO_ALT_NOR_FLASH:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_NOR_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_NOR_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_dirc = GPIO_NOR_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_pdis &= ~GPIO_NOR_MASK1;

            g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_NOR_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_NOR_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_dirc = GPIO_NOR_MASK2;
            g_gpio_system_context.p_gpio_register[2]->gpio_pdis &= ~GPIO_NOR_MASK2;

            g_gpio_system_context.p_gpio_register[3]->gpio_afsa &= ~GPIO_NOR_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsb &= ~GPIO_NOR_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_dirc = GPIO_NOR_MASK3;
            g_gpio_system_context.p_gpio_register[3]->gpio_pdis &= ~GPIO_NOR_MASK3;

            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_NOR_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_NOR_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_dirc = GPIO_NOR_MASK4;
            g_gpio_system_context.p_gpio_register[4]->gpio_pdis &= ~GPIO_NOR_MASK4;
            break;
            
        case GPIO_ALT_SPI0:
    		g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_SPI0_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_SPI0_MASK1;
           	g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_SPI0_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_SPI0_MASK1;
            
			g_gpio_system_context.p_gpio_register[7]->gpio_afsa &= ~GPIO_SPI0_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb &= ~GPIO_SPI0_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_dirc = GPIO_SPI0_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_pdis &= ~GPIO_SPI0_MASK2;
			break;
			
		case GPIO_ALT_SPI1:
    		g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_SPI1_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_SPI1_MASK;
       		g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_SPI1_MASK;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_SPI1_MASK;
	        break;
	    
	    case GPIO_ALT_SPI3:
    		g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_SPI3_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_SPI3_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_SPI3_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_SPI3_MASK1;
            
			g_gpio_system_context.p_gpio_register[1]->gpio_afsa &= ~GPIO_SPI3_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_SPI3_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_dirc = GPIO_SPI3_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_SPI3_MASK2;
			break;
			
    	case GPIO_ALT_STMAPE_1:
    		g_gpio_system_context.p_gpio_register[2]->gpio_afsa &= ~GPIO_STMAPE_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_STMAPE_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa = GPIO_STMAPE_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb &= ~GPIO_STMAPE_MASK1;
	        break;
	        
	   	case GPIO_ALT_STMAPE_2:
    		g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_STMAPE_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_STMAPE_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa = GPIO_STMAPE_MASK2;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_STMAPE_MASK2;
	        break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            break;
    }

    return(gpio_error);
}

/****************************************************************************/
/* NAME:	t_gpio_error GPIO_DisableAltFunction()   					    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to disable an entire alternate function */
/*              at once.                                                    */
/* PARAMETERS:	 															*/
/* IN:		t_gpio_alt_function alt_func: Alternate function to disable.    */
/* INOUT:	none															*/
/* OUT:		none                                          					*/
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong parameter is      */
/*                        passed as argument								*/
/*                        GPIO_UNSUPPORTED_FEATURE if the feature is not    */
/*						  supported on the given platform					*/
/*						  GPIO_OK               	    					*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant       										*/
/* REENTRANCY ISSUE: H/W registers are getting modified.          			*/

/****************************************************************************/
PUBLIC t_gpio_error GPIO_DisableAltFunction(t_gpio_alt_function alt_func)
{
    t_gpio_error    gpio_error = GPIO_OK;

    switch (alt_func)
    {
        case GPIO_ALT_SSP0:
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_SSP0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_SSP0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_dirc = GPIO_SSP0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_pdis &= ~GPIO_SSP0_MASK;
            break;

        case GPIO_ALT_SSP1:
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_SSP1_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_SSP1_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_dirc = GPIO_SSP1_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_pdis &= ~GPIO_SSP1_MASK;
            break;

        case GPIO_ALT_I2C0:
            g_gpio_system_context.p_gpio_register[4]->gpio_afsa &= ~GPIO_I2C0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_afsb &= ~GPIO_I2C0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_dirc = GPIO_I2C0_MASK;
            g_gpio_system_context.p_gpio_register[4]->gpio_pdis &= ~GPIO_I2C0_MASK;
            break;

        case GPIO_ALT_I2C1_1:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_I2C1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_I2C1_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_I2C1_MASK1;
            break;

        case GPIO_ALT_I2C1_2:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C1_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_I2C1_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_I2C1_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_I2C1_MASK2;
            break;

        case GPIO_ALT_I2C2_1:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_I2C2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_I2C2_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_I2C2_MASK1;
            break;

        case GPIO_ALT_I2C2_2:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C2_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_I2C2_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_I2C2_MASK2;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_I2C2_MASK2;
            break;

        case GPIO_ALT_I2C3_1:
            g_gpio_system_context.p_gpio_register[7]->gpio_afsa &= ~GPIO_I2C3_MASK1;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb &= ~GPIO_I2C3_MASK1;
            g_gpio_system_context.p_gpio_register[7]->gpio_dirc = GPIO_I2C3_MASK1;
            g_gpio_system_context.p_gpio_register[7]->gpio_pdis &= ~GPIO_I2C3_MASK1;
            break;

        case GPIO_ALT_I2C3_2:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_I2C3_MASK2;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_I2C3_MASK2;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_I2C3_MASK2;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_I2C3_MASK2;
            break;
            
        case GPIO_ALT_I2C4:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_I2C4_MASK;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_I2C4_MASK;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_I2C4_MASK;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_I2C4_MASK;
            break;

        case GPIO_ALT_MM_CARD0:
            g_gpio_system_context.p_gpio_register[0]->gpio_afsa &= ~GPIO_MMC0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_afsb &= ~GPIO_MMC0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_dirc = GPIO_MMC0_MASK1;
            g_gpio_system_context.p_gpio_register[0]->gpio_pdis &= ~GPIO_MMC0_MASK1;

            g_gpio_system_context.p_gpio_register[1]->gpio_afsa &= ~GPIO_MMC0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_afsb &= ~GPIO_MMC0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_dirc = GPIO_MMC0_MASK2;
            g_gpio_system_context.p_gpio_register[1]->gpio_pdis &= ~GPIO_MMC0_MASK2;
            break;

        case GPIO_ALT_MM_CARD1:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_MMC1_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_MMC1_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_MMC1_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_MMC1_MASK1;
            break;

        case GPIO_ALT_MM_CARD2:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_MMC2_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_MMC2_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_MMC2_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_MMC2_MASK1;
            break;

        case GPIO_ALT_MM_CARD3:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa |= GPIO_MMC3_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb |= GPIO_MMC3_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_MMC3_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_MMC3_MASK1;

            g_gpio_system_context.p_gpio_register[7]->gpio_afsa |= GPIO_MMC3_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_afsb &= ~GPIO_MMC3_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_dirc = GPIO_MMC3_MASK2;
            g_gpio_system_context.p_gpio_register[7]->gpio_pdis &= ~GPIO_MMC3_MASK2;
            break;

        case GPIO_ALT_MM_CARD4:
            g_gpio_system_context.p_gpio_register[6]->gpio_afsa &= ~GPIO_MMC4_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_afsb &= ~GPIO_MMC4_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_dirc = GPIO_MMC4_MASK1;
            g_gpio_system_context.p_gpio_register[6]->gpio_pdis &= ~GPIO_MMC4_MASK1;
            break;

        case GPIO_ALT_MM_CARD5:
            g_gpio_system_context.p_gpio_register[2]->gpio_afsa |= GPIO_MMC5_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_afsb |= GPIO_MMC5_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_dirc = GPIO_MMC5_MASK1;
            g_gpio_system_context.p_gpio_register[2]->gpio_pdis &= ~GPIO_MMC5_MASK1;

            g_gpio_system_context.p_gpio_register[3]->gpio_afsa |= GPIO_MMC5_MASK2;
            g_gpio_system_context.p_gpio_register[3]->gpio_afsb &= ~GPIO_MMC5_MASK2;
            g_gpio_system_context.p_gpio_register[3]->gpio_dirc = GPIO_MMC5_MASK2;
            g_gpio_system_context.p_gpio_register[3]->gpio_pdis &= ~GPIO_MMC5_MASK2;
            break;

        default:
            gpio_error = gpio_disablealt(alt_func);
            break;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************/
/* NAME:	t_gpio_error GPIO_SetPinConfig()            	    		    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This API sets configuration to one GPIO pin at a time.      */
/* PARAMETERS:	 															*/
/* IN:		t_gpio_pin pin: Pin for which config parameters apply.          */
/* IN:      t_gpio_config config : General configuration parameters.        */
/* INOUT:	none															*/
/* OUT:		none                                                            */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong or NULL parameter */
/*                        is passed as argument							    */
/*                        GPIO_UNSUPPORTED_FEATURE if the feature is not	*/
/*						  supported on the given platform					*/
/*						  GPIO_OK               	    					*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										    */
/* REENTRANCY ISSUE: H/W registers are getting modified.                    */
/* COMMENTS: 1> For __STN_8815 & __STN8820, If a pin is configured for 		*/
/*              interrupt event,it is not only configured, but enabled also.*/
/*           2> __STN_8815 & __STN8820 doesn't support GPIO_TRIG_HIGH_LEVEL */
/*              and GPIO_TRIG_LOW_LEVEL interrupt triggering.               */

/****************************************************************************/
PUBLIC t_gpio_error GPIO_SetPinConfig(t_gpio_pin pin, t_gpio_config config)
{
    t_gpio_error    gpio_error = GPIO_OK;
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;

    DBGENTER1(" configuring (%d) pin", pin);

    if (pin > GPIO_PIN_267)
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
    pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
    gpio_pin_mask = (t_uint32) (1 << pin_no);

    switch (config.mode)
    {
        case GPIO_MODE_SOFTWARE:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa &= ~gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb &= ~gpio_pin_mask;
            break;

        case GPIO_MODE_ALT_FUNCTION_A:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa |= gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb &= ~gpio_pin_mask;
            break;

        case GPIO_MODE_ALT_FUNCTION_B:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa &= ~gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb |= gpio_pin_mask;
            break;

        case GPIO_MODE_ALT_FUNCTION_C:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa |= gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb |= gpio_pin_mask;
            break;

        case GPIO_MODE_LEAVE_UNCHANGED:
            break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            DBGEXIT0(gpio_error);
            return(gpio_error);
    }

    if (config.mode == GPIO_MODE_SOFTWARE)
    {
        switch (config.direction)
        {
            case GPIO_DIR_INPUT:
                /*Pull-up/Pull-down is enabled for the pin by default, if not disabled before*/
                /*Directly assinging value to gpio_dirc as a read returns gpio_dir register */
                g_gpio_system_context.p_gpio_register[block_no]->gpio_dirc = gpio_pin_mask;
                break;

            case GPIO_DIR_OUTPUT:
                g_gpio_system_context.p_gpio_register[block_no]->gpio_dirs = gpio_pin_mask;
                break;

            case GPIO_DIR_LEAVE_UNCHANGED:
                break;

            default:
                gpio_error = GPIO_INVALID_PARAMETER;
                DBGEXIT0(gpio_error);
                return(gpio_error);
        }

        switch (config.trig)
        {
            case GPIO_TRIG_DISABLE:
                g_gpio_system_context.p_gpio_register[block_no]->gpio_rimsc &= ~gpio_pin_mask;
                g_gpio_system_context.p_gpio_register[block_no]->gpio_fimsc &= ~gpio_pin_mask;
                break;

            case GPIO_TRIG_RISING_EDGE:
                g_gpio_system_context.p_gpio_register[block_no]->gpio_rimsc |= gpio_pin_mask;
                g_gpio_system_context.p_gpio_register[block_no]->gpio_fimsc &= ~gpio_pin_mask;
                break;

            case GPIO_TRIG_FALLING_EDGE:
                g_gpio_system_context.p_gpio_register[block_no]->gpio_rimsc &= ~gpio_pin_mask;
                g_gpio_system_context.p_gpio_register[block_no]->gpio_fimsc |= gpio_pin_mask;
                break;

            case GPIO_TRIG_BOTH_EDGES:
                g_gpio_system_context.p_gpio_register[block_no]->gpio_rimsc |= gpio_pin_mask;
                g_gpio_system_context.p_gpio_register[block_no]->gpio_fimsc |= gpio_pin_mask;
                break;

            case GPIO_TRIG_LEAVE_UNCHANGED:
                break;

            /*No break*/
            case GPIO_TRIG_HIGH_LEVEL:
            case GPIO_TRIG_LOW_LEVEL:
                gpio_error = GPIO_UNSUPPORTED_FEATURE;
                DBGEXIT0(gpio_error);
                return(gpio_error);

            default:
                gpio_error = GPIO_INVALID_PARAMETER;
                DBGEXIT0(gpio_error);
                return(gpio_error);
        }
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_EnableDefaultVotage()									    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables default pull-up or pull-down configuration for the */
/*              requested pin.                                                          */
/* PARAMETERS:																            */
/* IN : 		t_gpio_pin	pin:   Pin for which the request is intended.               */
/* INOUT:	    none														            */
/* OUT:		    none                                                                    */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong or NULL parameter 			*/
/*                        is passed as argument							    			*/
/*                        GPIO_UNSUPPORTED_FEATURE if the feature is not				*/
/*						  supported on the given platform								*/
/*						  GPIO_REQUEST_NOT_APPLICABLE ifthe pin is in alternate function*/
/*						  mode															*/
/*						  GPIO_OK               	    					 			*/
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_EnableDefaultVoltage(t_gpio_pin pin)
{
    t_gpio_error    gpio_error = GPIO_OK;
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;
    DBGENTER1(" enable default voltage on pin (%d)", pin);

    if (pin > GPIO_PIN_267)
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
    pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
    gpio_pin_mask = (t_uint32) (1 << pin_no);

    if
    (
        ((g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa & gpio_pin_mask) != 0)
    ||  ((g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb & gpio_pin_mask) != 0)
    )
    {
        /*Pin is in alternate mode, so request is not applicable.*/
        gpio_error = GPIO_REQUEST_NOT_APPLICABLE;
    }
    else if ((g_gpio_system_context.p_gpio_register[block_no]->gpio_dir & gpio_pin_mask) == gpio_pin_mask)
    {
        /*Pin is in software but direction is output, so request is not applicable.*/
        gpio_error = GPIO_REQUEST_NOT_APPLICABLE;
    }
    else
    {
        /* Pin is in software mode with direction as input
	       So Enable pull-up or pull-down */
        g_gpio_system_context.p_gpio_register[block_no]->gpio_pdis &= ~gpio_pin_mask;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_DisableDefaultVotage()								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables default pull-up or pull-down configuration for the*/
/*              requested pin.                                                          */
/* PARAMETERS:																            */
/* IN : 		t_gpio_pin	pin:   Pin for which the request is intended.               */
/* INOUT:	    none	            										            */
/* OUT:		    none                                                                    */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong or NULL parameter 			*/
/*                        is passed as argument							    			*/
/*                        GPIO_UNSUPPORTED_FEATURE if the feature is not				*/
/*						  supported on the given platform								*/
/*						  GPIO_REQUEST_NOT_APPLICABLE ifthe pin is in alternate function*/
/*						  mode															*/
/*						  GPIO_OK               	    					 			*/
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_DisableDefaultVoltage(t_gpio_pin pin)
{
    t_gpio_error    gpio_error = GPIO_OK;
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;
    DBGENTER1(" disable default voltage on pin (%d)", pin);

    if (pin > GPIO_PIN_267)
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
    pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
    gpio_pin_mask = (t_uint32) (1 << pin_no);

    if
    (
        ((g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa & gpio_pin_mask) != 0)
    ||  ((g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb & gpio_pin_mask) != 0)
    )
    {
        /*Pin is in alternate mode, so request is not applicable.*/
        gpio_error = GPIO_REQUEST_NOT_APPLICABLE;
    }
    else if ((g_gpio_system_context.p_gpio_register[block_no]->gpio_dir & gpio_pin_mask) == gpio_pin_mask)
    {
        /*Pin is in software but direction is output, so request is not applicable.*/
        gpio_error = GPIO_REQUEST_NOT_APPLICABLE;
    }
    else
    {
        /* Pin is in software mode with direction as input
	       So Disable pull-up or pull-down */
        g_gpio_system_context.p_gpio_register[block_no]->gpio_pdis |= gpio_pin_mask;    /*Disable PULL-UP and PULL-Down */
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_SetPinSleepMode()	    								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: It sets the status and behavior of the pin when the system is in        */
/*              sleep mode.                                                             */
/* PARAMETERS:																            */
/* IN : 		t_gpio_pin	pin: Pin no for which sleep mode config parameters apply.   */
/* IN : 		t_gpio_sleep_config sleep_config: Sleep mode configuration parameters.  */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong parameter is    				*/
/*                        passed as argument											*/
/*                        GPIO_UNSUPPORTED_FEATURE if the feature is not    			*/
/*						  supported on the given platform								*/
/*						  GPIO_OK               	    								*/
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W registers are being modified                                   */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_SetPinSleepMode(t_gpio_pin pin, t_gpio_sleep_config sleep_config)
{
    t_gpio_error    gpio_error = GPIO_OK;
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;

    DBGENTER1(" sleep mode configuration to  (%d) pin", pin);

    if (pin > GPIO_PIN_267)
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
    pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
    gpio_pin_mask = (t_uint32) (1 << pin_no);

    switch (sleep_config.sleep_mode)
    {
        case GPIO_SLEEP_MODE_INPUT_DEFAULTVOLT:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_slpm &= ~gpio_pin_mask;
            break;

        case GPIO_SLEEP_MODE_CONTROLLED_BY_GPIO:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_slpm |= gpio_pin_mask;
            break;

        case GPIO_SLEEP_MODE_LEAVE_UNCHANGED:
            break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            DBGEXIT0(gpio_error);
            return(gpio_error);
    }

    switch (sleep_config.wake)
    {
        case GPIO_WAKE_DISABLE:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_rwimsc &= ~gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_fwimsc &= ~gpio_pin_mask;
            break;

        case GPIO_WAKE_FALLING_EDGE:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_fwimsc |= gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_rwimsc &= ~gpio_pin_mask;
            break;

        case GPIO_WAKE_RISING_EDGE:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_rwimsc |= gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_fwimsc &= ~gpio_pin_mask;
            break;

        case GPIO_WAKE_BOTH_EDGES:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_rwimsc |= gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_fwimsc |= gpio_pin_mask;
            break;

        case GPIO_WAKE_LEAVE_UNCHANGED:
            break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            DBGEXIT0(gpio_error);
            return(gpio_error);
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_ReadGpioPin()	           								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This API returns the value on one GPIO pin.                             */
/* PARAMETERS:																            */
/* IN : 		t_gpio_pin	pin: GPIO pin to read.                                      */
/* INOUT:	    none    													            */
/* OUT:		    t_gpio_data* p_value Pointer to store pin value : high (GPIO_DATA_HIGH) */
/*                           or low (GPIO_DATA_LOW).                                    */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong pin number is paased as    	*/
/*						  argument                                       				*/
/*                        GPIO_OK               	    					            */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Re-entrant   								        	                */
/* REENTRANCY ISSUE: N/A                                                                */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_ReadGpioPin(t_gpio_pin pin, t_gpio_data *p_value)
{
    t_gpio_error    gpio_error = GPIO_OK;
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;
    t_gpio_pin      max_pin;

    DBGENTER1(" reading (%d) pin", pin);
    if (NULL == p_value)
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    max_pin = GPIO_PIN_267;

    if (pin <= max_pin)
    {
        block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
        pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
        gpio_pin_mask = (t_uint32) (1 << pin_no);
        if ((g_gpio_system_context.p_gpio_register[block_no]->gpio_dat & gpio_pin_mask) != GPIO_ALL_ZERO)
        {
            *p_value = GPIO_DATA_HIGH;
        }
        else
        {
            *p_value = GPIO_DATA_LOW;
        }
    }
    else
    {
        gpio_error = GPIO_INVALID_PARAMETER;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_SetGpioPin()	           								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: It sets the GPIO pin to high.                                           */
/* PARAMETERS:																            */
/* IN : 		t_gpio_pin	pin: GPIO pin to set HIGH                                   */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong pin number is paased as    	*/
/*						  argument                                       				*/
/*                        GPIO_OK               	    					            */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_SetGpioPin(t_gpio_pin pin)
{
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;
    t_gpio_error    gpio_error = GPIO_OK;
    t_gpio_pin      max_pin;

    DBGENTER1(" setting (%d) pin", pin);

    max_pin = GPIO_PIN_267;

    if (pin <= max_pin)
    {
        block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
        pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
        gpio_pin_mask = (t_uint32) (1 << pin_no);
        g_gpio_system_context.p_gpio_register[block_no]->gpio_dats = gpio_pin_mask;
    }
    else
    {
        gpio_error = GPIO_INVALID_PARAMETER;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_ClearGpioPin()	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: It sets the GPIO pin to clear.                                          */
/* PARAMETERS:																            */
/* IN : 		t_gpio_pin	pin: GPIO pin to set Clear                                  */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if wrong pin number is paased as    	*/
/*						  argument                                       				*/
/*                        GPIO_OK               	    					            */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_ClearGpioPin(t_gpio_pin pin)
{
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;
    t_gpio_error    gpio_error = GPIO_OK;
    t_gpio_pin      max_pin;

    DBGENTER1(" clearing (%d) pin", pin);

    max_pin = GPIO_PIN_267;

    if (pin <= max_pin)
    {
        block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
        pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
        gpio_pin_mask = (t_uint32) (1 << pin_no);
        g_gpio_system_context.p_gpio_register[block_no]->gpio_datc = gpio_pin_mask;
    }
    else
    {
        gpio_error = GPIO_INVALID_PARAMETER;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

PRIVATE t_gpio_error gpio_readblock1(t_gpio_block_id block_id, t_uint32 *p_value, t_uint32 mask)
{
    t_gpio_error    gpio_error = GPIO_OK;

    switch (block_id)
    {
        case GPIO_BLOCK_16_BITS_176_TO_191:
            *p_value = (g_gpio_system_context.p_gpio_register[5]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_184_TO_199:
            *p_value = (g_gpio_system_context.p_gpio_register[5]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[6]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_192_TO_207:
            *p_value = g_gpio_system_context.p_gpio_register[6]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_200_TO_215:
            *p_value = (g_gpio_system_context.p_gpio_register[6]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_208_TO_223:
            *p_value = (g_gpio_system_context.p_gpio_register[6]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_216_TO_231:
            *p_value = (g_gpio_system_context.p_gpio_register[6]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[7]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_224_TO_239:
            *p_value = g_gpio_system_context.p_gpio_register[7]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_232_TO_247:
            *p_value = (g_gpio_system_context.p_gpio_register[7]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_240_TO_255:
            *p_value = (g_gpio_system_context.p_gpio_register[7]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_248_TO_263:
            *p_value = (g_gpio_system_context.p_gpio_register[7]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[8]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_256_TO_267:
            *p_value = g_gpio_system_context.p_gpio_register[8]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            break;
    }

    return(gpio_error);
}

PRIVATE t_gpio_error gpio_readblock0(t_gpio_block_id block_id, t_uint32 *p_value, t_uint32 mask)
{
    t_gpio_error    gpio_error = GPIO_OK;

    switch (block_id)
    {
        case GPIO_BLOCK_16_BITS_32_TO_47:
            *p_value = g_gpio_system_context.p_gpio_register[1]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_40_TO_55:
            *p_value = (g_gpio_system_context.p_gpio_register[1]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_48_TO_63:
            *p_value = (g_gpio_system_context.p_gpio_register[1]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_56_TO_71:
            *p_value = (g_gpio_system_context.p_gpio_register[1]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[2]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_64_TO_79:
            *p_value = g_gpio_system_context.p_gpio_register[2]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_72_TO_87:
            *p_value = (g_gpio_system_context.p_gpio_register[2]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_80_TO_95:
            *p_value = (g_gpio_system_context.p_gpio_register[2]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_88_TO_103:
            *p_value = (g_gpio_system_context.p_gpio_register[2]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[3]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_96_TO_111:
            *p_value = g_gpio_system_context.p_gpio_register[3]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_104_TO_119:
            *p_value = (g_gpio_system_context.p_gpio_register[3]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_112_TO_127:
            *p_value = (g_gpio_system_context.p_gpio_register[3]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_120_TO_135:
            *p_value = (g_gpio_system_context.p_gpio_register[3]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[4]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_128_TO_143:
            *p_value = g_gpio_system_context.p_gpio_register[4]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_136_TO_151:
            *p_value = (g_gpio_system_context.p_gpio_register[4]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_144_TO_159:
            *p_value = (g_gpio_system_context.p_gpio_register[4]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_152_TO_167:
            *p_value = (g_gpio_system_context.p_gpio_register[4]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[5]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_160_TO_175:
            *p_value = g_gpio_system_context.p_gpio_register[5]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_168_TO_183:
            *p_value = (g_gpio_system_context.p_gpio_register[5]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        default:
            gpio_error = gpio_readblock1(block_id, p_value, mask);
            break;
    }

    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_ReadBlock()   	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This API returns the value from a 16 bit or 32 bit block of GPIOs       */
/*              with mask applied.                                                      */
/* PARAMETERS:																            */
/* IN : 		t_gpio_block_id block_id: Selects one 16 bit or 32 bit block in the GPIO*/
/* IN :         t_uint32 mask: Allows to mask certain bits of the returned value.       */
/* INOUT:	    none    													            */
/* OUT:		    t_uint32* p_value: Pointer to 32 bits value read from GPIOs, with       */
/*                                 mask applied.                                        */
/* RETURN: t_gpio_error : GPIO_INVALID_PARAMETER if NULL or wrong parameter is passed   */
/*                        GPIO_OK               	    					            */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant   						    				                */
/* REENTRANCY ISSUE: N/A                                                                */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_ReadBlock(t_gpio_block_id block_id, t_uint32 *p_value, t_uint32 mask)
{
    t_gpio_error    gpio_error = GPIO_OK;

    DBGENTER2("block_id (%x) mask(%lx)", block_id, mask);
    if (NULL == p_value)
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    switch (block_id)
    {
        case GPIO_BLOCK_32_BITS_0_TO_31:
            *p_value = (g_gpio_system_context.p_gpio_register[0]->gpio_dat) & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_32_TO_63:
            *p_value = g_gpio_system_context.p_gpio_register[1]->gpio_dat & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_64_TO_95:
            *p_value = g_gpio_system_context.p_gpio_register[2]->gpio_dat & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_96_TO_127:
            *p_value = (g_gpio_system_context.p_gpio_register[3]->gpio_dat) & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_128_TO_159:
            *p_value = g_gpio_system_context.p_gpio_register[4]->gpio_dat & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_160_TO_191:
            *p_value = g_gpio_system_context.p_gpio_register[5]->gpio_dat & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_192_TO_223:
            *p_value = g_gpio_system_context.p_gpio_register[6]->gpio_dat & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_224_TO_255:
            *p_value = g_gpio_system_context.p_gpio_register[7]->gpio_dat & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_32_BITS_256_TO_267:
            *p_value = g_gpio_system_context.p_gpio_register[8]->gpio_dat & (mask & GPIO_32BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_0_TO_15:
            *p_value = g_gpio_system_context.p_gpio_register[0]->gpio_dat & (mask & GPIO_16BIT_MASK);
            break;

        case GPIO_BLOCK_16_BITS_8_TO_23:
            *p_value = (g_gpio_system_context.p_gpio_register[0]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT8)) >> GPIO_SHIFT8;
            break;

        case GPIO_BLOCK_16_BITS_16_TO_31:
            *p_value = (g_gpio_system_context.p_gpio_register[0]->gpio_dat & ((mask & GPIO_16BIT_MASK) << GPIO_SHIFT16)) >> GPIO_SHIFT16;
            break;

        case GPIO_BLOCK_16_BITS_24_TO_39:
            *p_value = (g_gpio_system_context.p_gpio_register[0]->gpio_dat & ((mask & GPIO_8BIT_MASK) << GPIO_SHIFT24)) >> GPIO_SHIFT24;
            *p_value |= (g_gpio_system_context.p_gpio_register[1]->gpio_dat & ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK)) << GPIO_SHIFT8;
            break;

        default:
            gpio_error = gpio_readblock0(block_id, p_value, mask);
            break;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

PRIVATE t_gpio_error gpio_blockwrite1(t_gpio_block_id block_id, t_uint32 value, t_uint32 mask)
{
    t_gpio_error    gpio_error = GPIO_OK;
    switch (block_id)
    {
        case GPIO_BLOCK_16_BITS_56_TO_71:
            g_gpio_system_context.p_gpio_register[1]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[1]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[2]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[2]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_64_TO_79:
            g_gpio_system_context.p_gpio_register[2]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[2]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_72_TO_87:
            g_gpio_system_context.p_gpio_register[2]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[2]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_80_TO_95:
            g_gpio_system_context.p_gpio_register[2]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[2]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        case GPIO_BLOCK_16_BITS_88_TO_103:
            g_gpio_system_context.p_gpio_register[2]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[2]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[3]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[3]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_96_TO_111:
            g_gpio_system_context.p_gpio_register[3]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[3]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_104_TO_119:
            g_gpio_system_context.p_gpio_register[3]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[3]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_112_TO_127:
            g_gpio_system_context.p_gpio_register[3]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[3]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        case GPIO_BLOCK_16_BITS_120_TO_135:
            g_gpio_system_context.p_gpio_register[3]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[3]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[4]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[4]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            break;
    }

    return(gpio_error);
}

PRIVATE t_gpio_error gpio_blockwrite0(t_gpio_block_id block_id, t_uint32 value, t_uint32 mask)
{
    t_gpio_error    gpio_error = GPIO_OK;
    switch (block_id)
    {
        case GPIO_BLOCK_16_BITS_128_TO_143:
            g_gpio_system_context.p_gpio_register[4]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[4]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_136_TO_151:
            g_gpio_system_context.p_gpio_register[4]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[4]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_144_TO_159:
            g_gpio_system_context.p_gpio_register[4]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[4]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        case GPIO_BLOCK_16_BITS_152_TO_167:
            g_gpio_system_context.p_gpio_register[4]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[4]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[5]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[5]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_160_TO_175:
            g_gpio_system_context.p_gpio_register[5]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[5]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_168_TO_183:
            g_gpio_system_context.p_gpio_register[5]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[5]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_176_TO_191:
            g_gpio_system_context.p_gpio_register[5]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[5]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        case GPIO_BLOCK_16_BITS_184_TO_199:
            g_gpio_system_context.p_gpio_register[5]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[5]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[6]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[6]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_192_TO_207:
            g_gpio_system_context.p_gpio_register[6]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[6]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_200_TO_215:
            g_gpio_system_context.p_gpio_register[6]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[6]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_208_TO_223:
            g_gpio_system_context.p_gpio_register[6]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[6]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        case GPIO_BLOCK_16_BITS_216_TO_231:
            g_gpio_system_context.p_gpio_register[6]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[6]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[7]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[7]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_224_TO_239:
            g_gpio_system_context.p_gpio_register[7]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[7]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_232_TO_247:
            g_gpio_system_context.p_gpio_register[7]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[7]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_240_TO_255:
            g_gpio_system_context.p_gpio_register[7]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[7]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        case GPIO_BLOCK_16_BITS_248_TO_263:
            g_gpio_system_context.p_gpio_register[7]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[7]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[8]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[8]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_256_TO_267:
            g_gpio_system_context.p_gpio_register[8]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[8]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        default:
            gpio_error = gpio_blockwrite1(block_id, value, mask);
            break;
    }

    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_WriteBlock()   	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This API writes value to 16 bit or 32 bit block at a time with          */
/*              mask applied.                                                           */
/* PARAMETERS:																            */
/* IN : 		t_gpio_block_id block_id: Selects one 16 bit or 32 bit block in the GPIO*/
/* IN:		    t_uint32 value:  Value to write with mask applied.                      */
/* IN :         t_uint32 mask: Allows to write only certain bits, and preserve others   */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      t_gpio_error : GPIO_INVALID_PARAMETER if wrong parameter is passed      */
/*                       	   GPIO_OK            	    					            */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register are being modified                                    */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_WriteBlock(t_gpio_block_id block_id, t_uint32 value, t_uint32 mask)
{
    t_gpio_error    gpio_error = GPIO_OK;

    DBGENTER2("block_id (%x) mask(%lx)", block_id, mask);
    switch (block_id)
    {
        case GPIO_BLOCK_32_BITS_0_TO_31:
            g_gpio_system_context.p_gpio_register[0]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[0]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_32_TO_63:
            g_gpio_system_context.p_gpio_register[1]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[1]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_64_TO_95:
            g_gpio_system_context.p_gpio_register[2]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[2]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_96_TO_127:
            g_gpio_system_context.p_gpio_register[3]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[3]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_128_TO_159:
            g_gpio_system_context.p_gpio_register[4]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[4]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_160_TO_191:
            g_gpio_system_context.p_gpio_register[5]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[5]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_192_TO_223:
            g_gpio_system_context.p_gpio_register[6]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[6]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_224_TO_255:
            g_gpio_system_context.p_gpio_register[7]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[7]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_32_BITS_256_TO_267:
            g_gpio_system_context.p_gpio_register[8]->gpio_datc = (mask & GPIO_32BIT_MASK) & (~(value & GPIO_32BIT_MASK));
            g_gpio_system_context.p_gpio_register[8]->gpio_dats = (mask & GPIO_32BIT_MASK) & ((value & GPIO_32BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_0_TO_15:
            g_gpio_system_context.p_gpio_register[0]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[0]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_8_TO_23:
            g_gpio_system_context.p_gpio_register[0]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[0]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_16_TO_31:
            g_gpio_system_context.p_gpio_register[0]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[0]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        case GPIO_BLOCK_16_BITS_24_TO_39:
            g_gpio_system_context.p_gpio_register[0]->gpio_datc =
                (
                    ((mask & GPIO_8BIT_MASK) & (~(value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[0]->gpio_dats =
                (
                    ((mask & GPIO_8BIT_MASK) & ((value & GPIO_8BIT_MASK))) <<
                    GPIO_SHIFT24
                );
            g_gpio_system_context.p_gpio_register[1]->gpio_datc = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (~((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            g_gpio_system_context.p_gpio_register[1]->gpio_dats = ((mask >> GPIO_SHIFT8) & GPIO_8BIT_MASK) & (((value >> GPIO_SHIFT8) & GPIO_8BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_32_TO_47:
            g_gpio_system_context.p_gpio_register[1]->gpio_datc = (mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK));
            g_gpio_system_context.p_gpio_register[1]->gpio_dats = (mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK));
            break;

        case GPIO_BLOCK_16_BITS_40_TO_55:
            g_gpio_system_context.p_gpio_register[1]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            g_gpio_system_context.p_gpio_register[1]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT8
                );
            break;

        case GPIO_BLOCK_16_BITS_48_TO_63:
            g_gpio_system_context.p_gpio_register[1]->gpio_datc =
                (
                    ((mask & GPIO_16BIT_MASK) & (~(value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            g_gpio_system_context.p_gpio_register[1]->gpio_dats =
                (
                    ((mask & GPIO_16BIT_MASK) & ((value & GPIO_16BIT_MASK))) <<
                    GPIO_SHIFT16
                );
            break;

        default:
            gpio_error = gpio_blockwrite0(block_id, value, mask);
            break;
    }

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

PRIVATE void gpio_getirqstatusblock0(t_gpio_multiple_device_id device_id, t_gpio_pin_irq_status *p_status)
{
    t_uint32    count = 0, status;

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_0)
    {
        status = g_gpio_system_context.p_gpio_register[0]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_0; count <= (t_uint32) GPIO_PIN_31; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_1)
    {
        status = g_gpio_system_context.p_gpio_register[1]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_32; count <= (t_uint32) GPIO_PIN_63; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_2)
    {
        status = g_gpio_system_context.p_gpio_register[2]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_64; count <= (t_uint32) GPIO_PIN_95; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_3)
    {
        status = g_gpio_system_context.p_gpio_register[3]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_96; count <= (t_uint32) GPIO_PIN_127; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_4)
    {
        status = g_gpio_system_context.p_gpio_register[4]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_128; count <= (t_uint32) GPIO_PIN_159; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }
}

PRIVATE void gpio_getirqstatusblock1(t_gpio_multiple_device_id device_id, t_gpio_pin_irq_status *p_status)
{
    t_uint32    count = 0, status;

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_5)
    {
        status = g_gpio_system_context.p_gpio_register[5]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_160; count <= (t_uint32) GPIO_PIN_191; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_6)
    {
        status = g_gpio_system_context.p_gpio_register[6]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_192; count <= (t_uint32) GPIO_PIN_223; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_7)
    {
        status = g_gpio_system_context.p_gpio_register[7]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_224; count <= (t_uint32) GPIO_PIN_255; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_uint32) device_id & (t_uint32) GPIO_DEVICE_ID_8)
    {
        status = g_gpio_system_context.p_gpio_register[8]->gpio_mis;
        for (count = (t_uint32) GPIO_PIN_256; count <= (t_uint32) GPIO_PIN_267; count++)
        {
            p_status->gpio[count] = (t_bool) ((status & (t_uint32) (1 << (count % GPIO_NO_OF_PINS))) ? TRUE : FALSE);
        }
    }

    if ((t_gpio_multiple_device_id) GPIO_DEVICE_ID_INVALID == device_id)
    {
        for (count = (t_uint32) GPIO_PIN_0; count <= (t_uint32) GPIO_PIN_267; count++)
        {
            p_status->gpio[count] = FALSE;
        }
    }
}

/****************************************************************************************/
/* NAME:	void GPIO_GetMulptipleIRQStatus () 	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function returns the updated interrupt status of pins according    */
/*              to deviceId(s) passed.                                                  */
/* PARAMETERS:																            */
/* IN : 		t_gpio_multiple_device_id device_id: device id(s) for which updation is */
/*                                                   required in p_status               */
/* INOUT:	    none    													            */
/* OUT:		    p_status  : Pointer to store updated interrupt status of desired pins   */
/* RETURN:      void                                                                    */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant   							    			                */
/* REENTRANCY ISSUE: N/A                                                                */

/****************************************************************************************/
PUBLIC void GPIO_GetMulptipleIRQStatus(t_gpio_multiple_device_id device_id, t_gpio_pin_irq_status *p_status)
{
    DBGENTER0();

    gpio_getirqstatusblock0(device_id, p_status);

    gpio_getirqstatusblock1(device_id, p_status);

    DBGEXIT0(GPIO_OK);
}

/****************************************************************************************/
/* NAME:	void GPIO_GetWakeupStatus () 	 	      								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function returns the wakeup status of all the pins in a given      */
/*				controller ID.															*/
/* PARAMETERS:																            */
/* IN : 		t_gpio_ctrl_id ctrl_id: gpio controller id(s) 							*/
/* INOUT:	    none    													            */
/* OUT:		    p_wakeup_status  : wakeup status									    */
/* RETURN:      void                                                                    */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant   							    			                */
/* REENTRANCY ISSUE: N/A                                                                */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_GetWakeupStatus(t_gpio_ctrl_id ctrl_id, t_gpio_wakeup_status *p_wakeup_status)
{
    DBGENTER1(" %lx ", (t_uint32) ctrl_id);

    if (NULL == p_wakeup_status)
    {
        DBGEXIT0(GPIO_INVALID_PARAMETER);
        return(GPIO_INVALID_PARAMETER);
    }

    *p_wakeup_status = g_gpio_system_context.p_gpio_register[ctrl_id]->gpio_wks;

    DBGEXIT0(GPIO_OK);
    return(GPIO_OK);
}

PRIVATE t_gpio_error gpio_configuregpiomode(t_gpio_pin pin, t_gpio_config config, t_gpio_sleep_config sleep_config)
{
    t_gpio_error    gpio_error = GPIO_OK;
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;

    block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
    pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
    gpio_pin_mask = (t_uint32) (1 << pin_no);

    switch (config.mode)
    {
        case GPIO_MODE_SOFTWARE:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa &= ~gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb &= ~gpio_pin_mask;
            break;

        case GPIO_MODE_ALT_FUNCTION_A:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa |= gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb &= ~gpio_pin_mask;

            break;

        case GPIO_MODE_ALT_FUNCTION_B:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa &= ~gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb |= gpio_pin_mask;

            break;

        case GPIO_MODE_ALT_FUNCTION_C:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsa |= gpio_pin_mask;
            g_gpio_system_context.p_gpio_register[block_no]->gpio_afsb |= gpio_pin_mask;

            break;

        case GPIO_MODE_LEAVE_UNCHANGED:
            break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
    }

    return(gpio_error);
}

/****************************************************************************************/
/* NAME:	t_gpio_error GPIO_ConfigureGpioState() 	 	      						    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function configures the gpio for the deep sleep state   			*/
/*																						*/
/* PARAMETERS:																            */
/* IN : 		t_gpio_pin pin, t_gpio_config config,t_gpio_sleep_config sleep_config	*/
/*																			            */
/* OUT:		    t_gpio_error					 									    */
/* RETURN:      void                                                                    */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant   							    			                */
/* REENTRANCY ISSUE: N/A                                                                */

/****************************************************************************************/
PUBLIC t_gpio_error GPIO_ConfigureGpioState(t_gpio_pin pin, t_gpio_config config, t_gpio_sleep_config sleep_config)
{
    t_gpio_error    gpio_error = GPIO_OK;
    t_uint32        block_no;   /* GPIO block to access */
    t_uint32        pin_no;     /* GPIO pin in that block*/
    t_uint32        gpio_pin_mask;

    if (pin > GPIO_PIN_267)
    {
        gpio_error = GPIO_INVALID_PARAMETER;
        DBGEXIT0(gpio_error);
        return(gpio_error);
    }

    block_no = ((t_uint32) pin / GPIO_NO_OF_PINS);
    pin_no = ((t_uint32) pin % GPIO_NO_OF_PINS);
    gpio_pin_mask = (t_uint32) (1 << pin_no);
    if (config.mode == GPIO_MODE_SOFTWARE)
    {
        switch (config.direction)
        {
            case GPIO_DIR_INPUT:
                /* Pin is in software mode with direction as input
	       So Enable pull-up or pull-down */
                if (config.level == GPIO_LEVEL_PULLUP)
                {
                    g_gpio_system_context.p_gpio_register[block_no]->gpio_dats = gpio_pin_mask;
                }

                if (config.level == GPIO_LEVEL_PULLDOWN)
                {               /*  Pin is in software mode with direction as input */
                    g_gpio_system_context.p_gpio_register[block_no]->gpio_datc = gpio_pin_mask;
                }

                g_gpio_system_context.p_gpio_register[block_no]->gpio_pdis &= ~gpio_pin_mask;

                if (config.level == GPIO_HIGH_IMPEDENCE)
                {
                    g_gpio_system_context.p_gpio_register[block_no]->gpio_pdis |= gpio_pin_mask;
                }

                g_gpio_system_context.p_gpio_register[block_no]->gpio_dirc = gpio_pin_mask;
                break;

            case GPIO_DIR_OUTPUT:
                if (config.level == GPIO_LEVEL_LOW)
                {
                    g_gpio_system_context.p_gpio_register[block_no]->gpio_datc = gpio_pin_mask;
                }

                if (config.level == GPIO_LEVEL_HIGH)
                {
                    g_gpio_system_context.p_gpio_register[block_no]->gpio_dats = gpio_pin_mask;
                }

                g_gpio_system_context.p_gpio_register[block_no]->gpio_dirs = gpio_pin_mask;
                break;

            case GPIO_DIR_LEAVE_UNCHANGED:
                break;

            default:
                gpio_error = GPIO_INVALID_PARAMETER;
                DBGEXIT0(gpio_error);
                return(gpio_error);
        }
    }

    switch (sleep_config.sleep_mode)
    {
        case GPIO_SLEEP_MODE_INPUT_DEFAULTVOLT:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_slpm &= ~gpio_pin_mask;
            break;

        case GPIO_SLEEP_MODE_CONTROLLED_BY_GPIO:
            g_gpio_system_context.p_gpio_register[block_no]->gpio_slpm |= gpio_pin_mask;
            break;

        case GPIO_SLEEP_MODE_LEAVE_UNCHANGED:
            break;

        default:
            gpio_error = GPIO_INVALID_PARAMETER;
            DBGEXIT0(gpio_error);
            return(gpio_error);
    }

    gpio_error = gpio_configuregpiomode(pin, config, sleep_config);

    DBGEXIT0(gpio_error);
    return(gpio_error);
}

