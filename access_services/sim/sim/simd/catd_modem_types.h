/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd_modem_types.h
 * Description     : catd modem types. The file collects data types in modem
 *
 * Author          :
 *
 */


#ifndef __catd_modem_types_h__
#define __catd_modem_types_h__ (1)

#include <stdlib.h>

// TODO: Put more defs here! Most are now in catd_modem.h

#define STE_STATUS(MAL_STAT, MAL_REQ, MAL_CMD)  \
        (((uint32_t)(MAL_STAT) << 16) |         \
        ((uint32_t)(MAL_REQ) << 8) |            \
        ((uint32_t)(MAL_CMD)))

#define STE_UICC_NOT_READY 0
#define STE_UICC_READY 1

// Reason codes for sim_state. Right now, very similar to the
// ste_uicc_status. This is because at the moment we issue status changes
// in parallel to sim_state_change, and it is the sending of ste_uicc_status
// using uiccd_sig_uicc_status* family. In the future, this list will expand.
typedef enum {
  STE_REASON_UNKNOWN = 0, // Msg for no particular reason
  STE_REASON_STARTUP_DONE,
  STE_REASON_PIN_NEEDED,
  STE_REASON_PIN_VERIFIED,
  STE_REASON_PUK_NEEDED,
  STE_REASON_PIN2_NEEDED,
  STE_REASON_PIN2_VERIFIED,
  STE_REASON_PUK2_NEEDED,
  STE_REASON_DISCONNECTED_CARD,
// Error sort of reason codes
  STE_REASON_PERMANENTLY_BLOCKED = 100,
  STE_REASON_NO_CARD,
  STE_REASON_REJECTED_CARD_INVALID,
  STE_REASON_REJECTED_CARD_SIM_LOCK,
  STE_REASON_REJECTED_CARD_CONSECUTIVE_6F00
} ste_reason_t;

typedef enum {
  STE_SIM_APP_TYPE_UNKNOWN = 0,
  STE_SIM_APP_TYPE_SIM,
  STE_SIM_APP_TYPE_USIM,
  STE_SIM_APP_TYPE_ISIM
} ste_sim_app_type_t;

typedef enum {
  STE_SIM_APP_STATUS_UNKNOWN,
  STE_SIM_APP_STATUS_ACTIVE,
  STE_SIM_APP_STATUS_NOT_ACTIVE,
  STE_SIM_APP_STATUS_ACTIVE_PIN,
  STE_SIM_APP_STATUS_ACTIVE_PUK
} ste_sim_app_status_t;

typedef enum {
  STE_SIM_PIN_MODE_NORMAL,
  STE_SIM_PIN_MODE_REPLACED
} ste_sim_app_pin_mode_t;

typedef enum {
  STE_SIM_APP_TELEKOM_UNKNOWN_MARK, // Unknown if it is marked for something or not
  STE_SIM_APP_TELEKOM_UNMARKED, // Unmarked
  STE_SIM_APP_TELEKOM_GSM_UMTS_MARKED, // This app is the gsm umts app to select
  STE_SIM_APP_TELEKOM_CDMA_MARKED // This app is the cdma app to select
} ste_sim_app_mark_t;

#define STE_SIM_APP_ID_UNKNOWN  0

typedef enum {
  STE_SIM_PIN_STATUS_UNKNOWN,
  STE_SIM_PIN_STATUS_ENABLED,
  STE_SIM_PIN_STATUS_DISABLED,
  STE_SIM_PIN_STATUS_ENABLED_NOT_VERIFIED,
  STE_SIM_PIN_STATUS_ENABLED_VERIFIED,
  STE_SIM_PIN_STATUS_ENABLED_BLOCKED,
  STE_SIM_PIN_STATUS_ENABLED_PERM_BLOCKED
} ste_sim_pin_status_t;

typedef enum {
  STE_SIM_STATUS_OK = 0,
  STE_SIM_STATUS_FAIL,
  STE_SIM_STATUS_UNKNOWN
} ste_sim_status_t;

typedef enum {
  STE_SIM_CARD_STATUS_UNKNOWN = 0,
  STE_SIM_CARD_STATUS_READY,
  STE_SIM_CARD_STATUS_NOT_READY,
  STE_SIM_CARD_STATUS_MISSING,
  STE_SIM_CARD_STATUS_INVALID,
  STE_SIM_CARD_STATUS_DISCONNECTED
} ste_sim_card_status_t;

