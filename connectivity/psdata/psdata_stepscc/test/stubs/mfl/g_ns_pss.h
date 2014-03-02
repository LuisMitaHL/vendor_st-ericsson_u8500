/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Service category Packet Switched Services (PSS)
 * 
 */

#ifndef _G_NS_PSS_H_
#define _G_NS_PSS_H_

#include "r_sys.h"
#include "r_basicdefinitions.h"
#include "t_basicdefinitions.h"
#include "r_pss.h"

#ifndef NS_CFG_ENABLE_MONOLITHIC_BUILD 
typedef PS_SDU_t * (* Do_PS_SDU_Alloc_V2_t)(const uint16 Size, 
                                            const uint16 Offset,
                                            const boolean IgnorePoolCounterLimitation);
typedef void       (* Do_PS_SDU_Free_t)(const PS_SDU_t ** const SDU_pp);

typedef struct
{
  Do_PS_SDU_Alloc_V2_t     Do_PS_SDU_Alloc_V2_p;
  Do_PS_SDU_Free_t      Do_PS_SDU_Free_p;
} PSS_Body_t;

extern const PSS_Body_t *PSS_Body_p;
#endif /* NS_CFG_ENABLE_MONOLITHIC_BUILD */

/* Declaration of request signals */
SIGID_INTERVAL_START_V2 (REQ_NS_PSS)
  SIGID_DEFINITION_V2(REQUEST_PS_PDP_CONTEXT_ACTIVATE_V3,
                      Request_PS_PDP_Context_Activate_V3_t),
  SIGID_DEFINITION_V2(REQUEST_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V3,
                      Request_PS_PDP_SecondaryContext_Activate_V3_t),
  SIGID_DEFINITION_V2(REQUEST_PS_PDP_CONTEXT_DEACTIVATE_V2,
                      Request_PS_PDP_Context_Deactivate_V2_t),
  SIGID_DEFINITION_V2(REQUEST_PS_PDP_CONTEXT_MODIFY_V3,
                      Request_PS_PDP_Context_Modify_V3_t),
  SIGID_DEFINITION_V2(REQUEST_PS_QOS_PROFILE_READ,
                      Request_PS_QOS_Profile_Read_t),
  SIGID_DEFINITION_V2(REQUEST_PS_TIMEANDDATAVOLUME_READ,
                      Request_PS_TimeAndDataVolume_Read_t),
  SIGID_DEFINITION_V2(REQUEST_PS_PERIODICREPORT_ENABLE,
                      Request_PS_PeriodicReport_Enable_t),
  SIGID_DEFINITION_V2(REQUEST_PS_PERIODICREPORT_DISABLE,
                      Request_PS_PeriodicReport_Disable_t),
  SIGID_DEFINITION_V2(REQUEST_PS_USERPLANESTATUS_READ,
                      Request_PS_UserPlaneStatus_Read_t),
  SIGID_DEFINITION_V2(REQUEST_PS_USERPLANESTATUSREPORT_ENABLE,
                      Request_PS_UserPlaneStatusReport_Enable_t),
  SIGID_DEFINITION_V2(REQUEST_PS_USERPLANESTATUSREPORT_DISABLE,
                      Request_PS_UserPlaneStatusReport_Disable_t),
  SIGID_DEFINITION_V2(REQUEST_PS_USERPLANESETTINGS_MODIFY,
                      Request_PS_UserPlaneSettings_Modify_t),
  SIGID_DEFINITION_V2(REQUEST_PS_USAGESTATUS_READ,
                      Request_PS_UsageStatus_Read_t),
  /*MBMS starts*/
  SIGID_DEFINITION_V2(REQUEST_PS_MBMS_NW_FEATURESUPPORT_GET,
                      Request_PS_MBMS_NW_FeatureSupport_Get_t),
  SIGID_DEFINITION_V2(REQUEST_PS_MBMS_SERVICE_ACTIVATE,
                      Request_PS_MBMS_Service_Activate_t),
  SIGID_DEFINITION_V2(REQUEST_PS_MBMS_SERVICE_DEACTIVATE,
                      Request_PS_MBMS_Service_Deactivate_t),
  SIGID_DEFINITION_V2(REQUEST_PS_MBMS_SESSION_RECEIVE,
                      Request_PS_MBMS_Session_Receive_t),
  /* FastDormancy*/
  SIGID_DEFINITION_V2(REQUEST_PS_FASTDORMANCY_ACTIVATE, 
                      Request_PS_FastDormancy_Activate_t),
