/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSCP_H__
#define __CGPSCP_H__
/**
* \file cgpscp.h
* \date 21/07/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by cgps7cp.c\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 21.07.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#include "cgpscommon.hi"

#ifdef __RTK_E__
#include "gfl.h"
#endif /*__RTK_E__*/

#include "lbscfg.h"


/* include internal headers */
#include "cgpscp.hic"
#include "cgpscp.him"
#include "cgpscp.hiv"
#include "cgpscp.hip"

#include "cgpsmain.him"

#include "cgpsutils.him"
#include "cgpsutils.hic"
#include "cgpsutils.hit"
#include "cgpsutils.hiv"
#include "cgpsutils.hip"

/* include HAL header */
#include "halgps.h"

#endif /* __CGPSCP_H__ */
