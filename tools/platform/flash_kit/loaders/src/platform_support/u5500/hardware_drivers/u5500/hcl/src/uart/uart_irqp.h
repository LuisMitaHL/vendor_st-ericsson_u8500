/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Headers for uart_irq.c for U5500
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _UART_IRQP_H_
#define _UART_IRQP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes                                                                    *
 *--------------------------------------------------------------------------*/
#ifndef _UART_IRQ_H_
#include "uart_irq.h"
#endif

#define UART0_P_ID0                 0x02
#define UART0_P_ID1                 0x08
#define UART0_P_ID2                 0x38
#define UART0_P_ID3                 0x00
#define UART0_CELL_ID0              0x0D
#define UART0_CELL_ID1              0xF0
#define UART0_CELL_ID2              0x05
#define UART0_CELL_ID3              0xB1

#define UART0_FIFO_SIZE             64
#define UART0_AUTO_SW_FLOW_CONTROL

#define UART1_P_ID0                 0x02
#define UART1_P_ID1                 0x08
#define UART1_P_ID2                 0x38
#define UART1_P_ID3                 0x00
#define UART1_CELL_ID0              0x0D
#define UART1_CELL_ID1              0xF0
#define UART1_CELL_ID2              0x05
#define UART1_CELL_ID3              0xB1

#define UART1_FIFO_SIZE             64
#define UART1_AUTO_SW_FLOW_CONTROL

#define UART2_P_ID0                 0x02
#define UART2_P_ID1                 0x08
#define UART2_P_ID2                 0x38
#define UART2_P_ID3                 0x00
#define UART2_CELL_ID0              0x0D
#define UART2_CELL_ID1              0xF0
#define UART2_CELL_ID2              0x05
#define UART2_CELL_ID3              0xB1

#define UART2_FIFO_SIZE             64
#define UART2_AUTO_SW_FLOW_CONTROL

#define UART3_P_ID0                 0x02
#define UART3_P_ID1                 0x08
#define UART3_P_ID2                 0x38
#define UART3_P_ID3                 0x00
#define UART3_CELL_ID0              0x0D
#define UART3_CELL_ID1              0xF0
#define UART3_CELL_ID2              0x05
#define UART3_CELL_ID3              0xB1

#define UART3_FIFO_SIZE             64
#define UART3_AUTO_SW_FLOW_CONTROL


#define UART_ID_SHIFT               16
#define UART_GETDEVICE(num) ((t_uart_device_id) ((num) >> UART_ID_SHIFT & 0x03))

/*    Macros for handling interrupts */
#define UART_AUTOBAUD_INTR_SHIFT    13

/* Interrupt register */
#define UART_IRQ_SRC_RI                     0x0001
#define UART_IRQ_SRC_CTS                    0x0002
#define UART_IRQ_SRC_DCD                    0x0004
#define UART_IRQ_SRC_DSR                    0x0008
#define UART_IRQ_SRC_RX                     0x0010
#define UART_IRQ_SRC_TX                     0x0020
#define UART_IRQ_SRC_RXTO                   0x0040
#define UART_IRQ_SRC_FE                     0x0080
#define UART_IRQ_SRC_PE                     0x0100
#define UART_IRQ_SRC_BE                     0x0200
#define UART_IRQ_SRC_OE                     0x0400
#define UART_IRQ_SRC_XOFF                   0x0800
#define UART_IRQ_SRC_TXFE                   0x1000

/* AUTOBAUD Interrupts */	
#define UART_IRQ_SRC_ABE                    0x01
#define UART_IRQ_SRC_ABD                    0x02
	
#define UART_IRQ_SRC_ALL_IT                 0x1FFF
#define UART_IRQ_SRC_NO_IT                  0x0

#define UART_IRQ_SRC_AUTOBAUD_DONE          0x00004000
#define UART_IRQ_SRC_AUTOBAUD_ERROR         0x00002000

#define UART_IRQ_SRC_DEVICE_ALL_INTERRUPT   0x00007FFF

/* UART registers description *
 *----------------------------*/
