/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cn_data_types_h__
#define __cn_data_types_h__ (1)

/***** Force structural alignment setting *****
 * When using different compilers and/or compiler settings the alignment may differ.
 * This is not acceptable for the IPC communication. A four byte alignment is chosen
 * since that is the word size of the ARM processor.
 */
#pragma pack(push,4)

#include "cn_general.h"

typedef cn_uintptr_t cn_client_tag_t;

#define CN_MCC_MNC_STRING_LENGTH 8
#define CN_MCC_RANGE_STRING_LENGTH 1
#define CN_MCC_STRING_LENGTH 3
#define CN_EMERGENCY_NUMBER_STRING_LENGTH 6
#define CN_RSSI_CONF_MAX_SEGMENTS 25
#define CN_MAX_STRING_SIZE 249 /* In the current socket context this is no performance issue */
#define CN_MAX_STRING_BUFF (1+CN_MAX_STRING_SIZE) /* leave room for terminator */
#define CN_MAX_NMR_RES_SIZE 16
#define CN_MAX_NMR_ARFCN_SIZE 65 /* 32 channels 2 byte for each */
#define CN_MAX_NMR_REPORT_SIZE 250
#define CN_MAX_DETAILED_CAUSE_LENGTH 32
#define CN_MAX_NUMBER_CALLS             (7)     /* Can have 5 calls in a conference + 1 held + 1 waiting */
#define CN_MAX_UUS_SIZE 240
#define CN_MAX_UUS_BUFF (1+CN_MAX_UUS_SIZE)   /* 240 bytes max + NULL termination */
#define CN_NETWORK_OPERATOR_MAX_LONG_LEN      (60)
#define CN_NETWORK_OPERATOR_MAX_SHORT_LEN     (30)
#define CN_MAX_REMOTE_PARTY_NAME_SIZE 122
#define CN_GSM_MAX_NEIGHBOUR_CELLS 15
#define CN_UMTS_MAX_DL_UARFCNS 3
#define CN_UMTS_MAX_CELLS_INCLUDED 32

/**
 * \enum cn_error_code_t
 * \brief Error codes
 *
 * \n This enum defines possible error codes for the call and networking interface.
 */
typedef enum {
    CN_UNKNOWN                           = 0,  /**< Unknown error code */
    CN_SUCCESS                           = 1,  /**< Success  */
    CN_FAILURE                           = 2,  /**< Failure */
    CN_REQUEST_NOT_SUPPORTED             = 3,  /**< Request not supported  */
    CN_REQUEST_INCORRECT_PARAMETERS      = 4,  /**< Request rejected because parameters provided are incorrect */
    CN_REQUEST_MODEM_TRANSACTION_ONGOING = 5,  /**< Modem state not established, then not possible to send any message to modem */
    CN_REQUEST_MODEM_RESET               = 6,  /**< Modem is off or reset, it means that host must reload modem SW and reset modem */
    CN_REQUEST_MODEM_NOT_READY           = 7,  /**< Modem is not ready */
    CN_REQUEST_BUSY                      = 8,  /**< Request can not be treated because another request is ongoing  */
    CN_REQUEST_ALREADY_ACTIVE            = 9,  /**< Requested state is already active */
    CN_REQUEST_SOCK_SEND_ERROR           = 10, /**< Error on socket send */
    CN_REQUEST_SOCK_CONN_ERROR           = 11, /**< Error on socket connection */
    CN_REQUEST_INVALID_BLOCK_TYPE        = 12, /**< Block type not correct */
    CN_REQUEST_INSUFFICIENT_MEMORY       = 13, /**< Memory allocation failure */
    CN_REQUEST_INVALID_DATA              = 14, /**< Invalid data passed to the routine */
    CN_REQUEST_NOT_ALLOWED               = 15, /**< Request not allowed  */
    CN_REQUEST_INCORRECT_PASSWORD        = 16, /**< Incorrect password */
    CN_REQUEST_FDN_BLOCKED               = 18, /**< Blocked by FDN control */
    CN_REQUEST_CC_REJECTED               = 19, /**< Rejected by Sat call control */
    CN_REQUEST_CC_SERVICE_MODIFIED_SS    = 20, /**< Service is modified by Sat call control to SS*/
    CN_REQUEST_CC_SERVICE_MODIFIED_USSD  = 21, /**< Service is modified by Sat call control to USSD*/
    CN_REQUEST_CC_SERVICE_MODIFIED_CALL  = 22, /**< Service is modified by Sat call control Call*/
    CN_REQUEST_SIM_RAT_REJECTED          = 23, /**< Rejected by SIM "RAT control" */
    CN_SUCCESS_AUTOMATIC_FALLBACK        = 24, /**< Successfully reverted to automatic network
                                                    selection after failed manual registration */
    CN_REQUEST_INTERRUPTED               = 25, /**< Request was interrupted */
    CN_REQUEST_CAUSE_NET_NOT_FOUND       = 26, /**< Request completed but with no network found */
} cn_error_code_t;

/**
 * \enum cn_service_class_t
 * \brief Services classes
 *
 * \n This enum defines service classes call and networking interface.
 */
typedef enum {
    CN_SERVICE_CLASS_GSM_VOICE                   = 1,   /**< Telephony */
    CN_SERVICE_CLASS_GSM_BEARER                  = 2,   /**< All Bearer */
    CN_SERVICE_CLASS_GSM_FACSIMILE               = 4,   /**< Facsimile services */
    CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE         = 5,   /**< All teleservices except SMS */
    CN_SERVICE_CLASS_GSM_SMS                     = 8,   /**< Short Message Services */
    CN_SERVICE_CLASS_GSM_FACSIMILE_SMS           = 12,  /**< All data teleservices */
    CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS     = 13,  /**< All teleservices */
    CN_SERVICE_CLASS_GSM_DEFAULT_MMI             = 15,
    CN_SERVICE_CLASS_GSM_DATA_CIRCUIT_SYNC       = 16,  /**< All data circuit sync */
    CN_SERVICE_CLASS_GSM_DATA_CIRCUIT_ASYNC      = 32,  /**< All data circuit async */
    CN_SERVICE_CLASS_GSM_PACKET_ACCESS           = 64,  /**< Dedicated packet access */
    CN_SERVICE_CLASS_GSM_PAD_ACCESS              = 128, /**< Dedicated PAD access */
    CN_SERVICE_CLASS_GSM_DATA_ASYNC_PAD_ACCESS   = 160, /**< All async services */
    CN_SERVICE_CLASS_GSM_DATA_SYNC_PACKET_ACCESS = 80,  /**< All sync services */
    CN_SERVICE_CLASS_GSM_DEFAULT                 = 7    /** Supported only in SS Query operations, i.e in
                                                            Query Call waiting, Query Call Forwarding, Query Call Barring ONLY */
} cn_service_class_t;


/**
 * \enum cn_rf_status_t
 * \brief RF status
 *
 * \n This enum defines the RF status.
 */
typedef enum {
    CN_RF_OFF        = 0, /**< RF is on */
    CN_RF_ON         = 1, /**< RF is off */
    CN_RF_TRANSITION = 2  /**< RF is in transition mode */
} cn_rf_status_t;


/**
 * \enum cn_modem_status_t
 * \brief modem status
 *
 * \n This enum defines the modem status.
 */
typedef enum {
    CN_MODEM_STATUS_OK                 = 0x00, /**< Request succeded */
    CN_MODEM_STATUS_REQ_FAIL           = 0x01, /**< Request failed */
    CN_MODEM_STATUS_NOT_ALLOWED        = 0x02, /**< Requested state not allowed */
    CN_MODEM_STATUS_ALREADY_ACTIVE     = 0X06, /**< Requested state already active */
    CN_MODEM_STATUS_TRANSITION_ONGOING = 0x16, /**< Transition in progress */
    CN_MODEM_STATUS_RESET_REQUIRED     = 0x17 /**< Reset required to fulfil the request */
} cn_modem_status_t;


/**
 * \enum cn_network_type_t
 * \brief Network types
 *
 * \n This enum defines network types for the call and network interface.
 */
typedef enum {
    CN_NETWORK_TYPE_GSM_WCDMA_PREFERRED_WCDMA     = 0, /**< GSM/WCDMA (WCDMA preferred) */
    CN_NETWORK_TYPE_GSM_ONLY                      = 1, /**< GSM only */
    CN_NETWORK_TYPE_WCDMA_ONLY                    = 2, /**< WCDMA only */
    CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE           = 3, /**< GSM/WCDMA (auto mode, according to PRL) */
    CN_NETWORK_TYPE_CDMA_EVDO_AUTO_MODE           = 4, /**< CDMA and EvDo (auto mode, according to PRL) */
    CN_NETWORK_TYPE_CDMA_ONLY                     = 5, /**< CDMA only */
    CN_NETWORK_TYPE_EVDO_ONLY                     = 6, /**< EvDo only */
    CN_NETWORK_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO_MODE = 7  /**< GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL) */
} cn_network_type_t;


/**
 * \enum cn_rat_type_t
 * \brief RAT types
 *
 * \n This enum defines RAT (Radio Access Technology) Types.
 */
typedef enum {
    CN_RAT_TYPE_UNKNOWN    = 0,  /**< Unknown */
    CN_RAT_TYPE_GPRS       = 1,  /**< GPRS */
    CN_RAT_TYPE_EDGE       = 2,  /**< EDGE (EGPRS) */
    CN_RAT_TYPE_UMTS       = 3,  /**< UMTS (W-CDMA) */
    CN_RAT_TYPE_IS95A      = 4,  /**< IS95A */
    CN_RAT_TYPE_IS95B      = 5,  /**< IS95B */
    CN_RAT_TYPE_1xRTT      = 6,  /**< 1xRTT */
    CN_RAT_TYPE_EVDO_REV_0 = 7,  /**< EvDO Rrv. 0 */
    CN_RAT_TYPE_EVDO_REV_A = 8,  /**< EvDO Rrv. A */
    CN_RAT_TYPE_HSDPA      = 9,  /**< HSDPA */
    CN_RAT_TYPE_HSUPA      = 10, /**< HSUPA */
    CN_RAT_TYPE_HSPA       = 11, /**< HSPA */
    CN_RAT_TYPE_GSM        = 12, /**< GSM */
    CN_RAT_TYPE_EPS        = 13, /**< EPS (Evolved Packet System) */
    CN_RAT_TYPE_GPRS_DTM   = 16, /**< GPRS DTM (Dual transfer mode) */
    CN_RAT_TYPE_EDGE_DTM   = 19  /**< EDGE DTM */
} cn_rat_type_t;


/**
 * \enum cn_cell_rat_type_t
 * \brief Cell RAT types
 *
 * \n This enum defines RAT (Radio Access Technology) Types for cell info.
 */
typedef enum {
    CN_CELL_RAT_UNKNOWN = 0, /* RAT type unknown */
    CN_CELL_RAT_GSM     = 1, /* RAT type GSM */
    CN_CELL_RAT_WCDMA   = 2, /* RAT type WCDMA */
    CN_CELL_RAT_EPS     = 3  /* RAT type EPS */
} cn_cell_rat_t;

/**
 * \enum cn_net_network_type
 * \brief Network types
 *
 * \n This enum defines the network types.
 */
typedef enum {
    CN_NET_GSM_HOME_PLMN      = 0,
    CN_NET_GSM_PREFERRED_PLMN = 1,
    CN_NET_GSM_FORBIDDEN_PLMN = 2,
    CN_NET_GSM_OTHER_PLMN     = 3,
    CN_NET_GSM_NO_PLMN_AVAIL  = 4
} cn_net_network_t;

/**
 * \enum cn_nw_service_status
 * \brief Network Service Status
 *
 * \n This enum defines the Network Service Status.
 */
typedef enum {
    CN_NW_SERVICE                 = 0, /* CS is in service */
    CN_NW_LIMITED_SERVICE         = 1, /* CS is in limited_service */
    CN_NW_NO_COVERAGE             = 2, /* CS is not in service. */
    CN_NW_LIMITED_SERVICE_WAITING = 3  /* CS is in limited service but waiting for PIN/PUK verification */
} cn_nw_service_status_t;

/**
 * \enum cn_request_reset_cpu_type
 * \brief Modem Reset CPU Type
 *
 * \n This enum defines the CPU type for Modem Reset.
 */
typedef enum {
    CN_CPU_L23  = 0x00,
    CN_CPU_L1   = 0x01,
    CN_CPU_ALL  = 0X02
} cn_cpu_type_t;

/**
 * \enum cn_forced_sleep_mode
 * \brief Set the modem CPU's to the given sleep mode
 *
 * \n This enum defines values for modem CPU's to the given sleep mode
 */
typedef enum {
    CN_MODEM_TEST_SLEEP_MODE_ALL_ALLOWED               = 0x00,  /**< Power-down sleep mode is allowed */
    CN_MODEM_TEST_SLEEP_MODE_PWR_DOWN_WITH_RF_CLK_REQ   = 0x10, /**< Power-down sleep mode with RF clock request is allowed */
    CN_MODEM_TEST_SLEEP_MODE_DEEP_SLEEP                 = 0x20, /**< Deep sleep mode without power-down is allowed */
    CN_MODEM_TEST_SLEEP_MODE_DEEP_SLEEP_WITH_RF_CLK_REQ = 0x30, /**< Deep sleep mode with RF clock request is allowed */
    CN_MODEM_TEST_SLEEP_MODE_LIGHT_WITHOUT_PLL          = 0x40, /**< Light sleep mode without HF clock request is allowed */
    CN_MODEM_TEST_SLEEP_MODE_LIGHT_WITH_PLL             = 0x50, /**< Light sleep mode with HF clock request is allowed  */
    CN_MODEM_TEST_SLEEP_MODE_NO_SLEEP                   = 0x60 /**< Sleep mode is not allowed (no WFI)  */
} cn_forced_sleep_mode_t;

/**
 * \struct cn_cell_info
 * \brief Cell Information
 *
 * \n This structure provides current cell information.
 */
typedef struct {
    cn_cell_rat_t               rat;                            /* RAT applicable to cell information*/
    cn_net_network_t            network_type;                   /* Network Type TODO align cn_network_info_t with this typing */
    cn_nw_service_status_t      service_status;                 /* Network Service Status. Cell Information */
    cn_uint32_t                 gsm_frame_nbr;                  /* GSM frame Number; valid */
    cn_uint32_t                 bands_available;                /* Available bands. */
    cn_uint32_t                 current_cell_id;                /* Current Cell ID */
    cn_uint16_t                 current_ac;                     /* Location Area Code in case of GSM and WCDMA; Tracking Area Code in case of EPS. */
    cn_uint16_t                 country_code;                   /* Country code */
    cn_uint16_t                 network_code;                   /* Network code */
    char                        mcc_mnc[CN_MAX_STRING_BUFF];    /* < Null-terminated string with MCC and MNC */
} cn_cell_info_t;


