/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSLIB_H__
#define __CGPSLIB_H__
/**
* \file cgpslib.h
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all include files used by cgps1lib.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#include "cgpscommon.hi"

#define ACCESS_RIGHTS_ALL

#ifdef __RTK_E__
/* OS time interface*/
#ifdef UTS_CPR
#include "utser.h"
#else
#include "libostim.h"
#endif

/*File system interface */
#include "gfl.h"

#undef ACCESS_RIGHTS_ALL

/*access to secure clock*/
#include "gsecurity.h"
#endif /*__RTK_E__*/

/*patch function*/
#include "cgpspatch.hic"
#include "cgpspatch.him"
#include "cgpspatch.hip"
#include "cgpspatch.hiv"

#include "cgpsdemo.hip"

/* cgpsutils interface*/
/*+SYScs46505*/
#include "cgpsutils.h"
/*-SYScs46505*/

#if defined(AGPS_UP_FTR)
/* include supl interface */
#include "cgpssupl.hip"
#endif /* AGPS_UP_FTR */

#endif /* __CGPSLIB_H__ */
