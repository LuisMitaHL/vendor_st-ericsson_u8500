/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/*F(*************************************************************************** 
* File name  : lsimcp.hic
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
/*  Balaji   | 22-JUL-2008   | Creation                                      */
/*  Archana  | 22-JUL-2008   | Modified                                      */
/*  Balaji  | 19-AUG-2008   | Modified                                      */
/*---------------------------------------------------------------------------*/

#ifndef LSIMCP_HIC
#define LSIMCP_HIC

/*CN(**************************************************************************
 *
 * Constant name : LSIMCP_CAP_NOT_SUPPORTED and LSIMCP_CAP_SUPPORTED
 *-----------------------------------------------------------------------------
 * Purpose
 * 
 *-----------------------------------------------------------------------------
**************************************************************************)CN*/
#define LSIMCP_CAP_NOT_SUPPORTED       0
#define LSIMCP_CAP_SUPPORTED           1


/*CN(**************************************************************************
 *
 * Constant name : LSIMCP_CAP_MASK_STANDALONE_LOCATION_METHOD,  LSIMCP_CAP_MASK_UE_BASED_OTDOA,
 * LSIMCP_CAP_MASK_NETWORK_ASSISTED_GPS_MSA, LSIMCP_CAP_MASK_NETWORK_ASSISTED_GPS_MSB,
 * LSIMCP_CAP_MASK_NETWORK_ASSISTED_GPS, LSIMCP_CAP_MASK_GPS_TIMING_OF_CELL_FRAMES_MEASUREMENT,
 * LSIMCP_CAP_MASK_IPDL, LSIMCP_CAP_MASK_RX_TX_TIME_DIFF_TYPE2_MEASUREMENT,
 * LSIMCP_CAP_MASK_GPS_MEAS_VALIDITY_IN_PCH_STATES and LSIMCP_CAP_MASK_SFN_SFN_OTD_TYPE_2_MEASUREMENT
 *-----------------------------------------------------------------------------
 * Purpose
 * CAPABILITY MASK  v_PositionCapability
 *  Bit 0:       Standalone location method(s)
 *               0-Not Supported
 *               1-Supported
 *  Bit 1:       UE based OTDOA supported
 *               0-Not Supported
 *               1-Supported
 *  Bit 2,3:         Network Assisted GPS
 *                   00-None
 *                   01-UE Assisted
 *                   10-UE Based
 *                   11-Both Modes
 *  
 *  Bit 4:       GPS timing of cell frames measurement
 *               0-Not Supported
 *               1-Supported
 *  Bit 5:       IPDL
 *               0-Not Supported
 *               1-Supported
 *  Bit 6:       Rx-Tx time difference type2 measurement
 *               0-Not Supported
 *               1-Supported
 *  Bit 7:       GPS measurement validity in CELL_PCH and URA_PCH states
 *               0-Not Supported
 *               1-Supported
 *  Bit 8:       SFN-SFN observed time difference type 2 measurement
 *  Bit 9-31:        Reserved for Future Use
 *
 *-----------------------------------------------------------------------------
**************************************************************************)CN*/
#define LSIMCP_CAP_MASK_STANDALONE_LOCATION_METHOD               0x00000001  /*Bit 0*/
#define LSIMCP_CAP_MASK_UE_BASED_OTDOA                           0x00000002  /*Bit 1*/
#define LSIMCP_CAP_MASK_NETWORK_ASSISTED_GPS_MSA                 0x00000004  /*Bit 2, LSB for Network Assisted*/
#define LSIMCP_CAP_MASK_NETWORK_ASSISTED_GPS_MSB                 0x00000008  /*Bit 3, MSB for Network Assisted*/
#define LSIMCP_CAP_MASK_NETWORK_ASSISTED_GPS                     0x0000000C  /*Bit 2,3 */
#define LSIMCP_CAP_MASK_GPS_TIMING_OF_CELL_FRAMES_MEASUREMENT    0x00000010  /*Bit 4*/
#define LSIMCP_CAP_MASK_IPDL                                     0x00000020  /*Bit 5*/
#define LSIMCP_CAP_MASK_RX_TX_TIME_DIFF_TYPE2_MEASUREMENT        0x00000040  /*Bit 6*/
#define LSIMCP_CAP_MASK_GPS_MEAS_VALIDITY_IN_PCH_STATES          0x00000080  /*Bit 7*/
#define LSIMCP_CAP_MASK_SFN_SFN_OTD_TYPE_2_MEASUREMENT           0x00000100  /*Bit 8*/