/**
 * \enum cn_network_search_mode_t
 * \brief Network Search Modes
 *
 * \n This enum defines available Network Search Modes
 */
typedef enum {
    CN_NETWORK_SEARCH_MODE_AUTOMATIC      = 0, /**< Automatic mode */
    CN_NETWORK_SEARCH_MODE_MANUAL         = 1  /**< Manual mode */
} cn_network_search_mode_t;


/**
 * \enum cn_network_access_conf_state_t
 * \brief Network access configuration state
 *
 * \n This enum defines the network access configuration states.
 */
typedef enum {
    CN_NETWORK_ACCESS_CONFIG_DISABLE = 0,
    CN_NETWORK_ACCESS_CONFIG_ENABLE  = 1
} cn_network_access_config_t;


/**
 * \struct cn_network_access_config_data_t
 * \brief Network access configuration data
 *
 * \n This structure provides network access configuration data.
 */
typedef struct {
    cn_network_access_config_t registration_conf; /**< Registration configuration */
    cn_network_access_config_t roaming_conf;      /**< Roaming configuration */
} cn_network_access_config_data_t;


/**
 * \enum cn_modem_reg_status_t
 * \brief Network Registration Status
 *
 * \n This enum defines Network Registration Status.
 */
typedef enum {
    CN_NOT_REG_NOT_SEARCHING                         = 0x00, /**< Not registered, not searching */
    CN_REGISTERED_TO_HOME_NW                         = 0x01, /**< Registerred to home network */
    CN_NOT_REG_SEARCHING_OP                          = 0x02, /**< Not registered, but seraching the operator */
    CN_REGISTRATION_DENIED                           = 0x03, /**< Registration denied */
    CN_REGISTRATION_UNKNOWN                          = 0x04, /**< Registration status unknown */
    CN_REGISTERED_ROAMING                            = 0x05,  /**< Registered in roaming */
    CN_NOT_REG_NOT_SEARCHING_EMERGENCY_CALLS_ALLOWED = 0x0A, /**< Not registered, not searching - emergency call is possible */
    CN_NOT_REG_SEARCHING_OP_EMERGENCY_CALLS_ALLOWED  = 0x0C, /**< Not registered, but searching the operator - emergency call is possible */
    CN_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED   = 0x0D, /**< Registration denied - emergency call is possible */
    CN_REGISTRATION_UNKNOWN_EMERGENCY_CALLS_ALLOWED  = 0x0E  /**< Registration status unknown - emergency call is possible */
} cn_modem_reg_status_t;


/**
 * \struct cn_registration_info_t
 * \brief Registration Data
 *
 * \n This structure provides Registration Data.
 */
typedef struct {
    cn_modem_reg_status_t reg_status;                      /**< Registration status      */
    cn_uint8_t    gprs_attached;                           /**< PS Network attach status */
    cn_uint8_t    cs_attached;                             /**< CS Network attach status */
    cn_rat_type_t rat;                                     /**< RAT */
    cn_uint32_t   cid;                                     /**< CID */
    cn_uint16_t   lac;                                     /**< LAC */
    cn_network_search_mode_t search_mode;                  /**< Network search mode */
    char          mcc_mnc[CN_MAX_STRING_BUFF];             /**< Null-terminated string with MCC and MNC */
    char          long_operator_name[CN_MAX_STRING_BUFF];  /**< Null-terminated string presenting long operator name */
    char          short_operator_name[CN_MAX_STRING_BUFF]; /**< Null-terminated string presenting short operator name */
} cn_registration_info_t;


/**
 * \enum cn_reg_status_trigger_level_t
 * \brief Trigger level for registration status event.
 *
 * \n This enum defines different trigger levels for CN_EVENT_MODEM_REGISTRATION_STATUS.
 * \n It specifies which parameters in \ref cn_registration_info_t that should trigger the
 * \n event in case of value change. When the trigger level is increased all triggers with
 * \n lower values are included as well.
 */
typedef enum {
    CN_REG_STATUS_TRIGGER_LEVEL_UNDEFINED          = -1, /** < initializer value                                              */
    CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM           =  0, /** < event disabled, no parameter can trigger event (default level) */
    CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE =  1, /** < change of parameter 'reg_status' will trigger event            */
    CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC         =  2, /** < change of parameters 'cid' and 'lac' will trigger event        */
    CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS         =  3, /** < change of all parameters will trigger event                    */
} cn_reg_status_trigger_level_t;


/**
 * \enum cn_network_selection_mode_t
 * \brief Network Selection Modes
 *
 * \n This enum defines available Network Selection Modes for registration
 * \n requests.
 */
typedef enum {
    CN_NETWORK_SELECTION_MODE_UNKNOWN          = 0, /**< Unknown mode */
    CN_NETWORK_SELECTION_MODE_MANUAL           = 1, /**< Manual mode */
    CN_NETWORK_SELECTION_MODE_AUTOMATIC        = 2, /**< Automatic moce */
    CN_NETWORK_SELECTION_MODE_USER_RESELECTION = 3, /**< Reselection */
    CN_NETWORK_SELECTION_MODE_NO_SELECTION     = 4  /**< No Selection; used to clear the state */
} cn_network_selection_mode_t;


/**
 * \struct cn_network_selection_t
 * \brief Data for Registration to Network
 *
 * \n This structure is used for selecting a network for registration.
 */
typedef struct {
    cn_network_selection_mode_t selection_mode; /**< Chosen Selection Mode */
    cn_uint8_t                  index;          /**< Index of the network */
} cn_network_selection_t;

/**
 * \struct cn_network_registration_data_t
 * \brief Registration Data
 *
 * \n This structure provides Registration Data.
 */
typedef struct {
    cn_uint8_t registration_status;  /**< Registration status */
    cn_uint8_t selection_mode;       /**< Network selection mode */
    cn_uint8_t GSM_operator_code_1;  /**< First byte of operator code */
    cn_uint8_t GSM_operator_code_2;  /**< Second byte of operator code */
    cn_uint8_t GSM_operator_code_3;  /**< Third byte of operator code */
    cn_uint8_t gsm_band_info;        /**< GSM band information */
    cn_uint8_t gsm_network_type;     /**< GSM Network Type */
    cn_uint8_t GPRS_support;         /**< Whether GPRS is supported */
    cn_uint8_t GPRS_network_mode;    /**< GPRS Network Mode */
    cn_uint8_t CS_services;          /**< Whether CS services are available */
    cn_uint8_t GPRS_services;        /**< */
    cn_uint8_t EGPRS_support;        /**< Wheter EGPRS is supported */
    cn_uint8_t DTM_support;          /**< Wheter DTM is supported */
    cn_uint8_t current_RAC;          /**< current RAC */
    cn_uint8_t HSDPA_available;      /**< Wheter HSDPA is available */
    cn_uint8_t HSUPA_available;      /**< Wheter HSUPA is available */
    cn_uint8_t camped_in_HPLMN;      /**< Whether camped to HPLMN */
    cn_uint8_t RAT_name;             /**< RAT Name */
    cn_uint8_t reject_code;          /**< Reject code, if error is reported */
    cn_uint16_t current_LAC;         /**< Current LAC */
    cn_uint32_t GSM_current_cell_id; /**< Current Cell ID */
} cn_network_registration_data_t;


/**
 * \struct cn_network_node_t
 * \brief Details of a particular network
 *
 * \n This structure provides details of a particular network as a node.
 */
typedef struct {
    cn_uint8_t   index;                             /**< Index of the network within modem */
    cn_uint8_t   network_status;                    /**< Network Status: 0 - unknown; 1 - available; 2 - current; 3 - forbidden; */
    cn_uint8_t   band_info;                         /**< GSM Band info: 0 - 900-1800; 1 - 850-1900; 2 - not available; 3 - all supported band; */
    cn_uint8_t   network_type;                      /**< Network Status: 0 - home PLMN; 1 - preferred PLMN; 2 - forbidden PLMN; 3 - other PLMN; 4 - no PLMN available */
    cn_uint8_t   umts_available;                    /**< 0 - UMTS available; 1 - UMTS not available */
    cn_uint16_t  current_lac;                       /**< Current LAC */
    cn_uint16_t  country_code;                      /**< Country code */
    cn_uint16_t  network_code;                      /**< Network code */
    char         mcc_mnc[CN_MCC_MNC_STRING_LENGTH]; /**< NULL- terminated string presenting MCC and MNC */
    char         long_op_name[CN_NETWORK_OPERATOR_MAX_LONG_LEN + 1]; /**< NULL- terminated string presenting long operator name */
    char         short_op_name[CN_NETWORK_OPERATOR_MAX_SHORT_LEN + 1]; /**< NULL- terminated string presenting short operator name */
} cn_network_info_t;


/**
 * \struct cn_manual_network_search_data_t
 * \brief Manual Network Search Data
 *
 * \n This structure provides Manual Network Search Data.
 */
typedef struct {
    cn_uint8_t          num_of_networks;    /**< Number of networks found */
    cn_network_info_t   cn_network_info[1]; /**< Network information, first array entry (dynamic array determined by num_of_networks) */
} cn_manual_network_search_data_t;


/**
 * \enum  cn_call_state_t
 * \brief This enum has values of CS call states.
 */
typedef enum {
    CN_CALL_STATE_ACTIVE   = 0, /**< CS Call state Active    */
    CN_CALL_STATE_HOLDING  = 1, /**< CS Call state On Hold   */
    CN_CALL_STATE_DIALING  = 2, /**< CS Call state Dialing   */
    CN_CALL_STATE_ALERTING = 3, /**< CS Call state Alerting  */
    CN_CALL_STATE_INCOMING = 4, /**< CS Call state Incoming  */
    CN_CALL_STATE_WAITING  = 5, /**< CS Call state Waiting   */
    CN_CALL_STATE_IDLE     = 6, /**< CS Call state Idle      */
    CN_CALL_STATE_MT_SETUP = 7  /**< CS Call state MT Setup  */
} cn_call_state_t;


/**
 * \enum cn_call_state_filter_t
 * \brief Filters to select call or calls based on state or states.
 *
 * \n This enum defines which call state(s) to filter. The values may be combined
 * except for the CN_CALL_STATE_FILTER_NONE enum value.
 *
 * Please note that the order is important, as if there are several eligible
 * calls for termination only the first found will be selected.
 */
typedef enum {
    CN_CALL_STATE_FILTER_NONE       = 0x00,     /**< No filter.                             */
    CN_CALL_STATE_FILTER_WAITING    = 0x01,     /**< Filter waiting call(s).                */
    CN_CALL_STATE_FILTER_ACTIVE     = 0x02,     /**< Filter active call(s).                 */
    CN_CALL_STATE_FILTER_DIALLING   = 0x04,     /**< Filter dialling call(s).               */
    CN_CALL_STATE_FILTER_ALERTING   = 0x08,     /**< Filter alerting call(s).               */
    CN_CALL_STATE_FILTER_INCOMING   = 0x10,     /**< Filter incoming call(s).               */
    CN_CALL_STATE_FILTER_HOLDING    = 0x20,     /**< Filter held call(s).                   */
    CN_CALL_STATE_FILTER_IDLING     = 0x40,     /**< Filter idle call(s).                   */
    CN_CALL_STATE_FILTER_INCLUSIVE  = 0x80,     /**< Filter all specified call-states.      *
                                                 **  If not set only the calls in the first *
                                                 **  found call-state will be included.     */
    CN_CALL_STATE_FILTER_ALL        =           /**< Filter all calls in all call-states    *
                                                 **  except idle.                           */
    CN_CALL_STATE_FILTER_HOLDING  |
    CN_CALL_STATE_FILTER_ACTIVE   |
    CN_CALL_STATE_FILTER_DIALLING |
    CN_CALL_STATE_FILTER_ALERTING |
    CN_CALL_STATE_FILTER_INCOMING |
    CN_CALL_STATE_FILTER_WAITING  |
    CN_CALL_STATE_FILTER_INCLUSIVE,
} cn_call_state_filter_t;


/**
 * \enum  cn_call_cause_type_sender_t
 * \brief This enum has values of last call cause type sender.
 */
typedef enum {
    CN_CALL_CAUSE_TYPE_SENDER_NONE    = 0, /**< None                                                     */
    CN_CALL_CAUSE_TYPE_SENDER_CLIENT  = 1, /**< Client cause value                                       */
    CN_CALL_CAUSE_TYPE_SENDER_SERVER  = 2, /**< Call Server cause value                                  */
    CN_CALL_CAUSE_TYPE_SENDER_NETWORK = 3  /**< Network cause value as per 3GPP TS 24.008 ch. 10.5.4.11  */
} cn_call_cause_type_sender_t;

/**
 * \enum  cn_call_client_cause_t
 * \brief This enum has values of last call cause sent by CN_CALL_CAUSE_TYPE_SENDER_CLIENT .
 */
typedef enum {
    CN_CALL_CLIENT_CAUSE_RELEASE_BY_USER      = 0x03,
    CN_CALL_CLIENT_CAUSE_BUSY_USER_REQUEST    = 0x04
} cn_call_client_cause_t;

/**
 * \enum  cn_call_server_cause_t
 * \brief This enum has values of last call cause sent by CN_CALL_CAUSE_TYPE_SENDER_SERVER .
 */
