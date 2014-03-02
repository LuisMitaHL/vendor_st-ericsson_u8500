/**
 * \file mal_ss.h
 * \brief Includes interface headers for MAL SS API
 *
 * \n Copyright (C) ST-Ericsson 2010. All rights reserved.
 * \n This code is ST-Ericsson proprietary and confidential.
 * \n Any use of the code for whatever purpose is subject to
 * \n specific written permission of ST-Ericsson SA.
 * \n
 * \n This contains the description of the Interface header file for MAL Supplementary Services support
 * \n Author: Jayarami Reddy <jayarami.reddy@stericsson.com>
 * \n         Parthasarathy Nagarajan <parthasarathy.nagarajan@stericsson.com> for ST-Ericsson
 * \n Version: 0.1
 */

/** \defgroup MAL_SS  MAL SS API
 *  \par
 *  MAL SS Library manages the Supplementary Services modem server ISI messages
 */
#ifndef _SS_LIB_H
#define _SS_LIB_H

#include <stdint.h>

/** \def SS_LIB_VERSION
 *  \brief Library Version for SS Lib
 *
 */
#define SS_LIB_VERSION            "SS Lib V 0.1.r14"

/* ----------------------------------------------------------------------- */
/* Event ID's                                                              */
/* ----------------------------------------------------------------------- */

/** \def MAL_SS_ON_USSD
 *  \brief USSD Indication to upper layer
 *
 */
#define MAL_SS_ON_USSD                               1006

/** \def MAL_SS_DETAILED_FAIL_CAUSE
 *  \brief SS error information to upper layer
 *
 */
#define MAL_SS_DETAILED_FAIL_CAUSE                   1007

/** \def MAL_SS_SERVICE_COMPLETED_RESP
 *  \brief Modem response for supplementary service request.
 *
 */
#define MAL_SS_SERVICE_COMPLETED_RESP                0x01

/** \def MAL_SS_SERVICE_FAILED_RESP
 *  \brief Failure modem response for supplementary service request.
 *
 */
#define MAL_SS_SERVICE_FAILED_RESP                   0x02

/** \def MAL_SS_GSM_USSD_SEND_RESP
 *  \brief Modem response for MO USSD Service request.
 *
 */
#define MAL_SS_GSM_USSD_SEND_RESP                    0x05

/** \def MAL_SS_SERVICE_NOT_SUPPORTED_RESP
 *  \brief Modem response for supplementary service not supported.
 *
 */
#define MAL_SS_SERVICE_NOT_SUPPORTED_RESP            0x03

/** \def MAL_SS_STATUS_IND
 *  \brief supplementary service indication from modem during supplementary service operations.
 *
 */
#define MAL_SS_STATUS_IND                            0x09

/** \def MAL_SS_GSM_USSD_RECEIVE_IND
 *  \brief USSD receive indication message from modem
 *  \n (for Network initiated USSD scenario)
 *
 */
#define MAL_SS_GSM_USSD_RECEIVE_IND                  0x06


/* ----------------------------------------------------------------------- */
/* SS_ERROR_TYPE_GSM_PASSWORD error values                                                                               */
/* ----------------------------------------------------------------------- */

/** \def MAL_SS_GSM_BAD_PASSWORD
 *  \brief Used only when network return error diagnostic parameter 'new Password Mismatch'
 *
 */
#define MAL_SS_GSM_BAD_PASSWORD     0x01


/** \def MAL_SS_GSM_BAD_PASSWORD_FORMAT
 *  \brief Used only when network return error diagnostic parameter 'invalid password format'
 *
 */
#define MAL_SS_GSM_BAD_PASSWORD_FORMAT  0x02


/* ----------------------------------------------------------------------- */
/* SS_ERROR_TYPE_GSM_MSG error values                                                                               */
/* ----------------------------------------------------------------------- */

/** \def MAL_SS_GSM_MSG_INCORRECT
 *  \brief Indicates that network response decoding fails or there is a conflict message streams between
 *   \n multiple MO and MT SS operations.
 *
 */
#define MAL_SS_GSM_MSG_INCORRECT         0x01


