/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   USB HCL
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*	Include file 		*/
#include "usb_regp.h"
#include "usb_p.h"

/*	Global variable about system information	*/
PRIVATE t_usb_system_context    g_usb_system_context;

/* For debug HCL */
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_USB
#define MY_DEBUG_ID             myDebugID_USB

PRIVATE t_dbg_level MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id    MY_DEBUG_ID = USB_HCL_DBG_ID;
#endif

t_comp_name USB_HCL_ID = COMP_USB;

/**********************************************************************************************/
/* NAME:	t_usb_error USB_SetDbgLevel()									                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug comments levels        */
/*															                                  */
/* PARAMETERS:													                              */
/* IN : 		t_dbg_level DebugLevel:		    identify USB debug level	  	              */
/* 															                                  */
/* 															                                  */
/* RETURN:		t_usb_error :                  USB_OK always.   				              */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT											                          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PUBLIC t_usb_error USB_SetDbgLevel(IN t_dbg_level debug_level)
{
    #ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = debug_level;
    #endif
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_GetDbgLevel()									                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug comments levels        */
/*															                                  */
/* PARAMETERS:													                              */
/* IN : 		t_dbg_level *DebugLevel:		    identify USB debug level	  	          */
/* 															                                  */
/* 															                                  */
/* RETURN:		t_usb_error :                  USB_OK  always.					              */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT											                          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PUBLIC t_usb_error USB_GetDbgLevel(IN t_dbg_level *p_debug_level)
{
    if (NULL == p_debug_level)
    {
        return(USB_INVALID_PARAMETER);
    }

#ifdef __DEBUG
    *p_debug_level = MY_DEBUG_LEVEL_VAR_NAME;
#endif
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_GetVersion()									                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the current version of the USB HCL.        		      */
/*															                                  */
/* PARAMETERS:													                              */
/* OUT : 		t_version *p_version:		    Version information. 	  	                  */
/* 															                                  */
/* 															                                  */
/* RETURN:		t_usb_error :                  USB_OK  always.					              */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT											                          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PUBLIC t_usb_error USB_GetVersion(OUT t_version *p_version)
{
    DBGENTER0();
    p_version->minor = USB_HCL_MINOR_ID;
    p_version->major = USB_HCL_MAJOR_ID;
    p_version->version = USB_HCL_VERSION_ID;
    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_Init()		  					     			                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the base address of USB subsystem and checks for validity   */
/*		    by checking the peripheral and primecell IDs.					                  */
/*															                                  */
/* PARAMETERS:													                              */
/* IN : 		t_logical_addres:		    baseaddress of the USB OTG Subsystem.	          */
/* 															                                  */
/* 															                                  */
/* RETURN:		t_usb_error :               USB_UNSUPPORTED_HW: if check fails                */
/*                                            USB_OK otherwise.                               */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT											                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PUBLIC t_usb_error USB_Init(IN t_logical_address usb_base_address)
{
    DBGENTER1(" %lx ", usb_base_address);

    /*	Set the base of the CSR and non-CSR registers of Device 0	*/
    g_usb_system_context.p_usb_0_register = (t_usb_0_register *) usb_base_address;
    
    /* Initialize the Critical Section */
    HCL_CRITICAL_SECTION_INIT(USB_HCL_ID,1);
    

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_GetDeviceStatus()					     			              */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the device status. 							              */
/*															                                  */
/* PARAMETERS:													                              */
/* OUT : 		t_usb_status:		    Device Status.				                          */
/* 															                                  */
/* 															                                  */
/* RETURN:		t_usb_error :               USB_INVALID_PARAMETER: if p_status is NULL        */
/*                                            USB_OK otherwise.                               */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT											                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PUBLIC t_usb_error USB_GetDeviceStatus(OUT t_usb_status *p_dev_status)
{
    DBGENTER0();
    if (NULL == p_dev_status)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }
       
    /* Current frame number */
    p_dev_status->frame_number = (t_uint16) ((g_usb_system_context.p_usb_0_register)->OTG_FMNO & OTG_FMNO_MASK);

    /*	Other specifics of the subsystem*/
    p_dev_status->high_speed_mode_enabled = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_HSMODE_MASK)) ? TRUE : FALSE);
    p_dev_status->rx_multipacket_fifo_enabled = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_MPRXE_MASK)) ? TRUE : FALSE);
    p_dev_status->tx_multipacket_fifo_enabled = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_MPTXE_MASK)) ? TRUE : FALSE);
    p_dev_status->is_big_endian = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_BE_MASK)) ? TRUE : FALSE);
    p_dev_status->high_bandwidth_rx_fifo_enabled = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_HBRXE_MASK)) ? TRUE : FALSE);
    p_dev_status->high_bandwidth_tx_fifo_enabled = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_HBTXE_MASK)) ? TRUE : FALSE);
    p_dev_status->dynamic_fifo_sizing_enabled = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_DNYF_MASK)) ? TRUE : FALSE);
    p_dev_status->soft_conn_disconn_enabled = (t_bool) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_SC_MASK)) ? TRUE : FALSE);
    p_dev_status->utmi_data_width = (t_uint8) ((0 != ((g_usb_system_context.p_usb_0_register)->OTG_CFD & OTG_CFD_UI_MASK)) ? 16 : 8);
        

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	void  USB_GetSysIRQSrcStatus()								                      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register      		      */
/*             and updates the status		             					                  */
/* PARAMETERS:													                              */
/* IN : 		misc: mask interrupt status				                                      */
/*              is_host: usb mode status                                                      */
/* OUT :        t_usb_irq_status *:   	status of the interrupt					              */
/* 															                                  */
/* RETURN:		none												                          */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NONREENTRANT								  			                          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PRIVATE void USB_GetSysIRQSrcStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status, t_bool is_host)
{
    if (MASK_NULL32 != misc)
    {
        if (misc & OTG_INTUSB_VBE_MASK)
        {
            p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_VBUSERROR;
        }

        if (misc & OTG_INTUSB_SREQ_MASK)
        {
            p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_SESSIONREQ;
        }

        if (misc & OTG_INTUSB_DISCON_MASK)
        {
            p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_DISCONNECT;
        }

        if (misc & OTG_INTUSB_CONN_MASK)
        {
            p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_CONNECT;
        }

        if (misc & OTG_INTUSB_SOF_MASK)
        {
            p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_SOF;
        }

        if (misc & OTG_INTUSB_RST_MASK)
        {
            if (TRUE == is_host)
            {
                p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_BABBLE;
            }
            else
            {
                p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_RESET;
            }
        }

        if (misc & OTG_INTUSB_RES_MASK)
        {
            p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_RESUME;
        }

        if (misc & OTG_INTUSB_SUSP_MASK)
        {
            p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_SUSPEND;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void  USB_GetInEndpIRQSrcStatus()								                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register      		      */
/*             and updates the status		             					                  */
/* PARAMETERS:													                              */
/* IN : 		misc: mask interrupt status				                                      */
/* OUT :        t_usb_irq_status *:   	status of the interrupt					              */
/* 															                                  */
/* RETURN:		none												                          */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT								  			                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PRIVATE void USB_GetInEndpIRQStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status)
{
    if (misc & OTG_INTEP1_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP1;
    }

    if (misc & OTG_INTEP2_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP2;
    }

    if (misc & OTG_INTEP3_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP3;
    }

    if (misc & OTG_INTEP4_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP4;
    }

    if (misc & OTG_INTEP5_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP5;
    }

    if (misc & OTG_INTEP6_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP6;
    }

    if (misc & OTG_INTEP7_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP7;
    }

    if (misc & OTG_INTEP8_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP8;
    }

    if (misc & OTG_INTEP9_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP9;
    }

    if (misc & OTG_INTEP10_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP10;
    }

    if (misc & OTG_INTEP11_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP11;
    }

    if (misc & OTG_INTEP12_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP12;
    }

    if (misc & OTG_INTEP13_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP13;
    }

    if (misc & OTG_INTEP14_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP14;
    }

    if (misc & OTG_INTEP15_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_IN_EP15;
    }
}

/**********************************************************************************************/
/* NAME:	void  USB_GetHostOutEndpIRQSrcStatus()								              */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register      		      */
/*             and updates the status		             					                  */
/* PARAMETERS:													                              */
/* IN : 		misc: mask interrupt status				                                      */
/* OUT :        t_usb_irq_status *:   	status of the interrupt					              */
/* 															                                  */
/* RETURN:		none												                          */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT								  			                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PRIVATE void USB_GetOutEndpIRQStatus(IN t_uint32 misc, OUT t_usb_irq_status *p_status)
{
    if (misc & OTG_INTEP1_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP1;
    }

    if (misc & OTG_INTEP2_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP2;
    }

    if (misc & OTG_INTEP3_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP3;
    }

    if (misc & OTG_INTEP4_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP4;
    }

    if (misc & OTG_INTEP5_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP5;
    }

    if (misc & OTG_INTEP6_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP6;
    }

    if (misc & OTG_INTEP7_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP7;
    }

    if (misc & OTG_INTEP8_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP8;
    }

    if (misc & OTG_INTEP9_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP9;
    }

    if (misc & OTG_INTEP10_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP10;
    }

    if (misc & OTG_INTEP11_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP11;
    }

    if (misc & OTG_INTEP12_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP12;
    }

    if (misc & OTG_INTEP13_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP13;
    }

    if (misc & OTG_INTEP14_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP14;
    }

    if (misc & OTG_INTEP15_MASK)
    {
        p_status->initial_irq.endp |= (t_uint32) USB_IRQ_SRC_OUT_EP15;
    }
}

/**********************************************************************************************/
/* NAME:	void  USB_GetInEndpIRQSrcStatus()								                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register      		      */
/*             and updates the status		             					                  */
/* PARAMETERS:													                              */
/* IN : 		misc:		identify the interrupt source				                      */
/*              is_host:    USB Mode                                                          */
/* OUT :          t_usb_irq_status *:   	status of the interrupt					          */
/* 															                                  */
/* RETURN:		none												                          */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT								  			                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PRIVATE void USB_GetInEndpIRQSrcStatus(IN t_uint32 mis, OUT t_usb_irq_status *p_status, t_bool is_host)
{
    if (MASK_NULL32 != mis)
    {
        if (FALSE == is_host)
        {
            /* Get IN Endpoint (Device) Irq status */
            USB_GetInEndpIRQStatus(mis, p_status);
        }
        else
        {
            /* Get OUT Endpoint (Host) Irq status */
            USB_GetOutEndpIRQStatus(mis, p_status);
        }
    }
}

/**********************************************************************************************/
/* NAME:	void  USB_GetOutEndpIRQSrcStatus()								                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register      		      */
/*             and updates the status		             					                  */
/* PARAMETERS:													                              */
/* IN : 		misc: mask interrupt status				                                      */
/*              is_host:    USB Mode                                                          */
/* OUT :          t_usb_irq_status *:   	status of the interrupt					          */
/* 															                                  */
/* RETURN:		none												                          */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT								  			                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PRIVATE void USB_GetOutEndpIRQSrcStatus(IN t_uint32 mis, OUT t_usb_irq_status *p_status, t_bool is_host)
{
    if (MASK_NULL32 != mis)
    {
        if (FALSE == is_host)
        {
            /* Get OUT Endpoint (Device) Irq status */
            USB_GetOutEndpIRQStatus(mis, p_status);
        }
        else
        {
            /* Get IN Endpoint (Host) Irq status */
            USB_GetInEndpIRQStatus(mis, p_status);
        }
    }
}

/**********************************************************************************************/
/* NAME:	void  USB_GetIRQSrcStatus()								                          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register      		      */
/*             and updates the status		             					                  */
/* PARAMETERS:													                              */
/* IN : 		t_usb_irq_src:		identify the interrupt source				              */
/* OUT :          t_usb_irq_status *:   	status of the interrupt					          */
/* 															                                  */
/* RETURN:		none												                          */
/*															                                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT								  			                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL											                          */

/**********************************************************************************************/
PUBLIC void USB_GetIRQSrcStatus(IN t_usb_irq_src irq_src, OUT t_usb_irq_status *p_status)
{
    t_uint32    ris;
    t_uint32    imsc;
    t_uint32    mis;
    t_bool      is_host;

    DBGENTER0();

    if (NULL == p_status)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return;
    }
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);    
    
    /*	Get the system interrupts.	*/
    is_host = (t_bool) (((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK) ? TRUE : FALSE);

    imsc = (g_usb_system_context.p_usb_0_register)->OTG_INTUSBEN & MASK_BYTE0;

    ris = (g_usb_system_context.p_usb_0_register)->OTG_INTUSB & MASK_BYTE0;

    mis = imsc & ris;

    p_status->initial_irq.system = MASK_NULL32;
    p_status->initial_irq.endp = MASK_NULL32;
    p_status->irq_state = USB_IRQ_STATE_NEW;

    /* Get the System Irq Status */
    USB_GetSysIRQSrcStatus(mis, p_status, is_host);

    /* Get the TX Interrupts. Device (IN interrupts) and Host (OUT Interrupts)	*/
    imsc = (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN;

    ris = (g_usb_system_context.p_usb_0_register)->OTG_INTTX;

    mis = imsc & ris;

    /*	EP0 alone alone belong to the system interrupt.	*/
    if (ris & OTG_INTEP0_MASK)
    {
        p_status->initial_irq.system |= (t_uint32) USB_IRQ_SRC_EP0;
    }

    /* Get IN(Device)/OUT(Host) Irq Status */
    USB_GetInEndpIRQSrcStatus(mis, p_status, is_host);

    /* Get the RX Interrupts. Device (OUT interrupts) and Host (IN Interrupts)	*/
    imsc = (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN;

    ris = (g_usb_system_context.p_usb_0_register)->OTG_INTRX;

    mis = imsc & ris;

    /* Get OUT(Device)/IN(Host) Irq Status */
    USB_GetOutEndpIRQSrcStatus(mis, p_status, is_host);

    /*	Update the interrupts, into the status variable.	*/
    p_status->pending_irq.system = p_status->initial_irq.system;
    p_status->pending_irq.endp = p_status->initial_irq.endp;
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);
        

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	void USB_GetUSBInEndpIRQSrc	    							                      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process the active interrupts one by one.				  */
/*             The status of the interrupt is provided by t_usb_irq_status. After each status */
/*             is processed the t_usb_irq_status, global HCL event information and p_event    */
/*			   are updated.																	  */
/* PARAMETERS:																                  */
/* IN :        t_usb_irq_status     *p_status: status of the interrupt			        	  */
/* OUT :       t_usb_event	        *p_event:  current event that is handled by    	      	  */
/*											   the routine.	                                  */
/* 			   t_usb_filter_mode filter_mode   filter to selectively process the interrupt    */
/* 																			                  */
/* RETURN:		t_usb_error		 		   Error status as follows			                  */
/*				USB_NO_MORE_PENDING_EVENT: when all the interrupts are processed successfully */
/*				USB_NO_MORE_FILTER_PENDING_EVENT: when all the interrupts of the 			  */
/*									 given filter are processed successfully.				  */
/*				USB_REMAINING_PENDING_EVENTS: When extended processing is required by the     */
/*									 upper layer. HCL updates the event parameter passed      */
/*									 which is then used by the upper layer to                 */
/*									 extend the process. (return value by default)			  */
/*				USB_NO_PENDING_EVENT_ERROR: when there is no interrupt to process             */
/*				USB_INTERNAL_EVENT:  When all the statuses are processed but the events 	  */
/*									 are not processed.	 									  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* 								REENTRANCY ISSUES							                  */
/* Variables updated are:												                      */
/*																		                      */
/*	1) g_usb_dev_system_context.usb_dev_event							                      */
/*--------------------------------------------------------------------------------------------*/

/**********************************************************************************************/
PRIVATE void USB_GetUSBInEndpIRQSrc(INOUT t_usb_irq_src *p_status, IN t_usb_event *p_event)
{
    /*	Check for any of the IN interrupts active	*/
    if (p_event->endp & 0x0000000F)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP1)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP1;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP2)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP2;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP3)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP3;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP4)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP4;
        }
    }
    else if (p_event->endp & 0x00000070)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP5)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP5;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP6)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP6;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP7)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP7;
        }
    }
    else if (p_event->endp & 0x00000780)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP8)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP8;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP9)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP9;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP10)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP10;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP11)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP11;
        }
    }
    else if (p_event->endp & 0x00007800)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP12)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP12;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP13)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP13;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP14)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP14;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_IN_EP15)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_IN_EP15;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_GetUSBOutEndpIRQSrc	    							                  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process the active interrupts one by one.				  */
/*             The status of the interrupt is provided by t_usb_irq_status. After each status */
/*             is processed the t_usb_irq_status, global HCL event information and p_event    */
/*			   are updated.																	  */
/* PARAMETERS:																                  */
/* IN :        t_usb_irq_status     *p_status: status of the interrupt			        	  */
/* OUT :       t_usb_event	        *p_event:  current event that is handled by    	      	  */
/*											   the routine.	                                  */
/* 			   t_usb_filter_mode filter_mode   filter to selectively process the interrupt    */
/* 																			                  */
/* RETURN:		t_usb_error		 		   Error status as follows			                  */
/*				USB_NO_MORE_PENDING_EVENT: when all the interrupts are processed successfully */
/*				USB_NO_MORE_FILTER_PENDING_EVENT: when all the interrupts of the 			  */
/*									 given filter are processed successfully.				  */
/*				USB_REMAINING_PENDING_EVENTS: When extended processing is required by the     */
/*									 upper layer. HCL updates the event parameter passed      */
/*									 which is then used by the upper layer to                 */
/*									 extend the process. (return value by default)			  */
/*				USB_NO_PENDING_EVENT_ERROR: when there is no interrupt to process             */
/*				USB_INTERNAL_EVENT:  When all the statuses are processed but the events 	  */
/*									 are not processed.	 									  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* 								REENTRANCY ISSUES							                  */
/* Variables updated are:												                      */
/*																		                      */
/*	1) g_usb_dev_system_context.usb_dev_event							                      */
/*--------------------------------------------------------------------------------------------*/

