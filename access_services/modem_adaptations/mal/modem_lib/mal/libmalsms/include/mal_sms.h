/** \file   mal_sms.h
 * \brief  This file includes headers for SMS-CBS MAL API.
 *
 * \n Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * \n This code is ST-Ericsson proprietary and confidential.
 * \n Any use of the code for whatever purpose is subject to
 * \n specific written permission of ST-Ericsson SA.
 * \par
 * \n MAL Header file for SMS-CBS Library
 * \n \author   ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v1.3  Added netlink socket fd, used to communicate with shm driver for modem silent reboot.
 * \n
 * \n v1.2  Added prefered SMS route set/get APIs
 * \n
 * \n v1.1  Incorporated the review comments.
 * \n
 * \n v1.0  Added Cell Broadcast, Memory Status APIs, CB data handling structure and corresponding events.
 * \n
 * \n v0.9  Replaced boolean with uint8_t
 * \n
 * \n v0.8  Added pointer to phonet socket fd in  mal_sms_cbs_init
 * \n          Minor syntax corrections
 * \n
 */

/** \defgroup  MAL_SMS_CBS SMS-CBS API
 *  \par
 *      This part describes the interface to SMS-CBS MAL Client
 */


#ifndef _SMS_LIB_H
#define _SMS_LIB_H

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

/** \def MAL_SMS_MAX_PACKED_ADDRESS_LENGTH
 *  \brief Maximum number of octets in a packed format subscriber number
 *
 *   Defines the maximum number of octets in a packed format subscriber number.
 */
#define MAL_SMS_MAX_PACKED_ADDRESS_LENGTH            10

/** \def MAL_SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH
 *  \brief Maximum length of TPDU's
 *
 *   Defines the maximum length of TPDU's.
 */
#define MAL_SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH        164

/** \def MAL_CBS_PAGE_MAX_LENGTH
 *  \brief Maximum length in bytes of a packed Cell Broadcast PAGE
 *
 *   Defines the maximum length in bytes of a packed Cell Broadcast PAGE.
 */
#define MAL_CBS_PAGE_MAX_LENGTH                      82

/** \def MAL_SMS_MESSAGE_SEND_RESP
 *  \brief SMS Send Response
 *
 *   The SMS send response event is received after sending
 *   the SMS. It provided success/failure of sent SMS.
 */
#define MAL_SMS_MESSAGE_SEND_RESP                    0x03

/** \def MAL_SMS_RECEIVED_MSG_REPORT_RESP
 *  \brief SMS Receive Report Response
 *
 *   The SMS receive report response event is recevied when
 *   mobile terminated SMS report response is received.
 */
#define MAL_SMS_RECEIVED_MSG_REPORT_RESP             0x3c


/** \def MAL_SMS_RECEIVED_MSG_IND
 *  \brief SMS Receive Indication
 *
 *   The SMS receive indication event is recieved when a new
 *   SMS is received.
 */
#define MAL_SMS_RECEIVED_MSG_IND                     0x43

/** \def MAL_SMS_RECEIVED_SIM_MSG_IND
 *  \brief SMS Received SIM indication
 *
 *   The SMS receive SIM indication event is recieved when a new
 *   SMS is received in SIM.
 */
#define MAL_SMS_RECEIVED_SIM_MSG_IND                 0x44

/** \def MAL_SMS_MEMORY_CAPACITY_EXC_IND
 *  \brief SMS memory capacity exceeded indication
 *
 *   The SMS memory capacity exceeded indication event is recieved when memory
 *   capacity exceeded flag is set at Modem.
 */
#define MAL_SMS_MEMORY_CAPACITY_EXC_IND              0x1D

/** \def MAL_SMS_RECEIVE_MESSAGE_RESP
 *  \brief SMS receive message response
 *
 *   The SMS receive message response event is recieved for memory status
 *   update request and for SMS reception activate request.
 *   SMS Server/RIL handling this event needs to be check
 */
#define MAL_SMS_RECEIVE_MESSAGE_RESP                 0x42

/** \def MAL_CB_ROUTING_RESP
 *  \brief CB Response Message
 *
 *   The SMS CB response event is received after subscribing/unsubscribing/update CB Service.
 */
#define MAL_CB_ROUTING_RESP                          0x35

