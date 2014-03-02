/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header File for UART
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _UART_HWP_H_
#define _UART_HWP_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "uart_p.h"
/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * All mode Prototypes                                                      *
 *--------------------------------------------------------------------------*/	
PRIVATE t_uart_error    uart_ChooseFifoTriggerLevel_Nomadik
                        (
                            IN  t_uart_device_id    uart_device_id,
                            IN  t_uart_tx_trigger   tx_trigger,
                            IN  t_uart_rx_trigger   rx_trigger,
                            OUT t_uart_fifo_trigger *p_trigger
                        );
PRIVATE t_uart_error    uart_SetFifoTriggerLevel
                        (
                            IN t_uart_device_id     uart_device_id,
                            IN t_uart_fifo_trigger  fifo_trigger
                        );


/*--------------------------------------------------------------------------*
 * ELEMENTARY mode Prototypes                                               *
 *--------------------------------------------------------------------------*/	
#ifdef __UART_ELEMENTARY


/*------------------------------------------------------------*
 * Resolve Prototypes                                         *
 *------------------------------------------------------------*/	
#define DoTransmit          uart_elem_TransmitxCharacters
#define DoReceive           uart_elem_ReceivexCharacters
#define DoPowerOn           uart_elem_PowerOn
#define DoPowerOff          uart_elem_PowerOff
#define DoSetConfiguration  uart_elem_SetConfiguration
#define DoConfigureAB       uart_elem_ConfigureAutoBaud
#define DoGetFormat         uart_elem_GetFormatDetails
#define DoDisable           uart_elem_Disable
#define DoSetTransferMode   uart_elem_SetTransferMode

PRIVATE t_uart_error    uart_elem_TransmitxCharacters
                        (
                            IN t_uart_device_id uart_device_id,
                            IN t_uint32         num_of_char_to_be_tx,
                            IN t_uint8          *p_data_char
                        );
PRIVATE t_uart_error    uart_elem_ReceivexCharacters
                        (
                            IN  t_uart_device_id        uart_device_id,
                            IN  t_uint32                num_of_char_to_be_recd,
                            OUT t_uint8                 *p_data_char,
                            OUT t_uint32                *p_num_of_char_recd,
                            OUT t_uart_receive_status   *p_receive_error
                        );
PRIVATE t_uart_error    uart_elem_PowerOn(IN t_uart_device_id uart_device_id);
PRIVATE t_uart_error    uart_elem_PowerOff(IN t_uart_device_id uart_device_id);
PRIVATE t_uart_error    uart_elem_SetConfiguration(IN t_uart_device_id uart_device_id, IN t_uart_config *p_config);
PRIVATE t_uart_error    uart_elem_SetTransferMode(IN t_uart_device_id uart_device_id, IN t_uint32 flags);
PRIVATE t_uart_error    uart_elem_ConfigureAutoBaud
                        (
                            IN t_uart_device_id     uart_device_id,
                            IN t_uart_set_or_clear  uart_action
                        );
PRIVATE t_uart_error    uart_elem_GetFormatDetails
                        (
                            IN  t_uart_device_id    uart_device_id,
                            OUT t_uart_stop_bits    *p_tx_stop_bits,
                            OUT t_uart_data_bits    *p_tx_data_bits,
                            OUT t_uart_parity_bit   *p_tx_parity_bit,
                            OUT t_uart_stop_bits    *p_rx_stop_bits,
                            OUT t_uart_data_bits    *p_rx_data_bits,
                            OUT t_uart_parity_bit   *p_rx_parity_bit,
			    OUT t_uart_baud_rate    *p_baud_rate,
                            OUT t_bool              *p_tx_fifo_enabled,
                            OUT t_bool              *p_rx_fifo_enabled,
                            OUT t_bool              *p_is_autobaud_done
                        );
PRIVATE t_uart_error    uart_elem_Disable(IN t_uart_device_id uart_device_id);

#define UpdateRxTrig(uart_device, val)
#define UpdateTxTrig(uart_device, val)




#elif defined (__UART_ENHANCED)
/*--------------------------------------------------------------------------*
 * ENHANCED mode Prototypes                                                 *
 *--------------------------------------------------------------------------*/