/**********************************************************************************************/
PRIVATE void USB_GetUSBOutEndpIRQSrc(INOUT t_usb_irq_src *p_status, IN t_usb_event *p_event)
{
    /*	Check for any of the OUT interrupts active	*/
    if (p_event->endp & 0x00078000)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP1)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP1;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP2)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP2;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP3)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP3;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP4)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP4;
        }
    }
    else if (p_event->endp & 0x00380000)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP5)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP5;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP6)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP6;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP7)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP7;
        }
    }
    else if (p_event->endp & 0x03C00000)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP8)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP8;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP9)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP9;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP10)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP10;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP11)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP11;
        }
    }
    else if (p_event->endp & 0x3C000000)
    {
        if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP12)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP12;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP13)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP13;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP14)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP14;
        }
        else if (p_event->endp & (t_uint32) USB_IRQ_SRC_OUT_EP15)
        {
            p_status->endp = (t_uint32) USB_IRQ_SRC_OUT_EP15;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_GetUSBSysIRQSrc	    							                      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process the active interrupts one by one.				  */
/*             The status of the interrupt is provided by t_usb_irq_status. After each status */
/*             is processed the t_usb_irq_status, global HCL event information and p_event    */
/*			   are updated.																	  */
/* PARAMETERS:																                  */
/* IN :        t_usb_irq_status     *p_status: status of the interrupt			        	  */
/* OUT :       t_usb_event	        *p_event:  current event that is handled by    	      	  */
/*											   the routine.	                                  */
/* 			   t_usb_filter_mode filter_mode   filter to selectively process the interrupt    */
/* 																			                  */
/* RETURN:		t_usb_error		 		   Error status as follows			                  */
/*				USB_NO_MORE_PENDING_EVENT: when all the interrupts are processed successfully */
/*				USB_NO_MORE_FILTER_PENDING_EVENT: when all the interrupts of the 			  */
/*									 given filter are processed successfully.				  */
/*				USB_REMAINING_PENDING_EVENTS: When extended processing is required by the     */
/*									 upper layer. HCL updates the event parameter passed      */
/*									 which is then used by the upper layer to                 */
/*									 extend the process. (return value by default)			  */
/*				USB_NO_PENDING_EVENT_ERROR: when there is no interrupt to process             */
/*				USB_INTERNAL_EVENT:  When all the statuses are processed but the events 	  */
/*									 are not processed.	 									  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* 								REENTRANCY ISSUES							                  */
/* Variables updated are:												                      */
/*																		                      */
/*	1) g_usb_dev_system_context.usb_dev_event							                      */
/*--------------------------------------------------------------------------------------------*/

/**********************************************************************************************/
PRIVATE void USB_GetUSBSysIRQSrc(INOUT t_usb_irq_src *p_status, IN t_usb_event *p_event)
{
    /* Getting one interrupt that is active */
    /*  Order of processing is based on the priorioty of the interrupts.   */
    if (p_event->system & (t_uint32) USB_IRQ_SRC_RESET)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_RESET;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_EP0)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_EP0;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_RESUME)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_RESUME;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_BABBLE)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_BABBLE;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_CONNECT)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_CONNECT;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_DISCONNECT)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_DISCONNECT;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_SESSIONREQ)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_SESSIONREQ;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_VBUSERROR)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_VBUSERROR;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_SOF)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_SOF;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_SUSPEND)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_SUSPEND;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_OTG)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_OTG;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_PORTSTAT)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_PORTSTAT;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_I2C)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_I2C;
    }
    else if (p_event->system & (t_uint32) USB_IRQ_SRC_DEVRESPONSE)
    {
        p_status->system = (t_uint32) USB_IRQ_SRC_DEVRESPONSE;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_FilterProcessIRQErrorCheck()	    							  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process the active interrupts one by one.				  */
/*             The status of the interrupt is provided by t_usb_irq_status. After each status */
/*             is processed the t_usb_irq_status, global HCL event information and p_event    */
/*			   are updated.																	  */
/* PARAMETERS:																                  */
/* IN :        t_usb_irq_status     *p_status: status of the interrupt			        	  */
/* OUT :       t_usb_event	        *p_event:  current event that is handled by    	      	  */
/*											   the routine.	                                  */
/* 			   t_usb_filter_mode filter_mode   filter to selectively process the interrupt    */
/* 																			                  */
/* RETURN:		t_usb_error		 		   Error status as follows			                  */
/*				USB_NO_MORE_PENDING_EVENT: when all the interrupts are processed successfully */
/*				USB_NO_MORE_FILTER_PENDING_EVENT: when all the interrupts of the 			  */
/*									 given filter are processed successfully.				  */
/*				USB_REMAINING_PENDING_EVENTS: When extended processing is required by the     */
/*									 upper layer. HCL updates the event parameter passed      */
/*									 which is then used by the upper layer to                 */
/*									 extend the process. (return value by default)			  */
/*				USB_NO_PENDING_EVENT_ERROR: when there is no interrupt to process             */
/*				USB_INTERNAL_EVENT:  When all the statuses are processed but the events 	  */
/*									 are not processed.	 									  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* 								REENTRANCY ISSUES							                  */
/* Variables updated are:												                      */
/*																		                      */
/*	1) g_usb_dev_system_context.usb_dev_event							                      */
/*--------------------------------------------------------------------------------------------*/

/**********************************************************************************************/
PRIVATE t_usb_error USB_FilterProcessIRQErrorCheck
(
    IN t_usb_irq_status     *p_status,
    IN t_usb_filter_mode    *p_filter_mode,
    INOUT t_usb_event       *p_event
)
{
    /* All the interrupts are serviced by the FilterProcessIRQSrc() but not acknowledged by the upperlayer. */
    if (USB_NO_FILTER_MODE == p_filter_mode->system && USB_NO_FILTER_MODE == p_filter_mode->endp)
    {
        *p_event = p_status->pending_irq;
        if (0 == p_event->system && 0 == p_event->endp)
        {
            if (0 == g_usb_system_context.usb_event.system && 0 == g_usb_system_context.usb_event.endp)
            {
                return(USB_NO_MORE_PENDING_EVENT);
            }
            else
            {
                return(USB_INTERNAL_EVENT);
            }
        }
    }
    else
    {   /*	Filtered processing.	*/
        p_event->system = p_filter_mode->system & (p_status->pending_irq).system;
        p_event->endp = p_filter_mode->endp & (p_status->pending_irq).endp;
        if (0 == p_event->system && 0 == p_event->endp)
        {
            if
            (
                0 == (g_usb_system_context.usb_event.system & p_filter_mode->system)
            &&  0 == (g_usb_system_context.usb_event.endp & p_filter_mode->endp)
            )
            {
                if (0 == g_usb_system_context.usb_event.system && 0 == g_usb_system_context.usb_event.endp)
                {
                    return(USB_NO_MORE_PENDING_EVENT);
                }
                else
                {
                    return(USB_NO_MORE_FILTER_PENDING_EVENT);
                }
            }
            else
            {
                return(USB_INTERNAL_EVENT);
            }
        }
    }

    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_FilterProcessIRQSrc()	    							          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process the active interrupts one by one.				  */
/*             The status of the interrupt is provided by t_usb_irq_status. After each status */
/*             is processed the t_usb_irq_status, global HCL event information and p_event    */
/*			   are updated.																	  */
/* PARAMETERS:																                  */
/* IN :        t_usb_irq_status     *p_status: status of the interrupt			        	  */
/* OUT :       t_usb_event	        *p_event:  current event that is handled by    	      	  */
/*											   the routine.	                                  */
/* 			   t_usb_filter_mode filter_mode   filter to selectively process the interrupt    */
/* 																			                  */
/* RETURN:		t_usb_error		 		   Error status as follows			                  */
/*				USB_NO_MORE_PENDING_EVENT: when all the interrupts are processed successfully */
/*				USB_NO_MORE_FILTER_PENDING_EVENT: when all the interrupts of the 			  */
/*									 given filter are processed successfully.				  */
/*				USB_REMAINING_PENDING_EVENTS: When extended processing is required by the     */
/*									 upper layer. HCL updates the event parameter passed      */
/*									 which is then used by the upper layer to                 */
/*									 extend the process. (return value by default)			  */
/*				USB_NO_PENDING_EVENT_ERROR: when there is no interrupt to process             */
/*				USB_INTERNAL_EVENT:  When all the statuses are processed but the events 	  */
/*									 are not processed.	 									  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* 								REENTRANCY ISSUES							                  */
/* Variables updated are:												                      */
/*																		                      */
/*	1) g_usb_dev_system_context.usb_dev_event							                      */
/*--------------------------------------------------------------------------------------------*/

/**********************************************************************************************/
PUBLIC t_usb_error USB_FilterProcessIRQSrc
(
    INOUT t_usb_irq_status  *p_status,
    OUT t_usb_event         *p_event,
    IN t_usb_filter_mode    filter_mode
)
{
    t_usb_error     error = USB_OK;
    t_usb_irq_src   status;
    t_usb_event     temp_filter;

    DBGENTER0();
    if (NULL == p_status || NULL == p_event)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }

    /* If the status is NEW and no interrupt to be serviced. */
    if
    (
        (USB_IRQ_STATE_NEW == p_status->irq_state)
    &&  (0 == (p_status->initial_irq).system && 0 == (p_status->initial_irq).endp)
    )
    {
        DBGEXIT0(USB_NO_PENDING_EVENT_ERROR);
        return(USB_NO_PENDING_EVENT_ERROR);
    }        
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);    
    
    p_status->irq_state = USB_IRQ_STATE_OLD;

    /* All the interrupts are serviced by the FilterProcessIRQSrc() but not acknowledged by the upperlayer. */
    error = USB_FilterProcessIRQErrorCheck(p_status, &filter_mode, &temp_filter);

    if (USB_OK != error)
    {
        HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);       
        DBGEXIT0(error);
        return(error);
    }

    status.system = status.endp = 0;

    /* Getting one interrupt that is active */
    /*  Order of processing is based on the priorioty of the interrupts.   */
    /* Getting one interrupt that is active */
    /*  Order of processing is based on the priorioty of the interrupts.   */
    if (temp_filter.system & (t_uint32) USB_IRQ_SRC_SYS_ALL)
    {
        USB_GetUSBSysIRQSrc(&status, &temp_filter);
    }
    else if (temp_filter.endp & (t_uint32) USB_IRQ_SRC_EP_ALL)
    {
        /* Get first IN active interrupts */
        if (temp_filter.endp & 0x00007FFF)
        {
            USB_GetUSBInEndpIRQSrc(&status, &temp_filter);
        }
        else if (temp_filter.endp & 0x3FFF8000)
        {
            USB_GetUSBOutEndpIRQSrc(&status, &temp_filter);
        }
    }

    /*	Update the system event variable.	*/
    g_usb_system_context.usb_event.system |= status.system;
    g_usb_system_context.usb_event.endp |= status.endp;
    (p_status->pending_irq).system &= ~(status.system);
    (p_status->pending_irq).endp &= ~(status.endp);

    p_event->system = status.system;
    p_event->endp = status.endp;

    /*	Process the interrupt.	*/
    usb_ProcessIt(&status);

    error = USB_REMAINING_PENDING_EVENTS;
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);       

    DBGEXIT2(error, " Sys = %lx, Endp =%lx ", p_event->system, p_event->endp);

    return(error);
}

/**********************************************************************************************/
/* NAME:	void  USB_AcknowledgeEvent()		    								          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: AcknowledgeEvent() is called by the upper layer, after processing the given   */
/*              event. This routine resets the event bit of the global event variable.		  */
/* PARAMETERS:																                  */
/* IN :        t_usb_event:   	 the event that has to be acknowledged					  	  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:		none									 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* 								REENTRANCY ISSUES							                  */
/* Variables updated are:												                      */
/*																		                      */
/*	1) g_usb_system_context.usb_dev_event		    					                      */
/*--------------------------------------------------------------------------------------------*/

