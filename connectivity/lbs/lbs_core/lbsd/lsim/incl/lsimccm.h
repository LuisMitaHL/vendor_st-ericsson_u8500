/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/*F(***************************************************************************
* File name  : lsimccm.h
* Module     : LSIMCC
*----------------------------------------------------------------------------*
*----------------------------------------------------------------------------*
*                             DESCRIPTION
*  This file contains the internal structures used by the module.
*
*
*----------------------------------------------------------------------------*
***************************************************************************)F*/
/* == HISTORY ===============================================================*/
/*  Name     |    Date       |     Action                                    */
/*---------------------------------------------------------------------------*/
/*  Balaji   | 08-MAR-2010   | Creation                                      */
/*---------------------------------------------------------------------------*/

#ifndef LSIMCCM_H
#define LSIMCCM_H

#include "agpsfsm.h"
#include "lsim.h"

/*===================================================================*/
/*=                REQUIRED INTERFACE                               =*/
/*===================================================================*/

#if defined (LSIM0FSM_C) || defined (LSIM6CCGNS_C) 

#define ACCESS_RIGHTS_LSIMCC

#include "gns.h"
#include "gnsInternal.h"

#endif /* LSIM0FSM_C || LSIM2CP_C || LSIM4CPGNS_C */



/*===================================================================*/
/*=                 PROVIDED INTERFACE        =*/
/*===================================================================*/
#include "lsimccm.hec"
#include "lsimccm.het"
#include "lsimccm.hep"


/*===================================================================*/
/*=                   PRIVATE USING            =*/
/*===================================================================*/

#ifdef ACCESS_RIGHTS_LSIMCC

#include "lsimccm.hip"

#undef ACCESS_RIGHTS_LSIMCC

#endif /* ACCESS_RIGHTS_LSIMCC */

#endif /* LSIMCCM_H */

