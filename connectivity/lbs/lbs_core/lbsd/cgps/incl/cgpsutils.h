/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSUTILS_H__
#define __CGPSUTILS_H__
/**
* \file cgpsutils.h
* \date 07/04/2008
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
*             <TD> 07.04.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/* include used to vsnprintf*/
#include <stdio.h>
/* needed for rand() function */
#include <stdlib.h>
#include <math.h>
#include "cgpscommon.hi"

#include "lbscfg.h"


/* RTK file system interface*/
#ifdef __RTK_E__
#include "gfl.h"
#endif /*__RTK_E__*/

/* include internal header*/
#include "cgpsutils.him"
#include "cgpsutils.hic"
#include "cgpsutils.hit"
#include "cgpsutils.hip"
#include "cgpsutils.hiv"

#include "halgps.h"

#if defined(AGPS_FTR)
#include "cgpscp.hiv"
#endif /* AGPS_FTR */

#include "cgpsmain.him"
#include "cgpsmain.hip"

#if defined(AGPS_UP_FTR)
/* include supl interface */
#include "cgpssupl.hip"
#endif /* AGPS_UP_FTR */

#endif /* __CGPSUTILS_H__ */
