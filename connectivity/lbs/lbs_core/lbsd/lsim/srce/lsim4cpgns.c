/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#define LSIM4CPGNS_C

#ifdef AGPS_FTR

#include "lsimcp.h"


#ifdef AGPS_TIME_SYNCH_FTR
#include "lsimfta.h"
#endif

#undef  FILE_NUMBER
#define FILE_NUMBER     4

void lsim4_01AidRequest(t_lsimcp_GpsAidRequest *pp_GpsAidRequest )
{
    u_gnsCP_MsgData    v_MsgData;
    uint8_t                vl_Index;
    s_gnsCP_GpsAidRequest *pl_gnsCP_GpsAidRequest=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsCpGpsAidRequest.v_AidMask   = pp_GpsAidRequest->v_AidMask; //@todo : Map the masks from LSIMCP to GNS
    v_MsgData.v_GnsCpGpsAidRequest.v_GpsWeek   = pp_GpsAidRequest->v_GpsWeek;
    v_MsgData.v_GnsCpGpsAidRequest.v_GpsToe    = pp_GpsAidRequest->v_GpsTow;
    v_MsgData.v_GnsCpGpsAidRequest.v_TTOELimit = pp_GpsAidRequest->v_TTOELimit;

    v_MsgData.v_GnsCpGpsAidRequest.v_NrOfSats = AGPS_MIN(pp_GpsAidRequest->v_NrOfSats,K_gnsCP_MAX_SAT_NR);


    for ( vl_Index = 0; vl_Index < v_MsgData.v_GnsCpGpsAidRequest.v_NrOfSats ; vl_Index++ )
    {
        v_MsgData.v_GnsCpGpsAidRequest.a_SatID[vl_Index] = pp_GpsAidRequest->a_SatID[vl_Index];
        v_MsgData.v_GnsCpGpsAidRequest.a_Iode[vl_Index]  = pp_GpsAidRequest->a_Iode[vl_Index];
    }

    GNS_ExecuteCpCallback( E_gnsCP_AID_REQ , sizeof( *pl_gnsCP_GpsAidRequest ) , &v_MsgData  );
}

void lsim4_02MeasResultInd( uint8_t v_SessionId , uint8_t v_FinalReport, t_lsimcp_MsrRes *pp_MsrRes )
{
    u_gnsCP_MsgData    v_MsgData;
    s_gnsCP_GpsMeasurements *pl_gnsCP_GpsMeasurements=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsCPGpsMeasResult.v_SessionId             = v_SessionId;
    v_MsgData.v_GnsCPGpsMeasResult.v_FinalReport           = v_FinalReport;
    v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_Status = pp_MsrRes->v_Status; // @todo : Map values

    if( LSIMCP_CGPS_OK == pp_MsrRes->v_Status )
    {

        uint8_t    vl_Index;
        s_gnsCP_MsrElement    *pl_MsrElement;
        t_lsimcp_MsrElement    *pl_MsrElementIn;


        v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_GpsTow           = pp_MsrRes->v_GpsTow;
        v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_GpsTowUnc        = pp_MsrRes->v_GpsTowUnc;
        v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_DeltaTow         = pp_MsrRes->v_Delta_TOW;
		v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_RefDeltaCellTime = pp_MsrRes->v_RefDeltaCellTime;
        v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_NrOfSats         = AGPS_MIN( pp_MsrRes->v_NrOfSats , K_gnsCP_MAX_SAT_NR );

        pl_MsrElement = v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.a_MsrElement;
        pl_MsrElementIn = pp_MsrRes->a_MsrElement;
#ifdef AGPS_TIME_SYNCH_FTR
        if( v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_RefDeltaCellTime != LSIMCP_S32_UNKNOWN)
        {
            lsim7_10FtaFillCellTimeInMeasRes(&v_MsgData.v_GnsCPGpsMeasResult);
        }
        else
        {
            v_MsgData.v_GnsCPGpsMeasResult.v_CellTimeType = E_gns_RAT_TYPE_NONE;
            memset(&v_MsgData.v_GnsCPGpsMeasResult.v_CellTime,0,sizeof(v_MsgData.v_GnsCPGpsMeasResult.v_CellTime));
        }
#endif /*AGPS_TIME_SYNCH_FTR*/

        for( vl_Index = 0 ; vl_Index < v_MsgData.v_GnsCPGpsMeasResult.v_Measurements.v_NrOfSats ; vl_Index++ , pl_MsrElement++ , pl_MsrElementIn++ )
        {
            pl_MsrElement->v_SatId         =    pl_MsrElementIn->v_SatId;
            pl_MsrElement->v_CNo           =    pl_MsrElementIn->v_CNo;
            pl_MsrElement->v_Doppler       =    pl_MsrElementIn->v_Doppler;
            pl_MsrElement->v_WholeChips    =    pl_MsrElementIn->v_WholeChips;
            pl_MsrElement->v_FracChips     =    pl_MsrElementIn->v_FracChips;
            pl_MsrElement->v_MpathIndic    =    pl_MsrElementIn->v_MpathIndic;
            pl_MsrElement->v_PseudoRmsErr  =    pl_MsrElementIn->v_PseudoRmsErr;
        }
    }
    /*Fine Time : clearing stored FTA infromation as session reults is sent  */
#ifdef AGPS_TIME_SYNCH_FTR
      lsim7_08FtaClearCellTimeInfo();
#endif /*AGPS_TIME_SYNCH_FTR*/

    GNS_ExecuteCpCallback( E_gnsCP_MEAS_RESULT_IND, sizeof( *pl_gnsCP_GpsMeasurements ) , &v_MsgData  );
}

