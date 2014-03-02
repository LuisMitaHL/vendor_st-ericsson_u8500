//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_ptypes.h
//
// $Header: X:/MKS Projects/prototype/prototype/asn1_interface/rcs/supl_ptypes.h 1.10 2009/01/14 14:27:47Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_ptypes
///
/// \file
/// \brief
///      Base SUPL types shared throughout the SUPL and Positioning solution.
///
///      Types required by the SUPL subsystem including types shared with the 
///      Positioning subsystem.
///
//*************************************************************************

#ifndef SUPL_PTYPES_H
#define SUPL_PTYPES_H

#include <stdio.h>

#include "gps_ptypes.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions for supl_asn1_abstraction
// The variable names used match those in the SUPL ASN.1 definitions.
// See the SUPL specification for a full description.

#define GN_GPS_VER_LEN 8 ///< From ASN.1 definition of Ver ::= BIT STRING(SIZE (64))

/// \brief
///      Position technology supported.
/// \details
/// <pre>
///   PosTechnology ::= SEQUENCE {
///      agpsSETassisted   BOOLEAN,
///      agpsSETBased      BOOLEAN,
///      autonomousGPS     BOOLEAN,
///      aFLT              BOOLEAN,
///      eCID              BOOLEAN,
///      eOTD              BOOLEAN,
///      oTDOA             BOOLEAN,
///      ...}
/// </pre>
typedef struct GN_PosTechnology{
   BL agpsSETassisted ;       ///< agpsSETassisted positioning technology is supported if TRUE.
   BL agpsSETBased ;          ///< agpsSETBased positioning technology is supported if TRUE.
   BL autonomousGPS ;         ///< autonomousGPS positioning technology is supported if TRUE.
   BL aFLT ;                  ///< aFLT positioning technology is supported if TRUE.
   BL eCID ;                  ///< eCID positioning technology is supported if TRUE.
   BL eOTD ;                  ///< eOTD positioning technology is supported if TRUE.
   BL oTDOA ;                 ///< oTDOA positioning technology is supported if TRUE.
} s_GN_PosTechnology;

/// \brief
///      Preferred AGPS positioning method.
/// \details
/// <pre>
///   PrefMethod ::= ENUMERATED {
///      agpsSETassistedPreferred,
///      agpsSETBasedPreferred,
///      noPreference}
/// </pre>
typedef enum GN_PrefMethod{
   GN_agpsSETassistedPreferred,
   GN_agpsSETBasedPreferred,
   GN_noPreference
} e_GN_PrefMethod;

/// \brief
///      Positioning protocol to be used.
/// \details
/// <pre>
///   PosProtocol ::= SEQUENCE {
///      tia801   BOOLEAN,
///      rrlp     BOOLEAN,
///      rrc      BOOLEAN,
///      ...}
/// </pre>
typedef struct GN_PosProtocol{
   BL tia801 ;                ///< TIA-801 positioning protocol is supported if TRUE.
   BL rrlp ;                  ///< RRLP positioning protocol is supported if TRUE.
   BL rrc ;                   ///< RRC positioning protocol is supported if TRUE.
} s_GN_PosProtocol;


/*ServicesSupported ::= SEQUENCE {
periodicTrigger BOOLEAN,
areaEventTrigger BOOLEAN,
...}
*/
typedef struct
{
   BL PeriodicTrigger;
   BL EventTrigger;
} s_GN_ServicesSupported;

/* RepMode ::= SEQUENCE {
realtime BOOLEAN,
quasirealtime BOOLEAN,
batch BOOLEAN,
...}
*/
typedef struct
{
   BL RealTime;
   BL QuasiRealTime;
   BL Batch;
} s_GN_ReportingMode;

/* BatchRepCap ::= SEQUENCE {
report-position BOOLEAN, -- set to “true” if reporting of position is
supported
report-measurements BOOLEAN, -- set to “true” if reporting of measurements is
supported
max-num-positions INTEGER (1..1024) OPTIONAL,
max-num-measurements INTEGER (1..1024) OPTIONAL,
...}
*/
typedef struct
{
   BL ReportPosition;      ///< set to “true” if reporting of position is supported
   BL ReportMeasurements;  ///< set to “true” if reporting of measurements is supported
   U2 MaxNumPosition;      ///< INTEGER (1..1024) OPTIONAL. 0 - Not set
   U2 MaxNumMeasuremetns;  ///< INTEGER (1..1024) OPTIONAL  0 - Not set
} s_GN_BatchRepCap;

