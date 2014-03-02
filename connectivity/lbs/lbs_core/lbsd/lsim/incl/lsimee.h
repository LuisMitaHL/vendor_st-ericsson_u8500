/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/*F(***************************************************************************
* File name  : lsimee.h
* Module     : LSIMEE
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
/*  Saswata   | 19-Jan-2012   | Creation                                      */
/*---------------------------------------------------------------------------*/

#ifndef LSIMEE_H
#define LSIMEE_H

#include "agpsfsm.h"
#include "lsim.h"

/*===================================================================*/
/*=                REQUIRED INTERFACE                               =*/
/*===================================================================*/

#if defined (LSIM0FSM_C) || defined (LSIM8EE_C) 

#define ACCESS_RIGHTS_LSIMEE

#include "gns.h"
#include "gnsInternal.h"
#include "gnsEE.h"

#endif 



/*===================================================================*/
/*=                 PROVIDED INTERFACE        =*/
/*===================================================================*/
#include "lsimee.hec"
#include "lsimee.het"
#include "lsimee.hep"


/*===================================================================*/
/*=                   PRIVATE USING            =*/
/*===================================================================*/

#ifdef ACCESS_RIGHTS_LSIMEE

#include "lsimee.hip"

#undef ACCESS_RIGHTS_LSIMEE

#endif /* ACCESS_RIGHTS_LSIMEE */

#endif /* LSIMEE_H */

