/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSUTILS_HIP__
#define __CGPSUTILS_HIP__
/**
* \file cgpsutils.hip
* \date 07/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by cgps4utils.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 07.04.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/**
* \fn uint8_t CGPS4_0IsRegistered(uint32_t vp_Handle)
* \brief This function returns TRUE if the handle is registered
* \param vp_Handle the handle to check
* \return TRUE or FALSE
*/
uint8_t CGPS4_01IsRegistered(uint32_t vp_Handle);


#ifdef AGPS_UP_FTR
/**
* \fn e_cgps_NotificationType CGPS4_02ConvertSuplNotificationType(e_GN_SUPL_Notify_Type vp_GN_SUPL_Notify_Type)
* \brief This function converts e_GN_SUPL_Notify_Type into \ref e_cgps_SuplNotificationType
* \param vp_GN_SUPL_Notify_Type type to convert
* \return \ref e_cgps_SuplNotificationType value
*/
e_cgps_NotificationType CGPS4_02ConvertSuplNotificationType(e_GN_SUPL_Notify_Type vp_GN_SUPL_Notify_Type);

/**
* \fn e_cgps_EncodingType CGPS4_03ConvertEncodingType(e_GN_SUPL_Encoding_Type vp_GN_SUPL_Encoding_Type)
* \brief This function converts e_GN_SUPL_Encoding_Type into \ref e_cgps_EncodingType
* \param vp_GN_SUPL_Encoding_Type type to convert
* \return \ref e_cgps_EncodingType value
*/
e_cgps_EncodingType CGPS4_03ConvertEncodingType(e_GN_SUPL_Encoding_Type vp_GN_SUPL_Encoding_Type);

/**
* \fn e_cgps_FormatIndicator CGPS4_04ConvertFormatIndicator(e_GN_SUPL_Notify_Fmt_Ind vp_GN_SUPL_Notify_Fmt_Ind)
* \brief This function converts e_GN_SUPL_Notify_Fmt_Ind into \ref e_cgps_FormatIndicator
* \param vp_GN_SUPL_Notify_Fmt_Ind type to convert
* \return \ref e_cgps_FormatIndicator value
*/
e_cgps_FormatIndicator CGPS4_04ConvertFormatIndicator(e_GN_SUPL_Notify_Fmt_Ind vp_GN_SUPL_Notify_Fmt_Ind);

/**
* \fn void CGPS4_06ReadString(char * pp_buffer, t_gfl_FILE * pp_File)
* \brief This function reads a line from pp_File and store it into pp_buffer
* \param [out] pp_buffer the buffer where the string is stored
* \param [in] pp_File stream to read
*/
void CGPS4_06ReadString(char ** pp_buffer, void * pp_File);

/**
* \fn t_lsimup_AddressType CGPS4_07ConvertTcpIpAddressType(e_TcpIp_AddressType vp_GN_SUPL_TcpIp_AddressType)
* \brief This function converts e_TcpIp_AddressType into t_lsimup_AddressType
* \param vp_GN_SUPL_TcpIp_AddressType type to convert
* \return t_lsimup_AddressType type value
*/
t_lsimup_AddressType CGPS4_07ConvertTcpIpAddressType(e_TcpIp_AddressType vp_GN_SUPL_TcpIp_AddressType);

/**
* \fn uint8_t CGPS4_08GetNMR(s_NMRElement ** pp_NMRList, t_lsimup_NMR * pp_NMR)
* \brief This function gets NMR list from a chain list and stores it to an array of s_NMRElement (SUPL lib type)
* \param pp_NMRList [out] array where NMR list is stored
* \param pp_NMR first element of the chainlist
* \param vp_nbCells number of cells in the chain list
* \return number of elements in pp_NMRList
*/
uint8_t CGPS4_08GetNMRList(s_NMRElement ** pp_NMRList, t_lsimup_NMR * pp_NMR, uint32_t vp_nbCells);

