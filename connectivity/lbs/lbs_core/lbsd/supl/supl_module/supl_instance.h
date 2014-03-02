
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_instance.h
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_instance.h 1.70 2009/01/14 14:27:50Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL Instance type declarations.
///
///      This header file contains declarations required to hold the instance
///      specific data for the SUPL subsystem.
///
//*************************************************************************

#ifndef SUPL_INSTANCE_H
#define SUPL_INSTANCE_H

/// Maximum number of SUPL instances valid at a time.
#define INSTANCE_SUPL_MAX     10
/// Maximum value of a SUPL instance.
#define INSTANCE_SUPL_LIMIT   0xffff

#include "gps_ptypes.h"
#include "supl_ptypes.h"
#include "supl_user_timers.h"
#include "pdu_defs.h"
#include "GN_SUPL_api.h"

// The variable names in the following definitions match the corresponding
// SUPL ASN.1 definitions.
// See the SUPL specification for a full description.

/// SUPL Instances possible from 1..65535.
typedef U2 t_SUPL_InstanceId;

/// \brief
///      IP address in either IPV4 or IPV6 format represented as a set of octets. Representing the ASN.1:
/// \details
/// <pre>
/// IPAddress ::= CHOICE {
///   ipv4Address  OCTET STRING(SIZE (4)),
///   ipv6Address  OCTET STRING(SIZE (16))}
/// </pre>
typedef union iPAddress {
   U1  ipv4Address[4];              ///<  4 octets for an IPV4 address.
   U1  ipv6Address[16];             ///< 16 octets for an IPV6 address.
} u_iPAddress;


/// Type of ID provided in #s_SetId.
typedef enum{
   GN_SETId_PR_NOTHING,             /* No components present */
   GN_SETId_PR_msisdn,
   GN_SETId_PR_mdn,
   GN_SETId_PR_min,
   GN_SETId_PR_imsi,
   GN_SETId_PR_nai,
   GN_SETId_PR_iPAddressV4,
   GN_SETId_PR_iPAddressV6
} e_SetIdType;


/// \brief
///      SET ID for use in a SUPL message. Representing the ASN.1:
/// \details
/// <pre>
/// SETId ::= CHOICE {
///   msisdn     OCTET STRING(SIZE (8)),
///   mdn        OCTET STRING(SIZE (8)),
///   min        BIT STRING(SIZE (34)), -- coded according to TIA-553
///   imsi       OCTET STRING(SIZE (8)),
///   nai        IA5String(SIZE (1..1000)),
///   iPAddress  IPAddress,
///   ...}
/// </pre>
typedef struct SetId {
   e_SetIdType     type;            ///< Type of SET Id.
   union {
      U1  msisdn[8];                ///< Mobile Station ISDN.
      U1  mdn[8];                   ///< Mobile Directory Number
      U1  min[(34/8)+1];            ///< Mobile Idenitification Number
      U1  imsi[8];                  ///< International Mobile Subscriber Identity.
      U1  *nai;                     ///< Network Access Identifier.
      u_iPAddress  iPAddress;       ///< IPV4 or IPV6 IP Address.
   } u;                             ///< Choice of the SET Id.
} s_SetId;


/// Types of SlpId possible.
typedef enum{
   GN_SLPAddress_PR_NOTHING,        /* No components present */
   GN_SLPAddress_PR_iPV4Address,
   GN_SLPAddress_PR_iPV6Address,
   GN_SLPAddress_PR_fQDN
} e_SlpIdType;


/// Local definition for SLP Address.
typedef struct GN_SLPAddress {
   e_SlpIdType     type;            ///< Type of SlpId present.
   union {
      u_iPAddress  iPAddress;       ///< IP Address.
      CH           FQDN[256];       ///< Fully Qualified Domain Name - Size is 255, plus 1 for string terminator.
   } u;                             ///< Choice of the IP address.
} s_GN_SLPAddress;


