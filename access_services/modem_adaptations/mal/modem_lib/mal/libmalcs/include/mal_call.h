/**
 * \file mal_call.h
 * \brief Includes interface headers for MAL CS call API.
 *
 * \n Copyright (C) ST-Ericsson 2010. All rights reserved.
 * \n This code is ST-Ericsson proprietary and confidential.
 * \n Any use of the code for whatever purpose is subject to
 * \n specific written permission of ST-Ericsson SA.
 * \n
 * \n This contains the description of the MAL CS Call Library's Interface header file information
 * \n Author: Jayarami Reddy <jayarami.reddy@stericsson.com>
 * \n         Parthasarathy Nagarajan <parthasarathy.nagarajan@stericsson.com> for ST-Ericsson
 * \n Version: 0.6
 * \n
 * \n Revision History:
 * \n   v0.6                 Added API for Call Last fail cause
 * \n
 * \n   v0.5                 Changes in EVENT ID values and removed two event types
 * \n
 * \n   v0.4                 correct suffix issue for mal_call_event_cb_t
 * \n                          Removed declaration mal_call_cbfunc_event_handler
 * \n
 * \n   v0.3                 Removed input param in mal_call_response_handler()
 * \n                          Minor updates to type names
 */

/** \defgroup MAL_CSCALL MAL CSCALL API
 *  \par
 *     MAL CS Call Library manages the CS Call modem server ISI messages
 */

#ifndef _CALL_LIB_H
#define _CALL_LIB_H

#include <stdint.h>

/** \def CALL_LIB_VERSION
 *  \brief Library Version for Call Lib
 *
 */
#define CALL_LIB_VERSION            "Call Lib V 0.6.r27"

/* ----------------------------------------------------------------------- */
/* Event ID's                                                              */
/* ----------------------------------------------------------------------- */

/** \def MAL_CALL_RING
 *  \brief Incoming call ring indication to upper layer.
 *
 */
#define MAL_CALL_RING                                1018

/** \def MAL_CALL_RING_WAITING
 *  \brief Waiting call indication to upper layer.
 *
 */
#define MAL_CALL_RING_WAITING                        1025

/** \def MAL_CALL_STATE_CHANGED
 *  \brief Call state change indication to upper layer \ref cscall_state.
 *
 */
#define MAL_CALL_STATE_CHANGED                       1001

/** \def MAL_CALL_SUPP_SVC_NOTIFICATION
 *  \brief Supplementary service related notification from the network to upper layer.
 *
 */
#define MAL_CALL_SUPP_SVC_NOTIFICATION               1011

/** \def MAL_CALL_CNAP
 *  \brief Supplementary service CNAP related notification from the network to upper layer.
 *
 */
#define  MAL_CALL_CNAP                               1002

/** \def MAL_CALL_COLP
 *  \brief Supplementary service COLP related notification from the network to upper layer.
 *
 */
#define MAL_CALL_COLP                                1003

/** \def MAL_CALL_GEN_ALERTING_TONE
 *  \brief Event to indicate start/stop the local generation of the ring tone.
 *
 */
#define MAL_CALL_GEN_ALERTING_TONE                   1004

/** \def MAL_CALL_MODEM_CREATE_RESP
 *  \brief Modem response for create request message during MO call dialup.
 *
 */
#define MAL_CALL_MODEM_CREATE_RESP                   0x02

/** \def MAL_CALL_MODEM_STATUS_RESP
 *  \brief Modem response for call status request message containing status info
 *  \n  for single or multiple calls.
 *
 */
#define MAL_CALL_MODEM_STATUS_RESP                   0x0E

/** \def MAL_CALL_MODEM_PROPERTY_GET_RESP
 *  \brief Modem response for call modem property get request containing the
 *  \n property info for requested modem property to get.
 *
 */
#define MAL_CALL_MODEM_PROPERTY_GET_RESP             0x20

/** \def MAL_CALL_MODEM_PROPERTY_SET_RESP
 *  \brief Modem response for call modem property set request containing the
 *  \n property info for requested modem property to set.
 *
 */
#define MAL_CALL_MODEM_PROPERTY_SET_RESP             0x22

/** \def MAL_CALL_MODEM_CONTROL_RESP
 *  \brief Modem response for call modem control request message.
 *
 */
#define MAL_CALL_MODEM_CONTROL_RESP                  0x12

/** \def MAL_CALL_MODEM_ANSWER_RESP
 *  \brief Modem response for call modem answer request message.
 *
 */
#define MAL_CALL_MODEM_ANSWER_RESP                   0x08

/** \def MAL_CALL_MODEM_RELEASE_RESP
 *  \brief Modem response for call modem release request message.
 *
 */
#define MAL_CALL_MODEM_RELEASE_RESP                  0x0A

/** \def MAL_CALL_MODEM_DTMF_STOP_RESP
 *  \brief Modem response for call modem DTMF stop request message.
 *
 */
#define MAL_CALL_MODEM_DTMF_STOP_RESP                0x1A

/** \def MAL_CALL_MODEM_DTMF_SEND_RESP
 *  \brief Modem response for call modem DTMF send request message.
 *
 */
#define MAL_CALL_MODEM_DTMF_SEND_RESP                0x18

/** \def MAL_CALL_MODEM_COMING_IND
 *  \brief Incoming call indication messgae from modem.
 *
 */
#define MAL_CALL_MODEM_COMING_IND                    0x03

/** \def MAL_CALL_MODEM_STATUS_IND
 *  \brief Call status indication messgae from modem containing the
 *  \n the current status of the call \ref cscall_state.
 *
 */
#define MAL_CALL_MODEM_STATUS_IND                    0x0F

/** \def MAL_CALL_MODEM_PROPERTY_SET_IND
 *  \brief Call modem property set indicaton message from modem.
 *
 */
#define MAL_CALL_MODEM_PROPERTY_SET_IND              0x23

/** \def MAL_CALL_MODEM_RELEASE_IND
 *  \brief Call modem release indicaton message from modem during a call release.
 *
 */