/**********************************************************************************************/
PUBLIC void USB_AcknowledgeEvent(IN t_usb_event dev_event)
{
    DBGENTER2(" %lx, %lx ", dev_event.system, dev_event.endp);
    
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
        
    g_usb_system_context.usb_event.system &= ~(dev_event.system);
    g_usb_system_context.usb_event.endp &= ~(dev_event.endp);
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);       

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_bool  USB_IsEventActive()			    								          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to check whether the given event is active or not by		  */
/*             checking the global event variable of the HCL.								  */
/* PARAMETERS:																                  */
/* IN :        t_usb_event:   	 	the event for which its status has to be     		  	  */
/*									    checked.		                                      */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:		t_bool		 TRUE if the event is active. 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_bool USB_IsEventActive(IN t_usb_event dev_event)
{
    DBGENTER2(" %lx, %lx ", dev_event.system, dev_event.endp);
        
    
    if
    (
        (g_usb_system_context.usb_event.system & dev_event.system)
    ||  (g_usb_system_context.usb_event.endp & dev_event.endp)
    )
    {
        DBGEXIT1(USB_OK, "%s", "TRUE");
        return(TRUE);
    }
        

    DBGEXIT1(USB_OK, "%s", "FALSE");
    return(FALSE);
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_ProcessEndp0It()	    	        							  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process all the interrupts that are high.				  */
/*             The status of the interrupt is provided by t_usb_irq_status.					  */
/* PARAMETERS:																                  */
/* IN :        t_bool is_host: USB Mode			                                              */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none                              		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NONREENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
void usb_ProcessEndp0It(t_bool is_host)
{
    if (TRUE == is_host)
    {
        if
        (
            USB_HOST_OUT == g_usb_system_context.last_transferred_token
        ||  USB_HOST_SETUP == g_usb_system_context.last_transferred_token
        )
        {
            usb_host_handle_status_intr(USB_ENDPNUM_0_OUT);
        }
        else
        {
            usb_host_handle_status_intr(USB_ENDPNUM_0_IN);
        }
    }
    else
    {
        usb_dev_handle_endp0_intr();
    }
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_ProcessIt()	    	        							      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to process all the interrupts that are high.				  */
/*             The status of the interrupt is provided by t_usb_irq_status.					  */
/* PARAMETERS:																                  */
/* IN :        t_usb_irq_status        *p_status:   	   status of the interrupt			  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none                              		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_ProcessIt(IN t_usb_irq_src *p_status)
{
    t_usb_irq_src               irq_src;
    t_bool                      is_host;
    t_uint16                    i;
    volatile t_usb_endp_config  endp_config;
    t_usb_error                 error = USB_OK;
    t_usb_endp_num              endp_num = USB_ENDPNUM_0_IN;    /* To avoid warning */

    DBGENTER0();

    is_host = (t_bool) (((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK) ? TRUE : FALSE);

    irq_src.system = (t_uint32) USB_IRQ_SRC_SYS_ALL;
    irq_src = *p_status;

    if (irq_src.system & (t_uint32) USB_IRQ_SRC_SYS_ALL)
    {
        /*	Reset Interrupt	*/
        if (irq_src.system & (t_uint32) USB_IRQ_SRC_RESET)
        {
            i = (g_usb_system_context.p_usb_0_register)->OTG_INTUSB;			
            i = (g_usb_system_context.p_usb_0_register)->OTG_INTTX;
            (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN = OTG_INTEP0_MASK;
            i = (g_usb_system_context.p_usb_0_register)->OTG_INTRX;		
			i=i+1;
			
            endp_config.endp_num = USB_ENDPNUM_0_OUT;
            endp_config.endp_type = USB_CONTROL_XFER;
            endp_config.maxpacketsize = USB_DEV_EP0_DEFAULT_MAXPCKSIZE;
            endp_config.fifo_size = USB_EP0_DEFAULT_FIFOSIZE;
            endp_config.enable_double_buffer = FALSE;
            endp_config.enable_multipacket = FALSE;
            endp_config.multiplier = FALSE;           
			error = USB_dev_ConfigureEndpoint(endp_config);
			if(USB_OK != error)
			{
				DBGEXIT0(error);
			}
            endp_config.endp_num = USB_ENDPNUM_0_IN;
            endp_config.endp_type = USB_CONTROL_XFER;
            endp_config.maxpacketsize = USB_DEV_EP0_DEFAULT_MAXPCKSIZE;
            endp_config.fifo_size = USB_EP0_DEFAULT_FIFOSIZE;
            endp_config.enable_double_buffer = FALSE;
            error = USB_dev_ConfigureEndpoint(endp_config);
            if(USB_OK != error)
			{
				DBGEXIT0(error);				
			}
            
        }
        else if (irq_src.system & (t_uint32) USB_IRQ_SRC_EP0)   /*	EP0 Interrupt	*/
        {
            /* Handle Endpoint0 Interrupt */
            usb_ProcessEndp0It(is_host);
        }

        /*	Disconnect Interrupt	*/
        else if (irq_src.system & (t_uint32) USB_IRQ_SRC_DISCONNECT)
        {
            g_usb_system_context.is_device_connected = FALSE;
        }

        /*	Connect Interrupt	*/
        else if (irq_src.system & (t_uint32) USB_IRQ_SRC_CONNECT)
        {
            g_usb_system_context.is_device_connected = TRUE;
        }
    }
    else if (irq_src.endp & (t_uint32) USB_IRQ_SRC_EP_ALL)
    {
        t_uint32    idx = 0;
        for (idx = 0; idx < 30; idx++)
        {
            if (1 == (irq_src.endp >> idx))
            {
                break;
            }
        }

        if (irq_src.endp & 0x7FFF)
        {
            endp_num = (t_usb_endp_num) (idx + 1);
        }
        else if (irq_src.endp & 0x3FFF8000)
        {
            endp_num = (t_usb_endp_num) (idx + 2);              /* For Out end point, don't count EP0_OUT */
        }

        if (TRUE == is_host)
        {
            usb_host_handle_status_intr(endp_num);
        }
        else
        {
            if (irq_src.endp & 0x7FFF)
            {
                usb_dev_handle_in_endp_intr(endp_num);
            }
            else if (irq_src.endp & 0x3FFF8000)
            {
                usb_dev_handle_out_endp_intr(endp_num);
            }
        }
    }

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_host_handle_status_in_endp0_intr()		        			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the host interrupts.									  */
/* PARAMETERS:																                  */
/* IN :        none                                         								  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	            						 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_host_handle_status_in_endp0_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short)
{
    t_size      xfer_len;
    t_uint32    status_value = MASK_NULL32;

    /*  Disable the interrupt   */
    (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN &= (t_uint16)~(1UL << endp_num_short);

    status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

    /*	STALL response from the device.	*/
    if (status_value & OTG_HCSR0_RXSTALL_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_RXSTALL_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_STALL;
    }

    /*	TIMEOUT error as there was no reply from the device.	*/
    else if (status_value & OTG_HCSR0_ERROR_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	}
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_ERROR_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_TIMEOUT;
    }

    /*	NAK response from the device.	*/
    else if (status_value & OTG_HCSR0_NAKTO_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	}     
     	(g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_NAKTO_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_NAK;
    }

    /*	ACK response from the device.	*/
    else if (status_value & OTG_HCSR0_RRDY_MASK)
    {
        xfer_len = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;

        if (xfer_len < g_usb_system_context.host_endpoint[endp_num].rx_length)
        {
            g_usb_system_context.host_endpoint[endp_num].rx_length = xfer_len;
        }

        /* 
           If rx_length = 0, upper layer is not interested in reading data.
           Flush the data here. Could be a spurious interrupt
        */
        if
        (
            (g_usb_system_context.host_endpoint[endp_num].rx_length == 0)
        ||  (g_usb_system_context.host_endpoint[endp_num].p_rx_buffer == NULL)
        )
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	    if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	    {
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	    }        
	}
        else
        {
            /*    Unload the received packet from Rx FIFO.    */
            usb_ReadFromFifo(endp_num_short, g_usb_system_context.host_endpoint[endp_num].p_rx_buffer, xfer_len);
        }
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_RRDY_MASK;    /* Clear the TRDY bit */

        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_ACK;
    }
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_host_handle_status_out_endp0_intr()		        			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the host interrupts.									  */
/* PARAMETERS:																                  */
/* IN :        none                                         								  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	            						 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_host_handle_status_out_endp0_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short)
{
    t_uint32    status_value = MASK_NULL32;

    /*  Disable the interrupt   */
    (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= (t_uint16)~(1UL << endp_num_short);

    status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

    /*	STALL response from the device.	*/
    if (status_value & OTG_HCSR0_RXSTALL_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_RXSTALL_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_STALL;
    }

    /*	TIMEOUT error as there was no reply from the device.	*/
    else if (status_value & OTG_HCSR0_ERROR_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_REQPKT_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_ERROR_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_TIMEOUT;
    }

    /*	NAK response from the device.	*/
    else if (status_value & OTG_HCSR0_NAKTO_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_REQPKT_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_NAKTO_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_NAK;
    }

    /*	ACK response from the device.	*/
    else if (0 == (status_value & OTG_HCSR0_TRDY_MASK))
    {
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_ACK;
    }
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_host_handle_status_in_endp_intr()		        			      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the host interrupts.									  */
/* PARAMETERS:																                  */
/* IN :        none                                         								  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	            						 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_host_handle_status_in_endp_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short)
{
    t_size      xfer_len;
    t_uint32    status_value = MASK_NULL32;

    /*  Disable the interrupt   */
    (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN &= (t_uint16)~(1UL << endp_num_short);

    status_value = (g_usb_system_context.p_usb_0_register)->OTG_RXCSR;

    /*	STALL response from the device.	*/
    if (status_value & OTG_HRXCSR_RXSTALL_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_RXSTALL_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_STALL;
    }

    /*	TIMEOUT error as there was no reply from the device.	*/
    else if (status_value & OTG_HRXCSR_ERR_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_RPK_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_ERR_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_TIMEOUT;
    }

    /*	NAK response from the device / CRC Error in the case of ISO transfer.	*/
    else if (status_value & OTG_HRXCSR_DERR_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_RPK_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_DERR_MASK;

        if
        (
            (t_uint32) USB_ISO_XFER ==
                (
                    ((g_usb_system_context.p_usb_0_register)->OTG_RXTYPE & OTG_TXRXTYPE_PROT_MASK) >>
                    OTG_TXRXTYPE_PROT_SHIFT
                )
        )
        {
            g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_CRCERR;
        }
        else
        {
            g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_NAK;
        }
    }

    /*	Incomplete Rx used mainly in high bandwidth ISO transfers.	*/
    else if (status_value & OTG_HRXCSR_IRX_MASK)
    {
        /*	Do nothing	*/
    }

    /*	ACK response from the device.	*/
    else if (status_value & OTG_HRXCSR_RRDY_MASK)
    {
        if (USB_DMA_MODE != g_usb_system_context.host_endpoint[endp_num].rx_mode)
        {
            xfer_len = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;

            if (xfer_len < g_usb_system_context.host_endpoint[endp_num].rx_length)
            {
                g_usb_system_context.host_endpoint[endp_num].rx_length = xfer_len;
            }

            /* 
              If rx_length = 0, upper layer is not interested in reading data,
              flush the data here.Could be a spurious interrupt.
            */
            if
            (
                (g_usb_system_context.host_endpoint[endp_num].rx_length == 0)
            ||  (g_usb_system_context.host_endpoint[endp_num].p_rx_buffer == NULL)
            )
            {
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
                if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	        {
                    (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	        }
	    }
            else
            {
                /*    Unload the data from the Rx FIFO.    */
                usb_ReadFromFifo(endp_num_short, g_usb_system_context.host_endpoint[endp_num].p_rx_buffer, xfer_len);
            }
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_RRDY_MASK;    /* Clear the RRDY bit */
        }
        else
        {
            /* If we get short packet, update the size of short packet received */
            xfer_len = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;
            g_usb_system_context.host_endpoint[endp_num].rx_length = xfer_len;
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= (~OTG_HRXCSR_DMAR_MASK);
        }

        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_ACK;
    }
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_host_handle_status_out_endp_intr()		        			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the host interrupts.									  */
/* PARAMETERS:																                  */
/* IN :        none                                         								  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	            						 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_host_handle_status_out_endp_intr(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short)
{
    t_uint32    status_value = MASK_NULL32;

    /*  Disable the interrupt   */
    (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= (t_uint16)~(1UL << endp_num_short);

    status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

    /*	STALL response from the device.	*/
    if (status_value & OTG_HTXCSR_RXS_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_RXS_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_STALL;
    }

    /*	TIMEOUT error as there was no reply from the device.	*/
    else if (status_value & OTG_HTXCSR_ERR_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	}
	(g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_ERR_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_TIMEOUT;
    }

    /*	NAK response from the device.	*/
    else if (status_value & OTG_HTXCSR_NAK_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	}
	(g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_NAK_MASK;
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_NAK;
    }

    /*	ACK response from the device.	*/
    else if (0 == (status_value & OTG_HTXCSR_TRDY_MASK))
    {
        g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_ACK;
    }
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_host_handle_status_intr()		        					      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the host interrupts.									  */
/* PARAMETERS:																                  */
/* IN :        none                                         								  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	            						 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_host_handle_status_intr(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    endp_num_short = (t_uint8) GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num)
    {
        /* Handle Out Endp0 status interrupt */
        usb_host_handle_status_out_endp0_intr(endp_num, endp_num_short);
    }
    else if (USB_ENDPNUM_0_IN == endp_num)
    {
        /* Handle In Endp0 status interrupt */
        usb_host_handle_status_in_endp0_intr(endp_num, endp_num_short);
    }

    /*  Non-Zero OUT Transfer    */
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        /* Handle Out Endp status interrupt */
        usb_host_handle_status_out_endp_intr(endp_num, endp_num_short);
    }

    /*  Non-Zero IN Transfer    */
    else
    {
        /* Handle In Endp status interrupt */
        usb_host_handle_status_in_endp_intr(endp_num, endp_num_short);
    }

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_ReadFromFifo()    										      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine reads data from the Rx FIFO.									  */
/* PARAMETERS:																                  */
/* IN  :		   size:   	   			   Size of the buffer.								  */
/* OUT :        void *:			   destination buffer.									  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_ReadFromFifo(IN t_uint8 endp_num_short, OUT void *p_dest, IN t_size size)
{
    t_size      i, j;
    t_uint32    *p_fifodata_0;
    t_uint32    data_received;

    DBGENTER0();

    p_fifodata_0 = usb_getFifoAddress(endp_num_short);
    
    if(NULL != p_fifodata_0)
    {
    	/* Read word by word as the memory is word aligned */
	    if (0 == ((t_uint32) p_dest % 4))
	    {
	        for (i = 0; i < (size / 4); i++)
	        {
	            ((t_uint32 *) p_dest)[i] = *p_fifodata_0;
	        }

	        size = size % 4;

	        /*	read the remaining bytes.	*/
	        if (size > 0)
	        {
	            data_received = (t_uint32) * p_fifodata_0;
	            for (j = 0; j < size; j++)
	            {
	                ((t_uint8 *)p_dest)[(4 * i) + j] = (t_uint8) (data_received >> (8 * j));
	            }
	        }
	    }
	    else
	    /* Read byte by byte as the memory is not word aligned */
	    {
	        for (i = 0; i < (size / 4); i++)
	        {
	            data_received = (t_uint32) * p_fifodata_0;
	            for (j = 0; j < 4; j++)
	            {
	            	((t_uint8 *)p_dest)[(4 * i) + j] = (t_uint8) (data_received >> (8 * j));
	            }
	        }

	        size = size % 4;

	        /*	read the remaining bytes.	*/
	        if (size > 0)
	        {
	            data_received = (t_uint32) * p_fifodata_0;
	            for (j = 0; j < size; j++)
	            {
	            	((t_uint8 *)p_dest)[(4 * i) + j] = (t_uint8) (data_received >> (8 * j));
	            }
	        }
	    }
    }

    

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_WriteToFifo()		    									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine writes data into the Tx FIFO.									  */
/* PARAMETERS:																                  */
/* IN :        t_uint8 *:			   Source buffer.										  */
/*			   size:   	   			   Size of the buffer.								      */
/*			   t_usb_endp_num:   	   Identify the endpoint.							      */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_WriteToFifo(IN t_uint8 endp_num_short, IN void *p_src, IN t_size size, IN t_usb_type usb_type)
{
    t_size      idx, jdx;
    t_uint32    *p_fifodata_0;
    t_uint32    data_totransfer;

    DBGENTER0();

    p_fifodata_0 = usb_getFifoAddress(endp_num_short);
    
    if(NULL != p_fifodata_0)
    {
    	/* Write word by word as memory is word aligned. */
	    if (0 == ((t_uint32) p_src % 4))
	    {
	        for (idx = 0; idx < (size / 4); idx++)
	        {
	            p_fifodata_0[0] = ((t_uint32 *) p_src)[idx];
	        }

	        size = size % 4;

	        /* Write remaining bytes less than word size */
	        if (size > 0)
	        {
	            for (jdx = 0; jdx < size; jdx++)
	            {
	                *((t_uint8 *) p_fifodata_0) = ((t_uint8 *)p_src)[(4 * idx) + jdx];
	            }
	        }
	    }
	    else
	    /* Write byte by byte as the memory is not word aligned. */
	    {
	        for (idx = 0; idx < (size / 4); idx++)
	        {
	            data_totransfer = 0;
	            for (jdx = 0; jdx < 4; jdx++)
	            {
	                data_totransfer |= (((t_uint8 *)p_src)[(4 * idx) + jdx] << (8 * jdx));
	            }

	            p_fifodata_0[0] = data_totransfer;
	        }

	        size = size % 4;

	        /* Write remaining bytes */
	        if (size > 0)
	        {
	            /* data_totransfer = 0; */
	            for (jdx = 0; jdx < size; jdx++)
	            {
	                *((t_uint8 *) p_fifodata_0) = ((t_uint8 *)p_src)[(4 * idx) + jdx];
	            }
	        }
	    }
    }
    

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_dev_handle_in_endp_intr()									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the IN endpoint interrupt.							  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	   Identify the endpoint.							      */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_dev_handle_in_endp_intr_itmode(IN t_usb_endp_num endp_num, IN t_uint8 endp_num_short)
{
    t_uint32    xfer_size;

    /*	Fill the Tx FIFO with the remaining bytes of data.	*/
    if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_multipacket)
    {
        xfer_size = (g_usb_system_context.dev_endpoint[endp_num].maxpacketsize) * (g_usb_system_context.dev_endpoint[endp_num].multiplier);
    }
    else
    {
        xfer_size = g_usb_system_context.dev_endpoint[endp_num].maxpacketsize;
    }

    if
    (
        xfer_size >
            (
                g_usb_system_context.dev_endpoint[endp_num].xfer_length -
                g_usb_system_context.dev_endpoint[endp_num].len_transferred
            )
    )
    {
        xfer_size = g_usb_system_context.dev_endpoint[endp_num].xfer_length - g_usb_system_context.dev_endpoint[endp_num].len_transferred;
    }

    usb_WriteToFifo
    (
        endp_num_short,
        g_usb_system_context.dev_endpoint[endp_num].p_xfer_buffer + g_usb_system_context.dev_endpoint[endp_num].len_transferred,
        xfer_size,
        USB_DEVICE_TYPE
    );

    g_usb_system_context.dev_endpoint[endp_num].len_transferred += xfer_size;

    /*  Clear the NAK status.   */
    (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~(OTG_DTXCSR_UR_MASK);

    if
    (
        g_usb_system_context.dev_endpoint[endp_num].len_transferred == g_usb_system_context.dev_endpoint[endp_num].
            xfer_length
    )
    {
        if (USB_WRITING_ENDED == g_usb_system_context.dev_endpoint[endp_num].xfer_status)
        {
            /*	send_zero_byte is set and if the data size is multiple of maxpacketsize 
        		then also transmit zero length packet.	*/
            if
            (
                0 ==
                    (
                        g_usb_system_context.dev_endpoint[endp_num].xfer_length %
                        g_usb_system_context.dev_endpoint[endp_num].maxpacketsize
                    )
                &&  TRUE == g_usb_system_context.dev_endpoint[endp_num].send_zero_byte
            )
            {
                g_usb_system_context.dev_endpoint[endp_num].send_zero_byte = FALSE;
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
            }
            else
            {
                /*	When all the bytes are transferred then update the status as 
    				USB_TRANSFER_ENDED and disable the interrupt.	*/
                g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
                (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
            }
        }

        /* Zero length packet, we set USB_TRANSFER_ENDED at the time of transferinit
and hence we need to clear only the interrupt here */
        else if (USB_TRANSFER_ENDED == g_usb_system_context.dev_endpoint[endp_num].xfer_status)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
        }
        else
        {
            /*	When all the bytes are written into the Tx FIFO then update the status as USB_WRITING_ENDED.	*/
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_WRITING_ENDED;
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
        }
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_dev_handle_in_endp_intr()									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the IN endpoint interrupt.							  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	   Identify the endpoint.							      */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_dev_handle_in_endp_intr(IN t_usb_endp_num endp_num)
{
    t_uint32    status_value = MASK_NULL32;
    t_uint8     endp_num_short;

    DBGENTER0();

    endp_num_short = (t_uint8) GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (endp_num_short & OTG_ENDPNO_INDX_MASK);

    status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

    /*	STALL sent from the device.	*/
    if (status_value & OTG_DTXCSR_SNT_MASK)
    {
        /* Clear the stall */
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_DTXCSR_SNT_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_OTHER_ERROR;
    }

    /*	Underrun error occurred in the device.	*/
    else if
        (
            status_value & OTG_DTXCSR_UR_MASK
        &&  USB_TRANSFER_ENDED == g_usb_system_context.dev_endpoint[endp_num].xfer_status
        )
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~(OTG_DTXCSR_UR_MASK);
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_UNDERRUN_ERROR;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_FFLU_MASK;
	
	if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_FFLU_MASK;
        }
    }

    /*	Data successfully sent from the Tx FIFO.	*/
    else if (0 == (status_value & OTG_DTXCSR_TRDY_MASK))
    {
        /*	ISO transfer for which, usually DMA mode is used then disable the interrupt and update the status as
        		USB_TRANSFER_ENDED	*/
        if (USB_ISO_XFER == g_usb_system_context.dev_endpoint[endp_num].endp_type)
        {
            if (USB_DMA_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
            {
                g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
                (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
            }
        }

        /*	Interrupt mode of transfer	*/
        else if (USB_IT_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
        {
            /* Handle In Endpoint Interrupt (Interrupt Mode) */
            usb_dev_handle_in_endp_intr_itmode(endp_num, endp_num_short);
        }
        else if (USB_DMA_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
        {
            /*    During DMA mode the last packet that is less than maxpacketsize should also be sent
                    by setting the TxPckRdy bit manually.    */
            /*(g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);*/
            if (USB_TRANSFER_ON_GOING == g_usb_system_context.dev_endpoint[endp_num].xfer_status)
            {
                /*    During DMA mode the last packet that is less than maxpacketsize should also be sent
                    by setting the TxPckRdy bit manually.    */
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
                g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_WRITING_ENDED;
            }
            else
            {
                (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
                g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
            }
        }
    }

    DBGEXIT0(USB_OK);
}


/****************************************************************************/
/* NAME:    USB_RegisterCallbackForExtraData   				    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to connect a function that will be call */
/* on a case when the USB host sends in more data than expected/initialised.*/
/* Only upto 512 bytes may be buffered                                      */
/*                                                                          */ 
/* PARAMETERS:                                                              */
/*           - fct: function pointer                                        */
/*                                                                          */
/*                                                                          */
/* RETURN: void                                                             */
/*              		                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
#ifdef __HCL_USB_FB
PUBLIC void USB_RegisterCallbackForExtraData(t_usb_extra_func_ptr usb_extra_fct)
{
    g_usb_system_context.extra_xfer_callback = usb_extra_fct;
    return; 
}
#endif


/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_dev_handle_out_endp_intr()								      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the OUT endpoint interrupt.							  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	   Identify the endpoint.							      */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_dev_handle_out_endp_intr(IN t_usb_endp_num endp_num)
{
    t_uint8     endp_num_short;
    t_uint32    status_value = MASK_NULL32;
    t_size      xfer_length = MASK_NULL32;

    DBGENTER0();

    /* g_usb_system_context.dev_endpoint[endp_num].remaining_bytes = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK; */
    if (USB_TRANSFER_ON_GOING == g_usb_system_context.dev_endpoint[endp_num].xfer_status)
    {
        endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
        (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

        status_value = (g_usb_system_context.p_usb_0_register)->OTG_RXCSR;

        /*	STALL sent from the device.	*/
        if (status_value & OTG_DRXCSR_SNT_MASK)
        {
            /* Clear OTG_DRXCSR_SNT_MASK */
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_SNT_MASK;
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_OTHER_ERROR;
        }

        /*	Overrun error occurred in the device.	*/
        else if (status_value & OTG_DRXCSR_OR_MASK)
        {
            /* Clear OTG_DRXCSR_OR_MASK */
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_OR_MASK;
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_OVERRUN_ERROR;
        }

        /*	CRC or bit-stuff error in the received packet.	*/
        else if (status_value & OTG_DRXCSR_ERR_MASK)
        {
            /* Clear OTG_DRXCSR_RRDY_MASK bit */
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_OTHER_ERROR;
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;
        }

        /*	Packet successfully received in Rx FIFO.	*/
        else if (status_value & OTG_DRXCSR_RRDY_MASK)
        {
            /*	Interrupt mode.	*/
            if (USB_IT_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
            {
                g_usb_system_context.dev_endpoint[endp_num].remaining_bytes = xfer_length = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;

                if
                (
                    xfer_length >
                        (
                            g_usb_system_context.dev_endpoint[endp_num].xfer_length -
                            g_usb_system_context.dev_endpoint[endp_num].len_transferred
                        )
                )

                {
#ifdef __HCL_USB_FB 
			g_usb_system_context.dev_endpoint[endp_num].overflow_bytes = xfer_length - 
				    (g_usb_system_context.dev_endpoint[endp_num].xfer_length - g_usb_system_context.dev_endpoint[endp_num].len_transferred);
#endif	
    			xfer_length =
                        (
                            g_usb_system_context.dev_endpoint[endp_num].xfer_length -
                            g_usb_system_context.dev_endpoint[endp_num].len_transferred
                        );
                }
                
		/* Unload the Rx FIFO.	*/
                usb_ReadFromFifo
                (
                    endp_num_short,
                    g_usb_system_context.dev_endpoint[endp_num].p_xfer_buffer + g_usb_system_context.dev_endpoint[endp_num].len_transferred,
                    xfer_length
                );
#ifdef __HCL_USB_FB 
		/* Unload the Rx FIFO for extra bytes sent by Host */
		if(NULL != g_usb_system_context.dev_endpoint[endp_num].overflow_bytes)
		{
                    usb_ReadFromFifo 
                    (
                        endp_num_short,
                        g_usb_system_context.dev_endpoint[endp_num].extra_xfer_buffer,
                        g_usb_system_context.dev_endpoint[endp_num].overflow_bytes 
                    );
		}		    
#endif
                /*	Clear the RxPckRdy bit, but setting FIFO flush bit is a faster operation.	*/
                /*(g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;*/
                g_usb_system_context.dev_endpoint[endp_num].len_transferred += xfer_length;

                /*	When all bytes of data received, or packet less than maxpacketsize is received
                    	then update the status as USB_TRANSFER_ENDED. */
                if
                (
                    g_usb_system_context.dev_endpoint[endp_num].len_transferred == g_usb_system_context.dev_endpoint[endp_num].xfer_length
                    ||  xfer_length < g_usb_system_context.dev_endpoint[endp_num].maxpacketsize
                )
                {
                    g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
                    (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN &= ~(1UL << endp_num_short);
                 //   (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;    Uncomment for testing Souvik
		    
#ifdef __HCL_USB_FB
		    if( (NULL!=(void *)(g_usb_system_context.extra_xfer_callback)) && (NULL != g_usb_system_context.dev_endpoint[endp_num].overflow_bytes))
		    { 
                        g_usb_system_context.extra_xfer_callback
				(
				    g_usb_system_context.dev_endpoint[endp_num].extra_xfer_buffer,
				    g_usb_system_context.dev_endpoint[endp_num].overflow_bytes, endp_num 
			        ); 
		    }

                    (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;
                    g_usb_system_context.dev_endpoint[endp_num].overflow_bytes = 0;
#endif	    
                }
                else
                {
                    (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;
                }
            }

            /*	DMA mode.	*/
            else if (USB_DMA_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
            {
                g_usb_system_context.dev_endpoint[endp_num].remaining_bytes = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;
                (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN &= ~(1UL << endp_num_short);
                g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
            }
        }
    }

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PRIVATE void  usb_dev_handle_endp0_intr()		    						      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine processes the EP0 interrupts (Setup, SetConfig, SetInterface, 	  */
/*             and EP0 IN & OUT interrupts.                                                   */
/* PARAMETERS:																                  */
/* IN :        none																			  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void usb_dev_handle_endp0_intr(void)
{
    t_uint32    status_value;
    t_size      xfer_length = MASK_NULL32;

    DBGENTER0();

    /*	OUT transfer, DATA stage. */
    if (USB_TRANSFER_ENDED != g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].xfer_status)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL8;

        status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

        /*	Packet successfully received.	*/
        if (status_value & OTG_DCSR0_RRDY_MASK)
        {
            xfer_length = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;

            if
            (
                xfer_length >
                    (
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].xfer_length -
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].len_transferred
                    )
            )
            {
                xfer_length =
                    (
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].xfer_length -
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].len_transferred
                    );
            }

            /*	Unload the packet from the Rx FIFO	*/
            usb_ReadFromFifo
            (
                MASK_NULL8,
                g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].p_xfer_buffer + g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].len_transferred,
                xfer_length
            );

            g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].len_transferred += xfer_length;

            /*	If all the bytes of data received or packet size less than maxpacketsize is received, 
                	then update the status as USB_TRANSFER_ENDED	*/
            if
            (
                g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].len_transferred == g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].xfer_length
                ||  xfer_length < g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].maxpacketsize
            )
            {
                g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].xfer_status = USB_TRANSFER_ENDED;

                /* Set DataEnd bit if the Data stage is over */
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_SERRDY_MASK | OTG_DCSR0_DATAEND_MASK;
            }
            else
            {
				g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].xfer_status = USB_TRANSFER_ON_GOING;
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_SERRDY_MASK;
            }
        }

        /*	If the transfer is stopped abruptly when new SETUP packet has arrived or when STALL response 
            	is sent.	*/
        if (status_value & (OTG_DCSR0_SETUPEND_MASK | OTG_DCSR0_SNTSTALL_MASK))
        {
            g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_OUT].xfer_status = USB_TRANSFER_ENDED;
        }
    }

    /*	IN transfer, DATA stage. */
    if (USB_TRANSFER_ENDED != g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].xfer_status)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL8;

        status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

        /*	Packet transmitted successfully.	*/
        if (0 == (status_value & OTG_DCSR0_TRDY_MASK))
        {
            xfer_length = g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].maxpacketsize;
            if
            (
                xfer_length >
                    (
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].xfer_length -
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].len_transferred
                    )
            )
            {
                xfer_length = g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].xfer_length - g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].len_transferred;
            }

            usb_WriteToFifo
            (
                MASK_NULL8,
                g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].p_xfer_buffer + g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].len_transferred,
                xfer_length,
                USB_DEVICE_TYPE
            );

            g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].len_transferred += xfer_length;

            if
            (
                g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].len_transferred == g_usb_system_context.
                    dev_endpoint[USB_ENDPNUM_0_IN].xfer_length
            )
            {
                if (USB_WRITING_ENDED == g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].xfer_status)
                {
                    /*	If send_zero_byte is set to TRUE and if the data size is multiple of maxpacketsize then 
                    		zero length packet is sent.*/
                    if
                    (
                        0 ==
                            (
                                g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].xfer_length %
                                g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].maxpacketsize
                            )
                        &&  TRUE == g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].send_zero_byte
                    )
                    {
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].send_zero_byte = FALSE;
                        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_TRDY_MASK;
                    }
                    else
                    {
                        /*	If all the bytes are transmitted then update the status as USB_TRANSFER_ENDED.	*/
                        g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].xfer_status = USB_TRANSFER_ENDED;
                    }
                }
                else
                {
                    /*	If all the bytes are written into the Tx FIFO then update the status as USB_WRITING_ENDED.	*/
                    g_usb_system_context.dev_endpoint[USB_ENDPNUM_0_IN].xfer_status = USB_WRITING_ENDED;
                    (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_DATAEND_MASK | OTG_DCSR0_TRDY_MASK;
                }
            }
            else
            {
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_TRDY_MASK;
            }
        }
    }

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_SetConfiguration()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the USB device HCL. It updates the different         */
/*              endpoint information and common registers like OTG_DCR, OTG_OFSZ, OTG_THR etc.*/
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_dev_config:		    configuration information to be set.	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:		t_usb_error :                               					          	  */
/*              USB_REQUEST_NOT_APPLICABLE: If it is High Speed.                              */
/*              USB_OK:                     Otherwise.                                        */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_SetConfiguration(IN t_usb_dev_config *dev_config)
{
    volatile t_usb_endp_config  endp_config;
    t_usb_error                 error = USB_OK;
    t_size                      i;

    DBGENTER1
    (
        " %s ",
        (USB_LOW_SPEED == dev_config->speed) ? "LOW_SPEED" :
            ((USB_FULL_SPEED == dev_config->speed) ? "FULL_SPEED" : "HIGH_SPEED")
    );

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    /*
    	Initialize all the registers and the global variables.
    */
    g_usb_system_context.is_device_connected = FALSE;
    g_usb_system_context.usb_event.system = MASK_NULL32;
    g_usb_system_context.usb_event.endp = MASK_NULL32;

    /*	Initialize all the endpoint specific details.	*/
    for (i = 0; i < USB_MAX_NUM_ENDPOINTS; i++)
    {
        g_usb_system_context.dev_endpoint[i].xfer_status = USB_TRANSFER_ENDED;
        g_usb_system_context.dev_endpoint[i].is_endpoint_configured = FALSE;
        g_usb_system_context.dev_endpoint[i].enable_double_buffer = FALSE;
        g_usb_system_context.dev_endpoint[i].enable_nyet = FALSE;
        g_usb_system_context.dev_endpoint[i].fifosize = dev_config->fifosize[i];
    }

    /* HS mode is enabled, and negotiation for the same with the host is carried out during the Reset. */
    if (USB_HIGH_SPEED == dev_config->speed)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_HSEN_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_DNY_MASK;
    #ifdef ST_8500ED	
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL |= OTG_TOPCTRL_ULPI_MASK;
    #endif
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR &= ~OTG_PWR_HSEN_MASK;
    }

    /*	Disable all the interrupts (by reading them) except the Reset and EP0 interrupt.	*/
    i = (g_usb_system_context.p_usb_0_register)->OTG_INTUSB;
	
    (g_usb_system_context.p_usb_0_register)->OTG_INTUSBEN = OTG_INTUSB_RST_MASK;        

    i = (g_usb_system_context.p_usb_0_register)->OTG_INTTX;
	
    (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN = OTG_INTEP0_MASK;        

    i = (g_usb_system_context.p_usb_0_register)->OTG_INTRX;
	
    (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN = MASK_NULL16;
        

    /*	Configure the EP0.	*/
    endp_config.endp_num = USB_ENDPNUM_0_OUT;
    endp_config.endp_type = USB_CONTROL_XFER;
    endp_config.maxpacketsize = USB_DEV_EP0_DEFAULT_MAXPCKSIZE;
    endp_config.fifo_size = USB_EP0_DEFAULT_FIFOSIZE;
    endp_config.enable_double_buffer = FALSE;
    endp_config.enable_nyet = FALSE;
    endp_config.enable_multipacket = FALSE;
    endp_config.multiplier = FALSE;
    error = USB_dev_ConfigureEndpoint(endp_config);
	if(USB_OK != error)
	{
		DBGEXIT0(error);
		return(error);
	}

    endp_config.endp_num = USB_ENDPNUM_0_IN;
    endp_config.endp_type = USB_CONTROL_XFER;
    endp_config.maxpacketsize = USB_DEV_EP0_DEFAULT_MAXPCKSIZE;
    endp_config.fifo_size = USB_EP0_DEFAULT_FIFOSIZE;
    endp_config.enable_double_buffer = FALSE;
    endp_config.enable_nyet = FALSE;
    error = USB_dev_ConfigureEndpoint(endp_config);
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);
   
    DBGEXIT0(error);
    return(error);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_ConfigureEndpoint()										      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures each endpoint with the endpoint configuration details */
/*				and also updates the endpoint information register.							  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          t_usb_endp_config:			config details of the given endpoint.			  */
/* OUT : 		none															  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_ConfigureEndpoint(IN t_usb_endp_config endp_config)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    endp_num_short = (t_uint8) GET_0_ENDPNUM_SHORT(endp_config.endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (endp_num_short & OTG_ENDPNO_INDX_MASK);

    g_usb_system_context.dev_endpoint[endp_config.endp_num].endp_type = endp_config.endp_type;
    g_usb_system_context.dev_endpoint[endp_config.endp_num].maxpacketsize = endp_config.maxpacketsize;
#ifdef __HCL_USB_FB
	if (!((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_HSMODE_MASK)) /* Do it only if Full Speed */
	{
        g_usb_system_context.dev_endpoint[endp_config.endp_num].maxpacketsize = 64;
    }
#endif
    g_usb_system_context.dev_endpoint[endp_config.endp_num].enable_double_buffer = endp_config.enable_double_buffer;
    g_usb_system_context.dev_endpoint[endp_config.endp_num].enable_nyet = endp_config.enable_nyet;
    if (0 != (endp_config.fifo_size % endp_config.maxpacketsize))
    {
        g_usb_system_context.dev_endpoint[endp_config.endp_num].fifosize = (t_usb_fifo_size) (endp_config.maxpacketsize);
    }
    else
    {
        g_usb_system_context.dev_endpoint[endp_config.endp_num].fifosize = (t_usb_fifo_size) endp_config.fifo_size;
    }

    /* g_usb_system_context.dev_endpoint[endp_config.endp_num].enable_multipacket = endp_config.enable_multipacket; */
    g_usb_system_context.dev_endpoint[endp_config.endp_num].is_endpoint_configured = TRUE;

    if (TRUE == g_usb_system_context.dev_endpoint[endp_config.endp_num].enable_multipacket)
    {
        /* g_usb_system_context.dev_endpoint[endp_config.endp_num].multiplier = endp_config.multiplier; */
        (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP |= ((endp_config.multiplier) << OTG_MAXP_MULT_SHIFT);
    }
#ifdef USB_FIFO_MGMT
    /* Reset the data endpoint toggle to 0 */
    /*
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_CLR_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_CLR_MASK;
*/
    /*  Update the Maxp Size, FIFO size, FIFO double buffering, and FIFO address of each endpoint. */
    if((USB_ENDPNUM_0_IN == endp_config.endp_num) || (USB_ENDPNUM_0_OUT == endp_config.endp_num))
	{
		return (usb_UpdateFifosEps());	
	}
	else
	{
			return(usb_AllocateFifoBuffer(endp_config.endp_num));
	}
#else
	return(usb_UpdateFifos());
#endif
}
#ifdef USB_FIFO_MGMT
/**********************************************************************************************/
/* NAME:	 void  usb_UpdateFifosEps()	                   						      		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine Updates the FIFO registers during the EP0 configuration.		  */
/* PARAMETERS:																                  */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		none.                                       						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */
/**********************************************************************************************/
PRIVATE t_usb_error usb_UpdateFifosEps(void)
{
		t_size      i;
		
		for (i = 1; i < (USB_MAX_NUM_ENDPOINTS / 2); i++)
		{
			/*Tx Endpoint*/
			(g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_TXFA = NULL;			
			
			/*Rx Endpoint*/
			(g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_RXFA = NULL;
						
			/* Ensure that the bit_numbers are cleared*/
			g_usb_system_context.bit_number[i] = 0x0;
			g_usb_system_context.bit_number[i + (USB_MAX_NUM_ENDPOINTS / 2)] = 0x0;
			g_usb_system_context.chunk_count[i] = 0x0;
			g_usb_system_context.chunk_count[i + (USB_MAX_NUM_ENDPOINTS / 2)] = 0x0;
		}
		
		for (i =0; i<6 ;i++)
				g_usb_system_context.fifo_pool[i] = 0x00000000;
		
		/*Now first 128 bytes of the fifo shall be used by the EP0 IN and OUT*/
		/*So in the mapping make that as 0x3 */
		g_usb_system_context.fifo_pool[0] = 0x00000003; /*Indicating that first 128 bytes are unavailable*/
	
	DBGEXIT0(USB_OK);
    return(USB_OK);		
}
/**********************************************************************************************/
/* NAME:	 t_usb_error  usb_AllocateFifoBuffer()	                   						  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:	This routine allocates the Fifo for a particular endpoint requested			  */
/* PARAMETERS:																                  */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		USB_INVALID_PARAMETER	   : If Fifo Size is invalid						  */
/*				USB_REQUEST_NOT_APPLICABLE : If Fifo memory is all Allocated and another 	  */
/*											 Request is generated							  */
/*				USB_OK					   : For Successfull Allocation						  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */
/**********************************************************************************************/
PRIVATE t_usb_error usb_AllocateFifoBuffer(t_usb_endp_num endp_num)
{
	t_uint16 offset = USB_EP0_DEFAULT_FIFOSIZE * 2;  /* Because of double buffering, you need to multiply by 2. */
	t_uint8     fifo_size = USB_INVALID_FIFO_SIZE;
	t_size      i;									 /* Endpoint number in integer*/ 
	t_uint32		chunks = 0x0;						 /*Gives the number of 64-bit chunks required*/
	t_uint8   bit_number= 0x2;          /* Bits 1 and 0 are used by endpoint 0 */
	t_uint32 fifo_requirement = 0x0;
	
	/*-----------------Tx Fifo Configuration--------------------*/
		
	/*Get the Endpoint for which the request for the FIFO has been requested*/
	i = (t_uint8) GET_0_ENDPNUM_SHORT(endp_num);
	
	(g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8)(i & OTG_ENDPNO_INDX_MASK);
	(g_usb_system_context.p_usb_0_register)->OTG_TXMAXP = g_usb_system_context.dev_endpoint[i].maxpacketsize & OTG_MAXP_MASK;
	
	fifo_size=usb_GetFifoSize(g_usb_system_context.dev_endpoint[i].fifosize);
	fifo_requirement = (t_uint32)g_usb_system_context.dev_endpoint[i].fifosize;
	if(USB_INVALID_FIFO_SIZE == fifo_size)
	{
			DBGEXIT0(USB_INVALID_PARAMETER);
			return(USB_INVALID_PARAMETER);
	}
	(g_usb_system_context.p_usb_0_register)->OTG_TXFSZ = fifo_size;
	if (TRUE == g_usb_system_context.dev_endpoint[i].enable_double_buffer)
	{
		(g_usb_system_context.p_usb_0_register)->OTG_TXFSZ |= OTG_FIFOSZ_DPB_MASK;
		fifo_requirement = fifo_requirement*2;
	}
	chunks = fifo_requirement/64; 		/*indicates no of 64 byte chunks that are needed*/
	bit_number = usb_GetOffsetCount(chunks);
	if(192 == bit_number)
  {
      DBGEXIT0(USB_REQUEST_NOT_APPLICABLE);
      return(USB_REQUEST_NOT_APPLICABLE);
  }
  g_usb_system_context.bit_number[i] = bit_number;
  g_usb_system_context.chunk_count[i] = chunks;
	offset = bit_number * 64; 
	(g_usb_system_context.p_usb_0_register)->OTG_TXFA = (t_uint16) (USB_DEV_0_GET_FIFO_OFFSET_VALUE(offset));
	
	/*------------------Rx Fifo Configuration--------------------*/
	(g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
  (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP = g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].maxpacketsize & OTG_MAXP_MASK;
	fifo_size = usb_GetFifoSize(g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize);
	fifo_requirement = (t_uint32)g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize;
  if(USB_INVALID_FIFO_SIZE == fifo_size)
  {
      DBGEXIT0(USB_INVALID_PARAMETER);
      return(USB_INVALID_PARAMETER);
  }
  (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ = fifo_size;
  if (TRUE == g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].enable_double_buffer)
  {
      (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ |= OTG_FIFOSZ_DPB_MASK;
      fifo_requirement = fifo_requirement*2;
  }
  chunks = fifo_requirement/64;
  bit_number = usb_GetOffsetCount(chunks);
  if(192 == bit_number)
  {
      DBGEXIT0(USB_REQUEST_NOT_APPLICABLE);
      return(USB_REQUEST_NOT_APPLICABLE);
  }
  g_usb_system_context.bit_number[i + (USB_MAX_NUM_ENDPOINTS / 2)] = bit_number;
  g_usb_system_context.chunk_count[i + (USB_MAX_NUM_ENDPOINTS / 2)] = chunks;
  offset = bit_number * 64;
  (g_usb_system_context.p_usb_0_register)->OTG_RXFA = (t_uint16) (USB_DEV_0_GET_FIFO_OFFSET_VALUE(offset));   
  
  DBGEXIT0(USB_OK);
  return(USB_OK);	
}
/**********************************************************************************************/
/* NAME:	  usb_DeallocateFifoBuffer()	                   						  		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:	This Deallocates the Fifo Buffer assigned to a particular Endpoint when the   */
/*				endpoint Deconfiguration is made											  */
/* PARAMETERS:	t_usb_endp_num : Endpoint number    						                  */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		USB_OK: Returns on successful deallocation    						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */
/**********************************************************************************************/
PRIVATE t_usb_error usb_DeallocateFifoBuffer(t_usb_endp_num endp_num)
{
  t_size      i;									 /* Endpoint number in integer*/ 
  t_uint8     bit_number =0x0;
  t_uint8     no_of_bits = 0x0;
  
	i = (t_uint8) GET_0_ENDPNUM_SHORT(endp_num);
	(g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
  (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP = 0;
  (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ = 0;
  (g_usb_system_context.p_usb_0_register)->OTG_TXFA = NULL;
  /*get the tx bit_number*/
  bit_number = g_usb_system_context.bit_number[i];
  no_of_bits = g_usb_system_context.chunk_count[i];
  usb_ClearBufferBits(bit_number,no_of_bits);
  g_usb_system_context.bit_number[i] = 0x0;
  g_usb_system_context.chunk_count[i]= 0x0;
  
  (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
  (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP = 0;
  (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ = 0;
  (g_usb_system_context.p_usb_0_register)->OTG_RXFA = NULL;
  /*get the rx bit number */
  bit_number = g_usb_system_context.bit_number[i + (USB_MAX_NUM_ENDPOINTS / 2)];
  no_of_bits = g_usb_system_context.chunk_count[i + (USB_MAX_NUM_ENDPOINTS / 2)];
  usb_ClearBufferBits(bit_number,no_of_bits);
  g_usb_system_context.bit_number[i + (USB_MAX_NUM_ENDPOINTS / 2)] = 0x0;
  g_usb_system_context.chunk_count[i + (USB_MAX_NUM_ENDPOINTS / 2)]= 0x0;

  DBGEXIT0(USB_OK);
  return(USB_OK);	
}
/**********************************************************************************************/
/* NAME:	  usb_ClearBufferBits()	                   						  		  		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:	This is used to clear the Fifo Buffer usage global context					  */
/* PARAMETERS:	bit_number : Gives the offset in the context    						      */
/*				no_of_bits : Gives the size in terms of 64Byte chunks						  */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		none									    						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */
/**********************************************************************************************/
PRIVATE void usb_ClearBufferBits(t_uint8 bit_number,t_uint8 no_of_bits)
{
    t_uint32 mask;
    
    mask = usb_GetMask(no_of_bits);
    
    if(bit_number<32)
    {
        mask = mask << bit_number;
        g_usb_system_context.fifo_pool[0] = g_usb_system_context.fifo_pool[0]^mask;
    }
    else if ((bit_number < 64) && (bit_number >= 32))
    {
      mask = mask << (bit_number - 32);
      g_usb_system_context.fifo_pool[1] = g_usb_system_context.fifo_pool[1]^mask;
    }
    else if ((bit_number < 96) && (bit_number >= 64))
    {
      mask = mask << (bit_number - 64);
      g_usb_system_context.fifo_pool[2] = g_usb_system_context.fifo_pool[2]^mask;
    }
    else if ((bit_number < 128) && (bit_number >= 96))
    {
      mask = mask << (bit_number - 96);
      g_usb_system_context.fifo_pool[3] = g_usb_system_context.fifo_pool[3]^mask;
    }
    else if ((bit_number < 160) && (bit_number >= 128))
    {
      mask = mask << (bit_number - 128);
      g_usb_system_context.fifo_pool[4] = g_usb_system_context.fifo_pool[4]^mask;
    }
    else
    {
      mask = mask << (bit_number - 160);
      g_usb_system_context.fifo_pool[5] = g_usb_system_context.fifo_pool[5]^mask;
    }
}

/**********************************************************************************************/
/* NAME:	  usb_GetOffsetCount()	                   						  		  		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:	This Returns the offset to be used for Fifo allocation for an endpoint		  */
/* PARAMETERS:	no_of_chunks : Indicates the number of 64byte chunks that are needed	      */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		t_uint8 : Gives an indirect Offset to the Endpoint Fifo Memory    			  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */
/**********************************************************************************************/
PRIVATE t_uint8  usb_GetOffsetCount(t_uint32 no_of_chunks)
{
    t_uint32 mask, temp_mask;
    t_uint8 i;
    t_uint8 bit_count = 0;
    t_uint32 temp, temp_result = 0;
    
    mask = usb_GetMask(no_of_chunks);
    temp_mask = mask;     
    temp = g_usb_system_context.fifo_pool[0];
    
    for(i=0 ; i< (32-no_of_chunks); i++)
    {
        temp_result = temp & mask;
        if(temp_result == 0)
        {
            g_usb_system_context.fifo_pool[0]|= mask;
            return(bit_count);
        }
        else
        { 
            mask = mask << 1;
            bit_count = bit_count + 1;
        }
    }
    bit_count = 32;
    mask = temp_mask;
    temp = g_usb_system_context.fifo_pool[1];
    for(i=0 ; i< (32-no_of_chunks); i++)
    {
        temp_result = temp & mask;
        if(temp_result == 0)
        {
            g_usb_system_context.fifo_pool[1]|= mask;
            return(bit_count);
        }
        else
        { 
            mask = mask << 1;
            bit_count = bit_count + 1;
        }
    }
    bit_count = 64;
    mask = temp_mask;
    temp = g_usb_system_context.fifo_pool[2];
    for(i=0 ; i< (32-no_of_chunks); i++)
    {
        temp_result = temp & mask;
        if(temp_result == 0)
        {
            g_usb_system_context.fifo_pool[2]|= mask;
            return(bit_count);
        }
        else
        { 
            mask = mask << 1;
            bit_count = bit_count + 1;
        }
    }
    bit_count = 96;
    mask = temp_mask;
    temp = g_usb_system_context.fifo_pool[3];
    for(i=0 ; i< (32-no_of_chunks); i++)
    {
        temp_result = temp & mask;
        if(temp_result == 0)
        {
            g_usb_system_context.fifo_pool[3]|= mask;
            return(bit_count);
        }
        else
        { 
            mask = mask << 1;
            bit_count = bit_count + 1;
        }
    }
    bit_count = 128;
    mask = temp_mask;
    temp = g_usb_system_context.fifo_pool[4];
    for(i=0 ; i< (32-no_of_chunks); i++)
    {
        temp_result = temp & mask;
        if(temp_result == 0)
        {
            g_usb_system_context.fifo_pool[4]|= mask;
            return(bit_count);
        }
        else
        { 
            mask = mask << 1;
            bit_count = bit_count + 1;
        }
    }
    bit_count = 160;
    mask = temp_mask;
    temp = g_usb_system_context.fifo_pool[5];
    for(i=0 ; i< (32-no_of_chunks); i++)
    {
        temp_result = temp & mask;
        if(temp_result == 0)
        {
            g_usb_system_context.fifo_pool[5]|= mask;
            return(bit_count);
        }
        else
        { 
            mask = mask << 1;
            bit_count = bit_count + 1;
        }
    }
    bit_count = 192;
    return(bit_count);
    
}
/**********************************************************************************************/
/* NAME:	  usb_GetMask()	                   						  		  		          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:	Based on the 64Byte Chunk Count, it returns the mask for the global context	  */
/* PARAMETERS:	chunk_count : Number of 64Byte chunks that are required						  */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		t_uint32 : Returns the Mask for the global context			    			  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */
/**********************************************************************************************/
PRIVATE t_uint32 usb_GetMask(t_uint32 chunk_count)
{
	t_uint32 mask = 0x00000000;
    switch(chunk_count)
    {
        case 1: mask =((t_uint32)USB_COUNT1);
				break;
        case 2: mask =((t_uint32)USB_COUNT2);
				break;
        case 3: mask =((t_uint32)USB_COUNT3);
				break;
        case 4: mask =((t_uint32)USB_COUNT4);
				break;
        case 5: mask =((t_uint32)USB_COUNT5);
				break;
        case 6: mask =((t_uint32)USB_COUNT6);
				break;
        case 7: mask =((t_uint32)USB_COUNT7);
				break;
        case 8: mask =((t_uint32)USB_COUNT8);
				break;
        case 9: mask =((t_uint32)USB_COUNT9);
				break;
        case 10: mask =((t_uint32)USB_COUNT10);
				break;
        case 11: mask =((t_uint32)USB_COUNT11);
				break;
        case 12: mask =((t_uint32)USB_COUNT12);
				break;
        case 13: mask =((t_uint32)USB_COUNT13);
				break;
        case 14: mask =((t_uint32)USB_COUNT14);
				break;
        case 15: mask =((t_uint32)USB_COUNT15);
				break;
        case 16: mask =((t_uint32)USB_COUNT16);
				break;
        case 17: mask =((t_uint32)USB_COUNT17);
				break;
        case 18: mask =((t_uint32)USB_COUNT18);
				break;
        case 19: mask =((t_uint32)USB_COUNT19);
				break;
        case 20: mask =((t_uint32)USB_COUNT20);
				break;
        case 21: mask =((t_uint32)USB_COUNT21);
				break;
        case 22: mask =((t_uint32)USB_COUNT22);
				break;
        case 23: mask =((t_uint32)USB_COUNT23);
				break;
        case 24: mask =((t_uint32)USB_COUNT24);
				break;
        case 25: mask =((t_uint32)USB_COUNT25);
				break;
        case 26: mask =((t_uint32)USB_COUNT26);
				break;
        case 27: mask =((t_uint32)USB_COUNT27);
				break;
        case 28: mask =((t_uint32)USB_COUNT28);
				break;
        case 29: mask =((t_uint32)USB_COUNT29);
				break;
        case 30: mask =((t_uint32)USB_COUNT30);
				break;
        case 31: mask =((t_uint32)USB_COUNT31);
				break;
        case 32: mask =((t_uint32)USB_COUNT32);
				break;
    }
	return(mask);
}
#endif
/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_ResetEndpointConfig()	        						      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures each endpoint with the endpoint configuration details */
/*				and also updates the endpoint information register.							  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          t_usb_endp_config:			config details of the given endpoint.			  */
/* OUT : 		none															  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_ResetEndpointConfig(IN t_usb_endp_num endp_num)
{
    t_usb_error error=USB_OK;
    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    g_usb_system_context.dev_endpoint[endp_num].is_endpoint_configured = FALSE;

    /*  Update the Maxp Size, FIFO size, FIFO double buffering, and FIFO address of each endpoint. */
 #ifdef USB_FIFO_MGMT
    error = usb_DeallocateFifoBuffer(endp_num);
 #else
    error=usb_UpdateFifos();
 #endif    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(error);
    return(error);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_SetPortConfig()		    					     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the port.   										  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_dev_port_config:  port configuration.							  		  */
/* OUT : 		none.                                       								  */
/* 																			                  */
/* RETURN:		t_usb_error.																  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_SetPortConfig(IN t_usb_dev_port_config port_config)
{
    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    /*  It is required to clear this bit after being set in after 10ms  (max 15ms)  */
    if (USB_SET == port_config.resume_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_RSM_MASK;
    }
    else if (USB_CLEAR == port_config.resume_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR &= ~OTG_PWR_RSM_MASK;
    }

    if (USB_SET == port_config.soft_connect_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_SFCN_MASK;
    }
    else if (USB_CLEAR == port_config.soft_connect_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR &= ~OTG_PWR_SFCN_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_GetPortStatus()			    				     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the port status.										  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		none.																  		  */
/* OUT : 		t_usb_dev_port_status*:	port_status information.							  */
/* 																			                  */
/* RETURN:		t_usb_error.																  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_GetPortStatus(OUT t_usb_dev_port_status *p_dev_port_status)
{
    DBGENTER0();

    if (NULL == p_dev_port_status)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }        
    
    p_dev_port_status->is_suspend_enabled = ((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_SUSP_MASK) ? TRUE : FALSE;
    p_dev_port_status->is_reset_enabled = ((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_RST_MASK) ? TRUE : FALSE;
    p_dev_port_status->is_overcurrent_enabled = FALSE;
        

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_host_SetPortConfig()		   					     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the port.   										  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_host_port_config:  port configuration.						  		  */
/* OUT : 		none.                                       								  */
/* 																			                  */
/* RETURN:		t_usb_error.																  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_SetPortConfig(IN t_usb_host_port_config port_config)
{
    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    if (USB_SET == port_config.suspend_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_SUSP_MASK;
    }
    else if (USB_CLEAR == port_config.suspend_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_RSM_MASK;
    }

    /*  It is required to clear this bit only after atleast 20ms.  */
    if (USB_SET == port_config.reset_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_RST_MASK;
    }
    else if (USB_CLEAR == port_config.reset_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR &= ~OTG_PWR_RST_MASK;
    }

    if (USB_CLEAR == port_config.port_enable)
    {
        /*  Do nothing.  */
    }

    if (USB_SET == port_config.port_power)
    {
        /*  Do nothing.  */
    }
    else if (USB_CLEAR == port_config.port_power)
    {
        /*  Do nothing.  */
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_host_GetPortStatus()			    			     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the port status.										  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		none.																  		  */
/* OUT : 		t_usb_host_port_status*:	port_status information.						  */
/* 																			                  */
/* RETURN:		t_usb_error.																  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_GetPortStatus(OUT t_usb_host_port_status *p_host_port_status)
{
    t_uint32    status_value = MASK_NULL32;
    DBGENTER0();

    if (NULL == p_host_port_status)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }
        
    
    status_value = (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL;
    p_host_port_status->is_resume_enabled = ((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_RSM_MASK) ? TRUE : FALSE;
    p_host_port_status->is_reset_enabled = ((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_RST_MASK) ? TRUE : FALSE;
    p_host_port_status->is_port_enabled = TRUE;
    p_host_port_status->is_device_connected = g_usb_system_context.is_device_connected;
    p_host_port_status->connected_device_speed = ((status_value & OTG_DEVCTL_LSDEV_MASK) == 0) ? ((status_value & OTG_DEVCTL_FSDEV_MASK) ? USB_FULL_SPEED : USB_HIGH_SPEED) : USB_LOW_SPEED;
        

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PUBLIC t_usb_error  USB_dev_SetOtgConfig()					    			      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to configure the SRP and HNP.							  */
/* PARAMETERS:																                  */
/* IN :        t_usb_dev_otg_config:		SRP and HNP configuration parameter.			  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_SetOtgConfig(IN t_usb_dev_otg_config otg_config)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);

    if (USB_SET == otg_config.srp_capable)
    {
        /*  Do nothing. */
    }
    else if (USB_CLEAR == otg_config.srp_capable)
    {
        /*  Do nothing. */
    }

    if (USB_SET == otg_config.hnp_capable)
    {
        /*  Do nothing. */
    }
    else if (USB_CLEAR == otg_config.hnp_capable)
    {
        /*  Do nothing. */
    }

    if (USB_SET == otg_config.hnp_enable)
    {
        /*  Do nothing. */
    }
    else if (USB_CLEAR == otg_config.hnp_enable)
    {
        /*  Do nothing. */
    }

    if (USB_SET == otg_config.hnp_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL |= OTG_DEVCTL_HREQ_MASK;
    }
    else if (USB_CLEAR == otg_config.hnp_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL &= ~OTG_DEVCTL_HREQ_MASK;
    }

    if (USB_SET == otg_config.session_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL |= OTG_DEVCTL_SESS_MASK;
    }
    else if (USB_CLEAR == otg_config.session_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL &= ~OTG_DEVCTL_SESS_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PUBLIC t_usb_error  USB_dev_GetOtgStatus()			    					      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine returns the current status of SRP and HNP.						  */
/* PARAMETERS:																                  */
/* IN :        none																			  */
/* OUT :       t_usb_dev_otg_status*:	SRP and HNP status.									  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_GetOtgStatus(OUT t_usb_dev_otg_status *p_dev_otg_status)
{
    t_uint32 vbus_level = MASK_NULL32;

    DBGENTER0();
        
    vbus_level = ((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_VBUS_MASK) >> OTG_DEVCTL_VBUS_SHIFT;

    p_dev_otg_status->hnp_success = TRUE;
    p_dev_otg_status->srp_success = TRUE;
    p_dev_otg_status->otg_status.con_mode =
        ((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_BDEV_MASK)
            ? USB_B_DEVICE_MODE : USB_A_DEVICE_MODE;
    p_dev_otg_status->otg_status.device_type = ((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK) ? USB_HOST_TYPE : USB_DEVICE_TYPE;

    switch (vbus_level)
    {
        case 0:
            p_dev_otg_status->otg_status.vbus_level = USB_BELOW_SESSIONEND;
            break;

        case 1:
            p_dev_otg_status->otg_status.vbus_level = USB_ABOVE_SESSIONEND;
            break;

        case 2:
            p_dev_otg_status->otg_status.vbus_level = USB_ABOVE_AVALID;
            break;

        case 3:
            p_dev_otg_status->otg_status.vbus_level = USB_ABOVE_VBUSVALID;
            break;
    }
        

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PUBLIC t_usb_error  USB_host_SetOtgConfig()				    				      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to configure the SRP and HNP.							  */
/* PARAMETERS:																                  */
/* IN :        t_usb_host_otg_config:		SRP and HNP configuration parameter.			  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   t_usb_error:				USB error value. 							          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_SetOtgConfig(IN t_usb_host_otg_config host_otg_config)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);

    if (USB_SET == host_otg_config.srp_capable)
    {
        /*  Do nothing  */
    }
    else if (USB_CLEAR == host_otg_config.srp_capable)
    {
        /*  Do nothing  */
    }

    if (USB_SET == host_otg_config.hnp_capable)
    {
        /*  Do nothing  */
    }
    else if (USB_CLEAR == host_otg_config.hnp_capable)
    {
        /*  Do nothing  */
    }

    if (USB_SET == host_otg_config.hnp_enable)
    {
        /*  Do nothing  */
    }
    else if (USB_CLEAR == host_otg_config.hnp_enable)
    {
        /*  Do nothing  */
    }

    if (USB_SET == host_otg_config.session_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL |= OTG_DEVCTL_SESS_MASK;
    }
    else if (USB_CLEAR == host_otg_config.session_request)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL &= ~OTG_DEVCTL_SESS_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PUBLIC t_usb_error  USB_host_GetOtgStatus()		    						      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine returns the current status of SRP and HNP.						  */
/* PARAMETERS:																                  */
/* IN :        none																			  */
/* OUT :       t_usb_host_otg_status*:	SRP and HNP status.									  */
/* 																			                  */
/* RETURN:	   t_usb_error: USB error value		 		 							          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_GetOtgStatus(OUT t_usb_host_otg_status *p_host_otg_status)
{
    t_uint32 vbus_level = MASK_NULL32;

    DBGENTER0();
        

    vbus_level = ((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_VBUS_MASK) >> OTG_DEVCTL_VBUS_SHIFT;

    p_host_otg_status->hnp_detected = FALSE;
    p_host_otg_status->srp_detected = FALSE;
    p_host_otg_status->is_connectorid_changed = FALSE;

    p_host_otg_status->otg_status.con_mode = ((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_BDEV_MASK) ? USB_B_DEVICE_MODE : USB_A_DEVICE_MODE;
    p_host_otg_status->otg_status.device_type = ((g_usb_system_context.p_usb_0_register)->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK) ? USB_HOST_TYPE : USB_DEVICE_TYPE;

    switch (vbus_level)
    {
        case 0:
            p_host_otg_status->otg_status.vbus_level = USB_BELOW_SESSIONEND;
            break;

        case 1:
            p_host_otg_status->otg_status.vbus_level = USB_ABOVE_SESSIONEND;
            break;

        case 2:
            p_host_otg_status->otg_status.vbus_level = USB_ABOVE_AVALID;
            break;

        case 3:
            p_host_otg_status->otg_status.vbus_level = USB_ABOVE_VBUSVALID;
            break;
    }
        

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_GetEndpStatus()	    									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the IN or OUT endpoint Status register contents.    	  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          t_usb_endp_num:					Identify the endpoint						  */
/* OUT : 		t_usb_dev_endp_status *:		IN/OUT endpoint status			  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_GetEndpStatus(IN t_usb_endp_num endp_num, OUT t_usb_dev_endp_status *p_endp_status)
{
    t_uint16    status_value = MASK_NULL16;
    t_uint8     endp_num_short;

    DBGENTER0();

    if (NULL == p_endp_status)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    *p_endp_status = MASK_NULL32;
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num)
    {
        status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;
        if (status_value & OTG_DCSR0_RRDY_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_OUT_RXPCKRDY;
        }
    }
    else if (USB_ENDPNUM_0_IN == endp_num)
    {
        status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;
        if (status_value & OTG_DCSR0_TRDY_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_IN_TXPCKRDY;
        }
    }
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        status_value = (g_usb_system_context.p_usb_0_register)->OTG_RXCSR;
        if (status_value & OTG_DRXCSR_ERR_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_OUT_STATUS_ERROR;
        }

        if (status_value & OTG_DRXCSR_OR_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_OUT_OVERRUN;
        }

        if (status_value & OTG_DRXCSR_IRX_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_OUT_INCOMPRX;
        }

        if (status_value & OTG_DRXCSR_FFULL_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_OUT_RXFIFOFULL;
        }

        if (status_value & OTG_DRXCSR_RRDY_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_OUT_RXPCKRDY;
        }
    }
    else
    {
        status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

        if (status_value & OTG_DTXCSR_UR_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_IN_UNDERRUN;
        }

        if (status_value & OTG_DTXCSR_ITX_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_IN_INCOMPTX;
        }

        if (status_value & OTG_DTXCSR_FNE_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_IN_FIFONOTEMPTY;
        }

        if (status_value & OTG_DTXCSR_TRDY_MASK)
        {
            *p_endp_status |= (t_uint32) USB_DEV_IN_TXPCKRDY;
        }
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_GetSetupStatus()										      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the SETUP Status register contents. 		   			  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          none																		  */
/* OUT : 		t_usb_setup_status *:			setup status					  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_GetSetupStatus(OUT t_usb_setup_status *p_setup_status)
{
    DBGENTER0();
    if (NULL == p_setup_status)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL8;

    p_setup_status->endp_num = USB_ENDPNUM_0_OUT;
    p_setup_status->is_setup_after_out = FALSE;
    p_setup_status->is_setup_received = ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DCSR0_RRDY_MASK) ? TRUE : FALSE;
    p_setup_status->is_setup_good = TRUE;
    p_setup_status->is_setup_end = ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DCSR0_SETUPEND_MASK) ? TRUE : FALSE;
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_GetSetupData()											      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the SETUP Data register contents. 			   			  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          none																		  */
/* OUT : 		t_usb_setup_request *:			setup data						  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_GetSetupData(OUT t_usb_setup_request *p_request)
{
    t_uint32    xfer_size = MASK_NULL32;

    DBGENTER0();

    if (NULL == p_request)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);

    /*	Get the SETUP Packet data from the Rx FIFO.	*/
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL32;

    xfer_size = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & MASK_BYTE0;

    if (xfer_size < sizeof(t_usb_setup_request))
    {
        usb_ReadFromFifo(MASK_NULL8, (t_uint8 *) p_request, xfer_size);
    }
    else
    {
        usb_ReadFromFifo(MASK_NULL8, (t_uint8 *) p_request, sizeof(t_usb_setup_request));
    }

    /* If data stage is not there, set the DataEnd bit also 
	   Same is Supported in USB_dev_CompleteSetupAndStatusStage & USB_dev_CompleteSetupStage
    */
    /*
    if (0 == p_request->wLength)    
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_DATAEND_MASK | OTG_DCSR0_SERRDY_MASK;
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_SERRDY_MASK;
    }
    */
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_GetSetAddress()											      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine the address of the device. This fn should be called in response  */
/*              to the SET_ADDRESS command.                                                   */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          t_uint8:                        Address value.								  */
/* OUT : 		none                                    						  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_SetAddress(IN t_uint8 address_value)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_FADDR = address_value & OTG_FADDR_MASK;
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_SetEndpointStallState()									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: To Stall the given Endpoint.												  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN : 		t_usb_endp_num:		     	identify the endpoint			  			      */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_SetEndpointStallState(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_SNDSTALL_MASK;
    }
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_SND_MASK;
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_SND_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_bool USB_dev_IsEndpointStalled()											      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: To Get the Stall State of the given Endpoint.								  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN : 		t_usb_endp_num:		     	identify the endpoint			  			      */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_bool :                    TRUE if stalled.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_bool USB_dev_IsEndpointStalled(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;
    t_bool is_endp_stalled = FALSE;

    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        if ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DCSR0_SNTSTALL_MASK)
        {            
            is_endp_stalled = TRUE;            
        }
    }
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        if
        (
            ((g_usb_system_context.p_usb_0_register)->OTG_RXCSR & OTG_DRXCSR_SNT_MASK)
        ||  ((g_usb_system_context.p_usb_0_register)->OTG_RXCSR & OTG_DRXCSR_SND_MASK)
        )
        {            
            is_endp_stalled = TRUE;            
        }
    }
    else
    {
        if
        (
            ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DTXCSR_SNT_MASK)
        ||  ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DTXCSR_SND_MASK)
        )
        {            
            is_endp_stalled = TRUE;
        }
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT1(USB_OK, "%s", "TRUE");
    return(is_endp_stalled);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_ClearEndpointStallState()								      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: To Clear the Stall state of the given Endpoint.								  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN : 		t_usb_endp_num:		     	identify the endpoint			  			      */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_ClearEndpointStallState(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_DCSR0_SNDSTALL_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_DCSR0_SNTSTALL_MASK;
    }
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_SND_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_SNT_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_CLR_MASK;
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_DTXCSR_SND_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_DTXCSR_SNT_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_CLR_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_GetTransferStatus()								          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine returns the current transfer status             				  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	 	Identify the endpoint					     		  */
/* OUT :       t_usb_xfer_status *:		current transfer status.                    		  */
/* 																			                  */
/* RETURN:		t_usb_error:			USB_OK always.	 							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_GetTransferStatus(IN t_usb_endp_num endp_num, OUT t_usb_xfer_status *p_xfer_status)
{
    DBGENTER0();

    if (NULL == p_xfer_status)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }
        
    
    *p_xfer_status = g_usb_system_context.dev_endpoint[endp_num].xfer_status;
        

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_FlushTransferFifo()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine flushes the transfer FIFO corresponding to the endpoint.		  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_endp_num:     Identify the endpoint.									  */
/* OUT : 		none.																		  */
/* 																			                  */
/* RETURN:		t_usb_error:                  												  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_FlushTransferFifo(IN t_usb_endp_num endp_num)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (GET_0_ENDPNUM_SHORT(endp_num) & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = OTG_DCSR0_FFLUSH_MASK;
        if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_FFLUSH_MASK;
        }
    }
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;
        if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;
        }
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = OTG_DTXCSR_FFLU_MASK;
        if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_FFLU_MASK;
        }
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_GetTransferBytes()									          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine returns the number of bytes transmitted or received.			  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	 	Identify the endpoint					     		  */
/* OUT :       t_size *:				The number of bytes transferred.					  */
/* 																			                  */
/* RETURN:		t_usb_error:							 							          */
/*              USB_REQUEST_NOT_APPICABLE: For Iso Transfers.                                 */
/*              USB_INVALID_PARAMETER:         For passing NULL parameters.                   */
/*              USB_OK:                    Otherwise.                                         */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_GetTransferBytes(IN t_usb_endp_num endp_num, OUT t_size *p_bytes)
{
    DBGENTER0();

    if (NULL == p_bytes)
    {
        DBGEXIT0(USB_INVALID_PARAMETER);
        return(USB_INVALID_PARAMETER);
    }
    
    
    if
    (
        (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    &&  (USB_DMA_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
    )
    {
        *p_bytes = g_usb_system_context.dev_endpoint[endp_num].remaining_bytes;
    }
    else
    {
        *p_bytes = g_usb_system_context.dev_endpoint[endp_num].len_transferred;
    }
    

    DBGEXIT1(USB_OK, " bytes = %ld ", *p_bytes);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_OutEndp_IsoTransferInitiate()									  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_OutEndp_IsoTransferInitiate(IN t_usb_endp_num endp_num, INOUT t_usb_dev_xfer_config xfer_config)
{
    t_uint8 endp_num_short;
    t_size  xfer_length = MASK_NULL32;

    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    if (USB_DMA_MODE == xfer_config.xfer_mode)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_ISOUP_MASK;

        if (USB_DMA_REQ_MODE_1 == xfer_config.dma_req_mode)
        {
            if (TRUE == xfer_config.auto_set_or_clear)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |=
                    (
                        OTG_DRXCSR_ISO_MASK |
                        OTG_DRXCSR_DRM_MASK |
                        OTG_DRXCSR_ACLR_MASK |
                        OTG_DRXCSR_DMAR_MASK
                    );
            }
            else
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |=
                    (
                        OTG_DRXCSR_ISO_MASK |
                        OTG_DRXCSR_DRM_MASK |
                        OTG_DRXCSR_DMAR_MASK
                    );
            }

            if
            (
                (
                    g_usb_system_context.dev_endpoint[endp_num].xfer_length %
                    g_usb_system_context.dev_endpoint[endp_num].maxpacketsize
                ) == 0
            )
            {
                g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
            }
        }
        else
        {
            if (TRUE == xfer_config.auto_set_or_clear)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |=
                    (
                        OTG_DRXCSR_ISO_MASK |
                        OTG_DRXCSR_ACLR_MASK |
                        OTG_DRXCSR_DMAR_MASK
                    );
            }
            else
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= (OTG_DRXCSR_ISO_MASK | OTG_DRXCSR_DMAR_MASK);
            }

            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
        }
        (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN |= (1UL << endp_num_short);
    }
    else if (USB_POLLING_MODE == xfer_config.xfer_mode)
    {
        xfer_length = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;

        if (xfer_length > xfer_config.xfer_size)
        {
            xfer_length = xfer_config.xfer_size;
        }

        usb_ReadFromFifo(endp_num_short, xfer_config.p_xfer_buffer, xfer_length);
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_RRDY_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_OutEndp_BulkIntrTransferInitiate()							  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_OutEndp_BulkIntrTransferInitiate
(
    IN t_usb_endp_num           endp_num,
    INOUT t_usb_dev_xfer_config xfer_config
)
{
    t_uint8     endp_num_short;
    t_uint32    status_value = 0;
    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;
    if (USB_ENDPNUM_0_OUT != endp_num)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~(OTG_DRXCSR_ACLR_MASK | OTG_DRXCSR_DMAR_MASK | OTG_DRXCSR_ISO_MASK);

        status_value = (g_usb_system_context.p_usb_0_register)->OTG_RXCSR;

        if (status_value & OTG_DRXCSR_SNT_MASK)
        {
            /* If OTG_DRXCSR_SNT_MASK is set clear this bit */
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_SNT_MASK;
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_OTHER_ERROR;

            /* Flush the FIFO only if OTG_DRXCSR_RRDY_MASK is set else do nothing */
            if (status_value & OTG_DRXCSR_RRDY_MASK)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;

                if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
                {
                    (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;
                }
            }
        }
        else if (status_value & OTG_DRXCSR_OR_MASK)
        {
            /* Clear overrun error */
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_OR_MASK;
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_OVERRUN_ERROR;

            /* Flush the FIFO only if OTG_DRXCSR_RRDY_MASK is set else do nothing */
            if (status_value & OTG_DRXCSR_RRDY_MASK)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;

                if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
                {
                    (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;
                }
            }
        }
        else if (status_value & OTG_DRXCSR_ERR_MASK)
        {
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_OTHER_ERROR;

            /* Flush the FIFO only if OTG_DRXCSR_RRDY_MASK is set else do nothing */
            if (status_value & OTG_DRXCSR_RRDY_MASK)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;

                if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
                {
                    (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK;
                }
            }

            /* Clear CRC error */
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;
        }
        else    /* if (status_value & OTG_DRXCSR_RRDY_MASK) */
        {
            /* Rx Status */
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;

            /* Ping should disable/enable before the transfer starts */
            /* Configure NYET */
            if (FALSE == g_usb_system_context.dev_endpoint[endp_num].enable_nyet)
            {
                if ((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_HSMODE_MASK) /* Do it only if High Speed */
                {
                    (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_DNY_MASK;
                }
            }

            /* Interrupt should be enable before the clearing OTG_DRXCSR_RRDY_MASK otherwise there is a chance of missing interrupt */
            (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN |= (1UL << endp_num_short);

#ifndef __HCL_USB_FB
            /* Flush the fifo if FIFO ready bit is set */
            if (status_value & OTG_DRXCSR_RRDY_MASK)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;

                /* (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_FFLU_MASK; */
            }
#endif
        }
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_OutEndp_BulkDMATransferInitiate()						      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_OutEndp_BulkDMATransferInitiate
(
    IN t_usb_endp_num           endp_num,
    INOUT t_usb_dev_xfer_config xfer_config
)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;

    /*  DMA Request mode 0, and enable DMA request. */
    g_usb_system_context.dev_endpoint[endp_num].remaining_bytes = 0;

    /* Configure NYET */
    if (FALSE == g_usb_system_context.dev_endpoint[endp_num].enable_nyet)
    {
        if ((g_usb_system_context.p_usb_0_register)->OTG_PWR & OTG_PWR_HSMODE_MASK) /* Do it only if High Speed */
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_DNY_MASK;
        }
    }

    /*  DMA Request mode 1 enable */
    if (USB_DMA_REQ_MODE_1 == xfer_config.dma_req_mode)
    {
        if (TRUE == xfer_config.auto_set_or_clear)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |=
                (
                    OTG_DRXCSR_DRM_MASK |
                    OTG_DRXCSR_ACLR_MASK |
                    OTG_DRXCSR_DMAR_MASK
                );
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= (OTG_DRXCSR_DRM_MASK | OTG_DRXCSR_DMAR_MASK);
        }

        if
        (
            (
                g_usb_system_context.dev_endpoint[endp_num].xfer_length %
                g_usb_system_context.dev_endpoint[endp_num].maxpacketsize
            ) == 0
        )
        {
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
        }
    }
    else
    {
        if (TRUE == xfer_config.auto_set_or_clear)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= (OTG_DRXCSR_ACLR_MASK | OTG_DRXCSR_DMAR_MASK);
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_DMAR_MASK;
        }

        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
    }
#ifndef __HCL_USB_FB
    (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN |= (1UL << endp_num_short);
#endif
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_OutEndp_TransferInitiate()									  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_OutEndp_TransferInitiate(IN t_usb_endp_num endp_num, INOUT t_usb_dev_xfer_config xfer_config)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    /*  OUT Transfer    */
    if (USB_ISO_XFER == g_usb_system_context.dev_endpoint[endp_num].endp_type)
    {
        /* Isochronous Transfer Initiate */
        USB_dev_OutEndp_IsoTransferInitiate(endp_num, xfer_config);
    }
    else if (USB_IT_MODE == xfer_config.xfer_mode)
    {
        /* Initiate Bulk/Interrupt Transfer (IT Mode) */
        USB_dev_OutEndp_BulkIntrTransferInitiate(endp_num, xfer_config);
    }
    else if (USB_DMA_MODE == xfer_config.xfer_mode)
    {
        /* Initiate Bulk/Interrupt Transfer (DMA Mode) */
        USB_dev_OutEndp_BulkDMATransferInitiate(endp_num, xfer_config);
    }
    else if (USB_POLLING_MODE == xfer_config.xfer_mode)
    {
        g_usb_system_context.dev_endpoint[endp_num].remaining_bytes = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;

        if (g_usb_system_context.dev_endpoint[endp_num].remaining_bytes > xfer_config.xfer_size)
        {
            g_usb_system_context.dev_endpoint[endp_num].remaining_bytes = xfer_config.xfer_size;
        }

        usb_ReadFromFifo
        (
            endp_num_short,
            xfer_config.p_xfer_buffer,
            g_usb_system_context.dev_endpoint[endp_num].remaining_bytes
        );

        /* The RRDY bit is not cleared here to give the user of HCL the control of this bit in its own hands: For synchronization purpose */
        if (TRUE == xfer_config.auto_set_or_clear)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;
        }

        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
    }

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_InEndp_IsoTransferInitiate()									  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_InEndp_IsoTransferInitiate(IN t_usb_endp_num endp_num, INOUT t_usb_dev_xfer_config xfer_config)
{
    t_uint8     endp_num_short;
    t_uint32    status_value = 0;

    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    if (USB_DMA_MODE == xfer_config.xfer_mode)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_ISOUP_MASK;

        if (TRUE == xfer_config.auto_set_or_clear)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |=
                (
                    OTG_DTXCSR_DMAR_MASK |
                    OTG_DTXCSR_ASET_MASK |
                    OTG_DTXCSR_ISO_MASK
                );
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= (OTG_DTXCSR_DMAR_MASK | OTG_DTXCSR_ISO_MASK);
        }

        status_value = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;

        if (status_value & OTG_DTXCSR_UR_MASK)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= (~OTG_DTXCSR_UR_MASK);
        }

        if (USB_DMA_REQ_MODE_1 == xfer_config.dma_req_mode)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= (OTG_DTXCSR_DRM_MASK);

            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
        }
        else
        {
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;
            (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN |= (1UL << endp_num_short);
        }
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_InEndp_BulkIntrTransferInitiate()							  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_InEndp_BulkIntrTransferInitiate
(
    IN t_usb_endp_num           endp_num,
    INOUT t_usb_dev_xfer_config xfer_config
)
{
    t_uint8 endp_num_short;
    t_size  xfer_length = MASK_NULL32;

    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    if (USB_ENDPNUM_0_IN == endp_num)
    {
        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;

        xfer_length = g_usb_system_context.dev_endpoint[endp_num].maxpacketsize;

        if
        (
            xfer_length >
                (
                    g_usb_system_context.dev_endpoint[endp_num].xfer_length -
                    g_usb_system_context.dev_endpoint[endp_num].len_transferred
                )
        )
        {
            xfer_length =
                (
                    g_usb_system_context.dev_endpoint[endp_num].xfer_length -
                    g_usb_system_context.dev_endpoint[endp_num].len_transferred
                );
        }

        usb_WriteToFifo(endp_num_short, xfer_config.p_xfer_buffer, xfer_length, USB_DEVICE_TYPE);

        g_usb_system_context.dev_endpoint[endp_num].len_transferred += xfer_length;

        if
        (
            g_usb_system_context.dev_endpoint[endp_num].len_transferred == g_usb_system_context.dev_endpoint[
                endp_num].xfer_length
        )
        {
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_WRITING_ENDED;
        }

        /* Set DataEnd bit along with TRDY bit if the Data Stage is over */
        if (USB_WRITING_ENDED == g_usb_system_context.dev_endpoint[endp_num].xfer_status)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_DATAEND_MASK | OTG_DCSR0_TRDY_MASK;
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_TRDY_MASK;
        }
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~(OTG_DTXCSR_ASET_MASK | OTG_DTXCSR_DMAR_MASK | OTG_DTXCSR_ISO_MASK);

        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;

        xfer_length = g_usb_system_context.dev_endpoint[endp_num].maxpacketsize;

        if
        (
            xfer_length >
                (
                    g_usb_system_context.dev_endpoint[endp_num].xfer_length -
                    g_usb_system_context.dev_endpoint[endp_num].len_transferred
                )
        )
        {
            xfer_length =
                (
                    g_usb_system_context.dev_endpoint[endp_num].xfer_length -
                    g_usb_system_context.dev_endpoint[endp_num].len_transferred
                );
        }

        usb_WriteToFifo(endp_num_short, xfer_config.p_xfer_buffer, xfer_length, USB_DEVICE_TYPE);
        g_usb_system_context.dev_endpoint[endp_num].len_transferred += xfer_length;

        if
        (
            g_usb_system_context.dev_endpoint[endp_num].len_transferred == g_usb_system_context.dev_endpoint[
                endp_num].xfer_length
        )
        {
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_WRITING_ENDED;
        }
#ifndef __HCL_USB_FB
        /* Interrupt should be enabled before OTG_DTXCSR_TRDY_MASK is set otherwise we will miss the interrupt */
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN |= (1UL << endp_num_short);
#endif
        /* USB sets OTG_DTXCSR_UR_MASK bit if an IN token is received when the TxPktRdy bit not set. The CPU should clear this bit. */
        if ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DTXCSR_UR_MASK)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_DTXCSR_UR_MASK;
        }
#ifdef __HCL_USB_FB
		 /* Interrupt should be enabled before OTG_DTXCSR_TRDY_MASK is set otherwise we will miss the interrupt */
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN |= (1UL << endp_num_short);
#endif
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_InEndp_BulkDMATransferInitiate()						      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_InEndp_BulkDMATransferInitiate(IN t_usb_endp_num endp_num, INOUT t_usb_dev_xfer_config xfer_config)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;

    if (USB_DMA_REQ_MODE_1 == xfer_config.dma_req_mode)
    {
        if (TRUE == xfer_config.auto_set_or_clear)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |=
                (
                    OTG_DTXCSR_DRM_MASK |
                    OTG_DTXCSR_ASET_MASK |
                    OTG_DTXCSR_DMAR_MASK
                );
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= (OTG_DTXCSR_DRM_MASK | OTG_DTXCSR_DMAR_MASK);
        }
#ifdef __HCL_USB_FB
		if
        (
            (
                g_usb_system_context.dev_endpoint[endp_num].xfer_length %
                g_usb_system_context.dev_endpoint[endp_num].maxpacketsize
            ) == 0
        )
        {
            g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
        }
#else
        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
#endif
    }
    else
    {
        if (TRUE == xfer_config.auto_set_or_clear)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= (OTG_DTXCSR_ASET_MASK | OTG_DTXCSR_DMAR_MASK);
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_DMAR_MASK;
        }
#ifdef __HCL_USB_FB
         g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED; 
#endif
    }
#ifndef __HCL_USB_FB
    if (xfer_config.xfer_size < g_usb_system_context.dev_endpoint[endp_num].maxpacketsize)
    {
        /* (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK; */
        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
    }
    else
    {
        g_usb_system_context.dev_endpoint[endp_num].len_transferred += g_usb_system_context.dev_endpoint[endp_num].maxpacketsize;
        if (USB_DMA_REQ_MODE_1 != xfer_config.dma_req_mode)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN |= (1UL << endp_num_short);
	}
    }
#endif
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_InEndp_TransferInitiate()									  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:	   none.							 							                  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_dev_InEndp_TransferInitiate(IN t_usb_endp_num endp_num, INOUT t_usb_dev_xfer_config xfer_config)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    /*  IN Transfer     */
    if (USB_ISO_XFER == g_usb_system_context.dev_endpoint[endp_num].endp_type)
    {
        /* Isochronous Transfer Initiate */
        USB_dev_InEndp_IsoTransferInitiate(endp_num, xfer_config);
    }

    if (USB_IT_MODE == xfer_config.xfer_mode)
    {
        /* Bulk/Interrupt Transfer(Interrupt Mode) Transfer Initiate */
        USB_dev_InEndp_BulkIntrTransferInitiate(endp_num, xfer_config);
    }
    else if (USB_DMA_MODE == xfer_config.xfer_mode)
    {
        /* Bulk/Interrupt Transfer(DMA Mode) Transfer Initiate */
        USB_dev_InEndp_BulkDMATransferInitiate(endp_num, xfer_config);
    }
    else if (USB_POLLING_MODE == xfer_config.xfer_mode)
    {
        usb_WriteToFifo(endp_num_short, xfer_config.p_xfer_buffer, xfer_config.xfer_size, USB_DEVICE_TYPE);
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
    }

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error  USB_dev_TransferInitiate()									          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine starts the reception / transmission for all four types of    	  */
/*             transfer in one of IT or POLLING or DMA mode.                                  */
/* PARAMETERS:																                  */
/* IN :        t_usb_endp_num:   	Identify the endpoint					     		      */
/* INOUT :	   t_usb_dev_xfer_config:   Transfer configuration parameter.              		  */
/* OUT :       none.																		  */
/* 																			                  */
/* RETURN:		t_usb_error:							 							          */
/*              USB_REQUEST_NOT_APPLICABLE: If the status is USB_TRANSFER_ON_GOING            */
/*                                          or If the endpoint is not configured              */
/*                                          or size is more than 1024 bytes.                  */
/*              USB_OK:                     Otherwise.                                        */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_TransferInitiate(IN t_usb_endp_num endp_num, INOUT t_usb_dev_xfer_config xfer_config)
{
    t_uint8 endp_num_short;

    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    if (g_usb_system_context.dev_endpoint[endp_num].xfer_status == USB_TRANSFER_UNDERRUN_ERROR)
    {
        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
    }

    if
    (
        USB_TRANSFER_ENDED != g_usb_system_context.dev_endpoint[endp_num].xfer_status
    ||  FALSE == g_usb_system_context.dev_endpoint[endp_num].is_endpoint_configured
    )
    {
        HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);       
        DBGEXIT0(USB_REQUEST_NOT_APPLICABLE);
        return(USB_REQUEST_NOT_APPLICABLE);
    }

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    /*	Initialize all the transfer related details.	*/
    g_usb_system_context.dev_endpoint[endp_num].p_xfer_buffer = xfer_config.p_xfer_buffer;
    g_usb_system_context.dev_endpoint[endp_num].xfer_length = xfer_config.xfer_size;
    g_usb_system_context.dev_endpoint[endp_num].len_transferred = 0;
    g_usb_system_context.dev_endpoint[endp_num].xfer_mode = xfer_config.xfer_mode;
    g_usb_system_context.dev_endpoint[endp_num].send_zero_byte = xfer_config.send_zero_byte;
    g_usb_system_context.dev_endpoint[endp_num].dma_req_mode = xfer_config.dma_req_mode;
    g_usb_system_context.dev_endpoint[endp_num].auto_set_or_clear = xfer_config.auto_set_or_clear;
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        /*  OUT Transfer    */
        USB_dev_OutEndp_TransferInitiate(endp_num, xfer_config);
    }
    else
    {   /*  IN Transfer     */
        USB_dev_InEndp_TransferInitiate(endp_num, xfer_config);
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_TransferCancel()										      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: To cancel the transfer operation											  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN : 		t_usb_endp_num:		     		Identify the endpoint			  		      */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_TransferCancel(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ENDED;
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        if (USB_TRANSFER_ENDED != g_usb_system_context.dev_endpoint[endp_num].xfer_status)
        {
            if (USB_ENDPNUM_0_OUT == endp_num)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_SERRDY_MASK;
            }
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = OTG_DCSR0_FFLUSH_MASK;

            if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = OTG_DCSR0_FFLUSH_MASK;
            }
        }
    }
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        if (USB_DMA_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_DMAR_MASK;
        }
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR = OTG_DRXCSR_FFLU_MASK;

        if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR = OTG_DRXCSR_FFLU_MASK;
        }
    }
    else
    {
        if (USB_DMA_MODE == g_usb_system_context.dev_endpoint[endp_num].xfer_mode)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_DTXCSR_DMAR_MASK;
        }
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = OTG_DTXCSR_FFLU_MASK;

        if (TRUE == g_usb_system_context.dev_endpoint[endp_num].enable_double_buffer)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = OTG_DTXCSR_FFLU_MASK;
        }
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_SetupIsoTransfer()										      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: To routine intiates the ISO transfer.										  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN : 		t_usb_endp_num:		     		Identify the endpoint			  		      */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void USB_dev_SetupIsoTransfer(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        /*  Do nothing. */
    }
    else
    {
        //(g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_UR_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= (~OTG_DTXCSR_UR_MASK);

        //(g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
        g_usb_system_context.dev_endpoint[endp_num].xfer_status = USB_TRANSFER_ON_GOING;
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN |= (1UL << endp_num_short);
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_CompleteControlStatus()									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: To routine completes the Status state of the control transfer and resets to   */
/*              setup state.                                                                  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN : 		t_usb_endp_num:		     		Identify the endpoint			  		      */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void USB_dev_CompleteControlStatus(void)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL32;

    if ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DCSR0_SETUPEND_MASK)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_SERSETEND_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_CompleteControlData()									      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: To routine completes the data stage of the control transfer and resets to     */
