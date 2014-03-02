/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Service category Packet Switched Services (PSS)
 * 
 */

#ifndef INCLUSION_GUARD_R_PSS_H
#define INCLUSION_GUARD_R_PSS_H

#include "r_sys.h"
#include "t_basicdefinitions.h"
#include "t_pss.h"

SIGID_INTERVAL_START_V2(CAT_ID_PSS)
  SIGID_INTERVAL_DEFINITION_V2(REQ_NS_PSS, 35),
  SIGID_INTERVAL_DEFINITION_V2(RESP_PSS_NS, 35),
  SIGID_INTERVAL_DEFINITION_V2(EVENT_PSS_NS, 30),
  SIGID_INTERVAL_DEFINITION_V2(DO_NS_PSS, 10),
  SIGID_INTERVAL_DEFINITION_V2(DONE_PSS_NS, 10),
  SIGID_INTERVAL_DEFINITION_V2(CONFIRM_NS_PSS, 0),
  SIGID_INTERVAL_DEFINITION_V2(MOD_NS_PSS, 0), 
SIGID_INTERVAL_END_V2(CAT_ID_PSS)

#ifndef SKIP_IN_REF_MAN //Multicast only
/**
 * @param EVENT_ID_PS_DATAPATHCONNECT_SENT
 * @param EVENT_ID_PS_PERIODICREPORT
 * @param EVENT_ID_PS_USERPLANESUSPENDED
 * @param EVENT_ID_PS_USERPLANERESUMED
 * @param EVENT_ID_PS_USAGESTATUS_CHANGED
 * @param EVENT_ID_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
 * @param EVENT_ID_PS_MBMS_SERVICESTATE_CHANGED
 * @param EVENT_ID_PS_MBMS_NW_FEATURESUPPORT_CHANGED
 * @param EVENT_ID_PS_MBMS_CONTEXTACTIVATION_REQUESTED
 */
#else //SKIP_IN_REF_MAN
/**
 * @param EVENT_ID_PS_DATAPATHCONNECT_SENT
 * @param EVENT_ID_PS_PERIODICREPORT
 * @param EVENT_ID_PS_USERPLANESUSPENDED
 * @param EVENT_ID_PS_USERPLANERESUMED
 * @param EVENT_ID_PS_MBMS_SERVICESTATE_CHANGED
 * @param EVENT_ID_PS_MBMS_NW_FEATURESUPPORT_CHANGED
 */
#endif //SKIP_IN_REF_MAN
EVENTID_INTERVAL_START(CAT_ID_PSS)
  EVENTID_DEFINITION(EVENT_ID_PS_DATAPATHCONNECT_SENT),
  EVENTID_DEFINITION(EVENT_ID_PS_PERIODICREPORT),
  EVENTID_DEFINITION(EVENT_ID_PS_USERPLANESUSPENDED),
  EVENTID_DEFINITION(EVENT_ID_PS_USERPLANERESUMED),
  EVENTID_DEFINITION(EVENT_ID_PS_USAGESTATUS_CHANGED),  
  EVENTID_DEFINITION(EVENT_ID_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2),  
  EVENTID_DEFINITION(EVENT_ID_PS_MBMS_SERVICESTATE_CHANGED),
  EVENTID_DEFINITION(EVENT_ID_PS_MBMS_NW_FEATURESUPPORT_CHANGED),
#ifndef SKIP_IN_REF_MAN //Multicast only
  EVENTID_DEFINITION(EVENT_ID_PS_MBMS_CONTEXTACTIVATION_REQUESTED),
#endif //SKIP_IN_REF_MAN
EVENTID_INTERVAL_END(CAT_ID_PSS)

/* Declaration of response signals */
SIGID_INTERVAL_START_V2 (RESP_PSS_NS)
  SIGID_DEFINITION_V2(RESPONSE_PS_PDP_CONTEXT_ACTIVATE_V2, Response_PS_PDP_Context_Activate_V2_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V2, Response_PS_PDP_SecondaryContext_Activate_V2_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_PDP_CONTEXT_DEACTIVATE_V2, Response_PS_PDP_Context_Deactivate_V2_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_PDP_CONTEXT_MODIFY_V3, Response_PS_PDP_Context_Modify_V3_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_QOS_PROFILE_READ, Response_PS_QOS_Profile_Read_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_TIMEANDDATAVOLUME_READ, Response_PS_TimeAndDataVolume_Read_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_PERIODICREPORT_ENABLE, Response_PS_PeriodicReport_Enable_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_PERIODICREPORT_DISABLE, Response_PS_PeriodicReport_Disable_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_USERPLANESETTINGS_MODIFY, Response_PS_UserPlaneSettings_Modify_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_USERPLANESTATUS_READ, Response_PS_UserPlaneStatus_Read_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_USERPLANESTATUSREPORT_ENABLE, Response_PS_UserPlaneStatusReport_Enable_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_USERPLANESTATUSREPORT_DISABLE, Response_PS_UserPlaneStatusReport_Disable_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_USAGESTATUS_READ, Response_PS_UsageStatus_Read_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_MBMS_SERVICE_ACTIVATE, Response_PS_MBMS_Service_Activate_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_MBMS_SERVICE_DEACTIVATE, Response_PS_MBMS_Service_Deactivate_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_MBMS_SESSION_RECEIVE, Response_PS_MBMS_Session_Receive_t),
  SIGID_DEFINITION_V2(RESPONSE_PS_MBMS_NW_FEATURESUPPORT_GET, Response_PS_MBMS_NW_FeatureSupport_Get_t),
#ifndef SKIP_IN_REF_MAN //Fast dormancy, customization
  SIGID_DEFINITION_V2(RESPONSE_PS_FASTDORMANCY_ACTIVATE, Response_PS_FastDormancy_t),
#endif //SKIP_IN_REF_MAN  
#ifndef SKIP_IN_REF_MAN //Multicast only
  SIGID_DEFINITION_V2(RESPONSE_PS_MBMS_CONTEXT_ACTIVATE, Response_PS_MBMS_Context_Activate_t),
#endif //SKIP_IN_REF_MAN
SIGID_INTERVAL_END_V2 (RESP_PSS_NS)

/* Declaration of done signals */
#ifndef SKIP_IN_REF_MAN 
SIGID_INTERVAL_START_V2 (DONE_PSS_NS)
  SIGID_DEFINITION_V2(DONE_PS_UPLINKSDU_SEND, Done_PS_UplinkSDU_Send_t),
  SIGID_DEFINITION_V2(DONE_PS_DOWNLINKSDU_ENABLE_V2, Done_PS_DownlinkSDU_Enable_V2_t),  
SIGID_INTERVAL_END_V2 (DONE_PSS_NS)
#endif //SKIP_IN_REF_MAN

/* Declaration of event signals */
SIGID_INTERVAL_START_V2 (EVENT_PSS_NS)
  SIGID_DEFINITION_V2(EVENT_PS_PDP_CONTEXTSTATUS, Event_PS_PDP_ContextStatus_t),
  SIGID_DEFINITION_V2(EVENT_PS_PDP_CONTEXTDEACTIVATE_V2, Event_PS_PDP_ContextDeactivate_V2_t),
  SIGID_DEFINITION_V2(EVENT_PS_DOWNLINKSDU_RECEIVED_V2, Event_PS_DownlinkSDU_Received_V2_t),
  SIGID_DEFINITION_V2(EVENT_PS_DATAPATHCONNECT_SENT, Event_PS_DataPathConnect_Sent_t),  
  SIGID_DEFINITION_V2(EVENT_PS_PERIODICREPORT, Event_PS_PeriodicReport_t),
  SIGID_DEFINITION_V2(EVENT_PS_USERPLANESUSPENDED, Event_PS_UserPlaneSuspended_t),
  SIGID_DEFINITION_V2(EVENT_PS_USERPLANERESUMED, Event_PS_UserPlaneResumed_t),
  SIGID_DEFINITION_V2(EVENT_PS_USAGESTATUS_CHANGED, Event_PS_UsageStatus_Changed_t),
  SIGID_DEFINITION_V2(EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2,Event_PS_UserPlaneEstablishmentTemporaryFailed_V2_t),
  SIGID_DEFINITION_V2(EVENT_PS_MBMS_SERVICESTATE_CHANGED, Event_PS_MBMS_ServiceState_Changed_t),
  SIGID_DEFINITION_V2(EVENT_PS_MBMS_NW_FEATURESUPPORT_CHANGED, Event_PS_MBMS_NW_FeatureSupport_Changed_t),
  SIGID_DEFINITION_V2(EVENT_PS_UPLINKCREDITS_GRANTED, Event_PS_UplinkCredits_Granted_t), 
#ifndef SKIP_IN_REF_MAN  //Multicast only
  SIGID_DEFINITION_V2(EVENT_PS_MBMS_CONTEXTACTIVATION_REQUESTED, Event_PS_MBMS_ContextActivation_Requested_t),
  SIGID_DEFINITION_V2(EVENT_PS_MBMS_CONTEXTSTATUS_CHANGED, Event_PS_MBMS_ContextStatus_Changed_t), 
#endif //SKIP_IN_REF_MAN
SIGID_INTERVAL_END_V2 (EVENT_PSS_NS)

#ifndef SKIP_IN_REF_MAN
#define EVENT_ID_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED EVENT_ID_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
#define EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
#endif //SKIP_IN_REF_MAN

/************************************************************************/
// Request_PS_PDP_Context_Activate_V3
/************************************************************************/
/**
 * Request activation of a PDP context. The caller (i.e. the service user) will 
 * have to use the value in the output parameter NSAPI_p, when addressing this 
 * specific PDP context in future global service requests.
 *
 * When the result code PS_PDP_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS is returned, 
 * the context activation procedure has started and the final outcome of the 
 * activation attempt is delivered to the service user in the signal 
 * EVENT_PS_PDP_CONTEXTSTATUS.
 *
 * @param [in] RequestCtrl_p         Pointer to struct controlling whether the 
 *                                   request is called in wait mode or no wait 
 *                                   mode.
 * @param [in] SigStruct_p           Response signal to be unpacked.
 * @param [in] AccessPointName_p     Pointer to a string containing the 
 *                                   logical name of the access point.
 * @param [in] HeaderCompression     Flag indicating whether the service user 
 *                                   wishes header compression to be used or 
 *                                   not.
 * @param [in] DataCompression       Flag indicating whether the service user 
 *                                   wishes data compression to be used or not.
 * @param [in] QOS_Type              Type of Quality of Service profile 
 *                                   requested.
 * @param [in] QOS_Reverted          Flag indicating whether revert 
 *                                   Quality of Service to old version or not.
 * @param [in] MinAcceptableQOS_p    Pointer to the minimum acceptable Quality 
 *                                   of Service.
 * @param [in] RequestedQOS_p        Pointer to the Quality of Service profile 
 *                                   requested.
 * @param [in] PDP_Type              Type of Packet Data Protocol (PDP).
 * @param [in] PDP_Address_p         Pointer to the PDP address of the service 
 *                                   user. A service user requesting a dynamic 
 *                                   IP address should set all valid elements 
 *                                   for that PDP type equal to the value 
 *                                   PS_PDP_ADDRESS_UNSPECIFIED.
 * @param [in] ProtocolOptions_p     Pointer to the protocol configuration 
 *                                   options that the service user wishes to 
 *                                   negotiate. Note that the first two octets, 
 *                                   i.e. the protocol configuration option IEI 
 *                                   and the length field, shall not be included.
 * @param [in] ProtocolOptionsLength Length of the requested protocol 
 *                                   configuration options parameter string.
 * @param [in] PS_Priority           Indicates if the PDP context shall be 
 *                                   prioritized over CS services.
 * @param [in] UserPlaneSettings_p   Contains user plane settings regarding AQM and
 *                                   packet prioritization
 * @param [out] NSAPI_p              Pointer to a value identifying the 
 *                                   activated PDP context.
 * @param [out] Result_p             Pointer to a value specifying the current 
 *                                   activation status of the PDP context 
 *                                   activation request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_PDP_CONTEXT_ACTIVATE_V2
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related EVENT_PS_PDP_CONTEXTSTATUS, EVENT_PS_PDP_CONTEXTDEACTIVATE_V2
 */