/* ----------------------------------------------------------------------- */
/* SS_ERROR_TYPE_MISC error values                                                                                             */
/* ----------------------------------------------------------------------- */

/** \def MAL_SS_SERVICE_BUSY
 *  \brief Indicates that procedure is rejected because SS server is busy due to ongoing SS transaction
 *
 */
#define MAL_SS_SERVICE_BUSY     0x00

/** \def MAL_SS_GSM_DATA_ERROR
 *  \brief Indicates that network has initiated Forward check SS indication procedure, see 3GPP TS 24.010
 *
 */
#define MAL_SS_GSM_DATA_ERROR   0x02

/** \def MAL_SS_GSM_REQUEST_CANCELLED
 *  \brief Indicates that procedure is cancelled.
 *
 */
#define MAL_SS_GSM_REQUEST_CANCELLED    0x04

/** \def MAL_SS_GSM_MM_ERROR
 *  \brief Indicates that procedure is rejected because lower layer failure occurs during active connection
 *
 */
#define MAL_SS_GSM_MM_ERROR     0x0B

/** \def MAL_SS_GSM_SERVICE_NOT_ON_FDN_LIST
 *  \brief Indicates that procedure is rejected because SS operation MMI string is not in FDN list.
 *
 */
#define MAL_SS_GSM_SERVICE_NOT_ON_FDN_LIST  0x0C

/** \def MAL_SS_CS_INACTIVE
 *  \brief Indicates that procedure is rejected because CS is inactive.
 *
 */
#define MAL_SS_CS_INACTIVE  0x0F

/** \def MAL_SS_GSM_SS_NOT_AVAILABLE
 *  \brief Indicates that procedure is rejected because serving cell does not support CS services.
 *
 */
#define MAL_SS_GSM_SS_NOT_AVAILABLE     0x12

/** \def MAL_SS_RESOURCE_CONTROL_DENIED
 *  \brief Indicates that procedure is rejected by resource controller.
 *
 */
#define MAL_SS_RESOURCE_CONTROL_DENIED  0x13

/** \def MAL_SS_RESOURCE_CONTROL_FAILURE
 *  \brief Indicates that procedure is rejected because resource control request syntax is not correct.
 *
 */
#define MAL_SS_RESOURCE_CONTROL_FAILURE     0x14

/** \def MAL_SS_RESOURCE_CONTROL_CONF_FAIL
 *  \brief Indicates that procedure is rejected because resource control configuration is missing.
 *
 */
#define MAL_SS_RESOURCE_CONTROL_CONF_FAIL   0x15

/** \def MAL_SS_GSM_USSD_BUSY
 *  \brief Indicates that procedure is rejected because SS server is busy due to ongoing USSD transaction.
 *
 */
#define MAL_SS_GSM_USSD_BUSY    0x48

/** \def MAL_SS_SERVICE_REQUEST_RELEASED
 *  \brief Indicates that SS service request is released.
 *
 */
#define MAL_SS_SERVICE_REQUEST_RELEASED     0x16

/** \def MAL_SS_SERVICE_UNKNOWN_ERROR
 *  \brief Unknown SS Error, this error code is reported by MAL client, this error value doesnot collide with 3GPP spec.
 *
 */
#define MAL_SS_SERVICE_UNKNOWN_ERROR     0x17

/** \def MAL_SS_SERVICE_NOT_SUPPORTED
 *  \brief SS Service not supported, this error code is reported by MAL client, this error value doesnot collide with 3GPP spec.
 *
 **/
#define MAL_SS_SERVICE_NOT_SUPPORTED    0x18


/**
 * \enum  mal_ss_clir_state
 * \brief This enum has values of CLIR service status in the network.
 */
typedef enum {
    CLIR_NOT_PROVISIONED = 0x0,
    CLIR_PROVISIONED_PERMANENT,
    CLIR_UNKNOWN,
    CLIR_TEMPARORY_MODE_RESTRICTED,
    CLIR_TEMPARORY_MODE_ALLOWED,
} mal_ss_clir_state;

/**
 * \enum  mal_ss_service_op
 * \brief This enum has values of ss service operation.
 */
typedef enum {
    MAL_SS_DEACTIVATION = 0x0,
    MAL_SS_ACTIVATION,
} mal_ss_service_op;