typedef enum {
    CN_CALL_SERVER_CAUSE_NO_CALL              = 0x01,
    CN_CALL_SERVER_CAUSE_ERROR_REQUEST        = 0x05,
    CN_CALL_SERVER_CAUSE_CALL_ACTIVE          = 0x07,
    CN_CALL_SERVER_CAUSE_NO_CALL_ACTIVE       = 0x08,
    CN_CALL_SERVER_CAUSE_INVALID_CALL_MODE    = 0x09,
    CN_CALL_SERVER_CAUSE_TOO_LONG_ADDRESS     = 0x0B,
    CN_CALL_SERVER_CAUSE_INVALID_ADDRESS      = 0x0C,
    CN_CALL_SERVER_CAUSE_EMERGENCY            = 0x0D,
    CN_CALL_SERVER_CAUSE_NO_SERVICE           = 0x0E,
    CN_CALL_SERVER_CAUSE_CODE_REQUIRED        = 0x10,
    CN_CALL_SERVER_CAUSE_NOT_ALLOWED          = 0x11,
    CN_CALL_SERVER_CAUSE_DTMF_ERROR           = 0x12,
    CN_CALL_SERVER_CAUSE_CHANNEL_LOSS         = 0x13,
    CN_CALL_SERVER_CAUSE_FDN_NOT_OK           = 0x14,
    CN_CALL_SERVER_CAUSE_BLACKLIST_BLOCKED    = 0x16,
    CN_CALL_SERVER_CAUSE_BLACKLIST_DELAYED    = 0x17,
    CN_CALL_SERVER_CAUSE_EMERGENCY_FAILURE    = 0x1A,
    CN_CALL_SERVER_CAUSE_NO_SIM               = 0x1F,
    CN_CALL_SERVER_CAUSE_SIM_LOCK_OPERATIVE   = 0x20,
    CN_CALL_SERVER_CAUSE_DTMF_SEND_ONGOING    = 0x24,
    CN_CALL_SERVER_CAUSE_CS_INACTIVE          = 0x25,
    CN_CALL_SERVER_CAUSE_NOT_READY            = 0x26,
    CN_CALL_SERVER_CAUSE_INCOMPATIBLE_DEST    = 0x27
} cn_call_server_cause_t;

/**
 * \enum  cn_call_network_cause_t
 * \brief This enum has values of last call cause sent by CN_CALL_CAUSE_TYPE_SENDER_NETWORK .
 * It is based on values in 3GPP spec TS24.008 annex H
 */
typedef enum {
    CN_CALL_NETWORK_CAUSE_UNASSIGNED_NUMBER   = 0x01,
    CN_CALL_NETWORK_CAUSE_NO_ROUTE            = 0x03,
    CN_CALL_NETWORK_CAUSE_CH_UNACCEPTABLE     = 0x06,
    CN_CALL_NETWORK_CAUSE_OPER_BARRING        = 0x08,
    CN_CALL_NETWORK_CAUSE_NORMAL              = 0x10,
    CN_CALL_NETWORK_CAUSE_USER_BUSY           = 0x11,
    CN_CALL_NETWORK_CAUSE_NO_USER_RESPONSE    = 0x12,
    CN_CALL_NETWORK_CAUSE_ALERT_NO_ANSWER     = 0x13,
    CN_CALL_NETWORK_CAUSE_CALL_REJECTED       = 0x15,
    CN_CALL_NETWORK_CAUSE_NUMBER_CHANGED      = 0x16,
    CN_CALL_NETWORK_CAUSE_NON_SELECT_CLEAR    = 0x1A,
    CN_CALL_NETWORK_CAUSE_DEST_OUT_OF_ORDER   = 0x1B,
    CN_CALL_NETWORK_CAUSE_INVALID_NUMBER      = 0x1C,
    CN_CALL_NETWORK_CAUSE_FACILITY_REJECTED   = 0x1D,
    CN_CALL_NETWORK_CAUSE_RESP_TO_STATUS      = 0x1E,
    CN_CALL_NETWORK_CAUSE_NORMAL_UNSPECIFIED  = 0x1F,
    CN_CALL_NETWORK_CAUSE_NO_CHANNEL          = 0x22,
    CN_CALL_NETWORK_CAUSE_NETW_OUT_OF_ORDER   = 0x26,
    CN_CALL_NETWORK_CAUSE_TEMPORARY_FAILURE   = 0x29,
    CN_CALL_NETWORK_CAUSE_CONGESTION          = 0x2A,
    CN_CALL_NETWORK_CAUSE_ACCESS_INFO_DISC    = 0x2B,
    CN_CALL_NETWORK_CAUSE_CHANNEL_NA          = 0x2C,
    CN_CALL_NETWORK_CAUSE_RESOURCES_NA        = 0x2F,
    CN_CALL_NETWORK_CAUSE_QOS_NA              = 0x31,
    CN_CALL_NETWORK_CAUSE_FACILITY_UNSUBS     = 0x32,
    CN_CALL_NETWORK_CAUSE_COMING_BARRED_CUG   = 0x37,
    CN_CALL_NETWORK_CAUSE_BC_UNAUTHORIZED     = 0x39,
    CN_CALL_NETWORK_CAUSE_BC_NA               = 0x3A,
    CN_CALL_NETWORK_CAUSE_SERVICE_NA          = 0x3F,
    CN_CALL_NETWORK_CAUSE_BEARER_NOT_IMPL     = 0x41,
    CN_CALL_NETWORK_CAUSE_ACM_MAX             = 0x44,
    CN_CALL_NETWORK_CAUSE_FACILITY_NOT_IMPL   = 0x45,
    CN_CALL_NETWORK_CAUSE_ONLY_RDI_BC         = 0x46,
    CN_CALL_NETWORK_CAUSE_SERVICE_NOT_IMPL    = 0x4F,
    CN_CALL_NETWORK_CAUSE_INVALID_TI          = 0x51,
    CN_CALL_NETWORK_CAUSE_NOT_IN_CUG          = 0x57,
    CN_CALL_NETWORK_CAUSE_INCOMPATIBLE_DEST   = 0x58,
    CN_CALL_NETWORK_CAUSE_INV_TRANS_NET_SEL   = 0x5B,
    CN_CALL_NETWORK_CAUSE_SEMANTICAL_ERR      = 0x5F,
    CN_CALL_NETWORK_CAUSE_INVALID_MANDATORY   = 0x60,
    CN_CALL_NETWORK_CAUSE_MSG_TYPE_INEXIST    = 0x61,
    CN_CALL_NETWORK_CAUSE_MSG_TYPE_INCOMPAT   = 0x62,
    CN_CALL_NETWORK_CAUSE_IE_NON_EXISTENT     = 0x63,
    CN_CALL_NETWORK_CAUSE_COND_IE_ERROR       = 0x64,
    CN_CALL_NETWORK_CAUSE_MSG_INCOMPATIBLE    = 0x65,
    CN_CALL_NETWORK_CAUSE_TIMER_EXPIRY        = 0x66,
    CN_CALL_NETWORK_CAUSE_PROTOCOL_ERROR      = 0x6F,
    CN_CALL_NETWORK_CAUSE_INTERWORKING        = 0x7F
} cn_call_network_cause_t;

/**
 * \enum  cn_cause_no_cli_t
 * \brief This enum has values for the causes why the Calling Line Information
 * is not available.
 * It is based on values in 3GPP spec TS24.008, "Cause of No CLI".
 */
typedef enum {
    CN_CAUSE_NO_CLI_UNAVAILABLE                 = 0x01,
    CN_CAUSE_NO_CLI_REJECT_BY_USER              = 0x02,
    CN_CAUSE_NO_CLI_INTERACTION                 = 0x03,
    CN_CAUSE_NO_CLI_COIN_LINE_PAYPHONE          = 0x04,
} cn_cause_no_cli_t;

/**
 * \enum  cn_presentation_t
 * \brief This enum has indicator values for presentation and screening.
 * It is based on values in 3GPP TS 24.008 ch. 10.5.4.9, 'Calling Party BCD Number IE'.
 */
typedef enum {
    CN_SCREEN_MASK                              = 0x03,
    CN_USER_NOT_SCREENED                        = 0x00,
    CN_USER_SCREENED_PASSED                     = 0x01,
    CN_USER_SCREENED_FAILED                     = 0x02,
    CN_SCREEN_NETWORK_PROVIDED                  = 0x03,

    CN_PRESENTATION_MASK                        = 0x60,
    CN_PRESENTATION_ALLOWED                     = 0x00,
    CN_PRESENTATION_RESTRICTED                  = 0x20,
    CN_PRESENTATION_UNAVAILABLE                 = 0x40,
} cn_presentation_t;

/**
 * \enum  cn_call_mode_t
 * \brief This enum has indicator values for call modes.
 * It is based in part on values in 3GPP TS 24.008 ch. 10.5.4.21, 'Progress indicator IE'.
 */
typedef enum {
    CN_CALL_MODE_EMERGENCY                      = 0x00,
    CN_CALL_MODE_SPEECH                         = 0x01,
    CN_CALL_MODE_ALS_2                          = 0xA2,
    CN_CALL_MODE_MULTIMEDIA                     = 0xA3,
    CN_CALL_MODE_UNKNOWN                        = 0xFF,
} cn_call_mode_t;


/**
 * \var  cn_call_cause_cause_t
 * \brief This enum type is a union of /ref cn_call_network_cause_t, /ref cn_call_server_cause_t, /ref cn_call_client_cause_t
 */
typedef cn_sint32_t cn_call_cause_cause_t;


/**
 * \struct  exit_cause
 * \brief This structure contains the details of call failure reasons.
*/
typedef struct {
    cn_call_cause_type_sender_t cause_type_sender; /**< Cause Type Sender \ref call_cause_type_sender */
    cn_call_cause_cause_t  cause;                  /**< Call Cause \ref cn_call_cause_cause_t*/
    cn_uint8_t  detailed_cause_length;             /**< Number of data octets in detail_cause array. */
    cn_uint8_t  detailed_cause[CN_MAX_DETAILED_CAUSE_LENGTH]; /**< Cause information element as defined in 3GPP TS 24.008. */
    /* Octets not used will be zero-padded. */
} cn_exit_cause_t;

/**
 * \struct  cn_call_context_t
 * \brief   CS Call context structure.
 * \brief   This structure contains the call context information for a CS Call.
 */
typedef struct {
    cn_call_state_t call_state;                   /**< Call state \ref cn_call_state_t */
    cn_call_state_t prev_call_state;              /**< Previous call state \ref cn_call_state_t */
    cn_sint32_t     call_id;                      /**< GSM Index for use with, eg, AT+CHLD */
    cn_sint32_t     address_type;                 /**< type of address, eg 145 = intl */
    cn_uint8_t      message_type;                 /**< Message type 1 = Setup, 2 = Connect, 3 = Release */
    cn_uint8_t      message_direction;            /**< Message direction 1 = Sent to network, 2 = Received from network, 3 = Internal */
    cn_uint8_t      transaction_id;               /**< Transaction ID. Transaction identifier from the received or sent message. TI flag is coded to bit 8
                                                       and bits 5 to 7 are TI value. Bits 1 to 4 are set to zero. If internal call release has occurred
                                                       field is coded as clearing would have been initiated by mobile i.e. similar to messages sent to network. */
    cn_bool_t       is_multiparty;                /**< nonzero if is multiparty call */
    cn_bool_t       is_MT;                        /**< nonzero if call is mobile terminated */
    cn_bool_t       als;                          /**< ALS line indicator if available (0 == line1) */
    cn_call_mode_t  mode;                         /**< Mode: 0=emergency, 1=speech, 162=ALS-2, 163=multimedia */
    cn_bool_t       is_voice_privacy;             /**< nonzero if CDMA voice privacy mode is active */
    cn_cause_no_cli_t cause_no_cli;               /**< 1 = CLI_UNAVAILABLE, 2 = CLI_REJECT_BY_USER, 3 = CLI_INTERACTION, 4 = CLI_COIN_LINE_PAYPHONE */
    cn_exit_cause_t call_cause;                   /**< Exit cause \ref cn_exit_cause_t  Valid when the callState is CALL_IDLE   */
    cn_presentation_t number_presentation;        /**< Presentation indicator \ref cn_presentation_t */
    char            number[CN_MAX_STRING_BUFF];   /**< Null-terminated phone number string */
    cn_presentation_t name_presentation;          /**< Presentation indicator \ref cn_presentation_t */
    cn_uint8_t      name_char_length;             /**< Remote party name character length */
    cn_uint16_t     name[CN_MAX_REMOTE_PARTY_NAME_SIZE]; /**< Remote party name, UCS-2 string. Not necessarily null-terminated. */
    cn_uint8_t      user_to_user[CN_MAX_UUS_BUFF]; /**< User to User IE. 3GPP TS 24.008 ch. 10.5.4.25 */
    cn_uint32_t     user_to_user_len;             /**< Length of user_to_user buffer. */
    cn_sint8_t      user_to_user_type;            /**< User-to-User IE' in 3GPP TS 24.008 ch. 10.5.4.25.
                                                      Type of user to user data. */
    cn_uint8_t      number_of_rings;              /**< Number of times a ring notification has been issued */
    cn_bool_t       updated;                      /**< Flag that when set indicates that the call has been updated with new information */
} cn_call_context_t;


/**
 * \enum  cn_net_detailed_fail_cause_class_t
 * \brief This enum has values of last net error cause type sender.
 */
typedef enum {
    CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET = 0, /* Network Reject codes  "GMMnn" */
    CN_NET_DETAILED_FAIL_CAUSE_CLASS_PSCC = 1, /* Packet Data error causes "SMnn" */
    CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS = 2, /* Supplementary Services error causes "SSnn" */
} cn_net_detailed_fail_cause_class_t;

/**
 * \enum  cn_net_gsm_cause_t
 * \brief This enum has values of last network error cause of class CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET.
 * It is based on values in 3GPP spec TS24.008 annex G
 */
