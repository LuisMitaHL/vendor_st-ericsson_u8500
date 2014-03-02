/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSUART_HIC__
#define __HALGPSUART_HIC__
/**
* \file halgpsuart.hic
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all defines used by halgps2Uart.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08</TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/**
* \def HALGPS_TRACE_DEBUG
*
* if set to 1, active TRACE for DEBUG MODE
* by default is set to 0
*/
#define HALGPS_TRACE_DEBUG 0

/**
* \def HALGPS_TRACE_UART_RCV
*
* if set to 1, trace all data received from UART
* by default is set to 0
*/
#define HALGPS_TRACE_UART_RCV 0

/**
* \def HALGPS_TRACE_UART_SEND
*
* if set to 1, trace all data sent to UART
* by default is set to 0
*/
#define HALGPS_TRACE_UART_SEND 0


/**
* \def HALGPS_UART_PORT_NUMBER
*
* Port number used by HALGPS
* by default HALGPS used the UART 0
*/
#ifndef HALGPS_UART_PORT_NUMBER
#define HALGPS_UART_PORT_NUMBER 0
#endif

/**
* \def HALGPS_UART_INT
*
* Interruption Id used by HALGPS for UART transfers
* by default value is INTHDL_IID_UART0
*/
#if HALGPS_UART_PORT_NUMBER == 0
#define HALGPS_UART_INT INTHDL_IID_UART0
#elif HALGPS_UART_PORT_NUMBER == 1
#define HALGPS_UART_INT INTHDL_IID_UART1
#endif
/* cf inthdl.hec: INTHDL_IID_UART0=24 and INTHDL_IID_UART1=23 */


/**
* \def HALGPS_MAX_RX_BUFFER_NUMBER
*
* Define the number of timeout buffer and it buffer
* by default is set to 8
* \see vg_HALGPS_UARTRxBuffer
*/
#define HALGPS_MAX_RX_BUFFER_NUMBER 8


/**
* \def HALGPS_RX_BUFFER_MAX_SIZE
*
* Define the timeout buffer size
* by default is set to 511
* !!!! this size add to  HALGPS_RX_BUFFER_IT_SIZE must be aligned to 32 bits
* \see vg_HALGPS_UARTRxBuffer
*/
#define HALGPS_RX_BUFFER_MAX_SIZE 511

/**
* \def HALGPS_RX_BUFFER_IT_SIZE
*
* Define the IT buffer size
* by default is set to 1
* It's very dangerous to change this value, because some data can waiting in the UART FIFO
* !!!! this size add to  HALGPS_RX_BUFFER_MAX_SIZE msut be aligned to 32 bits
* \see vg_HALGPS_UARTRxBuffer
*/
#define HALGPS_RX_BUFFER_IT_SIZE  1


/**
* \def HALGPS_TIMEOUT_VALUE
*
* Define the timeout for the timeout buffer.
* By default is set to 0xFFFF.\n
* 0xFFFF corresponding at 25 millisecond with the UART speed set to 115200 bps\n
* This timeout is depending of UART speed.\n
* One increment of this counter corresponding at 1/16 of receive character.
* With a configuration of one bit start, one bit parity and one bit of stop.\n
* the formula is : (8 * HALGPS_TIMEOUT_VALUE)/(uart_speed_in_bps * 16 * ( 8 + 1 +1 +1)) 
*/
#define HALGPS_TIMEOUT_VALUE 0xFFFF

/**
* \def HALGPS_TX_BUFFER_MAX_SIZE
*
* Define the Sending buffer size 
* by default is set to 128
* this size include the IORB size (64 Bytes) and the buffer size
* \see vg_HALGPS_UARTTxBuffer
*/
#define HALGPS_TX_BUFFER_MAX_SIZE 128 

/**
* \def HALGPS_MAX_TX_BUFFER_NUMBER
*
* Define Number of transmission buffer
* by default is set to 32
* the transmission buffer is a circular buffer containing HALGPS_MAX_TX_BUFFER_NUMBER buffer.
* \see vg_HALGPS_UARTTxBuffer
*/
#define HALGPS_MAX_TX_BUFFER_NUMBER 32 


#endif /* __HALGPSUART_HIC__ */
