//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_asn1_types.h
//
// 
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_asn1_types.h
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Typeddefs specifically for using the SUPL encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#ifndef INCLUSION_GUARD_SUPL_ASN1_TYPES_H
#define INCLUSION_GUARD_SUPL_ASN1_TYPES_H

/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

//internal supl
//#include "SUPL_osa.h"

#include "SUPL_ULP_ULP-PDU.h" /* SUPL version 2.0.0 */

/*
 **************************************************************************
 * Defines and type declarations
 **************************************************************************
*/

//Message Types
#define SUPL_ASN1_SUPL_INIT_CHOSEN UlpMessage_PR_msSUPLINIT
#define SUPL_ASN1_SUPL_START_CHOSEN UlpMessage_PR_msSUPLSTART
#define SUPL_ASN1_SUPL_RESP_CHOSEN UlpMessage_PR_msSUPLRESPONSE
#define SUPL_ASN1_SUPL_POS_INIT_CHOSEN UlpMessage_PR_msSUPLPOSINIT
#define SUPL_ASN1_SUPL_POS_CHOSEN UlpMessage_PR_msSUPLPOS
#define SUPL_ASN1_SUPL_END_CHOSEN UlpMessage_PR_msSUPLEND
#define SUPL_ASN1_SUPL_AUTH_REQ_CHOSEN UlpMessage_PR_msSUPLAUTHREQ
#define SUPL_ASN1_SUPL_AUTH_RESP_CHOSEN UlpMessage_PR_msSUPLAUTHRESP
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_TRIGGERED_START_CHOSEN UlpMessage_PR_msSUPLTRIGGEREDSTART
#define SUPL_ASN1_SUPL_TRIGGERED_RESP_CHOSEN UlpMessage_PR_msSUPLTRIGGEREDRESPONSE
#define SUPL_ASN1_SUPL_TRIGGERED_STOP_CHOSEN UlpMessage_PR_msSUPLTRIGGEREDSTOP
#define SUPL_ASN1_SUPL_NOTIFY_CHOSEN UlpMessage_PR_msSUPLNOTIFY
#define SUPL_ASN1_SUPL_NOTIFY_RESP_CHOSEN UlpMessage_PR_msSUPLNOTIFYRESPONSE
#define SUPL_ASN1_SUPL_SET_INIT_CHOSEN UlpMessage_PR_msSUPLSETINIT
#define SUPL_ASN1_SUPL_REPORT_CHOSEN UlpMessage_PR_msSUPLREPORT
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL INIT optional parameters
#define SUPL_ASN1_SUPL_INIT_SLP_ADDR sLPAddress
#define SUPL_ASN1_SUPL_INIT_QOP qoP
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_INIT_EXT ver2_SUPL_INIT_extension
#define SUPL_ASN1_SUPL_INIT_EXT_NOTIFICATION notificationMode
#define SUPL_ASN1_SUPL_INIT_EXT_SUPPORTED_NW_INFO supportedNetworkInformation
#define SUPL_ASN1_SUPL_INIT_EXT_TRIGGER_TYPE triggerType
#define SUPL_ASN1_SUPL_INIT_EXT_E_SLP_ADDR e_SLPAddress
#define SUPL_ASN1_SUPL_INIT_EXT_HIST_REPORT historicReporting
#define SUPL_ASN1_SUPL_INIT_EXT_PROT_LEVEL protectionLevel
#define SUPL_ASN1_SUPL_INIT_EXT_GNSS_TECH gnssPosTechnology
#define SUPL_ASN1_SUPL_INIT_EXT_MIN_MAJ_VERSION minimumMajorVersion
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL START optional parameters
#define SUPL_ASN1_SUPL_START_QOP qoP
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_START_EXT ver2_SUPL_START_extension
#define SUPL_ASN1_SUPL_START_EXT_LOC_IDS multipleLocationIds
#define SUPL_ASN1_SUPL_START_EXT_THIRD_PARTY thirdParty
#define SUPL_ASN1_SUPL_START_EXT_APP_ID applicationID
#define SUPL_ASN1_SUPL_START_EXT_POS position
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL TRIGGERED START optional parameters
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_TRIGGERED_START_VER ver
#define SUPL_ASN1_SUPL_TRIGGERED_START_QOP qoP
#define SUPL_ASN1_SUPL_TRIGGERED_START_LOC_IDS multipleLocationIds
#define SUPL_ASN1_SUPL_TRIGGERED_START_THIRD_PARTY thirdParty
#define SUPL_ASN1_SUPL_TRIGGERED_START_APP_ID applicationID
#define SUPL_ASN1_SUPL_TRIGGERED_START_TRIG_TYPE triggerType
#define SUPL_ASN1_SUPL_TRIGGERED_START_TRIG_PARAM triggerParams
#define SUPL_ASN1_SUPL_TRIGGERED_START_TRIG_POS position
#define SUPL_ASN1_SUPL_TRIGGERED_START_TRIG_REPORT_CAP reportingCap
#define SUPL_ASN1_SUPL_TRIGGERED_START_TRIG_CAUSE_CODE causeCode
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL TRIGGERED STOP optional parameters
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_TRIGGERED_STOP_STATUS_CODE statusCode
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL RESPONSE optional parameters
#define SUPL_ASN1_SUPL_RESP_SLP_ADDR sLPAddress
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_RESP_EXT ver2_SUPL_RESPONSE_extension
#define SUPL_ASN1_SUPL_RESP_EXT_SUPPORTED_NW_INFO supportedNetworkInformation
#define SUPL_ASN1_SUPL_RESP_EXT_APPROX_POS initialApproximateposition
#define SUPL_ASN1_SUPL_RESP_EXT_GNSS_TECH gnssPosTechnology
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL TRIGGERED RESPONSE optional parameters
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_TRIGGERED_RESP_TRIG_PARAM triggerParams
#define SUPL_ASN1_SUPL_TRIGGERED_RESP_SLP_ADDR sLPAddress
#define SUPL_ASN1_SUPL_TRIGGERED_RESP_SUPPORTED_NW_INFO supportedNetworkInformation
#define SUPL_ASN1_SUPL_TRIGGERED_RESP_REPORT_MODE reportingMode
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL NOTIFY RESPONSE optional parameters
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_NOTIFY_RESP_RESPONSE notificationResponse
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL POS INIT optional parameters
#define SUPL_ASN1_SUPL_POS_INIT_REQ_AID requestedAssistData
#define SUPL_ASN1_SUPL_POS_INIT_POS position
#define SUPL_ASN1_SUPL_POS_INIT_SUPL_POS sUPLPOS
#define SUPL_ASN1_SUPL_POS_INIT_VER ver
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_POS_INIT_EXT ver2_SUPL_POS_INIT_extension
#define SUPL_ASN1_SUPL_POS_INIT_EXT_LOC_IDS multipleLocationIds
#define SUPL_ASN1_SUPL_POS_INIT_EXT_GPS_REF_TIME utran_GPSReferenceTimeResult
#define SUPL_ASN1_SUPL_POS_INIT_EXT_GANSS_REF_TIME utran_GANSSReferenceTimeResult
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL POS optional parameters
#define SUPL_ASN1_SUPL_POS_VELOCITY velocity
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_POS_EXT ver2_SUPL_POS_extension
#define SUPL_ASN1_SUPL_POS_EXT_GPS_REF_TIME_AID utran_GPSReferenceTimeAssistance
#define SUPL_ASN1_SUPL_POS_EXT_GPS_REF_TIME_RESULT utran_GPSReferenceTimeResult
#define SUPL_ASN1_SUPL_POS_EXT_GANSS_REF_TIME_AID utran_GANSSReferenceTimeAssistance
#define SUPL_ASN1_SUPL_POS_EXT_GANSS_REF_TIME_RESULT utran_GANSSReferenceTimeResult
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL REPORT optional parameters
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_REPORT_SESSION_LIST sessionList
#define SUPL_ASN1_SUPL_REPORT_SET_CAP sETCapabilities
#define SUPL_ASN1_SUPL_REPORT_DATA_LIST reportDataList
#define SUPL_ASN1_SUPL_REPORT_VER ver
#define SUPL_ASN1_SUPL_REPORT_MORE moreComponents
#endif /* #ifndef SUPL_SUPL_V1 */

