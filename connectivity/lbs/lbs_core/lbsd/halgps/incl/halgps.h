/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPS_H__
#define __HALGPS_H__

/**\mainpage NXP - CONFIDENTIAL - Sy.Sol AGPS : HALGPS interface 
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
*             <TD> <b> Document Id </b> </TD><TD> VYn_ps26948 </TD>
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
*             <TD> <b>Version </b> </TD><TD> 1.1 </TD>
*     </TR>
*     <TR>
*             <TD> <b>Status</b> </TD><TD> Approved </TD>
*     </TR>
*     <TR>
*             <TD> <b>Date</b> </TD><TD> January 06th, 2008 </TD>
*     </TR>
* </TABLE>
*
* \section purpose Purpose and Scope
*      \arg This document describes the interface provided by HALGPS. It provides different services used by C-GPS and GPS library .
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
*     <TR>
*             <TD>UART</TD><TD>  Universal Asynchronous Receiver Transmitter  </TD>
*     </TR>
*     <TR>
*             <TD>KID</TD><TD> Kernel Independent Device  </TD>
*     </TR>
* </TABLE>
*
*\section revision Revision history
* <TABLE WIDTH=100%>
*      <TR BGCOLOR="#000000 WIDTH=100%" span style="color:#ffffff">
*            <TD>Version</TD><TD>Date</TD><TD>Description/Status</TD><TD>Writer</TD>
*     </TR>
*     <TR>
*             <TD>1.0 Proposed_1</TD><TD> 16/04/2008  </TD><TD> Creation </TD><TD>Mathieu BELOU </TD>
*     </TR>
*     <TR>
*             <TD>1.1 Proposed</TD><TD> 15/10/2008  </TD><TD> update </TD><TD>Mathieu BELOU </TD>
*     </TR>
*     <TR>
*             <TD>1.1 Approved </TD><TD> 06/01/2009  </TD><TD> approve by GPS team </TD><TD>Mathieu BELOU </TD>
*     </TR>
* </TABLE>
*/

/** \defgroup HLINIT Interface with HLINIT
 *  This module describes the interface between HLINIT (Hardware Layer Initialisation)  and HALGPS (Hardware Layer GPS)
 */
/** \defgroup CGPS Interface with CGPS
 *  This module describes the interface between CGPS (Control GPS) and HALGPS  (Hardware Layer GPS)
 */
/** \defgroup DBGMAN Interface with DBGMAN
 *  This module describes the interface between DBGMAN (Debug manager) and HALGPS (Hardware Layer GPS)
 */
 
#include "halgps.hec"
#include "halgps.het"
#include "halgps.hep"
#include "halgps.hem"

#if defined( GPS_FREQ_AID_FTR ) && defined( __RTK_E__ )
#include "halgpsgsa.h"
#endif /* GPS_FREQ_AID_FTR */

#endif /* __HALGPS_H__*/


