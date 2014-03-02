/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This File contains Private headers for UART
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _UARTP_H_
#define _UARTP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
#include "uart.h"
#include "uart_irqp.h"

#define UART_RESET_REG(reg)                 (reg = MASK_NULL32)

#define UART0_CHECK_PERIPH_ID_OK      (    (UART0_P_ID0 == g_uart_system_context.p_uart0_register->uartx_periphid0)  \
                                       &&  (UART0_P_ID1 == g_uart_system_context.p_uart0_register->uartx_periphid1)  \
                                       &&  (UART0_P_ID2 == g_uart_system_context.p_uart0_register->uartx_periphid2)  \
                                       &&  (UART0_P_ID3 == g_uart_system_context.p_uart0_register->uartx_periphid3)  \
                                       &&  (UART0_CELL_ID0 == g_uart_system_context.p_uart0_register->uartx_cellid0) \
                                       &&  (UART0_CELL_ID1 == g_uart_system_context.p_uart0_register->uartx_cellid1) \
                                       &&  (UART0_CELL_ID2 == g_uart_system_context.p_uart0_register->uartx_cellid2) \
                                       &&  (UART0_CELL_ID3 == g_uart_system_context.p_uart0_register->uartx_cellid3) \
                                      )

#define UART1_CHECK_PERIPH_ID_OK      (    (UART1_P_ID0 == g_uart_system_context.p_uart1_register->uartx_periphid0)  \
                                       &&  (UART1_P_ID1 == g_uart_system_context.p_uart1_register->uartx_periphid1)  \
                                       &&  (UART1_P_ID2 == g_uart_system_context.p_uart1_register->uartx_periphid2)  \
                                       &&  (UART1_P_ID3 == g_uart_system_context.p_uart1_register->uartx_periphid3)  \
                                       &&  (UART1_CELL_ID0 == g_uart_system_context.p_uart1_register->uartx_cellid0) \
                                       &&  (UART1_CELL_ID1 == g_uart_system_context.p_uart1_register->uartx_cellid1) \
                                       &&  (UART1_CELL_ID2 == g_uart_system_context.p_uart1_register->uartx_cellid2) \
                                       &&  (UART1_CELL_ID3 == g_uart_system_context.p_uart1_register->uartx_cellid3) \
                                      )	
	
#define UART2_CHECK_PERIPH_ID_OK      (    (UART2_P_ID0 == g_uart_system_context.p_uart2_register->uartx_periphid0)  \
                                       &&  (UART2_P_ID1 == g_uart_system_context.p_uart2_register->uartx_periphid1)  \
                                       &&  (UART2_P_ID2 == g_uart_system_context.p_uart2_register->uartx_periphid2)  \
                                       &&  (UART2_P_ID3 == g_uart_system_context.p_uart2_register->uartx_periphid3)  \
                                       &&  (UART2_CELL_ID0 == g_uart_system_context.p_uart2_register->uartx_cellid0) \
                                       &&  (UART2_CELL_ID1 == g_uart_system_context.p_uart2_register->uartx_cellid1) \
                                       &&  (UART2_CELL_ID2 == g_uart_system_context.p_uart2_register->uartx_cellid2) \
                                       &&  (UART2_CELL_ID3 == g_uart_system_context.p_uart2_register->uartx_cellid3) \
                                      )


/* Data register */
#define UART_DATA_MASK_DATA                 0xFF
#define UART_DATA_MASK_FE                   0x100
#define UART_DATA_MASK_PE                   0x200
#define UART_DATA_MASK_BE                   0x400
#define UART_DATA_MASK_OE                   0x800

#define UART_DATA_SHIFT_FE                  0x08
#define UART_DATA_SHIFT_PE                  0x09
#define UART_DATA_SHIFT_BE                  0x0A
#define UART_DATA_SHIFT_OE                  0x0B

/* Receive status register */
#define UART_RX_STATUS_MASK_FE              0x01
#define UART_RX_STATUS_MASK_PE              0x02
#define UART_RX_STATUS_MASK_BE              0x04
#define UART_RX_STATUS_MASK_OE              0x08

/* Timeout register */
#define UART_MASK_TIMEOUT_22_BITS           0x3FFFFF

