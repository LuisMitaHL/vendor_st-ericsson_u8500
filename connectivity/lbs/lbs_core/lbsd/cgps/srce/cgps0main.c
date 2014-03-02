/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps0main.c
* \date 01/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains system entry function for RTK process and FSM table
* All message receive by the other module are receive in this file\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 01.04.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 07.05.08</TD><TD> Y.DESAPHI </TD><TD> SUPL implementation </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS0MAIN_C__

#include "cgpsmain.h"





#ifdef WIN32
#pragma warning(disable : 4312)
#endif

#undef  MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef  FILE_NUMBER
#define FILE_NUMBER 0

/* ======================================================== */
/* STATE : CGPS FSM TABLE                                   */
/* ======================================================== */

/************************************************************************************************/
/*********************************** a_CGPSInitState ********************************************/
/************************************************************************************************/

MC_RTK_DFSM (a_CGPSInitState )

/* +LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
#if defined (DUAL_OS_GEN_FTR) || defined (CR_LMSQC18522_CPR)
/* -LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
MC_RTK_OFSM( CGPS_HWL_INIT_IND,                      CGPS0_60HwlInitInd,                    SAME )
#endif

/* + LMSqc32518 */
#ifdef CGPS_UPLOAD_PATCH_AT_INIT_FTR
MC_RTK_OFSM( CGPS_INITIALIZED_IND,                   CGPS0_64UploadPatch,                   UNKNOWN)
#endif /*CGPS_UPLOAD_PATCH_AT_INIT_FTR*/

#if defined AGPS_TEST_MODE_FTR && defined CGPS_CNO_VALUE_FTR
MC_RTK_OFSM( CGPS_GET_CNO_VALUE_REQ,                 CGPS9_04HandleGetCnoMessage,           SAME )
#endif /*AGPS_TEST_MODE_FTR && CGPS_CNO_VALUE_FTR*/
/* - LMSqc32518 */

#ifdef __RTK_E__
MC_RTK_OFSM( CGPS_UTC_CORRUPT_IND,                   CGPS0_03UTCCorrupted,                  SAME )
MC_RTK_OFSM( MAPI_MEA_AVAILABLE_DIR_IND,             CGPS0_37DirectoryReady,                SAME )
MC_RTK_OFSM( MAPI_MEA_GET_APPLICATION_PATH_CNF,      CGPS0_38GetDirectoryPathCnf,           SAME )
MC_RTK_OFSM( MAPI_MEA_REMOVE_DIR_IND,                CGPS0_39RemoveDirInd,                  SAME )
MC_RTK_OFSM( CGPS_TAT_INIT_CNF,                      CGPS6_13TATInitCnf,                    a_CGPSTATState )
#endif /*__RTK_E__*/

MC_RTK_OFSM( CGPS_INIT_IND,                          CGPS0_41InitInd,                       SAME )
MC_RTK_OFSM( CGPS_NEW_NAF_REGISTERED_IND,            CGPS0_18NewNafRegistered,              UNKNOWN )
MC_RTK_OFSM( CGPS_NAF_DEREGISTERED_IND,              CGPS0_19NafDeregistered,               SAME )
MC_RTK_OFSM( CGPS_UPDATE_USER_CONFIG,                CGPS0_46UserConfigurationReceived,     SAME )
MC_RTK_OFSM( CGPS_UPDATE_PLATFORM_CONFIG,            CGPS0_70PlatformConfigurationReceived, SAME )
MC_RTK_OFSM( CGPS_UPDATE_TRACE_CONFIG,               CGPS0_81TraceConfigurationReceived,    SAME )

/*+ LMSqc13768 : DeleteGpsData */
MC_RTK_OFSM( CGPS_DELETE_GPS_DATA_REQ,               CGPS0_63DeleteNVStoreFieldsReq,        SAME)
/*- LMSqc13768 : DeleteGpsData */

#ifdef AGPS_UP_FTR
MC_RTK_OFSM( CGPS_LSIMUP_INIT_CNF,                   CGPS0_26AdpInitCnf,                    SAME )
MC_RTK_OFSM( CGPS_LSIMUP_MOBILE_INFO_CNF,            CGPS5_05AdpMobileInfo,                 SAME )
MC_RTK_OFSM( CGPS_LSIMUP_MOBILE_INFO_IND,            CGPS5_05AdpMobileInfo,                 SAME )
MC_RTK_OFSM( CGPS_LSIMUP_SMS_PUSH_IND,               CGPS0_25SmsPushInd,                    UNKNOWN )
/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
MC_RTK_OFSM( CGPS_SUPL_PUSH_IND,                     CGPS0_56WapPushInd,                    UNKNOWN )
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */
/* +SUPLv2.0 Modifications */
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY0,   CGPS5_39HandleEventSuplTimer0,         SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY1,   CGPS5_40HandleEventSuplTimer1,         SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY2,   CGPS5_41HandleEventSuplTimer2,         SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY3,   CGPS5_42HandleEventSuplTimer3,         SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY4,   CGPS5_43HandleEventSuplTimer4,         SAME )
MC_RTK_OFSM( CGPS_LOCATION_RETRIEVAL_REQ,            CGPS0_77ThirdPartyLocReq,              UNKNOWN )

/* -SUPLv2.0 Modifications */
#endif /* AGPS_UP_FTR */

#ifdef AGPS_FTR
MC_RTK_OFSM( CGPS_LSIMCP_MS_ASSISTED_REQ,            CGPS7_00MsAssistedReq,                 UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_MS_BASED_REQ,               CGPS7_01MsBasedReq,                    UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_DATA_A_REQ,          CGPS7_05AssistanceDataAReq,            UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_TIME_REQ,                   CGPS7_06TimeReq,                       UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_REF_POS_REQ,                CGPS7_07RefPosReq,                     UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_DATA_B_REQ,          CGPS7_08AssistanceDataBReq,            UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ALMANAC_DATA_REQ,           CGPS7_09AlmanacDataReq,                UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_IONO_MODEL_REQ,      CGPS7_10AssistIonoModelReq,            UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_DGPS_CORRECTION_REQ, CGPS7_11AssistDGPSCorrectionReq,       UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_UTC_MODEL_REQ,       CGPS7_12AssistUTCModelReq,             UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_RTI_REQ,             CGPS7_13AssistRTIReq,                  UNKNOWN )
MC_RTK_OFSM( CGPS_EMERGENCY_START_REQ,               CGPS7_14ECallStart,                    UNKNOWN )
MC_RTK_OFSM( CGPS_EMERGENCY_STOP_REQ,                CGPS7_15ECallStop,                     UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_LOC_NOTIFY_REQ,             CGPS7_23NotifyReq,                     UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_RESET_GPS_DATA_REQ,         CGPS7_27ResetUEPosition,               UNKNOWN )
#endif /* AGPS_FTR */

#ifdef AGPS_TIME_SYNCH_FTR
MC_RTK_OFSM( CGPS_LSIMCP_FTA_PULSE_CNF,              CGPS7_43FtaPulseCnf,                   UNKNOWN )
#endif

#ifdef AGPS_TEST_MODE_FTR
MC_RTK_OFSM( CGPS_PRODUCTION_TEST_START_REQ,         CGPS9_30ProductionTestStartReq,        SAME )
MC_RTK_OFSM( CGPS_PRODUCTION_TEST_STOP_REQ,          CGPS9_31ProductionTestStopReq,         SAME )
#endif /* AGPS_TEST_MODE_FTR */

MC_RTK_OFSM( CGPS_LSIMEE_GET_EPHEMERIS_RSP,          CGPS11_03ExtendedEphRsp,               UNKNOWN )

/*XYBRID Integration :194997*/
MC_RTK_OFSM( CGPS_LSIMEE_GET_REFLOCATION_RSP,        CGPS11_04GetRefLocationRsp,            UNKNOWN )
/*XYBRID Integration :194997*/

MC_RTK_FFSM( CGPS0_02DefaultMessage, SAME)


/***********************************************************************************************/
/*********************************** a_CGPSRunState ********************************************/
/***********************************************************************************************/
MC_RTK_DFSM( a_CGPSRunState )
/* +LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
#if defined (DUAL_OS_GEN_FTR) || defined( CR_LMSQC18522_CPR )
/* -LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
MC_RTK_OFSM (CGPS_HWL_INIT_IND,                     CGPS0_60HwlInitInd,                     SAME )
#endif

MC_RTK_OFSM( CGPS_FIX_LOOP_TIMER_EXPIRY,            CGPS0_01timerExpired,                   SAME )
MC_RTK_OFSM( CGPS_INIT_IND,                         CGPS0_41InitInd,                        SAME )
MC_RTK_OFSM( CGPS_NEW_NAF_REGISTERED_IND,           CGPS0_18NewNafRegistered,               SAME )
MC_RTK_OFSM( CGPS_NAF_DEREGISTERED_IND,             CGPS0_19NafDeregistered,                SAME )
MC_RTK_OFSM( CGPS_FIX_REQ_IND ,                     CGPS0_40FixRequestedInd,                SAME )
MC_RTK_OFSM( CGPS_UPDATE_USER_CONFIG,               CGPS0_46UserConfigurationReceived,      SAME )
MC_RTK_OFSM( CGPS_UPDATE_PLATFORM_CONFIG,           CGPS0_70PlatformConfigurationReceived,  SAME )
MC_RTK_OFSM( CGPS_UPDATE_TRACE_CONFIG,              CGPS0_81TraceConfigurationReceived,     SAME )
MC_RTK_OFSM( CGPS_SAVE_NV_STORE_REQ,                CGPS0_49SaveNVStoreRequested,           SAME )
MC_RTK_OFSM( CGPS_SLEEP_TIMER_EXPIRY,               CGPS0_14SleepExpired,                   SAME )
MC_RTK_OFSM( CGPS_NMEA_TEST_CONTROL_IND,            CGPS3_00demoSetReceivePacket,           SAME )
/*+ LMSqc13768 : DeleteGpsData */
MC_RTK_OFSM (CGPS_DELETE_GPS_DATA_REQ,              CGPS0_63DeleteNVStoreFieldsReq,         SAME )
/*+ LMSqc13768 : DeleteGpsData */

#ifdef __RTK_E__
MC_RTK_OFSM( CGPS_UTC_CORRUPT_IND,                  CGPS0_03UTCCorrupted,                   SAME )
MC_RTK_OFSM( MAPI_MEA_AVAILABLE_DIR_IND,            CGPS0_37DirectoryReady,                 SAME )
MC_RTK_OFSM( MAPI_MEA_GET_APPLICATION_PATH_CNF,     CGPS0_38GetDirectoryPathCnf,            SAME )
MC_RTK_OFSM( MAPI_MEA_REMOVE_DIR_IND,               CGPS0_39RemoveDirInd,                   SAME )
#endif /*__RTK_E__*/

#if defined(AGPS_FTR) || defined(AGPS_UP_FTR)
MC_RTK_OFSM( CGPS_USER_NOTIFY_ANS,                  CGPS0_23HandleUserAnswer,               SAME )
MC_RTK_OFSM( CGPS_MOLF_START_IND,                   CGPS0_68MolfStartReq,                   UNKNOWN )
#endif /* AGPS_FTR || AGPS_UP_FTR */

#ifdef AGPS_UP_FTR
MC_RTK_OFSM( CGPS_LSIMUP_SMS_PUSH_IND,              CGPS0_25SmsPushInd,                     SAME )
/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
MC_RTK_OFSM( CGPS_SUPL_PUSH_IND,                    CGPS0_56WapPushInd,                     SAME )
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */
MC_RTK_OFSM( CGPS_LSIMUP_MOBILE_INFO_CNF,           CGPS5_05AdpMobileInfo,                  SAME )
MC_RTK_OFSM( CGPS_LSIMUP_MOBILE_INFO_IND,           CGPS5_05AdpMobileInfo,                  SAME )
MC_RTK_OFSM( CGPS_LSIMUP_ESTABLISH_BEARER_CNF,      CGPS5_02EstablishBearerCnf,             SAME )
MC_RTK_OFSM( CGPS_LSIMUP_ESTABLISH_BEARER_ERR,      CGPS5_03EstablishBearerErr,             SAME )
MC_RTK_OFSM( CGPS_LSIMUP_TCPIP_CONNECT_CNF,         CGPS5_04TcpIpConnectCnf,                SAME )
MC_RTK_OFSM( CGPS_LSIMUP_TCPIP_CONNECT_ERR,         CGPS5_09TcpIpConnectErr,                SAME )
MC_RTK_OFSM( CGPS_LSIMUP_RECEIVE_DATA_IND,          CGPS5_08ReceiveDataInd,                 SAME )
MC_RTK_OFSM( CGPS_LSIMUP_TCPIP_DISCONNECT_CNF,      CGPS5_11TcpIpDisconnectCnf,             SAME )
MC_RTK_OFSM( CGPS_LSIMUP_TCPIP_DISCONNECT_ERR,      CGPS5_14TcpIpDisconnectErr,             SAME )
MC_RTK_OFSM( CGPS_LSIMUP_TCPIP_DISCONNECT_IND,      CGPS5_12TcpIpDisconnectInd,             SAME )
MC_RTK_OFSM( CGPS_LSIMUP_CLOSE_BEARER_CNF,          CGPS5_06CloseBearerCnf,                 SAME )
MC_RTK_OFSM( CGPS_LSIMUP_CLOSE_BEARER_ERR,          CGPS5_15CloseBearerErr,                 SAME )
MC_RTK_OFSM( CGPS_LSIMUP_CLOSE_BEARER_IND,          CGPS5_07CloseBearerInd,                 SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY0,  CGPS5_39HandleEventSuplTimer0,          SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY1,  CGPS5_40HandleEventSuplTimer1,          SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY2,  CGPS5_41HandleEventSuplTimer2,          SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY3,  CGPS5_42HandleEventSuplTimer3,          SAME )
MC_RTK_OFSM( CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY4,  CGPS5_43HandleEventSuplTimer4,          SAME )
MC_RTK_OFSM( CGPS_LOCATION_RETRIEVAL_REQ,           CGPS0_77ThirdPartyLocReq,               UNKNOWN )
#endif /* AGPS_UP_FTR */

#ifdef AGPS_FTR
MC_RTK_OFSM( CGPS_LSIMCP_MS_ASSISTED_REQ,            CGPS7_00MsAssistedReq,                 SAME )
MC_RTK_OFSM( CGPS_LSIMCP_MS_BASED_REQ,               CGPS7_01MsBasedReq,                    SAME )
MC_RTK_OFSM( CGPS_LSIMCP_ABORT_REQ,                  CGPS7_02AbortReq,                      SAME )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_DATA_A_REQ,          CGPS7_05AssistanceDataAReq,            UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_TIME_REQ,                   CGPS7_06TimeReq,                       UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_REF_POS_REQ,                CGPS7_07RefPosReq,                     UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_DATA_B_REQ,          CGPS7_08AssistanceDataBReq,            UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ALMANAC_DATA_REQ,           CGPS7_09AlmanacDataReq,                UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_IONO_MODEL_REQ,      CGPS7_10AssistIonoModelReq,            UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_DGPS_CORRECTION_REQ, CGPS7_11AssistDGPSCorrectionReq,       UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_UTC_MODEL_REQ,       CGPS7_12AssistUTCModelReq,             UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_ASSIST_RTI_REQ,             CGPS7_13AssistRTIReq,                  UNKNOWN )
MC_RTK_OFSM( CGPS_EMERGENCY_START_REQ,               CGPS7_14ECallStart,                    SAME )
MC_RTK_OFSM( CGPS_EMERGENCY_STOP_REQ,                CGPS7_15ECallStop,                     SAME )
MC_RTK_OFSM( CGPS_LSIMCP_LOC_NOTIFY_REQ,             CGPS7_23NotifyReq,                     UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_LOC_NOTIFY_ABORT_IND,       CGPS7_41NotifyAbortInd,                SAME )
MC_RTK_OFSM( CGPS_LSIMCP_MOLR_ABORT_REQ,             CGPS7_33MolrAbortReq,                  UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_MOLR_START_RSP,             CGPS7_34MolrStartResp,                 UNKNOWN )
MC_RTK_OFSM( CGPS_LSIMCP_RESET_GPS_DATA_REQ,         CGPS7_27ResetUEPosition,               UNKNOWN )

#endif /* AGPS_FTR */

#if defined (GPS_FREQ_AID_FTR) && ( !defined (__RTK_E__) )
MC_RTK_OFSM( LSIMCCM_CLOCK_CAL_IND,                  CGPS8_05HandleClockCalibInd,           SAME )
#endif

#ifdef AGPS_TIME_SYNCH_FTR
MC_RTK_OFSM( CGPS_LSIMCP_FTA_PULSE_CNF,              CGPS7_43FtaPulseCnf,                   UNKNOWN )
#endif

/* + LMSqc32518 */
#if defined AGPS_TEST_MODE_FTR && defined CGPS_CNO_VALUE_FTR
MC_RTK_OFSM( CGPS_GET_CNO_VALUE_REQ,                 CGPS9_04HandleGetCnoMessage,           SAME )
#endif /*AGPS_TEST_MODE_FTR && CGPS_CNO_VALUE_FTR*/
/* - LMSqc32518 */

MC_RTK_OFSM( CGPS_LSIMEE_GET_EPHEMERIS_RSP,          CGPS11_03ExtendedEphRsp,               UNKNOWN )

/*XYBRID Integration :194997*/
MC_RTK_OFSM( CGPS_LSIMEE_GET_REFLOCATION_RSP,        CGPS11_04GetRefLocationRsp,            UNKNOWN )
/*XYBRID Integration :194997*/

MC_RTK_FFSM( CGPS0_02DefaultMessage, SAME)


/***********************************************************************************************/
/*********************************** a_CGPSTATState ********************************************/
/***********************************************************************************************/
MC_RTK_DFSM( a_CGPSTATState )

#ifdef __RTK_E__
/* +LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
#if defined (DUAL_OS_GEN_FTR) || defined( CR_LMSQC18522_CPR )
/* -LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
MC_RTK_OFSM( CGPS_HWL_INIT_IND,                      CGPS0_60HwlInitInd,                    SAME )
#endif

MC_RTK_OFSM( MAPI_MEA_AVAILABLE_DIR_IND,             CGPS0_37DirectoryReady,                SAME )
MC_RTK_OFSM( MAPI_MEA_GET_APPLICATION_PATH_CNF,      CGPS0_38GetDirectoryPathCnf,           SAME )
MC_RTK_OFSM( MAPI_MEA_REMOVE_DIR_IND,                CGPS0_39RemoveDirInd,                  SAME )
MC_RTK_OFSM( CGPS_TAT_LOOP_TIMER_EXPIRY,             CGPS6_00TATTimerExpired,               SAME )
MC_RTK_OFSM( CGPS_TAT_ACTIVATION_REQ,                CGPS6_01TATActivate,                   SAME )
MC_RTK_OFSM( CGPS_TAT_GET_INFO_REQ,                  CGPS6_02TATGetInfo,                    SAME )
MC_RTK_OFSM( CGPS_TAT_GET_LOCATION_REQ,              CGPS6_03TATGetLocation,                SAME )
MC_RTK_OFSM( CGPS_TAT_MODE_REQ,                      CGPS6_04TATMode,                       SAME )
MC_RTK_OFSM( CGPS_TAT_TRANSPARENT_MODE_REQ,          CGPS6_05TATTransparentMode,            SAME )
MC_RTK_OFSM( CGPS_TAT_TRANSPARENT_DATA_REQ,          CGPS6_11TATTransparentDataReq,         SAME )

/* + LMSQC06481 : Production test */
#ifdef CGPS_CNO_VALUE_FTR
MC_RTK_OFSM( CGPS_TAT_GET_CN_VALUE_REQ,              CGPS6_14GetCnValueReq,                 SAME )
#endif
/* - LMSQC06481 : Production test */
#endif /*__RTK_E__*/
#ifdef AGPS_TEST_MODE_FTR
MC_RTK_OFSM( CGPS_PRODUCTION_TEST_START_REQ,         CGPS9_30ProductionTestStartReq,        SAME )
MC_RTK_OFSM( CGPS_PRODUCTION_TEST_STOP_REQ,          CGPS9_31ProductionTestStopReq,         a_CGPSInitState )
MC_RTK_OFSM( CGPS_TREQ_LOOP_TIMER_EXPIRY,            CGPS9_44TREQTimerExpired,              SAME )
#endif /* AGPS_TEST_MODE_FTR */

MC_RTK_FFSM( CGPS0_02DefaultMessage, SAME )



#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/**************************************************************************************************/
/* CGPS0_00Init : Initialization of the module                                                    */
/**************************************************************************************************/
void CGPS0_00Init()
{
    uint32_t           vl_i;
    t_ProcessInstance  vl_FsmInstanceID;
    s_GN_GPS_Nav_Data *pl_GN_GPS_Nav_Data =NULL;

#ifdef AGPS_UP_FTR
    t_lsimup_InitReqM * p_MsgInitReq=NULL ;
#endif  /* AGPS_UP_FTR */

    /* Create instance process of CGPS */
    MC_RTK_CREATE_PROCESS_INSTANCE (PROCESS_CGPS, &vl_FsmInstanceID);

    CGPS4_35LoggingConfigurationFileRead();

    MC_CGPS_TRACE(("CGPS0_00Init InitState 0x%x", a_CGPSInitState));

#ifndef DUAL_OS_GEN_FTR
#ifndef __RTK_E__
    MC_HALGPS_INIT();
    if ( !OSA_InitCountingSemA( CGPS_SEM1, 1 )) // OSA_InitBinarySemaphore( CGPS_SEM1, 1 );
        { MC_CGPS_TRACE(("CGPS0_00Init : 0 != sem_init(CGPS_SEM1, 0, 1U)")); }
    if ( !OSA_InitCountingSemA( CGPS_SEM2, 0 )) // OSA_InitBinarySemaphore( CGPS_SEM2, 0 );
        { MC_CGPS_TRACE(("CGPS0_00Init : 0 != sem_init(CGPS_SEM2, 0, 0U)")); }
    MC_HALGPS_SET_SYSTEM_PATH((uint8_t *)"/data");
#endif /*__RTK_E__*/
#endif /* DUAL_OS_GEN_FTR */


/* Create the version string */
    snprintf( (char *)vg_Lbs_Host_Version , sizeof(vg_Lbs_Host_Version) , "ST-Ericsson_LBS-%02d.%03d.%03d-%03d.%03d" ,   \
                            LBS_MAJOR_VERSION,         \
                            LBS_MINOR_VERSION,         \
                            LBS_PATCH_VERSION,         \
                            LBS_CSL_PROJECT_IDENT,     \
                            LBS_CSL_PATCH_VERSION      \
                            );

    MC_CGPS_TRACE(("LBS Software Version = %s",vg_Lbs_Host_Version));

    /* Initialize NAF array */
    for ( vl_i=0 ; vl_i < K_CGPS_MAX_NUMBER_OF_NAF ; vl_i++ )
    {
        s_CgpsNaf[vl_i].v_State             = K_CGPS_NAF_UNUSED;
        s_CgpsNaf[vl_i].p_Callback          = NULL;
        s_CgpsNaf[vl_i].p_ThirdPartyPositionCallback    = NULL;
        s_CgpsNaf[vl_i].v_Mode              = K_CGPS_SINGLE_SHOT;
        s_CgpsNaf[vl_i].v_OutputType        = 0xFF;
        s_CgpsNaf[vl_i].v_Mask              = 0xFFFF;
        s_CgpsNaf[vl_i].v_FixRate           = 0;
        s_CgpsNaf[vl_i].v_AgeLimit          = 0;
        s_CgpsNaf[vl_i].v_HorizAccuracy     = 0;
        s_CgpsNaf[vl_i].v_VertAccuracy      = 0;
        s_CgpsNaf[vl_i].v_DeadlineOSTimeMS  = 0;
        s_CgpsNaf[vl_i].v_Suspended         = CGPS_MAX_PRIORITY_VALUE;
        s_CgpsNaf[vl_i].v_Config            = 0;
#ifdef CMCC_LOGGING_ENABLE
        s_CgpsNaf[vl_i].v_AlreadyUpdated    = FALSE;
        s_CgpsNaf[vl_i].v_RegisterTime      = 0;
#endif /*CMCC_LOGGING_ENABLE*/
        s_CgpsNaf[vl_i].v_LastApplicationUpdateTime = 0;
        s_CgpsNaf[vl_i].v_CurrentPositionStatus     = K_CGPS_POSITION_UNKNOWN;
        s_CgpsNaf[vl_i].v_AreaEventType             = K_CGPS_AREA_EVENT_NOT_SET;
        s_CgpsNaf[vl_i].p_TargetArea                = NULL;
        s_CgpsNaf[vl_i].v_ApplicationIDInfo.v_ApplicationIDInfo_present = FALSE;
        memset( (uint8_t*)&s_CgpsNaf[vl_i].v_ApplicationIDInfo, 0, sizeof(t_cgps_ApplicationID_Info) );
    }

    memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(*pl_GN_GPS_Nav_Data) );

    /* ++ LMSqb65223 */
    vg_Latest_Fix_Nav_Data.OS_Time_ms = 0;
    /* -- LMSqb65223 */
    vg_CGPS_Naf_Count = 0;

    vg_CgpsTestall.v_State                       = K_CGPS_NAF_UNUSED;
    vg_CgpsTestall.p_Callback                    = NULL;
    vg_CgpsTestall.p_ThirdPartyPositionCallback  = NULL;
    vg_CgpsTestall.v_Mode                        = K_CGPS_SINGLE_SHOT;
    vg_CgpsTestall.v_OutputType                  = 0xFF;
    vg_CgpsTestall.v_Mask                        = 0xFFFF;
    vg_CgpsTestall.v_FixRate                     = 0;
    vg_CgpsTestall.v_AgeLimit                    = 0;
    vg_CgpsTestall.v_HorizAccuracy               = 0;
    vg_CgpsTestall.v_VertAccuracy                = 0;
    vg_CgpsTestall.v_DeadlineOSTimeMS            = 0;
    vg_CgpsTestall.v_Suspended                   = CGPS_MAX_PRIORITY_VALUE;
    vg_CgpsTestall.v_LastApplicationUpdateTime   = 0;

    CGPS0_53UpdateSessionConfig();

#ifdef AGPS_UP_FTR
    /* initialise LSIMUP module */
    p_MsgInitReq  = (t_lsimup_InitReqM *)MC_RTK_GET_MEMORY(sizeof(*p_MsgInitReq));

    MC_RTK_SEND_MSG_TO_PROCESS( PROCESS_CGPS, 0,
                                PROCESS_LSIMUP, 0,
                                CGPS_LSIMUP_INIT_REQ,
                                (t_MsgHeader*)p_MsgInitReq );
#ifndef __RTK_E__
    CGPS0_27InitSupl();
#endif  /* __RTK_E__ */

    /* Initialize external applications array */
    for ( vl_i=0 ; vl_i < K_CGPS_MAX_NUMBER_OF_SUPL ; vl_i++ )
    {
        s_CgpsSupl[vl_i].v_IsRegistered                     = FALSE;
        s_CgpsSupl[vl_i].v_GPSHandle                        = NULL;
        s_CgpsSupl[vl_i].v_ConnectionHandle                 = 0;
        s_CgpsSupl[vl_i].v_ConnectionParm.Port              = 0;
        s_CgpsSupl[vl_i].v_bearer                           = NULL;
        s_CgpsSupl[vl_i].v_Suspended                        = CGPS_MAX_PRIORITY_VALUE;
        s_CgpsSupl[vl_i].v_ConnectionParm.TcpIp_AddressType = IP_None;
        s_CgpsSupl[vl_i].v_ConnectionParm.p_TcpIp_Address   = NULL;
        s_CgpsSupl[vl_i].v_PrevPositionStatus               = K_CGPS_POSITION_UNKNOWN;
        s_CgpsSupl[vl_i].v_PrevCellIdPositionStatus         = K_CGPS_POSITION_UNKNOWN;
        s_CgpsSupl[vl_i].v_TriggeredEventType               = K_CGPS_TRIGGER_NONE;
    }

    vg_CGPS_Supl_Count = 0;

    /* Initialize SUPL applications array */
    for ( vl_i=0 ; vl_i < K_CGPS_MAX_NUMBER_OF_SUPL ; vl_i++ )
    {
        s_CgpsNotification[vl_i].v_IsPending = FALSE;
        s_CgpsNotification[vl_i].v_Handle    = 0;
        s_CgpsNotification[vl_i].v_Timeout   = 0;
        s_CgpsNotification[vl_i].v_DefaultNotificationType = K_CGPS_NOTIFICATION_ONLY;
    }

    vg_CGPS_Notification_Count = 0;

    /* bearer initialisation */
    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_NOT_INITIALISED;

    /* Cell information initialisation */
    vg_CGPS_Supl_Location_Id.Type = (e_CellInfoType)0xFF;

    vg_CGPS_Supl_Pending_Connection = -1;

    vg_CGPS_Supl_Slp_Config.v_AddrLen    = 0;
    vg_CGPS_Supl_Slp_Config.v_Addr       = NULL;
    vg_CGPS_Supl_Slp_Config.v_AddrType   = K_CGPS_SLPADDR_FQDN;
    vg_CGPS_Supl_Slp_Config.v_PortNum    = CGPS_SUPL_DEFAULT_PORT;

    vg_CGPS_Supl_Eslp_Config.v_AddrLen   = 0;
    vg_CGPS_Supl_Eslp_Config.v_Addr      = NULL;
    vg_CGPS_Supl_Eslp_Config.v_AddrType  = K_CGPS_SLPADDR_FQDN;
    vg_CGPS_Supl_Eslp_Config.v_PortNum   = CGPS_SUPL_DEFAULT_PORT;

    vg_CGPS_Supl_AutoSlp_Config.v_AddrLen   = 0;
    vg_CGPS_Supl_AutoSlp_Config.v_Addr      = NULL;

    vg_CGPS_Supl_AutoEslp_Config.v_AddrLen   = 0;
    vg_CGPS_Supl_AutoEslp_Config.v_Addr      = NULL;