/**
* \fn uint8_t CGPS4_09GetMeasuredResultsList(s_MeasuredResult ** pp_MeasuredResultsList, t_lsimup_MeasuredResultsList * pp_lsimup_MeasuredResult)
* \brief This function gets measured results list from a chain list and stores it to an array of s_MeasuredResult (SUPL lib type)
* \param pp_MeasuredResultsList [out] array where measured results list is stored
* \param pp_lsimup_MeasuredResult first element of the chainlist
* \param vp_nbCells number of cells in the chain list
* \param vp_CellType cell type (WCDMA or TDSCDMA)
* \return number of elements in pp_MeasuredResultsList
*/
uint8_t CGPS4_09GetMeasuredResultsList(s_MeasuredResult ** pp_MeasuredResultsList, t_lsimup_MeasuredResultsList * pp_lsimup_MeasuredResult, uint32_t vp_nbCells,uint8_t vp_CellType);

#endif /*AGPS_UP_FTR*/


/**
* \fn void CGPS4_10SetLbsHostVersion( void )
* \brief This function sets the host version in GPS library contained in global variable. 
*\ The version information in vg_Lbs_Host_Version is set with this API. Caller needs to take care of validity of vg_Lbs_Host_Version 
* \return void
*/
void CGPS4_10SetLbsHostVersion( void );


/**
* \fn void CGPS4_10SendAcknowledge(const char* pp_Ack)
* \brief This function sends an acknowledge to all registered applications
* \param pp_Ack acknowledge string
*/
void CGPS4_11SendAcknowledge(const char* pp_Ack);

#ifdef AGPS_UP_FTR
/**
* \fn void CGPS4_13DeleteSuplContext(void* vp_Handle)
* \brief This function deletes the context dedicated to an external application
* \param vp_Handle external application handle
*/
void CGPS4_13DeleteSuplContext(void* vp_Handle);

/**
* \fn uint8_t CGPS4_14GetWCDMACellMeasure(s_CellMeasuredResult ** pp_CellMeasuredResultsList, t_lsimup_CellMeasuredResultsList * pp_lsimup_CellMeasuredResult)
* \brief This function gets WCDMA cell measured results list from a chain list and stores it to an array of s_CellMeasuredResult (SUPL lib type)
* \param pp_CellMeasuredResultsList the array to fill
* \param pp_lsimup_CellMeasuredResult the chainlist to get
* \param vp_nbCells number of cells in the chain list
* \param vp_CellType cell type (WCDMA or TDSCDMA)
* \return number of elements in pp_CellMeasuredResultsList
*/
uint8_t CGPS4_14GetWCDMACellMeasure(s_CellMeasuredResult ** pp_CellMeasuredResultsList, t_lsimup_cellMeasuredResultsList * pp_lsimup_CellMeasuredResult, uint32_t vp_nbCells, uint8_t vp_CellType);

/**
* \fn uint8_t CGPS4_15GetTDSCDMACellMeasure(s_CellMeasuredResult ** pp_CellMeasuredResultsList, t_lsimup_CellMeasuredResultsList * pp_lsimup_CellMeasuredResult)
* \brief This function gets TDSCDMA cell measured results list from a chain list and stores it to an array of s_CellMeasuredResult (SUPL lib type)
* \param pp_CellMeasuredResultsList the array to fill
* \param pp_lsimup_CellMeasuredResult the chainlist to get
* \param vp_nbCells number of cells in the chain list
* \return number of elements in pp_CellMeasuredResultsList
*/
uint8_t CGPS4_15GetTDSCDMACellMeasure(s_CellMeasuredResult ** pp_CellMeasuredResultsList, t_lsimup_cellMeasuredResultsList * pp_lsimup_CellMeasuredResult, uint32_t vp_nbCells);

#endif /*AGPS_UP_FTR*/

#ifdef AGPS_FTR

/**
* \fn uint8_t CGPS4_16GetShapeType(t_lsimcp_GpsPosition vp_GpsPos)
* \brief This function returns the shape type according to available fields
* \param vp_GpsPos Structure containing all needed fields
* \return LSIMCP_EP, LSIMCP_EP_UNC_CIRCLE, LSIMCP_EP_UNC_ELLIP, LSIMCP_EP_ALT, LSIMCP_EP_ALT_UNC_ELLIP or LSIMCP_EP_ARC    
*/
uint8_t CGPS4_16GetShapeType(t_lsimcp_GpsPosition *vp_GpsPos);

