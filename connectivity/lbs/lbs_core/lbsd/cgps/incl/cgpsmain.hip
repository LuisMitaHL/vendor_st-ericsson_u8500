/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSMAIN_HIP__
#define __CGPSMAIN_HIP__
/**
* \file cgpsmain.hip
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by cgps0main.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 07.05.08</TD><TD> Y.DESAPHI </TD><TD> SUPL implementation </TD>
*     </TR>
* </TABLE>
*/

/**
*\fn const t_OperationDescriptor* CGPS0_01timerExpired(t_RtkObject* p_FsmObject)
*\brief function call when the message CGPS_TIMER_ID (timer expired) is received
*
* This function is used to update the GPS library and
* upload GPS library patch.
* 
*\param p_FsmObject parameter use with fsm table not used with this function
*/
const t_OperationDescriptor* CGPS0_01timerExpired(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS0_02timerExpired(t_RtkObject* p_FsmObject)
*\brief Empty function used in fsm table
*
* Do nothing
* 
*\param p_FsmObject parameter use with fsm table not used with this function
*/
const t_OperationDescriptor* CGPS0_02DefaultMessage(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS0_03UTCCorrupted(t_RtkObject* p_FsmObject)
*\brief function calling HALGPS0_05UTCCorrupted
*
* Just calls HALGPS0_05UTCCorrupted with p_FsmObject
* 
*\param p_FsmObject parameter use with fsm table, sent to HALGPS0_05UTCCorrupted
*/
const t_OperationDescriptor* CGPS0_03UTCCorrupted(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS0_15ComaExpired(t_RtkObject* p_FsmObject)
*\brief function called when CGPS_COMA_TIMER expires
* 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS0_14SleepExpired(t_RtkObject* p_FsmObject);

/**
*\fn uint8_t CGPS0_16SendDataToNaf(void)
*\brief manage to send navigation data to all requesting applications
*/
void CGPS0_16SendDataToNaf(void);

/**
*\fn uint16_t CGPS0_17GetNMEA(uint32_t vp_Prev_OS_Time_ms, uint16_t vp_Mask, uint16_t vp_Max_Size, char* pp_NMEA_Buffer )
*\brief Get navigation data as a NMEA buffer
* 
*\param vp_Mask [in] NMEA mask, used to select NMEA information
*\param vp_Max_[in] size maximum size of the buffer
*\param pp_NMEA_Buffer [out] NMEA buffer
*
*\return buffer length
*/
uint16_t CGPS0_17GetNMEA(s_GN_GPS_Nav_Data *vp_navData, uint16_t vp_Mask, uint16_t vp_Max_Size, char* pp_NMEA_Buffer );

/**
*\fn const t_OperationDescriptor* CGPS0_18NewNafRegistered(t_RtkObject* p_FsmObject)
*\brief function called when CGPS_NEW_NAF_REGISTERED_IND is received
* 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS0_18NewNafRegistered(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS0_19NafDeregistered(t_RtkObject* p_FsmObject)
*\brief function called when CGPS_NAF_DEREGISTERED_IND is received
* 
*\param p_FsmObject parameter use with fsm table
*/
const t_OperationDescriptor* CGPS0_19NafDeregistered(t_RtkObject* p_FsmObject);

/**
*\fn bool CGPS0_52ConfigureCgpsNextState();
*\brief function called whenever there is transition of CGPS state
* 
*\param Next CGPS state to be transitioned 
*/
bool CGPS0_52ConfigureCgpsNextState(void);

/**
*\fn void CGPS0_53UpdateSessionConfig()
*\brief function called to update session state
* 
*/
void CGPS0_53UpdateSessionConfig( void);
/**
*\fn const t_OperationDescriptor* CGPS0_68MolfStartReq( t_RtkObject *p_FsmObject )
*\brief function called to start Location Forward request
* 
*/

const t_OperationDescriptor* CGPS0_68MolfStartReq( t_RtkObject *p_FsmObject );

/**
*\fn const t_OperationDescriptor* CGPS0_77ThirdPartyLocReq( t_RtkObject *p_FsmObject )
*\brief function called to start Location Reqtrieve request
*
*/
const t_OperationDescriptor* CGPS0_77ThirdPartyLocReq( t_RtkObject *p_FsmObject );
#if defined AGPS_UP_FTR || defined AGPS_FTR

/**
*\fn const t_OperationDescriptor*  CGPS0_23HandleUserAnswer(t_RtkObject* p_FsmObject)
*\brief function called when the message CGPS_USER_NOTIFY_ANS
*
* This function is used to handle user accept or reject 
*/
const t_OperationDescriptor*  CGPS0_23HandleUserAnswer(t_RtkObject* p_FsmObject);
/**
*\fn void CGPS0_24NotificationExpired(t_RtkObject* p_FsmObject)
*\brief function called when the message CGPS_SUPL_NOTIFICATION_TIMER_EXPIRY (timer expired) is received
*
* This function is used to accept or reject SUPL notification instead of user
*/
void CGPS0_24NotificationExpired(void);

#endif /* AGPS_UP_FTR || AGPS_FTR */

#ifdef AGPS_UP_FTR

/**
*\fn const t_OperationDescriptor* CGPS0_25SmsPushInd(t_RtkObject* p_FsmObject)
*\brief function called when the message CGPS_LSIMUP_SMS_PUSH_IND is received
*
* This function is used to indicate the SUPL INIT reception through SMS
*/
const t_OperationDescriptor* CGPS0_25SmsPushInd(t_RtkObject* p_FsmObject);


/**
*\fn const t_OperationDescriptor* CGPS0_26AdpInitCnf(t_RtkObject* p_FsmObject)
*\brief function called when the message CGPS_LSIMUP_INIT_CNF is received
*
* This function is used to reset the SUPL appli context
*/
const t_OperationDescriptor* CGPS0_26AdpInitCnf(t_RtkObject* p_FsmObject);


/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
/**
*\fn const t_OperationDescriptor* CGPS0_56WapPushInd(t_RtkObject* p_FsmObject)
*\brief function called when the message CGPS_SUPL_PUSH_IND is received
*
* This function is used to handle the the SUPL INIT reception through WAP Push
*/
const t_OperationDescriptor* CGPS0_56WapPushInd(t_RtkObject* p_FsmObject);


/**
*\fn const t_OperationDescriptor* CGPS0_57HandleSuplInit( t_cgps_SuplPush* pp_SuplPush )
*\brief function called to handle SUPL INIT recived via SMS or WAP push
*
* return Indicates if SUPL Init has been rejected
*/
bool CGPS0_57HandleSuplInit( t_cgps_SuplPush* pp_SuplPush );
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */

#endif /* AGPS_UP_FTR */

#ifdef AGPS_TEST_MODE_FTR

/* + LMSqc32518 */
#ifdef ATT_LOGGING_ENABLE
/* - LMSqc32518 */
void CGPS9_00HandleEvents(t_CgpsEventInformation* pp_EventInfo);

/* + LMSqc32518 */
#endif /*ATT_LOGGING_ENABLE*/
/* - LMSqc32518 */
#endif /*AGPS_TEST_MODE_FTR*/

/**
* \fn bool CGPS0_29SearchFreeHandle(t_CgpsRegisterResult ** vp_Result)
* \brief Search a free handle for an application.
* \return return true if a free index was found
* \param pp_Result [out] contain the handle index if it's found. 
*/
bool CGPS0_29SearchFreeHandle(t_CgpsRegisterResult ** pp_Result);

/**
* \fn e_cgps_Error CGPS0_30StartLowLayerGPS(void)
* \brief start low layer of GPS (lib and  HALGPS).
* \return K_CGPS_ERR_GPS_NOT_STARTED if GPS can't start
*/  
e_cgps_Error CGPS0_30StartLowLayerGPS(void);


/**
* \fn uint8_t CGPS0_32StartCGPS(void)
* \brief start GPS if it's not yet done 
* \return FALSE if the GPS can't start
*/
uint8_t CGPS0_32StartCGPS(void);

/**
* \fn void CGPS0_33StartLoop(void)
* \brief Start timer for CGPS loop  and put CGPS in running state     
*/
void CGPS0_33StartLoop(void);

/**
* \fn void CGPS0_34EndLoop()
* \brief if there are no more application stop main loop of cgps and go in sleep mode    .
*/
void CGPS0_34EndLoop( void );

/**
* \fn void CGPS0_36GetNafData(s_GN_GPS_Nav_Data *vp_navData, t_cgps_NavData * pp_NavDataToSend, e_cgps_NavDataType vp_OutputType, uint16_t vp_Mask)
* \brief Get data for a naf application
* \param vp_navData [in] strucure containing data.
* \param pp_NavDataToSend [out] strucure containing data.
* \param vp_OutputType type of data returned to the callback (NMEA or 'C' struct)
* \param vp_NmeaMask Nmea mask
*/
void CGPS0_36GetNafData(s_GN_GPS_Nav_Data *vp_navData, t_cgps_NavData * pp_NavDataToSend, e_cgps_NavDataType vp_OutputType, uint16_t vp_Mask);

/**
* \fn const t_OperationDescriptor* CGPS0_37DirectoryReady(t_RtkObject* p_FsmObject)
* \brief Function call by MEA when the directory used by lower layer is ready
* \param p_FsmObject RTKE object
*/
const t_OperationDescriptor* CGPS0_37DirectoryReady(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor* CGPS0_38GetDirectoryPathCnf(t_RtkObject* p_FsmObject)
* \brief Function call by MEA after that cgps call MAPI_MEA_GET_APPLICATION_PATH_REQ to know the path
* \param p_FsmObject RTKE object
*/
const t_OperationDescriptor* CGPS0_38GetDirectoryPathCnf(t_RtkObject* p_FsmObject);


/**
* \fn const t_OperationDescriptor* CGPS0_39RemoveDirInd(t_RtkObject* p_FsmObject)
* \brief 
* \param p_FsmObject RTKE object
*/
const t_OperationDescriptor*  CGPS0_39RemoveDirInd(t_RtkObject* p_FsmObject);


/**
* \fn const t_OperationDescriptor* CGPS0_40FixRequestedInd(t_RtkObject* p_FsmObject)
* \brief 
* \param p_FsmObject RTKE object
*/
const t_OperationDescriptor* CGPS0_40FixRequestedInd(t_RtkObject* p_FsmObject);


/**
* \fn const t_OperationDescriptor* CGPS0_41InitInd(t_RtkObject* p_FsmObject)
* \brief 
* \param p_FsmObject RTKE object
*/
const t_OperationDescriptor* CGPS0_41InitInd(t_RtkObject* p_FsmObject);

/**
* \fn void CGPS0_44CopyNavData( t_cgps_CStructNavData *pp_Destn , s_GN_GPS_Nav_Data *pp_Src )
* \brief 
* \param pp_Destn
* \param s_GN_GPS_Nav_Data *pp_Src
*/
void CGPS0_44CopyNavData( t_cgps_CStructNavData *pp_Destn , s_GN_GPS_Nav_Data *pp_Src );

/**
* \fn void CGPS0_46UserConfigurationReceived(t_RtkObject* p_FsmObject)
* \brief 
* \param t_RtkObject* p_FsmObject
*/
const t_OperationDescriptor* CGPS0_46UserConfigurationReceived(t_RtkObject* p_FsmObject);

/**
* \fn void CGPS0_70PlatformConfigurationReceived(t_RtkObject* p_FsmObject)
* \brief
* \param t_RtkObject* p_FsmObject
*/
const t_OperationDescriptor* CGPS0_70PlatformConfigurationReceived(t_RtkObject* p_FsmObject);


/**
* \fn void CGPS0_81TraceConfigurationReceived(t_RtkObject* p_FsmObject)
* \brief Updates the logging configuration update receievd from NAF interface
* \param t_RtkObject* p_FsmObject
*/
const t_OperationDescriptor* CGPS0_81TraceConfigurationReceived(t_RtkObject* p_FsmObject);

/**
* \fn const t_OperationDescriptor*  CGPS0_49SaveNVStoreRequested(t_RtkObject* p_FsmObject)
* \brief 
* \param t_RtkObject* p_FsmObject
*/
const t_OperationDescriptor* CGPS0_49SaveNVStoreRequested(t_RtkObject* p_FsmObject);

/* +LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */
void CGPS0_60HwlInitInd(t_RtkObject* p_FsmObject);
/* -LMSqc19757 : LMSqc18522 : Check return value from UTSER macro : GPS */

void CGPS0_60DualosInit(t_RtkObject* p_FsmObject);

/*+ LMSqc13768 : DeleteGpsData */ 
void CGPS0_63DeleteNVStoreFieldsReq(t_RtkObject * p_FsmObject);
/*- LMSqc13768 : DeleteGpsData */

/* + LMSqc32518 */
#ifdef CGPS_UPLOAD_PATCH_AT_INIT_FTR
const t_OperationDescriptor* CGPS0_64UploadPatch( t_RtkObject *p_FsmObject );
#endif /*CGPS_UPLOAD_PATCH_AT_INIT_FTR*/
/* - LMSqc32518 */
/*+ LMSqc48667 */
void CGPS0_65ResetGpsContext();
/*- LMSqc48667 */

#ifdef GPS_OVER_HCI
e_cgps_Error CGPS0_66HciInit();
#endif
/* +SUPLv2.0 Modifications */
#ifdef AGPS_UP_FTR
void CGPS5_39HandleEventSuplTimer0(t_RtkObject* p_FsmObject);
void CGPS5_40HandleEventSuplTimer1(t_RtkObject* p_FsmObject);
void CGPS5_41HandleEventSuplTimer2(t_RtkObject* p_FsmObject);
void CGPS5_42HandleEventSuplTimer3(t_RtkObject* p_FsmObject);
void CGPS5_43HandleEventSuplTimer4(t_RtkObject* p_FsmObject);
#endif
/* -SUPLv2.0 Modifications */

#endif /* __CGPSMAIN_HIP__ */
