/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief This file holds the M0 interrupt function prototypes and data structures exported by USB HCL
*        functions for U5500
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _USB_IRQ_H_
#define _USB_IRQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _USB_H_
#include "usb.h"
#endif


PUBLIC void             USB_SetBaseAddress(IN t_logical_address base_address);
PUBLIC void             USB_EnableIRQSrc(IN t_usb_irq_src irq_status);
PUBLIC void             USB_DisableIRQSrc(IN t_usb_irq_src irq_status);
PUBLIC t_bool           USB_IsPendingIRQSrc(IN t_usb_irq_src irq_status, OUT t_usb_irq_status *p_status);
PUBLIC t_usb_irq_src    USB_GetIRQSrc(void);

#ifdef __cplusplus
}
#endif
#endif /*	_USB_IRQ_H_	*/