typedef enum {
  STE_UICC_STATUS_UNKNOWN = 0, // Before uicc_card_ind(CARD_READY)
  STE_UICC_STATUS_INIT,    // card ready to be iniialized
  STE_UICC_STATUS_PIN_NEEDED, // pin_ind(card needs pin
  STE_UICC_STATUS_PUK_NEEDED, // pin_ind(card needs pin
  STE_UICC_STATUS_PIN2_NEEDED, // pin_ind(card needs pin
  STE_UICC_STATUS_PUK2_NEEDED, // pin_ind(card needs pin
  STE_UICC_STATUS_PIN_VERIFIED, // If pin was verified after main pin unlock
  STE_UICC_STATUS_PIN2_VERIFIED, // If pin2 was verified after main pin unlock
  STE_UICC_STATUS_READY, // pin was verified etc uicc_ind(startup_completed)
  STE_UICC_STATUS_STARTUP_COMPLETED, //uicc_ind(startup_completed)
  STE_UICC_STATUS_REJECTED_CARD_INVALID, // SIM card has been rejected due to invalid card
  STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK, // SIM card has been rejected due to SIM Lock validation failure
  STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00, // SIM card has been rejected due to three consecutive 6F00 status words.
  STE_UICC_STATUS_NO_CARD,
  STE_UICC_STATUS_REMOVED_CARD, // SIM card removed from reader
  STE_UICC_STATUS_REMOVED_CARD_PRESENCE_DETECTION, // SIM card removed due to presence detection.
  STE_UICC_STATUS_DISCONNECTED_CARD,
  STE_UICC_STATUS_CLOSED, // uicc_ind(shutting_down) and others?
  STE_UICC_STATUS_INTERNAL_CARD_RECOVERY // Internal to indicate Card is being recovered
} ste_uicc_status_t;

typedef enum {
  STE_UICC_CARD_TYPE_UNKNOWN,
  STE_UICC_CARD_TYPE_ICC,
  STE_UICC_CARD_TYPE_UICC,
  STE_UICC_CARD_TYPE_USB
} ste_uicc_card_type_t;

/**
 * @brief Data structure used for the PIN id
 * @param SIM_UICCD_PIN_ID_PIN1 The pin id for PIN1
 * @param SIM_UICCD_PIN_ID_PIN2 The pin id for PIN2
 * @param SIM_UICC_PIN_ID_UNKNOWN Value used when performing PIN operations in locked states.
 */
typedef enum {
  SIM_UICC_PIN_ID_PIN1 = 0,
  SIM_UICC_PIN_ID_PIN2,
  SIM_UICC_PIN_ID_UNKNOWN
} sim_uicc_pin_id_t;

/**
 * @brief Data structure used for the PIN id
 * @param SIM_UICC_PIN_PIN1 The pin id for PIN1
 * @param SIM_UICC_PIN_PUK1 The pin id for PUK1
 * @param SIM_UICC_PIN_PIN2 The pin id for PIN2
 * @param SIM_UICC_PIN_PUK2 The pin id for PUK2
 */
typedef enum {
  SIM_UICC_PIN_PIN1 = 0,
  SIM_UICC_PIN_PUK1,
  SIM_UICC_PIN_PIN2,
  SIM_UICC_PIN_PUK2
} sim_uicc_pin_puk_id_t;

/*
 * @brief Data structure containing the two status words defined by 3GPP TS 51.011
 * @param sw1 Status word 1
 * @param sw2 Status word 2
 */
typedef struct {
  uint8_t sw1;
  uint8_t sw2;
} sim_uicc_status_word_t;

/*
 * @brief Data structure used by the SAP response messages
 */
typedef enum
{
  SIM_UICC_SAP_STATUS_CODE_OK = 0,      /**< The UICC SAP request was performed successfully */
  SIM_UICC_SAP_STATUS_CODE_FAIL,        /**< The UICC SAP request failed. */
  SIM_UICC_SAP_STATUS_CODE_BUSY_ON_CALL /**< The request could not be performed due to ongoing call */
} sim_uicc_sap_status_code_t;

/**
 * @brief Structure used by the response functions
 *
 */