/* For an autoconfiguration request, MCC / MNC is needed to formulate the request address. Hence mobile information is needed from begining*/
    CGPS5_26LsimupMobileInfoReq();

#endif /*AGPS_UP_FTR*/

#ifdef AGPS_FTR
    /* Initialize CP applications array */
    for ( vl_i = 0 ; vl_i < (K_CGPS_MAX_NUMBER_OF_CP+1 ); vl_i++ )
    {
        s_CgpsCp[vl_i].v_IsRegistered              = FALSE;
        s_CgpsCp[vl_i].v_SessionID                 = 0;
        s_CgpsCp[vl_i].v_SessionType               = K_CGPS_CP_SESSION_TYPE_NOT_SET;
        s_CgpsCp[vl_i].v_PeriodicInterval          = 0;
        s_CgpsCp[vl_i].v_OptField                  = 0;
        s_CgpsCp[vl_i].v_HorizAccuracy             = 0;
        s_CgpsCp[vl_i].v_VertAccuracy              = 0;
        s_CgpsCp[vl_i].v_EnvChar                   = 0;
        s_CgpsCp[vl_i].v_Suspended                 = CGPS_MAX_PRIORITY_VALUE;
        s_CgpsCp[vl_i].v_LastApplicationUpdateTime = 0;
        memset( (uint8_t*)&s_CgpsCp[vl_i].v_Meas, 0, sizeof(s_GN_AGPS_Meas) );
    }
    vg_CGPS_Cp_Count = 0;

     CGPS7_39SendCapabilityInd(FALSE);
#endif

#ifdef __RTK_E__
    vg_Tat_Status         = CGPS_TAT_STATE_NOT_INITIATE;
    vg_TatTransparentMode = 0;
#endif /*__RTK_E__*/

#ifndef __RTK_E__
/* Call to delete GPS_UTC_Calib.bin file */
/* In Android platforms there is no API call to get system time which is persisteance across reboots
and also not affected by system time and time zone changes. so to avoid the use of corrupted UTC time,
Delete the UTC calibration file at init*/
    MC_HALGPS_DELETE_UTCCALIBSTORE();
#endif /* __RTK_E__ */

/* +LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
#if !( defined (DUAL_OS_GEN_FTR) || defined(CR_LMSQC18522_CPR))
/* -LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
    /* Initialize the GPS library */

    GN_GPS_Initialise();

    CGPS4_10SetLbsHostVersion();

    CGPS4_38UpdateConfiguration();
#endif
#ifdef AGPS_FTR
    CGPS7_31CpMoInstanceInitAll();
#endif

/*+ LMSqc32518 */
#ifdef CGPS_UPLOAD_PATCH_AT_INIT_FTR
    {
        t_CGPS_InitializedCgpsInd* pl_Msg;

        pl_Msg = (t_CGPS_InitializedCgpsInd*)MC_RTK_GET_MEMORY(sizeof(*pl_Msg));

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,
                                   PROCESS_CGPS,0,
                                   CGPS_INITIALIZED_IND,
                                   (t_MsgHeader*)pl_Msg );
    }
#else
    CGPS0_52ConfigureCgpsNextState();
#endif /* CGPS_UPLOAD_PATCH_AT_INIT_FTR */
/*- LMSqc32518 */

    vg_CGPS_Config_Ver_flag = FALSE;



    CGPS4_33UserConfigurationFileRead();
    //CGPS4_35LoggingConfigurationFileRead();
    //LBSCFG_LogReadConfiguration();
    CGPS4_42PlatformConfigurationFileRead();
#ifdef AGPS_TIME_SYNCH_FTR
    CGPS7_44FtaInitialize();
#endif /*AGPS_TIME_SYNCH_FTR*/
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/**************************************************************************************************/
/* CGPS0_01timerExpired : Timer function                                                          */
/**************************************************************************************************/
const t_OperationDescriptor*  CGPS0_01timerExpired(t_RtkObject* p_FsmObject)
{
    t_cgps_NavData         vl_NavDataToSend;
    s_GN_GPS_Nav_Data      vl_NavDataTmp;
    uint8_t                vl_returnNavData        = FALSE;
    uint32_t               vl_CurrentTime          = GN_GPS_Get_OS_Time_ms();
    const t_OperationDescriptor* pl_NextState = SAME;

    p_FsmObject = p_FsmObject;
/*    MC_CGPS_TRACE(("CGPS0_01timerExpired" ));*/

    switch ( vg_CGPS_State )
    {

    case K_CGPS_INACTIVE :
    {
        MC_CGPS_TRACE(("Ignore timer expiry as CGPS not running" ));
    }
    break;

    case K_CGPS_ACTIVE_GPS_ON :
    {
        /*first, relaunch timer*/
        MC_RTK_PROCESS_START_TIMER(CGPS_FIX_LOOP_TIMEOUT, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_FIX_LOOP_TIMER_DURATION));

        if( vg_CGPS_GpsState != K_CGPS_GPS_RUNNING )
        {
            MC_CGPS_TRACE(("ERROR:CGPS0_01timerExpired() - GPS not running but CGPS is active @ high frequency"));
            return pl_NextState;
        }

#if defined (AGPS_UP_FTR) || defined (AGPS_FTR)
        /* check CP / SUPL notification timeout */
        if( vg_CGPS_Notification_Count > 0 )
        {
            CGPS0_24NotificationExpired();
        }
#endif /*AGPS_UP_FTR || AGPS_FTR */

       CGPS4_39PGNVCommandsHandler();

#ifdef GPS_FREQ_AID_FTR
         CGPS8_04CheckGsaTimerExp(vl_CurrentTime);
#endif

        /* If in here there is at least one user so the GPS is already, or will be */
        /* powered up now. */
        GN_GPS_Update();
#ifdef AGPS_UP_FTR
        /* +LMSqc38060 */
        CGPS5_33HandleTimerExpiry();
        /* -LMSqc38060 */
        GN_SUPL_Handler();
#endif

        vl_returnNavData = GN_GPS_Get_Nav_Data(&vl_NavDataTmp);


        if( vl_returnNavData == FALSE )
        {
            /* No New Nav data was received from the baseband so GN_GPS_Update()   */
            /* did not call GN_GPS_Write_GNB_Ctrl() to send data to the baseband.  */
            /* If there is still some patch code data that needs to be sent to the */
            /* baseband, call the patch uploader.                                  */
            /* This main loop repeats every 50ms which means at 115200 baud TX you */
            /* can send up to 11520*0.050 = 576 bytes, or 22 x 26 byte messages.   */
            /* So target 20 messages, but this number may need to be reduced based */
            /* on the efficiency of the UART TX Driver                             */
            if( gn_Patch_Status > 0  &&  gn_Patch_Status < 7 )   /* Patching in progress*/
            {
                CGPS2_00UploadMEPatch( K_CGPS_PATCH_SEGMENT_LENGTH );
                CGPS0_52ConfigureCgpsNextState();
            }
        }
        else
        {
            /* we have a valid data copy it in the global navigation structure*/
            memcpy( (uint8_t*)&vg_Nav_Data, (uint8_t*)&vl_NavDataTmp, sizeof(vl_NavDataTmp) );
            MC_CGPS_TRACE(("NAV_DATA: Fix=%d %d Sat Status = %d %d LLA= %4.4lfN %4.4lfE %6.4lf Acc = Hor:%lf  Ver:%lf ",
                vg_Nav_Data.Valid_SingleFix,vg_Nav_Data.FixMode,
                vg_Nav_Data.SatsInView, vg_Nav_Data.SatsUsed,
                vg_Nav_Data.Latitude, vg_Nav_Data.Longitude, vg_Nav_Data.Altitude_MSL,
                vg_Nav_Data.H_AccMaj, vg_Nav_Data.V_AccEst));

            /* ++ LMSqb65223 */
            /* A new Nav Data is available. Check if it is a valid fix and save it */
            /* for possible use as a Single Shot if it fits Age Limit              */
            if( vg_Nav_Data.Valid_SingleFix )
            {
                vg_Latest_Fix_Nav_Data = vg_Nav_Data; /* saves the structure content */
#ifdef AGPS_HISTORICAL_DATA_FTR
                CGPS10_09UpdatePositionInHistDb();
#endif /*AGPS_HISTORICAL_DATA_FTR*/
            }
            /* -- LMSqb65223 */

            /* ER - 322372 */
            if( vg_CgpsTestall.v_State == K_CGPS_NAF_READY )
            {
                /* ++ LMSqb65223 */
                /* ++ LMSqc26033 */
                if((e_cgps_NavDataType)vg_CgpsTestall.v_OutputType == K_CGPS_NMEA_AND_C_STRUCT)
                {

                    CGPS0_36GetNafData(&vg_Nav_Data, &vl_NavDataToSend, K_CGPS_C_STRUCT, vg_CgpsTestall.v_Mask);
                    (vg_CgpsTestall.p_Callback)( vl_NavDataToSend );
                    MC_RTK_FREE_MEMORY( vl_NavDataToSend.p_NavData );
                    CGPS0_36GetNafData(&vg_Nav_Data, &vl_NavDataToSend, K_CGPS_NMEA, vg_CgpsTestall.v_Mask);

                }
                else
                /* -- LMSqc26033 */
                CGPS0_36GetNafData(&vg_Nav_Data, &vl_NavDataToSend, (e_cgps_NavDataType)vg_CgpsTestall.v_OutputType, vg_CgpsTestall.v_Mask);
                /* -- LMSqb65223 */
                /* Send data */
                (vg_CgpsTestall.p_Callback)( vl_NavDataToSend );
                MC_RTK_FREE_MEMORY( vl_NavDataToSend.p_NavData );
            }
        }

        /* send the last retrieved data to the requesting internal applications */
        CGPS0_16SendDataToNaf();

#ifdef AGPS_FTR
        /* send the last retrieved data to the requesting CP applications */
        CGPS7_03SendDataToCpAppli();
        CGPS7_46CheckGPSActiveRequiredForCp();
#endif /* AGPS_FTR */
    }
    break;

    case K_CGPS_ACTIVE_GPS_OFF:
    {
        /*first, relaunch timer*/
        MC_RTK_PROCESS_START_TIMER(CGPS_FIX_LOOP_TIMEOUT, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_FIX_LOOP_LOW_FREQ_TIMER_DURATION));
#ifdef AGPS_UP_FTR
        /* +LMSqc38060 */
        CGPS5_33HandleTimerExpiry();
        /* -LMSqc38060 */
        GN_SUPL_Handler();
#endif
        /* send the last retrieved data to the requesting internal applications */
        CGPS0_16SendDataToNaf();
    }
    break;

    default:
    {
        MC_CGPS_TRACE(("CGPS ERROR : Unknown state"));
    }
    break;

    }

/*+SYScs46505*/
    CGPS4_32CheckAssistanceNeedInTracking();
/*-SYScs46505*/

    return pl_NextState;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/**************************************************************************************************/
/* CGPS_02DefaultMessage : default function of the fsm table                                      */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_02DefaultMessage(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor *pl_State;
    const t_OperationDescriptor* pl_NextState = SAME;

    p_FsmObject = p_FsmObject;

    MC_RTK_GET_ACTIVE_PROCESS_STATE( PROCESS_CGPS, &pl_State );
    MC_ERR_RAISE(NO_BLOCKING_UNEXPECTED_FSM_MSG, pl_State, p_FsmObject);

    /* MC_DIN_ERROR( NO_BLOCKING, ERROR_1 ); */
    MC_CGPS_TRACE(("CGPS0_02DefaultMessage: Error ?!"));
    return pl_NextState;
}


#ifdef __RTK_E__
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/**************************************************************************************************/
/* CGPS0_03UTCCorrupted : just call HALGPS0_05UTCCorrupted                                        */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_03UTCCorrupted(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;

    MC_CGPS_TRACE(("CGPS0_03UTCCorrupted"));
    MC_HALGPS_UTCCORRUPTED();
    return pl_NextState;
}
#endif /*__RTK_E__*/


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/**************************************************************************************************/
/* CGPS0_04UTCCorrupted : just call HALGPS0_05UTCCorrupted                                        */
/**************************************************************************************************/
void CGPS0_04UTCCorruptedInd()
{
    t_CGPS_UtcCorrupt * pl_Msg=NULL;

    MC_CGPS_TRACE(("CGPS0_04UTCCorruptedInd"));

    /*send message to itself, the init is finished*/
    pl_Msg = (t_CGPS_UtcCorrupt*)MC_RTK_GET_MEMORY(sizeof(*pl_Msg));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,
                               PROCESS_CGPS,0,
                               CGPS_UTC_CORRUPT_IND,
                               (t_MsgHeader*)pl_Msg );
}


/* +LMSqb93565 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/**************************************************************************************************/
/* CGPS0_05RegisterFix : request for a single shot fix registration with perioodic updates        */
/**************************************************************************************************/
e_cgps_Error CGPS0_05RegisterFix(uint32_t vp_Handle,        t_cgps_NavigationCallback vp_Callback,
/* +LMSqc17721 - BNS : Change in function prototype */
                                 uint8_t  vp_OutputType,    uint16_t vp_NmeaMask,
/*                               uint8_t  vp_OutputType,    e_cgps_NmeaMask vp_NmeaMask,*/
/* -LMSqc17721 - BNS : Change in function prototype */
                                 uint32_t vp_FixRate,       uint32_t vp_Timeout,
                                 uint16_t vp_HorizAccuracy, uint16_t vp_VertAccuracy,
                                 uint32_t vp_AgeLimit)
{
    e_cgps_Error vl_Return = K_CGPS_NO_ERROR;

    MC_CGPS_TRACE(("CGPS0_05RegisterFix handle 0x%d",vp_Handle));

    /* check handle validity */
    if(!CGPS4_01IsRegistered(vp_Handle))
    {
        MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix handle not registered"));
        vl_Return = K_CGPS_ERR_INVALID_HANDLE;
    }
    else
    {
        /* check paramters */
        if(    ( vp_Callback      != NULL  )
            && ( vp_OutputType    == K_CGPS_NMEA || vp_OutputType == K_CGPS_C_STRUCT
                                     /* ++ LMSqc26033 */
                                      || vp_OutputType == K_CGPS_NMEA_AND_C_STRUCT )
                                     /* -- LMSqc26033 */
            && ( vp_FixRate        > 0     )
          )
        {
            if(     MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS    )
                 || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS  )
                 || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED )
              )
            {
                t_CGPS_FixReqInd *pl_CGPS_FixReqInd;
                uint32_t   vl_OsTimeStamp = GN_GPS_Get_OS_Time_ms();
                bool  vl_Result;

                (s_CgpsNaf[vp_Handle]).v_Mode              = K_CGPS_SINGLE_SHOT_WITH_PERIODIC_UPDATE;
                (s_CgpsNaf[vp_Handle]).p_Callback          = vp_Callback;
                (s_CgpsNaf[vp_Handle]).v_OutputType        = vp_OutputType;
                (s_CgpsNaf[vp_Handle]).v_FixRate           = vp_FixRate;
                (s_CgpsNaf[vp_Handle]).v_Mask              = vp_NmeaMask;
                (s_CgpsNaf[vp_Handle]).v_AgeLimit          = vp_AgeLimit;
                (s_CgpsNaf[vp_Handle]).v_HorizAccuracy     = vp_HorizAccuracy;
                (s_CgpsNaf[vp_Handle]).v_VertAccuracy      = vp_VertAccuracy;
#ifdef CMCC_LOGGING_ENABLE
                (s_CgpsNaf[vp_Handle]).v_AlreadyUpdated    = FALSE;
                (s_CgpsNaf[vp_Handle]).v_RegisterTime      = vl_OsTimeStamp;
#endif //CMCC_LOGGING_ENABLE
                (s_CgpsNaf[vp_Handle]).v_DeadlineOSTimeMS  = vl_OsTimeStamp + vp_FixRate;
                /* +LMSqc37499 */
                (s_CgpsNaf[vp_Handle]).v_FirstFixDone = FALSE;
                /* -LMSqc37499 */

                if( vp_Timeout != 0 )
                {
                    (s_CgpsNaf[vp_Handle]).v_Timeout= vl_OsTimeStamp + vp_Timeout;
                }
                else
                {
                    (s_CgpsNaf[vp_Handle]).v_Timeout = vp_Timeout;
                }

                (s_CgpsNaf[vp_Handle]).v_LastApplicationUpdateTime = 0;
                (s_CgpsNaf[vp_Handle]).v_State  = K_CGPS_NAF_READY;
                (s_CgpsNaf[vp_Handle]).v_Config = vg_CGPSNafSessionConfig;

                pl_CGPS_FixReqInd = (t_CGPS_FixReqInd * ) MC_RTK_GET_MEMORY( sizeof(*pl_CGPS_FixReqInd) );

                pl_CGPS_FixReqInd->v_Handle = (t_CgpsNaf *)vp_Handle;
                pl_CGPS_FixReqInd->v_Result = &vl_Result;

                if ( !MC_RTK_SEM_CONSUME( CGPS_SEM1 )) /* prevent other applis to use the semaphore #2 */
                    { MC_CGPS_TRACE(("CGPS0_05RegisterFix : 0 != sem_wait(CGPS_SEM1)")); }

                MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                           PROCESS_CGPS, 0,
                                           CGPS_FIX_REQ_IND,
                                           (t_MsgHeader *)pl_CGPS_FixReqInd);

                if ( !MC_RTK_SEM_CONSUME( CGPS_SEM2 )) /* Wait for CGPS to complete processing of the request */
                    { MC_CGPS_TRACE(("CGPS0_05RegisterFix : 0 != sem_wait(CGPS_SEM2)")); }
                if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM1 )) /* allow another application to use sem #2 */
                    { MC_CGPS_TRACE(("CGPS0_05RegisterFix : 0 != sem_post(CGPS_SEM1)")); }

                if( !vl_Result )
                {
                    vl_Return = K_CGPS_ERR_GPS_NOT_STARTED;
                }
            }
            else
            {
                vl_Return = K_CGPS_ERR_UNSUPPORTED_SERVICE;
            }
        }
        else
        {
            MC_CGPS_TRACE(("CGPS0_05RegisterFix parameters invalid"));
            vl_Return =  K_CGPS_ERR_INVALID_PARAMETER;
        }
    }

    return vl_Return;
}
/* -LMSqb93565 */


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/**************************************************************************************************/
/* CGPS0_09Deinit : Termination of the module                                                     */
/**************************************************************************************************/
void CGPS0_09Deinit()
{
#ifdef AGPS_UP_FTR
    /* Message body sent to LSIMUP */
    t_lsimup_DeInitReqM* p_MsgDeInitReq=NULL ;
    p_MsgDeInitReq  = (t_lsimup_DeInitReqM *)MC_RTK_GET_MEMORY(sizeof(*p_MsgDeInitReq));
#endif

    MC_HALGPS_POWERDOWN();

#if defined(AGPS_UP_FTR)
    /* SUPL feature deinitialisation */
    GN_SUPL_Handler_DeInit();

    /* LSIMUP deinitialisation */
    p_MsgDeInitReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,   0,
                               PROCESS_LSIMUP, 0,
                               CGPS_LSIMUP_DEINIT_REQ,
                               (t_MsgHeader *)p_MsgDeInitReq);

    /* free memory */
    CGPS5_16SuplFreeMemory();
#endif
    GN_GPS_Shutdown();

#ifndef __RTK_E__
    if ( !OSA_DeinitCountingSem( CGPS_SEM1 ))  //OSA_DeinitBinarySemaphore( CGPS_SEM1 );
        { MC_CGPS_TRACE(("CGPS0_09Deinit : 0 != sem_destroy(CGPS_SEM1)")); }
    if ( !OSA_DeinitCountingSem( CGPS_SEM2 )) //OSA_DeinitBinarySemaphore( CGPS_SEM2 );
        { MC_CGPS_TRACE(("CGPS0_09Deinit : 0 != sem_destroy(CGPS_SEM2)")); }
#endif

    /* Change GPS state and CGPS state*/
    vg_CGPS_GpsState = K_CGPS_GPS_OFF;
    vg_CGPS_State    = K_CGPS_INACTIVE;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/**************************************************************************************************/
/* CGPS0_10ServiceStartReq : Application registration                                             */
/**************************************************************************************************/
e_cgps_Error CGPS0_10ServiceStartReq(uint32_t* pp_Handle)
{
    t_CgpsRegisterResult vl_out;
    t_CGPS_NewNaf       *pl_Msg=NULL;
    pl_Msg        = (t_CGPS_NewNaf*)MC_RTK_GET_MEMORY(sizeof(*pl_Msg));
    pl_Msg->v_out = &vl_out;
    pl_Msg->p_AppIdInfo = NULL;
    if ( !MC_RTK_SEM_CONSUME( CGPS_SEM1 )) /* prevent other applis to use the semaphore #2 */
        { MC_CGPS_TRACE(("CGPS0_10ServiceStartReq : 0 != sem_wait(CGPS_SEM1)")); }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_CGPS,0,CGPS_NEW_NAF_REGISTERED_IND,(t_MsgHeader*)pl_Msg );

    MC_CGPS_TRACE(("Waiting for CGPS to complete processing of request"));
    if ( !MC_RTK_SEM_CONSUME( CGPS_SEM2 )) /* Wait for CGPS to complete processing of the request */
        { MC_CGPS_TRACE(("CGPS0_10ServiceStartReq : 0 != sem_wait(CGPS_SEM2)")); }

    MC_CGPS_TRACE(("allow another application to use sem #2"));
    if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM1 )) /* allow another application to use sem #2 */
        { MC_CGPS_TRACE(("CGPS0_10ServiceStartReq : 0 != sem_post(CGPS_SEM1)")); }

    if(vl_out.v_Result == K_CGPS_NO_ERROR)
    {
        *pp_Handle = vl_out.v_Handle;
//        MC_CGPS_TRACE_PARAM("Handle 0x%X",vl_out.v_Handle); Roy
    }

    return vl_out.v_Result;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/**************************************************************************************************/
/* CGPS0_11ServiceStopReq : Application deregistration                                            */
/**************************************************************************************************/
e_cgps_Error CGPS0_11ServiceStopReq(uint32_t vp_Handle)
{
    t_CgpsDeregisterResult vl_out;
    t_CGPS_NafDeregistered * pl_Msg=NULL;



    pl_Msg           = (t_CGPS_NafDeregistered*)MC_RTK_GET_MEMORY(sizeof(*pl_Msg));
    pl_Msg->v_Handle = vp_Handle;
    pl_Msg->v_out    = &vl_out;

    if ( !MC_RTK_SEM_CONSUME( CGPS_SEM1 )) /* prevent other applis to use the semaphore #2 */
        { MC_CGPS_TRACE(("CGPS0_11ServiceStopReq : 0 != sem_wait(CGPS_SEM1)")); }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_CGPS,0,CGPS_NAF_DEREGISTERED_IND,(t_MsgHeader*)pl_Msg );

    if ( !MC_RTK_SEM_CONSUME( CGPS_SEM2 )) /* Wait for CGPS to complete processing of the request */
        { MC_CGPS_TRACE(("CGPS0_11ServiceStopReq : 0 != sem_wait(CGPS_SEM2)")); }
    if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM1 )) /* allow another application to use sem #2 */
        { MC_CGPS_TRACE(("CGPS0_11ServiceStopReq : 0 != sem_post(CGPS_SEM1)")); }

    return vl_out.v_Result;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/**************************************************************************************************/