#endif /* AGPS_FTR */

/**
* \fn void CGPS4_17UpdateOperatorID(uint16_t vp_Mcc,uint16_t vp_Mnc)
* \brief This function update the index of the operator rules table from the MCC and MNC
* \param vp_Mcc the MCC of the operator
* \param vp_Mnc the MNC of the operator
*/
void CGPS4_17UpdateOperatorID(uint16_t vp_Mcc,uint16_t vp_Mnc);

/**
* \fn uint8_t CGPS4_18CheckNewSessionRight(e_cgps_cp_application_id vp_ApllicationType)
* \brief This function ceck if a new session can start or not
* \param vp_ApllicationType the Application wanting to start
* \return TRUE if the application can start
*/
uint8_t CGPS4_18CheckNewSessionRight(e_cgps_cp_application_id vp_ApllicationType);

/**
* \fn void CGPS4_19UpdateOtherApplicationState(e_cgps_cp_application_id vp_newApplicationId)
* \brief Suspend old application if it's needed, depending which new application must be launch
* \param vp_ApllicationType the Application wanting to start
*/
void CGPS4_19UpdateOtherApplicationState(e_cgps_cp_application_id vp_newApplicationId);

/**
* \fn void CGPS4_20CloseSessionOperatorRules(e_cgps_cp_application_id vp_closeApplicationId)
* \brief  One session is close check if another session must be activated ( not suspended state )
* \param vp_ApllicationType the closed Application 
* \return if return false we have a problem with operator rules 
*/
uint8_t CGPS4_20CloseSessionOperatorRules(e_cgps_cp_application_id vp_closeApplicationId);


/**
* \fn void CGPS4_20CloseSessionOperatorRules(e_cgps_cp_application_id vp_closeApplicationId)
* \brief  set all session suspended state 
* \param vp_applicationId Type secify the application that needed to update
* \vp_state state wanted 0 not suspended 1 suspended
*/
void CGPS4_21SetSuspendedState(e_cgps_cp_application_id vp_applicationId,uint8_t vp_state);

/**
* \fn void CGPS4_22ActivateOneSession(e_cgps_cp_application_id vp_applicationId)
* \brief Activate one session using the priority
* \param vp_applicationId Type secify the application that needed to update
*/
void CGPS4_22ActivateOneSession(e_cgps_cp_application_id vp_applicationId);


#ifdef AGPS_FTR
e_cgps_EncodingType CGPS4_23CPNotificationEncodingType(uint8_t vp_CPAlphabet);

e_cgps_LcsServiceType CGPS4_24CPNotificationServiceType(uint8_t vp_CPService);
#endif

/**
* \fn void CGPS4_25EnQueueMessage(t_ProcessIdentity v_SrcProcess, t_ProcessIdentity v_DstProcess,t_OperationType v_Event, uint8_t* p_Data);
* \brief Enqueue message 
*/
void CGPS4_25EnQueueMessage(t_ProcessIdentity v_SrcProcess, t_ProcessIdentity v_DstProcess,t_OperationType v_Event, uint8_t* p_Data);

/**
* \fn void CGPS4_26DeQueueMessage(void)
* \brief Send all message in the queue
*/
void CGPS4_26DeQueueMessage(void);

/**
* \fn void CGPS4_27EmptyQueueMessage(void)
* \brief Release all message from the queue
*/
void CGPS4_27EmptyQueueMessage(void);
/**
* \fn void CGPS4_28SendMsgToProcess(t_ProcessIdentity v_SrcProcess, t_ProcessIdentity v_DstProcess,t_OperationType v_Event, uint8_t* p_Data)
* \brief Send Message to LSIM only if its not emergency call else it enqueue the messages.
*/