/** \def MAL_CB_ROUTING_IND
 *  \brief CB SMS Received indication
 *
 *   The CB SMS receive indication event is recieved when a new
 *   CB SMS is received.
 */
#define MAL_CB_ROUTING_IND                           0x36

/** \def MAL_CB_SIM_ROUTING_IND
 *  \brief CB SMS Received SIM indication
 *
 *   The CB SMS receive SIM indication event is recieved when a new
 *   CB SMS is received to be stored in SIM.
 */
#define MAL_CB_SIM_ROUTING_IND                       0x37

/** \def MAL_SMS_SERVER_NOT_REACHABLE_RESP
 *  \brief  SMS Server not reachable response
 *
 *  The Modem SMS Server not reachable response is recieved request can
 *  not be delivered to Modem SMS Server.
 */
#define MAL_SMS_SERVER_NOT_REACHABLE_RESP            0x14

/** \def MAL_SMS_ROUTE_INFO_SET_RESP
 *  \brief  SMS prefered route setting request response
 *
 *  This event is received after setting the prefered route info for
 *  the SMS server
 */
#define MAL_SMS_ROUTE_INFO_SET_RESP                  0x31

/** \def MAL_SMS_ROUTE_INFO_GET_RESP
 *  \brief  SMS get route request response
 *
 *  This event is received after sending the request to modem to get
 *  the prefered SMS route information.
 */
#define MAL_SMS_ROUTE_INFO_GET_RESP                  0x33


/** \typedef typedef uint8_t  MAL_SMS_TP_MessageReference_t
 *  \brief  Message Reference (TP-MR)
 *
 * This is the Message Reference (TP-MR) of an SMS-SUBMIT
 * or an SMS-COMMAND submitted to the SC by the MS.
 */
typedef uint8_t MAL_SMS_TP_MessageReference_t;

/** \typedef typedef uint16_t  MAL_SMS_RP_ErrorCause_t
 * \brief   RP Error Cause
 *
 * RP Error Cause to be sent with a Deliver  Report.
 *
 */
typedef uint16_t MAL_SMS_RP_ErrorCause_t;

/** \typedef uint16_t MAL_CBS_MessageIdentifier_t
* \brief    Specifies MessageIdentifier. The Cell Broadcast message identifier in the range specified in document 3G TS 23.041[1].
*
* This type specifies MessageIdentifier.
*
*/
typedef uint16_t MAL_CBS_MessageIdentifier_t;

/** \typedef uint16_t MAL_CBS_DataCodingScheme_t
* \brief    Specifies DataCodingScheme. The Cell Broadcast Data Coding Scheme as specified in document 3G TS 23.041[1].
*
* This type specifies DataCodingScheme.
*
*/
typedef uint8_t MAL_CBS_DataCodingScheme_t;

/**
 * \enum SMS_CBS_req_type_t
 * \brief SMS return value
 *
 * This enum has values return to RIL
 */
typedef enum {
    MAL_SMS_CBS_SUCCESS = 0,        /**< Return success */
    MAL_SMS_CBS_FAIL = 1,           /**< Return failure */
    MAL_SMS_CBS_R_GENERAL_ERROR = 2,/**< Return general error */
} SMS_CBS_req_type_t;

/**
 * \enum MAL_SMS_CBS_Error_t
 * \brief Contains response information related codes.
 *
 * This enum contains response information related codes for Successful/Unsuccessful Operation.
 */