typedef volatile struct
{
    /* Type        Name                                 Description                                  Offset*/
    t_uint32    uartx_dr;                        /* Rx and Tx fifo data register                     0x000 */
    t_uint32    uartx_rsr_ecr;                   /* Receive status register                          0x004 */
    t_uint32    uartx_dmawm;                     /* DMA watermark Configure register                 0x008 */
    t_uint32    uartx_timeout;                   /* UART Timeout Period register                     0x00C */
    t_uint32    unused_1[(0x018 - 0x010) >> 2];
    t_uint32    uartx_fr;                        /* Flag register                                    0x018 */
    t_uint32    uartx_lcrh_rx;                   /* Receive line control register                    0x01C */
    t_uint32    uartx_ilpr;                      /* IrDA Low Power Register                          0x020 */
    t_uint32    uartx_ibrd;                      /* Integer baud rate divisor register               0x024 */
    t_uint32    uartx_fbrd;                      /* Fractional baud rate divisor register            0x028 */
    t_uint32    uartx_lcrh_tx;                   /* Transmit Line Control register                   0x02C */
    t_uint32    uartx_cr;                        /* Control register                                 0x030 */
    t_uint32    uartx_ifls;                      /* Interrupt Fifo level select register             0x034 */
    t_uint32    uartx_imsc;                      /* Interrupt mask set/clear register                0x038 */
    t_uint32    uartx_ris;                       /* Raw Interrupt status register                    0x03C */
    t_uint32    uartx_mis;                       /* Masked Interrupt status register                 0x040 */
    t_uint32    uartx_icr;                       /* Interrupt Clear register                         0x044 */
    t_uint32    uartx_dmacr;                     /* Dma Control register                             0x048 */
    t_uint32    unused_2[(0x050 - 0x04C) >> 2];
    t_uint32    uartx_xfcr;                      /* Additional Control register for SFC              0x050 */
    t_uint32    uartx_xon1;                      /* Xon1 character for Software flow control         0x054 */
    t_uint32    uartx_xon2;                      /* Xon2 character for Software flow control         0x058 */
    t_uint32    uartx_xoff1;                     /* Xoff1 character for Software flow control        0x05C */
    t_uint32    uartx_xoff2;                     /* Xoff2 character for Software flow control        0x060 */
    t_uint32    unused_3[(0x080 - 0x064) >> 2];
    t_uint32    uartx_itcr;                      /* Test Control register                            0x080 */
    t_uint32    uartx_itip;                      /* Integration Test Input register                  0x084 */
    t_uint32    uartx_itop;                      /* Integration Test Output register                 0x088 */
    t_uint32    uartx_tdr;                       /* Test Data register                               0x08C */
    t_uint32    unused_4[(0x100 - 0x090) >> 2];
    t_uint32    uartx_abcr;                      /* Autobaud Control register                        0x100 */
    t_uint32    uartx_absr;                      /* Autobaud Status register                         0x104 */
    t_uint32    uartx_abfmt;                     /* Autobaud Format register                         0x108 */
    t_uint32    unused_5[(0x150 - 0x10C) >> 2];
    t_uint32    uartx_abbdr;                     /* Autobaud Baud Divisor register                   0x150 */
    t_uint32    uartx_abbdfr;                    /* Autobaud Baud Divisor Fraction register          0x154 */
    t_uint32    uartx_abbmr;                     /* Autobaud Baud Measurement register               0x158 */
    t_uint32    uartx_abimsc;                    /* Autobaud Baud Interrupt Mask register            0x15C */
    t_uint32    uartx_abris;                     /* Autobaud Baud Raw status register                0x160 */
    t_uint32    uartx_abmis;                     /* Autobaud Baud Masked Status register             0x164 */
    t_uint32    uartx_abicr;                     /* Autobaud Baud Interrupt Clear register           0x168 */
    t_uint32    unused_6[(0xFD8 - 0x16C) >> 2];
    t_uint32    uartx_id_product_h_xy;           /* Identification Product register                  0xFD8 */
    t_uint32    uartx_id_provider;               /* Identification Provider Register                 0xFDC */
    t_uint32    uartx_periphid0;                 /* Peripheral id register: bits 7:0                 0xFE0 */
    t_uint32    uartx_periphid1;                 /* Peripheral id register: bits 15:8                0xFE4 */
    t_uint32    uartx_periphid2;                 /* Peripheral id register: bits 23:16               0xFE8 */
    t_uint32    uartx_periphid3;                 /* Peripheral id register: bits 31:24               0xFEC */
    t_uint32    uartx_cellid0;                   /* PrimeCell id register: bits 7:0                  0xFF0 */
    t_uint32    uartx_cellid1;                   /* PrimeCell id register: bits 15:8                 0xFF4 */
    t_uint32    uartx_cellid2;                   /* PrimeCell id register: bits 23:16                0xFF8 */
    t_uint32    uartx_cellid3;                   /* PrimeCell id register: bits 31:24                0xFFC */
} t_uart_register;



#ifdef __cplusplus
}   /* Allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _UART_IRQP_H_ */

/* End of file - uart_irqp.h */