RequestStatus_t
Request_PS_PDP_Context_Activate_V3(
  const RequestControlBlock_t *const RequestCtrl_p,
  const uint8 *const AccessPointName_p,
  const PS_HeaderCompression_t HeaderCompression,
  const PS_DataCompression_t DataCompression,
  const PS_QOS_Type_t QOS_Type,
  const boolean QOS_Reverted,  
  const PS_QOS_Profile_t *const MinAcceptableQOS_p,
  const PS_QOS_Profile_t *const RequestedQOS_p,
  const PS_PDP_Type_t PDP_Type,
  const PS_PDP_Address_t *const PDP_Address_p,
  const uint8 *const ProtocolOptions_p,
  const uint8 ProtocolOptionsLength,
  const PS_Priority_t PS_Priority,
  const PS_UserPlaneSettings_t *const UserPlaneSettings_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_PDP_ContextActivateResult_t *const Result_p);

/************************************************************************/
// Response_PS_PDP_Context_Activate_V2
/************************************************************************/
/**
 * Response function for Request_PS_PDP_Context_Activate_V3 
 *
 */

RequestStatus_t
Response_PS_PDP_Context_Activate_V2(
  const void *const SigStruct_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_PDP_ContextActivateResult_t *const Result_p);

#ifndef SKIP_IN_REF_MAN
#define Request_PS_PDP_Context_Activate_V2(RequestCtrl_p, AccessPointName_p, HeaderCompression, \
                                           DataCompression, QOS_Type, MinAcceptableQOS_p, \
                                           RequestedQOS_p, PDP_Type, PDP_Address_p, ProtocolOptions_p, \
                                           ProtocolOptionsLength, PS_Priority, UserPlaneSettings_p, NSAPI_p, Result_p) \
        Request_PS_PDP_Context_Activate_V3(RequestCtrl_p, AccessPointName_p, HeaderCompression, \
                                           DataCompression, QOS_Type, FALSE, MinAcceptableQOS_p, \
                                           RequestedQOS_p, PDP_Type, PDP_Address_p, ProtocolOptions_p, \
                                           ProtocolOptionsLength, PS_Priority, UserPlaneSettings_p, NSAPI_p, Result_p)
#define Request_PS_PDP_Context_Activate(RequestCtrl_p, AccessPointName_p, HeaderCompression, \
                                        DataCompression, QOS_Type, MinAcceptableQOS_p, \
                                        RequestedQOS_p, PDP_Type, PDP_Address_p, ProtocolOptions_p, \
                                        ProtocolOptionsLength, PS_Priority, NSAPI_p, Result_p) \
        Request_PS_PDP_Context_Activate_V2(RequestCtrl_p, AccessPointName_p, HeaderCompression, \
                                           DataCompression, QOS_Type, MinAcceptableQOS_p, \
                                           RequestedQOS_p, PDP_Type, PDP_Address_p, ProtocolOptions_p, \
                                           ProtocolOptionsLength, PS_Priority, NULL, NSAPI_p, Result_p)

#define RESPONSE_PS_PDP_CONTEXT_ACTIVATE RESPONSE_PS_PDP_CONTEXT_ACTIVATE_V2

#define Response_PS_PDP_Context_Activate Response_PS_PDP_Context_Activate_V2
#endif //SKIP_IN_REF_MAN


/************************************************************************/
// Request_PS_PDP_SecondaryContext_Activate_V3
/************************************************************************/

/**
 * Request the activation of a Secondary PDP context. This may only be called 
 * if there already exists an activated PDP context for a PDP address. The 
 * caller (i.e. the service user) will have to use the value in the output 
 * parameter NSAPI_p, when addressing this specific PDP context in future 
 * global service requests.
 *
 * When the result code PS_PDP_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS is returned 
 * the context activation procedure has started and the final outcome of the 
 * activation attempt is delivered to the service user in the signal 
 * EVENT_PS_PDP_CONTEXTSTATUS.
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether 
 *                                    the request is called in wait mode or no 
 *                                    wait mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] NSAPI_WithSamePDP_Addr The value of any NSAPI activated with 
 *                                    the same PDP address as the new PDP 
 *                                    context that is requested to be 
 *                                    activated.
 * @param [in] HeaderCompression      Flag indicating whether the service user 
 *                                    wishes header compression to be used or 
 *                                    not.
 * @param [in] DataCompression        Flag indicating whether the service user 
 *                                    wishes data compression to be used or 
 *                                    not.
 * @param [in] QOS_Type               Type of Quality of Service profile 
 *                                    requested.
 * @param [in] QOS_Reverted           Flag indicating whether revert 
 *                                    Quality of Service to old version or not. 
 * @param [in] MinAcceptableQOS_p     Pointer to the minimum acceptable 
 *                                    Quality of Service.
 * @param [in] RequestedQOS_p         Pointer to the Quality of Service 
 *                                    profile requested.
 * @param [in] TFT_OperationCode      The operation code for the Traffic Flow 
 *                                    Template of the PDP context to set-up. 
 *                                    The only possible values of this 
 *                                    parameter in this request function are:   
 *                                    \li \c PS_TFT_OPERATION_CODE_NO_OPERATION 
 *                                    \li \c PS_TFT_OPERATION_CODE_CREATE_NEW.
 * @param [in] PS_Priority            Indicates if the PDP context shall be 
 *                                    prioritized over CS services.
 * @param [in] TFT_PacketFilter_p     The packet filter part of the traffic flow 
 *                                    template to set-up for the activated PDP context. 
 *                                    This parameter is only valid if the previous 
 *                                    parameter holds the value PS_TFT_OPERATION_CODE_CREATE_NEW. 
 *                                    The parameter PacketFilterIdentifier 
 *                                    within each PacketFilter must be set by 
 *                                    the service user to a value between 0-7 
 *                                    and no packet filters may have the same 
 *                                    value within a TFT.
 * @param [in] TFT_ParameterList_p    The parameter list part of the traffic flow 
 *                                    template to set-up for the activated PDP context.
 * @param [in] UserPlaneSettings_p    Contains user plane settings regarding AQM and
 *                                    packet prioritization
 * @param [in] ProtocolOptions_p      Pointer to the protocol configuration 
 *                                    options that the service user wishes to 
 *                                    negotiate. Note that the first two octets, 
 *                                    i.e. the protocol configuration option IEI 
 *                                    and the length field, shall not be included.
 * @param [in] ProtocolOptionsLength_p Pointer to length of the requested protocol 
 *                                     configuration options parameter string.
 * @param [out] NSAPI_p               Pointer to a value identifying the 
 *                                    activated PDP context.
 * @param [out] Result_p              Pointer to a value specifying the 
 *                                    current activation status of the PDP 
 *                                    context activation request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V2
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related EVENT_PS_PDP_CONTEXTSTATUS, EVENT_PS_PDP_CONTEXTDEACTIVATE_V2
 */

RequestStatus_t
Request_PS_PDP_SecondaryContext_Activate_V3(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_NSAPI_t NSAPI_WithSamePDP_Addr,
  const PS_HeaderCompression_t HeaderCompression,
  const PS_DataCompression_t DataCompression,
  const PS_QOS_Type_t QOS_Type,
  const boolean QOS_Reverted,
  const PS_QOS_Profile_t *const MinAcceptableQOS_p,
  const PS_QOS_Profile_t *const RequestedQOS_p,
  const PS_TFT_OperationCode_t TFT_OperationCode,
  const PS_Priority_t PS_Priority,
  const PS_TFT_PacketFilter_t *const TFT_PacketFilter_p,
  const PS_TFT_ParameterList_t *const TFT_ParameterList_p,
  const PS_UserPlaneSettings_t *const   UserPlaneSettings_p,
  const uint8 *const ProtocolOptions_p,
  const uint8 *const ProtocolOptionsLength_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_PDP_ContextActivateResult_t *const Result_p);


/************************************************************************/
// Response_PS_PDP_SecondaryContext_Activate_V2
/************************************************************************/

/**
 * Response function for Request_PS_PDP_SecondaryContext_Activate_V3 
 *
 */

RequestStatus_t
Response_PS_PDP_SecondaryContext_Activate_V2 (
  const void *const SigStruct_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_PDP_ContextActivateResult_t *const Result_p);

#ifndef SKIP_IN_REF_MAN
#define Request_PS_PDP_SecondaryContext_Activate_V2(RequestCtrl_p, NSAPI_WithSamePDP_Addr, \
                                                    HeaderCompression, DataCompression, QOS_Type,\
                                                    MinAcceptableQOS_p, RequestedQOS_p, TFT_OperationCode, \
                                                    PS_Priority, TFT_PacketFilter_p, TFT_ParameterList_p, \
                                                    UserPlaneSettings_p, ProtocolOptions_p, ProtocolOptionsLength_p, \
                                                    NSAPI_p, Result_p) \
        Request_PS_PDP_SecondaryContext_Activate_V3(RequestCtrl_p, NSAPI_WithSamePDP_Addr, \
                                                    HeaderCompression, DataCompression, QOS_Type, FALSE, \
                                                    MinAcceptableQOS_p, RequestedQOS_p, TFT_OperationCode, \
                                                    PS_Priority, TFT_PacketFilter_p, TFT_ParameterList_p, \
                                                    UserPlaneSettings_p, ProtocolOptions_p, ProtocolOptionsLength_p, \
                                                    NSAPI_p, Result_p)