typedef enum {
    /* SMS_CAUSE   */
    MAL_SMS_OK                                          = 0x3000,  /**< OK Received */
    MAL_SMS_ERR_ROUTING_RELEASED                        = 0x3001,  /**< Routng Relesed */
    MAL_SMS_ERR_INVALID_PARAMETER                       = 0x3002,  /**< Invalid Parameter */
    MAL_SMS_ERR_DEVICE_FAILURE                          = 0x3003,  /**< Device Failure */
    MAL_SMS_ERR_PP_RESERVED                             = 0x3004,  /**< PP Reserved */
    MAL_SMS_ERR_ROUTE_NOT_AVAILABLE                     = 0x3005,  /**< Route Not Available */
    MAL_SMS_ERR_ROUTE_NOT_ALLOWED                       = 0x3006,  /**< Route Not Allowed */
    MAL_SMS_ERR_SERVICE_RESERVED                        = 0x3007,  /**< Service Reserved */
    MAL_SMS_ERR_INVALID_LOCATION                        = 0x3008,  /**< Invalid Location */
    MAL_SMS_ERR_NO_NETW_RESPONSE                        = 0x300B,  /**< No Network Response */
    MAL_SMS_ERR_DEST_ADDR_FDN_RESTRICTED                = 0x300C,  /**< Destination ADDR FDN Restricted */
    MAL_SMS_ERR_SMSC_ADDR_FDN_RESTRICTED                = 0x300D,  /**< SMSC ADDR FDN Restricted */
    MAL_SMS_ERR_RESEND_ALREADY_DONE                     = 0x300E,  /**< Resend Already Done */
    MAL_SMS_ERR_SMSC_ADDR_NOT_AVAILABLE                 = 0x300F,  /**< SMSC ADDR Not Available */
    MAL_SMS_ERR_ROUTING_FAILED                          = 0x3010,  /**< Routing Failed */
    MAL_SMS_ERR_CS_INACTIVE                             = 0x3011,  /**< CS Inactive */
    MAL_SMS_ERR_SAT_MO_CONTROL_MODIFIED                 = 0x3012,  /**< SAT MO Control Modified */
    MAL_SMS_ERR_SAT_MO_CONTROL_REJECT                   = 0x3013,  /**< SAT MO Control Reject */
    MAL_SMS_ERR_TRACFONE_FAILED                         = 0x3014,  /**< TRACFONE Failed */
    MAL_SMS_ERR_SENDING_ONGOING                         = 0x3015,  /**< Sending Ongoing */
    MAL_SMS_ERR_SERVER_NOT_READY                        = 0x3016,  /**< Server Not Ready */
    MAL_SMS_ERR_NO_TRANSACTION                          = 0x3017,  /**< No Transaction */
    MAL_SMS_ERR_REJECTED                                = 0x3018,  /**< Rejected */
    MAL_SMS_ERR_INVALID_SUBSCRIPTION_NR                 = 0x3019,  /**< Invalid Subscription Number */
    MAL_SMS_ERR_RECEPTION_FAILED                        = 0x301A,  /**< Reception Failed */
    MAL_SMS_ERR_RC_REJECTED                             = 0x301B,  /**< RC Rejected */
    MAL_SMS_ERR_ALL_SUBSCRIPTIONS_ALLOCATED             = 0x301C,  /**< All Subscription Allocated */
    MAL_SMS_ERR_SUBJECT_COUNT_OVERFLOW                  = 0x301D,  /**< Subject Count Overflow */
    MAL_SMS_ERR_DCS_COUNT_OVERFLOW                      = 0x301E,  /**< DCS Count Overflow */

    /* SMS_EXT_CAUSE */
    MAL_SMS_EXT_ERR_UNASSIGNED_NUMBER                   = 0x4001,  /**< Unassigned Number */
    MAL_SMS_EXT_ERR_OPER_DETERMINED_BARR                = 0x4008,  /**< Operation Determined BARR */
    MAL_SMS_EXT_ERR_CALL_BARRED                         = 0x400A,  /**< Called Barred */
    MAL_SMS_EXT_ERR_RESERVED                            = 0x400B,  /**< Reserved */
    MAL_SMS_EXT_ERR_MSG_TRANSFER_REJ                    = 0x4015,  /**< Message Transfer Rejected */
    MAL_SMS_EXT_ERR_MEMORY_CAPACITY_EXC                 = 0x4016,  /**< Memory Capacity Exceeded */
    MAL_SMS_EXT_ERR_DEST_OUT_OF_ORDER                   = 0x401B,  /**< Destination Out Of Order*/
    MAL_SMS_EXT_ERR_UNDEFINED_SUBSCRIBER                = 0x401C,  /**< Undefined Subscriber*/
    MAL_SMS_EXT_ERR_FACILITY_REJECTED                   = 0x401D,  /**< Facility Rejected*/
    MAL_SMS_EXT_ERR_UNKNOWN_SUBSCRIBER                  = 0x401E,  /**< Unknown Subscriber */
    MAL_SMS_EXT_ERR_NETW_OUT_OF_ORDER                   = 0x4026,  /**< Network Out Of Order */
    MAL_SMS_EXT_ERR_TEMPORARY_FAILURE                   = 0x4029,  /**< Temporary Failure */
    MAL_SMS_EXT_ERR_CONGESTION                          = 0x402A,  /**< Congestion */
    MAL_SMS_EXT_ERR_RESOURCE_UNAVAILABLE                = 0x402F,  /**< Resource Unavailable */
    MAL_SMS_EXT_ERR_REQ_FACILITY_NOT_SUB                = 0x4032,  /**< Requested Facility Not Subscribed */
    MAL_SMS_EXT_ERR_REQ_FACILITY_NOT_IMP                = 0x4045,  /**< Requested Facility Not Implemented */
    MAL_SMS_EXT_ERR_INVALID_REFERENCE                   = 0x4051,  /**< Invalid Reference */
    MAL_SMS_EXT_ERR_INCORRECT_MESSAGE                   = 0x405F,  /**< Incorrect Message */
    MAL_SMS_EXT_ERR_INVALID_MAND_INFO                   = 0x4060,  /**< Invalid MAND Information */
    MAL_SMS_EXT_ERR_INVALID_MSG_TYPE                    = 0x4061,  /**< Invalid Message Type */
    MAL_SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST                = 0x4062,  /**< Message Not COMP With ST */
    MAL_SMS_EXT_ERR_INVALID_INFO_ELEMENT                = 0x4063,  /**< Invalid INFO Element */
    MAL_SMS_EXT_ERR_PROTOCOL_ERROR                      = 0x406F,  /**< Protocol Error */
    MAL_SMS_EXT_ERR_INTERWORKING                        = 0x407F,  /**< Interworking */
    MAL_SMS_EXT_ERR_NO_CAUSE                            = 0x4080,  /**< No Cause */
    MAL_SMS_EXT_ERR_IMSI_UNKNOWN_HLR                    = 0x4082,  /**< IMSI Unknown HLR */
    MAL_SMS_EXT_ERR_ILLEGAL_MS                          = 0x4083,  /**< Illegal MS*/
    MAL_SMS_EXT_ERR_IMSI_UNKNOWN_VLR                    = 0x4084,  /**< IMSI Unknown VLR */
    MAL_SMS_EXT_ERR_IMEI_NOT_ACCEPTED                   = 0x4085,  /**< IMEI Not Accepted */
    MAL_SMS_EXT_ERR_ILLEGAL_ME                          = 0x4086,  /**< Illegal ME */
    MAL_SMS_EXT_ERR_PLMN_NOT_ALLOWED                    = 0x408B,  /**< PLMN Not Allowed */
    MAL_SMS_EXT_ERR_LA_NOT_ALLOWED                      = 0x408C,  /**< LA Not Allowed */
    MAL_SMS_EXT_ERR_ROAM_NOT_ALLOWED_LA                 = 0x408D,  /**< ROAM Not Allowed LA */
    MAL_SMS_EXT_ERR_NO_SUITABLE_CELLS_LA                = 0x408F,  /**< No Suitable Cells LA */
    MAL_SMS_EXT_ERR_NETWORK_FAILURE                     = 0x4091,  /**< Network Failure */
    MAL_SMS_EXT_ERR_MAC_FAILURE                         = 0x4094,  /**< MAC failure */
    MAL_SMS_EXT_ERR_SYNC_FAILURE                        = 0x4095,  /**< Sync Failure */
    MAL_SMS_EXT_ERR_LOW_LAYER_CONGESTION                = 0x4096,  /**< Low Layer Congestion */
    MAL_SMS_EXT_ERR_AUTH_UNACCEPTABLE                   = 0x4097,  /**< Auth Unacceptable */
    MAL_SMS_EXT_ERR_SERV_OPT_NOT_SUPPORTED              = 0x40A0,  /**< SERV Option Not Supported */
    MAL_SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED             = 0x40A1,  /**< SERV Option Not Subscribed */
    MAL_SMS_EXT_ERR_SERV_OPT_TEMP_OUT_OF_ORDER          = 0x40A2,  /**< SERV Option Temprary Out Of Order */
    MAL_SMS_EXT_ERR_CALL_CANNOT_BE_IDENTIFIED           = 0x40A6,  /**< Call Can Not Be Identified */
    MAL_SMS_EXT_ERR_SEMANTICALLY_INCORR_MSG             = 0x40DF,  /**< Semantically Incorrect Message */
    MAL_SMS_EXT_ERR_LOW_LAYER_INVALID_MAND_INFO         = 0x40E0,  /**< Low Layer Invalid MAND Information */
    MAL_SMS_EXT_ERR_LOW_LAYER_INVALID_MSG_TYPE          = 0x40E1,  /**< Low Layer Invalid Message Type */
    MAL_SMS_EXT_ERR_LOW_LAYER_MSG_TYPE_NOT_COMP_WITH_ST = 0x40E2,  /**< Low Layer Message Type Not COMP with ST */
    MAL_SMS_EXT_ERR_LOW_LAYER_INVALID_INFO_ELEMENT      = 0x40E3,  /**< Low Layer Invalid Information Element */
    MAL_SMS_EXT_ERR_CONDITIONAL_IE_ERROR                = 0x40E4,  /**< Conditional IE Error */
    MAL_SMS_EXT_ERR_LOW_LAYER_MSG_NOT_COMP_WITH_ST      = 0x40E5,  /**< Low Layer Message Not COMP with ST */
    MAL_SMS_EXT_ERR_CS_BARRED                           = 0x40E8,  /**< CS Barred */
    MAL_SMS_EXT_ERR_LOW_LAYER_PROTOCOL_ERROR            = 0x40EF   /**< Low Layer Protocol Error */
} MAL_SMS_CBS_Error_t;