void lsim4_03PosnResultInd(uint8_t v_SessionId , uint8_t v_FinalReport, t_lsimcp_GpsPosition *pp_GpsPosition )
{
    u_gnsCP_MsgData    v_MsgData;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );
    s_gnsCP_GpsPosition  *pl_gnsCP_GpsPosition=NULL;

    v_MsgData.v_GnsCPGpsPosnResult.v_SessionId         = v_SessionId;
    v_MsgData.v_GnsCPGpsPosnResult.v_FinalReport       = v_FinalReport;
    v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_Status = pp_GpsPosition->v_Status; // @todo : Map values

    if( LSIMCP_CGPS_OK == pp_GpsPosition->v_Status )
    {
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_FixType          =  pp_GpsPosition->v_FixType;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_ShapeType        =  pp_GpsPosition->v_ShapeType;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_SignOfLat        =  pp_GpsPosition->v_SignOfLat;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_GpsTow           =  pp_GpsPosition->v_GpsTow;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_GpsTowUnc        =  pp_GpsPosition->v_GpsTowUnc;
		v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_GpsTowSubms      =  pp_GpsPosition->v_GpsTowSubms;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_RefDeltaCellTime =  pp_GpsPosition->v_RefDeltaCellTime;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_Latitude         =  pp_GpsPosition->v_Latitude;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_Longitude        =  pp_GpsPosition->v_Longitude;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_Altitude         =  pp_GpsPosition->v_Altitude;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_UncertSemiMajor  =  pp_GpsPosition->v_UncertSemiMajor;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_UncertSemiMinor  =  pp_GpsPosition->v_UncertSemiMinor;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_AxisBearing      =  pp_GpsPosition->v_AxisAngle;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_DirectOfAlt      =  pp_GpsPosition->v_DirectOfAlt;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_UncertAltitude   =  pp_GpsPosition->v_UncertAltitude;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_Confidence       =  pp_GpsPosition->v_Confidence;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_IncludedAngle    =  pp_GpsPosition->v_IncludedAngle;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_OffsetAngle      =  pp_GpsPosition->v_OffsetAngle;
        v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_InnerRadius      =  pp_GpsPosition->v_InnerRadius;
#ifdef AGPS_TIME_SYNCH_FTR
       if( v_MsgData.v_GnsCPGpsPosnResult.v_Position.v_RefDeltaCellTime != LSIMCP_S32_UNKNOWN)
        {
            lsim7_11FtaFillCellTimeInPosRes(&v_MsgData.v_GnsCPGpsPosnResult);
        }
        else
        {
            v_MsgData.v_GnsCPGpsPosnResult.v_CellTimeType = E_gns_RAT_TYPE_NONE;
            memset(&v_MsgData.v_GnsCPGpsPosnResult.v_CellTime,0,sizeof(v_MsgData.v_GnsCPGpsPosnResult.v_CellTime));
        }
#endif /*AGPS_TIME_SYNCH_FTR*/
    }
    /*Fine Time : clearing stored FTA infromation as session reults is sent  */
#ifdef AGPS_TIME_SYNCH_FTR
    lsim7_08FtaClearCellTimeInfo();
#endif

    GNS_ExecuteCpCallback( E_gnsCP_POSN_RESULT_IND, sizeof( *pl_gnsCP_GpsPosition ) , &v_MsgData  );
}

void lsim4_04PosInstructInd( uint8_t vp_SessionId , s_gnsCP_PosInstruct * pp_PosInstruct )
{

    t_lsimcp_PosInstruct  vl_PosInstruct;
    uint32_t                   vl_PeriodicInterval = 0;

/*  +LMSqc34406 */
    uint8_t                    vl_ReportingAmount  = 0;
/*  -LMSqc34406 */
    vl_PosInstruct.v_OptField = 0;
    vl_PosInstruct.v_FillerU16 = 0;

    if( pp_PosInstruct->v_OptField & E_gnsCP_POS_INSTRUCT_HORIZONTAL_ACCURACY )
    {
        vl_PosInstruct.v_HorAccuracy = pp_PosInstruct->v_HorAccuracy;
        vl_PosInstruct.v_OptField |= LSIMCP_HORIZONTAL_ACCURACY;
    }
    else
    {
        vl_PosInstruct.v_HorAccuracy = LSIMCP_U8_UNKNOWN;
    }

    if( pp_PosInstruct->v_OptField & E_gnsCP_POS_INSTRUCT_VERTICAL_ACCURACY )
    {
        vl_PosInstruct.v_VertAccuracy = pp_PosInstruct->v_VertAccuracy;
        vl_PosInstruct.v_OptField |= LSIMCP_VERTICAL_ACCURACY;
    }
    else
    {
        vl_PosInstruct.v_VertAccuracy = LSIMCP_U8_UNKNOWN;
    }


    vl_PosInstruct.v_Timeout = ( pp_PosInstruct->v_OptField & E_gnsCP_POS_INSTRUCT_TIMEOUT ) ? pp_PosInstruct->v_Timeout : 0;

    vl_PosInstruct.v_EnvChar = LSIMCP_U8_UNKNOWN;

    vl_PeriodicInterval = ( pp_PosInstruct->v_OptField & E_gnsCP_POS_INSTRUCT_PERIODIC_INTERVAL ) ? pp_PosInstruct->v_PeriodicInterval : 0;

/*  +LMSqc34406 */
    vl_ReportingAmount = ( pp_PosInstruct->v_OptField & E_gnsCP_POS_INSTRUCT_AMOUNT_OF_REPORT ) ? pp_PosInstruct->v_Amount : 0;
/*  -LMSqc34406 */
    if( pp_PosInstruct->v_PosMethod == E_gnsCP_MS_BASED )
    {
        t_lsimcp_MSBasedReqM* pl_MSBasedReqM = NULL;

        pl_MSBasedReqM = (t_lsimcp_MSBasedReqM*)MC_RTK_GET_MEMORY(sizeof(* pl_MSBasedReqM));

        pl_MSBasedReqM->v_PeriodicInterval = vl_PeriodicInterval;
        memcpy( &(pl_MSBasedReqM->v_PosInstruct), &vl_PosInstruct, sizeof(vl_PosInstruct));
        pl_MSBasedReqM->v_SessionID          = vp_SessionId;
        pl_MSBasedReqM->v_NoReporting        = (pp_PosInstruct->v_PosReportType==E_gnsCP_NO_REPORT)?TRUE:FALSE;
        pl_MSBasedReqM->v_AssistanceReqGrant = pp_PosInstruct->v_AssistanceReqGrant;
/*  +LMSqc34406 */
        pl_MSBasedReqM->v_ReportingAmount = vl_ReportingAmount;
/*  -LMSqc34406 */

        DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_MS_BASED_REQ --> CGPS"));

        MC_RTK_SEND_MSG_TO_PROCESS( PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_MS_BASED_REQ,
                              (t_MsgHeader *)pl_MSBasedReqM);

    }
    else
    {
        t_lsimcp_MSAssistedReqM* pl_MSAssistedReqM = NULL;

        pl_MSAssistedReqM = (t_lsimcp_MSAssistedReqM*)MC_RTK_GET_MEMORY(sizeof(* pl_MSAssistedReqM));

        pl_MSAssistedReqM->v_PeriodicInterval = vl_PeriodicInterval;
        memcpy( &(pl_MSAssistedReqM->v_PosInstruct), &vl_PosInstruct, sizeof(vl_PosInstruct));
        pl_MSAssistedReqM->v_SessionID        = vp_SessionId;
        pl_MSAssistedReqM->v_NoReporting = (pp_PosInstruct->v_PosReportType==E_gnsCP_NO_REPORT)?TRUE:FALSE;
        pl_MSAssistedReqM->v_AssistanceReqGrant = pp_PosInstruct->v_AssistanceReqGrant;
/*  +LMSqc34406 */
        pl_MSAssistedReqM->v_ReportingAmount = vl_ReportingAmount;
/*  -LMSqc34406 */

        DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_MS_ASSISTED_REQ --> CGPS"));

        MC_RTK_SEND_MSG_TO_PROCESS( PROCESS_LSIMCP, 0,
                                  PROCESS_CGPS, 0,
                                  CGPS_LSIMCP_MS_ASSISTED_REQ,
                                  (t_MsgHeader *)pl_MSAssistedReqM);
    }
}

