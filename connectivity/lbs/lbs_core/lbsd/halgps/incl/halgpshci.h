/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __HALGPSHCI_H__
#define __HALGPSHCI_H__
/**
* \file halgpshci.h
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
*             <TD> 02.02.2010</TD><TD> A.BELVADI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/*.Includes for the whole external required interface
------------------------------------------------------*/
#include <stdio.h>

#ifdef AGPS_LINUX_FTR
#include <sys/poll.h>
/* ++ CG2900 Deep Sleep Mode */
#include <sys/ioctl.h>
/* -- CG2900 Deep Sleep Mode */
#endif

#ifdef __RTK_E__
#include "master.h"
/* cg2900hci driver include */
#include "cg2900hcidriver.h"

/* OS time interface sleep function */
#ifdef UTS_CPR
#include "utser.h"
#else
#include "libostim.h"
#endif /*UTS_CPR*/
#endif


/*include GPS lib interface type are used in halgpsmain.hiv*/
#include "GN_GPS_api.h"
#include "halgpsmain.h"

/*internal include */
#include "halgpshci.hic"
#include "halgpshci.hit"
#include "halgpshci.hip"
#include "halgpshci.hiv"

#endif // __HALGPSHCI_H__
