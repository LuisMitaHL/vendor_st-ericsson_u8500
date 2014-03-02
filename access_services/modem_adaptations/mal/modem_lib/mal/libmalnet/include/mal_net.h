/**
 * \file mal_net.h
 * \brief This file includes headers for NET MAL.
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL NET header file for NET Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v0.21   Adds support for Production Carrier Set/Clear
 * \n
 * \n v0.20   Adds support for query of current RAT name.
 * \n         Adds support for RAT name indication.
 * \n
 * \n v0.19   Add support for configuring the frequency of
 * \n         NET_MODEM_REG_STATUS_IND
 * \n
 * \n v0.18   Add support for RAT parameter during Manual Search
 * \n         Modify mal_net_manual_register() API.
 * \n
 * \n v0.17   Implements request for cell info
 * \n
 * \n v0.16   Implements unsolicited cell info indication
 * \n
 * \n v0.15   Implements API for NMR retrieval
 * \n
 * \n v0.14   Implements network configuration API
 * \n         mal_net_query_mode() is made an asynchronous call
 * \n
 * \n v0.13   Updates indications framework
 * \n
 * \n v0.12   Added Support For CCSQ and ENCSQ
 * \n
 * \n v0.11   Added modem error support
 * \n
 * \n v0.10   Added framework for Enable/Disbale indications
 * \n         Implemented HIND (RAB satus)indication handling
 * \n         Implemented event and callback for NITZ information
 * \n
 * \n v0.9    Fetch Neighbour Cell BER Information
 * \n
 * \n v0.8    Added mal_net_register_to_pref_rat to invoke registration to the network
 * \n         based on preffered Radio Access Technology (RAT)
 * \n
 * \n v0.7    MAL_NET_REGISTRATION_STATUS replaced with MAL_NET_MODEM_REG_STATUS_GET_RESP
 * \n         MAL_NET_STATE_CHANGED replaced with MAL_NET_MODEM_REG_STATUS_IND
 * \n         Manual search
 * \n         Manual search cancel
 * \n         Automatic registration
 * \n         Manual registration
 * \n
 * \n v0.6    mal_net_query_mode(..) is made a SYNCHRONOUS CALL
 * \n         Added MAL_NET_MODE_SET_RESP response event type
 * \n         Removed int mal_net_request_signal_strength(): duplicate
 * \n         MAL_NET_STATE_CHANGED Event ID: indicates NW State IND received.
 * \n
 * \n v0.5    Removed MAL_NET_REGISTRATION_GPRS_STATUS
 * \n         Removed declaration net_cbfunc_event
 * \n         Added _RESP suffix to MAL_NET_REGISTRATION_STATUS_RESP
 * \n
 * \n v0.4    Added comment that mal_net_get_rssi(..) is a SYNCHRONOUS CALL
 * \n         Removed MAL_NET_RSSI_RESP Macro
 * \n         Updated comments
 * \n         Added #include <stdint.h>
 */

/**
 * \defgroup  MAL_NET NET API
 * \par
 * \n This part describes the interface to NET MAL Client
 */

#ifndef MAL_NET_H
#define MAL_NET_H

#include <stdint.h>

#define NET_LIB_VERSION "NET Lib V 0.20.r1"
#define NET_MCC_MNC_STRING_LEN 8
#define MAL_GSM_MAX_NEIGHBOUR_CELLS 15
#define MAL_UMTS_MAX_DL_UARFCNS 3
#define MAL_UMTS_MAX_CELLS_INCLUDED 32

/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/

/**
 * \def MAL_NET_MODEM_REG_STATUS_GET_RESP
 * \brief Registration Status Response
 *
 * \n This response is received after sending the request to modem to get the
 * \n registration status, GPRS registration status, operator information or
 * \n mode of registration.
 */
#define MAL_NET_MODEM_REG_STATUS_GET_RESP 0x01

/** \def MAL_NET_RSSI_CONF_RESP
 *  \brief RSSI Configuration Response
 *
 *  \n This response is received after sending NET_RSSI_CONF_REQ request to modem.
 */
#define MAL_NET_RSSI_CONF_RESP 0x47

/** \def MAL_NET_CONFIGURE_RESP
 *  \brief  Response for setting the frequency of Regsitration status IND
 *
 *  \n This response is received after sending NET_CONFIGURE_REQ request to modem.
 */
#define MAL_NET_CONFIGURE_RESP 0x4B

/**
 * \def MAL_NET_RSSI_IND
 * \brief Radio Signal Strength and BER Indication
 *
 * \n Unsolicited indication reporting the Radio Signal Strength and BER.
 */
#define MAL_NET_RSSI_IND 0x1E

/**
 * \def MAL_NET_RSSI_GET_RESP
 * \brief Radio Signal Strength and BER Response
 *
 * \n This response is received afer sending the request to modem to get the
 * \n Radio Signal Strength and BER.
 */
#define MAL_NET_RSSI_GET_RESP 0x0C

/**
 * \def MAL_NET_MODEM_REG_STATUS_IND
 * \brief Modem Registration Status Indication
 *
 * \n Unsolocited indication reporting Modem Registration Status.
 */
#define MAL_NET_MODEM_REG_STATUS_IND 0x02

/**
 * \def MAL_NET_NETWORK_SELECT_MODE_SET_RESP
 * \brief Network Selectin Mode Set Response
 *
 * \n This response is received after sending the request to modem to set
 * \n the network selection mode.
 */
#define MAL_NET_NETWORK_SELECT_MODE_SET_RESP 0x1D

/**
 * \def MAL_NET_MODEM_AVAILABLE_GET_RESP
 * \brief Response with information about available networks
 *
 * \n This response is received after sending the request to modem to
 * \n search the available networks.
 */
#define MAL_NET_MODEM_AVAILABLE_GET_RESP 0x04

/**
 * \def MAL_NET_SET_RESP
 * \brief Response to a Registration Request
 *
 * \n This response is received after sending the request to modem to
 * \n register to the network.
 */
#define MAL_NET_SET_RESP 0x08

/**
 * \def MAL_NET_SET_ACCESS_CONF_RESP
 * \brief Response to a network access configuration request
 *
 * \n This response is received after sending the request to modem to
 * \n set the network access configuration.
 */
#define MAL_NET_SET_ACCESS_CONF_RESP 0x10

/**
 * \def MAL_NET_TEST_CARRIER_RESP
 * \brief Response to production carrier setting
 *
 * \n This response is received after sending the request to modem requesting
 * \n for a prodcution carrier setting/clearing operations.
 *
 */
#define MAL_NET_TEST_CARRIER_RESP 0x12

/**
 * \def MAL_NET_CS_CONTROL_RESP
 * \brief Response to a CS Operation Request
 *
 * \n This response is received after sending the request to modem requesting
 * \n a CS operation.
 *
 */
#define MAL_NET_CS_CONTROL_RESP 0x0E

/**
 * \def MAL_NET_AVAILABLE_CANCEL_RESP
 * \brief Response to a Manual Search Interrupt Request
 *
 * \n This response is received after sending the request to modem to interrupt
 * \n manual search.
 */
#define MAL_NET_AVAILABLE_CANCEL_RESP 0x06

/** \def MAL_NET_NITZ_NAME_IND
 *  \brief Network Name and Time-zone Indication
 *
 *  \n Unsolicited indication reporting Network Name and Time-zone.
 */
#define MAL_NET_NITZ_NAME_IND 0x43

/** \def MAL_NET_TIME_IND
 *  \brief Time indication
 *
 *  \n Unsolicited indication reporting Time.
 */
#define MAL_NET_TIME_IND 0x27

/**
 * \def MAL_NET_NEIGHBOUR_CELLS_RESP
 * \brief Response to a Neighbour Cell Info Request
 *
 * \n This response is received after sending the request to modem for
 * \n neighbour cell information.
 */
#define MAL_NET_NEIGHBOUR_CELLS_RESP 0x1B

/** \def MAL_NET_RADIO_INFO_IND
 *  \brief Radio Info indication
 *
 *  \n Unsolicited indication reporting change in radio status.
 */
#define MAL_NET_RADIO_INFO_IND 0x3f

/**
 * \def MAL_NET_CELL_INFO_GET_REQ
 * \brief Modem Cell Information Request
 *
 * \n Request cell information.
 */
#define MAL_NET_CELL_INFO_GET_REQ 0x40

/**
 * \def MAL_NET_CELL_INFO_GET_RESP
 * \brief Modem Cell Information Response
 *
 * \n Response reporting cell information.
 */
#define MAL_NET_CELL_INFO_GET_RESP 0x41

/**
 * \def MAL_NET_CELL_INFO_IND
 * \brief Modem Cell Information Indication
 *
 * \n Unsolocited indication reporting cell information.
 */
#define MAL_NET_CELL_INFO_IND 0x42

/**
 * \def MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO
 * \brief Unsolicited Neighbour Cell Info event
 *
 * \n Unsolicited indication reporting Neighbour Cell Information
 */
#define MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO 0xE0


/** \def MAL_NET_MODEM_DETAILED_FAIL_CAUSE
 *  \brief Detail cause for modem errors
 *
 *  \n Unsolicited indication reporting detail cause for modem error.
 */
#define MAL_NET_MODEM_DETAILED_FAIL_CAUSE  0xE1

/**
 * \def MAL_NET_DEREG_RESP
 * \brief Response to a de-registration Request
 *
 * \n This response is received after sending the request to modem to
 * \n de-register from the network.
 */
#define MAL_NET_DEREG_RESP 0xE2

/**
 * \def MAL_NET_NMR_INFO_RESP
 * \brief Response to a NMR Info Request
 *
 * \n This response is received after sending the request to modem for
 * \n NMR information.
 */
#define MAL_NET_NMR_INFO_RESP 0xE3

