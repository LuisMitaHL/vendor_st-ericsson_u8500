/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief  This file holds the M0 interrupt functions and data structures exported by Device HCL
*          functions for U5500.
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "usb_regp.h"
#include "usb_irq.h"

#define USB_MODE_HOST   0x0
#define USB_MODE_DEVICE 0x1

PRIVATE t_usb_0_register    *gp_usb_0_register;

PRIVATE void                USB_EnableSysIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Dev_EnableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Dev_EnableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Host_EnableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Host_EnableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status);

PRIVATE void                USB_DisableSysIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Dev_DisableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Dev_DisableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Host_DisableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status);
PRIVATE void                USB_Host_DisableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status);

PRIVATE void                USB_GetSysIRQSrc(t_usb_irq_src *sys_irq, t_uint8 mode);
PRIVATE void                USB_Dev_GetTxIRQSrc(t_usb_irq_src *dev_irq, t_uint32 dev_tx_mask);
PRIVATE void                USB_Dev_GetRxIRQSrc(t_usb_irq_src *dev_irq, t_uint32 dev_rx_mask);
PRIVATE void                USB_Host_GetTxIRQSrc(t_usb_irq_src *host_irq, t_uint32 host_tx_mask);
PRIVATE void                USB_Host_GetRxIRQSrc(t_usb_irq_src *host_irq, t_uint32 host_rx_mask);

PRIVATE void                USB_Dev_IsPendingTxIRQSrcEndp1_7
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         tx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );
PRIVATE void                USB_Host_IsPendingTxIRQSrcEndp1_7
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         tx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );
PRIVATE void                USB_Dev_IsPendingRxIRQSrcEndp1_7
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         rx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );
PRIVATE void                USB_Host_IsPendingRxIRQSrcEndp1_7
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         rx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );

PRIVATE void                USB_Dev_IsPendingTxIRQSrcEndp8_15
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         tx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );
PRIVATE void                USB_Host_IsPendingTxIRQSrcEndp8_15
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         tx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );
PRIVATE void                USB_Dev_IsPendingRxIRQSrcEndp8_15
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         rx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );
PRIVATE void                USB_Host_IsPendingRxIRQSrcEndp8_15
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         rx_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );

PRIVATE void                USB_IsPendingSysIRQSrc
                            (
                                IN t_usb_irq_src    *p_irq_status,
                                IN t_uint32         sys_mask,
                                OUT t_uint32        *p_irq_src,
                                OUT volatile t_bool *p_is_irq_pending
                            );

/**********************************************************************************************/
/* NAME:	PUBLIC void  USB_SetBaseAddress()										          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine sets the base address for accessing the USB subsystem.			  */
/* PARAMETERS:																                  */
/*             t_logical_address:	   Base Address.										  */
/* OUT :       none																			  */
/* 																			                  */
/* RETURN:	   none								 		 							          */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void USB_SetBaseAddress(IN t_logical_address base_address)
{
    gp_usb_0_register = (t_usb_0_register *) base_address;
}

/**********************************************************************************************/
/* NAME:	void USB_EnableSysIRQSrc()										     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_EnableSysIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_VBUSERROR)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_VBE_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SESSIONREQ)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_SREQ_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_DISCONNECT)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_DISCON_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_CONNECT)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_CONN_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SOF)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_SOF_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESET)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_RST_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_BABBLE)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_RST_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESUME)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_RES_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SUSPEND)
    {
        gp_usb_0_register->OTG_INTUSBEN |= OTG_INTUSB_SUSP_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_EP0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP0_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Dev_EnableInEndpIRQSrc(IN t_usb_irq_src* p_irq_status)			          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_EnableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Host_EnableInEndpIRQSrc(IN t_usb_irq_src* p_irq_status)			      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_EnableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Dev_EnableOutEndpIRQSrc(IN t_usb_irq_src* p_irq_status)				  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_EnableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN |= OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	USB_Host_EnableOutEndpIRQSrc(IN t_usb_irq_src* p_irq_status)				      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_EnableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN |= OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	USB_EnableIRQSrc(IN t_usb_irq_src irq_status)									  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void USB_EnableIRQSrc(IN t_usb_irq_src irq_status)
{
    t_uint8 mode;

    if (gp_usb_0_register->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK)
    {
        mode = USB_MODE_HOST;
    }
    else
    {
        mode = USB_MODE_DEVICE;
    }

    /* Enable System IRQ's */
    USB_EnableSysIRQSrc(&irq_status);

    if (USB_MODE_DEVICE == mode)
    {
        /* Enable IN ENDP IRQ's */
        USB_Dev_EnableInEndpIRQSrc(&irq_status);

        /* Enable OUT ENDP IRQ's */
        USB_Dev_EnableOutEndpIRQSrc(&irq_status);
    }
    else
    {
        /* Enable IN ENDP IRQ's */
        USB_Host_EnableInEndpIRQSrc(&irq_status);

        /* Enable OUT ENDP IRQ's */
        USB_Host_EnableOutEndpIRQSrc(&irq_status);
    }
}

