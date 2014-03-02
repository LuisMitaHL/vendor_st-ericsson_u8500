/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSEE_H__
#define __CGPSEE_H__


/**
* \file Cgpsee.h
* \date 19/01/2012
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by cgps4utils.c..\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 19.01.12</TD><TD> Saswatha</TD><TD> Extended Ephemeris </TD>
*     </TR>
* </TABLE>
*/

/* include used to vsnprintf*/
#include <stdio.h>
/* needed for rand() function */
#include <stdlib.h>
#include <math.h>
#include "cgpscommon.hi"


/* RTK file system interface*/
#ifdef __RTK_E__
#include "gfl.h"
#endif /*__RTK_E__*/

/* include internal header*/

#include "cgpsee.hic"
#include "cgpsee.hip"
#include "cgpsee.hiv"

#include "cgpsmain.him"
#include "cgpsmain.hip"

#include "cgpsutils.him"
#include "cgpsutils.hic"
#include "cgpsutils.hit"
#include "cgpsutils.hiv"
#include "cgpsutils.hip"


#include "halgps.h"



#define MC_CGPSEE_TRACE_REF_POS_FROM_MSL(X)           MC_CGPS_TRACE(("CGPS11_04GetRefLocationRsp from MSL: %1d, %3.7f,  %3.7f,  %3.7f,  %8d  %8d  %8d  %6u  %6u  %4u %4u", \
                                                                X.v_Type, X.v_Latitude, X.v_Longitude, X.v_Altitude, X.v_ECEF_X, X.v_ECEF_Y,X.v_ECEF_Z, X.v_UncertSemiMajor,X.v_UncertSemiMinor,X.v_OrientMajorAxis,X.v_Confidence));

#define MC_CGPSEE_TRACE_REF_POS_DATA(X)           MC_CGPS_TRACE(("CGPS11_04GetRefLocationRsp MSL to PE:  %3.7f,  %3.7f,  %3.7f,  %3.7f,  %5d,  %2d,  %3.7f,  %3.7f", \
                                                                  X.Latitude, X.Longitude, X.RMS_SMaj, X.RMS_SMin, X.RMS_SMajBrg, X.Height_OK,X.Height, X.RMS_Height));


BOOL        CGPS11_01EEClientGPSEphemerisCheck(const t_cgps_GPSExtendedEphData *eph,U1 SV );
BOOL CGPS11_02EEClientGlonEphemerisCheck(const t_cgps_GlonassExtendedEphData *eph,uint32_t  v_Requested_GLONASS_secs );
uint64_t    CGPS11_05EEClientTimeDiffCheck(uint64_t EEReqTimestamp);

void CGPS11_06EEClientDeleteSeedData();



#endif /* __CGPSEE_H__ */