/*              status stage.                                                                 */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN : 		t_usb_endp_num:		     		Identify the endpoint			  		      */
/* OUT:         none																		  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void USB_dev_CompleteControlData(IN t_usb_endp_num endp_num)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL32;
    (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_DATAEND_MASK;
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_host_SetConfiguration()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine the initializes the host configurations.	 					  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_host_config:			configuration input for the host initialization.  */
/* OUT : 		none.																		  */
/* 																			                  */
/* RETURN:		t_usb_error:                  												  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_SetConfiguration(IN t_usb_host_config *host_config)
{
    t_uint8     fifo_size=USB_INVALID_FIFO_SIZE;
    t_uint32    i;
    t_uint32    offset = 0;

    DBGENTER1
    (
        " %s ",
        (USB_LOW_SPEED == host_config->speed) ? "LOW_SPEED" :
            ((USB_FULL_SPEED == host_config->speed) ? "FULL_SPEED" : "HIGH_SPEED")
    );

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    /*
    	Initialize all the registers and the global variables.
    */
    g_usb_system_context.is_device_connected = FALSE;
    g_usb_system_context.usb_event.system = MASK_NULL32;
    g_usb_system_context.usb_event.endp = MASK_NULL32;
    (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_ENSUSPM_MASK;

    if (USB_HIGH_SPEED == host_config->speed)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR |= OTG_PWR_HSEN_MASK;
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_PWR &= ~OTG_PWR_HSEN_MASK;
    }

    /*	Initialize all the Tx and Rx FIFOs	*/
    for (i = 0; i < (USB_MAX_NUM_ENDPOINTS / 2); i++)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
	
	fifo_size = usb_GetFifoSize(host_config->fifoconfig[i].fifosize);
	if(USB_INVALID_FIFO_SIZE == fifo_size)
	{
            DBGEXIT0(USB_INVALID_PARAMETER);
            return(USB_INVALID_PARAMETER);
	}
        
	(g_usb_system_context.p_usb_0_register)->OTG_RXFSZ = fifo_size;
        (g_usb_system_context.p_usb_0_register)->OTG_RXFA = (t_uint16) (USB_DEV_0_GET_FIFO_OFFSET_VALUE(offset));
        g_usb_system_context.host_endpoint[i].enable_double_buffer = host_config->fifoconfig[i].enable_double_buffer;
	
        offset += (t_uint32) host_config->fifoconfig[i].fifosize;
        if (TRUE == host_config->fifoconfig[i].enable_double_buffer)
        {
            offset += (t_uint16) host_config->fifoconfig[i].fifosize;
            (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ |= OTG_FIFOSZ_DPB_MASK;
        }

        fifo_size = usb_GetFifoSize(host_config->fifoconfig[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize);	
	if(USB_INVALID_FIFO_SIZE == fifo_size)
	{
            DBGEXIT0(USB_INVALID_PARAMETER);
            return(USB_INVALID_PARAMETER);
	}

	(g_usb_system_context.p_usb_0_register)->OTG_TXFSZ = fifo_size;
        (g_usb_system_context.p_usb_0_register)->OTG_TXFA = (t_uint16) (USB_DEV_0_GET_FIFO_OFFSET_VALUE(offset));
        g_usb_system_context.host_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].enable_double_buffer = 
		                                   host_config->fifoconfig[i + (USB_MAX_NUM_ENDPOINTS / 2)].enable_double_buffer;

        offset += (t_uint32) host_config->fifoconfig[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize;

        if (TRUE == host_config->fifoconfig[i + (USB_MAX_NUM_ENDPOINTS / 2)].enable_double_buffer)
        {
            offset += (t_uint16) host_config->fifoconfig[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize;
            (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ |= OTG_FIFOSZ_DPB_MASK;
        }
    }

    i = (g_usb_system_context.p_usb_0_register)->OTG_INTUSB;  /*Used to clear the interrupts*/
	/*	Enable all the relevant system interrupts.	*/
    (g_usb_system_context.p_usb_0_register)->OTG_INTUSBEN =
        (
            OTG_INTUSB_VBE_MASK |
            OTG_INTUSB_SREQ_MASK |
            OTG_INTUSB_DISCON_MASK |
            OTG_INTUSB_CONN_MASK |
            OTG_INTUSB_RES_MASK
        );

    /*	Disable all the Tx and Rx interrupts.	*/
    i = (g_usb_system_context.p_usb_0_register)->OTG_INTTX;			/*Read the previous interrupts to clear interrupt*/
	
    (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN = MASK_NULL16;

    i = (g_usb_system_context.p_usb_0_register)->OTG_INTRX;			/*Read the previous interrupts to clear interrupt*/
	
    (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN = MASK_NULL16;

    /*	After initialization of the host, start the session.	*/
    (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL |= OTG_DEVCTL_SESS_MASK;
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_host_FlushTransferFifo()						     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine flushes both Tx and Rx FIFOs.				 					  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_endp_num:		     		Identify the endpoint			  		      */
/* OUT : 		none.																		  */
/* 																			                  */
/* RETURN:		t_usb_error:                  												  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_FlushTransferFifo(IN t_usb_endp_num endp_num)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (GET_0_ENDPNUM_SHORT(endp_num) & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	}
    }
    else if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {   /*  OUT Transfer    */
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HTXCSR_FFLU_MASK;
	if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HTXCSR_FFLU_MASK;
	}
    }
    else
    {   /*  IN Transfer    */
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_FFLU_MASK;
	if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_FFLU_MASK;
	}
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	void USB_host_Endp0_TransferInitiate()							     	          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initiates the transfer operation 							      */
/*																		                  	  */
/* PARAMETERS:																                  */
/* INOUT : 		t_uint8*:		transfer buffer.										 	  */
/* IN :			t_size:		size of transfer.												  */
/*				t_usb_host_xfer_config:			transfer configuration.						  */
/* OUT : 		none                                                                	 	  */
/* 																			                  */
/* RETURN:		none                                                                          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_host_Endp0_TransferInitiate
(
    INOUT t_uint8               *p_buffer,
    IN t_size                   size,
    IN t_usb_host_xfer_config   *p_xfer_config,
    IN t_uint32                 speed_mask,
    IN t_uint8                  endp_num_short
)
{
    t_size  xfer_length = MASK_NULL32;

    /* Disable/Enable PING Tokens in Status and Data phase of Control Transfer */
    if (TRUE == p_xfer_config->ping_disable)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_DISPING;
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_DISPING;
    }

    /*	Update the Tx Type and Tx Interval registers.	*/
    g_usb_system_context.last_transferred_token = p_xfer_config->token_type;
    (g_usb_system_context.p_usb_0_register)->OTG_TYPE0_TXTYPE = (t_uint8) ((t_uint32) speed_mask << OTG_TXRXTYPE_SPEED_SHIFT) & OTG_TXRXTYPE_SPEED_MASK;
    (g_usb_system_context.p_usb_0_register)->OTG_NAKLMT0_TXINTV = (MASK_BIT0 & OTG_TXRXINTV_MASK);

    /* Note: FlushFIFO should only be used when TxPktRdy/RxPktRdy is set. At other times, it may
     cause data to be corrupted. */
    if
    (
        ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_HCSR0_RRDY_MASK)
    ||  ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_HCSR0_TRDY_MASK)
    )
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	if(g_usb_system_context.host_endpoint[p_xfer_config->endp_num].enable_double_buffer)
	{
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	}
    }

    /*	For OUT or Setup packets, fill the Tx FIFO with the data.	*/
    if (USB_HOST_SETUP == p_xfer_config->token_type || USB_HOST_OUT == p_xfer_config->token_type)
    {
        xfer_length = size;

        if (xfer_length > p_xfer_config->maxpacketsize)
        {
            xfer_length = p_xfer_config->maxpacketsize;
        }

        if (xfer_length > 0)
        {
            usb_WriteToFifo(endp_num_short, p_buffer, xfer_length, USB_HOST_TYPE);
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_STATUSPKT_MASK;
        }
    }

    if (USB_DATA0 == p_xfer_config->data_toggle)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_DTWEN_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_DT_MASK;
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_DTWEN_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_DT_MASK;
    }

    /*	Finally, update the CSR0 register about the token information.	*/
    if (USB_HOST_SETUP == p_xfer_config->token_type)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= (OTG_HCSR0_SETUPPKT_MASK | OTG_HCSR0_TRDY_MASK);
    }
    else if (USB_HOST_IN == p_xfer_config->token_type)
    {
        g_usb_system_context.host_endpoint[p_xfer_config->endp_num].p_rx_buffer = p_buffer;
        g_usb_system_context.host_endpoint[p_xfer_config->endp_num].rx_length = size;
        g_usb_system_context.host_endpoint[p_xfer_config->endp_num].rx_mode = p_xfer_config->xfer_mode;
        xfer_length = size;
        if (0 == xfer_length)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |=
                (
                    OTG_HCSR0_REQPKT_MASK |
                    OTG_HCSR0_STATUSPKT_MASK
                );
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_REQPKT_MASK;
        }
    }
    else
    {
        xfer_length = size;
        if (xfer_length == 0)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= (OTG_HCSR0_TRDY_MASK | OTG_HCSR0_STATUSPKT_MASK);
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_TRDY_MASK;
        }
    }
    (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN |= (1UL << endp_num_short);
}