/**********************************************************************************************/
/* NAME:	USB_DisableSysIRQSrc(IN t_usb_irq_src* p_irq_status)							  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_DisableSysIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_VBUSERROR)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_VBE_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SESSIONREQ)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_SREQ_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_DISCONNECT)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_DISCON_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_CONNECT)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_CONN_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SOF)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_SOF_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESET)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_RST_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_BABBLE)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_RST_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESUME)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_RES_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SUSPEND)
    {
        gp_usb_0_register->OTG_INTUSBEN &= ~OTG_INTUSB_SUSP_MASK;
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_EP0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP0_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	USB_Dev_DisableInEndpIRQSrc(IN t_usb_irq_src* p_irq_status)						  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_DisableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	USB_Host_DisableInEndpIRQSrc(IN t_usb_irq_src* p_irq_status)					  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_DisableInEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Dev_DisableOutEndpIRQSrc(IN t_usb_irq_src* p_irq_status)				  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_DisableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTRXEN &= ~OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Host_DisableOutEndpIRQSrc(IN t_usb_irq_src* p_irq_status)				  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_DisableOutEndpIRQSrc(IN t_usb_irq_src *p_irq_status)
{
    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP1) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP1_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP2) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP2_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP3) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP3_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP4) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP4_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP5) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP5_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP6) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP6_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP7) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP7_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP8) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP8_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP9) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP9_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP10) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP10_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP11) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP11_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP12) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP12_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP13) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP13_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP14) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP14_MASK;
    }

    if ((p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP15) != 0)
    {
        gp_usb_0_register->OTG_INTTXEN &= ~OTG_INTEP15_MASK;
    }
}

/**********************************************************************************************/
/* NAME:	USB_DisableIRQSrc(IN t_usb_irq_src irq_status)									  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables the given interrupt.									  */
/*																		                  	  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:		    multiple or single interrupt source(s).    	      	  */
/* 																							  */
/* 																			                  */
/* RETURN:																		          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC void USB_DisableIRQSrc(IN t_usb_irq_src irq_status)
{
    t_uint8 mode;

    if (gp_usb_0_register->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK)
    {
        mode = USB_MODE_HOST;
    }
    else
    {
        mode = USB_MODE_DEVICE;
    }

    /* Enable System IRQ's */
    USB_DisableSysIRQSrc(&irq_status);

    if (USB_MODE_DEVICE == mode)
    {
        /* Enable IN ENDP IRQ's */
        USB_Dev_DisableInEndpIRQSrc(&irq_status);

        /* Enable OUT ENDP IRQ's */
        USB_Dev_DisableOutEndpIRQSrc(&irq_status);
    }
    else
    {
        /* Enable IN ENDP IRQ's */
        USB_Host_DisableInEndpIRQSrc(&irq_status);

        /* Enable OUT ENDP IRQ's */
        USB_Host_DisableOutEndpIRQSrc(&irq_status);
    }
}

/**********************************************************************************************/
/* NAME:	t_bool USB_GetPendingSysIRQSrc(IN t_usb_irq_src* p_irq_status, 
                                           OUT t_usb_irq_status *p_status)                    */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and checks whether the given interrupt source is active or not.	It also		  */