/**
 * \def MAL_NET_RAT_IND
 * \brief Current RAT name indication
 *
 * \n Unsolicited indication of current RAT name.
 */
#define MAL_NET_RAT_IND 0x35

/**
 * \def MAL_NET_RAT_RESP
 * \brief Response to a current RAT name query.
 *
 * \n This response is received after sending the request to modem for
 * \n current RAT name.
 */
#define MAL_NET_RAT_RESP 0x37

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * \enum mal_net_error_type
 * \brief Error types
 *
 * \n This enum defines error types for the NET MAL Library. These error types
 * \n are used as return values for the API routines.
 */
typedef enum {
    MAL_NET_SUCCESS                = 0x00, /**< Success  */
    MAL_NET_GENERAL_ERROR          = 0x01, /**< General error */
    MAL_NET_INTERRUPTED            = 0x12, /**< Request interrupted */
    MAL_NET_REQUEST_NOT_SUPPORTED  = 0xF1, /**< Request not supported */
    MAL_NET_INVALID_BLOCK_TYPE     = 0xF2, /**< Block type not correct */
    MAL_NET_INSUFFICIENT_MEMORY    = 0xF3, /**< Memory allocation failure */
    MAL_NET_INVALID_DATA           = 0xF4, /**< Invalid data passed to the routine */
    MAL_NET_SOCK_SEND_ERROR        = 0xF5, /**< Send operation on socket failed */
    MAL_NET_REQ_ALREADY_IN_PROCESS = 0xF6, /**< Request is already in process */
    MAL_NET_INVALID_RAT_TYPE       = 0xF7, /**< RAT type not valid for request or response */
    MAL_NET_REQUIRED_IND_NOT_SET   = 0xF8, /**< Required indication is not set */
    MAL_NET_IND_NOT_ARRIVED        = 0xF9, /**< Required indication has not come from modem */
    MAL_NET_CAUSE_NET_NOT_FOUND    = 0x70, /**< No network found */

    /* Errors reported by modem */
    MAL_NET_GSM_IMSI_UNKNOWN_IN_HLR        = 0x02, /**< IMSI unknown in HLR */
    MAL_NET_GSM_ILLEGAL_MS                 = 0x03, /**< Illegal MS */
    MAL_NET_GSM_IMSI_UNKNOWN_IN_VLR        = 0x04, /**< IMSI unknown in VLR */
    MAL_NET_GSM_IMEI_NOT_ACCEPTED          = 0x05, /**< IMEI not accepted */
    MAL_NET_GSM_ILLEGAL_ME                 = 0x06, /**< Illegal ME */
    MAL_NET_GSM_GPRS_SERVICES_NOT_ALLOWED  = 0x07, /**< GPRS services not allowed */
    MAL_NET_GSM_GPRS_AND_NON_GPRS_NA       = 0x08, /**< GPRS services and non-GPRS services not allowed */
    MAL_NET_GSM_MS_ID_CANNOT_BE_DERIVED    = 0x09, /**< MS identity cannot be derived by the network */
    MAL_NET_GSM_IMPLICITLY_DETACHED        = 0x0A, /**< Implicitly detached */
    MAL_NET_GSM_PLMN_NOT_ALLOWED           = 0x0B, /**< PLMN not allowed */
    MAL_NET_GSM_LA_NOT_ALLOWED             = 0x0C, /**< Location Area not allowed */
    MAL_NET_GSM_ROAMING_NOT_IN_THIS_LA     = 0x0D, /**< Roaming not allowed in this location area */
    MAL_NET_GSM_GPRS_SERV_NA_IN_THIS_PLMN  = 0x0E, /**< GPRS services not allowed in this PLMN */
    MAL_NET_GSM_NO_SUITABLE_CELLS_IN_LA    = 0x0F, /**< No suitable cells in location area */
    MAL_NET_GSM_MSC_TEMP_NOT_REACHABLE     = 0x10, /**< MSC temporarily not reachable */
    MAL_NET_GSM_NETWORK_FAILURE            = 0x11, /**< Network failure */
    MAL_NET_GSM_MAC_FAILURE                = 0x14, /**< MAC failure */
    MAL_NET_GSM_SYNCH_FAILURE              = 0x15, /**< Synch failure */
    MAL_NET_GSM_CONGESTION                 = 0x16, /**< Congestion */
    MAL_NET_GSM_AUTH_UNACCEPTABLE          = 0x17, /**< GSM authentication unacceptable */
    MAL_NET_GSM_SERV_OPT_NOT_SUPPORTED     = 0x20, /**< Service option not supported */
    MAL_NET_GSM_SERV_OPT_NOT_SUBSCRIBED    = 0x21, /**< Requested service option not subscribed */
    MAL_NET_GSM_SERV_TEMP_OUT_OF_ORDER     = 0x22, /**< Service option temporarily out of order */
    MAL_NET_GSM_RETRY_ENTRY_NEW_CELL_LOW   = 0x30, /**< Retry upon entry into a new cell */
    MAL_NET_GSM_RETRY_ENTRY_NEW_CELL_HIGH  = 0x3F, /**< Retry upon entry into a new cell */
    MAL_NET_GSM_SEMANTICALLY_INCORRECT     = 0x5F, /**< Semantically incorrect message */
    MAL_NET_GSM_INVALID_MANDATORY_INFO     = 0x60, /**< Invalid mandatory information */
    MAL_NET_GSM_MSG_TYPE_NONEXISTENT       = 0x61, /**< Message type non-existent or not implemented */
    MAL_NET_GSM_CONDITIONAL_IE_ERROR       = 0x64, /**< Conditional IE error */
    MAL_NET_GSM_MSG_TYPE_WRONG_STATE       = 0x65, /**< Message type not compatible with the protocol state */
    MAL_NET_GSM_PROTOCOL_ERROR_UNSPECIFIED = 0x6F  /**< Protocol error, unspecified */
} mal_net_error_type;


/**
 * \enum mal_net_selection_mode
 * \brief Network Selection Modes
 *
 * \n This enum defines available Network Selection Modes for registration.
 * \n requests.
 */
typedef enum {
    MAL_NET_SELECT_MODE_UNKNOWN          = 0x00, /**< Unknown mode */
    MAL_NET_SELECT_MODE_MANUAL           = 0x01, /**< Manual mode */
    MAL_NET_SELECT_MODE_AUTOMATIC        = 0x02, /**< Automatic mode */
    MAL_NET_SELECT_MODE_USER_RESELECTION = 0x03, /**< Reselection */
    MAL_NET_SELECT_MODE_NO_SELECTION     = 0x04  /**< No Selection; used to clear the state */
} mal_net_selection_mode;

/**
 * \enum mal_net_modem_reg_status
 * \brief Network Registration Status
 *
 * \n This enum defines Network Registration Status.
 */
typedef enum {
    MAL_NET_NOT_REG_NOT_SEARCHING                         = 0x00, /**< Not registered, not searching */
    MAL_NET_REGISTERED_TO_HOME_NW                         = 0x01, /**< Registerred to home network */
    MAL_NET_NOT_REG_SEARCHING_OP                          = 0x02, /**< Not registered, but seraching the operator */
    MAL_NET_REGISTRATION_DENIED                           = 0x03, /**< Registration denied */
    MAL_NET_REGISTRATION_UNKNOWN                          = 0x04, /**< Registration status unknown */
    MAL_NET_REGISTERED_ROAMING                            = 0x05, /**< Registered in roaming */
    MAL_NET_NOT_REG_NOT_SEARCHING_EMERGENCY_CALLS_ALLOWED = 0x0A, /**< Not registered, not searching - emergency call is possible */
    MAL_NET_NOT_REG_SEARCHING_OP_EMERGENCY_CALLS_ALLOWED  = 0x0C, /**< Not registered, but searching the operator - emergency call is possible */
    MAL_NET_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED   = 0x0D, /**< Registration denied - emergency call is possible */
    MAL_NET_REGISTRATION_UNKNOWN_EMERGENCY_CALLS_ALLOWED  = 0x0E  /**< Registration status unknown - emergency call is possible */
} mal_net_modem_reg_status;

/**
 * \enum mal_net_rat
 * \brief RAT types
 *
 * \n This enum defines RAT (Radio Access Technology) Types.
 */
typedef enum {
    MAL_NET_RAT_UNKNOWN    = 0x00, /**< Unknown */
    MAL_NET_RAT_GPRS       = 0x01, /**< GPRS */
    MAL_NET_RAT_EDGE       = 0x02, /**< EDGE (EGPRS) */
    MAL_NET_RAT_UMTS       = 0x03, /**< UMTS (W-CDMA) */
    MAL_NET_RAT_IS95A      = 0x04, /**< IS95A */
    MAL_NET_RAT_IS95B      = 0x05, /**< IS95B */
    MAL_NET_RAT_1xRTT      = 0x06, /**< 1xRTT */
    MAL_NET_RAT_EVDO_REV_0 = 0x07, /**< EvDO Rrv. 0 */
    MAL_NET_RAT_EVDO_REV_A = 0x08, /**< EvDO Rrv. A */
    MAL_NET_RAT_HSDPA      = 0x09, /**< HSDPA */
    MAL_NET_RAT_HSUPA      = 0x0A, /**< HSUPA */
    MAL_NET_RAT_HSPA       = 0x0B, /**< HSPA */
    MAL_NET_RAT_GSM        = 0x0C, /**< GSM */
    MAL_NET_RAT_EPS        = 0x0D, /**< EPS (Evolved Packet System) */
    MAL_NET_RAT_GPRS_DTM   = 0x10, /**< GPRS DTM (Dual transfer mode) */
    MAL_NET_RAT_EDGE_DTM   = 0x13  /**< EDGE DTM */
} mal_net_rat;

