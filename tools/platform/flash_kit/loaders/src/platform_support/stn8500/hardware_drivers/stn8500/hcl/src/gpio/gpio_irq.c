/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
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
#include "gpio_irq.h"
#include "gpio_irqp.h"
#include "gpio.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PRIVATE t_gpio_register *gp_gpio_register[GPIO_BLOCKS_COUNT];

/****************************************************************************/
/* NAME:	void GPIO_SetBaseAddress()										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stores base address with respect to the gpio   */
/*				block and does not check Peripheral ID and PCell ID.		*/
/* PARAMETERS:	 															*/
/* IN:		t_gpio_ctrl_id gpio_ctrl_id: GPIO controller id   				*/
/* IN:		t_logical_address gpio_base_address: Logical base address of 	*/
/*												 gpio block.				*/
/* INOUT:	none															*/
/* OUT:		none															*/
/*																			*/
/* RETURN: void                                                             */
/*--------------------------------------------------------------------------*/
/* REENTRANCY:	 Non Re-entrant												*/
/* REENTRANCY ISSUE: global array p_gpio_reg is getting modified 			*/

/****************************************************************************/
PUBLIC void GPIO_SetBaseAddress(t_gpio_ctrl_id gpio_ctrl_id, t_logical_address gpio_base_address)
{
    t_uint8 array_index;
 /*   if (GPIO_CTRL_ID_INVALID == gpio_ctrl_id)
    {
        return;
    }*/

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
            return;
    }

    /* Initializing the registers structure */
    gp_gpio_register[array_index] = (t_gpio_register *) gpio_base_address;

    return;
}

/****************************************************************************************/
/* NAME:	t_gpio_block_id GPIO_GetDeviceID()  	       								*/
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function returns the device id number associated with the          */
/*              interrupt source.                                                       */
/* PARAMETERS:																            */
/* IN : 		t_gpio_irq_src_id irq  : Is GPIO interrupt source                       */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      t_gpio_block_id :                                                       */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant   										                    */
/* REENTRANCY ISSUE: N/A                                                                */

/****************************************************************************************/
PUBLIC t_gpio_device_id GPIO_GetDeviceID(t_gpio_irq_src_id irq)
{
    if (irq <= GPIO_IRQ_SRC_31)
    {
        return(GPIO_DEVICE_ID_0);
    }
    else if ((irq >= GPIO_IRQ_SRC_32) && (irq <= GPIO_IRQ_SRC_63))
    {
        return(GPIO_DEVICE_ID_1);
    }
    else if ((irq >= GPIO_IRQ_SRC_64) && (irq <= GPIO_IRQ_SRC_95))
    {
        return(GPIO_DEVICE_ID_2);
    }
    else if ((irq >= GPIO_IRQ_SRC_96) && (irq <= GPIO_IRQ_SRC_127))
    {
        return(GPIO_DEVICE_ID_3);
    }
    else if ((irq >= GPIO_IRQ_SRC_128) && (irq <= GPIO_IRQ_SRC_159))
    {
        return(GPIO_DEVICE_ID_4);
    }
    else if ((irq >= GPIO_IRQ_SRC_160) && (irq <= GPIO_IRQ_SRC_191))
    {
        return(GPIO_DEVICE_ID_5);
    }
    else if ((irq >= GPIO_IRQ_SRC_192) && (irq <= GPIO_IRQ_SRC_223))
    {
        return(GPIO_DEVICE_ID_6);
    }
    else if ((irq >= GPIO_IRQ_SRC_224) && (irq <= GPIO_IRQ_SRC_255))
    {
        return(GPIO_DEVICE_ID_7);
    }
    else if ((irq >= GPIO_IRQ_SRC_256) && (irq <= GPIO_IRQ_SRC_267))
    {
        return(GPIO_DEVICE_ID_8);
    }
    else
    {
        return(GPIO_DEVICE_ID_INVALID);
    }
}

/****************************************************************************************/
/* NAME:	t_bool GPIO_IsPendingIRQSrc()            	   								*/
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function checks whether the GPIO interrupt source is active or not */
/* PARAMETERS:																            */
/* IN : 		t_gpio_irq_src_id irq_source	 Is it active or not                    */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      t_bool:  TRUE, if  it is active                                         */
/*                    : FALSE, If it is not active                                      */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant   										                    */
/* REENTRANCY ISSUE: N/A                                                                */