//SUPL END optional parameters
#define SUPL_ASN1_SUPL_END_POS position
#define SUPL_ASN1_SUPL_END_STATUS_CODE statusCode
#define SUPL_ASN1_SUPL_END_VER ver
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SUPL_END_EXT ver2_SUPL_END_extension
#define SUPL_ASN1_SUPL_END_EXT_SET_CAP sETCapabilities
#endif /* #ifndef SUPL_SUPL_V1 */

//Allowed Reporting Type
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_POSITIONS_ONLY AllowedReportingType_positionsOnly
#define SUPL_ASN1_MEASUREMENTS_ONLY AllowedReportingType_measurementsOnly
#define SUPL_ASN1_MEASUREMENT_AND_POSITIONS AllowedReportingType_positionsAndMeasurements
#endif /* #ifndef SUPL_SUPL_V1 */

//Cell Info
#define SUPL_ASN1_GSM_CELL_CHOSEN CellInfo_PR_gsmCell
#define SUPL_ASN1_WCDMA_CELL_CHOSEN CellInfo_PR_wcdmaCell

//Client Name
#define SUPL_ASN1_CLIENT_NAME clientName
#define SUPL_ASN1_CLIENT_NAME_TYPE clientNameType

//Encodng Type
#define SUPL_ASN1_ENCODING_TYPE encodingType
#define SUPL_ASN1_UCS2 EncodingType_ucs2
#define SUPL_ASN1_GSM_DEFAULT EncodingType_gsmDefault
#define SUPL_ASN1_UTF8 EncodingType_utf8

