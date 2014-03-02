#ifndef INCLUSION_GUARD_T_SMS_CB_SESSION_H
#define INCLUSION_GUARD_T_SMS_CB_SESSION_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION:
 *
 * SMS CB Session include file for exported interface types.
 *
 *************************************************************************/
#include <stdint.h>
#include "t_sms.h"

/********************
* boolean: TRUE/FALSE
*********************/
#ifndef FALSE
/** Value representing the boolean/logical value false. */
#define FALSE 0
#endif
#ifndef TRUE
/** Value representing the boolean/logical value true. */
#define TRUE (!FALSE)
#endif

/**
 * Return type for Global Service request and response functions.
 *
 * @param SMS_REQUEST_OK Used to indicate that the service has been performed successfully.
 * @param SMS_REQUEST_PENDING Used in NO_WAIT_MODE to indicate that the request has been sent to the server.
 * @param SMS_REQUEST_NOT_AVAILABLE Service is not available. Often depends on that the service is configured out.
 * @param SMS_REQUEST_FAILED Service failed.
 * @param SMS_REQUEST_FAILED_RESOURCE Used to indicate that there were not enough resources to perform the service.
 * @param SMS_REQUEST_TIME_OUT Used to indicate that the service timed out.
 * @param SMS_REQUEST_FAILED_PARAMETER Used to indicate that one/several of the parameters are incorrect.
 * @param SMS_REQUEST_FAILED_APPLICATION Application failed.
 * @param SMS_REQUEST_FAILED_UNPACKING Used to indicate that something went wrong when trying to unpack the signal.
 */
typedef enum {
    SMS_REQUEST_OK,
    SMS_REQUEST_PENDING,
    SMS_REQUEST_NOT_AVAILABLE,
    SMS_REQUEST_FAILED,
    SMS_REQUEST_FAILED_RESOURCE,
    SMS_REQUEST_TIME_OUT,
    SMS_REQUEST_FAILED_PARAMETER,
    SMS_REQUEST_FAILED_APPLICATION,
    SMS_REQUEST_FAILED_UNPACKING,
    SMS_REQUEST_FAILED_IPC
} SMS_RequestStatus_t;

/**
 * Return type for Event functions. Event functions are used to unpack signal
 * data from event signals. Event signals are those used to signal Event
 * Channel subscriber (when an event occurred) or those used to signal a client
 * process when a service with event has been requested (using the
 * Request-Response-Event design pattern).
 *
 * @param SMS_GS_EVENT_OK Used to indicate that the event data was successfully retrieved.
 * @param SMS_GS_EVENT_FAILED_UNPACKING Used to indicate that something went wrong when trying to unpack the event.
 * @param SMS_GS_EVENT_FAILED_APPLICATION Used to indicate that the application failed.
 * @param SMS_GS_EVENT_FAILED_PARAMETER Used to indicate that one/several of the parameters are incorrect.
 * @param SMS_GS_EVENT_NOT_AVAILABLE Used to indicate that the event unpack function is not available.
 */
typedef enum {
    SMS_EVENT_OK,
    SMS_EVENT_FAILED_UNPACKING,
    SMS_EVENT_FAILED_APPLICATION,
    SMS_EVENT_FAILED_PARAMETER,
    SMS_EVENT_NOT_AVAILABLE
} SMS_EventStatus_t;


/**
 * Used to specify whether a Global Service request is to be executed
 * synchronously (WAIT_MODE) or asynchronously (NO_WAIT_MODE).
 *
 * @param SMS_WAIT_MODE A Global Service will execute synchronously (i.e. "hang" until the call returns).
 * @param SMS_NO_WAIT_MODE A Global Service will execute asynchronously.
 */
typedef enum {
    SMS_WAIT_MODE,
    SMS_NO_WAIT_MODE
} SMS_WaitMode_t;



/**
 * Defines the value that is used to indicate that a client application did not
 * specify a client tag. This is only defined here for legacy software.
 */
#define SMS_CLIENT_TAG_NOT_USED 0


