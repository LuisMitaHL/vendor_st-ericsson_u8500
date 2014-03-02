//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_asn1_types.h
//
//
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_asn1_types.c
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Typedefs specifically for using the RRLP encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#ifndef INCLUSION_GUARD_SUPL_RRLP_ASN1_TYPES_H
#define INCLUSION_GUARD_SUPL_RRLP_ASN1_TYPES_H

/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

#include "RRLP_PDU.h"

/*
 **************************************************************************
 * Common definitions
 **************************************************************************
*/

#define RRLP_ASN1_REFERENCE_NUMBER_MIN (0)
#define RRLP_ASN1_REFERENCE_NUMBER_MAX (7)

/*
 **************************************************************************
 * Defines and type declarations
 **************************************************************************
*/

// Protocol Error
#define RRLP_ASN1_PROTOCOL_ERROR_EXTENSIONCONTAINER extensionContainer
#define RRLP_ASN1_PROTOCOL_ERROR_EXTENSION rel_5_ProtocolError_Extension

// Measure Position Response
#define RRLP_ASN1_MSR_POSITION_RSP_MULTIPLESETS multipleSets
#define RRLP_ASN1_MSR_POSITION_RSP_REFERENCEIDENTITY referenceIdentity
#define RRLP_ASN1_MSR_POSITION_RSP_OTD_MEASUREINFO otd_MeasureInfo
#define RRLP_ASN1_MSR_POSITION_RSP_LOCATIONINFO locationInfo
#define RRLP_ASN1_MSR_POSITION_RSP_GPS_MEASUREINFO gps_MeasureInfo
#define RRLP_ASN1_MSR_POSITION_RSP_LOCATIONERROR locationError
#define RRLP_ASN1_MSR_POSITION_RSP_EXTENSIONCONTAINER extensionContainer
#define RRLP_ASN1_MSR_POSITION_RSP_REL_98_EXTENSION rel_98_MsrPosition_Rsp_Extension
#define RRLP_ASN1_MSR_POSITION_RSP_REL_5_EXTENSION rel_5_MsrPosition_Rsp_Extension

// Measure Position Request
#define RRLP_ASN1_MSR_POSITION_REQ_REFERENCEASSISTDATA referenceAssistData
#define RRLP_ASN1_MSR_POSITION_REQ_MSRASSISTDATA msrAssistData
#define RRLP_ASN1_MSR_POSITION_REQ_SYSTEMINFOASSISTDATA systemInfoAssistData
#define RRLP_ASN1_MSR_POSITION_REQ_GPS_ASSISTDATA gps_AssistData
#define RRLP_ASN1_MSR_POSITION_REQ_EXTENSIONCONTAINER extensionContainer
#define RRLP_ASN1_MSR_POSITION_REQ_REL98_EXTENSION rel98_MsrPosition_Req_extension
#define RRLP_ASN1_MSR_POSITION_REQ_REL5_EXTENSION rel5_MsrPosition_Req_extension

// Release 5 Measure Position Response Extensions
#define RRLP_ASN1_REL_5_MSR_POSITION_RSP_EXTENSION_EXTENDED_REFERENCE extended_reference
#define RRLP_ASN1_REL_5_MSR_POSITION_RSP_EXTENSION_OTD_MEASUREINFO_5_EXT otd_MeasureInfo_5_Ext
#define RRLP_ASN1_REL_5_MSR_POSITION_RSP_EXTENSION_ULPSEUDOSEGIND ulPseudoSegInd

// Release 98 Measure Position Request Extensions
#define RRLP_ASN1_REL_98_MSR_POSITION_REQ_EXTENSION_REL98_EXT_EXPOTD rel98_Ext_ExpOTD
#define RRLP_ASN1_REL_98_MSR_POSITION_REQ_EXTENSION_GPSTIMEASSISTANCEMEASUREMENTREQUEST gpsTimeAssistanceMeasurementRequest
#define RRLP_ASN1_REL_98_MSR_POSITION_REQ_EXTENSION_GPSREFERENCETIMEUNCERTAINTY gpsReferenceTimeUncertainty

// Release 98 Measure Position Response Extensions
#define RRLP_ASN1_REL_98_MSR_POSITION_RSP_EXTENSION_TIMEASSISTANCEMEASUREMENTS timeAssistanceMeasurements

//Release 98 Extension Measure Info
#define RRLP_ASN1_REL_98_EXT_MEASURE_INFO_OTD_MEASUREINFO_R98_EXT otd_MeasureInfo_R98_Ext