/**
 * \enum mal_net_screen_state
 * \brief screen state
 *
 * \n This enum defines Screen States.
 */
typedef enum {
    MAL_NET_SCREEN_OFF = 0x00, /**< Screen on */
    MAL_NET_SCREEN_ON  = 0x01  /**< Screen off */
} mal_net_screen_state;

/**
 * \enum mal_net_cs_request_types
 * \brief Cellular System request types
 *
 * \n This enum defines CS request types.
 */
typedef enum {
    MAL_NET_CS_POWER_OFF  = 0x02, /**< To request to switch off the CS */
    MAL_NET_CS_POWER_ON   = 0x03, /**< To request to switch on the CS */
    MAL_NET_CS_POWER_WIFI = 0x05, /**< To request to switch to GAN flight mode */
} mal_net_cs_request_type;

/**
 * \enum mal_net_location_update_action
 * \brief Location update action
 *
 * \n This enum defines location update actions
 */
typedef enum {
    MAL_NET_LOCATION_UPDATE_DISABLE = 0x00, /**< Location update disabled */
    MAL_NET_LOCATION_UPDATE_ENABLE  = 0x01, /**< Location update enabled */
} mal_net_location_update_action;

/**
 * \enum mal_net_neighbour_rat_type
 * \brief RAT types associated with Neighbour Cell Information
 *
 * \n This enum defines RAT types associated with Neighbour Cell Information.
 */
typedef enum {
    MAL_NET_NEIGHBOUR_RAT_UNKNOWN = 0x00, /**< RAT type unknown */
    MAL_NET_NEIGHBOUR_RAT_2G      = 0x01, /**< RAT type 2G (GSM) */
    MAL_NET_NEIGHBOUR_RAT_3G      = 0x02  /**< RAT type 3G (UMTS) */
} mal_net_neighbour_rat_type;

/**
 * \enum mal_net_neigh_info_type
 * \brief Info types associated with Neighbour Cell Information
 *
 * \n This enum defines Info types associated with Neighbour Cell Information.
 */
typedef enum {
    MAL_NET_NEIGHBOUR_CELLS_EXT = 0x00,      /**< Only Neighbour ECID information present  */
    MAL_NET_NEIGHBOUR_CELLS_BASIC_EXT = 0x01 /**< Both Neighbour cell and ECID information present */
} mal_net_neigh_info_type;


/**
 * \enum mal_net_radio_info_ind_state
 * \brief radio info indication
 *
 * \n This enum defines the net radio info indication state
 */
typedef enum {
    MAL_NET_RADIO_INFO_IND_DISABLE = 0,
    MAL_NET_RADIO_INFO_IND_ENABLE = 1
} mal_net_radio_info_ind_state;

/**
 * \enum mal_net_ind_state
 * \brief net indication state
 *
 * \n This enum defines the net indication state
 */
typedef enum {
    MAL_NET_IND_DISABLE = 0,
    MAL_NET_IND_ENABLE = 1
} mal_net_ind_state;

/**
 * \enum mal_net_ind_type
 * \brief net indication list
 *
 * \n This enum defines the net indication supported
 */
typedef enum {
    MAL_NET_SELECT_MODEM_REG_STATUS_IND = 0x00,
    MAL_NET_SELECT_RSSI_IND             = 0x01,
    MAL_NET_SELECT_TIME_IND             = 0x02,
    MAL_NET_SELECT_RADIO_INFO_IND       = 0x03,
    MAL_NET_SELECT_NITZ_NAME_IND        = 0x04,
    MAL_NET_SELECT_CS_STATE_IND         = 0x05,
    MAL_NET_SELECT_CIPHERING_IND        = 0x06,
    MAL_NET_SELECT_CHANNEL_INFO_IND     = 0x07,
    MAL_NET_SELECT_RAT_IND              = 0x08,
    MAL_NET_SELECT_UMA_INFO_IND         = 0x09,
    MAL_NET_SELECT_CELL_INFO_IND        = 0x0A
} mal_net_ind_type;

/**
 * \enum mal_net_rab_status_type
 * \brief RAB status type defination
 *
 * \n This enum defines the RAB status
 */
typedef enum {
    MAL_NET_NEITHER_HSDPA_HSUPA = 0,
    MAL_NET_HSDPA               = 1,
    MAL_NET_HSUPA               = 2,
    MAL_NET_BOTH_HSDPA_HSUPA    = 3
} mal_net_rab_status_type;

/**
 * \enum mal_net_nmr_rat_type
 * \brief RAT type for NMR retrieval
 *
 * \n This enum defines the RAT type for NMR retrieval.
 */
typedef enum  {
    MAL_NET_NMR_RAT_GERAN = 0x01, /**< RAT type GERAN */
    MAL_NET_NMR_RAT_UTRAN = 0x02  /**< RAT type UTRAN */
} mal_net_nmr_rat_type;

/**
 * \enum mal_net_utran_nmr_type
 * \brief NMR types
 *
 * \n This enum defines NMR types.
 */
typedef enum  {
    MAL_NET_SIM_INTRA_FREQ_NMR = 0x02, /**< Intra-frequency NMR */
    MAL_NET_SIM_INTER_FREQ_NMR = 0x03, /**< Intra-frequency NMR */
    MAL_NET_SIM_INTER_RAT_NMR  = 0x04  /**< Inter-RAT NMR */
} mal_net_utran_nmr_type;

/**
 * \struct mal_net_ind_select_info
 * \brief Data for enable/disable the specific indication in NET server
 *
 * \n This structure is used for selecting a specific indication state
 */
typedef struct {
    mal_net_ind_type type;   /**< Select the indication type */
    mal_net_ind_state state; /**< Indication state to be set */
} mal_net_ind_select_info;

/**
 * \struct mal_net_network_selection
 * \brief Data for Registration to Network
 *
 * \n This structure is used for selecting a network for registration.
 */
typedef struct {
    mal_net_selection_mode selection_mode; /**< Chosen Selection Mode */
    uint8_t                index;          /**< Index of the network.
                                             * Ignored in case of automatic mode.
                                             */
} mal_net_network_selection;

/**
 * \enum mal_net_pref_rat
 * \brief Preffered RAT Type for Registration to Network
 *
 * \n This structure is used for selecting a preffered RAT for registration.
 * \n During Registration Preffered RAT type would be tried first, then other
 * \n RAT types would be tried .
 */
typedef enum {
    MAL_NET_RAT_SEQ_GSM      = 0x01,
    MAL_NET_RAT_SEQ_UMTS     = 0x02,
    MAL_NET_RAT_SEQ_EPS      = 0x04
} mal_net_pref_rat;

/**
 * \enum mal_net_nw_access_conf_state
 * \brief Network access configuration state
 *
 * \n This enum defines the network access configuration states.
 */
typedef enum {
    MAL_NET_CONF_DISABLE = 0,
    MAL_NET_CONF_ENABLE  = 1
} mal_net_nw_access_conf_state;

/**
 *\enum mal_net_test_carrier_operation
 *\brief Test Carrier Operation
 *
 *\n This enum defines the Test Carrier Operation
 */
typedef enum {
    MAL_NET_TEST_CARRIER_SET = 0,
    MAL_NET_TEST_CARRIER_CLEAR = 1
} mal_net_test_carrier_operation;

/**
 *\enum mal_net_carrier_type
 *\brief Test Carrier Type
 *
 *\n This enum defines the Test Carrier Type
 */
typedef enum {
    MAL_NET_CARRIER_TYPE_WCDMA = 0,
    MAL_NET_CARRIER_TYPE_GSM = 1
} mal_net_carrier_type;

/**
 *\enum mal_net_band_selection
 *\brief Band Selection Type for GSM
 *
 *\n This enum defines the Band Selection Type for GSM
 */
typedef enum {
    MAL_NET_GSM900_1800 = 0x00,
    MAL_NET_GSM850_1900 = 0x01
} mal_net_band_selection;

/**
 * \enum mal_net_cell_rat_type
 * \brief Cell RAT type
 *
 * \n This enum defines the RAT types applicable to cell information.
 */
typedef enum {
    MAL_NET_CELL_RAT_UNKNOWN = 0x0, /**< RAT type unknown */
    MAL_NET_CELL_RAT_GSM     = 0x1, /**< RAT type GSM */
    MAL_NET_CELL_RAT_WCDMA   = 0x2, /**< RAT type WCDMA */
    MAL_NET_CELL_RAT_EPS     = 0x3  /**< RAT type EPS */
} mal_net_cell_rat_type;

/**
 * \enum mal_net_network_type
 * \brief Network types
 *
 * \n This enum defines the network types.
 */
typedef enum {
    MAL_NET_GSM_HOME_PLMN      = 0x0, /**< GSM operator code equals to home
                                        * network operator code
                                        */
    MAL_NET_GSM_PREFERRED_PLMN = 0x1, /**< GSM operator code same as one of the
                                        * operator codes in PLMN selection list
                                        * of SIM card
                                        */
    MAL_NET_GSM_FORBIDDEN_PLMN = 0x2, /**< SM operator code same as one of the
                                        * forbidden PLMN operator codes
                                        */
    MAL_NET_GSM_OTHER_PLMN     = 0x3, /**< GSM operator code can not be
                                        * identified as neither home PLMN nor
                                        * preferred PLMN nor forbidden PLMN
                                        */
    MAL_NET_GSM_NO_PLMN_AVAIL  = 0x4  /**< GSM operator not available */
} mal_net_network_type;

/**
 * \enum mal_net_nw_service_status
 * \brief Network Service Status
 *
 * \n This enum defines the Network Service Status.
 */