#define Request_PS_PDP_SecondaryContext_Activate(RequestCtrl_p, NSAPI_WithSamePDP_Addr, \
                                                 HeaderCompression, DataCompression, QOS_Type,\
                                                 MinAcceptableQOS_p, RequestedQOS_p, TFT_OperationCode, \
                                                 PS_Priority, TFT_PacketFilter_p, NSAPI_p, Result_p)\
        Request_PS_PDP_SecondaryContext_Activate_V2(RequestCtrl_p, NSAPI_WithSamePDP_Addr, \
                                                    HeaderCompression, DataCompression, QOS_Type,\
                                                    MinAcceptableQOS_p, RequestedQOS_p, TFT_OperationCode, \
                                                    PS_Priority, TFT_PacketFilter_p, NULL, \
                                                    NULL, NULL, NULL, NSAPI_p, Result_p)

#define RESPONSE_PS_PDP_SECONDARYCONTEXT_ACTIVATE RESPONSE_PS_PDP_SECONDARYCONTEXT_ACTIVATE_V2

#define Response_PS_PDP_SecondaryContext_Activate Response_PS_PDP_SecondaryContext_Activate_V2
#endif //SKIP_IN_REF_MAN

/************************************************************************/
// Event_PS_PDP_ContextStatus
/************************************************************************/

/**
 * This event is sent to the service user to report a change in status of a PDP 
 * context.
 *
 * @param [in] SigStruct_p              Event signal to be unpacked.
 * @param [out] PDP_Type_p              Pointer to the type of Packet Data 
 *                                      Protocol (PDP) of the activated PDP 
 *                                      context.
 * @param [out] PDP_Address_p           Pointer to the PDP address of the 
 *                                      activated PDP context.
 * @param [out] ProtocolOptions_pp      A reference to the pointer to the 
 *                                      negotiated protocol configuration 
 *                                      options excluding the IEI octet and the 
 *                                      length octet. The value of this parameter 
 *                                      is not valid if  the ProtocolOptionsLength_p 
 *                                      is pointing to a value of 0.
 * @param [out] ProtocolOptionsLength_p Pointer to the length of the 
 *                                      negotiated protocol configuration 
 *                                      options parameter string.
 * @param [out] NSAPI_p                 Pointer to a value identifying the PDP 
 *                                      context.
 * @param [out] Cause_p                 Pointer to a value describing a cause 
 *                                      code in error situations.
 * @param [out] ContextEvent_p          Pointer to a value describing the 
 *                                      event.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_PDP_CONTEXTSTATUS
 */

EventStatus_t
Event_PS_PDP_ContextStatus(
  const void *const SigStruct_p,
  PS_PDP_Type_t *const PDP_Type_p,
  PS_PDP_Address_t *const PDP_Address_p,
  uint8 **const ProtocolOptions_pp,
  uint8 *const ProtocolOptionsLength_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_CauseCode_t *const Cause_p,
  PS_PDP_ContextEvent_t *const ContextEvent_p);


/************************************************************************/
// Request_PS_PDP_Context_Deactivate_V2
/************************************************************************/

/**
 * Request deactivation of one or all activated PDP context for a PDP address. 
 * In case all PDP contexts are deactivated by this request a separate 
 * EVENT_PS_PDP_CONTEXTDEACTIVATE_V2 will be sent for all contexts.
 *
 * @param [in] RequestCtrl_p     Pointer to struct controlling whether the 
 *                               request is called in wait mode or no wait 
 *                               mode.
 * @param [in] SigStruct_p       Response signal to be unpacked.
 * @param [in] NSAPI             A value identifying the PDP context to be 
 *                               deactivated.
 * @param [in] TearDownIndicator When the TearDownIndicator is set to 
 *                               PS_TEAR_DOWN_INDICATOR_TEARDOWN_REQUESTED the service 
 *                               provider will deactivate all PDP contexts with the same 
 *                               PDP address as the address specified by the 
 *                               parameter NSAPI.
 * @param [in] ProtocolOptions_p      Pointer to the protocol configuration 
 *                                    options that the service user wishes to 
 *                                    negotiate. Note that the first two octets, 
 *                                    i.e. the protocol configuration option IEI 
 *                                    and the length field, shall not be included.
 * @param [in] ProtocolOptionsLength_p Pointer to length of the requested protocol 
 *                                     configuration options parameter string.
 * @param [out] Result_p         Pointer to a value specifying the result of 
 *                               the PDP deactivation request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_PDP_CONTEXT_DEACTIVATE_V2
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related EVENT_PS_PDP_CONTEXTDEACTIVATE_V2
 */

RequestStatus_t
Request_PS_PDP_Context_Deactivate_V2(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_NSAPI_t NSAPI,
  const PS_TearDownIndicator_t TearDownIndicator,
  const uint8 *const ProtocolOptions_p,
  const uint8 *const ProtocolOptionsLength_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_PDP_Context_Deactivate_V2
/************************************************************************/

/**
 * Response function for Request_PS_PDP_Context_Deactivate_V2 
 *
 */

RequestStatus_t
Response_PS_PDP_Context_Deactivate_V2(
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p);

#ifndef SKIP_IN_REF_MAN
#define Request_PS_PDP_Context_Deactivate(RequestCtrl_p, NSAPI, TearDownIndicator, Result_p)\
        Request_PS_PDP_Context_Deactivate_V2(RequestCtrl_p, NSAPI, TearDownIndicator, NULL, NULL, Result_p)

#define RESPONSE_PS_PDP_CONTEXT_DEACTIVATE RESPONSE_PS_PDP_CONTEXT_DEACTIVATE_V2

#define Response_PS_PDP_Context_Deactivate Response_PS_PDP_Context_Deactivate_V2
#endif //SKIP_IN_REF_MAN

/************************************************************************/
// Event_PS_PDP_ContextDeactivate_V2
/************************************************************************/

/**
 * This event is sent following the deactivation of a PDP context or for each 
 * PDP context when the service user has requested tear-down of all PDP contexts 
 * for a PDP address.
 *
 * @param [in] SigStruct_p          Event signal to be unpacked.
 * @param [out] NSAPI_p             Pointer to a value identifying the PDP 
 *                                  context that has been deactivated.
 * @param [out] TimeAndDataVolume_p Pointer to a structure including time and 
 *                                  data volume counters for the deactivated 
 *                                  PDP context.
 * @param [out] ProtocolOptions_pp      A reference to the pointer to the 
 *                                      negotiated protocol configuration 
 *                                      options excluding the IEI octet and the 
 *                                      length octet. The value of this parameter 
 *                                      is not valid unless if the value that
 *                                      ProtocolOptionsLength_p points at is 0.
 * @param [out] ProtocolOptionsLength_p Pointer to the length of the 
 *                                      negotiated protocol configuration 
 *                                      options parameter string.
 * @param [out] Cause_p             Pointer to a value specifying the reason 
 *                                  for deactivation.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_PDP_CONTEXTDEACTIVATE_V2
 */

EventStatus_t
Event_PS_PDP_ContextDeactivate_V2(
  const void *const SigStruct_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_TimeAndDataVolume_t *const TimeAndDataVolume_p,
  uint8 **const ProtocolOptions_pp,
  uint8 *const ProtocolOptionsLength_p,
  PS_CauseCode_t *const Cause_p);

#ifndef SKIP_IN_REF_MAN
#define Event_PS_PDP_ContextDeactivate(SigStruct_p, NSAPI_p, TimeAndDataVolume_p, Cause_p) \
        Event_PS_PDP_ContextDeactivate_V2(SigStruct_p, NSAPI_p, TimeAndDataVolume_p, NULL, \
                                          NULL, Cause_p)

#define EVENT_PS_PDP_CONTEXTDEACTIVATE EVENT_PS_PDP_CONTEXTDEACTIVATE_V2
#endif //SKIP_IN_REF_MAN

/************************************************************************/
// Request_PS_PDP_Context_Modify_V3
/************************************************************************/

/**
 * This request function is used to either re-negotiate the Quality of Service 
 * profile or change the Traffic Flow Template of an activated PDP context.
 *
 * Note 1: It is the responsibility of the service user to allocate (and free) 
 * memory for any PCO to be sent to the network. If service user don't want 
 * to negotiate a PCO the ProtocolOptions_p shall be set to NULL and 
 * *ProtocolOptionsLength_p shall be set to zero.
 *
 * Note 2: if PCO is provided from network, then heap memory will be allocated by
 * the response function. It is the responsibility of the client to free this 
 * memory - if present - this can be determined via *ProtocolOptionsLength_p and 
 * *ProtocolOptions_pp. (Use HEAP_UNSAFE_FREE(&p) to free PCO from network)
 *
 * @param [in] RequestCtrl_p         Pointer to struct controlling whether the 
 *                                   request is called in wait mode or no wait 
 *                                   mode.
 * @param [in] SigStruct_p           Response signal to be unpacked.
 * @param [in] NSAPI                 A value identifying the PDP context.
 * @param [in] QOS_Type              Type of Quality of Service profile 
 *                                   requested. If the value is set to 
 *                                   PS_QOS_TYPE_UNSPECIFIED no modification of 
 *                                   the QoS will be performed by the request 
 *                                   function and the service provider will 
 *                                   ignore the two parameters below.
 * @param [in] MinAcceptableQOS_p    Pointer to the new minimum acceptable 
 *                                   Quality of Service.
 * @param [in] NewQOS_p              Pointer to the new Quality of Service 
 *                                   profile requested.
 * @param [in] TFT_OperationCode     The operation code for the Traffic Flow 
 *                                   Template of the PDP context that shall be 
 *                                   modified. All values are possible. The 
 *                                   validity of the next two parameters 
 *                                   depends on this value. If the value is set 
 *                                   to PS_TFT_OPERATION_CODE_NO_OPERATION no 
 *                                   modification of the TFT will be performed 
 *                                   by the request function and the service 
 *                                   provider will ignore the two parameters 
 *                                   below.
 * @param [in] PacketFiltersToDelete This parameter is only valid if 
 *                                   TFT_OperationCode= 
 *                                   PS_TFT_OPERATION_CODE_DELETE_FROM_EXISTING. The 
 *                                   parameter shall be seen as a bitmap where 
 *                                   a "1" in bit position x indicates that 
 *                                   packet filter x shall be removed. The LSB 
 *                                   in the byte shall be treated as bit 
 *                                   position 0. For example if packet filters 
 *                                   0, 1, 3 and 4 shall be deleted the value 
 *                                   of this parameter would be 0x1b.
 * @param [in] TFT_PacketFilter_p    This parameter is only valid if 
 *                                   TFT_OperationCode= 
 *                                   \li \c PS_TFT_OPERATION_CODE_CREATE_NEW  
 *                                   \li \c PS_TFT_OPERATION_CODE_ADD_TO_EXISTING 
 *                                   \li \c PS_TFT_OPERATION_CODE_REPLACE_IN_EXISTING 
 *                                   If the value is set to
 *                                   PS_TFT_OPERATION_CODE_CREATE_NEW 
 *                                   the parameter PacketFilterIdentifier 
 *                                   within each PacketFilter must be set by 
 *                                   the service user to a value between 0-7 
 *                                   and no packet filters may have the same 
 *                                   value within a TFT.
 *                                   If the value is set to 
 *                                   PS_TFT_OPERATION_CODE_ADD_TO_EXISTING or 
 *                                   PS_TFT_OPERATION_CODE_REPLACE_IN_EXISTING the 
 *                                   parameter PacketFilterIdentifier within 
 *                                   each PacketFilter must be set correctly by 
 *                                   the service user.
 * @param [in] TFT_ParameterList_p   The parameter list part of the traffic flow 
 *                                   template to set-up for the activated PDP context.
 * @param [in] ProtocolOptions_p      Pointer to the protocol configuration 
 *                                    options (PCO) that the service user wishes to 
 *                                    negotiate. Note that the first two octets, 
 *                                    i.e. the protocol configuration option IEI 
 *                                    and the length field, shall not be included. 
 *                                    If user has no PCO to negotiate this parameter
 *                                    shall be set to NULL.
 * @param [in,out] ProtocolOptionsLength_p Pointer to length of the requested/negotiated
 *                                         protocol options parameter string. The IEI 
 *                                         octet and the length octet are excluded.
 *                                         If user has no PCO to negotiate *ProtocolOptionsLength_p 
 *                                         shall be set to zero (i.e. this should be supplied
 *                                         as the address to a variable). 
 *                                         Note: this can be valid both when REQUEST_OK and 
 *                                         when REQUEST_FAILED_APPLICATION is returned (e.g.
 *                                         when MODIFY_REJECT message is received from network or
 *                                         if MODFIY_ACCEPT message with QOS not-acceptable to the UE).
 * @param [out] ProtocolOptions_pp   A reference to the pointer to the 
 *                                   negotiated protocol configuration 
 *                                   options excluding the IEI octet and the 
 *                                   length octet. *ProtocolOptions_pp shall be set to NULL by service user
 *                                   Note: this can be valid both when REQUEST_OK and 
 *                                   when REQUEST_FAILED_APPLICATION is returned (e.g.
 *                                   when MODIFY_REJECT message is received from network or
 *                                   if MODFIY_ACCEPT message with QOS not-acceptable to the UE)
 *                                   The value of this parameter is not valid if the value that 
 *                                   ProtocolOptionsLength points at is equal to 0.
 * @param [out] Cause_p              Pointer to a value describing a cause 
 *                                   code when the 
 *                                   *Result_p=PS_GENERAL_RESULT_REJECTED.
 * @param [out] Result_p             Pointer to a value specifying the result 
 *                                   of the modification request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_PDP_CONTEXT_MODIFY_V3
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_PDP_Context_Modify_V3(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_NSAPI_t NSAPI,
  const PS_QOS_Type_t QOS_Type,
  const PS_QOS_Profile_t *const MinAcceptableQOS_p,
  const PS_QOS_Profile_t *const NewQOS_p,
  const PS_TFT_OperationCode_t TFT_OperationCode,
  const uint8 PacketFiltersToDelete,
  const PS_TFT_PacketFilter_t *const TFT_PacketFilter_p,
  const PS_TFT_ParameterList_t *const TFT_ParameterList_p,
  const uint8 *const ProtocolOptions_p,
  uint8 *const ProtocolOptionsLength_p,
  uint8 **const ProtocolOptions_pp,
  PS_CauseCode_t *const Cause_p,
  PS_GeneralResult_t *const Result_p);

/************************************************************************/
// Response_PS_PDP_Context_Modify_V3
/************************************************************************/

/**
 * Response function for Request_PS_PDP_Context_Modify_V3 
 *
 */

RequestStatus_t
Response_PS_PDP_Context_Modify_V3(
  const void *const SigStruct_p,
  uint8 *const ProtocolOptionsLength_p,
  uint8 **const ProtocolOptions_pp,
  PS_CauseCode_t *const Cause_p,
  PS_GeneralResult_t *const Result_p);

#ifndef SKIP_IN_REF_MAN
#define Request_PS_PDP_Context_Modify(RequestCtrl_p, NSAPI, QOS_Type, MinAcceptableQOS_p,\
                                      NewQOS_p, TFT_OperationCode, PacketFiltersToDelete, \
                                      TFT_PacketFilter_p, Cause_p, Result_p) \
        Request_PS_PDP_Context_Modify_V3(RequestCtrl_p, NSAPI, QOS_Type, MinAcceptableQOS_p,\
                                         NewQOS_p, TFT_OperationCode, PacketFiltersToDelete, \
                                         TFT_PacketFilter_p, NULL, NULL, NULL, NULL, Cause_p, \
                                         Result_p)

