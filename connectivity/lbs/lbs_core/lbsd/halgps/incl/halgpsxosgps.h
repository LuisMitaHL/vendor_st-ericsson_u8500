/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSXOSGPS_H__
#define __HALGPSXOSGPS_H__
/**
* \file halgpsxosgps.h
* \date 25/05/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by halgps8xosgps.c..\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.08</TD><TD> A.BELVADI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/*.Includes for the whole external required interface
------------------------------------------------------*/
#include <stdio.h>

#include "master.h"

#ifdef GPS_OVER_XOSGPS
#include "xosgps.h"
#endif /* GPS_OVER_XOSGPS */

/*internal include */
#include "halgpsxosgps.hic"
#include "halgpsxosgps.hip"
#include "halgpsxosgps.hiv"

/*include GPS lib interface type are used in halgpsmain.hiv*/
#include "GN_GPS_api.h"
#include "halgpsmain.hiv"

#endif // __HALGPSXOSGPS_H__
