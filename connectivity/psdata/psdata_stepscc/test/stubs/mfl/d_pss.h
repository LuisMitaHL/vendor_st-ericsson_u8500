/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Service category Packet Switched Services (PSS)
 * 
 */

#ifndef INCLUSION_GUARD_D_PSS_H
#define INCLUSION_GUARD_D_PSS_H


#include "t_pss.h"

/*
 *====================================================================
 *
 * Function declarations
 *
 *====================================================================
 */

/**
 * A service user that has one or more PDP contexts or MBMS bearer services 
 * activated shall subscribe to this event by calling the function 
 * Request_EventChannel_Subscribe() with argument Event_Id set to 
 * EVENT_ID_PS_DATAPATHCONNECT_SENT. The service user shall also maintain a 
 * static local variable of type PROCESS for each PDP context / MBMS bearer service 
 * used by the service user. This variable identifies the current Service 
 * Handler for SDU transfer on the specified PDP context / MBMS bearer service
 * and shall be included as argument when calling Do_PS_UplinkSDU_Send_V2 and 
 * Do_PS_DownlinkSDU_Enable_V2. 
 *
 * Upon receiving this event, the service user shall call the 
 * correspondent event unpack function to extract the NSAPI parameter. 
 * If it matches the NSAPI parameter of a PDP context / MBMS bearer service
 * handled by the service user, the corresponding Service Handler variable 
 * shall be updated with the process identity extracted with said unpack 
 * function. Further, the downlink credit counter for the indicated 
 * PDP context shall be set to zero and any missing responses to 
 * previous calls to functions Do_PS_UplinkSDU_Send_V2 and 
 * Do_PS_DownlinkSDU_Enable_V2 for the indicated NSAPI shall be 
 * cancelled. The SDU transfer shall be continued by calling the 
 * uplink and the downlink functions, this time, of course, using the 
 * new value of the Service Handler variable. When the service user 
 * has deactivated all PDP contexts / MBMS bearer services, it 
 * should unsubscribe to this event.
 *
 * @param [in] Extended_NSAPI       Identity of the active PDP context / MBMS bearer service.
 *
 * @eventid  EVENT_ID_PS_DATAPATHCONNECT_SENT
 * @signalid EVENT_PS_DATAPATHCONNECT_SENT
 */

void Dispatch_PS_DataPathConnect_Sent(const PS_Extended_NSAPI_t Extended_NSAPI);

#ifndef SKIP_IN_REF_MAN
#define Dispatch_PS_DataPathConnect(NSAPI) Dispatch_PS_DataPathConnect_Sent((PS_Extended_NSAPI_t)NSAPI)
#endif //SKIP_IN_REF_MAN


/**
 * A service user that is interested in getting periodic reports of the time and data volume
 * counters for a PDP context should subscribe to this event. Note that the reports are
 * disabled by default and must be enabled through the global service 
 * Request_PS_PeriodicReport_Enable.
 *
 * @param [in] NSAPI                Identity of the PDP context that produce the report.
 *
 * @param [in] TimeAndDataVolume_p  Pointer to a structure including time and data
 *                                  volume counters for the PDP context with the
 *                                  identifier NSAPI.
 *
 * @eventid  EVENT_ID_PS_PERIODICREPORT
 * @signalid EVENT_PS_PERIODICREPORT
 */

void Dispatch_PS_PeriodicReport (const PS_NSAPI_t NSAPI,
                                 const PS_TimeAndDataVolume_t *const TimeAndDataVolume_p);


/**
 * A service user that is interested in getting PS user plane suspend notifications
 * should subscribe to this event. Note that the notifications are disabled by
 * default and must be enabled through the global service 
 * Request_PS_UserPlaneStatusReport_Enable.
 *
 * The service user should use this event as an indication on that the PS user
 * plane is temporarily unavailable. It is sent when an ongoing data transmission
 * is suspended, in idle mode to indicate that channel establishment is not
 * possible at the moment, and also to indicate that channel establishment is ongoing.
 *
 * The event is dispatched only if at least one client has enabled
 * suspend/resume notification and if at least one PDP context is active.
 *
 * @param [in] SuspendCause         Parameter indicating the reason for why the PS
 *                                  user plane is suspended.
 *
 * @param [in] EstimatedSuspendTime Parameter indicating the estimated suspend time.
 *                                  In this release the estimated suspend time is
 *                                  always set to PS_USER_PLANE_SUSPEND_TIME_UNKNOWN.
 *
 * @eventid  EVENT_ID_PS_USERPLANESUSPENDED
 * @signalid EVENT_PS_USERPLANESUSPENDED
 */
void Dispatch_PS_UserPlaneSuspended(const PS_UserPlaneSuspendCause_t SuspendCause,
                                    const uint32 EstimatedSuspendTime);


/**
 * A service user that is interested in getting PS user plane resume notifications
 * should subscribe to this event. Note that the notifications are disabled by
 * default and must be enabled through the global service 
 * Request_PS_UserPlaneStatusReport_Enable.
 *
 * The service user should use this event as an indication on that the PS user
 * plane is available. It is sent when an ongoing data transmission is resumed,
 * in idle mode to indicate that channel establishment is possible or when
 * a channel has been established.
 *
 * The event is dispatched only if at least one client has enabled
 * suspend/resume notification and if at least one PDP context is active.
 *
 * Note: No parameters are sent with this event, thus no event function is needed.
 *
 * @eventid  EVENT_ID_PS_USERPLANERESUMED
 * @signalid EVENT_PS_USERPLANERESUMED
 */
void Dispatch_PS_UserPlaneResumed(void);


