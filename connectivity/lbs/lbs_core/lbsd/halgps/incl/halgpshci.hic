
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __HALGPSHCI_HIC__
#define __HALGPSHCI_HIC__
/**
* \file halgpshci.hic
* \date 26/05/2009
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
*             <TD> 02.02.2010</TD><TD> Archana.B </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/



/**
* \def HALGPS_XOSGPS_MAX_BUF_READ_LEN
* Maximum length of the Read buffer
*/
#define HALGPS_HCI_MAX_BUF_READ_LEN  4096

/**
* \def HALGPS_XOSGPS_MAX_BUF_WRITE_LEN
* Maximum length of the Write buffer
*/
#define HALGPS_HCI_MAX_BUF_WRITE_LEN 2048

/*Begin Changes for fixing HW0x20 issue 30-Nov-2011 Mohan-194997*/
#define HALGPS_HCI_MAX_READ_LEN     1021   //Changed from 1024 to 1021 : 194997
#define HALGPS_HCI_MAX_WRITE_LEN    1021   //Changed from 1024 to 1021 : 194997
/*End Changes for fixing HW0x20 issue 30-Nov-2011 Mohan-194997*/


#define HALGPS_HCI_HEADER_LEN       3

#define HALGPS_HCI_POLL_TIME_MS     5 
/*  HCI commands 
    
0x01     Host transmit GPS Data (Host -> GPS IP) 
0x02     GPS transmit GPS Data (GPS IP -> Host) 
0x03     Number of completed packets event (GPS IP -> Host) 
0xFD     Arm SS event specific over GPS logical channel (GPS IP -> Host) 
0xFE     Arm SS cmd specific over GPS logical channel (Host -> GPS IP) 

*/

#define HALGPS_HCI_CMD_ACK          0x03

#define HALGPS_HCI_CMD_RECV_DATA    0x02

#define HALGPS_HCI_CMD_SEND_DATA    0x01

#define HALGPS_HCI_CMD_RECV_SS_EVENT 0xFD

#define HALGPS_HCI_CMD_SEND_SS_EVENT 0xFE

#define HALGPS_HCI_CMD_SS_GPS_ENABLE    0x00

#define HALGPS_HCI_CMD_SS_GPS_DISABLE   0x01
#define HALGPS_HCI_MAX_NUM_MISSED_ACKS 50

#define HALGPS_HCI_TX_ACK_MAX_RETRY 10

#define HALGPS_HCI_TX_ACK_MAX_RETRY_2 100

typedef enum
{
    K_HCI_STATE_NOT_CONNECTED,
    K_HCI_STATE_CONNECTED,
    K_HCI_STATE_WAITING_FOR_TXACK
} t_HALGPS_HciState;

typedef enum
{
    K_HCI_GPS_DISABLED,
    K_HCI_GPS_ENABLE_REQ,
    K_HCI_GPS_ENABLED,
    K_HCI_GPS_DISABLE_REQ
} t_HALGPS_HciGpsState;


#ifdef AGPS_LINUX_FTR
#define CG2900_CHAR_DEV_IOCTL_ENTER_DSM             _IOR('U', 215, int)
#define CG2900_CHAR_DEV_IOCTL_EXIT_DSM              _IOR('U', 216, int)
#endif


#ifdef HALGPS_HCI_LOGGING
#define MC_HALGPS_HCI_TRACE_STRING( X )         MC_HALGPS_TRACE_INF X
#define MC_HALGPS_HCI_TRACE_STRING_PARAM( X )   MC_HALGPS_TRACE_INF X 
#else
#define MC_HALGPS_HCI_TRACE_STRING( X )
#define MC_HALGPS_HCI_TRACE_STRING_PARAM( X )
#endif

#endif //__HALGPSHCI_HIC__