SIGID_INTERVAL_END_V2 (REQ_NS_PSS)

/* Declaration of do signals */
SIGID_INTERVAL_START_V2 (DO_NS_PSS)
  SIGID_DEFINITION_V2(DO_PS_UPLINKSDU_SEND_V2,
                      Do_PS_UplinkSDU_Send_V2_t),
  SIGID_DEFINITION_V2(DO_PS_DOWNLINKSDU_ENABLE_V2,
                      Do_PS_DownlinkSDU_Enable_V2_t),
SIGID_INTERVAL_END_V2 (DO_NS_PSS)

#define DO_PS_DOWNLINKSDU_ENABLE DO_PS_DOWNLINKSDU_ENABLE_V2
#define Done_PS_DownlinkSDU_Enable_t Done_PS_DownlinkSDU_Enable_V2_t

#define DO_PS_UPLINKSDU_SEND DO_PS_UPLINKSDU_SEND_V2
#define Do_PS_UplinkSDU_Send_t Do_PS_UplinkSDU_Send_V2_t

#define Event_PS_DownlinkSDU_Receive_t Event_PS_DownlinkSDU_Received_V2_t
#define Event_PS_DownlinkSDU_Received_t Event_PS_DownlinkSDU_Received_V2_t
#define Event_PS_DataPathConnect_t Event_PS_DataPathConnect_Sent_t

#define REQUEST_PS_PDP_CONTEXT_MODIFY_V2 REQUEST_PS_PDP_CONTEXT_MODIFY_V3
#define Request_PS_PDP_Context_Modify_V2_t Request_PS_PDP_Context_Modify_V3_t
#define Response_PS_PDP_Context_Modify_V2_t Response_PS_PDP_Context_Modify_V3_t
#define REQUEST_PS_PDP_CONTEXT_ACTIVATE_V2 REQUEST_PS_PDP_CONTEXT_ACTIVATE_V3
#define Request_PS_PDP_Context_Activate_V2_t Request_PS_PDP_Context_Activate_V3_t
#define REQUEST_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V2 REQUEST_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V3
#define Request_PS_PDP_SecondaryContext_Activate_V2_t Request_PS_PDP_SecondaryContext_Activate_V3_t

#define REQUEST_PS_PDP_CONTEXT_ACTIVATE_V2 REQUEST_PS_PDP_CONTEXT_ACTIVATE_V3
#define Request_PS_PDP_Context_Activate_V2_t Request_PS_PDP_Context_Activate_V3_t
#define REQUEST_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V2 REQUEST_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V3
#define Request_PS_PDP_SecondaryContext_Activate_V2_t Request_PS_PDP_SecondaryContext_Activate_V3_t

// The absolute max length of the APN is 100 octets
// this is the length of the APN + the length octet of the first label
#define PS_MAX_FORMATTED_APN_LENGTH (PS_MAX_APN_LENGTH + 1)

/************************************************************************/
// REQUEST_PS_PDP_CONTEXT_ACTIVATE_V3
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  uint8 AccessPointName[PS_MAX_FORMATTED_APN_LENGTH + 1]; /* + zero term */
  PS_HeaderCompression_t HdrCompression;
  PS_DataCompression_t DataCompression;
  PS_QOS_Type_t QOS_Type;
  boolean QOS_Reverted;
  PS_QOS_Profile_t MinAcceptableQOS;
  PS_QOS_Profile_t RequestedQOS;
  PS_PDP_Type_t PDP_Type;
  PS_PDP_Address_t PDP_Address;
  uint8 ProtocolOptions[PS_MAX_PROTOCOL_OPTIONS_LENGTH];
  uint8 ProtocolOptionsLength;
  PS_Priority_t PS_Priority;
  PS_UserPlaneSettings_t UserPlaneSettings;
} Request_PS_PDP_Context_Activate_V3_t;

