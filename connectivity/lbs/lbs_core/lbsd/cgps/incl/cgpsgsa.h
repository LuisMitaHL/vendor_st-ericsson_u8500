/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSGSA_H__
#define __CGPSGSA_H__
/**
* \file cgpsgsa.h
* \date 23/07/2009
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
*             <TD> 21.07.08</TD><TD> Anshuman Pandey</TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/
#ifdef __RTK_E__
#include "master.h"
#else
#include "gnsInternal.h"
#endif
/* include internal headers */

#include "cgpsgsa.hic"
#include "cgpsgsa.hip"
#include "cgpsgsa.hiv"

#include "cgpsutils.h"

#ifdef __RTK_E__
/* include HAL header */
#include "halgpsgsa.h"
#else
#include "lsimccm.h"
#endif

#endif /* __CGPSGSA_H__ */

