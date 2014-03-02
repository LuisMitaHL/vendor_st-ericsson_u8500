/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* == HISTORY ===============================================================*/
/*  Name     |    Date       |     Action                                    */
/*---------------------------------------------------------------------------*/
/*  Balaji   | 16-JUL-2008   | Creation                                      */
/*  Balaji   | 05-AUG-2008   | Modified                                      */
/*---------------------------------------------------------------------------*/

/*! \file lsimcp.h
 *  \brief Global access to LSIMCP definition
 */
 /*!     \defgroup lsimcp_interface LSIMCP interface */

 /*!    \ingroup lsimcp_interface
     \defgroup STRUCTURES_TO_GPS List of structures used in messages to CGPS */
 /*!    \ingroup lsimcp_interface
    \defgroup STRUCTURES_FROM_GPS  List of structures used in messages from CGPS */
 /*!    \ingroup lsimcp_interface
    \defgroup MESSAGES_TO_GPS  List of messages from Modem to GPS */
 /*!    \ingroup lsimcp_interface
     \defgroup MESSAGES_FROM_GPS  List of messages from GPS to MODEM */
 /*!    \ingroup lsimcp_interface
     \defgroup Variables_in_LSIMCP  List of variables used in LSIMCP */


#ifndef LSIMCP_H
#define LSIMCP_H

#include "agpsfsm.h"
#include "lsim.h"

/*===================================================================*/
/*=                REQUIRED INTERFACE                               =*/
/*===================================================================*/

#if defined (LSIM0FSM_C) || defined (LSIM2CP_C) || defined (LSIM4CPGNS_C)

#define ACCESS_RIGHTS_LSIMCP

#include "gns.h"
#include "gnsInternal.h"

#endif /* LSIM0FSM_C || LSIM2CP_C || LSIM4CPGNS_C */



/*===================================================================*/
/*=                 PROVIDED INTERFACE        =*/
/*===================================================================*/
#include "lsimcp.hec"
#include "lsimcp.het"
#include "lsimcp.hep"


/*===================================================================*/
/*=                   PRIVATE USING            =*/
/*===================================================================*/

#ifdef ACCESS_RIGHTS_LSIMCP

#include "lsimcp.hic"
#include "lsimcp.him"
#include "lsimcp.hit"
#include "lsimcp.hiv"
#include "lsimcp.hip"

#undef ACCESS_RIGHTS_LSIMCP

#endif /* ACCESS_RIGHTS_LSIMCP */

#endif /* LSIMCP_H */