#define MAL_CALL_MODEM_RELEASE_IND                   0x0B

/** \def MAL_CALL_MODEM_TERMINATED_IND
 *  \brief Call modem terminate indicaton message from modem during the call
 *   \n termination from network.
 *
 */
#define MAL_CALL_MODEM_TERMINATED_IND                0x0C

/** \def MAL_CALL_MODEM_MESSAGE_IND
 *  \brief Call modem message indicaton message from modem.
 *
 */
#define MAL_CALL_MODEM_MESSAGE_IND                   0x50

/** \def MAL_CALL_MODEM_EMERG_NBR_IND
 *  \brief Call modem emergency number indicaton message from modem.
 *
 */
#define MAL_CALL_MODEM_EMERG_NBR_IND                 0x31

/** \def MAL_CALL_MODEM_EMERG_NBR_MODIFY_RESP
 *  \brief Modem response for call modem MODIFY request message.
 *
 */
#define MAL_CALL_MODEM_EMERG_NBR_MODIFY_RESP         0x25

/** \def MAL_CALL_MODEM_EMERG_NBR_GET_RESP
 *  \brief Modem response for call modem GET request message.
 *
 */
#define MAL_CALL_MODEM_EMERG_NBR_GET_RESP            0x27

/** \def MAL_CALL_MODEM_MO_ALERT_IND
 *  \brief Modem MO alert indication to know the status of call ring tone for local tone generation.
 *
 */
#define MAL_CALL_MODEM_MO_ALERT_IND                  0x04

/** \def MAL_CALL_MODEM_USER_CONNECT_IND
 *  \brief Modem connection indication to know the status of user connection attach (attached or closed).
 *
 */
#define MAL_CALL_MODEM_USER_CONNECT_IND              0x33

/** \def MAL_CALL_MODEM_SERVICE_DENIED_IND
 *  \brief Service denied indication from modem when a particular service request is
 *  \n denied by the modem.
 *
 */
#define MAL_CALL_MODEM_SERVICE_DENIED_IND            0x32


/** \def MAL_CALL_MODEM_DTMF_STATUS_IND
 *  \brief DTMF Status Indication from modem during a DTMF operation
 *
 */
#define MAL_CALL_MODEM_DTMF_STATUS_IND                 0x1B


/** \def MAL_CALL_MODEM_CONTROL_IND
 *  \brief Control Indication from modem during a Control operation
 *
 */
#define MAL_CALL_MODEM_CONTROL_IND                   0x13

/** \def MAL_CALL_MODEM_NOTIFICATION_IND
 *  \brief Notification Indications from modem containing the Suppl Service notificatons
 *
 */
#define MAL_CALL_MODEM_NOTIFICATION_IND              0xA0

/** \def MAL_CALL_MODEM_MT_ALERT_IND
 *  \brief MT Alerting Indication from modem for a Incoming call
 *
 */
#define MAL_CALL_MODEM_MT_ALERT_IND              0x05

/** \def MAL_CALL_MODEM_WAITING_IND
 *  \brief Call Waiting Indication from modem
 *
 */
#define MAL_CALL_MODEM_WAITING_IND              0x06


/* ----------------------------------------------------------------------- */
/* Call Client cause value                                                 */
/* ----------------------------------------------------------------------- */

/** \def MAL_CALL_MODEM_CAUSE_RELEASE_BY_USER
 *  \brief Can be used by client to indicate normal call clearing.
 *  \n  Maps to cause 16 in network interface.
 *
 */
#define MAL_CALL_MODEM_CAUSE_RELEASE_BY_USER         0x03

/** \def MAL_CALL_MODEM_CAUSE_BUSY_USER_REQUEST
 *  \brief Can be used by client to indicate that user has rejected the call.
 *  \n  Maps to cause 17 in network interface.
 *
 */
#define MAL_CALL_MODEM_CAUSE_BUSY_USER_REQUEST       0x04


/* ----------------------------------------------------------------------- */
/* Call Server cause value                                                 */
/* ----------------------------------------------------------------------- */


/** \def MAL_CALL_MODEM_CAUSE_NO_CALL
 *  \brief Indicates that call does not exists
 *
 */
#define MAL_CALL_MODEM_CAUSE_NO_CALL                 0x01


/** \def MAL_CALL_MODEM_CAUSE_ERROR_REQUEST
 *  \brief Indicates that request has been erroneus.
 *
 */
#define MAL_CALL_MODEM_CAUSE_ERROR_REQUEST           0x05

/** \def MAL_CALL_MODEM_CAUSE_CALL_ACTIVE
 *  \brief Indicates that procedure is rejected because call is active.
 *
 */
#define MAL_CALL_MODEM_CAUSE_CALL_ACTIVE             0x07

/** \def MAL_CALL_MODEM_CAUSE_NO_CALL_ACTIVE
 *  \brief Indicates that procedure is rejected because no call is active.
 *
 */
#define MAL_CALL_MODEM_CAUSE_NO_CALL_ACTIVE          0x08

/** \def MAL_CALL_MODEM_CAUSE_INVALID_CALL_MODE
 *  \brief Indicates that procedure is rejected because call mode is invalid.
 *
 */
#define MAL_CALL_MODEM_CAUSE_INVALID_CALL_MODE       0x09

/** \def MAL_CALL_MODEM_CAUSE_TOO_LONG_ADDRESS
 *  \brief Indicates that procedure is rejected because address is too long.
 *
 */
#define MAL_CALL_MODEM_CAUSE_TOO_LONG_ADDRESS        0x0B

/** \def MAL_CALL_MODEM_CAUSE_INVALID_ADDRESS
 *  \brief Indicates that procedure is rejected because address is invalid.
 *
 */
#define MAL_CALL_MODEM_CAUSE_INVALID_ADDRESS         0x0C

/** \def MAL_CALL_MODEM_CAUSE_EMERGENCY
 *  \brief Indicates that call is cleared because emergency call establishment has started.
 *
 */
