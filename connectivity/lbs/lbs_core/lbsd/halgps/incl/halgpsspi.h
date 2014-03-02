/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSSPI_H__
#define __HALGPSSPI_H__
/**
* \file halgpsspi.h
* \date 17/07/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by halgps2spi.c..\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.08</TD><TD> M.COQUELIN </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/*.Includes for the whole external required interface
------------------------------------------------------*/
#include <stdio.h>

#ifdef __RTK_E__

#include "master.h"

/* UART dirver interface */
#include "kid.h"
#include "kid3api.h"
#include "kidSPI.h"
#include "kidGPIO.h"
/*LMSQB90946*/
#include "kidSCON.h"
/*end LMSQB90946*/
#include "config_llinit70peripheralGPIO.h"

/*used to map the interruption of SPI*/
#ifdef HDL_CLEANUP_INTHDL_CPR
#include "inthdl.h"
#else
#include "kidDMAU.h"
#include "din.h"
#endif
#endif /* __RTK_E__ */


#ifdef AGPS_LINUX_FTR
#include "agpsosa.h"
#include "halgps.h"
#endif  /* AGPS_LINUX_FTR */

/*internal include */
#include "halgpsspi.hic"
#include "halgpsspi.hit"
#include "halgpsspi.hiv"
#include "halgpsspi.hip"

/*trace debug MACRO*/
#include "halgpstrace.h"

/*include GPS lib interface type are used in halgpsmain.hiv*/
#include "GN_GPS_api.h"
#include "halgpsmain.hiv"

#endif // __HALGPSSPI_H__