void CGPS4_28SendMsgToProcess(t_ProcessIdentity v_SrcProcess, t_ProcessIdentity v_DstProcess,t_OperationType v_Event, t_MsgHeader * p_Data);

/* +LMSqb93714 */
/**
* \fn void CGPS4_29SaveTCXOCalibration(void)
* \brief Saves the TCXO calibration for future use
*/
void CGPS4_29SaveTCXOCalibration( void );

/**
* \fn void CGPS4_30RestoreTCXOCalibration(void)
* \brief Restores the TCXO calibration using previously saved value
*/
void CGPS4_30RestoreTCXOCalibration(void);

/**
* \fn void CGPS4_31ClearNVStore( int8_t * Items) 
* \brief  Clears the NV store maintained in GPS library based on selected items
* \param Items -  String describing the Data items.
*/
void CGPS4_31ClearNVStore( int8_t * Items) ;
/* -LMSqb93714 */
/*+SYScs46505*/
/**
* \fn void CGPS4_32CheckAssistanceNeedInTracking();
* \brief  Check for Assistance Required in Tracking mode
*/
void CGPS4_32CheckAssistanceNeedInTracking();
/**
* \fn void CGPS4_33InitializeAssistTracking(t_cgps_AssistTracking* vl_Assist) 
* \brief  Initialize Assistance Tracking
* \param vl_Assist -  Context of Tracking assistance database.
*/
void CGPS4_33InitializeAssistTracking(t_cgps_AssistTracking* vl_Assist);
/**
* \fn void CGPS4_34UpdateLastSUPLSessionTime(t_cgps_AssistTracking* vl_Assist)
* \brief  Update Last SUPL session timestamp
* \param vl_Assist -  Context of Tracking assistance database.
*/
void CGPS4_34UpdateLastSUPLSessionTime(t_cgps_AssistTracking* vl_Assist);
/**
* \fn void CGPS4_35UpdateNumSuplFailure(t_cgps_AssistTracking* vl_Assist)
* \brief  Update Number of SUPL Failure in a cell
* \param vl_Assist -  Context of Tracking assistance database.
*/
void CGPS4_35UpdateNumSuplFailure(t_cgps_AssistTracking* vl_Assist);


/*-SYScs46505*/
/**
* \fn void CGPS4_36ConvertQoP( t_CgpsNaf* pp_Handle , t_cgps_QopIndexFormat * pp_QoP, uint8_t vl_Latency )
* \brief  Converts the quality parameters to Scaled format.
*/
void CGPS4_36ConvertQoP( t_CgpsNaf* pp_Handle , t_cgps_QopIndexFormat * pp_QoP, uint8_t vl_Latency );
/**
* \fn void CGPS4_33UserConfigurationFileRead()
* \brief This function is called to update the user configuration from the\n
* \Configuration file\n
* \return none
*/

void CGPS4_33UserConfigurationFileRead( void );


/**
* \fn void CGPS4_34UserConfigurationFileWrite()
* \brief This function is called to update the user configuration from the\n
* \database to configuration file\n
* \return none
*/

void CGPS4_34UserConfigurationFileWrite( void );

/**
* \fn void CGPS4_35LoggingConfigurationFileRead()
* \brief This function is called to update the logging configuration from the\n
* \Configuration file\n
* \return none
*/

void CGPS4_35LoggingConfigurationFileRead(void);

/**
* \fn void CGPS4_42PlatformConfigurationFileRead()
* \brief This function is called to update the platform configuration from the\n
* \Configuration file\n
* \return none
*/

void CGPS4_42PlatformConfigurationFileRead(void);

/**
* \fn void CGPS4_37EslpAddressPopulate(t_cgps_Config  vl_Config,uint8_t vl_LengthESlpAddr)
* \brief This function is called to Populate the ESLP Addresses\n
* \ from the User Configuration File\n
* \return none
*/
void CGPS4_37EslpAddressPopulate(t_cgps_Config  vl_Config,uint8_t vl_LengthESlpAddr);