typedef enum {
    MAL_NET_SERVICE          = 0x0, /**< CS is in service */
    MAL_NET_LIMITED_SERVICE  = 0x1, /**< CS is in limited_service */
    MAL_NET_NO_COVERAGE      = 0x2,  /**< CS is not in service. */
    MAL_NET_LIMITED_SERVICE_WAITING = 0x3, /**< CS is in limited service, but waiting for PIN/PUK */
    MAL_NET_SERVICE_UNKNOWN         = 0x4 /**< CS is in service unknown */
} mal_net_nw_service_status;

/**
 * \enum mal_net_reg_status_set_mode
 * \brief Configuration values for registration status indication
 *
 * \n This enum defines the configuration values for NET_MODEM_REG_STATUS_IND frequency.
 */
typedef enum {
    MAL_NET_MSG_SEND_MODE_SEND_ALL              = 0x0, /**< Default mode. Message is sent every time some parameter indicated in message changes */
    MAL_NET_MSG_SEND_MODE_REG_STATUS_CHANGE     = 0x1, /**< Message is sent when registration status changes */
    MAL_NET_MSG_SEND_MODE_CELL_ID_OR_LAC_CHANGE = 0x2  /**< Message is sent when Cell ID and/or LAC changes */
} mal_net_reg_status_set_mode;

/**
 * \enum mal_net_rat_name
 * \brief RAT Name
 *
 * \n This enum defines Radio Access Technology names
 */
typedef enum {
    MAL_NET_RAT_NAME_UNKNOWN = 0,
    MAL_NET_RAT_NAME_GSM     = 1,
    MAL_NET_RAT_NAME_UMTS    = 2, /* W-CDMA */
    MAL_NET_RAT_NAME_EPS     = 4  /* Evolved Packet System - LTE */
} mal_net_rat_name;

/**
 * \enum mal_net_neighbour_request_type
 * \brief Neighbour Cell Request type
 *
 * \n This enum defines Neighbour Cell Request types
 */
typedef enum {
    MAL_NET_CELL_INFO_STATE_IDLE,               /* Idle state */
    MAL_NET_CELL_INFO_STATE_ECID_FETCH,         /* ECID information fetched */
    MAL_NET_CELL_INFO_STATE_NEIGH_FETCH,        /* Neighbour information fetched */
    MAL_NET_CELL_INFO_STATE_NEIGH_ECID_FETCH,   /* Neighbour and ECID information fetched */
    MAL_NET_CELL_INFO_STATE_SIM_FETCH           /* SIM information fetched */
} mal_net_neighbour_request_type;
/**
 * \struct mal_net_nw_access_conf_data
 * \brief Network access configuration data
 *
 * \n This structure defines the network access configuration data.
 */
typedef struct {
    mal_net_nw_access_conf_state reg_conf;  /**< Registration configuration */
    mal_net_nw_access_conf_state roam_conf; /**< Roaming configuration */
} mal_net_nw_access_conf_data;

/**
 * struct mal_net_test_carrier_data
 * \brief Test Carrier Data
 *
 *\n This structure defines the Test Carrier Data
 */
typedef struct {
    mal_net_test_carrier_operation carrier_operation;  /**<Test Carrier Operation */
    mal_net_carrier_type carrier_type;                 /**<Type of carrier GSM/WCDMA */
    mal_net_band_selection band_selection;             /**<Band selection for GSM */
    uint16_t channel_nbr;                              /**<Frequency */
} mal_net_test_carrier_data;

/**
 * \struct mal_net_registration_data
 * \brief Registration Data
 *
 * \n This structure provides Registration Data.
 */
typedef struct {
    uint8_t registration_status; /**< Registration status */
    uint8_t selection_mode;      /**< Network selection mode */
    uint8_t GSM_operator_code_1; /**< First byte of operator code */
    uint8_t GSM_operator_code_2; /**< Second byte of operator code */
    uint8_t GSM_operator_code_3; /**< Third byte of operator code */
    uint8_t fd_available_in_cell;/**< GSM band information was removed in later modem header files and replaced with fast dormancy variable */
    uint8_t gsm_network_type;    /**< GSM Network Type */
    uint8_t GPRS_support;        /**< Whether GPRS is supported in the cell */
    uint8_t GPRS_network_mode;   /**< GPRS Network Mode */
    uint8_t CS_services;         /**< Whether CS services are available */
    uint8_t GPRS_services;       /**< Whether attached to GPRS */
    uint8_t EGPRS_support;       /**< Wheter EGPRS is supported */
    uint8_t DTM_support;         /**< Wheter DTM is supported */
    uint8_t current_RAC;         /**< current RAC */
    uint8_t HSDPA_available;     /**< Wheter HSDPA is available */
    uint8_t HSUPA_available;     /**< Wheter HSUPA is available */
    uint8_t HSDPA_allocated;     /**< Whether HSDPA is allocated */
    uint8_t HSUPA_allocated;     /**< Whether HSUPA is allocated */
    uint8_t camped_in_HPLMN;     /**< Whether camped to HPLMN */
    uint8_t RAT_name;            /**< RAT Name */
    uint8_t reject_code;         /**< Reject code, if error is reported */
    uint16_t current_LAC;         /**< Current LAC */
    uint32_t GSM_current_cell_id; /**< Current Cell ID */
} mal_net_registration_data;

/**
 * \struct net_nitz_name
 * \brief Network nitz name data
 *
 * \n This structure provides network nitz name data.
 */
typedef struct {
    char    *text_p;
    uint8_t  length;
    uint8_t  add_ci;    /* Add country initials to text string. */
    uint8_t  dcs;       /* Data Coding Scheme for text string. */
    uint8_t  spare;     /* Number of spare bits in last byte. */
} net_nitz_name;

/**
 * \struct mal_net_registration_info
 * \brief Registration Data
 *
 * \n This structure provides Registration Data.
 */
typedef struct {
    uint32_t               reg_status;           /**< Registration status \ref mal_net_modem_reg_status */
    mal_net_rat            rat;                  /**< RAT */
    mal_net_selection_mode selection_mode;       /**< Network selection mode */
    uint32_t               cid;                  /**< CID */
    uint16_t               lac;                  /**< LAC */
    uint8_t                *mcc_mnc_string;      /**< Null-terminated string with MCC and MNC */
    uint8_t                gprs_attached;        /**< Whether GPRS is attached. 0 - Not attached; 1 - attached */
    uint8_t                cs_attached;          /**< Whether CS is attached. 0 - Not attached; 1 - attached */
#ifdef NAME_INFO_FROM_REG_INFO
    net_nitz_name           long_operator_name;
    net_nitz_name           short_operator_name;
#endif
} mal_net_registration_info;

/**
 * \struct mal_net_name_info
 * \brief Network name information data
 *
 * \n This structure provides network name information data.
 */
typedef struct {
    net_nitz_name    full_operator_name;  /**< Null-terminated string presenting full operator name */
    net_nitz_name    short_operator_name; /**< Null-terminated string presenting short operator name */
    uint8_t         *mcc_mnc_string;      /**< Null-terminated string with MCC and MNC */
} mal_net_name_info;

/**
 * \struct mal_net_network_node
 * \brief Details of a particular network
 *
 * \n This structure provides details of a particular network as a node.
 */
typedef struct _mal_net_network_node {
    uint8_t   index;            /**< Index of the network within modem */
    uint8_t   network_status;   /**< Network Status: 0 - unknown; 1 - available; 2 - current; 3 - forbidden; */
    uint8_t   band_info;        /**< GSM Band info: 0 - 900-1800; 1 - 850-1900; 2 - not available; 3 - all supported band; */
    uint8_t   network_type;     /**< Network Status: 0 - home PLMN; 1 - preferred PLMN; 2 - forbidden PLMN; 3 - other PLMN; 4 - no PLMN available */
    uint8_t   umts_available;   /**< 0 - UMTS available; 1 - UMTS not available */
    uint16_t  current_lac;      /**< Current LAC */
    uint16_t  country_code;     /**< Country code */
    uint16_t  network_code;     /**< Network code */
    uint8_t   mcc_mnc[NET_MCC_MNC_STRING_LEN];  /**< NULL- terminated string presenting MCC and MNC */
#ifdef NAME_INFO_FROM_REG_INFO
    net_nitz_name           long_op_name;
    net_nitz_name           short_op_name;
#endif
    struct _mal_net_network_node *next;         /**< Pointer to the next node */
} mal_net_network_node;

/**
 * \struct mal_net_manual_search_data
 * \brief Manual Network Search Data
 *
 * \n This structure provides Manual Network Search Data.
 */
typedef struct {
    uint8_t               num_of_networks; /**< Number of networks found */
    mal_net_network_node  *head;           /**< Head of the linked-list; each node presents a single network */
    mal_net_network_node  *tail;           /**< Tail of the linked-list; each node presents a single network */
} mal_net_manual_search_data;

/**
 * \struct mal_net_time_info
 * \brief Network Time Information
 *
 * \n This structure provides Network Time Information. It is possible that only
 * \n a subset of the information is provided. If a specific field hasn't been
 * \n supplied, MAL_NET_TIME_INFO_NOT_AVAILABLE is used to indicate this.
 */
#define MAL_NET_TIME_INFO_NOT_AVAILABLE 0x64
typedef struct {
    uint8_t year;                  /**< Year;   [0-99], MAL_NET_TIME_INFO_NOT_AVAILABLE. */
    uint8_t month;                 /**< Month;  [1-12], MAL_NET_TIME_INFO_NOT_AVAILABLE. */
    uint8_t day;                   /**< Day;    [1-31], MAL_NET_TIME_INFO_NOT_AVAILABLE. */
    uint8_t hour;                  /**< Hour;   [0-23], MAL_NET_TIME_INFO_NOT_AVAILABLE. */
    uint8_t minute;                /**< Minute; [0-59], MAL_NET_TIME_INFO_NOT_AVAILABLE. */
    uint8_t second;                /**< Second; [0-59], MAL_NET_TIME_INFO_NOT_AVAILABLE. */
    uint8_t time_zone_sign;        /**< Time zone sign; [0-1],
                                     *  0 - Positive (+) sign.
                                     *  1 - Negative (-) sign.
                                     */
    uint8_t time_zone_value;       /**< Number of quarter-hours */
    uint8_t day_light_saving_time; /**< Day light saving time;
                                     * [0-2], MAL_NET_TIME_INFO_NOT_AVAILABLE:
                                     * 0 - No adjustment.
                                     * 1 - One hour adjustment.
                                     * 2 - Two hour adjustment.
                                     */
} mal_net_time_info;

