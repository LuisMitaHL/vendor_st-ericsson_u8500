/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This File provides service routines for UART
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "debug.h"
#include "memory_mapping.h"
#include "uart_irq.h"
#include "uart.h"
#include "services.h"
#include "uart_services.h"
#include "gic.h"
#if ((!defined __PEPS_8500) && (!defined __PEPS_8500_V1) && (!defined __PEPS_8500_V2))  
#include "gpio.h"
#endif


#ifdef __UART_ENHANCED

/*--------------------------------------------------------------------------*
 * Variables                                                                *
 *--------------------------------------------------------------------------*/
PRIVATE t_callback  g_uart_callback = { NULL, NULL };

/*--------------------------------------------------------------------------*/
/* Public functions                                                         */
/*--------------------------------------------------------------------------*/
#ifdef UART_USE_FILTER_PROCESS_IRQ

/****************************************************************************/
/* NAME         :   SER_UART0_InterruptHandler                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Interrupt handler implementation                        */
/*  PARAMETERS  :                                                           */
/*          IN  :   irq_src :   UART Interrupts                             */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC void SER_UART0_InterruptHandler(IN t_uint32 irq_src)
{
    t_uart_error        uart_error;
    t_uart_irq_status   uart_status;
    t_uart_event        uart_event;
    t_bool              done;
    t_uart_param        uart_param;

    UART_GetIRQSrcStatus(UART0_DEVICE_ALL_IT, &uart_status);

    if (GIC_OK != GIC_DisableItLine(GIC_UART_0_LINE))
    {
        PRINT("\nError after GIC_DisableItLine in UART_DEVICE_ID_0 Handler");
        return;
    }

    if (GIC_OK != GIC_AcknowledgeItLine(GIC_UART_0_LINE))
    {
        PRINT("\nError after GIC_AcknowledgeItLine in UART_DEVICE_ID_0 Handler");
        return;
    }

    done = FALSE;
    do
    {
        uart_error = UART_FilterProcessIRQSrc(&uart_status, &uart_event, UART_NO_FILTER_MODE);
        switch (uart_error)
        {
            case UART_NO_PENDING_EVENT_ERROR:
            case UART_NO_MORE_PENDING_EVENT:
            case UART_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case UART_REMAINING_PENDING_EVENTS:
                if (0 != g_uart_callback.fct)
                {
                    uart_param.uart_event = uart_event;
                    g_uart_callback.fct(g_uart_callback.param, (void *) &uart_param);
                }

                UART_AcknowledgeEvent(&uart_event);
                break;

            case UART_INTERNAL_EVENT:
                break;
        }
    } while (!done);

    if (GIC_OK != GIC_EnableItLine(GIC_UART_0_LINE))
    {
        PRINT("\nError after GIC_EnableItLine in UART_DEVICE_ID_0 Handler");
        return;
    }
}

/****************************************************************************/
/* NAME         :   SER_UART1_InterruptHandler                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Interrupt handler implementation                        */
/*  PARAMETERS  :                                                           */
/*          IN  :   irq_src :   UART Interrupts                             */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC void SER_UART1_InterruptHandler(IN t_uint32 irq_src)
{
    t_uart_error        uart_error;
    t_uart_irq_status   uart_status;
    t_uart_event        uart_event;
    t_bool              done;
    t_uart_param        uart_param;

    UART_GetIRQSrcStatus(UART1_DEVICE_ALL_IT, &uart_status);

    if (GIC_OK != GIC_DisableItLine(GIC_UART_1_LINE))
    {
        PRINT("\nError after GIC_DisableItLine in UART_DEVICE_ID_1 Handler");
        return;
    }

    if (GIC_OK != GIC_AcknowledgeItLine(GIC_UART_1_LINE))
    {
        PRINT("\nError after GIC_AcknowledgeItLine in UART_DEVICE_ID_1 Handler");
        return;
    }

    done = FALSE;
    do
    {
        uart_error = UART_FilterProcessIRQSrc(&uart_status, &uart_event, UART_NO_FILTER_MODE);
        switch (uart_error)
        {
            case UART_NO_PENDING_EVENT_ERROR:
            case UART_NO_MORE_PENDING_EVENT:
            case UART_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case UART_REMAINING_PENDING_EVENTS:
                if (0 != g_uart_callback.fct)
                {
                    uart_param.uart_event = uart_event;
                    g_uart_callback.fct(g_uart_callback.param, (void *) &uart_param);
                }

                UART_AcknowledgeEvent(&uart_event);
                break;

            case UART_INTERNAL_EVENT:
                break;
        }
    } while (!done);

    if (GIC_OK != GIC_EnableItLine(GIC_UART_1_LINE))
    {
        PRINT("\nError after GIC_EnableItLine in UART_DEVICE_ID_1 Handler");
        return;
    }
}