/**********************************************************************************************/
/* NAME:	void USB_host_Out_TransferInitiate()							     	          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initiates the transfer operation 							      */
/*																		                  	  */
/* PARAMETERS:																                  */
/* INOUT : 		t_uint8*:		transfer buffer.										 	  */
/* IN :			t_size:		size of transfer.												  */
/*				t_usb_host_xfer_config:			transfer configuration.						  */
/* OUT : 		none                                                                	 	  */
/* 																			                  */
/* RETURN:		none                                                                          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_host_Out_TransferInitiate
(
    INOUT t_uint8               *p_buffer,
    IN t_size                   size,
    IN t_usb_host_xfer_config   *p_xfer_config,
    IN t_uint32                 speed_mask,
    IN t_uint8                  endp_num_short
)
{
    t_size  xfer_length = MASK_NULL32;

    /*	Update the Tx Type and Tx Interval registers.	*/
    (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP = p_xfer_config->maxpacketsize & OTG_MAXP_MASK;
    (g_usb_system_context.p_usb_0_register)->OTG_TYPE0_TXTYPE = (t_uint8)
        (
            (((t_uint32) speed_mask << OTG_TXRXTYPE_SPEED_SHIFT) & OTG_TXRXTYPE_SPEED_MASK) |
                (((t_uint32) p_xfer_config->xfer_type << OTG_TXRXTYPE_PROT_SHIFT) & OTG_TXRXTYPE_PROT_MASK) |
                    (((t_uint32) p_xfer_config->target_ep_number << OTG_TXRXTYPE_EP_SHIFT) & OTG_TXRXTYPE_EP_MASK)
        );
    (g_usb_system_context.p_usb_0_register)->OTG_NAKLMT0_TXINTV = (p_xfer_config->iso_int_xfer_interval & OTG_TXRXINTV_MASK);

    if (TRUE == p_xfer_config->auto_set_clear)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HTXCSR_ASET_MASK;
    }
    else
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~(OTG_HTXCSR_ASET_MASK);
    }


    if (USB_DMA_MODE == p_xfer_config->xfer_mode)
    {
        if (USB_DMA_REQ_MODE_0 == p_xfer_config->dma_req_mode)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~(OTG_HTXCSR_DRM_MASK);
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HTXCSR_DRM_MASK;
        }
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HTXCSR_DMAR_MASK;
    }
    else
    {
        xfer_length = size;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_DMAR_MASK;

        if (xfer_length > p_xfer_config->maxpacketsize)
        {
            xfer_length = p_xfer_config->maxpacketsize;
        }

        usb_WriteToFifo(endp_num_short, p_buffer, xfer_length, USB_HOST_TYPE);
    }

    /* if (USB_ISO_XFER != p_xfer_config->xfer_type)
    {*/

    (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN |= (1UL << endp_num_short);

    if (USB_DMA_MODE != p_xfer_config->xfer_mode)
    {
        /*	Finally, update the TxCSR register about the token information.	*/
        if (USB_HOST_SETUP == p_xfer_config->token_type)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= (OTG_HTXCSR_STP_MASK | OTG_HTXCSR_TRDY_MASK);
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HTXCSR_TRDY_MASK;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_host_In_TransferInitiate()							     	          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initiates the transfer operation 							      */
/*																		                  	  */
/* PARAMETERS:																                  */
/* INOUT : 		t_uint8*:		transfer buffer.										 	  */
/* IN :			t_size:		size of transfer.												  */
/*				t_usb_host_xfer_config:			transfer configuration.						  */
/* OUT : 		none                                                                	 	  */
/* 																			                  */
/* RETURN:		none                                                                          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_host_In_TransferInitiate
(
    INOUT t_uint8               *p_buffer,
    IN t_size                   size,
    IN t_usb_host_xfer_config   *p_xfer_config,
    IN t_uint32                 speed_mask,
    IN t_uint8                  endp_num_short
)
{
    t_size  xfer_length = MASK_NULL32;

    if (USB_POLLING_MODE == p_xfer_config->xfer_mode)
    {
        if ((g_usb_system_context.p_usb_0_register)->OTG_RXCSR & OTG_HRXCSR_RRDY_MASK)
        {
            xfer_length = (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;
            g_usb_system_context.host_endpoint[p_xfer_config->endp_num].p_rx_buffer = p_buffer;
            if (size < xfer_length)
            {
                xfer_length = size;
            }

            g_usb_system_context.host_endpoint[p_xfer_config->endp_num].rx_length = size;
            g_usb_system_context.host_endpoint[p_xfer_config->endp_num].rx_mode = p_xfer_config->xfer_mode;
            (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_ARQ_MASK;

            if (0 == xfer_length)
            {
            }
            else
            {
                /*	Unload the received packet from Rx FIFO.	*/
                usb_ReadFromFifo
                (
                    endp_num_short,
                    g_usb_system_context.host_endpoint[p_xfer_config->endp_num].p_rx_buffer,
                    size
                );

                g_usb_system_context.host_endpoint[p_xfer_config->endp_num].xfer_status = USB_HOST_ACK;
            }

            /* g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_RRDY_MASK; */
        }
    }
    else
    {
        /*	Update the Rx Type and Rx Interval registers.	*/
        g_usb_system_context.host_endpoint[p_xfer_config->endp_num].p_rx_buffer = p_buffer;
        g_usb_system_context.host_endpoint[p_xfer_config->endp_num].rx_length = size;
        g_usb_system_context.host_endpoint[p_xfer_config->endp_num].rx_mode = p_xfer_config->xfer_mode;
        (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP = p_xfer_config->maxpacketsize & OTG_MAXP_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXTYPE = (t_uint8)
            (
                (((t_uint32) speed_mask << OTG_TXRXTYPE_SPEED_SHIFT) & OTG_TXRXTYPE_SPEED_MASK) |
                    (((t_uint32) p_xfer_config->xfer_type << OTG_TXRXTYPE_PROT_SHIFT) & OTG_TXRXTYPE_PROT_MASK) |
                        (((t_uint32) p_xfer_config->target_ep_number << OTG_TXRXTYPE_EP_SHIFT) & OTG_TXRXTYPE_EP_MASK)
            );
        (g_usb_system_context.p_usb_0_register)->OTG_RXINTV = (p_xfer_config->iso_int_xfer_interval & OTG_TXRXINTV_MASK);

        /*	Finally, update the RxCSR register about the token information.	*/
        if (USB_DMA_MODE == p_xfer_config->xfer_mode)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_DMAR_MASK;

            if (USB_DMA_REQ_MODE_0 == p_xfer_config->dma_req_mode)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~(OTG_HRXCSR_DRM_MASK);
            }
            else
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_DRM_MASK;
            }

            if (TRUE == p_xfer_config->auto_set_clear)
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= (OTG_HRXCSR_ASET_MASK | OTG_HRXCSR_ARQ_MASK);
            }
            else
            {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~(OTG_HRXCSR_ARQ_MASK);
            }
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= (~OTG_HRXCSR_DMAR_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~(OTG_HRXCSR_ARQ_MASK);
        }

        /* No need to flush the fifo if RRDY is not set, will result in error. */
        if ((g_usb_system_context.p_usb_0_register)->OTG_RXCSR & OTG_HRXCSR_RRDY_MASK)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_FFLU_MASK;
            if(g_usb_system_context.host_endpoint[p_xfer_config->endp_num].enable_double_buffer)
	    {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_FFLU_MASK;
	    }
	}

        /* We are not supposed to set this bit. Data transfer fails some time. */
        /* (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_RRDY_MASK; */
        /* (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_ASET_MASK; */
        /*if (USB_ISO_XFER != p_xfer_config->xfer_type)
        {*/
            
	(g_usb_system_context.p_usb_0_register)->OTG_INTRXEN |= (1UL << endp_num_short);
    	(g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_RPK_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_host_TransferInitiate()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initiates the transfer operation 							      */
/*																		                  	  */
/* PARAMETERS:																                  */
/* INOUT : 		t_uint8*:		transfer buffer.										 	  */
/* IN :			t_size:		size of transfer.												  */
/*				t_usb_host_xfer_config:			transfer configuration.						  */
/* OUT : 		none                                                                	 	  */
/* 																			                  */
/* RETURN:		t_usb_error:                  												  */
/*				Possible error values are:													  */
/*				USB_INVALID_PARAMETER:				If buffer  is NULL.		            	  */
/*				USB_OK:							If everything's fine.						  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_TransferInitiate
(
    INOUT t_uint8               *p_buffer,
    IN t_size                   size,
    IN t_usb_host_xfer_config   xfer_config
)
{
    t_uint32    speed_mask = 0;
    t_uint8     endp_num_short;

    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    endp_num_short = GET_0_ENDPNUM_SHORT(xfer_config.endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    switch (xfer_config.speed)
    {
        case USB_HIGH_SPEED:
            speed_mask = MASK_BIT0;
            break;

        case USB_FULL_SPEED:
            speed_mask = MASK_BIT1;
            break;

        case USB_LOW_SPEED:
            speed_mask = (MASK_BIT0 | MASK_BIT1);
            break;
    }

    /*	Update the Function's address.	*/
    usb_host_UpdateFunctionDetails
    (
        xfer_config.token_addr,
        xfer_config.hub_addr,
        xfer_config.hub_port,
        xfer_config.endp_num
    );

    /*	For EP0.	*/
    if (USB_ENDPNUM_0_OUT == xfer_config.endp_num || USB_ENDPNUM_0_IN == xfer_config.endp_num)
    {
        /* Initiate Host Endp0 Transfer */
        USB_host_Endp0_TransferInitiate(p_buffer, size, &xfer_config, speed_mask, endp_num_short);
    }
    else if (USB_HOST_OUT == xfer_config.token_type || USB_HOST_SETUP == xfer_config.token_type)
    {
        /* Initiate Host Out Transfer */
        USB_host_Out_TransferInitiate(p_buffer, size, &xfer_config, speed_mask, endp_num_short);
    }
    else    /*	USB_HOST_IN	token.	*/
    {
        /* Initiate Host In Transfer */
        USB_host_In_TransferInitiate(p_buffer, size, &xfer_config, speed_mask, endp_num_short);
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_host_GetLastTransferStatus()					     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the status of the last transfer.						  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		none															          	  */
/* OUT : 		t_usb_host_xfer_status*:		    The transfer status of the last transfer. */
/* 				t_size*:							The number of bytes transferred.		  */
/* 																			                  */
/* RETURN:		t_usb_error :                  USB error code					          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_host_GetLastTransferStatus
(
    IN t_usb_endp_num           endp_num,
    OUT t_usb_host_xfer_status  *p_xfer_status,
    OUT t_size                  *p_size
)
{
    DBGENTER0();
        
    
    if (NULL != p_xfer_status)
    {
        *p_xfer_status = g_usb_system_context.host_endpoint[endp_num].xfer_status;
    }

    if (NULL != p_size)
    {
        *p_size = g_usb_system_context.host_endpoint[endp_num].rx_length;
    }
        

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	PUBLIC void  USB_UpdateTransceiver()	        							      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine Updates the I2C register.               						  */
/* PARAMETERS:																                  */
/*             t_uint8 reg:             register address.									  */
/*             t_uint8 value:           data value.                                           */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void USB_UpdateTransceiver(IN t_uint8 reg, IN t_uint8 value)
{
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
#ifdef ST_8500ED	
    if (value & USB_0_XCEIVER_ULPI_ENABLE)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL |= MASK_BIT0;
    }
    else if (value & USB_0_XCEIVER_ULPI_DISABLE)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL &= ~MASK_BIT0;
    }

    if (value & USB_0_XCEIVER_ULPI_DDR_ENABLE)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL |= MASK_BIT1;
    }
    else if (value & USB_0_XCEIVER_ULPI_DDR_DISABLE)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL &= ~MASK_BIT1;
    }

    if (value & USB_0_XCEIVER_XCLK_ENABLE)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL |= MASK_BIT3;
    }
    else if (value & USB_0_XCEIVER_XCLK_DISABLE)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL &= ~MASK_BIT3;
    }