/**
 * \struct mal_net_nmr_info
 * \brief NMR information received from modem
 *
 * \n This structure provides NMR information received from modem.
 */
typedef struct {
    mal_net_nmr_rat_type rat; /**< RAT type of the measurement */

    union {
        struct {
            uint8_t  *measurement_results; /** Measurement Results.
                                             * Pointer to an array uint8_t[16]
                                             * The contents are equal to Measurements Results
                                             * information element starting at octet 2. The
                                             * description can be found in GSM TS 24.008.
                                             */
            uint8_t  num_of_channels;      /**< Number of BCCH channels: 0 to 32 */
            uint16_t *ARFCN_list;          /**< Pointer to an array uint16_t[num_of_channels] */
        } geran_info;  /** Applicable if rat is MAL_NET_NMR_RAT_GERAN */

        struct {
            uint8_t  mm_report_len;        /**< Length of Measurement Report */
            uint8_t  *measurements_report; /**< Measurement Report.
                                             * Pointer to anarray uint8_t[mm_report_len]
                                             * The contents are equal to Measurements Report
                                             * coded as RRC:UL-DCCH message. The description
                                             * can be found in 3GPP TS 25.331.
                                             */
        } utran_info; /** Applicable if rat is MAL_NET_NMR_RAT_UTRAN */
    };
} mal_net_nmr_info;

/**
 * Define for MAX number of Segments Supported for RSSI Configuration
 */
#define NET_RSSI_CONF_MAX_SEGMENTS 25

/**
 * \struct mal_net_rssi_conf_data
 * \brief  Configure the sending of MAL_NET_RSSI_IND
 *
 * \n This message is used to configure the sending of MAL_NET_RSSI_IND.
 *
 * \n border[NET_RSSI_CONF_MAX_SEGMENTS+1] is the array of border values specified
 * \n by the CLIENT which defines boundaries for different Segments .
 * \n Whenever a Segment Boundary is crossed we get a Rssi Indication .
 *
 * \n For MAL CLIENT num_of_segment is equal to (Number of border values -1)
 * \n num_of_segments and border[] must be ignored if num_of_segments is 0.
 * \n Meaning of num_of_segments being 0 is that the information is not available.
 *
 * \n NOTE: Currently signal_level_type will be hardcoded to DBM in MAL. (TBD)
 */

typedef struct {
    uint8_t num_of_segments;   /**< No. of Segment: 1 to 25, num_of_segment is
                                 * equal to (Number of border values -1)
                                 */
    uint8_t signal_level_type; /**< 1 - DBm; 2 - PERCENT */
    uint8_t hysteresis_down;   /**< Hysteresis down: 0 to 20 */
    uint8_t hysteresis_up;     /**< Hysteresis up: 0 to 20  */
    uint8_t border[NET_RSSI_CONF_MAX_SEGMENTS+1];   /**< border values passed by CLIENT */
    uint8_t ccsq_mode;         /**< 0 - Disable; 1 - Enable; 255 - Not set */
} mal_net_rssi_conf_data;

/**
 * \struct mal_net_radio_info
 * \brief  Configure the sending of NET_RADIO_INFO_IND
 *
 * \n This message is used to configure the sending of NET_RADIO_INFO_IND.
 */
typedef struct {
    uint8_t state;       /**< State of UTRAN radio (0x0 :radio state is Idle/0x01:Cell DCH ) */
    uint8_t HSDPA_alloc; /**< status of HSDPA allocated */
    uint8_t HSUPA_alloc; /**< satatus of HSUPA allocated */
} mal_net_radio_info;

/**
 * \struct mal_net_manual_reg_info
 * \brief  Configure the sending of NET_SET_REQ
 *
 * \n This message is used to configure the sending of NET_SET_REQ.
 */
typedef struct {
    char *mcc_mnc; /**< MCC & MNC Values of the Operator requested for registration */
    uint8_t rat;   /**< RAT value requested for registration(Only GSM(0x00), UMTS(0x02) & RAT_NOT_REQUIRED(0xff) are supported) */
} mal_net_manual_reg_info;

/**
 * \struct mal_net_neigh_umts_cell_detailed_info
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
} mal_net_neigh_umts_cell_detailed_info;

/**
 * \struct mal_net_neigh_umts_uarfcn_info
 * \brief Information related to umts neighbour cell(Intra-Frequency and Inter-Frequency)measurement report.
 *
 * \n This structure provides umts neighbour cell measurement reports.
 */
typedef struct {
    uint16_t                                   dl_uarfcn;                             /**< Down-Link ARFCN (Absolute Radio Frequency Channel Number).INTEGER (0..16383). */
    uint8_t                                    utra_carrier_rssi;                     /**< RSSI Value of the cell.INTEGER (0..127). */
    uint8_t                                    num_of_cells;                          /**< Number of cell measurement results provided in cdma_neighbour_cell_detailed_info_t */
    mal_net_neigh_umts_cell_detailed_info      umts_cell_detailed_info[MAL_UMTS_MAX_CELLS_INCLUDED];  /**< Cell measurement results. */
} mal_net_neigh_umts_uarfcn_info;

/**
 * \struct mal_net_neigh_umts_info_extd
 * \brief Extended Information related to umts neighbour cell .
 *
 * \n This structure provides umts neighbour cell extended information.
 */
typedef struct {
    uint8_t                           num_of_dl_uarfcn;                                /**< Number of UMTS Carriers */
    mal_net_neigh_umts_uarfcn_info    umts_uarfcn_info[MAL_UMTS_MAX_DL_UARFCNS];       /**< umts Neighbour cell(Intra-Frequency and Inter-Frequency) Measurement Report. */
} mal_net_neigh_umts_info_extd;

/**
 * \struct mal_net_umts_serving_cell_info
 * \brief Information related to umts serving cell .
 *
 * \n This structure provides umts serving cell information.
 */
typedef struct {
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
} mal_net_umts_serving_cell_info;

/**
 * \struct mal_net_umts_cell_info
 * \brief Information related to the umts serving and neighbouring cell
 *
 * \n This structure provides umts cell information for the serving cell and neighbouring cell.
 */
typedef struct {
    mal_net_umts_serving_cell_info   umts_serving_cell_info;   /**< UMTS serving cell info*/
    mal_net_neigh_umts_info_extd     umts_info_extd;           /**< UMTS ECID info */
} mal_net_umts_cell_info;


/**
 * \struct mal_net_neigh_gsm_nmr_info_extd
 * \brief Extended Information related to ECID GSM Neighbour cell Measurement Reports
 *
 * \n This structure provides extended gsm neighbour cell measurement reports.
 */
typedef struct {
    uint16_t arfcn;            /**< ARFCN - Absolute Radio Frequency Channel Number.  INTEGER (0..1023). */
    uint8_t  bsic;             /**< BSIC  - Base Station Identity Code.  INTEGER (0..63). */
    int8_t  rxlev;             /**< RXLEV - Received signal Level.  INTEGER (0..63). */
} mal_net_neigh_gsm_nmr_info_extd;

/**
 * \struct mal_net_gsm_neigh_info_extd
 * \brief Extended Information related to ECID GSM Neighbour cell
 *
 * \n This structure provides extended gsm neighbour cell info.
 */
typedef struct {
    uint8_t                            num_of_neighbours;                              /**< ECID Number of neighbour cells */
    mal_net_neigh_gsm_nmr_info_extd    gsm_nmr_info_extd[MAL_GSM_MAX_NEIGHBOUR_CELLS]; /**< GSM Neighbour cell Measurement Reports. The number of which is provided in ecid_num_of_neighbours. */
} mal_net_gsm_neigh_info_extd;

/**
 * \struct mal_net_neigh_gsm_nmr_info
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
} mal_net_neigh_gsm_nmr_info;

/**
 * \struct mal_net_gsm_neigh_info_basic
 * \brief Basic Information related to GSM Neighbour cell
 *
 * \n This structure provides basic gsm neighbour cell info.
 */
typedef struct {
    uint8_t                        num_of_neighbours;                         /**< Number of neighbour GSM cells */
    mal_net_neigh_gsm_nmr_info     gsm_nmr_info[MAL_GSM_MAX_NEIGHBOUR_CELLS]; /**< GSM Neighbour cell Measurement Reports. The number of which is provided in ecid_num_of_neighbours. */
} mal_net_gsm_neigh_info_basic;

/**
 * \struct mal_net_gsm_serving_cell_info
 * \brief Information related to the gsm serving cell
 *
 * \n This structure provides gsm cell information for the serving cell.
 */
typedef struct {
    uint16_t    current_mcc;               /**< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t    current_mnc;               /**< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t    current_lac;               /**< Location Area Code of the serving cell.  INTEGER (0..65535). */
    uint16_t    current_cid;               /**< GSM Cell number identifier of the serving cell. INTEGER (0..65535).*/
    uint16_t    current_arfcn;             /**< ARFCN - Absolute Radio Frequency Channel Number.  INTEGER (0..1023). */
    uint8_t     current_bsic;              /**< BSIC  - Base Station Identity Code.  INTEGER (0..63). */
    int8_t      current_rxlev;             /**< RXLEV - Received signal Level.  INTEGER (0..63). */
    uint8_t     current_timing_advance;    /**< Network Timing Advance. INTEGER (0..255).*/
    uint8_t     pad1;                      /**< Padding byte.*/
    uint8_t     pad2;                      /**< Padding byte.*/
    uint8_t     pad3;                      /**< Padding byte.*/
} mal_net_gsm_serving_cell_info;