/// Enumeration identifying the positioning method.
typedef enum GN_PosMethod {
   GN_PosMethod_agpsSETassisted,    ///< SET-Assisted mode Only
   GN_PosMethod_agpsSETbased,       ///< SET-Based    mode Only
   GN_PosMethod_agpsSETassistedpref,///< SET-Assisted & SET-Based, but SET-Assisted preferred
   GN_PosMethod_agpsSETbasedpref,   ///< SET-Assisted & SET-Based, but SET-Based    preferred
   GN_PosMethod_autonomousGPS,      ///< Autonomous GPS
   GN_PosMethod_aFLT,               ///< AFLT
   GN_PosMethod_eCID,               ///< Enhanced Cell ID
   GN_PosMethod_eOTD,               ///< EOTD
   GN_PosMethod_oTDOA,              ///< O-TDOA
   GN_PosMethod_noPosition          ///< No position is used for notification and verification of a network based positioning sequence.
   /*
    * Enumeration is extensible
    */
} e_GN_PosMethod;

/// Enumeration identifying the permissable states within the SUPL subsystem
typedef enum {
   state_SUPL_Idle,                                      // 0
   state_SUPL_Push_Received,                             // 1
   state_SUPL_Comms_Open_Sent,                           // 2
   state_SUPL_SUPL_START_Sent,                           // 3
   state_SUPL_SUPL_RESPONSE_Sent,                        // 4

   state_SUPL_SUPL_POS_INIT_Sent,                        // 5
   state_SUPL_Waiting_For_RRLP,                          // 6
   state_SUPL_POS_Payload_Delivered,                     // 7
   state_SUPL_SUPL_POS_Sent,                             // 8
   state_SUPL_SUPL_END_Sent,

   state_SUPL_Server_Open_Requested,                     // 9
   state_SUPL_Server_Active,                             //10
   state_SUPL_Comms_Close_Sent,                          //11
   state_SUPL_Awaiting_Confirmation,                     //12
/* +LMSqb89433 */   
   state_SUPL_Awaiting_Confirmation_Or_Comms_Open_Sent,  //13
/* -LMSqb89433 */   
   state_SUPL_MO_Position_Requested,                     //14

/* RRR : 26-08-2010 : SUPL-2.0 specific states */
   state_SUPLv2_SUPL_TRIGGER_START_Sent,                 //15
   state_SUPLv2_SUPL_TRIGGER_Active,                     //16
   state_SUPLv2_SUPL_TRIGGER_Suspended,                  //17
   state_SUPLv2_Location_Notification,                   //18
   state_SUPLv2_SUPL_SET_INIT_Sent,                      //19
/* RRR : 26-08-2010 : SUPL-2.0 specific states */
   state_SUPL_Error                                      //20
} e_SUPL_States;


/// \brief
///      Quality of Position definition.
/// \details
/// <pre>
///   QoP ::= SEQUENCE {
///      horacc      INTEGER(0..127),
///      veracc      INTEGER(0..127) OPTIONAL, -- as defined in 3GPP TS 23.032 “uncertainty altitude”—
///      maxLocAge   INTEGER(0..65535) OPTIONAL,
///      delay       INTEGER(0..7) OPTIONAL, -- as defined in 3GPP TS 44.031
///      ...}
/// </pre>
typedef struct { // s_GN_QoP
   U1 horacc;        ///< horacc      INTEGER(0..127)
   U1 veracc;        ///< veracc      INTEGER(0..127) OPTIONAL -- as defined in 3GPP TS 23.032 “uncertainty altitude”—
   U2 maxLocAge;     ///< maxLocAge   INTEGER(0..65535) OPTIONAL [seconds] 
   U1 delay;         ///< delay       INTEGER(0..7) OPTIONAL -- as defined in 3GPP TS 44.031
   U1 *p_horacc;     ///< pointer to #s_GN_QoP:horacc    or NULL if #s_GN_QoP:horacc    is not present.
   U1 *p_veracc;     ///< pointer to #s_GN_QoP:veracc    or NULL if #s_GN_QoP:veracc    is not present.
   U2 *p_maxLocAge;  ///< pointer to #s_GN_QoP:maxLocAge or NULL if #s_GN_QoP:maxLocAge is not present.
   U1 *p_delay;      ///< pointer to #s_GN_QoP:delay     or NULL if #s_GN_QoP:delay     is not present.
} s_GN_QoP;