#define MAL_CALL_MODEM_CAUSE_EMERGENCY               0x0D

/** \def MAL_CALL_MODEM_CAUSE_NO_SERVICE
 *  \brief Indicates that call is cleared because of no service.
 *
 */
#define MAL_CALL_MODEM_CAUSE_NO_SERVICE              0x0E

/** \def MAL_CALL_MODEM_CAUSE_CODE_REQUIRED
 *  \brief Call creation has failed because SIM code required or PIN rights lost.
 *
 */
#define MAL_CALL_MODEM_CAUSE_CODE_REQUIRED           0x10

/** \def MAL_CALL_MODEM_CAUSE_NOT_ALLOWED
 *  \brief Indicates that procedure is not allowed. Used e.g. if call control or call
 *  \brief answer request arrives to invalid call state.
 *
 */
#define MAL_CALL_MODEM_CAUSE_NOT_ALLOWED             0x11

/** \def MAL_CALL_MODEM_CAUSE_DTMF_ERROR
 *  \brief Indicates that DMTF handling error has occurred.
 *
 */
#define MAL_CALL_MODEM_CAUSE_DTMF_ERROR              0x12

/** \def MAL_CALL_MODEM_CAUSE_CHANNEL_LOSS
 *  \brief Indicates that call is cleared because call re-establishment has failed.
 *
 */
#define MAL_CALL_MODEM_CAUSE_CHANNEL_LOSS            0x13

/** \def MAL_CALL_MODEM_CAUSE_FDN_NOT_OK
 *  \brief Call creation has failed because of Fixed Dialling Number check.
 *
 */
#define MAL_CALL_MODEM_CAUSE_FDN_NOT_OK              0x14

/** \def MAL_CALL_MODEM_CAUSE_BLACKLIST_BLOCKED
 *  \brief Blacklist has blocked the number.
 *
 */
#define MAL_CALL_MODEM_CAUSE_BLACKLIST_BLOCKED       0x16

/** \def MAL_CALL_MODEM_CAUSE_BLACKLIST_DELAYED
 *  \brief Blacklist has delayed the call establishment to the number.
 *
 */
#define MAL_CALL_MODEM_CAUSE_BLACKLIST_DELAYED       0x17

/** \def MAL_CALL_MODEM_CAUSE_EMERGENCY_FAILURE
 *  \brief Indicates that emergency call has failed because network is not found.
 *
 */
#define MAL_CALL_MODEM_CAUSE_EMERGENCY_FAILURE       0x1A

/** \def MAL_CALL_MODEM_CAUSE_NO_SIM
 *  \brief Call creation has failed because no connection to SIM card.
 *
 */
#define MAL_CALL_MODEM_CAUSE_NO_SIM                  0x1F

/** \def MAL_CALL_MODEM_CAUSE_SIM_LOCK_OPERATIVE
 *  \brief Call creation has failed because SIM lock is operative.
 *
 */
#define MAL_CALL_MODEM_CAUSE_SIM_LOCK_OPERATIVE      0x20

/** \def MAL_CALL_MODEM_CAUSE_DTMF_SEND_ONGOING
 *  \brief Indicates that procedure is rejected because DTMF sending is ongoing.
 *
 */
#define MAL_CALL_MODEM_CAUSE_DTMF_SEND_ONGOING       0x24

/** \def MAL_CALL_MODEM_CAUSE_CS_INACTIVE
 *  \brief Indicates that procedure is rejected because CS is inactive.
 *
 */
#define MAL_CALL_MODEM_CAUSE_CS_INACTIVE             0x25

/** \def MAL_CALL_MODEM_CAUSE_NOT_READY
 *  \brief Indicates that procedure is rejected because startup is ongoing.
 *
 */
#define MAL_CALL_MODEM_CAUSE_NOT_READY               0x26

/** \def MAL_CALL_MODEM_CAUSE_INCOMPATIBLE_DEST
 *  \brief Indicates that procedure is rejected because Bearer Capability negotiation has failed.
 *
 */
#define MAL_CALL_MODEM_CAUSE_INCOMPATIBLE_DEST       0x27

/* ----------------------------------------------------------------------- */
/* Call client user defined macros                                                 */
/* ----------------------------------------------------------------------- */

/** \def MAL_CALL_MAX_DIGITS_OF_EMERGENCY_NUM
 *  \brief Indicates that maximun number of digits in emergency number.
 *
 */
#define MAL_CALL_MAX_DIGITS_OF_EMERGENCY_NUM        6


/** \def MAL_CALL_MODEM_SERVICE_CAT_NONE
 *  \brief Indicates the service category in emergency number.
 *
 */
#define MAL_CALL_MODEM_SERVICE_CAT_NONE             0x0

/** \def MAL_CALL_MODEM_SERVICE_CAT_POLICE
 *  \brief Indicates the service category in emergency number.
 *
 */
#define MAL_CALL_MODEM_SERVICE_CAT_POLICE           0x1

/** \def MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE
 *  \brief Indicates the service category in emergency number.
 *
 */
#define MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE        0x2

/** \def MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE
 *  \brief Indicates the service category in emergency number.
 *
 */
#define MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE     0x4

/** \def MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD
 *  \brief Indicates the service category in emergency number.
 *
 */
#define MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD     0x8

/** \def MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE
 *  \brief Indicates the service category in emergency number.
 *
 */
#define MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE  0x10

/** \def MAL_CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED
 *  \brief Indicates the emergency number origin is hard coded.
 *
 */
#define MAL_CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED    0x01

/** \def MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM
 *  \brief Indicates the emergency number origin is SIM.
 *
 */
#define MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM           0x02

/** \def MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK
 *  \brief Indicates the emergency number origin is network.
 *
 */
#define MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK       0x04

/** \def MAL_CALL_MODEM_EMERG_NBR_ORIG_PERMANENT_MEM
 *  \brief Indicates the emergency number origin is permanent memory.
 *
 */
#define MAL_CALL_MODEM_EMERG_NBR_ORIG_PERMANENT_MEM 0x08

