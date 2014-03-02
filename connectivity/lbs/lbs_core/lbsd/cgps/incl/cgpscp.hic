/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSCP_HIC__
#define __CGPSCP_HIC__
/**
* \file cgpscp.hic
* \date 22/07/2008
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
*             <TD> 22.07.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/**
* \def CGPS_DEFAULT_DELTA_CORE_TIME
*
* define the default time for core time  transmition
*
*/
#define CGPS_DEFAULT_DELTA_CORE_TIME 750


/**
* \def CGPS_DEFAULT_TIMEOUT_ON_SUSPEND
*
* define the default timeout in cases where MSBased/MsAssisted request is received with NoReporting set.
*
*/
#define CGPS_DEFAULT_TIMEOUT_ON_SUSPEND        64000

/**
* \def CGPS_ASSISTANCE_MASK
*
* define the default mask for requesting all assistance data
*
*/
#define CGPS_ASSISTANCE_MASK  ( LSIMCP_AID_REQ_ALMANAC | LSIMCP_AID_REQ_UTC_MODEL | LSIMCP_AID_REQ_IONO_MODEL | LSIMCP_AID_REQ_NAV_MODEL | LSIMCP_AID_REQ_DGPS_CORR | LSIMCP_AID_REQ_REF_LOC | LSIMCP_AID_REQ_REF_TIME | LSIMCP_AID_REQ_ACQ_ASSIST | LSIMCP_AID_REQ_RTI )




#if defined( AGPS_FTR ) && defined( AGPS_TIME_SYNCH_FTR )
#define CGPS_TOW_ROLLOVER_TIME_MS           (7*24*3600*1000)
#define CGPS_FTA_PULSE_REQ_TIME_MS              3950
#endif

#define CGPS_WEEK_ROLLOVER_VALUE                1023
#ifdef AGPS_TIME_SYNCH_FTR
#define CGPS_CP_MAX_TOW_UNC                       95
#else /*AGPS_TIME_SYNCH_FTR*/
#define CGPS_CP_MAX_TOW_UNC                      100
#endif /*AGPS_TIME_SYNCH_FTR*/
#define CGPS_GPS_RUNNING_WITH_ONLY_ASSISTANCE  20000

/* CGPS may send any measurement which is older by max CGPS_MAX_AGE_GPS_MEAS ms if measurements degrade during a session.*/
#define CGPS_MAX_AGE_GPS_MEAS       3000


#endif /* __CGPSCP_HIC__ */