/**
 * A service user that is interested in getting notifications when the usage status
 * of the PS user plane changes, should subscribe to this event.
 *
 * @param [in] UserPlaneType    The current usage status of the PS user plane.
 * 
 * @eventid  EVENT_ID_PS_USAGESTATUS_CHANGED
 * @signalid EVENT_PS_USAGESTATUS_CHANGED
 */
void Dispatch_PS_UsageStatus_Changed(const PS_UserPlaneType_t UserPlaneType);


/**
 * This event is dispatched when the user plane could not be established due to temporary 
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
 * @param [in] NSAPI          Corresponding NSAPI.
 * @param [in] UplinkCredits  Uplink Credits.
 *
 * @eventid  EVENT_ID_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
 * @signalid EVENT_PS_USERPLANEESTABLISHMENTTEMPORARYFAILED_V2
 */
void Dispatch_PS_UserPlaneEstablishmentTemporaryFailed_V2(const PS_NSAPI_t NSAPI, const uint8 UplinkCredits);

#ifndef SKIP_IN_REF_MAN
#define Dispatch_PS_UserPlaneEstablishmentTemporaryFailed() \
        Dispatch_PS_UserPlaneEstablishmentTemporaryFailed_V2(PS_NSAPI_UNDEFINED, 0)
#endif //SKIP_IN_REF_MAN



/****************************** MBMS START ******************************/

/**
 * This dispatched event is sent when a change associated with an activated MBMS 
 * bearer service has occurred. Examples:
 * - Notification that the MBMS data of an activated MBMS bearer service (identified 
 * by the TMGI and session ID) is imminent to be received (i.e. MBMS session start)
 * - Notification that bearers have been released (i.e. MBMS session stop)
 *
 * @param [in] TMGI_p                       Pointer to TMGI of notified MBMS session.
 * @param [in] SessionID                    Session ID of notified MBMS session. Use of session ID is optional,
 *                                          when EventCause is PS_MBMS_EVENT_CAUSE_SESSION_ACTIVE. 
 *                                          If the session ID is not available the UE should assume that 
 *                                          this session will not be repeated (and consequently should be received by UE). 
 *                                          In case another session without session ID is announced for this service 
 *                                          the UE should assume that this session contains new content.
 * @param [in] EventCause                   Value specifying action for the notified MBMS
 *                                          service, e.g. PS_MBMS_EVENT_CAUSE_SESSION_ACTIVE.
 * @param [in] ValidParamsMask              A bitmask specifies if TMGI or SessionID is present.
 *
 * @eventid  EVENT_ID_PS_MBMS_SERVICESTATE_CHANGED
 * @signalid EVENT_PS_MBMS_SERVICESTATE_CHANGED
 */

void Dispatch_PS_MBMS_ServiceState_Changed(const PS_MBMS_TMGI_t *const TMGI_p,
                                           const PS_MBMS_SessionID_t SessionID,
                                           const PS_MBMS_EventCause_t EventCause,
                                           const uint8 ValidParamsMask);


/**
 * This dispatched event is sent after a successful GPRS attach or Routing Area Update (RAU).
 * It contains information about whether or not the current serving NW (SGSN) supports MBMS.
 *
 * This is only sent when the support from the NW changes (not after each attach/RAU). It is sent 
 * regardless of if any MBMS bearer service is activated.
 *
 * @param [in] MBMS_NW_FeatureSupport    New MBMS NW feature support status 
 *
 * @eventid  EVENT_ID_PS_MBMS_NW_FEATURESUPPORT_CHANGED
 * @signalid EVENT_PS_MBMS_NW_FEATURESUPPORT_CHANGED
 */

void Dispatch_PS_MBMS_NW_FeatureSupport_Changed(const PS_MBMS_NW_FeatureSupport_t MBMS_NW_FeatureSupport);


#ifndef SKIP_IN_REF_MAN //Multicast only
/**
 * This dispatched event is sent when the UE receives a request from the network to activate 
 * an MBMS context (i.e. NAS message REQUEST MBMS CONTEXT ACTIVATION is received by UE).
 *
 * Note: The NW is triggered to send the NAS message REQUEST MBMS CONTEXT ACTIVATION by UE sending
 * IGMP JOIN.
 *
 * @param [in] MulticastAddress_p     Multicast address for the MBMS bearer service 
 *                                    the NW wants to activate following IGMP JOIN
 * @param [in] AccessPointName_p      APN value for the service the NW wants to activate following 
 *                                    IGMP JOIN
 * @param [in] LinkedNSAPI            Linked NSAPI value for the service the NW wants to activate 
 *                                    following IGMP JOIN
 * @param [in] MBMS_ProtocolOptions_p Pointer to the MBMS specific protocol configuration options that 
 *                                    the NW wishes to negotiate (following an IGMP JOIN)
 * @param [in] ProtocolOptionsLength  Length of the proposed protocol configuration options 
 *                                    parameter string.
 *
 * @eventid  EVENT_ID_PS_MBMS_CONTEXTACTIVATION_REQUESTED
 * @signalid EVENT_PS_MBMS_CONTEXTACTIVATION_REQUESTED
 */

void Dispatch_PS_MBMS_ContextActivation_Requested(const PS_MBMS_MulticastAddress_t *const MulticastAddress_p,
                                                  const uint8 *const AccessPointName_p,
                                                  const PS_NSAPI_t LinkedNSAPI,
                                                  const uint8 *const MBMS_ProtocolOptions_p,
                                                  const uint8 ProtocolOptionsLength);
#endif //SKIP_IN_REF_MAN

/******************************* MBMS END *******************************/

#endif // INCLUSION_GUARD_D_PSS_H