#define RESPONSE_PS_PDP_CONTEXT_MODIFY RESPONSE_PS_PDP_CONTEXT_MODIFY_V3

#define Response_PS_PDP_Context_Modify(SigStruct_p, Cause_p, Result_p) \
        Response_PS_PDP_Context_Modify_V3(SigStruct_p, NULL, NULL, Cause_p, Result_p)


#define Request_PS_PDP_Context_Modify_V2(RequestCtrl_p, NSAPI, QOS_Type, MinAcceptableQOS_p,\
                                         NewQOS_p, TFT_OperationCode, PacketFiltersToDelete, \
                                         TFT_PacketFilter_p, TFT_ParameterList_p, ProtocolOptions_p, \
                                         ProtocolOptionsLength_p, Cause_p, Result_p) \
        Request_PS_PDP_Context_Modify_V3(RequestCtrl_p, NSAPI, QOS_Type, MinAcceptableQOS_p,\
                                         NewQOS_p, TFT_OperationCode, PacketFiltersToDelete, \
                                         TFT_PacketFilter_p, TFT_ParameterList_p, ProtocolOptions_p, \
                                         ProtocolOptionsLength_p, NULL, Cause_p, Result_p)

#define RESPONSE_PS_PDP_CONTEXT_MODIFY_V2 RESPONSE_PS_PDP_CONTEXT_MODIFY_V3

#define Response_PS_PDP_Context_Modify_V2(SigStruct_p, Cause_p, Result_p) \
        Response_PS_PDP_Context_Modify_V3(SigStruct_p, NULL, NULL, Cause_p, Result_p)
#endif //SKIP_IN_REF_MAN

/************************************************************************/
// Request_PS_QOS_Profile_Read
/************************************************************************/

/**
 * Request the Quality of Service profile of an activated PDP context.
 *
 * @param [in] RequestCtrl_p  Pointer to struct controlling whether the 
 *                            request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p    Response signal to be unpacked.
 * @param [in] NSAPI          A value identifying the PDP context to be read.
 * @param [out] QOS_Profile_p Pointer to the negotiated Quality of Service 
 *                            profile of the activated PDP context.
 * @param [out] Result_p      Pointer to a value specifying the result of the 
 *                            read request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_QOS_PROFILE_READ
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_QOS_Profile_Read(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_NSAPI_t NSAPI,
  PS_QOS_Profile_t *const QOS_Profile_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_QOS_Profile_Read
/************************************************************************/

/**
 * Response function for Request_PS_QOS_Profile_Read 
 *
 */