/**
 * \struct MAL_SMS_PackedAddress_t
 * \brief Contains Service Center Address information.
 *
 *  Service Center Address information. Address presented in packed format.
 */
typedef struct {
    uint8_t  Length; /**< The number of useful semi-octets in the AddressValue */
    uint8_t  TypeOfAddress; /**< Type of address as defined in the standards */
    uint8_t  AddressValue[MAL_SMS_MAX_PACKED_ADDRESS_LENGTH]; /**< The address value in packed format */
} MAL_SMS_PackedAddress_t;

/**
 * \struct MAL_SMS_TPDU_t
 * \brief Contains actual TPDU information.
 *
 * Contains the actual TPDU information. This information depends on which TPDU type that is used (e.g. SMS-SUBMIT, SMS-DELIVER).
 */
typedef struct {
    uint8_t  Length; /**< Indicates the length of the TPDU data */
    uint8_t  Data[MAL_SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH]; /**< The array that holds the TPDU data */
} MAL_SMS_TPDU_t;

/**
 * \struct MAL_SMS_SMSC_Address_TPDU_t
 * \brief Contains Service Center packed address and SMS TPDU.
 *
 * Encapsulate the Service Center packed address and MAL_SMS_TPDU_t in one type.
 */
typedef struct {
    MAL_SMS_PackedAddress_t  ServiceCenterAddress; /**< The packed address of the Service Center of the Short Message */
    MAL_SMS_TPDU_t           TPDU; /**< TThe Short Message TPDU */
} MAL_SMS_SMSC_Address_TPDU_t;