/*             updates the status(if not NULL) if it is active.								  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:	     identify the interrupt source				      	  	  */
/* OUT :        t_usb_irq_status:   	   		 status of the interrupt				      */
/* 																			                  */
/* RETURN:		t_bool		  TRUE if the source is active.							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_bool USB_GetPendingSysIRQSrc(IN t_usb_irq_src *p_irq_status, OUT t_usb_irq_status *p_status)
{
    t_usb_irq_src   irq_mask;
    t_bool          ret_val = FALSE;

    irq_mask.system = gp_usb_0_register->OTG_INTUSB & gp_usb_0_register->OTG_INTUSBEN;

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SOF)
    {
        if (irq_mask.system & OTG_INTUSB_SOF_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_SOF;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_VBUSERROR)
    {
        if (irq_mask.system & OTG_INTUSB_VBE_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_VBUSERROR;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SESSIONREQ)
    {
        if (irq_mask.system & OTG_INTUSB_SREQ_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_SESSIONREQ;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_DISCONNECT)
    {
        if (irq_mask.system & OTG_INTUSB_DISCON_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_DISCONNECT;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_CONNECT)
    {
        if (irq_mask.system & OTG_INTUSB_CONN_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_CONNECT;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESET)
    {
        if (irq_mask.system & OTG_INTUSB_RST_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_RESET;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_BABBLE)
    {
        if (irq_mask.system & OTG_INTUSB_RST_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_BABBLE;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESUME)
    {
        if (irq_mask.system & OTG_INTUSB_RES_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_RESUME;

            ret_val = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SUSPEND)
    {
        if (irq_mask.system & OTG_INTUSB_SUSP_MASK)
        {
            (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_SUSPEND;

            ret_val = TRUE;
        }
    }

    return(ret_val);
}

/**********************************************************************************************/
/* NAME:	void USB_IsPendingSysIRQSrc(IN t_usb_irq_src* p_irq_status,IN t_uint32 sys_mask, 
                                       OUT t_uint32* p_irq_src, OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and checks whether the given interrupt source is active or not.	It also		  */
