/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps7cp.c
* \date 21/07/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains all functions relative to the control plane\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 21.07.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 12.08.08 </TD><TD> M.BELOU </TD><TD> Correct errors and add operator rules </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS7CP_C__
#ifdef AGPS_FTR

#include "cgpscp.h"

#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 7


static void CGPS7_45sendNotification (t_lsimcp_LocNotifyReqM * pl_NotifReq , uint8_t vl_index);

static void CGPS7_47StoreGPSMeasurements ( s_GN_AGPS_Meas* pp_Meas, uint8_t  vp_index );


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/*****************************************************************************************************/
/* CGPS7_00MsAssistedReq : register or update a control plane application in MS assisted mode        */
/*****************************************************************************************************/
const t_OperationDescriptor* CGPS7_00MsAssistedReq(t_RtkObject* p_FsmObject)
{
    t_lsimcp_MSAssistedReqM * pl_MsAssistedReq = (t_lsimcp_MSAssistedReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    t_lsimcp_MsAssistedCnfM * pl_MsAssistedCnf = NULL ;
    pl_MsAssistedCnf= (t_lsimcp_MsAssistedCnfM*)MC_RTK_GET_MEMORY(sizeof(* pl_MsAssistedCnf));
    uint8_t vl_index = 0;
    bool vl_ret    = TRUE;
    uint8_t   vl_IsExistingSession   = FALSE;
    bool vl_IsCpHandleAvailable = FALSE;
    const t_OperationDescriptor* pl_NextState = SAME;

    MC_CGPS_TRACE(("CGPS7_00MsAssistedReq : start"));

    vg_AskForAssistanceData = ( pl_MsAssistedReq->v_AssistanceReqGrant )?FALSE:TRUE;  /* Allowed =0, Not Allowed = 1 */

    if (!vg_IsEmergencyCall)
    {
        if (!CGPS7_16SearchCPHandle(pl_MsAssistedReq->v_SessionID,(uint8_t*)&vl_index, (uint8_t*)&vl_IsExistingSession))
        {
            MC_CGPS_TRACE(("CGPS7_00MsAssistedReq : No CP Handle"));
        }
        else
        {
            vl_IsCpHandleAvailable = TRUE;
        }
    }
    else /* used the emergency session */
    {
        vl_index = K_CGPS_EMERGENCY_SESSION_INDEX;
        vl_IsCpHandleAvailable = TRUE;
        MC_CGPS_TRACE(("CGPS7_00MsAssistedReq : emergency request"));
    }

    if ( vl_IsCpHandleAvailable )
    {
        s_CgpsCp[vl_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;

        /* store or update CP session context */
        s_CgpsCp[vl_index].v_IsRegistered              = TRUE;
        s_CgpsCp[vl_index].v_SessionID                 = pl_MsAssistedReq->v_SessionID;
        s_CgpsCp[vl_index].v_SessionType               = K_CGPS_CP_SESSION_TYPE_MS_ASSISTED;
        s_CgpsCp[vl_index].v_OptField                  = pl_MsAssistedReq->v_PosInstruct.v_OptField;
        s_CgpsCp[vl_index].v_EnvChar                   = pl_MsAssistedReq->v_PosInstruct.v_EnvChar;
        s_CgpsCp[vl_index].v_LastApplicationUpdateTime = GN_GPS_Get_OS_Time_ms();

        if ((!vg_IsEmergencyCall) && (!vl_IsExistingSession))
        {
            if (CGPS4_18CheckNewSessionRight(CGPS_CP_NI_APPLICATION_ID))
            {
                CGPS4_19UpdateOtherApplicationState(CGPS_CP_NI_APPLICATION_ID);
                s_CgpsCp[vl_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
            }
            else
            {
                s_CgpsCp[vl_index].v_Suspended = CGPS_MIN_PRIORITY_VALUE;
            }
        }


        s_CgpsCp[vl_index].v_PeriodicInterval = pl_MsAssistedReq->v_PeriodicInterval;
    /* +LMSqc34406 */
#ifdef CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR
        s_CgpsCp[vl_index].v_ReportingAmount  = pl_MsAssistedReq->v_ReportingAmount;
#endif /*CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR*/
/*  -LMSqc34406 */

        CGPS7_25PopulateQop( vl_index, pl_MsAssistedReq->v_PosInstruct );
        if( TRUE == pl_MsAssistedReq->v_NoReporting )
        {
            s_CgpsCp[vl_index].v_Suspended      = CGPS_MIN_PRIORITY_VALUE;
/*In RRC, multiple Meas Control messages are received for initiation of a positioning session with initial Measurement Control messages
  having NoReporting set. In these cases the CP session is registered with CGPS which is suspended. Also CGPS is intended to send out
  measurements on timeout even in suspended state. Hence a Timeout of 64seconds is set so that session is not aborted immediately. The
  following requests with periodicReporting would set the required timeout value*/
            s_CgpsCp[vl_index].v_Timeout        = CGPS_DEFAULT_TIMEOUT_ON_SUSPEND;
        }

        if ((!vg_IsEmergencyCall) && (!vl_IsExistingSession))
        {
            vg_CGPS_Cp_Count++;
            vl_ret = CGPS0_52ConfigureCgpsNextState();
        }

        CGPS7_17CPAssistanteDataRequest( vl_index );
    }


    /* set the CNF session ID with REQ session ID*/
    pl_MsAssistedCnf->v_SessionID = pl_MsAssistedReq->v_SessionID;

    if (vl_ret == FALSE || vl_IsCpHandleAvailable == FALSE )
    {
        pl_MsAssistedCnf->v_Result = LSIMCP_CGPS_ERR;
    }
    else
    {
        if ( vg_CGPS_Cp_Assitance_No_Request )
        {
            vg_CGPS_Cp_Assitance_No_Request = 0 ;
        }
        /* send positive confirmation */
        pl_MsAssistedCnf->v_Result = LSIMCP_CGPS_OK;

/* + LMSQC19754 */
        MC_CGPS_SEND_PSTE_NMEA_ABORT_IND();
        MC_CGPS_SEND_PSTE_NMEA_FIX_COMPLETE();
/* - LMSQC19754 */
        MC_CGPS_SEND_PSTE_NMEA_FIX_REQUEST();
    }

/* + LMSqc32518 */
    MC_CGPS_TRACE_SESSION_INFO("CGPS7_00MsAssistedReq", s_CgpsCp[vl_index]);
/* - LMSqc32518 */

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_MS_ASSISTED_CNF,
                               (t_MsgHeader *)pl_MsAssistedCnf);

    pl_NextState = a_CGPSRunState;
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*****************************************************************************************************/
/* CGPS7_01BasedReq : register or update a control plane application in MS based mode                */
/*****************************************************************************************************/
const t_OperationDescriptor* CGPS7_01MsBasedReq(t_RtkObject* p_FsmObject)
{
    t_lsimcp_MSBasedReqM * pl_MsBasedReq = (t_lsimcp_MSBasedReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    t_lsimcp_MsBasedCnfM * pl_MsBasedCnf=NULL;
    pl_MsBasedCnf  = (t_lsimcp_MsBasedCnfM*)MC_RTK_GET_MEMORY(sizeof(*pl_MsBasedCnf));
    uint8_t vl_index = 0;
    bool vl_ret = TRUE;
    bool vl_IsExistingSession   = FALSE;
    bool vl_IsCpHandleAvailable = FALSE;
    const t_OperationDescriptor* pl_NextState = SAME;

    MC_CGPS_TRACE(("CGPS7_01MsBasedReq : start"));

    vg_AskForAssistanceData = ( pl_MsBasedReq->v_AssistanceReqGrant )?FALSE:TRUE;  /* Allowed =0, Not Allowed = 1 */

    if (!vg_IsEmergencyCall)
    {
        if (!CGPS7_16SearchCPHandle(pl_MsBasedReq->v_SessionID,(uint8_t*)&vl_index, (uint8_t*)&vl_IsExistingSession))
        {
            MC_CGPS_TRACE(("CGPS7_01MsBasedReq : No CP Handle"));
        }
        else
        {
            vl_IsCpHandleAvailable = TRUE;
        }
    }
    else /* used the emergency session */
    {
        vl_index = K_CGPS_EMERGENCY_SESSION_INDEX;
        vl_IsCpHandleAvailable = TRUE;
        MC_CGPS_TRACE(("CGPS7_01MsBasedReq : emergency request"));
    }

    if ( vl_IsCpHandleAvailable )
    {
        s_CgpsCp[vl_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;

        /* store or update CP session context */
        s_CgpsCp[vl_index].v_IsRegistered              = TRUE;
        s_CgpsCp[vl_index].v_SessionID                 = pl_MsBasedReq->v_SessionID;
        s_CgpsCp[vl_index].v_SessionType               = K_CGPS_CP_SESSION_TYPE_MS_BASED;
        s_CgpsCp[vl_index].v_OptField                  = pl_MsBasedReq->v_PosInstruct.v_OptField;
        s_CgpsCp[vl_index].v_EnvChar                   = pl_MsBasedReq->v_PosInstruct.v_EnvChar;
        s_CgpsCp[vl_index].v_LastApplicationUpdateTime = GN_GPS_Get_OS_Time_ms();

        if ((!vg_IsEmergencyCall) && (!vl_IsExistingSession))
        {
            if (CGPS4_18CheckNewSessionRight(CGPS_CP_NI_APPLICATION_ID))
            {
                CGPS4_19UpdateOtherApplicationState(CGPS_CP_NI_APPLICATION_ID);
                s_CgpsCp[vl_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
            }
            else
            {
                s_CgpsCp[vl_index].v_Suspended = CGPS_MIN_PRIORITY_VALUE;
            }
        }


        s_CgpsCp[vl_index].v_PeriodicInterval = pl_MsBasedReq->v_PeriodicInterval;
        /* +LMSqc34406 */
#ifdef CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR
        s_CgpsCp[vl_index].v_ReportingAmount  = pl_MsBasedReq->v_ReportingAmount;
#endif /*CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR*/
        /* -LMSqc34406 */

        CGPS7_25PopulateQop( vl_index, pl_MsBasedReq->v_PosInstruct );

        if( TRUE == pl_MsBasedReq->v_NoReporting )
        {
            s_CgpsCp[vl_index].v_Suspended = CGPS_MIN_PRIORITY_VALUE;
/*In RRC, multiple Meas Control messages are received for initiation of a positioning session with initial Measurement Control messages
  having NoReporting set. In these cases the CP session is registered with CGPS which is suspended. Also CGPS is intended to send out
  measurements on timeout even in suspended state. Hence a Timeout of 64seconds is set so that session is not aborted immediately. The
  following requests with periodicReporting would set the required timeout value*/
            s_CgpsCp[vl_index].v_Timeout        = CGPS_DEFAULT_TIMEOUT_ON_SUSPEND;
        }

        if ((!vg_IsEmergencyCall) && (!vl_IsExistingSession))
        {
            vg_CGPS_Cp_Count++;
            vl_ret = CGPS0_52ConfigureCgpsNextState();
        }
        CGPS7_17CPAssistanteDataRequest( vl_index );
    }

    /* set de CONF session ID with REQ session ID*/
    pl_MsBasedCnf->v_SessionID = pl_MsBasedReq->v_SessionID;

    if (vl_ret == FALSE || vl_IsCpHandleAvailable == FALSE )
    {
        pl_MsBasedCnf->v_Result = LSIMCP_CGPS_ERR;
    }
    else
    {
        if ( vg_CGPS_Cp_Assitance_No_Request )
        {
            vg_CGPS_Cp_Assitance_No_Request = 0 ;
        }
        pl_MsBasedCnf->v_Result = LSIMCP_CGPS_OK;

/* + LMSQC19754 */
        MC_CGPS_SEND_PSTE_NMEA_ABORT_IND();
        MC_CGPS_SEND_PSTE_NMEA_FIX_COMPLETE();
/* - LMSQC19754 */
        MC_CGPS_SEND_PSTE_NMEA_FIX_REQUEST();
    }

    MC_CGPS_TRACE_SESSION_INFO("CGPS7_01MsBasedReq", s_CgpsCp[vl_index]);

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_MS_BASED_CNF,
                               (t_MsgHeader *)pl_MsBasedCnf);
    pl_NextState = a_CGPSRunState;
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/*****************************************************************************************************/
/* CGPS7_02AbortReq : deregister a control plane application                                         */
/*****************************************************************************************************/
const t_OperationDescriptor* CGPS7_02AbortReq(t_RtkObject* p_FsmObject)
{
    t_lsimcp_AbortReqM * pl_AbortReq = (t_lsimcp_AbortReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    t_lsimcp_AbortCnfM * pl_AbortCnf = NULL;
    pl_AbortCnf  = (t_lsimcp_AbortCnfM*)MC_RTK_GET_MEMORY(sizeof(*pl_AbortCnf));
    const t_OperationDescriptor* pl_NextState = SAME;
    uint8_t vl_Index  = 0;
    uint8_t vl_found=FALSE;
    uint8_t vl_SessionToAbort = 0xFF;

    MC_CGPS_TRACE(("CGPS7_02AbortReq : Session abort : %i", pl_AbortReq->v_SessionID));

    for (vl_Index = 0; (vl_Index < (K_CGPS_MAX_NUMBER_OF_CP+1)) && (vl_found==FALSE); vl_Index++)
    {
        if (   s_CgpsCp[vl_Index].v_IsRegistered == TRUE
            && s_CgpsCp[vl_Index].v_SessionID    == pl_AbortReq->v_SessionID )
        {
            vl_found = TRUE;
            vl_SessionToAbort = vl_Index;
            MC_CGPS_TRACE(("CGPS7_02AbortReq : CGPS session index : %i", vl_Index));
        }
    }

    if(    TRUE == vl_found
        && vl_SessionToAbort != 0xFF )
    {
        CGPS7_26AbortSession( vl_SessionToAbort );
    }

    pl_AbortCnf->v_SessionID = pl_AbortReq->v_SessionID;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_ABORT_CNF,
                               (t_MsgHeader *)pl_AbortCnf);

    MC_CGPS_TRACE(("CGPS7_02AbortReq : CP session count : %d", vg_CGPS_Cp_Count));

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/*******************************************************************************************************/
/* CGPS7_03SendDataToCpAppli : Send data (position fix or measurements) to all elected CP applications */
/*******************************************************************************************************/
void CGPS7_03SendDataToCpAppli(void)
{
    uint8_t  vl_index = 0;
    uint32_t vl_CurrentTime = 0;
    uint8_t  vl_sendData = FALSE;
    uint32_t vl_GAD_Ref_TOW;
    int32_t vl_EFSP_dT_us;
    int16_t vl_GAD_Ref_TOW_Subms;
    s_GN_AGPS_GAD_Data  p_GAD;
    s_GN_AGPS_QoP       p_QoP;
    s_GN_AGPS_Meas      p_Meas;

    /*first thing to is to update the global data from the library if it's necessary*/
    if (vg_CGPS_Cp_Count>0)
    {
        vl_CurrentTime = GN_GPS_Get_OS_Time_ms();

        /* for each application */
        for (vl_index = 0; vl_index < (K_CGPS_MAX_NUMBER_OF_CP+1); vl_index++) /* +1 to include emergency session */
        {
            /* + LMSqc32518 */
            vl_sendData = FALSE;
            /* - LMSqc32518 */
/*Ref time is provided as part of measurement position request. No need to wait in suspend mode till all the assistance is received for setting the same to PE.*/
            if (s_CgpsCp[vl_index].v_IsRegistered == TRUE) 
            {
#ifdef AGPS_TIME_SYNCH_FTR
               if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_AGPS_FTA))
               {
                    CGPS7_42FtaCheckAndRequestPulse();
               }
#endif /*AGPS_TIME_SYNCH_FTR*/

/* + LMSqc32518 */
#ifndef AGPS_CP_SUSPEND_ON_AID_REQ
                if(s_CgpsCp[vl_index].v_Suspended == FALSE)
#endif/*!AGPS_CP_SUSPEND_ON_AID_REQ     */
/* - LMSqc32518 */
                {
                    if ((s_CgpsCp[vl_index].v_SessionType == K_CGPS_CP_SESSION_TYPE_MS_ASSISTED))
                    {
                        if (s_CgpsCp[vl_index].v_PeriodicInterval )
                        {
    /* + LMSqc32518 */
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                            if (s_CgpsCp[vl_index].v_Suspended == FALSE)
                            {
#endif /*AGPS_CP_SUSPEND_ON_AID_REQ  */
    /* - LMSqc32518 */

                            /* periodic application test to know if the timer expired and send the measurement*/
                            if ((s_CgpsCp[vl_index].v_LastApplicationUpdateTime + s_CgpsCp[vl_index].v_PeriodicInterval) <= vl_CurrentTime)
                            {
                                /*Time to send Data */
                                if (GN_AGPS_Get_Meas((U4*)&s_CgpsCp[vl_index].v_PreviousTimeLib,&p_Meas))
                                {
                                    /* send data to LSIMCP */
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli : Periodic Fix sent with quality : %i", p_Meas.Quality ));
                                    CGPS7_18CPSendMSAssistedData(&p_Meas,vl_index);
                                    /* + LMSqc32518 */
                                    vl_sendData = TRUE;
                                    /* - LMSqc32518 */
                                }
                                else
                                {
                                    /*send a request for assistance data if needed*/
                                    CGPS7_17CPAssistanteDataRequest( vl_index );

                                    /* send an error because we can't send data and the timer expired*/
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli : ERROR : Cannot send periidic fix data and timer expired!"));
                                    CGPS7_18CPSendMSAssistedData(NULL,vl_index);
                                    /* + LMSqc32518 */
                                    vl_sendData = TRUE;
                                    /* - LMSqc32518 */
                                }
                                s_CgpsCp[vl_index].v_LastApplicationUpdateTime += s_CgpsCp[vl_index].v_PeriodicInterval;
                            }
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                            }
#endif /*AGPS_CP_SUSPEND_ON_AID_REQ  */
                        }
                        else
                        {
                            /* case of MS assisted application registered for single shot fix*/
    /* + LMSqc32518 */
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                        if  (s_CgpsCp[vl_index].v_Suspended == FALSE) {
#endif /*AGPS_CP_SUSPEND_ON_AID_REQ  */
    /* - LMSqc32518 */
                            if (GN_AGPS_Get_Meas((U4*)&s_CgpsCp[vl_index].v_PreviousTimeLib,&p_Meas))
                            {
                                if (   p_Meas.Quality          >= CGPS_CP_DEFAULT_MEAS_QUALITY
                                    && p_Meas.Meas_GPS_TOW_Unc <  CGPS_CP_MAX_TOW_UNC
                                    /* We expect TOW to be resolved to at least 15ms. If time is available, we wait till this is done */

                                    )
                                {
                                    /* send data to LSIMCP */
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli :                 Measurement Sent. Quality %3d TOWUnc %3d, Times[ Current %10d, ReqRecvd %10d, Used %10d, Unused %10d ]", \
                                                                                p_Meas.Quality,               \
                                                                                p_Meas.Meas_GPS_TOW_Unc,      \
                                                                                vl_CurrentTime,               \
                                                                                s_CgpsCp[vl_index].v_LastApplicationUpdateTime,                  \
                                                                                vl_CurrentTime-s_CgpsCp[vl_index].v_LastApplicationUpdateTime,   \
                                                                                s_CgpsCp[vl_index].v_LastApplicationUpdateTime + s_CgpsCp[vl_index].v_Timeout - vl_CurrentTime));

                                    CGPS7_18CPSendMSAssistedData(&p_Meas,vl_index);
                                    vl_sendData = TRUE;
                                }
                                else
                                {
                                    CGPS7_47StoreGPSMeasurements(&p_Meas, vl_index);
                                }

                            }
    /* + LMSqc32518 */
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                           }
#endif /*AGPS_CP_SUSPEND_ON_AID_REQ  */
    /* - LMSqc32518 */
                            /* single shot test if timeout expired*/
                            if (((s_CgpsCp[vl_index].v_LastApplicationUpdateTime + s_CgpsCp[vl_index].v_Timeout) <= vl_CurrentTime)
                                    && !vl_sendData)
                            {
                                if (s_CgpsCp[vl_index].v_Meas.Quality !=0)
                                {
                                    /*answer with the best quality that we can*/
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli :       'TimeOut' Measurement Sent. Quality %3d TOWUnc %3d", \
                                                                                s_CgpsCp[vl_index].v_Meas.Quality,                                            \
                                                                                s_CgpsCp[vl_index].v_Meas.Meas_GPS_TOW_Unc));

                                    CGPS7_18CPSendMSAssistedData(&s_CgpsCp[vl_index].v_Meas,vl_index);
                                    /* + LMSqc32518 */
                                    vl_sendData = TRUE;
                                    /* - LMSqc32518 */
                                }
                                else
                                {
                                    /* send an error to LSIMCP we can't answer successfully at this request */
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli : ERROR 'TimeOut' Measurement NOT Sent."));
                                    CGPS7_18CPSendMSAssistedData(NULL,vl_index);
                                    /* + LMSqc32518 */
                                    vl_sendData = TRUE;
                                    /* - LMSqc32518 */
                                }
                            }
                        }
                    }
                    else if (s_CgpsCp[vl_index].v_SessionType == K_CGPS_CP_SESSION_TYPE_MS_BASED)
                    {
                        /* case of MS based  application registered for single shot fix*/
                        if (s_CgpsCp[vl_index].v_PeriodicInterval == 0)
                        {
                            p_QoP.Horiz_Accuracy = s_CgpsCp[vl_index].v_HorizAccuracy;
                            p_QoP.Vert_Accuracy  = s_CgpsCp[vl_index].v_VertAccuracy;
                            p_QoP.Deadline_OS_Time_ms = s_CgpsCp[vl_index].v_LastApplicationUpdateTime + s_CgpsCp[vl_index].v_Timeout;

    /* + LMSqc32518 */
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                        if  (s_CgpsCp[vl_index].v_Suspended == FALSE) {
#endif /*AGPS_CP_SUSPEND_ON_AID_REQ  */
    /* - LMSqc32518 */
                            if (GN_AGPS_Qual_Pos((U4*)&(s_CgpsCp[vl_index].v_PreviousTimeLib), &p_QoP))
                            {
                                if (GN_AGPS_Get_GAD_Data(NULL,(U4*)&vl_GAD_Ref_TOW, (I2*)&vl_GAD_Ref_TOW_Subms, (I4*) &vl_EFSP_dT_us, &p_GAD))
                                {
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli :                 Position Fix Sent. Times[ Current %10d, ReqRecvd %10d, Used %10d, Unused %10d ]", \
                                                                                vl_CurrentTime,                                                  \
                                                                                s_CgpsCp[vl_index].v_LastApplicationUpdateTime,                  \
                                                                                vl_CurrentTime-s_CgpsCp[vl_index].v_LastApplicationUpdateTime,   \
                                                                                s_CgpsCp[vl_index].v_LastApplicationUpdateTime + s_CgpsCp[vl_index].v_Timeout - vl_CurrentTime));

                                    CGPS7_19CPSendMSBasedData(&p_GAD,vl_GAD_Ref_TOW, vl_GAD_Ref_TOW_Subms, vl_EFSP_dT_us,vl_index);
                                    vl_sendData = TRUE;
                                }
                            }
    /* + LMSqc32518 */
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                            }
#endif /*AGPS_CP_SUSPEND_ON_AID_REQ  */
    /* - LMSqc32518 */

                            if ((s_CgpsCp[vl_index].v_LastApplicationUpdateTime + s_CgpsCp[vl_index].v_Timeout <= vl_CurrentTime)
                                    && (!vl_sendData))
                            {
                                if (GN_AGPS_Get_GAD_Data(NULL,(U4*)&vl_GAD_Ref_TOW, (I2*)&vl_GAD_Ref_TOW_Subms, (I4*) &vl_EFSP_dT_us, &p_GAD))
                                {
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli :       'Timeout' Position Fix Sent."));
                                    CGPS7_19CPSendMSBasedData(&p_GAD,vl_GAD_Ref_TOW, vl_GAD_Ref_TOW_Subms, vl_EFSP_dT_us,vl_index);
                                    vl_sendData = TRUE;
                                }
                                else
                                {
                                    MC_CGPS_TRACE(("CGPS7_03SendDataToCpAppli : ERROR 'Timeout' Position Fix NOT Sent."));
                                    CGPS7_19CPSendMSBasedData(NULL,0,0,0,vl_index);
                                    vl_sendData = TRUE;
                                }
                            }
                        }
                        else
    /* + LMSqc32518 */
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                        if  (s_CgpsCp[vl_index].v_Suspended == FALSE)
#endif /*AGPS_CP_SUSPEND_ON_AID_REQ  */
    /* - LMSqc32518 */
                        {
                            /* if timer expired*/
                            if ((s_CgpsCp[vl_index].v_LastApplicationUpdateTime + s_CgpsCp[vl_index].v_PeriodicInterval) <= vl_CurrentTime)
                            {
                                if (GN_AGPS_Get_GAD_Data((U4*)&(s_CgpsCp[vl_index].v_PreviousTimeLib),(U4*) &vl_GAD_Ref_TOW, (I2*)&vl_GAD_Ref_TOW_Subms, (I4*)&vl_EFSP_dT_us, &p_GAD))
                                {
                                    CGPS7_19CPSendMSBasedData(&p_GAD,vl_GAD_Ref_TOW, vl_GAD_Ref_TOW_Subms, vl_EFSP_dT_us,vl_index);
                                    /* + LMSqc32518 */
                                    vl_sendData = TRUE;
                                    /* - LMSqc32518 */
                                }
                                else
                                {
                                    CGPS7_19CPSendMSBasedData(NULL,0,0,0,vl_index);
                                    /* + LMSqc32518 */
                                    vl_sendData = TRUE;
                                    /* - LMSqc32518 */

                                    /*send a request for assistance data if needed*/
                                    CGPS7_17CPAssistanteDataRequest( vl_index );
                                }
                                s_CgpsCp[vl_index].v_LastApplicationUpdateTime += s_CgpsCp[vl_index].v_PeriodicInterval;
                            }
                        }
                    }
                }
            }
            /* + LMSqc32518 */
/* +LMSqc34406 */
#ifdef CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR
            if( vl_sendData )
            {
                uint8_t vl_AbortRequired = FALSE;

                if ( s_CgpsCp[vl_index].v_PeriodicInterval == 0 ) /* Single shot */
                {
                    vl_AbortRequired = TRUE;
                }
                else
                {
                    /* If v_ReportingAmount is already zero, we have request for inifinite reporting */
                    /* In this case, do not abort */
                    if( s_CgpsCp[vl_index].v_ReportingAmount > 0 )
                    {
                        /* Decrement to indicate one report sent to network */
                        s_CgpsCp[vl_index].v_ReportingAmount--;

                        /* If after decrement, the remaining report == 0 , then we have provided all
                                              requested reports to network. Abort the session*/
                        if( s_CgpsCp[vl_index].v_ReportingAmount == 0 )
                        {
                            vl_AbortRequired = TRUE;
                        }
                    }
                }

                if( TRUE == vl_AbortRequired )
#else
                if(vl_sendData && (s_CgpsCp[vl_index].v_PeriodicInterval == 0))
#endif /*CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR*/
/* -LMSqc34406 */
                {
                    CGPS7_26AbortSession( vl_index );
/* +LMSqc34406 */
#ifdef CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR
                }
#endif /*CGPS_TEMP_HANDLES_CP_REPORTING_AMNT_FTR*/
/* -LMSqc34406 */
            }
            /* - LMSqc32518 */
        }
    }
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/*******************************************************************************************************/
/* CGPS7_05AssistanceDataAReq : Reception of Assistance data                                           */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_05AssistanceDataAReq(t_RtkObject* p_FsmObject)
{
    t_lsimcp_AssistDataAReqM * pl_AssistDataAReq;
    s_GN_AGPS_Acq_Ass p_AA;
    const t_OperationDescriptor* pl_NextState = SAME;
    uint8_t vl_index = 0;
    uint8_t vl_ret=0;

    pl_AssistDataAReq = (t_lsimcp_AssistDataAReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if (CGPS7_21RcvCPData()== TRUE)
    {
        pl_NextState = a_CGPSRunState;
        p_AA.Ref_GPS_TOW = pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.v_GpsTow;
        p_AA.Num_AA = pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.v_NrOfSats;

        MC_CGPS_TRACE_ACQ_DATA(p_AA);

        for (vl_index = 0; vl_index < p_AA.Num_AA && vl_index < 16 /* see GN GPS Library ISD */ && vl_index < LSIMCP_MAX_SAT_NR; vl_index++)
        {
            p_AA.AA[vl_index].SatID          =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_SatId+1;
            p_AA.AA[vl_index].Doppler_0      =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_Doppler0;
            p_AA.AA[vl_index].Doppler_1      =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_Doppler1;
            p_AA.AA[vl_index].Doppler_Unc    =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_DopplerUncert;
            p_AA.AA[vl_index].Code_Phase     =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_CodePhase;
            p_AA.AA[vl_index].Int_Code_Phase =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_IntCodePhase;
            p_AA.AA[vl_index].GPS_Bit_Num    =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_GpsBitNumber;
            p_AA.AA[vl_index].Code_Window    =  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_SearchWindow;

            if ( pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_Azimuth == LSIMCP_U8_UNKNOWN )
            {
                p_AA.AA[vl_index].Azimuth    = 0;
            }
            else
            {
                p_AA.AA[vl_index].Azimuth    = pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_Azimuth;
            }

            if (  pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_Elevation == LSIMCP_U8_UNKNOWN )
            {
                p_AA.AA[vl_index].Elevation  = 0;
            }
            else
            {
                p_AA.AA[vl_index].Elevation  = pl_AssistDataAReq->v_AssistAData.s_AcquisAssist.a_AcquisElement[vl_index].v_Elevation;
            }

            MC_CGPS_TRACE_ACQ_ELEMENT(p_AA.AA[vl_index]);
        }

        vl_ret=GN_AGPS_Set_Acq_Ass(&p_AA);

        if (vl_ret == FALSE)
        {
            MC_CGPS_TRACE(("CGPS7_05AssistanceDataAReq : Library reject Acquisition Assistance data"));
        }
        else
        {
            vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_ACQ_ASSIST));
        }
    }
    else
    {
        MC_CGPS_TRACE(("CGPS7_05AssistanceDataAReq : Acquisition Assistance data is not injected in the library because the loop can't start"));
    }

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/*******************************************************************************************************/
/* CGPS7_06TimeReq : Reception of Time assistance                                                      */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_06TimeReq(t_RtkObject* p_FsmObject)
{
    t_lsimcp_TimeReqM * pl_TimeReq;
    const t_OperationDescriptor* pl_NextState = SAME;
    uint8_t vl_ret=0;

    pl_TimeReq = (t_lsimcp_TimeReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    if (pl_TimeReq->v_Src == LSIMCP_ASSIST_SOURCE_CP)
    {
        CGPS7_21RcvCPData();
    }

    /*Explicit check is  mde to check for GPS on mainly for non CP cases. The CP cases it is expected that GPS will be switched on 
    and also it will set vg_CGPS_Cp_Assitance_No_Request*/
    if(vg_CGPS_State != K_CGPS_ACTIVE_GPS_ON)
    {
        MC_CGPS_TRACE(("CGPS7_06TimeReq : Assistance data received but GPS not running"));
        return pl_NextState;
    }   
    

    pl_NextState = a_CGPSRunState;

    if (pl_TimeReq->v_RefTime.v_RefDeltaCellTime == 0x7FFFFFFF)
    {
        s_GN_AGPS_Ref_Time p_RTime;

        p_RTime.OS_Time_ms =       GN_GPS_Get_OS_Time_ms();
        p_RTime.TOW_ms =           pl_TimeReq->v_RefTime.v_GpsTow;
        p_RTime.WeekNo =           pl_TimeReq->v_RefTime.v_GpsWeek;
        p_RTime.RMS_ms =           CGPS_DEFAULT_DELTA_CORE_TIME;

        MC_CGPS_TRACE_REF_TIME(p_RTime);

        vl_ret = GN_AGPS_Set_Ref_Time(&p_RTime);
    }
    /* Fine time is handled as part of CGPS7_43FtaPulseCnf() */

    if (vl_ret == FALSE)
    {
        MC_CGPS_TRACE(("CGPS7_06TimeReq : library rejected the time assistance"));
    }

    if(  pl_TimeReq->v_RefTime.v_NumTowAssist )
    {
        uint8_t              vl_Index;
        s_GN_AGPS_TOW_Assist vl_TowAssistData;

        vl_TowAssistData.Num_TOWA = pl_TimeReq->v_RefTime.v_NumTowAssist;
        vl_TowAssistData.TOW_ms   = pl_TimeReq->v_RefTime.v_GpsTow;

        MC_CGPS_TRACE_TOW_ASSIST_DATA(vl_TowAssistData);

        for (vl_Index = 0; vl_Index < pl_TimeReq->v_RefTime.v_NumTowAssist; vl_Index++ )
        {
             vl_TowAssistData.TOWA[vl_Index].SatID           = pl_TimeReq->v_RefTime.a_TowAssist[vl_Index].v_Satid+1;
             vl_TowAssistData.TOWA[vl_Index].Anti_Spoof_flag = pl_TimeReq->v_RefTime.a_TowAssist[vl_Index].v_AntiSpoof;
             vl_TowAssistData.TOWA[vl_Index].Alert_flag      = pl_TimeReq->v_RefTime.a_TowAssist[vl_Index].v_Alert;
             vl_TowAssistData.TOWA[vl_Index].TLM_Reserved    = pl_TimeReq->v_RefTime.a_TowAssist[vl_Index].v_TlmRes;
             vl_TowAssistData.TOWA[vl_Index].TLM_Word        = pl_TimeReq->v_RefTime.a_TowAssist[vl_Index].v_TlmWord;

             MC_CGPS_TRACE_TOW_ASSIST_ELEMENT( vl_TowAssistData.TOWA[vl_Index]);
        }

        vl_ret = GN_AGPS_Set_TOW_Assist( &vl_TowAssistData );

        if (vl_ret == FALSE)
        {
              MC_CGPS_TRACE(("CGPS7_06TimeReq : library rejected the TOW assistance"));
        }
    }

    /*Assistance mask change needs to be done only for assistance data whose source is CP*/
    MC_CGPS_TRACE(("CGPS7_06TimeReq : vl_ret =%d,pl_TimeReq->v_Src=%d",vl_ret,pl_TimeReq->v_Src));
    if( (vl_ret == TRUE) && (pl_TimeReq->v_Src == LSIMCP_ASSIST_SOURCE_CP))
    {
        vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_REF_TIME));
    }

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/*******************************************************************************************************/
/* CGPS7_07RefPosReq : Reception of Reference Position assistance                                       */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_07RefPosReq(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    t_lsimcp_RefPosReqM * pl_RefPosReq = (t_lsimcp_RefPosReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    s_GN_AGPS_GAD_Data p_RPos;
    uint8_t vl_ret=0;

    if (CGPS7_21RcvCPData()== TRUE)
    {
        pl_NextState = a_CGPSRunState;
        p_RPos.latitudeSign         = pl_RefPosReq->v_GpsPosition.v_SignOfLat;
        p_RPos.latitude             = pl_RefPosReq->v_GpsPosition.v_Latitude;
        p_RPos.longitude            = pl_RefPosReq->v_GpsPosition.v_Longitude;
        p_RPos.uncertaintySemiMajor = pl_RefPosReq->v_GpsPosition.v_UncertSemiMajor;
        p_RPos.uncertaintySemiMinor = pl_RefPosReq->v_GpsPosition.v_UncertSemiMinor;
        p_RPos.orientationMajorAxis = pl_RefPosReq->v_GpsPosition.v_AxisAngle;
        p_RPos.uncertaintyAltitude  = pl_RefPosReq->v_GpsPosition.v_UncertAltitude;
        p_RPos.confidence           = pl_RefPosReq->v_GpsPosition.v_Confidence;
        p_RPos.altitudeDirection    = pl_RefPosReq->v_GpsPosition.v_DirectOfAlt ;
        p_RPos.altitude             = pl_RefPosReq->v_GpsPosition.v_Altitude  ;//-1=Altitude not present

        p_RPos.verdirect      = -1;
        p_RPos.bearing        = -1;
        p_RPos.horspeed       = -1;
        p_RPos.verspeed       = -1;
        p_RPos.horuncertspeed = 255;
        p_RPos.veruncertspeed = 255;

        MC_CGPS_TRACE_REF_POS_DATA(p_RPos);

/* + LMSQC19763 */
#ifdef TMP_IGNORE_REF_POS_FTR
/* +LMSQC17034 : Ref Pos not injected into GPS library */
       vl_ret = TRUE;
/* -LMSQC17034 : Ref Pos not injected into GPS library */
#else
        vl_ret=GN_AGPS_Set_GAD_Ref_Pos(&p_RPos);
#endif /*TMP_IGNORE_REF_POS_FTR*/
/* - LMSQC19763 */

        if (vl_ret == FALSE)
        {
            MC_CGPS_TRACE(("CGPS7_07RefPosReq : library reject the reference position"));
        }
        else
        {
            vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_REF_LOC));
        }
    }
    else
    {
        MC_CGPS_TRACE(("CGPS7_07RefPosReq : the reference position is not injected in the library because the loop can't start"));
    }
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/*******************************************************************************************************/
/* CGPS7_08AssistanceDataBReq : Reception of assistance data                                           */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_08AssistanceDataBReq(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    t_lsimcp_AssistDataBReqM * pl_AssistDataBReq = (t_lsimcp_AssistDataBReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    uint8_t vl_index;
    uint8_t vl_ret=0;

    MC_CGPS_TRACE_NAV_DATA(pl_AssistDataBReq->v_AssistBData.s_NavModel.v_NrOfSats);

    if (CGPS7_21RcvCPData()== TRUE)
    {
        pl_NextState = a_CGPSRunState;
        for (vl_index = 0; vl_index < pl_AssistDataBReq->v_AssistBData.s_NavModel.v_NrOfSats; vl_index++)
        {
            s_GN_AGPS_Eph_El p_Eph_El;
            U4 a_EphReservedWords[4];

            p_Eph_El.SatID       =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_SatId+1;
            p_Eph_El.CodeOnL2    =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemCodeOnL2;
            p_Eph_El.URA         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemURA;
            p_Eph_El.SVHealth    =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemSvHealth;
            p_Eph_El.FitIntFlag  =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemFitFlag;
            p_Eph_El.AODA        =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemAODO;
            p_Eph_El.L2Pflag     =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_Epheml2pFlag;
            p_Eph_El.TGD         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemTgd;
            p_Eph_El.af2         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemAf2;
            p_Eph_El.Week        =    0;
            p_Eph_El.toc         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemToc;
            p_Eph_El.toe         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemToe;
            p_Eph_El.IODC        =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemIodc;
            p_Eph_El.af1         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemAf1;
            p_Eph_El.dn          =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemDeltaN;
            p_Eph_El.IDot        =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemIdot;
            p_Eph_El.Crs         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemCrs;
            p_Eph_El.Crc         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemCrc;
            p_Eph_El.Cus         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemCus;
            p_Eph_El.Cuc         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemCuc;
            p_Eph_El.Cis         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemCis;
            p_Eph_El.Cic         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemCic;
            p_Eph_El.af0         =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemAf0;
            p_Eph_El.M0          =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemMo;
            p_Eph_El.e           =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_Epheme;
            p_Eph_El.APowerHalf  =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemAPowerhalf;
            p_Eph_El.Omega0      =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemOmegaA0;
            p_Eph_El.i0          =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemIo;
            p_Eph_El.w           =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemW;
            p_Eph_El.OmegaDot    =    pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_EphemOmegaAdot;

            a_EphReservedWords[0] = pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_Reserved1;
            a_EphReservedWords[1] = pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_Reserved2;
            a_EphReservedWords[2] = pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_Reserved3;
            a_EphReservedWords[3] = pl_AssistDataBReq->v_AssistBData.s_NavModel.a_NavData[vl_index].v_Reserved4;

            MC_CGPS_TRACE_NAV_ELEMENT(p_Eph_El);

            vl_ret=GN_AGPS_Set_Eph_El(&p_Eph_El);

            if (vl_ret == FALSE)
            {
                MC_CGPS_TRACE(("CGPS7_08AssistanceDataBReq : library reject the ephemeris for the sat %i ",  p_Eph_El.SatID));
            }
            else
            {
                vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_NAV_MODEL));
            }

            vl_ret = GN_AGPS_Set_Eph_Res(p_Eph_El.SatID, a_EphReservedWords);

            if (vl_ret == FALSE)
            {
                MC_CGPS_TRACE(("CGPS7_08AssistanceDataBReq : library reject the ephemeris reserved words for the sat %i ",  p_Eph_El.SatID));
            }
        }
    }
    else
    {
        MC_CGPS_TRACE(("CGPS7_08AssistanceDataBReq :  the ephemeris are not injected in the library because the loop can't start"));
    }
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/*******************************************************************************************************/
/* CGPS7_09AlmanachDataReq : Reception of almanac data                                                */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_09AlmanacDataReq(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    t_lsimcp_AlmanacDataReqM * pl_AlmanacDataReq = (t_lsimcp_AlmanacDataReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    uint8_t vl_index = 0;
    uint8_t vl_ret=0;

    if (CGPS7_21RcvCPData()== TRUE)
    {
        MC_CGPS_TRACE_ALMANAC_DATA(pl_AlmanacDataReq->v_AlmanacData.v_NrOfSats);

        pl_NextState = a_CGPSRunState;
        for (vl_index = 0; vl_index < pl_AlmanacDataReq->v_AlmanacData.v_NrOfSats; vl_index++)
        {
            s_GN_AGPS_Alm_El p_Alm_El;

            p_Alm_El.SatID      =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_SatId+1;
            p_Alm_El.WNa        =    pl_AlmanacDataReq->v_AlmanacData.v_AlmanacWNA;
            p_Alm_El.SVHealth   =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacSVhealth;
            p_Alm_El.toa        =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacToa;
            p_Alm_El.af0        =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacAF0;
            p_Alm_El.af1        =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacAF1;
            p_Alm_El.e          =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacE;
            p_Alm_El.delta_I    =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacDeltai;
            p_Alm_El.OmegaDot   =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacOmegaDot;
            p_Alm_El.APowerHalf =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacAsqrt;
            p_Alm_El.Omega0     =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacOmega0;
            p_Alm_El.w          =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacOmega;
            p_Alm_El.M0         =    pl_AlmanacDataReq->v_AlmanacData.a_AlmanacList[vl_index].v_AlmanacM0;

            MC_CGPS_TRACE_ALMANAC_ELEMENT(p_Alm_El);

#ifndef TEMP_GPS_ALM_EPH_MISMATCH_FTR
            vl_ret=GN_AGPS_Set_Alm_El(&p_Alm_El);
#endif /*TEMP_GPS_ALM_EPH_MISMATCH_FTR*/

            if (vl_ret == FALSE)
            {
                MC_CGPS_TRACE(("CGPS7_09AlmanacDataReq : library reject the almanac for the sat %i ",  p_Alm_El.SatID));
            }
            else
            {
                vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_ALMANAC));
            }
        }
    }
    else
    {
        MC_CGPS_TRACE(("CGPS7_09AlmanacDataReq :  the almanacs are not injected in the library because the loop can't start"));
    }
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/*******************************************************************************************************/
/* CGPS7_10AssistIonoModelReq : Reception of ionospheric model assistance                              */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_10AssistIonoModelReq(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    s_GN_AGPS_Ion p_Ion;
    t_lsimcp_AssistIonoModelReqM * pl_AssistIonoModelReq = (t_lsimcp_AssistIonoModelReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    uint8_t vl_ret=0;

    if (CGPS7_21RcvCPData()== TRUE)
    {
        pl_NextState = a_CGPSRunState;
        p_Ion.ZCount = 0;
        p_Ion.a0 = pl_AssistIonoModelReq->v_AssistIono.v_Alpha0;
        p_Ion.a1 = pl_AssistIonoModelReq->v_AssistIono.v_Alpha1;
        p_Ion.a2 = pl_AssistIonoModelReq->v_AssistIono.v_Alpha2;
        p_Ion.a3 = pl_AssistIonoModelReq->v_AssistIono.v_Alpha3;
        p_Ion.b0 = pl_AssistIonoModelReq->v_AssistIono.v_Beta0;
        p_Ion.b1 = pl_AssistIonoModelReq->v_AssistIono.v_Beta1;
        p_Ion.b2 = pl_AssistIonoModelReq->v_AssistIono.v_Beta2;
        p_Ion.b3 = pl_AssistIonoModelReq->v_AssistIono.v_Beta3;

        MC_CGPS_TRACE_IONO_DATA(p_Ion);

        vl_ret=GN_AGPS_Set_Ion(&p_Ion);

        if (vl_ret == FALSE)
        {
            MC_CGPS_TRACE(("CGPS7_10AssistIonoModelReq : library reject the ionospheric delay model"));
        }
        else
        {
            vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_IONO_MODEL));
        }
    }
    else
    {
        MC_CGPS_TRACE(("CGPS7_10AssistIonoModelReq : the ionospheric delay model is not injected in the library because the loop can't start"));
    }
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/*******************************************************************************************************/
/* CGPS7_11AssistDGPSCorrectionReq : Reception of DGPS correction                                      */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_11AssistDGPSCorrectionReq(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    p_FsmObject = p_FsmObject;
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/*******************************************************************************************************/
/* CGPS7_12AssistUTCModelReq : Reception of UTC model                                                  */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_12AssistUTCModelReq(t_RtkObject* p_FsmObject)
{
    s_GN_AGPS_UTC p_UTC;
    const t_OperationDescriptor* pl_NextState = SAME;
    t_lsimcp_AssistUtcModelReqM * pl_AssistUtcModelReq = (t_lsimcp_AssistUtcModelReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    uint8_t vl_ret=0;

    if (CGPS7_21RcvCPData()== TRUE)
    {
        pl_NextState = a_CGPSRunState;

        p_UTC.ZCount = 0;
        p_UTC.A1 =     pl_AssistUtcModelReq->v_AssistUtcm.v_UtcA1;
        p_UTC.A0 =     pl_AssistUtcModelReq->v_AssistUtcm.v_UtcA0;
        p_UTC.Tot =    pl_AssistUtcModelReq->v_AssistUtcm.v_UtcTot;
        p_UTC.WNt =    pl_AssistUtcModelReq->v_AssistUtcm.v_UtcWnT;
        p_UTC.dtLS =   pl_AssistUtcModelReq->v_AssistUtcm.v_UtcDeltaTLs;
        p_UTC.WNLSF =  pl_AssistUtcModelReq->v_AssistUtcm.v_UtcWnLsf;
        p_UTC.DN =     pl_AssistUtcModelReq->v_AssistUtcm.v_UtcDn;
        p_UTC.dtLSF =  pl_AssistUtcModelReq->v_AssistUtcm.v_UtcDeltaTLsf;

        MC_CGPS_TRACE_UTC_DATA(p_UTC);

        vl_ret=GN_AGPS_Set_UTC(&p_UTC);

        if (vl_ret == FALSE)
        {
            MC_CGPS_TRACE(("CGPS7_12AssistUTCModelReq : library reject the UTC correction model"));
        }
        else
        {
            vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_UTC_MODEL));
        }
    }
    else
    {
        MC_CGPS_TRACE(("CGPS7_12AssistUTCModelReq : the UTC correction model is not injected in the library because the loop can't start"));
    }
    return pl_NextState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/*******************************************************************************************************/
/* CGPS7_13AssistRTIReq : Reception of Real Time Integrity                                             */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_13AssistRTIReq(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    t_lsimcp_AssistRtiReqM * pl_AssistRtiReq = (t_lsimcp_AssistRtiReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    uint8_t pl_BadSVList[LSIMCP_MAX_SAT_NR];
    int8_t  pl_BadSVListPrint[LSIMCP_MAX_SAT_NR*3 + 1];
    uint8_t vl_index = 0;
    uint8_t vl_ret=0;

    if (CGPS7_21RcvCPData()== TRUE)
    {
        pl_NextState = a_CGPSRunState;

        MC_CGPS_TRACE(("CGPS7_13AssistRTIReq : NrOfSats %d", pl_AssistRtiReq->v_AssistRti.v_NrOfSats));

        memset(pl_BadSVListPrint,0,(LSIMCP_MAX_SAT_NR*3 + 1));

        for (vl_index = 0; vl_index < pl_AssistRtiReq->v_AssistRti.v_NrOfSats; vl_index++)
        {
            pl_BadSVList[vl_index] = pl_AssistRtiReq->v_AssistRti.a_RtiList[vl_index].v_BadSatId;

            snprintf( (char*)&pl_BadSVListPrint[vl_index*3], 3, " %u ", pl_BadSVList[vl_index] );
        }
        pl_BadSVListPrint[vl_index*3] = '\0';

        MC_CGPS_TRACE(("BAD SAT IDs : %s " ,  pl_BadSVListPrint));

        vl_ret = GN_AGPS_Set_Bad_SV_List(pl_AssistRtiReq->v_AssistRti.v_NrOfSats, pl_BadSVList);

        if (vl_ret == FALSE)
        {
            MC_CGPS_TRACE(("CGPS7_13AssistRTIReq : library reject the bad sat list"));
        }
        else
        {
            vg_AssistanceAidMask = vg_AssistanceAidMask & (~(LSIMCP_AID_REQ_RTI));
        }
    }
    else
    {
        MC_CGPS_TRACE(("CGPS7_12AssistUTCModelReq : the bad sat list is not injected in the library because the loop can't start"));
    }
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/*******************************************************************************************************/
/* CGPS7_14ECallStart : Start GPS service if necessary while waiting for an emergency CP session       */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_14ECallStart(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    p_FsmObject = p_FsmObject;

    s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;

    vg_CGPS_Cp_Count++;

    CGPS0_52ConfigureCgpsNextState();

    vg_IsEmergencyCall = TRUE;

    MC_CGPS_TRACE(("CGPS7_14ECallStart : ECall Started. TotalCpSessionCount = %d, SessionIsRegistered = %d, SessionIsSuspended = %d", \
                   vg_CGPS_Cp_Count, s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_IsRegistered, s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_Suspended ));

    pl_NextState = a_CGPSRunState;

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15
/*******************************************************************************************************/
/* CGPS7_15ECallStop : Application advert CGPS that a emergency Call is finished */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_15ECallStop(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState    = SAME;
    s_GN_AGPS_Meas*              pl_GN_AGPS_Meas = NULL;
    p_FsmObject = p_FsmObject;

    if (vg_IsEmergencyCall)
    {
        vg_IsEmergencyCall = FALSE;

        /* + LMSqb76925 */

        /* Three cases :
            1)CP request for emergency call started, Positioning session ended through CGPS7_02AbortReq. Later Ecall ends.
                s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_IsRegistered == FALSE
                CGPS7_02AbortReq is called first followed by CGPS7_15ECallStop.
                CP count decremented in CGPS7_15ECallStop
                Session parameters are cleared in CGPS7_02AbortReq.

            2)CP request for emergency call isn't started, we must stop the loop and update the other application
                s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_IsRegistered == FALSE
                CP count is decremented in CGPS7_15ECallStop. CGPS_LSIMCP_ABORT_REQ never sent.

            3)CP request for emergency started, but before position/measurement is sent, Emergency call is ended by user.
               s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_IsRegistered == TRUE.
               CGPS7_15ECallStop is called from MMI first before CP positioning sessions ends through CGPS7_02AbortReq.
               CP count decremented in CGPS7_02AbortReq.
               Session parameters are cleared through CGPS7_02AbortReq*/

        if (s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_IsRegistered == FALSE)
        {

            if (vg_CGPS_Cp_Count)
            {
                vg_CGPS_Cp_Count--;
            }

            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_SessionID        = 0;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_SessionType      = K_CGPS_CP_SESSION_TYPE_NOT_SET;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_PeriodicInterval = 0;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_OptField         = 0;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_HorizAccuracy    = 0;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_VertAccuracy     = 0;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_EnvChar          = 0;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_LastApplicationUpdateTime = 0;
            s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_Suspended        = CGPS_MAX_PRIORITY_VALUE;

            memset((uint8_t*)&s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_Meas,0,sizeof(*pl_GN_AGPS_Meas));

            if ((vg_CGPS_Cp_Count == 0))
            {
                vg_AssistanceAidMask = CGPS_ASSISTANCE_MASK;
            }

        }

        /* - LMSqb76925 */
        CGPS4_26DeQueueMessage();

        CGPS0_52ConfigureCgpsNextState();
    }

    MC_CGPS_TRACE(("CGPS7_15ECallStop : ECall Stopped. TotalCPSessionCount = %d, SessionIsRegistered = %d, SessionIsSuspended = %d",  \
        vg_CGPS_Cp_Count, s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_IsRegistered, s_CgpsCp[K_CGPS_EMERGENCY_SESSION_INDEX].v_Suspended ));

    pl_NextState = a_CGPSRunState;
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 16
/*****************************************************************************/
/* CGPS7_16SearchFreeCPHandle : search a free handle for CP application           */
/*****************************************************************************/
uint8_t CGPS7_16SearchCPHandle(uint8_t vp_SessionId, uint8_t * pp_Index, uint8_t* vp_IsExistingSession)
{
    uint8_t vl_return = TRUE;
    uint8_t vl_index = 0;

    *vp_IsExistingSession = FALSE;

    /* check if the session is already regsitered */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_CP; vl_index++)
    {
        if(    (s_CgpsCp[vl_index].v_IsRegistered == TRUE)
            && (s_CgpsCp[vl_index].v_SessionID == vp_SessionId)
          )
        {
            *vp_IsExistingSession = TRUE;
            break;
        }
    }

    /* if the session doesn't exist */
    if (vl_index == K_CGPS_MAX_NUMBER_OF_CP)
    {
        /* if the session isn't already registered then search for the first available index */
        for (vl_index = 0; s_CgpsCp[vl_index].v_IsRegistered && vl_index < K_CGPS_MAX_NUMBER_OF_CP; vl_index++);

        /* if there's no available index then send a negative response */
        if (vl_index == K_CGPS_MAX_NUMBER_OF_CP)
        {
            vl_return = FALSE;
        }
    }
    if (vl_return)
    {
        *pp_Index = vl_index;
    }

    return vl_return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 17
/*****************************************************************************/
/* CGPS7_17CPAssistanteDataRequest : ask LSIM fro assistance data from network        */
/*****************************************************************************/
void CGPS7_17CPAssistanteDataRequest( uint8_t vp_sessionId )
{
    if (vg_AskForAssistanceData)
    {

#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
        if( s_CgpsCp[vp_sessionId].v_Suspended == FALSE )
        {
#endif

            uint8_t vl_index = 0;
            uint16_t vl_Mask = 0;
            s_GN_AGPS_Assist_Req p_AR;

            /* check if assistance data is needed */
            if ((CGPS4_61CheckIfAssistanceRequired(&vl_Mask, &p_AR) == TRUE))
            {
                t_lsimcp_AidRequIndM * pl_AidRequInd =NULL;
                pl_AidRequInd  = (t_lsimcp_AidRequIndM*)MC_RTK_GET_MEMORY(sizeof(*pl_AidRequInd));

                pl_AidRequInd->v_GpsAidRequest.v_AidMask = vl_Mask;

                pl_AidRequInd->v_GpsAidRequest.v_AidMask =  pl_AidRequInd->v_GpsAidRequest.v_AidMask & vg_AssistanceAidMask;

                if ( s_CgpsCp[vp_sessionId].v_SessionType == K_CGPS_CP_SESSION_TYPE_MS_ASSISTED )
                {
                    pl_AidRequInd->v_GpsAidRequest.v_AidMask = pl_AidRequInd->v_GpsAidRequest.v_AidMask & vg_cgps_platform_config.v_MsaMandatoryMask;
                }
                else
                {
                    pl_AidRequInd->v_GpsAidRequest.v_AidMask = pl_AidRequInd->v_GpsAidRequest.v_AidMask & vg_cgps_platform_config.v_MsbMandatoryMask;
                }

                if (pl_AidRequInd->v_GpsAidRequest.v_AidMask == 0)
                {
                    MC_RTK_FREE_MEMORY(pl_AidRequInd);
                    return;
                }
                else
                {
                    pl_AidRequInd->v_GpsAidRequest.v_AidMask|= LSIMCP_AID_REQ_REF_TIME;
                    MC_CGPS_TRACE(("CGPS7_17CPAssistanteDataRequest : Missing AssistData : 0x%x" , pl_AidRequInd->v_GpsAidRequest.v_AidMask));
                }


                pl_AidRequInd->v_GpsAidRequest.v_GpsWeek = p_AR.gpsWeek;
                pl_AidRequInd->v_GpsAidRequest.v_GpsTow = p_AR.Toe;
                pl_AidRequInd->v_GpsAidRequest.v_NrOfSats = p_AR.Num_Sat;
                pl_AidRequInd->v_GpsAidRequest.v_TTOELimit = p_AR.Toe_Limit;

                for (vl_index = 0; vl_index < p_AR.Num_Sat && vl_index < LSIMCP_MAX_SAT_NR && vl_index < 32 /* see GN GPS Library ISD */ ; vl_index++)
                    pl_AidRequInd->v_GpsAidRequest.a_SatID[vl_index] = p_AR.SatID[vl_index] - 1;

                for (vl_index = 0; vl_index < p_AR.Num_Sat && vl_index < LSIMCP_MAX_SAT_NR && vl_index < 32 /* see GN GPS Library ISD */ ; vl_index++)
                    pl_AidRequInd->v_GpsAidRequest.a_Iode[vl_index] = p_AR.IODE[vl_index];

/* + LMSqc32518 */
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
                s_CgpsCp[vp_sessionId].v_Suspended = TRUE;
#endif/*AGPS_CP_SUSPEND_ON_AID_REQ     */
/* - LMSqc32518 */
                MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                           PROCESS_LSIMCP, 0,
                                           CGPS_LSIMCP_AID_REQU_IND,
                                           (t_MsgHeader *)pl_AidRequInd);

                vg_AskForAssistanceData = FALSE;
            }
#ifdef AGPS_CP_SUSPEND_ON_AID_REQ
        }
#endif

    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 18
/***************************************************************************************/
/* CGPS7_18CPSendDataToAssistedApplication : send ms assisted data to an Network application         */
/***************************************************************************************/
void CGPS7_18CPSendMSAssistedData(s_GN_AGPS_Meas * pp_Meas,uint8_t vp_sessionId)
{
    uint8_t vl_i = 0;

    t_lsimcp_MsrResultIndM * pl_MsrResultInd=NULL;
    pl_MsrResultInd = (t_lsimcp_MsrResultIndM*)MC_RTK_GET_MEMORY(sizeof(*pl_MsrResultInd));

    pl_MsrResultInd->v_SessionID       = s_CgpsCp[vp_sessionId].v_SessionID;

    if ( s_CgpsCp[vp_sessionId].v_PeriodicInterval == 0 ) /* Single shot */
    {
        pl_MsrResultInd->v_FinalReport = TRUE;
    }
    else
    {
        pl_MsrResultInd->v_FinalReport = FALSE;
    }


    if (pp_Meas)
    {
        pl_MsrResultInd->v_MsrRes.v_NrOfSats         = pp_Meas->Num_Meas;
        pl_MsrResultInd->v_MsrRes.v_Status           = LSIMCP_CGPS_OK;
        pl_MsrResultInd->v_MsrRes.v_RefFrame         = LSIMCP_U16_UNKNOWN; /* Setting it to UNKNOWN LSIM will update it according to latest Cell info */
        pl_MsrResultInd->v_MsrRes.v_GpsTow           = pp_Meas->Meas_GPS_TOW;
        pl_MsrResultInd->v_MsrRes.v_RefDeltaCellTime = pp_Meas->EFSP_dT_us;
        pl_MsrResultInd->v_MsrRes.v_GpsTowUnc        = pp_Meas->Meas_GPS_TOW_Unc;

        if( pp_Meas->Delta_TOW == -1 )
        {
            pl_MsrResultInd->v_MsrRes.v_Delta_TOW     = LSIMCP_U8_UNKNOWN;
        }
        else
        {
            pl_MsrResultInd->v_MsrRes.v_Delta_TOW     = (uint8_t)pp_Meas->Delta_TOW;
        }

        MC_CGPS_TRACE_MEAS_REPORT(pl_MsrResultInd->v_MsrRes);

        for (vl_i = 0; vl_i < pp_Meas->Num_Meas && vl_i < LSIMCP_MAX_SAT_NR; vl_i++)
        {
            pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i].v_SatId        =     pp_Meas->Meas[vl_i].SatID-1;
            pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i].v_CNo          =     pp_Meas->Meas[vl_i].SNR;
            pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i].v_Doppler      =     pp_Meas->Meas[vl_i].Doppler;
            pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i].v_WholeChips   =     pp_Meas->Meas[vl_i].Whole_Chips;
            pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i].v_FracChips    =     pp_Meas->Meas[vl_i].Fract_Chips;
            pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i].v_MpathIndic   =     pp_Meas->Meas[vl_i].MPath_Ind;
            pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i].v_PseudoRmsErr =     pp_Meas->Meas[vl_i].PR_RMS_Err;

            MC_CGPS_TRACE_SV_MEASUREMENT(pl_MsrResultInd->v_MsrRes.a_MsrElement[vl_i]);
        }

        GN_AGPS_Mark_Response_Sent();
    }
    else
    {
        pl_MsrResultInd->v_MsrRes.v_Status = LSIMCP_CGPS_ERR;
    }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_MSR_RESULT_IND,
                               (t_MsgHeader *)pl_MsrResultInd);
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 19
/***************************************************************************************/
/* CGPS7_19CPSendDataToAssistedApplication : send ms based data to an Network application        */
/* assemption : we always set the fixtype with the value of LSIMCP_FIX_TYPE_3D because th altitude is always return */
/* by the library but perhaps with a very big uncertaintyAltitude. This point must be test on the IOT to know */
/* if beyond an uncertaintyAltitude we must set the fix type at LSIMCP_FIX_TYPE_2D */
/***************************************************************************************/
void CGPS7_19CPSendMSBasedData(s_GN_AGPS_GAD_Data * vp_GAD,uint32_t vp_GAD_Ref_TOW, int16_t vp_GAD_Ref_TOW_Subms, int32_t vp_EFSP_dT_us,uint8_t vp_sessionId)
{
    t_lsimcp_PosIndM * pl_PosInd = NULL ;
    pl_PosInd = (t_lsimcp_PosIndM*)MC_RTK_GET_MEMORY(sizeof(*pl_PosInd ));

    pl_PosInd->v_SessionID = s_CgpsCp[vp_sessionId].v_SessionID;

    if ( s_CgpsCp[vp_sessionId].v_PeriodicInterval == 0 ) /* Single shot */
    {
        pl_PosInd->v_FinalReport = TRUE;
    }
    else
    {
        pl_PosInd->v_FinalReport = FALSE;
    }

    if (vp_GAD)
    {
        pl_PosInd->v_GpsPosition.v_Status           = LSIMCP_CGPS_OK;
        pl_PosInd->v_GpsPosition.v_RefFrame         = LSIMCP_U16_UNKNOWN; /* Setting it to UNKNOWN LSIM will update it according to latest Cell info */
        pl_PosInd->v_GpsPosition.v_GpsTowUnc        = LSIMCP_U32_UNKNOWN;
        pl_PosInd->v_GpsPosition.v_RefDeltaCellTime = vp_EFSP_dT_us;
        pl_PosInd->v_GpsPosition.v_GpsTow           = vp_GAD_Ref_TOW;
        if( vp_GAD_Ref_TOW_Subms == -1 )
        {
            pl_PosInd->v_GpsPosition.v_GpsTowSubms = LSIMCP_U16_UNKNOWN;
        }
        else
        {
            pl_PosInd->v_GpsPosition.v_GpsTowSubms = (uint16_t)vp_GAD_Ref_TOW_Subms;
        }
        pl_PosInd->v_GpsPosition.v_FixType          = LSIMCP_FIX_TYPE_3D;
        pl_PosInd->v_GpsPosition.v_SignOfLat        = vp_GAD->latitudeSign;
        pl_PosInd->v_GpsPosition.v_Latitude         = vp_GAD->latitude;
        pl_PosInd->v_GpsPosition.v_Longitude        = vp_GAD->longitude;
        pl_PosInd->v_GpsPosition.v_UncertSemiMajor  = vp_GAD->uncertaintySemiMajor;
        pl_PosInd->v_GpsPosition.v_UncertSemiMinor  = vp_GAD->uncertaintySemiMinor;
        pl_PosInd->v_GpsPosition.v_AxisAngle        = vp_GAD->orientationMajorAxis;
        pl_PosInd->v_GpsPosition.v_DirectOfAlt      = vp_GAD->altitudeDirection;
        pl_PosInd->v_GpsPosition.v_Altitude         = vp_GAD->altitude;//-1=Altitude not present
        pl_PosInd->v_GpsPosition.v_UncertAltitude   = vp_GAD->uncertaintyAltitude;
        pl_PosInd->v_GpsPosition.v_Confidence       = vp_GAD->confidence;
        pl_PosInd->v_GpsPosition.v_IncludedAngle    = LSIMCP_U8_UNKNOWN;
        pl_PosInd->v_GpsPosition.v_OffsetAngle      = LSIMCP_U8_UNKNOWN;
        pl_PosInd->v_GpsPosition.v_InnerRadius      = LSIMCP_U32_UNKNOWN;
        pl_PosInd->v_GpsPosition.v_ShapeType        = CGPS4_16GetShapeType(&(pl_PosInd->v_GpsPosition));

        MC_CGPS_SEND_PSTE_NMEA_FIX_GAD_REPORT(pl_PosInd->v_GpsPosition.v_Latitude , pl_PosInd->v_GpsPosition.v_SignOfLat , pl_PosInd->v_GpsPosition.v_Longitude, pl_PosInd->v_GpsPosition.v_Altitude , pl_PosInd->v_GpsPosition.v_DirectOfAlt );

        MC_CGPS_TRACE_POS_DATA( pl_PosInd->v_GpsPosition);

        GN_AGPS_Mark_Response_Sent();
    }
    else
    {
        pl_PosInd->v_GpsPosition.v_Status = LSIMCP_CGPS_ERR;
    }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_POS_IND,
                               (t_MsgHeader *)pl_PosInd);
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 21
/********************************************************************************************/
/* CGPS7_21RcvCPData :Start cgps loop and request timeout, if the request is not received   */
/*                     when the timer isssue the loop is stopped                            */
/********************************************************************************************/
uint8_t CGPS7_21RcvCPData()
{
    if (vg_CGPS_Cp_Assitance_No_Request == 0 && vg_CGPS_Cp_Count == 0)
    {
        uint32_t  vl_CurrentTime          = GN_GPS_Get_OS_Time_ms();

        vg_CGPS_Cp_Assitance_No_Request   = vl_CurrentTime + CGPS_GPS_RUNNING_WITH_ONLY_ASSISTANCE;

        MC_CGPS_TRACE(("CGPS7_21RcvCPData : No CP Request, Assistance recieved. K_CGPS_ACTIVE_GPS_ON state until %d even if no CP req recvd. Current time %d", vg_CGPS_Cp_Assitance_No_Request, vl_CurrentTime));

        CGPS0_52ConfigureCgpsNextState();
    }
    return TRUE;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 23
/********************************************************************************************/
/* CGPS7_23NotifyReq : Notify user about the CP session                                     */
/********************************************************************************************/
const t_OperationDescriptor* CGPS7_23NotifyReq(t_RtkObject* p_FsmObject)
{
    t_lsimcp_LocNotifyReqM * pl_NotifReq = (t_lsimcp_LocNotifyReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    const t_OperationDescriptor* pl_NextState = SAME;
    uint8_t vl_index=0;

    MC_CGPS_TRACE(("CGPS7_23NotifyReq : Start"));


    /* check if the notification callback has been registered, if not, the notification response is automatically sent according to GN_SUPL_Notify_Type value */
    if (vg_NotificationCallback == NULL)
    {
        CGPS7_24NotifySendCnf(pl_NotifReq->v_SsSessionID,pl_NotifReq->v_LocNotification.v_MtlrTypeVerify,LSIMCP_USER_NO_RSP);
    }
    else
    {
        /* search for the first available Notification context */
        for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_NOTIFICATIONS && s_CgpsNotification[vl_index].v_IsPending; vl_index++);

        /* if no more handle is available, the notification is cancelled */
        if (vl_index == K_CGPS_MAX_NUMBER_OF_NOTIFICATIONS)
        {
            MC_CGPS_TRACE(("CGPS7_23NotifyReq : Notification Handle not found!!!"));
            CGPS7_24NotifySendCnf(pl_NotifReq->v_SsSessionID,pl_NotifReq->v_LocNotification.v_MtlrTypeVerify,LSIMCP_USER_NO_RSP);
        }
        else
        {

            if (pl_NotifReq->v_LocNotification.v_MtlrTypeVerify == LSIMCP_MTLR_NOTIFY)
            {
                s_CgpsNotification[vl_index].v_DefaultNotificationType = K_CGPS_NOTIFICATION_ONLY;
                CGPS7_24NotifySendCnf(pl_NotifReq->v_SsSessionID,LSIMCP_MTLR_NOTIFY,LSIMCP_USER_NOTIFY_ONLY);
                CGPS7_45sendNotification(pl_NotifReq,vl_index);
            }
            else
            {
                if (pl_NotifReq->v_LocNotification.v_NotificationType == LSIMCP_VERIFY_LOC_ALLOWED_IF_NO_RSP)
                {
                    s_CgpsNotification[vl_index].v_DefaultNotificationType = K_CGPS_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA;
                }
                else
                {
                    s_CgpsNotification[vl_index].v_DefaultNotificationType = K_CGPS_NOTIFICATION_AND_VERIFICATION_DENIED_NA;
                }
                CGPS7_45sendNotification(pl_NotifReq,vl_index);
                /* CP notification context setting */
                vg_CGPS_Notification_Count++;
                s_CgpsNotification[vl_index].v_IsPending = TRUE;
                s_CgpsNotification[vl_index].v_Handle = (uint32_t)pl_NotifReq->v_SsSessionID;
                s_CgpsNotification[vl_index].v_HandleType = K_CGPS_CP_HANDLE;
                s_CgpsNotification[vl_index].v_Timeout = GN_GPS_Get_OS_Time_ms() + K_CGPS_CP_NOTIFICATION_TIMEOUT;

                /*Start GPS and  Loop Timer . Loop timer is started to wait for No-Response time out*/
                CGPS0_52ConfigureCgpsNextState();

                pl_NextState = a_CGPSRunState;
            }

        }
    }
    return pl_NextState;

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 24
/********************************************************************************************/
/* CGPS7_24NotifySendCnf : Send user response back to the network                           */
/********************************************************************************************/
void CGPS7_24NotifySendCnf(uint32_t vp_SessionID,uint8_t vp_RequestType,uint8_t vp_UserAnswer)
{
    t_lsimcp_LocNotifyCnfM * pl_NotifRsp = NULL;
    pl_NotifRsp = (t_lsimcp_LocNotifyCnfM*)MC_RTK_GET_MEMORY(sizeof(* pl_NotifRsp));

    MC_CGPS_TRACE(("CGPS7_24NotifySendCnf : User response sent out"));

    pl_NotifRsp->v_SsSessionID                   = vp_SessionID;
    pl_NotifRsp->v_LocNotifyRes.v_MtlrTypeVerify = vp_RequestType;
    pl_NotifRsp->v_LocNotifyRes.v_UserRsp        = vp_UserAnswer;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_LOC_NOTIFY_CNF,
                               (t_MsgHeader *)pl_NotifRsp);
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 25
/********************************************************************************************/
/* CGPS7_25PopulateQop : Populate CGPS Session database with input Qos                                     */
/********************************************************************************************/
void CGPS7_25PopulateQop( uint8_t vp_SessionIndex, t_lsimcp_PosInstruct vp_PosInstruct )
{
     s_CgpsCp[vp_SessionIndex].v_Timeout       = vp_PosInstruct.v_Timeout;

/* optional field set this parameters but it's not used */
    if (( vp_PosInstruct.v_OptField & LSIMCP_HORIZONTAL_ACCURACY ))
        {
           s_CgpsCp[vp_SessionIndex].v_HorizAccuracy = vp_PosInstruct.v_HorAccuracy;
        }
    else
       {
           s_CgpsCp[vp_SessionIndex].v_HorizAccuracy = 0XFF;
       }
    if (( vp_PosInstruct.v_OptField & LSIMCP_VERTICAL_ACCURACY ))
     {
        s_CgpsCp[vp_SessionIndex].v_VertAccuracy = vp_PosInstruct.v_VertAccuracy;
     }
    else
     {
         s_CgpsCp[vp_SessionIndex].v_VertAccuracy = 0XFF;
     }
}
 #undef PROCEDURE_NUMBER
 #define PROCEDURE_NUMBER 26
 /********************************************************************************************/
 /* CGPS7_26AbortSession : Aborts the session.                                     */
 /********************************************************************************************/
 void CGPS7_26AbortSession( uint8_t vp_SessionIndex )
 {
      s_GN_AGPS_Meas *pl_GN_AGPS_Meas=NULL;
     s_CgpsCp[vp_SessionIndex].v_IsRegistered     = FALSE;
     s_CgpsCp[vp_SessionIndex].v_SessionID        = 0;
     s_CgpsCp[vp_SessionIndex].v_SessionType      = K_CGPS_CP_SESSION_TYPE_NOT_SET;
     s_CgpsCp[vp_SessionIndex].v_PeriodicInterval = 0;
     s_CgpsCp[vp_SessionIndex].v_OptField         = 0;
     s_CgpsCp[vp_SessionIndex].v_HorizAccuracy    = 0;
     s_CgpsCp[vp_SessionIndex].v_VertAccuracy     = 0;
     s_CgpsCp[vp_SessionIndex].v_EnvChar          = 0;
     s_CgpsCp[vp_SessionIndex].v_LastApplicationUpdateTime = 0;
     s_CgpsCp[vp_SessionIndex].v_Suspended = CGPS_MAX_PRIORITY_VALUE;
      memset((uint8_t*)&s_CgpsCp[vp_SessionIndex].v_Meas,0,sizeof(*pl_GN_AGPS_Meas));
     MC_CGPS_TRACE(("CGPS7_26AbortSession : CGPS CP session index : %i is De-registered.", vp_SessionIndex));
     MC_CGPS_SEND_PSTE_NMEA_FIX_COMPLETE();
  /* + LMSqb76925 */
     if ( vg_CGPS_Cp_Count )
     {   /* If emergency call is still active, the session count is decremented in  CGPS7_15ECallStop */
         if (  !(( K_CGPS_EMERGENCY_SESSION_INDEX == vp_SessionIndex )
              && ( TRUE == vg_IsEmergencyCall )))
             vg_CGPS_Cp_Count--;
     }
 /* - LMSqb76925 */

     if ((vg_CGPS_Cp_Count == 0))
     {
         vg_AssistanceAidMask = CGPS_ASSISTANCE_MASK;
     }

     CGPS0_52ConfigureCgpsNextState();

}


 #undef PROCEDURE_NUMBER
 #define PROCEDURE_NUMBER 27
 /********************************************************************************************/
 /* CGPS7_27ResetUEPosition : Handle Reset UE Positioning data                                 */
 /********************************************************************************************/
 const t_OperationDescriptor* CGPS7_27ResetUEPosition(t_RtkObject* p_FsmObject)
 {
     MC_CGPS_TRACE(("CGPS7_27ResetUEPosition : Received UE RESET POSITIONING MESG"));
     p_FsmObject = p_FsmObject;

    /*Begin Changes from rahul for Glonass time correction issue 07-Dec-2011 Mohan-194997*/

     //CGPS0_62DeleteNVStoreFields(K_CGPS_FIELD_FACTORY_START);
     CGPS0_62DeleteNVStoreFields(K_CGPS_FIELD_COLD_START);

    /*End Changes from rahul for Glonass time correction issue 07-Dec-2011 Mohan-194997*/

     return SAME;
 }


/*LMSQCMOLR*/
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 28
/********************************************************************************************/
/* CGPS7_28CpMoInstanceNew :                                                              */
/********************************************************************************************/
t_CgpsCpMoAppli * CGPS7_28CpMoInstanceNew(void *pl_Handle, e_cgps_CpMoSessionType vl_Type)
{
    uint8_t vl_index;
    static uint8_t vl_SessionId=0;
    for (vl_index = 0; (vl_index < (K_CGPS_MAX_NUMBER_OF_CP)); vl_index++)
    {
        if ( s_CgpsCpMo[vl_index].v_IsRegistered == TRUE) continue;
        /*New session Id need to be created*/
        vl_SessionId++;
        vl_SessionId= (vl_SessionId)%10;
        s_CgpsCpMo[vl_index].v_SessionId    = vl_SessionId;
        s_CgpsCpMo[vl_index].v_IsRegistered = TRUE;
        s_CgpsCpMo[vl_index].v_Suspended    = CGPS_SESSION_NOT_SUSPENDED;
        s_CgpsCpMo[vl_index].v_SessionType  = vl_Type;
        s_CgpsCpMo[vl_index].v_NafHandle    = pl_Handle;
        vg_CGPS_Cp_Mo_Count++;

        MC_CGPS_TRACE(("CGPS7_28CpMoInstanceNew: MOLR N=%d I=%d R=%d H=%x S=%d SID=%d, T=%d",
                            vg_CGPS_Cp_Mo_Count,
                            vl_index,
                            s_CgpsCpMo[vl_index].v_IsRegistered,
                            s_CgpsCpMo[vl_index].v_NafHandle,
                            s_CgpsCpMo[vl_index].v_Suspended,
                            s_CgpsCpMo[vl_index].v_SessionId,
                            s_CgpsCpMo[vl_index].v_SessionType));

        return &s_CgpsCpMo[vl_index];
    }
    return NULL;
}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 29
/********************************************************************************************/
/* CGPS7_29CpMoInstanceFetch :                                                              */
/********************************************************************************************/
t_CgpsCpMoAppli * CGPS7_29CpMoInstanceFetch(uint8_t vl_SessionId, void *vp_NafHandle)
{
    uint8_t vl_index;
    if((vp_NafHandle == NULL) && (vl_SessionId == 0xFF)) return NULL;

    if(vp_NafHandle == NULL)
    {
        //Searching based on Session
        for (vl_index = 0; (vl_index < (K_CGPS_MAX_NUMBER_OF_CP)); vl_index++)
        {
            if ( s_CgpsCpMo[vl_index].v_IsRegistered == FALSE) continue;
            if ( s_CgpsCpMo[vl_index].v_SessionId != vl_SessionId ) continue;
            return &s_CgpsCpMo[vl_index];
        }
    }
    else //Searching based on NafHandle
    {
        for (vl_index = 0; (vl_index < (K_CGPS_MAX_NUMBER_OF_CP)); vl_index++)
        {
            if ( s_CgpsCpMo[vl_index].v_IsRegistered == FALSE) continue;
            if ( s_CgpsCpMo[vl_index].v_NafHandle != vp_NafHandle ) continue;
            return &s_CgpsCpMo[vl_index];
        }

    }


    return NULL;
}
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 30
/********************************************************************************************/
/* CGPS7_30CpMoInstanceInit :                                                              */
/********************************************************************************************/
void CGPS7_30CpMoInstanceInit(t_CgpsCpMoAppli * vp_Instance)
{
    if(vp_Instance == NULL) return;
    vp_Instance->v_IsRegistered = FALSE;
    vp_Instance->v_SessionId = 0;
    vp_Instance->v_SessionType = K_CGPS_CPMO_SESSION_TYPE_NOT_SET;
    vp_Instance->v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
    return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 31
/********************************************************************************************/
/* CGPS7_31CpMoInstanceInitAll :                                                              */
/********************************************************************************************/
void CGPS7_31CpMoInstanceInitAll()
{
    uint8_t vl_index;
    for(vl_index = 0; vl_index<K_CGPS_MAX_NUMBER_OF_CP;vl_index++)
        CGPS7_30CpMoInstanceInit(&s_CgpsCpMo[vl_index]);
    return;
}

/**
* \def CGPS_CP_CONNECTION_LATENCY
*
* Define the default estimated time to establish a connection
*/
#define CGPS_CP_CONNECTION_LATENCY 8



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 32
/******************************************************************************/
/* CGPS7_32StartCPMO : */
/******************************************************************************/
bool CGPS7_32StartCPMO( t_CgpsNaf* pp_Handle )
{
    bool vl_NewSession=FALSE;

    t_CgpsCpMoAppli *vp_Instance = NULL;
    t_lsimcp_MolrStartIndM *pl_MolrStartInd = NULL;

    MC_CGPS_TRACE(("+ CGPS7_32StartCPMO"));


    if( !MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED)
           || (!MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_CPLANE )))
    {
       return vl_NewSession;
    }

    if(!(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_MS_POS_TECH_MSBASED)) &&
        !(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED)))
        return vl_NewSession;



    if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET(K_CGPS_CONFIG_SEMIAUTO))
    {

        /*In case MS is requested to start the session in MS Based Mode, assitance data will be requested wiht netowkt
                 - MOAR requires to provide
                         - Assistance data
                         - Session Id
                         - Type of MOLR
                     */

        MC_CGPS_TRACE(("SEMI Auto Location Mode"));

        vp_Instance = CGPS7_28CpMoInstanceNew((void *)pp_Handle, K_CGPS_CPMO_SESSION_TYPE_MOAR);
        if (vp_Instance == NULL) return vl_NewSession;

        pl_MolrStartInd =  (t_lsimcp_MolrStartIndM*)MC_RTK_GET_MEMORY(sizeof(*pl_MolrStartInd));
        if(pl_MolrStartInd == NULL) return vl_NewSession;
       memset(pl_MolrStartInd, 0, sizeof(*pl_MolrStartInd));

       CGPS7_37FillGpsAidRequest(&pl_MolrStartInd->v_GpsAidRequest, FALSE);
       pl_MolrStartInd->v_Option |= K_LSIMCP_MOLR_OPT_ASSISTREQ;

    }
    else
    {
        /*In case MS Based is not supported, requested to start the session in MS Assisted Mode, baisc self location is initiated
                 - MOLR requires to provide
                         - QoP
                         - Session Id
                         - Type of MOLR
        */
        MC_CGPS_TRACE(("Basic Self Location Mode"));

        vp_Instance = CGPS7_28CpMoInstanceNew((void *)pp_Handle, K_CGPS_CPMO_SESSION_TYPE_MOLR);
        if (vp_Instance == NULL) return vl_NewSession;

        pl_MolrStartInd =    (t_lsimcp_MolrStartIndM*)MC_RTK_GET_MEMORY(sizeof(*pl_MolrStartInd));
        if(pl_MolrStartInd == NULL) return vl_NewSession;

        if ( (NULL != pp_Handle) && (pp_Handle->v_Mode != K_CGPS_PERIODIC))
        {
            t_cgps_QopIndexFormat v_QoP;
            CGPS4_36ConvertQoP( pp_Handle , &v_QoP, CGPS_CP_CONNECTION_LATENCY );
            pl_MolrStartInd->v_QopRequest.v_Horacc       =     v_QoP.v_Horacc;
            pl_MolrStartInd->v_QopRequest.v_Veracc       =     v_QoP.v_Veracc;
            pl_MolrStartInd->v_QopRequest.v_Delay        =     v_QoP.v_Delay;
            pl_MolrStartInd->v_QopRequest.v_MaxLocAge    =     v_QoP.v_MaxLocAge;
            pl_MolrStartInd->v_Option = K_LSIMCP_MOLR_OPT_QUALITYREQ;
        }

    }
    pl_MolrStartInd->v_SsSessionID = vp_Instance->v_SessionId;
    pl_MolrStartInd->v_MolrType    = vp_Instance->v_SessionType;

    pl_MolrStartInd->v_SupportedGADShapesMask = ( K_CGPS_SHAPE_E_PT | K_CGPS_SHAPE_E_PT_UNC_CIRCLE | K_CGPS_SHAPE_E_PT_UNC_ELLIP 
                                                    | K_CGPS_SHAPE_E_PT_ALT | K_CGPS_SHAPE_E_PT_ALT_UNC_ELLIP ) ;
                                                           /* Doesnot Supports
                                                                                                               Bit 3 - Polygon
                                                                                                               Bit 6 - EP arc*/

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_MOLR_START_IND,
                               (t_MsgHeader *)pl_MolrStartInd);


    vl_NewSession= TRUE;

    return vl_NewSession ;
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 33
/*****************************************************************************************************/
/* CGPS7_33MoarStopReq :  */
/*****************************************************************************************************/
const t_OperationDescriptor* CGPS7_33MolrAbortReq(t_RtkObject* p_FsmObject)
{
    t_lsimcp_MolrAbortReqM * pl_MoarStopReq = (t_lsimcp_MolrAbortReqM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    const t_OperationDescriptor* pl_NextState = SAME;

    MC_CGPS_TRACE(("CGPS7_33MolrAbortReq : Session=%d", pl_MoarStopReq->v_SsSessionID));

    CGPS7_30CpMoInstanceInit(CGPS7_29CpMoInstanceFetch(pl_MoarStopReq->v_SsSessionID, NULL));

    pl_NextState = a_CGPSRunState;

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 34
/*****************************************************************************************************/
/* CGPS7_34MolrStartResp :  */
/*****************************************************************************************************/
const t_OperationDescriptor* CGPS7_34MolrStartResp(t_RtkObject* p_FsmObject)
{
    t_lsimcp_MolrStartRspM * pl_MolrResp = (t_lsimcp_MolrStartRspM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    const t_OperationDescriptor* pl_NextState = SAME;
    t_CgpsCpMoAppli *vp_Instance              = CGPS7_29CpMoInstanceFetch(pl_MolrResp->v_SsSessionID, NULL);

    MC_CGPS_TRACE(("CGPS7_34MolrStartResp : Session=%d", pl_MolrResp->v_SsSessionID));

    if( vp_Instance == NULL )
    {
        MC_CGPS_TRACE(("CGPS7_34MolrStartResp : MO Instance not found"));
        return pl_NextState;
    }

    CGPS7_36SendMolrStopInd(vp_Instance);

    if( vp_Instance->v_NafHandle == NULL )
    {
        MC_CGPS_TRACE(("CGPS7_34MolrStartResp : Associated NAF handle not found"));
        return pl_NextState;
    }

    CGPS7_35UpdateLastPosition(&pl_MolrResp->v_GpsPosition);

    /*remove instance*/
    CGPS7_30CpMoInstanceInit(vp_Instance);

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 35
/*****************************************************************************************************/
/* CGPS7_35UpdateLastPosition :    */
/*****************************************************************************************************/
void CGPS7_35UpdateLastPosition(t_lsimcp_GpsPosition *pl_Pos )
{
    s_GN_AGPS_GAD_Data  vl_PosGadData;

    vl_PosGadData.latitudeSign              =  pl_Pos->v_SignOfLat;
    vl_PosGadData.latitude                  =  pl_Pos->v_Latitude;
    vl_PosGadData.longitude                 =  pl_Pos->v_Longitude;
    vl_PosGadData.orientationMajorAxis      =  pl_Pos->v_AxisAngle;
    vl_PosGadData.confidence                =  pl_Pos->v_Confidence;
    vl_PosGadData.uncertaintySemiMajor      =  pl_Pos->v_UncertSemiMajor;
    vl_PosGadData.uncertaintySemiMinor      =  pl_Pos->v_UncertSemiMinor;
    vl_PosGadData.altitude                  =  pl_Pos->v_Altitude ;  //-1=Altitude not present
    vl_PosGadData.altitudeDirection         =  pl_Pos->v_DirectOfAlt;
    vl_PosGadData.uncertaintyAltitude       =  pl_Pos->v_UncertAltitude;
    vl_PosGadData.bearing                   =  -1;
    vl_PosGadData.verdirect                 =  -1;
    vl_PosGadData.horspeed                  =  -1;
    vl_PosGadData.verspeed                  =  -1;
    vl_PosGadData.horuncertspeed            =  255;
    vl_PosGadData.veruncertspeed            =  255;

    MC_CGPS_TRACE_COMPUTED_POS_DATA(vl_PosGadData);

    GN_AGPS_Set_GAD_Ref_Pos(&vl_PosGadData);
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 36
/*****************************************************************************************************/
/* CGPS7_36SendMolrAbortInd :  */
/*****************************************************************************************************/
void CGPS7_36SendMolrStopInd(t_CgpsCpMoAppli *vp_Instance)
{
    t_lsimcp_MolrStopIndM * pl_MolrStopInd;
    pl_MolrStopInd = (t_lsimcp_MolrStopIndM*)MC_RTK_GET_MEMORY(sizeof(*pl_MolrStopInd));


    MC_CGPS_TRACE(("CGPS7_36SendMolrStopInd : Session=%d", vp_Instance->v_SessionId));
    pl_MolrStopInd->v_SsSessionID = vp_Instance->v_SessionId;
    pl_MolrStopInd->v_Error       = 0;//No Error
    pl_MolrStopInd->v_MolrType    = vp_Instance->v_SessionType;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMCP, 0,
                               CGPS_LSIMCP_MOLR_STOP_IND,
                               (t_MsgHeader *)pl_MolrStopInd);


    return;
}



 #undef PROCEDURE_NUMBER
 #define PROCEDURE_NUMBER 37
 /*****************************************************************************/
 /* CGPS7_37FillGpsAidRequest : send assistance data to LSIM        */
 /*****************************************************************************/
 void CGPS7_37FillGpsAidRequest(t_lsimcp_GpsAidRequest *p_GpsAidRequest, uint8_t vp_isAssistedSession)
 {
     uint8_t vl_index = 0;
     uint16_t vl_Mask = 0;
     s_GN_AGPS_Assist_Req p_AR;
     p_GpsAidRequest->v_AidMask = 0;

     if (!CGPS4_61CheckIfAssistanceRequired(&vl_Mask, &p_AR)) return;

     p_GpsAidRequest->v_AidMask = vl_Mask;

     p_GpsAidRequest->v_AidMask =    p_GpsAidRequest->v_AidMask & vg_AssistanceAidMask;

     if (vp_isAssistedSession)
     {
         p_GpsAidRequest->v_AidMask = p_GpsAidRequest->v_AidMask & vg_cgps_platform_config.v_MsaMandatoryMask;
     }
     else
     {
         p_GpsAidRequest->v_AidMask = p_GpsAidRequest->v_AidMask & vg_cgps_platform_config.v_MsbMandatoryMask;
     }

     if (p_GpsAidRequest->v_AidMask != 0)
     {
         MC_CGPS_TRACE(("CGPS7_17CPAssistanteDataRequest : Missing AssistData : 0x%x" , p_GpsAidRequest->v_AidMask));
         p_GpsAidRequest->v_AidMask|= LSIMCP_AID_REQ_REF_TIME;
     }
     p_GpsAidRequest->v_GpsWeek = p_AR.gpsWeek;
     p_GpsAidRequest->v_GpsTow = p_AR.Toe;
     p_GpsAidRequest->v_NrOfSats = p_AR.Num_Sat;
     p_GpsAidRequest->v_TTOELimit = p_AR.Toe_Limit;

     for (vl_index = 0; vl_index < p_AR.Num_Sat && vl_index < LSIMCP_MAX_SAT_NR && vl_index < 32 /* see GN GPS Library ISD */ ; vl_index++)
         p_GpsAidRequest->a_SatID[vl_index] = p_AR.SatID[vl_index] - 1;

     for (vl_index = 0; vl_index < p_AR.Num_Sat && vl_index < LSIMCP_MAX_SAT_NR && vl_index < 32 /* see GN GPS Library ISD */ ; vl_index++)
         p_GpsAidRequest->a_Iode[vl_index] = p_AR.IODE[vl_index];

 }


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 38
/*****************************************************************************************************/
/* CGPS7_38CheckMolrSession :  */
/*****************************************************************************************************/
void CGPS7_38CheckMolrSession(t_CgpsNaf *vp_Handle)
{
    t_CgpsCpMoAppli *vp_Instance = NULL;
    if(vg_CGPS_Cp_Mo_Count == 0) return;

    MC_CGPS_TRACE(("CGPS7_38CheckMolrSession : vp_Handle=%x", vp_Handle));

    vp_Instance = CGPS7_29CpMoInstanceFetch(0xFF, vp_Handle);

    if(vp_Instance == NULL) return;

    CGPS7_36SendMolrStopInd(vp_Instance);
    CGPS7_30CpMoInstanceInit(vp_Instance);

    vg_CGPS_Cp_Mo_Count--;

    return ;
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 39
/*****************************************************************************************************/
/* CGPS7_39SendCapabilityInd :  */
/*****************************************************************************************************/
void CGPS7_39SendCapabilityInd(uint8_t vp_Initial)
{
    if(vp_Initial)
    {
        t_lsimcp_PosCapabilityIndM * pl_PosCapInd;
        pl_PosCapInd    =   (t_lsimcp_PosCapabilityIndM*)MC_RTK_GET_MEMORY(sizeof(* pl_PosCapInd));


        MC_CGPS_TRACE(("CGPS7_39SendCapabilityInd"));
        pl_PosCapInd->v_Initial = vp_Initial;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                   PROCESS_LSIMCP, 0,
                                   CGPS_LSIMCP_POS_CAPABILITY_IND,
                                   (t_MsgHeader *)pl_PosCapInd);

    }


    return ;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 40
/*****************************************************************************************************/
/* CGPS7_40MolfStart :  */
/*****************************************************************************************************/
uint8_t CGPS7_40MolfStart(t_CgpsNaf* pl_Handle , t_CGPS_MolfStartInd *pl_CGPS_MolfStartInd)
{
    t_lsimcp_MolrStartIndM* pl_MolrStartInd = NULL;
    t_CgpsCpMoAppli*        pl_Instance     = NULL;

    pl_Instance = CGPS7_28CpMoInstanceNew((void *)pl_Handle, K_CGPS_CPMO_SESSION_TYPE_MOLF);
    if (pl_Instance == NULL) return FALSE;


    pl_MolrStartInd =    (t_lsimcp_MolrStartIndM*)MC_RTK_GET_MEMORY(sizeof(*pl_MolrStartInd));

    if(pl_MolrStartInd == NULL) return FALSE;

    memset(pl_MolrStartInd, 0, sizeof(*pl_MolrStartInd));

    /*Updating the Assistance Data information*/
    /*No need to udpate Assistance data information as this is not required in MOLF*/

   /*Updating the QoP Parameters*/
    if ( (NULL != pl_Handle) && (pl_Handle->v_Mode != K_CGPS_PERIODIC))
    {
        t_cgps_QopIndexFormat v_QoP;

        CGPS4_36ConvertQoP(pl_Handle , &v_QoP, CGPS_CP_CONNECTION_LATENCY );
        pl_MolrStartInd->v_QopRequest.v_Horacc    =    v_QoP.v_Horacc;
        pl_MolrStartInd->v_QopRequest.v_Veracc    =    v_QoP.v_Veracc;
        pl_MolrStartInd->v_QopRequest.v_Delay     =    v_QoP.v_Delay;
        pl_MolrStartInd->v_QopRequest.v_MaxLocAge =    v_QoP.v_MaxLocAge;
        pl_MolrStartInd->v_Option |= K_LSIMCP_MOLR_OPT_QUALITYREQ;

    }


    /*Updating the Client Information*/

    pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdConfig = pl_CGPS_MolfStartInd->vp_ClientInfo.v_ClientIdConfig;
    pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdTonNpi = pl_CGPS_MolfStartInd->vp_ClientInfo.v_ClientIdTonNpi;


    pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumConfig = pl_CGPS_MolfStartInd->vp_ClientInfo.v_MlcNumConfig;
    pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumTonNpi = pl_CGPS_MolfStartInd->vp_ClientInfo.v_MlcNumTonNpi;


    /*Update Client Information*/
    if(pl_CGPS_MolfStartInd->vp_ClientInfo.v_ClientExtIdLen > 0)
    {


        if(pl_CGPS_MolfStartInd->vp_ClientInfo.v_ClientExtIdLen >= LSIMCP_MAX_PHONE_NUM_LENGTH)
            pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdPhoneNumberLen= LSIMCP_MAX_PHONE_NUM_LENGTH - 1;
        else
            pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdPhoneNumberLen = pl_CGPS_MolfStartInd->vp_ClientInfo.v_ClientExtIdLen;


        memcpy(pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdPhoneNumber,
               pl_CGPS_MolfStartInd->vp_ClientInfo.a_ClientExtId,
               pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdPhoneNumberLen);

        pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdPhoneNumber[pl_MolrStartInd->v_3rdPartyRequest.v_ClientIdPhoneNumberLen] = '\0';
    }

    /*Update MLC number Information*/
    if(pl_CGPS_MolfStartInd->vp_ClientInfo.v_MlcNumberLen > 0)
    {
        if(pl_CGPS_MolfStartInd->vp_ClientInfo.v_MlcNumberLen >= LSIMCP_MAX_PHONE_NUM_LENGTH)
         pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumPhoneNumberLen = LSIMCP_MAX_PHONE_NUM_LENGTH -1;
        else
         pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumPhoneNumberLen = pl_CGPS_MolfStartInd->vp_ClientInfo.v_MlcNumberLen;

        memcpy(pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumPhoneNumber,
            pl_CGPS_MolfStartInd->vp_ClientInfo.a_MlcNumber,pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumPhoneNumberLen);

        pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumPhoneNumber[pl_MolrStartInd->v_3rdPartyRequest.v_MlcNumPhoneNumberLen] = '\0';
    }

    pl_MolrStartInd->v_Option |= K_LSIMCP_MOLR_OPT_THIRDPARTY;


    /*Updating Session information*/
    pl_MolrStartInd->v_SsSessionID = pl_Instance->v_SessionId;
    pl_MolrStartInd->v_MolrType =    pl_Instance->v_SessionType;

    pl_MolrStartInd->v_ServiceType = pl_CGPS_MolfStartInd->vp_ServType;
    pl_MolrStartInd->v_Option |= K_LSIMCP_MOLR_OPT_SERVICETYPE;


    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                   PROCESS_LSIMCP, 0,
                                   CGPS_LSIMCP_MOLR_START_IND,
                                   (t_MsgHeader *)pl_MolrStartInd);


    return TRUE;
}


/*LMSQCMOLR*/
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 41
/*****************************************************************************************************/
/* CGPS7_41NotifyAbortInd :  */
/*****************************************************************************************************/
const t_OperationDescriptor* CGPS7_41NotifyAbortInd(t_RtkObject* p_FsmObject)
{
    t_lsimcp_LocNotifyAbortIndM * pl_NotifAbortInd = (t_lsimcp_LocNotifyAbortIndM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    const t_OperationDescriptor* pl_NextState = SAME;
    uint8_t vl_index=0;
    bool    vl_NotificationFound = FALSE;

    MC_CGPS_TRACE(("CGPS7_41NotifyAbortInd : Start"));

    if( vg_CGPS_Notification_Count )
    {

        for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_NOTIFICATIONS ; vl_index++)
        {
            /* Check for Session Id and check if notification is still being handled in CGPS */
            if ( s_CgpsNotification[vl_index].v_Handle == pl_NotifAbortInd->v_SsSessionID
                 && s_CgpsNotification[vl_index].v_IsPending )
            {
                 vl_NotificationFound = TRUE;
                 break;
            }
        }

        /* if no more handle is available, the notification is not found */
        if (!vl_NotificationFound)
        {
            MC_CGPS_TRACE(("CGPS7_41NotifyAbortInd : Notification Handle not found or cleared from CGPS!!!"));
        }
        else
        {
            /* reset notification context */
            vg_CGPS_Notification_Count--;
            s_CgpsNotification[vl_index].v_IsPending  = FALSE;
            s_CgpsNotification[vl_index].v_Handle     = 0;
            s_CgpsNotification[vl_index].v_Timeout    = 0;
            s_CgpsNotification[vl_index].v_DefaultNotificationType = K_CGPS_NOTIFICATION_ONLY;

            CGPS0_52ConfigureCgpsNextState();
        }
    }
    return pl_NextState;
}


#ifdef AGPS_TIME_SYNCH_FTR
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 42
/*******************************************************************************************************/
/* CGPS7_42FtaCheckAndRequestPulse : Request for frame sync pulse for FTA                                                      */
/*******************************************************************************************************/
void CGPS7_42FtaCheckAndRequestPulse()
{
    if( vg_CGPS_Cp_Assitance_No_Request || (vg_CGPS_Cp_Count > 0 ) )
    {
        uint8_t  vl_IsSendFtaReq = FALSE;
        if( (vg_TimeSynchRequest_ms == 0)&& ((vg_AssistanceAidMask & LSIMCP_AID_REQ_REF_TIME) == 0 ))
            {

            t_lsim_FtaPulseReqM *pl_lsimcp_FtaPulseReqM = MC_RTK_GET_MEMORY(sizeof(*pl_lsimcp_FtaPulseReqM));
            if(pl_lsimcp_FtaPulseReqM == NULL)
            {
               MC_CGPS_TRACE(("CGPS7_42FtaCheckAndRequestPulse : Unable to allocate Memory pulse will not be Requested"));
               return;
            }
            MC_CGPS_TRACE(("CGPS7_42FtaCheckAndRequestPulse : Ref Time has been delivered. Request for pulse. AsstnDelvdTimeout = %d NumOfCPSession = %d", \
                            vg_CGPS_Cp_Assitance_No_Request,vg_CGPS_Cp_Count));

            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_LSIMCP,0,CGPS_LSIMCP_FTA_PULSE_REQ,(t_MsgHeader*) pl_lsimcp_FtaPulseReqM );

            vg_TimeSynchRequest_ms =  GN_GPS_Get_OS_Time_ms();
        }
        else if(vg_TimeSynchRequest_ms !=0)
        {
           uint32_t vl_CurrentTime_ms = GN_GPS_Get_OS_Time_ms();
           if((vl_CurrentTime_ms - vg_TimeSynchRequest_ms) > CGPS_FTA_PULSE_REQ_TIME_MS )
           {
               vg_TimeSynchRequest_ms = 0;
               MC_CGPS_TRACE(("CGPS7_42FtaCheckAndRequestPulse : Four Seconds elapsed since last request Setting the Request flag"));
           }
        }
    }
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 43
/*******************************************************************************************************/
/* CGPS7_42FtaCheckAndRequestPulse : Reception of fine time assistance                                                 */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS7_43FtaPulseCnf(t_RtkObject* p_FsmObject)
{
    t_lsim_FtaPulseCnfM *pl_FtaPulseCnf;
    int8_t vl_Ret;

    pl_FtaPulseCnf = (t_lsim_FtaPulseCnfM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    if ( (pl_FtaPulseCnf->v_IsRefTimeValid == TRUE))
    {
           s_GN_AGPS_EFSP_Time p_EFSPT;
           if( (vg_AssistanceAidMask & LSIMCP_AID_REQ_REF_TIME) == 0 )
           {

              double    vl_GpsTowMilliSec = ((double)pl_FtaPulseCnf->v_RefTime.v_GpsTow + pl_FtaPulseCnf->v_RefTime.v_RefDeltaCellTime/1000.0);
              uint16_t  vl_WeekNo         = pl_FtaPulseCnf->v_RefTime.v_GpsWeek;

              if(vl_GpsTowMilliSec > CGPS_TOW_ROLLOVER_TIME_MS)
              {
                    vl_GpsTowMilliSec -= CGPS_TOW_ROLLOVER_TIME_MS;

                    vl_WeekNo += 1;

                    if(vl_WeekNo > CGPS_WEEK_ROLLOVER_VALUE)
                    {
                          vl_WeekNo -= CGPS_WEEK_ROLLOVER_VALUE;

                    }
              }
              MC_CGPS_TRACE(("CGPS7_43FtaPulseCnf : v_GpsTow Milli Sec <%g> , v_GpsWeek <%u>", vl_GpsTowMilliSec,vl_WeekNo));

              p_EFSPT.TOW        =       vl_GpsTowMilliSec/1000.0;
              p_EFSPT.WeekNo     =       vl_WeekNo;
              p_EFSPT.Abs_RMS_us =       pl_FtaPulseCnf->v_RefTime.v_Prec;
              p_EFSPT.Rel_RMS_ns =       0xFFFFFFFF;
              p_EFSPT.OS_Time_ms =       GN_GPS_Get_OS_Time_ms();
              p_EFSPT.MultiplePulses =   TRUE;

              MC_CGPS_TRACE(("CGPS7_43FtaPulseCnf : TOW %lf, WeekNo %u, Abs_RMS_us %d, Rel_RMS_ns %d, OS_Time_ms = %d, MultiplePulses %d\n", \
                  p_EFSPT.TOW, p_EFSPT.WeekNo, p_EFSPT.Abs_RMS_us, p_EFSPT.Rel_RMS_ns, p_EFSPT.OS_Time_ms, p_EFSPT.MultiplePulses));

              vl_Ret = GN_AGPS_Set_EFSP_Time(&p_EFSPT);
              if(vl_Ret == TRUE)
              {
                MC_CGPS_TRACE(("CGPS7_43FtaPulseCnf : Fine Time Data accepted by Library"));
              }
              else
              {
                MC_CGPS_TRACE(("CGPS7_43FtaPulseCnf : Fine Time Data Rejected by Library"));
              }

           }
    }
    else
    {
          MC_CGPS_TRACE(("CGPS7_43FtaPulseCnf : "));

    }


    return SAME;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 44
/*******************************************************************************************************/
/* CGPS7_44FtaInitialize : Initialize FTA related globals                                                             */
/*******************************************************************************************************/
void CGPS7_44FtaInitialize()
{

    vg_TimeSynchRequest_ms = 0;
    MC_CGPS_TRACE(("CGPS7_44FtaInitialize"));
}
#endif /* AGPS_TIME_SYNCH_FTR */



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 45
/*******************************************************************************************************/
/* CGPS7_45sendNotification : Handles the notification procedures                                                            */
/*******************************************************************************************************/
static void CGPS7_45sendNotification (t_lsimcp_LocNotifyReqM * pl_NotifReq , uint8_t vl_index)
{


    t_cgps_Notification vl_cgps_Notification;
    memset(&vl_cgps_Notification, 0, sizeof(vl_cgps_Notification));

    /* callback parameters settings */
    vl_cgps_Notification.v_NotificationType = s_CgpsNotification[vl_index].v_DefaultNotificationType;

    vl_cgps_Notification.v_Handle = (void*)(uint32_t)vl_index;

    vl_cgps_Notification.v_RequestorIdLen = pl_NotifReq->v_LocNotification.v_RequestorIdLen;
    memcpy( (char*)vl_cgps_Notification.a_RequestorId,
            (char*)pl_NotifReq->v_LocNotification.a_RequestorId,
            vl_cgps_Notification.v_RequestorIdLen);
    vl_cgps_Notification.v_RequestorIdEncodingType =  CGPS4_23CPNotificationEncodingType(pl_NotifReq->v_LocNotification.v_RequestorIdAlphabet);
    vl_cgps_Notification.v_RequestorIdType = K_CGPS_NO_FORMAT;

    vl_cgps_Notification.v_ClientNameLen = pl_NotifReq->v_LocNotification.v_ClientNameLen;
    memcpy( (char*)vl_cgps_Notification.a_ClientName,
            (char*)pl_NotifReq->v_LocNotification.a_ClientName,
             vl_cgps_Notification.v_ClientNameLen );
    vl_cgps_Notification.v_ClientNameEncodingType =  CGPS4_23CPNotificationEncodingType(pl_NotifReq->v_LocNotification.v_ClientNameAlphabet);
    vl_cgps_Notification.v_ClientNameType = K_CGPS_NO_FORMAT;

    vl_cgps_Notification.v_CodeWordLen = pl_NotifReq->v_LocNotification.v_CodeWordLen;
    memcpy( (char*)vl_cgps_Notification.a_CodeWord,
            (char*)pl_NotifReq->v_LocNotification.a_CodeWord,
             vl_cgps_Notification.v_CodeWordLen);
    vl_cgps_Notification.v_CodeWordAlphabet =  CGPS4_23CPNotificationEncodingType(pl_NotifReq->v_LocNotification.v_CodeWordAlphabet);

    vl_cgps_Notification.v_TonNpi = pl_NotifReq->v_LocNotification.v_TonNpi;
    vl_cgps_Notification.v_TonNpiConfig = pl_NotifReq->v_LocNotification.v_TonNpiConfig;

    if( K_CGPS_MAX_PHONE_NUM_LENGTH <= sizeof(pl_NotifReq->v_LocNotification.a_PhoneNumber) )
    {
        memcpy( (char*)vl_cgps_Notification.a_PhoneNumber,
                (char*)pl_NotifReq->v_LocNotification.a_PhoneNumber,
                K_CGPS_MAX_PHONE_NUM_LENGTH);
    }
    else
    {
        memcpy( (char*)vl_cgps_Notification.a_PhoneNumber,
                (char*)pl_NotifReq->v_LocNotification.a_PhoneNumber,
                sizeof(pl_NotifReq->v_LocNotification.a_PhoneNumber));
    }

    vl_cgps_Notification.v_LcsServiceTypeId = CGPS4_24CPNotificationServiceType(pl_NotifReq->v_LocNotification.v_LcsServiceTypeId);

    vg_NotificationCallback(&vl_cgps_Notification);

    MC_CGPS_TRACE(("CGPS7_45NotifyReq : Notification callback called"));

}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 46
/*******************************************************************************************************/
/* CGPS7_46CheckGPSActiveRequiredForCp : Checks if GPS needs to be active for assistance data delivered without CP request */
/*******************************************************************************************************/
void CGPS7_46CheckGPSActiveRequiredForCp(void)
{
    if( vg_CGPS_Cp_Assitance_No_Request != 0 )
    {
        uint32_t  vl_CurrentTime = GN_GPS_Get_OS_Time_ms();

        if( vl_CurrentTime >= vg_CGPS_Cp_Assitance_No_Request )
        {
             vg_CGPS_Cp_Assitance_No_Request = 0;

             CGPS0_52ConfigureCgpsNextState();
        }
    }
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 47
/*******************************************************************************************************/
/* CGPS7_47StoreGPSMeasurements : Measurement caching for MSA mode of operation */
/*******************************************************************************************************/
static void CGPS7_47StoreGPSMeasurements ( s_GN_AGPS_Meas* pp_Meas, uint8_t  vp_index )
{
    bool vl_UpdateRequired = FALSE;

/*
    Note 1)This function is called only in a CP MSA single shot scenario.
            2)This function is called only if the latest GPS measurement is not meeting the desired quality & TOW Unc requirements.

    The purpose of the function is to update the session database with GPS measurements in following scenarios.
                        MeasNew                           MeasOld in DB
    Case    Quality    GPSTOWUnc                Quality    GPSTOWUnc
     1           x            y                                 x            y
     2           x            y                                 x            Y
     3           X            y                                 x            y
     4           X            y                                 x            Y
     5           x            y                                 X            Y
     Note : X>x & Y>y
     In other possible scenarios, DB is not updated with latest measurement.

    If GPS measurement in DB is older by 3 seconds over latest available measurement, DB is invariably updated.
*/

/* Case 1 2 3 & 4 */
    if (    pp_Meas->Quality          >= s_CgpsCp[vp_index].v_Meas.Quality
         && pp_Meas->Meas_GPS_TOW_Unc <= s_CgpsCp[vp_index].v_Meas.Meas_GPS_TOW_Unc
       )
    {
        vl_UpdateRequired = TRUE;
    }
/*Case 5*/
    else if(    pp_Meas->Quality          < s_CgpsCp[vp_index].v_Meas.Quality
             && pp_Meas->Meas_GPS_TOW_Unc < s_CgpsCp[vp_index].v_Meas.Meas_GPS_TOW_Unc
           )
    {
        vl_UpdateRequired = TRUE;
    }

    if ( pp_Meas->Meas_GPS_TOW - s_CgpsCp[vp_index].v_Meas.Meas_GPS_TOW >= CGPS_MAX_AGE_GPS_MEAS )
    {
        vl_UpdateRequired = TRUE;
    }

    if( vl_UpdateRequired )
    {
        memcpy(&s_CgpsCp[vp_index].v_Meas, (s_GN_AGPS_Meas*)pp_Meas, sizeof(*pp_Meas));
    }

    MC_CGPS_TRACE(("CGPS7_47StoreGPSMeasurements : New Meas Qual %3d TOWUnc %3d TOW %6d, Stored Qual %3d TowUnc %3d TOW %6d", \
                    pp_Meas->Quality, pp_Meas->Meas_GPS_TOW_Unc, pp_Meas->Meas_GPS_TOW,                                       \
                    s_CgpsCp[vp_index].v_Meas.Quality, s_CgpsCp[vp_index].v_Meas.Meas_GPS_TOW_Unc, s_CgpsCp[vp_index].v_Meas.Meas_GPS_TOW));

    return;
}




#endif /*AGPS_FTR*/
#undef __CGPS7CP_C__