void lsim4_05NavModelElmInd( uint8_t vp_NumSv ,   s_gnsCP_NavModelElm *pp_NavModelElm )
{
    t_lsimcp_AssistDataBReqM    *pl_AssistDataBReqM = NULL;
    t_lsimcp_SatEphemData        *pl_SatEph = NULL;
    uint8_t                            vl_Index = 0;

    pl_AssistDataBReqM = (t_lsimcp_AssistDataBReqM*)MC_RTK_GET_MEMORY (sizeof(*pl_AssistDataBReqM));

    vp_NumSv = AGPS_MIN(vp_NumSv , LSIMCP_MAX_SAT_NR);

    pl_AssistDataBReqM->v_AssistBData.s_NavModel.v_NrOfSats = vp_NumSv;
    pl_SatEph = pl_AssistDataBReqM->v_AssistBData.s_NavModel.a_NavData;

    for(vl_Index = 0 ; vl_Index < vp_NumSv ; vl_Index++ , pl_SatEph++ , pp_NavModelElm++ )
    {
        pl_SatEph->v_SatId           =    pp_NavModelElm->v_SatId;
        pl_SatEph->v_ExtBit          =    0;
        pl_SatEph->v_SatStatus       =    pp_NavModelElm->v_SatStatus;    //@todo : not correct
        pl_SatEph->v_EphemCodeOnL2   =    pp_NavModelElm->v_EphemCodeOnL2;
        pl_SatEph->v_EphemURA        =    pp_NavModelElm->v_EphemURA;
        pl_SatEph->v_EphemSvHealth   =    pp_NavModelElm->v_EphemSvHealth;
        pl_SatEph->v_Epheml2pFlag    =    pp_NavModelElm->v_Epheml2pFlag;
        pl_SatEph->v_EphemIodc       =    pp_NavModelElm->v_EphemIodc;
        pl_SatEph->v_EphemToc        =    pp_NavModelElm->v_EphemToc;
        pl_SatEph->v_Reserved1       =    pp_NavModelElm->v_Reserved1;
        pl_SatEph->v_Reserved2       =    pp_NavModelElm->v_Reserved2;
        pl_SatEph->v_Reserved3       =    pp_NavModelElm->v_Reserved3;
        pl_SatEph->v_Reserved4       =    pp_NavModelElm->v_Reserved4;
        pl_SatEph->v_EphemTgd        =    pp_NavModelElm->v_EphemTgd;
        pl_SatEph->v_EphemAf2        =    pp_NavModelElm->v_EphemAf2;
        pl_SatEph->v_EphemAf1        =    pp_NavModelElm->v_EphemAf1;
        pl_SatEph->v_EphemAf0        =    pp_NavModelElm->v_EphemAf0;
        pl_SatEph->v_EphemCrs        =    pp_NavModelElm->v_EphemCrs;
        pl_SatEph->v_EphemDeltaN     =    pp_NavModelElm->v_EphemDeltaN;
        pl_SatEph->v_EphemMo         =    pp_NavModelElm->v_EphemMo;
        pl_SatEph->v_EphemCuc        =    pp_NavModelElm->v_EphemCuc;
        pl_SatEph->v_EphemCus        =    pp_NavModelElm->v_EphemCus;
        pl_SatEph->v_Epheme          =    pp_NavModelElm->v_Epheme;
        pl_SatEph->v_EphemAPowerhalf =    pp_NavModelElm->v_EphemAPowerhalf;
        pl_SatEph->v_EphemToe        =    pp_NavModelElm->v_EphemToe;
        pl_SatEph->v_EphemFitFlag    =    pp_NavModelElm->v_EphemFitFlag;
        pl_SatEph->v_EphemAODO       =    pp_NavModelElm->v_EphemAODO;
        pl_SatEph->v_EphemCic        =    pp_NavModelElm->v_EphemCic;
        pl_SatEph->v_EphemCis        =    pp_NavModelElm->v_EphemCis;
        pl_SatEph->v_EphemOmegaA0    =    pp_NavModelElm->v_EphemOmegaA0;
        pl_SatEph->v_EphemIo         =    pp_NavModelElm->v_EphemIo;
        pl_SatEph->v_EphemIdot       =    pp_NavModelElm->v_EphemIdot;
        pl_SatEph->v_EphemCrc        =    pp_NavModelElm->v_EphemCrc;
        pl_SatEph->v_EphemW          =    pp_NavModelElm->v_EphemW;
        pl_SatEph->v_EphemOmegaAdot  =    pp_NavModelElm->v_EphemOmegaAdot;
    }

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ASSIST_DATA_B_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                                  PROCESS_CGPS, 0,
                                  CGPS_LSIMCP_ASSIST_DATA_B_REQ,
                                  (t_MsgHeader *)pl_AssistDataBReqM);
}

