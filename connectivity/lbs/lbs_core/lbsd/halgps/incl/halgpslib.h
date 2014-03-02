/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSLIB_H__
#define __HALGPSLIB_H__
/**
* \file halgpslib.h
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all include files used by halgps1lib.c.\n
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

#ifdef __RTK_E__

#define ACCESS_RIGHTS_ALL
/*RTK type definition*/
#include "master.h"

/* OS time interface*/
#ifdef UTS_CPR
#include "utser.h"
#else
#include "libostim.h"
#endif

#undef ACCESS_RIGHTS_ALL

/*access to secure clock*/
#include "gsecurity.h"
#elif AGPS_LINUX_FTR
#include "agpsosa.h"
#endif /*__RTK_E__*/

#include "lbscfg.h"

/*GPS lib interface*/
#include "gps_ptypes.h"
#include "GN_GPS_api.h"

/*need to used sqrt function */
#include <math.h>

/*external include*/
#include "halgps.het"
#include "halgps.hec"
#include "halgps.hep"
#include "halgps.hem"

/*internal include*/
#include "halgpslib.hic"
#include "halgpslib.hit"
#include "halgpslib.hiv"

/*UART interface*/
#if defined GPS_OVER_SPI
#include "halgpsspi.hip"
#elif GPS_OVER_UART
#include "halgpsuart.hip"
#elif GPS_OVER_XOSGPS
#include "halgpsxosgps.hic"
#include "halgpsxosgps.hip"
#elif GPS_OVER_HCI
#include "halgpshci.hic"
#include "halgpshci.hip"
#endif

/*file name declaration*/
#include "halgpsmain.h"

/*trace debug MACRO*/
#include "halgpstrace.h"

/*time conversion function*/
#include "halgpstime.hip"

#endif /* __HALGPSLIB_H__ */