/****************************************************************************/
/* NAME         :   SER_UART2_InterruptHandler                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Interrupt handler implementation                        */
/*  PARAMETERS  :                                                           */
/*          IN  :   irq_src :   UART Interrupts                             */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC void SER_UART2_InterruptHandler(IN t_uint32 irq_src)
{
    t_uart_error        uart_error;
    t_uart_irq_status   uart_status;
    t_uart_event        uart_event;
    t_bool              done;
    t_uart_param        uart_param;

    UART_GetIRQSrcStatus(UART2_DEVICE_ALL_IT, &uart_status);

    if (GIC_OK != GIC_DisableItLine(GIC_UART_2_LINE))
    {
        PRINT("\nError after GIC_DisableItLine in UART_DEVICE_ID_2 Handler");
        return;
    }

    if (GIC_OK != GIC_AcknowledgeItLine(GIC_UART_2_LINE))
    {
        PRINT("\nError after GIC_AcknowledgeItLine in UART_DEVICE_ID_2 Handler");
        return;
    }

    done = FALSE;
    do
    {
        uart_error = UART_FilterProcessIRQSrc(&uart_status, &uart_event, UART_NO_FILTER_MODE);
        switch (uart_error)
        {
            case UART_NO_PENDING_EVENT_ERROR:
            case UART_NO_MORE_PENDING_EVENT:
            case UART_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case UART_REMAINING_PENDING_EVENTS:
                if (0 != g_uart_callback.fct)
                {
                    uart_param.uart_event = uart_event;
                    g_uart_callback.fct(g_uart_callback.param, (void *) &uart_param);
                }

                UART_AcknowledgeEvent(&uart_event);
                break;

            case UART_INTERNAL_EVENT:
                break;
        }
    } while (!done);

    if (GIC_OK != GIC_EnableItLine(GIC_UART_2_LINE))
    {
        PRINT("\nError after GIC_EnableItLine in UART_DEVICE_ID_1 Handler");
        return;
    }
}
#endif /*  UART_USE_FILTER_PROCESS_IRQ  */

#ifndef UART_USE_FILTER_PROCESS_IRQ

/****************************************************************************/
/* NAME         :   SER_UART0_InterruptHandler                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Interrupt handler implementation                        */
/*  PARAMETERS  :                                                           */
/*          IN  :   irq_src :   UART Interrupts                             */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC void SER_UART0_InterruptHandler(IN t_uint32 irq_src)
{
    t_uart_irq_status   uart_status;
    t_uart_param        uart_param;

    if (GIC_OK != GIC_DisableItLine(GIC_UART_0_LINE))
    {
        PRINT("\nError after GIC_DisableItLine in UART_DEVICE_ID_0 Handler");
        return;
    }
    
    UART_GetIRQSrcStatus(UART0_DEVICE_ALL_IT, &uart_status);

    /* Service The IRQ */
    UART_ProcessIRQSrc(&uart_status);

    if (0 != g_uart_callback.fct)
    {
        uart_param.uart_event = uart_status.initial_irq;
        g_uart_callback.fct(g_uart_callback.param, (void *) &uart_param);
    }

    if (GIC_OK != GIC_EnableItLine(GIC_UART_0_LINE))
    {
        PRINT("\nError after GIC_EnableItLine in UART_DEVICE_ID_0 Handler");
        return;
    }
}