#endif
  
    if (value & USB_0_XCEIVER_SRST)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_SOFTRST |= (MASK_BIT1 | MASK_BIT0);
    }

#ifdef ST_8500ED	
    if (value & USB_0_XCEIVER_I2C_OFF)
    {
        (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL |= MASK_BIT4;
    }
#endif
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
}
#ifndef USB_FIFO_MGMT
/**********************************************************************************************/
/* NAME:	PUBLIC void  usb_UpdateFifos()	                   							      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine Updates the FIFO registers.               						  */
/* PARAMETERS:																                  */
/* OUT :                                                									  */
/* 																			                  */
/* RETURN:		none.                                       						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE t_usb_error usb_UpdateFifos(void)
{
    t_uint8     fifo_size=USB_INVALID_FIFO_SIZE;
    t_size      i;
    t_uint16    offset = USB_EP0_DEFAULT_FIFOSIZE * 2;  /* Because of double buffering, you need to multiply by 2. */

    for (i = 1; i < (USB_MAX_NUM_ENDPOINTS / 2); i++)
    {
        if (TRUE == g_usb_system_context.dev_endpoint[i].is_endpoint_configured)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP = g_usb_system_context.dev_endpoint[i].maxpacketsize & OTG_MAXP_MASK;

	    fifo_size=usb_GetFifoSize(g_usb_system_context.dev_endpoint[i].fifosize);
	    if(USB_INVALID_FIFO_SIZE == fifo_size)
	    {
                DBGEXIT0(USB_INVALID_PARAMETER);
                return(USB_INVALID_PARAMETER);
	    }
            (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ = fifo_size;
            (g_usb_system_context.p_usb_0_register)->OTG_TXFA = (t_uint16) (USB_DEV_0_GET_FIFO_OFFSET_VALUE(offset));

            offset += (t_uint16) g_usb_system_context.dev_endpoint[i].fifosize;

            if (TRUE == g_usb_system_context.dev_endpoint[i].enable_double_buffer)
            {
                offset += (t_uint16) g_usb_system_context.dev_endpoint[i].fifosize;
                (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ |= OTG_FIFOSZ_DPB_MASK;
            }
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_TXFA = NULL;
        }

        if (TRUE == g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].is_endpoint_configured)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP = g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].maxpacketsize & OTG_MAXP_MASK;
	    fifo_size = usb_GetFifoSize(g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize) ;
	    if(USB_INVALID_FIFO_SIZE == fifo_size)
	    {
                DBGEXIT0(USB_INVALID_PARAMETER);
                return(USB_INVALID_PARAMETER);
	    }
            (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ = fifo_size;
            (g_usb_system_context.p_usb_0_register)->OTG_RXFA = (t_uint16) (USB_DEV_0_GET_FIFO_OFFSET_VALUE(offset));

            offset += (t_uint16) g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize;

            if (TRUE == g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].enable_double_buffer)
            {
                offset += (t_uint16) g_usb_system_context.dev_endpoint[i + (USB_MAX_NUM_ENDPOINTS / 2)].fifosize;
                (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ |= OTG_FIFOSZ_DPB_MASK;
            }
        }
        else
        {
            (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (i & OTG_ENDPNO_INDX_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ = 0;
            (g_usb_system_context.p_usb_0_register)->OTG_RXFA = NULL;
        }
    }

    DBGEXIT0(USB_OK);
    return(USB_OK);
    
}
#endif
/**********************************************************************************************/
/* NAME:	PUBLIC void  usb_GetFifoSize()	                   							      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine maps the FIFO size to the appropriate register value.             */
/* PARAMETERS:																                  */
/*              t_usb_fifo_size:         FIFO Size enum.                                      */
/* OUT :        t_uint16:                register value of the fifo size          	          */
/* 																			                  */
/* RETURN:		none.                                       						          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE t_uint8 usb_GetFifoSize(IN t_usb_fifo_size fifosize)
{
    t_uint8    ret_val = USB_INVALID_FIFO_SIZE;

    switch (fifosize)
    {
        case USB_FIFO_SIZE_8:
            ret_val = 0;
            break;

        case USB_FIFO_SIZE_16:
            ret_val = 1;
            break;

        case USB_FIFO_SIZE_32:
            ret_val = 2;
            break;

        case USB_FIFO_SIZE_64:
            ret_val = 3;
            break;

        case USB_FIFO_SIZE_128:
            ret_val = 4;
            break;

        case USB_FIFO_SIZE_256:
            ret_val = 5;
            break;

        case USB_FIFO_SIZE_512:
            ret_val = 6;
            break;

        case USB_FIFO_SIZE_1024:
            ret_val = 7;
            break;

        case USB_FIFO_SIZE_2048:
            ret_val = 8;
            break;

        case USB_FIFO_SIZE_4096:
            ret_val = 9;
            break;
    }

    DBGEXIT0(USB_OK);
    return(ret_val);
}

/************************************************************************************************/
/* NAME:	PUBLIC void  usb_host_Out_Endp_UpdateFunctionDetails()	            				*/
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine updates the functions details like hub and function addresses.      */
/* PARAMETERS:											                                        */
/*              token_addr:              Function address.                                    	*/
/*              hub_addr:                Hub address.                                         	*/
/*              hub_port:                Hub port.                                            	*/
/*              endp_num:                Endpoint number.                                     	*/
/* OUT :        t_uint16:                register value of the fifo size          	          	*/
/* 												                                                */
/* RETURN:		none.                                       						            */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PRIVATE void usb_host_Out_Endp_UpdateFunctionDetails
(
    IN t_uint8          token_addr,
    IN t_uint8          hub_addr,
    IN t_uint8          hub_port,
    IN t_usb_endp_num   endp_num
)
{
    switch (endp_num)
    {
        case USB_ENDPNUM_0_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX0FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX0HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX0HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_1_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX1FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX1HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX1HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_2_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX2FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX2HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX2HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_3_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX3FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX3HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX3HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_4_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX4FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX4HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX4HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_5_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX5FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX5HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX5HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_6_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX6FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX6HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX6HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_7_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX7FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX7HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX7HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_8_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX8FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX8HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX8HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_9_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX9FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX9HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX9HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_10_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX10FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX10HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX10HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_11_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX11FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX11HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX11HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_12_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX12FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX12HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX12HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_13_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX13FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX13HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX13HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_14_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX14FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX14HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX14HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_15_OUT:
            (g_usb_system_context.p_usb_0_register)->OTG_TX15FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX15HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_TX15HP = (hub_port & OTG_HP_MASK);
            break;
    }
}

