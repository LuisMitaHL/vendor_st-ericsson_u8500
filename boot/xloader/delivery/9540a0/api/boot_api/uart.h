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

/** Baud Rate  enumeration for UART_CLK=26MHz */
typedef enum {
/* Baud Rate Divisor = BDR = UARTCLK/(16 x Baud Rate) * 64 = BRDI*64 + BRDF*64
                        /16 because OVSFACT = 0b
			64 for have 6bit DIVFRAC (0x0-0x3F)
			BRDI = INT(BDR/64)
			BRDF = INT(BDR-BDRI*64+0.5) 0.5 added for rounding*/

/* UARTCLK=26 MHz
==================
baudRATE    DIVINT  (BRDI)          DIVFRAC BRDF
110         14772   (0x39B4)        47      (0x2F)  0x2F39B4
1200        1354    (0x054A)        11      (0x0B)  0x0B054A
2400        677     (0x02A5)        5       (0x05)  0x0502A5
9600        169     (0x00A9)        17      (0x11)  0x1100A9
38400       42      (0x002A)        20      (0x14)  0x14002A
115200      14      (0x000E)        7       (0x07)  0x07000E
230400      7       (0x0007)        3       (0x03)  0x030007
460800      3       (0x0003)        34      (0x22)  0x220003
921600      1       (0x0001)        49      (0x31)  0x310001
1843200     0       (0x0000)        56      (0x38)  0x380000
3000000     0       (0x0000)        35      (0x23)  0x230000
*/
	BR110BAUD     =  0x2F39B4,
	BR1200BAUD    =  0x0B054A,
	BR2400BAUD    =  0x0502A5,
	BR9600BAUD    =  0x1100A9,
	BR38400BAUD   =  0x14002A,
	BR115200BAUD  =  0x07000E,
	BR230400BAUD  =  0x030007,
	BR460800BAUD  =  0x220003,
	BR921600BAUD  =  0x310001,
	BR1843200BAUD =  0x380000,
	BR3000000BAUD =  0x230000
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
