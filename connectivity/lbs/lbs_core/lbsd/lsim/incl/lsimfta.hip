/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/*F(*************************************************************************** 
* File name  : lsimfta.hip
* Module     : LSIMFTA
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
/*  Balaji   | 02-MAR-2011   | Creation                                      */
/*---------------------------------------------------------------------------*/

/**
\file lsimfta.hip
*/

#ifndef LSIMFTA_HIP
#define LSIMFTA_HIP

#ifdef AGPS_TIME_SYNCH_FTR

void lsim7_01HandleFtaPulseReq( t_RtkObject* p_FsmObject );


void lsim7_02SendFtaPulseReq( void );

void lsim7_03SendFtaPulseCnf( s_gnsFTA_PulseCnf *pp_RefTime );

/* This function stores the Cell Time received in REF_TIME into a local database t_lsim_FTAInfo by checking the Cell type*/
void lsim7_04FtaStoreCelltime(e_gns_RATType v_CellTimeType,u_gnsCP_CellTime *pp_CellTime);

static int32_t lsim7_05FtaCalculateCellDeltatime(s_gnsFTA_PulseCnf * pp_RefTime);

/* These function Calculates the Delta betwenn two sets of Cell time receive from REF TIME and GSM OR WCDMA FTA RESP*/
static int32_t lsim7_06FtaCalculateCellGsmDeltatime(s_gnsCPGsmCellTime *p_GsmGpsTime );

static int32_t lsim7_07FtaCalculateCellWcdmaFddDeltatime(s_gnsCPWcdmaFddCellTime *p_GsmWcdmaTime);

void lsim7_08FtaClearCellTimeInfo(void);

void lsim7_09FtaUpdateRefTime(t_lsimcp_RefTime *pp_RefTime);
void lsim7_10FtaFillCellTimeInMeasRes(s_gnsCP_GpsMeasResult *p_GpsMeasResult);
void lsim7_11FtaFillCellTimeInPosRes(s_gnsCP_GpsPosnResult *p_GpsPosnResult);

static void lsim7_12FtaConvertDeltaToGSMCellTime(int32_t vl_RefDeltaCellTime,s_gnsCPGsmCellTime *p_GsmCellTime);
static void lsim7_13FtaConvertDeltaToWCDMACellTime(int32_t vl_RefDeltaCellTime,uint32_t vl_GpsTow,s_gnsCPWcdmaFddCellTime *p_WcdmaFddCellTime);

static void lsim7_14FtaStorePulsetime(e_gns_RATType v_CellTimeType,u_gnsCP_CellTime *pp_CellTime);


#endif /*AGPS_TIME_SYNCH_FTR*/

#endif /* LSIMFTA_HIP */