typedef enum
{
  SIM_UICC_STATUS_CODE_OK = 0,                   /**< The UICC request was performed successfully */
  SIM_UICC_STATUS_CODE_FAIL,                     /**< The UICC request failed. Status code fail details parameter provides more details */
  SIM_UICC_STATUS_CODE_UNKNOWN,                  /**< Status is unknown */
  SIM_UICC_STATUS_CODE_NOT_READY,                /**< The UICC Server is not ready */
  SIM_UICC_STATUS_CODE_SHUTTING_DOWN,            /**< The UICC server could not serve the request since it is shutting down */
  SIM_UICC_STATUS_CODE_CARD_READY,               /**< The card is ready */
  SIM_UICC_STATUS_CODE_CARD_NOT_READY,           /**< The UICC server could not serve the request since the card is not ready */
  SIM_UICC_STATUS_CODE_CARD_DISCONNECTED,        /**< The UICC server could not serve the request since the card is disconnected */
  SIM_UICC_STATUS_CODE_CARD_NOT_PRESENT,         /**< The UICC server could not serve the request since the card is not present */
  SIM_UICC_STATUS_CODE_CARD_REJECTED,            /**< The UICC server could not serve the request since the card has been rejected */
  SIM_UICC_STATUS_CODE_APPL_ACTIVE,              /**< The application is active */
  SIM_UICC_STATUS_CODE_APPL_ACTIVE_PIN,          /**< The application is active and needs PIN*/
  SIM_UICC_STATUS_CODE_APPL_ACTIVE_PUK,          /**< The application is active and needs PUK*/
  SIM_UICC_STATUS_CODE_APPL_NOT_ACTIVE,          /**< The application is not active */
  SIM_UICC_STATUS_CODE_PIN_ENABLED,              /**< PIN verification is enabled */
  SIM_UICC_STATUS_CODE_PIN_ENABLED_NOT_VERIFIED, /**< PIN verification is enabled but not verified */
  SIM_UICC_STATUS_CODE_PIN_ENABLED_VERIFIED,     /**< PIN verification is enabled and verified */
  SIM_UICC_STATUS_CODE_PIN_ENABLED_BLOCKED,      /**< PIN verification is enabled but PUK needed */
  SIM_UICC_STATUS_CODE_PIN_ENABLED_PERM_BLOCKED, /**< PIN verification is enabled but perm blocked */
  SIM_UICC_STATUS_CODE_PIN_DISABLED             /**< PIN verification is disabled */
} sim_uicc_status_code_t;

/** @brief Structure used by the response function
 *  Details are provided when the sim_uicc_status_code_t differs from STE_UICC_STATUS_CODE_OK and
 *  STE_UICC_STATUS_CODE_FAIL.
 *
 */
typedef enum
{
  SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS = 0,                     /**< No details are available */
  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS,                 /**< The parameters supplied in the request are invalid */
  SIM_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND,                     /**< The requested file was not found */
  SIM_UICC_STATUS_CODE_FAIL_DETAILS_SECURITY_CONDITIONS_NOT_SATISFIED,  /**< The caller has insufficient access rights for the request */
  SIM_UICC_STATUS_CODE_FAIL_DETAILS_CARD_ERROR,                         /**< Could not communicate with the card */
  SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,              /**< The request is currently not supported */
  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR                      /**< An internal error has occured */
} sim_uicc_status_code_fail_details_t;

/**
 * @brief A type to indicate the kind of information to be read.
 */
typedef enum {
  SIM_UICC_GET_FILE_INFO_TYPE_EF,     /**< flag to indicate reading EF */
  SIM_UICC_GET_FILE_INFO_TYPE_DF,     /**< flag to indicate reading DF */
} sim_uicc_get_file_info_type_t;


/**
 * @brief Service type code used to specify service in service table.
 *
 */
