/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief This file holds the function declaration for the services.
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _USB_SERVICES_H_
#define _USB_SERVICES_H_

#include "services.h"

#define SER_USB_PRCC_AMBACLK_ENA 0x01
	
PUBLIC int SER_USB_RegisterCallback (t_callback_fct fct, void* param);

PUBLIC void SER_USB_WaitEnd(void);
void SER_USB_IntHandler(unsigned int irq);

#endif /*	_USB_SERVICES_H_	*/

#ifdef __cplusplus
}
#endif