void lsim4_06AcquisAssistInd( s_gnsCP_AcquisAssist *pp_AcquisAssist )
{
    t_lsimcp_AssistDataAReqM *pl_AssistDataAReqM = NULL;
    t_lsimcp_AcquisList      *pl_AcquisList=NULL;
    t_lsimcp_AcquisData         *pl_AcquisAssist=NULL;
    s_gnsCP_AcquisElement     *pl_AcquisAssistIn=NULL;
    uint8_t                          vl_Index;

    pl_AssistDataAReqM = (t_lsimcp_AssistDataAReqM*)MC_RTK_GET_MEMORY (sizeof(*pl_AssistDataAReqM));

    pl_AcquisList = &( pl_AssistDataAReqM->v_AssistAData.s_AcquisAssist );

    pl_AcquisList->v_GpsTow = pp_AcquisAssist->v_GpsTow;
    pl_AcquisList->v_Prec = pp_AcquisAssist->v_GpsTowUnc;

    pl_AcquisList->v_RefDeltaCellTime = pp_AcquisAssist->v_RefDeltaCellTime;

    pl_AcquisList->v_NrOfSats = AGPS_MIN(pp_AcquisAssist->v_NrOfSats , LSIMCP_MAX_SAT_NR );

    pl_AcquisAssist = pl_AcquisList->a_AcquisElement;
    pl_AcquisAssistIn = pp_AcquisAssist->a_AcqElement;

    for( vl_Index = 0 ; vl_Index < pl_AcquisList->v_NrOfSats ; vl_Index++,pl_AcquisAssist++,pl_AcquisAssistIn++)
    {
        pl_AcquisAssist->v_SatId         =    pl_AcquisAssistIn->v_SatId;
        pl_AcquisAssist->v_Doppler1      =    pl_AcquisAssistIn->v_Doppler1;
        pl_AcquisAssist->v_Doppler0      =    pl_AcquisAssistIn->v_Doppler0;
        pl_AcquisAssist->v_DopplerUncert =    pl_AcquisAssistIn->v_DopplerUncert;
        pl_AcquisAssist->v_IntCodePhase  =    pl_AcquisAssistIn->v_IntCodePhase;
        pl_AcquisAssist->v_CodePhase     =    pl_AcquisAssistIn->v_CodePhase;
        pl_AcquisAssist->v_GpsBitNumber  =    pl_AcquisAssistIn->v_GpsBitNumber;
        pl_AcquisAssist->v_SearchWindow  =    pl_AcquisAssistIn->v_SearchWindow;
        pl_AcquisAssist->v_Azimuth       =    pl_AcquisAssistIn->v_Azimuth;
        pl_AcquisAssist->v_Elevation     =    pl_AcquisAssistIn->v_Elevation;
    }

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ASSIST_DATA_A_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_ASSIST_DATA_A_REQ,
                              (t_MsgHeader *)pl_AssistDataAReqM);


}

void lsim4_07AlmanacElm( uint8_t vp_NumSv , s_gnsCP_AlmanacElm *pp_AlmanacElm , s_gnsCP_GlobalHealth *pp_GlobalHealth )
{
    t_lsimcp_AlmanacData          *pl_AlmanacData=NULL;
    t_lsimcp_AlmanacDataReqM    *pl_AlmanacDataReqM = NULL;
    t_lsimcp_AlmanacElement        *pl_AlmanacElement=NULL;
    uint8_t                            vl_Index = 0;

    pl_AlmanacDataReqM = (t_lsimcp_AlmanacDataReqM*)MC_RTK_GET_MEMORY (sizeof(*pl_AlmanacDataReqM));

    pl_AlmanacData = &( pl_AlmanacDataReqM->v_AlmanacData );
    pl_AlmanacData->v_AlmanacWNA        = pp_AlmanacElm->v_AlmanacWNA; /* We copy the first element's WeekNum to global */

    if( NULL == pp_GlobalHealth )
    {
        pl_AlmanacData->v_SvGlobalHeathFlag = FALSE;
    }
    else
    {
        uint8_t    vl_NumSvsToCopy = AGPS_MIN(LSIMCP_SV_HEALTH_LENGTH,K_gnsCP_SV_HEALTH_LENGTH);
        pl_AlmanacData->v_SvGlobalHeathFlag = TRUE;
        memcpy( pl_AlmanacData->a_SvGlobalHealth , pp_GlobalHealth->a_SvGlobalHealth , vl_NumSvsToCopy );
    }

    vp_NumSv = AGPS_MIN(vp_NumSv , LSIMCP_MAX_ALM_SAT_NR);
    pl_AlmanacData->v_NrOfSats          = vp_NumSv;

    pl_AlmanacElement = pl_AlmanacData->a_AlmanacList;

    for( vl_Index = 0 ; vl_Index < vp_NumSv ;vl_Index++, pp_AlmanacElm++ , pl_AlmanacElement++ )
    {
        pl_AlmanacElement->v_SatId            =   pp_AlmanacElm->v_SatId;
        pl_AlmanacElement->v_DataId           =   pp_AlmanacElm->v_DataId;

        pl_AlmanacElement->v_AlmanacE         =   pp_AlmanacElm->v_AlmanacE;
        pl_AlmanacElement->v_AlmanacToa       =   pp_AlmanacElm->v_AlmanacToa;
        pl_AlmanacElement->v_AlmanacSVhealth  =   pp_AlmanacElm->v_AlmanacSVhealth;
        pl_AlmanacElement->v_AlmanacDeltai    =   pp_AlmanacElm->v_AlmanacDeltai;

        pl_AlmanacElement->v_AlmanacAsqrt     =   pp_AlmanacElm->v_AlmanacAsqrt;
        pl_AlmanacElement->v_AlmanacOmega0    =   pp_AlmanacElm->v_AlmanacOmega0;
        pl_AlmanacElement->v_AlmanacOmega     =   pp_AlmanacElm->v_AlmanacOmega;
        pl_AlmanacElement->v_AlmanacM0        =   pp_AlmanacElm->v_AlmanacM0;
        pl_AlmanacElement->v_AlmanacOmegaDot  =   pp_AlmanacElm->v_AlmanacOmegaDot;
        pl_AlmanacElement->v_AlmanacAF0       =   pp_AlmanacElm->v_AlmanacAF0;
        pl_AlmanacElement->v_AlmanacAF1       =   pp_AlmanacElm->v_AlmanacAF1;
    }

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ALMANAC_DATA_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_ALMANAC_DATA_REQ,
                              (t_MsgHeader *)pl_AlmanacDataReqM);

}