/** \def MAL_CALL_MODEM_ID_CONFERENCE
 *  \brief Indicates the call id conference.
 *
 */
#define MAL_CALL_MODEM_ID_CONFERENCE                0x10

/** \def MAL_CALL_MODEM_ID_ALL
 *  \brief Indicates the call id all.
 *
 */
#define MAL_CALL_MODEM_ID_ALL                       0xF0


/**
 * \enum  cscall_state
 * \brief This enum has values of CS call states maintained in MAL CS Call Lib.
 */
typedef enum {
    CALL_ACTIVE       = 0,            /**< CS Call state Active    */
    CALL_HOLDING      = 1,            /**< CS Call state On Hold   */
    CALL_DIALING      = 2,            /**< CS Call state Dialing   */
    CALL_ALERTING     = 3,            /**< CS Call state Alerting  */
    CALL_INCOMING     = 4,            /**< CS Call state Incoming  */
    CALL_WAITING      = 5,            /**< CS Call state Waiting   */
    CALL_IDLE         = 6,            /**< CS Call state Idle      */
    CALL_MT_SETUP     = 7             /**< CS Call state MT Setup  */
} cscall_state;

/**
 * \enum  call_cause_type_sender
 * \brief This enum has values of last call cause type sender.
 */
typedef enum {
    CALL_CAUSE_NONE                      = 0,            /**< None                                                     */
    CALL_CAUSE_TYPE_CLIENT               = 1,            /**< Client cause value                                       */
    CALL_CAUSE_TYPE_SERVER               = 2,            /**< Call Server cause value                                  */
    CALL_CAUSE_TYPE_NETWORK              = 3,            /**< Network cause value as per 3GPP TS 24.008 ch. 10.5.4.11  */
} call_cause_type_sender;


/**
 * \enum  mal_call_modem_property
 * \brief This enum has values of supported call modem property.
 */
typedef enum {
    CALL_MODEM_PROPERTY_CTM               = 0,            /**< Cellular Text Telephone Modem property              */
    CALL_MODEM_PROPERTY_CLIR              = 1,            /**< CLIR Modem property              */
    CALL_MODEM_PROPERTY_ALS               = 2,            /**< ALS Modem property */
    CALL_MODEM_PROPERTY_UNKNOWN           = 3,            /**< Modem property Unknown                                   */
} mal_call_modem_property;

/**
 * \enum  mal_call_modem_prop_ctm
 * \brief This enum has possible values of CTM property setting.
 */
typedef enum {
    CALL_MODEM_PROPERTY_CTM_NOT_SUPPORTED               = 0,            /**< CTM property not supported             */
    CALL_MODEM_PROPERTY_CTM_SUPPORTED                   = 1             /**< CTM property supported              */
} mal_call_modem_prop_ctm;


/**
 * \enum  mal_call_modem_prop_als
 * \brief This enum has possible values of ALS property setting.
 */
typedef enum {
    CALL_MODEM_PROPERTY_ALS_LINE_NORMAL               = 0,            /**< ALS property LINE normal            */
    CALL_MODEM_PROPERTY_ALS_LINE_2                    = 1             /**< ALS property LINE 2                   */
} mal_call_modem_prop_als;


/**
 * \enum  mal_call_suppl_svc_notify
 * \brief This enum has enable/disable values for supplementary service related notifications.
 */
typedef enum {
    CALL_SUPPL_SVC_NOTIFY_DISABLE   = 0, /**< Disable supplementary service related notifications     */
    CALL_SUPPL_SVC_NOTIFY_ENABLE    = 1  /**< Enable supplementary service related notifications    */
} mal_call_suppl_svc_notify;


/**
 * \enum  mal_call_clir_service
 * \brief This enum has values of CLIR service status in the modem used for all outgoing calls, as defined in 3GPP 27.007 sec 7.7.
 */
typedef enum {
    CALL_CLIR_DEFAULT   = 0, /**< presentation indicator is used according to the subscription of the CLIR service     */
    CALL_CLIR_INVOCATION    = 1,  /**< restrict CLI presentation   */
    CALL_CLIR_SUPPRESSION    = 2  /**< allow CLI presentation    */
} mal_call_clir_service;

/**
 * \enum  mal_call_emerg_number_operation
 * \brief This enum has operation performed on emergency numbers.
 */
typedef enum {
    MAL_CALL_MODEM_EMERG_NUMBER_REMOVE = 0,
    MAL_CALL_MODEM_EMERG_NUMBER_ADD = 1
} mal_call_emerg_number_operation;

/**
 * \enum  mal_call_als_supported_lines
 * \brief This enum has ALS supported lines.
 */
typedef enum {
    ALS_LINE1 = 0,
    ALS_LINE1_AND_LINE2 = 1
} mal_call_als_supported_lines;

/**
 * \enum  mal_call_alert_tone
 * \brief This enum has alert tone actions.
 */
typedef enum {
    MAL_CALL_STOP_ALERT_TONE  = 0x00,
    MAL_CALL_START_ALERT_TONE = 0x01
} mal_call_alert_tone;

/**
 * \enum  mal_call_dtmf_string_type
 * \brief This enum has DTMF string types.
 */
typedef enum {
    UTF8_STRING = 0,  /* Supported UTF-8 type data : '0' to '9', '*', '#', 'A', 'B', 'C', 'D'. */
    BCD_STRING  = 1   /* Supported BCD string type data : '0' to '9',0xA ('*'),0xB ('#'), 0xF(aligner). */
} mal_call_dtmf_string_type;

/**
 * \enum  mal_call_call_type
 * \brief This enum has call types.
 */
typedef enum {
    MAL_CALL_TYPE_UNKNOWN   = 0,
    MAL_CALL_TYPE_EMERGENCY = 1,
    MAL_CALL_TYPE_VOICE     = 2
} mal_call_call_type;