// RESPONSE_PS_PDP_CONTEXT_ACTIVATE_V2
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI;
  PS_PDP_ContextActivateResult_t Result;
} Response_PS_PDP_Context_Activate_V2_t;

// REQUEST_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V3
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI_WithSamePDP_Addr;
  PS_HeaderCompression_t HdrCompression;
  PS_DataCompression_t DataCompression;
  PS_QOS_Type_t QOS_Type;
  boolean QOS_Reverted;
  PS_QOS_Profile_t MinAcceptableQOS;
  PS_QOS_Profile_t RequestedQOS;
  PS_TFT_OperationCode_t TFT_OperationCode;
  PS_Priority_t PS_Priority;
  PS_UserPlaneSettings_t UserPlaneSettings;
  PS_TFT_ParameterList_t TFT_ParameterList;
  uint8 ProtocolOptions[PS_MAX_PROTOCOL_OPTIONS_LENGTH];
  uint8 ProtocolOptionsLength;
  PS_TFT_PacketFilter_t TFT_PacketFilter;
} Request_PS_PDP_SecondaryContext_Activate_V3_t;

// RESPONSE_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V2
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI;
  PS_PDP_ContextActivateResult_t Result;
} Response_PS_PDP_SecondaryContext_Activate_V2_t;

// EVENT_PS_PDP_CONTEXTSTATUS
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_PDP_Type_t PDP_Type;
  PS_PDP_Address_t PDP_Address;
  uint8 ProtocolOptions[PS_MAX_PROTOCOL_OPTIONS_LENGTH];
  uint8 ProtocolOptionsLength;
  PS_NSAPI_t NSAPI;
  PS_CauseCode_t Cause;
  PS_PDP_ContextEvent_t ContextEvent;
} Event_PS_PDP_ContextStatus_t;

/************************************************************************/
// REQUEST_PS_PDP_CONTEXT_DEACTIVATE_V2
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI;
  PS_TearDownIndicator_t TearDownIndicator;
  uint8 ProtocolOptions[PS_MAX_PROTOCOL_OPTIONS_LENGTH];
  uint8 ProtocolOptionsLength;
} Request_PS_PDP_Context_Deactivate_V2_t;

// RESPONSE_PS_PDP_CONTEXT_DEACTIVATE_V2
typedef struct
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_GeneralResult_t Result;
} Response_PS_PDP_Context_Deactivate_V2_t;

// EVENT_PS_PDP_CONTEXTDEACTIVATE_V2
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI;
  PS_TimeAndDataVolume_t TimeAndDataVolume;
  uint8 ProtocolOptions[PS_MAX_PROTOCOL_OPTIONS_LENGTH];
  uint8 ProtocolOptionsLength;
  PS_CauseCode_t Cause;
} Event_PS_PDP_ContextDeactivate_V2_t;

/************************************************************************/
// REQUEST_PS_PDP_CONTEXT_MODIFY_V3
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI;
  PS_QOS_Type_t QOS_Type;
  PS_QOS_Profile_t MinAcceptableQOS;
  PS_QOS_Profile_t NewQOS; 
  PS_TFT_OperationCode_t TFT_OperationCode;
  uint8 PacketFiltersToDelete;
  PS_TFT_ParameterList_t TFT_ParameterList;
  uint8 ProtocolOptions[PS_MAX_PROTOCOL_OPTIONS_LENGTH];
  uint8 ProtocolOptionsLength;
  PS_TFT_PacketFilter_t TFT_PacketFilter;
} Request_PS_PDP_Context_Modify_V3_t;

// RESPONSE_PS_PDP_CONTEXT_MODIFY_V3
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  uint8 ProtocolOptions[PS_MAX_PROTOCOL_OPTIONS_LENGTH];
  uint8 ProtocolOptionsLength;
  PS_CauseCode_t Cause;
  PS_GeneralResult_t Result;
} Response_PS_PDP_Context_Modify_V3_t;
/************************************************************************/
// REQUEST_PS_TIMEANDDATAVOLUME_READ
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI;
} Request_PS_TimeAndDataVolume_Read_t;