void lsim4_08RefTimeInd(e_gnsAS_AssistSrc v_Src, s_gnsCP_RefTime *pp_RefTime )
{

    t_lsimcp_TimeReqM    *pl_TimeReqM = NULL;
    t_lsimcp_TowAssist    *pl_TowAssist=NULL;
    s_gnsCP_TowAssist    *pl_TowAssistIn=NULL;
    uint8_t                    vl_Index,vl_MaxSVs;

    pl_TimeReqM = (t_lsimcp_TimeReqM*)MC_RTK_GET_MEMORY (sizeof(*pl_TimeReqM));

    pl_TimeReqM->v_RefTime.v_GpsTow             =   pp_RefTime->v_GpsTow;
    pl_TimeReqM->v_RefTime.v_Prec               =   pp_RefTime->v_RefCellTimeUnc;
    pl_TimeReqM->v_RefTime.v_RefDeltaCellTime   =   pp_RefTime->v_RefDeltaCellTime;
    pl_TimeReqM->v_RefTime.v_GpsWeek            =   pp_RefTime->v_GpsWeek;
    pl_TimeReqM->v_RefTime.v_SfnTowUnc          =   pp_RefTime->v_GpsTowUnc ;
    pl_TimeReqM->v_RefTime.v_UtranGpsDriftRate  =   pp_RefTime->v_UtranGpsDriftRate;
#ifdef AGPS_TIME_SYNCH_FTR
    if(pp_RefTime->v_CellTimeType != LSIMCP_NO_CELL_TIME)
    {
    lsim7_09FtaUpdateRefTime(&pl_TimeReqM->v_RefTime);
    lsim7_04FtaStoreCelltime(pp_RefTime->v_CellTimeType,&pp_RefTime->v_CellTime);
    }
#endif /*AGPS_TIME_SYNCH_FTR*/

    vl_MaxSVs = AGPS_MIN( pp_RefTime->v_NumTowAssist , LSIMCP_MAX_SAT_NR ) ;

    pl_TimeReqM->v_RefTime.v_NumTowAssist  =  vl_MaxSVs;

    pl_TowAssist = pl_TimeReqM->v_RefTime.a_TowAssist;
    pl_TowAssistIn = pp_RefTime->a_TowAssist;

    for( vl_Index = 0 ; vl_Index <  vl_MaxSVs ; vl_Index++ ,pl_TowAssist++,pl_TowAssistIn++ )
    {
        pl_TowAssist->v_Satid      =   pl_TowAssistIn->v_Satid;
        pl_TowAssist->v_AntiSpoof  =   pl_TowAssistIn->v_AntiSpoof;
        pl_TowAssist->v_Alert      =   pl_TowAssistIn->v_Alert;
        pl_TowAssist->v_TlmRes     =   pl_TowAssistIn->v_TlmRes;
        pl_TowAssist->v_TlmWord    =   pl_TowAssistIn->v_TlmWord;
    }
    pl_TimeReqM->v_Src = v_Src;

    DEBUG_LOG_PRINT_LEV2(("TOW %d, Wk%d", pl_TimeReqM->v_RefTime.v_GpsTow, pl_TimeReqM->v_RefTime.v_GpsWeek));
    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_TIME_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_TIME_REQ,
                              (t_MsgHeader *) pl_TimeReqM);

}

void lsim4_09RefPositionInd( s_gnsCP_GpsPosition *pp_RefPosition )
{
    t_lsimcp_RefPosReqM* pl_RefPosReqM = NULL;

    pl_RefPosReqM = (t_lsimcp_RefPosReqM*)MC_RTK_GET_MEMORY (sizeof(* pl_RefPosReqM));

    pl_RefPosReqM->v_GpsPosition.v_Status           =              pp_RefPosition->v_Status;
    pl_RefPosReqM->v_GpsPosition.v_FixType          =             pp_RefPosition->v_FixType;
    pl_RefPosReqM->v_GpsPosition.v_RefFrame         =             (uint16_t)LSIMCP_U16_UNKNOWN;
    pl_RefPosReqM->v_GpsPosition.v_RefDeltaCellTime =     pp_RefPosition->v_RefDeltaCellTime;
    pl_RefPosReqM->v_GpsPosition.v_GpsTow           =              pp_RefPosition->v_GpsTow;
    pl_RefPosReqM->v_GpsPosition.v_Latitude         =             pp_RefPosition->v_Latitude;
        /* Latitude is only 23 bits long. Set all the other bits to zero */
    pl_RefPosReqM->v_GpsPosition.v_Latitude &= 0x7FFFFF;
    pl_RefPosReqM->v_GpsPosition.v_Longitude        =             pp_RefPosition->v_Longitude;
    /* Longitude requires sign extension based on bit 23 */
    if( pl_RefPosReqM->v_GpsPosition.v_Longitude & 0x800000 )
    {
        /* if 23th bit is 1, set bit 31-24 as 1*/
        pl_RefPosReqM->v_GpsPosition.v_Longitude |= 0xFF000000;
    }
    else
    {
        pl_RefPosReqM->v_GpsPosition.v_Longitude &= 0xFFFFFF;
    }
    pl_RefPosReqM->v_GpsPosition.v_ShapeType          =     pp_RefPosition->v_ShapeType;
    pl_RefPosReqM->v_GpsPosition.v_SignOfLat          =     pp_RefPosition->v_SignOfLat;
    pl_RefPosReqM->v_GpsPosition.v_UncertSemiMajor    =     pp_RefPosition->v_UncertSemiMajor;
    pl_RefPosReqM->v_GpsPosition.v_UncertSemiMinor    =     pp_RefPosition->v_UncertSemiMinor;
    pl_RefPosReqM->v_GpsPosition.v_Altitude           =     pp_RefPosition->v_Altitude;
    pl_RefPosReqM->v_GpsPosition.v_InnerRadius        =     pp_RefPosition->v_InnerRadius;
    pl_RefPosReqM->v_GpsPosition.v_AxisAngle          =     LSIMCP_U8_UNKNOWN;
    pl_RefPosReqM->v_GpsPosition.v_DirectOfAlt        =     pp_RefPosition->v_DirectOfAlt;
    pl_RefPosReqM->v_GpsPosition.v_UncertAltitude     =     pp_RefPosition->v_UncertAltitude;
    pl_RefPosReqM->v_GpsPosition.v_Confidence         =     pp_RefPosition->v_Confidence;
    pl_RefPosReqM->v_GpsPosition.v_IncludedAngle      =     pp_RefPosition->v_IncludedAngle;
    pl_RefPosReqM->v_GpsPosition.v_OffsetAngle        =     pp_RefPosition->v_OffsetAngle;
    pl_RefPosReqM->v_GpsPosition.v_PolygonNumOfPoints =  0;

    /* We ignore fields a_PolygonSignOfLat , a_PolygonLatitude and a_PolygonLongitude */

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_REF_POS_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                                PROCESS_CGPS, 0,
                                  CGPS_LSIMCP_REF_POS_REQ,
                                  (t_MsgHeader *)pl_RefPosReqM);
}

void lsim4_10IonoModelInd( s_gnsCP_IonoModel *pp_IonoModel )
{
    t_lsimcp_AssistIonoModelReqM* pl_AssistIonoModelReqM = NULL;

    pl_AssistIonoModelReqM = (t_lsimcp_AssistIonoModelReqM*)MC_RTK_GET_MEMORY (sizeof(* pl_AssistIonoModelReqM));

    pl_AssistIonoModelReqM->v_AssistIono.v_Alpha0 =   pp_IonoModel->v_Alpha0;
    pl_AssistIonoModelReqM->v_AssistIono.v_Alpha1 =   pp_IonoModel->v_Alpha1;
    pl_AssistIonoModelReqM->v_AssistIono.v_Alpha2 =   pp_IonoModel->v_Alpha2;
    pl_AssistIonoModelReqM->v_AssistIono.v_Alpha3 =   pp_IonoModel->v_Alpha3;
    pl_AssistIonoModelReqM->v_AssistIono.v_Beta0  =   pp_IonoModel->v_Beta0;
    pl_AssistIonoModelReqM->v_AssistIono.v_Beta1  =   pp_IonoModel->v_Beta1;
    pl_AssistIonoModelReqM->v_AssistIono.v_Beta2  =   pp_IonoModel->v_Beta2;
    pl_AssistIonoModelReqM->v_AssistIono.v_Beta3  =   pp_IonoModel->v_Beta3;

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ASSIST_IONO_MODEL_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_ASSIST_IONO_MODEL_REQ,
                              (t_MsgHeader *)pl_AssistIonoModelReqM);
}

