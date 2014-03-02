/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services routines for the I2C controller
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "memory_mapping.h"

#include "services.h"
#include "i2c_services.h"
#include "i2c.h"
#include "i2c_irq.h"
#include "gic.h"
#if ((defined ST_8500ED)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V1)||(defined ST_8500V2)||(defined ST_HREFV2))
#include "gpio.h"
#endif

/*--------------------------------------------------------------------------*
 * Global Data																*
 *--------------------------------------------------------------------------*/

#if ((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
    volatile t_bool             I2c_transferOk[4];
    volatile t_bool             I2c_transferFailed[4];
    volatile t_ser_i2c_callback I2c_last_transfer_status[4];
    t_bool                      I2C0_stat, I2C1_stat, I2C2_stat, I2C3_stat;
    t_callback                  callback_i2c[4];
#else
    volatile t_bool             I2c_transferOk[5];
    volatile t_bool             I2c_transferFailed[5];
    volatile t_ser_i2c_callback I2c_last_transfer_status[5];
    t_bool                      I2C0_stat, I2C1_stat, I2C2_stat, I2C3_stat, I2C4_stat;
    t_callback                  callback_i2c[5];
#endif

PRIVATE void                ser_i2c_InterruptHandler(IN t_uint32 irq);

/*--------------------------------------------------------------------------*
 * Private functions														*
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    ser_i2c_InterruptHandler		                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is called from I2C Interrupt handlers . This is*/
/* 				generic function which is called from I2C0, I2C1, I2C2, I2C3*/
/*                                                            and I2C4 ISRs	*/
/*                                                                          */
/* PARAMETERS: t_uint32 id                                                  */
/*  		                                                                */
/*																			*/
/* RETURN:                                                                  */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE void ser_i2c_InterruptHandler(IN t_uint32 id)
{
    t_i2c_error         err_status;
    t_i2c_irq_src_id    irq_src_id;
    t_i2c_irq_status    irq_status;
    t_i2c_active_event  cur_event;
    t_i2c_device_id     i2c_id; 

    /* Identify the IRQ source.*/
    irq_src_id = I2C_GetIRQSrc((t_i2c_device_id) id);

    /* Identify the IRQ status.*/
    I2C_GetIRQSrcStatus(irq_src_id, &irq_status);

    err_status = (t_i2c_error) I2C_FilterProcessIRQSrc(&irq_status, &cur_event);
    if (0 > (t_sint32) err_status)
    {
        I2C_PRINT("I2C_FilterProcessIRQSrc::Error in Procesing Interrupt for I2C%d", id);

        /* exit(-1); */
        return;
    }

    i2c_id = (t_i2c_device_id) cur_event.id;

    I2c_last_transfer_status[i2c_id].i2c_event.id = cur_event.id;
    I2c_last_transfer_status[i2c_id].i2c_event.type = cur_event.type;
    I2c_last_transfer_status[i2c_id].i2c_event.transfer_data = cur_event.transfer_data;
    
    

    switch (cur_event.type)
    {
        case I2C_TRANSFER_OK_EVENT:
        case I2C_CANCEL_EVENT:
            if (I2C0 == (t_i2c_device_id) cur_event.id)
            {
                I2C0_stat = (t_bool) 1;
                I2c_transferOk[0] = (t_bool) TRUE;
            }
            else if(I2C1 == (t_i2c_device_id) cur_event.id)
            {
                I2C1_stat = (t_bool) 1;
                I2c_transferOk[1] = (t_bool) TRUE;
            }
            
            else if (I2C2 == (t_i2c_device_id) cur_event.id)
            {
                I2C2_stat = (t_bool) 1;
                I2c_transferOk[2] = (t_bool) TRUE;
            }
            else if (I2C3 == (t_i2c_device_id) cur_event.id)
            {
                I2C3_stat = (t_bool) 1;
                I2c_transferOk[3] = (t_bool) TRUE;
            }
        #if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
            else if (I2C4 == (t_i2c_device_id) cur_event.id)
            {
                I2C4_stat = (t_bool) 1;
                I2c_transferOk[4] = (t_bool) TRUE;
            }
        #endif
            break;

        case I2C_INTERNAL_ERROR_EVENT:
        case I2C_BUS_ERROR_DETECTED_START_EVENT:
        case I2C_BUS_ERROR_DETECTED_STOP_EVENT:
        case I2C_TRANSMIT_FIFO_OVERRUN_EVENT:
        case I2C_BUS_ERROR_EVENT:
        case I2C_ARBITRATION_LOST_ERROR_EVENT:
        case I2C_ABORT_NACK_ON_ADDRESS_EVENT:
        case I2C_ABORT_NACK_ON_DATA_EVENT:
        case I2C_ABORT_ACK_ON_MASTER_CODE_EVENT:
        case I2C_OVERFLOW_EVENT:
            if (I2C0 == (t_i2c_device_id) cur_event.id)
            {
                I2c_transferFailed[0] = (t_bool) TRUE;
            }
            else if (I2C1 == (t_i2c_device_id) cur_event.id)
            {
                I2c_transferFailed[1] = (t_bool) TRUE;
            }
            else if (I2C2 == (t_i2c_device_id) cur_event.id)
            {
                I2c_transferFailed[2] = (t_bool) TRUE;
            }
            else if (I2C3 == (t_i2c_device_id) cur_event.id)
            {
                I2c_transferFailed[3] = (t_bool) TRUE;
            }
        #if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
            else if (I2C4 == (t_i2c_device_id) cur_event.id)
            {
                I2c_transferFailed[4] = (t_bool) TRUE;
            }
        #endif
            
            break;

        case I2C_INDEX_TX_EVENT:
        case I2C_DATA_TX_EVENT:
        case I2C_DATA_RX_EVENT:
        case I2C_WAITING_DATA_RX_EVENT:
        case I2C_TRANSMIT_FIFO_EMPTY_EVENT:
        case I2C_TRANSMIT_FIFO_FULL_EVENT:
        case I2C_RECEIVE_FIFO_EMPTY_EVENT:
        case I2C_RECEIVE_FIFO_NEARLY_FULL_EVENT:
        case I2C_RECEIVE_FIFO_FULL_EVENT:
        case I2C_READ_FROM_SLAVE_REQUEST_EVENT:
        case I2C_READ_FROM_SLAVE_EMPTY_EVENT:
        case I2C_WRITE_TO_SLAVE_REQUEST_EVENT:
        case I2C_SLAVE_TRANSACTION_DONE_EVENT:
            break;

        case I2C_NO_EVENT:
        default:
            I2C_PRINT("No Interrupt Processed\n");
            break;
    }

    if (I2C0 == (t_i2c_device_id) cur_event.id)
    {
        if (callback_i2c[0].fct != 0)
        {
            callback_i2c[0].fct(callback_i2c[0].param, (void *) I2c_last_transfer_status);
        }
    }
    else if (I2C1 == (t_i2c_device_id) cur_event.id)
    {
        if (callback_i2c[1].fct != 0)
        {
            callback_i2c[1].fct(callback_i2c[1].param, (void *) I2c_last_transfer_status);
        }
    }
    
    else if (I2C2 == (t_i2c_device_id) cur_event.id)
    {
        if (callback_i2c[2].fct != 0)
        {
            callback_i2c[2].fct(callback_i2c[2].param, (void *) I2c_last_transfer_status);
        }
    }
    else if (I2C3 == (t_i2c_device_id) cur_event.id)
    {
        if (callback_i2c[3].fct != 0)
        {
            callback_i2c[3].fct(callback_i2c[3].param, (void *) I2c_last_transfer_status);
        }
    }
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    else if (I2C4 == (t_i2c_device_id) cur_event.id)
    {
        if (callback_i2c[4].fct != 0)
        {
            callback_i2c[4].fct(callback_i2c[4].param, (void *) I2c_last_transfer_status);
        }
    }
#endif

  
}