typedef enum {
    CN_NET_GSM_CAUSE_IMSI_UNKNOWN_IN_HLR             = 0x02,
    CN_NET_GSM_CAUSE_ILLEGAL_MS                      = 0x03,
    CN_NET_GSM_CAUSE_IMSI_UNKNOWN_IN_VLR             = 0x04,
    CN_NET_GSM_CAUSE_IMEI_NOT_ACCEPTED               = 0x05,
    CN_NET_GSM_CAUSE_ILLEGAL_ME                      = 0x06,
    CN_NET_GSM_CAUSE_GPRS_SERVICES_NOT_ALLOWED       = 0x07,
    CN_NET_GSM_CAUSE_GPRS_AND_NON_GPRS_NA            = 0x08,
    CN_NET_GSM_CAUSE_MS_ID_CANNOT_BE_DERIVED         = 0x09,
    CN_NET_GSM_CAUSE_IMPLICITLY_DETACHED             = 0x0A,
    CN_NET_GSM_CAUSE_PLMN_NOT_ALLOWED                = 0x0B,
    CN_NET_GSM_CAUSE_LA_NOT_ALLOWED                  = 0x0C,
    CN_NET_GSM_CAUSE_ROAMING_NOT_IN_THIS_LA          = 0x0D,
    CN_NET_GSM_CAUSE_GPRS_SERV_NA_IN_THIS_PLMN       = 0x0E,
    CN_NET_GSM_CAUSE_NO_SUITABLE_CELLS_IN_LA         = 0x0F,
    CN_NET_GSM_CAUSE_MSC_TEMP_NOT_REACHABLE          = 0x10,
    CN_NET_GSM_CAUSE_NETWORK_FAILURE                 = 0x11,
    CN_NET_GSM_CAUSE_MAC_FAILURE                     = 0x14,
    CN_NET_GSM_CAUSE_SYNCH_FAILURE                   = 0x15,
    CN_NET_GSM_CAUSE_CONGESTION                      = 0x16,
    CN_NET_GSM_CAUSE_AUTH_UNACCEPTABLE               = 0x17,
    CN_NET_GSM_CAUSE_SERV_OPT_NOT_SUPPORTED          = 0x20,
    CN_NET_GSM_CAUSE_SERV_OPT_NOT_SUBSCRIBED         = 0x21,
    CN_NET_GSM_CAUSE_SERV_TEMP_OUT_OF_ORDER          = 0x22,
    CN_NET_GSM_CAUSE_NO_PDP_CONTEXT_ACTIVE           = 0x28,
    CN_NET_GSM_CAUSE_RETRY_ENTRY_NEW_CELL_LOW        = 0x30,
    CN_NET_GSM_CAUSE_RETRY_ENTRY_NEW_CELL_HIGH       = 0x3F,
    CN_NET_GSM_CAUSE_SEMANTICALLY_INCORRECT          = 0x5F,
    CN_NET_GSM_CAUSE_INVALID_MANDATORY_INFO          = 0x60,
    CN_NET_GSM_CAUSE_MSG_TYPE_NONEXISTENT            = 0x61,
    CN_NET_GSM_CAUSE_CONDITIONAL_IE_ERROR            = 0x64,
    CN_NET_GSM_CAUSE_MSG_TYPE_WRONG_STATE            = 0x65,
    CN_NET_GSM_CAUSE_PROTOCOL_ERROR_UNSPECIFIED      = 0x6F
} cn_net_gsm_cause_t;

/**
 * \enum  cn_net_gpds_cause_t
 * \brief This enum has values of last network error cause of class CN_NET_DETAILED_FAIL_CAUSE_CLASS_PSCC.
 * It is based on values in 3GPP spec TS24.008 Chapter "10.5.6.6 SM Cause" and annex I
 */
typedef enum {
    CN_NET_GPDS_CAUSE_UNKNOWN                      = 0x00,
    CN_NET_GPDS_CAUSE_IMSI                         = 0x02,
    CN_NET_GPDS_CAUSE_MS_ILLEGAL                   = 0x03,
    CN_NET_GPDS_CAUSE_IMEI_NOT_ACCEPTED            = 0x05,
    CN_NET_GPDS_CAUSE_ME_ILLEGAL                   = 0x06,
    CN_NET_GPDS_CAUSE_GPRS_NOT_ALLOWED             = 0x07,
    CN_NET_GPDS_CAUSE_NOT_ALLOWED                  = 0x08,
    CN_NET_GPDS_CAUSE_MS_IDENTITY                  = 0x09,
    CN_NET_GPDS_CAUSE_DETACH                       = 0x0A,
    CN_NET_GPDS_CAUSE_PLMN_NOT_ALLOWED             = 0x0B,
    CN_NET_GPDS_CAUSE_LA_NOT_ALLOWED               = 0x0C,
    CN_NET_GPDS_CAUSE_ROAMING_NOT_ALLOWED          = 0x0D,
    CN_NET_GPDS_CAUSE_GPRS_NOT_ALLOWED_IN_PLMN     = 0x0E,
    CN_NET_GPDS_CAUSE_NO_SUITABLE_CELLS_IN_LA      = 0x0F,
    CN_NET_GPDS_CAUSE_MSC_NOT_REACH                = 0x10,
    CN_NET_GPDS_CAUSE_PLMN_FAIL                    = 0x11,
    CN_NET_GPDS_CAUSE_MAC_FAIL                     = 0x14,
    CN_NET_GPDS_CAUSE_SYNCH_FAIL                   = 0x15,
    CN_NET_GPDS_CAUSE_NETWORK_CONGESTION           = 0x16,
    CN_NET_GPDS_CAUSE_LLC_SNDCP_FAILURE            = 0x19,
    CN_NET_GPDS_CAUSE_RESOURCE_INSUFF              = 0x1A,
    CN_NET_GPDS_CAUSE_APN                          = 0x1B,
    CN_NET_GPDS_CAUSE_PDP_UNKNOWN                  = 0x1C,
    CN_NET_GPDS_CAUSE_AUTHENTICATION               = 0x1D,
    CN_NET_GPDS_CAUSE_ACT_REJECT_GGSN              = 0x1E,
    CN_NET_GPDS_CAUSE_ACT_REJECT                   = 0x1F,
    CN_NET_GPDS_CAUSE_SERV_OPT_NOT_SUPPORTED       = 0x20,
    CN_NET_GPDS_CAUSE_SERV_OPT_NOT_SUBSCRIBED      = 0x21,
    CN_NET_GPDS_CAUSE_SERV_OPT_OUT_OF_ORDER        = 0x22,
    CN_NET_GPDS_CAUSE_NSAPI_ALREADY_USED           = 0x23,
    CN_NET_GPDS_CAUSE_DEACT_REGULAR                = 0x24,
    CN_NET_GPDS_CAUSE_QOS                          = 0x25,
    CN_NET_GPDS_CAUSE_NETWORK_FAIL                 = 0x26,
    CN_NET_GPDS_CAUSE_REACTIVATION_REQ             = 0x27,
    CN_NET_GPDS_CAUSE_FEAT_NOT_SUPPORTED           = 0x28,
    CN_NET_GPDS_CAUSE_TFT_SEMANTIC_ERROR           = 0x29,
    CN_NET_GPDS_CAUSE_TFT_SYNTAX_ERROR             = 0x2A,
    CN_NET_GPDS_CAUSE_CONTEXT_UNKNOWN              = 0x2B,
    CN_NET_GPDS_CAUSE_FILTER_SEMANTIC_ERROR        = 0x2C,
    CN_NET_GPDS_CAUSE_FILTER_SYNTAX_ERROR          = 0x2D,
    CN_NET_GPDS_CAUSE_CONT_WITHOUT_TFT             = 0x2E,
    CN_NET_GPDS_CAUSE_MULTICAST_MEMBERSHIP_TIMEOUT = 0x2F,
    CN_NET_GPDS_CAUSE_INVALID_SMS_XFER_VALUE       = 0x51, /* GSM 04.11 annex E-2 values */
    CN_NET_GPDS_CAUSE_SEMANTICALLY_INCORRECT       = 0x5F,
    CN_NET_GPDS_CAUSE_INVALID_MANDATORY_INFO       = 0x60,
    CN_NET_GPDS_CAUSE_MSG_TYPE_NOT_EXIST_OR_IMPLTD = 0x61,
    CN_NET_GPDS_CAUSE_MSG_TYPE_NOT_COMPAT_STATE    = 0x62,
    CN_NET_GPDS_CAUSE_IE_NOT_EXIST_OR_IMPLTD       = 0x63,
    CN_NET_GPDS_CAUSE_CONDITIONAL_IE_ERROR         = 0x64,
    CN_NET_GPDS_CAUSE_MSG_NOT_COMPAT_STATE         = 0x65,
    CN_NET_GPDS_CAUSE_UNSPECIFIED                  = 0x6F
} cn_net_gpds_cause_t;

/**
 * \enum  cn_net_ss_cause_t
 * \brief This enum has values of last network error cause of class CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS.
 * It is based on values in 3GPP spec as indicated.
 */
typedef enum {
    CN_NET_SS_CAUSE_UNKNOWN_ERROR              = 0x00,
    CN_NET_SS_CAUSE_UNKNOWN_SUBSCRIBER         = 0x01, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_ILLEGAL_SUBSCRIBER         = 0x09, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_BEARER_SERV_NOT_PROVISION  = 0x0A, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_TELE_SERV_NOT_PROVISION    = 0x0B, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_ILLEGAL_EQUIPMENT          = 0x0C, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_CALL_BARRED                = 0x0D, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_ILLEGAL_SS_OPERATION       = 0x10, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_SS_ERROR_STATUS            = 0x11, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_SS_NOT_AVAILABLE           = 0x12, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_SS_SUBSCRIPTION_VIOLATION  = 0x13, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_SS_INCOMPATIBILITY         = 0x14, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_FACILITY_NOT_SUPPORTED     = 0x15, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_SERVICE_REQUEST_RELEASED   = 0x16, /* From client, does not collide with 3GPP spec */
    CN_NET_SS_CAUSE_SERVICE_UNKNOWN_ERROR      = 0x17, /* From client, does not collide with 3GPP spec */
    CN_NET_SS_CAUSE_ABSENT_SUBSCRIBER          = 0x1B, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_SHORT_TERM_DENIAL          = 0x1D, /* See 3GPP TS 24.080 and 3GPP TS 29.002 */
    CN_NET_SS_CAUSE_LONG_TERM_DENIAL           = 0x1E, /* See 3GPP TS 24.080 and 3GPP TS 29.002 */
    CN_NET_SS_CAUSE_SYSTEM_FAILURE             = 0x22, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_DATA_MISSING               = 0x23, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_UNEXPECTED_DATA_VALUE      = 0x24, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_PASSWORD_REGISTR_FAILURE   = 0x25, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_NEGATIVE_PASSWORD_CHECK    = 0x26, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_MAXNUM_OF_PW_ATT_VIOLATION = 0x2B, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_UNKNOWN_ALPHABET           = 0x47, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_USSD_BUSY                  = 0x48, /* See 3GPP TS 24.080 */
    CN_NET_SS_CAUSE_NBR_SB_EXCEEDED            = 0x78, /* Obsolete */
    CN_NET_SS_CAUSE_MAXNUM_OF_MPTY_PART_EXCEED = 0x7E, /* Obsolete */
    CN_NET_SS_CAUSE_RESOURCES_NOT_AVAILABLE    = 0x7F  /* See 3GPP TS 24.080 */
} cn_net_ss_cause_t;

/**
 * \var  cn_net_cause_cause_t
 * \brief This enum type is a union of /ref cn_net_gsm_cause_t, /ref cn_net_gpds_cause_t, /ref cn_net_ss_cause_t
 */
typedef cn_sint32_t cn_net_cause_cause_t;


/**
 * \struct cn_net_detailed_fail_cause_t
 * \brief This structure contains the details of net failure reasons.
*/
typedef struct  {
    cn_net_detailed_fail_cause_class_t class; /* This implies the source; SS, NET, or PSCC */
    cn_net_cause_cause_t cause;
} cn_net_detailed_fail_cause_t;

/**
 * \struct cn_ss_error_code_t
 * \brief This enum contains SS specific error codes
*/
typedef enum {
    CN_SS_ERROR_CODE_NONE,
    CN_SS_ERROR_CODE_INTERNAL_ERROR,
    CN_SS_ERROR_CODE_CS_INACTIVE,
    CN_SS_ERROR_CODE_GSM_BAD_PASSWORD,
    CN_SS_ERROR_CODE_GSM_BAD_PASSWORD_FORMAT,
    CN_SS_ERROR_CODE_GSM_DATA_ERROR,
    CN_SS_ERROR_CODE_GSM_MSG_INCORRECT,
    CN_SS_ERROR_CODE_GSM_MM_ERROR,
    CN_SS_ERROR_CODE_GSM_REQUEST_CANCELLED,
    CN_SS_ERROR_CODE_GSM_SERVICE_NOT_ON_FDN_LIST,
    CN_SS_ERROR_CODE_GSM_SS_NOT_AVAILABLE,
    CN_SS_ERROR_CODE_GSM_USSD_BUSY,
    CN_SS_ERROR_CODE_RESOURCE_CONTROL_DENIED,
    CN_SS_ERROR_CODE_RESOURCE_CONTROL_FAILURE,
    CN_SS_ERROR_CODE_RESOURCE_CONTROL_CONF_FAIL,
    CN_SS_ERROR_CODE_SERVICE_BUSY,
    CN_SS_ERROR_CODE_SERVICE_NOT_SUPPORTED,
    CN_SS_ERROR_CODE_SERVICE_REQUEST_RELEASED,
    CN_SS_ERROR_CODE_SERVICE_UNKNOWN_ERROR
} cn_ss_error_code_t;

/**
 * \enum cn_ss_error_code_type_t
 * \brief This enum is used for determining which SS error cause that has been returned.
 */
typedef enum {
    CN_SS_ERROR_CODE_TYPE_NONE, /**< No error */
    CN_SS_ERROR_CODE_TYPE_CN,   /**< Error code as defined by cn_ss_error_code_t */
    CN_SS_ERROR_CODE_TYPE_GSM,  /**< The cause value is defined in 3GPP TS 24.080 (4.3.2)*/
    CN_SS_ERROR_CODE_TYPE_MM    /**< The cause value is defined in 3GPP TS 24.008 (10.5.3.6) */
} cn_ss_error_code_type_t;

/**
 * \enum cn_ss_error_t
 * \brief This structure contains a supplementary service error code.
*/
typedef struct {
    cn_ss_error_code_type_t cn_ss_error_code_type; /**< Type used to determine the relevant member in the cn_ss_error_value_t union */

    union cn_ss_error_value_s {
        cn_ss_error_code_t cn_ss_error_code; /* Used when ss_error_code_type is set to CN_SS_ERROR_CODE_CN */
        uint16_t cause_value;                /* 3GPP defined cause value as described by cn_ss_error_code */
    } cn_ss_error_value;

} cn_ss_error_t;

/**
 * \enum cn_response_ussd_t
 * \brief Data structure used as payload for an ussd response.
 */
typedef struct {
    cn_ss_error_t ss_error; /**< The USSD error code */
    uint32_t session_id;    /**< The unique session ID for the USSD request */
} cn_response_ussd_t;

/**
 * \enum cn_ss_status_ind_type_t
 * \brief This enum has values describing the cause of an SS status indication
*/
typedef enum {
    CN_SS_STATUS_INFO_TYPE_USSD_STOP,         /* The SS USSD request has stopped */
    CN_SS_STATUS_INFO_TYPE_USSD_FAILED,       /* The SS USSD request has failed */
    CN_SS_STATUS_INFO_TYPE_USSD_BUSY,         /* The SS USSD service is busy */
} cn_ss_status_info_type_t;

/**
 * \struct cn_ss_status_ind_t
 * \brief This struct contains the ss information sent in the SS status indication.
*/
typedef struct {
    cn_ss_status_info_type_t type;           /* The SS status indication type */
    cn_ss_error_t ss_error;                  /* The SS error code */
    uint32_t session_id;                     /* The unique session ID. Valid for USSD. */
} cn_ss_status_info_t;

