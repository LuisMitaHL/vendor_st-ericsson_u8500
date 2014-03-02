/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSMAIN_HIC__
#define __HALGPSMAIN_HIC__
/**
* \file halgpsmain.hic
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all defines used by halgps0Main.c.\n
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


/* FIXME used Share1 for the DEBUG/DEMO SESSION */
/* FIXME The official delivery must used the APP volume */

#ifdef AGPS_LINUX_FTR
/* +LMSqc16476 */
#ifdef GPS_OVER_HCI
#define GN_PORT_GNB              "/dev/cg2900_gnss"
/* -LMSqc16476 */
#elif defined( AGPS_PID0002_FTR )
#define GN_PORT_GNB              "/dev/spidev2.5"
#elif defined( GPS_OVER_UART )
#define GN_PORT_GNB              "/dev/ttyUSB0"
#else
#error No Platform Defined
#endif
#endif   /* AGPS_LINUX_FTR */


/** 
* \enum e_halgps_internal_state  
*
*define the HALGPS State
*
*/
typedef enum 
{
    HALGPS_NOT_INITIALIZED_INTERNAL_STATE,
    HALGPS_INITIALIZED_INTERNAL_STATE,
    HALGPS_INITIALISATION_FAILED_INTERNAL_STATE,
    HALGPS_RUNNING_INTERNAL_STATE,
} e_halgps_internal_state; 

#ifdef MMC_FTR
#define HALGPS_ROOT_PATH "/rem1"
#elif defined( DUAL_OS_GEN_FTR )
#define HALGPS_ROOT_PATH "/sysv"
#elif defined( AGPS_LINUX_FTR )
#define HALGPS_ROOT_PATH "/data"
#else
#define HALGPS_ROOT_PATH "/share1"
#endif

/**
* \def HALGPS_STORE_FILE_NAME
*
* Define the full path file name, where are stored the Non votile data for the GPS library
*/
#define HALGPS_STORE_FILE_NAME  "/GPS_NV_Data.bin"

/**
* \def HALGPS_UTC_CALIBRATION_FILE_NAME
*
* Define the full path file name, where are stored the RTC calibration
*/
#define HALGPS_UTC_CALIBRATION_FILE_NAME  "/GPS_UTC_Calib.bin"


/**
* \def HALGPS_READ_BUF_LEN
* Length of the buffer which will flush the Read buffer  
*/
#define HALGPS_READ_BUF_LEN  4096


#if defined(GPS_OVER_SPI) || defined (GPS_OVER_UART)

/**
* \def HALGPS_GPIO_RESET
*
* Define the GPIO number used to reset the GPS module
* On sysol 7210 and 5218 the gpio A15 is used
*/
#if !( defined(MUX_USAGE_GPS_RST) || defined(MUX_USAGE_AGPS_RESET) )
#define HALGPS_GPIO_RESET  GPIOA15
#else
 #ifdef MUX_USAGE_AGPS_RESET
  #define HALGPS_GPIO_RESET  MUX_USAGE_AGPS_RESET
 #else
  #define HALGPS_GPIO_RESET  MUX_USAGE_GPS_RST
 #endif
#endif

/**
* \def HALGPS_GPIO_POWER
*
* Define the GPIO number used to power up/down the GPS module
* On sysol 7210 and 5218 the gpio A29 is used
*/

#if !( defined(MUX_USAGE_GPS_PWR) || defined(MUX_USAGE_AGPS) || defined(MUX_USAGE_AGPS_ON_OFF) )
#define HALGPS_GPIO_POWER  GPIOA29
#else
 #ifdef MUX_USAGE_AGPS        /* For WAVE C Boards */
  #define HALGPS_GPIO_POWER  MUX_USAGE_AGPS
 #elif MUX_USAGE_AGPS_ON_OFF    /* For WAVE B Boards */
  #define HALGPS_GPIO_POWER  MUX_USAGE_AGPS_ON_OFF
 #else
  #define HALGPS_GPIO_POWER  MUX_USAGE_GPS_PWR
 #endif
#endif


#endif  /* GPS_OVER_SPI || GPS_OVER_UART */

/* +RRR */
#ifdef AGPS_LINUX_FTR
#define HALGPS_SYSTEM_TIME_OFFSET    300000
#endif /* AGPS_LINUX_FTR */
/* -RRR */


#endif /* __HALGPSMAIN_HIC__ */