// RESPONSE_PS_TIMEANDDATAVOLUME_READ
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_TimeAndDataVolume_t TimeAndDataVolume;
  PS_GeneralResult_t Result;
} Response_PS_TimeAndDataVolume_Read_t;

/************************************************************************/
// REQUEST_PS_QOS_PROFILE_READ
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t NSAPI;
} Request_PS_QOS_Profile_Read_t;

// RESPONSE_PS_QOS_PROFILE_READ
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_QOS_Profile_t QOS_Profile;
  PS_GeneralResult_t Result;
} Response_PS_QOS_Profile_Read_t;
/************************************************************************/
// REQUEST_PS_PERIODICREPORT_ENABLE 
typedef struct
{
  SigselectWithClientTag_t  SignalIdWithClientTag;
} Request_PS_PeriodicReport_Enable_t;

// RESPONSE_PS_PERIODICREPORT_ENABLE 
typedef struct
{
  SigselectWithClientTag_t  SignalIdWithClientTag;
  PS_GeneralResult_t        Result;
} Response_PS_PeriodicReport_Enable_t;

/************************************************************************/
// REQUEST_PS_PERIODICREPORT_DISABLE
typedef struct 
{
  SigselectWithClientTag_t  SignalIdWithClientTag;
} Request_PS_PeriodicReport_Disable_t;

// RESPONSE_PS_PERIODICREPORT_DISABLE 
typedef struct
{
  SigselectWithClientTag_t  SignalIdWithClientTag;
  PS_GeneralResult_t        Result;
} Response_PS_PeriodicReport_Disable_t;

/************************************************************************/
// REQUEST_PS_USERPLANESTATUS_READ
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
} Request_PS_UserPlaneStatus_Read_t;

// RESPONSE_PS_USERPLANESTATUS_READ
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
  boolean                    UserPlaneSuspended;
  PS_UserPlaneSuspendCause_t SuspendCause;
  PS_GeneralResult_t         Result;
} Response_PS_UserPlaneStatus_Read_t;

/************************************************************************/
// REQUEST_PS_USERPLANESTATUSREPORT_ENABLE
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
} Request_PS_UserPlaneStatusReport_Enable_t;

// RESPONSE_PS_USERPLANESTATUSREPORT_ENABLE
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
  PS_GeneralResult_t         Result;
} Response_PS_UserPlaneStatusReport_Enable_t;

/************************************************************************/
// REQUEST_PS_USERPLANESTATUSREPORT_DISABLE
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
} Request_PS_UserPlaneStatusReport_Disable_t;

// RESPONSE_PS_USERPLANESTATUSREPORT_DISABLE
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
  PS_GeneralResult_t         Result;
} Response_PS_UserPlaneStatusReport_Disable_t;

/************************************************************************/
// REQUEST_PS_USERPLANESETTINGS_MODIFY
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_NSAPI_t               NSAPI;
  PS_UserPlaneSettings_t   UserPlaneSettings;
} Request_PS_UserPlaneSettings_Modify_t;

//RESPONSE_PS_USERPLANESETTINGS_MODIFY
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_GeneralResult_t       Result;
} Response_PS_UserPlaneSettings_Modify_t;

/************************************************************************/
// REQUEST_PS_USAGESTATUS_READ
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
} Request_PS_UsageStatus_Read_t;

//RESPONSE_PS_USERPLANESETTINGS_MODIFY
typedef struct 
{
  SigselectWithClientTag_t SignalIdWithClientTag;
  PS_UserPlaneType_t       UserPlaneType;
  PS_GeneralResult_t       Result;
} Response_PS_UsageStatus_Read_t;

/************************************************************************/
//DO_PS_UPLINKSDU_SEND_V2
typedef struct
{ 
  SIGSELECT Primitive;
  PS_NSAPI_t NSAPI;
  uint8 DownlinkCredits;
#ifdef USE_PSS_HEAP
  PS_SDU_t* SDU_p;
#else  
  uint8 Reserved[PS_SDU_RESERVED_LENGTH -  
    sizeof(SIGSELECT) -
    sizeof(PS_NSAPI_t)-
    sizeof(uint8)]; 
  //declare the following variable to be compatible
  //with PS_SDU_t structure in t_pss.h
  uint32 *Dummy_p; 
  PS_SDU_Basic_t SDU;
#endif //USE_PSS_HEAP
} Do_PS_UplinkSDU_Send_V2_t;