/* CGPS0_12RegisterPeriodicFix : request for a periodic fix registration                          */
/**************************************************************************************************/
e_cgps_Error CGPS0_12RegisterPeriodicFix(uint32_t vp_Handle,     t_cgps_NavigationCallback vp_Callback,
/* +LMSqc17721 - BNS : Change in function prototype */
                                         uint8_t  vp_OutputType, uint16_t vp_NmeaMask,
/*                                       uint8_t  vp_OutputType, e_cgps_NmeaMask vp_NmeaMask,*/
/* -LMSqc17721 - BNS : Change in function prototype */
                                         uint32_t vp_FixRate)
{
    e_cgps_Error vl_Return = K_CGPS_NO_ERROR;

    MC_CGPS_TRACE(("CGPS0_12RegisterPeriodicFix handle : 0x%d",vp_Handle));

    /* TBD: Send a Message */
    /* check handle validity */
    if(!CGPS4_01IsRegistered(vp_Handle))
    {
        MC_CGPS_TRACE(("CGPS0_12RegisterPeriodicFix handle not registered"));
        vl_Return = K_CGPS_ERR_INVALID_HANDLE;
    }
    else
    {
        /* check paramters */
        if(   ( vp_Callback   != NULL )
           && ( vp_OutputType == K_CGPS_NMEA || vp_OutputType == K_CGPS_C_STRUCT
                                /* ++ LMSqc26033 */
                                || vp_OutputType == K_CGPS_NMEA_AND_C_STRUCT )
                                /* -- LMSqc26033 */
           && ( vp_FixRate    >  0    )
          )
        {
            if(     MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS    )
                 || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS  )
                 || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED )
              )
            {
                t_CGPS_FixReqInd *pl_CGPS_FixReqInd;
                bool             vl_Result;
                uint32_t   vl_OsTimeStamp = GN_GPS_Get_OS_Time_ms();

                (s_CgpsNaf[vp_Handle]).v_Mode           = K_CGPS_PERIODIC;
                (s_CgpsNaf[vp_Handle]).p_Callback       = vp_Callback;
                (s_CgpsNaf[vp_Handle]).v_OutputType     = vp_OutputType;
                (s_CgpsNaf[vp_Handle]).v_Mask           = vp_NmeaMask;
                (s_CgpsNaf[vp_Handle]).v_FixRate        = vp_FixRate;
                (s_CgpsNaf[vp_Handle]).v_HorizAccuracy  = 0;
                (s_CgpsNaf[vp_Handle]).v_VertAccuracy   = 0;
                (s_CgpsNaf[vp_Handle]).v_Timeout        = 128000;
                (s_CgpsNaf[vp_Handle]).v_State          = K_CGPS_NAF_READY;
                (s_CgpsNaf[vp_Handle]).v_Config         = vg_CGPSNafSessionConfig;
                (s_CgpsNaf[vp_Handle]).v_LastApplicationUpdateTime = 0;
                (s_CgpsNaf[vp_Handle]).v_DeadlineOSTimeMS          = vl_OsTimeStamp + vp_FixRate;
#ifdef CMCC_LOGGING_ENABLE
                (s_CgpsNaf[vp_Handle]).v_AlreadyUpdated    = FALSE;
                (s_CgpsNaf[vp_Handle]).v_RegisterTime      = vl_OsTimeStamp;
#endif //CMCC_LOGGING_ENABLE
                /* +LMSqc37499 */
                (s_CgpsNaf[vp_Handle]).v_FirstFixDone = FALSE;
                /* -LMSqc37499 */
                /* Demo appli behave incorrectly if not vp_FixRate */

                pl_CGPS_FixReqInd = (t_CGPS_FixReqInd * ) MC_RTK_GET_MEMORY( sizeof(*pl_CGPS_FixReqInd) );

                pl_CGPS_FixReqInd->v_Handle = (t_CgpsNaf *)vp_Handle;
                pl_CGPS_FixReqInd->v_Result = &vl_Result;

                if ( !MC_RTK_SEM_CONSUME( CGPS_SEM1 )) /* prevent other applis to use the semaphore #2 */
                    { MC_CGPS_TRACE(("CGPS0_12RegisterPeriodicFix : 0 != sem_wait(CGPS_SEM1)")); }

                MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                           PROCESS_CGPS, 0,
                                           CGPS_FIX_REQ_IND,
                                           (t_MsgHeader *)pl_CGPS_FixReqInd);

                if ( !MC_RTK_SEM_CONSUME( CGPS_SEM2 )) /* Wait for CGPS to complete processing of the request */
                    { MC_CGPS_TRACE(("CGPS0_12RegisterPeriodicFix : 0 != sem_wait(CGPS_SEM2)")); }
                if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM1 )) /* allow another application to use sem #2 */
                    { MC_CGPS_TRACE(("CGPS0_12RegisterPeriodicFix : 0 != sem_post(CGPS_SEM1)")); }

                if( !vl_Result )
                {
                    vl_Return = K_CGPS_ERR_GPS_NOT_STARTED;
                }
            }
            else
            {
                 vl_Return = K_CGPS_ERR_UNSUPPORTED_SERVICE;
            }
        }
        else
        {
            MC_CGPS_TRACE(("CGPS0_12RegisterPeriodicFix parameters invalid"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }
    }
    return vl_Return;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/**************************************************************************************************/
/* CGPS_13RegisterSingleShotFix : request for a single shot fix registration                      */
/**************************************************************************************************/
e_cgps_Error CGPS0_13RegisterSingleShotFix(uint32_t vp_Handle,       t_cgps_NavigationCallback vp_Callback,
/* +LMSqc17721 - BNS : Change in function prototype */
                                           uint8_t  vp_OutputType,   uint16_t vp_NmeaMask,
/*                                         uint8_t  vp_OutputType,   e_cgps_NmeaMask vp_NmeaMask, */
/* -LMSqc17721 - BNS : Change in function prototype */
                                           uint32_t vp_Timeout,      uint16_t vp_HorizAccuracy,
                                           uint16_t vp_VertAccuracy, uint32_t vp_AgeLimit)
{
    e_cgps_Error vl_Return = K_CGPS_NO_ERROR;

    MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix handle 0x%d",vp_Handle));

    /* TBD: Send a Message */
    /* check handle validity */
    if(!CGPS4_01IsRegistered(vp_Handle))
    {
        MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix handle not registered"));
        vl_Return = K_CGPS_ERR_INVALID_HANDLE;
    }
    else
    {
        /* check paramters */
        if(    ( vp_Callback      != NULL  )
            && ( vp_OutputType    == K_CGPS_NMEA || vp_OutputType == K_CGPS_C_STRUCT
                               /* ++ LMSqc26033 */
                               || vp_OutputType == K_CGPS_NMEA_AND_C_STRUCT )
                               /* -- LMSqc26033 */
          )
        {
            if(     MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS    )
                 || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS  )
                 || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED )
              )
            {
                t_CGPS_FixReqInd *pl_CGPS_FixReqInd;
                bool             vl_Result;
                uint32_t   vl_OsTimeStamp = GN_GPS_Get_OS_Time_ms();

                (s_CgpsNaf[vp_Handle]).v_Mode           = K_CGPS_SINGLE_SHOT;
                (s_CgpsNaf[vp_Handle]).p_Callback       = vp_Callback;
                (s_CgpsNaf[vp_Handle]).v_OutputType     = vp_OutputType;
                (s_CgpsNaf[vp_Handle]).v_Mask           = vp_NmeaMask;
                (s_CgpsNaf[vp_Handle]).v_AgeLimit       = vp_AgeLimit;
                (s_CgpsNaf[vp_Handle]).v_HorizAccuracy  = vp_HorizAccuracy;
                (s_CgpsNaf[vp_Handle]).v_VertAccuracy   = vp_VertAccuracy;
                (s_CgpsNaf[vp_Handle]).v_Config         = vg_CGPSNafSessionConfig;
                /* +LMSqb93565 */
                /* Single Shot now directly uses the Timeout field to decide on whether timeout has happened */
                (s_CgpsNaf[vp_Handle]).v_DeadlineOSTimeMS          = 0;
                (s_CgpsNaf[vp_Handle]).v_LastApplicationUpdateTime = 0;
                (s_CgpsNaf[vp_Handle]).v_State                     = K_CGPS_NAF_READY;
                /* +LMSqc37499 */
                (s_CgpsNaf[vp_Handle]).v_FirstFixDone = FALSE;
                /* -LMSqc37499 */
#ifdef CMCC_LOGGING_ENABLE
                (s_CgpsNaf[vp_Handle]).v_AlreadyUpdated    = FALSE;
                (s_CgpsNaf[vp_Handle]).v_RegisterTime      = vl_OsTimeStamp;
#endif //CMCC_LOGGING_ENABLE

                if( vp_Timeout != 0 )
                {
                    (s_CgpsNaf[vp_Handle]).v_Timeout = vl_OsTimeStamp + vp_Timeout;
                }
                else
                {
                    (s_CgpsNaf[vp_Handle]).v_Timeout = vp_Timeout;
                }
                /* -LMSqb93565 */

                pl_CGPS_FixReqInd = (t_CGPS_FixReqInd * ) MC_RTK_GET_MEMORY( sizeof(*pl_CGPS_FixReqInd) );

                pl_CGPS_FixReqInd->v_Handle = (t_CgpsNaf *)vp_Handle;
                pl_CGPS_FixReqInd->v_Result = &vl_Result;

                if ( !MC_RTK_SEM_CONSUME( CGPS_SEM1 )) /* prevent other applis to use the semaphore #2 */
                    { MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix : 0 != sem_wait(CGPS_SEM1)")); }

                MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                           PROCESS_CGPS, 0,
                                           CGPS_FIX_REQ_IND,
                                           (t_MsgHeader *)pl_CGPS_FixReqInd);

                if ( !MC_RTK_SEM_CONSUME( CGPS_SEM2 )) /* Wait for CGPS to complete processing of the request */
                    { MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix : 0 != sem_wait(CGPS_SEM2)")); }
                if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM1 )) /* allow another application to use sem #2 */
                    { MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix : 0 != sem_post(CGPS_SEM1)")); }

                if( !vl_Result )
                {
                    vl_Return = K_CGPS_ERR_GPS_NOT_STARTED;
                }
            }
            else
            {
                 vl_Return = K_CGPS_ERR_UNSUPPORTED_SERVICE;
            }
        }
        else
        {
            MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix parameters invalid"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }
    }

    return vl_Return;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/**************************************************************************************************/
/* CGPS0_14SleepExpired : Shut down the GPS when CGPS exits SLEEP                                 */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_14SleepExpired(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    p_FsmObject = p_FsmObject;

    MC_CGPS_TRACE(("CGPS0_14SleepExpired Sleep + coma expired go in OFF state (DEEP coma not allowed)"));
    MC_HALGPS_POWERDOWN();

    vg_CGPS_GpsState = K_CGPS_GPS_OFF;

    return pl_NextState;
}


/* +LMSqb93565 : Modifications to enclosed function*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 16
/**************************************************************************************************/
/* CGPS0_16SendDataToNaf : Send navigation data to requesting applications                        */
/**************************************************************************************************/
void CGPS0_16SendDataToNaf(void)
{
    uint8_t  vl_i;
    uint32_t vl_CurrentTime = GN_GPS_Get_OS_Time_ms();
    uint8_t  input_Valid_2D_Fix;                 /* Original value of 2D fix */
    uint8_t  input_Valid_3D_Fix;                 /* Original value of 3D fix */


#ifdef CMCC_LOGGING_ENABLE
    s_GN_GPS_Nav_Data * pl_nav_data_to_use;
#endif //#ifdef CMCC_LOGGING_ENABLE

    /*++ LMSqb65223 */
    input_Valid_2D_Fix = vg_Latest_Fix_Nav_Data.Valid_SingleFix;
    input_Valid_3D_Fix = vg_Latest_Fix_Nav_Data.Valid_SingleFix;
    /*-- LMSqb65223 */

    /* There is a New NAF Nav Solution available.*/
    for ( vl_i = 0 ; vl_i < K_CGPS_MAX_NUMBER_OF_NAF; vl_i++ )
    {
        /*check if the application is ready to receive data and if it has already received it*/
        if(   ( s_CgpsNaf[vl_i].v_State         == K_CGPS_NAF_READY )
           && ( s_CgpsNaf[vl_i].p_Callback      != NULL             )
           && ( s_CgpsNaf[vl_i].v_Suspended     == 0                )
           && ( s_CgpsNaf[vl_i].v_AreaEventType == K_CGPS_AREA_EVENT_NOT_SET )
          )
        {
            uint8_t vl_timeout;
            uint8_t vl_notify_this_user = FALSE;
            uint8_t vl_SingleShotDone   = FALSE;
            s_GN_GPS_Nav_Data * pl_nav_data_to_use = NULL;

            /* We have three cases:
                K_CGPS_SINGLE_SHOT := Update only if QoP is satisfied or Timeout
                K_CGPS_PERIODIC := Update on Deadline_OS_Time_ms overflow
                K_CGPS_SINGLE_SHOT_WITH_PERIODIC_UPDATE := Update if QoP is satisfied,Timeout or Deadline_OS_Time_ms
                */

            if(    ( s_CgpsNaf[vl_i].v_Mode == K_CGPS_SINGLE_SHOT )
                || ( s_CgpsNaf[vl_i].v_Mode == K_CGPS_SINGLE_SHOT_WITH_PERIODIC_UPDATE ) )
            {
                /* ++ LMSqb65223 */
                /* Use latest valid fix for single shot. In case of timeout, use latest data */
                if( s_CgpsNaf[vl_i].v_Timeout )
                {
                    vl_timeout = (vl_CurrentTime >= s_CgpsNaf[vl_i].v_Timeout);
                }
                else
                {
                    vl_timeout = 0;
                }

                pl_nav_data_to_use = vl_timeout ? &vg_Nav_Data : &vg_Latest_Fix_Nav_Data;
                /* -- LMSqb65223 */

                if(    (vl_timeout)    /* Time out */
                /* ++ LMSqb65223 */
                    || (   ( pl_nav_data_to_use->OS_Time_ms > 0  )
                        && (    ((vl_CurrentTime - pl_nav_data_to_use->OS_Time_ms) < ( s_CgpsNaf[vl_i].v_AgeLimit ))
                             || ( s_CgpsNaf[vl_i].v_AgeLimit     == 0 )
                           )
                       )
                  )
                /* -- LMSqb65223 */
                {
                    /* ++ LMSqb65223 */
                    vl_notify_this_user = GN_GPS_ReQual_Nav_Data(pl_nav_data_to_use,
                                                                 s_CgpsNaf[vl_i].v_HorizAccuracy ,
                                                                 s_CgpsNaf[vl_i].v_VertAccuracy   );
                    /* -- LMSqb65223 */

                    MC_CGPS_TRACE(("CGPS0_16SendDataToNaf singleshot quality is %i reached",vl_notify_this_user));
#ifdef CMCC_LOGGING_ENABLE

                    if (vl_timeout && (s_CgpsNaf[vl_i].v_AlreadyUpdated == FALSE))
                    {
                        if(!vl_notify_this_user)
                        {
                            s_CgpsNaf[vl_i].v_AlreadyUpdated = TRUE;
                            CGPS9_23LogPosition(vl_i, CGPS_SUPL_CMCC_REPONSE_TIMEOUT_CODE , pl_nav_data_to_use );
                        }
                    }
#endif
                    vl_notify_this_user = vl_notify_this_user ? vl_notify_this_user : vl_timeout;

                    vl_SingleShotDone   = vl_notify_this_user;
                }
            }

            /* if the application requests for a periodic fix and the currentTime doesn't match with its fix rate */
            if(    ( s_CgpsNaf[vl_i].v_Mode == K_CGPS_PERIODIC                         )
                || ( s_CgpsNaf[vl_i].v_Mode == K_CGPS_SINGLE_SHOT_WITH_PERIODIC_UPDATE )
              )
            {
                /* ++ LMSqb65223 */
                /* PERIODIC FIX REQUESTED */
                vl_timeout = (vl_CurrentTime >= s_CgpsNaf[vl_i].v_DeadlineOSTimeMS);
                pl_nav_data_to_use = &vg_Nav_Data;
                /* + LMSqc37499 */
                /* If fix is available in tracking mode. Send the update as soon as possible and reload */
                /* The requirement is as follows : */
                /* 1. Irrespective of periodic interval, the fix should be sent as soon as available */
                /* 2. The periodic interval should be as before, with just the fix sent being out of order */
                if(    (s_CgpsNaf[vl_i].v_FirstFixDone == FALSE )
                    && (    vg_Nav_Data.Valid_SingleFix
                         || vg_Nav_Data.Valid_NavMode
                       )
                    )
                {
                    vl_notify_this_user = TRUE;
                    s_CgpsNaf[vl_i].v_FirstFixDone = TRUE;
                }
                /* -LMSqc37499 */
                /* -- LMSqb65223 */

                if( vl_timeout )
                {
                    vl_notify_this_user = TRUE;

                    /* + LMSqc37231 : con-381 issue */
                    /* if position stored in vg_Nav_Data & vg_Latest_Fix_Nav_Data are within 1 timeout period of each other
                                    then use vg_Latest_Fix_Nav_Data to send the fix info
                                    The reason being that in CGPS0_01timerExpired , if vg_Nav_Data has a valid fix info, then the same
                                    is copied into vg_Latest_Fix_Nav_Data.
                                    The only case where vg_Nav_Data & vg_Latest_Fix_Nav_Data dont have indentical time stamps is if
                                    the position is generated from the SUPL layer and has been provided by the GN_SUPL_Position_Resp_Out
                                    function call.
                                    */
                   /* +++ LMSqc37231_2 : */
                   /*This is no longer necessary as the same functionality is provided by patch LMSqc37499.
                    37231 was introduced as fix available from SUPL END was not being provided
                    to higher layers in periodic fix cases.
                    37499 introduced change to display first fix as soon as available in periodic fix,
                    which means a Fix Available from SUPL END is provided to application.
                    */
                   /*if(
                        (vg_Nav_Data.OS_Time_ms < vg_Latest_Fix_Nav_Data.OS_Time_ms ) ||
                        ( (vg_Nav_Data.OS_Time_ms - vg_Latest_Fix_Nav_Data.OS_Time_ms ) < s_CgpsNaf[vl_i].Deadline_OS_Time_ms )
                        )
                    {
                        pl_nav_data_to_use = &vg_Latest_Fix_Nav_Data;
                    }*/
                    /* +++ LMSqc37231_2 : con-381 issue  */
                    /* - LMSqc37231: con-381 issue */
                }
            }
            //CGPS9_03LogPosition(vl_i, pl_nav_data_to_use );
            /* ER - 322372 */
            if(vl_notify_this_user)
            {
               t_cgps_NavData vl_NavDataToSend;
                /* ++ LMSqc26033 */
                if( (e_cgps_NavDataType)s_CgpsNaf[vl_i].v_OutputType == K_CGPS_NMEA_AND_C_STRUCT )
                {
                    CGPS0_36GetNafData(pl_nav_data_to_use,
                                   &vl_NavDataToSend,
                                   K_CGPS_C_STRUCT,
                                   s_CgpsNaf[vl_i].v_Mask);
                    (s_CgpsNaf[vl_i].p_Callback)( vl_NavDataToSend );
                    MC_RTK_FREE_MEMORY( vl_NavDataToSend.p_NavData );

                    CGPS0_36GetNafData(pl_nav_data_to_use,
                                   &vl_NavDataToSend,
                                   K_CGPS_NMEA,
                                   s_CgpsNaf[vl_i].v_Mask);

                }
                else
                /* -- LMSqc26033 */
                    CGPS0_36GetNafData(pl_nav_data_to_use,
                                       &vl_NavDataToSend,
                                       (e_cgps_NavDataType)s_CgpsNaf[vl_i].v_OutputType,
                                       s_CgpsNaf[vl_i].v_Mask);

                if(    pl_nav_data_to_use->Valid_SingleFix == TRUE
                    || pl_nav_data_to_use->Valid_NavMode == TRUE )
                {
#ifdef CMCC_LOGGING_ENABLE

                    if (pl_nav_data_to_use->Valid_SingleFix && (s_CgpsNaf[vl_i].v_AlreadyUpdated == FALSE))
                    {
                        s_CgpsNaf[vl_i].v_AlreadyUpdated = TRUE;
                        CGPS9_23LogPosition(vl_i, CGPS_SUPL_CMCC_POSITION_RESULT_CODE , pl_nav_data_to_use );
                    }
#endif
/* +LMSQC22162 */
                    MC_CGPS_SEND_PSTE_NMEA_FIX_WGS84_REPORT(pl_nav_data_to_use->Latitude, pl_nav_data_to_use->Longitude, pl_nav_data_to_use->Altitude_Ell);
/* -LMSQC22162 */
                }

                /* Send data */
                (s_CgpsNaf[vl_i].p_Callback)( vl_NavDataToSend );
                MC_RTK_FREE_MEMORY( vl_NavDataToSend.p_NavData );

                /*Store the last callback call time*/
                s_CgpsNaf[vl_i].v_LastApplicationUpdateTime = vl_CurrentTime;

                if(   ( vl_SingleShotDone )
                    /* ++ LMSqb91788    ANP 03/07/2009 */
                    /* ++ LMSqb85756 */
                   && (K_CGPS_NAF_READY == s_CgpsNaf[vl_i].v_State)
                    /* -- LMSqb85756 */
                    /* -- LMSqb91788    ANP 03/07/2009 */
                  )
                {
                    /* If a Single Shot Fix was satsified then de-register the callback.*/
                    /* If the user was happy then we assume it will stop the service.*/
                    /* However, if it was not happy, it might re-register a new callback for another go.*/
                    s_CgpsNaf[vl_i].v_State    = K_CGPS_NAF_REGISTERED;
                    s_CgpsNaf[vl_i].p_Callback = NULL;

#ifdef CMCC_LOGGING_ENABLE
                    s_CgpsNaf[vl_i].v_AlreadyUpdated = FALSE;
#endif /*CMCC_LOGGING_ENABLE*/

                    /* Restore validity flags */
                    vg_Latest_Fix_Nav_Data.Valid_SingleFix = input_Valid_2D_Fix;
                    vg_Latest_Fix_Nav_Data.Valid_NavMode   = input_Valid_3D_Fix;

/* +LMSQC22162 */
                    MC_CGPS_SEND_PSTE_NMEA_FIX_COMPLETE();
/* -LMSQC22162 */

                    CGPS0_52ConfigureCgpsNextState();
                }
                else
                {
                    /* Periodic fix */
                    if(   ( s_CgpsNaf[vl_i].v_Mode == K_CGPS_PERIODIC )
                       || ( s_CgpsNaf[vl_i].v_Mode == K_CGPS_SINGLE_SHOT_WITH_PERIODIC_UPDATE )
                      )
                    {
                        /* +LMSqc37499 */
                        if( vl_CurrentTime >= s_CgpsNaf[vl_i].v_DeadlineOSTimeMS )
                        {
                        /* Compute next Timeout */
                        /* We compute next timeout only if previous timeout had expired */
                            /* If we are sending a fix report out of order, then timeout will not have expired */
                        /* Compute next Timeout */
                        s_CgpsNaf[vl_i].v_DeadlineOSTimeMS += s_CgpsNaf[vl_i].v_FixRate;
                    }
                        /* -LMSqc37499 */
                    }
                }
            }   /* User is notified */
        }  /* valid Handle */
        else if(    ( s_CgpsNaf[vl_i].v_State         == K_CGPS_NAF_READY )
                 && ( s_CgpsNaf[vl_i].p_Callback      != NULL             )
                 && ( s_CgpsNaf[vl_i].v_Suspended     == 0                )
                 && ( s_CgpsNaf[vl_i].v_AreaEventType != K_CGPS_AREA_EVENT_NOT_SET )
               )
        {
            bool vl_HasTimedOut      = FALSE;
            bool vl_IsQosAttained    = FALSE;
            bool vl_IsReportingReqd  = FALSE;
            t_Cgps_Coordinate vl_CurrentPosition;
            s_GN_GPS_Nav_Data * pl_nav_data_to_use = NULL;

            if( s_CgpsNaf[vl_i].v_Mode == K_CGPS_SINGLE_SHOT )
            {
                vl_HasTimedOut  = (vl_CurrentTime >= s_CgpsNaf[vl_i].v_Timeout);

                if( GN_GPS_ReQual_Nav_Data(&vg_Nav_Data, s_CgpsNaf[vl_i].v_HorizAccuracy, s_CgpsNaf[vl_i].v_VertAccuracy ))
                {
                    vl_CurrentPosition.v_LatitudeSign = MC_CGPS_LAT_CONV_WGS84_DEG_TO_GAD_SIGN(vg_Nav_Data.Latitude);
                    vl_CurrentPosition.v_Latitude     = MC_CGPS_LAT_CONV_WGS84_DEG_TO_GAD(vg_Nav_Data.Latitude);
                    vl_CurrentPosition.v_Longitude    = MC_CGPS_LONG_CONV_WGS84_DEG_TO_GAD(vg_Nav_Data.Longitude);

                    vl_IsQosAttained =  TRUE;
                }
   
                pl_nav_data_to_use = &vg_Nav_Data ;
            }
            else if( s_CgpsNaf[vl_i].v_Mode == K_CGPS_PERIODIC )
            {
                if( vl_CurrentTime >= s_CgpsNaf[vl_i].v_DeadlineOSTimeMS )
                {
                    if( GN_GPS_ReQual_Nav_Data(&vg_Latest_Fix_Nav_Data, s_CgpsNaf[vl_i].v_HorizAccuracy, s_CgpsNaf[vl_i].v_VertAccuracy ) )
                    {
                        vl_CurrentPosition.v_LatitudeSign = MC_CGPS_LAT_CONV_WGS84_DEG_TO_GAD_SIGN(vg_Latest_Fix_Nav_Data.Latitude);
                        vl_CurrentPosition.v_Latitude     = MC_CGPS_LAT_CONV_WGS84_DEG_TO_GAD(vg_Latest_Fix_Nav_Data.Latitude);
                        vl_CurrentPosition.v_Longitude    = MC_CGPS_LONG_CONV_WGS84_DEG_TO_GAD(vg_Latest_Fix_Nav_Data.Longitude);

                        vl_IsQosAttained =  TRUE;
                    }
                    pl_nav_data_to_use = &vg_Latest_Fix_Nav_Data ;

                    s_CgpsNaf[vl_i].v_DeadlineOSTimeMS += s_CgpsNaf[vl_i].v_FixRate;
                }
            }

            if( vl_IsQosAttained )
            {

                if( CGPS4_48CheckForGeoAreaEvent( s_CgpsNaf[vl_i].p_TargetArea,
                                           s_CgpsNaf[vl_i].v_AreaEventType,
                                           s_CgpsNaf[vl_i].v_CurrentPositionStatus,
                                          &s_CgpsNaf[vl_i].v_CurrentPositionStatus,
                                          &vl_CurrentPosition))
                {
                    vl_IsReportingReqd = TRUE;
                }
            }

            if( vl_IsReportingReqd )
            {
                t_cgps_NavData vl_NavDataToSend;

                if( (e_cgps_NavDataType)s_CgpsNaf[vl_i].v_OutputType == K_CGPS_NMEA_AND_C_STRUCT )
                {
                    CGPS0_36GetNafData( pl_nav_data_to_use,
                                        &vl_NavDataToSend,
                                        K_CGPS_C_STRUCT,
                                        s_CgpsNaf[vl_i].v_Mask);
                    (s_CgpsNaf[vl_i].p_Callback)( vl_NavDataToSend );
                    MC_RTK_FREE_MEMORY( vl_NavDataToSend.p_NavData );

                    CGPS0_36GetNafData( pl_nav_data_to_use,
                                        &vl_NavDataToSend,
                                        K_CGPS_NMEA,
                                        s_CgpsNaf[vl_i].v_Mask);
                }
                else
                    /* -- LMSqc26033 */
                    CGPS0_36GetNafData( pl_nav_data_to_use,
                                        &vl_NavDataToSend,
                                        (e_cgps_NavDataType)s_CgpsNaf[vl_i].v_OutputType,
                                        s_CgpsNaf[vl_i].v_Mask);

                /* Send data */
                (s_CgpsNaf[vl_i].p_Callback)( vl_NavDataToSend );
                MC_RTK_FREE_MEMORY( vl_NavDataToSend.p_NavData );

                s_CgpsNaf[vl_i].v_LastApplicationUpdateTime = vl_CurrentTime;
            }


            if(    ( vl_HasTimedOut || vl_IsReportingReqd )
               &&  K_CGPS_NAF_READY   == s_CgpsNaf[vl_i].v_State
               &&  K_CGPS_SINGLE_SHOT == s_CgpsNaf[vl_i].v_Mode
              )
            {
                s_CgpsNaf[vl_i].v_State          = K_CGPS_NAF_REGISTERED;
                s_CgpsNaf[vl_i].p_Callback       = NULL;
#ifdef CMCC_LOGGING_ENABLE
                s_CgpsNaf[vl_i].v_AlreadyUpdated = FALSE;
#endif /*CMCC_LOGGING_ENABLE*/
                s_CgpsNaf[vl_i].v_AreaEventType          = K_CGPS_AREA_EVENT_NOT_SET;
                s_CgpsNaf[vl_i].v_CurrentPositionStatus  = K_CGPS_POSITION_UNKNOWN;

                if( s_CgpsNaf[vl_i].p_TargetArea != NULL )MC_RTK_FREE_MEMORY( s_CgpsNaf[vl_i].p_TargetArea );
/* +LMSQC22162 */
                MC_CGPS_SEND_PSTE_NMEA_FIX_COMPLETE();
/* -LMSQC22162 */

                CGPS0_52ConfigureCgpsNextState();
            }

        }
        else
        {
        /* In case of location forward, there is no update to application and hence NAF session is in suspended state.
                   This else part has been added explicitly to handle the case where GN_SUPL_Start_ThirdParty_Location_Transfer() call
                   didnt translate into any Connection Request out.
                    In such cases the CGPS would abort session on timeout set by user in setting up the location transfer
                    We need to do,
                    1.  Reset NAF sessions
                    2.  abort the SUPL session
                    3.  Reset GPS state */
            if(   ( s_CgpsNaf[vl_i].v_State     == K_CGPS_NAF_SUSPENDED )
               && ( s_CgpsNaf[vl_i].p_Callback  == NULL             )
              )
            {
                if(s_CgpsNaf[vl_i].v_Timeout >0)
                {
                    //Mulity by 1000 to convert to ms assuming that the user provides in seconds as vl_CurrentTime is in milliseconds
                    if((s_CgpsNaf[vl_i].v_Timeout) <= vl_CurrentTime)
                    {
                       return;
                    }
                    else
                    {
#if defined (AGPS_UP_FTR)
                       CGPS5_30VerifySuplActivity();
#endif /*AGPS_UP_FTR*/

                        s_CgpsNaf[vl_i].v_State             = K_CGPS_NAF_UNUSED;
                        s_CgpsNaf[vl_i].p_Callback          = NULL;
                        s_CgpsNaf[vl_i].v_Mode              = K_CGPS_SINGLE_SHOT;
                        s_CgpsNaf[vl_i].v_OutputType        = 0xFF;
                        s_CgpsNaf[vl_i].v_Mask              = 0xFFFF;
                        s_CgpsNaf[vl_i].v_FixRate           = 0;
                        s_CgpsNaf[vl_i].v_AgeLimit          = 0;
                        s_CgpsNaf[vl_i].v_HorizAccuracy     = 0;
                        s_CgpsNaf[vl_i].v_VertAccuracy      = 0;
                        s_CgpsNaf[vl_i].v_DeadlineOSTimeMS  = 0;
                        s_CgpsNaf[vl_i].v_Suspended         = CGPS_MAX_PRIORITY_VALUE;
                        s_CgpsNaf[vl_i].v_Config            = 0;
#ifdef CMCC_LOGGING_ENABLE
                        s_CgpsNaf[vl_i].v_AlreadyUpdated    = FALSE;
                        s_CgpsNaf[vl_i].v_RegisterTime       = 0;
#endif /*CMCC_LOGGING_ENABLE*/

                        s_CgpsNaf[vl_i].v_LastApplicationUpdateTime = 0;
                    }
                }
            }

            CGPS0_52ConfigureCgpsNextState();
        }
    }   /* i, Next registered user.*/

    return;
}
/* -LMSqb93565 */


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 17
/**************************************************************************************************/
/* CGPS0_17GetNMEA : get NMEA buffer according to the application's mask                          */
/**************************************************************************************************/
uint16_t CGPS0_17GetNMEA( s_GN_GPS_Nav_Data *vp_navData, uint16_t vp_Mask, uint16_t vp_Max_Size, char* pp_NMEA_Buffer )
{
    uint16_t vl_NMEA_bytes = 0;

    if( vp_Mask != 0 )  /* Message Mask is set*/
    {
        /* ++ LMSqb65223 */
        vl_NMEA_bytes = GN_GPS_Encode_NMEA(vp_Max_Size, pp_NMEA_Buffer , vp_Mask, vp_navData );
        /* -- LMSqb65223 */
    }

    return( vl_NMEA_bytes );
}