/**
 * \struct mal_net_gsm_cell_info
 * \brief Information related to the gsm serving and neighbouring cells
 *
 * \n This structure provides gsm cell information for the serving cell and neighbouring cells.
 */
typedef struct {
    mal_net_gsm_serving_cell_info   gsm_serving_cell_info;  /**< GSM Serving cell information */
    mal_net_neigh_info_type         neigh_info_type;        /**< Type to check which structure is filled Neighbour Info or ECID Information*/
    mal_net_gsm_neigh_info_basic    gsm_neigh_info_basic;   /**< GSM Neighbour cells information */
    mal_net_gsm_neigh_info_extd     gsm_neigh_info_extd;    /**< Extended GSM Neighbour cells information or ECID Information */
} mal_net_gsm_cell_info;

/**
 * \struct mal_net_neighbour_cells_info
 * \brief Detailed Information related to gsm/umts serving and neighbour cells
 *
 * \n This structure provides cell information for gsm/umts serving cell and all neighbour cells.
 */
typedef struct {
    mal_net_neighbour_rat_type          rat_type;           /**< RAT type associated with neighbour cell information                  */
    union {
        mal_net_gsm_cell_info           gsm_cells_info;     /**< Information related to the gsm serving and neighbouring cell    */
        mal_net_umts_cell_info          umts_cells_info;    /**< Information related to the umts serving and neighbouring cell   */
    } cells_info;
} mal_net_neighbour_cells_info;

/**
 * \struct mal_net_signal_info
 * \brief RSSI and BER Information related to current cell
 *
 * \n This structure provides RSSI and BER Information related to current cell.
 */
typedef struct {
    uint8_t rssi_dbm; /**< |x|, where x is RSSI in dBm. (E.g.: If RSSI value
                        * if -84 dBm, rssi_dbm will be 84.
                        * Valid range: -123 <= x <= -1; Unknown: x == 0)
                        */
    uint8_t no_of_signal_bars; /**< Number of signal bars; Range: 0 to 100 */
    uint8_t ber;      /**< Bit Rate Error; Valid range: 0 to 7; Undefined: 0x63 */
    uint8_t num_of_segments; /**< num_of_segment is equal to
                               * (Number of border values -1).
                               * Ignore num_of_segments and border[] if
                               * num_of_segments is 0.
                               */
    uint8_t ecno;   /**<EcNo parameter; range: 0 dB to 49 dB */
    uint8_t rat;        /**<RAT info */
    uint8_t border[NET_RSSI_CONF_MAX_SEGMENTS+1];   /**< border values passed to CLIENT */
} mal_net_signal_info;

/**
 * \struct mal_net_cell_info
 * \brief Cell Information
 *
 * \n This structure provides current cell information.
 */
typedef struct {
    mal_net_cell_rat_type     rat;              /**< RAT applicable to cell information*/
    mal_net_network_type      network_type;     /**< Network Type */
    mal_net_nw_service_status service_status;   /**< Network Service Status. Cell Information
                                                  * in this structure is valid only if
                                                  * service_status is not MAL_NET_NO_COVERAGE.
                                                  */
    uint32_t                  gsm_frame_nbr;    /**< GSM frame Number; valid
                                                  * only if rat is MAL_NET_CELL_RAT_GSM;
                                                  * 0xFFFFFFFF means info not available
                                                  */
    uint32_t                  bands_available;  /**< Available bands. Bitmasks:
                                                 *  For MAL_NET_CELL_RAT_GSM:
                                                 *    GSM 900 band  : 0x00000001
                                                 *    GSM 1800 band : 0x00000002
                                                 *    GSM 1900 band : 0x00000004
                                                 *    GSM 850 band  : 0x00000008
                                                 *  For MAL_NET_CELL_RAT_WCDMA:
                                                 *    WCDMA FDD BAND 1  : 0x00000001
                                                 *    WCDMA FDD BAND 2  : 0x00000002
                                                 *    WCDMA FDD BAND 3  : 0x00000004
                                                 *    WCDMA FDD BAND 4  : 0x00000008
                                                 *    WCDMA FDD BAND 5  : 0x00000010
                                                 *    WCDMA FDD BAND 6  : 0x00000020
                                                 *    WCDMA FDD BAND 7  : 0x00000040
                                                 *    WCDMA FDD BAND 8  : 0x00000080
                                                 *    WCDMA FDD BAND 9  : 0x00000100
                                                 *    WCDMA FDD BAND 10 : 0x00000200
                                                 *    WCDMA FDD BAND 11 : 0x00000400
                                                 *    WCDMA FDD BAND 12 : 0x00000800
                                                 *    WCDMA FDD BAND 13 : 0x00001000
                                                 *    WCDMA FDD BAND 14 : 0x00002000
                                                 *    WCDMA FDD BAND 15 : 0x00004000
                                                 *    WCDMA FDD BAND 16 : 0x00008000
                                                 *    WCDMA FDD BAND 17 : 0x00010000
                                                 *    WCDMA FDD BAND 18 : 0x00020000
                                                 *    WCDMA FDD BAND 19 : 0x00040000
                                                 *    WCDMA FDD BAND 20 : 0x00080000
                                                 *    WCDMA FDD BAND 21 : 0x00100000
                                                 *    WCDMA FDD BAND 22 : 0x00200000
                                                 *    WCDMA FDD BAND 23 : 0x00400000
                                                 *    WCDMA FDD BAND 24 : 0x00800000
                                                 *  For MAL_NET_CELL_RAT_EPS:
                                                 *    EUTRAN BAND 1  : 0x00000001
                                                 *    EUTRAN BAND 2  : 0x00000002
                                                 *    EUTRAN BAND 3  : 0x00000004
                                                 *    EUTRAN BAND 4  : 0x00000008
                                                 *    EUTRAN BAND 5  : 0x00000010
                                                 *    EUTRAN BAND 6  : 0x00000020
                                                 *    EUTRAN BAND 7  : 0x00000040
                                                 *    EUTRAN BAND 8  : 0x00000080
                                                 *    EUTRAN BAND 9  : 0x00000100
                                                 *    EUTRAN BAND 10 : 0x00000200
                                                 *    EUTRAN BAND 11 : 0x00000400
                                                 */
    uint32_t                 current_cell_id;  /**< Current Cell ID */
    uint16_t                 current_ac;       /**< Location Area Code in case of GSM and WCDMA;
                                                 *  Tracking Area Code in case of EPS)
                                                 */
    uint16_t                 country_code;     /**< Country code */
    uint16_t                 network_code;     /**< Network code */
    uint8_t                 *mcc_mnc_string;   /**< Null-terminated string with MCC and MNC */
} mal_net_cell_info;

/**
 * \typedef void (*mal_net_event_cb_t)(int32_t event_id, void *data,
 *                            mal_net_error_type error_code, void *client_tag)
 * \brief Event notification to Application
 *
 * \n Callback function for event notification to the Application. This callback is
 * \n used for both solicited responses and unsolicited indications.
 *
 * \param [in] event_id    ID of the event being notified
 * \param [in] data        Pointer to data associated with the event being notified
 * \param [in] error_code  Error code associated with the event
 * \param [in] client_tag  Client tag associated with the response or indication
 *
 * \par
 * Following is the detail on data based on the event type:
 * \par
 * \arg MAL_NET_MODEM_REG_STATUS_GET_RESP
 * \n Pointer to \ref mal_net_registration_info
 * \par
 * \arg MAL_NET_MODEM_REG_STATUS_IND
 * \n Pointer to \ref mal_net_registration_info
 * \par
 * \arg MAL_NET_NITZ_NAME_IND
 * \n Pointer to \ref mal_net_name_info
 * \par
 * \arg MAL_NET_NETWORK_SELECT_MODE_SET_RESP
 * \n NULL
 * \par
 * \arg MAL_NET_AVAILABLE_CANCEL_RESP
 * \n NULL
 * \par
 * \arg MAL_NET_RSSI_CONF_RESP
 * \n NULL
 * \par
 * \arg MAL_NET_RSSI_GET_RESP, MAL_NET_RSSI_IND
 * \n Pointer to \ref mal_net_signal_info
 * \par
 * \arg MAL_NET_TIME_IND
 * \n Pointer to \ref mal_net_time_info
 * \par
 * \par
 * \arg MAL_NET_RADIO_INFO_IND
 * \n Pointer to \ref uint8_t returning rab status
 * \par
 * \arg MAL_NET_MODEM_AVAILABLE_GET_RESP
 * \n Pointer to \ref mal_net_manual_search_data
 * \par
 * \arg MAL_NET_SET_RESP
 * \n NULL
 * \par
 * \arg MAL_NET_DEREG_RESP
 * \n NULL
 * \par
 * \arg MAL_NET_CS_CONTROL_RESP
 * \n NULL
 * \arg MAL_NET_NEIGHBOUR_CELLS_RESP, MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO
 * \n Pointer to \ref mal_net_neighbour_cells_info
 * \par
 * \arg MAL_NET_MODEM_DETAILED_FAIL_CAUSE
 * \n Pointer to \ref uint8_t returning detail cause for modem error
 * \par
 * \arg MAL_NET_NMR_INFO_RESP
 * \n Pointer to \ref mal_net_nmr_info
 * \par
 * \arg MAL_NET_CELL_INFO_IND
 * \n Pointer to \ref mal_net_cell_info
 * \par
 * \arg MAL_NET_CELL_INFO_GET_RESP
 * \n Pointer to \ref mal_net_cell_info
 * \par
 * \arg MAL_NET_RAT_RESP
 * \n Pointer to \ref mal_net_rat_name
 * \par
 * \arg MAL_NET_RAT_IND
 * \n Pointer to \ref mal_net_rat_name
 */