/**
* \fn void CGPS4_38UpdateConfiguration()
* \brief This function update the configuration to Position Engine\n
* \return none
*/
void CGPS4_38UpdateConfiguration();
/**
* \fn void CGPS4_39PGNVCommandsHandler()
* \brief This function handles the PGNV commands\n
* \return none
*/
void CGPS4_39PGNVCommandsHandler();
/**
* \fn void CGPS4_40ConfigurationPrint()
* \brief This function is used to print the configuration values\n
* \return none
*/
void CGPS4_40ConfigurationPrint();

/**
CGPS4_41CheckConfigToolVersionMismatch()
* \brief This function is used to print the configuration according to the version of config tool\n
* \return none
*/
void CGPS4_41CheckConfigToolVersionMismatch();


void CGPS4_42PlatformConfigurationFileRead();


/**
CGPS4_43ExtLNAConfig(bool vl_EnableLNA)
* \brief This function is used to to set the External LNA setting to GPS\n
* \return none
*/

void CGPS4_43ExtLNAConfig(bool vl_EnableLNA);

/**
CGPS4_44CheckLocationInGeoArea()
* \brief This function is checks if the location provided is within the defined area provided\n
* \return none
*/
e_cgps_RelativePosition CGPS4_44CheckLocationInGeoArea(t_cgps_GeographicTargetArea* pp_GeoAreaDefn, 
                                                       t_Cgps_Coordinate*           pp_CurrentPosition);



/**
CGPS4_45CheckLocationInCircle()
* \brief This function is checks if the location provided is within the defined circle provided\n
* \return none
*/
e_cgps_RelativePosition CGPS4_45CheckLocationInCircle( uint32_t  vp_Latitude, 
                                                       uint8_t   vp_LatSign, 
                                                       int32_t   vp_Longitude,
                                                       uint32_t  vp_CircularRadius,     
                                                       uint32_t  vp_CircularRadiusMin,
                                                       uint32_t  vp_CircularRadiusMax,  
                                                       t_Cgps_Coordinate* pp_CurrentPosition );


/**
CGPS4_46CheckLocationInEllipse()
* \brief This function is checks if the location provided is within the defined ellipse provided\n
* \return none
*/
e_cgps_RelativePosition CGPS4_46CheckLocationInEllipse(
                                                        uint32_t vp_Latitude,
                                                        uint8_t  vp_LatSign,
                                                        int32_t  vp_Longitude,
                                                        uint8_t  vp_EllipSemiMinorAngle,
                                                        uint32_t vp_EllipSemiMajor,           /**< Describes the Semi Major of Ellipse */
                                                        uint32_t vp_EllipSemiMajorMin,        /**< Describes the Minimum Semi Major of Ellipse */
                                                        uint32_t vp_EllipSemiMajorMax,        /**< Describes the Maximum Semi Major of Ellipse */
                                                        uint32_t vp_EllipSemiMinor,           /**< Describes the Semi Minor of Ellipse */
                                                        uint32_t vp_EllipSemiMinorMin,        /**< Describes the Minimum Semi Minor of Ellipse */
                                                        uint32_t vp_EllipSemiMinorMax,        /**< Describes the Maximum Semi Minor of Ellipse */
                                                        t_Cgps_Coordinate*  pp_CurrentPosition
                                                        );

/**
CGPS4_47CheckLocationInPolygon()
* \brief This function is checks if the location provided is within the defined ellipse provided\n
* \return none
*/
    e_cgps_RelativePosition CGPS4_47CheckLocationInPolygon( uint32_t*  pp_Latitude,
                                                            uint8_t*   pp_LatSign,
                                                            int32_t*   pp_Longitude,
                                                            uint8_t    vp_NumPoint,
                                                            uint32_t   vp_Hysteresis,
                                                            t_Cgps_Coordinate*  pp_CurrentPosition);



bool CGPS4_48CheckForGeoAreaEvent(t_cgps_GeographicTargetArea* pp_GeoAreaDefn,
                           e_cgps_AreaEventType         vp_AreaEventType,
                           e_cgps_RelativePosition      vp_PrevPositionStatus,
                           e_cgps_RelativePosition*     pp_CurrentPositionStatus,
                           t_Cgps_Coordinate*           pp_CurrentPosition);