/****************************************************************************/
/* NAME         :   SER_UART1_InterruptHandler                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Interrupt handler implementation                        */
/*  PARAMETERS  :                                                           */
/*          IN  :   irq_src :   UART Interrupts                             */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC void SER_UART1_InterruptHandler(IN t_uint32 irq_src)
{
    t_uart_irq_status   uart_status;
    t_uart_param        uart_param;

   if (GIC_OK != GIC_DisableItLine(GIC_UART_1_LINE))
    {
        PRINT("\nError after GIC_DisableItLine in UART_DEVICE_ID_1 Handler");
        return;
    }
   
    UART_GetIRQSrcStatus(UART1_DEVICE_ALL_IT, &uart_status);

    /* Service The IRQ */
    UART_ProcessIRQSrc(&uart_status);

    if (0 != g_uart_callback.fct)
    {
        uart_param.uart_event = uart_status.initial_irq;
        g_uart_callback.fct(g_uart_callback.param, (void *) &uart_param);
    }

    if (GIC_OK != GIC_EnableItLine(GIC_UART_1_LINE))
    {
        PRINT("\nError after GIC_EnableItLine in UART_DEVICE_ID_1 Handler");
        return;
    }
}


/****************************************************************************/
/* NAME         :   SER_UART2_InterruptHandler                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   Interrupt handler implementation                        */
/*  PARAMETERS  :                                                           */
/*          IN  :   irq_src :   UART Interrupts                             */
/*                                                                          */
/*      RETURN  :   void                                                    */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC void SER_UART2_InterruptHandler(IN t_uint32 irq_src)
{
    t_uart_irq_status   uart_status;
    t_uart_param        uart_param;
  
    if (GIC_OK != GIC_DisableItLine(GIC_UART_2_LINE))
    {
        PRINT("\nError after GIC_DisableItLine in UART_DEVICE_ID_2 Handler");
        return;
    }
    
    UART_GetIRQSrcStatus(UART2_DEVICE_ALL_IT, &uart_status);

    /* Service The IRQ */
    UART_ProcessIRQSrc(&uart_status);

    if (0 != g_uart_callback.fct)
    {
        uart_param.uart_event = uart_status.initial_irq;
        g_uart_callback.fct(g_uart_callback.param, (void *) &uart_param);
    }

    if (GIC_OK != GIC_EnableItLine(GIC_UART_2_LINE))
    {
        PRINT("\nError after GIC_EnableItLine in UART_DEVICE_ID_2 Handler");
        return;
    }
}
#endif /*  UART_USE_FILTER_PROCESS_IRQ  */

/****************************************************************************/
/*       NAME :   SER_UART_RegisterCallback                                 */
/*--------------------------------------------------------------------------*/
/*DESCRIPTION :   This routine allows to register users call back functions.*/
/*                The function will be called when following event occurs   */
/*                from UART_FilterProcessIRQSrc:                            */
/*                1. UART_REMAINING_PENDING_EVENTS                          */
/*                                                                          */
/*                When call back function will be invoked due to any of the */
/*                events mention above, user will receive two parameters.   */
/*                1. param : same as passed during registartion             */
/*                2. &t_ser_map_param : the stucture contains information   */
/*                                      about events, errors etc.           */
/*                                                                          */
/*                Call this function register your call back function.      */
/*                                                                          */
/* PARAMETERS :                                                             */
/*         IN :   uart_device_id  :   UART Device ID                        */
/*            :   callback_fct    :   pointer to function of type           */
/*                                    t_callback_fct                        */
/*      INOUT :   param           :   value which will be passed back to the*/
/*                                    function pointed by "callback_fct" as */
/*                                    first parameter                       */
/*                                                                          */
/*     RETURN :   SERVICE_OK      :   If no error is detected               */
/*            :   SERVICE_FAILED  :   Returned if callback function is      */
/*                                    NULL or device ID do not match        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY :   Non Re-entrant                                            */
/****************************************************************************/
PUBLIC t_ser_error SER_UART_RegisterCallback
(
    IN t_uart_device_id uart_device_id,
    IN t_callback_fct   callback_fct,
    INOUT void          *param
)
{
    if
    (
        (UART_DEVICE_ID_0 == uart_device_id)
    ||  (UART_DEVICE_ID_1 == uart_device_id)
    ||  (UART_DEVICE_ID_2 == uart_device_id)
    )
    {
        if (NULL == callback_fct)
        {
            PRINT("NULL CALLBACK FUNCTION\n");
            PRINT("Error UART_SER: From SER_UART_RegisterCallback\n");
            return(SERVICE_FAILED);
        }

        g_uart_callback.fct = callback_fct;
        g_uart_callback.param = param;
        return(SERVICE_OK);
    }
    else
    {
        PRINT("Error UART_SER: From SER_UART_RegisterCallback\n");
        return(SERVICE_FAILED);
    }
}