typedef enum
{
  SIM_UICC_SERVICE_TYPE_LOCAL_PHONE_BOOK = 0,
  SIM_UICC_SERVICE_TYPE_FDN,                       // Fixed Dialling Number
  SIM_UICC_SERVICE_TYPE_EXT_2,                     // Extension 2
  SIM_UICC_SERVICE_TYPE_SDN,                       // Service Dialling Number
  SIM_UICC_SERVICE_TYPE_EXT_3,                     // Extension 3
  SIM_UICC_SERVICE_TYPE_BDN,                       // Barred Dialling Number
  SIM_UICC_SERVICE_TYPE_EXT_4,                     // Extension 4
  SIM_UICC_SERVICE_TYPE_OCI_AND_OCT,               // Outgoing Call Information (OCI and OCT)
  SIM_UICC_SERVICE_TYPE_ICI_AND_ICT,               // Incoming Call Information (ICI and ICT)
  SIM_UICC_SERVICE_TYPE_SMS,                       // Short Message Storage
  SIM_UICC_SERVICE_TYPE_SMSR,                      // Short Message Service Reports
  SIM_UICC_SERVICE_TYPE_SMSP,                      // Short Message Service Parameters
  SIM_UICC_SERVICE_TYPE_AOC,                       // Advice of Charge
  SIM_UICC_SERVICE_TYPE_CCP,                       // Capability Configuration Parameters
  SIM_UICC_SERVICE_TYPE_CBMI,                      // Cell Broadcast Message Identifier
  SIM_UICC_SERVICE_TYPE_CBMIR,                     // Cell Broadcast Message Identifier Ranges
  SIM_UICC_SERVICE_TYPE_GID1,                      // Group Identifier Level 1
  SIM_UICC_SERVICE_TYPE_GID2,                      // Group Identifier Level 2
  SIM_UICC_SERVICE_TYPE_SPN,                       // Service Provider Name
  SIM_UICC_SERVICE_TYPE_PLMNWACT,                  // User controlled PLMN selector with Access Technology
  SIM_UICC_SERVICE_TYPE_MSISDN,                    // MSISDN
  SIM_UICC_SERVICE_TYPE_IMG,                       // Image
  SIM_UICC_SERVICE_TYPE_SOLSA,                     // Support of Localised Service Areas
  SIM_UICC_SERVICE_TYPE_ENH_PRECEDENCE_PREEMPTION, // Enhanced Multi-Level Precedence and Pre-emption Service
  SIM_UICC_SERVICE_TYPE_AAEM,                      // Automatic Answer for eMLPP
  SIM_UICC_SERVICE_TYPE_GSM_ACCESS,                // GSM Access
  SIM_UICC_SERVICE_TYPE_SMS_PP,                    // Data download via SMS-PP
  SIM_UICC_SERVICE_TYPE_SMS_CB,                    // Data download via SMS-CB
  SIM_UICC_SERVICE_TYPE_CALL_CONTROL_BY_USIM,      // Call Control by USIM
  SIM_UICC_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM,    // MO-SMS Control by USIM
  SIM_UICC_SERVICE_TYPE_RUN_AT_COMMAND,            // RUN AT COMMAND command
  SIM_UICC_SERVICE_TYPE_EST,                       // Enabled Services Table
  SIM_UICC_SERVICE_TYPE_ACL,                       // APN Control List
  SIM_UICC_SERVICE_TYPE_DCK,                       // Depersonalisation Control Keys
  SIM_UICC_SERVICE_TYPE_CNL,                       // Co-operative Network List
  SIM_UICC_SERVICE_TYPE_GSM_SECURITY_CONTEXT,      // GSM security context
  SIM_UICC_SERVICE_TYPE_CPBCCH,                    // CPBCCH Information
  SIM_UICC_SERVICE_TYPE_INV_SCAN,                  // Investigation Scan
  SIM_UICC_SERVICE_TYPE_MEXE,                      // MExE Service table
  SIM_UICC_SERVICE_TYPE_OPLMNWACT,                 // Operator controlled PLMN selector with Access Technology
  SIM_UICC_SERVICE_TYPE_HPLMNWACT,                 // HPLMN selector with Access Technology
  SIM_UICC_SERVICE_TYPE_EXT_5,                     // Extension 5
  SIM_UICC_SERVICE_TYPE_PNN,                       // PLMN Network Name
  SIM_UICC_SERVICE_TYPE_OPL,                       // Operator PLMN List
  SIM_UICC_SERVICE_TYPE_MBDN,                      // Mailbox Dialling Numbers
  SIM_UICC_SERVICE_TYPE_MWIS,                      // Message Waiting Indication Status
  SIM_UICC_SERVICE_TYPE_CFIS,                      // Call Forwarding Indication Status
  SIM_UICC_SERVICE_TYPE_SPDI,                      // Service Provider Display Information
  SIM_UICC_SERVICE_TYPE_MMS,                       // Multimedia Messaging Service
  SIM_UICC_SERVICE_TYPE_EXT_8,                     // Extension 8
  SIM_UICC_SERVICE_TYPE_CALL_CONTROL_ON_GPRS,      // Call control on GPRS by USIM
  SIM_UICC_SERVICE_TYPE_MMSUCP,                    // MMS User Connectivity Parameters
  SIM_UICC_SERVICE_TYPE_NIA,                       // Network's indication of alerting in the MS
  SIM_UICC_SERVICE_TYPE_VGCS_AND_VGCSS,            // VGCS Group Identifier List
  SIM_UICC_SERVICE_TYPE_VBS_AND_VBSS,              // VBS Group Identifier List
  SIM_UICC_SERVICE_TYPE_PSEUDO,                    // Pseudonym
  SIM_UICC_SERVICE_TYPE_UPLMNWLAN,                 // User Controlled PLMN selector for WLAN access
  SIM_UICC_SERVICE_TYPE_OPLMNWLAN,                 // Operator Controlled PLMN selector for WLAN access
  SIM_UICC_SERVICE_TYPE_USER_CONTR_WSID_LIST,      // User controlled WSID list
  SIM_UICC_SERVICE_TYPE_OPERATOR_CONTR_WSID_LIST,  // Operator controlled WSID list
  SIM_UICC_SERVICE_TYPE_VGCS_SECURITY,             // VGCS security
  SIM_UICC_SERVICE_TYPE_VBS_SECURITY,              // VBS security
  SIM_UICC_SERVICE_TYPE_WRI,                       // WLAN Reauthentication Identity
  SIM_UICC_SERVICE_TYPE_MMS_STORAGE,               // Multimedia Messages Storage
  SIM_UICC_SERVICE_TYPE_GBA,                       // Generic Bootstrapping Architecture
  SIM_UICC_SERVICE_TYPE_MBMS_SECURITY,             // MBMS security
  SIM_UICC_SERVICE_TYPE_DDL_USSD_APPL_MODE,        // Data download via USSD and USSD application mode
  SIM_UICC_SERVICE_TYPE_ADD_TERM_PROF,             // Additional TERMINAL PROFILE after UICC activation
  SIM_UICC_SERVICE_TYPE_CHV1_DISABLE,              // CHV1 disable function
  SIM_UICC_SERVICE_TYPE_ADN,                       // Abbreviated Dialling Numbers
  SIM_UICC_SERVICE_TYPE_PLMNSEL,                   // PLMN selector
  SIM_UICC_SERVICE_TYPE_EXT_1,                     // Extension 1
  SIM_UICC_SERVICE_TYPE_LND,                       // Last Number Dialled
  SIM_UICC_SERVICE_TYPE_MENU_SELECT,               // Menu selection
  SIM_UICC_SERVICE_TYPE_CALL_CONTROL,              // Call control
  SIM_UICC_SERVICE_TYPE_PROACTIVE_SIM,             // Proactive SIM
  SIM_UICC_SERVICE_TYPE_MO_SMS_CONTR_BY_SIM,       // Mobile Originated Short Message control by SIM
  SIM_UICC_SERVICE_TYPE_GPRS,                      // GPRS
  SIM_UICC_SERVICE_TYPE_USSD_STR_DATA_SUPPORT,     // USSD string data object supported in Call Control
  SIM_UICC_SERVICE_TYPE_ECCP,                      // Extended Capability Configuration Parameters
  SIM_UICC_SERVICE_TYPE_UNKNOWN
} sim_uicc_service_type_t;