// Assistance Data
#define RRLP_ASN1_ASSISTANCE_DATA_REFERENCEASSISTDATA referenceAssistData
#define RRLP_ASN1_ASSISTANCE_DATA_MSRASSISTDATA msrAssistData
#define RRLP_ASN1_ASSISTANCE_DATA_SYSTEMINFOASSISTDATA systemInfoAssistData
#define RRLP_ASN1_ASSISTANCE_DATA_GPS_ASSISTDATA gps_AssistData
#define RRLP_ASN1_ASSISTANCE_DATA_MOREASSDATATOBESENT moreAssDataToBeSent
#define RRLP_ASN1_ASSISTANCE_DATA_EXTENSIONCONTAINER extensionContainer
#define RRLP_ASN1_ASSISTANCE_DATA_REL98_EXTENSION rel98_AssistanceData_Extension
#define RRLP_ASN1_ASSISTANCE_DATA_REL5_EXTENSION rel5_AssistanceData_Extension

// Release 98 Assistance Data Extensions
#define RRLP_ASN1_REL_98_ASSISTANCE_DATA_EXTENSION_REL98_EXT_EXPOTD rel98_Ext_ExpOTD
#define RRLP_ASN1_REL_98_ASSISTANCE_DATA_EXTENSION_GPSTIMEASSISTANCEMEASUREMENTREQUEST gpsTimeAssistanceMeasurementRequest
#define RRLP_ASN1_REL_98_ASSISTANCE_DATA_EXTENSION_GPSREFERENCETIMEUNCERTAINTY gpsReferenceTimeUncertainty

// Accuracy Option
#define RRLP_ASN1_ACCURACY_OPT_ACCURACY accuracy

// Control Header
#define RRLP_ASN1_CONTROL_HEADER_REFERENCETIME referenceTime
#define RRLP_ASN1_CONTROL_HEADER_REFLOCATION refLocation
#define RRLP_ASN1_CONTROL_HEADER_DGPSCORRECTIONS dgpsCorrections
#define RRLP_ASN1_CONTROL_HEADER_NAVIGATIONMODEL navigationModel
#define RRLP_ASN1_CONTROL_HEADER_IONOSPHERICMODEL ionosphericModel
#define RRLP_ASN1_CONTROL_HEADER_UTCMODEL utcModel
#define RRLP_ASN1_CONTROL_HEADER_ALMANAC almanac
#define RRLP_ASN1_CONTROL_HEADER_ACQUISASSIST acquisAssist
#define RRLP_ASN1_CONTROL_HEADER_REALTIMEINTEGRITY realTimeIntegrity

// Reference Time
#define RRLP_ASN1_REFERENCE_TIME_GSMTIME gsmTime
#define RRLP_ASN1_REFERENCE_TIME_GPSTOWASSIST gpsTowAssist

// Time Relation
#define RRLP_ASN1_TIME_RELATION_GSMTIME gsmTime

// Acquisition Element
#define RRLP_ASN1_ACQUIS_ELEMENT_ADDIONALDOPPLER addionalDoppler
#define RRLP_ASN1_ACQUIS_ELEMENT_ADDIONALANGLE addionalAngle

// Location Info
#define RRLP_ASN1_LOCATION_INFO_GPSTOW gpsTOW

// GPS Time Assistance Measurements
#define RRLP_ASN1_GPS_TIME_ASSISTANCE_GPSTOWSUBMS gpsTowSubms
#define RRLP_ASN1_GPS_TIME_ASSISTANCE_DELTATOW deltaTow
#define RRLP_ASN1_GPS_TIME_ASSISTANCE_GPSREFERENCETIMEUNCERTAINTY gpsReferenceTimeUncertainty

// Location Error
#define RRLP_ASN1_LOCATION_ERROR_ADDITIONALASSISTANCEDATA additionalAssistanceData

// Additional Assistance Data
#define RRLP_ASN1_ADDITIONAL_ASSISTANCE_DATA_GPSASSISTANCEDATA gpsAssistanceData
#define RRLP_ASN1_ADDITIONAL_ASSISTANCE_DATA_EXTENSIONCONTAINER extensionContainer