/**
 * \struct  mal_call_dtmf_info
 * \brief This structure contains the details of dtmf send request.
*/
typedef struct {
    mal_call_dtmf_string_type   string_type;   /**< Type of the string \ref mal_call_dtmf_string_type */
    char                        *dtmf_string;  /**< ASCII or BCD byte stream,  \ref mal_call_dtmf_string_type supported bytes */
    uint16_t                    length;        /**< Number of DTMF numbers in case of BCD,
                                                    Number of DTMF digits in case of UTF-8 string type */
    uint16_t                    dtmf_duration_time; /**<values: 0(Default), 1(No delay), 5 to 1000 in ms */
    uint16_t                    dtmf_pause_time;    /**<values: 0(Default), 1(No delay), 5 to 1000 in ms */
} mal_call_dtmf_info;

/**
 * \struct  connected_line
 * \brief This structure contains the details of connected line information.
 */
typedef struct {
    uint8_t   addressType;               /**< as defined in 3GPP TS 24.008 ch. 10.5.4.7 */
    uint8_t   connectedLinePresentation; /**< as defined in 3GPP TS 24.008 ch. 10.5.4.9 */
    char     *connectedNumber;           /**< connected line number */
    uint8_t   connectedSubAddrType;      /**< as defined in 3GPP TS 24.008 ch. 10.5.4.14 */
    char     *connectedSubAddr;          /**< connected line sub-address */
} connected_line;

/**
 * \struct  exit_cause
 * \brief This structure contains the details of call failure reasons.
 *        See also \ref mal_call_request_last_call_fail_cause
 *        NOTE: The members detailed_cause_length and detailed_cause contains valid values only if there is detailed cause information from Modem or Network in case of call failure.
*/
typedef struct {
    call_cause_type_sender cause_type_sender;  /**< Cause Type Sender \ref call_cause_type_sender */
    uint8_t  cause;                            /**< Call Cause */
    uint8_t  detailed_cause_length;            /**< as defined in 3GPP TS 24.008 ch. 10.5.4.11 in IE 'Cause IE' octet 2 contains length */
    uint8_t *detailed_cause;                   /**< as defined in 3GPP TS 24.008 ch. 10.5.4.11 in IE 'Cause IE' from octet 3 till octet 2 length */
} exit_cause;

/**
*\struct mal_call_uus
*\brief Call user to user structure
*\brief The structure contains the uus length and data
*/
typedef struct {
    uint8_t     uus_length;     /**< as defined in 3GPP TS 24.008 ch. 10.5.4.25 in IE 'User-user IEI' octet 2 contains length */
    uint8_t     *uus;           /**< as defined in 3GPP TS 24.008 ch. 10.5.4.25 in IE 'User-user IEI' from octet 4 till octet 2 length */
} mal_call_uus;

/**
 * \struct mal_call_dailup
 * \brief  Call dialup structure
 * \brief  This structure contains the phone number and CLIR information
 *         provided by RIL for call request dialup \ref mal_call_request_dialup
 */
typedef struct {
    mal_call_call_type call_type;   /**< Call type \ref mal_call_call_type*/
    char      *phone_number;        /**< Phone number */
    int32_t    clir;                /**< same as 'n' paremeter in TS 27.007 7.7 "+CLIR"
                                      * clir == 0 on "use subscription default value"
                                      * clir == 1 on "CLIR invocation" (restrict CLI presentation)
                                      * clir == 2 on "CLIR suppression" (allow CLI presentation)
                                    */
    uint8_t    bc_length;          /**< As per Bearer Capability IE' in 3GPP TS 24.008 ch. 10.5.4.5. First byte contains octet 2*/
    uint8_t    *bc_data;           /**< As per Bearer Capability IE' in 3GPP TS 24.008 ch. 10.5.4.5. First byte contains octet 3 onwards*/
    mal_call_uus    *uus_info;     /**< UUS info \ref mal_call_uus*/
} mal_call_dailup;


/**
 * \enum  mal_call_call_mode
 * \brief This enum has call modes.
 */
typedef enum {
    MAL_CALL_MODE_EMERGENCY     = 0x00,
    MAL_CALL_MODE_SPEECH        = 0x01,
    MAL_CALL_MODE_ALS_LINE_2    = 0xA2,
    MAL_CALL_MODE_MULTIMEDIA    = 0xA3,
    MAL_CALL_MODE_UNKNOWN       = 0xFF,
} mal_call_call_mode;

/**
 * \struct  mal_call_context
 * \brief   MAL CS Call context structure.
 * \brief   This structure contains the call context information for a CS Call.
 * \brief   message_type, message_direction and transaction_id provide collective
 *          information related to network operation as seen in the context of the
 *          call.
 */
typedef struct {
    cscall_state            callState;            /**< Call state \ref cscall_state */
    int32_t                 callId;               /**< GSM Index for use with, eg, AT+CHLD */
    int32_t                 addrType;             /**< type of address, eg 145 = intl */
    uint8_t                 message_type;         /**< Message type
                                                       1 = Setup,
                                                       2 = Connect,
                                                       3 = Release */
    uint8_t                 message_direction;    /**< Message direction
                                                       1 = Sent to network,
                                                       2 = Received from network,
                                                       3 = Internal */
    uint8_t                 transaction_id;       /**< Transaction ID.
                                                       Transaction identifier from the received
                                                       or sent message. TI flag is coded to bit 8
                                                       and bits 5 to 7 are TI value. Bits 1 to 4
                                                       are set to zero. If internal call release
                                                       has occurred field is coded as clearing
                                                       would have been initiated by mobile i.e.
                                                       similar to messages sent to network. */
    char                    isMpty;               /**< nonzero if is mpty call */
    char                    isMT;                 /**< nonzero if call is mobile terminated */
    char                    als;                  /**< ALS line indicator if available (0 == line1) */
    mal_call_call_mode      call_mode;            /**< call_mode \ref mal_call_call_mode */
    char                    isVoicePrivacy;       /**< nonzero if CDMA voice privacy mode is active */
    char                   *number;               /**< phone number */
    uint8_t                 numberPresentation;   /**< The two least significant
                                                   *   bits (1-2) contain screening indicator and
                                                   *   the two bits (6-7) contain presentation
                                                   *   indicator. See 'Calling Party BCD Number IE
                                                   *   in 3GPP TS 24.008 ch. 10.5.4.9 */
    char                    call_cli_cause;       /**< 1 = CLI_UNAVAILABLE,
                                                       2 = CLI_REJECT_BY_USER,
                                                       3 = CLI_INTERACTION,
                                                       4 = CLI_COIN_LINE_PAYPHONE */
    uint8_t                 name_length;          /**< Remote party name character length */
    uint16_t               *name;                 /**< Remote party name follows unicode UCS2
                                                   *   coding scheme, last 2 bytes are NUL terminated */
    uint8_t                 namePresentation;     /**< The two least significant
                                                   *   bits (1-2) contain screening indicator and
                                                   *   the two bits (6-7) contain presentation
                                                   *   indicator. See 'numberPresentation' above. */
    connected_line          connectedLine;        /**< Connected line details \ref connected_line*/
    exit_cause              call_cause;           /**< Exit cause \ref exit_cause
                                                   Valid when the callState is CALL_IDLE   */
    mal_call_uus            call_uus_info;        /**< UUS info \ref mal_call_uus*/
} mal_call_context;

