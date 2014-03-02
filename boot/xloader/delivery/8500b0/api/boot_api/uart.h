/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file uart.h
 * \brief Boot uart driver
 * \author STMicroelectronics
 *
 * \addtogroup UART
 *
 * This module provides function to implement the Boot uart driver
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef __INC_BOOT_UART_H
#define __INC_BOOT_UART_H


/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/

#include "boot_types.h"
#include "boot_error.h"


/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/* Give the number of UART interface on which it is possible to boot
    UART1 + UART2 */
#define UART_BOOT_NB 2

/* When doing changes in this enum take care that uart management still works. */
/** UART choice, !! Reflect AF/BSV UART interface selection value */
typedef enum {
    UART1 = 0,    /**< (0) to choose UART1 (GPIO4-5) */
    UART2_B = 1,  /**< (1) to choose UART2 position B (GPIO33-34) */
    UART2_C = 2,  /**< (2) to choose UART2 position C (GPIO75-76) */
    NO_UART = 3   /**< (3) invalid  */
} t_uart_number;


/** Parity definition */
typedef enum {
    NOPARITY_BIT    ,  /**<  (0) */
    EVEN_PARITY_BIT ,  /**<  (1) */
    ODD_PARITY_BIT  ,  /**<  (2) */
    PARITY_0_BIT    ,  /**<  (3) stick parity mode */
    PARITY_1_BIT       /**<  (4) stick parity mode */
} t_uart_parity_bit;

/** Data bits number */
typedef enum {
    DATABITS_5 = 0x0,   /**<  (0x0) */
    DATABITS_6 = 0x1,   /**<  (0x1) */
    DATABITS_7 = 0x2,   /**<  (0x2) */
    DATABITS_8 = 0x3    /**<  (0x3) */
} t_uart_data_bits;

/** Stop bits number*/
typedef enum {
    ONE_STOPBIT = 0x0,   /**<  (0x0) */
    TWO_STOPBITS = 0x1   /**<  (0x1) */
} t_uart_stop_bits;

/** BRDI + BRDF = UartClk/(16*Baudrate) */

/** Baud Rate enumeration for UART_CLK=38.4MHz.*/
typedef enum {
BR110BAUD   =   0x0C553A,   /**<  (0x0C553A) UARTCLK=38.4MHz, BRDI=21818(0x553A), DIVFRAC=12 (BRDF=0.182)*/
BR1200BAUD  =   0x0007D0,   /**<  (0x0007D0) UARTCLK=38.4MHz, BRDI=2000(0x7D0), DIVFRAC=0 (BRDF=0)*/
BR2400BAUD  =   0x0003E8,   /**<  (0x0003E8) UARTCLK=38.4MHz, BRDI=1000(0x3E8), DIVFRAC=0 (BRDF=0)*/
BR9600BAUD  =   0x0000FA,   /**<  (0x0000FA) UARTCLK=38.4MHz, BRDI=250(0xFA), DIVFRAC=0 (BRDF=0)*/
BR38400BAUD =   0x20003E,   /**<  (0x20003E) UARTCLK=38.4MHz, BRDI=62, DIVFRAC=32(0x20) (BRDF=0.5)*/
BR115200BAUD=   0x350014,   /**<  (0x350014) UARTCLK=38.4MHz, BRDI=20, DIVFRAC=53(0x35) (BRDF=0.8)*/
BR230400BAUD=   0x1B000A,   /**<  (0x1B000A) UARTCLK=38.4MHz, BRDI=10, DIVFRAC=27(0x1B) (BRDF=0.417)*/
BR460800BAUD=   0x0D0005,   /**<  (0x0D0005) UARTCLK=38.4MHz, BRDI=5, DIVFRAC=13(0x0D) (BRDF=0.208)*/
BR921600BAUD=   0x270002,   /**<  (0x270002) UARTCLK=38.4MHz, BRDI=2, DIVFRAC=39(0x27) (BRDF=0.604)*/
BR1843200BAUD=  0x130001,   /**<  (0x130001) UARTCLK=38.4MHz, BRDI=1, DIVFRAC=19(0x13) (BRDF=0.302)*/
BR3000000BAUD=  0x330000    /**<  (0x330000) UARTCLK=38.4MHz, BRDI=0, DIVFRAC=51(0x33) (BRDF=0.8) */
} t_uart_baudrate;


/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Function definition
 *----------------------------------------------------------------------*/

PUBLIC void UART_Init (t_uart_number,t_uart_baudrate,t_uart_parity_bit,t_uart_stop_bits,t_uart_data_bits);
PUBLIC void UART_Close (t_uart_number);
PUBLIC void UART_SendData(t_uart_number,t_address,t_size);
PUBLIC void UART_StopTx(t_uart_number);

PUBLIC t_boot_error UART_GetData(t_uart_number,t_address,t_size);
PUBLIC t_boot_error UART_WaitForData(t_uart_number,t_address,t_size);
PUBLIC t_boot_error UART_GetNbBytesReceived(t_uart_number,t_uint32 *);

PUBLIC void UART_ProcessUart0It(void); 
PUBLIC void UART_ProcessUart1It(void);
PUBLIC void UART_ProcessUart2It(void);

PUBLIC t_boot_error UART_TransmitxCharacters(t_uart_number,t_address, t_size);
PUBLIC t_boot_error UART_GetNbBytesTransmitted(t_uart_number,t_uint32 *);

#endif /* __INC_BOOT_UART_H */
/* end of uart.h */
/** @} */
