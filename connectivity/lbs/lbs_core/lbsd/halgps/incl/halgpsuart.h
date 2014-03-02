/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSUART_H__
#define __HALGPSUART_H__
/**
* \file halgpsuart.h
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by halgps2uart.c..\n
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

#include <stdio.h>

#ifdef AGPS_LINUX_FTR

#include "agpsosa.h"

#endif

#ifdef __RTK_E__

#define ACCESS_RIGHTS_ALL
/*.Includes for the whole external required interface
------------------------------------------------------*/
#include <stdio.h>

#include "master.h"

/* UART dirver interface */
#include "kid.h"
#include "kid3api.h"
#include "kidUART.h"
#include "kidINTC.h"

/*used to map the interruption of UART*/
#ifdef HDL_CLEANUP_INTHDL_CPR
#include "inthdl.h"
#else
#include "kidDMAU.h"
#include "din.h"
#endif

#undef ACCESS_RIGHTS_ALL

#endif /* __RTK_E__ */

/*internal include */
#include "halgpsuart.hic"
#include "halgpsuart.hit"
#include "halgpsuart.hiv"
#include "halgpsuart.hip"

/*trace debug MACRO*/
#include "halgpstrace.h"

/*include GPS lib interface type are used in halgpsmain.hiv*/
#include "GN_GPS_api.h"
#include "halgpsmain.hic"
#include "halgpsmain.hiv"

#endif /* __HALGPSUART_H__ */
