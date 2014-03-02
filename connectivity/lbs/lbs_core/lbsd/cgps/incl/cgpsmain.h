/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSMAIN_H__
#define __CGPSMAIN_H__
/**
* \file cgpsmain.h
* \date 01/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by cgps0main.c..\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 01.04.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


#include "cgpscommon.hi"


#ifdef __RTK_E__
#define ACCESS_RIGHTS_ALL
#include "tatman.h"
#undef ACCESS_RIGHTS_ALL

#define ACCESS_RIGHTS_MAPI
#include "mea.h"
#undef ACCESS_RIGHTS_MAPI

#include "errhdl.h"
#include "gfl.h"
#include "utser.h"
#endif /*__RTK_E__*/

#ifdef AGPS_TEST_MODE_FTR
/* + LMSqc32518 */
#include "cgpsTestMode.hip"
#include "cgpsTestMode.hec"
#include "cgpsTestMode.het"
#include "cgpsTestMode.hiv"
/* - LMSqc32518 */
#endif /* AGPS_TEST_MODE_FTR */

/* patch upload interface*/
#include "cgpspatch.hip"

/* demo interface */
#include "cgpsdemo.hip"

/* include utils interface */
/* FIXME must be remove with profile management RTK file system interface*/

/*+SYScs46505*/
#include "cgpsutils.h"
/*-SYScs46505*/

#if defined(AGPS_UP_FTR)
/* include supl interface */
#include "cgpssupl.hip"
#include "cgpssupl.hic"
#endif /* AGPS_UP_FTR */

#if defined(AGPS_FTR)
/* include control plane interface */
#include "cgpscp.hip"
#endif /* AGPS_FTR */

#ifdef __RTK_E__
/* include tat interface for message defenition*/
#include "cgpstat.h"
#include "cgpstat.hic"
#include "cgpstat.hiv"
#include "cgpstat.hip"
#endif /*__RTK_E__*/

#include "cgpspatch.hic"
#include "cgpspatch.him"
#include "cgpspatch.hiv"

#ifdef GPS_FREQ_AID_FTR
#include "cgpsgsa.h"
#endif   /* GPS_FREQ_AID_FTR */

#ifdef CMCC_LOGGING_ENABLE
#include "cgpsTestMode.h"
#include "cgpsTestMode.hip"


#endif   /*CMCC_LOGGING_ENABLE*/

#ifdef AGPS_HISTORICAL_DATA_FTR
#include "cgpsHistoricalCell.hi"
#endif /* AGPS_HISTORICAL_DATA_FTR */

#include "cgpsee.h"
#include "cgpsee.hip"
#include "cgpsee.hic"



#endif /* __CGPSMAIN_H__ */