//IP Address
#define SUPL_ASN1_IPV4_CHOSEN IPAddress_PR_ipv4Address
#define SUPL_ASN1_IPV6_CHOSEN IPAddress_PR_ipv6Address

//Location Id Data
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_LOC_ID_REL_TIME_STAMP relativetimestamp
#endif /* #ifndef SUPL_SUPL_V1 */

//Navigation Model Data
#define SUPL_ASN1_NAV_MODEL_DATA navigationModelData
#define SUPL_ASN1_NAV_MODEL_SAT_INFO satInfo

//NMR
#define SUPL_ASN1_GSM_NMR nMR
#define SUPL_ASN1_GSM_NMR_CHOSEN 0
#define SUPL_ASN1_GSM_TA tA
#define SUPL_ASN1_GSM_TA_CHOSEN 0
#define SUPL_ASN1_WCDMA_FREQ_INFO frequencyInfo
#define SUPL_ASN1_WCDMA_FREQ_INFO_CHOSEN 0
#define SUPL_ASN1_WCDMA_PRIMARY_SCR_CODE primaryScramblingCode
#define SUPL_ASN1_WCDMA_PRIMARY_SCR_CODE_CHOSEN 0
#define SUPL_ASN1_WCDMA_MEAS_RESULTS_LIST measuredResultsList
#define SUPL_ASN1_WCDMA_MEAS_RESULTS_LIST_CHOSEN 0
#define SUPL_ASN1_WCDMA_PARAM_ID cellParametersId
#define SUPL_ASN1_WCDMA_PARAM_ID_CHOSEN 0
#define SUPL_ASN1_WCDMA_TA timingAdvance
#define SUPL_ASN1_WCDMA_TA_CHOSEN 0
#define SUPL_ASN1_WCDMA_FREQ_FREQ_INFO frequencyInfo
#define SUPL_ASN1_WCDMA_FREQ_FREQ_INFO_CHOSEN 0
#define SUPL_ASN1_WCDMA_FREQ_CARRIER_RSSI utra_CarrierRSSI
#define SUPL_ASN1_WCDMA_FREQ_CARRIER_RSSI_CHOSEN 0
#define SUPL_ASN1_WCDMA_FREQ_CELL_MEAS_RESULTS_LIST cellMeasuredResultsList
#define SUPL_ASN1_WCDMA_FREQ_CELL_MEAS_RESULTS_LIST_CHOSEN 0
#define SUPL_ASN1_WCDMA_CELL_CELL_ID cellIdentity
#define SUPL_ASN1_WCDMA_CELL_CELL_ID_CHOSEN 0
#define SUPL_ASN1_WCDMA_CELL_CPICH_EC_N0 cpich_Ec_N0
#define SUPL_ASN1_WCDMA_CELL_CPICH_EC_N0_CHOSEN 0
#define SUPL_ASN1_WCDMA_CELL_CPICH_RSCP cpich_RSCP
#define SUPL_ASN1_WCDMA_CELL_CPICH_RSCP_CHOSEN 0
#define SUPL_ASN1_WCDMA_CELL_PATHLOSS pathloss
#define SUPL_ASN1_WCDMA_CELL_PATHLOSS_CHOSEN 0
#define SUPL_ASN1_WCDMA_UARFCN_UL uarfcn_UL
#define SUPL_ASN1_FREQ_FDD_CHOSEN FrequencyInfo__modeSpecificInfo_PR_fdd
#define SUPL_ASN1_FREQ_TDD_CHOSEN FrequencyInfo__modeSpecificInfo_PR_tdd
#define SUPL_ASN1_CELL_FDD_CHOSEN CellMeasuredResults__modeSpecificInfo_PR_fdd
#define SUPL_ASN1_CELL_TDD_CHOSEN CellMeasuredResults__modeSpecificInfo_PR_tdd