/* ReportingCap ::= SEQUENCE {
minInt INTEGER (1..3600), -- units in seconds
maxInt INTEGER (1..1440) OPTIONAL, -- units in minutes
repMode RepMode,
batchRepCap BatchRepCap OPTIONAL, -- only used for batch and quasi
real time reporting
...}
*/
typedef struct
{
   U2                   MinimumInterval;  ///< INTEGER (1..3600), -- units in seconds
   U2                   MaximumInterval;  ///< INTEGER (1..1440) OPTIONAL, -- units in minutes . 0 - Not present
   s_GN_ReportingMode   ReportingMode;    ///< Reporting Modes Supported by SET
   s_GN_BatchRepCap     BatchRepCap;      ///< Batch Reporting Modes supported by SET. Only valid if Batch is valid ReportingMode
} s_GN_ReportingCapabilities;


/* GeoAreaShapesSupported ::= SEQUENCE {
ellipticalArea BOOLEAN,
polygonArea BOOLEAN,
...} */
/* @TODO */
typedef struct
{
   BL Ellipse; ///< set to “true” if ellipse area is supported for Geographic Area Event Triggers
   BL Polygon; ///< set to “true” if polygon area is supported for Geographic Area Event Triggers
} s_GN_GeographicAreasSupported;

/* EventTriggerCapabilities ::= SEQUENCE {
geoAreaShapesSupported GeoAreaShapesSupported,
maxNumGeoAreaSupported INTEGER (0..maxNumGeoArea) OPTIONAL,
maxAreaIdListSupported INTEGER (0..maxAreaIdList) OPTIONAL,
maxAreaIdSupportedPerList INTEGER (0..maxAreaId) OPTIONAL,
...}
*/
/* @TODO */
typedef struct
{
   s_GN_GeographicAreasSupported GeographicAreasSupported;
   U1                            MaxNumGeoArea;             ///< INTEGER (1..10),  -1 unset
   U1                            MaxAreaIdList;             ///< INTEGER (1..10),  -1 unset
   U1                            MaxAreaIdPerAreaIdList;    ///< INTEGER (1..10),  -1 unset
} s_GN_EventTriggerCapabilities;


/*SessionCapabilities ::= SEQUENCE {
maxNumberTotalSessions INTEGER (1..128),
maxNumberPeriodicSessions INTEGER (1..32),
maxNumberTriggeredSessions INTEGER (1..32),
...}
*/
typedef struct
{
   U1 MaxTotalSessions;    ///< INTEGER (1..128)
   U1 MaxPeriodicSessions; ///< INTEGER (1..32)
   U1 MaxTriggeredSession; ///< INTEGER (1..32)
} s_GN_SessionCapabilities;

/*
ServiceCapabilities ::= SEQUENCE {
servicesSupported ServicesSupported,
reportingCapabilities ReportingCap OPTIONAL,
eventTriggerCapabilities EventTriggerCapabilities OPTIONAL,
sessionCapabilities SessionCapabilities,
...}
*/
typedef struct
{
   s_GN_ServicesSupported        ServicesSupported;
   s_GN_ReportingCapabilities    ReportingCapabilities;
   s_GN_EventTriggerCapabilities EventTriggerCapabilities;
   s_GN_SessionCapabilities      SessionCapabilities;
} s_GN_SUPL_V2_SetCapabilities;


/// \brief
///      Capabilities of the SET for negotiation with SLP.
/// \details
/// <pre>
///   SETCapabilities ::= SEQUENCE {
///      posTechnology  PosTechnology,
///      prefMethod     PrefMethod,
///      posProtocol    PosProtocol,
///      ...}
/// </pre>
typedef struct GN_SetCapabilities {
   s_GN_PosTechnology   PosTechnology ;   ///< Positioning technology supported by the SET.
   e_GN_PrefMethod      PrefMethod ;      ///< Preferred method for SET positioning.
   s_GN_PosProtocol     PosProtocol ;     ///< Protocols supported by the SET.
   
   s_GN_SUPL_V2_SetCapabilities  V2_SetCapabilities;

} s_GN_SetCapabilities;