#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 18
/**************************************************************************************************/
/* CGPS0_18NewNafRegistered : take action when a new application has been registered              */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_18NewNafRegistered(t_RtkObject* p_FsmObject)
{
    t_CgpsRegisterResult *vl_retValue;
    t_CGPS_NewNaf        *vl_Msg = (t_CGPS_NewNaf *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    const t_OperationDescriptor* pl_NextState = SAME;
    vl_retValue  = vl_Msg->v_out;

    vl_retValue->v_Result = K_CGPS_NO_ERROR;


    MC_CGPS_TRACE(("CGPS0_18NewNafRegistered CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));

#ifdef CMCC_LOGGING_ENABLE
    CGPS9_23LogPosition ( vl_index, CGPS_SUPL_CMCC_AGPS_SESSION_STARTED, NULL);
#endif

    if( CGPS0_29SearchFreeHandle(&vl_retValue))
    {
        if(vl_Msg->p_AppIdInfo != NULL)
        {
            if(vl_Msg->p_AppIdInfo->v_ApplicationIDInfo_present)
            {
                s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.v_ApplicationIDInfo_present = vl_Msg->p_AppIdInfo->v_ApplicationIDInfo_present;
                memcpy( s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.a_ApplicationProvider, vl_Msg->p_AppIdInfo->a_ApplicationProvider, K_CGPS_MAX_APP_PROVIDER_LEN);
                memcpy( s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.a_ApplicationName,     vl_Msg->p_AppIdInfo->a_ApplicationName,     K_CGPS_MAX_APP_NAME_LEN);
                memcpy( s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.a_ApplicationVersion,  vl_Msg->p_AppIdInfo->a_ApplicationVersion,  K_CGPS_MAX_APP_VERSION_LEN);

                MC_CGPS_TRACE(("CGPS0_18NewNafRegistered: Application Provider - <%s>",          s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.a_ApplicationProvider));
                MC_CGPS_TRACE(("CGPS0_18NewNafRegistered: Application Name - <%s>",              s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.a_ApplicationName));
                MC_CGPS_TRACE(("CGPS0_18NewNafRegistered: Application Version - <%s>",           s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.a_ApplicationVersion));
                MC_CGPS_TRACE(("CGPS0_18NewNafRegistered: Application ID Info present - <%d>",   s_CgpsNaf[vl_retValue->v_Handle].v_ApplicationIDInfo.v_ApplicationIDInfo_present));

                //Now that we have copied the application id data so free the memory
                MC_RTK_FREE_MEMORY( vl_Msg->p_AppIdInfo );
                vl_Msg->p_AppIdInfo = NULL;
            }
        }
    }

    MC_CGPS_TRACE(("CGPS0_18NewNafRegistered Switch exit"));

    if(vl_retValue->v_Result == K_CGPS_NO_ERROR)
    {
        /* increase the number of applications */
        vg_CGPS_Naf_Count++;
    }

    CGPS0_52ConfigureCgpsNextState();

    MC_CGPS_TRACE(("CGPS0_18NewNafRegistered exit with handle : %d", vl_retValue->v_Handle));
    pl_NextState = a_CGPSRunState;
#ifndef __RTK_E__
    MC_CGPS_TRACE(("CGPS0_18NewNafRegistered signal SEM2", vl_retValue->v_Handle));
#endif /* __RTK_E__ */
    if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM2 ))
        { MC_CGPS_TRACE(("CGPS0_18NewNafRegistered : 0 != sem_post(CGPS_SEM2)")); }

    return pl_NextState;
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 19
/**************************************************************************************************/
/* CGPS0_19NafDeregistered : take action when an application has been deregistered                */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_19NafDeregistered(t_RtkObject* p_FsmObject)
{
    t_CGPS_NafDeregistered * pl_Naf_Deregister = (t_CGPS_NafDeregistered *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    const t_OperationDescriptor* pl_NextState = SAME;
    uint32_t vl_HandleIndex=pl_Naf_Deregister->v_Handle;

    MC_CGPS_TRACE(("CGPS0_19NafDeregistered called with handle : %d", pl_Naf_Deregister->v_Handle));

    if(!CGPS4_01IsRegistered(pl_Naf_Deregister->v_Handle))
    {
        pl_Naf_Deregister->v_out->v_Result = K_CGPS_ERR_INVALID_HANDLE;
    }
    else
    {
        pl_Naf_Deregister->v_out->v_Result = K_CGPS_NO_ERROR;
        /* reset all fields */
        (s_CgpsNaf[vl_HandleIndex]).v_State          = K_CGPS_NAF_UNUSED;
        (s_CgpsNaf[vl_HandleIndex]).p_Callback       = NULL;
        (s_CgpsNaf[vl_HandleIndex]).p_ThirdPartyPositionCallback   = NULL;
        (s_CgpsNaf[vl_HandleIndex]).v_Mode           = K_CGPS_SINGLE_SHOT;
        (s_CgpsNaf[vl_HandleIndex]).v_OutputType     = 0xFF;
        (s_CgpsNaf[vl_HandleIndex]).v_Mask           = 0xFFFF;
        (s_CgpsNaf[vl_HandleIndex]).v_FixRate        = 0;
        (s_CgpsNaf[vl_HandleIndex]).v_AgeLimit       = 0;
        (s_CgpsNaf[vl_HandleIndex]).v_HorizAccuracy  = 0;
        (s_CgpsNaf[vl_HandleIndex]).v_VertAccuracy   = 0;
        (s_CgpsNaf[vl_HandleIndex]).v_Suspended      = CGPS_MAX_PRIORITY_VALUE;
        (s_CgpsNaf[vl_HandleIndex]).v_DeadlineOSTimeMS          = 0;
        (s_CgpsNaf[vl_HandleIndex]).v_LastApplicationUpdateTime = 0;
        (s_CgpsNaf[vl_HandleIndex]).v_CurrentPositionStatus     = K_CGPS_POSITION_UNKNOWN;

        (s_CgpsNaf[vl_HandleIndex]).v_ApplicationIDInfo.v_ApplicationIDInfo_present = FALSE;
        memset(&((s_CgpsNaf[vl_HandleIndex]).v_ApplicationIDInfo), 0 , sizeof(t_cgps_ApplicationID_Info));

        if( (s_CgpsNaf[vl_HandleIndex]).v_AreaEventType != K_CGPS_AREA_EVENT_NOT_SET )
        {
            if( (s_CgpsNaf[vl_HandleIndex]).p_TargetArea != NULL )
                MC_RTK_FREE_MEMORY( (s_CgpsNaf[vl_HandleIndex]).p_TargetArea );
        }

        (s_CgpsNaf[vl_HandleIndex]).v_AreaEventType  = K_CGPS_AREA_EVENT_NOT_SET;

#ifdef CMCC_LOGGING_ENABLE
        (s_CgpsNaf[vl_HandleIndex]).v_AlreadyUpdated = FALSE;
#endif /*CMCC_LOGGING_ENABLE*/

        /* decrease the number of applications */
        vg_CGPS_Naf_Count--;
#if defined(AGPS_FTR)
        CGPS7_38CheckMolrSession(&s_CgpsNaf[vl_HandleIndex]);
#endif
/* +LMSQC22162 */

        CGPS0_52ConfigureCgpsNextState();

/* -LMSQC22162 */

/* +LMSQC22162 */
        MC_CGPS_SEND_PSTE_NMEA_FIX_COMPLETE();
/* -LMSQC22162 */

    }


/* +LMSqc38060 */
#if defined (AGPS_UP_FTR)
    CGPS5_30VerifySuplActivity();
#endif
/* -LMSqc38060 */


    if (! MC_RTK_SEM_PRODUCE( CGPS_SEM2 ))
        { MC_CGPS_TRACE(("CGPS0_19NafDeregistered : 0 != sem_post(CGPS_SEM2)")); }
    return pl_NextState;
}


#if defined(AGPS_UP_FTR) || defined(AGPS_FTR)
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 21
/**************************************************************************************************/
/* CGPS0_21NotificationRegisterReq : registers Notifications callback for the user                */
/**************************************************************************************************/
void CGPS0_21NotificationRegisterReq(t_cgps_NotificationCallback vp_Callback)
{
    MC_CGPS_TRACE(("CGPS0_21NotificationRegisterReq"));
    vg_NotificationCallback = vp_Callback;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 22
/**************************************************************************************************/
/* CGPS0_22SuplNotificationUserAnswerReq : returns the user answer of a SUPL notification message */
/**************************************************************************************************/
void CGPS0_22NotificationUserAnswerReq(e_cgps_UserAnswer vp_AnswerUser, void * pp_Handle)
{
    t_CGPS_UserNotifyAns* pl_UserNotifyAns=NULL;
    pl_UserNotifyAns  = (t_CGPS_UserNotifyAns*)MC_RTK_GET_MEMORY(sizeof(*pl_UserNotifyAns));

    pl_UserNotifyAns->v_UserAnswer = vp_AnswerUser;
    pl_UserNotifyAns->v_Handle     = pp_Handle;

    MC_RTK_SEND_MSG_TO_PROCESS( PROCESS_CGPS,0,
                                PROCESS_CGPS,0,
                                CGPS_USER_NOTIFY_ANS,
                                (t_MsgHeader*)pl_UserNotifyAns);
}
#endif /*AGPS_UP_FTR || AGPS_FTR */


#if defined AGPS_UP_FTR || defined AGPS_FTR
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 23
/**************************************************************************************************/
/* CGPS0_23HandleUserAnswer : returns the user answer of a SUPL notification message              */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_23HandleUserAnswer(t_RtkObject* p_FsmObject)
{
    t_CGPS_UserNotifyAns*        pl_UserNotifyAns = (t_CGPS_UserNotifyAns*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    const t_OperationDescriptor* pl_NextState     = SAME;

    uint8_t          vl_index       = 0;
    bool        vl_Result      = FALSE;
    uint32_t         vl_NotifIndex  = (uint32_t)pl_UserNotifyAns->v_Handle;
#ifdef AGPS_UP_FTR
    e_GN_Status vl_Status      = GN_SUCCESS;
#endif
#ifdef AGPS_FTR
    uint8_t          vl_requestType = 0;
#endif


    e_cgps_UserAnswer vp_AnswerUser = pl_UserNotifyAns->v_UserAnswer;

    MC_CGPS_TRACE(("CGPS0_23HandleUserAnswer : User Response = %i", vp_AnswerUser));

    /* check if the notification is still pending */
    if(!s_CgpsNotification[vl_NotifIndex].v_IsPending)
    {
        /* ++LMSqb95231  */
        return pl_NextState;
        /* --LMSqb95231  */
    }

    /* send response to the SUPL library */
    if(s_CgpsNotification[vl_NotifIndex].v_HandleType == K_CGPS_SUPL_HANDLE)
    {
#ifdef AGPS_UP_FTR
        vl_Result = GN_SUPL_Notification_Rsp_In((void*)(uint32_t)(s_CgpsNotification[vl_NotifIndex].v_Handle),
                                                &vl_Status,
                                                vp_AnswerUser==K_CGPS_USER_ACCEPT?TRUE:FALSE);

        /* Removed check on answer because this is irrelevant to whether or not SMS was accepted.
        Also SUPL Context MUST NOT BE DELETED when consent is denied as SUPL library still needs the
        context to send a SUPL END back */
        if(!vl_Result)
        {
            t_lsimup_SmsPushRjtM * pl_lsimup_SmsPushRjt=NULL;
            pl_lsimup_SmsPushRjt = (t_lsimup_SmsPushRjtM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_SmsPushRjt));
            pl_lsimup_SmsPushRjt->v_SessionHandle   = vg_CGPS_LSIMUP_Session_Handle;
            pl_lsimup_SmsPushRjt->v_RejectionReason = K_LSIMUP_REJECTION_REASON_SMS_PUSH_FAIL;

            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                       PROCESS_LSIMUP, 0,
                                       CGPS_LSIMUP_SMS_PUSH_RJT,
                                       (t_MsgHeader *)pl_lsimup_SmsPushRjt);

            for ( vl_index = 0;
                  s_CgpsSupl[vl_index].v_GPSHandle != (void*)(uint32_t)(s_CgpsNotification[vl_NotifIndex].v_Handle);
                  vl_index++);

            CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);
        }
        else
        {
            t_lsimup_SmsPushRspM * pl_lsimup_SmsPushRsp=NULL;
            pl_lsimup_SmsPushRsp =  (t_lsimup_SmsPushRspM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_SmsPushRsp));
            pl_lsimup_SmsPushRsp->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                       PROCESS_LSIMUP, 0,
                                       CGPS_LSIMUP_SMS_PUSH_RSP,
                                       (t_MsgHeader *)pl_lsimup_SmsPushRsp);
        }

        GN_SUPL_Handler();
#endif /* AGPS_UP_FTR */
    }
#ifdef AGPS_FTR
    else
    {
        if(s_CgpsNotification[vl_NotifIndex].v_DefaultNotificationType == K_CGPS_NOTIFICATION_ONLY)
        {
            vl_requestType = LSIMCP_MTLR_NOTIFY;
        }
        else
        {
            vl_requestType = LSIMCP_MTLR_VERIFY;

            if(vp_AnswerUser==K_CGPS_USER_ACCEPT)
            {
                CGPS7_24NotifySendCnf(s_CgpsNotification[vl_NotifIndex].v_Handle,vl_requestType,LSIMCP_USER_ACCEPT);
            }

            else if( vp_AnswerUser==K_CGPS_USER_REJECT )
            {
                CGPS7_24NotifySendCnf(s_CgpsNotification[vl_NotifIndex].v_Handle,vl_requestType,LSIMCP_USER_REJECT);
            }
        }
    }
#endif

    /* reset notification context */
    vg_CGPS_Notification_Count--;
    s_CgpsNotification[vl_NotifIndex].v_IsPending  = FALSE;
    s_CgpsNotification[vl_NotifIndex].v_Handle     = 0;
    s_CgpsNotification[vl_NotifIndex].v_Timeout    = 0;
    s_CgpsNotification[vl_NotifIndex].v_DefaultNotificationType = K_CGPS_NOTIFICATION_ONLY;

    /* If any CP notifications are closed, check if GPS and update loop timer still needs to be running */
    CGPS0_52ConfigureCgpsNextState();
    return pl_NextState;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 24
/**************************************************************************************************/
/* CGPS0_24NotificationExpired : check if notification has expired                                */
/**************************************************************************************************/
void CGPS0_24NotificationExpired(void)
{
    uint8_t   vl_index       = 0;
    uint32_t  vl_currentTime = GN_GPS_Get_OS_Time_ms();
    bool vl_notificationRsp;

#ifdef AGPS_UP_FTR
    e_GN_Status vl_Status = GN_SUCCESS;
    t_lsimup_SmsPushRspM * pl_lsimup_SmsPushRsp;
    t_lsimup_SmsPushRjtM * pl_lsimup_SmsPushRjt;
#endif /*AGPS_UP_FTR*/

    MC_CGPS_TRACE(("CGPS0_24NotificationExpired"));

    for ( vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_NOTIFICATIONS ; vl_index++ )
    {
        if(    s_CgpsNotification[vl_index].v_IsPending
            && s_CgpsNotification[vl_index].v_Timeout < vl_currentTime
          )
        {
            if(s_CgpsNotification[vl_index].v_HandleType == K_CGPS_SUPL_HANDLE)
            {
#ifdef AGPS_UP_FTR
                switch (s_CgpsNotification[vl_index].v_DefaultNotificationType)
                {
                    case K_CGPS_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA:
                        /* send response to the SUPL library */
                        vl_notificationRsp = GN_SUPL_Notification_Rsp_In((void*)(uint32_t)(s_CgpsNotification[vl_index].v_Handle),
                                                                         &vl_Status,
                                                                         TRUE);
                    break;
                    case K_CGPS_NOTIFICATION_AND_VERIFICATION_DENIED_NA:
                    default:
                        vl_notificationRsp = GN_SUPL_Notification_Rsp_In((void*)(uint32_t)(s_CgpsNotification[vl_index].v_Handle),
                                                                         &vl_Status,
                                                                         FALSE);
                    break;
                }

                GN_SUPL_Handler();

                /* Removed check on answer because this is irrelevant to whether or not SMS was accepted.
                   Also SUPL Context MUST NOT BE DELETED when consent is denied as SUPL library still needs the
                   context to send a SUPL END back */

                if(!vl_notificationRsp )
                {
                    t_lsimup_SmsPushRjtM  *pl_lsimup_SmsPushRjtM=NULL;
                    uint8_t vl_i;
                    pl_lsimup_SmsPushRjt = (t_lsimup_SmsPushRjtM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_SmsPushRjtM));
                    pl_lsimup_SmsPushRjt->v_SessionHandle   = vg_CGPS_LSIMUP_Session_Handle;
                    pl_lsimup_SmsPushRjt->v_RejectionReason = K_LSIMUP_REJECTION_REASON_UNKNOWN;

                    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,   0,
                                               PROCESS_LSIMUP, 0,
                                               CGPS_LSIMUP_SMS_PUSH_RJT,
                                               (t_MsgHeader *)pl_lsimup_SmsPushRjt);

                    for ( vl_i = 0;
                          s_CgpsSupl[vl_i].v_GPSHandle != (void*)(uint32_t)(s_CgpsNotification[vl_index].v_Handle);
                          vl_i++ );

                    CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_i]);
                }
                else
                {
                    t_lsimup_SmsPushRspM    *pl_lsimup_SmsPushRspM=NULL;
                    pl_lsimup_SmsPushRsp = (t_lsimup_SmsPushRspM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_SmsPushRspM));
                    pl_lsimup_SmsPushRsp->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

                    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,   0,
                                               PROCESS_LSIMUP, 0,
                                               CGPS_LSIMUP_SMS_PUSH_RSP,
                                               (t_MsgHeader *)pl_lsimup_SmsPushRsp);
                }
#endif /*AGPS_UP_FTR*/
            }
#ifdef AGPS_FTR
            else
            {
                CGPS7_24NotifySendCnf(s_CgpsNotification[vl_index].v_Handle,LSIMCP_MTLR_VERIFY,LSIMCP_USER_NO_RSP);
            }
#endif
        /* Indicate expiry of notification */
            {

               /* ++LMSqb95231  */
               t_cgps_Notification vl_cgps_Notification;

               memset(&vl_cgps_Notification, 0, sizeof(vl_cgps_Notification));

               /* --LMSqb95231  */
               vl_cgps_Notification.v_Handle = (void*)(uint32_t)vl_index;

               vl_cgps_Notification.v_NotificationType        = K_CGPS_NOTIFICATION_CANCEL;
               vl_cgps_Notification.v_RequestorIdEncodingType = K_CGPS_MAX_ENCODINGTYPE;
               vl_cgps_Notification.v_ClientNameEncodingType  = K_CGPS_MAX_ENCODINGTYPE;
               vl_cgps_Notification.v_CodeWordAlphabet        = K_CGPS_MAX_ENCODINGTYPE;

               vl_cgps_Notification.v_RequestorIdLen  = 0;
               vl_cgps_Notification.v_RequestorIdType = K_CGPS_MAX_FORMATINDICATOR;
               vl_cgps_Notification.v_ClientNameLen   = 0;
               vl_cgps_Notification.v_ClientNameType  = K_CGPS_MAX_FORMATINDICATOR;

               vl_cgps_Notification.v_CodeWordLen  = 0;
               vl_cgps_Notification.v_TonNpi       = 0;
               vl_cgps_Notification.v_TonNpiConfig = 0; /* not populated */

               vl_cgps_Notification.v_LcsServiceTypeId = K_CGPS_NAVIGATION;
               /* ++LMSqb95231  */
               memset(vl_cgps_Notification.a_PhoneNumber, 0, sizeof(vl_cgps_Notification.a_PhoneNumber));
               /* --LMSqb95231  */
               vg_NotificationCallback(&vl_cgps_Notification);
            }

            /* reset SUPL notification context */
            vg_CGPS_Notification_Count--;
            s_CgpsNotification[vl_index].v_IsPending = FALSE;
            s_CgpsNotification[vl_index].v_Handle    = 0;
            s_CgpsNotification[vl_index].v_Timeout   = 0;
            s_CgpsNotification[vl_index].v_DefaultNotificationType = K_CGPS_NOTIFICATION_ONLY;
            /* If any CP notifications are closed, check if GPS and update loop timer still needs to be running */
            CGPS0_52ConfigureCgpsNextState();
        }
    }
}
#endif /*AGPS_UP_FTR || AGPS_FTR*/


#ifdef AGPS_UP_FTR
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 25
/**************************************************************************************************/
/* CGPS0_25SmsPushInd : receive a SMS from network                                                */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_25SmsPushInd(t_RtkObject* p_FsmObject)
/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
{
    const t_OperationDescriptor* pl_NextState  = SAME;

    MC_CGPS_TRACE(("CGPS0_25SmsPushInd : start"));

    if( p_FsmObject != NULL)
    {
        bool                  vl_rejectSMS  = FALSE;
        t_cgps_SuplPush *      pl_SuplPush= NULL;
        pl_SuplPush  = (t_cgps_SuplPush*) MC_RTK_GET_MEMORY ( sizeof( *pl_SuplPush ));
        t_lsimup_SmsPushIndM* pl_SmsPushInd = (t_lsimup_SmsPushIndM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

        if( pl_SuplPush != NULL )
        {
            pl_SuplPush->v_PayloadLength    = pl_SmsPushInd->v_PayloadLength;
            pl_SuplPush->v_HashLength       = pl_SmsPushInd->v_HashLength;
            pl_SuplPush->v_pointerToPayload = pl_SmsPushInd->v_pointerToPayload;
            pl_SuplPush->v_pointerToHash    = pl_SmsPushInd->v_pointerToHash;

            vl_rejectSMS = CGPS0_57HandleSuplInit(pl_SuplPush);

            /* Free the allocated pointer*/
            MC_RTK_FREE_MEMORY(pl_SuplPush);
        }

        if( pl_SmsPushInd->v_pointerToPayload != NULL )
        {
            MC_RTK_FREE_MEMORY(pl_SmsPushInd->v_pointerToPayload);
        }
        if( pl_SmsPushInd->v_pointerToHash != NULL )
        {
            MC_RTK_FREE_MEMORY(pl_SmsPushInd->v_pointerToHash);
        }

        if(vl_rejectSMS)
        {
            t_lsimup_SmsPushRjtM * pl_SmsPushRjt=NULL;
            pl_SmsPushRjt                    = (t_lsimup_SmsPushRjtM*)MC_RTK_GET_MEMORY(sizeof(*pl_SmsPushRjt));
            pl_SmsPushRjt->v_SessionHandle   = vg_CGPS_LSIMUP_Session_Handle;
            pl_SmsPushRjt->v_RejectionReason = K_LSIMUP_REJECTION_REASON_UNKNOWN;

            pl_NextState = SAME;

            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,   0,
                                       PROCESS_LSIMUP, 0,
                                       CGPS_LSIMUP_SMS_PUSH_RJT,
                                       (t_MsgHeader *)pl_SmsPushRjt);
        }
        else
        {
            pl_NextState = a_CGPSRunState;
        }

    }

    MC_CGPS_TRACE(("CGPS0_25SmsPushInd : end"));

    return pl_NextState;
}


/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */




#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 26
/**************************************************************************************************/
/* CGPS0_26AdpInitCnf : LSIMUP initialisation successful                                          */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_26AdpInitCnf(t_RtkObject* p_FsmObject)
{
    t_lsimup_InitCnfM* pl_lsimup_InitCnf      = (t_lsimup_InitCnfM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    const t_OperationDescriptor* pl_NextState = SAME;

    /* Store the LSIMUP session handle */
    vg_CGPS_LSIMUP_Session_Handle = pl_lsimup_InitCnf->v_SessionHandle;
    return pl_NextState;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 27
/**************************************************************************************************/
/* CGPS0_27InitSupl : calls MC_LSIMUP_MOBILE_INFO_REQ to initialise SUPL                          */
/**************************************************************************************************/
void CGPS0_27InitSupl(void)
{

    t_CGPS_initInd  *pl_initInd=NULL;

    /* Allocate memory for response */
    pl_initInd = (t_CGPS_initInd *)MC_RTK_GET_MEMORY(sizeof(*pl_initInd));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_INIT_IND,
                               (t_MsgHeader *)pl_initInd);
}
#endif /* AGPS_UP_FTR */


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 28
/**************************************************************************************************/
/* CGPS0_28NmeaTestCtrlReq : PGNV command request from an external module                         */
/**************************************************************************************************/
e_cgps_Error CGPS0_28NmeaTestCtrlReq(uint8_t* pp_Buffer, uint32_t vp_Length)
{
    t_CGPS_NmeaTestControlInd* pl_CGPS_NmeaTestControl=NULL;

    if(pp_Buffer == NULL)
    {
        return K_CGPS_ERR_INVALID_PARAMETER;
    }

    pl_CGPS_NmeaTestControl           = (t_CGPS_NmeaTestControlInd*)MC_RTK_GET_MEMORY(sizeof(*pl_CGPS_NmeaTestControl));
    pl_CGPS_NmeaTestControl->p_Data   = (uint8_t*)MC_RTK_GET_MEMORY(vp_Length);
    memcpy(pl_CGPS_NmeaTestControl->p_Data, pp_Buffer, vp_Length);
    pl_CGPS_NmeaTestControl->v_Length = vp_Length;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_CGPS,0,CGPS_NMEA_TEST_CONTROL_IND,(t_MsgHeader*)pl_CGPS_NmeaTestControl );

    return K_CGPS_NO_ERROR;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 29
/**************************************************************************************************/
/* CGPS0_29SearchFreeHandle : search a free handle for Native application                         */
/**************************************************************************************************/
bool CGPS0_29SearchFreeHandle(t_CgpsRegisterResult ** vp_Result)
{
    uint8_t vl_handle_index = 0;
    uint8_t vl_available_handle_found = FALSE;

    /* search for the first unused handle */
    while(    vl_handle_index < K_CGPS_MAX_NUMBER_OF_NAF
           && (!vl_available_handle_found)
         )
    {
        if( s_CgpsNaf[vl_handle_index].v_State == K_CGPS_NAF_UNUSED )
        {
            ((t_CgpsRegisterResult *)(vp_Result[0]))->v_Handle  = (uint32_t)vl_handle_index;
            s_CgpsNaf[vl_handle_index].v_State                  = K_CGPS_NAF_REGISTERED;

            if(CGPS4_18CheckNewSessionRight(CGPS_AUTONOMOUS_APPLICATION_ID))
            {
                CGPS4_19UpdateOtherApplicationState(CGPS_AUTONOMOUS_APPLICATION_ID);
                s_CgpsNaf[vl_handle_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
            }
            else
            {
                s_CgpsNaf[vl_handle_index].v_Suspended= CGPS_MIN_PRIORITY_VALUE;
            }
            vl_available_handle_found = TRUE;
        }
        vl_handle_index++;
    }

    if( !vl_available_handle_found )
    {
        ((t_CgpsRegisterResult *)(vp_Result[0]))->v_Result = K_CGPS_ERR_NO_AVAILABLE_HANDLE;
    }

    return vl_available_handle_found;
}



#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 30
/**************************************************************************************************/
/* CGPS0_30StartLowLayerGPS :  start low layer of GPS (lib and  HALGPS)                           */
/**************************************************************************************************/
e_cgps_Error CGPS0_30StartLowLayerGPS()
{
    e_cgps_Error vl_retVal=K_CGPS_NO_ERROR;
    e_halgps_state vl_Return;

    MC_CGPS_TRACE(("CGPS0_30StartLowLayerGPS"));

    //LBSCFG_LogGetConfiguration(K_LBSCFG_CGPS, &vg_LBSCFG_LogParam.v_Cgps);

#ifdef GPS_OVER_HCI
    if( CGPS0_66HciInit() != K_CGPS_NO_ERROR  )
    {
        MC_CGPS_TRACE(("CGPS0_30StartLowLayerGPS CGPS0_66HciInit failed"));
        vl_retVal = K_CGPS_ERR_GPS_NOT_STARTED;
    }
    else
    {
#endif

        vl_Return = MC_HALGPS_POWERUP();

        if( (vl_Return != HALGPS_OK) && (vl_Return != HALGPS_ALREADY_RUN))
        {
            vl_retVal = K_CGPS_ERR_GPS_NOT_STARTED;
        }
        else
        {
            MC_CGPS_TRACE(("CGPS0_30StartLowLayerGPS GPS Module is already ON"));

        }
#ifdef GPS_OVER_HCI
    }
#endif

    return  vl_retVal;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 31
/**************************************************************************************************/
/* CGPS0_31StopCGPS : stop GPS                                                                    */
/**************************************************************************************************/
void CGPS0_31StopCGPS()
{
    MC_CGPS_TRACE(("CGPS0_31StopCGPS CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));

    MC_HALGPS_POWERDOWN();
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 32
/**************************************************************************************************/
/* CGPS0_32StartCGPS : start GPS if it's not yet done                                             */
/**************************************************************************************************/
uint8_t CGPS0_32StartCGPS()
{
    uint8_t vl_return = FALSE;

    MC_CGPS_TRACE(("CGPS0_32StartCGPS Entry CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));


    switch (vg_CGPS_GpsState)
    {
        case K_CGPS_GPS_OFF:
        {
            /* For WAKE LOCK ER - 320658 */

            MC_HALGPS_SLEEP( FALSE );

            if (CGPS0_30StartLowLayerGPS()==K_CGPS_NO_ERROR)
            {
                vg_CGPS_GpsState = K_CGPS_GPS_RUNNING;
                vl_return = TRUE;
            }

            CGPS4_38UpdateConfiguration();

            CGPS4_43ExtLNAConfig(TRUE);


#ifdef AGPS_TIME_SYNCH_FTR
             if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_AGPS_FTA))
             {
                GN_GPS_Set_EXT_FRM_SYNC_pin(TRUE);
             }
#endif //AGPS_TIME_SYNCH_FTR

/*+ LMSqc32518 */

#ifdef GPS_FREQ_AID_FTR
            if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_FREQ_AIDING))
            {
               CGPS8_01ActivateGsa();
            }
            else
#endif /*GPS_FREQ_AID_FTR*/
            {
                CGPS4_30RestoreTCXOCalibration();
            }


// +13_NOV_2010 this function is called whenever GPS is initialized. This loads the saved historical data into the memory
#ifdef AGPS_HISTORICAL_DATA_FTR
            CGPS10_03ReadingHistoricalCellIdTable();

#endif /*AGPS_HISTORICAL_DATA_FTR*/
// -13_NOV_2010 this function is called whenever GPS is initialized. This loads the saved historical data into the memory
/*- LMSqc32518 */
        }
        break;

        case K_CGPS_GPS_SLEEP:
        {
            /* stop timer to OFF */
            MC_RTK_STOP_TIMER(CGPS_SLEEP_TIMEOUT);

/* ++ LMSqc34507 */
            MC_HALGPS_SLEEP(FALSE);
/* -- LMSqc34507 */
            MC_HALGPS_CLEAR_RX_BUF();
            /* In Sleep or Coma so UART activity will wake up the baseband.*/
            GN_GPS_WakeUp();

            CGPS4_38UpdateConfiguration();

            CGPS4_43ExtLNAConfig(TRUE);

#ifdef AGPS_TIME_SYNCH_FTR
             if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_AGPS_FTA))
             {
                GN_GPS_Set_EXT_FRM_SYNC_pin(TRUE);
             }
#endif //AGPS_TIME_SYNCH_FTR

#ifdef GPS_FREQ_AID_FTR
            if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_FREQ_AIDING))
            {
                CGPS8_01ActivateGsa();
            }
            else
#endif /*GPS_FREQ_AID_FTR*/
            {
                CGPS4_30RestoreTCXOCalibration();
            }

#ifdef AGPS_HISTORICAL_DATA_FTR
            CGPS10_03ReadingHistoricalCellIdTable();
#endif /*AGPS_HISTORICAL_DATA_FTR*/
            vg_CGPS_GpsState = K_CGPS_GPS_RUNNING;
            vl_return        = TRUE;
        }
        break;

        case K_CGPS_GPS_RUNNING:
            vl_return = TRUE;
        break;
    }

    MC_CGPS_TRACE(("CGPS0_32StartCGPS Exit CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));
/*+SYScs46505*/
    CGPS4_33InitializeAssistTracking(&vg_AssistTrack);
/*-SYScs46505*/

    return vl_return;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 33
/**************************************************************************************************/
/* CGPS0_33StartLoop : Start timer for CGPS loop  and put CGPS in running state                   */
/**************************************************************************************************/
void CGPS0_33StartLoop()
{
    char pl_LbsLibVer[GN_GPS_SIZE_VER];

    MC_CGPS_TRACE(("CGPS0_33StartLoop CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));
/* +LMSqc41350 */
    MC_CGPS_TRACE(("LBS Software Version = %s",vg_Lbs_Host_Version));

    GN_GPS_Get_LibVersion( pl_LbsLibVer );
    pl_LbsLibVer[GN_GPS_SIZE_VER-1] = '\0';

    MC_CGPS_TRACE(("GPS library version = %s", pl_LbsLibVer));

/* -LMSqc41350 */
    switch( vg_CGPS_State )
    {
        case K_CGPS_ACTIVE_GPS_ON:
            MC_RTK_PROCESS_START_TIMER(CGPS_FIX_LOOP_TIMEOUT, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_FIX_LOOP_TIMER_DURATION));
        break;

        case K_CGPS_ACTIVE_GPS_OFF:
            MC_RTK_PROCESS_START_TIMER(CGPS_FIX_LOOP_TIMEOUT, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_FIX_LOOP_LOW_FREQ_TIMER_DURATION ));
        break;

        case K_CGPS_INACTIVE:
        default:
            MC_CGPS_TRACE(("Error : CGPS in Inactive state, No timer start required" ));
        break;
    }
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 34
/**************************************************************************************************/
/* CGPS0_34EndLoop : Stop main loop of cgps and go in sleep mode                                */
/**************************************************************************************************/
void CGPS0_34EndLoop()
{
    bool vl_CgpsStraightToOff = FALSE;
    uint32_t  vl_sleepTimeout      = 0;
#ifdef CMCC_LOGGING_ENABLE
    uint8_t vl_index = 0;
#endif

    MC_CGPS_TRACE(("CGPS0_34EndLoop Entry. CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));

    CGPS4_63PrintGPSEphmeris();
    CGPS4_64PrintGlonassEphmeris();

    CGPS4_43ExtLNAConfig(FALSE);

    MC_RTK_STOP_TIMER(CGPS_FIX_LOOP_TIMEOUT);


    if ( (!vg_cgps_platform_config.v_SleepTimerDuration)
        &&(!vg_cgps_platform_config.v_ComaTimerDuration)
        && (!vg_cgps_platform_config.v_DeepComaDuration) )
    {

       vl_CgpsStraightToOff = TRUE;
       MC_CGPS_TRACE(("vl_CgpsStraightToOff : %d ",vl_CgpsStraightToOff));

    }

#ifdef GPS_FREQ_AID_FTR
    if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_FREQ_AIDING))
    {
       CGPS8_02DeActivateGsa();
    }
    else
#endif
    {
        CGPS4_29SaveTCXOCalibration();
    }

#ifdef AGPS_TIME_SYNCH_FTR
    if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_AGPS_FTA))
    {
       CGPS7_44FtaInitialize();
       GN_GPS_Set_EXT_FRM_SYNC_pin(FALSE);
    }
#endif /*AGPS_TIME_SYNCH_FTR*/

    if(    vg_CGPS_ColdStartMode == K_CGPS_COLD_START_ALWAYS
        || vg_CGPS_ColdStartMode == K_CGPS_WARM_START_ALWAYS
        || vg_CGPS_ColdStartMode == K_CGPS_HOT_START_ALWAYS )
    {
        vl_sleepTimeout = 0;
    }
    else
    {
        vl_sleepTimeout = vg_cgps_platform_config.v_SleepTimerDuration;
    }

    GN_GPS_Sleep( vl_sleepTimeout, vg_cgps_platform_config.v_ComaTimerDuration);

    MC_HALGPS_SLEEP(TRUE);
    vg_CGPS_GpsState = K_CGPS_GPS_SLEEP;

    MC_CGPS_TRACE(("CGPS0_34EndLoop : Sleep timeout: %u / Coma timeout: %u /Deep Comma: %u",  vl_sleepTimeout,  vg_cgps_platform_config.v_ComaTimerDuration, vg_cgps_platform_config.v_DeepComaDuration));

    if(    ( (vl_sleepTimeout + vg_cgps_platform_config.v_ComaTimerDuration + vg_cgps_platform_config.v_DeepComaDuration) > K_CGPS_TIMER_SAFETY_MARGIN )
        && vl_CgpsStraightToOff == FALSE
      )
    {
        /* if sleep duration and coma duration are equal to 0, go straight to off */
             MC_RTK_PROCESS_START_TIMER(CGPS_SLEEP_TIMEOUT,
                                   MC_DIN_MILLISECONDS_TO_TICK((vl_sleepTimeout + vg_cgps_platform_config.v_ComaTimerDuration + vg_cgps_platform_config.v_DeepComaDuration-K_CGPS_TIMER_SAFETY_MARGIN)*1000));
    }

    else if ((vl_CgpsStraightToOff == TRUE)
             || (vg_cgps_platform_config.v_ComaTimerDuration + vg_cgps_platform_config.v_DeepComaDuration <= K_CGPS_TIMER_SAFETY_MARGIN))
    {
        CGPS0_14SleepExpired(NULL);
    }

    /*+ LMSqc48667 */
    CGPS0_65ResetGpsContext();
    CGPS4_62HandleStartMode();
    /*- LMSqc48667 */

#ifdef AGPS_HISTORICAL_DATA_FTR
           // write entire historical data from memory to the SD card when GPS is turned off
          CGPS10_04WriteHistoricalCellIdTable();
#endif /*AGPS_HISTORICAL_DATA_FTR*/
    CGPS4_55SendEventNotify(K_CGPS_ERR_GPS_ENGINE_OFF);
    MC_CGPS_TRACE(("CGPS0_34EndLoop Exit. CGPS_State=%s, Gps_State=%s", a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));


#ifdef CMCC_LOGGING_ENABLE
    CGPS9_23LogPosition ( vl_index, CGPS_SUPL_CMCC_AGPS_SESSION_ENDED, NULL);
#endif

}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 35
/**************************************************************************************************/
/* CGPS0_35TestRegisterForAll : Test used only register callback for receive all information when */
/*                              an application is registered                                      */
/**************************************************************************************************/
e_cgps_Error CGPS0_35TestRegisterForAll(t_cgps_NavigationCallback vp_Callback, uint8_t vp_OutputType,uint16_t vp_NmeaMask)
{
    e_cgps_Error vl_Return;

    /* check paramters */
    if(   ( vp_Callback != NULL)
       && (    vp_OutputType == K_CGPS_NMEA
            || vp_OutputType == K_CGPS_C_STRUCT
            /* ++ LMSqc26033 */
            || vp_OutputType == K_CGPS_NMEA_AND_C_STRUCT )
            /* -- LMSqc26033 */
      )
    {
        vg_CgpsTestall.v_Mode               = K_CGPS_PERIODIC;
        vg_CgpsTestall.p_Callback           = vp_Callback;
        vg_CgpsTestall.v_OutputType         = vp_OutputType;
        vg_CgpsTestall.v_Mask               = vp_NmeaMask;
        vg_CgpsTestall.v_AgeLimit           = 0;/*Not used*/
        vg_CgpsTestall.v_HorizAccuracy      = 0;/*Not used*/
        vg_CgpsTestall.v_VertAccuracy       = 0;/*Not used*/
        vg_CgpsTestall.v_DeadlineOSTimeMS   = 0;/*Not used*/
        vg_CgpsTestall.v_State              = K_CGPS_NAF_READY;
        vg_CgpsTestall.v_LastApplicationUpdateTime = 0;/*Not used*/

        vl_Return = K_CGPS_NO_ERROR;
    }
    else
    {
        vg_CgpsTestall.v_State              = K_CGPS_NAF_UNUSED;
        vg_CgpsTestall.p_Callback           = NULL;
        vg_CgpsTestall.v_Mode               = K_CGPS_SINGLE_SHOT;
        vg_CgpsTestall.v_OutputType         = 0xFF;
        vg_CgpsTestall.v_Mask               = 0xFFFF;
        vg_CgpsTestall.v_FixRate            = 0;
        vg_CgpsTestall.v_AgeLimit           = 0;
        vg_CgpsTestall.v_HorizAccuracy      = 0;
        vg_CgpsTestall.v_VertAccuracy       = 0;
        vg_CgpsTestall.v_DeadlineOSTimeMS   = 0;
        vg_CgpsTestall.v_Suspended          = CGPS_MAX_PRIORITY_VALUE;
        vg_CgpsTestall.v_LastApplicationUpdateTime = 0;

        vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
    }

    return vl_Return;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 36
/**************************************************************************************************/
/* CGPS0_36GetNafData : Get data for a naf application                                            */
/**************************************************************************************************/
void CGPS0_36GetNafData(s_GN_GPS_Nav_Data *vp_navData, t_cgps_NavData * pp_NavDataToSend, e_cgps_NavDataType vp_OutputType, uint16_t vp_Mask)
{
    /* Compute data to send */
    if( vp_OutputType == K_CGPS_NMEA )        /* NMEA*/
    {
        uint16_t vl_NMEA_Bytes=0;
        /* ++ LMSqb65223 */
        pp_NavDataToSend->p_NavData = MC_RTK_GET_MEMORY( K_CGPS_NMEA_BUFFER_SIZE );

        vl_NMEA_Bytes = CGPS0_17GetNMEA( vp_navData,
                                         vp_Mask,
                                         K_CGPS_NMEA_BUFFER_SIZE,
                                         pp_NavDataToSend->p_NavData );
        /* -- LMSqb65223 */

        pp_NavDataToSend->v_Type = K_CGPS_NMEA;
        pp_NavDataToSend->v_Length = vl_NMEA_Bytes;
    }
    else if( vp_OutputType == K_CGPS_C_STRUCT )   /*'C' structs*/
    {
        t_cgps_CStructNavData  *pl_cgps_CStructNavData=NULL;
        pp_NavDataToSend->p_NavData = MC_RTK_GET_MEMORY( sizeof( *pl_cgps_CStructNavData ) );
        pp_NavDataToSend->v_Type    = K_CGPS_C_STRUCT;
        pp_NavDataToSend->v_Length  = sizeof( *pl_cgps_CStructNavData );

        CGPS0_44CopyNavData( (t_cgps_CStructNavData *)pp_NavDataToSend->p_NavData , vp_navData );
    }
}

#ifdef __RTK_E__
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 37
/**************************************************************************************************/
/* CGPS0_37DirectoryReady : Directory used by the library is ready to use                         */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_37DirectoryReady(t_RtkObject* p_FsmObject)
{
    t_mea_MapiGetApplicationPathReq * pl_MapiGetApplicationPathReq=NULL;

    pl_MapiGetApplicationPathReq  =  (t_mea_MapiGetApplicationPathReq *) MC_RTK_GET_MEMORY( sizeof(*pl_MapiGetApplicationPathReq) );

    const t_OperationDescriptor* pl_NextState = SAME;

    /* ++ LMSqb92010 */
    t_mea_MapiAvailableDirInd* pl_MapiAvailableDirInd = (t_mea_MapiAvailableDirInd*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if(    pl_MapiAvailableDirInd != NULL
        && pl_MapiAvailableDirInd->s_ApplicationIdList.p_ApplicationIdList != NULL
      )
    {
        MC_RTK_FREE_MEMORY(pl_MapiAvailableDirInd->s_ApplicationIdList.p_ApplicationIdList);
    }
    /* +- LMSqb92010 */

    pl_MapiGetApplicationPathReq->v_ApplicationId = MEA_AGPS_APPLICATION_ID /* MEA_GPS_LIBRARY_ID */;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_MEA,  0,
                               MAPI_MEA_GET_APPLICATION_PATH_REQ,
                               (t_MsgHeader *)pl_MapiGetApplicationPathReq);

    return pl_NextState;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 38
/**************************************************************************************************/
/* CGPS0_38GetDirectoryPathCnf : path of the directory used by library                            */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_38GetDirectoryPathCnf(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;

    t_mea_MapiGetApplicationPathCnf *pl_MsgGetApplicationPathCnf = \
                         (t_mea_MapiGetApplicationPathCnf*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    /* ++ LMSqb92010 */
    if(    pl_MsgGetApplicationPathCnf            != NULL
        && pl_MsgGetApplicationPathCnf->p_DirPath != NULL )
    {
    /* +- LMSqb92010 */
        MC_HALGPS_SET_SYSTEM_PATH(pl_MsgGetApplicationPathCnf->p_DirPath);

        /* ++ LMSqb92010 */
        MC_RTK_FREE_MEMORY(pl_MsgGetApplicationPathCnf->p_DirPath);
        /* +- LMSqb92010 */
    }

    return pl_NextState;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 39
/**************************************************************************************************/
/* CGPS0_39RemoveDirInd :                                                                         */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_39RemoveDirInd(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;

    t_mea_MapiRemoveDirRsp  *pl_MapiRemoveDirRsp=NULL;

    /* ++ LMSqb92010 */
    t_mea_MapiRemoveDirInd* pl_MapiRemoveDirInd = (t_mea_MapiRemoveDirInd*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if(     pl_MapiRemoveDirInd != NULL
         && pl_MapiRemoveDirInd->s_ApplicationIdList.p_ApplicationIdList != NULL )
    {
        MC_RTK_FREE_MEMORY(pl_MapiRemoveDirInd->s_ApplicationIdList.p_ApplicationIdList);
    }
    /* +- LMSqb92010 */

    /* Allocate memory for response */
    pl_MapiRemoveDirRsp = (t_mea_MapiRemoveDirRsp *)MC_RTK_GET_MEMORY(sizeof(*pl_MapiRemoveDirRsp));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_MEA, 0,
                               MAPI_MEA_REMOVE_DIR_RSP,
                               (t_MsgHeader *)pl_MapiRemoveDirRsp);
    return pl_NextState;

}
#endif /*__RTK_E__*/


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 40
/**************************************************************************************************/
/* CGPS0_40FixRequestedInd :                                                                      */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_40FixRequestedInd(t_RtkObject* p_FsmObject)
{
    /*++ FIDO: 429488 A-GPS function is unuseful after start GPS standalone */
    uint8_t vl_needAssist = FALSE;
    uint16_t vl_Mask      = 0;

    /*-- FIDO: 429488 A-GPS function is unuseful after start GPS standalone */
    t_CGPS_FixReqInd *pl_FixReqInd = ( t_CGPS_FixReqInd* )MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    uint32_t vl_Handle             = (uint32_t)pl_FixReqInd->v_Handle;

#if defined (AGPS_FTR) || defined (AGPS_UP_FTR) || defined (AGPS_HISTORICAL_DATA_FTR)
    s_GN_AGPS_Assist_Req p_AR;
    vl_needAssist = CGPS4_61CheckIfAssistanceRequired(&vl_Mask, &p_AR);
#endif /* AGPS_FTR || AGPS_UP_FTR || AGPS_HISTORICAL_DATA_FTR */

    /*+ LMSqc04080 : v_Result is a bool pointer */
    /*pl_FixReqInd->v_Result   = CGPS0_52ConfigureCgpsNextState();*/
    *(pl_FixReqInd->v_Result)  = CGPS0_52ConfigureCgpsNextState();
    /*- LMSqc04080 */

    CGPS4_41CheckConfigToolVersionMismatch();

/* +LMSQC22162 */
    if( *(pl_FixReqInd->v_Result) == TRUE )
    {
        MC_CGPS_SEND_PSTE_NMEA_FIX_REQUEST();
    }
/* -LMSQC22162 */

    if ( ! MC_RTK_SEM_PRODUCE( CGPS_SEM2 ))
    {
        MC_CGPS_TRACE(("CGPS0_40FixRequestedInd : 0 != sem_post(CGPS_SEM2)"));
    }

    /*++ FIDO: 429488 A-GPS function is unuseful after start GPS standalone */

    MC_CGPS_TRACE(("CGPS0_40FixRequestedInd : vl_needAssist: %d",vl_needAssist));
#if defined(AGPS_UP_FTR) || defined(AGPS_FTR)
    if(((MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE) ||
         MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE)) && vl_needAssist))
    {
    /*-- FIDO: 429488 A-GPS function is unuseful after start GPS standalone */
        if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE))
        {
#ifdef AGPS_UP_FTR
            /*++ FIDO: 429488 A-GPS function is unuseful after start GPS standalone */
            /* Don't do supl session is SUPL session is already in progress */
            if(    (      (vg_CGPS_Supl_Count == 0) 
                       || (    MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_SUPL_MULTI_SESSION) 
                            && vg_CGPS_Supl_Count
                          )
                    ) 
            /*-- FIDO: 429488 A-GPS function is unuseful after start GPS standalone */
#ifdef AGPS_FTR
               &&  ((vg_CGPS_Cp_Count == 0) &&(vg_CGPS_Cp_Assitance_No_Request == 0))//Don't do SUPL session if emergency call is running.
#endif
               )
            {
                 CGPS5_13StartSUPL(&s_CgpsNaf[vl_Handle],NULL);
            }
#endif
        }
        else
        {
#ifdef AGPS_FTR
            if((vg_CGPS_Cp_Count == 0) && (vg_CGPS_Cp_Assitance_No_Request == 0)) //Don't do CP MO session is CP session is in progress already.
                CGPS7_32StartCPMO(&s_CgpsNaf[vl_Handle]);
#endif
        }
    /*++ FIDO: 429488 A-GPS function is unuseful after start GPS standalone */
    }
#endif
    /*-- FIDO: 429488 A-GPS function is unuseful after start GPS standalone */
    return SAME;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 41
/**************************************************************************************************/
/* CGPS0_41InitInd :                                                                              */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_41InitInd(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    p_FsmObject = p_FsmObject;

#ifdef AGPS_UP_FTR
    /* initialise SUPL library */
    GN_SUPL_Handler_Init();

    CGPS5_19InitSuplBearer();

    CGPS5_10InitSuplLibrary();
#endif /* AGPS_UP_FTR */

    return pl_NextState;
}


#ifdef AGPS_FTR
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 42
/**************************************************************************************************/
/* CGPS0_42ECall_Start :                                                                          */
/**************************************************************************************************/
void CGPS0_42ECall_Start(void)
{
    t_CGPS_EStartReq* p_MsgECallStartReq =NULL;
    p_MsgECallStartReq  = (t_CGPS_EStartReq *)MC_RTK_GET_MEMORY(sizeof(*p_MsgECallStartReq));


    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_EMERGENCY_START_REQ,
                               (t_MsgHeader *)p_MsgECallStartReq);
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 43
/**************************************************************************************************/
/* CGPS0_42ECall_Stop :                                                                           */
/**************************************************************************************************/
void CGPS0_43ECall_Stop(void)
{

    t_CGPS_EStopReq* p_MsgECallStopReq=NULL;
    p_MsgECallStopReq  = (t_CGPS_EStopReq *)MC_RTK_GET_MEMORY(sizeof(*p_MsgECallStopReq));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_EMERGENCY_STOP_REQ,
                               (t_MsgHeader *)p_MsgECallStopReq);
}
#endif /* AGPS_FTR */


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 44
/**************************************************************************************************/
/* CGPS0_44CopyNavData :                                                                          */
/**************************************************************************************************/
void CGPS0_44CopyNavData( t_cgps_CStructNavData *pp_Destn , s_GN_GPS_Nav_Data *pp_Src )
{
    uint8_t vl_SatIndex;
 
    pp_Destn->v_LocalTTag        = pp_Src->Local_TTag;
    pp_Destn->v_OSTimeMs         = pp_Src->OS_Time_ms;
    pp_Destn->v_Year             = pp_Src->Year;
    pp_Destn->v_Month            = pp_Src->Month;
    pp_Destn->v_Day              = pp_Src->Day;
    pp_Destn->v_Hours            = pp_Src->Hours;
    pp_Destn->v_Minutes          = pp_Src->Minutes;
    pp_Destn->v_Seconds          = pp_Src->Seconds;
    pp_Destn->v_Milliseconds     = pp_Src->Milliseconds;
    pp_Destn->v_GpsWeekNo        = pp_Src->Gps_WeekNo;
    pp_Destn->v_GpsTOW           = pp_Src->Gps_TOW;
    pp_Destn->v_UtcCorrection    = pp_Src->UTC_Correction;
    pp_Destn->v_X                = pp_Src->X;
    pp_Destn->v_Y                = pp_Src->Y;
    pp_Destn->v_Z                = pp_Src->Z;
    pp_Destn->v_Latitude         = pp_Src->Latitude;
    pp_Destn->v_Longitude        = pp_Src->Longitude;
    pp_Destn->v_AltitudeEll      = pp_Src->Altitude_Ell;
    pp_Destn->v_AltitudeMSL      = pp_Src->Altitude_MSL;
    pp_Destn->v_SpeedOverGround  = pp_Src->SpeedOverGround;
    pp_Destn->v_CourseOverGround = pp_Src->CourseOverGround;
    pp_Destn->v_VerticalVelocity = pp_Src->VerticalVelocity;
    pp_Destn->v_NAccEst          = pp_Src->N_AccEst;
    pp_Destn->v_EAccEst          = pp_Src->E_AccEst;
    pp_Destn->v_VAccEst          = pp_Src->V_AccEst;
    pp_Destn->v_HAccMaj          = pp_Src->H_AccMaj;
    pp_Destn->v_HAccMin          = pp_Src->H_AccMin;
    pp_Destn->v_HAccMajBrg       = pp_Src->H_AccMajBrg;
    pp_Destn->v_HVelAccEst       = pp_Src->HVel_AccEst;
    pp_Destn->v_VVelAccEst       = pp_Src->VVel_AccEst;
    pp_Destn->v_COG_AccEst       = pp_Src->COG_AccEst;
    pp_Destn->v_PRResRMS         = pp_Src->PR_ResRMS;
    pp_Destn->v_HDOP             = pp_Src->H_DOP;
    pp_Destn->v_VDOP             = pp_Src->V_DOP;
    pp_Destn->v_PDOP             = pp_Src->P_DOP;
    pp_Destn->v_FixType          = pp_Src->FixType;
    pp_Destn->v_Valid2DFix       = pp_Src->Valid_SingleFix;
    pp_Destn->v_Valid3DFix       = pp_Src->Valid_SingleFix;
    pp_Destn->v_FixMode          = pp_Src->FixMode;
    pp_Destn->v_SatsInView       = pp_Src->SatsInView;
    pp_Destn->v_SatsUsed         = pp_Src->SatsUsed;

    /*Glonass SV id are not dispalyed properly in UI*/
    for(vl_SatIndex=0; vl_SatIndex< K_CGPS_NMEA_SV; vl_SatIndex++)
    {
        pp_Destn->v_SatsInViewConstell[vl_SatIndex] = pp_Src->SatsInViewConstell[vl_SatIndex];
    }
 
    memcpy( pp_Destn->v_SatsInViewSVId,     pp_Src->SatsInViewSVid,     sizeof( pp_Src->SatsInViewSVid )     );
    memcpy( pp_Destn->v_SatsInViewSNR,      pp_Src->SatsInViewSNR,      sizeof( pp_Src->SatsInViewSNR )      );
    memcpy( pp_Destn->v_SatsInViewJNR,      pp_Src->SatsInViewJNR,      sizeof( pp_Src->SatsInViewJNR )      );
    memcpy( pp_Destn->v_SatsInViewAzim,     pp_Src->SatsInViewAzim,     sizeof( pp_Src->SatsInViewAzim )     );
    memcpy( pp_Destn->v_SatsInViewElev,     pp_Src->SatsInViewElev,     sizeof( pp_Src->SatsInViewElev )     );
    memcpy( pp_Destn->v_SatsInViewUsed,     pp_Src->SatsInViewUsed,     sizeof( pp_Src->SatsInViewUsed )     );
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 45
/**************************************************************************************************/
/* CGPS0_45UpdateUserConfiguration                                                                */
/**************************************************************************************************/
e_cgps_Error CGPS0_45UpdateUserConfiguration(t_cgps_Config * pp_Config)
{
    t_cgps_Config   *pl_cgps_Config=NULL;

    t_CGPS_UserConfig* pl_MsgUserConfig=NULL;
    pl_MsgUserConfig  = (t_CGPS_UserConfig *)MC_RTK_GET_MEMORY(sizeof(*pl_MsgUserConfig));

    memcpy((uint8_t*)(&pl_MsgUserConfig->v_Config),(uint8_t*)pp_Config,sizeof(*pl_cgps_Config));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_UPDATE_USER_CONFIG,
                               (t_MsgHeader *)pl_MsgUserConfig);

    return K_CGPS_NO_ERROR;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 46
/**************************************************************************************************/
/* CGPS0_46UserConfigurationReceived :                                                            */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_46UserConfigurationReceived(t_RtkObject* p_FsmObject)
{
    t_CGPS_UserConfig *pl_MsgUserConfig;
    s_GN_GPS_Config vl_GPS_Config;
    const t_OperationDescriptor* pl_NextState = SAME;

    //GN_GPS_Get_Config( &vl_GPS_Config ); SRO changed due to crash in library for stelp test

    pl_MsgUserConfig = ( t_CGPS_UserConfig* )MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set MS assisted positioning method to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_MS_POS_TECH_MSBASED) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_MS_POS_TECH_MSBASED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : set MS Based positioning method to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_MSBASED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : set Autonomous positioning method to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS)?TRUE:FALSE ));
    }
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_FREQ_AIDING) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_FREQ_AIDING);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : set Frequency Aiding Allowed to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_FREQ_AIDING)?TRUE:FALSE ));
    }

