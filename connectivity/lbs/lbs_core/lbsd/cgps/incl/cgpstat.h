/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSTAT_H__
#define __CGPSTAT_H__
/**
* \file cgpstat.h
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all include needed by TAT\n
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

/*!\mainpage NXP - CONFIDENTIAL - Sy.Sol 7200 AGPS : TAT interface for GPS module 
*
* \section document Document information
* <TABLE>
*       <TR BGCOLOR="#000000 WIDTH=100%" span style="color:#ffffff">
*            <TD>Info</TD><TD>Content</TD>
*     </TR>
*     <TR>
*             <TD> <b> Project Name</b> </TD><TD> TIGRIS </TD>
*     </TR>
*     <TR>
*             <TD> <b> Project Id</b> </TD><TD> NA </TD>
*     </TR>
*     <TR>
*             <TD> <b> Document Id </b> </TD><TD> VYn_ps26808 </TD>
*     </TR>
*     <TR>
*             <TD> <b>Author </b></TD><TD> Mathieu BELOU </TD>
*     </TR>
*     <TR>
*             <TD> <b>Compagny</b> </TD><TD> NXP </TD>
*     </TR>
*     <TR>
*             <TD> <b>Department</b> </TD><TD>  BL CS - MST SySol, Le Mans  </TD>
*     </TR>
*     <TR>
*             <TD> <b>Version </b> </TD><TD> 1.0 </TD>
*     </TR>
*     <TR>
*             <TD> <b>Status</b> </TD><TD> Proposed 3 </TD>
*     </TR>
*     <TR>
*             <TD> <b>Date</b> </TD><TD> JULY 4th, 2008 </TD>
*     </TR>
* </TABLE>
*
* \section purpose Purpose and Scope
*      \arg This document describes the interface provided by CGPS for TAT module.
*
*\section references References
*  <TABLE WIDTH=100%>
*     <TR BGCOLOR="#000000 WIDTH=100%" span style="color:#ffffff">
*            <TD>Reference</TD><TD>Id.</TD><TD>Document name</TD><TD>Date Issued</TD><TD>Status</TD><TD>Version</TD>
*     </TR>
*     <TR>
*             <TD>[GN_GPS API] </TD><TD> None</TD><TD> GloNav GPS Library API Documentation </TD><TD> 19-03-2008 </TD><TD> Approved </TD><TD> 1.1 </TD> 
*     </TR>
*     <TR>
*             <TD>[SWAD HALGPS] </TD><TD> VYn_ps26609 </TD><TD>Software Architecture Design HAL GPS Module  </TD><TD> 03-04-2008 </TD><TD> Approved </TD><TD> 1.0 </TD> 
*     </TR>
* </TABLE>
*
*\section definitions Definitions
* <TABLE WIDTH=100%>
*      <TR BGCOLOR="#000000 WIDTH=100%" span style="color:#ffffff">
*            <TD> Definition</TD><TD>Description</TD>
*     </TR>
*     <TR>
*             <TD> HAL </TD><TD> Hardware Abstraction Layer  </TD>
*     </TR>
* </TABLE>
*
*\section revision Revision history
* <TABLE WIDTH=100%>
*      <TR BGCOLOR="#000000 WIDTH=100%" span style="color:#ffffff">
*            <TD>Version</TD><TD>Date</TD><TD>Description/Status</TD><TD>Writer</TD>
*     </TR>
*     <TR>
*             <TD>1.0 Proposed_1</TD><TD> 07/04/2008  </TD><TD> Creation </TD><TD>Mathieu BELOU </TD>
*     </TR>
*     <TR>
*             <TD>1.0 Proposed_2</TD><TD> 29/04/2008  </TD><TD> Update after CGPS_TAT_MODE_REQ/CNF implementation </TD><TD>Yoann DESAPHI </TD>
*     </TR>
*     <TR>
*             <TD>1.0 Proposed_3</TD><TD> 04/07/2008  </TD><TD> Upadte after review</TD><TD>Mathieu BELOU </TD>
*     </TR>
* </TABLE>
*/
/*. Definition for messages  */
/**
*\addtogroup CGPS_TAT
*\{
*/
#if defined(__CGPS6TAT_C__)

#include "cgpscommon.hi"

#define ACCESS_RIGHTS_ALL
#include "din.h"
#undef ACCESS_RIGHTS_ALL

/* include cgpsmain interface*/
#include "cgpsmain.him"
#include "cgpsmain.hip"

#include "cgpstat.hec"
#include "cgpstat.het"

#include "cgpstat.hiv"
#include "cgpstat.hic"
#include "cgpstat.hip"

/*patch interface*/
#include "cgpspatch.hic"
#include "cgpspatch.him"
#include "cgpspatch.hip"
#include "cgpspatch.hiv"

#include "cgpsutils.him"

#include "cgpsTestMode.hip"

/* include HAL GPS interface */
#include "halgps.h"
#else
#include "cgps.hec"
#include "cgps.het"
#include "cgpstat.hec"
#include "cgpstat.het"
#include "cgpstat.hep"
#include "cgpstat.hem"
#endif
/**
*\}
*/
#endif /* __CGPSTAT_H__ */