/****************************************************************************/
/* NAME:    SER_I2C0_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is ISR for I2C0								*/
/* PARAMETERS	:                                                           */
/* 		IN  	: t_uint32 irq: I2C IRQ                           		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:None :                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_I2C0_InterruptHandler(IN t_uint32 irq)
{

    t_gic_error gic_error;
    gic_error = GIC_DisableItLine(GIC_I2C_0_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_DisableItLine::Error in Procesing Interrupt for I2C0");

        /*	    exit(-1);*/
        return;
    }

    ser_i2c_InterruptHandler((t_uint32) I2C0);
/*
    gic_error = GIC_AcknowledgeItLine(GIC_I2C_0_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_AcknowledgeItLine::Error in Procesing Interrupt for I2C0");
        return;

    }
*/
    gic_error = GIC_EnableItLine(GIC_I2C_0_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_EnableItLine::Error in Procesing Interrupt for I2C0");
        return;

        /*	    exit(-1);*/
    }

}

/****************************************************************************/
/* NAME:    SER_I2C1_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is ISR for I2C1								*/
/* PARAMETERS	:                                                           */
/* 		IN  	: t_uint32 irq: I2C IRQ                           		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:void :                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_I2C1_InterruptHandler(IN t_uint32 irq)
{

    t_gic_error gic_error;
    gic_error = GIC_DisableItLine(GIC_I2C_1_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_DisableItLine::Error in Procesing Interrupt for I2C1");

        /*	    exit(-1);*/
        return;
    }

    ser_i2c_InterruptHandler((t_uint32) I2C1);

