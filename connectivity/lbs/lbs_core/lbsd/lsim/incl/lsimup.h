/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
/*****************************************************************************/
/*! \file lsimup.h
 *  \brief Global access to LSIMUP definition
 */
 /*!     \defgroup lsimup_interface LSIMUP interface */

 /*!    \ingroup lsimup_interface
     \defgroup lsimup_Init_DeInit_services List of Init/Deinit services provided to user */
 /*!    \ingroup lsimup_interface
    \defgroup lsimup_Mobile_Network_services List of Mobile and Network services provided to user */
 /*!    \ingroup lsimup_interface
    \defgroup lsimup_SMS_services List of SMS services provided to user */
 /*!    \ingroup lsimup_interface
     \defgroup lsimup_SUPL_services List of SUPL services provided to user */
 /*!    \ingroup lsimup_interface
     \defgroup lsimup_Data_services List of Data transfer services provided to user */
 /*!    \ingroup lsimup_interface
     \defgroup Lsimup_Certificate_Management SUPL server certificate management */


#ifndef LSIMUP_H
#define LSIMUP_H

#include "agpsfsm.h"
#include "lsim.h"

/*======================================================================*/

/*======================================================================*/
/* REQUIRED INTERFACE                                                   */
/*======================================================================*/

/*======================================================================*/
#if defined (LSIM0FSM_C) || defined (LSIM1UP_C) || defined (LSIM3UPGNS_C)

/*======================================================================*/

#define ACCESS_RIGHTS_LSIMUP
#include "gns.h"
#include "gnsInternal.h"
#include "gnsSUPL_Api.h"
#ifdef GNS_CELL_INFO_FTR
#include "gnsCellInfo_Api.h"
#endif
#endif /* defined (LSIM0FSM_C) || defined (LSIM1UP_C) || defined (LSIM3UPGNS_C) */

/*======================================================================*/
/* PROVIDED INTERFACE                                                   */
/*======================================================================*/
#if defined(ACCESS_RIGHTS_CGPS) || defined(LSIMUP_TST_FTR)

#include "lsimup.hec"
#include "lsimup.het"
#endif /* defined(ACCESS_RIGHTS_CGPS) */


/*======================================================================*/
/* PRIVATE USING                                                        */
/*======================================================================*/

/*======================================================================*/
#if defined(ACCESS_RIGHTS_LSIMUP)
/*======================================================================*/

#include "lsimup.hec"
#include "lsimup.het"
#include "lsimup.hep"

#include "lsimup.hip"
#include "lsimup.hiv"

#undef ACCESS_RIGHTS_LSIMUP

#endif /* defined(ACCESS_RIGHTS_LSIMUP) */


/*======================================================================*/
#endif /* LSIMUP_H */