//Notification
#define SUPL_ASN1_NOTIFICATION notification
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_NOTIFICATION_EXT ver2_Notification_extension
#define SUPL_ASN1_NOTIFICATION_EXT_EMERGENCY_LOC emergencyCallLocation
#endif /* #ifndef SUPL_SUPL_V1 */

//Notification Format
#define SUPL_ASN1_LOGICAL_NAME FormatIndicator_logicalName
#define SUPL_ASN1_EMAIL FormatIndicator_e_mailAddress
#define SUPL_ASN1_MSISDN FormatIndicator_msisdn
#define SUPL_ASN1_URL FormatIndicator_url
#define SUPL_ASN1_SIP_URL FormatIndicator_sipUrl
#define SUPL_ASN1_MIN FormatIndicator_min
#define SUPL_ASN1_MDN FormatIndicator_mdn
#define SUPL_ASN1_IMS_PUBLIC_ID FormatIndicator_iMSPublicidentity

//Notification Mode
#define SUPL_ASN1_BASED_ON_LOCATION NotificationMode_basedOnLocation

//Notification Response
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_ALLOWED NotificationResponse_allowed
#define SUPL_ASN1_DENIED NotificationResponse_notAllowed
#endif /* #ifndef SUPL_SUPL_V1 */

//Notification Type
#define SUPL_ASN1_NO_NOTIFICATION NotificationType_noNotificationNoVerification
#define SUPL_ASN1_NOTIFY_ONLY NotificationType_notificationOnly
#define SUPL_ASN1_ACCEPT_NO_REPLY NotificationType_notificationAndVerficationAllowedNA
#define SUPL_ASN1_DENY_NO_REPLY NotificationType_notificationAndVerficationDeniedNA
#define SUPL_ASN1_PRIVACY_OVERRIDE NotificationType_privacyOverride

//Payload
#define SUPL_ASN1_RRLP PosPayLoad_PR_rrlpPayload

//Position Data
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_POS_DATA positionData
#define SUPL_ASN1_POS_DATA_POS_METHOD posMethod
#define SUPL_ASN1_POS_DATA_GNSS_POS_TECH gnssPosTechnology
#define SUPL_ASN1_POS_DATA_GNSS_SIGNAL_INFO ganssSignalsInfo
#endif /* #ifndef SUPL_SUPL_V1 */

//Position Estimate
#define SUPL_ASN1_POS_EST_ALTITUDE altitudeInfo
#define SUPL_ASN1_POS_EST_UNCERTAINTY uncertainty
#define SUPL_ASN1_POS_EST_CONFIDENCE confidence
#define SUPL_ASN1_POS_EST_VELOCITY velocity
#define SUPL_ASN1_NORTH PositionEstimate__latitudeSign_north
#define SUPL_ASN1_SOUTH PositionEstimate__latitudeSign_south
#define SUPL_ASN1_DEPTH AltitudeInfo__altitudeDirection_depth
#define SUPL_ASN1_HEIGHT AltitudeInfo__altitudeDirection_height

//Positioning Method
#define SUPL_ASN1_AGPS_MSA PosMethod_agpsSETassisted
#define SUPL_ASN1_AGPS_MSB PosMethod_agpsSETbased
#define SUPL_ASN1_AGPS_MSA_PREF PosMethod_agpsSETassistedpref
#define SUPL_ASN1_AGPS_MSB_PREF PosMethod_agpsSETbasedpref
#define SUPL_ASN1_AGPS_AUTO PosMethod_autonomousGPS
#define SUPL_ASN1_E_CID PosMethod_eCID
#define SUPL_ASN1_NO_POSITION PosMethod_noPosition
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_HIST_DATA PosMethod_ver2_historicalDataRetrieval
#define SUPL_ASN1_SESSION_INFO PosMethod_ver2_sessioninfoquery
#endif /* #ifndef SUPL_SUPL_V1 */