/****************************************************************************************/
PUBLIC t_bool GPIO_IsPendingIRQSrc(t_gpio_irq_src_id irq_source)
{
    t_gpio_irq_src_id   max_value = GPIO_IRQ_SRC_0;

    max_value = GPIO_IRQ_SRC_267;

    if (irq_source <= max_value)
    {
        if
        (
            (
                gp_gpio_register[((t_uint32) irq_source / GPIO_NO_OF_PINS)]->gpio_mis & (t_uint32)
                    (1 << ((t_uint32) irq_source % GPIO_NO_OF_PINS))
            ) != GPIO_ALL_ZERO
        )
        {
            return(TRUE);
        }
        else
        {
            return(FALSE);
        }
    }
    else
    {
        return(FALSE);
    }
}

/****************************************************************************************/
/* NAME:	void GPIO_ClearIRQSrc()            	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function allows to acknowledge an interrupt source.                */
/* PARAMETERS:																            */
/* IN : 		t_gpio_irq_src_id ack_irq: GPIO interrupt source to acknowledge.        */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      void                                                                    */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */

/****************************************************************************************/
PUBLIC void GPIO_ClearIRQSrc(t_gpio_irq_src_id ack_irq)
{
    t_gpio_irq_src_id   max_value = GPIO_IRQ_SRC_267;

    if (ack_irq <= max_value)
    {
        gp_gpio_register[((t_uint32) ack_irq / GPIO_NO_OF_PINS)]->gpio_ic |= (t_uint32) (1 << ((t_uint32) ack_irq % GPIO_NO_OF_PINS));
    }
 /*   else
    {
        return;
    }*/
}

/****************************************************************************************/
/* NAME:	void GPIO_EnableIRQSrc()          	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function enables an interrupt source.                              */
/* PARAMETERS:																            */
/* IN : 		t_gpio_irq_src_id en_irq: GPIO interrupt source to enable.              */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      void                                                                    */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */
/* COMMENTS:  This API is not applicable for STn8820 as interrupt source gets enabled   */
/*            in GPIO_setPinConfig() API itself.                                        */

/****************************************************************************************/
PUBLIC void GPIO_EnableIRQSrc(t_gpio_irq_src_id en_irq)
{
    return;
}

/****************************************************************************************/
/* NAME:	void GPIO_DisableIRQSrc()          	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function disables an interrupt source.                              */
/* PARAMETERS:																            */
/* IN : 		t_gpio_irq_src_id dis_irq: GPIO interrupt source to disable.              */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      void                                                                    */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */

/****************************************************************************************/
PUBLIC void GPIO_DisableIRQSrc(t_gpio_irq_src_id dis_irq)
{
    if (dis_irq <= GPIO_IRQ_SRC_267)
    {
        gp_gpio_register[((t_uint32) dis_irq / GPIO_NO_OF_PINS)]->gpio_rimsc &= ~(t_uint32) (1 << ((t_uint32) dis_irq % GPIO_NO_OF_PINS));
        gp_gpio_register[((t_uint32) dis_irq / GPIO_NO_OF_PINS)]->gpio_fimsc &= ~(t_uint32) (1 << ((t_uint32) dis_irq % GPIO_NO_OF_PINS));
    }
  /*  else
    {
        return;
    }*/
}

/****************************************************************************************/
/* NAME:	t_gpio_irq_src_id	GPIO_GetIRQSrc()          	       			    	    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function returns lowest active interrupt source for the device id  */
/*              passed.                                                                 */
/* PARAMETERS:																            */
/* IN : 		t_gpio_device_id device_id : GPIO deviceid                              */
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      t_gpio_irq_src_id :lowest active interrupt source.                      */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	 Re-entrant   						    				                */
/* REENTRANCY ISSUE: N/A                                                                */
/* COMMENT: No error management is provided, so user is supposed to pass currect        */
/*          device id. and this function should be called when irqsource is active for  */
/*          device.                                                                     */