/**
 * \enum  cn_notification_type_t
 * \brief This enum describe the type of supplementary service notification.
 */
typedef enum {
    CN_NOTIFICATION_TYPE_MO_INTERMEDIATE = 0, /**< MO intermediate result code */
    CN_NOTIFICATION_TYPE_MT_UNSOLICITED = 1, /**< MT unsolicited result code */
} cn_notification_type_t;


/**
 * \struct  cn_supp_svc_notification_t
 * \brief   Supplementary service notification structure.
 * \brief   This structure contains the supplementary service notification
 *          information for the network.
 */
typedef struct {
    cn_notification_type_t notification_type; /**< MO intermediate/MT unsolicited */
    cn_sint32_t code;                         /**< "code1" for MO/"code2" for MT. */
    cn_sint32_t index;                        /**< CUG index. See 27.007 7.17. */
    cn_sint32_t address_type;                 /**< Type of address, eg 145 = intl (MT-only) */
    char        number[CN_MAX_STRING_BUFF];   /**< Null-terminated number (MT-only) */
} cn_supp_svc_notification_t;


/**
 * \struct  cn_call_list_t
 * \brief   Call list structure
 * \brief   This structure contains information about the current calls.
 */
typedef struct {
    cn_uint8_t        nr_of_calls;
    cn_call_context_t call_context[1]; /* dynamic array of cn_call_context_t with the size nr_of_calls */
} cn_call_list_t;


/**
 * \enum cn_call_type_t
 * \brief This enum describes available call types.
 */
typedef enum {
    CN_CALL_TYPE_UNKNOWN,        /**< Unknown call type */
    CN_CALL_TYPE_EMERGENCY_CALL, /**< Emergency call    */
    CN_CALL_TYPE_VOICE_CALL      /**< Normal voice call */
} cn_call_type_t;


/**
 * \struct cn_dial_t
 * \brief  Call dial structure
 * \brief  This structure contains the phone number and CLIR information
 */
#define CN_BEARER_CAPABILITY_DATA_MAX_SIZE 248
typedef struct {
    cn_call_type_t call_type;     /**< Specifies intended call type (the pre-dial checker may alter the type) */
    cn_bool_t      sat_initiated; /**< true if initiated by SAT */
    char           phone_number[CN_MAX_STRING_BUFF]; /**< Null-terminated phone number string
                                                       *
                                                       * Syntax of a valid phone number:
                                                       * phone number = ['+']<digits>[<dtmf part>]
                                                       * <digits>     = (digit)+
                                                       * <digit>      = {0..9}
                                                       * <dtmf part>  = ('w'|'p')(<dtmf>)*
                                                       * <dtmf>       = (<digit>|'*'|'#'|'p')
                                                       *
                                                       * Examples: "12345678", "+4612345678", "123w456p*#", etc
                                                       */
    cn_uint8_t     clir; /**< same as 'n' parameter in TS 27.007 7.7 "+CLIR"
                          *   clir == 0 on "use subscription default value"
                          *   clir == 1 on "CLIR invocation" (restrict CLI presentation)
                          *   clir == 2 on "CLIR suppression" (allow CLI presentation)
                          */
    cn_uint8_t     bc_length; /**< Bearer Capability IE data length (3GPP TS 24.008 ch. 10.5.4.5). Refers to octet 2 in the IE. */
    cn_uint8_t     bc_data[CN_BEARER_CAPABILITY_DATA_MAX_SIZE]; /**< Bearer Capability IE data buffer (3GPP TS 24.008 ch. 10.5.4.5).
                                                                     Refers to octet 3 in the IE and onwards */
    cn_sint8_t     user_to_user[CN_MAX_UUS_BUFF]; /**< User to User IE. 3GPP TS 24.008 ch. 10.5.4.25 */
    cn_uint8_t     user_to_user_len;
} cn_dial_t;

typedef cn_uint8_t cn_rssi_t; /**< The RSSI value is normally expressed with the unit dBm. The actual value presented
                               *\n here is |x| where x is the value of RSSI. For example, if RSSI value if -84 dBm,
                               *\n the cn_rssi_t value passed will be 84.
                               *\n the typical value range is between -51 and -113 dBm.
                               */

/**
 * \enum  cn_clip_status_t
 * \brief This enum describe the CLIP status.
 */
typedef enum {
    CN_CLIP_STATUS_NOT_PROVISIONED = 0, /**< CLIP not provisioned */
    CN_CLIP_STATUS_PROVISIONED     = 1, /**< CLIP provisioned */
} cn_clip_status_t;


/**
 * \enum  cn_cnap_status_t
 * \brief This enum describe the CNAP status.
 */
typedef enum {
    CN_CNAP_STATUS_NOT_PROVISIONED = 0, /**< CNAP not provisioned */
    CN_CNAP_STATUS_PROVISIONED     = 1, /**< CNAP provisioned */
} cn_cnap_status_t;

/**
 * \enum  cn_colr_status_t
 * \brief This enum describe the COLR status.
 */
typedef enum {
    CN_COLR_STATUS_NOT_PROVISIONED = 0, /**< COLR not provisioned */
    CN_COLR_STATUS_PROVISIONED     = 1, /**< COLR provisioned */
} cn_colr_status_t;

/**
 * \enum  cn_clir_setting_t
 * \brief This enum describe the subscriber CLIR setting for outgoing calls (n value in +CLIR, 3GPP TS 27.007)
 */
typedef enum {
    CN_CLIR_SETTING_SERVICE_SUBSCRIPTION = 0, /**< presentation indicator is used according to the subscription of the CLIR service */
    CN_CLIR_SETTING_INVOCATION = 1,           /**< CLIR invocation  */
    CN_CLIR_SETTING_SUPPRESSION = 2           /**< CLIR suppression */
} cn_clir_setting_t;

/**
 * \enum  cn_sleep_mode_t
 * \brief This enum describe the modem sleep mode ON/OFF
 */
typedef enum {
    CN_SLEEP_TEST_MODE_OFF  = 0, /**< Modem Sleep Mode ON */
    CN_SLEEP_TEST_MODE_ON   = 1  /**< Modem Sleep Mode OFF */
} cn_sleep_mode_setting_t;

/**
 * \enum  cn_clir_status_t
 * \brief This enum describe the subscriber CLIR service status in the network (m value in +CLIR, 3GPP TS 27.007)
 */
typedef enum {
    CN_CLIR_STATUS_NOT_PROVISIONED           = 0, /**< CLIR not provisioned                        */
    CN_CLIR_STATUS_PROVISIONED_PERMANENT     = 1, /**< CLIR provisioned in permanent mode          */
    CN_CLIR_STATUS_UNKNOWN                   = 2, /**< unknown (e.g. no network, etc.)             */
    CN_CLIR_STATUS_TEMPARORY_MODE_RESTRICTED = 3, /**< CLIR temporary mode presentation restricted */
    CN_CLIR_STATUS_TEMPARORY_MODE_ALLOWED    = 4  /**< CLIR temporary mode presentation allowed    */
} cn_clir_status_t;


/**
 * \struct  cn_clir_t
 * \brief   CLIR setting and status
 */
typedef struct {
    cn_clir_setting_t setting; /**< CLIR setting for outgoing calls */
    cn_clir_status_t  status;  /**< CLIR service status in the network */
} cn_clir_t;


/**
 * \enum  cn_als_line_t
 * \brief This enum has possible values of ALS property setting.
 */
typedef enum {
    CN_ALS_LINE_NORMAL               = 0,            /**< ALS property LINE normal            */
    CN_ALS_LINE_TWO                  = 1             /**< ALS property LINE 2                   */
} cn_als_line_t;


/**
 * \enum  cn_supported_als_lines_t
 * \brief This enum has ALS supported lines.
 */
typedef enum {
    CN_SUPPORTED_ALS_LINE1           = 0,
    CN_SUPPORTED_ALS_LINE1_AND_LINE2 = 1
} cn_supported_als_lines_t;

/**
 * \struct  cn_modem_prop_value_als_t
 * \brief   Modem property for ALS (Alternate Line Subscription)
 */
typedef struct {
    cn_als_line_t            current_line;    /**< Current line set in modem (used by SET and GET request) */
    cn_supported_als_lines_t supported_lines; /**< Supported lines by the modem (only used in GET request, ignored in SET case) */
} cn_modem_prop_value_als_t;


/**
 * \struct  cn_modem_prop_value_clir_t
 * \brief   Modem property for CLIR (Calling Line Identification Restriction)
 */
typedef struct {
    cn_clir_setting_t clir_setting;   /* "n" parameter from 3GPP 27.007: parameter sets the adjustment for outgoing calls */
    cn_uint32_t       service_status; /* "m" parameter from 3GPP 27.007: parameter shows the subscriber CLIR service status in the network */
} cn_modem_prop_value_clir_t;         /* NOTE: the service status is not possible to set, only read. It is ignored if specified in the set request. */

/**
 * \enum  cn_modem_prop_value_ctm_t
 * \brief This enum has possible values of CTM property setting.
 */
typedef enum {
    CN_MODEM_PROP_VALUE_CTM_NOT_SUPPORTED = 0, /**< CTM property not supported */
    CN_MODEM_PROP_VALUE_CTM_SUPPORTED     = 1  /**< CTM property supported     */
} cn_modem_prop_value_ctm_t;

/**
 * \enum  cn_modem_property_type_t
 * \brief This enum has values of supported call modem property.
 */
typedef enum {
    CN_MODEM_PROPERTY_TYPE_CTM     = 0, /**< Cellular Text Telephone Modem property */
    CN_MODEM_PROPERTY_TYPE_CLIR    = 1, /**< CLIR Modem property */
    CN_MODEM_PROPERTY_TYPE_ALS     = 2, /**< ALS Modem property */
    CN_MODEM_PROPERTY_TYPE_UNKNOWN = 3  /**< Modem property Unknown */
} cn_modem_property_type_t;

/**
 * \enum  cn_comfort_tone_generation_t
 * \brief Local tone generation indication
 */
typedef enum {
    CN_COMFORT_TONE_GENERATION_STOP    = 0, /**< Stop  local generation of comfort tones */
    CN_COMFORT_TONE_GENERATION_START   = 1, /**< Start local generation of comfort tones */
} cn_comfort_tone_generation_t;


/**
 * \struct  cn_modem_property_t
 * \brief   Modem property data
 */
typedef struct {
    cn_modem_property_type_t type; /* Indicates which union member to use */
    union {
        cn_modem_prop_value_ctm_t  ctm;
        cn_modem_prop_value_clir_t clir;
        cn_modem_prop_value_als_t  als;
    } value;
} cn_modem_property_t;


/**
 * \enum  cn_cssn_setting_t
 * \brief This enum has enable/disable values for supplementary service related notifications.
 */
typedef enum {
    CN_SUPPL_SVC_NOTIFY_DISABLE   = 0, /**< Disable supplementary service related notifications     */
    CN_SUPPL_SVC_NOTIFY_ENABLE    = 1  /**< Enable supplementary service related notifications    */
} cn_cssn_setting_t;


/**
 * \enum  cn_dtmf_string_type_t
 * \brief This enum has enable/disable values for supplementary service related notifications.
 */
typedef enum {
    CN_DTMF_STRING_TYPE_ASCII   = 0, /**< ASCII byte stream */
    CN_DTMF_STRING_TYPE_BCD     = 1  /**< BCD byte stream */
} cn_dtmf_string_type_t;


/**
 * \enum  cn_call_waiting_service_t
 * \brief This enum describe call waiting settings.
 */
typedef enum {
    CN_CALL_WAITING_SERVICE_DISABLED = 0, /**< Disable Call Waiting Supplementary Service */
    CN_CALL_WAITING_SERVICE_ENABLED  = 1, /**< Enable Call Waiting Supplementary Service */
} cn_call_waiting_service_t;


/**
 * \struct cn_call_waiting_t
 * \brief Call waiting structure
 */
typedef struct {
    cn_call_waiting_service_t setting;       /**< call waiting service configuration      */
    cn_uint32_t               service_class; /**< service class info defined in TS 27.007 */
} cn_call_waiting_t;


/**
 * \enum  cn_call_barring_service_t
 * \brief This enum describe call barring settings.
 */
typedef enum {
    CN_CALL_BARRING_DEACTIVATION = 0,       /**< Disable Call barring supplementary service*/
    CN_CALL_BARRING_ACTIVATION   = 1,       /**< Enable Call barring Supplementary Service */
} cn_call_barring_service_t;


/**
 * \struct  cn_call_barring_t
 * \brief   Structure information containing SS Operation and service class.
 */
typedef struct {
    cn_call_barring_service_t   setting;        /**< ss service operation \ref  cn_call_barring_service_t */
    cn_uint32_t                 service_class;  /**<  service class as per TS 27.007 */
} cn_call_barring_t;


/**
 * \struct cn_time_info_t;
 * \brief Network Time Information
 *
 * \n This structure provides Network Time Information. It is possible that only
 * \n a subset of the information is provided. If a specific field hasn't been
 * \n supplied, CN_TIME_INFO_NOT_AVAILABLE is used to indicate this.
 */
#define CN_TIME_INFO_NOT_AVAILABLE 0x64
typedef struct {
    cn_uint8_t year;                  /**< Year;   [0-99], CN_TIME_INFO_NOT_AVAILABLE. */
    cn_uint8_t month;                 /**< Month;  [1-12], CN_TIME_INFO_NOT_AVAILABLE. */
    cn_uint8_t day;                   /**< Day;    [1-31], CN_TIME_INFO_NOT_AVAILABLE. */
    cn_uint8_t hour;                  /**< Hour;   [0-23], CN_TIME_INFO_NOT_AVAILABLE. */
    cn_uint8_t minute;                /**< Minute; [0-59], CN_TIME_INFO_NOT_AVAILABLE. */
    cn_uint8_t second;                /**< Second; [0-59], CN_TIME_INFO_NOT_AVAILABLE. */
    cn_uint8_t time_zone_sign;        /**< Time zone sign; [0-1],
                                       *  0 - Positive (+) sign.
                                       *  1 - Negative (-) sign.
                                       */
    cn_uint8_t time_zone_value;       /**< Number of quarter-hours */
    cn_uint8_t day_light_saving_time; /**< Day light saving time;
                                       * [0-2], CN_TIME_INFO_NOT_AVAILABLE:
                                       * 0 - No adjustment.
                                       * 1 - One hour adjustment.
                                       * 2 - Two hour adjustment.
                                       */
} cn_time_info_t;