//Positioning Protocol
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_POS_PROTOCOL_EXT ver2_PosProtocol_extension
#define SUPL_ASN1_POS_PROTOCOL_EXT_RRLP_VER posProtocolVersionRRLP
#define SUPL_ASN1_POS_PROTOCOL_EXT_RRC_VER posProtocolVersionRRC
#define SUPL_ASN1_POS_PROTOCOL_EXT_TIA801_VER posProtocolVersionTIA801
#define SUPL_ASN1_POS_PROTOCOL_EXT_LPP_VER posProtocolVersionLPP
#endif /* #ifndef SUPL_SUPL_V1 */

//Positioning Technology
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_POS_TECH_EXT ver2_PosTechnology_extension
#define SUPL_ASN1_POS_TECH_EXT_GANSS_POS_METHOD gANSSPositionMethods
#endif /* #ifndef SUPL_SUPL_V1 */

//Preferred Method
#define SUPL_ASN1_MSA_PREFERRED PrefMethod_agpsSETassistedPreferred
#define SUPL_ASN1_MSB_PREFERRED PrefMethod_agpsSETBasedPreferred
#define SUPL_ASN1_NO_PREFERENCE PrefMethod_noPreference

//Protection Level
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_PROTECTION_LEVEL protectionLevel
#define SUPL_ASN1_BASIC_PROT_PARAMS basicProtectionParams
#define SUPL_ASN1_NULL_PROT ProtLevel_nullProtection
#define SUPL_ASN1_BASIC_PROT ProtLevel_basicProtection
#endif /* #ifndef SUPL_SUPL_V1 */

//Proxy Mode
#define SUPL_ASN1_PROXY SLPMode_proxy

//QoP
#define SUPL_ASN1_QOP_VERTICAL_ACC veracc
#define SUPL_ASN1_QOP_MAX_LOC_AGE maxLocAge
#define SUPL_ASN1_QOP_DELAY delay

//Reporting Capabilities
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_MAX_INTERVAL maxInt
#define SUPL_ASN1_BATCH_REPORT_CAPABILITY batchRepCap
#endif /* #ifndef SUPL_SUPL_V1 */

//Reporting Criteria
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_REPORTING_CRITERIA reportingCriteria
#define SUPL_ASN1_TIME_WINDOW timeWindow
#define SUPL_ASN1_MAX_NUM_REPORTS maxNumberofReports
#define SUPL_ASN1_MIN_TIME_INTERVAL minTimeInterval
#endif /* #ifndef SUPL_SUPL_V1 */

//Report Data
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_REPORT_POSITION_DATA positionData
#define SUPL_ASN1_REPORT_MULT_LOC_IDS multipleLocationIds
#define SUPL_ASN1_REPORT_RESULT_CODE resultCode
#define SUPL_ASN1_REPORT_TIME_STAMP timestamp
#endif /* #ifndef SUPL_SUPL_V1 */

//Reporting Mode
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_REAL_TIME RepModeOptions_realtime
#define SUPL_ASN1_QUASI_REAL_TIME RepModeOptions_quasirealtime
#define SUPL_ASN1_BATCH RepModeOptions_batch
#define SUPL_ASN1_BATCH_CONDITIONS batchRepConditions
#define SUPL_ASN1_BATCH_TYPE batchRepType
#endif /* #ifndef SUPL_SUPL_V1 */

//Requested Assistance Data
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_REQ_AID_DATA_EXT ver2_RequestedAssistData_extension
#endif /* #ifndef SUPL_SUPL_V1 */

//Requestor Id
#define SUPL_ASN1_REQUESTOR_ID requestorId
#define SUPL_ASN1_REQUESTOR_ID_TYPE requestorIdType