/**
 * \struct sms_message_send_resp_t
 * \brief SMS message send response
 *
 *  This structure has message reference and error code
 */
typedef struct {
    MAL_SMS_TP_MessageReference_t  *msg_ref;  /**< TP-Message-Reference.
                                              * Valid if SM sent successful
                                              * or when SM sent failed with
                                              * external cause only */
    MAL_SMS_CBS_Error_t            *err_type; /**< Error Code */

    /** \todo
        report TBD : For now not used SMS Send SUBMIT Report.
        NULL if Submit report is absent
    */
}   sms_message_send_resp_t;

/**
 * \struct cbs_routing_resp_t
 * \brief CB SMS subscription response
 *
 *  This structure holds the Subscription number and error code
 */
typedef struct {
    MAL_SMS_CBS_Error_t *Error_p;              /**< Error Code */
    uint8_t              SubscriptionNumber_p;  /**< Subscription number : Only applicable for SET; For Remove: NULL*/
} cbs_routing_resp_t;

/**
 * \struct cbs_header_t
 * \brief CB Header
 *
 *  This structure holds the CB message header
 */
typedef struct {
    uint16_t serial_number;
    uint16_t message_id;
    uint8_t pages;
    uint8_t dcs;
} cbm_header_t;

/**
 * \struct cbs_message_t
 * \brief CB SMS data
 *
 *  This structure holds the CB message
 */
