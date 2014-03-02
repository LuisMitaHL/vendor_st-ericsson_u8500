/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

 /*F(*************************************************************************** 
* File name  : lsimccm.hip
* Module     : LSIMCCM
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
/*  Balaji   | 18-MAR-2010   | Creation                                      */
/*---------------------------------------------------------------------------*/

/**
\file lsimccm.hip
*/

#ifndef LSIMCCM_HIP
#define LSIMCCM_HIP

void lsim5_01HandleClockCalReq(t_RtkObject* p_FsmObject);

void lsim6_02ClockCalReq( t_lsimccm_ClockCalReqM*  pp_ClockCal  );


#endif /* LSIMCCM_HIP */