/**
 * \struct  mal_supp_svc_notification
 * \brief   MAL supplementary service notification  structure.
 * \brief   This structure contains the supplementary service notification
 *          information for the network.
 */
typedef struct {
    int32_t     notificationType;    /**<
                                          0 = intermediate call setup result code
                                          1 = MT setup or unsolicited result code */
    int32_t     code;                /**< See 27.007 7.17
                                          "code1" for notificationType = 0
                                          "code2" for notificationType = 1. */
    int32_t     index;               /**< CUG index. See 27.007 7.17. */
    int32_t     type;                /**< "type" from 27.007 7.17 (MT only). */
    char       *number;              /**< "number" from 27.007 7.17
                                          (MT only, may be NULL). */
} mal_supp_svc_notification;


/**
 * \struct  mal_call_emerg_number_struct
 * \brief   MAL call emergency number request structure.
 */
typedef struct {
    mal_call_emerg_number_operation operation; /**< Operation ADD or REMOVE*/
    uint8_t *mcc;    /**< Mobile Country Code, Null terminated char array of max:3, NULL when unused */
    uint8_t *mcc_range;/**< Mobile Country Code Sequence char, ranges: '2' to '9', NULL when unused */
    uint8_t srvc_cat; /**< Service Category, bit mask value as mentioned in Call client user defined macros */
    uint8_t *emergency_num; /**< Emergency number, NULL terminated char array of max:6 */
} mal_call_emerg_number_struct;


/**
 * \struct  emergency_number
 * \brief   MAL call emergency number structure.
 * \typedef mal_call_emerg_num_list_struct
 * \brief   MAL call emergency number response structure.
 */
typedef struct emergency_number {
    uint8_t *alpha_id;  /**< alpha identifer, NULL terminated char array */
    uint8_t emer_num_origin; /**<  value of memory origin */
    uint8_t emergency_num[MAL_CALL_MAX_DIGITS_OF_EMERGENCY_NUM + 1]; /**<  Emergency number, array of max:6 + 1 NUL terminator */
    uint8_t srvc_cat; /**< Service Category, bit mask value as mentioned in Call client user defined macros  */
    struct emergency_number *next;  /**< self referential pointer */
} mal_call_emerg_num_list_struct;

/**
 * \struct  emergency_number_entry
 * \brief   MAL call emergency number entry.
 * \typedef mal_call_emergency_number_entry
 * \brief   MAL call emergency number entry structure.
 */
typedef struct emergency_number_entry {
    uint8_t emer_num_origin; /**<  value of memory origin */
    uint8_t srvc_cat;
    uint8_t num_len;
    uint8_t emergency_num[MAL_CALL_MAX_DIGITS_OF_EMERGENCY_NUM + 1];
} mal_call_emergency_number_entry;

/**
 * \struct  emergency_number_list
 * \brief   MAL call emergency number list.
 * \typedef mal_call_emergency_number_list
 * \brief   MAL call emergency number indication list structure.
 */
typedef struct emergency_number_list {
    uint8_t num_of_numbers;
    struct emergency_number_entry emergency_numbers[1];
} mal_call_emergency_number_list;

/**
 * \struct  mal_call_modem_prop_als_resp
 * \brief   MAL call modem property als response structure.
 */
typedef struct {
    mal_call_modem_prop_als current_line; /**< Current line set in modem \ref mal_call_modem_prop_als */
    mal_call_als_supported_lines supported_lines; /**< Supported lines by the modem \ref mal_call_als_supported_lines */
} mal_call_modem_prop_als_resp;




/** Function/Service list of MAL Call client library */