//RRLP Component
#define RRLP_ASN1_COMPONENT_NOTHING RRLP_RRLP_Component_PR_NOTHING
#define RRLP_ASN1_COMPONENT_MSRROSITIONREQ RRLP_RRLP_Component_PR_msrPositionReq
#define RRLP_ASN1_COMPONENT_MSRPOSITIONRSP RRLP_RRLP_Component_PR_msrPositionRsp
#define RRLP_ASN1_COMPONENT_ASSISTANCEDATA RRLP_RRLP_Component_PR_assistanceData
#define RRLP_ASN1_COMPONENT_ASSISTANCEDATAACK RRLP_RRLP_Component_PR_assistanceDataAck
#define RRLP_ASN1_COMPONENT_PROTOCOLERROR RRLP_RRLP_Component_PR_protocolError

//More Assistance Data To Be Sent
#define RRLP_ASN1_MORE_ASS_DATA_TO_BE_SENT_NOMOREMESSAGES RRLP_MoreAssDataToBeSent_noMoreMessages
#define RRLP_ASN1_MORE_ASS_DATA_TO_BE_SENT_MOREMESSAGESONTHEWAY RRLP_MoreAssDataToBeSent_moreMessagesOnTheWay

//Position Method
#define RRLP_ASN1_POSITION_METHOD_EOTD RRLP_PositionMethod_eotd
#define RRLP_ASN1_POSITION_METHOD_GPS RRLP_PositionMethod_gps
#define RRLP_ASN1_POSITION_METHOD_GPSOREOTD RRLP_PositionMethod_gpsOrEOTD

//Method Type
#define RRLP_ASN1_METHOD_TYPE_NOTHING RRLP_MethodType_PR_NOTHING
#define RRLP_ASN1_METHOD_TYPE_MSASSISTED RRLP_MethodType_PR_msAssisted
#define RRLP_ASN1_METHOD_TYPE_MSBASED RRLP_MethodType_PR_msBased
#define RRLP_ASN1_METHOD_TYPE_MSBASEDPREF RRLP_MethodType_PR_msBasedPref
#define RRLP_ASN1_METHOD_TYPE_MSASSISTEDPREF RRLP_MethodType_PR_msAssistedPref

// Multipath Indicator
#define RRLP_ASN1_MPATH_INDIC_NOTMEASURED RRLP_MpathIndic_notMeasured
#define RRLP_ASN1_MPATH_INDIC_LOW RRLP_MpathIndic_low
#define RRLP_ASN1_MPATH_INDIC_MEDIUM RRLP_MpathIndic_medium
#define RRLP_ASN1_MPATH_INDIC_HIGH RRLP_MpathIndic_high

// Location Error Reasons
#define RRLP_ASN1_LOC_ERROR_REASON_UNDEFINED RRLP_LocErrorReason_unDefined
#define RRLP_ASN1_LOC_ERROR_REASON_NOTENOUGHBTSS RRLP_LocErrorReason_notEnoughBTSs
#define RRLP_ASN1_LOC_ERROR_REASON_NOTENOUGHSATS RRLP_LocErrorReason_notEnoughSats
#define RRLP_ASN1_LOC_ERROR_REASON_EOTDLOCCALASSDATAMISSING RRLP_LocErrorReason_eotdLocCalAssDataMissing
#define RRLP_ASN1_LOC_ERROR_REASON_EOTDASSDATAMISSING RRLP_LocErrorReason_eotdAssDataMissing
#define RRLP_ASN1_LOC_ERROR_REASON_GPSLOCCALASSDATAMISSING RRLP_LocErrorReason_gpsLocCalAssDataMissing
#define RRLP_ASN1_LOC_ERROR_REASON_GPSASSDATAMISSING RRLP_LocErrorReason_gpsAssDataMissing
#define RRLP_ASN1_LOC_ERROR_REASON_METHODNOTSUPPORTED RRLP_LocErrorReason_methodNotSupported
#define RRLP_ASN1_LOC_ERROR_REASON_NOTPROCESSED RRLP_LocErrorReason_notProcessed
#define RRLP_ASN1_LOC_ERROR_REASON_REFBTSFORGPSNOTSERVINGBTS RRLP_LocErrorReason_refBTSForGPSNotServingBTS
#define RRLP_ASN1_LOC_ERROR_REASON_REFBTSFOREOTDNOTSERVINGBTS RRLP_LocErrorReason_refBTSForEOTDNotServingBTS

