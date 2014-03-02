/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief This module provides some support routines for the test environnement
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/

#include "hcl_defs.h" 
#include "memory_mapping.h"
#include "usb_services.h"
#include "gic.h"
#include "usb.h"
#include "usb_irq.h"
#include "gpio.h"
#ifndef ST_8500ED
#include "ab8500_core_services.h"
#endif

/*--------------------------------------------------------------------------*
 * Global Variables                                                         *
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * Constants and new types                                                  *
 *--------------------------------------------------------------------------*/
PRIVATE void USB_Reset_Seq(void);
PRIVATE t_callback    g_usb_callback = {0,0};

/****************************************************************************/
/* NAME:    SER_USB_IntHandler()                                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the USB Interrupt Handler                 */
/*                                                                          */
/* PARAMETERS: int irq                                                      */
/*                                                                          */
/* RETURN: int                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/

void SER_USB_IntHandler(unsigned int irq)
{
    t_usb_error     error;
    t_usb_irq_status L_status;
    t_usb_event L_event;
    t_bool done=FALSE;
    t_usb_irq_src irq_src = { USB_IRQ_SRC_SYS_ALL, USB_IRQ_SRC_EP_ALL};
    t_usb_filter_mode filter_mode = { USB_NO_FILTER_MODE, USB_NO_FILTER_MODE};
    t_gic_error     error_gic;
     
    /*Get the interrupt status. */
    USB_GetIRQSrcStatus(irq_src, &L_status);

    error_gic = GIC_DisableItLine(GIC_USB_OTG_LINE);
    
    do 
    {
        error = USB_FilterProcessIRQSrc(&L_status, &L_event, filter_mode);
        switch(error)
        {
            case USB_NO_PENDING_EVENT_ERROR:
            case USB_NO_MORE_PENDING_EVENT:
            case USB_NO_MORE_FILTER_PENDING_EVENT:
                done = TRUE;
                break;

            case USB_REMAINING_PENDING_EVENTS:
                if(g_usb_callback.fct != 0)  /*  Just to optimize, the callback does not handle the endpoint interrupts.    */
                {
                    g_usb_callback.fct(g_usb_callback.param, (void *)&L_event);
                }
                USB_AcknowledgeEvent(L_event);
                break;

            case USB_INTERNAL_EVENT:
                break;
        }

    }while(TRUE != done);

    error_gic = GIC_EnableItLine(GIC_USB_OTG_LINE);
    
    /*coverity[self_assign]*/
    error_gic = error_gic;
}

/****************************************************************************/
/* NAME:    SER_USB_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the uart                            */
/* the void as parameter, and int as return value are mandatory.            */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/

PUBLIC void SER_USB_Init( t_uint8 usb_ser_mask ) 
{
    t_gic_func_ptr p_old_handler;
    t_gic_error     error_gic;
#ifndef ST_8500ED  
    t_uint8 data_out=0;
#endif
    
    /* Clock Enable for Peripheral5-USB */
    (*(volatile t_uint32 *)(PRCC_5_CTRL_REG_BASE_ADDR)) |= SER_USB_PRCC_AMBACLK_ENA;
        
    
    if(USB_Init( USB_REG_BASE_ADDR) == USB_UNSUPPORTED_HW)
    {
        PRINT("Unsupported USB Host Hardware.");
        return;
    }

    
    USB_SetBaseAddress(USB_REG_BASE_ADDR);   
    
    error_gic = GIC_DisableItLine(GIC_USB_OTG_LINE);
    if (GIC_OK != error_gic)
    {
        PRINT("GIC_DisableItLine::Error in Disabling Interrupt for USB");
        return;
    }
    
    error_gic = GIC_ChangeDatum(GIC_USB_OTG_LINE, (t_gic_func_ptr) SER_USB_IntHandler, &p_old_handler);
    if (GIC_OK != error_gic)
    {
        PRINT("GIC_ChangeDatum::Error in Binding Interrupt for USB");
        return;
    }
        
    error_gic = GIC_EnableItLine(GIC_USB_OTG_LINE);
    if (GIC_OK != error_gic)
    {
        PRINT("GIC_EnableItLine::Error in Enabling Interrupt for USB");
        return;
    }

#ifndef ST_8500ED
    /*------------------------------------4500 Settings for device mode---------------------------------*/

    SER_SSP_Init(INIT_SSP0);
    SER_AB8500_CORE_Init();

    /* Watchdog settings: Enable */
    data_out = 0x01;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x02, 0x01, 0x01, &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x02, 0x01, 0x01, data_out) FAILED for data_out 0x01!!!");
        return;
    }    

 
    /* Watchdog settings: Kick */
   data_out = 0x03;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x02, 0x01, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x02, 0x01, 0x01, data_out) FAILED for data_out 0x03!!!.");
        return;
    }  

    /* Watchdog settings: Disable */
   data_out = 0x00;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x02, 0x01, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x02, 0x01, 0x01, data_out) FAILED for data_out 0x00!!!.");
        return;
     } 


#if 0     /* For HOST enable this part */

  data_out = 0x01;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x03, 0x81, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x03, 0x82, 0x01, data_out) FAILED for data_out 0x01!!!.");
        return;
    }

  data_out = 0xFF;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x05, 0x87, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x03, 0x82, 0x01, data_out) FAILED for data_out 0x01!!!.");
        return;
    } 


    /* Enable Vbus */
  data_out = 0x01;
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x03, 0x82, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x03, 0x82, 0x01, data_out) FAILED for data_out 0x01!!!.");
        return;
    } 
#endif


    /* Set UsbDeviceModeEna in USBPhyCtrl Register */
    data_out = 0x01;                              /* Make this 2 for Host */
    if(SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x05, 0x8A, 0x01,  &data_out))
    {
        PRINT("SER_AB8500_CORE_Write(0x05, 0x8A, 0x01, data_out) FAILED for data_out 0x02!!!.");
        return;
    }
#endif

    USB_Reset_Seq();
}

/****************************************************************************/
/* NAME:    SER_USB_RegisterCallback                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to connect a function that will be call */
/* on any USB event.                                                        */
/*                                                                          */
/* PARAMETERS:                                                              */
/*           - fct: function pointer                                        */
/*           - param: parameter to use when call the previous function      */
/*                                                                          */
/* RETURN: int                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC int SER_USB_RegisterCallback (t_callback_fct fct, void* param)
{
    g_usb_callback.fct = fct;
    g_usb_callback.param = param;
    return HCL_OK;
}


/****************************************************************************/
/* NAME:    SER_USB_WaitEnd(void)                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to Wait a function                      */
/*                                                                          */
/*                                                                          */
/* PARAMETERS:                                                              */
/*           none.                                                          */
/*                                                                          */
/* RETURN: int                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_USB_WaitEnd(void)
{
    /*    Do nothing.    */
}

/****************************************************************************/
/* NAME:    SER_USB_Close(void)                                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to disconnect a function that was       */
/*              registered earlier.                                         */
/*                                                                          */
/* PARAMETERS:                                                              */
/*           none.                                                          */
/*                                                                          */
/* RETURN: int                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_USB_Close( void )
{
    /*    Do nothing.    */
}


PRIVATE void USB_Reset_Seq(void)
{
    t_gpio_error gpio_error;

    gpio_error = GPIO_EnableAltFunction(GPIO_ALT_USB_OTG);
    if(gpio_error != GPIO_OK)
	    PRINT("Error in GPIO Configuration");

  /* Touareg related intialization neeed to be put here */
  
#ifndef __HCL_USB_FB  
    USB_UpdateTransceiver(NULL ,0x11);
#endif
}