/**
 * \enum  cn_ussd_type_t
 * \brief This enum describe USSD type codes.
 */
typedef enum {
    CN_USSD_TYPE_USSD_NOTIFY                  = 0, /**< USSD-Notify                                        */
    CN_USSD_TYPE_USSD_REQUEST                 = 1, /**< USSD-Request                                       */
    CN_USSD_TYPE_USSD_SESSION_TERMINATED      = 2, /**< Session terminated by network                      */
    CN_USSD_TYPE_USSD_OTHER_LOCAL_CLIENT      = 3, /**< other local client (eg, SIM Toolkit) has responded */
    CN_USSD_TYPE_USSD_OPERATION_NOT_SUPPORTED = 4, /**< Operation not supported                            */
    CN_USSD_TYPE_USSD_NETWORK_TIMEOUT         = 5  /**<  Network timeout                                   */
} cn_ussd_type_t;

/**
 * \enum cn_ussd_received_type_t
 * \brief This enum describe the ussd received type
*/
typedef enum {
    CN_USSD_RECEIVED_TYPE_GSM_USSD_REQUEST, /**< An USSD request has been received from the network */
    CN_USSD_RECEIVED_TYPE_GSM_USSD_NOTIFY,  /**< An USSD notification has been received from the network */
    CN_USSD_RECEIVED_TYPE_GSM_USSD_COMMAND, /**< An USSD command has been received from the network */
    CN_USSD_RECEIVED_TYPE_GSM_USSD_END      /**< The USSD request has been terminated by the network */
} cn_ussd_received_type_t;

/**
 * \struct  cn_ussd_info_t
 * \brief   USSD Infomation structure.
 * \brief   This structure contains the populated USSD information which are used to inform upper layers.
 */
typedef struct {
    cn_ussd_type_t type;                             /**< USSD Type (Valid for CN_RESPONSE_USSD)                                                     */
    cn_ussd_received_type_t received_type;           /**< USSD received Type (Valid for CN_RESPONSE_USSD)                                            */
    cn_uint32_t length;                              /**< USSD length (Valid for cn_request_ussd and CN_RESPONSE_USSD)                               */
    cn_uint8_t dcs;                                  /**< dcs, data coding scheme (Valid for cn_request_ussd and CN_RESPONSE_USSD)                   */
    uint32_t session_id;                             /**< Session ID of the USSD request. (Valid for CN_RESPONSE_USSD)                               */
    cn_bool_t sat_initiated;                         /**< true if initiated by SAT */
    uint8_t me_initiated;                            /**< Valid when received_type is CN_USSD_RECEIVED_TYPE_GSM_USSD_END.
                                                          Indicator of if the USSD_END was initiated by the ME or the network (0 = network, 1 == ME) */
    unsigned char ussd_string[CN_MAX_STRING_BUFF];  /**< Non-zero terminated USSD message string, if USSD type code is either 0 or 1 else NULL
                                                         (Valid for cn_request_ussd and CN_RESPONSE_USSD)                                            */
    cn_bool_t suppress_ussd_on_end;                  /**<Valid when received_type is CN_USSD_RECEIVED_TYPE_GSM_USSD_END for SAT initiated USSD */
} cn_ussd_info_t;


/**
 * \enum cn_cellular_system_command_t
 * \brief Cellular System request types
 *
 * \n This enum defines CS request types.
 */
typedef enum {
    CN_CELLULAR_SYSTEM_COMMAND_POWER_OFF       = 0x02, /**< To request to switch off the CS         */
    CN_CELLULAR_SYSTEM_COMMAND_POWER_ON        = 0x03, /**< To request to switch on the CS          */
    CN_CELLULAR_SYSTEM_COMMAND_GAN_FLIGHT_MODE = 0x05  /**< To request to switch to GAN flight mode */
} cn_cellular_system_command_t;


/**
 * \enum cn_user_status_t
 * \brief activity status
 *
 * \n This enum defines the user activity status
 */
typedef enum {
    CN_USER_STATUS_UNKNOWN  = 0, /**< Unknown user status */
    CN_USER_STATUS_ACTIVE   = 1, /**< User is active      */
    CN_USER_STATUS_INACTIVE = 2  /**< User is inactive    */
} cn_user_status_t;

/**
 * \enum cn_battery_status_t
 * \brief battery status
 *
 * \n This enum defines the battery charging status
 */
typedef enum {
    CN_BATTERY_STATUS_UNKNOWN  = 0,     /**< Unknown battery status */
    CN_BATTERY_STATUS_IN_CHARGER   = 1, /**< Battery charging */
    CN_BATTERY_STATUS_NORMAL = 2,       /**< Battery normal */
    CN_BATTERY_STATUS_LOW = 3           /**< Battery low */
} cn_battery_status_t;


/**
 * \enum cn_neighbour_rat_type_t
 * \brief RAT types associated with Neighbour Cell Information
 *
 * \n This enum defines RAT types associated with Neighbour Cell Information.
 */
typedef enum {
    CN_NEIGHBOUR_RAT_TYPE_UNKNOWN = 0, /**< RAT type unknown   */
    CN_NEIGHBOUR_RAT_TYPE_2G      = 1, /**< RAT type 2G (GSM)  */
    CN_NEIGHBOUR_RAT_TYPE_3G      = 2  /**< RAT type 3G (UMTS) */
} cn_neighbour_rat_type_t;

/**
 * \enum cn_hsxpa_mode_t
 * \brief hsxpa mode
 *
 * \n This enum defines enable/disable values for HSXPA
 */
typedef enum {
    CN_HSXPA_DISABLE = 1,   /**< Disable both HSUPA and HSDPA */
    CN_HSXPA_ENABLE = 2     /**< Enable both HSUPA and HSDPA */
} cn_hsxpa_mode_t;

/**
 * \enum cn_neigh_info_type_t
 * \brief Info types associated with Neighbour Cell Information
 *
 * \n This enum defines Info types associated with Neighbour Cell Information.
 */
typedef enum {
    CN_NET_NEIGHBOUR_CELLS_EXT       = 0x00,       /**< Only Neighbour ECID information present  */
    CN_NET_NEIGHBOUR_CELLS_BASIC_EXT = 0x01        /**< Both Neighbour cell and ECID information present */
} cn_neigh_info_type_t;

/**
 * \struct cn_neigh_umts_cell_detailed_info_t
 * \brief Detailed Information related to umts neighbour cell measurement.
 *
 * \n This structure provides detailed umts neighbour cell measurement reports.
 */
typedef struct {
    uint32_t ucid;             /**< Cell Identity.  INTEGER (0..268435455).*/
    uint16_t psc;              /**< Primary Scrambling Code.  INTEGER (0..511). */
    uint8_t  cpich_ecno;       /**< Received energy per chip/power density.INTEGER  (0..63).*/
    int8_t   cpich_rscp;       /**< Received Signal Code Power.INTEGER  (-4 to 127 ). */
    uint8_t  cpich_pathloss;   /**< Path Loss.INTEGER (46..173).*/
    uint8_t  pad1;             /**< Padding byte.*/
    uint8_t  pad2;             /**< Padding byte.*/
    uint8_t  pad3;             /**< Padding byte.*/
}cn_neigh_umts_cell_detailed_info_t;

/**
 * \struct cn_neigh_umts_uarfcn_info_t
 * \brief Information related to umts neighbour cell(Intra-Frequency and Inter-Frequency)measurement report.
 *
 * \n This structure provides umts neighbour cell measurement reports.
 */
typedef struct {
    uint16_t                                   dl_uarfcn;                             /**< Down-Link ARFCN (Absolute Radio Frequency Channel Number).INTEGER (0..16383). */
    uint8_t                                    utra_carrier_rssi;                     /**< RSSI Value of the cell.INTEGER (0..127). */
    uint8_t                                    num_of_cells;                          /**< Number of cell measurement results provided in cdma_neighbour_cell_detailed_info_t */
    cn_neigh_umts_cell_detailed_info_t         umts_cell_detailed_info[CN_UMTS_MAX_CELLS_INCLUDED];  /**< Cell measurement results. */
}cn_neigh_umts_uarfcn_info_t;

/**
 * \struct cn_neigh_umts_info_extd_t
 * \brief Extended Information related to umts neighbour cell .
 *
 * \n This structure provides umts neighbour cell extended information.
 */
typedef struct{
    uint8_t                        num_of_dl_uarfcn;                             /**< ECID Number of neighbour cells                                            */
    cn_neigh_umts_uarfcn_info_t    umts_uarfcn_info[CN_UMTS_MAX_DL_UARFCNS];       /**< umts Neighbour cell(Intra-Frequency and Inter-Frequency) Measurement Report. */
}cn_neigh_umts_info_extd_t;

/**
 * \struct cn_umts_serving_cell_info_t
 * \brief Information related to umts serving cell .
 *
 * \n This structure provides umts serving cell information.
 */
typedef struct{
    uint32_t                          current_ucid;               /**< Cell number identifier of the serving cell, i.e. UC-ID Identity.
                                                                                                                           INTEGER (0..268435455).  UC-ID is composed of RNC-ID and C-ID. */
    uint16_t                          current_mcc;                /**< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t                          current_mnc;                /**< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t                          current_psc;                /**< Primary scrambling code of serving cell.  INTEGER (0..511).*/
    uint16_t                          current_dl_uarfcn;          /**< Down-Link ARFCN (Absolute Radio Frequency Channel Number).INTEGER (0..16383). */
    uint8_t                           current_utra_carrier_rssi;  /**< RSSI Value of the cell.INTEGER (0..127). */
    uint8_t                           current_cpich_ecno;         /**< Received energy per chip/power density.INTEGER  (0..63).*/
    int8_t                            current_cpich_rscp;         /**< Received Signal Code Power.INTEGER  (-4 to 127 ). */
    uint8_t                           current_cpich_pathloss;     /**< Path Loss.INTEGER (46..173).*/
}cn_umts_serving_cell_info_t;

/**
 * \struct cn_umts_cell_info_t
 * \brief Information related to the umts serving and neighbouring cell
 *
 * \n This structure provides umts cell information for the serving cell and neighbouring cell.
 */
typedef struct {
    cn_umts_serving_cell_info_t   umts_serving_cell_info;   /**< UMTS serving cell info*/
    cn_neigh_umts_info_extd_t     umts_info_extd;           /**< UMTS ECID info */
}cn_umts_cell_info_t;


/**
 * \struct cn_neigh_gsm_nmr_info_extd_t
 * \brief Extended Information related to ECID GSM Neighbour cell Measurement Reports
 *
 * \n This structure provides extended gsm neighbour cell measurement reports.
 */
typedef struct {
    uint16_t arfcn;            /**< ARFCN - Absolute Radio Frequency Channel Number.  INTEGER (0..1023). */
    uint8_t  bsic;             /**< BSIC  - Base Station Identity Code.  INTEGER (0..63). */
    int8_t   rxlev;            /**< RXLEV - Received signal Level.  INTEGER (0..63). */
}cn_neigh_gsm_nmr_info_extd_t;

/**
 * \struct cn_gsm_neigh_info_extd_t
 * \brief Extended Information related to ECID GSM Neighbour cell
 *
 * \n This structure provides extended gsm neighbour cell info.
 */
typedef struct{
    uint8_t                            num_of_neighbours;                              /**< ECID Number of neighbour cells */
    cn_neigh_gsm_nmr_info_extd_t       gsm_nmr_info_extd[CN_GSM_MAX_NEIGHBOUR_CELLS]; /**< GSM Neighbour cell Measurement Reports. The number of which is provided in ecid_num_of_neighbours. */
}cn_gsm_neigh_info_extd_t;

/**
 * \struct cn_neigh_gsm_nmr_info_t
 * \brief Information related to GSM Neighbour cell measurements
 *
 * \n This structure provides gsm neighbour cell measurements report.
 */
typedef struct {
    uint16_t               mcc;               /**< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t               mnc;               /**< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t               lac;               /**< Location Area Code of the serving cell.  INTEGER (0..65535). */
    uint16_t               cid;               /**< GSM Cell number identifier of the serving cell. INTEGER (0..65535).*/
    int8_t                 rxlev;             /**< RXLEV - Received signal Level.  INTEGER (0..63). */
    uint8_t                pad1;              /**< Padding byte.*/
    uint8_t                pad2;              /**< Padding byte.*/
    uint8_t                pad3;              /**< Padding byte.*/
}cn_neigh_gsm_nmr_info_t;

/**
 * \struct cn_gsm_neigh_info_basic_t
 * \brief Basic Information related to GSM Neighbour cell
 *
 * \n This structure provides basic gsm neighbour cell info.
 */
typedef struct{
    uint8_t                        num_of_neighbours;                         /**< Number of neighbour GSM cells */
    cn_neigh_gsm_nmr_info_t        gsm_nmr_info[CN_GSM_MAX_NEIGHBOUR_CELLS]; /**< GSM Neighbour cell Measurement Reports. The number of which is provided in ecid_num_of_neighbours. */
}cn_gsm_neigh_info_basic_t;

/**
 * \struct cn_gsm_serving_cell_info_t
 * \brief Information related to the gsm serving cell
 *
 * \n This structure provides gsm cell information for the serving cell.
 */
typedef struct{
    uint16_t    current_mcc;               /**< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t    current_mnc;               /**< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t    current_lac;               /**< Location Area Code of the serving cell.  INTEGER (0..65535). */
    uint16_t    current_cid;               /**< GSM Cell number identifier of the serving cell. INTEGER (0..65535).*/
    uint16_t    current_arfcn;             /**< ARFCN - Absolute Radio Frequency Channel Number.  INTEGER (0..1023). */
    uint8_t     current_bsic;              /**< BSIC  - Base Station Identity Code.  INTEGER (0..63). */
    int8_t      current_rxlev;             /**< RXLEV - Received signal Level.  INTEGER (0..63). */
    uint8_t     current_timing_advance;    /**< Network Timing Advance. INTEGER (0..255).*/
    uint8_t     pad1;                      /**< Padding byte.*/
    uint8_t    pad2;                       /**< Padding byte.*/
    uint8_t    pad3;                       /**< Padding byte.*/

}cn_gsm_serving_cell_info_t;

/**
 * \struct cn_gsm_cell_info_t
 * \brief Information related to the gsm serving and neighbouring cells
 *
 * \n This structure provides gsm cell information for the serving cell and neighbouring cells.
 */
