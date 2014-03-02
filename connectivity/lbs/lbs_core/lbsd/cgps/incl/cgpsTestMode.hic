/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSTESTMODE_HIC__
#define __CGPSTESTMODE_HIC__
/**
* \file cgpsTestMode.hic
* \date 19/11/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains all defines used by cgps7cp.c.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 19.11.09</TD><TD> BALAJI NATAKALA </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/* + NMEA sentences are sometimes larger than 64 */
/* #define K_CGPS_MAX_NMEA_SIZE 64 */
#define K_CGPS_MAX_NMEA_SIZE 96


/* + LMSQC06481 : Production test */
#ifdef CGPS_CNO_VALUE_FTR
#define CHARTOINT(c) (uint8_t)((c>='0'&&c<='9')?(c-'0'):(c-'7'))
#endif /* CGPS_CNO_VALUE_FTR */
/* - LMSQC06481 : Production test */

#ifdef CMCC_LOGGING_ENABLE

/**
* \def CGPS_SUPL_CMCC_NETWORKCONNECTION_CODE
*
* Define the network connection code used in to generate CMCC log
*/

#define CGPS_SUPL_CMCC_STRING_MAX_LENGHT 256

#define CGPS_SUPL_CMCC_NETWORKCONNECTION_CODE 0x01000000

#define CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_CODE 0x10000000

#define CGPS_SUPL_CMCC_NETWORKCONNECTION_FAILURE_CODE 0x21000000

#define CGPS_SUPL_CMCC_SERVERCONNECTION_CODE 0x01000001

#define CGPS_SUPL_CMCC_SERVERCONNECTION_FAILURE_CODE 0x21000001

#define CGPS_SUPL_CMCC_TLS_FAILURE_CODE 0x21000003

#define CGPS_SUPL_CMCC_POSITION_RESULT_CODE 0x00000003

#define CGPS_SUPL_CMCC_AGPS_SESSION_ENDED 0x00000001

#define CGPS_SUPL_CMCC_AGPS_SESSION_STARTED    0x00000000

#define CGPS_SUPL_CMCC_REPONSE_TIMEOUT_CODE 0x13000000

#define CGPS_SUPL_CMCC_NETWORKCONNECTION_STRING "Network Connection success"

#define CGPS_SUPL_CMCC_SERVERCONNECTION_STRING "Server Connection success"

#define CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_STRING "SET-Initiated session,  no network connection,  fallback to standalone"

#define CGPS_SUPL_CMCC_NETWORKCONNECTION_FAILURE_STRING "Network Connection Failure"

#define CGPS_SUPL_CMCC_SERVERCONNECTION_FAILURE_STRING "Server connection failure"

#define CGPS_SUPL_CMCC_TLS_FAILURE_STRING "TLS Failure during SUPL"

#define CGPS_SUPL_CMCC_CANNOT_PRODUCE_POSITION_WITHIN_RESP_TIME_STRING "GPS cannot produce a position within resp_time"

#define CGPS_SUPL_CMCC_POSITION_QUALITY_NOT_REACHED_STRING "Position quality not reached"

#define CGPS_SUPL_CMCC_AUTONOMOUS_SET_STRING "Autonomous has been set by user, no AGPS session"


#endif //#ifdef CMCC_LOGGING_ENABLE

#define CGPS_TREQ_READDATA_MAX_LENGTH        4097


#endif /* __CGPSTESTMODE_HIC__ */