/**
 * \typedef typedef void (*mal_call_event_cb_t)(int32_t mal_call_event_id, void* data, int32_t mal_error, void* client_tag);
 * \brief callback function for any expected or unexpected event information from modem.
 * \param [in] mal_call_event_id id of unsolicited event.
 * \param [in] data pointer to parameter values associated with this event (only fail cause at the moment)
 * \param [in] mal_error error type associated with this event
 * \param [in] client_tag In case of response, pointer to RIL client specific data assosicated with the service request.
 *                                In case of Indications, NULL
 * return None
 *
 * \par
 * Following is the detail on data based on the event type:
 * \par
 * \arg MAL_CALL_RING
 * \n Pointer to \ref mal_call_context structure
 * \par
 * \arg MAL_CALL_RING_WAITING
 * \n Pointer to \ref mal_call_context structure
 * \par
 * \arg MAL_CALL_STATE_CHANGED
 * \n Pointer to \ref mal_call_context structure
 * \par
 * \arg MAL_CALL_SUPP_SVC_NOTIFICATION
 * \n Pointer to \ref mal_supp_svc_notification structure
 * \par
 * \arg MAL_CALL_MODEM_CREATE_RESP
 * \n Pointer to \ref mal_call_context structure, when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
 * \arg MAL_CALL_MODEM_STATUS_RESP
 * \n NULL
 * \par
 * \arg MAL_CALL_MODEM_PROPERTY_GET_RESP
 * \n For mal_call_request_get_clir request or,
 * \n For mal_call_request_modem_property_get request with property type CALL_MODEM_PROPERTY_CLIR,
 * \n Pointer to \ref mal_call_clir_service (as per TS 27.007 Sec 7.7) when mal_error is MAL_SUCCESS.
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL.
 * \n For mal_call_request_modem_property_get request with property type CALL_MODEM_PROPERTY_CTM,
 * \n Pointer to \ref mal_call_modem_prop_ctm when mal_error is MAL_SUCCESS.
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL.
 * \n For mal_call_request_modem_property_get request with property type CALL_MODEM_PROPERTY_ALS,
 * \n Pointer to \ref mal_call_modem_prop_als_resp when mal_error is MAL_SUCCESS.
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL.
 * \par
 * \arg MAL_CALL_MODEM_PROPERTY_SET_RESP
 * \n NULL
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
 * \arg MAL_CALL_MODEM_CONTROL_RESP
 * \n NULL, when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
 * \arg MAL_CALL_MODEM_ANSWER_RESP
 * \n NULL, when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
 * \arg MAL_CALL_MODEM_RELEASE_RESP
 * \n NULL, when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
 * \arg MAL_CALL_MODEM_DTMF_STOP_RESP
 * \n NULL, when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
 * \arg MAL_CALL_MODEM_DTMF_SEND_RESP
 * \n NULL  when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
  * \arg MAL_CALL_MODEM_EMERG_NBR_MODIFY_RESP
 * \n NULL, when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
  * \arg MAL_CALL_MODEM_EMERG_NBR_GET_RESP
 * \n singly linked list head pointer to \ref mal_call_emerg_num_list_struct, when mal_error is MAL_SUCCESS
 * \n Pointer to \ref exit_cause or NULL(no error information is available), when mal_error is MAL_FAIL
 * \par
 * \arg MAL_CALL_MODEM_COMING_IND
 * \n NULL
 * \par
 * \arg MAL_CALL_MODEM_STATUS_IND
 * \n NULL
 * \par
 * \arg MAL_CALL_MODEM_PROPERTY_SET_IND
 * \n NULL
 * \par
 * \arg MAL_CALL_MODEM_RELEASE_IND
 * \n NULL
 * \par
 * \arg MAL_CALL_MODEM_TERMINATED_IND
 * \n NULL
 * \par
 * \arg MAL_CALL_MODEM_MESSAGE_IND
 * \n NULL
 * \par
 * \arg MAL_CALL_MODEM_SERVICE_DENIED_IND
 * \n NULL
 * \par
 * \arg MAL_CALL_CNAP
 * \n Pointer to \ref mal_call_context structure
 * \par
 * \arg MAL_CALL_COLP
 * \n Pointer to \ref mal_call_context structure
 * \par
 * \arg MAL_CALL_GEN_ALERTING_TONE
 * \n Pointer to \ref mal_call_alert_tone enum
 */

typedef    void (*mal_call_event_cb_t)(int32_t mal_call_event_id, void *data, int32_t mal_error, void *client_tag);


/**
 *   \fn int32_t mal_call_init(int32_t *fd_call);
 *   \brief This Api is used to initialize the call library,Opens socket using phonet lib and
 *   \brief Should be called only once at the time of initialization.
 *   \param [in] fd_call points to file descriptor of call socket. Data Type: int32_t
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_init(int32_t *fd_call);

/**
 *   \fn int32_t mal_call_config(void)
 *   \brief This Api is used to conigure the call library(if any properties to be set).
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_config(void);

/**
 *   \fn int32_t mal_call_deinit(void)
 *   \brief This Api is used to de-initialize the call library.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_deinit(void);

/**
 *   \fn int32_t mal_call_register_callback(mal_call_event_cb_t event_cb)
 *   \brief This Api is a call back handler for the events.
 *   \param[in] event_cb event callback handler.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_register_callback(mal_call_event_cb_t event_cb);

/**
 *   \fn void mal_call_response_handler(void)
 *   \brief This Api is called by application to inform that some message is
 *          available from modem for processing.
 *   \return None
 */
void mal_call_response_handler(void);