#ifdef AGPS_UP_FTR

bool CGPS4_56CheckForAreaIdEvent(t_cgps_AreaIdList*   pp_AreaIdDefn,
                           e_cgps_AreaEventType         vp_AreaEventType,
                           e_cgps_RelativePosition      vp_PrevPositionStatus,
                           e_cgps_RelativePosition*     pp_CurrentPositionStatus,
                           s_LocationId*                pp_LocationId);

/**
CGPS4_57CheckLocationInAreaId()
* \brief This function is checks if the location provided is within the defined area provided\n
* \return none
*/
e_cgps_RelativePosition CGPS4_57CheckLocationInAreaId(t_cgps_AreaIdList*   pp_AreaIdDefn,
                                                       s_LocationId*                pp_LocationId);



/* CGPS4_58CheckLocationInGsmAreaId : Check if position is inside a GsmAreaId   */

e_cgps_RelativePosition CGPS4_58CheckLocationInGsmAreaId(
                                                                   uint16_t  vp_GSMRefMcc,
                                                                   uint16_t  vp_GSMRefMnc,
                                                                   uint16_t  vp_GSMRefLac,
                                                                   uint16_t  vp_GSMRefCi,
                                                                   s_LocationId* pp_LocationId
                                                                  );

/* CGPS4_59CheckLocationInWcdmaAreaId : Check if position is inside a WcdmaAreaId   */

e_cgps_RelativePosition CGPS4_59CheckLocationInWcdmaAreaId(
                                                                   uint16_t  vp_WCDMARefMcc,
                                                                   uint16_t  vp_WCDMARefMnc,
                                                                  // uint16_t  vp_WCDMARefLac,area_id_zahid,lac not in structure
                                                                   uint32_t  vp_WCDMARefUc,
                                                                   s_LocationId* pp_LocationId);

/* CGPS4_60CheckLocationInCdmaAreaId : Check if position is inside CdmaAreaId  */

e_cgps_RelativePosition CGPS4_60CheckLocationInCdmaAreaId(
                                                                   uint16_t  vp_CDMARefSid,
                                                                   uint16_t  vp_CDMARefNid,
                                                                   uint16_t  vp_CDMARefBaseid,
                                                                   s_LocationId* pp_LocationId
                                                                  );


#endif


e_cgps_Error  CGPS4_49ValidateLatitudeSign(  uint8_t vp_LatSign );


e_cgps_Error  CGPS4_50ValidateLatitude(  uint32_t vp_Latitude );


e_cgps_Error  CGPS4_51ValidateLongitude(  int32_t vp_Longitude );


e_cgps_Error  CGPS4_52ValidateTargetAreaParams(  t_cgps_GeographicTargetArea* pp_TargetArea );


e_cgps_Error  CGPS4_53ValidatePositioningParams( t_cgps_PositioningParams* pp_PositioningParams );

float CGPS4_54DistanceCalc(float vp_LatRadA, float vp_LongRadA, float vp_LatRadB, float vp_LongRadB);

/**
CGPS4_55SendEventNotify ( e_cgps_GpsStatus vp_Result )
* \brief This function is used to  Send event notification to  Android framework\n
* \return none
*/
void CGPS4_55SendEventNotify ( e_cgps_GpsStatus vp_Result );

#if defined (AGPS_FTR) || defined (AGPS_UP_FTR) || defined (AGPS_HISTORICAL_DATA_FTR)
bool CGPS4_61CheckIfAssistanceRequired(uint16_t *vl_Mask, s_GN_AGPS_Assist_Req *p_AR);
#endif /* AGPS_FTR || AGPS_UP_FTR || AGPS_HISTORICAL_DATA_FTR */


void CGPS4_62HandleStartMode();

void CGPS4_63PrintGPSEphmeris();
void CGPS4_64PrintGlonassEphmeris();


#endif /* __CGPSUTILS_HIP__ */
