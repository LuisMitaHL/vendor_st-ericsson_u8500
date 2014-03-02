/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSSPI_HIC__
#define __HALGPSSPI_HIC__
/**
* \file halgpsspi.hic
* \date 17/07/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all defines used by halgps2spi.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.08</TD><TD> M.COQUELIN </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/**
* \def HALGPS_TRACE_SPI_RCV
*
* if set to 1, trace all data received from SPI
* by default is set to 0
*/
#define HALGPS_TRACE_SPI_RCV 0

/**
* \def HALGPS_TRACE_SPI_SEND
*
* if set to 1, trace all data sent to SPI
* by default is set to 0
*/
#define HALGPS_TRACE_SPI_SEND 0

/**
* \def HALGPS_SPI_PORT_NUMBER
*
* Port number used by HALGPS
* by default HALGPS used the SPI 0
*/
#ifndef HALGPS_SPI_PORT_NUMBER
#define HALGPS_SPI_PORT_NUMBER 0
#endif

#ifdef __RTK_E__

/**
* \def HALGPS_SPI_INT
*
* Interruption Id used by HALGPS for SPI transfers
* by default value is INTHDL_IID_SPI0
*/
#if HALGPS_SPI_PORT_NUMBER == 0
#define HALGPS_SPI_INT INTHDL_IID_SPI0
#elif HALGPS_SPI_PORT_NUMBER == 1
#define HALGPS_SPI_INT INTHDL_IID_SPI1
#endif
/* cf inthdl.hec: INTHDL_IID_SPI0=26 and INTHDL_IID_SPI1=25 */


/**
* \def HALGPS_GPIO_SPI_SS
*
* GPIO number used as SPI SS line (Slave Select)
*/
#if !( defined(MUX_USAGE_GPS_SS) || defined(MUX_USAGE_AGPS_SPI_SS_N) )
#define HALGPS_GPIO_SPI_SS  GPIOA13
#else
 #ifdef MUX_USAGE_AGPS_SPI_SS_N
  #define HALGPS_GPIO_SPI_SS  MUX_USAGE_AGPS_SPI_SS_N
 #else
  #define HALGPS_GPIO_SPI_SS  MUX_USAGE_GPS_SS
 #endif
#endif

/*LMSqb90946*/
/**
* \def HALGPS_GPIO_SPI_DATIO1
*
* GPIO number used as SPI SS line (Slave Select)
*/
#if (defined(HALGPS_SPI_OPTIMIZATION))
  #define HALGPS_GPIO_SPI_DATIO1           GPIOE21 /*default 6710*/
#endif
/*LMSqb90946*/

#endif    /* __RTK_E__ */

/**
* \def HALGPS_TX_BUFFER_MAX_SIZE
*
* Define the tx buffer size
* by default is set to 4096
* \see vg_HALGPS_SpiRxBuffer
*/
#define HALGPS_TX_BUFFER_MAX_SIZE 4096

/**
* \def HALGPS_TX_BUFFER_MAX_SIZE
*
* Define the tx buffer size
* by default is set to 256
* \see vg_HALGPS_SpiRxBuffer
*/
#define HALGPS_RX_BUFFER_MAX_SIZE 256 /* max capability of GPS chips */

/**
* \def HALGPS_MAX_RETRY
*
*/
#define HALGPS_MAX_RETRY  5

/**
* \def HALGPS_SPI_SPEED
* SPI speed in kbits/s
*/
#define HALGPS_SPI_SPEED  610

/**
* \def GPS_DPW_SPI_SELECTOR
*
*/
#if defined (DPW_SPI0)
#   if HALGPS_SPI_PORT_NUMBER == 0
#      define HALGPS_DPW_SPI_SELECTOR DPW_SPI0
#   elif HALGPS_SPI_PORT_NUMBER == 1
#      define HALGPS_DPW_SPI_SELECTOR DPW_SPI1
#   elif HALGPS_SPI_PORT_NUMBER == 2
#      define HALGPS_DPW_SPI_SELECTOR DPW_SPI2
#   endif
#else
#   define HALGPS_DPW_SPI_SELECTOR DPW_SPI
#endif

/**
* \def HALGPS_SPI_PADDING_BUF_LEN
* SPI padding buffer length
*/
#define HALGPS_SPI_PADDING_BUF_LEN  19

#endif //__HALGPSSPI_HIC__