/*
    gic_error = GIC_AcknowledgeItLine(GIC_I2C_1_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_AcknowledgeItLine::Error in Procesing Interrupt for I2C1");
        return;

    }
*/
    gic_error = GIC_EnableItLine(GIC_I2C_1_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_EnableItLine::Error in Procesing Interrupt for I2C1");
        return;

        /*	    exit(-1);*/
    }

}


/****************************************************************************/
/* NAME:    SER_I2C2_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is ISR for I2C2								*/
/* PARAMETERS	:                                                           */
/* 		IN  	: t_uint32 irq: I2C IRQ                           		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:void :                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_I2C2_InterruptHandler(IN t_uint32 irq)
{

    t_gic_error gic_error;
    gic_error = GIC_DisableItLine(GIC_I2C_2_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_DisableItLine::Error in Procesing Interrupt for I2C2");

        /*	    exit(-1);*/
        return;
    }

    ser_i2c_InterruptHandler((t_uint32) I2C2);

/*
    gic_error = GIC_AcknowledgeItLine(GIC_I2C_2_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_AcknowledgeItLine::Error in Procesing Interrupt for I2C2");
        return;

    }
*/
    gic_error = GIC_EnableItLine(GIC_I2C_2_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_EnableItLine::Error in Procesing Interrupt for I2C2");
        return;

        /*	    exit(-1);*/
    }

}


/****************************************************************************/
/* NAME:    SER_I2C3_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is ISR for I2C3								*/
/* PARAMETERS	:                                                           */
/* 		IN  	: t_uint32 irq: I2C IRQ                           		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:void :                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_I2C3_InterruptHandler(IN t_uint32 irq)
{

    t_gic_error gic_error;
    gic_error = GIC_DisableItLine(GIC_I2C_3_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_DisableItLine::Error in Procesing Interrupt for I2C3");

        /*	    exit(-1);*/
        return;
    }

    ser_i2c_InterruptHandler((t_uint32) I2C3);

/*
    gic_error = GIC_AcknowledgeItLine(GIC_I2C_3_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_AcknowledgeItLine::Error in Procesing Interrupt for I2C3");
        return;

    }
*/
    gic_error = GIC_EnableItLine(GIC_I2C_3_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_EnableItLine::Error in Procesing Interrupt for I2C3");
        return;

        /*	    exit(-1);*/
    }

}

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))

/****************************************************************************/
/* NAME:    SER_I2C4_InterruptHandler                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is ISR for I2C4								*/
/* PARAMETERS	:                                                           */
/* 		IN  	: t_uint32 irq: I2C IRQ                           		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:void :                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_I2C4_InterruptHandler(IN t_uint32 irq)
{

    t_gic_error gic_error;
    gic_error = GIC_DisableItLine(GIC_I2C_4_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_DisableItLine::Error in Procesing Interrupt for I2C4");

        /*	    exit(-1);*/
        return;
    }

    ser_i2c_InterruptHandler((t_uint32) I2C4);

/*
    gic_error = GIC_AcknowledgeItLine(GIC_I2C_4_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_AcknowledgeItLine::Error in Procesing Interrupt for I2C4");
        return;

    }
*/
    gic_error = GIC_EnableItLine(GIC_I2C_4_LINE);
    if (0 > (t_sint32) gic_error)
    {
        I2C_PRINT("GIC_EnableItLine::Error in Procesing Interrupt for I2C4");
        return;

        /*	    exit(-1);*/
    }

}

#endif