/****************************************************************************/
/*        NAME   :  SER_UART_TransmitWaitEnd                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION   :  This routine allows to wait for transmission to complete*/
/*                  Valid for Interrupt mode. Polling mode is already       */
/*                  blocking and for DMA mode call SER_DMA_WaitEnd          */
/*  PARAMETERS   :  uart_device_id  :   UART number                         */
/*                                                                          */
/*      RETURN   :  SERVICE_OK      :   If no error is detected             */
/*               :  SERVICE_FAILED  :   If Transmission timed out!!         */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY   :  Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_ser_error SER_UART_TransmitWaitEnd(IN t_uart_device_id uart_device_id)
{
    t_uart_it_communication_status  comm_status_uart;
    t_uart_error                    uart_error;
    t_uint32                        wait_count = SER_UART_WAIT_COUNT;
    do
    {
        uart_error = UART_GetComStatus(uart_device_id, &comm_status_uart);
        if (NULL == wait_count--)
        {
            PRINT("Error : Transmission timed out!!\n");
            PRINT("Error UART_SER: From SER_UART_TransmitWaitEnd\n");
            return(SERVICE_FAILED);
        }
    /*coverity[uninit_use]*/
    } while (comm_status_uart.tx_comm_status != UART_TRANSMIT_ENDED);
    if (UART_OK == uart_error)
    {
        PRINT("Transmission sucessfull\n");
        return(SERVICE_OK);
    }   /* end if */
    else
    {
        PRINT("Error in Transmission !!\n");
        return(SERVICE_FAILED);
    }   /* end else */
}

/****************************************************************************/
/*        NAME  :    SER_UART_ReceiveWaitEnd                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine allows to wait for reception to complete   */
/*                  Valid for Interrupt mode. Polling mode is already       */
/*                  blocking and for DMA mode call SER_DMA_WaitEnd          */
/*  PARAMETERS  :   uart_device_id  :   UART number                         */
/*                                                                          */
/*      RETURN  :   SERVICE_OK      :   If no error is detected             */
/*              :   SERVICE_FAILED  :   If Reception timed out!!            */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   Non Re-entrant                                          */
/****************************************************************************/
PUBLIC t_ser_error SER_UART_ReceiveWaitEnd(IN t_uart_device_id uart_device_id)
{
    t_uart_it_communication_status  comm_status_uart;
    t_uart_error                    uart_error;
    t_uint32                        wait_count = SER_UART_WAIT_COUNT;
    do
    {
        uart_error = UART_GetComStatus(uart_device_id, &comm_status_uart);
        if (NULL == wait_count--)
        {
            PRINT("Error : Reception timed out!!\n");
            PRINT("Error UART_SER: From SER_UART_ReceiveWaitEnd\n");
            return(SERVICE_FAILED);
        }
    /*coverity[uninit_use]*/
    } while (comm_status_uart.rx_comm_status != UART_RECEIVE_ENDED);
    if (UART_OK == uart_error)
    {
        PRINT("Transmission sucessfull\n");
        return(SERVICE_OK);
    }   /* end if */
    else
    {
        PRINT("Error in Transmission !!\n");
        return(SERVICE_FAILED);
    }   /* end else */
}
#endif /*  UART_ENHANCED  */