/**
 * \enum  mal_ss_ussd_type
 * \brief This enum has values of ss ussd type.
 */
typedef enum {
    MAL_SS_GSM_USSD_REQUEST = 0x1, /**< MT USSD request received from network */
    MAL_SS_GSM_USSD_NOTIFY  = 0x2, /**< USSD notification is received from network */
    MAL_SS_GSM_USSD_COMMAND = 0x3, /**< USSD command is received from network, further user action needed */
    MAL_SS_GSM_USSD_END     = 0x4  /**< USSD terminated by network */
} mal_ss_ussd_type;

/**
 * \enum  mal_ss_error_type
 * \brief This enum has values of ss error type
 */
typedef enum {
    SS_ERROR_NONE               = 0,            /**< None */
    SS_ERROR_TYPE_GSM_PASSWORD  = 1,            /**< SS GSM PASSWORD error value as mentioned
                                                                                             in SS_ERROR_TYPE_GSM_PASSWORD error values */
    SS_ERROR_TYPE_GSM           = 2,            /**< SS GSM error value as per 3GPP TS 24.080 sec 4.3.2 */
    SS_ERROR_TYPE_GSM_MSG       = 3,            /**< SS GSM MSG error value as mentioned in SS_ERROR_TYPE_GSM_MSG error values */
    SS_ERROR_TYPE_MISC          = 4,            /**< SS MISC error value as mentioned in SS_ERROR_TYPE_MISC error values */
    SS_ERROR_TYPE_MM            = 5             /**< MM cause value as per 3GPP TS 24.008 sec 10.5.3.6 */
} mal_ss_error_type;

/**
 * \enum  mal_ss_serv_class_info
 * \brief This enum has values of various serivce classes supported
 */
typedef enum {
    GSM_VOICE = 1,                          /**< Telephony */
    GSM_BEARER = 2,                         /**< All Bearer */
    GSM_FACSIMILE = 4,                      /**< Facsimile services */
    GSM_VOICE_FACSIMILE = 5,                /**< All teleservices except SMS */
    GSM_SMS = 8,                            /**< Short Message Services */
    GSM_FACSIMILE_SMS = 12,                 /**< All data teleservices */
    GSM_VOICE_FACSIMILE_SMS = 13,           /**< All teleservices */
    GSM_DATA_CIRCUIT_SYNC = 16,             /**< All data circuit sync */
    GSM_DATA_CIRCUIT_ASYNC = 32,            /**< All data circuit async */
    GSM_PACKET_ACCESS = 64,                 /**< Dedicated packet access */
    GSM_PAD_ACCESS = 128,                   /**< Dedicated PAD access */
    GSM_DATA_ASYNC_PAD_ACCESS = 160,        /**< All async services */
    GSM_DATA_SYNC_PACKET_ACCESS = 80,       /**< All sync services */
    GSM_DEFAULT = 7,                        /** Supported only in SS Query operations, i.e in
                                                Query Call waiting, Query Call Forwarding, Query Call Barring ONLY */
    GSM_DEFAULT_MMI = 15,
    NONE = 0,                               /**< None */
} mal_ss_serv_class_info;

/**
 * \enum  mal_ss_ussd_status_ind_type
 * \brief This enum has values of various USSD SS serivce status indications
 */
typedef enum {
    MAL_SS_STATUS_REQUEST_USSD_STOP   = 0x03, /**< SS USSD request stopped/ended */
    MAL_SS_STATUS_REQUEST_USSD_FAILED = 0x04, /**< SS USSD request failed */
    MAL_SS_STATUS_REQUEST_USSD_BUSY   = 0x08, /**< SS USSD request busy */
} mal_ss_ussd_status_ind_type;

/**
 * \struct  mal_ss_error_type_value
 * \brief This structure contains the details of ss failure error type and values.
*/
typedef struct {
    mal_ss_error_type error_type;       /**< SS Error Type \ref mal_ss_error_type */
    int32_t   error_value;              /**< SS Error value as per error type */
} mal_ss_error_type_value;


/**
 * \struct  mal_ss_service_op_class
 * \brief   Structure information containing SS Operation and service class.
 */
