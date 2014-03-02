/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSMAIN_H__
#define __HALGPSMAIN_H__
/**
* \file halgpsmain.h
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by halgps0main.c..\n
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
#endif  /* AGPS_LINUX_FTR */

#include "lbsosatrace.h"

#ifdef __RTK_E__

/* RTK file system interface */
#include "gfl.h"

/* register to secure module (utc corrution) */
#include "gsecurity.h"


/* OS time interface sleep function */
#ifdef UTS_CPR
#include "utser.h"
#else
#include "libostim.h"
#endif /*UTS_CPR*/

/* include GPIO interface to start/stop GPS module */
#include "kid.h"
#include "kid3api.h"
#include "kidGPIO.h"
#include "config_llinit70peripheralGPIO.h"

/* used to activate sysol sleep mode*/
#define ACCESS_RIGHTS_SL3
#include "dpws.h"
#undef ACCESS_RIGHTS_SL3
#endif /*__RTK_E__*/

#include "lbscfg.h"

/* include GPS lib interface */
#include "GN_GPS_api.h"

/* include for file descriptor declaration */
#include "halgpslib.hit"
#include "halgpslib.hiv"

#include "halgpsmain.hic"
#include "halgps.h"

/* include internal header */
#include "halgpsmain.hiv"
#include "halgpsmain.hip"

#ifdef GPS_OVER_SPI
/* include HAL spi header */
#include "halgpsspi.hic"
#include "halgpsspi.hip"
#elif GPS_OVER_UART
/* include HAL uart header */
#include "halgpsuart.hip"
#elif GPS_OVER_XOSGPS
/* include HAL xosgps header */
#include "halgpsxosgps.hic"
#include "halgpsxosgps.hip"
#elif GPS_OVER_HCI
#include "halgpshci.hic"
#include "halgpshci.hip"

#endif


/* include trace variable */
#include "halgpstrace.h"


#endif /* __HALGPSMAIN_H__ */