typedef struct {
    cn_gsm_serving_cell_info_t   gsm_serving_cell_info;  /**< GSM Serving cell information */
    cn_neigh_info_type_t         neigh_info_type;        /**< Type to check which structure is filled Neighbour Info or ECID Information*/
    cn_gsm_neigh_info_basic_t    gsm_neigh_info_basic;   /**< GSM Neighbour cells information */
    cn_gsm_neigh_info_extd_t     gsm_neigh_info_extd;    /**< Extended GSM Neighbour cells information or ECID Information */
}cn_gsm_cell_info_t;

/**
 * \struct cn_neighbour_cells_info_t
 * \brief Detailed Information related to gsm/umts serving and neighbour cells
 *
 * \n This structure provides cell information for gsm/umts serving cell and all neighbour cells.
 */
typedef struct {
    cn_neighbour_rat_type_t          rat_type;           /**< RAT type associated with neighbour cell information                  */
    union{
        cn_gsm_cell_info_t           gsm_cells_info;     /**< Information related to the gsm serving and neighbouring cell    */
        cn_umts_cell_info_t          umts_cells_info;    /**< Information related to the umts serving and neighbouring cell   */
    }cells_info;
} cn_neighbour_cells_info_t;

/**
 * \struct cn_signal_info_t
 * \brief RSSI and BER Information related to current cell
 *
 * \n This structure provides RSSI and BER Information related to current cell.
 */
typedef struct {
    cn_uint8_t rssi_dbm; /**< |x|, where x is RSSI in dBm. (E.g.: If RSSI value
                        * if -84 dBm, rssi_dbm will be 84.
                        * Valid range: -123 <= x <= -1; Unknown: x == 0)
                        * NOTE : For backward compatibility reasons this parameter
                        * needs to always be first in this structure and of the
                        * same type as cn_rssi_t, i.e. cn_uint8_t
                        */
    cn_uint8_t no_of_signal_bars; /**< Number of signal bars; Range: 0 to 100 */
    cn_uint8_t ber;      /**< Bit Rate Error; Valid range: 0 to 7; Undefined: 0x63 */
    cn_uint8_t num_of_segments; /**< num_of_segment is equal to
                               * (Number of border values -1).
                               * Ignore num_of_segments and border[] if
                               * num_of_segments is 0.
                               */
    cn_uint8_t ecno;     /**< EcNo parameter; Valid range: 0 dB to 49 dB; */
    cn_uint8_t border[CN_RSSI_CONF_MAX_SEGMENTS+1];   /**< border values passed to CLIENT */
    cn_uint8_t rat;      /**< RAT info; */
} cn_signal_info_t;


/**
 * \enum cn_call_forwarding_reason_t
 * \brief Call forwarding reason
 *
 * \n This enum defines enable/disable values for HSXPA
 */
typedef enum {
    CN_CALL_FORWARDING_REASON_UNCONDITIONAL                   = 0, /**< Unconditional call forwarding                    */
    CN_CALL_FORWARDING_REASON_MOBILE_BUSY                     = 1, /**< Mobile busy                                      */
    CN_CALL_FORWARDING_REASON_NO_REPLY                        = 2, /**< No reply                                         */
    CN_CALL_FORWARDING_REASON_NOT_REACHABLE                   = 3, /**< Not reachable                                    */
    CN_CALL_FORWARDING_REASON_ALL_CALL_FORWARDING             = 4, /**< All call forwarding (3GPP TS 22.030)             */
    CN_CALL_FORWARDING_REASON_ALL_CONDITIONAL_CALL_FORWARDING = 5  /**< All conditional call forwarding (3GPP TS 22.030) */
} cn_call_forwarding_reason_t;

/**
 * \struct  cn_call_forward_info_t
 * \brief   Information related to call forwarding
 *
 * \brief   This structure contains the CallForward information
 */
typedef struct {
    cn_sint32_t status; /**<
                          * For \ref cn_request_query_call_forward request
                          * status is not used
                          *
                          * For \ref cn_request_set_call_forward request
                          * status is:
                          * 0 = deactivation
                          * 1 = activation
                          * 2 = interrogation
                          * 3 = registration
                          * 4 = erasure */
    cn_call_forwarding_reason_t reason;                     /**< from TS 27.007 7.11 "reason"                                         */
    cn_sint32_t                 service_class;              /**< From 27.007 +CCFC/+CLCK "class". Zero means user doesn't input class */
    cn_sint32_t                 toa;                        /**< "type" from TS 27.007 7.11                                           */
    char                        number[CN_MAX_STRING_SIZE]; /**< "number" from TS 27.007 7.11. May be NULL                            */
    cn_sint32_t                 time_seconds;               /**< for CF no reply only, Valid values are 5 | 10 | 15 | 20 | 25 | 30
                                                                 for other CF reason, set to 0                                        */
} cn_call_forward_info_t;


/**
 * \struct  cn_call_forward_info_list_t
 * \brief   Call Forward Infomation list.
 * \brief   This structure contains the populated array with Call Forward information
 */
typedef struct {
    int32_t                 num;              /**< Index count to array of /ref cn_call_forward_info_t
                                                   for each distinct registered phone number */
    cn_call_forward_info_t  call_fwd_info[1]; /**< /ref cn_call_forward_info_t  */
} cn_call_forward_info_list_t;


/**
 * Define for MAX number of Segments Supported for RSSI Configuration
 */
#define CN_RSSI_CONF_MAX_SEGMENTS 25

/**
 * \struct cn_signal_info_config_t
 * \brief  Configure the sending of CN_EVENT_SIGNAL_INFO
 *
 * \n This message is used to configure the sending of CN_EVENT_SIGNAL_INFO.
 *
 * \n border[CN_RSSI_CONF_MAX_SEGMENTS+1] is the array of border values specified
 * \n by the CLIENT which defines boundaries for different Segments.
 * \n Whenever a Segment Boundary is crossed we get a Rssi Indication.
 *
 * \n For CN CLIENT num_of_segment is equal to (Number of border values -1)
 * \n num_of_segments and border[] must be ignored if num_of_segments is 0.
 * \n Meaning of num_of_segments being 0 is that the information is not available.
 *
 * \n NOTE: Currently signal_level_type will be hardcoded to DBM in CN. (TBD)
 */

typedef struct {
    cn_uint8_t num_of_segments;                     /**< No. of Segment: 1 to 25, num_of_segment is
                                                      * equal to (Number of border values -1)
                                                      */
    cn_uint8_t signal_level_type;                   /**< 1 - DBm; 2 - PERCENT */
    cn_uint8_t hysteresis_down;                     /**< Hysteresis down: 0 to 20 */
    cn_uint8_t hysteresis_up;                       /**< Hysteresis up: 0 to 20  */
    cn_uint8_t border[CN_RSSI_CONF_MAX_SEGMENTS+1]; /**< border values passed by CLIENT */
    cn_uint8_t ccsq_mode;                           /**< 0 - Disable; 1 - Enable; 255 - Not set */
} cn_signal_info_config_t;


/**
 * \enum cn_rssi_mode_t
 * \brief RSSI event reporting enable/disable
 *
 * \n This enum defines RSSI event reporting enable/disable modes.
 */
typedef enum {
    CN_RSSI_EVENT_REPORTING_DISABLE      = 0x0,
    CN_RSSI_EVENT_REPORTING_ENABLE       = 0x1
} cn_rssi_mode_t;


/**
 * \enum cn_encsq_mode_t
 * \brief ENCSQ mode
 *
 * \n This enum defines ENCSQ enable/disable modes.
 */
typedef enum {
    CN_ENCSQ_DISABLE        = 0x0,
    CN_ENCSQ_ENABLE         = 0x1
} cn_encsq_mode_t;


/**
 * \enum cn_event_reporting_type_t
 * \brief event reporting types
 *
 * \n This enum defines events that can be enabled/disabled.
 */
typedef enum {
    CN_MODEM_REGISTRATION_STATUS         = 0x00, /* CN_EVENT_MODEM_REGISTRATION_STATUS */
    CN_EVENT_REPORTING_TYPE_TIME         = 0x02, /* CN_EVENT_TIME_INFO    */
    CN_EVENT_REPORTING_TYPE_RAB_STATUS   = 0x03, /* CN_EVENT_RAB_STATUS   */
    CN_EVENT_REPORTING_TYPE_NETWORK_INFO = 0x04, /* CN_EVENT_NETWORK_INFO */
    CN_EVENT_REPORTING_TYPE_RAT_NAME     = 0x08  /* CN_EVENT_RAT_NAME     */
} cn_event_reporting_type_t;


/**
 * \enum cn_rab_status_t
 * \brief RAB status type definition
 *
 * \n This enum defines the RAB status
 */
typedef enum {
    CN_RAB_STATUS_NEITHER_HSDPA_HSUPA = 0,
    CN_RAB_STATUS_HSDPA               = 1,
    CN_RAB_STATUS_HSUPA               = 2,
    CN_RAB_STATUS_BOTH_HSDPA_HSUPA    = 3
} cn_rab_status_t;


/**
 * \struct cn_network_names_t
 * \brief Network names
 *
 * \n This enum defines the available network names.
 */
typedef enum {
    CN_NETWORK_NAME_LONG = 0,
    CN_NETWORK_NAME_SHORT = 1,
    CN_NETWORK_NAME_COUNT = 2,
} cn_network_names_t;


/**
 * \struct cn_network_name_dcs_t
 * \brief Network name data coding scheme
 *
 * \n This enum defines the data coding schemes for the network names.
 * \n Aligned with 3GPP TS 24.008 Network Name information element, table 10.5.94
 */
typedef enum {
    CN_NETWORK_NAME_DCS_GSM7 = 0,
    CN_NETWORK_NAME_DCS_UCS2 = 1,
} cn_network_name_dcs_t;

/**
 * \struct cn_network_name_info_t
 * \brief Network name information data
 *
 * \n This structure provides network name information data.
 */
typedef struct {
    char mcc_mnc[CN_MCC_MNC_STRING_LENGTH]; /**< NULL terminated string presenting MCC and MNC         */
    struct {
        char text[CN_MAX_STRING_BUFF];      /**< String representing long or short operator name  */
        cn_uint8_t length;                  /* Length text string in bytes */
        cn_bool_t  add_ci;                  /* Add country initials to text string */
        cn_network_name_dcs_t dcs;          /* Data Coding Scheme for text string */
        cn_uint8_t spare;                   /* Number of spare bits in last byte. */
    } name[CN_NETWORK_NAME_COUNT];
} cn_network_name_info_t;


/**
 * \brief Baseband version string
 */
typedef char cn_baseband_version_t[CN_MAX_STRING_BUFF]; /* NULL terminated string */

/**
 * \struct  cn_pp_flag_t
 * \brief   PP flags structure, pp_feature identifies the flag.
 */
typedef struct {
    cn_uint16_t pp_value;
    cn_uint16_t pp_feature;
} cn_pp_flag_t;

/**
 * \struct  cn_pp_flag_list_t
 * \brief   Read PP flags response structure.
 */
typedef struct {
    cn_uint16_t count;
    cn_pp_flag_t info[1]; /* Dynamic size */
} cn_pp_flag_list_t;

/**
 * \enum cn_emergency_call_services_t
 * \brief Emergency call service flags
 *
 * \n This enum describe services in \ref cn_emergency_call_service_bf_t.
 */
typedef enum {
    CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED     = 0x0,
    CN_EMERGENCY_CALL_SERVICE_POLICE          = 0x1,
    CN_EMERGENCY_CALL_SERVICE_AMBULANCE       = 0x2,
    CN_EMERGENCY_CALL_SERVICE_FIRE_BRIGADE    = 0x4,
    CN_EMERGENCY_CALL_SERVICE_MARINE_GUARD    = 0x8,
    CN_EMERGENCY_CALL_SERVICE_MOUNTAIN_RESCUE = 0x10
} cn_emergency_call_services_t;

typedef cn_uint8_t cn_emergency_call_service_bf_t; /* bit field type */

/**
 * \enum  cn_emergency_number_operation_t
 * \brief This enum has operation performed on emergency numbers.
 */
typedef enum {
    CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST = 0,
    CN_EMERGENCY_NUMBER_OPERATION_ADD        = 1,
    CN_EMERGENCY_NUMBER_OPERATION_REMOVE     = 2
} cn_emergency_number_operation_t;

/**
 * \struct  cn_emergency_number_config_t
 * \brief   Emergency number request structure.
 */
typedef struct {
    cn_emergency_call_service_bf_t service_type;                /**< Emergency call service type                           */
    char emergency_number[CN_EMERGENCY_NUMBER_STRING_LENGTH+1]; /**< Emergency number, NULL terminated char array          */
    char mcc[CN_MCC_STRING_LENGTH+1];                           /**< Mobile Country Code, Null terminated char array       */
    char mcc_range[CN_MCC_RANGE_STRING_LENGTH+1];               /**< Mobile Country Code Sequence char, ranges: '2' to '9' */
} cn_emergency_number_config_t;

/**
* \struct  cn_emergency_number_t
* \brief   A stored emergency number.
*/
typedef struct {
    cn_emergency_call_service_bf_t service_type;                /**< Emergency call service type; bitfield        */
    char emergency_number[CN_EMERGENCY_NUMBER_STRING_LENGTH+1]; /**< Emergency number, NULL terminated char array */
} cn_emergency_number_t;

/**
* \struct  cn_emergency_number_list_t
* \brief   List of stored emergency numbers.
*/
typedef struct cn_emergency_number_list_struct {
    cn_uint8_t  num_of_emergency_numbers;      /**< Number of emergency numbers */
    cn_emergency_number_t emergency_number[1]; /**< Emergency number, Dynamic array with the length num_of_emergency_numbers */
} cn_emergency_number_list_t;

/**
 * \enum cn_nmr_rat_type_t
 * \brief RAT type for NMR retrieval
 *
 * \n This enum defines the RAT type for NMR retrieval.
 */
typedef enum  {
    CN_NMR_RAT_TYPE_GERAN = 0x01, /**< RAT type GERAN */
    CN_NMR_RAT_TYPE_UTRAN  = 0x02  /**< RAT type UTRAN */
} cn_nmr_rat_type_t;

/**
 * \enum cn_nmr_utran_type_t
 * \brief NMR types
 *
 * \n This enum defines NMR types.
 */
typedef enum  {
    CN_NMR_UTRAN_TYPE_INTRA_FREQ_NMR = 0x02, /**< Intra-frequency NMR */
    CN_NMR_UTRAN_TYPE_INTER_FREQ_NMR = 0x03, /**< Inter-frequency NMR */
    CN_NMR_UTRAN_TYPE_INTER_RAT_NMR  = 0x04  /**< Inter-RAT NMR */
} cn_nmr_utran_type_t;