typedef void (*mal_net_event_cb_t)(int32_t event_id, void *data,
                                   mal_net_error_type error_code,
                                   void *client_tag);

/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/

/**
 * \fn int32_t mal_net_init(int32_t *fd_net)
 * \brief Initialize NET Library
 *
 * \n This routine is used to initialize the Network Library and open the socket
 * \n using Phonet Liibrary. This routine should be called only once at the time
 * \n of initialization.
 * \param [out] fd_net   Pointer to file descriptor of NET socket
 * \return \ref          mal_net_error_type
 */
int32_t mal_net_init(int32_t *fd_net);

/**
 * \fn int32_t mal_net_deinit(void)
 * \brief De-initialize NET Library
 *
 * \n This routine is used to de-initialize the Network Library.
 * \param            void
 * \return    \ref   mal_net_error_type
 */
int32_t mal_net_deinit(void);

/**
 * \fn int32_t mal_net_register_callback(mal_net_event_cb_t event_cb)
 * \brief Register callbacks from application
 *
 * This routine registers the callback function for passing events
 * to the Application.
 * \param [in] event_cb     Event callback handler
 * \return    \ref          mal_net_error_type
 */
int32_t mal_net_register_callback(mal_net_event_cb_t event_cb);

/**
 * \fn void mal_net_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \n available from modem for processing.
 * \param    void
 * \return   void
 */
void mal_net_response_handler(void);

/**
 * \fn int32_t mal_net_control_cs (mal_net_cs_request_type req, void *client_tag)
 * \brief Control Cellular System
 *
 * \n This routine is used to control the Cellular System. Expected event type
 * \n in case of successful invocation of this routine is MAL_NET_CS_CONTROL_RESP.
 * \n NOTE: To be deprecated; do not use.
 * \n
 * \param [in] req           Requested CS state \ref mal_net_cs_request_type
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_control_cs(mal_net_cs_request_type req, void *client_tag);

/**
 * \fn int32_t mal_net_deregister(void *client_tag)
 * \brief De-register from the network
 *
 * \n This routine is used to invoke de-registration from the network. Expected
 * \n event type in case of successful invocation of this routine is
 * \n MAL_NET_DEREG_RESP.
 * \n
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_deregister(void *client_tag);

/**
 * \fn int32_t mal_net_register(void *client_tag)
 * \brief Register to the specified network
 *
 * \n This routine is used to invoke registration to the network. Expected event
 * \n type in case of successful invocation of this routine is MAL_NET_SET_RESP.
 * \n
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_register(void *client_tag);

/**
 * \fn int32_t mal_net_manual_register(mal_net_manual_reg_info *reg_info, void *client_tag)
 * \brief Register to the specified network and specified RAT
 *
 * \n This routine is used to invoke registration to the network based on MNC
 * \n and MCC of the network and the RAT. Expected event type in case of successful
 * \n invocation of this routine is MAL_NET_SET_RESP.
 * \n
 * \param [in] reg_info      Structure containing string of MNC and MCC of the
 *                                desired network and RAT parameter.
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_manual_register(mal_net_manual_reg_info *reg_info, void *client_tag);


/**
 * \fn int32_t mal_net_register_to_pref_rat(mal_net_pref_rat rat_seq, void *client_tag)
 * \brief Register Preffered Radio Access Technology (RAT) type to the specified network.
 *
 * \n This routine is used to invoke registration to the network based on
 * \n preffered Radio Access Technology (RAT) .
 * \n During Registration Preffered RAT type would be tried first,
 * \n then other RAT would be tried .
 * \n Expected event type in case of successful
 * \n invocation of this routine is MAL_NET_SET_RESP.
 * \n
 * \param [in] rat_seq       Enum for prefered RAT to be registered to the network
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_register_to_pref_rat(mal_net_pref_rat rat_seq, void *client_tag);

/**
 * \fn int32_t mal_net_manual_search(void *client_tag)
 * \brief Manual search of available networks
 *
 * \n This routine is used to invoke Manual Search of Networks. Expected event
 * \n type in case of successful invocation of this routine is
 * \n MAL_NET_MODEM_AVAILABLE_GET_RESP.
 *
 * \param [in] client_tag     Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref            mal_net_error_type
 */
int32_t mal_net_manual_search(void *client_tag);

/**
 * \fn int32_t mal_net_interrupt_search(void *client_tag)
 * \brief Interrupt search of networks
 *
 * \n This routine is used to interrupt Manual Search of Networks. Expected
 * \n event type in case of successful invocation of this routine is
 * \n MAL_NET_AVAILABLE_CANCEL_RESP.
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_interrupt_search(void *client_tag);

/**
 * \fn int32_t mal_net_get_encsq_mode(uint8_t *encsq_mode)
 * \brief Api to Get the value of encsq_mode .Possible values can be 1 or 0 (Enable / Disable)
 *
 * \n This routine is used to GET the current state encsq_mode.
 *
 *
 * \param [in] encsq_mode      Current Value of encsq_mode is filled and Passed Back.
 * \param [in] client_tag      Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref             mal_net_error_type
 */
int32_t mal_net_get_encsq_mode(uint8_t *encsq_mode);

/**
 * \fn int32_t mal_net_enable_disable_rssi(uint8_t encsq_mode, void *client_tag)
 * \brief Api to Enable/Disable the sending of MAL_NET_RSSI_IND
 *
 * \n This routine is used to Enable/Disable the sending of Net RSSI Indication.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_NET_RSSI_CONF_RESP.
 *
 * \param [in] encsq_mode      encsq_mode is passed 1 or 0 depending upon if
 *                             we want to Enable/Disable MAL_NET_RSSI_IND
 * \param [in] client_tag      Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref             mal_net_error_type
 */
int32_t mal_net_enable_disable_rssi(uint8_t encsq_mode, void *client_tag);

/**
 * \fn int32_t mal_net_configure_rssi(mal_net_rssi_conf_data *data, void *client_tag)
 * \brief Api to Configure the sending of MAL_NET_RSSI_IND
 *
 * \n This routine is used to configure the sending of Net RSSI Indication(MAL_NET_RSSI_IND).
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_NET_RSSI_CONF_RESP.
 *
 * \param [in] data          Pointer to Structure mal_net_rssi_conf_data.
 *                             Structure elements num_of_segments,hysteresis_down,
 *                             hysteresis_up,signal_level_type,low_limit,up_limit.
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_configure_rssi(mal_net_rssi_conf_data *data, void *client_tag);


/**
 * \fn int32_t mal_net_get_rssi_conf_values(mal_net_rssi_conf_data *rssi_conf_data);
 * \brief Api to Read the Configured Values for the sending of MAL_NET_RSSI_IND
 *
 * \n This routine reads the Configured Values for sending of Net RSSI Indication(MAL_NET_RSSI_IND).
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_NET_RSSI_CONF_RESP.
 *
 * \param [in] rssi_conf_data   Pointer to Structure mal_net_rssi_conf_data.
 *                              This will be filled and returned back.
 *                              Structure elements num_of_segments,hysteresis_down,
 *                              hysteresis_up,signal_level_type,low_limit,up_limit and ccsq_mode-255(DEFAULT).
 * \param [in] client_tag       Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref              mal_net_error_type
 */
int32_t mal_net_get_rssi_conf_values(mal_net_rssi_conf_data *rssi_conf_data);

/**
 * \fn int32_t mal_net_get_rssi(void *client_tag)
 * \brief Query for RSSI and BER information
 *
 * \n This routine is used for getting signal strength and Bit Error Rate (BER).
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_NET_RSSI_GET_RESP.
 * \param [in] client_tag  Void pointer to client specific data. The client
 *                           specific data is transparent to MAL and is
 *                           returned as-is in the response.
 * \return    \ref         mal_net_error_type
 */
int32_t mal_net_get_rssi(void *client_tag);

/**
 * \fn int32_t mal_net_set_mode(void *data, void *client_tag)
 * \brief Set network Search Mode
 *
 * \n This routine is used for setting Network Search Mode. Expected event type
 * \n in case of successful invocation of this routine is
 * \n MAL_NET_NETWORK_SELECT_MODE_SET_RESP.
 *
 * \param [in] data         Integer value specifying the mode to be set:
 *                               0 - Auotomatic Selection, 1 - Manual Selection
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    \ref          mal_net_error_type
 */
int32_t mal_net_set_mode(void *data, void *client_tag);

/**
 * \fn int32_t mal_net_query_mode(void* client_tag)
 * \brief Query current network selection mode
 *
 * \n This routine is used for querying network selection mode. Expected
 * \n event type in case of successful invocation of this routine is
 * \n MAL_NET_MODEM_REG_STATUS_GET_RESP.
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref      mal_net_error_type
 */
int32_t mal_net_query_mode(void *client_tag);