/****************************************************************************/
/* NAME:    SER_I2C_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for I2C initialization 				*/
/* PARAMETERS	:                                                           */
/* 		IN  	: t_uint8:  The I2C controller to be initialized       		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:None :                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_I2C_Init(IN t_uint8 mask)
{
    /* Initialize the base address of I2C */
    t_i2c_error error_i2c;
    t_gic_error error_gic;
    t_gic_func_ptr    old_handler;

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    /* AMBA & Kernel clock enable for Peripheral3-I2C0 */
    (*(volatile t_uint32 *)(PRCC_3_CTRL_REG_BASE_ADDR))                             |= I2C0_AMBA_CLK_EN_VAL;
    (*(volatile t_uint32 *)(PRCC_3_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) |= I2C0_KERNEL_CLK_EN_VAL;
    
    /* AMBA & Kernel clock enable for Peripheral1-I2C1/2/4 */
    (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR))                             |= I2C1_AMBA_CLK_EN_VAL;
    (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) |= I2C1_KERNEL_CLK_EN_VAL;
    
    /* AMBA & Kernel clock enable for Peripheral2-I2C3 */
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR))                             |= I2C3_AMBA_CLK_EN_VAL;
    (*(volatile t_uint32 *)(PRCC_2_CTRL_REG_BASE_ADDR + PRCC_KERNEL_CLK_EN_OFFSET)) |= I2C3_KERNEL_CLK_EN_VAL;
#endif


    if (INIT_I2C0 == mask)
    {
        I2C_SetBaseAddress(I2C0, I2C_0_REG_BASE_ADDR);
        error_i2c = I2C_Init(I2C0, I2C_0_REG_BASE_ADDR);
        if (0 > (t_sint32) error_i2c)
        {
            I2C_PRINT("I2C_Init::Error in Initializing I2C0");
            return;
        }
    }

    if (INIT_I2C1 == mask)
    {
        I2C_SetBaseAddress(I2C1, I2C_1_REG_BASE_ADDR);
        error_i2c = I2C_Init(I2C1, I2C_1_REG_BASE_ADDR);
        if (0 > (t_sint32) error_i2c)
        {
            I2C_PRINT("I2C_Init::Error in Initializing I2C1");
            return;
        }
    }
    
    if (INIT_I2C2 == mask)
    {
        I2C_SetBaseAddress(I2C2, I2C_2_REG_BASE_ADDR);
        error_i2c = I2C_Init(I2C2, I2C_2_REG_BASE_ADDR);
        if (0 > (t_sint32) error_i2c)
        {
            I2C_PRINT("I2C_Init::Error in Initializing I2C2");
            return;
        }
    }

    if (INIT_I2C3 == mask)
    {
        I2C_SetBaseAddress(I2C3, I2C_3_REG_BASE_ADDR);
        error_i2c = I2C_Init(I2C3, I2C_3_REG_BASE_ADDR);
        if (0 > (t_sint32) error_i2c)
        {
            I2C_PRINT("I2C_Init::Error in Initializing I2C3");
            return;
        }
    }
    
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    if (INIT_I2C4 == mask)
    {
        I2C_SetBaseAddress(I2C4, I2C_4_REG_BASE_ADDR);
        error_i2c = I2C_Init(I2C4, I2C_4_REG_BASE_ADDR);
        if (0 > (t_sint32) error_i2c)
        {
            I2C_PRINT("I2C_Init::Error in Initializing I2C4");
            return;
        }
    }