typedef struct {
    mal_ss_service_op service_op;       /**< ss service operation \ref  mal_ss_service_op */
    int32_t service_class;              /**<  service class information  */
} mal_ss_service_op_class;

/**
 * \struct  mal_ss_gsm_additional_info
 * \brief   Additional Information structure as part of SS Service response.
 * \brief   This structure contains RETURN RESULT information as part of SS Service response
 */
typedef struct {
    uint8_t     return_result_length;     /**< Length of the Return result buffer */
    uint8_t    *return_result;            /**< First octet contains ASN.1 coded RETURN RESULT component operation code.
                                             * Remaining octets contain ASN.1 coded RETURN RESULT component parameters.
                                             * String is not zero-terminated */
} mal_ss_gsm_additional_info;

/**
 * \struct  mal_ss_call_forward_info
 * \brief   Call Forward Infomation structure.
 * \brief   This structure contains the CallForward information required in \ref mal_ss_query_call_forward and \ref mal_ss_set_call_forward methods.
 */
typedef struct {
    int32_t            status;         /**<
                                            * For \ref mal_ss_query_call_forward request
                                            * status is not used
                                            *
                                            * For \ref mal_ss_set_call_forward request
                                            * status is:
                                            * 0 = disable
                                            * 1 = enable
                                            * 2 = interrogate
                                            * 3 = registeration
                                            * 4 = erasure
                                         */

    int32_t            reason;          /**< from TS 27.007 7.11 "reason" */
    mal_ss_serv_class_info            serviceClass;    /**< Service class Info \ref  mal_ss_serv_class_info */
    int32_t            toa;             /**< "type" from TS 27.007 7.11 */
    char              *number;          /**< "number" from TS 27.007 7.11. May be NULL */
    int32_t            timeSeconds;     /**< for CF no reply only, Valid values are 5 | 10 | 15 | 20 | 25 | 30
                                                                            for other CF reason, set to 0 */
} mal_ss_call_forward_info;

/**
 * \struct  mal_ss_call_fwd_info
 * \brief   Call Forward Infomation structure.
 * \brief   This structure contains the populated Call Forward information from modem which are used by upper layers
 */
typedef struct {
    int32_t                           num;                /**< Index count to array of \ref mal_ss_call_forward_info pointer's
                                                               for each distinct registered phone number */
    mal_ss_call_forward_info         *call_fwd_info;      /**< \ref mal_ss_call_forward_info  */
} mal_ss_call_fwd_info;

/**
 * \struct  mal_ss_ussd_data
 * \brief   USSD Infomation structure.
 * \brief   This structure contains the USSD related information
 */
typedef struct {
    uint8_t     length;     /**< Length of the USSD string */
    uint8_t     dcs;        /**< Data coding scheme as per 3GPP TS 23.038, Chapter 5 'Cell Broadcast Data Coding Scheme' */
    uint8_t     suppress_ussd_on_end;  /**<TRUE, if the current USSD request was SAT initiated */
    uint8_t    *ussd_str;       /**< USSD string in bytes. String is not zero-terminated */
} mal_ss_ussd_data;

/**
 * \struct  mal_ss_ussd_info
 * \brief   USSD Infomation structure to upper layer.
 * \brief   This structure contains the populated USSD information which are used by upper layers
 */
typedef struct {
    int32_t ussd_type_code;        /**< USSD Type codes which is one of the below value,
                                        *      0   no further user action required(network initiated USSD Notify)
                                        *      1   further user action required(network initiated USSD Request)
                                        *      2   Session terminated by network
                                        *      3   other local client (eg, SIM Toolkit) has responded
                                        *      4   Operation not supported
                                        *      5   Network timeout */
    mal_ss_ussd_data ussd_data;     /**< USSD Infomation structure  \ref mal_ss_ussd_data */
    mal_ss_ussd_type ussd_received_type;     /**< USSD type enum \ref mal_ss_ussd_type */
    void                         *ussd_session_id; /**< USSD Session Id,
                                            for MO USSD, Pointer to client_tag
                                            for MT USSD, NULL */
    uint8_t              is_ussd_ue_terminated; /**< Indicates whether USSD is terminated by UE or NW.
                                                                               1    UE terminated the USSD session
                                                                            0   N/W terminated the USSD session
                                                                            Valid only when ussd_received_type is MAL_SS_GSM_USSD_END */
    uint8_t          suppress_ussd_on_end; /**<TRUE, if the current USSD request was SAT initiated */
} mal_ss_ussd_info;