/// \brief
///      Indicator for presence of velocity information.
/// \details
/// <pre>
///   Velocity ::= CHOICE { -- velocity definition as per 23.032
///      horvel           Horvel,
///      horandvervel     Horandvervel,
///      horveluncert     Horveluncert,
///      horandveruncert  Horandveruncert,
///      ...}
/// </pre>
typedef enum {
   GN_novelocity,
   GN_horvel,
   GN_horandvervel,
   GN_horveluncert,
   GN_horandveruncert
} e_GN_VelocityType;


/// \brief
///      Indicator for mode SLP supports.
/// \details
/// <pre>
/// SLPMode ::= ENUMERATED {proxy(0), nonProxy(1)}
/// </pre>
typedef enum {
   GN_proxy,                        ///< SLP works in Proxy Mode.
   GN_nonProxy                      ///< SLP works in Non Proxy Mode.
} e_GN_SLPMode;


/// \brief
///      GN representation of the Position entity.
/// \details
/// <pre>
///   Position ::= SEQUENCE {
///      timestamp         UTCTime, -- shall include seconds and shall use UTC time.
///      positionEstimate  PositionEstimate,
///      velocity          Velocity OPTIONAL,
///      ...}
///
///   PositionEstimate ::= SEQUENCE {
///      latitudeSign  ENUMERATED {north, south},
///      latitude      INTEGER(0..8388607),
///      longitude     INTEGER(-8388608..8388607),
///      uncertainty
///         SEQUENCE {uncertaintySemiMajor  INTEGER(0..127),
///                   uncertaintySemiMinor  INTEGER(0..127),
///                   orientationMajorAxis  INTEGER(0..180)} OPTIONAL,
///      -- angle in degree between major axis and North
///      confidence    INTEGER(0..100) OPTIONAL,
///      altitudeInfo  AltitudeInfo OPTIONAL,
///      ...}
/// </pre>
typedef struct GN_Position { // s_GN_Position
   U4 OS_Time_ms;                   ///< OS Timestamp of position solution.
   BL Valid_2D_Fix;                 ///< Is the published 2D position fix "Valid" relative to the required Horizontal Accuracy Masks ?
   BL Valid_3D_Fix;                 ///< Is the published 3D position fix "Valid" relative to both the required Horizontal and Vertical Accuracy Masks ?
   //           YYYYMMDDHHMMSS Z+EOS
   CH timestamp[   4+2+2+2+2+2+1+1+  1]; ///< Time Stamp of Position solution in UTC.
   U1 latitudeSign;                 ///< GAD latitide sign [0 = North, 1 = South]
   U4 latitude;                     ///< GAD latitude
   I4 longitude;                    ///< GAD longitude
   BL uncertaintyPresent;           ///< GAD uncertainty present ?
   U1 uncertaintySemiMajor;         ///< GAD uncertainty semi-major axis
   U1 uncertaintySemiMinor;         ///< GAD uncertainty semi-minor axis
   U1 orientationMajorAxis;         ///< GAD orientation of semi-major axis
   BL confidencePresent;            ///< GAD confidence present ?
   U1 confidence;                   ///< GAD confidence
   BL altitudeInfoPresent;          ///< GAD altitude info present ?
   // Altitude Info
   U1 altitudeDirection;            ///< GAD altitude direction [0 = height, 1 = depth]
   U2 altitude;                     ///< GAD altitude
   U1 altUncertainty;               ///< GAD alttitude uncertainty
   BL velocityPresent;              ///< GAD velocity present ?
   // Velocity
   e_GN_VelocityType VelocityIndicator;   ///< Velocity Indicator identifying which velocity elements are present.
   U4 verdirect;                    ///< GAD [0 = up, 1 = down]
   U4 bearing;                      ///< GAD bearing [degrees]
   U4 horspeed;                     ///< GAD horizontal speed
   U4 verspeed;                     ///< GAD vertical   speed
   U4 horuncertspeed;               ///< GAD horizontal speed uncertainty
   U4 veruncertspeed;               ///< GAD vertical   speed uncertainty
} s_GN_Position;