void lsim4_11UtcModelInd( s_gnsCP_UtcModel *pp_UtcModel )
{
    t_lsimcp_AssistUtcModelReqM* pl_AssistUtcModelReqM = NULL;

    pl_AssistUtcModelReqM = (t_lsimcp_AssistUtcModelReqM*)MC_RTK_GET_MEMORY (sizeof(* pl_AssistUtcModelReqM ));

    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcA1        =   pp_UtcModel->v_UtcA1;
    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcA0        =   pp_UtcModel->v_UtcA0;
    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcTot       =   pp_UtcModel->v_UtcTot;
    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcWnT       =   pp_UtcModel->v_UtcWnT;
    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcDeltaTLs  =   pp_UtcModel->v_UtcDeltaTLs;
    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcWnLsf     =   pp_UtcModel->v_UtcWnLsf;
    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcDn        =   pp_UtcModel->v_UtcDn;
    pl_AssistUtcModelReqM->v_AssistUtcm.v_UtcDeltaTLsf =   pp_UtcModel->v_UtcDeltaTLsf;

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ASSIST_UTC_MODEL_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_ASSIST_UTC_MODEL_REQ,
                              (t_MsgHeader *)pl_AssistUtcModelReqM);
}

void lsim4_12RtIntegrityInd( s_gnsCP_RTIntegrity *pp_RtIntegrity )
{
    t_lsimcp_AssistRtiReqM    *pl_AssistRtiReqM = NULL;
    uint8_t                        vl_Index;

    pl_AssistRtiReqM = (t_lsimcp_AssistRtiReqM*)MC_RTK_GET_MEMORY (sizeof(*pl_AssistRtiReqM));

    pl_AssistRtiReqM->v_AssistRti.v_NrOfSats = AGPS_MIN( pp_RtIntegrity->v_NrOfSats , LSIMCP_MAX_SAT_NR );

    for( vl_Index = 0 ; vl_Index < pl_AssistRtiReqM->v_AssistRti.v_NrOfSats ; vl_Index++ )
    {
        pl_AssistRtiReqM->v_AssistRti.a_RtiList[vl_Index].v_BadSatId
            = pp_RtIntegrity->a_BadSatId[vl_Index];
    }

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ASSIST_RTI_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                          PROCESS_CGPS, 0,
                          CGPS_LSIMCP_ASSIST_RTI_REQ,
                          (t_MsgHeader *)pl_AssistRtiReqM);
}

void lsim4_13DgpsCorrectionsInd( s_gnsCP_DGpsCorr *pp_DgpsCorr )
{
    t_lsimcp_AssistDgpsCorr                *pl_AssistDgpsCorr=NULL;
    t_lsimcp_AssistDgpsCorrectionReqM    *pl_AssistDgpsCorrectionReqM = NULL;
    t_lsimcp_SatDgpsData                *pl_SatDgpsData=NULL;
    s_gnsCP_SatDgpsData                    *pl_SatDgpsDataIn=NULL;
    uint8_t                                    vl_Index;

    pl_AssistDgpsCorrectionReqM = (t_lsimcp_AssistDgpsCorrectionReqM*)MC_RTK_GET_MEMORY (sizeof(*pl_AssistDgpsCorrectionReqM));

    pl_AssistDgpsCorr = &(pl_AssistDgpsCorrectionReqM->v_AssistDgpsCorr);

    pl_AssistDgpsCorr->v_GpsTow = pp_DgpsCorr->v_GpsTow;
    pl_AssistDgpsCorr->v_Health = pp_DgpsCorr->v_Health;

    pl_AssistDgpsCorr->v_NrOfSats = AGPS_MIN( pp_DgpsCorr->v_NrOfSats , LSIMCP_MAX_SAT_NR );

    pl_SatDgpsData = pl_AssistDgpsCorr->a_DgpsList;
    pl_SatDgpsDataIn = pp_DgpsCorr->a_DgpsList;

    for( vl_Index = 0 ; vl_Index < pl_AssistDgpsCorr->v_NrOfSats ; vl_Index++ , pl_SatDgpsData++ , pl_SatDgpsDataIn++ )
    {
        pl_SatDgpsData->v_SatId =  pl_SatDgpsDataIn->v_SatId;
        pl_SatDgpsData->v_Iode  =  pl_SatDgpsDataIn->v_Iode;
        pl_SatDgpsData->v_Urde  =  pl_SatDgpsDataIn->v_Udre;
        pl_SatDgpsData->v_Rrc   =  pl_SatDgpsDataIn->v_Rrc;
        pl_SatDgpsData->v_Prc   =  pl_SatDgpsDataIn->v_Prc;
    }

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ASSIST_DGPS_CORRECTION_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                               PROCESS_CGPS,   0,
                               CGPS_LSIMCP_ASSIST_DGPS_CORRECTION_REQ,
                              (t_MsgHeader *)pl_AssistDgpsCorrectionReqM);
}


void lsim4_14AbortInd( s_gnsCP_Abort *pp_Abort )
{
        t_lsimcp_AbortReqM* pl_AbortReqM = NULL;

        pl_AbortReqM = (t_lsimcp_AbortReqM*)MC_RTK_GET_MEMORY(sizeof(* pl_AbortReqM));

        pl_AbortReqM -> v_SessionID = pp_Abort->v_SessionId;

        DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_ABORT_REQ --> CGPS"));
       /*Fine Time : clearing stored FTA infromation as session reults is sent  */
#ifdef AGPS_TIME_SYNCH_FTR
        lsim7_08FtaClearCellTimeInfo();
#endif /*AGPS_TIME_SYNCH_FTR*/

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_ABORT_REQ,
                              (t_MsgHeader *)pl_AbortReqM);
}

void lsim4_15AbortCnf( uint8_t v_SessionId )
{
    u_gnsCP_MsgData    v_MsgData;
    s_gnsCP_GpsAbortCnf *pl_gnsCP_GpsAbortCnf=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsCPGpsAbortCnf.v_SessionId = v_SessionId;

    GNS_ExecuteCpCallback( E_gnsCP_ABORT_CNF, sizeof( *pl_gnsCP_GpsAbortCnf) , &v_MsgData  );
}

