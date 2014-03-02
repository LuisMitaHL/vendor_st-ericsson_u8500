/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

 /*F(*************************************************************************** 
* File name  : lsimee.hip
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
/*  Saswata  | 19-JAN-2012   | Creation                                      */
/*---------------------------------------------------------------------------*/

/**
\file lsimee.hip
*/

#ifndef LSIMEE_HIP
#define LSIMEE_HIP


void lsim8_3GetExtendedEphemerisReq(t_RtkObject* p_FsmObject);
void lsim8_4GetRefLocationReq(t_RtkObject* p_FsmObject);
void lsim8_5GetRefTimeReq(t_RtkObject* p_FsmObject);
void lsim8_6DeleteSeedReq(t_RtkObject* p_FsmObject);
void lsim8_7FeedBCEInd(t_RtkObject* p_FsmObject);





#endif /* LSIMEE_HIP */

