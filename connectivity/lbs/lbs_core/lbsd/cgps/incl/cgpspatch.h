/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSPATCH_H__
#define __CGPSPATCH_H__
/**
* \file cgpspatch.h
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all include file used by cgps2patch.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/
#include <stdio.h>
#include <stdlib.h>
#include "cgpscommon.hi"

#ifdef __RTK_E__
#include "din.h"
#endif /*__RTK_E__*/

/* include trace interface*/
/*+SYScs46505*/
#include "cgpsutils.h"
/*-SYScs46505*/

/*internal include*/
#include "cgpspatch.hic"
#include "cgpspatch.him"
#include "cgpspatch.hip"
#include "cgpspatch.hiv"

#endif /* __CGPSPATCH_H__ */