/**
 * Used in all Global Service request functions. It specifies the function
 * execution mode, WAIT_MODE or NO_WAIT_MODE, Client Tag and the client callback
 * specification.
 *
 * @param WaitMode Specifies whether the service executes in WAIT_MODE or NO_WAIT_MODE
 * @param ClientTag The Client Tag value for the service.
 */
typedef struct {
    SMS_WaitMode_t WaitMode;
    SMS_ClientTag_t ClientTag;
    int Socket;
} SMS_RequestControlBlock_t;

/*
**========================================================================
** Signal IDs of Reqest, Response, Event and Timer signals. (All signals)
**========================================================================
*/
/**
  * @param REQUEST_SMS_CB_SESSIONCREATE                 Request signal for create SMS CB Server communication session.
  * @param REQUEST_SMS_CB_SESSIONDESTROY                Request signal for destroy SMS CB Server communication session.
  * @param REQUEST_SMS_SHORTMESSAGEREAD                 Request signal for read SMS.
  * @param REQUEST_SMS_SHORTMESSAGEWRITE                Request signal for write SMS.
  * @param REQUEST_SMS_SHORTMESSAGEDELETE               Request signal for delete SMS and associated Status Report.
  * @param REQUEST_SMS_STATUSREPORTREAD                 Request signal for read Status Report.
  * @param REQUEST_SMS_SHORTMESSAGESTATUSSET            Request signal for set SMS status.
  * @param REQUEST_SMS_SHORTMESSAGEFIND                 Request signal for find SMS.
  * @param REQUEST_SMS_STORAGECAPACITYGET               Request signal for get SMS capacity.
  * @param REQUEST_SMS_STORAGECLEAR                     Request signal for clear storage.
  * @param REQUEST_SMS_EVENTSSUBSCRIBE                  Request signal for subscribe to SMS unsolicited events including MT SMS received.
  * @param REQUEST_SMS_EVENTSUNSUBSCRIBE                Request signal for unsubscribe to SMS unsolicited events including MT SMS received.
  * @param REQUEST_SMS_APPLICATIONPORTSUBSCRIBE         Request signal for subscribe to SMS containing specific application ports.
  * @param REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE       Request signal for unsubscribe to SMS containing specific application ports.
  * @param REQUEST_SMS_SHORTMESSAGEINFOREAD             Request signal for read SMS TPDU header information.
  * @param REQUEST_SMS_SHORTMESSAGEDELIVER              Request signal for deliver TPDU.
  * @param REQUEST_SMS_SHORTMESSAGESEND                 Request signal for send SMS.
  * @param REQUEST_SMS_DELIVERREPORTSEND                Request signal for send Deliver Report.
  * @param REQUEST_SMS_MEMCAPACITYAVAILABLESEND         Request signal for send Memory Capacity Available.
  * @param REQUEST_SMS_MEMCAPACITYFULLREPORT            Request signal for report Memory Capacity Full.
  * @param R_REQ_SMS_PREFERREDSTORAGESET                Request signal for set Preferred Storage.
  * @param R_REQ_SMS_DELIVERREPORTCONTROLSET            Request signal for set Deliver Report Control.
  * @param R_REQ_SMS_DELIVERREPORTCONTROLGET            Request signal for get Deliver Report Storage.
  * @param R_REQ_SMS_MO_ROUTESET                        Request signal for set MO SMS route.
  * @param R_REQ_SMS_MO_ROUTEGET                        Request signal for get MO SMS route.
  * @param R_REQ_SMS_RELAYLINKCONTROLSET                Request signal for set Relay link control.
  * @param R_REQ_SMS_RELAYLINKCONTROLGET                Request signal for get Relay link control.
  * @param RESPONSE_SMS_CB_SESSIONCREATE                Response signal for create SMS CB Server communication session.
  * @param RESPONSE_SMS_CB_SESSIONDESTROY               Response signal for destroy SMS CB Server communication session.
  * @param RESPONSE_SMS_SHORTMESSAGEREAD                Response signal for read SMS.
  * @param RESPONSE_SMS_SHORTMESSAGEWRITE               Response signal for write SMS.
  * @param RESPONSE_SMS_SHORTMESSAGEDELETE              Response signal for delete SMS and associated Status Report.
  * @param RESPONSE_SMS_STATUSREPORTREAD                Response signal for read Status Report.
  * @param RESPONSE_SMS_SHORTMESSAGESTATUSSET           Response signal for set SMS status.
  * @param RESPONSE_SMS_SHORTMESSAGEFIND                Response signal for find SMS.
  * @param RESPONSE_SMS_STORAGECAPACITYGET              Response signal for get SMS capacity.
  * @param RESPONSE_SMS_STORAGECLEAR                    Response signal for clear storage.
  * @param RESPONSE_SMS_EVENTSSUBSCRIBE                 Response signal for subscribe to SMS unsolicited events including MT SMS received.
  * @param RESPONSE_SMS_EVENTSUNSUBSCRIBE               Response signal for unsubscribe to SMS unsolicited events including MT SMS received.
  * @param RESPONSE_SMS_APPLICATIONPORTSUBSCRIBE        Response signal for subscribe to SMS containing specific application ports.
  * @param RESPONSE_SMS_APPLICATIONPORTUNSUBSCRIBE      Response signal for unsubscribe to SMS containing specific application ports.
  * @param RESPONSE_SMS_SHORTMESSAGEINFOREAD            Response signal for read SMS TPDU header information.
  * @param RESPONSE_SMS_SHORTMESSAGEDELIVER             Response signal for deliver TPDU.
  * @param RESPONSE_SMS_SHORTMESSAGESEND                Response signal for send SMS.
  * @param RESPONSE_SMS_DELIVERREPORTSEND               Response signal for send Deliver Report.
  * @param RESPONSE_SMS_MEMCAPACITYAVAILABLESEND        Response signal for send Memory Capacity Available.
  * @param REQUEST_SMS_MEMCAPACITYFULLREPORT            Response signal for report Memory Capacity Full.
  * @param R_RESP_SMS_PREFERREDSTORESET                 Response signal for set Preferred Storage.
  * @param R_RESP_SMS_DELIVERREPORTCONTROLSET           Response signal for set Deliver Report Control.
  * @param R_RESP_SMS_DELIVERREPORTCONTROLGET           Response signal for get Deliver Report Storage.
  * @param R_RESP_SMS_MO_ROUTESET                       Response signal for set MO SMS route.
  * @param R_RESP_SMS_MO_ROUTEGET                       Response signal for get MO SMS route.
  * @param R_RESP_SMS_RELAYLINKCONTROLSET               Response signal for set Relay link control.
  * @param R_RESP_SMS_RELAYLINKCONTROLGET               Response signal for get Relay link control.
  * @param EVENT_SMS_SERVERSTATUSCHANGED                Event signal to notify application of the Message Transport Server's status.
  * @param EVENT_SMS_ERROROCCURRED                      Event signal to notify application of an error.
  * @param EVENT_SMS_STORAGESTATUSCHANGED               Event signal to notify application of MTS storage status.
  * @param EVENT_SMS_SHORTMESSAGERECEIVED               Event signal to notify application of a received SMS.
  * @param EVENT_SMS_APPLICATIONPORTMATCHED             Event signal to notify application of a received SMS matching a subscribed application port range.
  * @param EVENT_SMS_STATUSREPORTRECEIVED               Event signal to notify application of a received Status Report.
  * @param REQUEST_CBS_SUBSCRIBE                        Request signal for subscription to receive CB Messages
  * @param REQUEST_CBS_UNSUBSCRIBE                      Request signal for unsubscription to receive CB Messages
  * @param REQUEST_CBS_GETNUMBEROFSUBSCRIPTIONS         Request signal for retrieval of current number of subscriptions
  * @param REQUEST_CBS_GETSUBSCRIPTIONS                 Request signal for retrieval of current subscriptions
  * @param REQUEST_CBS_ACTIVATE                         Request signal for activation of CB Message reception
  * @param REQUEST_CBS_DEACTIVATE                       Request signal for deactivation of CB Message reception
  * @param REQUEST_CBS_CELLBROADCASTMESSAGEREAD          Request signal for reading a CB Message
  * @param RESPONSE_CBS_SUBSCRIBE                       Response signal for subscription to receive CB Messages
  * @param RESPONSE_CBS_UNSUBSCRIBE                     Response signal for unsubscription to receive CB Messages
  * @param REQUEST_CBS_GETNUMBEROFSUBSCRIPTIONS         Response signal for retrieval of current number of subscriptions
  * @param RESPONSE_CBS_GETSUBSCRIPTIONS                Response signal for retrieval of current subscriptions
  * @param RESPONSE_CBS_ACTIVATE                        Response signal for activation of CB Message reception
  * @param RESPONSE_CBS_DEACTIVATE                      Response signal for deactivation of CB Message reception
  * @param RESPONSE_CBS_CELLBROADCASTMESSAGEREAD        Response signal for reading a CB Message
  * @param EVENT_CBS_CELLBROADCASTMESSAGERECEIVE        Event signal to notify application of a received CB Message
  */