typedef struct {
    cbm_header_t header;
    uint8_t useful_data_length;
    uint8_t data[MAL_CBS_PAGE_MAX_LENGTH];
} cbm_message_t;

/**
 * \struct cbs_routing_ind_t
 * \brief CB SMS data
 *
 *  This structure holds the entire CB message indication
 */
typedef struct {
    uint8_t number_of_messages;
    cbm_message_t messages[1];
} cbs_routing_ind_t;


/**
 * \enum mal_sms_route_info_t
 * \brief SMS route route service preference selection
 *
 * This enum defines the SMS route
 */
typedef enum {
    MAL_SMS_ROUTE_PS    = 0, /**< PS route is used  */
    MAL_SMS_ROUTE_CS    = 1, /**< CS route is used  */
    MAL_SMS_ROUTE_PS_CS = 2, /**< PS is prefered, use CS if PS not available */
    MAL_SMS_ROUTE_CS_PS = 3  /**< CS is prefered, use PS if CS not available */
} mal_sms_route_info_t;

/**
 * \struct sms_routing_read_resp_t
 * \brief CB SMS routing info read response
 *
 *  This structure holds the prefered route currently selected and error code
 */
typedef struct {
    MAL_SMS_CBS_Error_t *Error_p;      /**< Error Code */
    mal_sms_route_info_t  route_info;  /**< Prefered SMS route currently selected */
} sms_routing_read_resp_t;



/** \typedef typedef void (*mal_sms_cbs_event_t)(int event_id, void *data)
 *  \brief Event notification to SMS server
 *
 *  Callback functions for handling SMS/CBS response and indication events
 *
 *  \param [in] eventID
 *  \param [in] data pointer
 *  \par
 *  Following is the detail on data based on the event type:
 *  \arg MAL_SMS_MESSAGE_SEND_RESP \n sms_message_send_resp_t* sms_send_resp
 *  \par
 *  \arg MAL_SMS_RECEIVED_MSG_IND \n MAL_SMS_SMSC_Address_TPDU_t* sms_tpdu
 *  \par
 *  \arg MAL_SMS_RECEIVED_SIM_MSG_IND \n SMS_RECEIVED_SIM_MSG_IND_t* sim_sms_tpdu
 *  \par
 *  \arg MAL_SMS_MEMORY_CAPACITY_EXC_IND \n NULL
 *  \par
 *  \arg MAL_SMS_RECEIVED_MSG_REPORT_RESP \n MAL_SMS_CBS_Error_t* err_type:
 *                                        \n Possible values are
 *                                        \n MAL_SMS_OK  |
 *                                        \n MAL_SMS_ERR_NO_TRANSACTION  |
 *                                        \n MAL_SMS_ERR_INVALID_PARAMETER |
 *                                        \n MAL_SMS_ERR_RECEPTION_FAILED
 *  \par
 *  \arg MAL_SMS_RECEIVE_MESSAGE_RESP \n  MAL_SMS_CBS_Error_t* err_type:
 *                                    \n Possible values are
 *                                    \n MAL_SMS_OK  |
 *                                    \n MAL_SMS_ERR_DEVICE_FAILURE |
 *                                    \n MAL_SMS_ERR_CS_INACTIVE |
 *                                    \n MAL_SMS_ERR_SERVER_NOT_READY
 *  \par
 *  \arg MAL_CB_ROUTING_RESP \n cbs_routing_resp_t* cbs_subs_resp
 *  \par
 *  \arg MAL_CB_ROUTING_IND  \n cbs_routing_ind_t* cbs_page
 *  \par
 *  \arg MAL_CB_SIM_ROUTING_IND  \n cbs_routing_ind_t* sim_cbs_page
 *  \par
 *  \arg MAL_SMS_SERVER_NOT_REACHABLE_RESP  \n NULL
 *  \par
 *  \arg MAL_SMS_ROUTE_INFO_SET_RESP \n  MAL_SMS_CBS_Error_t* err_type:
 *                                    \n Possible values are
 *                                    \n MAL_SMS_OK  |
 *                                    \n MAL_SMS_ERR_INVALID_PARAMETER |
 *                                    \n MAL_SMS_ERR_DEVICE_FAILURE |
 *                                    \n MAL_SMS_ERR_SERVER_NOT_READY |
 *                                    \n MAL_SMS_ERR_CS_INACTIVE
 *  \par
 *  \arg MAL_SMS_ROUTE_INFO_GET_RESP \n sms_routing_read_resp_t* sms_route_read
 *
 *  \par
 *  \return void
 */