//Session Id
#define SUPL_ASN1_SET_SESSION_ID setSessionID
#define SUPL_ASN1_SET_MSISDN_CHOSEN SETId_PR_msisdn
#define SUPL_ASN1_SET_MDN_CHOSEN SETId_PR_mdn
#define SUPL_ASN1_SET_MIN_CHOSEN SETId_PR_min
#define SUPL_ASN1_SET_IMSI_CHOSEN SETId_PR_imsi
#define SUPL_ASN1_SET_NAI_CHOSEN SETId_PR_nai
#define SUPL_ASN1_SET_IPADDR_CHOSEN SETId_PR_iPAddress
#define SUPL_ASN1_SLP_SESSION_ID slpSessionID

//SET capabilities
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_SET_CAP_EXT ver2_SETCapabilities_extension
#define SUPL_ASN1_SET_CAP_EXT_SERVICE_CAP serviceCapabilities
#endif /* #ifndef SUPL_SUPL_V1 */

//SLP Address
#define SUPL_ASN1_SLP_IPADDR_CHOSEN SLPAddress_PR_iPAddress
#define SUPL_ASN1_SLP_FQDN_CHOSEN SLPAddress_PR_fQDN

//Status Code
#define SUPL_ASN1_UNSPECIFIED StatusCode_unspecified
#define SUPL_ASN1_SYSTEM_FAILURE StatusCode_systemFailure
#define SUPL_ASN1_UNEXPECTED_MESSAGE StatusCode_unexpectedMessage
#define SUPL_ASN1_PROTOCOL_ERROR StatusCode_protocolError
#define SUPL_ASN1_DATA_MISSING StatusCode_dataMissing
#define SUPL_ASN1_UNEXPECTED_DATA_VALUE StatusCode_unexpectedDataValue
#define SUPL_ASN1_POS_METHOD_FAILURE StatusCode_posMethodFailure
#define SUPL_ASN1_POS_METHOD_MISMATCH StatusCode_posMethodMismatch
#define SUPL_ASN1_POS_PROTOCOL_MISMATCH StatusCode_posProtocolMismatch
#define SUPL_ASN1_TARGET_SET_NOT_REACHABLE StatusCode_targetSETnotReachable
#define SUPL_ASN1_VERSION_NOT_SUPPORTED StatusCode_versionNotSupported
#define SUPL_ASN1_RESOURCE_SHORTAGE StatusCode_resourceShortage
#define SUPL_ASN1_INVALID_SESSION_ID StatusCode_invalidSessionId
#define SUPL_ASN1_NON_PROXY_MODE_NOT_SUPPORTED StatusCode_nonProxyModeNotSupported
#define SUPL_ASN1_PROXY_MODE_NOT_SUPPORTED StatusCode_proxyModeNotSupported
#define SUPL_ASN1_POSITIONING_NOT_PERMITTED StatusCode_positioningNotPermitted
#define SUPL_ASN1_AUTH_NET_FAILURE StatusCode_authNetFailure
#define SUPL_ASN1_AUTH_SUPL_INIT_FAILURE StatusCode_authSuplinitFailure
#define SUPL_ASN1_CONSENT_DENIED_BY_USER StatusCode_consentDeniedByUser
#define SUPL_ASN1_CONSENT_GRANTED_BY_USER StatusCode_consentGrantedByUser
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_INCOMPATIBLE_PROT_LEVEL StatusCode_ver2_incompatibleProtectionLevel
#define SUPL_ASN1_SERVICE_NOT_SUPPORTED StatusCode_ver2_serviceNotSupported
#define SUPL_ASN1_INSUFFICIENT_INTERVAL StatusCode_ver2_insufficientInterval
#define SUPL_ASN1_NO_SUPL_COVERAGE StatusCode_ver2_noSUPLCoverage
#define SUPL_ASN1_SESSION_STOPPED StatusCode_ver2_sessionStopped
#endif /* #ifndef SUPL_SUPL_V1 */

//Status
#define SUPL_ASN1_CURRENT Status_current

//Third Party Identity
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_TTTP_LOGICAL_NAME ThirdPartyID_PR_logicalName
#define SUPL_ASN1_TTTP_MSISDN ThirdPartyID_PR_msisdn
#define SUPL_ASN1_TTTP_EMAIL ThirdPartyID_PR_emailaddr
#define SUPL_ASN1_TTTP_URI ThirdPartyID_PR_uri
#define SUPL_ASN1_TTTP_SIP_URL ThirdPartyID_PR_sip_uri
#define SUPL_ASN1_TTTP_IMS_PUBLIC_ID ThirdPartyID_PR_ims_public_identity
#define SUPL_ASN1_TTTP_MIN ThirdPartyID_PR_min
#define SUPL_ASN1_TTTP_MDN ThirdPartyID_PR_mdn
#endif /* #ifndef SUPL_SUPL_V1 */