/*             updates the status(if not NULL) if it is active.								  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:	     identify the interrupt source				      	  	  */
/* OUT :        t_usb_irq_status:   	   		 status of the interrupt				      */
/* 																			                  */
/* RETURN:		t_bool		  TRUE if the source is active.							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_IsPendingSysIRQSrc
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         sys_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SOF)
    {
        if (sys_mask & OTG_INTUSB_SOF_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_SOF;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_VBUSERROR)
    {
        if (sys_mask & OTG_INTUSB_VBE_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_VBUSERROR;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SESSIONREQ)
    {
        if (sys_mask & OTG_INTUSB_SREQ_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_SESSIONREQ;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_DISCONNECT)
    {
        if (sys_mask & OTG_INTUSB_DISCON_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_DISCONNECT;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_CONNECT)
    {
        if (sys_mask & OTG_INTUSB_CONN_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_CONNECT;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESET)
    {
        if (sys_mask & OTG_INTUSB_RST_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_RESET;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_BABBLE)
    {
        if (sys_mask & OTG_INTUSB_RST_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_BABBLE;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_RESUME)
    {
        if (sys_mask & OTG_INTUSB_RES_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_RESUME;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->system & (t_uint32) USB_IRQ_SRC_SUSPEND)
    {
        if (sys_mask & OTG_INTUSB_SUSP_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_SUSPEND;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	USB_Dev_IsPendingTxIRQSrcEndp1_7(IN t_usb_irq_src* p_irq_status,IN t_uint32 tx_mask, 
                                       OUT t_uint32* p_irq_src, OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_IsPendingTxIRQSrcEndp1_7
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         tx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP1)
    {
        if (tx_mask & OTG_INTEP1_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP1;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP2)
    {
        if (tx_mask & OTG_INTEP2_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP2;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP3)
    {
        if (tx_mask & OTG_INTEP3_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP3;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP4)
    {
        if (tx_mask & OTG_INTEP4_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP4;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP5)
    {
        if (tx_mask & OTG_INTEP5_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP5;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP6)
    {
        if (tx_mask & OTG_INTEP6_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP6;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP7)
    {
        if (tx_mask & OTG_INTEP7_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP7;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Host_IsPendingTxIRQSrcEndp1_7(IN t_usb_irq_src* p_irq_status,
                  IN t_uint32 tx_mask, OUT t_uint32* p_irq_src, OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_IsPendingTxIRQSrcEndp1_7
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         tx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP1)
    {
        if (tx_mask & OTG_INTEP1_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP1;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP2)
    {
        if (tx_mask & OTG_INTEP2_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP2;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP3)
    {
        if (tx_mask & OTG_INTEP3_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP3;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP4)
    {
        if (tx_mask & OTG_INTEP4_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP4;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP5)
    {
        if (tx_mask & OTG_INTEP5_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP5;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP6)
    {
        if (tx_mask & OTG_INTEP6_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP6;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP7)
    {
        if (tx_mask & OTG_INTEP7_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP7;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Dev_IsPendingRxIRQSrcEndp1_7(IN t_usb_irq_src* p_irq_status,
                   IN t_uint32 rx_mask, OUT t_uint32* p_irq_src,OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_IsPendingRxIRQSrcEndp1_7
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         rx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP1)
    {
        if (rx_mask & OTG_INTEP1_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP1;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP2)
    {
        if (rx_mask & OTG_INTEP2_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP2;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP3)
    {
        if (rx_mask & OTG_INTEP3_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP3;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP4)
    {
        if (rx_mask & OTG_INTEP4_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP4;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP5)
    {
        if (rx_mask & OTG_INTEP5_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP5;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP6)
    {
        if (rx_mask & OTG_INTEP6_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP6;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP7)
    {
        if (rx_mask & OTG_INTEP7_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP7;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Host_IsPendingRxIRQSrcEndp1_7(IN t_usb_irq_src* p_irq_status,
                   IN t_uint32 rx_mask, OUT t_uint32* p_irq_src,OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_IsPendingRxIRQSrcEndp1_7
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         rx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP1)
    {
        if (rx_mask & OTG_INTEP1_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP1;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP2)
    {
        if (rx_mask & OTG_INTEP2_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP2;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP3)
    {
        if (rx_mask & OTG_INTEP3_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP3;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP4)
    {
        if (rx_mask & OTG_INTEP4_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP4;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP5)
    {
        if (rx_mask & OTG_INTEP5_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP5;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP6)
    {
        if (rx_mask & OTG_INTEP6_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP6;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP7)
    {
        if (rx_mask & OTG_INTEP7_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP7;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Dev_IsPendingTxIRQSrcEndp8_15(IN t_usb_irq_src* p_irq_status,
                  IN t_uint32 tx_mask, OUT t_uint32* p_irq_src, OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_IsPendingTxIRQSrcEndp8_15
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         tx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP8)
    {
        if (tx_mask & OTG_INTEP8_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP8;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP9)
    {
        if (tx_mask & OTG_INTEP9_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP9;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP10)
    {
        if (tx_mask & OTG_INTEP10_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP10;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP11)
    {
        if (tx_mask & OTG_INTEP11_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP11;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP12)
    {
        if (tx_mask & OTG_INTEP12_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP12;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP13)
    {
        if (tx_mask & OTG_INTEP13_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP13;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP14)
    {
        if (tx_mask & OTG_INTEP14_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP14;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP15)
    {
        if (tx_mask & OTG_INTEP15_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP15;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Host_IsPendingTxIRQSrcEndp8_15(IN t_usb_irq_src* p_irq_status,
                  IN t_uint32 tx_mask, OUT t_uint32* p_irq_src, OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_IsPendingTxIRQSrcEndp8_15
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         tx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP8)
    {
        if (tx_mask & OTG_INTEP8_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP8;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP9)
    {
        if (tx_mask & OTG_INTEP9_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP9;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP10)
    {
        if (tx_mask & OTG_INTEP10_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP10;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP11)
    {
        if (tx_mask & OTG_INTEP11_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP11;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP12)
    {
        if (tx_mask & OTG_INTEP12_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP12;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP13)
    {
        if (tx_mask & OTG_INTEP13_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP13;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP14)
    {
        if (tx_mask & OTG_INTEP14_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP14;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP15)
    {
        if (tx_mask & OTG_INTEP15_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP15;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Dev_IsPendingRxIRQSrcEndp8_15(IN t_usb_irq_src* p_irq_status,
                   IN t_uint32 rx_mask, OUT t_uint32* p_irq_src,OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_IsPendingRxIRQSrcEndp8_15
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         rx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP8)
    {
        if (rx_mask & OTG_INTEP8_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP8;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP9)
    {
        if (rx_mask & OTG_INTEP9_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP9;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP10)
    {
        if (rx_mask & OTG_INTEP10_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP10;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP11)
    {
        if (rx_mask & OTG_INTEP11_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP11;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP12)
    {
        if (rx_mask & OTG_INTEP12_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP12;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP13)
    {
        if (rx_mask & OTG_INTEP13_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP13;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP14)
    {
        if (rx_mask & OTG_INTEP14_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP14;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_OUT_EP15)
    {
        if (rx_mask & OTG_INTEP15_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_OUT_EP15;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	void USB_Host_IsPendingRxIRQSrcEndp8_15(IN t_usb_irq_src* p_irq_status,
                   IN t_uint32 rx_mask, OUT t_uint32* p_irq_src,OUT t_bool* p_is_irq_pending) */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_IsPendingRxIRQSrcEndp8_15