#endif
    

    if (INIT_I2C0 == mask)
    {
        error_gic = GIC_DisableItLine(GIC_I2C_0_LINE);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_DisableItLine::Error in Disabling Interrupt for I2C0");
            return;
        }

        error_gic = GIC_ChangeDatum(GIC_I2C_0_LINE, SER_I2C0_InterruptHandler, &old_handler);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_ChangeDatum::Error in Binding Interrupt for I2C0");
            return;
        }

        error_gic = GIC_EnableItLine(GIC_I2C_0_LINE);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_EnableItLine::Error in Enabling Interrupt for I2C0");
            return;
        }
    }
    else if (INIT_I2C1 == mask)
    {
        error_gic = GIC_DisableItLine(GIC_I2C_1_LINE);

        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_DisableItLine::Error in Disabling Interrupt for I2C1");
            return;
        }

        error_gic = GIC_ChangeDatum(GIC_I2C_1_LINE, SER_I2C1_InterruptHandler, &old_handler);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_ChangeDatum::Error in Binding Interrupt for I2C1");
            return;

            /*    	    exit(-1);*/
        }

        error_gic = GIC_EnableItLine(GIC_I2C_1_LINE);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_EnableItLine::Error in Enabling Interrupt for I2C1");
            return;

            /*    	    exit(-1);*/
        }
    }
    else if (INIT_I2C2 == mask)
    {
        error_gic = GIC_DisableItLine(GIC_I2C_2_LINE);

        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_DisableItLine::Error in Disabling Interrupt for I2C2");
            return;
        }

        error_gic = GIC_ChangeDatum(GIC_I2C_2_LINE, SER_I2C2_InterruptHandler, &old_handler);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_ChangeDatum::Error in Binding Interrupt for I2C2");
            return;

            /*    	    exit(-1);*/
        }

        error_gic = GIC_EnableItLine(GIC_I2C_2_LINE);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_EnableItLine::Error in Enabling Interrupt for I2C2");
            return;

            /*    	    exit(-1);*/
        }
    }
    else if (INIT_I2C3 == mask)
    {
        error_gic = GIC_DisableItLine(GIC_I2C_3_LINE);

        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_DisableItLine::Error in Disabling Interrupt for I2C3");
            return;
        }

        error_gic = GIC_ChangeDatum(GIC_I2C_3_LINE, SER_I2C3_InterruptHandler, &old_handler);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_ChangeDatum::Error in Binding Interrupt for I2C3");
            return;

            /*    	    exit(-1);*/
        }

        error_gic = GIC_EnableItLine(GIC_I2C_3_LINE);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_EnableItLine::Error in Enabling Interrupt for I2C3");
            return;

            /*    	    exit(-1);*/
        }
    }
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    else if (INIT_I2C4 == mask)
    {
        error_gic = GIC_DisableItLine(GIC_I2C_4_LINE);

        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_DisableItLine::Error in Disabling Interrupt for I2C4");
            return;
        }

        error_gic = GIC_ChangeDatum(GIC_I2C_4_LINE, SER_I2C4_InterruptHandler, &old_handler);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_ChangeDatum::Error in Binding Interrupt for I2C4");
            return;

            /*    	    exit(-1);*/
        }

        error_gic = GIC_EnableItLine(GIC_I2C_4_LINE);
        if (0 > (t_sint32) error_gic)
        {
            I2C_PRINT("GIC_EnableItLine::Error in Enabling Interrupt for I2C4");
            return;

            /*    	    exit(-1);*/
        }
    }
#endif
    
    I2c_transferOk[0] = (t_bool) FALSE;
    I2c_transferOk[1] = (t_bool) FALSE;
    I2c_transferOk[2] = (t_bool) FALSE;
    I2c_transferOk[3] = (t_bool) FALSE;
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    I2c_transferOk[4] = (t_bool) FALSE;
#endif
    I2c_transferFailed[0] = (t_bool) FALSE;
    I2c_transferFailed[1] = (t_bool) FALSE;    
    I2c_transferFailed[2] = (t_bool) FALSE;
    I2c_transferFailed[3] = (t_bool) FALSE;
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))   
    I2c_transferFailed[4] = (t_bool) FALSE;
#endif
    
}