/****************************************************************************/
/*        NAME    :    SER_UART_Init                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION    :    This routine initialize the UART                     */
/*                     1. Program CPLD registers                            */
/*                     2. Install default Interrupt Handler                 */
/*                     3. Set UART base address.                            */
/*                     4. Enables GPIO altrenate function                   */
/*  PARAMETERS    :                                                         */
/*          IN    :    uart_device_id  : UART number to be initialized      */
/*                                                                          */
/*      RETURN    :    void                                                 */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY    :    Non Re-entrant                                       */
/*    COMMENTS    :    Should be done after enabling GIC services           */
/****************************************************************************/
PUBLIC void SER_UART_Init(IN t_uint8 uart_device_id)
{
   
#ifdef __UART_ENHANCED
    t_gic_func_ptr  old_datum;
    t_gic_error     gic_error;
#endif /* #ifdef __UART_ENHANCED */

#if ((!defined __PEPS_8500) && (!defined __PEPS_8500_V1) && (!defined __PEPS_8500_V2))  
    t_gpio_error    gpio_status;
	(*(volatile t_uint32 *)(SER_PRCM_UARTCLK_MGT_REG)) |= SER_UARTCLK_ENA;     /* UARTCLK  38.4 MHz (from SYSCLK) (clock enabled) */
#endif 

#ifdef __UART_ENHANCED
    /* This is to be done after GIC Initialization */
    g_uart_callback.fct = 0;
    g_uart_callback.param = 0;

    if (INIT_UART0 == (INIT_UART0 & uart_device_id))
    {
        /* Change The associated Interrupt Handler for UART 0 */
        gic_error = GIC_ChangeDatum(GIC_UART_0_LINE, SER_UART0_InterruptHandler, &old_datum);
        if (GIC_OK != gic_error)
        {
            PRINT("\nGIC error after GIC_ChangeDatum  :: %d", gic_error);
            return;
        }

        gic_error = GIC_EnableItLine(GIC_UART_0_LINE);
        if (GIC_OK != gic_error)
        {
            PRINT("\nGIC error after GIC_EnableItLine  :: %d", gic_error);
            return;
        }
    }   /* end if INIT_UART0 */

    if (INIT_UART1 == (INIT_UART1 & uart_device_id))
    {
        /* Change The associated Interrupt Handler for UART 1 */
        gic_error = GIC_ChangeDatum(GIC_UART_1_LINE, SER_UART1_InterruptHandler, &old_datum);
        if (GIC_OK != gic_error)
        {
            PRINT("\nGIC error after GIC_ChangeDatum  :: %d", gic_error);
            return;
        }

        gic_error = GIC_EnableItLine(GIC_UART_1_LINE);
        if (GIC_OK != gic_error)
        {
            PRINT("\nGIC error after GIC_EnableItLine  :: %d", gic_error);
            return;
        }
    }   /* end if INIT_UART1 */

    if (INIT_UART2 == (INIT_UART2 & uart_device_id))
    {
        /* Change The associated Interrupt Handler for UART 2 */
        gic_error = GIC_ChangeDatum(GIC_UART_2_LINE, SER_UART2_InterruptHandler, &old_datum);
        if (GIC_OK != gic_error)
        {
            PRINT("\nGIC error after GIC_ChangeDatum  :: %d", gic_error);
            return;
        }

        gic_error = GIC_EnableItLine(GIC_UART_2_LINE);
        if (GIC_OK != gic_error)
        {
            PRINT("\nGIC error after GIC_EnableItLine  :: %d", gic_error);
            return;
        }
    }   /* end if INIT_UART2 */
#endif /*  __UART_ENHANCED  */

    if (INIT_UART0 == (INIT_UART0 & uart_device_id))
    {
	/* Set PRCC Clocks */
        (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR )) |= SER_UART_0_AMBA_KERNEL_CLK;
        (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR + UART_PRCC_KCKEN_REG_OFFSET)) |= SER_UART_0_AMBA_KERNEL_CLK;
    
    	UART_SetBaseAddress(UART_DEVICE_ID_0, UART_0_CTRL_REG_BASE_ADDR);

        if (UART_UNSUPPORTED_HW == UART_Init(UART_DEVICE_ID_0, UART_0_CTRL_REG_BASE_ADDR))
        {
            PRINT("UART_DEVICE_ID_0 Memory map failed.");
            return;
        }
    }   /* end if INIT_UART0 */

    if (INIT_UART1 == (INIT_UART1 & uart_device_id))
    {
	/* Set PRCC Clocks */
        (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR)) |= SER_UART_1_AMBA_KERNEL_CLK;
        (*(volatile t_uint32 *)(PRCC_1_CTRL_REG_BASE_ADDR + UART_PRCC_KCKEN_REG_OFFSET)) |= SER_UART_1_AMBA_KERNEL_CLK;

	UART_SetBaseAddress(UART_DEVICE_ID_1, UART_1_CTRL_REG_BASE_ADDR);

        if (UART_UNSUPPORTED_HW == UART_Init(UART_DEVICE_ID_1, UART_1_CTRL_REG_BASE_ADDR))
        {
            PRINT("UART_DEVICE_ID_1 Memory map failed.");
            return;
        }
    }   /* end if INIT_UART1 */

    if (INIT_UART2 == (INIT_UART2 & uart_device_id))
    {
	/* Set PRCC Clocks */
        (*(volatile t_uint32 *)(PRCC_3_CTRL_REG_BASE_ADDR)) |= SER_UART_2_AMBA_KERNEL_CLK;
        (*(volatile t_uint32 *)(PRCC_3_CTRL_REG_BASE_ADDR + UART_PRCC_KCKEN_REG_OFFSET)) |= SER_UART_2_AMBA_KERNEL_CLK;

	UART_SetBaseAddress(UART_DEVICE_ID_2, UART_2_CTRL_REG_BASE_ADDR);
        if (UART_UNSUPPORTED_HW == UART_Init(UART_DEVICE_ID_2, UART_2_CTRL_REG_BASE_ADDR))
        {
            PRINT("UART_DEVICE_ID_2 Memory map failed.");
            return;
        }
    }   /* end if INIT_UART2 */


