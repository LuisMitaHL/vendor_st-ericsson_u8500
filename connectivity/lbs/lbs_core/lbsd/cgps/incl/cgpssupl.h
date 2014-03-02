/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSSUPL_H__
#define __CGPSSUPL_H__
/**
* \file cgpssupl.h
* \date 16/05/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by cgps5supl.c..\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 16.05.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifdef __RTK_E__
/* RTK file system interface*/
#include "gfl.h"

/* include gsecurity interface */
#include "gsecurity.h"
#endif /*__RTK_E__ */

/* Needed for log() and floor() functions */
#include <math.h>

#include "cgpscommon.hi"


/* include cgpsmain interface */
#include "cgpsmain.him"
#include "cgpsmain.hip"

/* include cgpssupl interface */
#include "cgpssupl.hic"
#include "cgpssupl.hip"

/* include internal header*/
/*+SYScs46505*/
#include "cgpsutils.h"
/*-SYScs46505*/
#ifdef AGPS_HISTORICAL_DATA_FTR
#include "cgpsHistoricalCell.hi"
#endif /* AGPS_HISTORICAL_DATA_FTR */
/*-SYScs46505*/




#ifdef CMCC_LOGGING_ENABLE
#include "cgpsmain.hiv"
#include "cgpsTestMode.hic"
#include "cgpsTestMode.hip"
#include "cgpsTestMode.hiv"
#endif

#endif /* __CGPSSUPL_H__ */