(
    IN t_usb_irq_src    *p_irq_status,
    IN t_uint32         rx_mask,
    OUT t_uint32        *p_irq_src,
    OUT volatile t_bool *p_is_irq_pending
)
{
    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP8)
    {
        if (rx_mask & OTG_INTEP8_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP8;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP9)
    {
        if (rx_mask & OTG_INTEP9_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP9;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP10)
    {
        if (rx_mask & OTG_INTEP10_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP10;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP11)
    {
        if (rx_mask & OTG_INTEP11_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP11;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP12)
    {
        if (rx_mask & OTG_INTEP12_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP12;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP13)
    {
        if (rx_mask & OTG_INTEP13_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP13;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP14)
    {
        if (rx_mask & OTG_INTEP14_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP14;
            *p_is_irq_pending = TRUE;
        }
    }

    if (p_irq_status->endp & (t_uint32) USB_IRQ_SRC_IN_EP15)
    {
        if (rx_mask & OTG_INTEP15_MASK)
        {
            *p_irq_src |= (t_uint32) USB_IRQ_SRC_IN_EP15;
            *p_is_irq_pending = TRUE;
        }
    }
}

/**********************************************************************************************/
/* NAME:	t_bool  USB_IsPendingIRQSrc()										              */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and checks whether the given interrupt source is active or not.	It also		  */
/*             updates the status(if not NULL) if it is active.								  */
/* PARAMETERS:																                  */
/* IN : 		t_usb_irq_src:	     identify the interrupt source				      	  	  */
/* OUT :        t_usb_irq_status:   	   		 status of the interrupt				      */
/* 																			                  */
/* RETURN:		t_bool		  TRUE if the source is active.							          */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_bool USB_IsPendingIRQSrc(IN t_usb_irq_src irq_status, OUT t_usb_irq_status *p_status)
{
    t_uint8         mode;
    t_uint16        rx_mask, tx_mask;
    volatile t_bool ret_val = FALSE;
    t_uint32        irq_src_mask;
    t_uint32        usb_irq_status = 0;

    if (NULL != p_status)
    {
        (p_status->pending_irq).system = (p_status->pending_irq).endp = MASK_NULL32;
    }

    if (gp_usb_0_register->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK)
    {
        mode = USB_MODE_HOST;
    }
    else
    {
        mode = USB_MODE_DEVICE;
    }

    irq_src_mask = gp_usb_0_register->OTG_INTUSB & gp_usb_0_register->OTG_INTUSBEN;

    /* Get Pending Sys Irq Source */
    USB_IsPendingSysIRQSrc(&irq_status, irq_src_mask, &usb_irq_status, &ret_val);

    tx_mask = gp_usb_0_register->OTG_INTTXEN & gp_usb_0_register->OTG_INTTX;
    rx_mask = gp_usb_0_register->OTG_INTRXEN & gp_usb_0_register->OTG_INTRX;

    if (irq_status.system & (t_uint32) USB_IRQ_SRC_EP0)
    {
        if (tx_mask & OTG_INTEP0_MASK)
        {
            if (NULL != p_status)
            {
                (p_status->pending_irq).system |= (t_uint32) USB_IRQ_SRC_EP0;
            }

            ret_val = TRUE;
        }
    }

    if (NULL != p_status)
    {
        (p_status->pending_irq).system |= usb_irq_status;
    }

    usb_irq_status = 0;

    if (USB_MODE_DEVICE == mode)
    {
        /* Check for Pending Tx Interrupt */
        USB_Dev_IsPendingTxIRQSrcEndp1_7(&irq_status, tx_mask, &usb_irq_status, &ret_val);
        USB_Dev_IsPendingTxIRQSrcEndp8_15(&irq_status, tx_mask, &usb_irq_status, &ret_val);

        /* Check for Pending Rx Interrupt */
        USB_Dev_IsPendingRxIRQSrcEndp1_7(&irq_status, rx_mask, &usb_irq_status, &ret_val);
        USB_Dev_IsPendingRxIRQSrcEndp8_15(&irq_status, rx_mask, &usb_irq_status, &ret_val);
    }
    else if (USB_MODE_HOST == mode)
    {
        /* Check for Pending Tx Interrupt */
        USB_Host_IsPendingTxIRQSrcEndp1_7(&irq_status, tx_mask, &usb_irq_status, &ret_val);
        USB_Host_IsPendingTxIRQSrcEndp8_15(&irq_status, tx_mask, &usb_irq_status, &ret_val);

        /* Check for Pending Rx Interrupt */
        USB_Host_IsPendingRxIRQSrcEndp1_7(&irq_status, rx_mask, &usb_irq_status, &ret_val);
        USB_Host_IsPendingRxIRQSrcEndp8_15(&irq_status, rx_mask, &usb_irq_status, &ret_val);
    }

    if (NULL != p_status)
    {
        (p_status->pending_irq).endp |= usb_irq_status;
        (p_status->initial_irq).system = (p_status->pending_irq).system;
        (p_status->initial_irq).endp = (p_status->pending_irq).endp;
        p_status->irq_state = USB_IRQ_STATE_NEW;
    }

    return(ret_val);
}

/**********************************************************************************************/
/* NAME:	t_usb_irq_src  USB_GetSysIRQSrc()	    							          	      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_GetSysIRQSrc(t_usb_irq_src *sys_irq, t_uint8 mode)
{
    t_uint32    system_mask = MASK_NULL32;

    system_mask = gp_usb_0_register->OTG_INTUSB & gp_usb_0_register->OTG_INTUSBEN;

    if (system_mask & OTG_INTUSB_VBE_MASK)
    {
        sys_irq->system |= (t_uint32) USB_IRQ_SRC_VBUSERROR;
    }

    if (system_mask & OTG_INTUSB_SREQ_MASK)
    {
        sys_irq->system |= (t_uint32) USB_IRQ_SRC_SESSIONREQ;
    }

    if (system_mask & OTG_INTUSB_DISCON_MASK)
    {
        sys_irq->system |= (t_uint32) USB_IRQ_SRC_DISCONNECT;
    }

    if (system_mask & OTG_INTUSB_CONN_MASK)
    {
        sys_irq->system |= (t_uint32) USB_IRQ_SRC_CONNECT;
    }

    if (system_mask & OTG_INTUSB_SOF_MASK)
    {
        sys_irq->system |= (t_uint32) USB_IRQ_SRC_SOF;
    }

    if (system_mask & OTG_INTUSB_RST_MASK)
    {
        if (USB_MODE_DEVICE == mode)
        {
            sys_irq->system |= (t_uint32) USB_IRQ_SRC_RESET;
        }
        else
        {
            sys_irq->system |= (t_uint32) USB_IRQ_SRC_BABBLE;
        }
    }

    if (system_mask & OTG_INTUSB_RES_MASK)
    {
        sys_irq->system |= (t_uint32) USB_IRQ_SRC_RESUME;
    }

    if (system_mask & OTG_INTUSB_SUSP_MASK)
    {
        sys_irq->system |= (t_uint32) USB_IRQ_SRC_SUSPEND;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_irq_src  USB_Dev_GetTxIRQSrc()	    							          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_GetTxIRQSrc(t_usb_irq_src *dev_irq, t_uint32 dev_tx_mask)
{
    if (dev_tx_mask & OTG_INTEP1_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP1;
    }

    if (dev_tx_mask & OTG_INTEP2_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP2;
    }

    if (dev_tx_mask & OTG_INTEP3_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP3;
    }

    if (dev_tx_mask & OTG_INTEP4_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP4;
    }

    if (dev_tx_mask & OTG_INTEP5_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP5;
    }

    if (dev_tx_mask & OTG_INTEP6_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP6;
    }

    if (dev_tx_mask & OTG_INTEP7_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP7;
    }

    if (dev_tx_mask & OTG_INTEP8_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP8;
    }

    if (dev_tx_mask & OTG_INTEP9_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP9;
    }

    if (dev_tx_mask & OTG_INTEP10_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP10;
    }

    if (dev_tx_mask & OTG_INTEP11_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP11;
    }

    if (dev_tx_mask & OTG_INTEP12_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP12;
    }

    if (dev_tx_mask & OTG_INTEP13_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP13;
    }

    if (dev_tx_mask & OTG_INTEP14_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP14;
    }

    if (dev_tx_mask & OTG_INTEP15_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP15;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_irq_src  USB_Host_GetTxIRQSrc()	    							          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_GetTxIRQSrc(t_usb_irq_src *host_irq, t_uint32 host_tx_mask)
{
    if (host_tx_mask & OTG_INTEP1_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP1;
    }

    if (host_tx_mask & OTG_INTEP2_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP2;
    }

    if (host_tx_mask & OTG_INTEP3_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP3;
    }

    if (host_tx_mask & OTG_INTEP4_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP4;
    }

    if (host_tx_mask & OTG_INTEP5_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP5;
    }

    if (host_tx_mask & OTG_INTEP6_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP6;
    }

    if (host_tx_mask & OTG_INTEP7_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP7;
    }

    if (host_tx_mask & OTG_INTEP8_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP8;
    }

    if (host_tx_mask & OTG_INTEP9_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP9;
    }

    if (host_tx_mask & OTG_INTEP10_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP10;
    }

    if (host_tx_mask & OTG_INTEP11_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP11;
    }

    if (host_tx_mask & OTG_INTEP12_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP12;
    }

    if (host_tx_mask & OTG_INTEP13_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP13;
    }

    if (host_tx_mask & OTG_INTEP14_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP14;
    }

    if (host_tx_mask & OTG_INTEP15_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP15;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_irq_src  USB_Dev_GetRxIRQSrc()	    							          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Dev_GetRxIRQSrc(t_usb_irq_src *dev_irq, t_uint32 dev_rx_mask)
{
    if (dev_rx_mask & OTG_INTEP1_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP1;
    }

    if (dev_rx_mask & OTG_INTEP2_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP2;
    }

    if (dev_rx_mask & OTG_INTEP3_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP3;
    }

    if (dev_rx_mask & OTG_INTEP4_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP4;
    }

    if (dev_rx_mask & OTG_INTEP5_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP5;
    }

    if (dev_rx_mask & OTG_INTEP6_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP6;
    }

    if (dev_rx_mask & OTG_INTEP7_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP7;
    }

    if (dev_rx_mask & OTG_INTEP8_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP8;
    }

    if (dev_rx_mask & OTG_INTEP9_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP9;
    }

    if (dev_rx_mask & OTG_INTEP10_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP10;
    }

    if (dev_rx_mask & OTG_INTEP11_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP11;
    }

    if (dev_rx_mask & OTG_INTEP12_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP12;
    }

    if (dev_rx_mask & OTG_INTEP13_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP13;
    }

    if (dev_rx_mask & OTG_INTEP14_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP14;
    }

    if (dev_rx_mask & OTG_INTEP15_MASK)
    {
        dev_irq->endp |= (t_uint32) USB_IRQ_SRC_OUT_EP15;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_irq_src  USB_Host_GetTxIRQSrc()	    							          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PRIVATE void USB_Host_GetRxIRQSrc(t_usb_irq_src *host_irq, t_uint32 host_rx_mask)
{
    if (host_rx_mask & OTG_INTEP1_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP1;
    }

    if (host_rx_mask & OTG_INTEP2_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP2;
    }

    if (host_rx_mask & OTG_INTEP3_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP3;
    }

    if (host_rx_mask & OTG_INTEP4_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP4;
    }

    if (host_rx_mask & OTG_INTEP5_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP5;
    }

    if (host_rx_mask & OTG_INTEP6_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP6;
    }

    if (host_rx_mask & OTG_INTEP7_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP7;
    }

    if (host_rx_mask & OTG_INTEP8_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP8;
    }

    if (host_rx_mask & OTG_INTEP9_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP9;
    }

    if (host_rx_mask & OTG_INTEP10_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP10;
    }

    if (host_rx_mask & OTG_INTEP11_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP11;
    }

    if (host_rx_mask & OTG_INTEP12_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP12;
    }

    if (host_rx_mask & OTG_INTEP13_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP13;
    }

    if (host_rx_mask & OTG_INTEP14_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP14;
    }

    if (host_rx_mask & OTG_INTEP15_MASK)
    {
        host_irq->endp |= (t_uint32) USB_IRQ_SRC_IN_EP15;
    }
}

/**********************************************************************************************/
/* NAME:	t_usb_irq_src  USB_GetIRQSrc()	    							          	      */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION:This routine allows to read the Mask Interrupt Status register				  */
/*             and updates the status.		             									  */
/* PARAMETERS:																                  */
/* IN : 		none.																      	  */
/* OUT :        none.																          */
/* 																			                  */
/* RETURN:		t_usb_irq_src:	IRQ Mask 										          	  */
/*																		                      */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: REENTRANT																	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY ISSUE: NIL													                  */

/**********************************************************************************************/
PUBLIC t_usb_irq_src USB_GetIRQSrc(void)
{
    t_uint8         mode;
    t_uint32        rx_mask = MASK_NULL32;
    t_uint32        tx_mask = MASK_NULL32;
    t_usb_irq_src   l_irq;

    l_irq.system = l_irq.endp = MASK_NULL32;

    if (gp_usb_0_register->OTG_DEVCTL & OTG_DEVCTL_HOST_MASK)
    {
        mode = USB_MODE_HOST;
    }
    else
    {
        mode = USB_MODE_DEVICE;
    }

    /* Get System Irq Src */
    USB_GetSysIRQSrc(&l_irq, mode);

    tx_mask = gp_usb_0_register->OTG_INTTX & gp_usb_0_register->OTG_INTTXEN;
    rx_mask = gp_usb_0_register->OTG_INTRX & gp_usb_0_register->OTG_INTRXEN;

    if (tx_mask & OTG_INTEP0_MASK)
    {
        l_irq.system |= (t_uint32) USB_IRQ_SRC_EP0;
    }

    if (USB_MODE_HOST == mode)
    {
        /* Get Tx Irq Source */
        USB_Host_GetTxIRQSrc(&l_irq, tx_mask);

        /* Get Rx Irq Dource */
        USB_Host_GetRxIRQSrc(&l_irq, rx_mask);
    }
    else
    {
        /* Get Tx Irq Source */
        USB_Dev_GetTxIRQSrc(&l_irq, tx_mask);

        /* Get Rx Irq Source */
        USB_Dev_GetRxIRQSrc(&l_irq, rx_mask);
    }

    return(l_irq);
}