/****************************************************************************************/
PUBLIC t_gpio_irq_src_id GPIO_GetIRQSrc(t_gpio_device_id device_id)
{
    t_uint32    irq_status = 0;
    t_uint32    irq_src = 0;

    switch (device_id)
    {
        case GPIO_DEVICE_ID_0:
            irq_status = gp_gpio_register[0]->gpio_mis;
            break;

        case GPIO_DEVICE_ID_1:
            irq_status = gp_gpio_register[1]->gpio_mis;
            break;

        case GPIO_DEVICE_ID_2:
            irq_status = gp_gpio_register[2]->gpio_mis;
            break;

        case GPIO_DEVICE_ID_3:
            irq_status = gp_gpio_register[3]->gpio_mis;
            break;

        case GPIO_DEVICE_ID_4:
            irq_status = gp_gpio_register[4]->gpio_mis;
            break;

        case GPIO_DEVICE_ID_5:
            irq_status = gp_gpio_register[5]->gpio_mis;
            break;

        case GPIO_DEVICE_ID_6:
            irq_status = gp_gpio_register[6]->gpio_mis;
            break;
            
        case GPIO_DEVICE_ID_7:
            irq_status = gp_gpio_register[7]->gpio_mis;
            break;
            
        case GPIO_DEVICE_ID_8:
            irq_status = gp_gpio_register[8]->gpio_mis;
            break;
    }

    irq_status = irq_status &~(irq_status - 1);
    irq_src = (irq_status & GPIO_IRQ_BIT_POSITION_1) ? 1 : 0;
    irq_src |= ((irq_status & GPIO_IRQ_BIT_POSITION_2) ? 1 : 0) << 1;
    irq_src |= ((irq_status & GPIO_IRQ_BIT_POSITION_3) ? 1 : 0) << 2;
    irq_src |= ((irq_status & GPIO_IRQ_BIT_POSITION_4) ? 1 : 0) << 3;
    irq_src |= ((irq_status & GPIO_IRQ_BIT_POSITION_5) ? 1 : 0) << 4;

    return((t_gpio_irq_src_id) irq_src);
}

/****************************************************************************************/
/* NAME:	void GPIO_EnableInterruptSrc()     	       								    */
/*--------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function enables an interrupt source.                              */
/* PARAMETERS:																            */
/* IN : 		t_gpio_irq_src_id en_irq: GPIO interrupt source to enable.              */
/*				t_gpio_trig : Defines interrupt configuration for a gpio pin 			*/
/* INOUT:	    none    													            */
/* OUT:		    none                                                                    */
/* RETURN:      void                                                                    */
/*--------------------------------------------------------------------------------------*/
/* REENTRANCY:	Non Re-entrant   										                */
/* REENTRANCY ISSUE: H/W register is being modified                                     */
/* COMMENTS: 									                                        */
/****************************************************************************************/

PUBLIC void GPIO_EnableInterruptSrc(t_gpio_irq_src_id en_irq, t_gpio_trig gpio_trig)
{
    if (en_irq <= GPIO_IRQ_SRC_267)
    {
        switch (gpio_trig)
        {
            case GPIO_TRIG_RISING_EDGE:
                gp_gpio_register[((t_uint32) en_irq / GPIO_NO_OF_PINS)]->gpio_rimsc |= ((t_uint32) (1 << ((t_uint32) en_irq % GPIO_NO_OF_PINS)));
                gp_gpio_register[((t_uint32) en_irq / GPIO_NO_OF_PINS)]->gpio_fimsc &= ~((t_uint32) (1 << ((t_uint32) en_irq % GPIO_NO_OF_PINS)));
                break;

            case GPIO_TRIG_FALLING_EDGE:
                gp_gpio_register[((t_uint32) en_irq / GPIO_NO_OF_PINS)]->gpio_rimsc &= ~((t_uint32) (1 << ((t_uint32) en_irq % GPIO_NO_OF_PINS)));
                gp_gpio_register[((t_uint32) en_irq / GPIO_NO_OF_PINS)]->gpio_fimsc |= ((t_uint32) (1 << ((t_uint32) en_irq % GPIO_NO_OF_PINS)));
                break;

            case GPIO_TRIG_BOTH_EDGES:
                gp_gpio_register[((t_uint32) en_irq / GPIO_NO_OF_PINS)]->gpio_rimsc |= ((t_uint32) (1 << ((t_uint32) en_irq % GPIO_NO_OF_PINS)));
                gp_gpio_register[((t_uint32) en_irq / GPIO_NO_OF_PINS)]->gpio_fimsc |= ((t_uint32) (1 << ((t_uint32) en_irq % GPIO_NO_OF_PINS)));
                break;

            case GPIO_TRIG_LEAVE_UNCHANGED:
                break;
        }
    }
  /*  else
    {
        return;
    }*/
}