/**
 *   \fn int32_t mal_call_request_dialup(void* data, void *client_tag)
 *   \brief This Api is used for the dial up request.
 *   \param[in] data pointer to the data. data type: \ref  mal_call_dailup structure type.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_dialup(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_hangup(void* data, void *client_tag)
 *   \brief This Api is used for the hang up request(mobile originated/mobile terminated).
 *   \param[in] data pointer to the data. data type: uint8_t callId.
 *   \param[in] client_tag Void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_hangup(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_answer(void* data, void *client_tag)
 *   \brief This Api is used for the answer request.
 *   \param[in] data pointer to the data. data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_answer(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_getcalllist(void* data, void** response, void* numOfCalls)
 *   \brief This Api is used to get the current call status. This is a SYNCHRONOUS CALL.
 *   \param[in] data pointer to the data. data to be NULL/uint8_t callid.
 *   \param[out] response pointer to the response.
 *               response to be pointer to pointer to \ref mal_call_context structure type.
 *   \param[out] numOfCalls number of current calls. data type: uint8_t
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_getcalllist(void *data, void **response, void *numOfCalls);


/**
 *   \fn int32_t mal_call_request_freecalllist(void* data)
 *   \brief This Api is used free the call list. This is a SYNCHRONOUS CALL.
 *   \param[in] data pointer to the data.
 *              Data type: pointer to \ref mal_call_context structure type
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_freecalllist(void *data);

/**
 *   \fn int32_t mal_call_request_hold(void* data, void *client_tag)
 *   \brief This Api is used for the hold request.
 *   \param[in] data pointer to the data(input data).Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_hold(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_resume(void* data, void *client_tag)
 *   \brief This Api is used for the resume request of the hold call.
 *   \param[in] data pointer to the data. Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_resume(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_swap_call(void* data, void *client_tag)
 *   \brief This Api is used to swap the hold and active calls.
 *   \param[in] data pointer to the data. Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_swap_call(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_conference_call(void* data, void *client_tag)
 *   \brief This Api is used for the conference call request to join the call in the conference/to build the conference.
 *   \param[in] data pointer to the data. Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_conference_call(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_conference_call_split(void* data, void *client_tag)
 *   \brief This Api is used to split the call from the conference.
 *   \param[in] data pointer to the data. Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_conference_call_split(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_explicit_call_transfer(void* data, void *client_tag)
 *   \brief This Api is used to transfer the call(explicit call transfer).
 *   \brief This API to be used only when 2 calls ars existing, one
 *   \brief call is active/alerting and other is in hold state.
 *   \param[in] data pointer to the data. Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_explicit_call_transfer(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_release(void* data, void *client_tag)
 *   \brief This Api is used for the release request.
 *   \param[in] data pointer to the data.Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_release(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_get_clir(void *client_tag)
 *   \brief This Api is used get call modem line id property setting from the call server.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_get_clir(void *client_tag);

/**
 *   \fn int32_t mal_call_request_set_clir(void* data, void *client_tag)
 *   \brief This Api is used set clir property of the call server.
 *   \param[in] data pointer to the data.
 *              Data type: uint8_t. Possible values, "n" parameter from TS 27.007 7.7
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_set_clir(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_dtmf_send(void* call_id, void* data, void *client_tag)
 *   \brief This Api is used to Send a DTMF tone(s).
 *   \param[in] call_id modem call id.  Data type: uint8_t callId.
 *   \param[in] data pointer to the data.
 *              Data type: pointer to structure \ref mal_call_dtmf_info structure.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_dtmf_send(void *call_id, void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_dtmf_start(void* call_id, void* data, void *client_tag)
 *   \brief This Api is used to Start playing a DTMF tone. Continue playing DTMF tone until
 *   \brief mal_call_request_dtmf_stop request is received.
 *   \brief If a mal_call_request_dtmf_send request is received while a tone is currently playing,
 *   \brief it should cancel the previous tone and play the new one.
 *   \param[in] call_id modem call id.  Data type: uint8_t callId.
 *   \param[in] data pointer to the data.
 *              Data type: char (single character with one of 12 values: 0-9,*,#)
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_dtmf_start(void *call_id, void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_dtmf_stop(void* data, void *client_tag)
 *   \brief This Api is used to Stop currently playing DTMF tone.
 *   \param[in] data pointer to the data. Data type: uint8_t callId.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_dtmf_stop(void *data, void *client_tag);

/**
 *  \fn int32_t mal_call_request_last_call_fail_cause(void* data)
 *  \brief  The request is DEPRECATED,
 *  \brief  Call fail cause is provided in \ref MAL_CALL_STATE_CHANGED indication when Call State is IDLE.
 *  \brief  This Api is used to get the Last CALL fail cause. This is a SYNCHRONOUS CALL.
 *  \param[out] data pointer to the data.
 *              Data type: \ref exit_cause.
 *   \return mal error type. Data type: int32_t (SUCCESS)
 */
int32_t mal_call_request_last_call_fail_cause(void *data);

/**
 *  \fn int32_t mal_call_set_supp_svc_notification(void* data)
 *  \brief This Api is used to enable /disable supplementary service related notifications from the network.
 *  \param[in] data pointer to the data. Data type: \ref mal_call_suppl_svc_notify.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_set_supp_svc_notification(void *data);

/**
 *  \fn int32_t mal_call_request_modem_property_get(void *property, void *client_tag)
 *  \brief This Api is used to get call server modem property setting for the provided property type.
 *  \param[in] property pointer to the call modem property. Data type: \ref mal_call_modem_property.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_modem_property_get(void *property, void *client_tag);

/**
 *  \fn int32_t mal_call_request_modem_property_set(void *property, void *data, void *client_tag)
 *  \brief This Api is used to set the call server modem property setting for the provided property type.
 *  \param[in] property pointer to the call modem property. Data type: \ref mal_call_modem_property.
 *  \param[in] data pointer to the data.
 *             If property type is CALL_MODEM_PROPERTY_CLIR,Data type: uint8_t. "n" parameter from TS 27.007 7.7
 *             If property type is CALL_MODEM_PROPERTY_CTM,Data type: \ref mal_call_modem_prop_ctm.
 *             If property type is CALL_MODEM_PROPERTY_ALS,Data type: \ref mal_call_modem_prop_als.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_modem_property_set(void *property, void *data, void *client_tag);


/**
 *   \fn int32_t mal_call_request_modem_emerg_number_modify(void *data, void *client_tag)
 *   \brief This Api is used to modify the emergency numbers stored in the modem.
 *   \param[in] data pointer to the emergency number struct. Data type: \ref mal_call_emerg_number_struct.
 *                    if the operation is ADD, note that only one emergency number can be added in modem at a time.
 *                    if the operation is REMOVE, All the emergency number stored in the modem permenant memory shall be erased.
 *                                                            Its not required to populate the rest of the members in \ref mal_call_emerg_number_struct.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_modem_emerg_number_modify(void *data, void *client_tag);

/**
 *   \fn int32_t mal_call_request_get_emerg_num_list(void *client_tag)
 *   \brief This Api is used to get the emergency number list stored in modem, In the response message
 *    for this request a head node of list of type mal_call_emerg_num_list_struct is sent.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_get_emerg_num_list(void *client_tag);

/**
 *  \fn int32_t mal_call_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for call module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_call_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for call module. This is a synchronous call.
 *   \param[out] debug level for call  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_call_request_get_debug_level(uint8_t *level);

#endif //__MAL_CALL_H