#ifdef AGPS_UP_FTR
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set User Plane allowed to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_MS_POS_TECH_ECID) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_MS_POS_TECH_ECID);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : set Cell ID positioning method to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_MS_POS_TECH_ECID)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_SUPL_MULTI_SESSION) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_SUPL_MULTI_SESSION);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : multi SI permission %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_SUPL_MULTI_SESSION)?TRUE:FALSE ));
    }
#endif

#ifdef AGPS_FTR
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set Control Plane allowed to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE)?TRUE:FALSE ));
    }
     if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_AGPS_FTA) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_AGPS_FTA);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : set Fine Time Assistance in CP allowed to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_FTA)?TRUE:FALSE ));
    }
#endif

#if defined(AGPS_UP_FTR) || defined(AGPS_FTR)
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set MO positioning sessions permission to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set MT positioning sessions permission to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED)?TRUE:FALSE ));
    }
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_NOTLS_SUPPORTED) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_NOTLS_SUPPORTED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set NO TLS Supported to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_NOTLS_SUPPORTED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_SUPLV2_SUPPORTED) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_SUPLV2_SUPPORTED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set SUPL v2.0 to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_SUPLV2_SUPPORTED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_SEMIAUTO ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_SEMIAUTO);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set Semi Auto positioning sessions permission to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_SEMIAUTO)?TRUE:FALSE ));
    }

#endif /* AGPS_UP_FTR || AGPS_FTR */

#if defined(AGPS_UP_FTR) && defined(AGPS_FTR)
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Set Transport Pref UPlane allowed to %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE)?TRUE:FALSE ));
    }
#endif /* AGPS_UP_FTR && AGPS_FTR */

#ifdef LBS_GLONASS

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_GPS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_GPS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : GPS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_GPS_ENABLED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_GLONASS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_GLONASS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : GLONASS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_GLONASS_ENABLED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_SBAS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_SBAS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : SBAS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_SBAS_ENABLED)?TRUE:FALSE ));
    }


#endif
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : EXTENDED EPHEMERIS GPS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GPS_ENABLED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : EXTENDED EPHEMERIS GLONASS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_EXTENDED_EPHEMERIS_GLONASS_ENABLED)?TRUE:FALSE ));
    }


    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_WIFI_POS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_WIFI_POS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Wi-Fi Positioning Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_WIFI_POS_ENABLED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_QZSS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_QZSS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : QZSS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_QZSS_ENABLED)?TRUE:FALSE ));
    }

    
    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : SNTP Reference Time Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_SNTP_REF_TIME_ENABLED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : SELF ASSISTED GPS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_SELF_ASSISTED_GPS_ENABLED)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgUserConfig->v_Config.v_ConfigMaskValidity, K_CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED ) )
    {
        MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(pl_MsgUserConfig->v_Config.v_ConfigMask, K_CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED);
        MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : SELF ASSISTED GLONASS Enabled/Disabled :  %i",
            (vg_CGPS_Configuration & K_CGPS_CONFIG_SELF_ASSISTED_GLONASS_ENABLED)?TRUE:FALSE ));
    }

#ifdef AGPS_UP_FTR
    if(pl_MsgUserConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_SUPL_SERVER_CONFIG)
    {
        if(    ( pl_MsgUserConfig->v_Config.v_ServerConfig.v_AddrType != K_CGPS_SLPADDR_DFLT )
            && ( pl_MsgUserConfig->v_Config.v_ServerConfig.v_AddrLen  != 0                   ))
        {
            vg_CGPS_Supl_Slp_Config.v_AddrType   = pl_MsgUserConfig->v_Config.v_ServerConfig.v_AddrType;
            vg_CGPS_Supl_Slp_Config.v_AddrLen = pl_MsgUserConfig->v_Config.v_ServerConfig.v_AddrLen;
            if( vg_CGPS_Supl_Slp_Config.v_Addr )
            {
                MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Slp_Config.v_Addr);
            }
            vg_CGPS_Supl_Slp_Config.v_Addr = (int8_t *)MC_RTK_GET_MEMORY(sizeof(char)* (vg_CGPS_Supl_Slp_Config.v_AddrLen +1));
            memcpy(vg_CGPS_Supl_Slp_Config.v_Addr,pl_MsgUserConfig->v_Config.v_ServerConfig.v_Addr,vg_CGPS_Supl_Slp_Config.v_AddrLen);
            vg_CGPS_Supl_Slp_Config.v_Addr[vg_CGPS_Supl_Slp_Config.v_AddrLen] = 0x0;
            /* +LMSqc22768 : 03-FEB-2010 */
            MC_RTK_FREE_MEMORY( pl_MsgUserConfig->v_Config.v_ServerConfig.v_Addr ) ; /* Release the memory allocated to SUPL server Address */
            /* +LMSqc22768 : 03-FEB-2010 */
            MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Change SUPL server type %i, address %s",vg_CGPS_Supl_Slp_Config.v_AddrType,vg_CGPS_Supl_Slp_Config.v_Addr));

            if( vg_CGPS_Supl_Eslp_Config.v_Addr )
            {
                MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Eslp_Config.v_Addr);
            }
            vg_CGPS_Supl_Eslp_Config.v_Addr = (int8_t *)MC_RTK_GET_MEMORY( vg_CGPS_Supl_Slp_Config.v_AddrLen + 1 );
            memcpy( vg_CGPS_Supl_Eslp_Config.v_Addr, vg_CGPS_Supl_Slp_Config.v_Addr,  vg_CGPS_Supl_Slp_Config.v_AddrLen + 1);
        
            vg_CGPS_Supl_Eslp_Config.v_AddrType = vg_CGPS_Supl_Slp_Config.v_AddrType;
            vg_CGPS_Supl_Eslp_Config.v_AddrLen = vg_CGPS_Supl_Slp_Config.v_AddrLen ;
        }

/*
BNS : Has to be verified if the following has any impact. The interface says user configuration has to be set to default value when slpAddrType is K_CGPS_SLPADDR_DFLT
        else if(pl_MsgUserConfig->v_Config.v_ServerConfig.v_AddrType == K_CGPS_SLPADDR_DFLT)
        {
            CGPS5_10InitSuplLibrary();
        }
*/

        /* ++ LMSqc00855    ANP 15/07/2009 */
        if(pl_MsgUserConfig->v_Config.v_ServerConfig.v_PortNum != 0 )
        {
            vg_CGPS_Supl_Slp_Config.v_PortNum = pl_MsgUserConfig->v_Config.v_ServerConfig.v_PortNum;
            MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : SUPL port number : %u" , vg_CGPS_Supl_Slp_Config.v_PortNum ));
        }
        else
        {
            vg_CGPS_Supl_Slp_Config.v_PortNum = CGPS_SUPL_DEFAULT_PORT;
        }

        vg_CGPS_Supl_Eslp_Config.v_PortNum    = vg_CGPS_Supl_Slp_Config.v_PortNum ;

        /* -- LMSqc00855    ANP 15/07/2009 */
    }
    if(pl_MsgUserConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_SUPL_ESLP_ADDR_CONFIG)
    {
        uint8_t v_Index;

        for(v_Index = 0; v_Index < CGPS_MAX_ESLP_ADDRESS;v_Index++)
        {
            uint8_t v_EslpAddrLength = strlen( (const char*)pl_MsgUserConfig->v_Config.v_EslpServerConfig.s_ESlpAddr[v_Index]);

            if( vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[v_Index])
            {
                MC_RTK_FREE_MEMORY(vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[v_Index]);
            }

            vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[v_Index] = (int8_t *)MC_RTK_GET_MEMORY(sizeof(char)* (v_EslpAddrLength +1));
            memcpy(vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[v_Index],pl_MsgUserConfig->v_Config.v_EslpServerConfig.s_ESlpAddr[v_Index],v_EslpAddrLength);
            *(vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[v_Index] + v_EslpAddrLength ) = 0x0;

            MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : ESLP Address%i Address %s",v_Index,vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[v_Index]));

        }

    }


    /* ++ LMSqb89864 - BNS 27 / 04 / 2009*/
    if( pl_MsgUserConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_SUPL_BEARER_CONFIG )
    {
        if( pl_MsgUserConfig->v_Config.s_BearerConfig.s_APN != NULL )
        /* If set to NULL default APN from the configuration file is set */
        {
            uint8_t vl_LengthApn = strlen( (const char*)pl_MsgUserConfig->v_Config.s_BearerConfig.s_APN );

            if(vg_CGPS_SUPL_Bearer.s_APN)
            {
                MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_APN);
            }
            vg_CGPS_SUPL_Bearer.s_APN = (int8_t*)MC_RTK_GET_MEMORY(vl_LengthApn + 1);
            strncpy( (char*)vg_CGPS_SUPL_Bearer.s_APN,
                     (const char*)pl_MsgUserConfig->v_Config.s_BearerConfig.s_APN,
                     vl_LengthApn +1 );
            MC_RTK_FREE_MEMORY( pl_MsgUserConfig->v_Config.s_BearerConfig.s_APN );

            if(    pl_MsgUserConfig->v_Config.s_BearerConfig.s_Login    != NULL
                && pl_MsgUserConfig->v_Config.s_BearerConfig.s_Password != NULL )
            {
                uint8_t vl_LengthLogin    = strlen( (const char*)pl_MsgUserConfig->v_Config.s_BearerConfig.s_Login );
                uint8_t vl_LengthPassword = strlen( (const char*)pl_MsgUserConfig->v_Config.s_BearerConfig.s_Password );

                if(vg_CGPS_SUPL_Bearer.s_Login)
                {
                    MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_Login);
                }
                if(vg_CGPS_SUPL_Bearer.s_Password)
                {
                    MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_Password);
                }

                vg_CGPS_SUPL_Bearer.s_Login = (int8_t*)MC_RTK_GET_MEMORY( vl_LengthLogin + 1 );
                strncpy( (char*)vg_CGPS_SUPL_Bearer.s_Login,
                         (const char*)pl_MsgUserConfig->v_Config.s_BearerConfig.s_Login,
                         vl_LengthLogin + 1 );
                MC_RTK_FREE_MEMORY( pl_MsgUserConfig->v_Config.s_BearerConfig.s_Login );

                vg_CGPS_SUPL_Bearer.s_Password = (int8_t*)MC_RTK_GET_MEMORY( vl_LengthPassword + 1 );

                strncpy( (char*) vg_CGPS_SUPL_Bearer.s_Password,
                         (const char*)pl_MsgUserConfig->v_Config.s_BearerConfig.s_Password,
                         vl_LengthPassword + 1 );

                MC_RTK_FREE_MEMORY( pl_MsgUserConfig->v_Config.s_BearerConfig.s_Password );
            }

            MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Change SUPL APN address changed"));
        }
        else
        {
            /* If set to NULL default APN from the configuration file is set */
            CGPS5_19InitSuplBearer();
            MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : Changed SUPL APN address to default values"));
        }
    }
    /* -- LMSqb89864 - BNS 27 / 04 / 2009*/
#endif

    if(pl_MsgUserConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_TTFF_SENSITIVITY)
    {
        switch (pl_MsgUserConfig->v_Config.v_SensTtff)
        {
            case K_CGPS_SENSITIVITY_TTFF_FASTER:
            {
                vl_GPS_Config.ColdTTFF = GN_GPS_COLD_TTFF_FASTER;
                break;
            }
            case K_CGPS_SENSITIVITY_TTFF_SENSITIVE:
            {
                vl_GPS_Config.ColdTTFF = GN_GPS_COLD_TTFF_SENSITIVE;
                break;
            }
            case K_CGPS_SENSITIVITY_TTFF_BALANCE:
            default:
            {
                vl_GPS_Config.ColdTTFF = GN_GPS_COLD_TTFF_BALANCE;
                break;
            }
        }
        vg_cgps_user_config.v_SensTtff = vl_GPS_Config.ColdTTFF;
    }

    if(pl_MsgUserConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_POWER_PREFERENCE)
    {
        switch (pl_MsgUserConfig->v_Config.v_PowerPref)
        {
            case K_CGPS_POWER_PERF_HI_PERF:
            {
                vl_GPS_Config.PowerPerf = GN_GPS_POW_PERF_HI_PERF;
                break;
            }
            case K_CGPS_POWER_PERF_LO_POW:
            {
                vl_GPS_Config.PowerPerf = GN_GPS_POW_PERF_LO_POW;
                break;
            }
            case K_CGPS_POWER_PERF_DEFAULT:
            default:
            {
                vl_GPS_Config.PowerPerf = GN_GPS_POW_PERF_DEFAULT;
                break;
            }
        }
        vg_cgps_user_config.v_PowerPref = vl_GPS_Config.PowerPerf;
    }

    if(pl_MsgUserConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_SENSITIVITY_MODE)
    {
        switch (pl_MsgUserConfig->v_Config.v_SensMode)
        {
            case K_CGPS_SENSITIVITY_MODE_HIGH:
            {
                vl_GPS_Config.SensMode = GN_GPS_SENS_MODE_HIGH;
                break;
            }
            case K_CGPS_SENSITIVITY_MODE_NORMAL:
            {
                vl_GPS_Config.SensMode = GN_GPS_SENS_MODE_NORMAL;
                break;
            }
            case K_CGPS_SENSITIVITY_MODE_DYNAMIC:
            default:
            {
                vl_GPS_Config.SensMode = GN_GPS_SENS_MODE_DYNAMIC;
                break;
            }
        }
        vg_cgps_user_config.v_SensMode = vl_GPS_Config.SensMode;
    }

    if (vg_CGPS_GpsState == K_CGPS_GPS_RUNNING)
            CGPS4_38UpdateConfiguration();

    if(pl_MsgUserConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_COLD_START_MODE)
    {
        MC_CGPS_TRACE(("Stored User Configuration for Start Mode = %d", vg_CGPS_ColdStartMode));
        vg_CGPS_ColdStartMode = pl_MsgUserConfig->v_Config.v_ColdStart;
        switch( vg_CGPS_ColdStartMode )
        {
            case K_CGPS_COLD_START_FIRST_ONLY:
                vg_CGPS_ColdStartMode = K_CGPS_COLD_START_DEFAULT;
                MC_CGPS_DELETE_NV_STORE_FIELD(K_CGPS_FIELD_COLD_START);
            break;
            case K_CGPS_COLD_START_ALWAYS:
                MC_CGPS_DELETE_NV_STORE_FIELD(K_CGPS_FIELD_COLD_START);
            break;

            case K_CGPS_WARM_START_ALWAYS:
                MC_CGPS_DELETE_NV_STORE_FIELD(K_CGPS_FIELD_WARM_START);
            break;

            case K_CGPS_HOT_START_ALWAYS:
                MC_CGPS_DELETE_NV_STORE_FIELD(K_CGPS_FIELD_TIME);
            break;

            case K_CGPS_COLD_START_DEFAULT:
            default:
                /*Nothing to do*/
            break;
        }
    }

    CGPS0_53UpdateSessionConfig();

    //GN_GPS_Set_Config( &vl_GPS_Config ); SRO changed due to crash in library for stelp test

#if defined(AGPS_UP_FTR)
    CGPS5_21SuplConfig();
#endif /* AGPS_UP_FTR */

    return pl_NextState;
}






/* + LMSqb92911 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 48
/**************************************************************************************************/
/* CGPS0_48SaveNVStore :                                                                          */
/**************************************************************************************************/
void CGPS0_48SaveNVStore()
{
    t_CGPS_SaveNvStoreReq   *pl_CGPS_SaveNvStoreReq=NULL;


    pl_CGPS_SaveNvStoreReq = (t_CGPS_SaveNvStoreReq*)MC_RTK_GET_MEMORY(sizeof(*pl_CGPS_SaveNvStoreReq));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_SAVE_NV_STORE_REQ,
                               (t_MsgHeader *)pl_CGPS_SaveNvStoreReq);
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 49
/**************************************************************************************************/
/* CGPS0_49SaveNVStoreRequested :                                                                 */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_49SaveNVStoreRequested(t_RtkObject* p_FsmObject)
{
    MC_CGPS_TRACE(("CGPS0_49SaveNVStoreRequested : Entry"));
    p_FsmObject = p_FsmObject;

    if( vg_CGPS_GpsState == K_CGPS_GPS_RUNNING )
    {

        uint16_t NV_Size     = 0;              /* Size of the GN_GPS_Lib NV Store*/
        uint8_t *p_NV_Store = NULL;           /* Pointer to the internal GN_GPS_Lib NV Store*/

        /* Get a pointer to and the size of the libraries internal NV Store RAM copy.*/
        NV_Size = GN_GPS_Get_NV_Store( &p_NV_Store );

        if(    ( p_NV_Store != NULL )
            && ( NV_Size     > 0    ) )
        {
            /* Save the cleared NV store back to the host file system.*/
            GN_GPS_Write_NV_Store( NV_Size, p_NV_Store );
        }
        else
        {
            MC_CGPS_TRACE(("No NV Data Present!"));
        }
    }

    MC_CGPS_TRACE(("CGPS0_49SaveNVStoreRequested : Exit"));

    return SAME;
}
/* - LMSqb92911 */


/* ++ LMSqc00847    ANP 14/07/2009 */
#ifdef AGPS_UP_FTR
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 50
/**************************************************************************************************/
/* CGPS0_50UpdateCertificate : Update Certificate                                                 */
/**************************************************************************************************/
e_cgps_Error CGPS0_50UpdateCertificate(uint8_t CertId, uint32_t v_CertLen, uint8_t * p_CertName, uint8_t* p_Cert)
{
    e_cgps_Error v_Error = K_CGPS_ERR_INVALID_PARAMETER;

    if( ( NULL != p_Cert ) && ( 0 != v_CertLen ) )
    {
        t_lsimup_CertificateIndM  *pl_lsimup_CertificateInd=NULL;


        pl_lsimup_CertificateInd = (t_lsimup_CertificateIndM *)MC_RTK_GET_MEMORY( sizeof(*pl_lsimup_CertificateInd) );

        pl_lsimup_CertificateInd->v_CertificateId  = CertId;
        pl_lsimup_CertificateInd->p_Certificate    = p_Cert;
        pl_lsimup_CertificateInd->v_CertificateLen = v_CertLen;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                   PROCESS_LSIMUP, 0,
                                   CGPS_LSIMUP_CERTIFICATE_IND,
                                   (t_MsgHeader *)pl_lsimup_CertificateInd);

        v_Error = K_CGPS_NO_ERROR;
    }

    if( NULL != p_CertName )
    {
        MC_RTK_FREE_MEMORY(p_CertName);
    }

    return v_Error;
}
/* -- LMSqc00847    ANP 14/07/2009 */


/* ++ LMSqb95626    ANP 03/07/2009 */
/* ++ LMSqb93693 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 51
/**************************************************************************************************/
/* CGPS0_51DataCntNotificationRegisterReq : register data connection notification callback        */
/*                                          for the user                                          */
/**************************************************************************************************/
void CGPS0_51DataCntNotificationRegisterReq(t_cgps_DataCntNotificationCallback vp_Callback)
{
    MC_CGPS_TRACE(("CGPS0_51DataCntNotificationRegisterReq"));
    vg_DataCntNotificationCallback = vp_Callback;
}
#endif /* AGPS_UP_FTR */
/* -- LMSqb93693 */
/* -- LMSqb95626    ANP 03/07/2009 */


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 52
/**************************************************************************************************/
/* CGPS0_52ConfigureCgpsNextState :                                                               */
/**************************************************************************************************/
bool CGPS0_52ConfigureCgpsNextState()
{
    uint32_t  vl_NextState           = vg_CGPS_State;
    uint8_t   vl_NafGpsRequiredCount = 0;
    uint8_t   vl_NafNbpCount         = 0;
    bool vl_PatchUploadPending  = FALSE;
    bool vl_Return              = TRUE;

    bool vl_AssistedGpsSessionRunning = 0
#ifdef AGPS_UP_FTR
                || vg_CGPS_Supl_Count
#endif
#ifdef AGPS_FTR
                || vg_CGPS_Cp_Count || vg_CGPS_Cp_Assitance_No_Request
#endif
#if defined AGPS_FTR || defined AGPS_UP_FTR
                || vg_CGPS_Notification_Count
#endif
;


/*+ LMSqc32518 */
#ifndef CGPS_UPLOAD_PATCH_AT_INIT_FTR
/*- LMSqc32518 */
    if( gn_Patch_Status > 0  &&  gn_Patch_Status < 7 )
/*+ LMSqc32518 */
#else
    if( gn_Patch_Status >= 0  &&  gn_Patch_Status < 7 )
#endif /*!CGPS_UPLOAD_PATCH_AT_INIT_FTR*/
/*- LMSqc32518 */
    {
        vl_PatchUploadPending = TRUE;
    }

    /* Compute the following only if this condition is true */
    if(     (    FALSE == vl_PatchUploadPending
              && 0     == vl_AssistedGpsSessionRunning
            )
        && vg_CGPS_Naf_Count
      )
    {
        uint8_t vl_Index  = 0;

        for ( vl_Index = 0 ; vl_Index < K_CGPS_MAX_NUMBER_OF_NAF; vl_Index++ )
        {
            if( K_CGPS_NAF_READY == s_CgpsNaf[vl_Index].v_State )
            {
                /* Check if any NAF session requires GPS to be ON */
                if(   MC_CGPS_IS_FLAG_SET_IN_PARAM1 (s_CgpsNaf[vl_Index].v_Config, K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS  )
                   || MC_CGPS_IS_FLAG_SET_IN_PARAM1 (s_CgpsNaf[vl_Index].v_Config, K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS )
                  )
                {
                    vl_NafGpsRequiredCount++;
                }
                else if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 (s_CgpsNaf[vl_Index].v_Config, K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED ) )
                {
                    vl_NafNbpCount++;
                }
            }
        }
    }

    if(    vl_NafGpsRequiredCount
        || vl_AssistedGpsSessionRunning
        || vl_PatchUploadPending
      )
    {
        vl_NextState = K_CGPS_ACTIVE_GPS_ON;
    }
    else if( vl_NafNbpCount )
    {
        vl_NextState = K_CGPS_ACTIVE_GPS_OFF;
    }
    else
    {
        vl_NextState = K_CGPS_INACTIVE;
    }

    if( vg_CGPS_State != vl_NextState )
    {
        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState State Change occurence. Current CGPS_State=%s, Next CGPS_State=%s, Current Gps_State=%s",
                         a_CGPS_StateAsEnum[vg_CGPS_State], a_CGPS_StateAsEnum[vl_NextState], a_GPS_StateAsEnum[vg_CGPS_GpsState]));

        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState Autonmous Count=%u", vg_CGPS_Naf_Count));
#if defined (AGPS_UP_FTR) || defined (AGPS_FTR)
        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState Notif     Count=%u", vg_CGPS_Notification_Count));
#endif /* AGPS_UP_FTR || AGPS_FTR */
#if defined (AGPS_UP_FTR)
        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState SUPL      Count=%u", vg_CGPS_Supl_Count));
#endif /*AGPS_UP_FTR */
#if defined (AGPS_FTR)
        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState CP        Count=%u", vg_CGPS_Cp_Count));
        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState CP MOLR   Count=%u", vg_CGPS_Cp_Mo_Count));
        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState CP Assistance delivered without request=%s", (vg_CGPS_Cp_Assitance_No_Request)?"TRUE":"FALSE"));
#endif /* AGPS_FTR */
        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState Patch Upload Status=%u", gn_Patch_Status));


        switch( vg_CGPS_State )
        {
            case K_CGPS_INACTIVE:
            {
                switch( vl_NextState )
                {
                    case K_CGPS_ACTIVE_GPS_OFF:
                    {
                        vg_CGPS_State = vl_NextState;
                        CGPS0_33StartLoop();
                    }
                    break;
                    case K_CGPS_ACTIVE_GPS_ON:
                    {
                        vg_CGPS_State = vl_NextState;
                        vl_Return = CGPS0_32StartCGPS();
                        CGPS0_33StartLoop();
                    }
                    break;
                }
            }
            break;

            case K_CGPS_ACTIVE_GPS_OFF:
            {
                switch( vl_NextState )
                {
                    case K_CGPS_INACTIVE:
                    {
                        vg_CGPS_State = vl_NextState;
                        CGPS4_34UserConfigurationFileWrite();
                        MC_RTK_STOP_TIMER(CGPS_FIX_LOOP_TIMEOUT);
                    }
                    break;
                    case K_CGPS_ACTIVE_GPS_ON:
                    {
                        vg_CGPS_State = vl_NextState;
                        MC_RTK_STOP_TIMER(CGPS_FIX_LOOP_TIMEOUT);
                        vl_Return = CGPS0_32StartCGPS();
                        CGPS0_33StartLoop();
                    }
                    break;
                }
            }
            break;

            case K_CGPS_ACTIVE_GPS_ON:
            {
                switch( vl_NextState )
                {
                    case K_CGPS_INACTIVE:
                    {
                        vg_CGPS_State = vl_NextState;
                        CGPS4_34UserConfigurationFileWrite();
                        CGPS0_34EndLoop();
                    }
                    break;
                    case K_CGPS_ACTIVE_GPS_OFF:
                    {
                        vg_CGPS_State = vl_NextState;
                        CGPS0_34EndLoop();
                        CGPS0_33StartLoop();
                    }
                    break;
                }
            }
            break;
        }

        MC_CGPS_TRACE(("CGPS0_52ConfigureCgpsNextState New states : CGPS_State=%s, Gps_State=%s",
                            a_CGPS_StateAsEnum[vg_CGPS_State], a_GPS_StateAsEnum[vg_CGPS_GpsState]));
    }

    return vl_Return;
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 53
/**************************************************************************************************/
/* CGPS0_53UpdateSessionConfig :                                                                  */
/**************************************************************************************************/
void CGPS0_53UpdateSessionConfig()
{
    uint8_t vl_Config = 0;

    if( MC_CGPS_IS_CONFIGURATION_ELEMENT_SET ( K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS ) )
    {
        vl_Config = vl_Config | K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS;
    }

    if(    (   MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE   )
            || MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE   )
           )
        && (   MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED  )
            || MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_MS_POS_TECH_MSBASED     )
           )
        &&     MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED )
      )
    {
        vl_Config = vl_Config | K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS;
    }

    if( FALSE == (   MC_CGPS_IS_FLAG_SET_IN_PARAM1( vl_Config,  K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS   )
                  || MC_CGPS_IS_FLAG_SET_IN_PARAM1( vl_Config,  K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS )
                 )
      )
    {
        if(    MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_MS_POS_TECH_ECID        )
            && MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE   )
            && MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED )
           )
        {
             vl_Config = vl_Config | K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED;
        }
    }

    vg_CGPSNafSessionConfig = vl_Config;
}