void lsim4_16SendLocNotifyReq( uint32_t vp_SessionId , s_gnsCP_LocNotification *pp_Notification )
{
    t_lsimcp_LocNotifyReqM *pl_LocNotifyReqM;

    s_gnsCP_LocNotification *pl_gnsCP_LocNotification=NULL;
    pl_LocNotifyReqM = (t_lsimcp_LocNotifyReqM *)MC_RTK_GET_MEMORY(sizeof(*pl_LocNotifyReqM));

    pl_LocNotifyReqM->v_SsSessionID = vp_SessionId;

    memcpy( &pl_LocNotifyReqM->v_LocNotification , pp_Notification , sizeof(*pl_gnsCP_LocNotification) );

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_LOC_NOTIFY_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                               PROCESS_CGPS, 0,
                               CGPS_LSIMCP_LOC_NOTIFY_REQ,
                               pl_LocNotifyReqM);

}

void lsim4_17SendLocNotifyRsp( uint32_t v_SessionId , uint8_t v_UserResponse )
{
    u_gnsCP_MsgData    v_MsgData;
    s_gnsCP_GpsNotifyRsp  *pl_gnsCP_GpsNotifyRsp=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsCpGpsNotifyRsp.v_SessionId    = v_SessionId;
    v_MsgData.v_GnsCpGpsNotifyRsp.v_UserResponse = v_UserResponse;

    GNS_ExecuteCpCallback( E_gnsCP_NOTIFY_RSP, sizeof( *pl_gnsCP_GpsNotifyRsp) , &v_MsgData  );
}


void lsim4_18SendResetGpsDataReq ( void )
{
    t_lsimcp_ResetGpsDataReqM* pl_ResetGpsDataReq = NULL;

    pl_ResetGpsDataReq = (t_lsimcp_ResetGpsDataReqM*)MC_RTK_GET_MEMORY (sizeof(* pl_ResetGpsDataReq));

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_RESET_GPS_DATA_REQ --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_RESET_GPS_DATA_REQ,
                              (t_MsgHeader *)pl_ResetGpsDataReq);

    return;
}


