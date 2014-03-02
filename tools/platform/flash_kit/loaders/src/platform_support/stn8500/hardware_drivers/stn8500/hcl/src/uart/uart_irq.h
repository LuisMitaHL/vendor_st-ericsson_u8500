/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This File contains public headers for uart_irq.c (U8500)
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _UART_IRQ_H_
#define _UART_IRQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#ifndef __INC_DBG_H
#include "debug.h"
#endif
	
/* UART Device ID */
typedef enum
{
    UART_DEVICE_ID_0        = 0x0,
    UART_DEVICE_ID_1        = 0x1,
    UART_DEVICE_ID_2        = 0x2,
    UART_DEVICE_ID_INVALID  = 0x3
} t_uart_device_id;

/* Enumeration t_uart_irq_src_id or ORing of enumeration t_uart_irq_src_id  *
 *--------------------------------------------------------------------------*/
typedef t_uint32        t_uart_irq_src;

/*--------------------------------------------------------------------------*
 * Functions declaration                                                    *
 *--------------------------------------------------------------------------*/
/* Initialization functions *
 *--------------------------*/
PUBLIC void             UART_SetBaseAddress(IN t_uart_device_id uart_device_id, IN t_logical_address uart_base_address);

/* Interrupt Management    * 
 * M0 functions            *
 *----------------------*/
PUBLIC void             UART_EnableIRQSrc(IN t_uart_irq_src enable_irq);
PUBLIC void             UART_DisableIRQSrc(IN t_uart_irq_src disable_irq);
PUBLIC t_uart_irq_src   UART_GetIRQSrc(IN t_uart_device_id uart_device_id);
PUBLIC void             UART_ClearIRQSrc(IN t_uart_irq_src clear_irq);
PUBLIC t_bool           UART_IsPendingIRQSrc(IN t_uart_irq_src irq);
PUBLIC t_uart_device_id UART_GetDeviceID(IN t_uart_irq_src irq_src);

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _UART_IRQ_H_ */

/* End of file - uart_irq.h */

