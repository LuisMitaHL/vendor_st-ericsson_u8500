/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This File contains Headers for Services Routines for UART
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _UART_SERVICES_H_
#define _UART_SERVICES_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __UART_ENHANCED
/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
#define SER_UART_WAIT_COUNT 0xFFFFFFFF
#endif /*  UART_ENHANCED  */

#define SER_PRCM_UARTCLK_MGT_REG   0x80157018
#define SER_UARTCLK_ENA            0x320

#define SER_UART_0_AMBA_KERNEL_CLK 0x01
#define SER_UART_1_AMBA_KERNEL_CLK 0x02
#define SER_UART_2_AMBA_KERNEL_CLK 0x40
#define UART_PRCC_KCKEN_REG_OFFSET 0x08

#ifdef __UART_ENHANCED

/* Tyepdefs */
typedef struct
{
    t_uart_event    uart_event;
} t_uart_param;

/* Public Functions */
PUBLIC void         SER_UART0_InterruptHandler(IN t_uint32 irq_src);
PUBLIC void         SER_UART1_InterruptHandler(IN t_uint32 irq_src);

PUBLIC void         SER_UART2_InterruptHandler(IN t_uint32 irq_src);
PUBLIC t_ser_error  SER_UART_RegisterCallback
                    (
                        IN t_uart_device_id uart_device_id,
                        IN t_callback_fct   callback_fct,
                        INOUT void          *param
                    );
PUBLIC t_ser_error  SER_UART_TransmitWaitEnd(IN t_uart_device_id uart_device_id);
PUBLIC t_ser_error  SER_UART_ReceiveWaitEnd(IN t_uart_device_id uart_device_id);
#endif /*  __UART_ENHANCED  */


#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* end _UART_SERVICES_H_ */

/* End of file - uart_services.h */