/**
 * \struct  mal_ss_ussd_status_ind_info
 * \brief   USSD serivce status infomation structure.
 * \brief   This structure contains the populated USSD serivce status information which are used by upper layers
 */
typedef struct {
    mal_ss_ussd_status_ind_type   ussd_status_ind_type; /**< USSD serivce status indications \ref mal_ss_ussd_status_ind_type */
    mal_ss_error_type_value      *error_info;      /**< SS failure error type and values \ref mal_ss_error_type_value
                                                  Value is NULL if there is no error information is available */
    void                         *ussd_session_id; /**< USSD Session Id,
                                            for MO USSD, Pointer to client_tag
                                            for MT USSD, NULL */
    uint8_t              is_ussd_ue_terminated; /**< Indicates whether USSD is terminated by UE or NW.
                                                                               1    UE terminated the USSD session
                                                                            0   N/W terminated the USSD session */
} mal_ss_ussd_status_ind_info;
/**
 * \struct  mal_ss_service_response
 * \brief   SS Service response information.
 * \brief   This structure contains SS Service response information from modem
 */
typedef struct {
    uint8_t                           response[2];        /**< SS Service response information, refer event callback function for details. */
    mal_ss_call_fwd_info             *call_forward_info ;  /**< Call Forward Infomation structure See \ref mal_ss_call_fwd_info */
    mal_ss_gsm_additional_info       *additional_info;    /**< RETURN RESULT information as part of SS Service response  See \ref mal_ss_gsm_additional_info */
    mal_ss_error_type_value          *error_info;         /**< SS failure error type and value See \ref mal_ss_error_type_value */

} mal_ss_service_response;

/**
 * \typedef typedef void (*mal_ss_event_cb_t)(int32_t mal_ss_event_id, void* data, int32_t mal_error, void *client_tag);
 * \brief      callback function for any expected or unexpected event information from modem.
 * \param [in] mal_ss_event_id id of solicited/unsolicited event.
 * \param [in] data pointer to parameter values associated with this event
 * \param [in] mal_error error type associated with this event
 * \param [in] client_tag In case of response, pointer to RIL client specific data assosicated with the service request.
 *                                 In case of Indications, NULL
 * \return None
 *
 * \par MAL_SS_ON_USSD
 * \arg Pointer to \ref mal_ss_ussd_info structure
 * \n
 * \par MAL_SS_DETAILED_FAIL_CAUSE
 * \arg Pointer to mal_ss_error_type_value structure
 * \n
 * \par MAL_SS_SERVICE_COMPLETED_RESP
 * \arg For Query call waiting service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] shall contain either 0 for "disabled" or 1 for "enabled"
 * \n   if enabled, response[1] shall contain TS 27.007 service class bit vector of services for which call waiting is enabled.
 * \n   if disabled, response[1] shall contain the requested serivce class value.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For Activate/Deactivate call waiting service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] and response[1] are un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For query CLIP service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] shall contain either 0 for "CLIP not provisioned" or 1 for "CLIP provisioned"
 * \n   response[1] is un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For query CNAP service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] shall contain either 0 for "CNAP not provisioned" or 1 for "CNAP provisioned"
 * \n   response[1] is un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For query COLP service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] shall contain either 0 for "COLP not provisioned" or 1 for "COLP provisioned"
 * \n   response[1] is un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For query COLR service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] shall contain either 0 for "COLR not provisioned" or 1 for "COLR provisioned"
 * \n   response[1] is un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For Query Call forwarding status service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] and response[1] are un-used.
 * \n   Pointer to call_forward_info structure containing call forward Infomation.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info will be NULL.
 * \n
 * \arg For Activate/Deactive Call forwarding service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] and response[1] are un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For Get CLIR service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] shall contain the "m" parameter from TS 27.007 Sec 7.7, See \ref mal_ss_clir_state enumeration.
 * \n   response[1] is un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For Change barring password, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] and response[1] are un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For Query Call barring status service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] shall contain either 0 for "disabled" or 1 for "enabled"
 * \n   if enabled, response[1] shall contain TS 27.007 service class bit vector of services for which call barring is enabled.
 * \n   if disabled, response[1] shall contain the requested serivce class value.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \arg For Activate/Deactive Call barring service request, Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] and response[1] are un-used.
 * \n   if additional information is available, Pointer to additional_info structure else NULL.
 * \n   error_info and call_forward_info will be NULL.
 * \n
 * \par MAL_SS_SERVICE_FAILED_RESP
 * \arg Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] and response[1] are un-used.
 * \n   Pointer to mal_ss_error_type_value structure containing the error information.
 * \n   additional_info and call_forward_info will be NULL.
 * \n
 * \par MAL_SS_GSM_USSD_SEND_RESP
 * \arg NULL
 * \n
 * \par MAL_SS_SERVICE_NOT_SUPPORTED_RESP
 * \arg Pointer to mal_ss_service_response structure. where in,
 * \n   response[0] and response[1] are un-used.
 * \n   Pointer to mal_ss_error_type_value structure with error_type as SS_ERROR_TYPE_MISC and
 *      error_value as MAL_SS_SERVICE_NOT_SUPPORTED.
 * \n   additional_info and call_forward_info will be NULL.
 * \arg
 * \n
 * \par MAL_SS_STATUS_IND
 * \arg Pointer to mal_ss_ussd_status_ind_info structure.
 * \n
 */