/****************************************************************************/
/* NAME:    SER_I2C_ConfigureDefault                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: this routine configure i2c and i2c gpio in alternate        */
/*              both i2c are set in master at 100Khz                        */
/* PARAMETERS	:                                                           */
/* 		IN  	: None                                                 		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:t_i2c_error: The I2C error                                 */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_error SER_I2C_ConfigureDefault(void)
{
    t_i2c_error             error_status;
    t_i2c_device_config     i2c_device_config;
    t_i2c_transfer_config   i2c_transfer_config;

#if ((defined ST_8500ED)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V1)||(defined ST_8500V2)||(defined ST_HREFV2))
    t_gpio_error            gpio_error = GPIO_OK;
#endif

    i2c_device_config.controller_i2c_address = 0;
    i2c_transfer_config.i2c_transfer_frequency = 100000;
    i2c_transfer_config.bus_control_mode = I2C_BUS_MASTER_MODE;

    i2c_transfer_config.i2c_transmit_interrupt_threshold = 4;
    i2c_transfer_config.i2c_receive_interrupt_threshold = 4;
    i2c_transfer_config.transmit_burst_length = 0;
    i2c_transfer_config.receive_burst_length = 0;

    i2c_device_config.input_frequency = 48000000;

    i2c_device_config.i2c_digital_filter_control = I2C_DIGITAL_FILTERS_OFF;
    i2c_device_config.i2c_dma_sync_logic_control = I2C_DISABLE;
    i2c_device_config.i2c_start_byte_procedure = I2C_DISABLE;
	i2c_device_config.i2c_high_speed_master_code = 0x01;

    i2c_transfer_config.i2c_loopback_mode = I2C_DISABLE;
    i2c_transfer_config.index_transfer_mode = I2C_TRANSFER_MODE_INTERRUPT;
    i2c_transfer_config.data_transfer_mode = I2C_TRANSFER_MODE_INTERRUPT;
    i2c_transfer_config.i2c_transfer_frequency = 100000;
    i2c_transfer_config.bus_control_mode = I2C_BUS_MASTER_MODE;
    i2c_transfer_config.i2c_slave_general_call_mode = I2C_NO_GENERAL_CALL_HANDLING;

    i2c_device_config.slave_data_setup_time = 14;

    error_status = I2C_SetDeviceConfiguration(I2C0, &i2c_device_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }

    error_status = I2C_SetDeviceConfiguration(I2C1, &i2c_device_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }

    error_status = I2C_SetTransferConfiguration(I2C0, &i2c_transfer_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }

    error_status = I2C_SetTransferConfiguration(I2C1, &i2c_transfer_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }
    
    error_status = I2C_SetDeviceConfiguration(I2C2, &i2c_device_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }

    error_status = I2C_SetDeviceConfiguration(I2C3, &i2c_device_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }

    error_status = I2C_SetTransferConfiguration(I2C2, &i2c_transfer_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }

    error_status = I2C_SetTransferConfiguration(I2C3, &i2c_transfer_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }
    
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    error_status = I2C_SetDeviceConfiguration(I2C4, &i2c_device_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }
    
    error_status = I2C_SetTransferConfiguration(I2C4, &i2c_transfer_config);
    if (I2C_OK != error_status)
    {
        return(error_status);
    }
#endif


#if ((defined ST_8500ED)||(defined ST_HREFED)||(defined ST_HREFV1)||(defined ST_8500V1)||(defined ST_8500V2)||(defined ST_HREFV2))
    gpio_error = GPIO_EnableAltFunction(GPIO_ALT_I2C0);
    if (GPIO_OK != gpio_error)
    {
        return(I2C_SW_FAILED);
    }

    gpio_error = GPIO_EnableAltFunction(GPIO_ALT_I2C1_1);
    if (GPIO_OK != gpio_error)
    {
        return(I2C_SW_FAILED);
    }
    gpio_error = GPIO_EnableAltFunction(GPIO_ALT_I2C2_1);
    if (GPIO_OK != gpio_error)
    {
        return(I2C_SW_FAILED);
    }

    gpio_error = GPIO_EnableAltFunction(GPIO_ALT_I2C3_1);
    if (GPIO_OK != gpio_error)
    {
        return(I2C_SW_FAILED);
    }
    
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined ST_8500V2)||(defined ST_HREFV2))   
    gpio_error = GPIO_EnableAltFunction(GPIO_ALT_I2C4);
    if (GPIO_OK != gpio_error)
    {
        return(I2C_SW_FAILED);
    }
#endif

#endif
    return(error_status);
}

/****************************************************************************/
/* NAME:    SER_I2C_Close	                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This function is to stop I2C services		 				*/
/* PARAMETERS	:                                                           */
/* 		IN  	: None                                                 		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:None                                                       */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_I2C_Close(void)
{
    /*TBD*/
}

