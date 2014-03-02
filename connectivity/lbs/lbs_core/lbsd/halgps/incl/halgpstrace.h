/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSTRACE_H__
#define __HALGPSTRACE_H__
/**
* \file halgpstrace.h
* \date 10/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all include to used trace in HLGPS module\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 10.04.08</TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifdef __RTK_E__

#include "master.h"

/*RTK file system interface*/
#include "gfl.h"
#elif AGPS_LINUX_FTR
#include "agpsosa.h"
#endif /*__RTK_E__*/

#include "halgpstrace.hic"
#include "halgpstrace.hiv"
#include "halgpstrace.him"

#endif /*__HALGPSTRACE_H__*/