/* Flag register */
#define UART_FLAG_MASK_CTS                  0x01
#define UART_FLAG_MASK_DSR                  0x02
#define UART_FLAG_MASK_DCD                  0x04
#define UART_FLAG_MASK_BUSY                 0x08
#define UART_FLAG_MASK_RXE                  0x010
#define UART_FLAG_MASK_TXF                  0x020
#define UART_FLAG_MASK_RXF                  0x040
#define UART_FLAG_MASK_TXE                  0x080
#define UART_FLAG_MASK_RI                   0x0100

#define UART_FLAG_SHIFT_CTS                 0x00
#define UART_FLAG_SHIFT_DSR                 0x01
#define UART_FLAG_SHIFT_DCD                 0x02
#define UART_FLAG_SHIFT_BUSY                0x03
#define UART_FLAG_SHIFT_RXE                 0x04
#define UART_FLAG_SHIFT_TXF                 0x05
#define UART_FLAG_SHIFT_RXF                 0x06
#define UART_FLAG_SHIFT_TXE                 0x07
#define UART_FLAG_SHIFT_RI                  0x08

/* Line control register */
#define UART_LINEC_MASK_SB                  0x01
#define UART_LINEC_MASK_PAR                 0x02
#define UART_LINEC_MASK_EVEN                0x04
#define UART_LINEC_MASK_STOP                0x08
#define UART_LINEC_MASK_ENFIFOS             0x010
#define UART_LINEC_MASK_WL                  0x060
#define UART_LINEC_MASK_SPAR                0x080

#define UART_LINEC_SHIFT_SB                 0x0
#define UART_LINEC_SHIFT_PAR                0x01
#define UART_LINEC_SHIFT_EVEN               0x02
#define UART_LINEC_SHIFT_STOP               0x03
#define UART_LINEC_SHIFT_ENFIFOS            0x04
#define UART_LINEC_SHIFT_WL                 0x05
#define UART_LINEC_SHIFT_SPAR               0x07

/* Control register */
#define UART_CONTROL_MASK_UEN               0x01
#define UART_CONTROL_MASK_SIREN             0x02
#define UART_CONTROL_MASK_SIRLP             0x04
#define UART_CONTROL_MASK_OVSFACT           0x08
#define UART_CONTROL_MASK_LOOPB             0x080
#define UART_CONTROL_MASK_TXEN              0x0100
#define UART_CONTROL_MASK_RXEN              0x0200
#define UART_CONTROL_MASK_DTR               0x0400
#define UART_CONTROL_MASK_RTS               0x0800
#define UART_CONTROL_MASK_UDCD              0x01000
#define UART_CONTROL_MASK_URI               0x02000
#define UART_CONTROL_MASK_RTSFLOW           0x04000
#define UART_CONTROL_MASK_CTSFLOW           0x08000

#define UART_CONTROL_SHIFT_UEN              0x00
#define UART_CONTROL_SHIFT_SIREN            0x01
#define UART_CONTROL_SHIFT_SIRLP            0x02
#define UART_CONTROL_SHIFT_OVSFACT          0x03
#define UART_CONTROL_SHIFT_LOOPB            0x07
#define UART_CONTROL_SHIFT_TXEN             0x08
#define UART_CONTROL_SHIFT_RXEN             0x09
#define UART_CONTROL_SHIFT_DTR              0x0A
#define UART_CONTROL_SHIFT_RTS              0x0B
#define UART_CONTROL_SHIFT_UDCD             0x0C
#define UART_CONTROL_SHIFT_URI              0x0D
#define UART_CONTROL_SHIFT_RTSFLOW          0x0E
#define UART_CONTROL_SHIFT_CTSFLOW          0x0F

/* Interrupt fifo level select register */
#define UART_FIFO_LEVEL_MASK_TX             0x07
#define UART_FIFO_LEVEL_MASK_RX             0x38
#define UART_FIFO_THREE_FOURTH_TRIGGER_VAL  48
#define UART_FIFO_TRIGGER_RX_SHIFT          0x03
	
/* Product ID register */
#define UART_PRODUCT_MASK_Y                 0x0000F
#define UART_PRODUCT_MASK_X                 0x000F0
#define UART_PRODUCT_MASK_H                 0x00300
#define UART_PRODUCT_MASK_ID                0x3FC00	
	