/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
#ifdef AGPS_UP_FTR
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 56
/**************************************************************************************************/
/* CGPS0_56WapPushInd :                                                                       */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_56WapPushInd(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState  = SAME;

    MC_CGPS_TRACE(("CGPS0_56WapPushInd : start"));

    if( p_FsmObject != NULL )
    {
        bool                 vl_RejectWap  = FALSE;
        t_CGPS_SuplPushInd*  pl_WapPushInd = (t_CGPS_SuplPushInd *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

        vl_RejectWap = CGPS0_57HandleSuplInit(&pl_WapPushInd->v_SuplPush);

        if( pl_WapPushInd->v_SuplPush.v_pointerToPayload != NULL )
        {
            MC_RTK_FREE_MEMORY( pl_WapPushInd->v_SuplPush.v_pointerToPayload );
        }
        if( pl_WapPushInd->v_SuplPush.v_pointerToHash != NULL )
        {
            MC_RTK_FREE_MEMORY( pl_WapPushInd->v_SuplPush.v_pointerToHash );
        }

        if( !vl_RejectWap )
        {
            pl_NextState = a_CGPSRunState;
        }
    }

    MC_CGPS_TRACE(("CGPS0_56WapPushInd : end"));

    return pl_NextState;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 57
/**************************************************************************************************/
/* CGPS0_57HandleSuplInit :                                                                       */
/**************************************************************************************************/
bool CGPS0_57HandleSuplInit( t_cgps_SuplPush* pp_SuplPush )
{
    uint8_t    vl_Index                = 0;
    bool  vl_AvailableIndexFound  = FALSE;
    bool  vl_RejectSuplInit       = FALSE;



#ifdef CMCC_LOGGING_ENABLE
    CGPS9_23LogPosition ( vl_Index, CGPS_SUPL_CMCC_AGPS_SESSION_STARTED, NULL);
#endif

    /* new external application registration */
    /* search for the first unused handle */
    while(    ( vl_Index < K_CGPS_MAX_NUMBER_OF_SUPL )
           && ( vl_AvailableIndexFound==FALSE    )
         )
    {
        if( s_CgpsSupl[vl_Index].v_IsRegistered == FALSE )
        {
            vl_AvailableIndexFound = TRUE;
            MC_CGPS_TRACE(("CGPS0_57HandleSuplInit : new handle dedicated : 0x%X", &s_CgpsSupl[vl_Index]));
            break;
        }
        else
        {
            vl_Index++;
        }
    }

    /* if the max number of external applications has been reached then the registration is rejected */
    if( vl_AvailableIndexFound == FALSE )
    {
        vl_RejectSuplInit   = TRUE;
        MC_CGPS_TRACE(("CGPS0_57HandleSuplInit : no handle found"));
    }

    else
    {
        MC_CGPS_TRACE(("CGPS0_57HandleSuplInit : NAF[%d], Supl[%d]", vg_CGPS_Naf_Count, vg_CGPS_Supl_Count));

        /* ++ LMSqb94416 ANP 15/07/2009 */
        if(   FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE)
           || FALSE == MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_AGPS_SESSION_MT_ALLOWED)
          )
        {
            vl_RejectSuplInit = TRUE;
            MC_CGPS_TRACE(("CGPS0_57HandleSuplInit : SUPL MO Allowed == FALSE"));
        }
        /* -- LMSqb94416 ANP 15/07/2009 */

        /* ++ LMSqb94416 ANP 15/07/2009 */
        if(vl_RejectSuplInit == FALSE)
        /* -- LMSqb94416 ANP 15/07/2009 */
        {
            if(CGPS4_18CheckNewSessionRight(CGPS_SUPL_APPLICATION_ID))
            {
                CGPS4_19UpdateOtherApplicationState(CGPS_SUPL_APPLICATION_ID);
                s_CgpsSupl[vl_Index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
            }
            else
            {
                s_CgpsSupl[vl_Index].v_Suspended = CGPS_MIN_PRIORITY_VALUE;
            }

            s_CgpsSupl[vl_Index].v_IsRegistered = TRUE;
            /* increase the number of applications */
            vg_CGPS_Supl_Count++;

            CGPS0_52ConfigureCgpsNextState();

            /* +LMSqc18414 */
            if( NULL != pg_CGPS_Supl_Init )
            {
                MC_RTK_FREE_MEMORY(pg_CGPS_Supl_Init);
            }

            pg_CGPS_Supl_Init = (uint8_t*) MC_RTK_GET_MEMORY( pp_SuplPush->v_PayloadLength );
            memcpy( pg_CGPS_Supl_Init , pp_SuplPush->v_pointerToPayload , pp_SuplPush->v_PayloadLength );
            vg_CGPS_Supl_Init_Length = pp_SuplPush->v_PayloadLength;
            /* -LMSqc18414 */


            if(!CGPS5_00SuplPushInd((uint32_t)&(s_CgpsSupl[vl_Index]), pp_SuplPush))
            {
                MC_CGPS_TRACE(("CGPS0_57HandleSuplInit : Hash code or SMS PDU problem"));
                vl_RejectSuplInit = TRUE;
                s_CgpsSupl[vl_Index].v_IsRegistered = FALSE;
                s_CgpsSupl[vl_Index].v_Suspended    = CGPS_MAX_PRIORITY_VALUE;
                vg_CGPS_Supl_Count--;
                CGPS4_20CloseSessionOperatorRules(CGPS_SUPL_APPLICATION_ID);
                CGPS0_52ConfigureCgpsNextState();
            }
            else
            {
                 CGPS4_55SendEventNotify(K_CGPS_ERR_GPS_ENGINE_ON);
            }
        }
        else
        {
            MC_CGPS_TRACE(("CGPS0_57HandleSuplInit : CGPS can't start"));
        }
    }


    return vl_RejectSuplInit;
}
#endif /*AGPS_UP_FTR*/
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 58
/**************************************************************************************************/
/* CGPS0_58EnableGps :                                                                       */
/**************************************************************************************************/
void CGPS0_58EnableGps(void)
{
    MC_CGPS_TRACE(("CGPS INFO 0_56EnableGps: GPS Enabled"));

}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 59
/**************************************************************************************************/
/* CGPS0_59DisableGps :                                                                       */
/**************************************************************************************************/
void CGPS0_59DisableGps(void)
{
    switch( vg_CGPS_State )
    {
    case K_CGPS_INACTIVE:
    case K_CGPS_ACTIVE_GPS_OFF:
    /*Do Nothing*/
    break;

    case K_CGPS_ACTIVE_GPS_ON:
    {
            vg_CGPS_State = K_CGPS_ACTIVE_GPS_OFF;
        CGPS0_14SleepExpired(NULL);
        CGPS0_33StartLoop();
    }
    break;

    default:
        MC_CGPS_TRACE(("CGPS WARN 0_57DisableGps: Wrong CGPS state"));
    break;
    }

    MC_CGPS_TRACE(("CGPS INFO 0_57DisableGps: GPS Disabled"));
}

/* +LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
#if defined (DUAL_OS_GEN_FTR) || defined ( CR_LMSQC18522_CPR )
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 60
/**
 * @brief
 *
 * @param p_FsmObject
 */
void CGPS0_60HwlInitInd(t_RtkObject* p_FsmObject)
{
    s_GN_GPS_Config vl_GPS_Config;
    /* initialize the GPS library */
#ifdef DUAL_OS_GEN_FTR
    MC_HALGPS_SET_SYSTEM_PATH((uint8_t *) "/sysv");
#endif

    GN_GPS_Initialise();

    CGPS4_10SetLbsHostVersion();

    /* Initialize PATH */
    CGPS4_38UpdateConfiguration();
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 61
/**
 * @brief
 *
 * @param p_FsmObject
 */
void  CGPS0_61LaunchInit(void)
{
    t_CGPS_init_dualos *pl_Msg=NULL;
    pl_Msg  =MC_RTK_GET_MEMORY(sizeof(*pl_Msg));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_HWL_INIT_IND,
                               (t_MsgHeader *)pl_Msg);

}
#endif
/* -LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */

/*+ LMSqc13768 : DeleteGpsData */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 62
void CGPS0_62DeleteNVStoreFields( uint32_t v_Fields )
{
    t_CGPS_DeleteGpsDataReq   *pl_CGPS_DeleteGpsDataReq=NULL;

    pl_CGPS_DeleteGpsDataReq = (t_CGPS_DeleteGpsDataReq*)MC_RTK_GET_MEMORY(sizeof(*pl_CGPS_DeleteGpsDataReq));

    pl_CGPS_DeleteGpsDataReq->v_FieldsToDelete = v_Fields;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_DELETE_GPS_DATA_REQ,
                               (t_MsgHeader *)pl_CGPS_DeleteGpsDataReq);
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 63
void CGPS0_63DeleteNVStoreFieldsReq( t_RtkObject *p_FsmObject )
{
    uint32_t vl_FieldsToDelete;
    uint8_t  vl_Count=0;
    int8_t   vl_FieldString[16];
    t_CGPS_DeleteGpsDataReq   *pl_CGPS_DeleteGpsDataReq;

    pl_CGPS_DeleteGpsDataReq = ( t_CGPS_DeleteGpsDataReq* )MC_CGPS_GET_MSG_POINTER(p_FsmObject);


    vl_FieldsToDelete = pl_CGPS_DeleteGpsDataReq->v_FieldsToDelete;


    if( 0 == vl_FieldsToDelete )
    {
        return;
    }


    /*  Seed deletion is independent of PE library.No need to send any request to PE library.
            Intentionally kept this request out of the below sequence of checks.
            Intentionally not incremented the counter: vl_Count . this should be not be incremented for seed deletion.
            13-July-2012-mohanbabu.n@stericsson.com 194997*/
    if( vl_FieldsToDelete & K_CGPS_FIELD_SEED_DATA)
    {
      MC_CGPS_TRACE(("Valid NV Store Delete Req : K_CGPS_FIELD_SEED_DATA" ));
      CGPS11_06EEClientDeleteSeedData();
    }


    if( vl_FieldsToDelete & K_CGPS_FIELD_FACTORY_START )
    {
    /*Begin Changes from rahul for Glonass time correction issue 07-Dec-2011 Mohan-194997*/
        //vl_Count = snprintf( (char *)vl_FieldString ,16, "AEPTUHIF" );
        vl_Count = snprintf( (char *)vl_FieldString ,16, "FACT" );
    /*End Changes from rahul for Glonass time correction issue 07-Dec-2011 Mohan-194997*/
    }
    else if ( vl_FieldsToDelete & K_CGPS_FIELD_COLD_START )
    {
        vl_Count = snprintf( (char *)vl_FieldString ,16, "COLD" );
    }
    else if ( vl_FieldsToDelete & K_CGPS_FIELD_WARM_START )
    {
        vl_Count = snprintf( (char *)vl_FieldString ,16, "WARM" );
    }
    else if ( vl_FieldsToDelete & K_CGPS_FIELD_HOT_START )
    {
        vl_Count = snprintf( (char *)vl_FieldString ,16, "HOT" );
    }
    else
    {
        if( vl_FieldsToDelete & K_CGPS_FIELD_ALMANAC )
        {
            vl_FieldString[ vl_Count ] = 'A';
            vl_Count++;
        }

        if( vl_FieldsToDelete & K_CGPS_FIELD_EPHEMERIS)
        {
            vl_FieldString[ vl_Count ] = 'E';
            vl_Count++;
        }

        if( vl_FieldsToDelete & K_CGPS_FIELD_POSITION)
        {
            vl_FieldString[ vl_Count ] = 'P';
            vl_Count++;
        }

        if( vl_FieldsToDelete & K_CGPS_FIELD_TIME)
        {
            vl_FieldString[ vl_Count ] = 'T';
            vl_Count++;
        }

        if( vl_FieldsToDelete & K_CGPS_FIELD_UTC)
        {
            vl_FieldString[ vl_Count ] = 'U';
            vl_Count++;
        }

        if( vl_FieldsToDelete & K_CGPS_FIELD_SV_HEALTH)
        {
            vl_FieldString[ vl_Count ] = 'H';
            vl_Count++;
        }

        if( vl_FieldsToDelete & K_CGPS_FIELD_IONO_MODEL)
        {
            vl_FieldString[ vl_Count ] = 'I';
            vl_Count++;
        }

        if( vl_FieldsToDelete & K_CGPS_FIELD_TCXO_CALIB)
        {
            vl_FieldString[ vl_Count ] = 'F';
            vl_Count++;
        }
        if( vl_FieldsToDelete & K_CGPS_FIELD_UP_LOCID)
        {
            CGPS5_48Delete_LocationID_Store( );
        }
    }
/* ++ LMSqc41399 */
    if( vl_Count > 0 )
    {
        s_GN_GPS_Nav_Data *pl_GN_GPS_Nav_Data=NULL;
        if(vg_CGPS_GpsState == K_CGPS_GPS_SLEEP )
/* -- LMSqc41399 */
        {
            uint8_t vl_Index;
/* ++ LMSqc34507 */
            MC_HALGPS_SLEEP( FALSE );
/* -- LMSqc34507 */
        /* Wakeup the GPS chip, delete the NV Store and put it back to COMA */
            GN_GPS_WakeUp();

            for( vl_Index = 0 ; vl_Index < 5 ; vl_Index++ )
            {
#ifdef __RTK_E__
#ifdef UTS_CPR
                MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,100);
#else
                MC_OST_START_TASK_TIMER(100);
#endif
#else
                OSA_Sleep( 100 );
#endif
                GN_GPS_Update();
            }

            GN_GPS_Sleep( 0 , K_CGPS_COMA_TIMER_DURATION );

            vl_FieldString[ vl_Count ] = 0;
            MC_CGPS_TRACE(("Valid NV Store Delete Req : %s" ,vl_FieldString));
            if( 0 != strncmp((const char*)vl_FieldString, "HOT", strlen((const char*)vl_FieldString)))
                CGPS4_31ClearNVStore( vl_FieldString );
/* ++ LMSqc34507 */
            MC_HALGPS_SLEEP( TRUE );
/* -- LMSqc34507 */
        /* Clear all previous Data */
            memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(*pl_GN_GPS_Nav_Data) );
            memset( (uint8_t*)&vg_Latest_Fix_Nav_Data, 0, sizeof(*pl_GN_GPS_Nav_Data) );
        }
        /* ++ LMSqc41399 */
        else if(vg_CGPS_GpsState == K_CGPS_GPS_OFF )
        {
            MC_CGPS_TRACE(("Valid NV Store Delete Req : %s" ,vl_FieldString));
            vl_FieldString[ vl_Count ] = 0;

            if( 0 != strncmp((const char*)vl_FieldString, "HOT", strlen((const char*)vl_FieldString)))
                CGPS4_31ClearNVStore( vl_FieldString );

            /* Clear all previous Data */
            memset( (uint8_t*)&vg_Nav_Data, 0, sizeof(*pl_GN_GPS_Nav_Data) );
            memset( (uint8_t*)&vg_Latest_Fix_Nav_Data, 0, sizeof(*pl_GN_GPS_Nav_Data) );
        }
        else if(vg_CGPS_GpsState == K_CGPS_GPS_RUNNING)
        {
            /*NO need to do anythings*/
            MC_CGPS_TRACE(("GPS is running, no need to delete assistance!!"));
        }

        /* -- LMSqc41399 */
    }
}
/*- LMSqc13768 : DeleteGpsData */

/*+ LMSqc32518 */
#ifdef CGPS_UPLOAD_PATCH_AT_INIT_FTR
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 64
/**************************************************************************************************/
/* CGPS0_64UploadPatch :                                                                       */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_64UploadPatch( t_RtkObject *p_FsmObject )
{
    const t_OperationDescriptor* pl_NextState = a_CGPSRunState;

    CGPS0_52ConfigureCgpsNextState();

    return pl_NextState;
}
#endif /*CGPS_UPLOAD_PATCH_AT_INIT_FTR*/
/*- LMSqc32518 */
/*+ LMSqc48667 */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 65
/**************************************************************************************************/
/* CGPS0_65ResetGpsContext :  This function is called at the end of a GPS session to cleanup after it                          */
/**************************************************************************************************/
void CGPS0_65ResetGpsContext()
{
    memset( &vg_Nav_Data , 0 , sizeof(vg_Nav_Data) );
}
/*- LMSqc48667 */


#ifdef GPS_OVER_HCI
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 66
/**************************************************************************************************/
/* CGPS0_66HciInit :                                                                       */
/**************************************************************************************************/
e_cgps_Error CGPS0_66HciInit()
{
    /* Sequence for Patch Upload over HCI */
    /* Upload Patch first then do init */
    MC_CGPS_TRACE(("CGPS0_66HciInit Entry"));


    MC_CGPS_TRACE(("Calling GN_GPS_Initialise"));
    /* initialize the GPS library */

    GN_GPS_Initialise();

    MC_CGPS_TRACE(("GN_GPS_Initialise done."));

    CGPS4_10SetLbsHostVersion();

    MC_CGPS_TRACE(("Calling GN_GPS_Get_Config"));

    /* Initialize PATH */
    CGPS4_38UpdateConfiguration();

    MC_CGPS_TRACE(("CGPS0_66HciInit Exit"));

    return K_CGPS_NO_ERROR;
}
#endif


#if defined( AGPS_UP_FTR ) || defined( AGPS_FTR )
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 67
/**************************************************************************************************/
/* CGPS0_67RegisterLocationForward :                                                                       */
/**************************************************************************************************/

e_cgps_Error CGPS0_67RegisterLocationForward(uint32_t vp_Handle,
                                                      t_cgps_ClientInfo *pp_ClientInfo,
                                                      e_cgps_LcsServiceType vp_ServType,
                                                      t_cgps_QoP  *pp_QoP)
{

       e_cgps_Error vl_Return = K_CGPS_NO_ERROR;

       MC_CGPS_TRACE(("CGPS0_67RegisterLocationForward : Handle %d", vp_Handle));

       if(!CGPS4_01IsRegistered(vp_Handle))
       {
           MC_CGPS_TRACE(("CGPS0_65 handle not registered"));
           vl_Return = K_CGPS_ERR_INVALID_HANDLE;
       }
       else
       {
           t_CGPS_MolfStartInd *pl_CGPS_MolfStartInd;
           uint32_t   vl_OsTimeStamp = GN_GPS_Get_OS_Time_ms();

           (s_CgpsNaf[vp_Handle]).v_Mode           = K_CGPS_SINGLE_SHOT;
           (s_CgpsNaf[vp_Handle]).p_Callback       = NULL;
           (s_CgpsNaf[vp_Handle]).v_OutputType     = 0;
           (s_CgpsNaf[vp_Handle]).v_Mask           = 0;
           (s_CgpsNaf[vp_Handle]).v_AgeLimit       = pp_QoP->vp_AgeLimit;
           (s_CgpsNaf[vp_Handle]).v_HorizAccuracy  = pp_QoP->vp_HorizAccuracy;
           (s_CgpsNaf[vp_Handle]).v_VertAccuracy   = pp_QoP->vp_VertAccuracy;
           (s_CgpsNaf[vp_Handle]).v_Config         = vg_CGPSNafSessionConfig;
           /* Single Shot now directly uses the Timeout field to decide on whether timeout has happened */
           (s_CgpsNaf[vp_Handle]).v_DeadlineOSTimeMS          = 0;
           (s_CgpsNaf[vp_Handle]).v_LastApplicationUpdateTime = 0;
           /*Suspended as NAF is not used for self location */
           (s_CgpsNaf[vp_Handle]).v_State                     = K_CGPS_NAF_SUSPENDED;
           (s_CgpsNaf[vp_Handle]).v_FirstFixDone      = FALSE;

           #ifdef CMCC_LOGGING_ENABLE

           (s_CgpsNaf[vp_Handle]).v_AlreadyUpdated    = FALSE;
           (s_CgpsNaf[vp_Handle]).v_RegisterTime        = vl_OsTimeStamp;

           #endif

           if( pp_QoP->vp_Timeout != 0 )
           {
               (s_CgpsNaf[vp_Handle]).v_Timeout = vl_OsTimeStamp + pp_QoP->vp_Timeout;
           }
           else
           {
               (s_CgpsNaf[vp_Handle]).v_Timeout = pp_QoP->vp_Timeout;
           }

           pl_CGPS_MolfStartInd = (t_CGPS_MolfStartInd * ) MC_RTK_GET_MEMORY( sizeof(*pl_CGPS_MolfStartInd) );
           if(pl_CGPS_MolfStartInd != NULL)
           {
               pl_CGPS_MolfStartInd->vp_Handle     = vp_Handle;
               pl_CGPS_MolfStartInd->vp_ServType   = vp_ServType;
               pl_CGPS_MolfStartInd->vp_QoP        = *pp_QoP;
               pl_CGPS_MolfStartInd->vp_ClientInfo = *pp_ClientInfo;

              MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                      PROCESS_CGPS, 0,
                                      CGPS_MOLF_START_IND,
                                      (t_MsgHeader *)pl_CGPS_MolfStartInd);
           }
           else
           {
               vl_Return = K_CGPS_ERR_MOLF_FAILED;
           }


       }

   return vl_Return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 68
/**************************************************************************************************/
/* CGPS0_68MolfStartReq :                                                                       */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_68MolfStartReq( t_RtkObject *p_FsmObject )
{
    const t_OperationDescriptor* pl_NextState = a_CGPSRunState;
    uint8_t vl_Status = FALSE;
    e_cgps_Error vl_ErrorStatus = K_CGPS_NO_ERROR;
    t_CGPS_MolfStartInd   *pl_CGPS_MolfStartInd;
    bool  vl_Result;

    pl_CGPS_MolfStartInd = ( t_CGPS_MolfStartInd* )MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if (MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED))
    {
        if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE) == FALSE)
        {


            if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE))
            {
#ifdef AGPS_FTR
               vl_Status = CGPS7_40MolfStart((&s_CgpsNaf[pl_CGPS_MolfStartInd->vp_Handle]) , pl_CGPS_MolfStartInd);
#else
               vl_Status = FALSE;
               MC_CGPS_TRACE(("CGPS7_40MolfStart : CP is not enabled at compile time."));
#endif
            }
        }
        else
        {
#ifdef AGPS_UP_FTR

            if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_SUPLV2_SUPPORTED) == TRUE)
            {

                if((MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE) == TRUE)||
                   (MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE)))
                {
                       //We got start SUPL session as the user requested for the third party location transfer
                     if((pl_CGPS_MolfStartInd->vp_ClientInfo.t_thirdparty_info.v_ThirdPartyId >= 1 )&&(pl_CGPS_MolfStartInd->vp_ClientInfo.t_thirdparty_info.v_ThirdPartyId <= 8))
                     {
                         if((strcmp((const char*)pl_CGPS_MolfStartInd->vp_ClientInfo.t_thirdparty_info.a_ThirdPartyName , "") != 0)
                              && (strcmp((const char*)pl_CGPS_MolfStartInd->vp_ClientInfo.t_thirdparty_info.a_ThirdPartyName , "0" ) != 0))
                         {
                            vl_Result = CGPS5_13StartSUPL(NULL, (t_cgps_ThirdPartyInfo *)&(pl_CGPS_MolfStartInd->vp_ClientInfo.t_thirdparty_info));
                             if(vl_Result == TRUE)
                             {   
                                 //Called here configureNextState fn as we are starting a SUPL session, we have to update the NAF sessions and initialize the GPS library
                                 CGPS0_52ConfigureCgpsNextState();
                             }
                              
                         }
                         else
                         {
                             vl_ErrorStatus = K_CGPS_ERR_INVALID_PARAMETER;
                         }
                     }
                     else
                     {
                        vl_ErrorStatus = K_CGPS_ERR_INVALID_PARAMETER;
                     }

                }
            }
#endif/* AGPS_UP_FTR*/
         }
    }
    if(vl_Status == FALSE)
    {
          MC_CGPS_TRACE(("CGPS0_68: MOLF not supported Config= %x",vg_CGPS_Configuration ));
    }

    return pl_NextState;
}
#endif /* AGPS_FTR | AGPS_UP_FTR*/


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 69
/**************************************************************************************************/
/* CGPS0_69UpdatePlatformConfiguration                                                                */
/**************************************************************************************************/
e_cgps_Error CGPS0_69UpdatePlatformConfiguration(t_cgps_PlatformConfig * pp_Config)
{
    t_cgps_PlatformConfig   *pl_cgps_PlatformConfig=NULL;

    t_CGPS_PlatformConfig* pl_MsgPlatformConfig=NULL;
    pl_MsgPlatformConfig  = (t_CGPS_PlatformConfig *)MC_RTK_GET_MEMORY(sizeof(*pl_MsgPlatformConfig));

    memcpy((uint8_t*)(&pl_MsgPlatformConfig->v_Config),(uint8_t*)pp_Config,sizeof(*pl_cgps_PlatformConfig));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_UPDATE_PLATFORM_CONFIG,
                               (t_MsgHeader *)pl_MsgPlatformConfig);

    return K_CGPS_NO_ERROR;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 70