typedef void (*mal_sms_cbs_event_t)(int event_id, void *data);


/** \fn int mal_sms_cbs_init(int *fd_sms)
 *  \brief MAL_API - Startup function for using SMS-CBS MAL
 *
 *  Initializes SMS-CBS MAL data structures and state,
 *  Creates Phonet Socket Session.
 *  Creates netlink Socket Session.
 *  \param [out] fd_sms points to file descriptor of SMS-CBS socket
 *  \param [out] fd_netlnk points to file descriptor of netlink socket
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_cbs_init(int *fd_sms, int *fd_netlnk);


/** \fn int mal_sms_cbs_register_callback(mal_sms_cbs_event_t event_cb)
 *  \brief MAL_API - Register Callback.
 *
 *  Registers callback with MAL SMS-CBS library
 *  \param [in] event_cb  Callback function pointer
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_cbs_register_callback(mal_sms_cbs_event_t event_cb);


/** \fn int mal_sms_cbs_config(void)
 *  \brief MAL-API - Configures SMS-CBS MAL Parameters.
 *
 *  Reads default config parameters or sets specific config parameters
 *  This function should be called only once and
 *  immediately after mal_sms_cbs_init().
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_cbs_config(void);


/** \fn mal_sms_cbs_deinit
 *  \brief MAL-API - SMS server call this API to deinitialize SMS-CBS library
 *
 *  De-Initializes SMS-CBS MAL data structures and state,
 *  deletes Phonet Socket Session.
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_cbs_deinit();


/** \fn void mal_sms_cbs_response_handler(void)
 *  \brief MAL-API - SMS-CBS Response and Indication Handler .
 *
 *  This function sends Responses and Indications to SMS server
 *  \return void
 */
void mal_sms_cbs_response_handler();