/// \brief
///      GN representation for the status code returned in SUPL messages.
/// \details
/// <pre>
///   StatusCode ::= ENUMERATED {
///      unspecified(0), systemFailure(1), unexpectedMessage(2), protocolError(3),
///      dataMissing(4), unexpectedDataValue(5), posMethodFailure(6),
///      posMethodMismatch(7), posProtocolMismatch(8), targetSETnotReachable(9),
///      versionNotSupported(10), resourceShortage(11), invalidSessionId(12),
///      nonProxyModeNotSupported(13), proxyModeNotSupported(14),
///      positioningNotPermitted(15), authNetFailure(16), authSuplinitFailure(17), consentDeniedByUser(100),
///      consentGrantedByUser(101), ...
///      }
/// </pre>
typedef enum GN_StatusCode {
   GN_StatusCode_NONE,
   GN_StatusCode_unspecified,
   GN_StatusCode_systemFailure,
   GN_StatusCode_unexpectedMessage,
   GN_StatusCode_protocolError,
   GN_StatusCode_dataMissing,
   GN_StatusCode_unexpectedDataValue,
   GN_StatusCode_posMethodFailure,
   GN_StatusCode_posMethodMismatch,
   GN_StatusCode_posProtocolMismatch,
   GN_StatusCode_targetSETnotReachable,
   GN_StatusCode_versionNotSupported,
   GN_StatusCode_resourceShortage,
   GN_StatusCode_invalidSessionId,
   GN_StatusCode_nonProxyModeNotSupported,
   GN_StatusCode_proxyModeNotSupported,
   GN_StatusCode_positioningNotPermitted,
   GN_StatusCode_authNetFailure,
   GN_StatusCode_authSuplinitFailure,
   GN_StatusCode_consentDeniedByUser,
   GN_StatusCode_consentGrantedByUser,

   //SUPLV2.0 related Status code 
   GN_StatusCode_ver2_sessionStopped
} e_GN_StatusCode;


typedef struct
{
   BL PT_agpsSETAssisted;        ///< Positioning Technology AGPS SET Assisted is supported if TRUE.
   BL PT_agpsSETBased;           ///< Positioning Technology AGPS SET Based is supported if TRUE.
   BL PT_autonomousGPS;          ///< Positioning Technology Autonomous GPS is supported if TRUE.
   BL PT_eCID;                   ///< Positioning Technology Enhanced Cell ID is supported if TRUE.
} s_GN_SUPL_PosTech;

typedef struct
{
   s_GN_SUPL_Connect_Req_Parms   SUPL_Connect_Req_Parms;    ///< Store for keeping the SUPL connection parameters for this instance.
   s_GN_SUPL_PosTech        SUPL_PT_Supported;
} s_GN_SUPL_SessionConfig;

typedef struct
{
   s_GN_GPS_SUPL_Timer           UT1;                       ///< User Timer 1.
   s_GN_GPS_SUPL_Timer           UT2;                       ///< User Timer 2.
   s_GN_GPS_SUPL_Timer           UT3;                       ///< User Timer 3.
   s_GN_GPS_SUPL_Timer           UT4;                       ///< User Timer 4.
} s_GN_SUPL_UserTimers;




