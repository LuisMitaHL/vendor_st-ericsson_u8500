/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSTESTMODE_H__
#define __CGPSTESTMODE_H__
/**
* \file cgpsTestMode.h
* \date 19/11/2009
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
*             <TD> 19.11.09</TD><TD> B NATAKALA </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#include "cgpscommon.hi"
#include <stdio.h> 
#include <string.h>


#ifdef __RTK_E__
#ifdef UTS_CPR
#include "utser.h"
#else
#include "libostim.h"
#endif /*UTS_CPR*/
#endif /* __RTK_E__ */


/* include internal headers */
#include "cgpsTestMode.hic"
#include "cgpsTestMode.him"
#include "cgpsTestMode.hiv"
#include "cgpsTestMode.hip"
#include "cgpsTestMode.hiv"



/*patch interface*/
#include "cgpspatch.hic"
#include "cgpspatch.hip"
#include "cgpspatch.hiv"

#include "cgpsTestMode.hec"
#include "cgpsTestMode.het"



#include "cgpsutils.hic"
#include "cgpsutils.him"
#include "cgpsutils.hit"
#include "cgpsutils.hip"

#endif /* __CGPSTESTMODE_H__ */