/** \fn int mal_sms_request_send_tpdu(MAL_SMS_SMSC_Address_TPDU_t* sms_tpdu,
 *                              uint8_t SMS_Route,
 *                              uint8_t More_SMS_toSend)
 *  \brief MAL_API - SMS send request
 *
 *  This function is used to send SMS.
 *  Corresponding RIL command is: RIL_REQUEST_SEND_SMS
 *  \param [in] sms_tpdu         TPDU to be sent to SMS server
 *  \param [in] SMS_Route        0 for default, 1 for CS path, 2 for PS path
 *  \param [in] More_SMS_toSend  TRUE-> more SMS to send
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_request_send_tpdu(MAL_SMS_SMSC_Address_TPDU_t *sms_tpdu,
                              uint8_t SMS_Route,
                              uint8_t More_SMS_toSend);


/** \fn int mal_sms_request_send_deliver_report(MAL_SMS_RP_ErrorCause_t RP_ERR_Cause,
 *                                       MAL_SMS_SMSC_Address_TPDU_t* sms_tpdu)
 *  \brief MAL_API - SMS send delivery report
 *
 *  This function is used to acknowledge successful or failed receipt of SMS.
 *  Corresponding RIL command is: RIL_REQUEST_SMS_ACKNOWLEDGE
 *  \param [in] RP_ERR_Cause  Error cause
 *                                        \n Possible values are
 *                                        \n SMS_OK  If MT SM reception was successful |
 *                                        \n SMS_EXT_ERR_MEMORY_CAPACITY_EXC  If the received SM could
 *                                                                            not be stored due to
 *                                                                            memory full condition |
 *                                        \n SMS_EXT_ERR_PROTOCOL_ERROR  When the UE receives class 2
 *                                                                       SM and [U]SIM is full but there
 *                                                                       is other storage available at the
 *                                                                       UE
 *
 *  \param [in] sms_tpdu      TPDU to be sent to SMS server
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_request_send_deliver_report(MAL_SMS_RP_ErrorCause_t RP_ERR_Cause,
                                        MAL_SMS_SMSC_Address_TPDU_t *sms_tpdu);


/** \fn int mal_sms_request_set_memory_status(uint8_t memory_status )
 *  \brief MAL_API - SMS set memory status
 *
 *  This function is used to set the memory status.
 *  Corresponding RIL command is: RIL_REQUEST_REPORT_SMS_MEMORY_STATUS
 *  \param [in] memory_status   : 1 Memory is available
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_request_set_memory_status(uint8_t memory_status);


/** \fn int mal_cbs_request_routing_set(uint8_t SubscribetoAll, uint8_t SubscriptionNumber, uint8_t SIM_Subscription,
 *                              MAL_CBS_MessageIdentifier_t*  CBMI_List, uint8_t CBMI_List_Length ,
 *                              MAL_CBS_DataCodingScheme_t*  DCS_List, uint8_t DCS_List_Length);
 *  \brief MAL_API - Set/Update GSM/WCDMA Cell Broadcast SMS config
 *
 *  This Api is used to set/update GSM/WCDMA Cell Broadcast SMS config
 *  Corresponding RIL command is: RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG/??ACTIVATE(Select on)?? TBD
 * \param [in] SubscribetoAll               :  0 subscribe to CBS_MessageIdentifier list and CBS_DataCodingScheme list,
 *                                             1 subscribe all broadcast pages (SubscriptionNumber, CBS_MessageIdentifier   *                                             and CBS_DataCodingScheme list ignored)
 * \param [in] SubscriptionNumber           :  0 New subscription,
 *                                             1 To update existing subscription
 * \param [in] SIM_Subscription             :  0 subscription is not for SIM,
 *                                             1 subscription is for SIM
 * \param [in] CBMI_List                    :  CBMI list to subscribe to
 * \param [in] CBMI_List_Length             :  Size of CBID list (no. of 16bit CBID's)
 * \param [in] DCS_List                     :  DCs list to subscribe to
 * \param [in] DCS_List_Length              :  Size of DCS list (no. of 8bit DCS's)
 * \return SMS_CBS_req_type_t return value
 */


int mal_cbs_request_routing_set(uint8_t SubscribetoAll, uint8_t SubscriptionNumber, uint8_t SIM_Subscription, MAL_CBS_MessageIdentifier_t  *CBMI_List, uint8_t CBMI_List_Length, MAL_CBS_DataCodingScheme_t  *DCS_List, uint8_t DCS_List_Length);

/** \fn int mal_cbs_request_routing_remove(uint8_t SubscriptionNumber);
 *  \brief MAL_API - Removes Cell Broadcast subscription
 *
 *  This Api is used to remove Cell Broadcast subscription
 *  Corresponding RIL command is: RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG/??ACTIVATE(Select off)?? TBD
 * \param [in] SubscriptionNumber   :   Subscription to remove
 * \return SMS_CBS_req_type_t return value
 */

int mal_cbs_request_routing_remove(uint8_t SubscriptionNumber);

/** \fn mal_sms_set_route(mal_sms_route_info_t route_info)
 *
 *  \brief MAL_API - SMS route selection
 *
 *  This routine is used to set the prefered SMS route .
 *  Expected event type in case of successful invocation of this routine is
 *  MAL_SMS_ROUTE_INFO_SET_RESP
 *
 *  \param [in] route_info          : prefered route info to be set
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_set_route(mal_sms_route_info_t route_info);


/** \fn mal_sms_get_route(void)
 *
 *  \brief MAL_API - Get the current SMS route
 *
 *  This routine is used to get the current prefered SMS route selected
 *  Expected event type in case of successful invocation of this routine is
 *  MAL_SMS_ROUTE_INFO_GET_RESP
 *
 *  \return \ref SMS_CBS_req_type_t return value
 */
int mal_sms_get_route(void);

/**
 *  \fn int32_t mal_sms_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for sms module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_sms_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_sms_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for sms module. This is a synchronous call.
 *   \param[out] debug level for sms module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_sms_request_get_debug_level(uint8_t *level);

#endif //__MAL_SMS_H