#if ((!defined __PEPS_8500) && (!defined __PEPS_8500_V1) && (!defined __PEPS_8500_V2))  
    if (INIT_UART0 == (INIT_UART0 & uart_device_id))
    {
        /* Enable UART pins at GPIO level */
        gpio_status = GPIO_EnableAltFunction(GPIO_ALT_UART_0);
        if (GPIO_OK != gpio_status)
        {
            PRINT("Error: GPIO could not enable UART_DEVICE_ID_0!\n");
            PRINT("\nGPIO status from GPIO_EnableAltFunction :: %d", gpio_status);
            return;
        }
    }   /* end if INIT_UART0 */

    if (INIT_UART1 == (INIT_UART1 & uart_device_id))
    {
        gpio_status = GPIO_EnableAltFunction(GPIO_ALT_UART_1);
        if (GPIO_OK != gpio_status)
        {
            PRINT("Error: GPIO could not enable UART_DEVICE_ID_1!\n");
            PRINT("\nGPIO status from GPIO_EnableAltFunction  :: %d", gpio_status);
            return;
        }
    }   /* end if INIT_UART1 */

    if (INIT_UART2 == (INIT_UART2 & uart_device_id))
    {
        gpio_status = GPIO_EnableAltFunction(GPIO_ALT_UART_2);
        if (GPIO_OK != gpio_status)
        {
            PRINT("Error: GPIO could not enable UART_DEVICE_ID_2!\n");
            PRINT("\nGPIO status from GPIO_EnableAltFunction  :: %d", gpio_status);
            return;
        }
    }   /* end if INIT_UART2 */
#endif 

   
}

/****************************************************************************/
/* NAME         :   SER_UART_Close                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION  :   This routine disables all UARTs                         */
/*  PARAMETERS  :   void                                                    */
/*                                                                          */
/*      RETURN  :   NONE                                                    */
/*--------------------------------------------------------------------------*/
/*  REENTRANCY  :   NA                                                      */
/****************************************************************************/
PUBLIC void SER_UART_Close(void)
{
    if (UART_OK != UART_Disable(UART_DEVICE_ID_0))
    {
        PRINT("Error: Could not Disable UART0!\n");
    }

    if (UART_OK != UART_Disable(UART_DEVICE_ID_1))
    {
        PRINT("Error: Could not Disable UART1!\n");
    }

    if (UART_OK != UART_Disable(UART_DEVICE_ID_2))
    {
        PRINT("Error: Could not Disable UART2!\n");
    }
}

/* End of uart_services.c */


