/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/*F(***************************************************************************
* File name  : lsim7fta.h
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

#ifndef LSIMFTA_H
#define LSIMFTA_H

#include "agpsfsm.h"
#include "lsim.h"


/*===================================================================*/
/*=                REQUIRED INTERFACE                               =*/
/*===================================================================*/

#if defined (LSIM0FSM_C) || defined (LSIM7FTA_C)

#include "gns.h"
#include "gnsInternal.h"

#endif /* LSIM0FSM_C || LSIM7FTA_C*/


/*===================================================================*/
/*=                 PROVIDED INTERFACE        =*/
/*===================================================================*/
#include "lsimcp.h"


/*===================================================================*/
/*=                   PRIVATE USING            =*/
/*===================================================================*/
#include "lsimfta.hic"
#include "lsimfta.hit"
#include "lsimfta.hiv"
#include "lsimfta.hip"
#include "lsimfta.him"


#endif /*LSIMFTA_H*/