#define DoTransmit          uart_enh_TransmitxCharacters
#define DoReceive           uart_enh_ReceivexCharacters
#define DoPowerOn           uart_enh_PowerOn
#define DoPowerOff          uart_enh_PowerOff
#define DoSetConfiguration  uart_enh_SetConfiguration
#define DoConfigureAB       uart_enh_ConfigureAutoBaud
#define DoGetFormat         uart_enh_GetFormatDetails
#define DoDisable           uart_enh_Disable
#define DoSetTransferMode   uart_enh_SetTransferMode

PRIVATE t_uart_error    uart_enh_TransmitxCharacters
                        (
                            IN t_uart_device_id uart_device_id,
                            IN t_uint32         num_of_char_to_be_tx,
                            IN t_uint8          *p_data_char
                        );
PRIVATE t_uart_error    uart_enh_ReceivexCharacters
                        (
                            IN  t_uart_device_id        uart_device_id,
                            IN  t_uint32                num_of_char_to_be_recd,
                            OUT t_uint8                 *p_data_char,
                            OUT t_uint32                *p_num_of_char_recd,
                            OUT t_uart_receive_status   *p_receive_error
                        );
PRIVATE t_uart_error    uart_enh_PowerOn(IN t_uart_device_id uart_device_id);
PRIVATE t_uart_error    uart_enh_PowerOff(IN t_uart_device_id uart_device_id);
PRIVATE t_uart_error    uart_enh_SetTransferMode(IN t_uart_device_id uart_device_id, IN t_uint32 flags);
PRIVATE t_uart_error    uart_enh_SetConfiguration(IN t_uart_device_id uart_device_id, IN t_uart_config *p_config);
PRIVATE t_uart_error    uart_enh_ConfigureAutoBaud
                        (
                            IN t_uart_device_id     uart_device_id,
                            IN t_uart_set_or_clear  uart_action
                        );
PRIVATE t_uart_error    uart_enh_GetFormatDetails
                        (
                            IN  t_uart_device_id    uart_device_id,
                            OUT t_uart_stop_bits    *p_tx_stop_bits,
                            OUT t_uart_data_bits    *p_tx_data_bits,
                            OUT t_uart_parity_bit   *p_tx_parity_bit,
                            OUT t_uart_stop_bits    *p_rx_stop_bits,
                            OUT t_uart_data_bits    *p_rx_data_bits,
                            OUT t_uart_parity_bit   *p_rx_parity_bit,
			    OUT t_uart_baud_rate    *p_baud_rate,
                            OUT t_bool              *p_tx_fifo_enabled,
                            OUT t_bool              *p_rx_fifo_enabled,
                            OUT t_bool              *p_is_autobaud_done
                        );
PRIVATE t_uart_error    uart_enh_Disable(IN t_uart_device_id uart_device_id);

PRIVATE t_uart_error    uart_FillFifoTx
                        (
                            IN  t_uart_device_id    uart_device_id,
                            IN  t_uint8              *p_data_char,
                            IN  t_uint32             num_of_char_to_be_written,
                            OUT t_uint32            *p_num_of_char_written
                        );
PRIVATE t_uart_error    uart_WritexCharacters
                        (
                            IN  t_uart_device_id    uart_device_id,
                            IN  t_uint8              *p_data_char,
                            IN  t_uint32             num_of_char_to_be_written,
                            OUT t_size                *p_num_of_char_written
                        );
PRIVATE t_uart_error    uart_ReadxCharacters
                        (
                            IN  t_uart_device_id        uart_device_id,
                            IN  t_uint32                char_number_to_read,
                            OUT t_uint8                 *p_data_char,
                            OUT t_uint32                *p_char_number_read,
                            OUT t_uart_receive_status   *p_receive_error
                        );
PRIVATE t_uart_error    uart_ProcessIt(INOUT t_uart_irq_status *p_status);

#define UpdateRxTrig(uart_device, val)  ( (uart_device##_trigger_rx) = (val) )
#define UpdateTxTrig(uart_device, val)  ( (uart_device##_trigger_tx) = (val) )
#endif /* __UART_ENHANCED */

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _UART_HWP_H_ */

/* End of file - uart_hwp.h */