typedef    void (*mal_ss_event_cb_t)(int32_t mal_ss_event_id, void *data, int32_t mal_error, void *client_tag);

/**
 *   \fn int32_t mal_ss_init(int32_t *fd_ss);
 *   \brief     This Api is used to initialize the supplementary services library,Opens socket using phonet lib and
 *   \brief     Should be called only once at the time of initialization.
 *   \param[out] fd_ss points to file descriptor of ss socket
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_init(int32_t *fd_ss);

/**
 *   \fn int32_t mal_ss_config(void);
 *   \brief     This Api is used to conigure the ss library(if any properties to be set).
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_config(void);

/**
 *   int32_t mal_ss_deinit(void);
 *   \brief     This Api is used to de-initialize the ss library.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_deinit(void);

/**
 *   \fn int32_t mal_ss_register_callback(mal_ss_event_cb_t event_cb);
 *   \brief     This Api is a call back handler for the events.
 *   \param[in] event_cb event callback handler.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_register_callback(mal_ss_event_cb_t event_cb);

/**
 *   \fn void mal_ss_response_handler(void);
 *   \brief     This Api is called by application to inform that some message is available from modem for processing.
 *   \return None
 */
void mal_ss_response_handler(void);

/**
 *   \fn int32_t mal_ss_request_ussd(void* data, void* client_tag);
 *   \brief     This Api is used to send MO USSD string.
 *   \param[in] data pointer to \ref mal_ss_ussd_data structure containing the USSD informtation.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_request_ussd(void *data, void *client_tag);

/**
 *   \fn int32_t mal_ss_cancel_ussd(void* client_tag);
 *   \brief     This Api is used to cancel the USSD operation.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_cancel_ussd(void *client_tag);

/**
 *   \fn int32_t mal_ss_request_query_clip(void *client_tag);
 *   \brief     This Api is used to query the status of the CLIP supplementary service for Tele services
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_request_query_clip(void *client_tag);

/**
 *   \fn int32_t mal_ss_query_call_waiting(void* data, void *client_tag);
 *   \brief      This Api is used to query the call waiting state for a specified service class.
 *   \param[in]  data This param contains the service class information to query.
 *               Data type: int32_t. Possible values:  \ref mal_ss_serv_class_info
 *   \param[in]  client_tag void pointer to client specific data.
 *               The client specific data is transparent to MAL and is returned as-is in the response.
 *               Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_query_call_waiting(void *data, void *client_tag);

/**
 *   \fn int32_t mal_ss_set_call_waiting(void* data, void *client_tag);
 *   \brief     This Api is used to configure the current call waiting state for a specified service class.
 *   \brief     configuring call waiting state for multiple serivce class,
 *   \param[in] data pointer to the data. Data type: int32_t.
 *              Possible values: data[0] is 0 for "disabled" and 1 for "enabled",
 *              data[1] is the service class information \ref mal_ss_serv_class_info.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_set_call_waiting(void *data, void *client_tag);

/**
 *   \fn int32_t mal_ss_query_call_forward(void* data, void *client_tag);
 *   \brief     This Api is used to query the call forwarding status for a specified service class.
 *   \param[in] data pointer to \ref mal_ss_call_forward_info structure containing the call forward informtation.
 *                    Note that reason and serviceClass( as per \ref mal_ss_serv_class_info) in
 *                    \ref mal_ss_call_forward_info structure are the required params.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_query_call_forward(void *data, void *client_tag);

/**
 *   \fn int32_t mal_ss_set_call_forward(void* data, void *client_tag);
 *   \brief     This Api is used to configure call forward rule in call services.
 *   \param[in] data pointer to \ref mal_ss_call_forward_info structure containing the call forward informtation.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_set_call_forward(void *data, void *client_tag);

/**
 *   \fn int32_t mal_ss_change_barring_password(void* str_code, void* old_pword, void* new_pword, void* client_tag);
 *   \brief     This Api is used to change the call barring facility password
 *   \param[in] str_code pointer to facility string code as specified in TS 27.007 sec 7.4
 *              (eg "AB" for All Barring services)
 *   \param[in] old_pword pointer to old Password. Data type: char pointer
 *   \param[in] new_pword pointer to new Password. Data type: char pointer
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_change_barring_password(void *str_code, void *old_pword, void *new_pword, void *client_tag);

/**
 *   \fn int32_t mal_ss_query_call_barring(void *str_code, void *svc_class, void *client_tag);
 *   \brief     This Api is used to query the call barring status.
 *   \param[in] str_code pointer to facility string code as specified in TS 27.007 sec 7.4
 *              (eg "AO" for Barr All Outgoing Calls)
 *   \param[in] svc_class This param contains the service class information \ref mal_ss_serv_class_info to query.
 *               Data type: int32_t. Possible values: as per TS 27.007
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_query_call_barring(void *str_code, void *svc_class, void *client_tag);

/**
 *   \fn int32_t mal_ss_set_call_barring(void *str_code, void *svc_op_class, void *pword, void *client_tag);
 *   \brief     This Api is used to configure the call barring facility in call services.
 *   \param[in] str_code pointer to facility string code as specified in TS 27.007 sec 7.4
 *              (eg "AO" for Barr All Outgoing Calls)
 *   \param[in] svc_op_class pointer to \ref mal_ss_service_op_class. For service_class refer \ref mal_ss_serv_class_info.
 *   \param[in] pword pointer to password. Data type: char pointer.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_set_call_barring(void *str_code, void *svc_op_class, void *pword, void *client_tag);

/**
 *   \fn  int32_t mal_ss_calling_name_presentation_query(void *client_tag);
 *   \brief     This Api is used to query name presentation facility in call services.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_calling_name_presentation_query(void *client_tag);

/**
 *   \fn int32_t mal_ss_request_query_colp(void *client_tag);
 *   \brief     This Api is used to query the connected line presentation support.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_request_query_colp(void *client_tag);

/**
 *   \fn int32_t mal_ss_request_query_colr(void *client_tag);
 *   \brief     This Api is used to query the connected line id restriction support.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_request_query_colr(void *client_tag);

/**
 *   \fn int32_t mal_call_request_get_clir(void *client_tag)
 *   \brief This Api is used get the subscriber CLIR service status in the network.
 *   \param[in] client_tag void pointer to client specific data.
 *              The client specific data is transparent to MAL and is returned as-is in the response.
 *              Data type: Void pointer.
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_request_get_clir(void *client_tag);

/*
 *  \fn int32_t mal_ss_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for ss module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_ss_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for ss module. This is a synchronous call.
 *   \param[out] debug level for ss  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_ss_request_get_debug_level(uint8_t *level);

#endif //__MAL_SS_H