/****************************************************************************/
/* NAME:    SER_I2C_Wait_End                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This function is to stop I2C services		 				*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: None                                                		*/
/*     InOut    : None                                                      */
/* 		OUT 	: None       												*/
/*                                                                          */
/* RETURN		:t_i2c_event:Inform the  user about the last occurred event */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_i2c_event SER_I2C_WaitEnd(IN t_i2c_device_id id)
{
    t_uint8 idx = (t_uint8) id;
   
    if(I2C0 == idx)
    {
    	while (((t_bool) TRUE != I2c_transferOk[idx]) && ((t_bool) TRUE != I2c_transferFailed[idx]) && !((*((t_uint32 *)(0x80004014)) & 0x7C) == 0xC))
    	;
    	
    	if(((*((t_uint32 *)(0x80004014)) & 0x7C) == 0xC))
    	{
    		return(I2C_ABORT_NACK_ON_ADDRESS_EVENT);
    	}	   	
    }
    else if(I2C1 == idx)
    {
    	while (((t_bool) TRUE != I2c_transferOk[idx]) && ((t_bool) TRUE != I2c_transferFailed[idx]) && !((*((t_uint32 *)(0x80122014)) & 0x7C) == 0xC))
    	;
    	
    	if(((*((t_uint32 *)(0x80122014)) & 0x7C) == 0xC))
    	{
    		return(I2C_ABORT_NACK_ON_ADDRESS_EVENT);
    	}	   	
    }
    else if(I2C2 == idx)
    {
    	while (((t_bool) TRUE != I2c_transferOk[idx]) && ((t_bool) TRUE != I2c_transferFailed[idx]) && !((*((t_uint32 *)(0x80128014)) & 0x7C) == 0xC))
    	;
    	
    	if(((*((t_uint32 *)(0x80128014)) & 0x7C) == 0xC))
    	{
    		return(I2C_ABORT_NACK_ON_ADDRESS_EVENT);
    	}	   	
    }
    else if(I2C3 == idx)
    {
    	while (((t_bool) TRUE != I2c_transferOk[idx]) && ((t_bool) TRUE != I2c_transferFailed[idx]) && !((*((t_uint32 *)(0x80110014)) & 0x7C) == 0xC))
    	;
    	
    	if(((*((t_uint32 *)(0x80110014)) & 0x7C) == 0xC))
    	{
    		return(I2C_ABORT_NACK_ON_ADDRESS_EVENT);
    	}	   	
    }
#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
    else if(I2C4 == idx)
    {
    	while (((t_bool) TRUE != I2c_transferOk[idx]) && ((t_bool) TRUE != I2c_transferFailed[idx]) && !((*((t_uint32 *)(0x8012A014)) & 0x7C) == 0xC))
    	;
    	
    	if(((*((t_uint32 *)(0x8012A014)) & 0x7C) == 0xC))
    	{
    		return(I2C_ABORT_NACK_ON_ADDRESS_EVENT);
    	}	   	
    }
#endif

    if ((t_bool) TRUE == I2c_transferFailed[idx])
    {
        I2c_transferFailed[idx] = (t_bool) FALSE;
        I2c_transferOk[idx] = (t_bool) FALSE;
        return(I2c_last_transfer_status[id].i2c_event.type);
    }
    else
    {
        I2c_transferFailed[idx] = (t_bool) FALSE;
        I2c_transferOk[idx] = (t_bool) FALSE;
        return(I2C_TRANSFER_OK_EVENT);
    }
   
}

/****************************************************************************/
/* NAME:    SER_I2C_RegisterCallback                                        */
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
PUBLIC int SER_I2C_RegisterCallback(IN t_i2c_device_id id, IN t_callback_fct callback_fct, IN void *param)
{
    t_uint8 idx = (t_uint8) id;
    callback_i2c[idx].fct = callback_fct;
    callback_i2c[idx].param = param;
    return(HCL_OK);
}