typedef enum {
  SIM_UICC_SERVICE_STATUS_UNKNOWN = 0,
  SIM_UICC_SERVICE_STATUS_ENABLED,
  SIM_UICC_SERVICE_STATUS_DISABLED
} sim_uicc_service_status_t;


typedef enum {
  SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN = 0,
  SIM_UICC_SERVICE_AVAILABLE,
  SIM_UICC_SERVICE_NOT_AVAILABLE
} sim_uicc_service_availability_t;

/**
 * Struct used to store output data returned of the function
 * ste_mal_card_status
 */
typedef struct {
  int num_apps;
  ste_uicc_card_type_t card_type;
  ste_sim_card_status_t card_status;
} ste_mal_card_status_data_t;

typedef struct {
  ste_sim_app_type_t     app_type;
  ste_sim_app_status_t   app_status;
  ste_sim_app_pin_mode_t pin_mode;
  int                    activated_app; // 1 if (to be) activated, else 0
  uint8_t                *aid;    // Size is max 16
  size_t                 aid_len; // Actual size
  char                   *label;  // Recommended to be < 32, but not a req
  size_t                 label_len;  // Actual length
  ste_sim_pin_status_t   pin_state;
  ste_sim_pin_status_t   pin2_state;
} ste_mal_app_status_data_t;

/*
 * @brief Data values for parameter Command Force, used when sending an APDU
 */
typedef enum {
  STE_UICC_APDU_CMD_FORCE_NOT_USED = 0,
  STE_UICC_APDU_CMD_FORCE
} ste_uicc_apdu_command_force_t;

#endif
