/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSCP_HIP__
#define __CGPSCP_HIP__
/**
* \file cgpscp.hip
* \date 21/07/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by cgps7cp.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 21.07.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/**
* \fn const t_OperationDescriptor* CGPS7_00MsAssistedReq(t_RtkObject* p_FsmObject)
* \brief MS assisted request from LSIMCP
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS7_00MsAssistedReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS7_01MsBasedReq(t_RtkObject* p_FsmObject)
* \brief MS based request from LSIMCP
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS7_01MsBasedReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS7_02AbortReq(t_RtkObject* p_FsmObject)
* \brief Deregister a CP application
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS7_02AbortReq(t_RtkObject* p_FsmObject);

/**
* \fn void CGPS7_03SendDataToCpAppli()
* \brief Send data (position fix or measurements) to all elected CP applications
*/
void CGPS7_03SendDataToCpAppli(void);


/**
* \fn const t_OperationDescriptor*  CGPS7_05AssistanceDataAReq(t_RtkObject* p_FsmObject)
* \brief Reception of assistance data
*/
const t_OperationDescriptor* CGPS7_05AssistanceDataAReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor*  CGPS7_06TimeReq(t_RtkObject* p_FsmObject)
* \brief Reception of time assistance
*/
const t_OperationDescriptor* CGPS7_06TimeReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor*  CGPS7_07RefPosReq(t_RtkObject* p_FsmObject)
* \brief Reception of reference position assistance
*/
const t_OperationDescriptor* CGPS7_07RefPosReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor*  CGPS7_08AssistanceDataBReq(t_RtkObject* p_FsmObject)
* \brief Reception of assistance data
*/
const t_OperationDescriptor* CGPS7_08AssistanceDataBReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS7_09AlmanacDataReq(t_RtkObject* p_FsmObject)
* \brief Reception of almanac data
*/
const t_OperationDescriptor* CGPS7_09AlmanacDataReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS7_10AssistIonoModelReq(t_RtkObject* p_FsmObject)
* \brief Reception of ionospheric model assistance
*/
const t_OperationDescriptor* CGPS7_10AssistIonoModelReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS7_11AssistDGPSCorrectionReq(t_RtkObject* p_FsmObject)
* \brief Reception of DGPS correction
*/
const t_OperationDescriptor*  CGPS7_11AssistDGPSCorrectionReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS7_12AssistUTCModelReq(t_RtkObject* p_FsmObject)
* \brief Reception of UTC model
*/
const t_OperationDescriptor* CGPS7_12AssistUTCModelReq(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS7_13AssistRTIReq(t_RtkObject* p_FsmObject)
* \brief Reception of Real Time Integrity
*/
const t_OperationDescriptor* CGPS7_13AssistRTIReq(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS7_14ECallStart(t_RtkObject* p_FsmObject)
*\brief  This function is used to start GPS service as soon asa an emergency call is established
* 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS7_14ECallStart(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS7_14ECallStart(t_RtkObject* p_FsmObject)
* \brief This function is used to advert CGPS that emergency call is finished
* 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS7_15ECallStop(t_RtkObject* p_FsmObject);



/**
* \fn uint8_t CGPS7_16SearchCPHandle(uint8_t vp_SessionId, uint8_t * pp_Index)
* \brief search a free handle for CP application    
* \param vp_SessionId session ID of the request
* \param pp_Index[out] Index found
* \param vp_IsExistingSession[out] Set to TRUE if this session already exist
* \return return TRUE if an index (new one or existing)was found.
*/
uint8_t CGPS7_16SearchCPHandle(uint8_t vp_SessionId, uint8_t * pp_Index, uint8_t* vp_IsExistingSession);

/**
* \fn void CGPS7_17CPAssistanteDataRequest()
* \brief ask LSIM for assistance data from network if it's require by the GPS library
*/
void   CGPS7_17CPAssistanteDataRequest( uint8_t vp_sessionId );

/**
* \fn void CGPS7_18CPSendMSAssistedData(s_GN_AGPS_Meas * pp_Meas,uint8_t vp_sessionId);
* \brief  send ms assisted data to an Network application    
* \param vp_sessionId session ID of the request
* \param pp_Meas  Assisted data to send
*/
void CGPS7_18CPSendMSAssistedData(s_GN_AGPS_Meas * pp_Meas,uint8_t vp_sessionId);

/**
* \fn void CGPS7_16SearchCPHandle(uint8_t vp_SessionId, uint8_t * pp_Index)
* \brief  send ms based data to an Network application
* \param vp_GAD  Pointer to the output GAD Data.
* \param vp_GAD_Ref_TOW GAD Data Reference GPS Time of Week 
* \param vp_EFSP_dT_us   Delta-Time to the last received External Frame Sync Pulse
* \param vp_sessionId session ID of the request
*/
void CGPS7_19CPSendMSBasedData(s_GN_AGPS_GAD_Data * vp_GAD,uint32_t vp_GAD_Ref_TOW, int16_t vp_GAD_Ref_TOW_Subms, int32_t vp_EFSP_dT_us,uint8_t vp_sessionId);

/**
*\fn uint8_t CGPS7_21RcvCPData()
*\brief Start cgps loop and request timeout, if the request is not received when the timer isssue the loop is stopped
* 
*\return True if the loop and the timeout are started.
*/
uint8_t CGPS7_21RcvCPData(void);

/**
*\fn const t_OperationDescriptor* CGPS7_22RequestTimeoutExpired(t_RtkObject* p_FsmObject)
*\brief  the request timeout expired  the loop is stopped if there aren't any session running  
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS7_22RequestTimeoutExpired(t_RtkObject* p_FsmObject);

/**
* \fn t_OperationDescriptor* CGPS7_23NotifyReq(t_RtkObject* p_FsmObject)
* \brief 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS7_23NotifyReq(t_RtkObject* p_FsmObject);

/**
* \fn void CGPS7_24NotifySendCnf(uint32_t vp_SessionID,uint8_t vp_RequestType,uint8_t vp_UserAnswer)
* \brief 
*\param vp_SessionID
*\param vp_RequestType
*\param vp_UserAnswer
*/
void CGPS7_24NotifySendCnf(uint32_t vp_SessionID,uint8_t vp_RequestType,uint8_t vp_UserAnswer);

/**
* \fn void CGPS7_25PopulateQop( uint8_t vp_SessionIndex, t_lsimcp_PosInstruct vp_PosInstruct )
* \brief Populates the CGPS session database with QOP
*\param vp_SessionIndex
*\param vp_PosInstruct
*/
void CGPS7_25PopulateQop( uint8_t vp_SessionIndex, t_lsimcp_PosInstruct vp_PosInstruct );

/**
* \fn void  CGPS7_26AbortSession( uint8_t vp_SessionIndex )
* \brief De-registers the session 
*\param vp_SessionIndex
*/
void CGPS7_26AbortSession( uint8_t vp_SessionIndex );

/* +LMSqcResetUEPosition */
/**
* \fn const t_OperationDescriptor* CGPS7_27ResetUEPosition(t_RtkObject* p_FsmObject)
* \brief 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS7_27ResetUEPosition(t_RtkObject* p_FsmObject);
/* -LMSqcResetUEPosition */

t_CgpsCpMoAppli * CGPS7_28CpMoInstanceNew(void *pl_Handle, e_cgps_CpMoSessionType vl_Type);
t_CgpsCpMoAppli * CGPS7_29CpMoInstanceFetch(uint8_t vl_SessionId, void *vp_NafHandle);
void CGPS7_30CpMoInstanceInit(t_CgpsCpMoAppli * vp_Instance);
void CGPS7_31CpMoInstanceInitAll();
bool CGPS7_32StartCPMO( t_CgpsNaf* pp_Handle );
const t_OperationDescriptor* CGPS7_33MolrAbortReq(t_RtkObject* p_FsmObject);
const t_OperationDescriptor* CGPS7_34MolrStartResp(t_RtkObject* p_FsmObject);
void CGPS7_35UpdateLastPosition(t_lsimcp_GpsPosition *p_Pos);
void CGPS7_36SendMolrStopInd(t_CgpsCpMoAppli *vp_Instance);
void CGPS7_37FillGpsAidRequest(t_lsimcp_GpsAidRequest *p_GpsAidRequest,uint8_t vp_isAssistedSession);
void CGPS7_38CheckMolrSession(t_CgpsNaf *vp_Handle);
void CGPS7_39SendCapabilityInd(uint8_t vp_Initial);
uint8_t CGPS7_40MolfStart(t_CgpsNaf* pl_Handle , t_CGPS_MolfStartInd *pl_CGPS_MolfStartInd);

/**
* \fn t_OperationDescriptor* CGPS7_41NotifyAbortInd(t_RtkObject* p_FsmObject)
* \brief 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS7_41NotifyAbortInd(t_RtkObject* p_FsmObject);
#if defined( AGPS_TIME_SYNCH_FTR )
void  CGPS7_42FtaCheckAndRequestPulse(void);
const t_OperationDescriptor* CGPS7_43FtaPulseCnf(t_RtkObject* p_FsmObject);
void CGPS7_44FtaInitialize();
#endif

void CGPS7_46CheckGPSActiveRequiredForCp(void);



#endif /* __CGPSCP_HIP__ */