/**************************************************************************************************************************/
/*                                                   Timers declaration                                                   */
/**************************************************************************************************************************/

/*CN(**************************************************************************
 *
 * Constant name : F_LSIMCP_ASSIST_DATA_TIMER_EXPIRY, LSIMCP_ASSIST_DATA_TIMER_EXPIRY
 * and LSIMCP_ASSIST_DATA_TIMEOUT
 *-----------------------------------------------------------------------------
 * Purpose
 * Fix loop timer expiry message definition
 *-----------------------------------------------------------------------------
**************************************************************************)CN*/
#define F_LSIMCP_ASSIST_DATA_TIMER_EXPIRY       MC_PCC_FUNCTION_TYPE(LSIMCP_F_ASSIST_DATA_TIMER, PCC_T_TIM)
#define LSIMCP_ASSIST_DATA_TIMER_EXPIRY         MC_RTK_PROCESS_OPERATION(PROCESS_LSIMCP, F_LSIMCP_ASSIST_DATA_TIMER_EXPIRY)
#define LSIMCP_ASSIST_DATA_TIMEOUT              MC_RTK_PROCESS_TIMER(PROCESS_LSIMCP, 0, LSIMCP_ASSIST_DATA_TIMER_EXPIRY)


/*---------------------------------------------------------------------------*/
/* END                                                                                                                  */
/*---------------------------------------------------------------------------*/


/*CN(**************************************************************************
 *
 * Constant name : t_lsimcp_cap_nwassist_gps
 *-----------------------------------------------------------------------------
 * Purpose
 * Following enum shall be used to define the Capability of Device for Network Assisted GPS support
 *-----------------------------------------------------------------------------
**************************************************************************)CN*/
typedef enum
{
    LSIMCP_CAP_NWASSIST_GPS_NONE,  
    LSIMCP_CAP_NWASSIST_GPS_MSA,  
    LSIMCP_CAP_NWASSIST_GPS_MSB, 
    LSIMCP_CAP_NWASSIST_GPS_BOTH  

} t_lsimcp_cap_nwassist_gps;


/*CN(**************************************************************************
 *
 * Constant name : F_LSIMCP_MODEM_INIT_TIMER_EXPIRY, LSIMCP_MODEM_INIT_TIMER_EXPIRY
 * and LSIMCP_MODEM_INIT_TIMEOUT
 *-----------------------------------------------------------------------------
 * Purpose
 * TO delay initialization of Modem and forward capability
 *-----------------------------------------------------------------------------
**************************************************************************)CN*/

#define K_LSIMCP_MODEM_INIT_TIMER_DURATION 40000 // 40 sec


/* \def F_LSIMCP_MODEM_INIT_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_LSIMCP_MODEM_INIT_TIMER_EXPIRY            MC_PCC_FUNCTION_TYPE(LSIMCP_F_MODEM_INIT_TIMER, PCC_T_TIM)
#define LSIMCP_MODEM_INIT_TIMER_EXPIRY              MC_RTK_PROCESS_OPERATION(PROCESS_LSIMCP, F_LSIMCP_MODEM_INIT_TIMER_EXPIRY)
#define LSIMCP_MODEM_INIT_TIMEOUT                   MC_RTK_PROCESS_TIMER(PROCESS_LSIMCP, 0, LSIMCP_MODEM_INIT_TIMER_EXPIRY)


#endif /*LSIMCP_HIC*/

