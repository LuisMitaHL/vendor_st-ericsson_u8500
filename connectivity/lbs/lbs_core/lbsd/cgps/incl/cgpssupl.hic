/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSSUPL_HIC__
#define __CGPSSUPL_HIC__
/**
* \file cgpssupl.hic
* \date 22/05/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all constants used by cgps5supl.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 16.05.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifdef DUAL_OS_GEN_FTR
#define CGPS_SUPL_SMS_PAYLOAD_FILE_NAME  "/sysv/SUPL_SMS_payload.bin"
#else
#define CGPS_SUPL_SMS_PAYLOAD_FILE_NAME  "/share1/SUPL_SMS_payload.bin"
#endif

/**
* \def CGPS_SUPL_SERVER_FILE_NAME
*
* Define the full path file name, where are stored informations about the server used for SUPL demo
*/

#ifdef DUAL_OS_GEN_FTR
#define CGPS_SUPL_SERVER_FILE_NAME  "/sysv/SUPL_demo_server.cfg"
#elif AGPS_LINUX_FTR
#define CGPS_SUPL_SERVER_FILE_NAME  "/etc/SUPL_demo_server.cfg"
#else
#define CGPS_SUPL_SERVER_FILE_NAME  "/share1/SUPL_demo_server.cfg"
#endif

/* +LMSqc03064 */

/**
* \def CGPS_SUPL_DEFAULT_APN
*
* Define the APN used if the file SUPL_demo_server.cfg is not found
*/
#define CGPS_SUPL_DEFAULT_APN "Spirent"

/**
* \def CGPS_SUPL_DEFAULT_LOGIN
*
* Define the login used if the file SUPL_demo_server.cfg is not found
*/
#define CGPS_SUPL_DEFAULT_LOGIN "Spirent"

/**
* \def CGPS_SUPL_DEFAULT_PWD
*
* Define the password used if the file SUPL_demo_server.cfg is not found
*/
#define CGPS_SUPL_DEFAULT_PWD "Spirent"

/**
* \def CGPS_SUPL_DEFAULT_IPADDRESS
*
* Define the IP address used if the file SUPL_demo_server.cfg is not found
*/

#define CGPS_SUPL_DEFAULT_IPADDRESS "192.168.0.35"

/**
* \def CGPS_SUPL_DEFAULT_FQDN
*
* Define the IP address used if the file SUPL_demo_server.cfg is not found
*/

#define CGPS_SUPL_DEFAULT_FQDN "www.spirent-lcs.com"

/* -LMSqc03064 */

/**
* \def CGPS_SUPL_DEFAULT_PORT
*
* Define the port used if the file SUPL_demo_server.cfg is not found
*/
#define CGPS_SUPL_DEFAULT_PORT  7275

/**
* \def CGPS_SUPL_CONNECTION_LATENCY
*
* Define the default estimated time to establish a connection
*/
#define CGPS_SUPL_CONNECTION_LATENCY 8

/* + LMSqc32881 */
#define CGPS_SUPL_DEFAULT_HOR_ACC       25 /* This corresponds to horizontal acc of 100m  k = log( (hor_acc_in_m/10) + 1 ) / log ( 1.1 ) */
#define CGPS_SUPL_DEFAULT_VER_ACC       47 /* This corresponds to vertical acc of 100m k = log( (ver_acc_in_m/45) + 1 ) / log ( 1.025 ) */
#define CGPS_SUPL_DEFAULT_DELAY         7  /* This corresponds to 128s delay 2^7 */
#define CGPS_SUPL_DEFAULT_MAX_LOC_AGE   10 /* Dont want fix older than 10s */
/* - LMSqc32881 */


/*+LMSqc38060 */        
#define CGPS_SUPL_MAX_CONNECTION_SETUP_TIME    30000 //30 sec
#define CGPS_SUPL_MAX_BEARER_SETUP_TIME        30000 //30 sec
/*-LMSqc38060 */        

#endif /* __CGPSSUPL_HIC__ */