//Trigger Parameters
#ifndef SUPL_SUPL_V1
#define SUPL_ASN1_PERIODIC TriggerType_periodic
#define SUPL_ASN1_PERIODIC_START_TIME startTime
#define SUPL_ASN1_AREA_EVENT TriggerType_areaEvent
#define SUPL_ASN1_PERIODIC_CHOSEN TriggerParams_PR_periodicParams
#endif /* #ifndef SUPL_SUPL_V1 */

//Velocity
#define SUPL_ASN1_HOR_VEL_CHOSEN Velocity_PR_horvel
#define SUPL_ASN1_HOR_AND_VER_VEL_CHOSEN Velocity_PR_horandvervel
#define SUPL_ASN1_HOR_VEL_UNCERT_CHOSEN Velocity_PR_horveluncert
#define SUPL_ASN1_HOR_AND_VER_VEL_UNCERT_CHOSEN Velocity_PR_horandveruncert

//PDU
typedef ULP_PDU_t SUPL_ASN1_ulp_pdu_t;

//ULP Header
typedef ULP_PDU_t SUPL_ASN1_ulp_hdr_t;

//ULP Message
typedef UlpMessage_t SUPL_ASN1_ulp_message_t;

//Message Types
typedef SUPLINIT_t SUPL_ASN1_supl_init_t;
typedef SUPLSTART_t SUPL_ASN1_supl_start_t;
typedef SUPLRESPONSE_t SUPL_ASN1_supl_resp_t;
typedef SUPLPOSINIT_t SUPL_ASN1_supl_pos_init_t;
typedef SUPLPOS_t SUPL_ASN1_supl_pos_t;
typedef SUPLEND_t SUPL_ASN1_supl_end_t;
typedef SUPLAUTHREQ_t SUPL_ASN1_supl_auth_req_t;
typedef SUPLAUTHRESP_t SUPL_ASN1_supl_auth_resp_t;
#ifndef SUPL_SUPL_V1
typedef Ver2_SUPL_INIT_extension_t SUPL_ASN1_supl_init_ext_t;
typedef Ver2_SUPL_START_extension_t SUPL_ASN1_supl_start_ext_t;
typedef Ver2_SUPLTRIGGEREDSTART_t SUPL_ASN1_supl_triggered_start_t;
typedef Ver2_SUPL_RESPONSE_extension_t SUPL_ASN1_supl_resp_ext_t;
typedef Ver2_SUPLTRIGGEREDRESPONSE_t SUPL_ASN1_supl_triggered_resp_t;
typedef Ver2_SUPLTRIGGEREDSTOP_t SUPL_ASN1_supl_triggered_stop_t;
typedef Ver2_SUPL_POS_INIT_extension_t SUPL_ASN1_supl_pos_init_ext_t;
typedef Ver2_SUPL_POS_extension_t SUPL_ASN1_supl_pos_ext_t;
typedef Ver2_SUPLNOTIFY_t SUPL_ASN1_supl_notify_t;
typedef Ver2_SUPLNOTIFYRESPONSE_t SUPL_ASN1_supl_notify_resp_t;
typedef Ver2_SUPLSETINIT_t SUPL_ASN1_supl_set_init_t;
typedef Ver2_SUPLREPORT_t SUPL_ASN1_supl_report_t;
typedef Ver2_SUPL_END_extension_t SUPL_ASN1_supl_end_ext_t;
#endif /* #ifndef SUPL_SUPL_V1 */