/* Blocking Data Services */
/****************************************************************************/
/* NAME:    SER_I2C_WriteSingleDataBlocking                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Write a single data byte to a I2C slave device              */
/*                                                                          */
/* PARAMETERS: void                                                         */
/* IN : t_i2c_id id : 	the I2C controller to be used.                      */
/*	    t_uint16 slave_address : the address of the slave device.           */
/*      t_uint16  index_value : register index value                        */
/*      t_i2c_index_format  index_format: defines the index format          */
/*	    t_uint8 data : the data byte to be written to the slave device.     */
/* OUT : None                                                               */
/* RETURN: t_i2c_error                                                      */
/* Note: This API is applicable only,when data transfer mode set to         */
/*       interrput mode                                                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_i2c_error SER_I2C_WriteSingleDataBlocking
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              data
)
{
    t_i2c_error i2c_error;
    t_i2c_event i2c_event;

    I2c_transferFailed[id] = (t_bool) FALSE;
    I2c_transferOk[id] = (t_bool) FALSE;

    i2c_error = (t_i2c_error) I2C_WriteSingleData(id, slave_address, index_format, index_value, data);
    if (I2C_OK != i2c_error)
    {
        return(i2c_error);
    }

    i2c_event = SER_I2C_WaitEnd(id);

    if (I2C_TRANSFER_OK_EVENT == i2c_event)
    {
        return(I2C_OK);
    }
    else
    {
        return(I2C_SW_FAILED);
    }
}

/****************************************************************************/
/* NAME:    SER_I2C_WriteMultipleDataBlocking                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Write a multiple data byte to a I2C slave device            */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN : t_i2c_id id : 	the I2C controller to be used.                      */
/*	    t_uint16 slave_address : the address of the slave device.           */
/*      t_uint16  index_value : register index value                        */
/*      t_i2c_index_format  index_format: defines the index format          */
/*	    *t_uint8 data : the data byte to be written to the slave device.    */
/*      t_unit32  count : no of data bytes to be transfered                 */
/* OUT : None                                                               */
/* RETURN: t_i2c_error                                                      */
/* Note: This API is applicable only,when data transfer mode set to         */
/*       interrput mode                                                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_i2c_error SER_I2C_WriteMultipleDataBlocking
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              *p_data,
    IN t_uint32             count
)
{
    t_i2c_error i2c_error;
    t_i2c_event i2c_event;

    I2c_transferFailed[id] = (t_bool) FALSE;
    I2c_transferOk[id] = (t_bool) FALSE;

    i2c_error = (t_i2c_error) I2C_WriteMultipleData(id, slave_address, index_format, index_value, p_data, count);
    if (I2C_OK != i2c_error)
    {
        return(i2c_error);
    }

    i2c_event = SER_I2C_WaitEnd(id);

    if (I2C_TRANSFER_OK_EVENT == i2c_event)
    {
        return(I2C_OK);
    }
    else
    {
        return(I2C_SW_FAILED);
    }
}

/****************************************************************************/
/* NAME:    SER_I2C_ReadSingleDataBlocking                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Receive  a single data byte from I2C slave device           */
/*                                                                          */
/* PARAMETERS: void                                                         */
/* IN : t_i2c_id id : 	the I2C controller to be used.                      */
/*	    t_uint16 slave_address : the address of the slave device.           */
/*      t_uint16  index_value : register index value                        */
/*      t_i2c_index_format  index_format: defines the index format          */
/*	    *t_uint8 data : the data byte to be read from the slave device.     */
/* OUT : None                                                               */
/* RETURN: t_i2c_error                                                      */
/* Note: This API is applicable only,when data transfer mode set to         */
/*       interrput mode                                                     */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_i2c_error SER_I2C_ReadSingleDataBlocking
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              *p_data
)
{
    t_i2c_error i2c_error;
    t_i2c_event i2c_event;

    I2c_transferFailed[id] = (t_bool) FALSE;
    I2c_transferOk[id] = (t_bool) FALSE;

    i2c_error = (t_i2c_error) I2C_ReadSingleData(id, slave_address, index_format, index_value, p_data);
    if (I2C_OK != i2c_error)
    {
        return(i2c_error);
    }

    i2c_event = SER_I2C_WaitEnd(id);

    if (I2C_TRANSFER_OK_EVENT == i2c_event)
    {
        return(I2C_OK);
    }
    else
    {
        return(I2C_SW_FAILED);
    }
}

/****************************************************************************/
/* NAME:    SER_I2C_ReadMultipleDataBlocking                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Receive multiple  data bytes from I2C slave device          */
/*                                                                          */
/* PARAMETERS: void                                                         */
/* IN : t_i2c_id id : 	the I2C controller to be used.                      */
/*	    t_uint16 slave_address : the address of the slave device.           */
/*      t_uint16  index_value : register index value                        */
/*      t_i2c_index_format  index_format: defines the index format          */
/*	    *t_uint8 data : the data byte to be read from the slave device.     */
/*      t_unit32  count : no of data bytes to be read                       */
/* OUT : None                                                               */
/* RETURN: t_i2c_error                                                      */
/* Note: This API is applicable only,when data transfer mode set to         */
/*       interrput mode                                                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_i2c_error SER_I2C_ReadMultipleDataBlocking
(
    IN t_i2c_device_id      id,
    IN t_uint16             slave_address,
    IN t_i2c_index_format   index_format,
    IN t_uint16             index_value,
    IN t_uint8              *p_data,
    IN t_uint32             count
)
{
    t_i2c_error i2c_error;
    t_i2c_event i2c_event;

    I2c_transferFailed[id] = (t_bool) FALSE;
    I2c_transferOk[id] = (t_bool) FALSE;

    i2c_error = (t_i2c_error) I2C_ReadMultipleData(id, slave_address, index_format, index_value, p_data, count);
    if (i2c_error != I2C_OK)
    {
        return(i2c_error);
    }

    i2c_event = SER_I2C_WaitEnd(id);

    if (I2C_TRANSFER_OK_EVENT == i2c_event)
    {
        return(I2C_OK);
    }
    else
    {
        return(I2C_SW_FAILED);
    }
}