typedef struct
{
   s_GN_GPS_SUPL_Timer     UT5;  ///< From sending of SUPL NOTIFY RESPONSE to receipt of SUPL END. Only applicable to “notification based on location”scenarios.
   s_GN_GPS_SUPL_Timer     UT6;  ///< From sending of SUPL REPORT to receipt of SUPLNOTIFY or SUPL END. Only applicable to “notification based on location” in nonproxy mode scenarios.
   s_GN_GPS_SUPL_Timer     UT7;  ///< Only applicable to triggered scenarios.From sending of SUPL TRIGGERED STOP to receipt of SUPL END.
   s_GN_GPS_SUPL_Timer     UT8;  ///< Only applicable to triggered periodic scenarios.From sending the last SUPL REPORT message to receipt of SUPL END.
   s_GN_GPS_SUPL_Timer     UT9;  ///< Only applicable to SET Initiated Location Request of Another SET.From sending of SUPL SET INIT to receipt of SUPL END.
   s_GN_GPS_SUPL_Timer     UT10; ///< Only applicable to the Session Info Query scenarios.From sending of SUPL REPORT to receipt of SUPL END for the Session Info Query Session.
} s_GN_SUPL_V2_UserTimers;

typedef struct{
    s_GN_SUPL_V2_UserTimers      UserTimers;
    s_GN_SUPL_V2_Triggers        Triggers;
    s_GN_SUPL_V2_ThirdPartyInfo  *p_ThirdPartyData;  /// To collect/update ThirdParty info. incase of SI session in SUPL_START,SUPL_TRIGGERED_START,SUPL_SET_INIT messages
    s_GN_SUPL_V2_ApplicationInfo *p_ApplicationID;    //p_ApplicationID info for SUPL transactions, SUPL_START,SUPL_TRIGGERED_START,SUPL_SET_INIT
    BL                           Emergency_CallReq; //<Flag to indicate emergency location request>
    CH                           Eslp_FQDN[256];    //<To store the E-SLP Address from the N/W>
    BL                           Eslp_Address_Present;  //<Flsg to indicate the precence of the E-SLP Address in SUPL INIT>
    BL                           ThirdParty_Retrieve;
    s_GN_SUPL_V2_SupportedNetworkInfo *p_SupportedNetworkInfo;
} s_GN_SUPL_V2_InstanceData;



/// The SUPL instance data describes data needed for each instance of the SUPL Agent.
typedef struct SUPL_InstanceData
{
   e_SUPL_States                 State;                     ///< Current SUPL State for this instance.
   U2                            SetSessionId;              ///< Session ID held for the Set. 
   s_GN_SUPL_SessionConfig       SessionCfg;                ///< SUPL configuration when the SUPL instance was created
   s_GN_SUPL_UserTimers          UserTimers;                ///< User Timers Associated with SUPL session
   s_SetId                       SetId;                     ///< Set ID used in PDU.
   U1                            *p_SlpSessionId;           ///< Session ID held for the Slp.
   s_GN_SLPAddress               SlpId;                     ///< Slp ID used in PDU.
   s_PDU_Encoded                 SUPL_INIT;                 ///< Store for SUPL-INIT.
   s_GN_QoP                      *p_GN_QoP;                 ///< Store for the QoP parameters.
   s_LocationId                  LocationId;                ///< Store for the Location Id to pass back to the server for the reference position.
   e_GN_PosMethod                GN_PosMethod_Requested;    ///< Store for the PosMethod received in a SUPL-INIT.
   s_GN_SUPL_Notification_Parms  SUPL_Notification;         ///< Store for data passed in a SUPL notification.
   BL                            Notification_Accepted;     ///< Flag to indicate the user response to the indication.
   BL                            ServerConnectionAvailable; ///< Flag to indicate that a connection to the server was in place.
   s_GN_RequestedAssistData      GN_RequestedAssistData;    ///< Identifier for Assistance Data required.
   BL                            MO_Request;                ///< Flag to ensure we deal with MO requests correctly.
   U1                            *p_VER ;                   ///< Store for VER which is used in SUPL PDUs.
   e_GN_StatusCode               ErrorStatus;               ///< Store for Status Code to be returned.
   BL                            PrivacyMode;               ///< Flag to ensure we deal correctly with Privacy Mode
   BL                            NetworkInitiated;          ///< Flag to indicated if Network Initiated Session
   long                          TransactionSuplVersion ;   ///< Field used to store Major Version supported for this transaction in NI cases.
   s_GN_SUPL_V2_InstanceData     V2_Data;                   ///< Store all SUPLv2.0 Instance Data

} s_SUPL_InstanceData;