/* +LMSqcMOLR */
void lsim4_19SendMolrAbortReq (uint8_t vp_SessionId,uint8_t vp_ErrorReason, uint8_t v_MolrType)
{
    t_lsimcp_MolrAbortReqM* pl_Req = NULL;

    pl_Req = (t_lsimcp_MolrAbortReqM*)MC_RTK_GET_MEMORY (sizeof(* pl_Req));

    pl_Req->v_SsSessionID = vp_SessionId;
    pl_Req->v_Error = vp_ErrorReason;
    pl_Req->v_MolrType = v_MolrType;

    DEBUG_LOG_PRINT_LEV2(("CGPS <-- CGPS_LSIMCP_MOLR_ABORT_REQ"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_MOLR_ABORT_REQ,
                              (t_MsgHeader *)pl_Req);


    return;
}
void lsim4_20SendMolrStartResp (uint8_t vp_SessionId,s_gnsCP_GpsPosition *pp_RefPosition)
{
    t_lsimcp_MolrStartRspM* pl_Resp = NULL;

    pl_Resp = (t_lsimcp_MolrStartRspM*)MC_RTK_GET_MEMORY (sizeof(* pl_Resp));

    pl_Resp->v_SsSessionID = vp_SessionId;


    pl_Resp->v_GpsPosition.v_Status             =    pp_RefPosition->v_Status;
    pl_Resp->v_GpsPosition.v_FixType            =    pp_RefPosition->v_FixType;
    pl_Resp->v_GpsPosition.v_RefFrame           =    (uint16_t)LSIMCP_U16_UNKNOWN;
    pl_Resp->v_GpsPosition.v_RefDeltaCellTime   =    pp_RefPosition->v_RefDeltaCellTime;
    pl_Resp->v_GpsPosition.v_GpsTow             =    pp_RefPosition->v_GpsTow;
    pl_Resp->v_GpsPosition.v_Latitude           =    pp_RefPosition->v_Latitude;
    /* Latitude is only 23 bits long. Set all the other bits to zero */
    pl_Resp->v_GpsPosition.v_Latitude          &=    0x7FFFFF;
    pl_Resp->v_GpsPosition.v_Longitude          =    pp_RefPosition->v_Longitude;
    /* Longitude requires sign extension based on bit 23 */
    if( pl_Resp->v_GpsPosition.v_Longitude & 0x800000 )
    {
        /* if 23th bit is 1, set bit 31-24 as 1*/
        pl_Resp->v_GpsPosition.v_Longitude |= 0xFF000000;
    }
    else
    {
        pl_Resp->v_GpsPosition.v_Longitude &= 0xFFFFFF;
    }
    pl_Resp->v_GpsPosition.v_ShapeType          =    pp_RefPosition->v_ShapeType;
    pl_Resp->v_GpsPosition.v_SignOfLat          =    pp_RefPosition->v_SignOfLat;
    pl_Resp->v_GpsPosition.v_UncertSemiMajor    =    pp_RefPosition->v_UncertSemiMajor;
    pl_Resp->v_GpsPosition.v_UncertSemiMinor    =    pp_RefPosition->v_UncertSemiMinor;
    pl_Resp->v_GpsPosition.v_Altitude           =    pp_RefPosition->v_Altitude;
    pl_Resp->v_GpsPosition.v_InnerRadius        =    pp_RefPosition->v_InnerRadius;
    pl_Resp->v_GpsPosition.v_AxisAngle          =    LSIMCP_U8_UNKNOWN;
    pl_Resp->v_GpsPosition.v_DirectOfAlt        =    pp_RefPosition->v_DirectOfAlt;
    pl_Resp->v_GpsPosition.v_UncertAltitude     =    pp_RefPosition->v_UncertAltitude;
    pl_Resp->v_GpsPosition.v_Confidence         =    pp_RefPosition->v_Confidence;
    pl_Resp->v_GpsPosition.v_IncludedAngle      =    pp_RefPosition->v_IncludedAngle;
    pl_Resp->v_GpsPosition.v_OffsetAngle        =    pp_RefPosition->v_OffsetAngle;
    pl_Resp->v_GpsPosition.v_PolygonNumOfPoints =    0;


    DEBUG_LOG_PRINT_LEV2(("CGPS <--- CGPS_LSIMCP_MOLR_START_RSP"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_MOLR_START_RSP,
                              (t_MsgHeader *)pl_Resp);

    return;
}


void lsim4_21SendMolrStartInd(uint8_t v_SsSessionID,
                                  uint8_t v_Option,
                                  uint8_t v_ServiceType,
                                  e_gnsCP_MolrType v_MolrType,
                                  s_gnsCP_GpsAidRequest *p_GpsAidRequest,
                                  s_gnsCP_QoP *p_QopRequest,
                                  s_gnsCP_3rdPartyClient *p_3rdPartyRequest,
                                  uint16_t  v_SupportedGADShapesMask )

{
    u_gnsCP_MsgData    v_MsgData;
    s_gnsCP_GpsMolrStartInd *pl_gnsCP_GpsMolrStartInd=NULL;
    uint8_t vl_index = 0;
    DEBUG_LOG_PRINT_LEV2(("GNS <--- E_gnsCP_MOLR_START_IND"));

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );
    v_MsgData.v_GnsCpGpsMolrStartInd.v_SessionId                                =   v_SsSessionID;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_Option                                   =   v_Option;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_ServiceType                              =   v_ServiceType;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_MolrType                                 =   v_MolrType;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_QopRequest                               =  *p_QopRequest;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_ClientIdConfig         =   p_3rdPartyRequest->v_ClientIdConfig;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_ClientIdTonNpi         =   p_3rdPartyRequest->v_ClientIdTonNpi;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_ClientIdPhoneNumberLen =   p_3rdPartyRequest->v_ClientIdPhoneNumberLen;
    strncpy((char *)v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_ClientIdPhoneNumber, (const char *)p_3rdPartyRequest->v_ClientIdPhoneNumber, K_gnsCP_MAX_PHONE_NUM_LENGTH);

    v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_MlcNumConfig           =   p_3rdPartyRequest->v_MlcNumConfig;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_MlcNumTonNpi           =   p_3rdPartyRequest->v_MlcNumTonNpi;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_MlcNumPhoneNumberLen   =   p_3rdPartyRequest->v_MlcNumPhoneNumberLen;
    strncpy((char *)v_MsgData.v_GnsCpGpsMolrStartInd.v_3rdPartyRequest.v_MlcNumPhoneNumber,(const char *)p_3rdPartyRequest->v_MlcNumPhoneNumber, K_gnsCP_MAX_PHONE_NUM_LENGTH);

    v_MsgData.v_GnsCpGpsMolrStartInd.v_SupportedGADShapesMask = v_SupportedGADShapesMask ;

    v_MsgData.v_GnsCpGpsMolrStartInd.v_GpsAidRequest.v_AidMask                  =  p_GpsAidRequest->v_AidMask;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_GpsAidRequest.v_GpsToe                   =  p_GpsAidRequest->v_GpsToe;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_GpsAidRequest.v_GpsWeek                  =  p_GpsAidRequest->v_GpsWeek;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_GpsAidRequest.v_NrOfSats                 =  p_GpsAidRequest->v_NrOfSats;
    v_MsgData.v_GnsCpGpsMolrStartInd.v_GpsAidRequest.v_TTOELimit                =  p_GpsAidRequest->v_TTOELimit;

    for(vl_index = 0; vl_index < p_GpsAidRequest->v_NrOfSats && vl_index < LSIMCP_MAX_SAT_NR && vl_index < 32 /* see GN GPS Library ISD */ ; vl_index++)
         v_MsgData.v_GnsCpGpsMolrStartInd.v_GpsAidRequest.a_SatID[vl_index] = p_GpsAidRequest->a_SatID[vl_index] - 1;

    for (vl_index = 0; vl_index < p_GpsAidRequest->v_NrOfSats && vl_index < LSIMCP_MAX_SAT_NR && vl_index < 32 /* see GN GPS Library ISD */ ; vl_index++)
         v_MsgData.v_GnsCpGpsMolrStartInd.v_GpsAidRequest.a_Iode[vl_index] = p_GpsAidRequest->a_Iode[vl_index];

    GNS_ExecuteCpCallback( E_gnsCP_MOLR_START_IND, sizeof( *pl_gnsCP_GpsMolrStartInd) , &v_MsgData  );
    return;

}



void lsim4_22SendMolrStopInd(uint8_t v_SsSessionID,
                                  e_gnsCP_MolrType v_MolrType,
                                  e_gnsCP_MolrErrorReason v_ErrorReason)

{
    u_gnsCP_MsgData    v_MsgData;
    s_gnsCP_GpsMolrStopInd *pl_gnsCP_GpsMolrStopInd=NULL;
    DEBUG_LOG_PRINT_LEV2(("GNS <--- E_gnsCP_MOLR_STOP_IND"));

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );
    v_MsgData.v_GnsCPGpsMolrStopInd.v_SessionId   =  v_SsSessionID;
    v_MsgData.v_GnsCPGpsMolrStopInd.v_MolrType    =  v_MolrType;
    v_MsgData.v_GnsCPGpsMolrStopInd.v_ErrorReason =  v_ErrorReason;

    GNS_ExecuteCpCallback( E_gnsCP_MOLR_STOP_IND, sizeof( *pl_gnsCP_GpsMolrStopInd) , &v_MsgData  );

    return;
}


void lsim4_23SendPosCapabilityInd()

{
    u_gnsCP_MsgData    v_MsgData;
    s_gnsCP_GpsPosCapabilityInd *pl_gnsCP_GpsPosCapabilityInd=NULL;
    DEBUG_LOG_PRINT_LEV2(("GNS <--- E_gnsCP_POSN_CAPABILITY_IND"));

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    GNS_ExecuteCpCallback( E_gnsCP_POSN_CAPABILITY_IND, sizeof( *pl_gnsCP_GpsPosCapabilityInd) , &v_MsgData  );

    return;
}



/* -LMSqcMOLR */
void lsim4_24SendLocNotifyAbortInd( uint32_t vp_SessionId)
{
    t_lsimcp_LocNotifyAbortIndM *pl_LocNotifyAbortIndM;

    pl_LocNotifyAbortIndM = (t_lsimcp_LocNotifyAbortIndM *)MC_RTK_GET_MEMORY(sizeof(*pl_LocNotifyAbortIndM));

    pl_LocNotifyAbortIndM->v_SsSessionID = vp_SessionId;

    DEBUG_LOG_PRINT_LEV2(("CGPS_LSIMCP_LOC_NOTIFY_ABORT_IND --> CGPS"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                               PROCESS_CGPS, 0,
                               CGPS_LSIMCP_LOC_NOTIFY_ABORT_IND,
                               pl_LocNotifyAbortIndM);
}


#endif /* AGPS_FTR */

#undef LSIM4CPGNS_C