//General
typedef AltitudeInfo_t SUPL_ASN1_altitude_info_t;
typedef long SUPL_ASN1_arfcn_t;
typedef uint8_t SUPL_ASN1_bit_str_t;
typedef long SUPL_ASN1_bsic_t;
typedef UTRA_CarrierRSSI_t SUPL_ASN1_carrier_rssi_t;
typedef CellInfo_t SUPL_ASN1_cell_info_t;
typedef CellMeasuredResults_t SUPL_ASN1_cell_measured_results_t;
typedef long SUPL_ASN1_confidence_t;
typedef CPICH_Ec_N0_t SUPL_ASN1_cpich_ec_no_t;
typedef CPICH_RSCP_t SUPL_ASN1_cpich_rscp_t;
typedef long SUPL_ASN1_delay_t;
typedef FrequencyInfo_t SUPL_ASN1_frequency_info_t;
typedef LocationId_t SUPL_ASN1_location_id_t;
typedef long SUPL_ASN1_max_loc_age_t;
typedef MeasuredResultsList_t SUPL_ASN1_measured_results_list_t;
typedef MeasuredResults_t SUPL_ASN1_measured_results_t;
typedef NavigationModel_t SUPL_ASN1_navigation_model_t;
typedef NMR_t SUPL_ASN1_nmr_t;
typedef NMRelement_t SUPL_ASN1_nmr_element_t;
typedef Notification_t SUPL_ASN1_notification_t;
typedef uint8_t SUPL_ASN1_octet_str_t;
typedef Pathloss_t SUPL_ASN1_pathloss_t;
typedef Position_t SUPL_ASN1_position_t;
typedef PosMethod_t SUPL_ASN1_pos_method_t;
typedef PosPayLoad_t SUPL_ASN1_pos_payload_t;
typedef QoP_t SUPL_ASN1_qop_t;
typedef RequestedAssistData_t SUPL_ASN1_req_assist_data_t;
typedef long SUPL_ASN1_rx_lev_t;
typedef SatelliteInfo_t SUPL_satellite_info_t;
typedef SatelliteInfoElement_t SUPL_ASN1_satellite_info_element_t;
typedef long SUPL_ASN1_scrambling_code_t;
typedef SessionID_t SUPL_ASN1_session_id_t;
typedef SETCapabilities_t SUPL_ASN1_set_capabilities_t;
typedef SetSessionID_t SUPL_ASN1_set_session_id_t;
typedef SLPAddress_t SUPL_ASN1_slp_address_t;
typedef SlpSessionID_t SUPL_ASN1_slp_session_id_t;
typedef StatusCode_t SUPL_ASN1_status_code_t;
typedef long SUPL_ASN1_timing_advance_t;
typedef UARFCN_t SUPL_ASN1_uarfcn_t;
typedef struct PositionEstimate__uncertainty SUPL_ASN1_uncertainty_t;
typedef uint8_t SUPL_ASN1_utc_time_t;
typedef Velocity_t SUPL_ASN1_velocity_t;
typedef Ver_t SUPL_ASN1_ver_t;
typedef long SUPL_ASN1_vertical_accuracy_t;
#ifndef SUPL_SUPL_V1
typedef HistoricReporting_t SUPL_ASN1_historic_reporting_t;
typedef LocationIdData_t SUPL_ASN1_location_id_data_t;
typedef NotificationResponse_t SUPL_ASN1_notification_resp_t;
typedef PositionData_t SUPL_ASN1_position_data_t;
typedef Ver2_PosProtocol_extension_t SUPL_ASN1_pos_protocol_ext_t;
typedef PosProtocolVersion3GPP_t SUPL_ASN1_pos_protocol_version_t;
typedef Ver2_PosTechnology_extension_t SUPL_ASN1_pos_technology_ext_t;
typedef ProtectionLevel_t SUPL_ASN1_protection_level_t;
typedef ReportData_t SUPL_ASN1_report_data_t;
typedef ReportDataList_t SUPL_ASN1_report_data_list_t;
typedef ReportingCap_t SUPL_ASN1_reporting_cap_t;
typedef ReportingMode_t SUPL_ASN1_reporting_mode_t;
typedef SessionList_t SUPL_ASN1_session_list_t;
typedef Ver2_SETCapabilities_extension_t SUPL_ASN1_set_capabilities_ext_t;
typedef ThirdParty_t SUPL_ASN1_third_party_t;
typedef ThirdPartyID_t SUPL_ASN1_third_party_id_t;
typedef TriggerParams_t SUPL_ASN1_trigger_params_t;
typedef TriggerType_t SUPL_ASN1_trigger_type_t;
#endif /* #ifndef SUPL_SUPL_V1 */

#endif /* INCLUSION_GUARD_SUPL_ASN1_TYPES_H */