/**
 * \fn int32_t mal_net_req_operator_code(void *client_tag)
 * \brief Request for Operator Code
 *
 * \n This routine is used for requesting operator code. Expected event type
 * \n in case of successful invocation of this routine is
 * \n MAL_NET_MODEM_REG_STATUS_GET_RESP.
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_req_operator_code(void *client_tag);

/**
 * \fn int32_t mal_net_req_registration_state_normal(void *client_tag)
 * \brief Request for registration data
 *
 * \n This routine is used for querying GSM registration state. Expected event
 * \n type in case of successful invocation of this routine is
 * \n MAL_NET_MODEM_REG_STATUS_GET_RESP.
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_req_registration_state_normal(void *client_tag);

/**
 * \fn int32_t mal_net_req_registration_state_gprs(void *client_tag)
 * \brief Request for GPRS registration data
 *
 * \n This routine is used for querying GPRS registration state. Expected event
 * \n type in case of successful invocation of this routine is
 * \n MAL_NET_MODEM_REG_STATUS_GET_RESP.
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_req_registration_state_gprs(void *client_tag);

/**
 * \fn int32_t mal_net_get_neighbour_cell_ids (mal_net_neighbour_rat_type rat, void *client_tag, mal_net_neighbour_request_type request_type)
 * \brief Request for neighbour cell information
 *
 * \n This routine is used to invoke request for getting neighbour cell
 * \n information. Expected event type in case of successful invocation of
 * \n this routine is MAL_NET_NEIGHBOUR_CELLS_RESP.
 * \n
 * \param [in] rat   \ref  mal_net_neighbour_rat_type
 * \param [in] client_tag  Void pointer to client specific data. The client
 *                             specific data is transparent to MAL and is
 *                             returned as-is in the response.
 * \param [in] request_type \ref mal_net_neighbour_request_type
 * \return    \ref         mal_net_error_type
 */
int32_t mal_net_get_neighbour_cell_ids(mal_net_neighbour_rat_type rat, void *client_tag, mal_net_neighbour_request_type request_type);

/**
 * \fn void mal_net_set_control_unsol_neighbour_cell_info(uint8_t mode, mal_net_neighbour_rat_type rat)
 * \brief Disable/Enable Unsolicited Neighbour Cell Info
 *
 * \n This routine is used to enable or disable unsolicited neighbour cell
 * \n information and set associated RAT type. If enabled, then only event
 * \n MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO will be passed to upper layer.
 * \n
 * \param [in] mode  0 - Disable; 1 - Enable
 * \param [in] rat   \ref mal_net_neighbour_rat_type
 * \return     \ref  mal_net_error_type
 */
int32_t mal_net_set_control_unsol_neighbour_cell_info(uint8_t mode, mal_net_neighbour_rat_type rat);

/**
 * \fn void mal_net_get_control_unsol_neighbour_cell_info(uint8_t* mode, mal_net_neighbour_rat_type* rat)
 * \brief Disable/Enable Unsolicited Neighbour Cell Info
 *
 * \n This routine is used to read the status (enable or disable) for the RAT
 * \n type of unsolicited neighbour cell information. If enabled, then only event
 * \n MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO will be passed to upper layer.
 * \n
 * \param [out] mode 0 - Disable; 1 - Enable
 * \param [in]  rat  \ref mal_net_neighbour_rat_type
 * \return     \ref  mal_net_error_type
 */
int32_t mal_net_get_control_unsol_neighbour_cell_info(uint8_t *mode, mal_net_neighbour_rat_type rat);

/**
 * \fn int32_t mal_net_req_screen_state(mal_net_screen_state state, void *client_tag)
 * \brief Request to change screen state
 *
 * \n This routine is used to invoke request for changing screen state. When
 * \n the screen is requested to be off, the NET Library will notify the
 * \n baseband to suppress certain notifications (eg, signal strength and
 * \n changes in LAC/CID or BID/SID/NID/latitude/longitude) in an effort to
 * \n conserve power. When the screen is requested to be on, the NET Library
 * \n will notify the baseband to provide these notifications.
 * \n
 * \n NOTE: This is a synchronous call; there will be no response from modem.
 * \n       To be deprecated; do not use.
 * \n
 * \param [in] state \ref    Value from enum mal_net_screen_state
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_net_error_type
 */
int32_t mal_net_req_screen_state(mal_net_screen_state state, void *client_tag);

/**
 * \fn int32_t mal_net_set_location_updates(mal_net_location_update_action action, void *client_tag)
 * \brief Request to enable or disable location updates
 *
 * \n This routine is used to invoke request for enabling/disabling network
 * \n state change notifications due to changes in LAC and/or CID (for GSM)
 * \n or BID/SID/NID/latitude/longitude (for CDMA).
 *
 * \n NOTE: This is a synchronous call; there will be no response from modem.
 * \n       To be deprecated; do not use.
 * \n
 * \param [in] action \ref    Value from enum mal_net_location_update_action
 * \param [in] client_tag     Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref            mal_net_error_type
 */
int32_t mal_net_set_location_updates(mal_net_location_update_action action, void *client_tag);

/**
 * \fn int32_t mal_net_set_ind_state(mal_net_ind_select_info indication,void *client_tag)
 * \brief Request to enable or disable any specific net server indications
 *
 * \n This routine is used to invoke request for enabling/disabling any network
 * \n indications
 * \n
 * \param [in] ind_type \ref  Value from enum mal_net_ind_type
 * \param [in] ind_state      Void from enum mal_net_ind_state.
 *
 * \return    \ref            mal_net_error_type
 */
int32_t mal_net_set_ind_state(mal_net_ind_type ind_type, mal_net_ind_state ind_state);

/**
 * \fn int32_t mal_net_get_ind_state(mal_net_ind_type ind_type, mal_net_ind_state *ind_state)
 * \brief Request to get the enable/disable status of specific net server indications
 *
 * \n This routine is used to get the enable/disable stauts any network
 * \n indications
 * \n
 * \param [in] ind_type \ref  Value from structure mal_net_ind_select_info
 * \param [out] ind_state      Pointer to mal_net_ind_state having value for current
 *                             indication state.
 *
 * \return    \ref            mal_net_error_type
 */
int32_t mal_net_get_ind_state(mal_net_ind_type ind_type, mal_net_ind_state *ind_state);

/**
 * \fn int32_t mal_net_get_rab_status(mal_net_rab_status_type *rab_status)
 * \brief Request to get the enable/disable RAB status (HSDPA/HSUPA status)
 *
 * \n This routine is used to get the RAB status in network info indication
 * \n
 * \param [out] rab_status   Pointer to mal_net_rab_status_type having value for
 *
 *
 * \return    \ref            mal_net_error_type
 */
int32_t mal_net_get_rab_status(mal_net_rab_status_type *rab_status);

/**
 * \fn int32_t mal_net_configure_network_access(mal_net_nw_access_conf_data *conf_data,
 *                                              void *client_tag)
 * \brief Request for configuring network access
 *
 * \n This routine is used for configuring network access. Currently this
 * \n routine is expected to be used internally, and there is no event
 * \n associated with the invocation of this routine.
 *
 * \param [in] conf_data    Pointer to requested configuration data
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    \ref          mal_net_error_type
 */
int32_t mal_net_configure_network_access(mal_net_nw_access_conf_data *conf_data,
        void *client_tag);

/**
 * \fn int32_t mal_net_test_carrier(mal_net_test_carrier_data *carrier_data,
 *                                              void *client_tag)
 * \brief Request for Setting/Clearing  the production carrier
 *
 * \n This routine is used to setup the production carrier and the access
 * \n technology related setting parameters.
 *
 * \param [in] carrier_data    Pointer to requested carrier data
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    \ref          mal_net_error_type
 */
int32_t mal_net_test_carrier(mal_net_test_carrier_data *carrier_data,
                             void *client_tag);


/**
 * \fn int32_t int32_t mal_net_req_nmr_info(mal_net_nmr_rat_type rat_type,
 * \n                    mal_net_utran_nmr_type nmr_type, void* client_tag)
 * \brief Request to get the NMR information
 *
 * \n This routine is used to get NMR information for GERAN and UTRAN.
 * \n
 * \param [in] rat_type    \ref mal_net_nmr_rat_type
 * \param [in] nmr_type    \ref mal_net_utran_nmr_type Processed only if rat
 *                             is MAL_NET_NMR_RAT_UTRAN.
 * \param [in] client_tag  Void pointer to client specific data. The client
 *                             specific data is transparent to MAL and is
 *                             returned as-is in the response.
 * \return    \ref            mal_net_error_type
 */
int32_t mal_net_req_nmr_info(mal_net_nmr_rat_type rat_type,
                             mal_net_utran_nmr_type nmr_type, void *client_tag);

/**
 *  \fn int32_t mal_net_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for net module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_net_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_net_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for net module. This is a synchronous call.
 *   \param[out] debug level for net module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_net_request_get_debug_level(uint8_t *level);

/**
 * \fn void mal_net_req_cell_info(void *client_tag)
 * \brief Request service status and general cell information.
 *
 * \n This routine is used to request service status and general cell information.
 * \param [in] client_tag  Void pointer to client specific data. The client
 *                             specific data is transparent to MAL and is
 *                             returned as-is in the response.
 * \return     \ref  mal_net_error_type
 */
int32_t mal_net_req_cell_info(void *client_tag);

/**
 * \fn int32_t mal_net_configure_reg_status(uint8_t reg_status_mode, void *client_tag)
 * \brief Configure NET_MODEM_REG_STATUS_IND frequency.
 *
 * \n This routine is used to configure the frequency of registration status indication.
 *
 * \param [in] reg_status_mode \ref Value from enum mal_net_reg_status_set_mode
 * \param [in] client_tag  Void pointer to client specific data. The client
 *                             specific data is transparent to MAL and is
 *                             returned as-is in the response.
 * \return     \ref  mal_net_error_type
 */
int32_t mal_net_configure_reg_status(mal_net_reg_status_set_mode reg_status_mode, void *client_tag);

/**
 * \fn
 * \brief Request current RAT name.
 *
 * \n This routine is used to request the current RAT name.
 * \n Expected event type in case of successful invocation of
 * \n this routine is MAL_NET_RAT_RESP.
 *
 * \param [in] client_tag  Void pointer to client specific data. The client
 *                             specific data is transparent to MAL and is
 *                             returned as-is in the response.
 * \return     \ref  mal_net_error_type
 */
int32_t mal_net_req_current_rat_name(void *client_tag);

#endif /* MAL_NET_H */