/**
 * \struct cn_nmr_info_t
 * \brief NMR information received from modem
 *
 * \n This structure provides NMR information received from modem.
 */
typedef struct {
    cn_nmr_rat_type_t rat; /**< RAT type of the measurement */

    union {
        struct {
            cn_uint8_t  measurement_results[CN_MAX_NMR_RES_SIZE]; /** Measurement Results.
                                             * The contents are equal to Measurements Results
                                             * information element starting at octet 2. The
                                             * description can be found in GSM TS 24.008.
                                             */
            cn_uint8_t  num_of_channels;      /**< Number of BCCH channels: 0 to 32 */
            cn_uint16_t ARFCN_list[CN_MAX_NMR_ARFCN_SIZE];          /**< an array uint16_t[num_of_channels] */
        } geran_info;  /** Applicable if rat is MAL_NET_NMR_RAT_GERAN */

        struct {
            cn_uint8_t  mm_report_len;        /**< Length of Measurement Report */
            cn_uint8_t  measurements_report[CN_MAX_NMR_REPORT_SIZE]; /**< Measurement Report.
                                             * The contents are equal to Measurements Report
                                             * coded as RRC:UL-DCCH message. The description
                                             * can be found in 3GPP TS 25.331.
                                             */
        } utran_info; /** Applicable if rat is MAL_NET_NMR_RAT_UTRAN */
    } nmr_info;
} cn_nmr_info_t;

/**
 * \enum cn_timing_advance_validity_t
 * \brief Timing advance information validity
 *
 * \n This enum defines timing advance information validity types.
 */
typedef enum {
    CN_TIMING_ADVANCE_NOT_VALID = 0x00, /**< Timing advance not valid */
    CN_GSS_TIMING_ADVANCE_VALID = 0x01  /**< Timing advance valid */
} cn_timing_advance_validity_t;

/**
 * \struct cn_timing_advance_validity_t
 * \brief Timing advance information
 *
 * \n This structure defines timing advance information.
 */
typedef struct {
    cn_timing_advance_validity_t    validity; /**< Validity of information */
    cn_uint8_t                      value;    /**< Timing advance value:
                                                * Valid range: 0 to 63;
                                                * 0xFF - Not available
                                                */
} cn_timing_advance_info_t;

/**
 * Maximum length of MMI-string in bytes.
 */
#define CN_SS_MAX_MMI_STRING_LENGTH   (80)

/**
 * ASCII coded MMI NUL-terminated character sequence.
 */
typedef cn_uint8_t cn_ss_mmi_string_t[CN_SS_MAX_MMI_STRING_LENGTH + 1];

/**
 * \enum cn_ss_ton_t
 * \brief Type used to specify Type Of Number (TON)
 *
 * @param CN_SS_TON_UNKNOWN               0000
 * @param CN_SS_TON_INTERNATIONAL         0001
 * @param CN_SS_TON_NATIONAL              0010
 * @param CN_SS_TON_NETWORK_SPECIFIC      0011
 * @param CN_SS_TON_DEDICATED_PAD_ACCESS  0100
 * @param CN_SS_TON_ALPHANUMERIC          0101
 * @param CN_SS_TON_ABBREVIATED           0110
 * @param CN_SS_TON_RESERVED_EXT          0111
 * @param CN_SS_TON_NOT_A_DIALLING_NUMBER 0111
 */
typedef enum {
    CN_SS_TON_UNKNOWN               = 0x00,
    CN_SS_TON_INTERNATIONAL         = 0x01,
    CN_SS_TON_NATIONAL              = 0x02,
    CN_SS_TON_NETWORK_SPECIFIC      = 0x03,
    CN_SS_TON_DEDICATED_PAD_ACCESS  = 0x04,
    CN_SS_TON_ALPHANUMERIC          = 0x05,
    CN_SS_TON_ABBREVIATED           = 0x06,
    CN_SS_TON_RESERVED_EXT          = 0x07,
    CN_SS_TON_NOT_A_DIALLING_NUMBER = 0x07
} cn_ss_ton_t;

/**
 * \enum cn_ss_npi_t
 * \brief Type used to specify Numbering Plan Id (NPI)
 *
 * @param CN_SS_NPI_UNKNOWN               0000
 * @param CN_SS_NPI_ISDN                  0001
 * @param CN_SS_NPI_DATA                  0011
 * @param CN_SS_NPI_TELEX                 0100
 * @param CN_SS_NPI_SC_SPECIFIC_1         0101
 * @param CN_SS_NPI_SC_SPECIFIC_2         0110
 * @param CN_SS_NPI_NATIONAL              1000
 * @param CN_SS_NPI_PRIVATE               1001
 * @param CN_SS_NPI_ERMES                 1010
 * @param CN_SS_NPI_RESERVED_EXT          1111
 * @param CN_SS_NPI_NOT_A_DIALLING_NUMBER 1111
 */
typedef enum {
    CN_SS_NPI_UNKNOWN               = 0x00,
    CN_SS_NPI_ISDN                  = 0x01,
    CN_SS_NPI_DATA                  = 0x03,
    CN_SS_NPI_TELEX                 = 0x04,
    CN_SS_NPI_SC_SPECIFIC_1         = 0x05,
    CN_SS_NPI_SC_SPECIFIC_2         = 0x06,
    CN_SS_NPI_NATIONAL              = 0x08,
    CN_SS_NPI_PRIVATE               = 0x09,
    CN_SS_NPI_ERMES                 = 0x0A,
    CN_SS_NPI_RESERVED_EXT          = 0x0F,
    CN_SS_NPI_NOT_A_DIALLING_NUMBER = 0x0F
} cn_ss_npi_t;

/**
 * \struct cn_ss_command_t
 *
 * \brief A supplementary service command to be interpreted by CNS and sent to network.
 *
 * @param mmi_string          Is an ASCII coded and normally NUL-terminated
 *                            character sequence for control of a supplementary
 *                            service.
 *
 * @param mmi_string_length   Value indicates length of mmi_string in
 *                            case the string is not NUL-terminated.
 *                            The parameter mmi_string should normally be
 *                            NUL-terminated and mmi_string_length 0.
 *
 * @param sat_initiated       If TRUE, the FDN list shall not be checked and the last
 *                            number dialed shall not be updated.
 *
 * @param ton_npi_used        TRUE when type_of_number and numbering_plan_id is used
 *                            to associate with a phone number in mmi_string.
 *                            ton_npi_used should normally be set to FALSE,
 *                            - Type Of Number for a phone number included in
 *                            mmi_string will then be set to either CN_SS_TON_INTERNATIONAL
 *                            or CN_SS_TON_UNKNOWN, depending on whether the phone number
 *                            starts with "+" or not.
 *                            Numbering Plan Identity for a phone number
 *                            included in MMI_String will be set to CN_SS_NPI_ISDN,
 *                            when ton_npi_used is FALSE.
 *
 * @param type_of_number      Is valid only when ton_npi_used is TRUE.
 *                            When valid, this Type Of Number will be associated with a
 *                            phone number included in mmi_string.
 *
 * @param numbering_plan_id   Is valid only when ton_npi_used is TRUE.
 *                            When valid, this Numbering Plan Identity will be associated
 *                            with a phone number included in mmi_string.
 *
 * @param additional_results  If set to true, result data (still packed)
 *                            shall be provided from the network.
 */
typedef struct {
    cn_ss_mmi_string_t mmi_string;
    cn_uint8_t         mmi_string_length;
    cn_bool_t          sat_initiated;
    cn_bool_t          ton_npi_used;
    cn_ss_ton_t        type_of_number;
    cn_ss_npi_t        numbering_plan_id;
    cn_bool_t          additional_results;
} cn_ss_command_t;

/**
 * \brief Maximum length (in bytes) of Additional Result Data
 */
#define CN_SS_MAX_ADDITIONAL_RESULT_LENGTH (170)

/**
 * \struct cn_ss_additional_result_t
 * \brief Result data from the network in a still packed format.
 *
 * When additional_results is set to true in cn_ss_command_t additional
 * results will be provided in this struct.
 *
 * @param length                  Is the number of bytes in additional_result_data.
 * @param additional_result_data  Is the raw result byte string as received
 *                                from the network, where the first byte is
 *                                the operation code.
 */
typedef struct {
    cn_uint8_t length;
    cn_uint8_t additional_result_data[CN_SS_MAX_ADDITIONAL_RESULT_LENGTH];
} cn_ss_additional_result_t;

/**
 * \struct cn_ss_response_t
 * \brief Result data for ss requests.
 *
 * @param error_code              Network error code if failure.
 * @param additional_result       Raw result data from network.
 */
typedef struct {
    cn_ss_error_t             error_code;
    cn_ss_additional_result_t additional_result;
} cn_ss_response_t;

/**
 * \enum cn_service_type_after_sat_cc_t
 * \brief Type of service after SAT CC
 *
 * @param CN_SERVICE_TYPE_AFTER_SAT_CC_UNKNOWN
 * @param CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL
 * @param CN_SERVICE_TYPE_AFTER_SAT_CC_SS
 * @param CN_SERVICE_TYPE_AFTER_SAT_CC_USSD
 */
typedef enum {
    CN_SERVICE_TYPE_AFTER_SAT_CC_UNKNOWN,
    CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL,
    CN_SERVICE_TYPE_AFTER_SAT_CC_SS,
    CN_SERVICE_TYPE_AFTER_SAT_CC_USSD
} cn_service_type_after_sat_cc_t;


/**
 * \struct cn_response_dial_t
 * \brief Response data related to the CN_RESPONSE_DIAL message.
 *
 * @param service_type            The final service type after SAT CC (relevant for modification of service case).
 * @param data                    Response data specific to the service type.
 */
typedef struct {
    cn_service_type_after_sat_cc_t service_type;
    union {
        cn_uint8_t call_id; /* relevant when the service type is VOICE_CALL */
    } data;
} cn_response_dial_t;

/**
 * \enum cn_operator_foramt_t
 * \brief Operator name format
 *
 * @param CN_OPERATOR_FORMAT_LONG
 * @param CN_OPERATOR_FORMAT_SHORT
 * @param CN_OPERATOR_FORMAT_NUMERIC
 * @param CN_OPERATOR_FORMAT_UNKNOWN
 */
typedef enum {
    CN_OPERATOR_FORMAT_LONG,
    CN_OPERATOR_FORMAT_SHORT,
    CN_OPERATOR_FORMAT_NUMERIC,
    CN_OPERATOR_FORMAT_UNKNOWN
} cn_operator_format_t;

/**
 * \struct cn_cops_request_t
 * \brief Request data for network registration.
 *
 * @param format                Format of the operator name
 * @param act                   Access technology
 * @param operator              Operator name
 */
typedef struct {
    cn_operator_format_t format;
    uint8_t act;
    char operator[CN_NETWORK_OPERATOR_MAX_LONG_LEN + 1];
} cn_network_registration_req_data_t;

/**
 * \enum cn_rat_name_t
 * \brief RAT Name.
 *
 * @param CN_RAT_NAME_UNKNOWN
 * @param CN_RAT_NAME_GSM
 * @param CN_RAT_NAME_UMTS
 * @param CN_RAT_NAME_EPS
 */
typedef enum {
    CN_RAT_NAME_UNKNOWN = 0,
    CN_RAT_NAME_GSM     = 1,
    CN_RAT_NAME_UMTS    = 2, /* W-CDMA */
    CN_RAT_NAME_EPS     = 4  /* Evolved Packet System - LTE */
} cn_rat_name_t;

/**
 * \enum cn_tx_back_off_event_t
 * \brief Event intended for the modem so that TX power reduction
 * can be activated/deactivated.
 */
typedef enum {
    CN_TX_BACK_OFF_EVENT_NO_EVENT                        = 0,
    CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_CLOSED     = 1,
    CN_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_OPEN       = 2,
    CN_TX_BACK_OFF_EVENT_DVBH_INACTIVE                   = 3,
    CN_TX_BACK_OFF_EVENT_DVBH_ACTIVE                     = 4,
    CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_FAR            = 5,
    CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR           = 6,
    CN_TX_BACK_OFF_EVENT_GPS_INACTIVE                    = 7,
    CN_TX_BACK_OFF_EVENT_GPS_ACTIVE                      = 8,
    CN_TX_BACK_OFF_EVENT_ANTENNA_INACTIVE                = 9,
    CN_TX_BACK_OFF_EVENT_ANTENNA_ACTIVE                  = 10,
    CN_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_INACTIVE     = 11,
    CN_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_ACTIVE       = 12,
    CN_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_INACTIVE           = 13,
    CN_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE             = 14,
    CN_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_INACTIVE = 15,
    CN_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_ACTIVE   = 16,
    CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_INACTIVE      = 17,
    CN_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_ACTIVE        = 18
} cn_tx_back_off_event_t;

/**
 * \struct cn_empage_info_t
 * \brief Engineer mode information
 *
 * \n This structure provides engineer mode information.
 */
typedef struct {
    int32_t page;  /* Page */
    int32_t seqnr; /* Sequence number */
} cn_empage_info_t;

/**
 * \struct cn_empage_t
 * \brief Engineer Mode configuration data
 *
 * \n This structure provides engineering mode configuration data.
 */
typedef struct {
    int32_t mode;   /**< Mode configuration */
    int32_t page;   /**< Page configuration */
    int32_t timer;  /**< Timer configuration*/
} cn_empage_t;

/* Abort compilation if the size of an enum isn't as expected. It is up to each client module to enforce the size (-fno-short-enums) */
C_ASSERT(sizeof(cn_rf_status_t) == 4, size_of_enums);

#pragma pack(pop) /* go back to previous packing setting */

/**
 * \enum cn_call_forwarding_status_t
 * \brief Call forwarding status
 *
 * \n This enum defines status values for call forwarding
 */
typedef enum {
    CN_CALL_FORWARDING_STATUS_ACTIVATION                    = 1, /* Activation */
    CN_CALL_FORWARDING_STATUS_DEACTIVATION                  = 2, /* Deactivation */
    CN_CALL_FORWARDING_STATUS_REGISTRATION                  = 3, /* Registration */
    CN_CALL_FORWARDING_STATUS_ERASURE                       = 4, /* Erasure */
    CN_CALL_FORWARDING_STATUS_INTERROGATION                 = 5, /* Interrogation */
    CN_CALL_FORWARDING_STATUS_GSM_PASSWORD_REGISTRATION     = 6  /* GSM password registration */
} cn_call_forwarding_status_t;

#endif /* __cn_data_types_h__ */