//*************************************************************************
/// \brief
///     Definition of the basic SUPL instance type.
//*************************************************************************
typedef struct SUPL_Instance
{
   t_SUPL_InstanceId     ThisInstanceId;     ///< Identifies the instance ID. Range [1..65535]
   BL                    EventPending;       ///< Can be set during processing to indicate that more processing is necessary.
   void                 *Handle;             ///< Abstract Handle.
   void                 *POS_Handle;         ///< Abstract Handle.
   s_SUPL_InstanceData  *p_SUPL;             ///< SUPL specific data.
   struct SUPL_Instance *Peer_SUPL_Instance; ///< Indicates peer SUPL instance (NULL if invalid)
} s_SUPL_Instance;

//*************************************************************************
/// \brief
///     Container for instances.
//*************************************************************************
typedef struct SUPL_Instances
{
   U1                MaxInstanceCount;                   ///< Maximum number of instances supported.
   t_SUPL_InstanceId InstanceIdHighWaterMark;            ///< Identifies the highest assigned instance ID. Range [1..65535]
   s_SUPL_Instance   *InstanceList[INSTANCE_SUPL_MAX];   ///< Container for instance data.
} s_SUPL_Instances;

extern s_SUPL_Instances SUPL_Instances;      ///< Container for all valid SUPL instances.


//*****************************************************************************
/// \brief
///      Measured Results from a WCDMA network.
/// \details
///      Set of measured results from a WCDMA network.

typedef struct s_GN_SUPL_V2_Multiple_Loc_ID
{
    struct s_GN_SUPL_V2_Multiple_Loc_ID    *next;
    struct s_GN_SUPL_V2_Multiple_Loc_ID    *prev;
    s_LocationId                            v_LocationId;
    U4                                      v_OSTimeMs;      ///< Local Operating System Time [msec] 
}  s_GN_SUPL_V2_Multiple_Loc_ID ;


void SUPL_Instance_Handler(       void );
void SUPL_Instances_Initialise(   U1 MaxInstanceCount );
void SUPL_Instances_DeInitialise( void );

s_SUPL_Instance *SUPL_Instance_Get_Data(         s_SUPL_Instance *p_ThisInstance );
s_SUPL_Instance *SUPL_Instance_Request_New(      void );
s_SUPL_Instance *SUPL_Instance_Base_Request_New( s_SUPL_Instances *p_Instances );
s_SUPL_Instance *SUPL_Instance_From_Handle(      void *Handle );
s_SUPL_Instance *SUPL_Instance_From_POS_Handle(  void *POS_Handle );

BL   SUPL_Instance_Delete(           s_SUPL_Instance *p_ThisInstance );
BL   SUPL_Instance_Base_Delete(      s_SUPL_Instance *p_ThisInstance );
CH *SUPL_StateDesc_From_State( e_SUPL_States State );

void SUPL_Instance_State_Transition( s_SUPL_Instance *p_ThisInstance, e_SUPL_States New_State, char *Cause );

void supl_Free_LocationId_Data( s_LocationId *p_LocationId );

void supl_Free_ThirdParty_Data( s_GN_SUPL_V2_ThirdPartyInfo *p_ThirdParty_Data );
void supl_Free_ApplicationID_Data(s_GN_SUPL_V2_ApplicationInfo *p_ApplicationID);
#endif // SUPL_INSTANCE_H