#define UART_PRODUCT_SHIFT_Y                0x00
#define UART_PRODUCT_SHIFT_X                0x04
#define UART_PRODUCT_SHIFT_H                0x08
#define UART_PRODUCT_SHIFT_ID               0x0A
	
/* Provider ID Register */
#define UART_PROVIDER_MASK_ID               0x3FFF
#define UART_PROVIDER_SHIFT_ID              0x0000


#define UART_XFCR_MASK_SFCEN        0x01
#define UART_XFCR_MASK_SFRMOD1      0x02
#define UART_XFCR_MASK_SFRMOD2      0x04
#define UART_XFCR_MASK_SFTMOD1      0x08
#define UART_XFCR_MASK_SFTMOD2      0x10
#define UART_XFCR_MASK_XON_ANY      0x20
#define UART_XFCR_MASK_SPEC_CHAR    0x40

#define UART_ISTXFIFOFULL()     ((t_bool) ((lp_uart_register->uartx_fr & UART_FLAG_MASK_TXF) >> UART_FLAG_SHIFT_TXF))
#define UART_ISRXFIFOEMPTY()    ((t_bool) ((lp_uart_register->uartx_fr & UART_FLAG_MASK_RXE) >> UART_FLAG_SHIFT_RXE))
#define UART_ISTXFIFOEMPTY()    ((t_bool) ((lp_uart_register->uartx_fr & UART_FLAG_MASK_TXE) >> UART_FLAG_SHIFT_TXE))

/*----------------------------------Clock is 38.4MHz----------------------------*/
#define UART_CLK                          38400000UL        /* 38.4MHz (Change this to change Baud Rate settings) */

/*--------------------------------Settings for BaudRate-------------------------*/
#define UART_MASK_ALL6                    0x3F

#define UART_BAUDVAL_OVSFACT0(baud_rate)  ( ((UART_CLK>>4)*1000)/(baud_rate) )             /* baud = UARTCLK/(16*Baud_Rate) */
#define UART_BAUDVAL_OVSFACT1(baud_rate)  ( ((UART_CLK>>3)*100)/((baud_rate)/10) )         /* baud = UARTCLK/(8*Baud_Rate) */

#define UART_CONVERT_FBRD(baud, integer)  ((( (baud) - (integer)*1000) * 64 + 500) / 1000) /* IBRD = fractional_part*64 + 0.5 */
#define UART_CONVERT_IBRD(baud)           ( (baud)/1000 )

#define UART_FRACTION_DIV_19200_BAUD      0x00
#define UART_INTEGER_DIV_19200_BAUD       0x7D

#define UART_FRACTION_DIV_38400_BAUD      0x20
#define UART_INTEGER_DIV_38400_BAUD       0x3E

#define UART_FRACTION_DIV_57600_BAUD      0x28
#define UART_INTEGER_DIV_57600_BAUD       0x29
	
#define UART_FRACTION_DIV_115200_BAUD     0x35
#define UART_INTEGER_DIV_115200_BAUD      0x14

/* Synthesis of the 4 fifo trigger values corresponding to the 3 UARTs */
typedef struct
{
    t_uint32 trig0_rx;
    t_uint32 trig0_tx;
    t_uint32 trig1_rx;
    t_uint32 trig1_tx;
    t_uint32 trig2_rx;
    t_uint32 trig2_tx;
} t_uart_fifo_trigger;

/*--------------------------------------------------------------------------*
 * UART Device context used for UART_PowerOn and UART_PowerOff APIs         *
 * Context value to be saved before Power Off and restored after Power On   *
 *--------------------------------------------------------------------------*/
typedef struct
{
    t_uint32    uart_imsc_reg;
} t_uart_device_context;