//DONE_PS_UPLINKSDU_SEND
typedef struct 
{
  SIGSELECT Primitive;
  PS_NSAPI_t NSAPI;
  PS_GeneralResult_t Result;
} Done_PS_UplinkSDU_Send_t;

/************************************************************************/
//DO_PS_DOWNLINKSDU_ENABLE
typedef struct
{ 
  SIGSELECT Primitive;
  PS_NSAPI_t NSAPI;
  uint8 Credits;
} Do_PS_DownlinkSDU_Enable_t;

//DO_PS_DOWNLINKSDU_ENABLE_V2
typedef struct
{ 
  SIGSELECT Primitive;
  PS_Extended_NSAPI_t NSAPI;
  uint8 Credits;
} Do_PS_DownlinkSDU_Enable_V2_t;

//DONE_PS_DOWNLINKSDU_ENABLE_V2
typedef struct 
{
  SIGSELECT Primitive;
  uint8 Credits;
  PS_Extended_NSAPI_t NSAPI;
  PS_GeneralResult_t Result;
} Done_PS_DownlinkSDU_Enable_V2_t;

//EVENT_PS_DOWNLINKSDU_RECEIVED_V2
typedef struct 
{
  SIGSELECT Primitive;
  PS_Extended_NSAPI_t NSAPI; 
  uint8 UplinkCredits;
#ifdef USE_PSS_HEAP
  PS_SDU_t* SDU_p;
#else 
  uint8 Reserved[PS_SDU_RESERVED_LENGTH -
    sizeof(SIGSELECT) -
    sizeof(PS_Extended_NSAPI_t) -
    sizeof(uint8)];  
  //declare the following variable to be compatible
  //with PS_SDU_t structure in t_pss.h
  uint32 *Dummy_p; 
  PS_SDU_Basic_t SDU;
#endif //USE_PSS_HEAP
} Event_PS_DownlinkSDU_Received_V2_t;


//EVENT_PS_UPLINKCREDITS_GRANTED
typedef struct
{
  SIGSELECT               Primitive;
  uint8                   UplinkCredits;
  PS_Extended_NSAPI_t     NSAPI;
} Event_PS_UplinkCredits_Granted_t;

/************************************************************************/
//EVENT_PS_DATAPATHCONNECT_SENT
typedef struct 
{
  SigselectWithClientTag_t    SigselectWithClientTag;
  PS_Extended_NSAPI_t         NSAPI;
} Event_PS_DataPathConnect_Sent_t;

/************************************************************************/
// EVENT_PS_PERIODICREPORT
typedef struct 
{
  SigselectWithClientTag_t  SigselectWithClientTag;
  PS_NSAPI_t                NSAPI;
  PS_TimeAndDataVolume_t    TimeAndDataVolume;
} Event_PS_PeriodicReport_t;

/************************************************************************/
// EVENT_PS_USERPLANESUSPENDED
typedef struct
{
  SigselectWithClientTag_t   SigselectWithClientTag;
  PS_UserPlaneSuspendCause_t SuspendCause;
  uint32                     EstimatedSuspendTime;
} Event_PS_UserPlaneSuspended_t;

/************************************************************************/
// EVENT_PS_USERPLANERESUMED
typedef struct
{
  SigselectWithClientTag_t   SigselectWithClientTag;
} Event_PS_UserPlaneResumed_t;

/************************************************************************/
// EVENT_PS_USAGESTATUS_CHANGED
typedef struct
{
  SigselectWithClientTag_t  SigselectWithClientTag;
  PS_UserPlaneType_t        UserPlaneType; 
} Event_PS_UsageStatus_Changed_t;

