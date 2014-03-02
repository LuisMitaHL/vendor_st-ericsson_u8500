/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSDEMO_H__
#define __CGPSDEMO_H__
/**
* \file cgpsdemo.h
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal include file needed by cgps3demo.c..\n
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

#ifdef __CGPS3DEMO_C__

#include "cgpscommon.hi"


#ifdef __RTK_E__
#define ACCESS_RIGHTS_ALL
#include "gfl.h"

/* #include "dbg.h" */
/* cf: /vobs/package226/psdbg/modules/dbgman/incl/tatman.h */
#include "tatman.h"
#undef ACCESS_RIGHTS_ALL
#endif /*__RTK_E__*/

/*internal include*/
#include "cgpsdemo.hic"
#include "cgpsdemo.hiv"

#endif /*__CGPS3DEMO_C__*/
#endif /*__CGPSDEMO_H__*/