/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
typedef struct
{
    t_uart_register                 *p_uart0_register;      /* Pointer to UART_DEVICE_ID_0 registers structure */
    t_uart_register                 *p_uart1_register;      /* Pointer to UART_DEVICE_ID_1 registers structure */
    t_uart_register                 *p_uart2_register;      /* Pointer to UART_DEVICE_ID_2 registers structure */
    t_uart_config                   uart0_config;           /* UART_DEVICE_ID_0 configuration */
    t_uart_config                   uart1_config;           /* UART_DEVICE_ID_1 configuration */
    t_uart_config                   uart2_config;           /* UART_DEVICE_ID_2 configuration */
    t_uint32                        uart0_transfer_flags;   /* UART_DEVICE_ID_0 Transfer mode flag */
    t_uint32                        uart1_transfer_flags;   /* UART_DEVICE_ID_1 Transfer mode flag */
    t_uint32                        uart2_transfer_flags;   /* UART_DEVICE_ID_2 Transfer mode flag */
    t_bool                          uart0_is_power_off;     /* UART_DEVICE_ID_0 power management flag */
    t_bool                          uart1_is_power_off;     /* UART_DEVICE_ID_1 power management flag */
    t_bool                          uart2_is_power_off;     /* UART_DEVICE_ID_2 power management flag */

#ifdef __UART_ELEMENTARY
    t_uart_device_context           uart0_device_context;   /* UART_DEVICE_ID_0 Device context */
    t_uart_device_context           uart1_device_context;   /* UART_DEVICE_ID_1 Device context */
    t_uart_device_context           uart2_device_context;   /* UART_DEVICE_ID_2 Device context */
#endif /* __UART_ELEMENTARY */

#ifdef __UART_ENHANCED
    /* Status for the SW Flow Control */
    t_bool                          uart0_is_xon_recd;
    t_bool                          uart0_is_xoff_recd;

    t_bool                          uart1_is_xon_recd;
    t_bool                          uart1_is_xoff_recd;

    t_bool                          uart2_is_xon_recd;
    t_bool                          uart2_is_xoff_recd;

    /* Status to indicate the completion of autobaud */
    t_bool                          uart0_is_autobaud_done;
    t_bool                          uart1_is_autobaud_done;
    t_bool                          uart2_is_autobaud_done;

    /* Transfer details of each uart */
    t_uint32                        uart0_number_of_char_to_be_tx;
    t_uint32                        uart0_number_of_char_to_be_rx;
    t_uint32                        uart1_number_of_char_to_be_tx;
    t_uint32                        uart1_number_of_char_to_be_rx;
    t_uint32                        uart2_number_of_char_to_be_tx;
    t_uint32                        uart2_number_of_char_to_be_rx;

    /* Transmit buffer */
    t_uint8                         *p_uart0_transmit_buffer;
    t_uint8                         *p_uart1_transmit_buffer;
    t_uint8                         *p_uart2_transmit_buffer;

    /* Receive buffer */
    t_uint8                         *p_uart0_receive_buffer;
    t_uint8                         *p_uart1_receive_buffer;
    t_uint8                         *p_uart2_receive_buffer;

    /* Variables to manage current byte number transfered under UART_IT */
    t_uint32                        uart0_nrx;
    t_uint32                        uart0_ntx;
    t_uint32                        uart1_nrx;
    t_uint32                        uart1_ntx;
    t_uint32                        uart2_nrx;
    t_uint32                        uart2_ntx;

    /* Communication status */
    t_uart_it_communication_status  uart0_comm_status;
    t_uart_it_communication_status  uart1_comm_status;
    t_uart_it_communication_status  uart2_comm_status;

    /* Variable to know if a call to UART_SetConfiguration() is required */
    t_bool                          uart0_need_to_call_config;
    t_bool                          uart1_need_to_call_config;
    t_bool                          uart2_need_to_call_config;

    /* Variable to know the receive status for both uart */
    t_uart_receive_status           uart0_rx_status;
    t_uart_receive_status           uart1_rx_status;
    t_uart_receive_status           uart2_rx_status;

    /* For event management */
    t_uart_event                    uart0_event;
    t_uart_event                    uart1_event;
    t_uart_event                    uart2_event;

    /* Variables to store the trigger level of each uart */
    t_uint32                         uart0_trigger_tx;
    t_uint32                         uart0_trigger_rx;
    t_uint32                         uart1_trigger_tx;
    t_uint32                         uart1_trigger_rx;
    t_uint32                         uart2_trigger_tx;
    t_uint32                         uart2_trigger_rx;
#endif /* __UART_ENHANCED */
} t_uart_system_context;

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _UARTP_H_ */

/* End of file - uartp.h */