/************************************************************************/
// EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
typedef struct
{
  SigselectWithClientTag_t   SigselectWithClientTag;
  PS_NSAPI_t NSAPI;
  uint8 UplinkCredits;
} Event_PS_UserPlaneEstablishmentTemporaryFailed_V2_t;

/****************************** MBMS START ******************************/

/************************************************************************/
// REQUEST_PS_MBMS_NW_FEATURESUPPORT_GET
typedef struct
{
  SigselectWithClientTag_t     SigselectWithClientTag;
} Request_PS_MBMS_NW_FeatureSupport_Get_t;

// RESPONSE_PS_MBMS_NW_FEATURESUPPORT_GET
typedef struct
{
  SigselectWithClientTag_t     SignalIdWithClientTag;
  PS_MBMS_NW_FeatureSupport_t  MBMS_NW_FeatureSupport;
  PS_GeneralResult_t           Result;
} Response_PS_MBMS_NW_FeatureSupport_Get_t;

/************************************************************************/
// REQUEST_PS_MBMS_SERVICE_ACTIVATE
typedef struct
{
  SigselectWithClientTag_t     SigselectWithClientTag;
  PS_Extended_NSAPI_t          Extended_NSAPI;
  PS_MBMS_TMGI_t               TMGI;
  PS_MBMS_Mode_t               MBMS_Mode;
} Request_PS_MBMS_Service_Activate_t;

// RESPONSE_PS_MBMS_SERVICE_ACTIVATE
typedef struct
{
  SigselectWithClientTag_t     SignalIdWithClientTag;
  PS_Extended_NSAPI_t          Extended_NSAPI;
  PS_GeneralResult_t           Result;
} Response_PS_MBMS_Service_Activate_t;

/************************************************************************/
// REQUEST_PS_MBMS_SERVICE_DEACTIVATE
typedef struct
{
  SigselectWithClientTag_t     SigselectWithClientTag;
  PS_Extended_NSAPI_t          Extended_NSAPI;
} Request_PS_MBMS_Service_Deactivate_t;

// RESPONSE_PS_MBMS_SERVICE_DEACTIVATE
typedef struct
{
  SigselectWithClientTag_t     SignalIdWithClientTag;
  PS_GeneralResult_t           Result;
} Response_PS_MBMS_Service_Deactivate_t;

/************************************************************************/
// REQUEST_PS_MBMS_SESSION_RECEIVE
typedef struct
{
  SigselectWithClientTag_t     SigselectWithClientTag;
  PS_MBMS_TMGI_t               TMGI;
  PS_MBMS_SessionID_t          SessionID;
  uint8                        ValidParamsMask;
} Request_PS_MBMS_Session_Receive_t;

// RESPONSE_PS_MBMS_SESSION_RECEIVE
typedef struct
{
  SigselectWithClientTag_t     SignalIdWithClientTag;
  PS_GeneralResult_t           Result;
} Response_PS_MBMS_Session_Receive_t;

/************************************************************************/
//EVENT_PS_MBMS_NW_FEATURESUPPORT_CHANGED
typedef struct 
{
  SigselectWithClientTag_t     SigselectWithClientTag;
  PS_MBMS_NW_FeatureSupport_t  MBMS_NW_FeatureSupport;
} Event_PS_MBMS_NW_FeatureSupport_Changed_t;

/************************************************************************/
//EVENT_PS_MBMS_SERVICESTATE_CHANGED
typedef struct 
{
  SigselectWithClientTag_t     SigselectWithClientTag;
  PS_MBMS_TMGI_t               TMGI;
  PS_MBMS_SessionID_t          SessionID;
  PS_MBMS_EventCause_t         EventCause;
  uint8                        ValidParamsMask;
} Event_PS_MBMS_ServiceState_Changed_t;

/************************************************************************/
//REQUEST_PS_FASTDORMANCY_ACTIVATE
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
} Request_PS_FastDormancy_Activate_t;

//RESPONSE_PS_FASTDORMANCY_ACTIVATE
typedef struct
{
  SigselectWithClientTag_t   SignalIdWithClientTag;
  PS_GeneralResult_t         Result;
} Response_PS_FastDormancy_Activate_t;

#endif // _G_NS_PSS_H_
/* End of file */