typedef enum {
    /*
     **========================================================================
     ** Signal IDs of Request signals
     **========================================================================
     */
    /*
     * SMS CB Session Interface
     *
     */
    REQUEST_SMS_CB_SESSIONCREATE,
    REQUEST_SMS_CB_SESSIONDESTROY,
    REQUEST_SMS_CB_SHUTDOWN,
  /**
   * Short Message Storage Interface
   *
   */
    REQUEST_SMS_SHORTMESSAGEREAD,
    REQUEST_SMS_SHORTMESSAGEWRITE,
    REQUEST_SMS_SHORTMESSAGEDELETE,
    REQUEST_SMS_STATUSREPORTREAD,
    REQUEST_SMS_SHORTMESSAGESTATUSSET,
    REQUEST_SMS_SHORTMESSAGEFIND,
    REQUEST_SMS_STORAGECAPACITYGET,
    REQUEST_SMS_STORAGECLEAR,
    REQUEST_SMS_SHORTMESSAGEINFOREAD,

  /**
   * Short Message Interface
   *
   */
    REQUEST_SMS_EVENTSSUBSCRIBE,
    REQUEST_SMS_EVENTSUNSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTSUBSCRIBE,
    REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE,
    REQUEST_SMS_SHORTMESSAGEDELIVER,
    REQUEST_SMS_SHORTMESSAGESEND,
    REQUEST_SMS_DELIVERREPORTSEND,
    REQUEST_SMS_MEMCAPACITYAVAILABLESEND,
    REQUEST_SMS_MEMCAPACITYFULLREPORT,
    REQUEST_SMS_MEMCAPACITYGETSTATE,

  /**
   * Restricted Short Message Interface
   *
   */
    R_REQ_SMS_PREFERREDSTORAGESET,
    R_REQ_SMS_DELIVERREPORTCONTROLSET,
    R_REQ_SMS_DELIVERREPORTCONTROLGET,
    R_REQ_SMS_MO_ROUTESET,
    R_REQ_SMS_MO_ROUTEGET,
    R_REQ_SMS_RELAYLINKCONTROLSET,
    R_REQ_SMS_RELAYLINKCONTROLGET,
    /*
     **========================================================================
     ** Signal IDs of Response signals
     **========================================================================
     */
    /*
     * SMS CB Session Interface
     *
     */
    RESPONSE_SMS_CB_SESSIONCREATE,
    RESPONSE_SMS_CB_SESSIONDESTROY,

    /*
     * SMS Storage Interface
     *
     */
    RESPONSE_SMS_SHORTMESSAGEREAD,
    RESPONSE_SMS_SHORTMESSAGEWRITE,
    RESPONSE_SMS_SHORTMESSAGEDELETE,
    RESPONSE_SMS_STATUSREPORTREAD,
    RESPONSE_SMS_SHORTMESSAGESTATUSSET,
    RESPONSE_SMS_SHORTMESSAGEFIND,
    RESPONSE_SMS_STORAGECAPACITYGET,
    RESPONSE_SMS_STORAGECLEAR,
    RESPONSE_SMS_SHORTMESSAGEINFOREAD,

    /*
     * SMS Interface
     *
     */
    RESPONSE_SMS_EVENTSSUBSCRIBE,
    RESPONSE_SMS_EVENTSUNSUBSCRIBE,
    RESPONSE_SMS_APPLICATIONPORTSUBSCRIBE,
    RESPONSE_SMS_APPLICATIONPORTUNSUBSCRIBE,
    RESPONSE_SMS_SHORTMESSAGEDELIVER,
    RESPONSE_SMS_SHORTMESSAGESEND,
    RESPONSE_SMS_DELIVERREPORTSEND,
    RESPONSE_SMS_MEMCAPACITYAVAILABLESEND,
    RESPONSE_SMS_MEMCAPACITYFULLREPORT,
    RESPONSE_SMS_MEMCAPACITYGETSTATE,

    /*
     * Restricted SMS Interface
     *
     */
    R_RESP_SMS_RELAYLINKCONTROLSET,
    R_RESP_SMS_RELAYLINKCONTROLGET,
    R_RESP_SMS_PREFERREDSTORAGESET,
    R_RESP_SMS_DELIVERREPORTCONTROLSET,
    R_RESP_SMS_DELIVERREPORTCONTROLGET,
    R_RESP_SMS_MO_ROUTESET,
    R_RESP_SMS_MO_ROUTEGET,

/*
**========================================================================
** Signal IDs of Event signals
**========================================================================
*/
    EVENT_SMS_SERVERSTATUSCHANGED,
    EVENT_SMS_ERROROCCURRED,
    EVENT_SMS_STORAGESTATUSCHANGED,
    EVENT_SMS_SHORTMESSAGERECEIVED,
    EVENT_SMS_APPLICATIONPORTMATCHED,
    EVENT_SMS_STATUSREPORTRECEIVED,
/*
**========================================================================
** Signal IDs of Timer signals
**========================================================================
*/
/*
 * Timer Command Identities
 */
    MSG_STORAGE_TIMER_IND,
    MSG_SMSTORAGE_INIT_TIMER_IND,
    MSG_OTADP_TIMER_IND,
    MSG_SMS_DELIVER_REPORT_TIMER_IND,
    MSG_RELAY_LINK_CONTROL_TIMER_IND,
    MSG_SMS_MORETOSEND_TIMER_IND,
    MSG_MAL_CONFIGURE_RETRY_TIMER_IND,

    /*
     **========================================================================
     ** Signal IDs of Request signals
     **========================================================================
     */
  /**
   * Cell Broadcast Interface
   *
   */

  /**
    * @brief The REQUEST_CBS_SUBSCRIBE identifies the request signal to subscribe to Cell Broadcast messages.
    */
    REQUEST_CBS_SUBSCRIBE,

  /**
    * @brief The REQUEST_CBS_UNSUBSCRIBE identifies the request signal to unsubscribe from Cell Broadcast messages.
    */
    REQUEST_CBS_UNSUBSCRIBE,

  /**
    * @brief The REQUEST_CBS_GETNUMBEROFSUBSCRIPTIONS identifies the request signal to get the current number of subscriptions.
    */
    REQUEST_CBS_GETNUMBEROFSUBSCRIPTIONS,

  /**
    * @brief The REQUEST_CBS_GETSUBSCRIPTIONS identifies the request signal to get the current subscriptions.
    */
    REQUEST_CBS_GETSUBSCRIPTIONS,

  /**
    * @brief The REQUEST_CBS_ACTIVATE identifies the request signal to activate current Cell Broadcast subscriptions.
    */
    REQUEST_CBS_ACTIVATE,

  /**
    * @brief The REQUEST_CBS_DEACTIVATE identifies the request signal to deactivate current Cell Broadcast subscriptions.
    */
    REQUEST_CBS_DEACTIVATE,

  /**
    * @brief The REQUEST_CBS_CELLBROADCASTMESSAGEREAD identifies the request signal to read a Cell Broadcast message stored in RAM.
    */
    REQUEST_CBS_CELLBROADCASTMESSAGEREAD,

    /*
     **========================================================================
     ** Signal IDs of Response signals
     **========================================================================
     */
    /*
     * Cell Broadcast Interface
     *
     */

  /**
    * @brief The RESPONSE_CBS_SUBSCRIBE identifies the response signal to subscribe to Cell Broadcast messages.
    */
    RESPONSE_CBS_SUBSCRIBE,

  /**
    * @brief The RESPONSE_CBS_UNSUBSCRIBE identifies the response signal to unsubscribe from Cell Broadcast messages.
    */
    RESPONSE_CBS_UNSUBSCRIBE,

  /**
    * @brief The RESPONSE_CBS_GETNUMBEROFSUBSCRIPTIONS identifies the response signal to get the current number of subscriptions.
    */
    RESPONSE_CBS_GETNUMBEROFSUBSCRIPTIONS,

  /**
    * @brief The RESPONSE_CBS_GETSUBSCRIPTIONS identifies the response signal to get the current subscriptions.
    */
    RESPONSE_CBS_GETSUBSCRIPTIONS,

  /**
    * @brief The RESPONSE_CBS_ACTIVATE identifies the response signal to activate current Cell Broadcast subscriptions.
    */
    RESPONSE_CBS_ACTIVATE,

  /**
    * @brief The RESPONSE_CBS_DEACTIVATE identifies the response signal to deactivate current Cell Broadcast subscriptions.
    */
    RESPONSE_CBS_DEACTIVATE,

  /**
    * @brief The RESPONSE_CBS_CELLBROADCASTMESSAGEREAD identifies the response signal to read a Cell Broadcast message stored in RAM.
    */
    RESPONSE_CBS_CELLBROADCASTMESSAGEREAD,

    /*
     **========================================================================
     ** Signal IDs of Event signals
     **========================================================================
     */

  /**
    * @brief The EVENT_CBS_CELLBROADCASTMESSAGERECEIVED identifies the event signal to receive a Cell Broadcast message.
    */
    EVENT_CBS_CELLBROADCASTMESSAGERECEIVED,

    /*
     **========================================================================
     ** Signal IDs of Internal signals
     **========================================================================
     */

  /**
    * @brief The CBS_MESSAGE_RECEIVED_IND identifies the event signal to receive an alive timer for test purposes.
    */
    CBS_MESSAGE_RECEIVED_IND,

    /*
     **========================================================================
     ** Signal IDs of Timer signals
     **========================================================================
     */

  /**
    * @brief The CAT_REFRESH_FILE_CHANGE_TIMER_IND identifies the event signal to receive a timer signal needed when
    *        handling multiple EF File notifications as a result of a CAT Refresh event.
    */
    CAT_REFRESH_FILE_CHANGE_TIMER_IND,

  /**
    * @brief The CBS_ALIVE_TIMER_IND identifies the event signal to receive an alive timer for test purposes.
    */
    CBS_ALIVE_TIMER_IND,

    /*
     **========================================================================
     ** Signal IDs of Test Harness signals
     **========================================================================
     */
#ifdef SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED
  /**
    * @brief The REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_IND signal is used for internal test purposes.
    */
    REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_IND,

  /**
    * @brief The REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_FILE_IND signal is used for internal test purposes.
    */
    REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_FILE_IND,
#endif
#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
  /**
    * @brief The REQUEST_SMS_TEST_UTIL_MEMORY_STATUS_GET signal is used for internal test purposes.
    */
    REQUEST_SMS_TEST_UTIL_MEMORY_STATUS_GET
#endif
} SMS_SignalIdentifiers_t;


#endif                          //INCLUSION_GUARD_T_SMS_CB_SESSION_H
