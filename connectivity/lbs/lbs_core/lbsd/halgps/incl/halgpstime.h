/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSTIME_H__
#define __HALGPSTIME_H__
/**
* \file halgpstime.h
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all include file used by halgps3time.c.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08</TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifdef AGPS_LINUX_FTR
#include "agpsosa.h"
#endif   /* AGPS_LINUX_FTR */


/*GPS lib type declaration*/
#include "gps_ptypes.h"

/*internal include*/
#include "halgpstime.hiv"
#include "halgpstime.hip"


#endif /* __HALGPSTIME_H__ */