/************************************************************************************************/
/* NAME:	PUBLIC void  usb_host_In_Endp_UpdateFunctionDetails()	            				*/
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine updates the functions details like hub and function addresses.    	*/
/* PARAMETERS:											                                        */
/*              token_addr:              Function address.                                    	*/
/*              hub_addr:                Hub address.                                         	*/
/*              hub_port:                Hub port.                                            	*/
/*              endp_num:                Endpoint number.                                     	*/
/* OUT :        t_uint16:                register value of the fifo size          	          	*/
/* 												                                                */
/* RETURN:		none.                                       						            */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PRIVATE void usb_host_In_Endp_UpdateFunctionDetails
(
    IN t_uint8          token_addr,
    IN t_uint8          hub_addr,
    IN t_uint8          hub_port,
    IN t_usb_endp_num   endp_num
)
{
    switch (endp_num)
    {
        case USB_ENDPNUM_0_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX0FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX0HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX0HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_1_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX1FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX1HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX1HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_2_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX2FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX2HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX2HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_3_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX3FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX3HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX3HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_4_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX4FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX4HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX4HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_5_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX5FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX5HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX5HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_6_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX6FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX6HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX6HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_7_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX7FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX7HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX7HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_8_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX8FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX8HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX8HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_9_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX9FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX9HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX9HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_10_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX10FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX10HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX10HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_11_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX11FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX11HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX11HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_12_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX12FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX12HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX12HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_13_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX13FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX13HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX13HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_14_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX14FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX14HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX14HP = (hub_port & OTG_HP_MASK);
            break;

        case USB_ENDPNUM_15_IN:
            (g_usb_system_context.p_usb_0_register)->OTG_RX15FAD = (token_addr & OTG_FAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX15HAD = (hub_addr & OTG_HAD_MASK);
            (g_usb_system_context.p_usb_0_register)->OTG_RX15HP = (hub_port & OTG_HP_MASK);
            break;
    }
}

/************************************************************************************************/
/* NAME:	PUBLIC void  usb_host_UpdateFunctionDetails()	            				      	*/
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine updates the functions details like hub and function addresses.    	*/
/* PARAMETERS:											                                        */
/*              token_addr:              Function address.                                    	*/
/*              hub_addr:                Hub address.                                         	*/
/*              hub_port:                Hub port.                                            	*/
/*              endp_num:                Endpoint number.                                     	*/
/* OUT :        t_uint16:                register value of the fifo size          	          	*/
/* 												                                                */
/* RETURN:		none.                                       						            */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PRIVATE void usb_host_UpdateFunctionDetails
(
    IN t_uint8          token_addr,
    IN t_uint8          hub_addr,
    IN t_uint8          hub_port,
    IN t_usb_endp_num   endp_num
)
{
    if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {
        /* Host Update Function Details (OUT Endpoint) */
        usb_host_Out_Endp_UpdateFunctionDetails(token_addr, hub_addr, hub_port, endp_num);
    }
    else
    {
        /* Host Update Function Details (IN Endpoint) */
        usb_host_In_Endp_UpdateFunctionDetails(token_addr, hub_addr, hub_port, endp_num);
    }

    DBGEXIT0(USB_OK);
}

/************************************************************************************************/
/* NAME: t_usb_error USB_host_ResetToggleData( IN t_usb_endp_num)			                    */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears/Resets the Data Toogle 						                */
/*												                                                */
/* PARAMETERS:											                                        */
/* INOUT : 		None									                                        */
/*			endp_num:			The end point number			                                */
/* OUT : 			                                                                	 	  	*/
/* 												                                                */
/* RETURN:		t_usb_error:		Not applicable if requested for End Point number 0	        */
/*												                                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PUBLIC t_usb_error USB_host_ResetToggleData(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    /* Point to relevant endpoint's register */
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);       
        return(USB_REQUEST_NOT_APPLICABLE);
    }
    else if (((t_uint32) endp_num) > (USB_MAX_NUM_ENDPOINTS / 2))   /* OUT endpoint */
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_CLR_MASK;        
    }
    else    /* IN Endpoint */
    {        
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_CLR_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/************************************************************************************************/
/* NAME:	PRIVATE void  usb_getFifoAddress()								                    */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine returns the base address of the appropriate FIFO.                 	*/
/* PARAMETERS:											                                        */
/* IN :         endp_num_short:          Identify the endpoint.                               	*/
/* OUT :        none 										                                    */
/* 												                                                */
/* RETURN:		t_uint32 *:             The base address of the FIFO.				            */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PRIVATE t_uint32 *usb_getFifoAddress(t_uint8 endp_num_short)
{
    t_uint32    *lp_fifo_base_address = NULL;
    switch (endp_num_short)
    {
        case 0:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO0));
            break;

        case 1:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO1));
            break;

        case 2:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO2));
            break;

        case 3:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO3));
            break;

        case 4:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO4));
            break;

        case 5:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO5));
            break;

        case 6:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO6));
            break;

        case 7:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO7));
            break;

        case 8:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO8));
            break;

        case 9:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO9));
            break;

        case 10:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO10));
            break;

        case 11:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO11));
            break;

        case 12:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO12));
            break;

        case 13:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO13));
            break;

        case 14:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO14));
            break;

        case 15:
            lp_fifo_base_address = ((t_uint32 *) &((g_usb_system_context.p_usb_0_register)->OTG_FIFO15));
            break;
    }

    DBGEXIT0(USB_OK);
    return(lp_fifo_base_address);
}

/************************************************************************************************/
/* NAME:	PUBLIC void  USB_dev_SetTxRxRdyForDma(IN t_usb_endp_num)					        */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine sets the TxRdy or RxRdy bit manually after gettinga DMA interrupt 	*/
/* PARAMETERS:											                                        */
/* IN :        	NONE										                                    */
/*             										                                            */
/* OUT :                                                								        */
/* 												                                                */
/* RETURN:		t_usb_error :   USB_OK always                				  	                */
/************************************************************************************************/
/* COMMENTS :  	Only to be used for Mentor IP & for IN EP with DMA Mode only        	  		*/
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PUBLIC t_usb_error USB_dev_SetTxRxRdyForDma(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (((t_uint32) endp_num) >= (USB_MAX_NUM_ENDPOINTS / 2))
    {   /*  OUT Transfer */
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_DRXCSR_RRDY_MASK;
    }
    else
    {   /*  IN Transfer  */
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_TRDY_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/************************************************************************************************************/
/* NAME:	PUBLIC void  USB_SetMultiplierAndFifoSize(IN t_usb_dev_xfer_config, INOUT t_usb_endp_config *)	*/
/*----------------------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine gets the best multiplier factor & accordingly gets the Fifo size  			    */
/*				for Bulk transfer 							                                                */
/* PARAMETERS:											                                                    */
/* IN :        	NONE										                                                */
/*             										                                                        */
/* OUT :                                                								                    */
/* 												                                                            */
/* RETURN:		t_uint32 :                   remaining bytes to be transfered				                */
/************************************************************************************************************/
/* COMMENTS :  	Only to be used only for Mentor IP 	 				                                        */
/*----------------------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                            */
/*----------------------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                            */

/************************************************************************************************************/
PUBLIC void USB_SetMultiplierAndFifoSize(IN t_usb_dev_xfer_config xfer_config, INOUT t_usb_endp_config *endp_config)
{
    t_uint32    i, remaining_bytes = 0, m = 0;

    endp_config->multiplier = 0;

    for (i = 32; i >= 1; i--)
    {
        m = ((xfer_config.xfer_size) % (i * (endp_config->maxpacketsize)));

        if (i == 32)
        {
            remaining_bytes = m;
            endp_config->multiplier = i;
        }

        if (m == 0)
        {
            remaining_bytes = m;
            endp_config->multiplier = i;
            break;
        }
        else
        {
            if (m < remaining_bytes)
            {
                remaining_bytes = m;
                endp_config->multiplier = i;
            }
        }
    }
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);

    g_usb_system_context.dev_endpoint[endp_config->endp_num].multiplier = (endp_config->multiplier);
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);
    
    endp_config->fifo_size = (endp_config->multiplier) * (endp_config->maxpacketsize);

    DBGEXIT0(USB_OK);
}

