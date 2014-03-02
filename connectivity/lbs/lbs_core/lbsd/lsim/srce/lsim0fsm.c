/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/


#define LSIM0FSM_C

#ifdef AGPS_UP_FTR
#include "lsimup.h"
#endif

#ifdef AGPS_FTR
#include "lsimcp.h"
#endif

#ifdef GPS_FREQ_AID_FTR
#include "lsimccm.h"
#endif
#ifdef AGPS_TIME_SYNCH_FTR
#include "lsimfta.h"
#endif /*AGPS_TIME_SYNCH_FTR*/

#include "lsimee.h"
#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_LSIM

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_LSIM

#undef  FILE_NUMBER
#define FILE_NUMBER     0

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0

/*P(***************************************************************************
 * Procedure name :LSIMUP_0IgnoreMessage
 * Object :
 *----------------------------------------------------------------------------*
 * Input parameters  :
 * -------------------
 *   None
 *
 * Output parameters :
 * -------------------
 *   None
 *
 * Used variables    :
 * -------------------
 *   None
 *
 * Used procedures   :
 * -------------------
 *   None
 *
 *----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*
 *                                    DESCRIPTION
 * This function has to be called when we are not able to manage the message.
 *
 *----------------------------------------------------------------------------*
 ***************************************************************************)P*/
void LSIM_0IgnoreMessage(t_RtkObject* p_FsmObject)
{
    t_MsgHeader* p_MsgHdr = (t_MsgHeader*) p_FsmObject->u_ReceivedObj.p_MessageAddress;

    DEBUG_LOG_PRINT_LEV1(("Ignored Message %u from process %u\n",p_MsgHdr->v_OperationType , p_MsgHdr->v_Source ) );
}



/*P(***************************************************************************
 * Procedure name :LSIMUP_0InitInstance
 * Object : LSIMUP FSM init
 *----------------------------------------------------------------------------*
 * Input parameters  :
 * -------------------
 *   None
 *
 * Output parameters :
 * -------------------
 *   None
 *
 * Used variables    :
 * -------------------
 *   None
 *
 * Used procedures   :
 * -------------------
 *   None
 *
 *----------------------------------------------------------------------------*
 *----------------------------------------------------------------------------*
 *                                    DESCRIPTION
 * This function has to be called before lauching LSIMUP process.
 * It initializes an instance of the process and set the features.
 *----------------------------------------------------------------------------*
 ***************************************************************************)P*/
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
void LSIM0_00Init(void)
{
   DEBUG_LOG_PRINT_LEV1(("LSIM0_00Init : Executed"));
} /* LSIMUP_0InitInstance */

#if defined( AGPS_UP_FTR ) || defined( AGPS_FTR )
MC_RTK_DFSM( a_LSIMInitState )
#ifdef AGPS_UP_FTR
MC_RTK_OFSM (CGPS_LSIMUP_INIT_REQ,                lsim1_01InitReq,                SAME)
MC_RTK_OFSM (CGPS_LSIMUP_DEINIT_REQ,            lsim1_02DeInitReq,                SAME)
MC_RTK_OFSM (CGPS_LSIMUP_ESTABLISH_BEARER_REQ,     lsim1_04EstablishBearerReq,     SAME)
MC_RTK_OFSM (CGPS_LSIMUP_CLOSE_BEARER_REQ,         lsim1_07CloseBearerReq,         SAME)
MC_RTK_OFSM (CGPS_LSIMUP_TCPIP_CONNECT_REQ,        lsim1_05TcpIpConnectReq,        SAME)
MC_RTK_OFSM (CGPS_LSIMUP_TCPIP_DISCONNECT_REQ,    lsim1_06DisconnectReq,            SAME)
MC_RTK_OFSM (CGPS_LSIMUP_SEND_DATA_REQ,            lsim1_08SendDataReq,            SAME)
MC_RTK_OFSM (CGPS_LSIMUP_MOBILE_INFO_REQ,        lsim1_03MobileInfoReq,            SAME)
#endif
#ifdef AGPS_FTR
MC_RTK_OFSM (CGPS_LSIMCP_MSR_RESULT_IND,        lsim2_01HandleGpsMsrResultInd,  SAME )
MC_RTK_OFSM (CGPS_LSIMCP_POS_IND,               lsim2_02HandleGpsPosInd,        SAME )
MC_RTK_OFSM (CGPS_LSIMCP_AID_REQU_IND,          lsim2_03HandleGpsAidRequInd,    SAME )
MC_RTK_OFSM (CGPS_LSIMCP_ABORT_CNF,             lsim2_04HandleGpsAbortCnf,      SAME )
MC_RTK_OFSM (CGPS_LSIMCP_MS_ASSISTED_CNF,       lsim2_05HandleGpsMsAssistedCnf, SAME )
MC_RTK_OFSM (CGPS_LSIMCP_MS_BASED_CNF,          lsim2_06HandleGpsMsBasedCnf,    SAME )
MC_RTK_OFSM (CGPS_LSIMCP_LOC_NOTIFY_CNF,         lsim2_07HandleGpsLocNotifyCnf , SAME)
MC_RTK_OFSM (CGPS_LSIMCP_MOLR_START_IND,         lsim2_08HandleMolrStartInd ,SAME)
MC_RTK_OFSM (CGPS_LSIMCP_MOLR_STOP_IND,         lsim2_09HandleMolrStopInd,         SAME)
MC_RTK_OFSM (CGPS_LSIMCP_POS_CAPABILITY_IND,    lsim2_10HandlePosCapabilityInd, SAME)
MC_RTK_OFSM (LSIMCP_MODEM_INIT_TIMEOUT,         lsim2_11HandlePosCapabilityDelay, SAME)
#ifdef AGPS_TIME_SYNCH_FTR
MC_RTK_OFSM(CGPS_LSIMCP_FTA_PULSE_REQ,          lsim7_01HandleFtaPulseReq, SAME)
#endif
#endif
#ifdef GPS_FREQ_AID_FTR
MC_RTK_OFSM(LSIMCCM_CLOCK_CAL_REQ, lsim5_01HandleClockCalReq, SAME)
#endif

#ifdef AGPS_EE_FTR
MC_RTK_OFSM (CGPS_LSIMEE_GET_EPHEMERIS_REQ,          lsim8_3GetExtendedEphemerisReq,        SAME)
MC_RTK_OFSM (CGPS_LSIMEE_GET_REFLOCATION_REQ,        lsim8_4GetRefLocationReq,        SAME)
MC_RTK_OFSM (CGPS_LSIMEE_GET_REFTIME_REQ,        lsim8_5GetRefTimeReq,        SAME)
MC_RTK_OFSM (CGPS_LSIMEE_DELETE_SEED_REQ,        lsim8_6DeleteSeedReq,        SAME)
MC_RTK_OFSM (CGPS_LSIMEE_FEED_BCE_IND,        lsim8_7FeedBCEInd,        SAME)
#endif
MC_RTK_FFSM( LSIM_0IgnoreMessage,           SAME )
#endif /* #if defined( AGPS_UP_FTR ) || defined( AGPS_FTR ) */
#undef PROCEDURE_NUMBER
#undef LSIM0FSM_C