/// \brief
///      Identifier to indicate the type of SUPL message.
/// \details
/// <pre>
///   UlpMessage ::= CHOICE {
///      msSUPLINIT     SUPLINIT,
///      msSUPLSTART    SUPLSTART,
///      msSUPLRESPONSE SUPLRESPONSE,
///      msSUPLPOSINIT  SUPLPOSINIT,
///      msSUPLPOS      SUPLPOS,
///      msSUPLEND      SUPLEND,
///      msSUPLAUTHREQ  SUPLAUTHREQ,
///      msSUPLAUTHRESP SUPLAUTHRESP,
///      ...}
/// </pre>
typedef enum GN_UlpMessageType{
   GN_UlpMessage_PR_NOTHING, /* No components present */
   GN_UlpMessage_PR_msSUPLINIT,
   GN_UlpMessage_PR_msSUPLSTART,
   GN_UlpMessage_PR_msSUPLRESPONSE,
   GN_UlpMessage_PR_msSUPLPOSINIT,
   GN_UlpMessage_PR_msSUPLPOS,
   GN_UlpMessage_PR_msSUPLEND,
   GN_UlpMessage_PR_msSUPLAUTHREQ,
   GN_UlpMessage_PR_msSUPLAUTHRESP,   
/* +RRR : 26-08-2010 : SUPLv2 */
   
   GN_UlpMessage_PR_msSUPLNOTIFY,
   GN_UlpMessage_PR_msSUPLNOTIFYRESPONSE,
   GN_UlpMessage_PR_msSUPLREPORT,
   GN_UlpMessage_PR_msSUPLTRIGGEREDSTART,
   GN_UlpMessage_PR_msSUPLTRIGGEREDRESPONSE,
   GN_UlpMessage_PR_msSUPLTRIGGEREDSTOP,
   GN_UlpMessage_PR_msSUPLSETINIT
   
/* -RRR : 26-08-2010 : SUPLv2 */
} e_GN_UlpMessageType;

/// \brief
///      Status of the Location ID provided to the SLP.
/// \details
/// <pre>
///   Status ::= ENUMERATED {
///      stale(0),
///      current(1),
///      unknown(2),
///      ...}
/// </pre>
typedef enum GN_LocationID_Status {
   GN_Location_Status_stale,
   GN_Location_Status_current,
   GN_Location_Status_unknown
} e_GN_LocationID_Status;

/// \brief
///      Assistance data required by the SET.
/// \details
/// <pre>
///   RequestedAssistData ::= SEQUENCE {
///      almanacRequested                 BOOLEAN,
///      utcModelRequested                BOOLEAN,
///      ionosphericModelRequested        BOOLEAN,
///      dgpsCorrectionsRequested         BOOLEAN,
///      referenceLocationRequested       BOOLEAN,
///      referenceTimeRequested           BOOLEAN,
///      acquisitionAssistanceRequested   BOOLEAN,
///      realTimeIntegrityRequested       BOOLEAN,
///      navigationModelRequested         BOOLEAN,
///      navigationModelData              NavigationModel OPTIONAL,
///      ...}
///
///   NavigationModel ::= SEQUENCE {
///      gpsWeek  INTEGER(0..1023),
///      gpsToe   INTEGER(0..167),
///      nSAT     INTEGER(0..31),
///      toeLimit INTEGER(0..10),
///      satInfo  SatelliteInfo OPTIONAL,
///      ...}
///
///-- Further information on this fields can be found
///-- in 3GPP TS 44.031 and 49.031
/// SatelliteInfo ::= SEQUENCE (SIZE (1..31)) OF SatelliteInfoElement
///
/// SatelliteInfoElement ::= SEQUENCE {
///    satId INTEGER(0..63),
///    iODE  INTEGER(0..255),
///    ...}
/// </pre>

typedef struct GN_RequestedAssistData {
   BL almanacRequested ;               ///< Flag to indicate whether Almanac is requested.
   BL utcModelRequested ;              ///< Flag to indicate whether UTC Model is requested.
   BL ionosphericModelRequested ;      ///< Flag to indicate whether Ionospheric Model is requested.
   BL dgpsCorrectionsRequested ;       ///< Flag to indicate whether DGPS Corrections are requested.
   BL referenceLocationRequested ;     ///< Flag to indicate whether Reference Location is requested.
   BL referenceTimeRequested ;         ///< Flag to indicate whether Reference Time is requested.
   BL acquisitionAssistanceRequested ; ///< Flag to indicate whether Acquisition Assistance is requested.
   BL realTimeIntegrityRequested ;     ///< Flag to indicate whether Real Time Integrity is requested.
   BL navigationModelRequested ;       ///< Flag to indicate whether Navigation Model is requested.
   BL navigationModelDataPresent ;     ///< Flag to indicate whether Navigation Model data is present.
   U2 gpsWeek ;                        ///< GPS Week of Navigation Model present.
   U1 gpsToe ;                         ///< GPS TOE of Navigation Model present.
   U1 nSAT ;                           ///< Number of Satellites for which Navigation Model is present.
   U1 toeLimit ;                       ///< Limit of the TOE for which Navigation Model is present.
   BL satInfoPresent ;                 ///< Flag to indicate whether a list of Satellites, for which Navigation Model data is present, is present.
   U1 SatID[64] ;                      ///< Satellite ID's in use [range 1..32]
   U1 IODE[64] ;                       ///< Ephemeris IODE for the satellites in use [range 0.255]
} s_GN_RequestedAssistData;

#endif // SUPL_PTYPES_H