RequestStatus_t
Response_PS_QOS_Profile_Read(
  const void *const SigStruct_p,
  PS_QOS_Profile_t *const QOS_Profile_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_TimeAndDataVolume_Read
/************************************************************************/
/**
 * Requests the time and data volume counter for an activated PDP context. This 
 * request may be done at any instance in time when a PDP context is active.
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the 
 *                                  request is called in wait mode or no wait 
 *                                  mode.
 * @param [in] SigStruct_p          Response signal to be unpacked.
 * @param [in] NSAPI                A value identifying from what PDP context 
 *                                  time and data volume counter is requested.
 * @param [out] TimeAndDataVolume_p Pointer to a structure including time and 
 *                                  data volume counters for the PDP context 
 *                                  with the identifier NSAPI.
 * @param [out] Result_p            Pointer to a value specifying the result 
 *                                  of the request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_TIMEANDDATAVOLUME_READ
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_TimeAndDataVolume_Read(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_NSAPI_t NSAPI,
  PS_TimeAndDataVolume_t *const TimeAndDataVolume_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_TimeAndDataVolume_Read
/************************************************************************/

/**
 * Response function for Request_PS_TimeAndDataVolume_Read 
 *
 */

RequestStatus_t
Response_PS_TimeAndDataVolume_Read (
  const void *const SigStruct_p,
  PS_TimeAndDataVolume_t *const TimeAndDataVolume_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_PeriodicReport_Enable
/************************************************************************/

/**
 * Requests that the periodic reporting of time and data volume counters for 
 * all PDP contexts shall be enabled. Many service users are able to 
 * enable/disable periodic reporting and as long as at least one client has 
 * enabled the reporting the periodic reports are sent through a dispatch. 
 * Thus, in addition to calling this request function the service user 
 * must subscribe to the event channel to get the reports. This request may be 
 * done at any instance in time.
 *
 * @param [in] RequestCtrl_p Pointer to struct controlling whether the request 
 *                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p   Response signal to be unpacked.
 * @param [out] Result_p     Pointer to a value specifying the result of the 
 *                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_PERIODICREPORT_ENABLE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_PeriodicReport_Enable (
  const RequestControlBlock_t *const RequestCtrl_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_PeriodicReport_Enable 
/************************************************************************/

/**
 * Response function for Request_PS_PeriodicReport_Enable 
 *
 */

RequestStatus_t
Response_PS_PeriodicReport_Enable (
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_PeriodicReport_Disable
/************************************************************************/

/**
 * Requests that the periodic report of time and data volume counters for all 
 * PDP contexts shall be disabled. Since many service users are able to enable 
 * periodic reporting this request does not turn off the periodic reports if 
 * another service also has enabled it. Thus, in addition to calling this 
 * request function the service user must un-subscribe to the event channel to 
 * be sure to not receive more reports. This request may be done at any 
 * instance in time.
 *
 * @param [in] RequestCtrl_p Pointer to struct controlling whether the request 
 *                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p   Response signal to be unpacked.
 * @param [out] Result_p     Pointer to a value specifying the result of the 
 *                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_PERIODICREPORT_DISABLE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_PeriodicReport_Disable (
  const RequestControlBlock_t *const RequestCtrl_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_PeriodicReport_Disable 
/************************************************************************/

/**
 * Response function for Request_PS_PeriodicReport_Disable 
 *
 */

RequestStatus_t
Response_PS_PeriodicReport_Disable (
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p);

/************************************************************************/
// Request_PS_UserPlaneSettings_Modify
/************************************************************************/
/**
 * Request used to modify user plane settings regarding the AQM and SDU 
 * prioritization values that were set during PDP context activation
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the 
 *                                    request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] UserPlaneSettings_p    Contains user plane settings regarding AQM and
 *                                    SDU prioritization
 * @param [in] NSAPI                  Identifier for the affected PDP context.
 * @param [out] Result_p              Pointer to a value specifying the result of 
 *                                    the request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_USERPLANESETTINGS_MODIFY
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */
RequestStatus_t Request_PS_UserPlaneSettings_Modify(
  const RequestControlBlock_t *const    RequestCtrl_p,
  const PS_NSAPI_t                      NSAPI,
  const PS_UserPlaneSettings_t *const   UserPlaneSettings_p,
  PS_GeneralResult_t *const             Result_p);

/**
 * Response function for Response_PS_UserPlaneSettings_Modify 
 *
 */
RequestStatus_t Response_PS_UserPlaneSettings_Modify(
  const void *const          SigStruct_p,
  PS_GeneralResult_t *const  Result_p);

/************************************************************************/
// Request_PS_UserPlaneStatus_Read
/************************************************************************/

/**
 * Requests the current status/availability, "suspended" or "not suspended",
 * of the PS user plane.
 *
 * This request may be done at any instance in time when Network Signalling is
 * activated.
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the 
 *                                    request is called in wait mode or no wait 
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [out] UserPlaneSuspended_p  Pointer to a boolean indicating whether
 *                                    the PS user plane is suspended or not.
 * @param [out] SuspendCause_p        Pointer to a value indicating the reason
 *                                    for why the PS user plane is suspended.
 *                                    This value is valid only if param
 *                                    UserPlaneSuspended_p
 *                                    equals TRUE.
 * @param [out] Result_p              Pointer to a value specifying the result 
 *                                    of the request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input
 *                                    parameter. No request has been sent to
 *                                    the service provider. The output
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service
 *                                    provider, but the request of the Global
 *                                    Service was not satisfied. The result
 *                                    parameter indicates the error cause.
 *                                    Other output parameters do not contain
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_USERPLANESTATUS_READ
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_UserPlaneStatus_Read(
  const RequestControlBlock_t *const RequestCtrl_p,
  boolean *const UserPlaneSuspended_p,
  PS_UserPlaneSuspendCause_t *const SuspendCause_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_UserPlaneStatus_Read
/************************************************************************/

/**
 * Response function for Request_PS_UserPlaneStatus_Read
 *
 */

RequestStatus_t
Response_PS_UserPlaneStatus_Read(
  const void *const SigStruct_p,
  boolean *const UserPlaneSuspended_p,
  PS_UserPlaneSuspendCause_t *const SuspendCause_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_UserPlaneStatusReport_Enable
/************************************************************************/

/**
 * Requests that the PS user plane suspend/resume notification shall be enabled.
 *
 * Many service users are able to enable/disable the suspend/resume notification
 * and as long as at least one client has enabled suspend/resume notification
 * the notifications are sent through two dispatched events,
 * EVENT_PS_USERPLANESUSPENDED and EVENT_PS_USERPLANERESUMED.
 * Thus, in addition to calling this request function the service user must
 * subscribe to the event channels to get the events.
 *
 * This request may be done at any instance in time.
 *
 * @param [in] RequestCtrl_p Pointer to struct controlling whether the request
 *                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p   Response signal to be unpacked.
 * @param [out] Result_p     Pointer to a value specifying the result of the 
 *                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was
 *                                    successfully satisfied. The output
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service
 *                                    provider. The output parameters do not
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input
 *                                    parameter. No request has been sent to
 *                                    the service provider. The output
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service
 *                                    provider, but the request of the Global
 *                                    Service was not satisfied. The result
 *                                    parameter indicates the error cause.
 *                                    Other output parameters do not contain
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be
 *                                    unpacked. The output parameters do not
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_USERPLANESTATUSREPORT_ENABLE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_UserPlaneStatusReport_Enable(
  const RequestControlBlock_t *const RequestCtrl_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_UserPlaneStatusReport_Enable
/************************************************************************/

/**
 * Response function for Request_PS_UserPlaneStatusReport_Enable
 *
 */

RequestStatus_t
Response_PS_UserPlaneStatusReport_Enable(
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_UserPlaneStatusReport_Disable
/************************************************************************/

/**
 * Requests that the PS user plane suspend/resume notification shall be disabled.
 *
 * Since many service users are able to enable the suspend/resume notification
 * this request does not turn off suspend/resume notification if another client
 * also has enabled it. Thus, in addition to calling this request function the
 * service user must un-subscribe to the event channel to be sure to not receive
 * more events.
 *
 * This request may be done at any instance in time.
 *
 * @param [in] RequestCtrl_p Pointer to struct controlling whether the request
 *                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p   Response signal to be unpacked.
 * @param [out] Result_p     Pointer to a value specifying the result of the
 *                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was
 *                                    successfully satisfied. The output
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service
 *                                    provider. The output parameters do not
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input
 *                                    parameter. No request has been sent to
 *                                    the service provider. The output
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service
 *                                    provider, but the request of the Global
 *                                    Service was not satisfied. The result
 *                                    parameter indicates the error cause.
 *                                    Other output parameters do not contain
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be
 *                                    unpacked. The output parameters do not
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_USERPLANESTATUSREPORT_DISABLE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_UserPlaneStatusReport_Disable(
  const RequestControlBlock_t *const RequestCtrl_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_UserPlaneStatusReport_Disable
/************************************************************************/

/**
 * Response function for Request_PS_UserPlaneStatusReport_Disable
 *
 */

RequestStatus_t
Response_PS_UserPlaneStatusReport_Disable(
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_UsageStatus_Read
/************************************************************************/

/**
 * Reads the current usage status of the PS user plane.
 * 
 * The values returned here are from the last dispatch of
 *
 * \li \c EVENT_PS_USAGESTATUS_CHANGED
 *
 * @param [in] RequestCtrl_p    Pointer to struct controlling whether the request
 *                              is called in wait mode or no wait mode.
 * @param [in] SigStruct_p      Response signal to be unpacked.
 * @param [out] UserPlaneType_p Pointer to a value indicating the current
 *                              usage status of the PS user plane.
 * @param [out] Result_p        Pointer to a value specifying the result of the
 *                              request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was
 *                                    successfully satisfied. The output
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service
 *                                    provider. The output parameters do not
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input
 *                                    parameter. No request has been sent to
 *                                    the service provider. The output
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service
 *                                    provider, but the request of the Global
 *                                    Service was not satisfied. The result
 *                                    parameter indicates the error cause.
 *                                    Other output parameters do not contain
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be
 *                                    unpacked. The output parameters do not
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_USAGESTATUS_READ
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t 
Request_PS_UsageStatus_Read(
  const RequestControlBlock_t *const RequestCtrl_p,
  PS_UserPlaneType_t          *const UserPlaneType_p,
  PS_GeneralResult_t          *const Result_p);

/**
 * Response function for Request_PS_UsageStatus_Read
 *
 */

RequestStatus_t 
Response_PS_UsageStatus_Read(
  const void          *const SigStruct_p,
  PS_UserPlaneType_t  *const UserPlaneType_p,
  PS_GeneralResult_t  *const Result_p);


/************************************************************************/
//Do_PS_UplinkSDU_Send_V2
/************************************************************************/

/**
 * The service user shall call this do function when an uplink SDU is ready for 
 * transmission on a PS bearer service (i.e. a PDP context).  
 *
 * @param [in] DownlinkCredits  Piggybacked DownlinkCredits for the activated
 *                              PDP context. 
 * @param [in] NSAPI       Identity of the activated PDP context that shall 
 *                         handle the uplink SDU.
 * @param [in] PS_DataPath The identity of the process that shall handle this 
 *                         request. In order to get this information, the 
 *                         service user will have to subscribe to the event 
 *                         EVENT_ID_PS_DATAPATHCONNECT_SENT.
 * @param [out] SDU_pp     A reference to the pointer to the uplink SDU 
 *                         buffer. When the function returns, the uplink SDU is 
 *                         forwarded to the SwBP service provider. Depending on 
 *                         the implementation, the SDU buffer pointer might be 
 *                         handed over to the service provider or the SDU 
 *                         buffer copied inside this do function. If the 
 *                         pointer is handed over, PS_SDU_NULL is returned. If 
 *                         a copying has taken place, the buffer pointer is not 
 *                         changed and the service user is as usual responsible 
 *                         for the future of the SDU buffer. Note that the SDU 
 *                         buffer must be allocated by using the function 
 *                         Do_PS_SDU_Alloc.
 *                         The service user must make sure that the following 
 *                         is fulfilled:
 *                         SDU_Size <= PS_MAX_SDU_SIZE
 *                         SDU_Offset >= PS_SDU_OFFSET_FOR_UPLINK_HEADERS 
 *                         otherwise the function will return with 
 *                         PS_GENERAL_RESULT_FAILED_PARAMETER and the (simple) 
 *                         service will not be performed.
 *                         The service user should if possible align the SDU 
 *                         to an even 32-bit address.
 *
 * @return PS_GeneralResult_t The result of the simple service. If the
 *                            Do-function returns anything else than
 *                            PS_GENERAL_RESULT_OK there will not be any
 *                            DO-signal sent to the service provider.
 *
 * @sigbased Yes.
 * @related None.
 */

PS_GeneralResult_t
Do_PS_UplinkSDU_Send_V2(
  const uint8 DownlinkCredits,
  const PS_NSAPI_t NSAPI,
  const PROCESS PS_DataPath,
  PS_SDU_t **const SDU_pp);

#ifndef SKIP_IN_REF_MAN
#define Do_PS_UplinkSDU_Send(NSAPI, PS_DataPath, SDU_pp)  Do_PS_UplinkSDU_Send_V2(MAX_UINT8, NSAPI, PS_DataPath, SDU_pp)
#endif //SKIP_IN_REF_MAN
/************************************************************************/
// Done_PS_UplinkSDU_Send
/************************************************************************/

/**
 * Done function for Do_PS_UplinkSDU_Send
 *
 */
#ifndef SKIP_IN_REF_MAN
PS_GeneralResult_t
Done_PS_UplinkSDU_Send(
  const void *const SigStruct_p,
  PS_NSAPI_t *const NSAPI_p);
#endif //SKIP_IN_REF_MAN

/************************************************************************/
// Event_PS_DataPathConnect_Sent
/************************************************************************/

/**
 * This event is sent to the service user to report the current user plane 
 * endpoint of the service provider for a specific PDP context / MBMS bearer
 * service identified by NSAPI.
 * 
 * Client uses NSAPI together with bitmasks to determine if the event is
 * connected to a PDP context or to a MBMS bearer service.
 *
 * @param [in] SigStruct_p               Event signal to be unpacked.
 * @param [out] Extended_NSAPI_p         Pointer to the NSAPI value of the PDP context
 *                                       / MBMS bearer service that is affected by the event.
 * @param [out] PS_DataPath_p            Pointer to the service provider user plane
 *                                       endpoint that the service user must use for the
 *                                       user plane data communication.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_DATAPATHCONNECT_SENT
 */

EventStatus_t Event_PS_DataPathConnect_Sent(const void *const SigStruct_p,
                                            PS_Extended_NSAPI_t *const Extended_NSAPI_p,
                                            PROCESS *const PS_DataPath_p);

#ifndef SKIP_IN_REF_MAN
EventStatus_t Event_PS_DataPathConnect(const void *const SigStruct_p,
                                       PS_NSAPI_t *const NSAPI_p,
                                       PROCESS *const PS_DataPath_p);

#define EVENT_PS_DATAPATHCONNECT EVENT_PS_DATAPATHCONNECT_SENT
#define EVENT_ID_PS_DATAPATHCONNECT EVENT_ID_PS_DATAPATHCONNECT_SENT
#endif //SKIP_IN_REF_MAN


/************************************************************************/
// Event_PS_PeriodicReport
/************************************************************************/

/**
 * This event is sent for a PDP context if a service user has requested to
 * enable the periodic reporting. 
 *
 * A service user that handles more than one PDP context (NSAPI) must look
 * at the NSAPI_p in order to separate between the different PDP contexts.
 *
 * @param [in] SigStruct_p          Event signal to be unpacked.
 * @param [out] NSAPI_p             Pointer to a value identifying the PDP 
 *                                  context that the report originated from. 
 * @param [out] TimeAndDataVolume_p Pointer to a structure including time
 *                                  and data volume counters for the PDP
 *                                  context.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 * @sigbased No.
 * @related None.
 * @signalid EVENT_PS_PERIODICREPORT
 */

EventStatus_t
Event_PS_PeriodicReport (
  const void *const SigStruct_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_TimeAndDataVolume_t *const TimeAndDataVolume_p);


/************************************************************************/
// Event_PS_UserPlaneSuspended
/************************************************************************/

/**
 * This event is sent if a service user has requested to enable the PS user
 * plane suspend/resume notification.
 *
 * @param [in] SigStruct_p             Event signal to be unpacked.
 * @param [out] SuspendCause_p         Pointer to a value indicating the reason
 *                                     for why the PS user plane is suspended.
 * @param [out] EstimatedSuspendTime_p Pointer to a value indicating the
 *                                     estimated suspend time. In this release
 *                                     the estimated suspend time is always set
 *                                     to PS_USER_PLANE_SUSPEND_TIME_UNKNOWN.
 * 
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 * @sigbased No.
 * @related None.
 * @signalid EVENT_PS_USERPLANESUSPENDED
 */

EventStatus_t
Event_PS_UserPlaneSuspended(
  const void *const SigStruct_p,
  PS_UserPlaneSuspendCause_t *const SuspendCause_p,
  uint32 *const EstimatedSuspendTime_p);

/************************************************************************/
// Event_PS_UserPlaneResumed
/************************************************************************/

// No event function is needed since no parameters are sent.


/************************************************************************/
// Event_PS_UserPlaneEstablishmentTemporaryFailed
/************************************************************************/

/**
 * This event indicates that the user plane could not be established due to temporary 
 * network failure, e.g. when SERVICE REQUEST has been rejected with cause 17, 
 * when the Service request timer T3317 has expired or when the access class 
 * is barred in the current cell. The event is also sent if the RRC connection
 * is released after SERVICE ACCEPT. 
 * The client should stop transfer of UL user data when receiving this event.
 *
 * @param [in] SigStruct_p          Event signal to be unpacked.
 * 
 * @retval GS_EVENT_OK               
 * @retval GS_EVENT_FAILED_UNPACKING  Wrong event signal
 *
 * @signalid EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED
 */

EventStatus_t Event_PS_UserPlaneEstablishmentTemporaryFailed (
                const void *const SigStruct_p);

/************************************************************************/
// Event_PS_UserPlaneEstablishmentTemporaryFailed_v2
/************************************************************************/

/**
 * This event indicates that the user plane could not be established due to temporary 
 * network failure, e.g. when SERVICE REQUEST has been rejected with cause 17, 
 * when the Service request timer T3317 has expired or when the access class 
 * is barred in the current cell. The event is also sent if the RRC connection
 * is released after SERVICE ACCEPT. 
 *
 * On receiving  EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
 * with piggybacked credit value larger than 0, the client can resume transfer of UL user 
 * data corresponding to the indicated NSAPI, if the transfer was previously stopped 
 * due to no credits.
 *
 * @param [in] SigStruct_p          Event signal to be unpacked.
 * @param [out] NSAPI_p             Pointer to a value indicating the Corresponding NSAPI.
 * @param [out] UplinkCredits_p     Pointer to a value indicating the Uplink Credits.
 * 
 * @retval GS_EVENT_OK               
 * @retval GS_EVENT_FAILED_UNPACKING  Wrong event signal
 *
 * @signalid EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
 */
				
EventStatus_t Event_PS_UserPlaneEstablishmentTemporaryFailed_V2 (
                const void *const SigStruct_p,
                PS_NSAPI_t *const NSAPI_p,
                uint8 *const UplinkCredits_p);

/************************************************************************/
// Do_PS_SDU_Alloc_V2
/************************************************************************/

/**
 * Both the service user and the service provider shall use this function to 
 * allocate memory for an SDU buffer.
 *
 * @param [in] Size   The size needed for the SDU that shall be stored in the 
 *                    allocated SDU buffer. Note that this parameter shall not 
 *                    exceed PS_MAX_SDU_SIZE.
 * @param [in] Offset The initial value of the SDU_Offset parameter for the 
 *                    requested SDU buffer.  Note, that the following formula 
 *                    applies to the input parameters:  SDU_Size + SDU_Offset ( 
 *                    PS_MAX_DATA_SIZE The service provider will not allocate 
 *                    an SDU buffer if the criterion is not met.
 * @param [in] IgnorePoolCounterLimitation If the parameter is set to true, then the function
 *                    will always increase the SigToken counter regardless of the counter 
 *                    limitation and return a pointer to a SDU buffer. If set to false, then 
 *                    the SigToken counters will limit the amount of memory that
 *                    can be allocated and the function will only return a SDU buffer
 *                    as long as the SigToken counters have not reached their upper limits.
 *
 * @return The function returns a pointer to the allocated SDU buffer. This may
 *         be set to PS_SDU_NULL if unsuccessful and must be checked by the
 *         caller. If successful the parameters of the SDU buffer (PS_SDU_t)
 *         will be set to the following values: 
 *         \li \c SDU_Offset = Offset
 *         \li \c SDU_Size = 0
 *         \li \c MaxDataSize = Size+Offset
 *
 * @waitmode N/A
 * @sigbased No.
 * @related None.
 */

PS_SDU_t *Do_PS_SDU_Alloc_V2(const uint16 Size, 
                             const uint16 Offset,
                             const boolean IgnorePoolCounterLimitation);

// Do_PS_SDU_Alloc is a macros which is replaced by Do_PS_SDU_Alloc. So the same description as for Do_PS_SDU_Alloc_V2
#define Do_PS_SDU_Alloc(Size, Offset) \
        Do_PS_SDU_Alloc_V2((Size), (Offset), FALSE)

/************************************************************************/
// Do_PS_SDU_Free
/************************************************************************/

/**
 * This function shall be used by both the service user and the service 
 * provider when de-allocating memory for an SDU buffer.
 *
 * @param [in] SDU_pp A reference to the pointer to the SDU buffer. This is 
 *                    set to PS_SDU_NULL if successful.
 *
 * @waitmode N/A
 * @sigbased No.
 * @related None.
 */

void Do_PS_SDU_Free(const PS_SDU_t ** const SDU_pp);


/****************************** MBMS START ******************************/

/************************************************************************/
// Request_PS_MBMS_NW_FeatureSupport_Get
/************************************************************************/

#ifndef SKIP_IN_REF_MAN
/**
 * This request returns the latest dispatched value of whether or not the current
 * serving NW (SGSN) supports MBMS.
 * Note: If MBMS support is not indicated by NW then any incoming request to activate
 * an MBMS context (multicast only) will be rejected.
 *
 * @param [in] RequestCtrl_p                 Pointer to struct controlling whether the request 
 *                                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                   Response signal to be unpacked.
 * @param [out] MBMS_NW_FeatureSupport_p     Pointer to a value specifying if MBMS is supported by 
 *                                           current serving NW (SGSN) or not.
 * @param [out] Result_p                     Pointer to a value specifying the result of the 
 *                                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_MBMS_NW_FEATURESUPPORT_GET
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */
#else //SKIP_IN_REF_MAN
/**
 * This request returns the latest dispatched value of whether or not the current
 * serving NW (SGSN) supports MBMS.
 *
 * @param [in] RequestCtrl_p                 Pointer to struct controlling whether the request 
 *                                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                   Response signal to be unpacked.
 * @param [out] MBMS_NW_FeatureSupport_p     Pointer to a value specifying if MBMS is supported by 
 *                                           current serving NW (SGSN) or not.
 * @param [out] Result_p                     Pointer to a value specifying the result of the 
 *                                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_MBMS_NW_FEATURESUPPORT_GET
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */
#endif //SKIP_IN_REF_MAN

RequestStatus_t
Request_PS_MBMS_NW_FeatureSupport_Get (
  const RequestControlBlock_t *const RequestCtrl_p,
  PS_MBMS_NW_FeatureSupport_t *const MBMS_NW_FeatureSupport_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_MBMS_NW_FeatureSupport_Get
/************************************************************************/

/**
 * Response function for Response_PS_MBMS_NW_FeatureSupport_Get
 *
 */

RequestStatus_t
Response_PS_MBMS_NW_FeatureSupport_Get (
  const void *const SigStruct_p,
  PS_MBMS_NW_FeatureSupport_t *const MBMS_NW_FeatureSupport_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_MBMS_Service_Activate
/************************************************************************/

/**
 * This request informs NS that the service user is interested to receive
 * the MBMS data of a particular MBMS bearer service (either multicast or
 * (enhanced) broadcast mode). This will trigger NS to send a notification
 * (including the MBMS session ID and TMGI) when the MBMS data is imminent
 * to be received (MBMS session start). Only when the service user responds
 * to the notification with a confirm to receive the upcoming MBMS session,
 * will the user plane receive the MBMS data associated with this MBMS
 * session ID. Otherwise the upcoming MBMS session is ignored.
 *
 * When activating an (enhanced) MBMS broadcast service the value of the
 * *Extended_NSAPI_p shall be set to PS_MBMS_NSAPI_BROADCAST and then a unique
 * NSAPI value for the MBMS bearer service will be provided via the response
 * function. The supplied value shall be used in all forthcoming requests
 * using the PSS function category.
 * 
 * Note: it is upper layer responsibility to make sure that the same MBMS
 * bearer service is not activated more than once.
 *
 * @param [in] RequestCtrl_p                 Pointer to struct controlling whether the request 
 *                                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                   Response signal to be unpacked.
 * @param [in] TMGI_p                        Pointer to the TMGI of the MBMS bearer service the
 *                                           service user wants to activate.
 * @param [in] MBMS_Mode                     Identifies MBMS mode for the MBMS bearer service to be activated.
 * @param [in, out] Extended_NSAPI_p         NSAPI value of the service to activate. For an MBMS (enhanced)
 *                                           broadcast service this should be set to PS_MBMS_NSAPI_BROADCAST
 *                                           when calling function. A unique value will be provided via
 *                                           response function.
 * @param [out] Result_p                     Pointer to a value specifying the result of the 
 *                                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_MBMS_SERVICE_ACTIVATE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_MBMS_Service_Activate (
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_MBMS_TMGI_t *const TMGI_p,
  const PS_MBMS_Mode_t MBMS_Mode,
  PS_Extended_NSAPI_t *const Extended_NSAPI_p,  
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_MBMS_Service_Activate
/************************************************************************/

/**
 * Response function for Response_PS_MBMS_Service_Activate
 *
 */

RequestStatus_t
Response_PS_MBMS_Service_Activate (
  const void *const SigStruct_p,
  PS_Extended_NSAPI_t *const Extended_NSAPI_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_MBMS_Service_Deactivate
/************************************************************************/

/**
 * This request is used by the service user to deactivate a previously activated
 * MBMS (enhanced) broadcast bearer service.
 *
 * @param [in] RequestCtrl_p                 Pointer to struct controlling whether the request 
 *                                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                   Response signal to be unpacked.
 * @param [in] Extended_NSAPI                NSAPI value of the service to deactivate.
 * @param [out] Result_p                     Pointer to a value specifying the result of the 
 *                                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_MBMS_SERVICE_DEACTIVATE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_MBMS_Service_Deactivate(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_Extended_NSAPI_t Extended_NSAPI,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_MBMS_Service_Deactivate
/************************************************************************/

/**
 * Response function for Response_PS_MBMS_Service_Deactivate
 *
 */

RequestStatus_t
Response_PS_MBMS_Service_Deactivate(
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Request_PS_MBMS_Session_Receive
/************************************************************************/

/**
 * This request is used by the service user to confirm reception of MBMS data
 * that has been announced in a previously dispatched MBMS service state event.
 * The MBMS data to confirm is identified by the TMGI and Session ID.
 * The service user shall confirm MBMS data reception for each MBMS
 * session it wants to receive. The purpose with this global service is to
 * prevent receiving the same session multiple times.
 *
 * The application shall only request reception of sessions that has been
 * notified via Event_PS_MBMS_ServiceState_Changed.
 *
 * If a service user does not want to receive a notified session it simply
 * does not call this request.
 *
 * Note: there are no timing requirements regarding how soon after an MBMS
 * session has been announced an application need to confirm reception.
 *
 * @param [in] RequestCtrl_p                 Pointer to struct controlling whether the request 
 *                                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                   Response signal to be unpacked.
 * @param [in] TMGI_p                        Pointer to the TMGI of the MBMS bearer service the
 *                                           service user wants to receive.
 * @param [in] SessionID                     ID of the session which the application wants to receive.
 * @param [in] ValidParamsMask               Specify if SessionID is present.
 * @param [out] Result_p                     Pointer to a value specifying the result of the 
 *                                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_MBMS_SESSION_RECEIVE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related None.
 */

RequestStatus_t
Request_PS_MBMS_Session_Receive (
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_MBMS_TMGI_t *const TMGI_p,
  const PS_MBMS_SessionID_t SessionID,
  const uint8 ValidParamsMask,
  PS_GeneralResult_t *const Result_p);


/************************************************************************/
// Response_PS_MBMS_Session_Receive
/************************************************************************/

/**
 * Response function for Response_PS_MBMS_Session_Receive
 *
 */

RequestStatus_t
Response_PS_MBMS_Session_Receive(
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p); 


/************************************************************************/
// Event_PS_MBMS_NW_FeatureSupport_Changed
/************************************************************************/

/**
 * This dispatched event is sent after a successful GPRS attach or Routing Area Update (RAU).
 * It contains information about whether or not the current serving NW (SGSN) supports MBMS.
 *
 * This is only sent when the support from the NW changes (not after each attach/RAU).
 * It is sent regardless of if any MBMS bearer service is activated.
 *
 * @param [in] SigStruct_p                  Event signal to be unpacked.
 * @param [out] MBMS_NW_FeatureSupport_p    Pointer indicating new NW feature support status.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_MBMS_NW_FEATURESUPPORT_CHANGED
 */

EventStatus_t Event_PS_MBMS_NW_FeatureSupport_Changed(
  const void *const SigStruct_p,
  PS_MBMS_NW_FeatureSupport_t *const MBMS_NW_FeatureSupport_p);


/************************************************************************/
// Event_PS_MBMS_ServiceState_Changed
/************************************************************************/

/**
 * This dispatched event is sent when a change associated with an activated MBMS
 * bearer services has occurred.
 * Examples:
 * - Notification that the MBMS session data of an activated MBMS bearer service
 *   (identified by the TMGI and Session ID) is imminent to be received
 *   (i.e. MBMS session start).
 * - Notification that the MBMS session is completed (i.e. MBMS session stop).
 * - Notification that the UE has moved out of MBMS coverage.
 *
 * @param [in] SigStruct_p                   Event signal to be unpacked.
 * @param [out] TMGI_p                       Pointer to TMGI of notified MBMS session.
 * @param [out] SessionID_p                  Pointer to session ID of notified MBMS session.
 * @param [out] EventCause_p                 Pointer to value specifying action for the notified MBMS session.
 * @param [out] ValidParamsMask_p            Pointer to value specifying present of TMGI and SessionID.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_MBMS_SERVICESTATE_CHANGED
 */

EventStatus_t Event_PS_MBMS_ServiceState_Changed(
  const void *const SigStruct_p,
  PS_MBMS_TMGI_t *const TMGI_p,
  PS_MBMS_SessionID_t *const SessionID_p,
  PS_MBMS_EventCause_t *const EventCause_p,
  uint8 *const ValidParamsMask_p);


/************************************************************************/
// Do_PS_DownlinkSDU_Enable_V2
/************************************************************************/

/**
 * The service user should call this do function when it is ready to handle
 * downlink SDUs that are received on PDP context(s) or MBMS bearer service(s).
 *
 * The NSAPI value should be set to the value of an activated PDP context or
 * MBMS bearer service.
 *
 * @param [in] Extended_NSAPI Identifer of the activated PDP context or MBMS bearer
 *                            service that the service user requests downlink SDU from.
 * @param [in] PS_DataPath    The identity of the process that shall handle this
 *                            request. In order to get this information, the
 *                            service user will have to subscribe to the event
 *                            EVENT_ID_PS_DATAPATHCONNECT_SENT.
 * @param [in] Credits        The number of downlink SDUs that the service user
 *                            requests to receive.
 *
 * @return PS_GeneralResult_t The result of the simple service. 
 *
 * @sigbased Yes.
 * @related EVENT_PS_DOWNLINKSDU_RECEIVED_V2, EVENT_PS_UPLINKCREDITS_GRANTED
 */

PS_GeneralResult_t
Do_PS_DownlinkSDU_Enable_V2(
  const PS_Extended_NSAPI_t Extended_NSAPI,
  const PROCESS PS_DataPath,
  const uint8 Credits);


/************************************************************************/
// Done_PS_DownlinkSDU_Enable_V2
/************************************************************************/

/**
 * Done function for Do_PS_DownlinkSDU_Enable_V2
 * 
 * @param [in] SigStruct_p  Event signal to be unpacked.
 * 
 */
#ifndef SKIP_IN_REF_MAN
PS_GeneralResult_t
Done_PS_DownlinkSDU_Enable_V2 (
  const void *const SigStruct_p,
  uint8 *const Credits_p,
  PS_Extended_NSAPI_t *const Extended_NSAPI_p);

#define Do_PS_DownlinkSDU_Enable(NSAPI, PS_DataPath, Credits) \
        Do_PS_DownlinkSDU_Enable_V2((PS_Extended_NSAPI_t)NSAPI, PS_DataPath, Credits)

#define DONE_PS_DOWNLINKSDU_ENABLE DONE_PS_DOWNLINKSDU_ENABLE_V2

PS_GeneralResult_t
Done_PS_DownlinkSDU_Enable (
  const void *const SigStruct_p,
  uint8 *const Credits_p,
  PS_NSAPI_t *const NSAPI_p);
#endif //SKIP_IN_REF_MAN


/************************************************************************/
// Event_PS_DownlinkSDU_Received_V2
/************************************************************************/

/**
 * If a service user has requested downlink SDUs (i.e. called function 
 * Do_PS_DownlinkSDU_Enable_V2), downlink SDUs are transferred to 
 * the service user in a number of these event signals.
 *
 * A service user that handles more than one PDP context and/or MBMS bearer service
 * must look at the Extended_NSAPI_p in order to separate between the different
 * services (PDP or MBMS).
 * 
 * @param [in,out] SigStruct_pp   See below. This is both an input and output 
 *                                parameter.
 * @param [out] SDU_pp            A reference to the pointer to the received 
 *                                downlink SDU buffer. Note that the service user 
 *                                shall not allocate the SDU buffer and that the 
 *                                pointer shall reference PS_SDU_NULL at function 
 *                                entry. The service provider must make sure that 
 *                                the following is fulfilled:
 *                                SDU_Size <=PS_MAX_SDU_SIZE
 *                                SDU_Offset >= PS_SDU_OFFSET_FOR_DOWNLINK_HEADERS  
 *                                otherwise the function will return with 
 *                                GS_EVENT_FAILED_UNPACKING.  
 *                                The service provider should if possible align 
 *                                the SDU to an even 32-bit address.
 * @param [out] Extended_NSAPI_p  Pointer to a value identifying to what PDP context
 *                                or MBMS bearer service the Event corresponds to 
 *                                (so that a service user can separate between several 
 *                                PDP contexts / MBMS bearer services).
 * @param [out] UplinkCredits_p   Pointer to piggybacked uplink credits value. 
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_DOWNLINKSDU_RECEIVED_V2
 */
  
EventStatus_t
Event_PS_DownlinkSDU_Received_V2(
  void **const SigStruct_pp,
  PS_SDU_t **const SDU_pp,
  PS_Extended_NSAPI_t *const Extended_NSAPI_p,
  uint8 *const UplinkCredits_p);

#ifndef SKIP_IN_REF_MAN
EventStatus_t
Event_PS_DownlinkSDU_Receive(
  void **const SigStruct_pp,
  PS_SDU_t **const SDU_pp,
  PS_NSAPI_t *const NSAPI_p);

#define Event_PS_DownlinkSDU_Received(SigStruct_pp, SDU_pp, Extended_NSAPI_p) \
        Event_PS_DownlinkSDU_Received_V2(SigStruct_pp, SDU_pp, Extended_NSAPI_p, NULL)
#define EVENT_PS_DOWNLINKSDU_RECEIVE EVENT_PS_DOWNLINKSDU_RECEIVED_V2
#define EVENT_PS_DOWNLINKSDU_RECEIVED EVENT_PS_DOWNLINKSDU_RECEIVED_V2
#endif //SKIP_IN_REF_MAN


/************************************************************************/
//Event_PS_UplinkCredits_Granted
/************************************************************************/
/**
 * This event carries the Uplink Credits value for the Uplink Flow. This event is
 * sent from the service provider to the service user when 
 * 1)a service user grants downlink credits to the service provider for the first time
 * 2)there is a need to update the service user with uplink credits 
 * 3)there is a new service user.
 *
 * @param [in] SigStruct_p            See below. This is both an input and output 
 *                                    parameter.
 * @param [out] UplinkCredits_p       Pointer to UplinkCredits value. 
 * @param [out] Extended_NSAPI_p      Pointer to a value identifying the 
 *                                    activated PDP context.
 *
 * @retval GS_EVENT_OK                Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING  The event signal could not be unpacked. 
 *                                    The output parameters do not contain valid 
 *                                    data.
 *
 * @signalid EVENT_PS_UPLINKCREDITS_GRANTED
 */

EventStatus_t
Event_PS_UplinkCredits_Granted(
  const void *const SigStruct_p,
  uint8 *const UplinkCredits_p,
  PS_Extended_NSAPI_t *const Extended_NSAPI_p);


#ifndef SKIP_IN_REF_MAN //Multicast only
/************************************************************************/
// Request_PS_MBMS_Context_Activate
/************************************************************************/

/**
 * This request is used to request activation of an MBMS context towards core NW.
 * The service user should call this function following reception of dispatched
 * event EVENT_PS_MBMS_CONTEXTACTIVATION_REQUESTED.
 * 
 * Note: It is the responsibility of the service user to allocate (and free) 
 * memory for any PCO to be sent to the network. If service user don't want 
 * to negotiate a PCO the MBMS_ProtocolOptions_p shall be set to NULL and 
 * MBMS_ProtocolOptionsLength shall be set to zero.
 * 
 * Note: this function is only valid for MBMS multicast services
 * (i.e. not MBMS (enhanced) broadcast services)
 *
 * @param [in] RequestCtrl_p                 Pointer to struct controlling whether the request 
 *                                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p                   Response signal to be unpacked.
 * @param [in] LinkedNSAPI                   Identifier of the PDP context on which a JOIN was sent.
 * @param [in] MBMS_MulticastAddress_p       Pointer to the multicast address of the MBMS bearer
 *                                           service the service user wants to activate.
 * @param [in] AccessPointName_p             Pointer to a string containing the 
 *                                           logical name of the access point.
 * @param [in] MBMS_ProtocolOptions_p        Pointer to the protocol configuration 
 *                                           options that the service user wishes to 
 *                                           negotiate. Note that the first two octets, 
 *                                           i.e. the protocol configuration option IEI 
 *                                           and the length field, shall not be included.
 * @param [in] MBMS_ProtocolOptionsLength    Length (bytes)  of the requested protocol 
 *                                           configuration options parameter string.
 * @param [out] MBMS_NSAPI_p                 MBMS NSAPI value of the MBMS multicast service to activate.
 * @param [out] Result_p                     Pointer to a value specifying the result of the 
 *                                           request.
 *
 * @retval REQUEST_OK                 The request of the Global Service was  
 *                                    successfully satisfied. The output 
 *                                    parameters contain valid data.
 * @retval REQUEST_PENDING            The request has been sent to the service 
 *                                    provider. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_FAILED_PARAMETER   There is an error in the input 
 *                                    parameter. No request has been sent to 
 *                                    the service provider. The output 
 *                                    parameters do not contain valid data.
 * @retval REQUEST_FAILED_APPLICATION The request has been sent to the service 
 *                                    provider, but the request of the Global 
 *                                    Service was not satisfied. The result 
 *                                    parameter indicates the error cause. 
 *                                    Other output parameters do not contain 
 *                                    valid data.
 * @retval REQUEST_FAILED_UNPACKING   The response signal could not be 
 *                                    unpacked. The output parameters do not 
 *                                    contain valid data.
 * @retval REQUEST_NOT_AVAILABLE      Service not available.
 *
 * @signalid RESPONSE_PS_MBMS_CONTEXT_ACTIVATE
 * @waitmode Both Wait mode and No wait mode is supported
 * @clienttag Yes
 * @related EVENT_PS_MBMS_CONTEXTSTATUS_CHANGED, EVENT_PS_PDP_CONTEXTDEACTIVATE_V2
 */

RequestStatus_t
Request_PS_MBMS_Context_Activate (
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_Extended_NSAPI_t LinkedNSAPI,
  const PS_MBMS_MulticastAddress_t * const MBMS_MulticastAddress_p,
  const uint8 *const AccessPointName_p,
  const uint8 *const MBMS_ProtocolOptions_p,
  const uint8 MBMS_ProtocolOptionsLength,
  PS_Extended_NSAPI_t *const MBMS_NSAPI_p, 
  PS_MBMS_ContextActivateResult_t *const Result_p);


/************************************************************************/
// Response_PS_MBMS_Context_Activate
/************************************************************************/

/**
 * Response function for Response_PS_MBMS_Context_Activate
 *
 */

RequestStatus_t
Response_PS_MBMS_Context_Activate (
  const void *const SigStruct_p,
  PS_Extended_NSAPI_t *const MBMS_NSAPI_p, 
  PS_MBMS_ContextActivateResult_t *const Result_p);


/************************************************************************/
// Event_PS_MBMS_ContextActivation_Requested
/************************************************************************/

/**
 * This dispatched event is sent when (following an IGMP JOIN from UE to NW)
 * the UE receives a request from the network to activate an MBMS context
 * (i.e. NAS message REQUEST MBMS CONTEXT ACTIVATION is received by UE).
 *
 * @param [in] SigStruct_p                    Event signal to be unpacked.
 * @param [out] MBMS_MulticastAddress_p       Pointer to multicast address for the MBMS bearer
 *                                            service the UE should activate following IGMP JOIN.
 * @param [out] AccessPointName_p             Pointer to APN value for the MBMS bearer service
 *                                            the UE should activate following IGMP JOIN.
 * @param [out] LinkedNSAPI_p                 Pointer to Linked NSAPI value for the MBMS bearer
 *                                            service the UE should activate following IGMP JOIN.
 * @param [out] MBMS_ProtocolOptions_pp       A reference to the pointer to the negotiated MBMS specific
 *                                            protocol configuration options excluding the IEI octet and
 *                                            the length octet for the MBMS bearer service the UE should
 *                                            activate following IGMP JOIN.
 * @param [out] MBMS_ProtocolOptionsLength_p  Pointer to the length (bytes) of the negotiated protocol
 *                                            configuration options.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_MBMS_CONTEXTACTIVATION_REQUESTED
 */

EventStatus_t Event_PS_MBMS_ContextActivation_Requested(
  const void *const SigStruct_p,
  PS_MBMS_MulticastAddress_t *const MBMS_MulticastAddress_p,
  uint8 *const AccessPointName_p,
  PS_Extended_NSAPI_t *const LinkedNSAPI_p,
  uint8 **const MBMS_ProtocolOptions_pp,
  uint8 *const MBMS_ProtocolOptionsLength_p);


/************************************************************************/
// Event_PS_MBMS_ContextStatus_Changed
/************************************************************************/

/**
 * This event is sent to the service user to report a change in status of an
 * MBMS context (multicast only).
 *
 * @param [in] SigStruct_p                    Event signal to be unpacked.
 * @param [out] TMGI_p                        Pointer to MBMS bearer service ID of
 *                                            joined service.
 * @param [out] MBMS_NSAPI_p                  Pointer to MBMS NSAPI value used to
 *                                            identify activated MBMS context.
 * @param [out] MBMS_ProtocolOptions_pp       A reference to the pointer to the 
 *                                            negotiated MBMS protocol configuration 
 *                                            options excluding the IEI octet and the 
 *                                            length octet.
 * @param [out] MBMS_ProtocolOptionsLength_p  Pointer to the length (bytes) of the 
 *                                            negotiated protocol configuration 
 *                                            options parameter string.
 * @param [out] Cause_p                       Pointer to a value describing a cause 
 *                                            code in error situations.
 * @param [out] ContextEvent_p                Pointer to a value describing the 
 *                                            event.
 *
 * @retval GS_EVENT_OK               Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING The event signal could not be unpacked. 
 *                                   The output parameters do not contain valid 
 *                                   data.
 *
 * @signalid EVENT_PS_MBMS_CONTEXTSTATUS_CHANGED
 */

EventStatus_t
Event_PS_MBMS_ContextStatus_Changed(
  const void *const SigStruct_p,
  PS_MBMS_TMGI_t *const TMGI_p,
  PS_Extended_NSAPI_t *const MBMS_NSAPI_p,
  uint8 **const MBMS_ProtocolOptions_pp,
  uint8 *const MBMS_ProtocolOptionsLength_p,
  PS_CauseCode_t *const Cause_p,
  PS_MBMS_ContextEvent_t *const ContextEvent_p);
#endif //SKIP_IN_REF_MAN

/******************************* MBMS END *******************************/

/************************************************************************/
// Event_PS_UsageStatus_Changed
/************************************************************************/

/**
 * This event is sent when the usage status of the PS user plane changes.
 *
 * @param [in]  SigStruct_p           Event signal to be unpacked.
 * @param [out] UserPlaneType_p       Pointer to a value indicating the current
 *                                    usage status of the PS user plane.
 * 
 * @retval GS_EVENT_OK                Output is valid.
 * @retval GS_EVENT_FAILED_UNPACKING  The event signal could not be unpacked. 
 *                                    The output parameters do not contain valid 
 *                                    data.
 * @sigbased No.
 * @related None.
 * @signalid EVENT_PS_USAGESTATUS_CHANGED
 */

EventStatus_t Event_PS_UsageStatus_Changed(
  const void*               const SigStruct_p,
  PS_UserPlaneType_t*       const UserPlaneType_p);

#ifndef SKIP_IN_REF_MAN //fast dormancy, customization
/************************************************************************/
// Request_PS_FastDormancy_Activate
/************************************************************************/

/**
 * This request informs NS that the service user decides to initiate
 * fast dormancy for the UE.
 * 
 * Note: UE will initiate PS signalling connection release on receiving 
 * this request function. In case if there is no concurrent CS signalling
 * connection, RRC state will switched to IDLE after PS signalling connection
 * is released. As a result, UE enters the so call "Dormancy" state. 
 * This means that a REQUEST_OK only indicates the acceptance of fast dormancy 
 * request from service user. Either UE enters "Dormancy" state or not 
 * depends on other conditions.
 * 
 * @param [in]  RequestCtrl_p                Pointer to struct controlling whether the request 
 *                                           is called in wait mode or no wait mode.
 * @param [in]  SigStruct_p                  Response signal to be unpacked.
 * @param [out] Result_p                     Pointer to a value specifying the result of the 
 *                                           request. 
 */

RequestStatus_t
Request_PS_FastDormancy_Activate( 
  const RequestControlBlock_t *const RequestCtrl_p,
  PS_GeneralResult_t *const Result_p );


/************************************************************************/
// Response_PS_FastDormancy_Activate
/************************************************************************/

/**
 * Response function for Request_PS_FastDormancy_Activate
 *
 */
RequestStatus_t
Response_PS_FastDormancy_Activate(
  const void *const SigStruct_p,
  PS_GeneralResult_t *const Result_p );
#endif //SKIP_IN_REF_MAN

/************************************************************************/

#endif // INCLUSION_GUARD_R_PSS_H 

/* End of file */