// Error Codes
#define RRLP_ASN1_ERROR_CODES_UNDEFINED RRLP_ErrorCodes_unDefined
#define RRLP_ASN1_ERROR_CODES_MISSINGCOMPONET RRLP_ErrorCodes_missingComponet
#define RRLP_ASN1_ERROR_CODES_INCORRECTDATA RRLP_ErrorCodes_incorrectData
#define RRLP_ASN1_ERROR_CODES_MISSINGIEORCOMPONENTELEMENT RRLP_ErrorCodes_missingIEorComponentElement
#define RRLP_ASN1_ERROR_CODES_MESSAGETOOSHORT RRLP_ErrorCodes_messageTooShort
#define RRLP_ASN1_ERROR_CODES_UNKNOWREFERENCENUMBER RRLP_ErrorCodes_unknowReferenceNumber

// Navigation Model Status
#define RRLP_ASN1_SAT_STATUS_NEWSATELLITEANDMODELUC RRLP_SatStatus_PR_newSatelliteAndModelUC
#define RRLP_ASN1_SAT_STATUS_OLDSATELLITEANDMODEL RRLP_SatStatus_PR_oldSatelliteAndModel
#define RRLP_ASN1_SAT_STATUS_NEWNAVIMODELUC RRLP_SatStatus_PR_newNaviModelUC

//Fix Type
#define RRLP_ASN1_FIX_TYPE_TWODFIX RRLP_FixType_twoDFix
#define RRLP_ASN1_FIX_TYPE_THREEDFIX RRLP_FixType_threeDFix

// Values
#define RRLP_ASN1_GPS_MEASURE_SET_NO_REF_FRAME NULL


//PDU
typedef RRLP_PDU_t RRLP_ASN1_pdu_t;

// Types
typedef RRLP_LocErrorReason_t RRLP_ASN1_LocErrorReason_t;
typedef RRLP_LocationError_RRLP_t RRLP_ASN1_LocationError_t;
typedef RRLP_Rel_5_MsrPosition_Rsp_Extension_t RRLP_ASN1_Rel_5_MsrPosition_Rsp_Extension_t;
typedef RRLP_Rel_98_MsrPosition_Rsp_Extension_t RRLP_ASN1_Rel_98_MsrPosition_Rsp_Extension_t;
typedef RRLP_Ext_GeographicalInformation_t RRLP_ASN1_Ext_GeographicalInformation_t;
typedef RRLP_ControlHeader_t RRLP_ASN1_ControlHeader_t;
typedef RRLP_GPSTOWAssistElement_t RRLP_ASN1_GPSTOWAssist_t;
typedef RRLP_SatElement_t RRLP_ASN1_SatElement_t;
typedef RRLP_NavModelElement_t RRLP_ASN1_NavModelElement_t;
typedef RRLP_UncompressedEphemeris_t RRLP_ASN1_UncompressedEphemeris_t;
typedef RRLP_AlmanacElement_t RRLP_ASN1_AlmanacElement_t;
typedef RRLP_AcquisElement_t RRLP_ASN1_AcquisElement_t;
typedef RRLP_SatelliteID_t RRLP_ASN1_BadSatelliteSet_t;
typedef RRLP_SeqOfGPS_MsrElement_t RRLP_ASN1_SeqOfGPS_MsrElement_t;
typedef RRLP_LocationInfo_t RRLP_ASN1_LocationInfo_t;
typedef RRLP_Extended_reference_t RRLP_ASN1_Extended_reference_t;
typedef RRLP_GPS_MeasureInfo_t RRLP_ASN1_GPS_MeasureInfo_t;
typedef RRLP_GPS_MsrSetElement_t RRLP_ASN1_GPS_MsrSetElement_t;
typedef RRLP_GPS_MsrElement_t RRLP_ASN1_GPS_MsrElement_t;
typedef RRLP_GPSTimeAssistanceMeasurements_t RRLP_ASN1_GPSTimeAssistanceMeasurements_t;
typedef RRLP_GPSReferenceTimeUncertainty_t RRLP_ASN1_GPSReferenceTimeUncertainty_t;
typedef RRLP_GPSAssistanceData_t RRLP_ASN1_GPSAssistanceData_t;
typedef RRLP_AdditionalAssistanceData_t RRLP_ASN1_AdditionalAssistanceData_t;

// Enum Types
typedef e_RRLP_ErrorCodes RRLP_ASN1_ErrorCodes;
typedef e_RRLP_PositionMethod RRLP_ASN1_PositionMethod;
typedef e_RRLP_LocErrorReason RRLP_ASN1_LocErrorReason;
typedef e_RRLP_MoreAssDataToBeSent RRLP_ASN1_MoreAssDataToBeSent;

#endif                // INCLUSION_GUARD_SUPL_RRLP_ASN1_TYPES_H
