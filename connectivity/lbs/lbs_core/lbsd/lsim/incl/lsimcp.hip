/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/*F(*************************************************************************** 
* File name  : lsimcp.hip
* Module     : LSIMCP
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
/*  Balaji   | 16-JUL-2008   | Creation                                      */
/*  Balaji   | 05-AUG-2008   | Modified                                      */
/*  Archana  | 05-AUG-2008   | Modified                                      */
/*  Balaji   | 19-AUG-2008   | Modified                                      */
/*---------------------------------------------------------------------------*/

#ifndef LSIMCP_HIP
#define LSIMCP_HIP

void lsim2_01HandleGpsMsrResultInd(t_RtkObject* p_FsmObject);

void lsim2_02HandleGpsPosInd(t_RtkObject* p_FsmObject);

void lsim2_03HandleGpsAidRequInd(t_RtkObject* p_FsmObject);

void lsim2_04HandleGpsAbortCnf(t_RtkObject* p_FsmObject);

void lsim2_05HandleGpsMsAssistedCnf(t_RtkObject* p_FsmObject);

void lsim2_06HandleGpsMsBasedCnf(t_RtkObject* p_FsmObject);

void lsim2_07HandleGpsLocNotifyCnf(t_RtkObject* p_FsmObject);

/*+LMSqcMOLR*/
void lsim2_08HandleMolrStartInd( t_RtkObject* p_FsmObject );

void lsim2_09HandleMolrStopInd( t_RtkObject* p_FsmObject );

void lsim2_10HandlePosCapabilityInd( t_RtkObject* p_FsmObject );

void lsim2_11HandlePosCapabilityDelay( t_RtkObject* p_FsmObject );

/*-LMSqcMOLR*/



void lsim4_01AidRequest(t_lsimcp_GpsAidRequest *pp_GpsAidRequest );

void lsim4_02MeasResultInd( uint8_t v_SessionId , uint8_t v_FinalReport, t_lsimcp_MsrRes *pp_MsrRes );

void lsim4_03PosnResultInd(uint8_t v_SessionId , uint8_t v_FinalReport, t_lsimcp_GpsPosition *pp_GpsPosition );

void lsim4_15AbortCnf( uint8_t v_SessionId );





#endif /* LSIMCP_HIP */