/************************************************************************************************/
/* NAME:	PUBLIC void  USB_SaveSystemContext()							                    */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine saves the contents of USB registers.        	       			  	*/
/* PARAMETERS:											                                        */
/*             										                                            */
/* OUT :                                                								        */
/* 												                                                */
/* RETURN:		t_usb_error :                   USB_OK always.					                */
/************************************************************************************************/
/* COMMENTS :  											                                        */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                    */

/************************************************************************************************/
PUBLIC void USB_SaveSystemContext(void)
{
    DBGENTER0();
        
    
    g_usb_system_context.usb_save_device_0_context[0] = (g_usb_system_context.p_usb_0_register)->OTG_PWR;
    g_usb_system_context.usb_save_device_0_context[1] = (g_usb_system_context.p_usb_0_register)->OTG_RXCSR;
    g_usb_system_context.usb_save_device_0_context[2] = (g_usb_system_context.p_usb_0_register)->OTG_INTUSBEN;
    g_usb_system_context.usb_save_device_0_context[3] = (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN;
    g_usb_system_context.usb_save_device_0_context[4] = (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN;
    g_usb_system_context.usb_save_device_0_context[5] = (g_usb_system_context.p_usb_0_register)->OTG_FADDR;
    g_usb_system_context.usb_save_device_0_context[6] = (g_usb_system_context.p_usb_0_register)->OTG_INDX;
    g_usb_system_context.usb_save_device_0_context[7] = (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP;
    g_usb_system_context.usb_save_device_0_context[8] = (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR;
    g_usb_system_context.usb_save_device_0_context[9] = (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP;
    g_usb_system_context.usb_save_device_0_context[10] = (g_usb_system_context.p_usb_0_register)->OTG_TYPE0_TXTYPE;
    g_usb_system_context.usb_save_device_0_context[11] = (g_usb_system_context.p_usb_0_register)->OTG_NAKLMT0_TXINTV;
    g_usb_system_context.usb_save_device_0_context[12] = (g_usb_system_context.p_usb_0_register)->OTG_RXTYPE;
    g_usb_system_context.usb_save_device_0_context[13] = (g_usb_system_context.p_usb_0_register)->OTG_RXINTV;
    g_usb_system_context.usb_save_device_0_context[14] = (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL;
    g_usb_system_context.usb_save_device_0_context[15] = (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ;
    g_usb_system_context.usb_save_device_0_context[16] = (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ;
    g_usb_system_context.usb_save_device_0_context[17] = (g_usb_system_context.p_usb_0_register)->OTG_TXFA;
    g_usb_system_context.usb_save_device_0_context[18] = (g_usb_system_context.p_usb_0_register)->OTG_RXFA;
    g_usb_system_context.usb_save_device_0_context[19] = (g_usb_system_context.p_usb_0_register)->OTG_VCNTL;
    g_usb_system_context.usb_save_device_0_context[20] = (g_usb_system_context.p_usb_0_register)->OTG_UVBCTRL;
    g_usb_system_context.usb_save_device_0_context[21] = (g_usb_system_context.p_usb_0_register)->OTG_UINTMASK;
    g_usb_system_context.usb_save_device_0_context[22] = (g_usb_system_context.p_usb_0_register)->OTG_UINTSRC;
    g_usb_system_context.usb_save_device_0_context[23] = (g_usb_system_context.p_usb_0_register)->OTG_UREGDATA;
    g_usb_system_context.usb_save_device_0_context[24] = (g_usb_system_context.p_usb_0_register)->OTG_UREGADDR;
    g_usb_system_context.usb_save_device_0_context[25] = (g_usb_system_context.p_usb_0_register)->OTG_UREGCTRL;
    g_usb_system_context.usb_save_device_0_context[26] = (g_usb_system_context.p_usb_0_register)->OTG_URAWDATA;
    g_usb_system_context.usb_save_device_0_context[27] = (g_usb_system_context.p_usb_0_register)->OTG_LINKINFO;
    g_usb_system_context.usb_save_device_0_context[28] = (g_usb_system_context.p_usb_0_register)->OTG_VPLEN;
    
#ifdef ST_8500ED
    g_usb_system_context.usb_save_device_0_context[29] = (g_usb_system_context.p_usb_0_register)->OTG_DMASEL;
    g_usb_system_context.usb_save_device_0_context[30] = (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL;
#endif        

    DBGEXIT0(USB_OK);
}

/************************************************************************************************/
/* NAME:	PUBLIC void  USB_RestoreSystemContext()							*/
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine restores the contents in to USB register.              			  	*/
/* PARAMETERS:											*/
/*             										              */
/* OUT :                                                								*/
/* 												*/
/* RETURN:		t_usb_error :                   USB_OK always.					*/
/************************************************************************************************/
/* COMMENTS :  											*/
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT									*/
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/************************************************************************************************/
PUBLIC void USB_RestoreSystemContext(void)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_PWR = (t_uint8) (g_usb_system_context.usb_save_device_0_context[0]);
    (g_usb_system_context.p_usb_0_register)->OTG_RXCSR = (t_uint16) (g_usb_system_context.usb_save_device_0_context[1]);
    (g_usb_system_context.p_usb_0_register)->OTG_INTUSBEN = (t_uint8) (g_usb_system_context.usb_save_device_0_context[2]);
    (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN = (t_uint16) (g_usb_system_context.usb_save_device_0_context[3]);
    (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN = (t_uint16) (g_usb_system_context.usb_save_device_0_context[4]);
    (g_usb_system_context.p_usb_0_register)->OTG_FADDR = (t_uint8) (g_usb_system_context.usb_save_device_0_context[5]);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (t_uint8) (g_usb_system_context.usb_save_device_0_context[6]);
    (g_usb_system_context.p_usb_0_register)->OTG_TXMAXP = (t_uint16) (g_usb_system_context.usb_save_device_0_context[7]);
    (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = (t_uint16) (g_usb_system_context.usb_save_device_0_context[8]);
    (g_usb_system_context.p_usb_0_register)->OTG_RXMAXP = (t_uint16) (g_usb_system_context.usb_save_device_0_context[9]);
    (g_usb_system_context.p_usb_0_register)->OTG_TYPE0_TXTYPE = (t_uint8) (g_usb_system_context.usb_save_device_0_context[10]);
    (g_usb_system_context.p_usb_0_register)->OTG_NAKLMT0_TXINTV = (t_uint8) (g_usb_system_context.usb_save_device_0_context[11]);
    (g_usb_system_context.p_usb_0_register)->OTG_RXTYPE = (t_uint8) (g_usb_system_context.usb_save_device_0_context[12]);
    (g_usb_system_context.p_usb_0_register)->OTG_RXINTV = (t_uint8) (g_usb_system_context.usb_save_device_0_context[13]);
    (g_usb_system_context.p_usb_0_register)->OTG_DEVCTL = (t_uint8) (g_usb_system_context.usb_save_device_0_context[14]);
    (g_usb_system_context.p_usb_0_register)->OTG_TXFSZ = (t_uint8) (g_usb_system_context.usb_save_device_0_context[15]);
    (g_usb_system_context.p_usb_0_register)->OTG_RXFSZ = (t_uint8) (g_usb_system_context.usb_save_device_0_context[16]);
    (g_usb_system_context.p_usb_0_register)->OTG_TXFA = (t_uint16) (g_usb_system_context.usb_save_device_0_context[17]);
    (g_usb_system_context.p_usb_0_register)->OTG_RXFA = (t_uint16) (g_usb_system_context.usb_save_device_0_context[18]);
    (g_usb_system_context.p_usb_0_register)->OTG_VCNTL = g_usb_system_context.usb_save_device_0_context[19];
    (g_usb_system_context.p_usb_0_register)->OTG_UVBCTRL = (t_uint8) g_usb_system_context.usb_save_device_0_context[20];
    (g_usb_system_context.p_usb_0_register)->OTG_UINTMASK = (t_uint8) g_usb_system_context.usb_save_device_0_context[21];
    (g_usb_system_context.p_usb_0_register)->OTG_UINTSRC = (t_uint8) g_usb_system_context.usb_save_device_0_context[22];
    (g_usb_system_context.p_usb_0_register)->OTG_UREGDATA = (t_uint8) g_usb_system_context.usb_save_device_0_context[23];
    (g_usb_system_context.p_usb_0_register)->OTG_UREGADDR = (t_uint8) g_usb_system_context.usb_save_device_0_context[24];
    (g_usb_system_context.p_usb_0_register)->OTG_UREGCTRL = (t_uint8) g_usb_system_context.usb_save_device_0_context[25];
    (g_usb_system_context.p_usb_0_register)->OTG_URAWDATA = (t_uint8) g_usb_system_context.usb_save_device_0_context[26];
    (g_usb_system_context.p_usb_0_register)->OTG_LINKINFO = (t_uint8) g_usb_system_context.usb_save_device_0_context[27];
    (g_usb_system_context.p_usb_0_register)->OTG_VPLEN = (t_uint8) g_usb_system_context.usb_save_device_0_context[28];

#ifdef ST_8500ED
    (g_usb_system_context.p_usb_0_register)->OTG_DMASEL = (t_uint8) g_usb_system_context.usb_save_device_0_context[29];
    (g_usb_system_context.p_usb_0_register)->OTG_TOPCTRL = (t_uint8) g_usb_system_context.usb_save_device_0_context[30];
#endif

    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
}

/************************************************************************************************/
/* NAME:	PUBLIC t_usb_error USB_dev_EnterTestMode()							                */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine makes the tests on USB IP by putting the IP in Test mode, only valid*/
/*             for Mentro IP.	                                                                */
/* PARAMETERS:											                                        */
/*												                                                */
/* OUT :												                                        */
/* 												                                                */
/* RETURN:		t_usb_error :                   USB_OK always.					                */
/************************************************************************************************/
/* COMMENTS :  											                                        */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PUBLIC t_usb_error USB_dev_EnterTestMode(IN t_usb_dev_test_mode test_mode_selector)
{
    /* USB 2.0 specified 53 bytes packet */
    t_uint8 test_packet[56] =
    {
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xaa,
        0xee,
        0xee,
        0xee,
        0xee,
        0xee,
        0xee,
        0xee,
        0xee,
        0xef,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0x7f,
        0xbf,
        0xdf,
        0xef,
        0xf7,
        0xfb,
        0xed,
        0xfc,
        0x7e,
        0xbf,
        0xdf,
        0xef,
        0xf7,
        0xfb,
        0xfd,
        0x7e,
        0x00,
        0x00,
        0x00
    };

    DBGENTER0();

    switch (test_mode_selector)
    {
        /* Only valid in High Speed Mode, entering this test in Full Speed mode will have no affect */
        case USB_TEST_J:
            {
                g_usb_system_context.p_usb_0_register->OTG_TM = OTG_TESTMODE_J_MASK;
            }
            break;

        case USB_TEST_K:
            {
                g_usb_system_context.p_usb_0_register->OTG_TM = OTG_TESTMODE_K_MASK;
            }
            break;

        case USB_TEST_SE0_NAK:
            {
                g_usb_system_context.p_usb_0_register->OTG_TM = OTG_TESTMODE_SE0NAK_MASK;
            }
            break;

        case USB_TEST_PACKET:
            {
                /* Set Start Session bit */
                g_usb_system_context.p_usb_0_register->OTG_DEVCTL |= OTG_DEVCTL_SESS_MASK;

                /* Set Index register to EP0 */
                g_usb_system_context.p_usb_0_register->OTG_INDX = 0;

                /* Set EP0 TX/RX FIFO size to 64 and TX/RX FIFO address to 0x0000 */
                g_usb_system_context.p_usb_0_register->OTG_TXFSZ |= 0x03;
                g_usb_system_context.p_usb_0_register->OTG_RXFSZ |= 0x03;
                g_usb_system_context.p_usb_0_register->OTG_TXFA = 0x0000;
                g_usb_system_context.p_usb_0_register->OTG_RXFA = 0x0000;

                /* Write Test Packet to EP0 FIFO */
                usb_WriteToFifo(0, test_packet, 53, USB_DEVICE_TYPE);

                /* Set the Start Packet test bit in Test mode register */
                g_usb_system_context.p_usb_0_register->OTG_TM = OTG_TESTMODE_PACKET_TEST_MASK;

		/* Set TxPktRdy bit in EP0 control register */
                g_usb_system_context.p_usb_0_register->OTG_CSR0 |= 0x02;

                /*USB_DEV_1_HCL_DELAY(); */

            }
            break;

        case USB_TEST_FORCE_ENABLE_HOST:
            {
                g_usb_system_context.p_usb_0_register->OTG_TM = OTG_TESTMODE_FORCE_ENABLE_HOST_MASK;
            }
            break;

        default:
            return(USB_REQUEST_NOT_APPLICABLE);
    }

    return(USB_OK);
}

/************************************************************************************************/
/* NAME:	t_usb_error USB_dev_ClearRxReady()								                    */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION:											                                        */
/*												                                                */
/* PARAMETERS:											                                        */
/* IN : 		t_usb_endp_num:		     		Identify the endpoint			                */
/* OUT:         none											                                */
/* 												                                                */
/* RETURN:		t_usb_error :                   USB_OK always.					                */
/*												                                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                    */

/************************************************************************************************/
PUBLIC void USB_dev_IsRxReadySet(t_bool *bRxReady)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL32;
    if ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_DCSR0_RRDY_MASK)
    {
        *bRxReady = TRUE;
    }
    else
    {
        *bRxReady = FALSE;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
}

/************************************************************************************************/
/* NAME:	t_usb_error USB_host_TransferCancel()							                    */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine cancels the host side transfer operation 					        */
/*												                                                */
/* PARAMETERS:											                                        */
/* INOUT : 		t_uint8*:		transfer buffer.						                        */
/* IN :			t_size:		size of transfer.						                            */
/*				t_usb_host_xfer_config:			transfer configuration.	                        */
/* OUT : 		none                                                                	 	    */
/* 												                                                */
/* RETURN:		t_usb_error:                  							                        */
/*				Possible error values are:						                                */
/*				USB_INVALID_PARAMETER:				If buffer  is NULL.	                        */
/*				USB_OK:						If everything's fine                                */
/*												                                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PUBLIC t_usb_error USB_host_TransferCancel(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);

    /* Point to relevant endpoint's register */
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    g_usb_system_context.host_endpoint[endp_num].xfer_status = USB_HOST_ACK;
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        if (USB_ENDPNUM_0_OUT == endp_num)
        {
            /* Clear the Setup Packet bit, if OUT endpoint */
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_SETUPPKT_MASK;
        }
        else
        {
            /* Clear the Status Packet and Request Packet bit, if IN endpoint */
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_STATUSPKT_MASK;
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_REQPKT_MASK;

            /* Clear all the error for the previous transfer */
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_RXSTALL_MASK;

            /* Unset all transfer parameter */
            g_usb_system_context.host_endpoint[endp_num].rx_length = 0;
            g_usb_system_context.host_endpoint[endp_num].p_rx_buffer = NULL;
        }
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_ERROR_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HCSR0_NAKTO_MASK;

        /* If TxPktRdy or RxPktRdy bit is set, flush the FIFO */
        if
        (
            ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_HCSR0_RRDY_MASK)
        ||  ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_HCSR0_TRDY_MASK)
        )
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
            if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	    {
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HCSR0_FFLUSH_MASK;
	    }
	}

        /* EP0 transfer interrupt */
        g_usb_system_context.last_transferred_token = USB_HOST_OUT;
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);
    }
    else if (((t_uint32) endp_num) < (USB_MAX_NUM_ENDPOINTS / 2))   /* If IN endpoint */
    {
        if (USB_DMA_MODE == g_usb_system_context.host_endpoint[endp_num].rx_mode)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_DMAR_MASK;
        }

        /* Clear the request. */
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_RPK_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~(OTG_HRXCSR_ARQ_MASK);
        if ((g_usb_system_context.p_usb_0_register)->OTG_RXCSR & OTG_HRXCSR_RRDY_MASK)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_FFLU_MASK;
            if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	    {
                (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_HRXCSR_FFLU_MASK;
	    }
	}

        /* Clear all transfer related error */
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_ERR_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_DERR_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_RXSTALL_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR &= ~OTG_HRXCSR_IRX_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_INTRXEN &= ~(1UL << endp_num_short);

        /* Unset transfer related parameter */
        g_usb_system_context.host_endpoint[endp_num].rx_length = 0;
        g_usb_system_context.host_endpoint[endp_num].p_rx_buffer = NULL;
    }
    else    /* If OUT endpoint */
    {
        if (USB_DMA_MODE == g_usb_system_context.host_endpoint[endp_num].rx_mode)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_DMAR_MASK;
        }
        (g_usb_system_context.p_usb_0_register)->OTG_INTTXEN &= ~(1UL << endp_num_short);

        /* Clear Auto set feature */
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_ASET_MASK;
        if ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_HCSR0_TRDY_MASK)
        {
            (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR = OTG_HTXCSR_FFLU_MASK;
            if(g_usb_system_context.host_endpoint[endp_num].enable_double_buffer)
	    {
                (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_HTXCSR_FFLU_MASK;
	    }
	}

        /* Clear all transfer related error */
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_ERR_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_NAK_MASK;
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR &= ~OTG_HTXCSR_RXS_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);

    return(USB_OK);
}

/************************************************************************************************/
/* NAME: t_usb_host_data_toggle USB_host_GetToggleData(IN t_usb_endp_num)		                */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initiates the transfer operation 						            */
/*												                                                */
/* PARAMETERS:											                                        */
/* INOUT : 		None									                                        */
/*			endp_num:			The end point number			                                */
/* OUT : 			                                                                	 	  	*/
/* 												                                                */
/* RETURN:		t_usb_host_data_toggle:		Data Toggle condition (0 or 1)		                */
/*												                                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PUBLIC t_usb_host_data_toggle USB_host_GetToggleData(IN t_usb_endp_num endp_num)
{
    t_uint8                 endp_num_short;
    t_usb_host_data_toggle  t_data_toggle = USB_DATA0;

    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    /* Point to relevant endpoint's register */
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        if ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_HCSR0_DT_MASK)
        {
            t_data_toggle = USB_DATA1;
        }
    }
    else if (((t_uint32) endp_num) > (USB_MAX_NUM_ENDPOINTS / 2))
    {
        if ((g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR & OTG_HTXCSR_DT_MASK)
        {
            t_data_toggle = USB_DATA1;
        }
    }
    else
    {
        if ((g_usb_system_context.p_usb_0_register)->OTG_RXCSR & OTG_HRXCSR_DT_MASK)
        {
            t_data_toggle = USB_DATA1;
        }
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(t_data_toggle);
}

/************************************************************************************************/
/* NAME: t_usb_error USB_dev_ResetToggleData( IN t_usb_endp_num)			                    */
/*----------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears/Resets the Data Toogle 						                */
/*												                                                */
/* PARAMETERS:											                                        */
/* INOUT : 		None									                                        */
/*			endp_num:			The end point number			                                */
/* OUT : 			                                                                	 	  	*/
/* 												                                                */
/* RETURN:		t_usb_error:		Not applicable if requested for End Point number 0	        */
/*												                                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT									                                */
/*----------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL										                                */

/************************************************************************************************/
PUBLIC t_usb_error USB_dev_ResetToggleData(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    DBGENTER0();

    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    /* Point to relevant endpoint's register */
    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    if (USB_ENDPNUM_0_OUT == endp_num || USB_ENDPNUM_0_IN == endp_num)
    {
        HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);       
        return(USB_REQUEST_NOT_APPLICABLE);
    }
    else if (((t_uint32) endp_num) > (USB_MAX_NUM_ENDPOINTS / 2))   /* OUT endpoint */
    {
        (g_usb_system_context.p_usb_0_register)->OTG_RXCSR |= OTG_DRXCSR_CLR_MASK;
    }
    else    /* IN Endpoint */
    {
        (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DTXCSR_CLR_MASK;
    }
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);

    DBGEXIT0(USB_OK);
    return(USB_OK);
}


/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_CompleteSetupAndStatusStage()                                 */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: 			   																  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          																              */
/* OUT : 		none															  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_CompleteSetupAndStatusStage(void)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL32;
    (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_DATAEND_MASK | OTG_DCSR0_SERRDY_MASK;
    
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);
    
    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error USB_dev_CompleteSetupStage(IN t_usb_device_id device_id)     		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: 			   																  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* IN:          device_id																	  */
/* OUT : 		none															  		      */
/* 																			                  */
/* RETURN:		t_usb_error :                   USB_OK always.						          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_error USB_dev_CompleteSetupStage(void)
{
    DBGENTER0();
    
    /* Enter Critical Section */
    HCL_CRITICAL_SECTION_ENTER(USB_HCL_ID,1);
    
    (g_usb_system_context.p_usb_0_register)->OTG_INDX = MASK_NULL32;
    (g_usb_system_context.p_usb_0_register)->OTG_CSR0_TXCSR |= OTG_DCSR0_SERRDY_MASK;
    
    /* Exit Critical Section */
    HCL_CRITICAL_SECTION_EXIT(USB_HCL_ID,1);
    
    DBGEXIT0(USB_OK);
    return(USB_OK);
}

/**********************************************************************************************/
/* NAME:	t_usb_error usb_NrOfBytesInRxFIFO(IN t_usb_endp_num endp_num)		     		  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: 			   																  */
/*																		                  	  */
/* PARAMETERS:																				  */
/* 																			                  */
/* RETURN:		t_uint32 :                   Number of bytes in Rx USB FIFO.				  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_uint32 usb_NrOfBytesInRxFIFO(IN t_usb_endp_num endp_num)
{
    t_uint8 endp_num_short;

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    (g_usb_system_context.p_usb_0_register)->OTG_INDX = (endp_num_short & OTG_ENDPNO_INDX_MASK);

    return (g_usb_system_context.p_usb_0_register)->OTG_CNT0_RXCNT & OTG_RXCNT_MASK;
}

/**********************************************************************************************/
/* NAME:	PUBLIC void  usb_ExternalReadFromFifo()    								      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:Public interface for reading data from the Rx FIFO.							  */
/* PARAMETERS:																                  */
/* IN  :		   size:   	   			   Size of the buffer.								  */
/* OUT :         void *:			   destination buffer.									  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: NON REENTRANT																  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void usb_ExternalReadFromFifo(IN t_usb_endp_num endp_num, OUT void *p_dest, IN t_size size)
{
    t_size      i, j;
    t_uint32    *p_fifodata_0;
    t_uint32    data_received;
    t_uint8     endp_num_short;

    endp_num_short = GET_0_ENDPNUM_SHORT(endp_num);

    usb_ReadFromFifo(endp_num_short, p_dest, size);
}