/**************************************************************************************************/
/* CGPS0_70PlatformConfigurationReceived :                                                            */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_70PlatformConfigurationReceived(t_RtkObject* p_FsmObject)
{
    t_CGPS_PlatformConfig *pl_MsgPlatformConfig;
    const t_OperationDescriptor* pl_NextState = SAME;

    pl_MsgPlatformConfig = ( t_CGPS_PlatformConfig* )MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgPlatformConfig->v_Config.v_Config1MaskValid, K_CGPS_CONFIG_PUREMSA_CP) )
    {
        MC_CGPS_UPDATE_PLATFORM_CONFIGURATION_ELEMENT(pl_MsgPlatformConfig->v_Config.v_Config1Mask, K_CGPS_CONFIG_PUREMSA_CP);
        MC_CGPS_TRACE(("CGPS0_70PlatformConfigurationReceived : Set Pure MSA CP to %i",
            (vg_cgps_platform_config.v_Config1Mask & K_CGPS_CONFIG_PUREMSA_CP)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgPlatformConfig->v_Config.v_Config1MaskValid, K_CGPS_CONFIG_PUREMSA_UP) )
    {
        MC_CGPS_UPDATE_PLATFORM_CONFIGURATION_ELEMENT(pl_MsgPlatformConfig->v_Config.v_Config1Mask, K_CGPS_CONFIG_PUREMSA_UP);
        MC_CGPS_TRACE(("CGPS0_70PlatformConfigurationReceived : Set Pure MSA UP to %i",
            (vg_cgps_platform_config.v_Config1Mask & K_CGPS_CONFIG_PUREMSA_UP)?TRUE:FALSE ));
    }

    if( MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( pl_MsgPlatformConfig->v_Config.v_Config1MaskValid, K_CGPS_CONFIG_EXT_LNA) )
    {
        MC_CGPS_UPDATE_PLATFORM_CONFIGURATION_ELEMENT(pl_MsgPlatformConfig->v_Config.v_Config1Mask, K_CGPS_CONFIG_EXT_LNA);
        MC_CGPS_TRACE(("CGPS0_70PlatformConfigurationReceived : Set External LNA to %i",
            (vg_cgps_platform_config.v_Config1Mask & K_CGPS_CONFIG_EXT_LNA)?TRUE:FALSE ));
    }


    if(pl_MsgPlatformConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_MSA_MANDATORY_MASK)
    {
        vg_cgps_platform_config.v_MsaMandatoryMask = pl_MsgPlatformConfig->v_Config.v_MsaMandatoryMask;

        MC_CGPS_TRACE(("CGPS0_70PlatformConfigurationReceived : Change MSA Mandatory Mask Setting %i",vg_cgps_platform_config.v_MsaMandatoryMask));
    }

    if(pl_MsgPlatformConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_MSB_MANDATORY_MASK)
    {
        vg_cgps_platform_config.v_MsbMandatoryMask = pl_MsgPlatformConfig->v_Config.v_MsbMandatoryMask;

        MC_CGPS_TRACE(("CGPS0_70PlatformConfigurationReceived : Change MSB Mandatory Mask Setting %i",vg_cgps_platform_config.v_MsbMandatoryMask));
    }

    if(pl_MsgPlatformConfig->v_Config.v_Config2Mask & K_CGPS_CONFIG2_GPS_POWER_SAVING_INT)
    {
        vg_cgps_platform_config.v_SleepTimerDuration = pl_MsgPlatformConfig->v_Config.v_SleepTimerDuration;
        vg_cgps_platform_config.v_ComaTimerDuration = pl_MsgPlatformConfig->v_Config.v_ComaTimerDuration;
        vg_cgps_platform_config.v_DeepComaDuration = pl_MsgPlatformConfig->v_Config.v_DeepComaDuration;

        MC_CGPS_TRACE(("CGPS0_70PlatformConfigurationReceived : Change SleepTimeDuration ComaTimeDuration DeepComaDuration Setting %i %i %i",\
            vg_cgps_platform_config.v_SleepTimerDuration,vg_cgps_platform_config.v_ComaTimerDuration,vg_cgps_platform_config.v_DeepComaDuration));

    }

    return pl_NextState;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 71
/**************************************************************************************************/
/* CGPS0_71EventNotificationRegister : Notifies the NI GPS ON OFF Notification events */
/**************************************************************************************************/
e_cgps_GpsStatus CGPS0_71EventNotificationRegister(t_cgps_NotifyEventsCallback vp_Callback )
{
    e_cgps_GpsStatus vl_Return;

    MC_CGPS_TRACE(("CGPS0_71EventNotificationRegister"));

    if (vp_Callback != NULL)
    {
        vg_CgpsEvtNotify = vp_Callback;
        vl_Return = K_CGPS_ERR_GPS_NO_ERROR;
    }
    else
    {
         vl_Return = K_CGPS_ERR_GPS_ERROR;
    }

    return vl_Return;
}



#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 72
/**************************************************************************************************/
/* CGPS0_72RegisterGeoTargetAreaFence : Registers a function to report positioning event    */
/**************************************************************************************************/
e_cgps_Error CGPS0_72RegisterGeoTargetAreaFence( uint32_t                     vp_Handle,
                                                 t_cgps_NavigationCallback    vp_Callback,
                                                 uint8_t                      vp_OutputType,
                                                 uint16_t                     vp_NmeaMask,
                                                 t_cgps_PositioningParams     vp_PositioningParams,
                                                 t_cgps_GeographicTargetArea* pp_TargetArea,
                                                 uint16_t                     vp_SpeedThreshold,
                                                 e_cgps_AreaEventType         vp_AreaEventType
                                               )
{
    e_cgps_Error vl_Return = K_CGPS_NO_ERROR;
    e_cgps_Error vl_Error  = K_CGPS_NO_ERROR;

    MC_CGPS_TRACE(("CGPS0_72RegisterGeoTargetAreaFence Handle %d", vp_Handle));

/* Check parameter validity */
    if(!CGPS4_01IsRegistered(vp_Handle))
    {
        MC_CGPS_TRACE(("ERR handle not registered"));
        vl_Return = K_CGPS_ERR_INVALID_HANDLE;
    }
    else
    {
        if ( vp_Callback  == NULL  )
        {
            MC_CGPS_TRACE(("ERR : Callback not set"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }
        MC_CGPS_TRACE(( "Parameter Output Type : %i", vp_OutputType));
        if( vp_OutputType != K_CGPS_NMEA && vp_OutputType != K_CGPS_C_STRUCT && vp_OutputType != K_CGPS_NMEA_AND_C_STRUCT )
        {
            MC_CGPS_TRACE(("ERR : Invalid output type"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }
        MC_CGPS_TRACE(( "Parameter NMEA Mask : %i", vp_NmeaMask));

        vl_Error = CGPS4_53ValidatePositioningParams( &vp_PositioningParams );
        if( vl_Error != K_CGPS_NO_ERROR )
            vl_Return = vl_Error;

        vl_Error = CGPS4_52ValidateTargetAreaParams( pp_TargetArea );
        if( vl_Error != K_CGPS_NO_ERROR )
            vl_Return = vl_Error;

        MC_CGPS_TRACE(( "Parameter Speed Threshold : %i", vp_SpeedThreshold));
        MC_CGPS_TRACE(( "Parameter Area Event Type : %i", vp_AreaEventType));

        if( vp_SpeedThreshold < 1 || vp_SpeedThreshold > 200 )
        {
            MC_CGPS_TRACE(("ERR : Speed Threshold not in range [range 0..100]"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }

        if(    vp_AreaEventType != K_CGPS_ENTERING_AREA && vp_AreaEventType != K_CGPS_INSIDE_AREA
            && vp_AreaEventType != K_CGPS_OUTSIDE_AREA  && vp_AreaEventType != K_CGPS_LEAVING_AREA
          )
        {
            MC_CGPS_TRACE(("ERR : Invalid event type"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }
    }
/* End of parameter validity check */

    if( vl_Return == K_CGPS_NO_ERROR )
    {

        MC_CGPS_TRACE(("CGPS0_72RegisterGeoTargetAreaFence Input Parameters validity check PASS"));

        if(     MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS    )
             || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS  )
             || MC_CGPS_IS_FLAG_SET_IN_PARAM1 ( vg_CGPSNafSessionConfig, K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED )
          )
        {
            t_CGPS_FixReqInd *pl_CGPS_FixReqInd;
            bool              vl_Result;
            uint32_t          vl_OsTimeStamp = GN_GPS_Get_OS_Time_ms();
            uint8_t           vl_NumOfElem   = 0;

            if( vp_PositioningParams.v_NumberOfFixes == 1 )
            {
                (s_CgpsNaf[vp_Handle]).v_Mode               = K_CGPS_SINGLE_SHOT;
                (s_CgpsNaf[vp_Handle]).v_AgeLimit           = vp_PositioningParams.v_AgeLimit;

                /* Single Shot now directly uses the Timeout field to decide on whether timeout has happened */
                (s_CgpsNaf[vp_Handle]).v_DeadlineOSTimeMS   = 0;

                if(  vp_PositioningParams.v_Timeout != 0 )
                {
                    (s_CgpsNaf[vp_Handle]).v_Timeout = vl_OsTimeStamp + vp_PositioningParams.v_Timeout;
                }
                else
                {
                    (s_CgpsNaf[vp_Handle]).v_Timeout = vp_PositioningParams.v_Timeout;
                }
            }
            else
            {
                (s_CgpsNaf[vp_Handle]).v_Mode                = K_CGPS_PERIODIC;
                (s_CgpsNaf[vp_Handle]).v_FixRate             = vp_PositioningParams.v_FixInterval;
                (s_CgpsNaf[vp_Handle]).v_Timeout             = 128000;
                (s_CgpsNaf[vp_Handle]).v_DeadlineOSTimeMS    = vl_OsTimeStamp
                                                                + vp_PositioningParams.v_FixInterval
                                                                + vp_PositioningParams.v_StartTime * 1000;
                (s_CgpsNaf[vp_Handle]).v_StartTime           = vp_PositioningParams.v_StartTime ;
                (s_CgpsNaf[vp_Handle]).v_StopTime            = vp_PositioningParams.v_StopTime ;
                /*TODO - Number of fixes to be handled */
            }

            if( vp_PositioningParams.v_HorizAccuracy )
            {
                (s_CgpsNaf[vp_Handle]).v_HorizAccuracy             = vp_PositioningParams.v_HorizAccuracy;
            }
            else
            {
                (s_CgpsNaf[vp_Handle]).v_HorizAccuracy             = 100;
            }

            if( vp_PositioningParams.v_VertAccuracy )
            {
                (s_CgpsNaf[vp_Handle]).v_VertAccuracy              = vp_PositioningParams.v_VertAccuracy;
            }
            else
            {
                (s_CgpsNaf[vp_Handle]).v_VertAccuracy              = 150;
            }

            (s_CgpsNaf[vp_Handle]).v_State                     = K_CGPS_NAF_READY;
            (s_CgpsNaf[vp_Handle]).p_Callback                  = vp_Callback;
            (s_CgpsNaf[vp_Handle]).v_OutputType                = vp_OutputType;
            (s_CgpsNaf[vp_Handle]).v_Mask                      = vp_NmeaMask;
            (s_CgpsNaf[vp_Handle]).v_Config                    = vg_CGPSNafSessionConfig;
            (s_CgpsNaf[vp_Handle]).v_LastApplicationUpdateTime = 0;
#ifdef CMCC_LOGGING_ENABLE
            (s_CgpsNaf[vp_Handle]).v_AlreadyUpdated            = FALSE;
            (s_CgpsNaf[vp_Handle]).v_RegisterTime              = vl_OsTimeStamp;
#endif //CMCC_LOGGING_ENABLE
            (s_CgpsNaf[vp_Handle]).v_FirstFixDone              = FALSE;
            (s_CgpsNaf[vp_Handle]).v_AreaEventType             = vp_AreaEventType;
            (s_CgpsNaf[vp_Handle]).p_TargetArea                = (t_cgps_GeographicTargetArea*)MC_RTK_GET_MEMORY(sizeof(*pp_TargetArea));

            vl_NumOfElem = (pp_TargetArea->v_ShapeType==K_CGPS_POLYGON_AREA)?pp_TargetArea->v_NumberOfPoints:1;

            memcpy((s_CgpsNaf[vp_Handle]).p_TargetArea,   pp_TargetArea,   sizeof(*pp_TargetArea));
            memcpy((s_CgpsNaf[vp_Handle]).p_TargetArea->v_LatSign,   pp_TargetArea->v_LatSign,   (sizeof(uint8_t) * vl_NumOfElem));
            memcpy((s_CgpsNaf[vp_Handle]).p_TargetArea->v_Latitude,  pp_TargetArea->v_Latitude,  (sizeof(uint32_t)* vl_NumOfElem));
            memcpy((s_CgpsNaf[vp_Handle]).p_TargetArea->v_Longitude, pp_TargetArea->v_Longitude, (sizeof(int32_t) * vl_NumOfElem));

            CGPS4_52ValidateTargetAreaParams( (s_CgpsNaf[vp_Handle]).p_TargetArea );

            /*TODO - Speed treshold to be handled*/

            pl_CGPS_FixReqInd = (t_CGPS_FixReqInd * ) MC_RTK_GET_MEMORY( sizeof(*pl_CGPS_FixReqInd) );

            pl_CGPS_FixReqInd->v_Handle = (t_CgpsNaf *)vp_Handle;
            pl_CGPS_FixReqInd->v_Result = &vl_Result;

            if ( !MC_RTK_SEM_CONSUME( CGPS_SEM1 )) /* prevent other applis to use the semaphore #2 */
                { MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix : 0 != sem_wait(CGPS_SEM1)")); }

            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                       PROCESS_CGPS, 0,
                                       CGPS_FIX_REQ_IND,
                                       (t_MsgHeader *)pl_CGPS_FixReqInd);

            if ( !MC_RTK_SEM_CONSUME( CGPS_SEM2 )) /* Wait for CGPS to complete processing of the request */
                { MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix : 0 != sem_wait(CGPS_SEM2)")); }
            if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM1 )) /* allow another application to use sem #2 */
                { MC_CGPS_TRACE(("CGPS0_13RegisterSingleShotFix : 0 != sem_post(CGPS_SEM1)")); }

            if( !vl_Result )
            {
                vl_Return = K_CGPS_ERR_GPS_NOT_STARTED;
            }
        }
        else
        {
             vl_Return = K_CGPS_ERR_UNSUPPORTED_SERVICE;
        }
    }

    return vl_Return;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 73
/**************************************************************************************************/
/* CGPS0_73ServiceStartReq2 : Send the Application ID                                            */
/**************************************************************************************************/
e_cgps_Error CGPS0_73ServiceStartReq2(uint32_t* pp_Handle, t_cgps_ApplicationID_Info *p_AppId_Info)
{
    t_CgpsRegisterResult vl_out;
    t_CGPS_NewNaf       *pl_Msg=NULL;
    pl_Msg        = (t_CGPS_NewNaf*)MC_RTK_GET_MEMORY(sizeof(*pl_Msg));

    vl_out.v_Result = K_CGPS_NO_ERROR;

    MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2: Inside"));


    if( p_AppId_Info != NULL &&  p_AppId_Info->v_ApplicationIDInfo_present )
    {
        pl_Msg->p_AppIdInfo = (t_cgps_ApplicationID_Info*) MC_RTK_GET_MEMORY(sizeof(t_cgps_ApplicationID_Info));

        if((strcmp((const char*)p_AppId_Info->a_ApplicationProvider , "") != 0) && (strcmp((const char*)p_AppId_Info->a_ApplicationName , "") != 0)
            && (strcmp((const char*)p_AppId_Info->a_ApplicationProvider , "0" ) != 0)&& (strcmp((const char*)p_AppId_Info->a_ApplicationName , "0" ) != 0))
        {
            memcpy(pl_Msg->p_AppIdInfo, p_AppId_Info, sizeof(t_cgps_ApplicationID_Info));
            if((strcmp((const char*)p_AppId_Info->a_ApplicationVersion , "") == 0))
            {
                strncpy((char*)pl_Msg->p_AppIdInfo->a_ApplicationVersion , "0" , K_CGPS_MAX_APP_VERSION_LEN );
                MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2 if "": Application Version %d",pl_Msg->p_AppIdInfo->a_ApplicationVersion));
            }
            pl_Msg->p_AppIdInfo->v_ApplicationIDInfo_present = TRUE ;

            MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2: Application ID Info present =%d",pl_Msg->p_AppIdInfo->v_ApplicationIDInfo_present));
            MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2: Application Provider %s",pl_Msg->p_AppIdInfo->a_ApplicationProvider));
            MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2: Application Name %s",pl_Msg->p_AppIdInfo->a_ApplicationName));
            MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2: Application Version %s",pl_Msg->p_AppIdInfo->a_ApplicationVersion));
        }
        else
        {

           pl_Msg->p_AppIdInfo->v_ApplicationIDInfo_present = FALSE ;
           MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2: Application ID Info present =%d",pl_Msg->p_AppIdInfo->v_ApplicationIDInfo_present));
           vl_out.v_Result = K_CGPS_ERR_INVALID_PARAMETER;
           MC_RTK_FREE_MEMORY(pl_Msg->p_AppIdInfo);
           pl_Msg->p_AppIdInfo = NULL;
        }
    }
    else
    {
        pl_Msg->p_AppIdInfo = NULL;
    }

    pl_Msg->v_out = &vl_out;

    if ( !MC_RTK_SEM_CONSUME( CGPS_SEM1 )) /* prevent other applis to use the semaphore #2 */
        { MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2 : 0 != sem_wait(CGPS_SEM1)")); }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_CGPS,0,CGPS_NEW_NAF_REGISTERED_IND,(t_MsgHeader*)pl_Msg );

    MC_CGPS_TRACE(("Waiting for CGPS to complete processing of request"));
    if ( !MC_RTK_SEM_CONSUME( CGPS_SEM2 )) /* Wait for CGPS to complete processing of the request */
        { MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2 : 0 != sem_wait(CGPS_SEM2)")); }

    MC_CGPS_TRACE(("allow another application to use sem #2"));
    if ( !MC_RTK_SEM_PRODUCE( CGPS_SEM1 )) /* allow another application to use sem #2 */
        { MC_CGPS_TRACE(("CGPS0_73ServiceStartReq2 : 0 != sem_post(CGPS_SEM1)")); }

    if(vl_out.v_Result == K_CGPS_NO_ERROR)
    {
        *pp_Handle = vl_out.v_Handle;
    }

    return vl_out.v_Result;
}

#if defined( AGPS_UP_FTR )

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 76
/**************************************************************************************************/
/* CGPS0_76RegisterLocationRetrieval : Registers a function to retriev the location of a third party    */
/**************************************************************************************************/
e_cgps_Error CGPS0_76RegisterLocationRetrieval(  int32_t                      vp_Handle,
                                                        t_cgps_NavigationCallback    vp_Callback,
                                                        t_cgps_ClientInfo            *pp_ClientInfo,
                                                        t_cgps_QoP                   *pp_Qop)

{

    e_cgps_Error vl_Return = K_CGPS_NO_ERROR;

    MC_CGPS_TRACE(("CGPS0_76RegisterLocationRetrieval : Handle %d", vp_Handle));

    if(!CGPS4_01IsRegistered(vp_Handle))
    {
        MC_CGPS_TRACE(("CGPS0_76 handle not registered"));
        vl_Return = K_CGPS_ERR_INVALID_HANDLE;
    }
    else
    {
        if((pp_ClientInfo->t_thirdparty_info.v_ThirdPartyId < 1)||(pp_ClientInfo->t_thirdparty_info.v_ThirdPartyId > 8))
        {
            MC_CGPS_TRACE(( "CGPS0_76RegisterLocationRetrieval : Invalid Third Party id %d", pp_ClientInfo->t_thirdparty_info.v_ThirdPartyId));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }
        if((strcmp((const char*)pp_ClientInfo->t_thirdparty_info.a_ThirdPartyName , "") == 0)
             ||(strcmp((const char*)pp_ClientInfo->t_thirdparty_info.a_ThirdPartyName , "0" ) == 0))
        {
            MC_CGPS_TRACE(("CGPS0_76RegisterLocationRetrieval: Invalid Third Party Name"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }

        if(strlen((const char*)pp_ClientInfo->t_thirdparty_info.a_ThirdPartyName)== 0 || strlen((const char*)pp_ClientInfo->t_thirdparty_info.a_ThirdPartyName) > K_CGPS_MAX_THIRDPARTY_NAME_LEN )
        {
            MC_CGPS_TRACE(("CGPS0_76RegisterLocationRetrieval: Invalid Third Party Name"));
            vl_Return = K_CGPS_ERR_INVALID_PARAMETER;
        }
        if(vl_Return == K_CGPS_NO_ERROR)
        {
            t_CGPS_ThirdPartyReqInd *pl_CGPS_ThirdPartyReqInd;
            uint32_t   vl_OsTimeStamp = GN_GPS_Get_OS_Time_ms();

            (s_CgpsNaf[vp_Handle]).v_Mode                           = K_CGPS_SINGLE_SHOT;
            (s_CgpsNaf[vp_Handle]).p_ThirdPartyPositionCallback     = vp_Callback;
            (s_CgpsNaf[vp_Handle]).v_OutputType                     = 0;
            (s_CgpsNaf[vp_Handle]).v_Mask                           = 0;
            (s_CgpsNaf[vp_Handle]).v_AgeLimit                       = pp_Qop->vp_AgeLimit;
            (s_CgpsNaf[vp_Handle]).v_HorizAccuracy                  = pp_Qop->vp_HorizAccuracy;
            (s_CgpsNaf[vp_Handle]).v_VertAccuracy                   = pp_Qop->vp_VertAccuracy;
            /* Positioning is  considered network based for third party retrieval as positioning is not done in UE */
            (s_CgpsNaf[vp_Handle]).v_Config                         = K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED;
            (s_CgpsNaf[vp_Handle]).v_DeadlineOSTimeMS               = 0;
            (s_CgpsNaf[vp_Handle]).v_LastApplicationUpdateTime      = 0;
            (s_CgpsNaf[vp_Handle]).v_State                          = K_CGPS_NAF_READY;
            (s_CgpsNaf[vp_Handle]).v_FirstFixDone                   = FALSE;

#ifdef CMCC_LOGGING_ENABLE
            (s_CgpsNaf[vp_Handle]).v_AlreadyUpdated                 = FALSE;
            (s_CgpsNaf[vp_Handle]).v_RegisterTime                   = vl_OsTimeStamp;
#endif

            if( pp_Qop->vp_Timeout != 0 )
            {
                (s_CgpsNaf[vp_Handle]).v_Timeout = vl_OsTimeStamp + pp_Qop->vp_Timeout;
            }
            else
            {
                (s_CgpsNaf[vp_Handle]).v_Timeout = pp_Qop->vp_Timeout;
            }

            pl_CGPS_ThirdPartyReqInd = (t_CGPS_ThirdPartyReqInd * ) MC_RTK_GET_MEMORY( sizeof(*pl_CGPS_ThirdPartyReqInd) );

            if(pl_CGPS_ThirdPartyReqInd != NULL)
            {
                pl_CGPS_ThirdPartyReqInd->vp_Handle     = vp_Handle;
                pl_CGPS_ThirdPartyReqInd->vp_QoP        = *pp_Qop;
                pl_CGPS_ThirdPartyReqInd->vp_ClientInfo = *pp_ClientInfo;

                MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                           PROCESS_CGPS, 0,
                           CGPS_LOCATION_RETRIEVAL_REQ,
                           (t_MsgHeader *)pl_CGPS_ThirdPartyReqInd);
            }
        }


    }

    return vl_Return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 77
/**************************************************************************************************/
/* CGPS0_77ThirdPartyLocReq :                                                                       */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_77ThirdPartyLocReq( t_RtkObject *p_FsmObject )
{
    const t_OperationDescriptor* pl_NextState = a_CGPSRunState;
    t_CGPS_ThirdPartyReqInd*     pl_CGPS_ThirdPartReqInd;
    bool  vl_Result;

    pl_CGPS_ThirdPartReqInd = ( t_CGPS_ThirdPartyReqInd* )MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if(    (MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE))
        && (MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_PREF_UPLANE ))
       )
    {
        if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_SUPLV2_SUPPORTED) == TRUE)
        {
               //We got start SUPL session as the user requested for the third party location retrieval
             MC_CGPS_TRACE(("CGPS0_77: Third Party Id = %d",     pl_CGPS_ThirdPartReqInd->vp_ClientInfo.t_thirdparty_info.v_ThirdPartyId ));
             MC_CGPS_TRACE(("CGPS0_77: Third Party Name = %s \n",pl_CGPS_ThirdPartReqInd->vp_ClientInfo.t_thirdparty_info.a_ThirdPartyName ));
             vl_Result =  CGPS5_47ThirdPartyStartSUPL(&s_CgpsNaf[pl_CGPS_ThirdPartReqInd->vp_Handle], (t_cgps_ThirdPartyInfo *)&(pl_CGPS_ThirdPartReqInd->vp_ClientInfo.t_thirdparty_info));
             if(vl_Result == TRUE)
             {
                 CGPS0_52ConfigureCgpsNextState();
             }
        }
    }
    else
    {
         MC_CGPS_TRACE(("CGPS0_77: ThirdPartyLocReq not supported Config= %x",vg_CGPS_Configuration ));
    }

    return pl_NextState;
}
#endif /* AGPS_UP_FTR*/


#ifdef AGPS_TEST_MODE_FTR
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 78
/**************************************************************************************************/
/* CGPS0_78ProductionTestStartReq :                         */
/**************************************************************************************************/
e_cgps_Error CGPS0_78ProductionTestStartReq(uint16_t v_TestMask, t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    t_CGPS_ProdTestStartReq* pl_ProdTest=NULL;

    pl_ProdTest           = (t_CGPS_ProdTestStartReq*)MC_RTK_GET_MEMORY(sizeof(*pl_ProdTest));
    pl_ProdTest->v_State = (uint32_t)CGPS_TREQ_STATE_START;
    pl_ProdTest->v_TestMask   = v_TestMask;
    pl_ProdTest->p_ProdTestReq   = (t_CGPS_TestRequest*)MC_RTK_GET_MEMORY(sizeof(t_CGPS_TestRequest));
    memcpy(pl_ProdTest->p_ProdTestReq, p_cgpsTREQ_Req, sizeof(t_CGPS_TestRequest));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_CGPS,0,CGPS_PRODUCTION_TEST_START_REQ,(t_MsgHeader*)pl_ProdTest );

    return K_CGPS_NO_ERROR;
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 79
/**************************************************************************************************/
/* CGPS0_79ProductionTestStopReq :                         */
/**************************************************************************************************/

e_cgps_Error CGPS0_79ProductionTestStopReq(uint16_t v_TestMask, uint8_t v_Status)
{
    t_CGPS_ProdTestStopReq* pl_ProdTest=NULL;
    pl_ProdTest           = (t_CGPS_ProdTestStopReq*)MC_RTK_GET_MEMORY(sizeof(*pl_ProdTest));
    pl_ProdTest->v_TestMask   = v_TestMask;
    pl_ProdTest->v_Status   = v_Status;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_CGPS,0,CGPS_PRODUCTION_TEST_STOP_REQ,(t_MsgHeader*)pl_ProdTest );

    return K_CGPS_NO_ERROR;
}
#endif




#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 80
/**************************************************************************************************/
/* CGPS0_80UpdateTraceConfiguration                                                                */
/**************************************************************************************************/
e_cgps_Error CGPS0_80UpdateTraceConfiguration(t_cgps_ModuleTraceConfig * pp_Config)
{
    t_cgps_ModuleTraceConfig   *pl_TraceConfig    = NULL;
    t_cgps_TraceConfig         *pl_MsgTraceConfig = NULL;

    pl_MsgTraceConfig  = (t_cgps_TraceConfig *)MC_RTK_GET_MEMORY(sizeof(*pl_MsgTraceConfig));

    memcpy((uint8_t*)(&pl_MsgTraceConfig->v_Config),(uint8_t*)pp_Config, sizeof(*pl_TraceConfig));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_UPDATE_TRACE_CONFIG,
                               (t_MsgHeader *)pl_MsgTraceConfig );

    return K_CGPS_NO_ERROR;
}


#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 81
/**************************************************************************************************/
/* CGPS0_81TraceConfigurationReceived :                                                            */
/**************************************************************************************************/
const t_OperationDescriptor* CGPS0_81TraceConfigurationReceived(t_RtkObject* p_FsmObject)
{
    t_cgps_TraceConfig          *pl_MsgTraceConfig;
    const t_OperationDescriptor *pl_NextState = SAME;
    t_lbscfg_LogParam            vl_TraceConfig;

    pl_MsgTraceConfig = ( t_cgps_TraceConfig* )MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    vl_TraceConfig.v_Type     = pl_MsgTraceConfig->v_Config.v_Destination;
    vl_TraceConfig.v_Filesize = pl_MsgTraceConfig->v_Config.v_Filesize;
    vl_TraceConfig.v_Opt      = pl_MsgTraceConfig->v_Config.v_Severity;

    MC_CGPS_TRACE(("LogConfig Module=%d Destn=%d FileSize=%d Severity=%d\n",             \
                   pl_MsgTraceConfig->v_Config.v_Source,             \
                   pl_MsgTraceConfig->v_Config.v_Destination,        \
                   pl_MsgTraceConfig->v_Config.v_Filesize,           \
                   pl_MsgTraceConfig->v_Config.v_Severity));

    LBSCFG_LogSetConfiguration((e_lbscfg_LogModuleType)pl_MsgTraceConfig->v_Config.v_Source,&vl_TraceConfig);

    return pl_NextState;
}


/*
 * Purpose for this section is to force checking at compilation time
 * and detect possible change or discrepencies between CGPS expectations
 * and external declaration.
 */
#define MC_CGPS_STRUCT_CHECK_DECLARE(cgpsType, externalType) \
     cgpsType vg_##cgpsType##_unusedVar; \
     externalType vg_##externalType##_unusedVar;

#define MC_CGPS_STRUCT_CHECK_FIELD(cgpsType, externalType, cgpsField, externalField) \
   typedef char cgpsType##cgpsField ## _unusedType[\
      1+sizeof(vg_##cgpsType##_unusedVar.cgpsField) \
       -sizeof(vg_##externalType##_unusedVar.externalField) \
      ]; \
   typedef char t_##externalType##externalField##_unusedType[ \
      1+sizeof(vg_##externalType##_unusedVar.externalField) \
       - sizeof(vg_##cgpsType##_unusedVar.cgpsField)];

#define MC_CGPS_STRUCT_CHECK_END(cgpsType, externalType) \
   typedef char t_##cgpsType ## _inf[1+sizeof(cgpsType) - sizeof(externalType)]; \
   typedef char t_##externalType ## _sup[1+sizeof(externalType) - sizeof(cgpsType)];

#if __RTK_E__
MC_CGPS_STRUCT_CHECK_DECLARE(t_cgps_CStructNavData, s_GN_GPS_Nav_Data)

MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_LocalTTag, Local_TTag)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_OSTimeMs, OS_Time_ms)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Year, Year)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Month, Month)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Day, Day)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Hours, Hours)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Minutes, Minutes)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Seconds, Seconds)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Milliseconds, Milliseconds)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_GpsWeekNo, Gps_WeekNo)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_GpsTOW, Gps_TOW)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_UtcCorrection, UTC_Correction)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_X, X)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Y, Y)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Z, Z)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Latitude, Latitude)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Longitude, Longitude)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_AltitudeEll, Altitude_Ell)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_AltitudeMSL, Altitude_MSL)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SpeedOverGround, SpeedOverGround)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_CourseOverGround, CourseOverGround)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_VerticalVelocity, VerticalVelocity)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_NAccEst, N_AccEst)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_EAccEst, E_AccEst)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_VAccEst, V_AccEst)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_HAccMaj, H_AccMaj)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_HAccMin, H_AccMin)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_HAccMajBrg, H_AccMajBrg)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_HVelAccEst, HVel_AccEst)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_VVelAccEst, VVel_AccEst)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_COG_AccEst, COG_AccEst)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_PRResRMS, PR_ResRMS)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_HDOP, H_DOP)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_VDOP, V_DOP)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_PDOP, P_DOP)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_FixType, FixType)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Valid2DFix, Valid_2D_Fix)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Valid3DFix, Valid_3D_Fix)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_FixMode, FixMode)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInView, SatsInView)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsUsed, SatsUsed)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInViewConstell, SatsInViewConstell)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInViewSVId, SatsInViewSVid)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInViewSNR, SatsInViewSNR)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInViewJNR, SatsInViewJNR)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInViewAzim, SatsInViewAzim)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInViewElev, SatsInViewElev)
MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_SatsInViewUsed, SatsInViewUsed)
/* MC_CGPS_STRUCT_CHECK_FIELD(t_cgps_CStructNavData, s_GN_GPS_Nav_Data, v_Padding, Padding) */

/* MC_CGPS_STRUCT_CHECK_END(t_cgps_CStructNavData, s_GN_GPS_Nav_Data) */
#endif /*__RTK_E__*/
#undef __CGPS0MAIN_C__

