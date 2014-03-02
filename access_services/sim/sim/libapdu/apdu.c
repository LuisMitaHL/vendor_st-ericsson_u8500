/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : apdu.c
 * Description     : Apdu handling.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */

#include "apdu.h"
#include "apdu_utilities.h"
#include "sim_internal.h"
#include "sim.h"
#include "sim_unused.h"

#include <string.h>
#include <assert.h>

#define STE_SAT_MAX_APDU_SIZE               (0xFF)
//constants for specific TLVs.
#define COMMAND_DETAILS_TOTAL_SIZE          (5)   /* Ref. TS 31.111 section 8.6 */
#define COMMAND_DETAILS_LENGTH              (3)   /* Ref. TS 31.111 section 8.6 */
#define DEVICE_IDENTITIES_TOTAL_SIZE        (4)   /* Ref. TS 31.111 section 8.7 */
#define DEVICE_IDENTITIES_LENGTH            (2)   /* Ref. TS 31.111 section 8.7 */
#define ICON_IDENTITIES_TOTAL_SIZE          (4)   /* Ref. TS 31.111 section 8.31 */
#define ICON_IDENTITIES_LENGTH              (2)   /* Ref. TS 31.111 section 8.31 */
#define IMMEDIATE_RESPONSE_TOTAL_SIZE       (2)   /* Ref. TS 31.111 section 8.43 */
#define IMMEDIATE_RESPONSE_LENGTH           (0)   /* Ref. TS 31.111 section 8.43 */
#define DURATION_TOTAL_SIZE                 (4)   /* Ref. TS 31.111 section 8.8 */
#define DURATION_LENGTH                     (2)   /* Ref. TS 31.111 section 8.8 */
#define BATTERY_TOTAL_SIZE                  (3)   /* Ref. TS 31.111 section 8.76 */
#define BATTERY_LENGTH                      (1)   /* Ref. TS 31.111 section 8.76 */
#define ACCESS_TECHNOLOGY_TOTAL_SIZE        (3)
#define ACCESS_TECHNOLOGY_LENGTH            (1)
#define LOCATION_TOTAL_SIZE                 (9)
#define LOCATION_LENGTH                     (7)
#define LOCATION_EXTENDED_TOTAL_SIZE        (11)
#define LOCATION_EXTENDED_LENGTH            (9)
#define SEARCH_MODE_TOTAL_SIZE              (3)   /* Ref. TS 102.223 section 8.75 */
#define SEARCH_MODE_LENGTH                  (1)   /* Ref. TS 102.223 section 8.75 */
#define FRAME_IDENTIFIER_TOTAL_SIZE         (3)   /* Ref. TS 31.111 section 6.5.6 */
#define FRAME_IDENTIFIER_LENGTH             (1)   /* Ref. TS 31.111 section 6.5.6 */
#define GENERAL_RESULT_SIZE                 (1)   /* Ref. TS 31.111 section 8.12 */
#define ADDITIONAL_INFO_SIZE                (1)   /* Ref. TS 31.111 section 8.12 */
#define RESPONSE_LENGTH_TOTAL_SIZE          (4)   /* Ref. TS 102.223 section 8.11 */
#define RESPONSE_LENGTH_LENGTH              (2)   /* Ref. TS 102.223 section 8.11 */
#define TIMER_IDENTIFIER_TOTAL_SIZE         (3)
#define TIMER_VALUE_TOTAL_SIZE              (5)
#define TIMER_IDENTIFIER_LENGTH             (1)
#define TIMER_VALUE_LENGTH                  (3)
#define TIMING_ADVANCE_TOTAL_SIZE           (4)
#define TIMING_ADVANCE_LENGTH               (2)
#define LOCATION_NMR_GERAN_SIZE             (16)
#define LOCATION_NMR_GERAN_SIZE_TOTAL       (18)
#define LOCATION_DATE_TIME_TIMEZONE_LENGTH  (7)
#define LOCATION_DATE_TIME_TIMEZONE_TOTAL   (9)

#define MAX_TEXT_STRING_LENGTH            (0xFF)

#define TAG_WITH_ONE_LENGTH_BYTE      (2)
#define TAG_WITH_TWO_LENGTH_BYTES     (3)

#define UCS2_FORMAT                                           (0x80)
#define UCS2_81_PREFIX_FORMAT                                 (0x81)
#define UCS2_82_PREFIX_FORMAT                                 (0x82)

/** Maximum size in bytes of sub-address */
#define SAT_MAX_SIZE_SUBADDRESS               (80)
/** Maximum size in bytes of Capability Configuration Parameters */
#define SAT_MAX_SIZE_CCP                      (14)
/** Maximum number of bearer parameters */
#define SAT_MAX_NUMBER_BEARER_PARAMS           (6)
/** Maximum size in bytes of DTMF String */
#define SAT_MAX_SIZE_DTMF_STRING              (80)
/** Maximum size in bytes of SMS TPDU */
#define SAT_MAX_SIZE_SMS_TPDU                (165)
/** Maximum size in bytes of Send SMS TPDU */
#define SAT_MAX_SIZE_SEND_SMS_TPDU           (184)

#define SIZE_TRANSACTION_ID_TLV       (3)     /* Only one transaction ID Ref.TS 31.111 7.5.x */
#define SIZE_EVENT_LIST_TLV           (3)     /* Only one event list item Ref.TS 31.111 7.5.x */

/* BER TLV tags */
#define STE_SAT_PROACTIVE_COMMAND_TAG                         (0xD0)
#define STE_SAT_SMS_PP_DOWNLOAD_TAG                           (0xD1)
#define STE_SAT_CB_DOWNLOAD_TAG                               (0xD2)
#define STE_SAT_MENU_SELECTION_TAG                            (0xD3)
#define STE_SAT_CALL_CONTROL_TAG                              (0xD4)
#define STE_SAT_MO_SMS_CONTROL_TAG                            (0xD5)
#define STE_SAT_EVENT_DOWNLOAD_TAG                            (0xD6)
#define STE_SAT_TIMER_EXPIRATION_TAG                          (0xD7)

/* Device identities */

#define DEVICE_KEYPAD                                         (0x01)
#define DEVICE_DISPLAY                                        (0x02)
#define DEVICE_EARPIECE                                       (0x03)
#define DEVICE_SIM                                            (0x81)
#define DEVICE_ME                                             (0x82)
#define DEVICE_NETWORK                                        (0x83)

#define DEVICE_CHANNEL1                                       (0x21)
#define DEVICE_CHANNEL2                                       (0x22)
#define DEVICE_CHANNEL3                                       (0x23)
#define DEVICE_CHANNEL4                                       (0x24)
#define DEVICE_CHANNEL5                                       (0x25)
#define DEVICE_CHANNEL6                                       (0x26)
#define DEVICE_CHANNEL7                                       (0x27)
#define BAD_CHANNEL                                           (0x00)

/* Constants for unpacking/packing */

#define  COMPREHENSION_REQUIRED_MASK                          (0x80)

#define  MAX_LENGTH_BYTE1                                     (0x7F)
#define  MIN_LENGTH_BYTE2                                     (0x80)
#define  TWO_BYTES_LENGTH_IND                                 (0x81)

// <Description of constant>

/****************************************************************************
                     UICC APDU constants
*****************************************************************************/

#define STE_UICC_MAX_APDU_SIZE             (261)





/****************************************************************************
                     Proactive command constants
*****************************************************************************/

/* SIMPLE-TLV tags */

#define  DUMMY_TAG                     (0x00)
#define  COMMAND_DETAILS_TAG           (0x01)
#define  DEVICE_IDENTITIES_TAG         (0x02)
#define  RESULT_TAG                    (0x03)
#define  DURATION_TAG                  (0x04)
#define  ALPHA_IDENTIFIER_TAG          (0x05)
#define  ADDRESS_TAG                   (0x06)
#define  CAPABILITY_CONFIG_PARAMS_TAG  (0x07)
#define  CALLED_PARTY_SUBADDRESS_TAG   (0x08)
#define  SS_STRING_TAG                 (0x09)
#define  USSD_STRING_TAG               (0x0A)
#define  SMS_TPDU_TAG                  (0x0B)
#define  CELL_BROADCAST_PAGE_TAG       (0x0C)
#define  TEXT_STRING_TAG               (0x0D)
#define  TONE_TAG                      (0x0E)
#define  ITEM_TAG                      (0x0F)
#define  ITEM_IDENTIFIER_TAG           (0x10)
#define  RESPONSE_LENGTH_TAG           (0x11)
#define  FILE_LIST_TAG                 (0x12)
#define  LOCATION_INFORMATION_TAG      (0x13)
#define  IMEI_TAG                      (0x14)
#define  HELP_REQUEST_TAG              (0x15)
#define  NMR_TAG                       (0x16)
#define  DEFAULT_TEXT_TAG              (0x17)
#define  ITEM_NEXT_ACTION_IND_TAG      (0x18)
#define  EVENT_LIST_TAG                (0x19)
#define  CAUSE_TAG                     (0x1A)
#define  LOCATION_STATUS_TAG           (0x1B)
#define  TRANSACTION_IDENTIFIER_TAG    (0x1C)
#define  BCCH_CHANNEL_LIST_TAG         (0x1D)
#define  ICON_IDENTIFIER_TAG           (0x1E)
#define  ITEM_ICON_ID_LIST_TAG         (0x1F)
#define  TIMER_IDENTIFIER_TAG          (0x24)
#define  TIMER_VALUE_TAG               (0x25)
#define  DATE_TIME_ZONE_TAG            (0x26)
#define  CALL_CONTROL_ACTION_TAG       (0x27)
#define  AT_COMMAND_TAG                (0x28)
#define  AT_RESPONSE_TAG               (0x29)
#define  BC_REPEAT_INDICATOR_TAG       (0x2A)
#define  IMMEDIATE_RESPONSE_TAG        (0x2B)
#define  DTMF_STRING_TAG               (0x2C)
#define  LANGUAGE_TAG                  (0x2D)
#define  TIMING_ADVANCE_TAG            (0x2E)
#define  AID_TAG                       (0x2F)
#define  BROWSER_IDENTITY_TAG          (0x30)
#define  URL_TAG                       (0x31)
#define  BEARER_TAG                    (0x32)
#define  PROVISIONING_FILE_REF_TAG     (0x33)
#define  BROWSER_TERMINATION_CAUSE_TAG (0x34)
#define  BEARER_DESCRIPTION_TAG        (0x35)
#define  CHANNEL_DATA_TAG              (0x36)
#define  CHANNEL_DATA_LENGTH_TAG       (0x37)
#define  CHANNEL_STATUS_TAG            (0x38)
#define  BUFFER_SIZE_TAG               (0x39)
#define  USIM_ME_INTERFACE_TRANS_TAG   (0x3C)
#define  OTHER_ADDRESS_TAG             (0x3E)
#define  ACCESS_TECHNOLOGY_TAG         (0x3F)
#define  NETWORK_ACCESS_NAME_TAG       (0x47)
#define  CDMA_SMS_TPDU_TAG             (0x48)
#define  TEXT_ATTRIBUTE_TAG            (0x50)
#define  ITEM_TEXT_ATTRIBUTE_LIST_TAG  (0x51)
#define  PDP_CONTEXT_ACTIVATION_TAG    (0x52)
#define  IMEISV_TAG                    (0x62)
#define  BATTERY_TAG                   (0x63)
#define  NETWORK_SEARCH_MODE_TAG       (0x65)
#define  FRAME_IDENTIFIER_TAG          (0x68)

#define  UTRAN_MEASUREMENT_QUALIFIER_TAG  (0x69)

#define  TERMINAL_PROFILE_DOWNLOAD_TAG  (0x6C)  // TODO What is the correct value?


//Time Unit definition for TAG Duration
#define DURATION_TIME_UNIT_MINUTE               (0x00)
#define DURATION_TIME_UNIT_SECOND               (0x01)
#define DURATION_TIME_UNIT_TENS_OF_SECOND       (0x02)

/* Command qualifiers - Bitmasks and Values*/

#define GENERIC_HELP_INFO_AVAILABLE                   (0x80)
#define GENERIC_HELP_INFO_AVAILABLE_MASK              GENERIC_HELP_INFO_AVAILABLE

#define DISPLAY_TEXT_HIGH_PRIORITY                    (0x01)
#define DISPLAY_TEXT_HIGH_PRIORITY_MASK               DISPLAY_TEXT_HIGH_PRIORITY
#define DISPLAY_TEXT_CLEAR_BY_USER                    (0x80)
#define DISPLAY_TEXT_CLEAR_BY_USER_MASK               DISPLAY_TEXT_CLEAR_BY_USER


#define USER_INPUT_ALPHABET_SET                       (0x01)
#define USER_INPUT_DIGITS                             (0x00)
#define USER_INPUT_TYPE_MASK                          USER_INPUT_ALPHABET_SET

#define USER_INPUT_UCS2_ALPHABET                      (0x02)
#define USER_INPUT_ALPHABET_MASK                      USER_INPUT_UCS2_ALPHABET


#define GET_INPUT_DO_NOT_ECHO_INPUT                   (0x04)
#define GET_INPUT_DO_NOT_ECHO_MASK                    GET_INPUT_DO_NOT_ECHO_INPUT

#define GET_INPUT_SM_PACK_USER_INPUT                  (0x08)
#define GET_INPUT_SM_PACK_USER_INPUT_MASK             GET_INPUT_SM_PACK_USER_INPUT


#define GET_INKEY_YES_NO_INPUT                        (0x04)
#define GET_INKEY_YES_NO_INPUT_MASK                   GET_INKEY_YES_NO_INPUT
#define GET_INKEY_IMMEDIATE_RESPONSE                  (0x08)
#define GET_INKEY_IMMEDIATE_RESPONSE_MASK             GET_INKEY_IMMEDIATE_RESPONSE

#define TIMER_MANAGEMENT_TIMER_START                  (0x00)
#define TIMER_MANAGEMENT_TIMER_START_MASK             TIMER_MANAGEMENT_TIMER_START
#define TIMER_MANAGEMENT_TIMER_DEACTIVATE             (0x01)
#define TIMER_MANAGEMENT_TIMER_DEACTIVATE_MASK        TIMER_MANAGEMENT_TIMER_DEACTIVATE
#define TIMER_MANAGEMENT_TIMER_CURRENT_VALUE          (0x02)
#define TIMER_MANAGEMENT_TIMER_CURRENT_VALUE_MASK     TIMER_MANAGEMENT_TIMER_CURRENT_VALUE
#define TIMER_MANAGEMENT_TIMER_NONE                   (0x03)
#define TIMER_MANAGEMENT_TIMER_NONE_MASK              TIMER_MANAGEMENT_TIMER_NONE

#define SETUP_CALL_ONLY_IF_NOT_BUSY                   (0x00)
#define SETUP_CALL_WITH_REDIAL_ONLY_IF_NOT_BUSY       (0x01)
#define SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD        (0x02)
#define SETUP_CALL_WITH_REDIAL_PUT_ALL_OTHER_ON_HOLD  (0x03)
#define SETUP_CALL_DISCONNECT_ALL_OTHER_CALLS         (0x04)
#define SETUP_CALL_WITH_REDIAL_DISCONNECT_ALL_OTHER   (0x05)

#define SETUP_MENU_PREFERRED_SELECTION                (0x01)
#define SETUP_MENU_PREFERRED_SELECTION_MASK           SETUP_MENU_PREFERRED_SELECTION
#define SETUP_MENU_HELP_INFO_AVAILABLE                (0x80)
#define SETUP_MENU_HELP_INFO_AVAILABLE_MASK           SETUP_MENU_HELP_INFO_AVAILABLE

#define SEL_ITEM_PRESENTATION_SPECIFIED_MASK          (0x01)
#define SEL_ITEM_PRESENTATION_NAV_OPT_MASK            (0x02)
#define SEL_ITEM_USING_SOFT_KEY_MASK                  (0x04)
#define SEL_ITEM_HELP_INFO_AVAILABLE_MASK             (0x80)

#define ICON_SELF_EXPLANATORY_MASK                    (0x01)
#define SEND_SMS_PACKING_REQ_BY_ME_MASK               (0x01)

#define REFRESH_USIM_INIT_AND_FULL_FILE_CHANGE        (0x00)
#define REFRESH_FILE_CHANGE                           (0x01)
#define REFRESH_USIM_INIT_AND_FILE_CHANGE             (0x02)
#define REFRESH_USIM_INIT                             (0x03)
#define REFRESH_UICC_RESET                            (0x04)
#define REFRESH_USIM_APP_RESET                        (0x05)
#define REFRESH_3G_SESSION_RESET                      (0x06)

#define TIMER_MANAGEMENT_MODE_MASK                    (0x03)
#define TIMER_MANAGEMENT_MODE_START                   (0x00)
#define TIMER_MANAGEMENT_MODE_STOP                    (0x01)
#define TIMER_MANAGEMENT_MODE_GET                     (0x02)
#define TIMER_MANAGEMENT_TIMERS_NBR_OF                (0x08)
#define TIMER_MANAGEMENT_TIMERS_START_ID              (0x01)
#define TIMER_MANAGEMENT_TIMERS_END_ID                (TIMER_MANAGEMENT_TIMERS_NBR_OF)


#define PROVIDE_LOCATION_INFORMATION                  (0x00)    //NOTE: The numbering of STE_SAT_Provide_Local_Info_Type_t exactly matches the numbering of PROVIDE_LOCATION_INFORMATION, etc.,
#define PROVIDE_ME_IMEI                               (0x01)    // which is exactly the binary value of command qualifier. So if this is changed in one place, the other places need to be updated.
#define PROVIDE_NMR                                   (0x02)
#define PROVIDE_DATE_TIME_ZONE                        (0x03)
#define PROVIDE_LANGUAGE                              (0x04)
#define PROVIDE_TIMING_ADVANCE                        (0x05)
#define PROVIDE_ACCESS_TECHNOLOGY                     (0x06)
#define PROVIDE_ME_IMEISV                             (0x08)
#define PROVIDE_BATTERY_CHARGE_STATE                  (0x0A)

#define LANGUAGE_NOTIFICATION_QUALIFIER               (0x01)
#define LANGUAGE_NOTIFICATION_MASK                    LANGUAGE_NOTIFICATION_QUALIFIER

#define LINK_ESTABLISHMENT_QUALIFIER_MASK             (0x01)
#define AUTOMATIC_RECONNECTION_QUALIFIER_MASK         (0x02)

#define SEND_DATA_STORE_DATA_MASK                     (0x01)

// 'Event' definitions for Set Up Event List command

#define SET_UP_EVENT_LIST_MT_CALL                                     (0x00)
#define SET_UP_EVENT_LIST_CALL_CONNECTED                              (0x01)
#define SET_UP_EVENT_LIST_CALL_DISCONNECTED                           (0x02)
#define SET_UP_EVENT_LIST_LOCATION_STATUS                             (0x03)
#define SET_UP_EVENT_LIST_USER_ACTIVITY                               (0x04)
#define SET_UP_EVENT_LIST_IDLE_SCREEN_AVAILABLE                       (0x05)
#define SET_UP_EVENT_LIST_CARD_READER_STATUS                          (0x06)
#define SET_UP_EVENT_LIST_LANGUAGE_SELECTION                          (0x07)
#define SET_UP_EVENT_LIST_BROWSER_TERMINATED                          (0x08)
#define SET_UP_EVENT_LIST_DATA_AVAILABLE                              (0x09)
#define SET_UP_EVENT_LIST_CHANNEL_STATUS                              (0x0A)
#define SET_UP_EVENT_LIST_ACCESS_TECHNOLOGY_CHANGE                    (0x0B)
#define SET_UP_EVENT_LIST_DISPLAY_PARAMETERS_CHANGED                  (0x0C)
#define SET_UP_EVENT_LIST_LOCAL_CONNECTION                            (0x0D)
#define SET_UP_EVENT_LIST_NW_SEARCH_MODE_CHANGE                       (0x0E)
#define SET_UP_EVENT_LIST_BROWSING_STATUS                             (0x0F)
#define SET_UP_EVENT_LIST_FRAMES_INFO_CHANGE                          (0x10)
#define SET_UP_EVENT_LIST_HCI_CONNECTIVITY_EVENT                      (0x13)

//NS related definitions
#define INITIALISE_SAT_CELL_ID                  {0xFF, 0xFF, 0xFF, 0xFF, FALSE}
#define EXTENDED_LOCATION_INFO_SIZE             (9)     /* TS 31.111 section 8.19 */
#define LOCATION_INFO_SIZE                      (7)     /* TS 31.111 section 8.19 */


//Command details information
typedef struct {
    uint8_t                   CommandNumber;      //command number in command details TAG
    uint8_t                   CommandQualifier;   //command qualifier in command details TAG
    uint8_t                   CommandType;        //command type id
    ste_command_result_t      CommandResult;      //command Result for the current command
} ste_command_info_t;

//the union structure for different PC
typedef union {
    ste_apdu_pc_display_text_t        *pc_dt_p;       //display text
    ste_apdu_pc_poll_interval_t       *pc_pi_p;       //poll interval
    ste_apdu_pc_provide_local_info_t  *pc_pli_p;      //provide local information
    ste_apdu_pc_set_up_event_list_t   *pc_sel_p;      //set up event list
    ste_apdu_pc_set_up_menu_t         *pc_sum_p;      //set up menu
    ste_apdu_pc_get_inkey_t           *pc_gik_p;      //get inkey
    ste_apdu_pc_get_input_t           *pc_gi_p;       //get input
    ste_apdu_pc_more_time_t           *pc_mt_p;       //more time
    ste_apdu_pc_polling_off_t         *pc_po_p;       //polling off
    ste_apdu_pc_timer_management_t    *pc_tm_p;       //timer management
    ste_apdu_pc_send_short_message_t  *pc_ssm_p;      //send short message
    ste_apdu_pc_send_dtmf_t           *pc_dtmf_p;     //send DTMF
    ste_apdu_pc_send_ss_t             *pc_ss_p;       //send SS
    ste_apdu_pc_send_ussd_t           *pc_ussd_p;     //send USSD
    ste_apdu_pc_setup_call_t          *pc_sc_p;       //setup call
    ste_apdu_pc_refresh_t             *pc_r_p;        //refresh
    ste_apdu_pc_language_notification_t *pc_ln_p;     //language notification
} ste_apdu_data_t;

//general structure for parsed APDUs.
struct ste_parsed_apdu_s {
    uint8_t                 apdu_kind;  //the kind, Ex. PC, EC, TR
    uint8_t                 type_id;    //the command type id, indicate which command type it is in the union
    ste_apdu_data_t         apdu_data;  //the real data structure
    ste_command_info_t      command_info;       //command details TAG info
};


//internal functions declaration
#if 0
static boolean          Validate_APDU_Kind(uint8_t apdu_kind);
static boolean          Validate_APDU_Type(uint8_t apdu_type);
#endif

static boolean  ValidateDataFlag(uint8_t flags);

static boolean  ParseAndValidateBER_TLV_Length(const uint8_t **CmdData_pp,
                                               uint8_t        *Length_p);

static boolean  ParseAndValidateSimpleTLV_Length(const uint8_t *CmdData_p,
                                                 uint8_t       *TotalLength_p,
                                                 uint8_t       *dataLength);

static void     ParseDeviceIdentities(const uint8_t * CmdData_p,
                                      uint8_t       * SourceID_p,
                                      uint8_t       * DestinationID_p);

static void     ParseTextString(const uint8_t * CmdData_p,
                                uint8_t         dataLength,
                                uint8_t       * CodingScheme_p,
                                uint8_t       * TextString_p);

static void     ParseIconIdentifier(const uint8_t * CmdData_p,
                                    uint8_t       * IconQualifier_p,
                                    uint8_t       * IconIdentifier_p);

static void     ParseTimerIdentifier(const uint8_t * CmdData_p,
                     ste_apdu_timer_identifier_code_value_t *timer_code_value_p);

static void     ParseResponseLength(const uint8_t              *CmdData_p,
                                    ste_apdu_response_length_t *ResponseLength_p);

static void     ParseDuration(const uint8_t * CmdData_p,
                              uint8_t       * TimeUnit_p,
                              uint8_t       * TimeInterval_p);

static boolean  ParseEventList(const uint8_t * CmdData_p,
                               uint8_t         Length,
                               unsigned int  * EventList_p);

static void     ParseApplicationID(const uint8_t * CmdData_p, uint8_t dataLength,
                                   ste_apdu_application_id_t * aid_p);

static void     ParseFileList(const uint8_t * CmdData_p, uint8_t dataLength,
                              ste_apdu_file_list_t * file_list_p);

static ste_sat_apdu_error_t
Parse_PC_DisplayText(const uint8_t               * TLV_data_p,
                     uint8_t                       dataLength,
                     ste_apdu_pc_display_text_t ** parsed_pc_dt_pp,
                     uint8_t                       CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_ProvideLocalInfo(const uint8_t                     * TLV_data_p,
                          uint8_t                             dataLength,
                          ste_apdu_pc_provide_local_info_t ** parsed_pc_pli_pp,
                          uint8_t                             CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_SetupEventList(const uint8_t                    * TLV_data_p,
                        uint8_t                            dataLength,
                        ste_apdu_pc_set_up_event_list_t ** parsed_pc_sel_pp);

static ste_sat_apdu_error_t
Parse_PC_SetupMenu(const uint8_t              * TLV_data_p,
                   uint8_t                      dataLength,
                   ste_apdu_pc_set_up_menu_t ** parsed_pc_sum_pp,
                   uint8_t                      CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_GetInkey(const uint8_t            * TLV_data_p,
                  uint8_t                    dataLength,
                  ste_apdu_pc_get_inkey_t ** parsed_pc_gik_pp,
                  uint8_t                    CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_GetInput(const uint8_t            * TLV_data_p,
                  uint8_t                    dataLength,
                  ste_apdu_pc_get_input_t ** parsed_pc_gi_pp,
                  uint8_t                    CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_MoreTime(const uint8_t            * TLV_data_p,
                  uint8_t                    dataLength,
                  ste_apdu_pc_more_time_t ** parsed_pc_mt_pp);

static ste_sat_apdu_error_t
Parse_PC_PollingOff(const uint8_t              * TLV_data_p,
                    uint8_t                      dataLength,
                    ste_apdu_pc_polling_off_t ** parsed_pc_po_pp);

static ste_sat_apdu_error_t
Parse_PC_PollInterval(const uint8_t                * TLV_data_p,
                      uint8_t                        dataLength,
                      ste_apdu_pc_poll_interval_t ** parsed_pc_pi_pp);

static ste_sat_apdu_error_t
Parse_PC_TimerManagement(const uint8_t *                   TLV_data_p,
                         uint8_t                           dataLength,
                         ste_apdu_pc_timer_management_t ** parsed_pc_tm_pp,
                         uint8_t                           CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_SendShortMessage(const uint8_t * TLV_data_p,
                          uint8_t dataLength,
                          ste_apdu_pc_send_short_message_t ** parsed_pc_ssm_pp,
                          uint8_t CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_SendDTMF(const uint8_t * TLV_data_p,
                  uint8_t dataLength,
                  ste_apdu_pc_send_dtmf_t ** parsed_pc_dtmf_pp);

static ste_sat_apdu_error_t
Parse_PC_SendSS(const uint8_t * TLV_data_p,
                uint8_t dataLength,
                ste_apdu_pc_send_ss_t ** parsed_pc_ss_pp);

static ste_sat_apdu_error_t
Parse_PC_SendUSSD(const uint8_t * TLV_data_p,
                  uint8_t dataLength,
                  ste_apdu_pc_send_ussd_t ** parsed_pc_ussd_pp);

static ste_sat_apdu_error_t
Parse_PC_SetupCall(const uint8_t * TLV_data_p,
                   uint8_t dataLength,
                   ste_apdu_pc_setup_call_t ** parsed_pc_sc_pp,
                   uint8_t CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_Refresh(const uint8_t * TLV_data_p,
                 uint8_t dataLength,
                 ste_apdu_pc_refresh_t ** parsed_pc_r_pp,
                 uint8_t CommandQualifier);

static ste_sat_apdu_error_t
Parse_PC_LanguageNotification(const uint8_t            * TLV_data_p,
                              uint8_t                    dataLength,
                              ste_apdu_pc_language_notification_t ** parsed_pc_ln_pp);

static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_DisplayText(uint8_t                      CommandQualifier,
                                          ste_apdu_pc_display_text_t * pc_dt_p);

static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_SetupMenu(uint8_t                     CommandQualifier,
                                        ste_apdu_pc_set_up_menu_t * pc_sum_p);

static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_GetInkey(uint8_t                   CommandQualifier,
                                       ste_apdu_pc_get_inkey_t * pc_gik_p);

static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_GetInput(uint8_t                   CommandQualifier,
                                       ste_apdu_pc_get_input_t * pc_gi_p);

static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_SetupCall(uint8_t                    CommandQualifier,
                                        ste_apdu_pc_setup_call_t * pc_sc_p);

static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_TimerManagement(uint8_t                         CommandQualifier,
                                              ste_apdu_pc_timer_management_t *pc_tm_p);

static void Create_CommandQualifier_PC_DisplayText(ste_parsed_apdu_t * apdu_info_p);

static void Create_CommandQualifier_PC_GetInkey(ste_parsed_apdu_t * apdu_info_p);

static void Create_CommandQualifier_PC_SetupMenu(ste_parsed_apdu_t * apdu_info_p);

static void Create_CommandQualifier_PC_GetInput(ste_parsed_apdu_t * apdu_info_p);

static void Create_CommandQualifier_PC_TimerManagement(ste_parsed_apdu_t * apdu_info_p);

static ste_sat_apdu_error_t
ParseProactiveCommand(const uint8_t     * TLV_data_p,
                      uint8_t             dataLength,
                      ste_parsed_apdu_t * parsed_apdu_info_p);

static ste_sat_apdu_error_t
CreateCommandDetailsTLV(ste_command_info_t * CommandInfo_p,
                        uint8_t            * CommandDetailsTLV_p);

static ste_sat_apdu_error_t
CreateDeviceIdentitiesTLV(uint8_t SourceDevice,
                          uint8_t DestDevice,
                          boolean CompReq,
                          uint8_t * Dest_p);

static ste_sat_apdu_error_t
CreateTimerIdentifierTLV(ste_apdu_timer_identifier_code_value_t timer_code,
                         boolean                                CompReq,
                         uint8_t                              * TLV_buffer_p);

static ste_sat_apdu_error_t CreateTimerValueTLV(uint32_t   TimerValue,
                                                boolean CompReq,
                                                uint8_t * TLV_buffer_p);

static ste_sat_apdu_error_t CreateAlphaIdentifierTLV(uint8_t * source_string_p,
                                                     uint8_t   string_length,
                                                     const boolean CompReq,
                                                     uint8_t * TLV_buffer_p,
                                                     uint8_t * TotalLen_p);

static ste_sat_apdu_error_t CreateItemListTLV(uint8_t * source_string_p,
                                              const boolean CompReq,
                                              uint8_t * TLV_buffer_p,
                                              uint8_t * TotalLen_p);

static ste_sat_apdu_error_t CreateNextActionTLV(uint8_t * source_string_p,
                                                uint8_t   string_length,
                                                const boolean CompReq,
                                                uint8_t * TLV_buffer_p,
                                                uint8_t * TotalLen_p);

static ste_sat_apdu_error_t CreateIconIdentifierTLV(const uint8_t IconIdentifier,
                                                    const uint8_t IconQualifier,
                                                    const boolean CompReq,
                                                    uint8_t     * Dest_p);

static ste_sat_apdu_error_t CreateImmediateResponseTLV(const boolean CompReq,
                                                       uint8_t     * Dest_p);

static ste_sat_apdu_error_t CreateBatteryTLV(uint8_t  State,
                                             uint8_t  * Dest_p);

static ste_sat_apdu_error_t CreateDurationTLV(const size_t  Duration,
                                              const boolean CompReq,
                                              uint8_t     * Dest_p);

static ste_sat_apdu_error_t CreateTimerManagementTLV(const uint8_t hours,
                                                     const uint8_t minutes,
                                                     const uint8_t seconds,
                                                     const boolean CompReq,
                                                     uint8_t     * Dest_p);

static ste_sat_apdu_error_t CreateFrameIdentifierTLV(uint8_t       FrameIdentifier,
                                                     const boolean CompReq,
                                                     uint8_t     * Dest_p);

static ste_sat_apdu_error_t CreateTextStringTLV(uint8_t * source_string_p,
                                                uint8_t   string_length,
                                                uint8_t   CodingScheme,
                                                const boolean CompReq,
                                                uint8_t * TLV_buffer_p,
                                                uint8_t * TotalLen_p);

static ste_sat_apdu_error_t CreateTextAttributeTLV(uint8_t * source_string_p,
                                                   uint8_t   string_length,
                                                   const boolean CompReq,
                                                   uint8_t * TLV_buffer_p,
                                                   uint8_t * TotalLen_p);

static ste_sat_apdu_error_t CreateIMEI_TLV(const IMEI_t * IMEI_p,
                                           uint8_t      * IMEI_Result_p,
                                           uint8_t      * IMEI_Length_p);

static ste_sat_apdu_error_t CreateIMEISV_TLV(const IMEISV_t * IMEISV_p,
                                             uint8_t        * IMEISV_Result_p,
                                             uint8_t        * IMEISV_Length_p);

static ste_sat_apdu_error_t CreateDateTimeTimezone_TLV(const ste_apdu_date_time_t * datetime,
                                                       uint8_t                    * DateTime_Result_p,
                                                       uint8_t                    * DateTime_Length_p);

static ste_sat_apdu_error_t CreateEventListTLV(unsigned int EventList,
                                               boolean CompReq,
                                               uint8_t * Dest_p,
                                               uint8_t * TotalLen_p);

static uint8_t CreateDialStringTLV(ste_apdu_dial_string_t * DialString_p,
                                   uint8_t                * Dest_p);

static uint8_t CreateCCP_TLV(ste_apdu_ccp_t   ccp,
                             uint8_t        * Dest_p);

static uint8_t CreateBCRepeatInd_TLV(uint8_t   bc_ind,
                                     uint8_t * Dest_p);

static uint8_t CreateSubAddressTLV(ste_apdu_sub_address_t    SubAddress,
                                   uint8_t                 * Dest_p);

static uint8_t CreateAddressTLV(ste_apdu_address_t * address_p,
                                uint8_t            * Dest_p);

static uint8_t
CreateCallControlResponse_TLV(ste_apdu_call_control_response_t * cc_response_p,
                              uint8_t * dest_p);

static ste_sat_apdu_error_t
CreateCallControlResultTLV(ste_cat_call_control_response_t * cat_rsp_p,
                           uint8_t * result_p,
                           uint8_t * result_len_p);

static ste_sat_apdu_error_t
CreateSendUSSD_ResultTLV(ste_cat_call_control_response_t * cat_rsp_p,
                         uint8_t * result_p,
                         uint8_t * result_len_p);

static uint8_t          BuildLengthTLV(const uint8_t Length,
                                       uint8_t * LengthTLV_p);

static ste_sat_apdu_error_t
Create_PC_APDU_DisplayText(ste_parsed_apdu_t * source_apdu_info_p,
                           uint8_t           * V_buffer_p,
                           uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_ProvideLocalInfo(ste_parsed_apdu_t * source_apdu_info_p,
                                uint8_t           * V_buffer_p,
                                uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_SetupEventList(ste_parsed_apdu_t * source_apdu_info_p,
                              uint8_t           * V_buffer_p,
                              uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_SetupMenu(ste_parsed_apdu_t * source_apdu_info_p,
                         uint8_t           * V_buffer_p,
                         uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_GetInkey(ste_parsed_apdu_t * source_apdu_info_p,
                        uint8_t           * V_buffer_p,
                        uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_GetInput(ste_parsed_apdu_t * source_apdu_info_p,
                        uint8_t           * V_buffer_p,
                        uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_MoreTime(ste_parsed_apdu_t * source_apdu_info_p,
                        uint8_t           * V_buffer_p,
                        uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_PollingOff(ste_parsed_apdu_t * source_apdu_info_p,
                          uint8_t           * V_buffer_p,
                          uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_PollInterval(ste_parsed_apdu_t * source_apdu_info_p,
                            uint8_t           * V_buffer_p,
                            uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
Create_PC_APDU_TimerManagement(ste_parsed_apdu_t * source_apdu_info_p,
                               uint8_t           * V_buffer_p,
                               uint8_t           * V_Length_p);

static ste_sat_apdu_error_t
CreateProactiveCommandAPDU(ste_parsed_apdu_t * source_apdu_info_p,
                           uint8_t           * TLV_data_p,
                           size_t            * dataLength_p);

static void Delete_APDU_PC_DisplayText(ste_apdu_pc_display_text_t * pc_dt_p);

static void Delete_APDU_PC_SetupMenu(ste_apdu_pc_set_up_menu_t * pc_sum_p);

static void Delete_APDU_PC_GetInkey(ste_apdu_pc_get_inkey_t * pc_gik_p);

static void Delete_APDU_PC_GetInput(ste_apdu_pc_get_input_t * pc_gi_p);

static void Delete_APDU_PC_SendShortMessage(ste_apdu_pc_send_short_message_t * pc_ssm_p);

static void Delete_APDU_PC_SendDTMF(ste_apdu_pc_send_dtmf_t * pc_dtmf_p);

static void Delete_APDU_PC_SendSS(ste_apdu_pc_send_ss_t * pc_ss_p);

static void Delete_APDU_PC_SendUSSD(ste_apdu_pc_send_ussd_t * pc_ussd_p);

static void Delete_APDU_PC_SetupCall(ste_apdu_pc_setup_call_t * pc_sc_p);

static void Delete_APDU_PC_LanguageNotification(ste_apdu_pc_language_notification_t * pc_ln_p);

static uint8_t CreateResultTLV(const uint8_t GeneralResult,
                               const uint8_t AdditionalInfoSize,
                               const uint8_t * const AdditionalInfo_p,
                               const boolean CompReq,
                               uint8_t * RspData_p);

static ste_sat_apdu_error_t
CreateTerminalResponse(ste_command_info_t * CommandInfo_p,
                       uint8_t * ResponseData_p,
                       size_t * ResponseLength_p);

static boolean ConvertDialTextToBCD(ste_sim_text_t  * sim_text_p,
                                    uint8_t         * result_p,
                                    uint16_t          max_len,
                                    uint16_t        * actual_len_p);

static boolean ConvertSimTextToAPDUAddress(ste_sim_text_t  * sim_text_p,
                                           ste_apdu_address_t * apdu_addr_p);

static boolean ConvertClientAddressToAPDU(ste_cat_cc_dialled_address_t * client_addr_p,
                                          ste_apdu_address_t * apdu_addr_p);

static boolean FormatCallControlInfo(ste_cat_call_control_t     * cc_p,
                                     ste_apdu_ec_call_control_t * cc_info_p);

static boolean FormatCallControlRspInfo(ste_cat_call_control_response_t  * cat_rsp_p,
                                        ste_apdu_call_control_response_t * cc_apdu_p);

static boolean FormatSmsControlInfo(ste_cat_sms_control_t     * sc_p,
                                    ste_apdu_ec_sms_control_t * sc_info_p);

static void DeleteCallControlInfo(ste_apdu_ec_call_control_t * cc_info_p);

static void DeleteCallControlRspInfo(ste_apdu_call_control_response_t * cc_rsp_p);

static void DeleteSmsControlInfo(ste_apdu_ec_sms_control_t * sc_info_p);

//#############################################################################################################################
// internal functions definition
//#############################################################################################################################


/*************************************************************************
 * Function:      ConvertDialTextToBCD
 *
 * Description:   convert the sim text structure from client to BCD format string
 *
 * Input Params:  sim_text_p:       the sim text data from client
 *
 * Output Params: result_p:         the result string
 *
 * Return:        TRUE: conversion OK, FALSE: Failed
 *
 * Notes:
 *
 *************************************************************************/
static boolean ConvertDialTextToBCD(ste_sim_text_t  * sim_text_p,
                                    uint8_t         * result_p,
                                    uint16_t          max_len,
                                    uint16_t        * actual_len_p)
{
    int rv;

    if (sim_text_p == NULL || result_p == NULL || actual_len_p == NULL)
    {
        return FALSE;
    }
    switch (sim_text_p->text_coding)
    {
        case STE_SIM_ASCII8:
        {
            rv = sim_convert_ASCII8_to_BCD(sim_text_p, max_len, actual_len_p, result_p);
        }
        break;
        case STE_SIM_UCS2:
        {
            rv = sim_convert_UCS2_to_BCD(sim_text_p, max_len, actual_len_p, result_p);
        }
        break;
        case STE_SIM_BCD:
        {
            strncpy((char*)result_p, (char*)sim_text_p->text_p, max_len);
            *actual_len_p = max_len;
        }
        break;
        default:
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*************************************************************************
 * Function:      ConvertSimTextToAPDUAddress
 *
 * Description:   convert the sim client text format structure to APDU format address
 *
 * Input Params:  sim_text_p:          the sim text data from client
 *
 * Output Params: apdu_addr_p:         the result address structure
 *
 * Return:        TRUE: conversion OK, FALSE: Failed
 *
 * Notes:
 *
 *************************************************************************/
static boolean ConvertSimTextToAPDUAddress(ste_sim_text_t  * sim_text_p,
                                           ste_apdu_address_t * apdu_addr_p)
{
    if (apdu_addr_p == NULL || sim_text_p == NULL)
    {
        return FALSE;
    }

    //get the real length in bytes
    apdu_addr_p->str_length = sim_get_text_string_length(sim_text_p);
    if (apdu_addr_p->str_length > 0)
    {
        uint16_t actual_len = 0;

        apdu_addr_p->dial_text_p = malloc(apdu_addr_p->str_length + 1);
        if (!apdu_addr_p->dial_text_p)
        {
            STE_SAT_LOG_ERROR("ConvertSimTextToAPDUAddress: memory allocation failed.\n");
            return FALSE;
        }
        memset(apdu_addr_p->dial_text_p, 0, apdu_addr_p->str_length + 1);
        if (!ConvertDialTextToBCD(sim_text_p,
                                  apdu_addr_p->dial_text_p,
                                  apdu_addr_p->str_length,
                                  &actual_len))
        {
            STE_SAT_LOG_ERROR("ConvertSimTextToAPDUAddress: Conversion to BCD failed.\n");
            free(apdu_addr_p->dial_text_p);
            return FALSE;
        }
        //set the real length of the address
        apdu_addr_p->str_length = actual_len;
    }
    else
    {
        STE_SAT_LOG_ERROR("ConvertSimTextToAPDUAddress: No called address for setup call.\n");
        return FALSE;
    }

    return TRUE;
}

/*************************************************************************
 * Function:      ConvertSimStringToAPDUAddress
 *
 * Description:   convert the sim client string format structure to APDU format address
 *
 * Input Params:  sim_str_p:           the sim string data from client
 *
 * Output Params: apdu_addr_p:         the result address structure
 *
 * Return:        TRUE: conversion OK, FALSE: Failed
 *
 * Notes:
 *
 *************************************************************************/
static boolean ConvertSimStringToAPDUAddress(ste_sim_string_t  * sim_str_p,
                                             ste_apdu_address_t * apdu_addr_p)
{
    if (apdu_addr_p == NULL || sim_str_p == NULL)
    {
        return FALSE;
    }

    apdu_addr_p->str_length = sim_str_p->no_of_bytes;
    if (apdu_addr_p->str_length > 0)
    {
        apdu_addr_p->dial_text_p = malloc(apdu_addr_p->str_length + 1);
        if (!apdu_addr_p->dial_text_p)
        {
            STE_SAT_LOG_ERROR("ConvertSimStringToAPDUAddress: memory allocation failed.\n");
            return FALSE;
        }
        memset(apdu_addr_p->dial_text_p, 0, apdu_addr_p->str_length + 1);
        memcpy(apdu_addr_p->dial_text_p, sim_str_p->str_p, apdu_addr_p->str_length);
    }
    else
    {
        STE_SAT_LOG_ERROR("ConvertSimStringToAPDUAddress: No called address for setup call.\n");
        return FALSE;
    }

    return TRUE;
}

/*************************************************************************
 * Function:      ConvertSimStringToAlphaId
 *
 * Description:   convert the sim string text format structure to APDU format alpha id
 *
 * Input Params:  sim_str_p:         the sim string data from client
 *
 * Output Params: alpha_p:           the result alpha id structure
 *
 * Return:        void
 *
 * Notes:
 *
 *************************************************************************/
static void ConvertSimStringToAlphaId(ste_sim_string_t  * sim_str_p,
                                      ste_apdu_alpha_identifier_t * alpha_p)
{
    size_t    length = 0;
    uint8_t * index_p = NULL;

    if (alpha_p == NULL || sim_str_p == NULL)
    {
        return;
    }

    //the maximum length of the alpha id should be sim_str_p->no_of_bytes + 1, considering the prefix
    index_p = malloc(sim_str_p->no_of_bytes + 1 + 1);
    if (!index_p)
    {
        return;
    }
    memset(index_p, 0, sim_str_p->no_of_bytes + 1 + 1);
    alpha_p->alpha_id_p = index_p;

    /* Copy Alpha ID (should be coded as for EF ADN) so insert UCS2 indication if necessary */
    switch (sim_str_p->text_coding)
    {
        case STE_SIM_UCS2:
        {
            length = sim_str_p->no_of_bytes + 1;
            *index_p++ = UCS2_FORMAT;
        }
        break;
        case STE_SIM_UCS2_81Prefix:
        {
            length = sim_str_p->no_of_bytes + 1;
            *index_p++ = UCS2_81_PREFIX_FORMAT;
        }
        break;
        case STE_SIM_UCS2_82Prefix:
        {
            length = sim_str_p->no_of_bytes + 1;
            *index_p++ = UCS2_82_PREFIX_FORMAT;
        }
        break;
        default:
        {
            length = sim_str_p->no_of_bytes;
        }
        break;
    }

    memcpy(index_p, sim_str_p->str_p, sim_str_p->no_of_bytes);
    alpha_p->alpha_id_length = length;
}

/*************************************************************************
 * Function:      ConvertClientAddressToAPDU
 *
 * Description:   convert the sim client dialled address structure to APDU format
 *
 * Input Params:  client_addr_p:       the sim address structure from client
 *
 * Output Params: apdu_addr_p:         the result address structure
 *
 * Return:        TRUE: conversion OK, FALSE: Failed
 *
 * Notes:
 *
 *************************************************************************/
static boolean ConvertClientAddressToAPDU(ste_cat_cc_dialled_address_t * client_addr_p,
                                          ste_apdu_address_t * apdu_addr_p)
{
    int                   rv;
    uint8_t               ton_npi;
    ste_sim_text_t      * sim_text_p = NULL;

    if (apdu_addr_p == NULL || client_addr_p == NULL)
    {
        return FALSE;
    }

    //convert the ton and npi info
    rv = sim_encode_ton_npi(client_addr_p->ton, client_addr_p->npi, &ton_npi);
    apdu_addr_p->ton_npi = ton_npi;

    //now copy the call string info
    sim_text_p = client_addr_p->dialled_string_p;
    if (!ConvertSimTextToAPDUAddress(sim_text_p, apdu_addr_p))
    {
        STE_SAT_LOG_ERROR("ConvertClientAddressToAPDU: ConvertSimTextToAPDUAddress failed.\n");
        return FALSE;
    }

    return TRUE;
}

/*************************************************************************
 * Function:      FormatCallControlInfo
 *
 * Description:   convert the call control related parameters from client to internal defined info struture of APDU
 *
 * Input Params:  cc_p:       the parameters from client
 *
 * Output Params: cc_info_p:  the internal info structure
 *
 * Return:        TRUE: conversion OK, FALSE: Failed
 *
 * Notes:
 *
 *************************************************************************/
static boolean FormatCallControlInfo(ste_cat_call_control_t     * cc_p,
                                     ste_apdu_ec_call_control_t * cc_info_p)
{
    if (cc_p == NULL || cc_info_p == NULL)
    {
        return FALSE;
    }

    switch (cc_p->cc_type)
    {
        case STE_CAT_CC_CALL_SETUP:
        {
            ste_cat_cc_call_setup_t * call_p = cc_p->cc_data.call_setup_p;

            if (!call_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlInfo: No data for setup call.\n");
                return FALSE;
            }
            if (!ConvertClientAddressToAPDU(&(call_p->address), &(cc_info_p->dial_string.address)))
            {
                STE_SAT_LOG_ERROR("FormatCallControlInfo: ConvertClientAddressToAPDU failed.\n");
                return FALSE;
            }

            //set the call control type
            cc_info_p->dial_string.cc_type = STE_APDU_CC_CALL_SETUP;
            //now set the bit mask
            cc_info_p->bit_mask |= EC_CALL_CONTROL_DialString_present;

            //now ready to check CCP 1
            if (call_p->ccp_1.len > 0 && call_p->ccp_1.ccp_p != NULL)
            {
                cc_info_p->ccp_1.ccp_length = call_p->ccp_1.len;
                cc_info_p->ccp_1.ccp_p = malloc(cc_info_p->ccp_1.ccp_length + 1);
                if (!cc_info_p->ccp_1.ccp_p)
                {
                    STE_SAT_LOG_ERROR("FormatCallControlInfo: memory allocation failed.\n");
                    free(cc_info_p->dial_string.address.dial_text_p);
                    return FALSE;
                }
                memset(cc_info_p->ccp_1.ccp_p, 0, cc_info_p->ccp_1.ccp_length + 1);
                memcpy(cc_info_p->ccp_1.ccp_p, call_p->ccp_1.ccp_p, cc_info_p->ccp_1.ccp_length);

                cc_info_p->bit_mask |= EC_CALL_CONTROL_CCP1_present;
            }
            //now ready to check CCP 2
            if (call_p->ccp_2.len > 0 && call_p->ccp_2.ccp_p != NULL)
            {
                cc_info_p->ccp_2.ccp_length = call_p->ccp_2.len;
                cc_info_p->ccp_2.ccp_p = malloc(cc_info_p->ccp_2.ccp_length + 1);
                if (!cc_info_p->ccp_2.ccp_p)
                {
                    STE_SAT_LOG_ERROR("FormatCallControlInfo: memory allocation failed.\n");
                    free(cc_info_p->dial_string.address.dial_text_p);
                    free(cc_info_p->ccp_1.ccp_p);
                    return FALSE;
                }
                memset(cc_info_p->ccp_2.ccp_p, 0, cc_info_p->ccp_2.ccp_length + 1);
                memcpy(cc_info_p->ccp_2.ccp_p, call_p->ccp_2.ccp_p, cc_info_p->ccp_2.ccp_length);

                cc_info_p->bit_mask |= EC_CALL_CONTROL_CCP2_present;
            }
            //now ready to check sub address
            if (call_p->sub_address.len > 0 && call_p->sub_address.sub_address_p != NULL)
            {
                cc_info_p->sub_address.sub_address_len = call_p->sub_address.len;
                cc_info_p->sub_address.sub_address_p = malloc(cc_info_p->sub_address.sub_address_len + 1);
                if (!cc_info_p->sub_address.sub_address_p)
                {
                    STE_SAT_LOG_ERROR("FormatCallControlInfo: memory allocation failed.\n");
                    free(cc_info_p->dial_string.address.dial_text_p);
                    free(cc_info_p->ccp_1.ccp_p);
                    free(cc_info_p->ccp_2.ccp_p);
                    return FALSE;
                }
                memset(cc_info_p->sub_address.sub_address_p, 0, cc_info_p->sub_address.sub_address_len + 1);
                memcpy(cc_info_p->sub_address.sub_address_p, call_p->sub_address.sub_address_p, cc_info_p->sub_address.sub_address_len);

                cc_info_p->bit_mask |= EC_CALL_CONTROL_SubAddress_present;
            }
            //now ready to check BC repeat IND
            if (call_p->bc_repeat_ind != 0)
            {
                cc_info_p->bc_repeat_ind = call_p->bc_repeat_ind;
                cc_info_p->bit_mask |= EC_CALL_CONTROL_BCRepeatInd_present;
            }
        }
        break;
        case STE_CAT_CC_SS:
        {
            ste_cat_cc_ss_t * cc_ss_p = cc_p->cc_data.ss_p;

            if (!cc_ss_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlInfo: No data for call control SS.\n");
                return FALSE;
            }
            if (!ConvertClientAddressToAPDU(&(cc_ss_p->address), &(cc_info_p->dial_string.address)))
            {
                STE_SAT_LOG_ERROR("FormatCallControlInfo: ConvertClientAddressToAPDU failed.\n");
                return FALSE;
            }
            //set the call control type
            cc_info_p->dial_string.cc_type = STE_APDU_CC_SS;

            //now set the bit mask
            cc_info_p->bit_mask |= EC_CALL_CONTROL_DialString_present;
        }
        break;
        case STE_CAT_CC_USSD:
        {
            ste_cat_cc_ussd_t * cc_ussd_p = cc_p->cc_data.ussd_p;
            ste_sim_string_t  * sim_str_p = NULL;

            if (!cc_ussd_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlInfo: No data for call control USSD.\n");
                return FALSE;
            }
            //set the call dcs for ussd
            cc_info_p->dial_string.dcs = cc_ussd_p->ussd_data.dcs;
            sim_str_p = cc_ussd_p->ussd_data.dialled_string_p;

            if (!ConvertSimStringToAPDUAddress(sim_str_p, &(cc_info_p->dial_string.address)))
            {
                STE_SAT_LOG_ERROR("ConvertClientAddressToAPDU: ConvertSimStringToAPDUAddress failed.\n");
                return FALSE;
            }
            //set the call control type
            cc_info_p->dial_string.cc_type = STE_APDU_CC_USSD;

            //now set the bit mask
            cc_info_p->bit_mask |= EC_CALL_CONTROL_DialString_present;
        }
        break;
        case STE_CAT_CC_PDP:
        {
            ste_cat_cc_pdp_t * cc_pdp_p = cc_p->cc_data.pdp_p;
            ste_sim_string_t  * sim_str_p = NULL;

            if (!cc_pdp_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlInfo: No data for call control PDP context activation.\n");
                return FALSE;
            }
            sim_str_p = &(cc_pdp_p->pdp_context);

            if (!ConvertSimStringToAPDUAddress(sim_str_p, &(cc_info_p->dial_string.address)))
            {
                STE_SAT_LOG_ERROR("ConvertClientAddressToAPDU: ConvertSimStringToAPDUAddress failed.\n");
                return FALSE;
            }
            //set the call control type
            cc_info_p->dial_string.cc_type = STE_APDU_CC_PDP;

            //now set the bit mask
            cc_info_p->bit_mask |= EC_CALL_CONTROL_DialString_present;
        }
        break;
        default:
        {
            STE_SAT_LOG_ERROR("Invalid call control type: %d\n", cc_p->cc_type);
            return FALSE;
        }
    }
    return TRUE;
}

/*************************************************************************
 * Function:      FormatSmsControlInfo
 *
 * Description:   convert the sms control related parameters from client to internal defined info struture of APDU
 *
 * Input Params:  sc_p:       the parameters from client
 *
 * Output Params: sc_info_p:  the internal info structure
 *
 * Return:        TRUE: conversion OK, FALSE: Failed
 *
 * Notes:
 *
 *************************************************************************/
static boolean FormatSmsControlInfo(ste_cat_sms_control_t     * sc_p,
                                    ste_apdu_ec_sms_control_t * sc_info_p)
{
    if (sc_p == NULL || sc_info_p == NULL)
    {
        return FALSE;
    }

    if (!ConvertClientAddressToAPDU(&(sc_p->smsc), &(sc_info_p->smsc)))
    {
        STE_SAT_LOG_ERROR("FormatSmsControlInfo: ConvertClientAddressToAPDU failed.\n");
        return FALSE;
    }
    //now set the bit mask
    sc_info_p->bit_mask |= EC_SMS_CONTROL_SMSC_Address_present;

    if (!ConvertClientAddressToAPDU(&(sc_p->dest), &(sc_info_p->destination)))
    {
        STE_SAT_LOG_ERROR("FormatSmsControlInfo: ConvertClientAddressToAPDU failed.\n");
        return FALSE;
    }
    //now set the bit mask
    sc_info_p->bit_mask |= EC_SMS_CONTROL_DestAddress_present;

    return TRUE;
}

/*************************************************************************
 * Function:      FormatCallControlRspInfo
 *
 * Description:   convert the call control response structure from client
 *                to internal defined info struture of APDU
 *
 * Input Params:  cat_rsp_p:  the cc result structure from client
 *
 * Output Params: cc_apdu_p:  the result internal apdu info structure
 *
 * Return:        TRUE: conversion OK, FALSE: Failed
 *
 * Notes:
 *
 *************************************************************************/
static boolean FormatCallControlRspInfo(ste_cat_call_control_response_t  * cat_rsp_p,
                                        ste_apdu_call_control_response_t * cc_apdu_p)
{
    ste_cat_call_control_t     * cc_p = NULL;

    if (cat_rsp_p == NULL || cc_apdu_p == NULL)
    {
        return FALSE;
    }

    memset(cc_apdu_p, 0, sizeof(ste_apdu_call_control_response_t));
    cc_p = &(cat_rsp_p->cc_info);

    switch (cc_p->cc_type)
    {
        case STE_CAT_CC_CALL_SETUP:
        {
            ste_cat_cc_call_setup_t * call_p = cc_p->cc_data.call_setup_p;

            if (!call_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlRspInfo: No data for setup call.\n");
                return FALSE;
            }
            if (!ConvertClientAddressToAPDU(&(call_p->address), &(cc_apdu_p->rsp_dial_string.address)))
            {
                STE_SAT_LOG_ERROR("FormatCallControlRspInfo: ConvertClientAddressToAPDU failed.\n");
                return FALSE;
            }

            //set the call control type
            cc_apdu_p->rsp_dial_string.cc_type = STE_APDU_CC_CALL_SETUP;
            //now set the bit mask
            cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_DialString_present;

            //now ready to check CCP 1
            if (call_p->ccp_1.len > 0 && call_p->ccp_1.ccp_p != NULL)
            {
                cc_apdu_p->rsp_ccp_1.ccp_length = call_p->ccp_1.len;
                cc_apdu_p->rsp_ccp_1.ccp_p = malloc(cc_apdu_p->rsp_ccp_1.ccp_length + 1);
                if (!cc_apdu_p->rsp_ccp_1.ccp_p)
                {
                    STE_SAT_LOG_ERROR("FormatCallControlRspInfo: memory allocation failed.\n");
                    free(cc_apdu_p->rsp_dial_string.address.dial_text_p);
                    return FALSE;
                }
                memset(cc_apdu_p->rsp_ccp_1.ccp_p, 0, cc_apdu_p->rsp_ccp_1.ccp_length + 1);
                memcpy(cc_apdu_p->rsp_ccp_1.ccp_p, call_p->ccp_1.ccp_p, cc_apdu_p->rsp_ccp_1.ccp_length);

                cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_CCP1_present;
            }
            //now ready to check CCP 2
            if (call_p->ccp_2.len > 0 && call_p->ccp_2.ccp_p != NULL)
            {
                cc_apdu_p->rsp_ccp_2.ccp_length = call_p->ccp_2.len;
                cc_apdu_p->rsp_ccp_2.ccp_p = malloc(cc_apdu_p->rsp_ccp_2.ccp_length + 1);
                if (!cc_apdu_p->rsp_ccp_2.ccp_p)
                {
                    STE_SAT_LOG_ERROR("FormatCallControlRspInfo: memory allocation failed.\n");
                    free(cc_apdu_p->rsp_dial_string.address.dial_text_p);
                    free(cc_apdu_p->rsp_ccp_1.ccp_p);
                    return FALSE;
                }
                memset(cc_apdu_p->rsp_ccp_2.ccp_p, 0, cc_apdu_p->rsp_ccp_2.ccp_length + 1);
                memcpy(cc_apdu_p->rsp_ccp_2.ccp_p, call_p->ccp_2.ccp_p, cc_apdu_p->rsp_ccp_2.ccp_length);

                cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_CCP2_present;
            }
            //now ready to check sub address
            if (call_p->sub_address.len > 0 && call_p->sub_address.sub_address_p != NULL)
            {
                cc_apdu_p->rsp_sub_address.sub_address_len = call_p->sub_address.len;
                cc_apdu_p->rsp_sub_address.sub_address_p = malloc(cc_apdu_p->rsp_sub_address.sub_address_len + 1);
                if (!cc_apdu_p->rsp_sub_address.sub_address_p)
                {
                    STE_SAT_LOG_ERROR("FormatCallControlRspInfo: memory allocation failed.\n");
                    free(cc_apdu_p->rsp_dial_string.address.dial_text_p);
                    free(cc_apdu_p->rsp_ccp_1.ccp_p);
                    free(cc_apdu_p->rsp_ccp_2.ccp_p);
                    return FALSE;
                }
                memset(cc_apdu_p->rsp_sub_address.sub_address_p, 0, cc_apdu_p->rsp_sub_address.sub_address_len + 1);
                memcpy(cc_apdu_p->rsp_sub_address.sub_address_p,
                       call_p->sub_address.sub_address_p,
                       cc_apdu_p->rsp_sub_address.sub_address_len);

                cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_SubAddress_present;
            }
            //now ready to check BC repeat IND
            if (call_p->bc_repeat_ind != 0)
            {
                cc_apdu_p->rsp_bc_repeat_ind = call_p->bc_repeat_ind;
                cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_BCRepeatInd_present;
            }
        }
        break;
        case STE_CAT_CC_SS:
        {
            ste_cat_cc_ss_t * cc_ss_p = cc_p->cc_data.ss_p;

            if (!cc_ss_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlRspInfo: No data for call control SS.\n");
                return FALSE;
            }
            if (!ConvertClientAddressToAPDU(&(cc_ss_p->address), &(cc_apdu_p->rsp_dial_string.address)))
            {
                STE_SAT_LOG_ERROR("FormatCallControlRspInfo: ConvertClientAddressToAPDU failed.\n");
                return FALSE;
            }
            //set the call control type
            cc_apdu_p->rsp_dial_string.cc_type = STE_APDU_CC_SS;

            //now set the bit mask
            cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_DialString_present;
        }
        break;
        case STE_CAT_CC_USSD:
        {
            ste_cat_cc_ussd_t * cc_ussd_p = cc_p->cc_data.ussd_p;
            ste_sim_string_t  * sim_str_p = NULL;

            if (!cc_ussd_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlRspInfo: No data for call control USSD.\n");
                return FALSE;
            }
            //set the call dcs for ussd
            cc_apdu_p->rsp_dial_string.dcs = cc_ussd_p->ussd_data.dcs;
            sim_str_p = cc_ussd_p->ussd_data.dialled_string_p;

            if (!ConvertSimStringToAPDUAddress(sim_str_p, &(cc_apdu_p->rsp_dial_string.address)))
            {
                STE_SAT_LOG_ERROR("ConvertClientAddressToAPDU: ConvertSimStringToAPDUAddress failed.\n");
                return FALSE;
            }
            //set the call control type
            cc_apdu_p->rsp_dial_string.cc_type = STE_APDU_CC_USSD;

            //now set the bit mask
            cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_DialString_present;
        }
        break;
        case STE_CAT_CC_PDP:
        {
            ste_cat_cc_pdp_t  * cc_pdp_p = cc_p->cc_data.pdp_p;
            ste_sim_string_t  * sim_str_p = NULL;

            if (!cc_pdp_p)
            {
                STE_SAT_LOG_ERROR("FormatCallControlRspInfo: No data for call control PDP context activation.\n");
                return FALSE;
            }
            sim_str_p = &(cc_pdp_p->pdp_context);

            if (!ConvertSimStringToAPDUAddress(sim_str_p, &(cc_apdu_p->rsp_dial_string.address)))
            {
                STE_SAT_LOG_ERROR("ConvertClientAddressToAPDU: ConvertSimStringToAPDUAddress failed.\n");
                return FALSE;
            }
            //set the call control type
            cc_apdu_p->rsp_dial_string.cc_type = STE_APDU_CC_PDP;

            //now set the bit mask
            cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_DialString_present;
        }
        break;
        default:
        {
            STE_SAT_LOG_ERROR("Invalid call control type: %d\n", cc_p->cc_type);
            return FALSE;
        }
    }
    //convert the cc result
    cc_apdu_p->cc_result = cat_rsp_p->cc_result;
    cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_Reulst_present;
    //convert the alpha id
    if (cat_rsp_p->user_indication.str_p && cat_rsp_p->user_indication.no_of_bytes > 0)
    {
        ConvertSimStringToAlphaId(&(cat_rsp_p->user_indication), &(cc_apdu_p->alpha_id));
        cc_apdu_p->bit_mask |= EC_CALL_CONTROL_RSP_AlphaIdentifier_present;
    }

    return TRUE;
}

static void DeleteCallControlInfo(ste_apdu_ec_call_control_t * cc_info_p)
{
    if (!cc_info_p)
    {
        return;
    }
    if ((cc_info_p->bit_mask & EC_CALL_CONTROL_DialString_present)
        && cc_info_p->dial_string.address.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_info_p->dial_string.address.dial_text_p);
    }
    if ((cc_info_p->bit_mask & EC_CALL_CONTROL_CCP1_present)
        && cc_info_p->ccp_1.ccp_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_info_p->ccp_1.ccp_p);
    }
    if ((cc_info_p->bit_mask & EC_CALL_CONTROL_CCP2_present)
        && cc_info_p->ccp_2.ccp_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_info_p->ccp_2.ccp_p);
    }
    if ((cc_info_p->bit_mask & EC_CALL_CONTROL_SubAddress_present)
        && cc_info_p->sub_address.sub_address_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_info_p->sub_address.sub_address_p);
    }
}

static void DeleteSmsControlInfo(ste_apdu_ec_sms_control_t * sc_info_p)
{
    if (!sc_info_p)
    {
        return;
    }
    if ((sc_info_p->bit_mask & EC_SMS_CONTROL_SMSC_Address_present)
        && sc_info_p->smsc.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(sc_info_p->smsc.dial_text_p);
    }
    if ((sc_info_p->bit_mask & EC_SMS_CONTROL_DestAddress_present)
        && sc_info_p->destination.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(sc_info_p->destination.dial_text_p);
    }
}

static void DeleteCallControlRspInfo(ste_apdu_call_control_response_t * cc_rsp_p)
{
    if (!cc_rsp_p)
    {
        return;
    }
    if ((cc_rsp_p->bit_mask & EC_CALL_CONTROL_RSP_DialString_present)
        && cc_rsp_p->rsp_dial_string.address.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_rsp_p->rsp_dial_string.address.dial_text_p);
    }
    if ((cc_rsp_p->bit_mask & EC_CALL_CONTROL_RSP_CCP1_present)
        && cc_rsp_p->rsp_ccp_1.ccp_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_rsp_p->rsp_ccp_1.ccp_p);
    }
    if ((cc_rsp_p->bit_mask & EC_CALL_CONTROL_RSP_CCP2_present)
        && cc_rsp_p->rsp_ccp_2.ccp_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_rsp_p->rsp_ccp_2.ccp_p);
    }
    if ((cc_rsp_p->bit_mask & EC_CALL_CONTROL_RSP_SubAddress_present)
        && cc_rsp_p->rsp_sub_address.sub_address_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_rsp_p->rsp_sub_address.sub_address_p);
    }
    if ((cc_rsp_p->bit_mask & EC_CALL_CONTROL_RSP_AlphaIdentifier_present)
        && cc_rsp_p->alpha_id.alpha_id_p != NULL)
    {
        STE_SAT_MEM_FREE(cc_rsp_p->alpha_id.alpha_id_p);
    }

}

/*************************************************************************
 * Function:      Validate_APDU_Kind
 *
 * Description:   validate a given APDU kind, to see if it is understandable
 *
 * Input Params:  apdu_kind: apdu kind to be verified
 *
 * Output Params: None
 *
 * Return:        TRUE: valid kind, FALSE: invalid kind
 *
 * Notes:
 *
 *************************************************************************/
#if 0
// UNUSED
static boolean Validate_APDU_Kind(uint8_t apdu_kind)
{
    if (apdu_kind >= STE_APDU_KIND__MAX || apdu_kind == STE_APDU_KIND_NONE) {
        return FALSE;
    }
    return TRUE;
}
#endif

/*************************************************************************
 * Function:      Validate_APDU_Type
 *
 * Description:   validate a given APDU type, to see if it is understandable
 *
 * Input Params:  apdu_type: apdu type to be verified
 *
 * Output Params: None
 *
 * Return:        TRUE: valid type, FALSE: invalid type
 *
 * Notes:
 *
 *************************************************************************/
#if 0
// UNUSED
static boolean Validate_APDU_Type(uint8_t apdu_type)
{
    if (apdu_type >= STE_APDU_FIRST_RESERVED_COMMAND_TYPE
        || apdu_type == STE_APDU_CMD_TYPE_NONE) {
        return FALSE;
    }
    return TRUE;
}
#endif

/*************************************************************************
 * Function:      ValidateDataFlag
 *
 * Description:   validate a given APDU data type, to see if it is understandable
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static boolean ValidateDataFlag(uint8_t flags)
{
    if (flags > STE_APDU_FIRST_RESERVED_COMMAND_TYPE) {
        return FALSE;
    }
    return TRUE;
}

/*************************************************************************
 * Function:      ParseLanguageIdentifier
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseLanguageIdentifier(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_language_t * language_p)
{
    language_p->length = dataLength;
    language_p->language_p = NULL;

    if (dataLength > 0)
    {
        language_p->language_p = STE_SAT_MEM_ALLOCATE(dataLength + 1);
        if (!language_p->language_p)
        {
            STE_SAT_LOG_ERROR("ParseLanguageIdentifier: memory allocation failed.\n");
        }
        else
        {
            STE_SAT_MEM_COPY(language_p->language_p, CmdData_p, dataLength);
        }
    }
}

/*************************************************************************
 * Function:      ParseAndValidateBER_TLV_Length
 *
 * Description:   Returns the size (coded on 1 or 2 bytes) of a TLV object.
 *                At entry *CmdData_pp points to the Tag.
 *
 * Input Params:  CmdData_pp    Address of pointer to Tag part of TLV
 *
 * Output Params: CmdData_pp    incremented to point to 1st byte after the length
 *                Length_p      size and data of length component of TLV.
 *
 * Return:        TRUE if length format is valid, FALSE if error detected
 *
 * Notes:         This function DOES check for correct length coding and
 *                returns FALSE if an error is found.
 *
 *************************************************************************/
static boolean
ParseAndValidateBER_TLV_Length(const uint8_t ** CmdData_pp, uint8_t * Length_p)
{
    boolean                 Valid = TRUE;

    /* Point to first (only ?) Length byte */
    (*CmdData_pp)++;

    if (**CmdData_pp == TWO_BYTES_LENGTH_IND)
    { /* Length is coded on two bytes */
        (*CmdData_pp)++;        // Point to actual length in following byte
        if (**CmdData_pp < MIN_LENGTH_BYTE2)
        {  /* Second byte value is less than expected for two-byte format - flag error */
            STE_SAT_LOG_ERROR("ParseAndValidateBER_TLV_Length: Error in length byte 2: %d \n", **CmdData_pp);
            Valid = FALSE;
        }  /* end Second byte value is less than expected for two-byte format - flag error */
    } /* end Length is coded on two bytes */
    else if (**CmdData_pp > MAX_LENGTH_BYTE1)
    { /* First byte value is greater than expected for one-byte format - flag error */
        STE_SAT_LOG_ERROR("ParseAndValidateBER_TLV_Length: Error in length byte 1: %d \n", **CmdData_pp);
        Valid = FALSE;
    }
    /* end First byte value is greater than expected for one-byte format - flag error */
    if (Valid)
    {                /* Extract length data and point to first value byte following length */
        *Length_p = **CmdData_pp;
        (*CmdData_pp)++;
    }
    /* end Extract length data and point to first value byte following length */
    return (Valid);

}                               /* end ParseAndValidateBER_TLV_Length */

/*************************************************************************
 * Function:      ParseAndValidateSimpleTLV_Length
 *
 * Description:   Returns the size (coded on 1 or 2 bytes) of a TLV object.
 *                At entry *CmdData_p points to Tag of the TLV being parsed
 *
 * Input Params:  CmdData_p       Address of pointer to start of the TLV
 *
 * Output Params: Length          The length of data/value part
 *                TotalLength_p   Total length of TLV object (i.e. including length
 *                                of Tag and length of Length i.e. + 2 or +3 bytes).
 *
 * Return:        TRUE if length format is valid, FALSE if error detected
 *
 * Notes:         This function DOES check for correct length coding and
 *                returns FALSE if an error is found.
 *
 *************************************************************************/
static boolean
ParseAndValidateSimpleTLV_Length(const uint8_t * CmdData_p,
                                 uint8_t * TotalLength_p,
                                 uint8_t * dataLength)
{
    uint8_t                 Length = 0;
    boolean                 LengthIsOnTwoBytes = FALSE; /* Initialise for the most common case */
    boolean                 Valid = TRUE;

    /* Point to first first (only?) Length byte */
    CmdData_p++;

    if (*CmdData_p == TWO_BYTES_LENGTH_IND)
    {   /* Length is coded on two bytes */
        CmdData_p++;            /* Point to actual length in following byte */
        if (*CmdData_p < MIN_LENGTH_BYTE2)
        {    /* Second byte value is less than expected for two-byte format - flag error */
            STE_SAT_LOG_ERROR("ParseAndValidateSimpleTLV_Length: Error in length byte 2: %d \n", *CmdData_p);
            Valid = FALSE;
        }    /* end Second byte value is less than expected for two-byte format - flag error */
        LengthIsOnTwoBytes = TRUE;
    } /* end Length is coded on two bytes */
    else if (*CmdData_p > MAX_LENGTH_BYTE1)
    {   /* First byte value is greater than expected for one-byte format - flag error */
        STE_SAT_LOG_ERROR("ParseAndValidateSimpleTLV_Length: Error in length byte 1: %d \n", *CmdData_p);
        Valid = FALSE;
    }
    /* end First byte value is greater than expected for one-byte format - flag error */
    if (Valid)
    {                /* Extract length data and point to first value byte following length */
        Length = *CmdData_p;
        *dataLength = Length;
        if (LengthIsOnTwoBytes)
        {
            *TotalLength_p = Length + 3;
        }
        else
        {
            *TotalLength_p = Length + 2;
        }
    }

    return (Valid);

}                               /* end ParseAndValidateSimpleTLV_Length */

/*************************************************************************
 * Function:      ParseDeviceIdentities
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseDeviceIdentities(const uint8_t * CmdData_p,
                      uint8_t * SourceID_p,
                      uint8_t * DestinationID_p)
{
    *SourceID_p = *CmdData_p;
    CmdData_p++;
    *DestinationID_p = *CmdData_p;
}

/*************************************************************************
 * Function:      ParseTimerIdentifier
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseTimerIdentifier(const uint8_t * CmdData_p,
                     ste_apdu_timer_identifier_code_value_t * timer_code_value_p)
{
    switch (*CmdData_p) {
    case 1:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE. \n");
            break;
        }
    case 2:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_TWO;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_TWO. \n");
            break;
        }
    case 3:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_THREE;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_THREE. \n");
            break;
        }
    case 4:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FOUR;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FOUR. \n");
            break;
        }
    case 5:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FIVE;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FIVE. \n");
            break;
        }
    case 6:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SIX;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SIX. \n");
            break;
        }
    case 7:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SEVEN;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SEVEN. \n");
            break;
        }
    case 8:
        {
            *timer_code_value_p = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_EIGHT;
            STE_SAT_LOG_INFO("Timer code value: STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_EIGHT. \n");
            break;
        }
    default:
        {
            break;
        }
    }
}

/*************************************************************************
 * Function:      ParseTextString
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseTextString(const uint8_t * CmdData_p, uint8_t dataLength,
                uint8_t * CodingScheme_p, uint8_t * TextString_p)
{
    *CodingScheme_p = *CmdData_p;
    CmdData_p++;
    STE_SAT_MEM_COPY(TextString_p, CmdData_p, dataLength - 1);
}

/*************************************************************************
 * Function:      ParseIconIdentifier
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseIconIdentifier(const uint8_t * CmdData_p,
                    uint8_t * IconQualifier_p, uint8_t * IconIdentifier_p)
{
    *IconQualifier_p = *CmdData_p;
    CmdData_p++;
    *IconIdentifier_p = *CmdData_p;
}

/*************************************************************************
 * Function:      ParseResponseLength
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseResponseLength(const uint8_t * CmdData_p,
                    ste_apdu_response_length_t * ResponseLength_p)
{
    ResponseLength_p->min_length = *CmdData_p;
    CmdData_p++;
    ResponseLength_p->max_length = *CmdData_p;
}

/*************************************************************************
 * Function:      ParseDuration
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseDuration(const uint8_t * CmdData_p,
              uint8_t * TimeUnit_p,
              uint8_t * TimeInterval_p)
{
    *TimeUnit_p = *CmdData_p;
    CmdData_p++;
    *TimeInterval_p = *CmdData_p;
}

/*************************************************************************
 * Function:      ParseEventList
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static boolean
ParseEventList(const uint8_t * CmdData_p,
               uint8_t Length,
               unsigned int *EventList_p)
{
    uint8_t                   i;

    for (i = 0; i < Length; i++)
    {
        //According to TS.102223, chapter 8.25, each event type shall not appear more than once in the list.
        //here we may need to add some error check to avoid such case
        if ((*CmdData_p) < STE_SAT_SET_UP_EVENT_LIST_END
            && !((STE_SAT_SET_UP_EVENT_LIST_BIT_SEED << (*CmdData_p)) & (*EventList_p)))
        {
            (*EventList_p) |= STE_SAT_SET_UP_EVENT_LIST_BIT_SEED << (*CmdData_p);
            CmdData_p++;
        }
        else
        {
            STE_SAT_LOG_ERROR("ParseEventList: Unexpected event, EVENT: 0x%x \n", *CmdData_p);
            return FALSE;
        }
    }
    STE_SAT_LOG_INFO("ParseEventList: Result Event List value: 0x%x \n", *EventList_p);

    return TRUE;
}

/*************************************************************************
 * Function:      ParseAddress
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseAddress(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_address_t * address_p)
{
    assert(CmdData_p);
    assert(address_p);

    // Default values in case of error....
    address_p->ton_npi     = 0;    // unknown TON & unknown NPI
    address_p->str_length  = 0;
    address_p->dial_text_p = NULL;


    if (dataLength > 0) {
        address_p->ton_npi = *CmdData_p;
        CmdData_p++;
        dataLength--;
    }

    if (dataLength > 0) {
        address_p->dial_text_p = malloc(dataLength);
        if (address_p->dial_text_p) {
            address_p->str_length = dataLength;
            STE_SAT_MEM_COPY(address_p->dial_text_p, CmdData_p, address_p->str_length);
        }
    }
}


/*************************************************************************
 * Function:      ParseSMS_TPDU
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseSMS_TPDU(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_sms_tpdu_t * tpdu_p)
{
    tpdu_p->data_len = dataLength;
    if (dataLength > 0) {
        tpdu_p->data_p = malloc(dataLength + 1);
        if (!tpdu_p->data_p) {
            return;
        }
        memset(tpdu_p->data_p, 0, dataLength + 1);

        STE_SAT_MEM_COPY(tpdu_p->data_p, CmdData_p, dataLength);
    }
    else {
        tpdu_p->data_p = NULL;
    }
}


/*************************************************************************
 * Function:      ParseCDMA_SMS_TPDU
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:  currently not support
 *
 *************************************************************************/
static void
ParseCDMA_SMS_TPDU(const uint8_t * UNUSED(CmdData_p), uint8_t UNUSED(dataLength), ste_apdu_sms_tpdu_t * tpdu_p)
{
    //TODO: Currently not support, pls refer to specification 3GPP2 C.S0015-0 [29] to implement this function
    if (tpdu_p->data_p)
    {
        return;
    }
}


/*************************************************************************
 * Function:      ParseDTMF
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseDTMF(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_dtmf_string_t * dtmf_p)
{
    dtmf_p->data_len = dataLength;
    dtmf_p->data_p = NULL;
    if (dataLength > 0) {
        dtmf_p->data_p = malloc(dataLength + 1);
        if (dtmf_p->data_p) {
            memset(dtmf_p->data_p, 0, dataLength + 1);
            STE_SAT_MEM_COPY(dtmf_p->data_p, CmdData_p, dataLength);
        }
    }
}

/*************************************************************************
 * Function:      ParseSS
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseSS(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_address_t * ss_p)
{
    //SS string is formatted exactly the same as Address, so reuse that function
    ParseAddress(CmdData_p, dataLength, ss_p);
}

/*************************************************************************
 * Function:      ParseUSSD
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseUSSD(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_ussd_string_t * ussd_p)
{
    ussd_p->dcs = *CmdData_p;
    CmdData_p++;
    ussd_p->str_len = dataLength - 1;
    ussd_p->str_p = NULL;
    if (ussd_p->str_len > 0) {
        ussd_p->str_p = malloc(dataLength);
        if (ussd_p->str_p) {
            memset(ussd_p->str_p, 0, dataLength);
            STE_SAT_MEM_COPY(ussd_p->str_p, CmdData_p, dataLength - 1);
        }
    }
}


/*************************************************************************
 * Function:      ParseAlphaIdentifier
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseAlphaIdentifier(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_alpha_identifier_t * alpha_p)
{
    alpha_p->alpha_id_length = dataLength;
    alpha_p->alpha_id_p = NULL;

    if (dataLength > 0)
    {
        alpha_p->alpha_id_p = STE_SAT_MEM_ALLOCATE(dataLength + 1);
        if (!alpha_p->alpha_id_p)
        {
            STE_SAT_LOG_ERROR("ParseAlphaIdentifier: memory allocation failed.\n");
        }
        else
        {
            STE_SAT_MEM_COPY(alpha_p->alpha_id_p, CmdData_p, dataLength);
        }
    }
}

/*************************************************************************
 * Function:      ParseCCP
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseCCP(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_ccp_t * ccp_p)
{
    assert(CmdData_p);
    assert(ccp_p);

    if (dataLength >= 2 )
    {
        /* See 'Bearer Capability IE' in 3GPP TS 24.008 ch. 10.5.4.5. Byte
           contains octet 2 from 'Bearer Capability IE'.*/
        ccp_p->ccp_length = *CmdData_p;
        CmdData_p++;
        dataLength--;

        /* See 'Bearer Capability IE' in 3GPP TS 24.008 ch. 10.5.4.5. First byte
           contains octet 3 from 'Bearer Capability IE'.*/
        ccp_p->ccp_p = STE_SAT_MEM_ALLOCATE(dataLength + 1);
        if (!ccp_p->ccp_p)
        {
            STE_SAT_LOG_ERROR("ParseCCP: memory allocation failed.\n");
            return;
        }
        STE_SAT_MEM_COPY(ccp_p->ccp_p, CmdData_p, dataLength);
    }
    else
    {
        ccp_p->ccp_p = NULL;
    }
}


/*************************************************************************
 * Function:      ParseSubAddress
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseSubAddress(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_sub_address_t * sub_address_p)
{
    sub_address_p->sub_address_len = dataLength;
    sub_address_p->sub_address_p = NULL;

    if (dataLength > 0)
    {
        sub_address_p->sub_address_p = STE_SAT_MEM_ALLOCATE(dataLength + 1);
        if (!sub_address_p->sub_address_p)
        {
            STE_SAT_LOG_ERROR("ParseSubAddress: memory allocation failed.\n");
        }
        else
        {
            STE_SAT_MEM_COPY(sub_address_p->sub_address_p, CmdData_p, dataLength);
        }
    }
}

/*************************************************************************
 * Function:      ParseTextAttribute
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseTextAttribute(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_text_attribute_t * text_attr_p)
{
    text_attr_p->text_attribute_length = dataLength;
    text_attr_p->text_attribute_p = NULL;

    if (dataLength > 0)
    {
        text_attr_p->text_attribute_p = STE_SAT_MEM_ALLOCATE(dataLength + 1);
        if (!text_attr_p->text_attribute_p)
        {
            STE_SAT_LOG_ERROR("ParseTextAttribute: memory allocation failed.\n");
        }
        else
        {
            STE_SAT_MEM_COPY(text_attr_p->text_attribute_p, CmdData_p, dataLength);
        }
    }
}

/*************************************************************************
 * Function:      ParseApplicationID
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseApplicationID(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_application_id_t * aid_p)
{
    if ( dataLength > STE_APDU_MAX_AID_LENGTH ) {
        STE_SAT_LOG_ERROR("ParseApplicationID: Unexpectedly long data length=%d\n",dataLength);
        return;
    }

    aid_p->len = dataLength;
    memset(aid_p->app, 0, dataLength);

    if (dataLength > 0)
    {
        memcpy(aid_p->app, CmdData_p, dataLength);
    }
}

/*************************************************************************
 * Function:      ParseFileList
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
ParseFileList(const uint8_t * CmdData_p, uint8_t dataLength, ste_apdu_file_list_t * file_list_p)
{
    size_t  nfiles;
    size_t  pos;
    file_list_p->nr_of_files = *CmdData_p++;
    dataLength--;

    file_list_p->files_p = malloc(file_list_p->nr_of_files * sizeof *file_list_p->files_p);
    if (file_list_p->files_p == NULL)
    {
        STE_SAT_LOG_ERROR("ParseFileList: memory allocation failed.\n");
        return;
    }
    for ( nfiles = 0 ; nfiles < file_list_p->nr_of_files ; nfiles++ ) {
        file_list_p->files_p[nfiles].file_path = NULL;
        file_list_p->files_p[nfiles].path_len  = 0;
    }

    // TS102.223 Para 8.18
    // an entry in the file description shall therefore always begin with '3FXX'.
    // There can be any number of Dedicated File entries between the Master File and
    // Elementary File. There shall be no delimiters between files, as this is implied
    // by the fact that the full path to any EF starts with '3FXX' and ends with an
    // Elementary type file.
    nfiles = 0;
    pos    = 0;
    do {
        size_t start = pos;
        if ( CmdData_p[start] == 0x3F ) {
            do {
                pos += 2;   // Skip the xx bytes
            } while ( pos < dataLength && CmdData_p[pos] != 0x3F );
            // A path exists between [start..pos)
            size_t path_len = pos - start;
            file_list_p->files_p[nfiles].file_path =
                    malloc( path_len * sizeof(*file_list_p->files_p[nfiles].file_path) );
            if ( file_list_p->files_p[nfiles].file_path == NULL ) {
                STE_SAT_LOG_ERROR("ParseFileList: memory allocation failed.\n");
                return;
            } else {
                memcpy(file_list_p->files_p[nfiles].file_path,
                       &CmdData_p[start],
                       path_len);
                file_list_p->files_p[nfiles].path_len = path_len;
                nfiles++;
            }
        } else {
            STE_SAT_LOG_ERROR("ParseFileList: Expected 0x3F at start=%zd, got %hhx\n", start, CmdData_p[start] );
            break;
        }
    } while ( pos < dataLength && nfiles < file_list_p->nr_of_files );

    if ( pos != dataLength ) {
        STE_SAT_LOG_ERROR("ParseFileList: consumed %zd bytes out of %hhd available\n", pos, dataLength );
    }
    if ( nfiles != file_list_p->nr_of_files ) {
        STE_SAT_LOG_ERROR("ParseFileList: found %zd files, expected %hhd\n", nfiles, file_list_p->nr_of_files );
    }
}

/*************************************************************************
 * Function:      Parse_PC_DisplayText
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_DisplayText(const uint8_t * TLV_data_p,
                     uint8_t dataLength,
                     ste_apdu_pc_display_text_t ** parsed_pc_dt_pp,
                     uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_DisplayText: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_DisplayText: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_DisplayText: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_DISPLAY != DestinationID) {
                    STE_SAT_LOG_ERROR("Parse_PC_DisplayText: Unexpected device ID: Source ID: 0x%x, Destination ID: 0x%x.\n", SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_dt_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_dt_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_dt_pp)->bit_mask |= PC_DISPLAY_TEXT_DeviceID_present;
            }
            break;
        case TEXT_STRING_TAG:
            {
                uint8_t                   CodingScheme;

                if ( Length ) {
                (*parsed_pc_dt_pp)->text_string.text_length = Length - 1;       //Length minus the length for coding scheme, that is 1 byte.
                if ((*parsed_pc_dt_pp)->text_string.text_length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_dt_pp)->text_string.text_string_p = STE_SAT_MEM_ALLOCATE(Length);
                    if (NULL == (*parsed_pc_dt_pp)->text_string.text_string_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_DisplayText: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                ParseTextString(TLV_data_p, Length, &CodingScheme, (*parsed_pc_dt_pp)->text_string.text_string_p);
                (*parsed_pc_dt_pp)->text_string.coding_scheme = CodingScheme;
                (*parsed_pc_dt_pp)->bit_mask |= PC_DISPLAY_TEXT_TextString_present;
                }
            }
            break;
        case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_dt_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_dt_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_dt_pp)->icon_id.icon_identifier == 0) {
                    STE_SAT_LOG_WARNING("Parse_PC_DisplayText: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_dt_pp)->bit_mask |= PC_DISPLAY_TEXT_IconIdentifier_present;
            }
            break;
        case IMMEDIATE_RESPONSE_TAG:
            {
                //directly set this value when there is such TLV.
                (*parsed_pc_dt_pp)->immediate_response = 0x01;
                (*parsed_pc_dt_pp)->bit_mask |= PC_DISPLAY_TEXT_ImmediateResponse_present;
            }
            break;
        case DURATION_TAG:
            {
                uint8_t                   TimeUnit;
                uint8_t                   TimeInterval;

                ParseDuration(TLV_data_p, &TimeUnit, &TimeInterval);
                (*parsed_pc_dt_pp)->bit_mask |= PC_DISPLAY_TEXT_Duration_present;
                // convert to an unified Time Unit and set it to the structure for outside user, time unit: TENS_OF_SECOND
                switch (TimeUnit) {
                case DURATION_TIME_UNIT_MINUTE:
                    {
                        (*parsed_pc_dt_pp)->duration = TimeInterval * 10 * 60;
                    }
                    break;
                case DURATION_TIME_UNIT_SECOND:
                    {
                        (*parsed_pc_dt_pp)->duration = TimeInterval * 10;
                    }
                    break;
                case DURATION_TIME_UNIT_TENS_OF_SECOND:
                    {
                        (*parsed_pc_dt_pp)->duration = TimeInterval;
                    }
                    break;
                default:
                    {
                        STE_SAT_LOG_WARNING("Parse_PC_DisplayText: Unknown Time Unit for Duration Tag.\n");
                    }
                }
            }
            break;
        case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_dt_pp)->text_attribute.text_attribute_length = Length;
                if (Length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_dt_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_dt_pp)->text_attribute.text_attribute_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_DisplayText: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_dt_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_dt_pp)->bit_mask |= PC_DISPLAY_TEXT_TextAttribute_present;
            }
            break;
        case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_dt_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_dt_pp)->bit_mask |= PC_DISPLAY_TEXT_FrameIdentifier_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_DisplayText: Unknown Tag value in Display Text command.\n");
            }
        }                       // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }                           //end of while (dataLength >= 2)

    //check the command qualifier and interpret it
    RetVal = Interpret_CommandQualifier_PC_DisplayText(CommandQualifier, *parsed_pc_dt_pp);

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_ProvideLocalInfo
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_ProvideLocalInfo(const uint8_t * TLV_data_p,
                          uint8_t dataLength,
                          ste_apdu_pc_provide_local_info_t ** parsed_pc_pli_pp,
                          uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        STE_SAT_LOG_INFO("Parse_PC_ProvideLocalInfo: Parse Tag: 0x%x \n", TagValue);
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_ProvideLocalInfo: Unexpected length for specific tag, Tag: %d \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }
        TLV_data_p += (TotalLength - Length);   // move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_ME != DestinationID) {
                    STE_SAT_LOG_ERROR("Parse_PC_ProvideLocalInfo: Unexpected device ID: Source ID: %d, Destination ID: %d.\n", SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_pli_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_pli_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_pli_pp)->bit_mask |= PC_PROVIDE_LOCAL_INFO_DeviceID_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_ProvideLocalInfo: Unknown Tag value in Provide Local Info command.\n");
            }
        }                       // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }                           //end of while (dataLength >= 2)

    //check the command qualifier to see which kind of info is needed
    //NOTE: The numbering of STE_SAT_Provide_Local_Info_Type_t exactly matches the numbering of
    //PROVIDE_LOCATION_INFORMATION, etc., which is exactly the binary value of command qualifier
    //so here just simply set the info_type value to the one of command qualifer
    (*parsed_pc_pli_pp)->info_type = (ste_apdu_provide_local_info_type_t) CommandQualifier;
    (*parsed_pc_pli_pp)->bit_mask |= PC_PROVIDE_LOCAL_INFO_Type_present;

    /* Check for NMR, check if UTRAN_MEASUREMENT_TAG are set? If so, then UTRAN otherwise GERAN */
    if (*(TLV_data_p) == UTRAN_MEASUREMENT_QUALIFIER_TAG) {
        (*parsed_pc_pli_pp)->nmr_rat_type = CN_NMR_RAT_TYPE_UTRAN;
        (*parsed_pc_pli_pp)->nmr_utran_type = *(TLV_data_p+2);
    } else {
        (*parsed_pc_pli_pp)->nmr_rat_type = CN_NMR_RAT_TYPE_GERAN;
    }

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_SetupEventList
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_SetupEventList(const uint8_t * TLV_data_p,
                        uint8_t dataLength,
                        ste_apdu_pc_set_up_event_list_t ** parsed_pc_sel_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        STE_SAT_LOG_INFO("Parse_PC_SetupEventList: Parse Tag: 0x%x \n", TagValue);

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_SetupEventList: Unexpected length for specific tag, Tag: %d \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }
        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_ME != DestinationID) {
                    STE_SAT_LOG_ERROR("Parse_PC_SetupEventList: Unexpected device ID: Source ID: %d, Destination ID: %d.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_sel_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_sel_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_sel_pp)->bit_mask |= PC_SET_UP_EVENT_LIST_DeviceID_present;
            }
            break;
        case EVENT_LIST_TAG:
            {
                // the event_list parameter should have been initialized when allocate memory for the pointer
                if (Length > 0) {
                    //there is event in the APDU
                    if (FALSE == ParseEventList(TLV_data_p, Length, &((*parsed_pc_sel_pp)->event_list)))
                    {
                        (*parsed_pc_sel_pp)->event_list = 0;    //reset the event list
                        STE_SAT_LOG_ERROR("Parse_PC_SetupEventList: Event List is wrong.\n");
                        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                    }
                } else {
                    (*parsed_pc_sel_pp)->event_list = 0;        //reset the previous event list, according to TS.102223, chapter 6.6.16
                    STE_SAT_LOG_INFO("Parse_PC_SetupEventList: Event List is empty.\n");
                }
                (*parsed_pc_sel_pp)->bit_mask |= PC_SET_UP_EVENT_LIST_Event_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_SetupEventList: Unknown Tag value in Set Up Event List command.\n");
            }
        }                       // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }                           //end of while (dataLength >= 2)

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_SetupMenu
 *
 * Description:   This method is used to parse the proactive command "Set
 *                Up Menu".
 *
 * Input Params:  TLV_data_p       The raw binary data associated with the
 *                                 proactive command as sent by the UICC.
 *                dataLength       The length of the Apdu.
 *
 * Output Params: parsed_pc_sum_pp The proactive command structure after it
 *                                 has been parsed.
 *
 * Return:        RetVal           Returns a valid error code in case of
 *                                 failure.
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_SetupMenu(const uint8_t * TLV_data_p,
                   uint8_t dataLength,
                   ste_apdu_pc_set_up_menu_t ** parsed_pc_sum_pp,
                   uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        STE_SAT_LOG_INFO("Parse_PC_SetupMenu: Parse Tag: 0x%x \n", TagValue);
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: Unexpected length for specific tag, Tag: %d \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }
        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_DISPLAY != DestinationID) {
                    STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: Unexpected device ID: Source ID: %d, Destination ID: %d.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_sum_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_sum_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_DeviceID_present;
            }
            break;
        case ALPHA_IDENTIFIER_TAG:
            {
                (*parsed_pc_sum_pp)->alpha_identifier.alpha_id_length = Length;
                if (Length > 0) {
                    //allocate memory for alpha identifier string
                    (*parsed_pc_sum_pp)->alpha_identifier.alpha_id_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_sum_pp)->alpha_identifier.alpha_id_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Alpha Identifier string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_sum_pp)->alpha_identifier.alpha_id_p, TLV_data_p, Length);
                STE_SAT_LOG_INFO("Result AlphaIdentifier value: %s \n", (*parsed_pc_sum_pp)->alpha_identifier.alpha_id_p);
                (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_AlphaIdentifier_present;
            }
            break;
        case ITEM_TAG:
            {
                ste_apdu_node_t        *Temp_p = NULL;
                char                   *Debug_p = NULL;

                if (Length > 0) {
                    //Check if this is the first node
                    if (0 == (*parsed_pc_sum_pp)->item_list.NumberOfNodes) {
                        //allocate memory for the first node in the item data list
                        (*parsed_pc_sum_pp)->item_list.Header_p = (ste_apdu_node_t *)STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_node_t));
                        if (NULL == (*parsed_pc_sum_pp)->item_list.Header_p) {
                            STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: memory allocation failed.\n");
                            return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                        }
                        //allocate memory for the data part of the first node
                        (*parsed_pc_sum_pp)->item_list.Header_p->Data_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                        if (NULL == (*parsed_pc_sum_pp)->item_list.Header_p->Data_p) {
                            STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: memory allocation failed.\n");
                            return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                        }
                        //copy the Alpha Identifier string directly to the structure
                        STE_SAT_MEM_COPY((*parsed_pc_sum_pp)->item_list.Header_p->Data_p, TLV_data_p, Length);
                        Debug_p = (char *) (*parsed_pc_sum_pp)->item_list.Header_p->Data_p;
                        Debug_p++;
                        STE_SAT_LOG_INFO("Result Item Data value length for the first item in the item list: %d \n", Length);
                        STE_SAT_LOG_INFO("Result Item Data value for the first item in the item list: %s \n", Debug_p);
                        (*parsed_pc_sum_pp)->item_list.Header_p->Next_p = NULL;
                    } else {
                        //allocate memory for successive nodes in the item data list
                        ste_apdu_node_t *new = (ste_apdu_node_t *)STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_node_t));
                        if (NULL == new ) {
                            STE_SAT_LOG_ERROR("Parse_PC_DisplayText: memory allocation failed.\n");
                            return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                        }
                        //allocate memory for the data part of the first node
                        new->Data_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                        new->Next_p = NULL;
                        if (NULL == new->Data_p) {
                            STE_SAT_MEM_FREE(new);
                            STE_SAT_LOG_ERROR("Parse_PC_DisplayText: memory allocation failed.\n");
                            return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                        }
                        //copy the data part of the item data string directly to the structure
                        STE_SAT_MEM_COPY(new->Data_p, TLV_data_p, Length);
                        Debug_p = (char *) new->Data_p;
                        Debug_p++;
                        STE_SAT_LOG_INFO("Result Item Data value length for the successive items in the item list: %d \n", Length);
                        STE_SAT_LOG_INFO("Result Item Data value for the successive items in the item list: %s \n", Debug_p);
                        // Find the end of the list, and append new node
                        Temp_p = (*parsed_pc_sum_pp)->item_list.Header_p;
                        while ( Temp_p->Next_p != NULL ) {
                            Temp_p = Temp_p->Next_p;
                        }
                        Temp_p->Next_p = new;
                    }
                    (*parsed_pc_sum_pp)->item_list.NumberOfNodes++;
                }
                STE_SAT_LOG_INFO("Result Number of items in the item data list: %d \n", (*parsed_pc_sum_pp)->item_list.NumberOfNodes);
                (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_ItemList_present;
            }
            break;
        case ITEM_NEXT_ACTION_IND_TAG:
            {
                (*parsed_pc_sum_pp)->next_action.NextActionLen = Length;

                if (Length > 0) {
                    //allocate memory for item next action indicator tag
                    (*parsed_pc_sum_pp)->next_action.NextActionIndicator_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_sum_pp)->next_action.NextActionIndicator_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Item Next Action Indicator list data directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_sum_pp)->next_action.NextActionIndicator_p, TLV_data_p, Length);
                (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_NextAction_present;
            }
            break;
        case ICON_IDENTIFIER_TAG:
            {
                uint8_t                   IconIndentifier = 0;
                uint8_t                   IconQualifier = 0;
                ParseIconIdentifier(TLV_data_p, &IconIndentifier, &IconQualifier);

                (*parsed_pc_sum_pp)->icon_id.icon_identifier = IconIndentifier;
                (*parsed_pc_sum_pp)->icon_id.icon_qualifier = IconQualifier;
                (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_IconIdentifier_present;
            }
            break;
        case ITEM_ICON_ID_LIST_TAG:
            {
                (*parsed_pc_sum_pp)->icon_id_list.DisplayWithAlphaId = *TLV_data_p;

                if (Length > 0) {
                    (*parsed_pc_sum_pp)->icon_id_list.IconIdList.Header_p = (ste_apdu_node_t *)STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_node_t));
                    //allocate memory for icon identifier list tag
                    (*parsed_pc_sum_pp)->icon_id_list.IconIdList.Header_p->Data_p = STE_SAT_MEM_ALLOCATE(Length-1);
                    if (NULL == (*parsed_pc_sum_pp)->icon_id_list.IconIdList.Header_p->Data_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                    //copy the Item ID list data directly to the structure
                    STE_SAT_MEM_COPY((*parsed_pc_sum_pp)->icon_id_list.IconIdList.Header_p->Data_p, TLV_data_p+1, Length-1);
                    STE_SAT_LOG_INFO("Result Icon Id List value: %s \n", (char *) (*parsed_pc_sum_pp)->icon_id_list.IconIdList.Header_p->Data_p);
                    (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_IconIdList_present;
                }
            }
            break;
        case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_sum_pp)->text_attribute.text_attribute_length = Length;
                if (Length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_sum_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_sum_pp)->text_attribute.text_attribute_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_SetupMenu: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_sum_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_TextAttribute_present;
            }
            break;
        case ITEM_TEXT_ATTRIBUTE_LIST_TAG:
            {
                if (Length > 0) {
                    (*parsed_pc_sum_pp)->text_attribute_list.Header_p = (ste_apdu_node_t *)STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_node_t));
                    //allocate memory for text attribute list
                    (*parsed_pc_sum_pp)->text_attribute_list.Header_p->Data_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_sum_pp)->text_attribute_list.Header_p->Data_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_SetUpMenu: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                    //copy the Text attribute string directly to the structure
                    STE_SAT_MEM_COPY((*parsed_pc_sum_pp)->text_attribute_list.Header_p->Data_p, TLV_data_p, Length);
                    (*parsed_pc_sum_pp)->bit_mask |= PC_SET_UP_MENU_TextAttributeList_present;
                }
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_SetupMenu: Unknown Tag value in Set Up Menu command.\n");
            }
            break;
        }                       // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }                           //end of while (dataLength >= 2)

    //check the command qualifier and interpret it
    RetVal = Interpret_CommandQualifier_PC_SetupMenu(CommandQualifier, *parsed_pc_sum_pp);

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_GetInkey
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_GetInkey(const uint8_t * TLV_data_p,
                  uint8_t dataLength,
                  ste_apdu_pc_get_inkey_t ** parsed_pc_gik_pp,
                  uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_GetInkey: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_GetInkey: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_GetInkey: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.
        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                STE_SAT_LOG_INFO("Parse_PC_GetInkey: Device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                 SourceID, DestinationID);
                (*parsed_pc_gik_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_gik_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_gik_pp)->bit_mask |= PC_GET_INKEY_DeviceID_present;
            }
            break;
        case TEXT_STRING_TAG:
            {
                uint8_t                   CodingScheme;

                if ( Length ) {
                (*parsed_pc_gik_pp)->text_string.text_length = Length - 1;      //Length minus the length for coding scheme, that is 1 byte.
                if ((*parsed_pc_gik_pp)->text_string.text_length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_gik_pp)->text_string.text_string_p = STE_SAT_MEM_ALLOCATE(Length);
                    if (NULL == (*parsed_pc_gik_pp)->text_string.text_string_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_GetInkey: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                ParseTextString(TLV_data_p, Length, &CodingScheme, (*parsed_pc_gik_pp)->text_string.text_string_p);
                (*parsed_pc_gik_pp)->text_string.coding_scheme = CodingScheme;
                (*parsed_pc_gik_pp)->bit_mask |= PC_GET_INKEY_TextString_present;
                }
            }
            break;
        case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_gik_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_gik_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_gik_pp)->icon_id.icon_identifier == 0) {
                    STE_SAT_LOG_WARNING("Parse_PC_GetInkey: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_gik_pp)->bit_mask |= PC_GET_INKEY_IconIdentifier_present;
            }
            break;
        case DURATION_TAG:
            {
                uint8_t                   TimeUnit;
                uint8_t                   TimeInterval;

                ParseDuration(TLV_data_p, &TimeUnit, &TimeInterval);
                (*parsed_pc_gik_pp)->bit_mask |= PC_GET_INKEY_Duration_present;
                // convert to an unified Time Unit and set it to the structure for outside user, time unit: TENS_OF_SECOND
                switch (TimeUnit) {
                case DURATION_TIME_UNIT_MINUTE:
                    {
                        (*parsed_pc_gik_pp)->duration = TimeInterval * 10 * 60;
                    }
                    break;
                case DURATION_TIME_UNIT_SECOND:
                    {
                        (*parsed_pc_gik_pp)->duration = TimeInterval * 10;
                    }
                    break;
                case DURATION_TIME_UNIT_TENS_OF_SECOND:
                    {
                        (*parsed_pc_gik_pp)->duration = TimeInterval;
                    }
                    break;
                default:
                    {
                        STE_SAT_LOG_WARNING("Parse_PC_GetInkey: Unknown Time Unit for Duration Tag.\n");
                    }
                }
            }
            break;
        case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_gik_pp)->text_attribute.text_attribute_length =
                    Length;
                if (Length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_gik_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);

                    if (NULL == (*parsed_pc_gik_pp)->text_attribute.text_attribute_p) {
                        STE_SAT_LOG_ERROR ("Parse_PC_GetInkey: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_gik_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_gik_pp)->bit_mask |= PC_GET_INKEY_TextAttribute_present;
            }
            break;
        case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_gik_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_gik_pp)->bit_mask |= PC_GET_INKEY_FrameIdentifier_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_GetInkey: Unknown Tag value in Get Inkey command.\n");
            }
        }                       // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }                           //end of while (dataLength >= 2)

    //check the command qualifier and interpret it
    RetVal = Interpret_CommandQualifier_PC_GetInkey(CommandQualifier, *parsed_pc_gik_pp);

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_GetInput
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_GetInput(const uint8_t * TLV_data_p,
                  uint8_t dataLength,
                  ste_apdu_pc_get_input_t ** parsed_pc_gi_pp,
                  uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_GetInput: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_GetInput: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_GetInput: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.
        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                STE_SAT_LOG_INFO("Parse_PC_GetInput: Device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                 SourceID, DestinationID);
                (*parsed_pc_gi_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_gi_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_gi_pp)->bit_mask |= PC_GET_INPUT_DeviceID_present;
            }
            break;
        case TEXT_STRING_TAG:
            {
                uint8_t                   CodingScheme;

                if ( Length ) {
                (*parsed_pc_gi_pp)->text_string.text_length = Length - 1;       //Length minus the length for coding scheme, that is 1 byte.
                if ((*parsed_pc_gi_pp)->text_string.text_length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_gi_pp)->text_string.text_string_p = STE_SAT_MEM_ALLOCATE(Length);
                    if (NULL == (*parsed_pc_gi_pp)->text_string.text_string_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_GetInput: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                ParseTextString(TLV_data_p, Length, &CodingScheme, (*parsed_pc_gi_pp)->text_string.text_string_p);
                (*parsed_pc_gi_pp)->text_string.coding_scheme = CodingScheme;
                (*parsed_pc_gi_pp)->bit_mask |= PC_GET_INPUT_TextString_present;
                }
            }
            break;
        case RESPONSE_LENGTH_TAG:
            {
                ParseResponseLength(TLV_data_p, &((*parsed_pc_gi_pp)->response_length));
                if ((*parsed_pc_gi_pp)->response_length.min_length > (*parsed_pc_gi_pp)->response_length.max_length) {
                    STE_SAT_LOG_ERROR("Parse_PC_GetInput: Invalid Min, Max Response Length.");
                    return STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
                }
                (*parsed_pc_gi_pp)->bit_mask |= PC_GET_INPUT_ResponseLength_present;
            }
            break;
        case DEFAULT_TEXT_TAG:
            {
                uint8_t                   CodingScheme;

                if ( Length ) {
                (*parsed_pc_gi_pp)->default_text.text_length = Length - 1;      //Length minus the length for coding scheme, that is 1 byte.
                if ((*parsed_pc_gi_pp)->default_text.text_length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_gi_pp)->default_text.text_string_p = STE_SAT_MEM_ALLOCATE(Length);
                    if (NULL == (*parsed_pc_gi_pp)->default_text.text_string_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_GetInput: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                ParseTextString(TLV_data_p, Length, &CodingScheme, (*parsed_pc_gi_pp)->default_text.text_string_p);
                (*parsed_pc_gi_pp)->default_text.coding_scheme = CodingScheme;
                (*parsed_pc_gi_pp)->bit_mask |= PC_GET_INPUT_DefaultText_present;
                }
            }
            break;
        case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_gi_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_gi_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_gi_pp)->icon_id.icon_identifier == 0) {
                    STE_SAT_LOG_WARNING("Parse_PC_GetInput: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_gi_pp)->bit_mask |= PC_GET_INPUT_IconIdentifier_present;
            }
            break;
        case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_gi_pp)->text_attribute.text_attribute_length = Length;
                if (Length > 0) {
                    //allocate memory for string text
                    (*parsed_pc_gi_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_gi_pp)->text_attribute.text_attribute_p) {
                        STE_SAT_LOG_ERROR("Parse_PC_GetInput: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_gi_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_gi_pp)->bit_mask |= PC_GET_INPUT_TextAttribute_present;
            }
            break;
        case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_gi_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_gi_pp)->bit_mask |= PC_GET_INPUT_FrameIdentifier_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_GetInput: Unknown Tag value in Get Input command.\n");
            }
        }                       // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }                           //end of while (dataLength >= 2)

    //check the command qualifier and interpret it
    RetVal = Interpret_CommandQualifier_PC_GetInput(CommandQualifier, *parsed_pc_gi_pp);

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_TimerManagement
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_TimerManagement(const uint8_t * TLV_data_p,
                         uint8_t dataLength,
                         ste_apdu_pc_timer_management_t ** parsed_pc_tm_pp,
                         uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_TimerManagement: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_TimerManagement: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_TimerManagement: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.
        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                STE_SAT_LOG_INFO("Parse_PC_TimerManagement: Device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                 SourceID, DestinationID);
                (*parsed_pc_tm_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_tm_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_tm_pp)->bit_mask |= PC_TIMER_MANAGEMENT_DeviceID_present;
            }
            break;
        case TIMER_IDENTIFIER_TAG:
            {
                ste_apdu_timer_identifier_code_value_t timer_code_value = STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_UNKNOWN;
                (*parsed_pc_tm_pp)->timer_identifier.timer_identifier_length = Length;
                ParseTimerIdentifier(TLV_data_p, &timer_code_value);
                (*parsed_pc_tm_pp)->timer_identifier.timer_code = timer_code_value;
                (*parsed_pc_tm_pp)->bit_mask |= PC_TIMER_MANAGEMENT_TimerIdentifier_present;
            }
            break;
        case TIMER_VALUE_TAG:
            {
                uint32_t                   TimerValue = 0;
                (*parsed_pc_tm_pp)->timer_value.timer_value_length = Length;
                //First convert the hour value
                TimerValue = TimerValue + ste_apdu_decode_bcd(*TLV_data_p) * 60 * 60; // TimerValue + (ste_apdu_swap_nibble(*TLV_data_p) * 60 * 60);
                //Next the minutes value
                TimerValue = TimerValue + ste_apdu_decode_bcd(*( TLV_data_p + 1 )) * 60;
                //Finally the seconds
                TimerValue = TimerValue + ste_apdu_decode_bcd(*( TLV_data_p + 2 ));
                STE_SAT_LOG_INFO("Parse_PC_TimerManagement: Timer Value: %d \n", TimerValue);
                (*parsed_pc_tm_pp)->timer_value.timer_value_data = TimerValue;
                (*parsed_pc_tm_pp)->bit_mask |= PC_TIMER_MANAGEMENT_TimerValue_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_TimerManagement: Unknown Tag value in Timer Management command.\n");
            }
        }                       // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }                           //end of while (dataLength >= 2)

    //check the command qualifier and interpret it
    RetVal = Interpret_CommandQualifier_PC_TimerManagement(CommandQualifier, *parsed_pc_tm_pp);

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_MoreTime
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_MoreTime(const uint8_t * TLV_data_p,
                  uint8_t dataLength,
                  ste_apdu_pc_more_time_t ** parsed_pc_mt_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_MoreTime: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_MoreTime: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_MoreTime: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.
        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                STE_SAT_LOG_INFO("Parse_PC_MoreTime: Device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                 SourceID, DestinationID);
                (*parsed_pc_mt_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_mt_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_mt_pp)->bit_mask |= PC_MORE_TIME_DeviceID_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_MoreTime: Unknown Tag value in MoreTime command.\n");
            }
        }
        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }

    return RetVal;
}


/*************************************************************************
 * Function:      Parse_PC_PollingOff
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_PollingOff(const uint8_t * TLV_data_p,
                    uint8_t dataLength,
                    ste_apdu_pc_polling_off_t ** parsed_pc_po_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_PollingOff: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_PollingOff: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_PollingOff: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.
        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                STE_SAT_LOG_INFO("Parse_PC_PollingOff: Device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                 SourceID, DestinationID);
                (*parsed_pc_po_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_po_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_po_pp)->bit_mask |= PC_POLLING_OFF_DeviceID_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_PollingOff: Unknown Tag value in PollingOff command.\n");
            }
        }
        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_PollInterval
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_PollInterval(const uint8_t * TLV_data_p,
                      uint8_t dataLength,
                      ste_apdu_pc_poll_interval_t ** parsed_pc_pi_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_PollInterval: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_PollInterval: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_PollInterval: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.
        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                STE_SAT_LOG_INFO("Parse_PC_PollInterval: Device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                 SourceID, DestinationID);
                (*parsed_pc_pi_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_pi_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_pi_pp)->bit_mask |= PC_POLL_INTERVAL_DeviceID_present;
            }
            break;
        case DURATION_TAG:
            {
                uint8_t                   TimeUnit;
                uint8_t                   TimeInterval;

                ParseDuration(TLV_data_p, &TimeUnit, &TimeInterval);

                (*parsed_pc_pi_pp)->bit_mask |= PC_POLL_INTERVAL_Duration_present;
                // convert to an unified Time Unit and set it to the structure for outside user, time unit: TENS_OF_SECOND
                switch (TimeUnit) {
                case DURATION_TIME_UNIT_MINUTE:
                    {
                        (*parsed_pc_pi_pp)->duration = TimeInterval * 10 * 60;
                        STE_SAT_LOG_INFO("Parse_PC_PollInterval: Duration unit is minutes.");
                    }
                    break;
                case DURATION_TIME_UNIT_SECOND:
                    {
                        (*parsed_pc_pi_pp)->duration = TimeInterval * 10;
                        STE_SAT_LOG_INFO("Parse_PC_PollInterval: Duration unit is seconds.");
                    }
                    break;
                case DURATION_TIME_UNIT_TENS_OF_SECOND:
                    {
                        (*parsed_pc_pi_pp)->duration = TimeInterval;
                        STE_SAT_LOG_INFO("Parse_PC_PollInterval: Duration unit is tens of seconds.");
                    }
                    break;
                default:
                    {
                        STE_SAT_LOG_WARNING("Parse_PC_PollInterval: Unknown Time Unit for Duration Tag.\n");
                    }
                }
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_PollInterval: Unknown Tag value in PollInterval command.\n");
            }
        }
        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }

    return RetVal;
}


/*************************************************************************
 * Function:      Parse_PC_SendShortMessage
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_SendShortMessage(const uint8_t * TLV_data_p,
                          uint8_t dataLength,
                          ste_apdu_pc_send_short_message_t ** parsed_pc_ssm_pp,
                          uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;             //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2)
    {
        STE_SAT_LOG_INFO("Parse_PC_SendShortMessage: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_SendShortMessage: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length))
        {
            STE_SAT_LOG_ERROR("Parse_PC_SendShortMessage: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue)
        {
            case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_NETWORK != DestinationID)
                {
                    STE_SAT_LOG_ERROR("Parse_PC_SendShortMessage: Unexpected device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_ssm_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_ssm_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_DeviceID_present;
            }
            break;
            case ALPHA_IDENTIFIER_TAG:
            {
                (*parsed_pc_ssm_pp)->alpha_identifier.alpha_id_length = Length;
                if (Length > 0)
                {
                    //allocate memory for alpha identifier string
                    (*parsed_pc_ssm_pp)->alpha_identifier.alpha_id_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_ssm_pp)->alpha_identifier.alpha_id_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendShortMessage: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Alpha Identifier string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_ssm_pp)->alpha_identifier.alpha_id_p, TLV_data_p, Length);
                STE_SAT_LOG_INFO("Result AlphaIdentifier value: %s \n", (*parsed_pc_ssm_pp)->alpha_identifier.alpha_id_p);
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_AlphaIdentifier_present;
            }
            break;
            case ADDRESS_TAG:
            {
                ParseAddress(TLV_data_p, Length, &((*parsed_pc_ssm_pp)->address));
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_Address_present;
            }
            break;
            case SMS_TPDU_TAG:
            {
                if (Length > SAT_MAX_SIZE_SEND_SMS_TPDU)
                {
                    STE_SAT_LOG_ERROR("Parse_PC_SendShortMessage: the size of TPDU is wrong.\n");
                    return STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
                }
                ParseSMS_TPDU(TLV_data_p, Length,  &((*parsed_pc_ssm_pp)->tpdu));
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_3GPP_SMS_TPDU_present;
            }
            break;
            case CDMA_SMS_TPDU_TAG:
            {
                ParseCDMA_SMS_TPDU(TLV_data_p, Length,  &((*parsed_pc_ssm_pp)->tpdu));
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_CDMA_SMS_TPDU_present;
            }
            break;
            case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_ssm_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_ssm_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_ssm_pp)->icon_id.icon_identifier == 0)
                {
                    STE_SAT_LOG_WARNING("Parse_PC_SendShortMessage: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_IconIdentifier_present;
            }
            break;
            case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_ssm_pp)->text_attribute.text_attribute_length = Length;
                if (Length > 0)
                {
                    //allocate memory for string text
                    (*parsed_pc_ssm_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_ssm_pp)->text_attribute.text_attribute_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendShortMessage: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_ssm_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_TextAttribute_present;
            }
            break;
            case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_ssm_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_ssm_pp)->bit_mask |= PC_SEND_SMS_FrameIdentifier_present;
            }
            break;
            default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_SendShortMessage: Unknown Tag value in send short message command.\n");
            }
        }  // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }  //end of while (dataLength >= 2)

    //check the command qualifier and interpret it
    (*parsed_pc_ssm_pp)->command_details.packing_required = CommandQualifier & SEND_SMS_PACKING_REQ_BY_ME_MASK;

    return RetVal;
}


/*************************************************************************
 * Function:      Parse_PC_SendDTMF
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_SendDTMF(const uint8_t * TLV_data_p,
                  uint8_t dataLength,
                  ste_apdu_pc_send_dtmf_t ** parsed_pc_dtmf_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;             //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2)
    {
        STE_SAT_LOG_INFO("Parse_PC_SendDTMF: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_SendDTMF: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length))
        {
            STE_SAT_LOG_ERROR("Parse_PC_SendDTMF: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue)
        {
            case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_NETWORK != DestinationID)
                {
                    STE_SAT_LOG_ERROR("Parse_PC_SendDTMF: Unexpected device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_dtmf_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_dtmf_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_dtmf_pp)->bit_mask |= PC_SEND_DTMF_DeviceID_present;
            }
            break;
            case ALPHA_IDENTIFIER_TAG:
            {
                (*parsed_pc_dtmf_pp)->alpha_identifier.alpha_id_length = Length;
                if (Length > 0)
                {
                    //allocate memory for alpha identifier string
                    (*parsed_pc_dtmf_pp)->alpha_identifier.alpha_id_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_dtmf_pp)->alpha_identifier.alpha_id_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendDTMF: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Alpha Identifier string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_dtmf_pp)->alpha_identifier.alpha_id_p, TLV_data_p, Length);
                STE_SAT_LOG_INFO("Result AlphaIdentifier value: %s \n", (*parsed_pc_dtmf_pp)->alpha_identifier.alpha_id_p);
                (*parsed_pc_dtmf_pp)->bit_mask |= PC_SEND_DTMF_AlphaIdentifier_present;
            }
            break;
            case DTMF_STRING_TAG:
            {
                ParseDTMF(TLV_data_p, Length,  &((*parsed_pc_dtmf_pp)->dtmf));
                (*parsed_pc_dtmf_pp)->bit_mask |= PC_SEND_DTMF_DTMF_String_present;
            }
            break;
            case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_dtmf_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_dtmf_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_dtmf_pp)->icon_id.icon_identifier == 0)
                {
                    STE_SAT_LOG_WARNING("Parse_PC_SendDTMF: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_dtmf_pp)->bit_mask |= PC_SEND_DTMF_IconIdentifier_present;
            }
            break;
            case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_dtmf_pp)->text_attribute.text_attribute_length = Length;
                if (Length > 0)
                {
                    //allocate memory for string text
                    (*parsed_pc_dtmf_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_dtmf_pp)->text_attribute.text_attribute_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendDTMF: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_dtmf_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_dtmf_pp)->bit_mask |= PC_SEND_DTMF_TextAttribute_present;
            }
            break;
            case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_dtmf_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_dtmf_pp)->bit_mask |= PC_SEND_DTMF_FrameIdentifier_present;
            }
            break;
            default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_SendDTMF: Unknown Tag value in send dtmf command. 0x%x\n", TagValue);
            }
        }  // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }  //end of while (dataLength >= 2)

    return RetVal;
}


/*************************************************************************
 * Function:      Parse_PC_SendSS
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_SendSS(const uint8_t * TLV_data_p,
                uint8_t dataLength,
                ste_apdu_pc_send_ss_t ** parsed_pc_ss_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;             //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2)
    {
        STE_SAT_LOG_INFO("Parse_PC_SendSS: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_SendSS: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length))
        {
            STE_SAT_LOG_ERROR("Parse_PC_SendSS: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue)
        {
            case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_NETWORK != DestinationID)
                {
                    STE_SAT_LOG_ERROR("Parse_PC_SendSS: Unexpected device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_ss_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_ss_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_ss_pp)->bit_mask |= PC_SEND_SS_DeviceID_present;
            }
            break;
            case ALPHA_IDENTIFIER_TAG:
            {
                (*parsed_pc_ss_pp)->alpha_identifier.alpha_id_length = Length;
                if (Length > 0)
                {
                    //allocate memory for alpha identifier string
                    (*parsed_pc_ss_pp)->alpha_identifier.alpha_id_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_ss_pp)->alpha_identifier.alpha_id_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendSS: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Alpha Identifier string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_ss_pp)->alpha_identifier.alpha_id_p, TLV_data_p, Length);
                STE_SAT_LOG_INFO("Result AlphaIdentifier value: %s \n", (*parsed_pc_ss_pp)->alpha_identifier.alpha_id_p);
                (*parsed_pc_ss_pp)->bit_mask |= PC_SEND_SS_AlphaIdentifier_present;
            }
            break;
            case SS_STRING_TAG:
            {
                ParseSS(TLV_data_p, Length,  &((*parsed_pc_ss_pp)->ss_string));
                (*parsed_pc_ss_pp)->bit_mask |= PC_SEND_SS_SS_String_present;
            }
            break;
            case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_ss_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_ss_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_ss_pp)->icon_id.icon_identifier == 0)
                {
                    STE_SAT_LOG_WARNING("Parse_PC_SendSS: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_ss_pp)->bit_mask |= PC_SEND_SS_IconIdentifier_present;
            }
            break;
            case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_ss_pp)->text_attribute.text_attribute_length = Length;
                if (Length > 0)
                {
                    //allocate memory for string text
                    (*parsed_pc_ss_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_ss_pp)->text_attribute.text_attribute_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendSS: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_ss_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_ss_pp)->bit_mask |= PC_SEND_SS_TextAttribute_present;
            }
            break;
            case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_ss_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_ss_pp)->bit_mask |= PC_SEND_SS_FrameIdentifier_present;
            }
            break;
            default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_SendSS: Unknown Tag value in send ss command. 0x%x\n", TagValue);
            }
        }  // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }  //end of while (dataLength >= 2)

    return RetVal;
}


/*************************************************************************
 * Function:      Parse_PC_SendUSSD
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_SendUSSD(const uint8_t * TLV_data_p,
                  uint8_t dataLength,
                  ste_apdu_pc_send_ussd_t ** parsed_pc_ussd_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;             //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2)
    {
        STE_SAT_LOG_INFO("Parse_PC_SendUSSD: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_SendUSSD: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length))
        {
            STE_SAT_LOG_ERROR("Parse_PC_SendUSSD: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue)
        {
            case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_NETWORK != DestinationID)
                {
                    STE_SAT_LOG_ERROR("Parse_PC_SendUSSD: Unexpected device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_ussd_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_ussd_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_ussd_pp)->bit_mask |= PC_SEND_USSD_DeviceID_present;
            }
            break;
            case ALPHA_IDENTIFIER_TAG:
            {
                (*parsed_pc_ussd_pp)->alpha_identifier.alpha_id_length = Length;
                if (Length > 0)
                {
                    //allocate memory for alpha identifier string
                    (*parsed_pc_ussd_pp)->alpha_identifier.alpha_id_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_ussd_pp)->alpha_identifier.alpha_id_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendUSSD: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Alpha Identifier string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_ussd_pp)->alpha_identifier.alpha_id_p, TLV_data_p, Length);
                STE_SAT_LOG_INFO("Result AlphaIdentifier value: %s \n", (*parsed_pc_ussd_pp)->alpha_identifier.alpha_id_p);
                (*parsed_pc_ussd_pp)->bit_mask |= PC_SEND_USSD_AlphaIdentifier_present;
            }
            break;
            case USSD_STRING_TAG:
            {
                ParseUSSD(TLV_data_p, Length,  &((*parsed_pc_ussd_pp)->ussd_string));
                (*parsed_pc_ussd_pp)->bit_mask |= PC_SEND_USSD_USSD_String_present;
            }
            break;
            case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_ussd_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_ussd_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_ussd_pp)->icon_id.icon_identifier == 0)
                {
                    STE_SAT_LOG_WARNING("Parse_PC_SendUSSD: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_ussd_pp)->bit_mask |= PC_SEND_USSD_IconIdentifier_present;
            }
            break;
            case TEXT_ATTRIBUTE_TAG:
            {
                (*parsed_pc_ussd_pp)->text_attribute.text_attribute_length = Length;
                if (Length > 0)
                {
                    //allocate memory for string text
                    (*parsed_pc_ussd_pp)->text_attribute.text_attribute_p = STE_SAT_MEM_ALLOCATE(Length + 1);
                    if (NULL == (*parsed_pc_ussd_pp)->text_attribute.text_attribute_p)
                    {
                        STE_SAT_LOG_ERROR("Parse_PC_SendUSSD: memory allocation failed.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                //copy the Text attribute string directly to the structure
                STE_SAT_MEM_COPY((*parsed_pc_ussd_pp)->text_attribute.text_attribute_p, TLV_data_p, Length);
                (*parsed_pc_ussd_pp)->bit_mask |= PC_SEND_USSD_TextAttribute_present;
            }
            break;
            case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_ussd_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_ussd_pp)->bit_mask |= PC_SEND_USSD_FrameIdentifier_present;
            }
            break;
            default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_SendUSSD: Unknown Tag value in send ussd command. 0x%x\n", TagValue);
            }
        }  // end of switch (TagValue)

        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }  //end of while (dataLength >= 2)

    return RetVal;
}


/*************************************************************************
 * Function:      Parse_PC_SetupCall
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:         This command has 2 optional Alpha identifier and Icon Identifier Tags.
 *                The structure of the command is shown below in simplified form
 *
 *                    COMMAND_DETAILS_TAG                             Mandatory
 *                    DEVICE_IDENTITIES_TAG                           Mandatory
 *                    ALPHA_IDENTIFIER_TAG (user confirmation phase)  Optional
 *                    ADDRESS_TAG                                     Mandatory
 *                    CAPABILITY_CONFIG_PARAMS_TAG                    Optional
 *                    CALLED_PARTY_SUBADDRESS_TAG                     Optional
 *                    DURATION_TAG,                                   Optional
 *                    ICON_IDENTIFIER_TAG (user confirmation phase)   Optional
 *                    ALPHA_IDENTIFIER_TAG (Call setup phase)         Optional
 *                    ICON_IDENTIFIER_TAG (Call setup phase)          Optional
 *                    TEXT_ATTRIBUTE_TAG (user confirmation phase)    Optional
 *                    TEXT_ATTRIBUTE_TAG (Call setup phase)           Optional
 *                    FRAME_IDENTIFIER_TAG (Call setup phase)         Optional
 *
 *                The following tests are used to determine whether a specific Alpha
 *                identifier or Icon identifier exists (i.e. distinguish between them)
 *                1. If an Alpha identifier Tag is found which is immediately preceded
 *                   by a Device identities Tag then it must be the 'user confirmation'
 *                   Alpha identifier - any other preceding Tag means that it's for
 *                   'Call setup'
 *                2. If an Icon identifier Tag is found which is immediately preceded
 *                   by an Alpha identifier Tag then it must be for 'Call setup' -
 *                   any other preceding Tag means it's for 'user confirmation'
 *                   (this works because a corresponding Alpha identifier always
 *                   exists for an Icon identifier).
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_SetupCall(const uint8_t * TLV_data_p,
                   uint8_t dataLength,
                   ste_apdu_pc_setup_call_t ** parsed_pc_sc_pp,
                   uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;  //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;       //the length for value part only.
    uint8_t                   TagValue;
    uint8_t                   preceded_tag = 0x0;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2)
    {
        STE_SAT_LOG_INFO("Parse_PC_SetupCall: Data Length left to be parsed: %d \n", dataLength);
        STE_SAT_LOG_INFO("Parse_PC_SetupCall: Parse Tag: 0x%x \n", (*TLV_data_p));

        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length))
        {
            STE_SAT_LOG_ERROR("Parse_PC_SetupCall: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue)
        {
            case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_NETWORK != DestinationID)
                {
                    STE_SAT_LOG_ERROR("Parse_PC_SetupCall: Unexpected device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_sc_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_sc_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_DeviceID_present;
                preceded_tag = DEVICE_IDENTITIES_TAG;
            }
            break;
            case ALPHA_IDENTIFIER_TAG:
            {
                if (preceded_tag == DEVICE_IDENTITIES_TAG
                    && !((*parsed_pc_sc_pp)->bit_mask & PC_SETUP_CALL_AlphaIdentifier_1_present))
                {
                    ParseAlphaIdentifier(TLV_data_p, Length, &((*parsed_pc_sc_pp)->alpha_id_1));
                    (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_AlphaIdentifier_1_present;
                }
                else
                {
                    ParseAlphaIdentifier(TLV_data_p, Length, &((*parsed_pc_sc_pp)->alpha_id_2));
                    (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_AlphaIdentifier_2_present;
                }
                preceded_tag = ALPHA_IDENTIFIER_TAG;
            }
            break;
            case ADDRESS_TAG:
            {
                ParseAddress(TLV_data_p, Length, &((*parsed_pc_sc_pp)->address));
                (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_Address_present;
                preceded_tag = ADDRESS_TAG;
            }
            break;
            case CAPABILITY_CONFIG_PARAMS_TAG:
            {
                ParseCCP(TLV_data_p, Length, &((*parsed_pc_sc_pp)->ccp));
                (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_CCP_present;
                preceded_tag = CAPABILITY_CONFIG_PARAMS_TAG;
            }
            break;
            case CALLED_PARTY_SUBADDRESS_TAG:
            {
                ParseSubAddress(TLV_data_p, Length, &((*parsed_pc_sc_pp)->sub_address));
                (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_SubAddress_present;
                preceded_tag = CALLED_PARTY_SUBADDRESS_TAG;
            }
            break;
            case ICON_IDENTIFIER_TAG:
            {
                if (preceded_tag == ALPHA_IDENTIFIER_TAG
                    && !((*parsed_pc_sc_pp)->bit_mask & PC_SETUP_CALL_IconIdentifier_2_present))
                {
                    ParseIconIdentifier(TLV_data_p,
                                        &((*parsed_pc_sc_pp)->icon_id_2.icon_qualifier),
                                        &((*parsed_pc_sc_pp)->icon_id_2.icon_identifier));
                    if ((*parsed_pc_sc_pp)->icon_id_2.icon_identifier == 0)
                    {
                        STE_SAT_LOG_WARNING("Parse_PC_SetupCall: Invalid Icon Identifier.");
                    }
                    //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                    (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_IconIdentifier_2_present;
                }
                else if (!((*parsed_pc_sc_pp)->bit_mask & PC_SETUP_CALL_IconIdentifier_1_present))
                {
                    ParseIconIdentifier(TLV_data_p,
                                        &((*parsed_pc_sc_pp)->icon_id_1.icon_qualifier),
                                        &((*parsed_pc_sc_pp)->icon_id_1.icon_identifier));
                    if ((*parsed_pc_sc_pp)->icon_id_1.icon_identifier == 0)
                    {
                        STE_SAT_LOG_WARNING("Parse_PC_SetupCall: Invalid Icon Identifier.");
                    }
                    //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                    (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_IconIdentifier_1_present;
                }
                else
                {
                    STE_SAT_LOG_WARNING("Parse_PC_SetupCall: apdu data is not understandable.\n");
                }

                preceded_tag = ICON_IDENTIFIER_TAG;
            }
            break;
            case DURATION_TAG:
            {
                uint8_t                   TimeUnit;
                uint8_t                   TimeInterval;

                ParseDuration(TLV_data_p, &TimeUnit, &TimeInterval);
                (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_Duration_present;
                // convert to an unified Time Unit and set it to the structure for outside user, time unit: TENS_OF_SECOND
                switch (TimeUnit)
                {
                    case DURATION_TIME_UNIT_MINUTE:
                    {
                        (*parsed_pc_sc_pp)->duration = TimeInterval * 10 * 60;
                    }
                    break;
                    case DURATION_TIME_UNIT_SECOND:
                    {
                        (*parsed_pc_sc_pp)->duration = TimeInterval * 10;
                    }
                    break;
                    case DURATION_TIME_UNIT_TENS_OF_SECOND:
                    {
                        (*parsed_pc_sc_pp)->duration = TimeInterval;
                    }
                    break;
                    default:
                    {
                        STE_SAT_LOG_WARNING("Parse_PC_SetupCall: Unknown Time Unit for Duration Tag.\n");
                    }
                }
                preceded_tag = DURATION_TAG;
            }
            break;
            case TEXT_ATTRIBUTE_TAG:
            {
                if (((*parsed_pc_sc_pp)->bit_mask & PC_SETUP_CALL_AlphaIdentifier_1_present)
                    && !((*parsed_pc_sc_pp)->bit_mask & PC_SETUP_CALL_TextAttribute_1_present))
                {
                    ParseTextAttribute(TLV_data_p, Length, &((*parsed_pc_sc_pp)->text_attribute_1));
                    (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_TextAttribute_1_present;
                }
                else
                {
                    ParseTextAttribute(TLV_data_p, Length, &((*parsed_pc_sc_pp)->text_attribute_2));
                    (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_TextAttribute_2_present;
                }

                preceded_tag = TEXT_ATTRIBUTE_TAG;
            }
            break;
            case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_sc_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_sc_pp)->bit_mask |= PC_SETUP_CALL_FrameIdentifier_present;
                preceded_tag = FRAME_IDENTIFIER_TAG;
            }
            break;
            default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_SetupCall: Unknown Tag value in Setup Call command.\n");
            }
        }  // end of switch (TagValue)

        TLV_data_p += Length;  //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;  //set the remaining unparsed data length.
    }  //end of while (dataLength >= 2)

    //check the command qualifier and interpret it
    RetVal = Interpret_CommandQualifier_PC_SetupCall(CommandQualifier, *parsed_pc_sc_pp);

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_Refresh
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 *                    COMMAND_DETAILS_TAG                             Mandatory
 *                    DEVICE_IDENTITIES_TAG                           Mandatory
 *                    FILE_LIST_TAG                                   Optional
 *                    AID_TAG                                         Optional
 *                    ALPHA_IDENTIFIER_TAG                            Optional
 *                    ICON_IDENTIFIER_TAG                             Optional
 *                    TEXT_ATTRIBUTE_TAG                              Optional
 *                    FRAME_IDENTIFIER_TAG                            Optional
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_Refresh(const uint8_t * TLV_data_p,
                 uint8_t dataLength,
                 ste_apdu_pc_refresh_t ** parsed_pc_r_pp,
                 uint8_t CommandQualifier)
{
    uint8_t                   TotalLength;  //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;       //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2)
    {
        STE_SAT_LOG_INFO("Parse_PC_Refresh: Data Length left to be parsed: %d \n", dataLength);
        STE_SAT_LOG_INFO("Parse_PC_Refresh: Parse Tag: 0x%x \n", (*TLV_data_p));

        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;

        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length))
        {
            STE_SAT_LOG_ERROR("Parse_PC_Refresh: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.

        switch (TagValue)
        {
            case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                if (DEVICE_SIM != SourceID || DEVICE_ME != DestinationID)
                {
                    STE_SAT_LOG_ERROR("Parse_PC_Refresh: Unexpected device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                      SourceID, DestinationID);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
                (*parsed_pc_r_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_r_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_DeviceID_present;
            }
            break;
            case ALPHA_IDENTIFIER_TAG:
            {
                ParseAlphaIdentifier(TLV_data_p, Length, &((*parsed_pc_r_pp)->alpha_identifier));
                (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_AlphaIdentifier_present;
            }
            break;
            case ICON_IDENTIFIER_TAG:
            {
                ParseIconIdentifier(TLV_data_p,
                                    &((*parsed_pc_r_pp)->icon_id.icon_qualifier),
                                    &((*parsed_pc_r_pp)->icon_id.icon_identifier));
                if ((*parsed_pc_r_pp)->icon_id.icon_identifier == 0)
                {
                    STE_SAT_LOG_WARNING("Parse_PC_Refresh: Invalid Icon Identifier.");
                }
                //TODO: The icon ID needs to be converted??? ConvertIconIdentifierTLV_ToSAT_TF
                (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_IconIdentifier_present;
            }
            break;
            case TEXT_ATTRIBUTE_TAG:
            {
                ParseTextAttribute(TLV_data_p, Length, &((*parsed_pc_r_pp)->text_attribute));
                (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_TextAttribute_present;
            }
            break;
            case FRAME_IDENTIFIER_TAG:
            {
                //copy the frame identifier directly to the structure, only one byte
                (*parsed_pc_r_pp)->frame_identifier = *TLV_data_p;
                (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_FrameIdentifier_present;
            }
            break;
            case FILE_LIST_TAG:
            {
                ParseFileList(TLV_data_p, Length, &((*parsed_pc_r_pp)->file_list));
                (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_FileList_present;
            }
            break;
            case AID_TAG:
            {
                ParseApplicationID(TLV_data_p, Length, &((*parsed_pc_r_pp)->app_id));
                (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_AID_present;
            }
            break;
            default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_Refresh: Unknown Tag value in Refresh command.\n");
            }
        }  // end of switch (TagValue)

        TLV_data_p += Length;  //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;  //set the remaining unparsed data length.
    }  //end of while (dataLength >= 2)

    // Assumes 1:1 mapping between the 3GPP values and the local enum type
    (*parsed_pc_r_pp)->command_details.refresh_type = CommandQualifier;
    (*parsed_pc_r_pp)->bit_mask |= PC_REFRESH_CommandDetails_present;

    return RetVal;
}

/*************************************************************************
 * Function:      Parse_PC_LanguageNotification
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Parse_PC_LanguageNotification(const uint8_t            * TLV_data_p,
                              uint8_t                    dataLength,
                              ste_apdu_pc_language_notification_t ** parsed_pc_ln_pp)
{
    uint8_t                   TotalLength;        //the total length for one specific TLV, including Tag, Length, and Value
    uint8_t                   Length;     //the length for value part only.
    uint8_t                   TagValue;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    while (dataLength >= 2) {
        STE_SAT_LOG_INFO("Parse_PC_LanguageNotification: Data Length left to be parsed: %d \n", dataLength);

        STE_SAT_LOG_INFO("Parse_PC_LanguageNotification: Parse Tag: 0x%x \n", (*TLV_data_p));
        TagValue = (*TLV_data_p) & ~COMPREHENSION_REQUIRED_MASK;
        if (!ParseAndValidateSimpleTLV_Length(TLV_data_p, &TotalLength, &Length)) {
            STE_SAT_LOG_ERROR("Parse_PC_LanguageNotification: Unexpected length for specific tag, Tag: 0x%x \n", TagValue);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        TLV_data_p += (TotalLength - Length);   //move pointer TLV_data_p to the value/data part of this TLV for this specific Tag.
        switch (TagValue) {
        case DEVICE_IDENTITIES_TAG:
            {
                uint8_t                   SourceID;
                uint8_t                   DestinationID;

                ParseDeviceIdentities(TLV_data_p, &SourceID, &DestinationID);
                STE_SAT_LOG_INFO("Parse_PC_LanguageNotification: Device ID: Source ID: 0x%x, Destination ID: 0x%x.\n",
                                 SourceID, DestinationID);
                (*parsed_pc_ln_pp)->device_id.source_ID = SourceID;
                (*parsed_pc_ln_pp)->device_id.destination_ID = DestinationID;
                (*parsed_pc_ln_pp)->bit_mask |= PC_LANGUAGE_NOTIFICATION_DeviceID_present;
            }
            break;
        case LANGUAGE_TAG:
            {
                ParseLanguageIdentifier(TLV_data_p, Length, &((*parsed_pc_ln_pp)->language));
                (*parsed_pc_ln_pp)->bit_mask |= PC_LANGUAGE_NOTIFICATION_Language_present;
            }
            break;
        default:
            {
                STE_SAT_LOG_WARNING("Parse_PC_LanguageNotification: Unknown Tag value in LanguageNotification command.\n");
            }
            break;
        }
        TLV_data_p += Length;   //move pointer TLV_data_p to the first byte of next TLV, or end of the BER TLV.
        dataLength -= TotalLength;      //set the remaining unparsed data length.
    }

    return RetVal;
}

/*************************************************************************
 * Function:      Interpret_CommandQualifier_PC_DisplayText
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_DisplayText(uint8_t CommandQualifier,
                                          ste_apdu_pc_display_text_t * pc_dt_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    if (pc_dt_p != NULL) {      /* Interpret command qualifier - Ref. 31.111 section 8.6 */

        if ((CommandQualifier & DISPLAY_TEXT_HIGH_PRIORITY_MASK) == DISPLAY_TEXT_HIGH_PRIORITY_MASK) {
            pc_dt_p->command_details.priority = SAT_HIGH_PRIORITY;
        } else {
            pc_dt_p->command_details.priority = SAT_NORMAL_PRIORITY;
        }

        if ((CommandQualifier & DISPLAY_TEXT_CLEAR_BY_USER_MASK) == DISPLAY_TEXT_CLEAR_BY_USER_MASK) {
            pc_dt_p->command_details.clear_text = SAT_WAIT_FOR_USER_TO_CLEAR;
        } else {
            pc_dt_p->command_details.clear_text = SAT_CLEAR_AFTER_DELAY;
        }
    }
    /* end Interpret command qualifier */
    return RetVal;
}

/*************************************************************************
 * Function:      Interpret_CommandQualifier_PC_TimerManagement
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_TimerManagement(uint8_t CommandQualifier,
                                              ste_apdu_pc_timer_management_t * pc_tm_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    if (pc_tm_p != NULL)
    {      /* Interpret command qualifier - Ref. 31.111 section 8.6 */
        if ((CommandQualifier & TIMER_MANAGEMENT_TIMER_NONE_MASK) == TIMER_MANAGEMENT_TIMER_START)
        {
            pc_tm_p->command_details.timer_action = TIMER_START;
        }
        else if ((CommandQualifier & TIMER_MANAGEMENT_TIMER_NONE_MASK) == TIMER_MANAGEMENT_TIMER_DEACTIVATE)
        {
            pc_tm_p->command_details.timer_action = TIMER_DEACTIVATE;
        }
        else if ((CommandQualifier & TIMER_MANAGEMENT_TIMER_NONE_MASK) == TIMER_MANAGEMENT_TIMER_CURRENT_VALUE)
        {
            pc_tm_p->command_details.timer_action = TIMER_GET_CURRENT_VALUE;
        }
        else if ((CommandQualifier & TIMER_MANAGEMENT_TIMER_NONE_MASK) == TIMER_MANAGEMENT_TIMER_NONE)
        {
            pc_tm_p->command_details.timer_action = TIMER_NONE;
        }
        else
        {
            STE_SAT_LOG_WARNING("Interpret_CommandQualifier_PC_TimerManagement: Unknown Command Qualifier.\n");
        }
    }
    /* end Interpret command qualifier */
    return RetVal;
}

/*************************************************************************
 * Function:      Interpret_CommandQualifier_PC_SetupMenu
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_SetupMenu(uint8_t CommandQualifier,
                                        ste_apdu_pc_set_up_menu_t * pc_sum_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    if (pc_sum_p != NULL)
    {     /* Interpret command qualifier - Ref. 31.111 section 8.6 */

        if ((CommandQualifier & SETUP_MENU_PREFERRED_SELECTION_MASK) == SETUP_MENU_PREFERRED_SELECTION)
        {
            pc_sum_p->command_details.PreferredSelection = PREFERRED_SELECTION_SOFT_KEY;
        }
        else
        {
            pc_sum_p->command_details.PreferredSelection = PREFERRED_SELECTION_NONE;
        }
        if ((CommandQualifier & SETUP_MENU_HELP_INFO_AVAILABLE_MASK) == SETUP_MENU_HELP_INFO_AVAILABLE)
        {
            pc_sum_p->command_details.HelpInfoAvailable = HELP_INFO_AVAILABLE;
        }
        else
        {
            pc_sum_p->command_details.HelpInfoAvailable = HELP_INFO_UNAVAILABLE;
        }
    }
    /* end Interpret command qualifier */
    return RetVal;
}

/*************************************************************************
 * Function:      Interpret_CommandQualifier_PC_GetInkey
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_GetInkey(uint8_t CommandQualifier,
                                       ste_apdu_pc_get_inkey_t * pc_gik_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    if (pc_gik_p != NULL)
    {
        // This option overrides the other 2 below
        if ((CommandQualifier & GET_INKEY_YES_NO_INPUT_MASK) == GET_INKEY_YES_NO_INPUT)
        {
            pc_gik_p->command_details.ResponseType = STE_APDU_GET_INKEY_YES_NO;
        }
        else
        {
            if ((CommandQualifier & USER_INPUT_TYPE_MASK) == USER_INPUT_DIGITS)
            {
                pc_gik_p->command_details.ResponseType = STE_APDU_GET_INKEY_DIGITS_AST_HASH_PLUS_ONLY;
            }
            // Alpabetic input. Decide which alphabet:
            else if ((CommandQualifier & USER_INPUT_ALPHABET_MASK) == USER_INPUT_UCS2_ALPHABET)
            {
                pc_gik_p->command_details.ResponseType = STE_APDU_GET_INKEY_UCS2_CODED;
            }
            else
            {
                pc_gik_p->command_details.ResponseType = STE_APDU_GET_INKEY_ENTIRE_SMS_DEFAULT_ALPHABET;
            }
            //check the bit 4 to see if user response should be displayed
            if ((CommandQualifier & GET_INKEY_IMMEDIATE_RESPONSE_MASK) == GET_INKEY_IMMEDIATE_RESPONSE)
            {
                pc_gik_p->command_details.Mode = STE_APDU_GET_INKEY_IMMEDIATE_RESPONSE;
            }
            else
            {
                pc_gik_p->command_details.Mode = STE_APDU_GET_INKEY_DISPLAY_RESPONSE;
            }
        }
        if ((CommandQualifier & GENERIC_HELP_INFO_AVAILABLE_MASK) == GENERIC_HELP_INFO_AVAILABLE)
        {
            pc_gik_p->command_details.HelpInfoAvailable = TRUE;
        }
        else
        {
            pc_gik_p->command_details.HelpInfoAvailable = FALSE;
        }
    }

    return RetVal;
}

/*************************************************************************
 * Function:      Interpret_CommandQualifier_PC_GetInput
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static                  ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_GetInput(uint8_t CommandQualifier,
                                       ste_apdu_pc_get_input_t * pc_gi_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    if (pc_gi_p == NULL)
    {
        STE_SAT_LOG_ERROR("Interpret_CommandQualifier_PC_GetInput: Invalid pointer.\n");
        return STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION;
    }

    if ((CommandQualifier & GET_INPUT_SM_PACK_USER_INPUT_MASK) == GET_INPUT_SM_PACK_USER_INPUT)
    {
        pc_gi_p->command_details.PackFormat = STE_APDU_GET_INPUT_SMS_PACKED;
    }
    else
    {
        pc_gi_p->command_details.PackFormat = STE_APDU_GET_INPUT_UNPACKED;
    }

    if ((CommandQualifier & GET_INPUT_DO_NOT_ECHO_MASK) == GET_INPUT_DO_NOT_ECHO_INPUT)
    {
        pc_gi_p->command_details.EchoAllowed = FALSE;
    }
    else
    {
        pc_gi_p->command_details.EchoAllowed = TRUE;
    }

    if ((CommandQualifier & USER_INPUT_TYPE_MASK) == USER_INPUT_DIGITS)
    {
        pc_gi_p->command_details.ResponseType = STE_APDU_GET_INPUT_DIGITS_AST_HASH_PLUS_ONLY;
    }
    // Alpabetic input. Decide which alphabet:
    else if ((CommandQualifier & USER_INPUT_ALPHABET_MASK) == USER_INPUT_UCS2_ALPHABET)
    {
        // Note: ignore the PackingIsRequired flag, it's not relevant for UCS2
        pc_gi_p->command_details.ResponseType = STE_APDU_GET_INPUT_UCS2_CODED;
        // Check for illegal combinations
        if (!pc_gi_p->command_details.EchoAllowed)
        {
            // Not allowed Alphabet entry in Hidden Entry mode
            STE_SAT_LOG_WARNING("Interpret_CommandQualifier_PC_GetInput: Invalid parameter.\n");
        }
    }
    else
    {
        pc_gi_p->command_details.ResponseType = STE_APDU_GET_INPUT_ENTIRE_SMS_DEFAULT_ALPHABET;
        // Check for illegal combinations
        if (!pc_gi_p->command_details.EchoAllowed)
        {
            // Not allowed Alphabet entry in Hidden Entry mode
            STE_SAT_LOG_WARNING("Interpret_CommandQualifier_PC_GetInput: Invalid parameter.\n");
        }
    }

    if ((CommandQualifier & GENERIC_HELP_INFO_AVAILABLE_MASK) == GENERIC_HELP_INFO_AVAILABLE)
    {
        pc_gi_p->command_details.HelpInfoAvailable = TRUE;
    }
    else
    {
        pc_gi_p->command_details.HelpInfoAvailable = FALSE;
    }

    return RetVal;
}


/*************************************************************************
 * Function:      Interpret_CommandQualifier_PC_SetupCall
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Interpret_CommandQualifier_PC_SetupCall(uint8_t CommandQualifier,
                                        ste_apdu_pc_setup_call_t * pc_sc_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    if (pc_sc_p != NULL)
    {      /* Interpret command qualifier - Ref. 31.111 section 8.6 */
        pc_sc_p->command_details.redial = FALSE;

        switch (CommandQualifier)
        {
            case SETUP_CALL_WITH_REDIAL_ONLY_IF_NOT_BUSY:
            {
                pc_sc_p->command_details.redial = TRUE;
            }
            // no break
            case SETUP_CALL_ONLY_IF_NOT_BUSY:
            {
                pc_sc_p->command_details.option = STE_APDU_ONLY_IF_NOT_BUSY;
            }
            break;

            case SETUP_CALL_WITH_REDIAL_PUT_ALL_OTHER_ON_HOLD:
            {
                pc_sc_p->command_details.redial = TRUE;
            }
            // no break
            case SETUP_CALL_PUT_ALL_OTHER_CALLS_ON_HOLD:
            {
                pc_sc_p->command_details.option = STE_APDU_PUT_ALL_OTHER_CALLS_ON_HOLD;
            }
            break;

            case SETUP_CALL_WITH_REDIAL_DISCONNECT_ALL_OTHER:
            {
                pc_sc_p->command_details.redial = TRUE;
            }
            // no break
            case SETUP_CALL_DISCONNECT_ALL_OTHER_CALLS:
            {
                pc_sc_p->command_details.option = STE_APDU_DISCONNECT_ALL_OTHER_CALLS;
            }
            break;

            default:
            {
            // Reserved value
                RetVal = STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
            }
        }
    }
    /* end Interpret command qualifier */
    return RetVal;
}

/*************************************************************************
 * Function:      Create_CommandQualifier_PC_DisplayText
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
Create_CommandQualifier_PC_DisplayText(ste_parsed_apdu_t * apdu_info_p)
{
    if (apdu_info_p != NULL)
    {  /* create command qualifier - Ref. 31.111 section 8.6 */
        //init the value
        apdu_info_p->command_info.CommandQualifier = 0;
        if (apdu_info_p->apdu_data.pc_dt_p->command_details.priority == SAT_HIGH_PRIORITY)
        {
            apdu_info_p->command_info.CommandQualifier |= DISPLAY_TEXT_HIGH_PRIORITY_MASK;
        }
        if (apdu_info_p->apdu_data.pc_dt_p->command_details.clear_text == SAT_WAIT_FOR_USER_TO_CLEAR)
        {
            apdu_info_p->command_info.CommandQualifier |= DISPLAY_TEXT_CLEAR_BY_USER_MASK;
        }
    }
    /* end create command qualifier */
    return;
}

/*************************************************************************
 * Function:      Create_CommandQualifier_PC_SetupMenu
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
Create_CommandQualifier_PC_SetupMenu(ste_parsed_apdu_t * apdu_info_p)
{
    if (apdu_info_p != NULL)
    {  /* create command qualifier - Ref. 31.111 section 8.6 */
        //init the value
        apdu_info_p->command_info.CommandQualifier = 0;
        if (apdu_info_p->apdu_data.pc_sum_p->command_details.PreferredSelection == PREFERRED_SELECTION_SOFT_KEY)
        {
            apdu_info_p->command_info.CommandQualifier |= SETUP_MENU_PREFERRED_SELECTION_MASK;
        }
        if (apdu_info_p->apdu_data.pc_sum_p->command_details.HelpInfoAvailable == HELP_INFO_AVAILABLE)
        {
            apdu_info_p->command_info.CommandQualifier |= SETUP_MENU_HELP_INFO_AVAILABLE_MASK;
        }
    }
    /* end create command qualifier */
    return;
}

static void Create_CommandQualifier_PC_TimerManagement(ste_parsed_apdu_t * apdu_info_p)
{
    if (apdu_info_p != NULL)
    {  /* create command qualifier - Ref. 31.111 section 8.6 */
        //init the value
        apdu_info_p->command_info.CommandQualifier = 0;
        if (apdu_info_p->apdu_data.pc_tm_p->command_details.timer_action == TIMER_START)
        {
            apdu_info_p->command_info.CommandQualifier |= TIMER_MANAGEMENT_TIMER_START_MASK;
        }
        else if (apdu_info_p->apdu_data.pc_tm_p->command_details.timer_action == TIMER_DEACTIVATE)
        {
            apdu_info_p->command_info.CommandQualifier |= TIMER_MANAGEMENT_TIMER_DEACTIVATE_MASK;
        }
        else if (apdu_info_p->apdu_data.pc_tm_p->command_details.timer_action == TIMER_GET_CURRENT_VALUE)
        {
            apdu_info_p->command_info.CommandQualifier |= TIMER_MANAGEMENT_TIMER_CURRENT_VALUE_MASK;
        }
        else if (apdu_info_p->apdu_data.pc_tm_p->command_details.timer_action == TIMER_NONE)
        {
            apdu_info_p->command_info.CommandQualifier |= TIMER_MANAGEMENT_TIMER_NONE_MASK;
        }
        else
        {
            //DO Nothing
        }
    }
    /* end create command qualifier */
    return;
}


/*************************************************************************
 * Function:      Create_CommandQualifier_PC_GetInkey
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Create_CommandQualifier_PC_GetInkey(ste_parsed_apdu_t * apdu_info_p)
{
    if (apdu_info_p != NULL)
    {  /* create command qualifier - Ref. 31.111 section 8.6 */
        //init the value
        apdu_info_p->command_info.CommandQualifier = 0;
        if (apdu_info_p->apdu_data.pc_gik_p->command_details.HelpInfoAvailable == TRUE)
        {
            apdu_info_p->command_info.CommandQualifier |= GENERIC_HELP_INFO_AVAILABLE;
        }

        if (apdu_info_p->apdu_data.pc_gik_p->command_details.Mode == STE_APDU_GET_INKEY_IMMEDIATE_RESPONSE)
        {
            apdu_info_p->command_info.CommandQualifier |= GET_INKEY_IMMEDIATE_RESPONSE;
        }

        switch (apdu_info_p->apdu_data.pc_gik_p->command_details.ResponseType)
        {
            case STE_APDU_GET_INKEY_YES_NO:
            {
                apdu_info_p->command_info.CommandQualifier |= GET_INKEY_YES_NO_INPUT;
            }
            break;
            case STE_APDU_GET_INKEY_UCS2_CODED:
            {
                apdu_info_p->command_info.CommandQualifier |= USER_INPUT_UCS2_ALPHABET; //set the bit 2
                apdu_info_p->command_info.CommandQualifier |= USER_INPUT_ALPHABET_SET;  //set the bit 1 as well
            }
            break;
            case STE_APDU_GET_INKEY_ENTIRE_SMS_DEFAULT_ALPHABET:
            {
                apdu_info_p->command_info.CommandQualifier |= USER_INPUT_ALPHABET_SET;  //set the bit 1
            }
            break;
            case STE_APDU_GET_INKEY_DIGITS_AST_HASH_PLUS_ONLY:
            {
                //do nothing, related bit then should be 0.
            }
            break;
            default:
            {
                /* Unsupported Response Type */
                STE_SAT_LOG_ERROR("Create_CommandQualifier_PC_GetInkey: Unsupported Response Type: %d.\n",
                     apdu_info_p->apdu_data.pc_gik_p->command_details.ResponseType);
            }
        }
    }

    return;
}

/*************************************************************************
 * Function:      Create_CommandQualifier_PC_GetInput
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Create_CommandQualifier_PC_GetInput(ste_parsed_apdu_t *
                                                apdu_info_p)
{
    if (apdu_info_p != NULL)
    {  /* create command qualifier - Ref. 31.111 section 8.6 */
        //init the value
        apdu_info_p->command_info.CommandQualifier = 0;
        //TODO: check the combination of different params to see if they are valid
        if (apdu_info_p->apdu_data.pc_gi_p->command_details.HelpInfoAvailable == TRUE)
        {
            apdu_info_p->command_info.CommandQualifier |= GENERIC_HELP_INFO_AVAILABLE;
        }

        if (apdu_info_p->apdu_data.pc_gi_p->command_details.PackFormat == STE_APDU_GET_INPUT_SMS_PACKED)
        {
            apdu_info_p->command_info.CommandQualifier |= GET_INPUT_SM_PACK_USER_INPUT;
        }

        if (apdu_info_p->apdu_data.pc_gi_p->command_details.EchoAllowed == FALSE)
        {
            apdu_info_p->command_info.CommandQualifier |= GET_INPUT_DO_NOT_ECHO_INPUT;
        }

        switch (apdu_info_p->apdu_data.pc_gi_p->command_details.ResponseType)
        {
            case STE_APDU_GET_INPUT_UCS2_CODED:
            {
                apdu_info_p->command_info.CommandQualifier |= USER_INPUT_UCS2_ALPHABET; //set the bit 2
                apdu_info_p->command_info.CommandQualifier |= USER_INPUT_ALPHABET_SET;  //set the bit 1 as well
            }
            break;
            case STE_APDU_GET_INPUT_ENTIRE_SMS_DEFAULT_ALPHABET:
            {
                apdu_info_p->command_info.CommandQualifier |= USER_INPUT_ALPHABET_SET;  //set the bit 1
            }
            break;
            case STE_APDU_GET_INPUT_DIGITS_AST_HASH_PLUS_ONLY:
            {
                //do nothing, related bit then should be 0.
            }
            break;
            default:
            {
                /* Unsupported Response Type */
                STE_SAT_LOG_ERROR("Create_CommandQualifier_PC_GetInput: Unsupported Response Type: %d.\n",
                     apdu_info_p->apdu_data.pc_gi_p->command_details.ResponseType);
            }
        }
    }
    return;
}


/*************************************************************************
 * Function:      ParseProactiveCommand
 *
 * Description:   This method parses the proactive commands and builds up
 *                a structured output from binary data provided by the
 *                USIM.
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
ParseProactiveCommand(const uint8_t * TLV_data_p,
                      uint8_t dataLength,
                      ste_parsed_apdu_t * parsed_apdu_info_p)
{
    uint8_t                   TypeOfCommand;
    uint8_t                   Length;
    uint8_t                   RemainingLength;
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;

    STE_SAT_LOG_INFO("ParseProactiveCommand: Parse PC Command detail.\n");

    /* Command details should be the first SIMPLE-TLV */
    if ((*TLV_data_p & ~COMPREHENSION_REQUIRED_MASK) != COMMAND_DETAILS_TAG) {  /* Impossible to continue since we do not know which command it is - abort with error */
        STE_SAT_LOG_ERROR("ParseProactiveCommand: Unexpected Proactive Command details, Tag: 0x%x \n", *TLV_data_p);
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }
    TLV_data_p++;               /* point to Command details length */

    Length = *TLV_data_p++;

    STE_SAT_LOG_INFO("ParseProactiveCommand: Command Detail length: 0x%x.\n", Length);

    if ((Length < 3) || (dataLength < (Length + 2))) {  /* Unexpected length for command details - abort with error */
        STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }
    /* Copy components of command details into Parsed proactive command */
    parsed_apdu_info_p->command_info.CommandNumber = *TLV_data_p;
    TLV_data_p++;

    /* Command details OK; check TypeOfCommand to continue: */
    TypeOfCommand = *TLV_data_p;
    parsed_apdu_info_p->command_info.CommandType = TypeOfCommand;

    TLV_data_p++;               // point to the command qualifier
    //TODO: check the validity of command qualifier
    parsed_apdu_info_p->command_info.CommandQualifier = *TLV_data_p;

    TLV_data_p++;

    /* TLV_data_p now points to first byte after command details */
    RemainingLength = dataLength - (Length + 2);

    STE_SAT_LOG_INFO("ParseProactiveCommand: Command Type: 0x%x.\n", TypeOfCommand);

    switch (TypeOfCommand) {
    case STE_APDU_CMD_TYPE_DISPLAY_TEXT:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_DISPLAY_TEXT;

            parsed_apdu_info_p->apdu_data.pc_dt_p = (ste_apdu_pc_display_text_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_display_text_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_dt_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_dt_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_DisplayText(TLV_data_p,
                                              RemainingLength,
                                              &(parsed_apdu_info_p->apdu_data.pc_dt_p),
                                              parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length for display text - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO;

            parsed_apdu_info_p->apdu_data.pc_pli_p = (ste_apdu_pc_provide_local_info_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_provide_local_info_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_pli_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_pli_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_ProvideLocalInfo(TLV_data_p,
                                                   RemainingLength,
                                                   &(parsed_apdu_info_p->apdu_data.pc_pli_p),
                                                   parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST;

            parsed_apdu_info_p->apdu_data.pc_sel_p = (ste_apdu_pc_set_up_event_list_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_set_up_event_list_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_sel_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_sel_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_SetupEventList(TLV_data_p,
                                                 RemainingLength,
                                                 &(parsed_apdu_info_p->apdu_data.pc_sel_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_MENU:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SET_UP_MENU;

            parsed_apdu_info_p->apdu_data.pc_sum_p = (ste_apdu_pc_set_up_menu_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_set_up_menu_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_sum_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_sum_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_SetupMenu(TLV_data_p,
                                            RemainingLength,
                                            &(parsed_apdu_info_p->apdu_data.pc_sum_p),
                                            parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_GET_INKEY:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_GET_INKEY;

            parsed_apdu_info_p->apdu_data.pc_gik_p =(ste_apdu_pc_get_inkey_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_get_inkey_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_gik_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_gik_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_GetInkey(TLV_data_p,
                                           RemainingLength,
                                           &(parsed_apdu_info_p->apdu_data.pc_gik_p),
                                           parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_GET_INPUT:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_GET_INPUT;

            parsed_apdu_info_p->apdu_data.pc_gi_p =(ste_apdu_pc_get_input_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_get_input_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_gi_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_gi_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_GetInput(TLV_data_p,
                                           RemainingLength,
                                           &(parsed_apdu_info_p->apdu_data.pc_gi_p),
                                           parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_MORE_TIME:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_MORE_TIME;

            parsed_apdu_info_p->apdu_data.pc_mt_p =(ste_apdu_pc_more_time_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_more_time_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_mt_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_mt_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_MoreTime(TLV_data_p,
                                           RemainingLength,
                                           &(parsed_apdu_info_p->apdu_data.pc_mt_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_POLL_INTERVAL:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_POLL_INTERVAL;

            parsed_apdu_info_p->apdu_data.pc_pi_p =(ste_apdu_pc_poll_interval_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_poll_interval_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_pi_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_pi_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_PollInterval(TLV_data_p,
                                               RemainingLength,
                                               &(parsed_apdu_info_p->apdu_data.pc_pi_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_POLLING_OFF:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_POLLING_OFF;

            parsed_apdu_info_p->apdu_data.pc_po_p = (ste_apdu_pc_polling_off_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_polling_off_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_po_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_po_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_PollingOff(TLV_data_p,
                                             RemainingLength,
                                             &(parsed_apdu_info_p->apdu_data.pc_po_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_TIMER_MANAGEMENT:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_TIMER_MANAGEMENT;

            parsed_apdu_info_p->apdu_data.pc_tm_p = (ste_apdu_pc_timer_management_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_timer_management_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_tm_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_tm_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_TimerManagement(TLV_data_p,
                                                  RemainingLength,
                                                  &(parsed_apdu_info_p->apdu_data.pc_tm_p),
                                                  parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_REFRESH:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_REFRESH;

            parsed_apdu_info_p->apdu_data.pc_r_p = (ste_apdu_pc_refresh_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_refresh_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_r_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_r_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_Refresh(TLV_data_p,
                                          RemainingLength,
                                          &(parsed_apdu_info_p->apdu_data.pc_r_p),
                                          parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_SET_UP_CALL:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SET_UP_CALL;

            parsed_apdu_info_p->apdu_data.pc_sc_p = (ste_apdu_pc_setup_call_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_setup_call_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_sc_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_sc_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_SetupCall(TLV_data_p,
                                            RemainingLength,
                                            &(parsed_apdu_info_p->apdu_data.pc_sc_p),
                                            parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_SEND_SS:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SEND_SS;

            parsed_apdu_info_p->apdu_data.pc_ss_p = (ste_apdu_pc_send_ss_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_send_ss_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_ss_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_ss_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_SendSS(TLV_data_p,
                                         RemainingLength,
                                         &(parsed_apdu_info_p->apdu_data.pc_ss_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_SEND_USSD:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SEND_USSD;

            parsed_apdu_info_p->apdu_data.pc_ussd_p = (ste_apdu_pc_send_ussd_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_send_ussd_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_ussd_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_ussd_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_SendUSSD(TLV_data_p,
                                           RemainingLength,
                                           &(parsed_apdu_info_p->apdu_data.pc_ussd_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE;

            parsed_apdu_info_p->apdu_data.pc_ssm_p = (ste_apdu_pc_send_short_message_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_send_short_message_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_ssm_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_ssm_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_SendShortMessage(TLV_data_p,
                                                   RemainingLength,
                                                   &(parsed_apdu_info_p->apdu_data.pc_ssm_p),
                                                   parsed_apdu_info_p->command_info.CommandQualifier);
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_SEND_DTMF:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SEND_DTMF;

            parsed_apdu_info_p->apdu_data.pc_dtmf_p = (ste_apdu_pc_send_dtmf_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_send_dtmf_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_dtmf_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_dtmf_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_SendDTMF(TLV_data_p,
                                           RemainingLength,
                                           &(parsed_apdu_info_p->apdu_data.pc_dtmf_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION:
        {
            //set to internal type ID for outside user
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION;

            parsed_apdu_info_p->apdu_data.pc_ln_p =(ste_apdu_pc_language_notification_t *)
                STE_SAT_MEM_ALLOCATE(sizeof(ste_apdu_pc_language_notification_t));

            if (NULL == parsed_apdu_info_p->apdu_data.pc_ln_p) {
                STE_SAT_LOG_ERROR("ParseProactiveCommand: memory allocation failed.\n");
                return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
            }
            parsed_apdu_info_p->apdu_data.pc_ln_p->bit_mask = 0;
            if (RemainingLength >= 2) {
                RetVal = Parse_PC_LanguageNotification(TLV_data_p,
                                                       RemainingLength,
                                                       &(parsed_apdu_info_p->apdu_data.pc_ln_p));
            } else {            /* Unexpected length - abort with error */
                STE_SAT_LOG_ERROR("ParseProactiveCommand: Proactive Command length error\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_LAUNCH_BROWSER:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_LAUNCH_BROWSER;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_LAUNCH_BROWSER unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_PLAY_TONE:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_PLAY_TONE;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_PLAY_TONE unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_SELECT_ITEM:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SELECT_ITEM;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_SELECT_ITEM unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_SET_UP_IDLE_MODE_TEXT:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SET_UP_IDLE_MODE_TEXT;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_SET_UP_IDLE_MODE_TEXT unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_RUN_AT:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_RUN_AT;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_RUN_AT unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_OPEN_CHANNEL:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_OPEN_CHANNEL;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_OPEN_CHANNEL unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_CLOSE_CHANNEL:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_CLOSE_CHANNEL;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_CLOSE_CHANNEL unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_RECEIVE_DATA:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_RECEIVE_DATA;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_RECEIVE_DATA unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_SEND_DATA:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SEND_DATA;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_SEND_DATA unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_GET_CHANNEL_STATUS:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_GET_CHANNEL_STATUS;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_GET_CHANNEL_STATUS unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_SERVICE_SEARCH:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_SERVICE_SEARCH;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_SERVICE_SEARCH unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_GET_SERVICE_INFOMATION:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_GET_SERVICE_INFOMATION;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_GET_SERVICE_INFOMATION unimplemented\n");
        }
        break;

    case STE_APDU_CMD_TYPE_DECLARE_SERVICE:
        {
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_DECLARE_SERVICE;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: STE_APDU_CMD_TYPE_DECLARE_SERVICE unimplemented\n");
        }
        break;

    default:
        {
            /* Unsupported Proactive Command */
            parsed_apdu_info_p->type_id = STE_APDU_CMD_TYPE_NONE;
            STE_SAT_LOG_ERROR("ParseProactiveCommand: Unsupported proactive command\n");
            RetVal = STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
        }
    }                           /* end switch */

    return (RetVal);
}

/*************************************************************************
 * Function:      CreateCommandDetailsTLV
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateCommandDetailsTLV(ste_command_info_t * CommandInfo_p,
                        uint8_t * CommandDetailsTLV_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    /* Set up command details TLV */
    *CommandDetailsTLV_p++ = COMMAND_DETAILS_TAG | COMPREHENSION_REQUIRED_MASK;
    *CommandDetailsTLV_p++ = COMMAND_DETAILS_LENGTH;
    *CommandDetailsTLV_p++ = CommandInfo_p->CommandNumber;
    *CommandDetailsTLV_p++ = CommandInfo_p->CommandType;
    *CommandDetailsTLV_p = CommandInfo_p->CommandQualifier;

    return (RetVal);
}

/*************************************************************************
 * Function:      CreateDeviceIdentitiesTLV
 *
 * Description:   Builds the Device Identities simple TLV
 *
 * Input Params:  SourceDevice      Source device ID
 *                DestDevice        Destination device ID
 *                CompReq           Comprehension Required
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateDeviceIdentitiesTLV(uint8_t   SourceDevice,
                          uint8_t   DestDevice,
                          boolean   CompReq,
                          uint8_t * Dest_p)
{
    uint8_t                   Tag = DEVICE_IDENTITIES_TAG;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }

    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */
    *Dest_p++ = DEVICE_IDENTITIES_LENGTH;       /* LENGTH */
    *Dest_p++ = SourceDevice;   /* VALUE */
    *Dest_p = DestDevice;

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateItemIdentifierTLV
 *
 * Description:
 *
 * Input Params:
 *
 *
 *
 * Output Params:
 *
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateTimerIdentifierTLV(ste_apdu_timer_identifier_code_value_t timer_code,
                         boolean CompReq,
                         uint8_t * Dest_p)
{
    uint8_t     Tag = TIMER_IDENTIFIER_TAG;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */

    *Dest_p++ = TIMER_IDENTIFIER_LENGTH;        /* LENGTH */
    *Dest_p = timer_code;

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateItemValueTLV
 *
 * Description:
 *
 * Input Params:
 *
 *
 *
 * Output Params:
 *
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateTimerValueTLV(uint32_t timer_value, boolean CompReq, uint8_t * Dest_p)
{
    uint8_t                   Tag = TIMER_VALUE_TAG;
    uint8_t                   TempTimerVal = 0;

    TempTimerVal = timer_value;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */

    *Dest_p++ = TIMER_VALUE_LENGTH;     /* LENGTH */
    *Dest_p++ = TempTimerVal / 60 * 60;

    TempTimerVal = timer_value % (60 * 60);

    *Dest_p++ = TempTimerVal;

    TempTimerVal = TempTimerVal % 60;
    *Dest_p = TempTimerVal;


    return STE_SAT_APDU_ERROR_NONE;
}


/*************************************************************************
 * Function:      CreateIconIdentifierTLV
 *
 * Description:   Builds the Icon Identifier simple TLV
 *
 * Input Params:  IconIdentifier    Icon Identifier ID
 *                IconQualifier     Icon Qualifier
 *                CompReq           Comprehension Required
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateIconIdentifierTLV(const uint8_t IconIdentifier,
                        const uint8_t IconQualifier,
                        const boolean CompReq,
                        uint8_t * Dest_p)
{
    uint8_t                   Tag = ICON_IDENTIFIER_TAG;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */

    *Dest_p++ = ICON_IDENTITIES_LENGTH; /* LENGTH */
    *Dest_p++ = IconQualifier;  /* VALUE */
    *Dest_p = IconIdentifier;

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateResponseLengthTLV
 *
 * Description:   Builds the Response Length simple TLV
 *
 * Input Params:  RespLen_p         Response Length structure
 *                CompReq           Comprehension Required
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateResponseLengthTLV(ste_apdu_response_length_t * RespLen_p,
                        const boolean CompReq,
                        uint8_t * Dest_p)
{
    uint8_t                   Tag = RESPONSE_LENGTH_TAG;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */

    *Dest_p++ = RESPONSE_LENGTH_LENGTH; /* LENGTH */
    *Dest_p++ = RespLen_p->min_length;  /* VALUE */
    *Dest_p = RespLen_p->max_length;

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateImmediateResponseTLV
 *
 * Description:   Builds the immediate response simple TLV
 *
 * Input Params:  CompReq           Comprehension Required
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateImmediateResponseTLV(const boolean CompReq, uint8_t * Dest_p)
{
    uint8_t  Tag = IMMEDIATE_RESPONSE_TAG;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */

    *Dest_p = 0x00;             /* LENGTH */
    /* NO VALUE for this specific TLV */

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateAccessTechnologyTLV
 *
 * Description:   Builds the access technology simple TLV
 *
 * Input Params:  type              RAT type
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateAccessTechnologyTLV(cn_cell_rat_t type,
                          uint8_t * Dest_p)
{
    *Dest_p++ = ACCESS_TECHNOLOGY_TAG; /* TAG */
    *Dest_p++ = ACCESS_TECHNOLOGY_LENGTH; /* LENGTH */

    switch (type)
    {
        case CN_CELL_RAT_GSM:
        {
            *Dest_p = 0x0; /* VALUE */
        }
        break;
        case CN_CELL_RAT_WCDMA:
        {
            *Dest_p = 0x03; /* VALUE */
        }
        break;
        case CN_CELL_RAT_UNKNOWN:
        case CN_CELL_RAT_EPS:
        default:
        {
            STE_SAT_LOG_ERROR("CreateAccessTechnologyTLV: unexpected rat:%d.\n", type);
            return STE_SAT_APDU_ERROR_UNKNOWN;
        }
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateBatteryTLV
 *
 * Description:   Builds the battery simple TLV
 *
 * Input Params:  State             Batery charge state
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateBatteryTLV(uint8_t State,
                 uint8_t * Dest_p)
{
    *Dest_p++ = BATTERY_TAG;           /* TAG */
    *Dest_p++ = BATTERY_LENGTH;        /* LENGTH */
    *Dest_p = State;                   /* VALUE */

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateLocationTLV
 *
 * Description:   Builds the location simple TLV
 *
 * Input Params:  State             Batery charge state
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateLocationTLV(ste_apdu_location_info_t *location_info_p,
                 uint8_t * Dest_p)
{
    if (location_info_p == NULL) {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    /* TAG */
    *Dest_p++ = LOCATION_INFORMATION_TAG;

    /* LENGTH */
    if (location_info_p->cell_info.UseExtended) {
        *Dest_p++ = LOCATION_EXTENDED_LENGTH;
    } else {
        *Dest_p++ = LOCATION_LENGTH;
    }

    /* MCC & MNC */
    memcpy(Dest_p, &(location_info_p->net_info.PLMN), sizeof(location_info_p->net_info.PLMN));
    Dest_p += sizeof(location_info_p->net_info.PLMN);

    /* LAC */
    memcpy(Dest_p, &(location_info_p->net_info.LAC), sizeof(location_info_p->net_info.LAC));
    Dest_p += sizeof(location_info_p->net_info.LAC);

    /* Cell id */
    *Dest_p++ = (location_info_p->cell_info.cid >> 8) & 0xFF; //high byte
    *Dest_p++ = location_info_p->cell_info.cid & 0xFF; //low byte
    if (location_info_p->cell_info.UseExtended) {
        *Dest_p++ = (location_info_p->cell_info.cid >> 24) & 0xFF; //ext high byte
        *Dest_p = (location_info_p->cell_info.cid >> 16) & 0xFF; //ext low byte
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/**
 * Writes a Network search mode TLV to specified buffer.
 *
 * This function assumes that there is enough space (3 bytes) for the TLV in the buffer.
 *
 * @param   sm  Networks search mode as defined by ETSI TS 102.223v7.13 section 8.75
 *              No validity check are made on this vaule.
 *
 * @param   cr  Controls if the comprehension required bit in the tag should be set or not.
 *              The comprehension required bit will be set if cr evaluates to true.
 *
 * @param   dst Pointer to the buffer where the TLV should be written.
 *
 * @return  STE_SAT_APDU_ERROR_NONE (always)
 */
static ste_sat_apdu_error_t CreateSearchModeTLV(uint8_t sm, boolean cr, uint8_t* dst)
{
    *dst++ = ((cr) ? 0x80 : 0x00) | NETWORK_SEARCH_MODE_TAG; // Comprehension required & tag
    *dst++ = SEARCH_MODE_LENGTH;  // Length of remaining data, always 1 byte
    *dst   = sm;
    return STE_SAT_APDU_ERROR_NONE;
}

/**
 * Writes a Timing Advance TLV to specified buffer.
 *
 * This function assumes that there is enough space (4 bytes) for the TLV in the buffer.
 *
 * Note: this function does not check that the arguments passed are valid.
 *
 * @param   status  The status of the timing advance value (e.g. if it is measured for/on
 *                  a current active channel or if it is an old value).
 *
 * @param   value   The timing advance value.
 *
 * @param   cr  Controls if the comprehension required bit in the tag should be set or not.
 *              The comprehension required bit will be set if cr evaluates to true.
 *
 * @param   dst Pointer to the buffer where the TLV should be written.
 *
 * @return  STE_SAT_APDU_ERROR_NONE (always)
 */
static ste_sat_apdu_error_t CreateTimingAdvanceTLV(uint8_t status, uint8_t value, boolean cr, uint8_t* dst)
{
    *dst++ = ((cr) ? 0x80 : 0x00) | TIMING_ADVANCE_TAG; // Comprehension required & tag
    *dst++ = TIMING_ADVANCE_LENGTH;  // Length of remaining data
    *dst++ = status;
    *dst   = value;
    return STE_SAT_APDU_ERROR_NONE;
}


/*************************************************************************
 * Function:      CreateNMRTLV
 *
 * Description:   Builds the nmr TLV
 *
 * Input Params:  nmr_geran_result      Pointer to measurement result geran
 *                num_of_channels       Number of BCCH channels: 0 to 32
 *                bcch_list             An array of bcch channels
 *                utran_report_length   Measurement report length utran
 *                nmr_utran_report      Pointer to measurement report utran
 *
 * Output Params: Desp_p                Pointer to byte array to which TLV will
 *                                      be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t CreateNMRTLV(uint8_t* nmr_geran_result,
                                         uint8_t num_of_channels,
                                         uint16_t* bcch_list,
                                         uint8_t utran_report_length,
                                         uint8_t* nmr_utran_report,
                                         uint8_t nmr_rat_type,
                                         uint8_t* length,
                                         uint8_t* Dest_p)
{
    int i, len = 0;
    *length = 0;
    *Dest_p++ = NMR_TAG | COMPREHENSION_REQUIRED_MASK;

    if (nmr_rat_type == CN_NMR_RAT_TYPE_GERAN) {
        bit_buffer_t bcch;

        *Dest_p++ = LOCATION_NMR_GERAN_SIZE;

        //Fill in the Network measurement result for GERAN
        for (i=0; i<LOCATION_NMR_GERAN_SIZE; i++) {
            *Dest_p++ = nmr_geran_result[i];
        }

        /* Length of whole TLV so far */
        *length += LOCATION_NMR_GERAN_SIZE_TOTAL;

        // BCCH channels!
        *Dest_p++ = BCCH_CHANNEL_LIST_TAG | COMPREHENSION_REQUIRED_MASK;

        /*
         * Pack BCCH list into a bit string
         */
        bit_buffer_init(&bcch, num_of_channels * 10); // Need space for 10 bits per channel.
        assert(bcch.size); // Allocation must have succeded!
        for (i = 0; i < num_of_channels; i++) {
            bit_buffer_append_bits(&bcch, (unsigned)bcch_list[i], 10);
        }

        len = bit_buffer_length(&bcch);

        /* Length of BCCH list value field in TLV */
        *Dest_p++ = len;

        /*
         * Copy packed BCCH channel list
         */
        for (i = 0; i < len; i++) {
            *Dest_p++ = bcch.storage[i];
        }
        bit_buffer_deinit(&bcch);

        *length += 2+len; //Length of channels.

    } else if (CN_NMR_RAT_TYPE_UTRAN) {
        *Dest_p++ = utran_report_length;

        //Fill in the Network measurement report for UTRAN
        for (i=0; i<utran_report_length; i++) {
            *Dest_p++ = nmr_utran_report[i];
        }

        /* Length of whole TLV */
        *length = 2+utran_report_length;
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateDurationTLV
 *
 * Description:   Builds the duration simple TLV
 *
 * Input Params:  Duration          Duration, time unit: tens of second
 *                CompReq           Comprehension Required
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateDurationTLV(const size_t Duration,
                  const boolean CompReq,
                  uint8_t * Dest_p)
{
    uint8_t       Tag = DURATION_TAG;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */
    *Dest_p++ = DURATION_LENGTH;        /* LENGTH */

    //calculate the duration to put the best time unit into the TLV
    if (Duration % 600 == 0) {
        //exactly minute, set time unit to be MINUTE
        *Dest_p++ = DURATION_TIME_UNIT_MINUTE;
        *Dest_p = Duration / 600;       /* VALUE */
    } else if (Duration % 10 == 0) {
        //exactly second, set time unit to be SECOND
        *Dest_p++ = DURATION_TIME_UNIT_SECOND;
        *Dest_p = Duration / 10;        /* VALUE */
    } else {
        //only tens of second is possible, set time unit to be TENS_OF_SECOND
        *Dest_p++ = DURATION_TIME_UNIT_TENS_OF_SECOND;
        *Dest_p = Duration;     /* VALUE */
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateTimerManagementTLV
 *
 * Description:   Builds the Timer Management simple TLV.
 *
 * Input Params:  hours     Duration time before timer expire.
 *                minutes   Duration time before timer expire.
 *                seconds   Duration time before timer expire.
 *                CompReq   Comprehension Required
 *
 * Output Params: Dest_p    Pointer to byte array to which TLV will
 *                          be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t CreateTimerManagementTLV(const uint8_t hours,
                                                     const uint8_t minutes,
                                                     const uint8_t seconds,
                                                     const boolean CompReq,
                                                     uint8_t     * Dest_p)
{
    uint8_t       Tag = TIMER_VALUE_TAG;

    if (CompReq) {              /* Comprehension Required. */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* End Comprehension Required. */
    *Dest_p++ = Tag;                       /* TAG */
    *Dest_p++ = TIMER_VALUE_LENGTH;        /* LENGTH */

    *Dest_p++ = hours;
    *Dest_p++ = minutes;
    *Dest_p++ = seconds;

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateFrameIdentifierTLV
 *
 * Description:   Builds the FRAME IDENTIFIER simple TLV
 *
 * Input Params:  FrameIdentifier   Frame identifier
 *                CompReq           Comprehension Required
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static  ste_sat_apdu_error_t
CreateFrameIdentifierTLV(uint8_t FrameIdentifier,
                         const boolean CompReq,
                         uint8_t * Dest_p)
{
    uint8_t     Tag = FRAME_IDENTIFIER_TAG;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */
    *Dest_p++ = FRAME_IDENTIFIER_LENGTH;        /* LENGTH */
    *Dest_p = FrameIdentifier;  /* VALUE */

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateTextStringTLV
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateTextStringTLV(uint8_t * source_string_p,
                    uint8_t string_length,
                    uint8_t CodingScheme,
                    const boolean CompReq,
                    uint8_t * TLV_buffer_p,
                    uint8_t * TotalLen_p)
{
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                   LengthLen = 0;
    uint8_t                   Tag = TEXT_STRING_TAG;      /* TAG (without comprehension required flag) */

    if (string_length == 0 || source_string_p == NULL) {
        STE_SAT_LOG_INFO("CreateTextStringTLV: PC DisplayText string Length = 0.\n");
        *TotalLen_p = 0;
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //set the TAG value
    //check if comprehension required flag
    if (CompReq) {
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    *TLV_buffer_p++ = Tag;      //set the tag and move to next byte
    *TotalLen_p = 1;

    //check if it is a two byte length or one byte length, the total length = The string length + coding scheme(1 byte)
    LengthLen = BuildLengthTLV(string_length + 1, TLV_buffer_p);
    *TotalLen_p += LengthLen;
    TLV_buffer_p += LengthLen;  //move to the first byte after Length of TextString TLV

    // TODO: convert Text.Coding (in TextCoding_t format) into SMS coding scheme ???
    //set the coding scheme byte and move to next byte
    *TLV_buffer_p++ = CodingScheme;
    (*TotalLen_p)++;            //add 1 byte for coding scheme

    //copy the text string
    if (string_length > 0) {
        STE_SAT_MEM_COPY(TLV_buffer_p, source_string_p, string_length);
        *TotalLen_p += string_length;
    }

    return (RetVal);
}

/*************************************************************************
 * Function:      CreateTextAttributeTLV
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateTextAttributeTLV(uint8_t * source_string_p,
                       uint8_t string_length,
                       const boolean CompReq,
                       uint8_t * TLV_buffer_p,
                       uint8_t * TotalLen_p)
{
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                   LengthLen = 0;
    uint8_t                   Tag = TEXT_ATTRIBUTE_TAG;   /* TAG (without comprehension required flag) */

    //set the TAG value
    //check if comprehension required flag
    if (CompReq) {
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    *TLV_buffer_p++ = Tag;      //set the tag and move to next byte
    *TotalLen_p = 1;

    if (string_length == 0 || source_string_p == NULL) {
        STE_SAT_LOG_INFO("CreateTextAttributeTLV: PC DisplayText Text Attribute Length = 0.\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }
    //check if it is a two byte length or one byte length
    LengthLen = BuildLengthTLV(string_length, TLV_buffer_p);
    *TotalLen_p += LengthLen;
    TLV_buffer_p += LengthLen;  //move to the first byte after Length of TextString TLV

    //copy the text string
    if (string_length > 0) {
        STE_SAT_MEM_COPY(TLV_buffer_p, source_string_p, string_length);
        *TotalLen_p += string_length;
    }

    return (RetVal);
}

/*************************************************************************
 * Function:      CreateAlphaIdentifierTLV
 *
 * Description:   This method creates an alpha identifer TLV data.
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateAlphaIdentifierTLV(uint8_t * source_string_p,
                         uint8_t string_length,
                         const boolean CompReq,
                         uint8_t * TLV_buffer_p,
                         uint8_t * TotalLen_p)
{
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                   LengthLen = 0;
    uint8_t                   Tag = ALPHA_IDENTIFIER_TAG;   /* TAG (without comprehension required flag) */

    //set the TAG value
    //check if comprehension required flag
    if (CompReq) {
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    *TLV_buffer_p++ = Tag;      //set the tag and move to next byte
    *TotalLen_p = 1;

    if (string_length == 0 || source_string_p == NULL) {
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }
    //check if it is a two byte length or one byte length
    LengthLen = BuildLengthTLV(string_length, TLV_buffer_p);
    *TotalLen_p += LengthLen;
    TLV_buffer_p += LengthLen;  //move to the first byte after Length of TextString TLV

    //copy the text string
    if (string_length > 0) {
        STE_SAT_MEM_COPY(TLV_buffer_p, source_string_p, string_length);
        *TotalLen_p += string_length;
    }

    return (RetVal);
}



/*************************************************************************
 * Function:      CreateItemListTLV
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateItemListTLV(uint8_t * source_string_p,
                  const boolean CompReq,
                  uint8_t * TLV_buffer_p,
                  uint8_t * TotalLen_p)
{
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                   LengthLen = 0;
    uint8_t                   StringLen = 0;
    uint8_t                   Tag = ITEM_TAG;   /* TAG (without comprehension required flag) */

    //set the TAG value
    //check if comprehension required flag
    if (CompReq) {
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    *TLV_buffer_p++ = Tag;      //set the tag and move to next byte
    *TotalLen_p = 1;

    if (source_string_p == NULL) {
        STE_SAT_LOG_INFO("CreateAlphaIdentifierTLV: PC SetUpMenu Text Attribute Length = 0.\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    StringLen = (uint8_t) strlen((char *) source_string_p);
    //check if it is a two byte length or one byte length
    LengthLen = BuildLengthTLV(StringLen, TLV_buffer_p);
    *TotalLen_p += LengthLen;
    TLV_buffer_p += LengthLen;  //move to the first byte after
    //Length of TextString TLV

    //copy the text string
    if (StringLen > 0) {
        STE_SAT_MEM_COPY(TLV_buffer_p, source_string_p, StringLen);
        *TotalLen_p += StringLen;
    }

    return (RetVal);
}

/*************************************************************************
 * Function:      CreateNextActionTLV
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateNextActionTLV(uint8_t * source_string_p,
                    uint8_t string_length,
                    const boolean CompReq,
                    uint8_t * TLV_buffer_p,
                    uint8_t * TotalLen_p)
{
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                   LengthLen = 0;
    uint8_t                   Tag = ITEM_NEXT_ACTION_IND_TAG;   /* TAG (without comprehension required flag) */

    //set the TAG value
    //check if comprehension required flag
    if (CompReq) {
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    *TLV_buffer_p++ = Tag;      //set the tag and move to next byte
    *TotalLen_p = 1;

    if (string_length == 0 || source_string_p == NULL) {
        STE_SAT_LOG_INFO ("CreateNextActionTLV: PC SetUpMenu Text Attribute Length = 0.\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }
    //check if it is a two byte length or one byte length
    LengthLen = BuildLengthTLV(string_length, TLV_buffer_p);
    *TotalLen_p += LengthLen;
    TLV_buffer_p += LengthLen;  //move to the first byte after Length of TextString TLV

    //copy the text string
    if (string_length > 0) {
        STE_SAT_MEM_COPY(TLV_buffer_p, source_string_p, string_length);
        *TotalLen_p += string_length;
    }

    return (RetVal);
}


/*************************************************************************
 * Function:      CreateIMEI_TLV
 *
 * Description:   Builds the IMEI simple TLV
 *
 * Input Params:  IMEI_p            pointer to IMEI structure
 *
 * Output Params: IMEI_Result_p     Pointer to byte array to which TLV will
 *                                  be added.
 *                IMEI_Length_p     The length of the result
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateIMEI_TLV(const IMEI_t * IMEI_p,
               uint8_t * IMEI_Result_p,
               uint8_t * IMEI_Length_p)
{
    *IMEI_Result_p++ = IMEI_TAG;
    *IMEI_Result_p++ = IMEI_p->Length;
    *IMEI_Result_p++ = (uint8_t) ((IMEI_p->Digit[0] << 4) | ((0x0f & (IMEI_p->OddEvenIndication << 3)) | IMEI_p->TypeOfIdentity));
    *IMEI_Result_p++ =
        (uint8_t) ((IMEI_p->Digit[2] << 4) | (0x0f & IMEI_p->Digit[1]));
    *IMEI_Result_p++ =
        (uint8_t) ((IMEI_p->Digit[4] << 4) | (0x0f & IMEI_p->Digit[3]));
    *IMEI_Result_p++ =
        (uint8_t) ((IMEI_p->Digit[6] << 4) | (0x0f & IMEI_p->Digit[5]));
    *IMEI_Result_p++ =
        (uint8_t) ((IMEI_p->Digit[8] << 4) | (0x0f & IMEI_p->Digit[7]));
    *IMEI_Result_p++ =
        (uint8_t) ((IMEI_p->Digit[10] << 4) | (0x0f & IMEI_p->Digit[9]));
    *IMEI_Result_p++ =
        (uint8_t) ((IMEI_p->Digit[12] << 4) | (0x0f & IMEI_p->Digit[11]));
    *IMEI_Result_p++ =
        (uint8_t) ((0x00 << 4) | (0x0f & IMEI_p->Digit[13])); /*Last digit = spare digit always 0 if transmitted from ME*/
    *IMEI_Length_p = (IMEI_p->Length + 2);      /* Add tag and length bytes */

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateIMEISV_TLV
 *
 * Description:   Builds the IMEISV simple TLV
 *
 * Input Params:  IMEISV_p            pointer to IMEISV structure
 *
 * Output Params: IMEISV_Result_p     Pointer to byte array to which TLV will
 *                                    be added.
 *                IMEISV_Length_p     The length of the result
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t CreateIMEISV_TLV(const IMEISV_t * IMEISV_p,
                                             uint8_t        * IMEISV_Result_p,
                                             uint8_t        * IMEISV_Length_p)
{
    *IMEISV_Result_p++ = IMEISV_TAG;
    *IMEISV_Result_p++ = IMEISV_p->Length;
    *IMEISV_Result_p++ = (uint8_t) ((IMEISV_p->Digit[0] << 4) | ((0x0f & (IMEISV_p->OddEvenIndication << 3)) | IMEISV_p->TypeOfIdentity));
    *IMEISV_Result_p++ =
        (uint8_t) ((IMEISV_p->Digit[2] << 4) | (0x0f & IMEISV_p->Digit[1]));
    *IMEISV_Result_p++ =
        (uint8_t) ((IMEISV_p->Digit[4] << 4) | (0x0f & IMEISV_p->Digit[3]));
    *IMEISV_Result_p++ =
        (uint8_t) ((IMEISV_p->Digit[6] << 4) | (0x0f & IMEISV_p->Digit[5]));
    *IMEISV_Result_p++ =
        (uint8_t) ((IMEISV_p->Digit[8] << 4) | (0x0f & IMEISV_p->Digit[7]));
    *IMEISV_Result_p++ =
        (uint8_t) ((IMEISV_p->Digit[10] << 4) | (0x0f & IMEISV_p->Digit[9]));
    *IMEISV_Result_p++ =
        (uint8_t) ((IMEISV_p->Digit[12] << 4) | (0x0f & IMEISV_p->Digit[11]));
    *IMEISV_Result_p++ =
        (uint8_t) ((IMEISV_p->SVN[0] << 4) | (0x0f & IMEISV_p->Digit[13]));
    *IMEISV_Result_p++ =
        (uint8_t) ((0x00 << 4) | (IMEISV_p->SVN[1] & 0x0f)); /*Dummy last byte, but spec says it should be zero*/
    *IMEISV_Length_p = (IMEISV_p->Length + 2);      /* Add tag and length bytes */

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateDateTimeTimezone_TLV
 *
 * Description:   Builds the Date,Time and Timezone simple TLV
 *
 * Input Params:  datetime              pointer to date,time and timezone structure
 *
 * Output Params: DateTime_Result_p     Pointer to byte array to which TLV will
 *                                      be added.
 *                DateTime_Length_p     The length of the result
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t CreateDateTimeTimezone_TLV(const ste_apdu_date_time_t * datetime,
                                                       uint8_t                    * DateTime_Result_p,
                                                       uint8_t                    * DateTime_Length_p)
{
    *DateTime_Result_p++ = DATE_TIME_ZONE_TAG | COMPREHENSION_REQUIRED_MASK;
    *DateTime_Result_p++ = LOCATION_DATE_TIME_TIMEZONE_LENGTH;
    *DateTime_Result_p++ = (uint8_t) (((datetime->date.year & 0x0f) << 4) | ((datetime->date.year >> 4) & 0x0f));
    *DateTime_Result_p++ = (uint8_t) (((datetime->date.month & 0x0f) << 4) | ((datetime->date.month >> 4) & 0x0f));
    *DateTime_Result_p++ = (uint8_t) (((datetime->date.day & 0x0f) << 4) | ((datetime->date.day >> 4) & 0x0f));
    *DateTime_Result_p++ = (uint8_t) (((datetime->time.hours & 0x0f) << 4) | ((datetime->time.hours >> 4) & 0x0f));
    *DateTime_Result_p++ = (uint8_t) (((datetime->time.minutes & 0x0f) << 4) | ((datetime->time.minutes >> 4) & 0x0f));
    *DateTime_Result_p++ = (uint8_t) (((datetime->time.seconds & 0x0f) << 4) | ((datetime->time.seconds >> 4) & 0x0f));

    //Time zone, should be FF if timezone unknown
    if (datetime->timezone.timezone >= -12 && datetime->timezone.timezone <=13) {
        *DateTime_Result_p++ = (uint8_t) ((0xff & (datetime->timezone.daylight << 6)) | datetime->timezone.timezone);
    } else {
        *DateTime_Result_p++ = 0xFF; //Uknown timezone!
    }

    *DateTime_Length_p = LOCATION_DATE_TIME_TIMEZONE_TOTAL;

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateDeviceIdentitiesTLV
 *
 * Description:   Builds the Device Identities simple TLV
 *
 * Input Params:  SourceDevice      Source device ID
 *                DestDevice        Destination device ID
 *                CompReq           Comprehension Required
 *
 * Output Params: Desp_p            Pointer to byte array to which TLV will
 *                                  be added.
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateEventListTLV(unsigned int EventList,
                   boolean CompReq,
                   uint8_t * Dest_p,
                   uint8_t * TotalLen_p)
{
    uint8_t                   Tag = EVENT_LIST_TAG;
    uint8_t                   TempEvent[STE_SAT_SET_UP_EVENT_LIST_END];
    uint8_t                   EventListLen = 0;
    uint8_t                   LengthLen = 0;
    uint8_t                   i;

    if (CompReq) {              /* Comprehension Required */
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */
    *Dest_p++ = Tag;            /* TAG */

    *TotalLen_p = 1;

    if (EventList == 0) {
        STE_SAT_LOG_INFO("CreateEventListTLV: PC Set Up Event List to create an empty event list.\n");
        *Dest_p = 0;            //set the length of event list to 0 then return
        *TotalLen_p += 1;       // to add the length of 'Length' byte
        return STE_SAT_APDU_ERROR_NONE;
    }
    //there is something in the event list, count the event
    //NOTE: For now, since the maximum number of event is STE_SAT_SET_UP_EVENT_LIST_END, and this value is below MAX_LENGTH_BYTE1
    //we can say the byte number for 'L' in TLV is one byte, but still we calculate it according to the rule
    memset(TempEvent, 0, STE_SAT_SET_UP_EVENT_LIST_END);

    for (i = 0; i < STE_SAT_SET_UP_EVENT_LIST_END; i++) {
        if (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED & EventList) {
            //found the bit set to 1, means there is an event represented by this bit, save the value to the temp array first
            TempEvent[EventListLen] = i;

            EventListLen++;
        }
        //shift the event list value for one bit
        EventList = EventList >> 1;
    }


    //check if it is a two byte length or one byte length, create the value
    LengthLen = BuildLengthTLV(EventListLen, Dest_p);
    *TotalLen_p += LengthLen;
    Dest_p += LengthLen;        //move to the first byte after Length of Event List TLV

    //copy the real event list from the temp array
    if (EventListLen > 0) {
        STE_SAT_MEM_COPY(Dest_p, TempEvent, EventListLen);
        *TotalLen_p += EventListLen;
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateDialStringTLV
 *
 * Description:   Converts dial string from call control requested action
 *                (call, SS or USSD) into the appropriate TLV format.
 *
 * Input Params:  DialString_p        Call control requested action dial string.
 *
 * Output Params: Dest_p              TLV format for dial string.
 *
 * Return:        Total TLV size
 *
 * Notes:
 *
 *************************************************************************/
static uint8_t CreateDialStringTLV(ste_apdu_dial_string_t * DialString_p,
                                   uint8_t                * Dest_p)
{
    uint8_t tlv_len = 0;
    uint8_t Tag = DUMMY_TAG;

    switch (DialString_p->cc_type)
    {
      case STE_APDU_CC_CALL_SETUP:
        Tag = ADDRESS_TAG;        /* Format Address TLV */
        break;
      case STE_APDU_CC_SS:
        Tag = SS_STRING_TAG;      /* Format SS String TLV */
        break;
      case STE_APDU_CC_USSD:
        Tag = USSD_STRING_TAG;    /* Format USSD String TLV */
        break;
      case STE_APDU_CC_PDP:
        Tag = PDP_CONTEXT_ACTIVATION_TAG;    /* Format PDP CONTEXT String TLV */
        break;
      default:
        STE_SAT_LOG_ERROR("CreateDialStringTLV: Invalid string type for CALL CONTROL.\n");
        break;
    } /* end switch */

    if (Tag != DUMMY_TAG)
    { /* Valid String Type */
        Tag |= COMPREHENSION_REQUIRED_MASK;
        *Dest_p++ = Tag;
    }

    if ((DialString_p->cc_type != STE_APDU_CC_TYPE_UNKNOWN) &&
        (DialString_p->address.str_length > 0) &&
        (DialString_p->address.dial_text_p))
    { /* Format length and value data */
        uint8_t* str_p =     NULL;  /* used for copying the (converted) value part */
        uint16_t str_len = 0;


        str_p = (uint8_t *)DialString_p->address.dial_text_p;
        str_len = DialString_p->address.str_length;

        if (str_len > MAX_LENGTH_BYTE1)
        {   /* Code Length on two bytes */
            *Dest_p++ = TWO_BYTES_LENGTH_IND;
            tlv_len = TAG_WITH_TWO_LENGTH_BYTES;
        }
        else
        {   /* Overall size includes only one length byte */
            tlv_len = TAG_WITH_ONE_LENGTH_BYTE;
        }

        //Switch among all the types, the format for PDP is newly added
        switch (DialString_p->cc_type)
        {
            case STE_APDU_CC_CALL_SETUP:
            case STE_APDU_CC_SS:
            {
                /* Add 1 byte to dial string length for TON/NPI */
                *Dest_p++ = (uint8_t)str_len + 1;
                /* Convert to TON/NPI byte has been done outside of this function */
                *Dest_p++ = DialString_p->address.ton_npi;
                tlv_len++;
            }
            break;
            case STE_APDU_CC_USSD:
            {
                /* Add 1 byte to dial string length for DCS */
                *Dest_p++ = (uint8_t)str_len + 1;
                /* Set DCS byte for USSD string TLV */
                *Dest_p++ = DialString_p->dcs;
                tlv_len++;
            }
            break;
            case STE_APDU_CC_PDP:
            {
                *Dest_p++ = (uint8_t)str_len;
            }
            break;
            default:
            {
                STE_SAT_LOG_ERROR("CreateDialStringTLV: Invalid string type for CALL CONTROL.\n");
            }
            break;
        } /* end switch */

        if (str_p != NULL)
        {
            memcpy(Dest_p, str_p, str_len);
            tlv_len += (uint8_t)str_len;
        }
        else
        {
            tlv_len = 0; /* Error with conversion */
        }

    } /* end Format length and value data */

    return (tlv_len);
}

/*************************************************************************
 * Function:      CreateCCP_TLV
 *
 * Description:   Converts CCP from call control requested action
 *                (call, SS or USSD) into the appropriate TLV format.
 *
 * Input Params:  ccp         Call control requested action CCP.
 *
 * Output Params: Dest_p      TLV format for CCP.
 *
 * Return:        Total TLV size
 *
 * Notes:
 *
 *************************************************************************/
static uint8_t CreateCCP_TLV(ste_apdu_ccp_t   ccp,
                             uint8_t          * Dest_p)
{
    uint8_t Size = 0;
    uint8_t Size_temp = 0;

    uint8_t Tag = CAPABILITY_CONFIG_PARAMS_TAG;

    Tag |= COMPREHENSION_REQUIRED_MASK;

    *Dest_p++ = Tag; /* TAG */

    if (ccp.ccp_length > SAT_MAX_SIZE_CCP)
    { /* SizeCCP was incorrect */
        Size_temp = SAT_MAX_SIZE_CCP;
    }
    else
    {
        Size_temp = ccp.ccp_length;
    }

    Size = Size_temp + TAG_WITH_ONE_LENGTH_BYTE;

    *Dest_p++ = Size_temp;
    /* Copy CCP */
    memcpy(Dest_p, ccp.ccp_p, Size_temp);

    return (Size);
}

/*************************************************************************
 * Function:      CreateSubAddressTLV
 *
 * Description:   Converts Subaddress from call control requested action
 *                (call, SS or USSD) into the appropriate TLV format.
 *
 * Input Params:  SubAddress      Call control requested action SubAddress.
 *
 * Output Params: Dest_p          TLV format for SubAddress.
 *
 * Return:        Total TLV size
 *
 * Notes:
 *
 *************************************************************************/
static uint8_t CreateSubAddressTLV(ste_apdu_sub_address_t    SubAddress,
                                   uint8_t                 * Dest_p)
{
    uint8_t Size = 0;
    uint8_t Size_temp = 0;

    uint8_t Tag = CALLED_PARTY_SUBADDRESS_TAG;

    Tag |= COMPREHENSION_REQUIRED_MASK;

    *Dest_p++ = Tag; /* TAG */

    if (SubAddress.sub_address_len > SAT_MAX_SIZE_SUBADDRESS)
    { /* SizeSubAddress was incorrect */
        Size_temp = SAT_MAX_SIZE_SUBADDRESS;
    }
    else
    {
        Size_temp = SubAddress.sub_address_len;
    }

    Size = Size_temp + TAG_WITH_ONE_LENGTH_BYTE;

    *Dest_p++ = Size_temp;

    memcpy(Dest_p, SubAddress.sub_address_p, Size_temp);

    return (Size);
}

/*************************************************************************
 * Function:      CreateBCRepeatInd_TLV
 *
 * Description:   Converts BC Repeat IND from call control requested action
 *                (call, SS or USSD) into the appropriate TLV format.
 *
 * Input Params:  bc_ind         Call control requested action bc repeat ind.
 *
 * Output Params: Dest_p      TLV format for CCP.
 *
 * Return:        Total TLV size
 *
 * Notes:
 *
 *************************************************************************/
static uint8_t CreateBCRepeatInd_TLV(uint8_t   bc_ind,
                                     uint8_t * Dest_p)
{
    uint8_t Size = 0;

    uint8_t Tag = BC_REPEAT_INDICATOR_TAG;

    Tag |= COMPREHENSION_REQUIRED_MASK;

    *Dest_p++ = Tag; /* TAG */

    Size = 1 + TAG_WITH_ONE_LENGTH_BYTE;

    *Dest_p++ = 1;
    *Dest_p++ = bc_ind;

    return (Size);
}

/*************************************************************************
 * Function:      CreateAddressTLV
 *
 * Description:   Converts address data structure into the appropriate TLV format.
 *
 * Input Params:  address_p        pointer to address data structure.
 *
 * Output Params: Dest_p           TLV format for address.
 *
 * Return:        Total TLV size
 *
 * Notes:
 *
 *************************************************************************/
static uint8_t CreateAddressTLV(ste_apdu_address_t * address_p,
                                uint8_t            * Dest_p)
{
    uint8_t Size =  0;
    uint8_t Tag = ADDRESS_TAG;

    Tag |= COMPREHENSION_REQUIRED_MASK;
    *Dest_p++ = Tag;

    if ((address_p->str_length > 0) &&
        (address_p->dial_text_p))
    {
        uint8_t* Copy_p = NULL;  /* used for copying the (converted) value part */
        uint16_t BCD_LenOut = 0;

        Copy_p = (uint8_t *)address_p->dial_text_p;
        BCD_LenOut = address_p->str_length;

        if (BCD_LenOut > MAX_LENGTH_BYTE1)
        {   /* Code Length on two bytes */
            *Dest_p++ = TWO_BYTES_LENGTH_IND;
            Size = (uint8_t)BCD_LenOut + 1 + TAG_WITH_TWO_LENGTH_BYTES;
        }
        else
        {   /* Overall size includes only one length byte */
            Size = (uint8_t)BCD_LenOut + 1 + TAG_WITH_ONE_LENGTH_BYTE;
        }

        /* Add 1 byte for TON/NPI */
        *Dest_p++ = (uint8_t)BCD_LenOut + 1;
        *Dest_p++ = address_p->ton_npi;

        if (Copy_p != NULL)
        {
            memcpy(Dest_p, Copy_p, BCD_LenOut);
        }
        else
        {
            Size = 0; /* Error with conversion */
        }

    } /* end Format length and value data */

    return (Size);
}

/*************************************************************************
 * Function:      CreateCallControlResponse_TLV
 *
 * Description:   Builds the Call control response TLV for terminal response.
 *
 * Input Params:  cc_response_p         Call control response in APDU specific format
 *
 * Output Params: dest_p                TLV format for call control action.
 *
 * Return:        Total TLV size
 *
 * Notes:         Only used for Terminal Responses therefore,
 *                31.111 Sect 6.8 states  "For all SIMPLE-TLV objects with Min=N,
 *                the ME should set the CR flag to comprehension not required".
 *
 *************************************************************************/
static uint8_t
CreateCallControlResponse_TLV(ste_apdu_call_control_response_t * cc_response_p,
                              uint8_t * dest_p)
{
    uint8_t                     total_len = 0;
    uint8_t*                    index_p;
    uint8_t*                    Length_p = NULL;
    uint8_t                     Length = 0;
    ste_sat_apdu_error_t        RetVal = STE_SAT_APDU_ERROR_NONE;

    if (!cc_response_p || !dest_p)
    {
        return 0;
    }

    index_p = dest_p;

    *index_p++ = CALL_CONTROL_ACTION_TAG;
    Length_p = index_p;  // => first byte of total length of SIMPLE-TLVs

    /*
      Reserve one byte to be used if the total length exceeds 0x7F. The two
      bytes length indication will be overwritten if one byte length value.
    */

    *Length_p = TWO_BYTES_LENGTH_IND;
    Length_p++; // point to second length byte
    index_p++;
    index_p++; /* Move to the start of the dial string field */

    if (cc_response_p->bit_mask & EC_CALL_CONTROL_RSP_DialString_present)
    {
        Length = CreateDialStringTLV(&(cc_response_p->rsp_dial_string), index_p);
        *Length_p += Length;
        index_p += Length; /* Move to the start of the CCP 1 field */
    }

    if (cc_response_p->bit_mask & EC_CALL_CONTROL_RSP_CCP1_present)
    {
        Length = CreateCCP_TLV(cc_response_p->rsp_ccp_1, index_p);
        *Length_p += Length;
        index_p += Length; /* Move to the start of the subAddress field */
    }

    if (cc_response_p->bit_mask & EC_CALL_CONTROL_RSP_SubAddress_present)
    {
        Length = CreateSubAddressTLV(cc_response_p->rsp_sub_address, index_p);
        *Length_p += Length;
        index_p += Length; /* Move to the start of the Location Information field */
    }

    if (cc_response_p->bit_mask & EC_CALL_CONTROL_RSP_AlphaIdentifier_present)
    {
        RetVal = CreateAlphaIdentifierTLV(cc_response_p->alpha_id.alpha_id_p,
                                          cc_response_p->alpha_id.alpha_id_length,
                                          FALSE,  //Comprehension required flag
                                          index_p,
                                          &Length);

        if (RetVal != STE_SAT_APDU_ERROR_NONE)
        {
            return 0;
        }
        *Length_p += Length;
        index_p += Length; /* Move to the start of the Location Information field */
    }

    if (cc_response_p->bit_mask & EC_CALL_CONTROL_RSP_BCRepeatInd_present)
    {
        Length = CreateBCRepeatInd_TLV(cc_response_p->rsp_bc_repeat_ind, index_p);
        *Length_p += Length;
        index_p += Length;
    }

    if (cc_response_p->bit_mask & EC_CALL_CONTROL_RSP_CCP2_present)
    {
        Length = CreateCCP_TLV(cc_response_p->rsp_ccp_2, index_p);
        *Length_p += Length;
        index_p += Length;
    }

    /* Determine if the size is contained in one or two bytes */
    if (*Length_p <= MAX_LENGTH_BYTE1)
    {
        /* Overall size is the length plus 2 bytes for the tag and length */
        total_len = (*Length_p) + 2;
        /* Remove the additional size byte */
        memmove(Length_p - 1, Length_p, *Length_p + 1);
    }
    else
    {
        /* Overall size is the length plus 3 bytes for the tag and length */
        total_len = (*Length_p) + 3;
    }

    return total_len;
}


/*************************************************************************
 * Function:      CreateCallControlResultTLV
 *
 * Description:   Builds the call control result TLVs
 *
 * Input Params:  cat_rsp_p         pointer to call control result
 *
 * Output Params: result_p          Pointer to byte array to which TLV will
 *                                  be added.
 *                result_len_p      The length of the result
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateCallControlResultTLV(ste_cat_call_control_response_t * cat_rsp_p,
                           uint8_t * result_p,
                           uint8_t * result_len_p)
{
    ste_apdu_call_control_response_t  cc_rsp;
    uint8_t                           length;
    ste_sat_apdu_error_t              RetVal = STE_SAT_APDU_ERROR_NONE;

    if (!cat_rsp_p || !result_p || !result_len_p)
    {
        STE_SAT_LOG_ERROR("CreateCallControlResultTLV: The input param is NULL!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    *result_len_p = 0;
    //convert the cc result info from client to APDU internal structure ste_apdu_call_control_response_t
    if (!FormatCallControlRspInfo(cat_rsp_p, &cc_rsp))
    {
        STE_SAT_LOG_ERROR("CreateCallControlResultTLV: convert the call control rsp info to apdu specific format failed.\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
    }
    //now ready to put the data into APDU binary
    //first check if the call control request action is present or not
    if (cc_rsp.bit_mask & EC_CALL_CONTROL_RSP_Reulst_present
        && cc_rsp.cc_result == STE_APDU_ALLOWED_WITH_MODIFICATIONS)
    {
        length = CreateCallControlResponse_TLV(&cc_rsp, result_p);
        *result_len_p += length;
        result_p += length;
        //create the second result TLV
        length = CreateResultTLV(SAT_RES_CMD_PERFORMED_SUCCESSFULLY,
                                 0,
                                 NULL,
                                 FALSE,        //TODO, Comprehension required???????????
                                 result_p);
        *result_len_p += length;
        result_p += length;

        if (cc_rsp.rsp_dial_string.cc_type == STE_APDU_CC_USSD)
        { /* USSD string was actually dialed so add USSD return result in 2nd Text string TLV */
            RetVal = CreateTextStringTLV(cc_rsp.rsp_dial_string.address.dial_text_p,
                                         cc_rsp.rsp_dial_string.address.str_length,
                                         cc_rsp.rsp_dial_string.dcs,
                                         FALSE,     //Comprehension required flag
                                         result_p,  //this points to the first empty byte
                                         &length);  //returned total TLV length for Text String
            if (RetVal == STE_SAT_APDU_ERROR_NONE)
            {
                *result_len_p += length;
                result_p += length;
            }
        } /* end USSD string was actually dialed so add USSD return result in 2nd Text string TLV */

    }
    DeleteCallControlRspInfo(&cc_rsp);

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      CreateSendUSSD_ResultTLV
 *
 * Description:   Builds the send ussd call control result TLVs
 *
 * Input Params:  cat_rsp_p         pointer to send ussd call control result
 *
 * Output Params: result_p          Pointer to byte array to which TLV will
 *                                  be added.
 *                result_len_p      The length of the result
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateSendUSSD_ResultTLV(ste_cat_call_control_response_t * cat_rsp_p,
                         uint8_t * result_p,
                         uint8_t * result_len_p)
{
    ste_apdu_call_control_response_t  cc_rsp;
    uint8_t                           length;
    ste_sat_apdu_error_t              RetVal = STE_SAT_APDU_ERROR_NONE;

    if (!cat_rsp_p || !result_p || !result_len_p)
    {
        STE_SAT_LOG_ERROR("CreateCallControlResultTLV: The input param is NULL!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    *result_len_p = 0;
    //convert the cc result info from client to APDU internal structure ste_apdu_call_control_response_t
    if (!FormatCallControlRspInfo(cat_rsp_p, &cc_rsp))
    {
        STE_SAT_LOG_ERROR("CreateCallControlResultTLV: convert the call control rsp info to apdu specific format failed.\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
    }
    //now ready to put the data into APDU binary
    //put the ussd string into the terminal response
    if (cc_rsp.rsp_dial_string.cc_type == STE_APDU_CC_USSD)
    {
        RetVal = CreateTextStringTLV(cc_rsp.rsp_dial_string.address.dial_text_p,
                                     cc_rsp.rsp_dial_string.address.str_length,
                                     cc_rsp.rsp_dial_string.dcs,
                                     FALSE,     //Comprehension required flag
                                     result_p,  //this points to the first empty byte
                                     &length);  //returned total TLV length for Text String
        if (RetVal == STE_SAT_APDU_ERROR_NONE)
        {
            *result_len_p += length;
            result_p += length;
        }
    }

    //check if the call control request action is present or not
    if (cc_rsp.bit_mask & EC_CALL_CONTROL_RSP_Reulst_present
        && cc_rsp.cc_result == STE_APDU_ALLOWED_WITH_MODIFICATIONS)
    {
        length = CreateCallControlResponse_TLV(&cc_rsp, result_p);
        *result_len_p += length;
        result_p += length;
        //create the second result TLV
        length = CreateResultTLV(SAT_RES_CMD_PERFORMED_SUCCESSFULLY,
                                 0,
                                 NULL,
                                 FALSE,        //TODO, Comprehension required???????????
                                 result_p);
        *result_len_p += length;
        result_p += length;
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      BuildLengthTLV
 *
 * Description:   Converts the uint8_t Length to
 *                Lenght TLV as defined in a proactive command
 *                (Ref. 31.111)
 *
 * Input Params:  Length             value to be converted to TLV
 *
 * Output Params: LengthTLV_p        Pointer to start of Length TLV
 *
 * Return:        Number of bytes used by Length TLV
 *
 * Notes:
 *
 *************************************************************************/
static uint8_t
BuildLengthTLV(const uint8_t Length, uint8_t * LengthTLV_p)
{

    uint8_t TLVLength = 0;

    if (Length > MAX_LENGTH_BYTE1) {    /* Length too big for one byte */
        *LengthTLV_p++ = TWO_BYTES_LENGTH_IND;
        TLVLength++;            /* Include extra length byte in overall lenvelope size */
    }

    /* end Length too big for one byte */
    /* Fill in length field */
    *LengthTLV_p++ = Length;
    TLVLength++;

    return (TLVLength);
}                               /* BuildLengthTLV */

/*************************************************************************
 * Function:      Create_PC_APDU_DisplayText
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Create_PC_APDU_DisplayText(ste_parsed_apdu_t * source_apdu_info_p,
                           uint8_t * V_buffer_p,
                           uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //create the command qualifier byte from the info in the command info
    Create_CommandQualifier_PC_DisplayText(source_apdu_info_p);
    STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: APDU Command Qualifier: 0x%d.\n",
         source_apdu_info_p->command_info.CommandQualifier);
    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;
    STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Command Details: %d.\n", *V_Length_p);

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_dt_p->bit_mask & PC_DISPLAY_TEXT_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_dt_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_dt_p->device_id.destination_ID,
                                           TRUE,   //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Device Identities: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_DisplayText: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Text String is present
    if (source_apdu_info_p->apdu_data.pc_dt_p->bit_mask & PC_DISPLAY_TEXT_TextString_present) {
        uint8_t                   TLV_TextStringLen = 0;

        RetVal = CreateTextStringTLV(source_apdu_info_p->apdu_data.pc_dt_p->text_string.text_string_p,
                                     source_apdu_info_p->apdu_data.pc_dt_p->text_string.text_length,
                                     source_apdu_info_p->apdu_data.pc_dt_p->text_string.coding_scheme,
                                     TRUE,  //Comprehension required flag
                                     V_buffer_p + (*V_Length_p),        //this points to the first empty byte
                                     &TLV_TextStringLen);       //returned total TLV length for Text String, including TAG, length, coding scheme, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += TLV_TextStringLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Text String: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_DisplayText: Text String is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Icon Identifier is present
    if (source_apdu_info_p->apdu_data.pc_dt_p->bit_mask & PC_DISPLAY_TEXT_IconIdentifier_present) {
        RetVal = CreateIconIdentifierTLV(source_apdu_info_p->apdu_data.pc_dt_p->icon_id.icon_identifier,
                                         source_apdu_info_p->apdu_data.pc_dt_p->icon_id.icon_qualifier,
                                         TRUE,   //Comprehension required flag
                                         V_buffer_p + (*V_Length_p));   //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += ICON_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Icon Identities: %d.\n", *V_Length_p);
    }
    //check if Immediate Response is present
    if (source_apdu_info_p->apdu_data.pc_dt_p->bit_mask & PC_DISPLAY_TEXT_ImmediateResponse_present) {
        RetVal = CreateImmediateResponseTLV(TRUE,       //Comprehension required flag
                                            V_buffer_p + (*V_Length_p));        //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += IMMEDIATE_RESPONSE_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Immediate Response: %d.\n", *V_Length_p);
    }
    //check if Duration is present
    if (source_apdu_info_p->apdu_data.pc_dt_p->bit_mask & PC_DISPLAY_TEXT_Duration_present) {
        RetVal = CreateDurationTLV(source_apdu_info_p->apdu_data.pc_dt_p->duration,     //time unit, tens of seconds
                                   TRUE,        //Comprehension required flag
                                   V_buffer_p + (*V_Length_p)); //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DURATION_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Duration: %d.\n", *V_Length_p);
    }
    //check if Text Attribute is present
    if (source_apdu_info_p->apdu_data.pc_dt_p->bit_mask & PC_DISPLAY_TEXT_TextAttribute_present) {
        uint8_t                   TLV_TextAttributeLen = 0;

        RetVal = CreateTextAttributeTLV(source_apdu_info_p->apdu_data.pc_dt_p->text_attribute.text_attribute_p,
                                        source_apdu_info_p->apdu_data.pc_dt_p->text_attribute.text_attribute_length,
                                        TRUE,      //Comprehension required flag
                                        V_buffer_p + (*V_Length_p),     //this points to the first empty byte
                                        &TLV_TextAttributeLen); //returned total TLV length for Text Attribute, including TAG, length, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += TLV_TextAttributeLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Text Attribute: %d.\n", *V_Length_p);
    }
    //check if Frame Identifier is present
    if (source_apdu_info_p->apdu_data.pc_dt_p->bit_mask & PC_DISPLAY_TEXT_FrameIdentifier_present) {
        RetVal = CreateFrameIdentifierTLV(source_apdu_info_p->apdu_data.pc_dt_p->frame_identifier,
                                          TRUE,        //Comprehension required flag
                                          V_buffer_p + (*V_Length_p));  //this points to the first empty byte

        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += FRAME_IDENTIFIER_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length after create Frame Identifier: %d.\n", *V_Length_p);
    }

    STE_SAT_LOG_INFO("Create_PC_APDU_DisplayText: Buffer Length: %d.\n", *V_Length_p);

    return RetVal;
}

/*************************************************************************
 * Function:      Create_PC_APDU_ProvideLocalInfo
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Create_PC_APDU_ProvideLocalInfo(ste_parsed_apdu_t * source_apdu_info_p,
                                uint8_t * V_buffer_p,
                                uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //create the command qualifier byte from the info in the command info
    //directly set the value since it matches the binary value in specification
    source_apdu_info_p->command_info.CommandQualifier =
        source_apdu_info_p->apdu_data.pc_pli_p->info_type;
    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_pli_p->bit_mask & PC_PROVIDE_LOCAL_INFO_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_pli_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_pli_p->device_id.destination_ID,
                                           TRUE, //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR
            ("Create_PC_APDU_ProvideLocalInfo: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    return RetVal;
}

/*************************************************************************
 * Function:      Create_PC_APDU_SetupEventList
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static                  ste_sat_apdu_error_t
Create_PC_APDU_SetupEventList(ste_parsed_apdu_t * source_apdu_info_p,
                              uint8_t * V_buffer_p,
                              uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //create the command qualifier byte, RFU, set to 0
    source_apdu_info_p->command_info.CommandQualifier = 0;
    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_sel_p->bit_mask & PC_PROVIDE_LOCAL_INFO_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_sel_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_sel_p->device_id.destination_ID,
                                           TRUE, //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_SetupEventList: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if event list is present
    if (source_apdu_info_p->apdu_data.pc_sel_p->bit_mask & PC_SET_UP_EVENT_LIST_Event_present) {
        uint8_t                   TLV_Length = 0;

        //create event list TLV
        RetVal = CreateEventListTLV(source_apdu_info_p->apdu_data.pc_sel_p->event_list,
                                    TRUE,   //Comprehension required flag
                                    V_buffer_p + (*V_Length_p), //this points to the first empty byte after Device Identities
                                    &TLV_Length);       //the created event list length

        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for event list TLV
        (*V_Length_p) += TLV_Length;
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_SetupEventList: Event List is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    return RetVal;
}

/*************************************************************************
 * Function:      Create_PC_APDU_SetupMenu
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Create_PC_APDU_SetupMenu(ste_parsed_apdu_t * source_apdu_info_p,
                         uint8_t * V_buffer_p,
                         uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    Create_CommandQualifier_PC_SetupMenu(source_apdu_info_p);
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;
    STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length after create Command Details: %d.\n", *V_Length_p);

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_sum_p->bit_mask & PC_SET_UP_MENU_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_sum_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_sum_p->device_id.destination_ID,
                                           TRUE, //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length after create Device Identities: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_SetupMenu: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }


    //check if Alpha Identifier is present
    if (source_apdu_info_p->apdu_data.pc_sum_p->bit_mask & PC_SET_UP_MENU_AlphaIdentifier_present) {
        uint8_t                   TLV_Length = 0;

        RetVal = CreateAlphaIdentifierTLV(source_apdu_info_p->apdu_data.pc_sum_p->alpha_identifier.alpha_id_p,
                                          source_apdu_info_p->apdu_data.pc_sum_p->alpha_identifier.alpha_id_length,
                                          TRUE,  //Comprehension required flag
                                          V_buffer_p + (*V_Length_p),   //this points to the first empty byte
                                          &TLV_Length);
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += TLV_Length;
        STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length after Alpha Identifier: %d.\n", *V_Length_p);
    }
    //check if Item List is present
    if (source_apdu_info_p->apdu_data.pc_sum_p->
        bit_mask & PC_SET_UP_MENU_ItemList_present) {
        uint8_t                   TLV_Length = 0;
        uint8_t                   i = 0;

        for (i = 0; i < source_apdu_info_p->apdu_data.pc_sum_p->item_list.NumberOfNodes; i++) {
            RetVal = CreateItemListTLV(source_apdu_info_p->apdu_data.pc_sum_p->item_list.Header_p->Data_p,
                                       TRUE,        //Comprehension required flag
                                       V_buffer_p + (*V_Length_p),      //this points to the first empty byte
                                       &TLV_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
            //add the length for Command Details
            (*V_Length_p) += TLV_Length;
            STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length after Item List: %d.\n", *V_Length_p);
        }
    }
    //check if Item Next Action Indicator is present
    if (source_apdu_info_p->apdu_data.pc_sum_p->bit_mask & PC_SET_UP_MENU_NextAction_present) {
        uint8_t                   TLV_Length = 0;

        RetVal = CreateNextActionTLV(source_apdu_info_p->apdu_data.pc_sum_p->next_action.NextActionIndicator_p,
                                     source_apdu_info_p->apdu_data.pc_sum_p->next_action.NextActionLen,
                                     TRUE,        //Comprehension required flag
                                     V_buffer_p + (*V_Length_p),        //this points to the first empty byte
                                     &TLV_Length);
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += TLV_Length;
        STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length after Next Action Indicator: %d.\n", *V_Length_p);
    }
    //check if Icon Identifier is present
    if (source_apdu_info_p->apdu_data.pc_sum_p->bit_mask & PC_SET_UP_MENU_IconIdentifier_present) {
        RetVal = CreateIconIdentifierTLV(source_apdu_info_p->apdu_data.pc_sum_p->icon_id.icon_identifier,
                                         source_apdu_info_p->apdu_data.pc_sum_p->icon_id.icon_qualifier,
                                         TRUE, //Comprehension required flag
                                         V_buffer_p + (*V_Length_p));   //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += ICON_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length after create Icon Identities: %d.\n", *V_Length_p);
    }
    //check if Text Attribute is present
    if (source_apdu_info_p->apdu_data.pc_sum_p->bit_mask & PC_SET_UP_MENU_TextAttribute_present) {
        uint8_t                   TLV_TextAttributeLen = 0;

        RetVal = CreateTextAttributeTLV(source_apdu_info_p->apdu_data.pc_sum_p->text_attribute.text_attribute_p,
                                        source_apdu_info_p->apdu_data.pc_sum_p->text_attribute.text_attribute_length,
                                        TRUE,    //Comprehension required flag
                                        V_buffer_p + (*V_Length_p),     //this points to the first empty byte
                                        &TLV_TextAttributeLen); //returned total TLV length for Text Attribute, including TAG, length, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += TLV_TextAttributeLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length after create Text Attribute: %d.\n", *V_Length_p);
    }


    STE_SAT_LOG_INFO("Create_PC_APDU_SetupMenu: Buffer Length: %d.\n", *V_Length_p);

    return RetVal;
}


/*************************************************************************
 * Function:      Create_PC_APDU_GetInkey
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t Create_PC_APDU_GetInkey(ste_parsed_apdu_t *
                                                    source_apdu_info_p,
                                                    uint8_t * V_buffer_p,
                                                    uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //create the command qualifier byte from the info in the command info
    Create_CommandQualifier_PC_GetInkey(source_apdu_info_p);
    STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: APDU Command Qualifier: 0x%d.\n",
         source_apdu_info_p->command_info.CommandQualifier);

    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;
    STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length after create Command Details: %d.\n", *V_Length_p);

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_gik_p->bit_mask & PC_GET_INKEY_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_gik_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_gik_p->device_id.destination_ID,
                                           TRUE, //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length after create Device Identities: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_GetInkey: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Text String is present
    if (source_apdu_info_p->apdu_data.pc_gik_p->bit_mask & PC_GET_INKEY_TextString_present) {
        uint8_t                   TLV_TextStringLen = 0;

        RetVal = CreateTextStringTLV(source_apdu_info_p->apdu_data.pc_gik_p->text_string.text_string_p,
                                     source_apdu_info_p->apdu_data.pc_gik_p->text_string.text_length,
                                     source_apdu_info_p->apdu_data.pc_gik_p->text_string.coding_scheme,
                                     TRUE,       //Comprehension required flag
                                     V_buffer_p + (*V_Length_p),        //this points to the first empty byte
                                     &TLV_TextStringLen);       //returned total TLV length for Text String, including TAG, length, coding scheme, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += TLV_TextStringLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length after create Text String: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_GetInkey: Text String is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Icon Identifier is present
    if (source_apdu_info_p->apdu_data.pc_gik_p->bit_mask & PC_GET_INKEY_IconIdentifier_present) {
        RetVal = CreateIconIdentifierTLV(source_apdu_info_p->apdu_data.pc_gik_p->icon_id.icon_identifier,
                                         source_apdu_info_p->apdu_data.pc_gik_p->icon_id.icon_qualifier,
                                         TRUE, //Comprehension required flag
                                         V_buffer_p + (*V_Length_p));   //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += ICON_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length after create Icon Identities: %d.\n", *V_Length_p);
    }
    //check if Duration is present
    if (source_apdu_info_p->apdu_data.pc_gik_p->bit_mask & PC_GET_INKEY_Duration_present) {
        RetVal = CreateDurationTLV(source_apdu_info_p->apdu_data.pc_gik_p->duration,    //time unit, tens of seconds
                                   TRUE,        //Comprehension required flag
                                   V_buffer_p + (*V_Length_p)); //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DURATION_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length after create Duration: %d.\n", *V_Length_p);
    }
    //check if Text Attribute is present
    if (source_apdu_info_p->apdu_data.pc_gik_p->bit_mask & PC_GET_INKEY_TextAttribute_present) {
        uint8_t                   TLV_TextAttributeLen = 0;

        RetVal = CreateTextAttributeTLV(source_apdu_info_p->apdu_data.pc_gik_p->text_attribute.text_attribute_p,
                                        source_apdu_info_p->apdu_data.pc_gik_p->text_attribute.text_attribute_length,
                                        TRUE,    //Comprehension required flag
                                        V_buffer_p + (*V_Length_p),     //this points to the first empty byte
                                        &TLV_TextAttributeLen); //returned total TLV length for Text Attribute, including TAG, length, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += TLV_TextAttributeLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length after create Text Attribute: %d.\n", *V_Length_p);
    }
    //check if Frame Identifier is present
    if (source_apdu_info_p->apdu_data.pc_gik_p->bit_mask & PC_GET_INKEY_FrameIdentifier_present) {
        RetVal = CreateFrameIdentifierTLV(source_apdu_info_p->apdu_data.pc_gik_p->frame_identifier,
                                          TRUE,       //Comprehension required flag
                                          V_buffer_p + (*V_Length_p));  //this points to the first empty byte

        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += FRAME_IDENTIFIER_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length after create Frame Identifier: %d.\n", *V_Length_p);
    }

    STE_SAT_LOG_INFO("Create_PC_APDU_GetInkey: Buffer Length: %d.\n", *V_Length_p);

    return RetVal;
}



/*************************************************************************
 * Function:      Create_PC_APDU_GetInput
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t Create_PC_APDU_GetInput(ste_parsed_apdu_t *
                                                    source_apdu_info_p,
                                                    uint8_t * V_buffer_p,
                                                    uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //create the command qualifier byte from the info in the command info
    Create_CommandQualifier_PC_GetInput(source_apdu_info_p);

    STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: APDU Command Qualifier: 0x%d.\n",
                     source_apdu_info_p->command_info.CommandQualifier);

    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;
    STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Command Details: %d.\n", *V_Length_p);

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_gi_p->bit_mask & PC_GET_INPUT_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_gi_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_gi_p->device_id.destination_ID,
                                           TRUE,   //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Device ID
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Device Identities: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_GetInput: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Text String is present
    if (source_apdu_info_p->apdu_data.pc_gi_p->bit_mask & PC_GET_INPUT_TextString_present) {
        uint8_t                   TLV_TextStringLen = 0;

        RetVal = CreateTextStringTLV(source_apdu_info_p->apdu_data.pc_gi_p->text_string.text_string_p,
                                     source_apdu_info_p->apdu_data.pc_gi_p->text_string.text_length,
                                     source_apdu_info_p->apdu_data.pc_gi_p->text_string.coding_scheme,
                                     TRUE,  //Comprehension required flag
                                     V_buffer_p + (*V_Length_p),        //this points to the first empty byte
                                     &TLV_TextStringLen);       //returned total TLV length for Text String, including TAG, length, coding scheme, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Text String
        (*V_Length_p) += TLV_TextStringLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Text String: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_GetInput: Text String is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Response Length is present
    if (source_apdu_info_p->apdu_data.pc_gi_p->bit_mask & PC_GET_INPUT_ResponseLength_present) {
        RetVal = CreateResponseLengthTLV(&(source_apdu_info_p->apdu_data.pc_gi_p->response_length),
                                         TRUE,       //Comprehension required flag
                                         V_buffer_p + (*V_Length_p));   //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Response Length
        (*V_Length_p) += RESPONSE_LENGTH_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Response Length: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_GetInput: Response Length is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Default Text is present
    if (source_apdu_info_p->apdu_data.pc_gi_p->bit_mask & PC_GET_INPUT_DefaultText_present) {
        uint8_t                   TLV_DefaultTextLen = 0;

        RetVal = CreateTextStringTLV(source_apdu_info_p->apdu_data.pc_gi_p->default_text.text_string_p,
                                     source_apdu_info_p->apdu_data.pc_gi_p->default_text.text_length,
                                     source_apdu_info_p->apdu_data.pc_gi_p->default_text.coding_scheme,
                                     TRUE,       //Comprehension required flag
                                     V_buffer_p + (*V_Length_p),        //this points to the first empty byte
                                     &TLV_DefaultTextLen);      //returned total TLV length for Default Text, including TAG, length, coding scheme, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Default Text
        (*V_Length_p) += TLV_DefaultTextLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Default Text: %d.\n", *V_Length_p);
    }
    //check if Icon Identifier is present
    if (source_apdu_info_p->apdu_data.pc_gi_p->bit_mask & PC_GET_INPUT_IconIdentifier_present) {
        RetVal = CreateIconIdentifierTLV(source_apdu_info_p->apdu_data.pc_gi_p->icon_id.icon_identifier,
                                         source_apdu_info_p->apdu_data.pc_gi_p->icon_id.icon_qualifier,
                                         TRUE,   //Comprehension required flag
                                         V_buffer_p + (*V_Length_p));   //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for ICON ID
        (*V_Length_p) += ICON_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Icon Identities: %d.\n", *V_Length_p);
    }
    //check if Text Attribute is present
    if (source_apdu_info_p->apdu_data.pc_gi_p->bit_mask & PC_GET_INPUT_TextAttribute_present) {
        uint8_t                   TLV_TextAttributeLen = 0;

        RetVal = CreateTextAttributeTLV(source_apdu_info_p->apdu_data.pc_gi_p->text_attribute.text_attribute_p,
                                        source_apdu_info_p->apdu_data.pc_gi_p->text_attribute.text_attribute_length,
                                        TRUE,      //Comprehension required flag
                                        V_buffer_p + (*V_Length_p),     //this points to the first empty byte
                                        &TLV_TextAttributeLen); //returned total TLV length for Text Attribute, including TAG, length, and text length
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Text Attribute
        (*V_Length_p) += TLV_TextAttributeLen;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Text Attribute: %d.\n", *V_Length_p);
    }
    //check if Frame Identifier is present
    if (source_apdu_info_p->apdu_data.pc_gi_p->bit_mask & PC_GET_INPUT_FrameIdentifier_present) {
        RetVal = CreateFrameIdentifierTLV(source_apdu_info_p->apdu_data.pc_gi_p->frame_identifier,
                                          TRUE,        //Comprehension required flag
                                          V_buffer_p + (*V_Length_p));  //this points to the first empty byte

        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Frame ID
        (*V_Length_p) += FRAME_IDENTIFIER_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length after create Frame Identifier: %d.\n", *V_Length_p);
    }

    STE_SAT_LOG_INFO("Create_PC_APDU_GetInput: Buffer Length: %d.\n", *V_Length_p);

    return RetVal;
}


static ste_sat_apdu_error_t
Create_PC_APDU_TimerManagement(ste_parsed_apdu_t * source_apdu_info_p,
                               uint8_t * V_buffer_p, uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //create the command qualifier byte from the info in the command info
    Create_CommandQualifier_PC_TimerManagement(source_apdu_info_p);
    STE_SAT_LOG_INFO("Create_PC_APDU_TimerManagement: APDU Command Qualifier: 0x%d.\n",source_apdu_info_p->command_info.CommandQualifier);
    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;
    STE_SAT_LOG_INFO("Create_PC_APDU_TimerManagement: Buffer Length after create Command Details: %d.\n", *V_Length_p);

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_tm_p->bit_mask & PC_TIMER_MANAGEMENT_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_tm_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_tm_p->device_id.destination_ID,
                                           TRUE,   //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Device ID
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_TimerManagement: Buffer Length after create Device Identities: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_TimerManagement: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    //check if Timer Identifier is present
    if (source_apdu_info_p->apdu_data.pc_tm_p->bit_mask & PC_TIMER_MANAGEMENT_TimerIdentifier_present) {
        //create timer identifier TLV
        RetVal = CreateTimerIdentifierTLV(source_apdu_info_p->apdu_data.pc_tm_p->timer_identifier.timer_code,
                                          TRUE,     //Comprehension required flag
                                          V_buffer_p + (*V_Length_p));  //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Device ID
        (*V_Length_p) += TIMER_IDENTIFIER_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_TimerManagement: Buffer Length after create Timer Identifier: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_TimerManagement: Timer Identifier is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    if (source_apdu_info_p->apdu_data.pc_tm_p->bit_mask & PC_TIMER_MANAGEMENT_TimerValue_present) {
        RetVal = CreateTimerValueTLV(source_apdu_info_p->apdu_data.pc_tm_p->timer_value.timer_value_data,
                                     TRUE,     //Comprehension required flag
                                     V_buffer_p + (*V_Length_p));
        if ( RetVal != STE_SAT_APDU_ERROR_NONE ) {
            return RetVal;
        }
        (*V_Length_p) += TIMER_VALUE_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_TimerManagement: Buffer Length after create Timer Value: %d.\n", *V_Length_p);
    } else if ( source_apdu_info_p->command_info.CommandQualifier == TIMER_START ) {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_TimerManagement: Timer Value is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    } else {
        STE_SAT_LOG_INFO("Create_PC_APDU_TimerManagement: Timer Value is not present!\n");
    }

    STE_SAT_LOG_INFO("Create_PC_APDU_TimerManagement: Buffer Length: %d.\n", *V_Length_p);
    return RetVal;
}


/*************************************************************************
 * Function:      Create_PC_APDU_MoreTime
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t Create_PC_APDU_MoreTime(ste_parsed_apdu_t *
                                                    source_apdu_info_p,
                                                    uint8_t * V_buffer_p,
                                                    uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //Here the Command Qaulifier is set as RFU
    //directly set the value since it matches the binary value in specification
    source_apdu_info_p->command_info.CommandQualifier = 0;
    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal =CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_mt_p->bit_mask & PC_MORE_TIME_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_mt_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_mt_p->device_id.destination_ID,
                                           TRUE,   //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_ProvideLocalInfo: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    return RetVal;
}


/*************************************************************************
 * Function:      Create_PC_APDU_PollingOff
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
Create_PC_APDU_PollingOff(ste_parsed_apdu_t * source_apdu_info_p,
                          uint8_t * V_buffer_p,
                          uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //Here the Command Qaulifier is set as RFU
    //directly set the value since it matches the binary value in specification
    source_apdu_info_p->command_info.CommandQualifier = 0;
    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_po_p->bit_mask & PC_POLLING_OFF_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_po_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_po_p->device_id.destination_ID,
                                           TRUE,   //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_ProvideLocalInfo: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    return RetVal;
}

/*************************************************************************
 * Function:      Create_PC_APDU_PollInterval
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t Create_PC_APDU_PollInterval(ste_parsed_apdu_t *
                                                        source_apdu_info_p,
                                                        uint8_t * V_buffer_p,
                                                        uint8_t * V_Length_p)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //Here the Command Qaulifier is set as RFU
    //directly set the value since it matches the binary value in specification
    source_apdu_info_p->command_info.CommandQualifier = 0;
    //assemble the bytes for command details, and put the bytes into buffer
    //TODO: Command Number???
    RetVal = CreateCommandDetailsTLV(&source_apdu_info_p->command_info, V_buffer_p);

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        return RetVal;
    }
    //add the length for Command Details
    (*V_Length_p) += COMMAND_DETAILS_TOTAL_SIZE;

    //check if Device ID is present
    if (source_apdu_info_p->apdu_data.pc_pi_p->bit_mask & PC_POLL_INTERVAL_DeviceID_present) {
        //create device identities TLV
        RetVal = CreateDeviceIdentitiesTLV(source_apdu_info_p->apdu_data.pc_pi_p->device_id.source_ID,
                                           source_apdu_info_p->apdu_data.pc_pi_p->device_id.destination_ID,
                                           TRUE,   //Comprehension required flag
                                           V_buffer_p + (*V_Length_p)); //this points to the first empty byte after command details
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DEVICE_IDENTITIES_TOTAL_SIZE;
    }
    //check if Duration is present
    if (source_apdu_info_p->apdu_data.pc_pi_p->bit_mask & PC_POLL_INTERVAL_Duration_present) {
        RetVal = CreateDurationTLV(source_apdu_info_p->apdu_data.pc_pi_p->duration,     //time unit, tens of seconds
                                   TRUE,        //Comprehension required flag
                                   V_buffer_p + (*V_Length_p)); //this points to the first empty byte
        if (RetVal != STE_SAT_APDU_ERROR_NONE) {
            return RetVal;
        }
        //add the length for Command Details
        (*V_Length_p) += DURATION_TOTAL_SIZE;
        STE_SAT_LOG_INFO("Create_PC_APDU_PollInterval: Buffer Length after create Duration: %d.\n", *V_Length_p);
    } else {
        //This part is mandatory, if not present, return error
        STE_SAT_LOG_ERROR("Create_PC_APDU_PollInterval: Device ID is not present!\n");
        return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
    }

    return RetVal;
}

/*************************************************************************
 * Function:      CreateProactiveCommandAPDU
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateProactiveCommandAPDU(ste_parsed_apdu_t * source_apdu_info_p,
                           uint8_t * TLV_data_p,
                           size_t * dataLength_p)
{
    uint8_t                   TLV_Length = 0;     //actual total length for the created APDU, including the length for BER_TLV TAG
    uint8_t                   V_buffer[STE_SAT_MAX_APDU_SIZE];    //the string excluding BER_TLV TAG and Length
    uint8_t                   V_Length = 0;       //the total length for the V_buffer, excluding TAG and Length for BER_TLV
    uint8_t                   BER_TLV_LengthLen = 0;      //the length for BER TLV LENGTH bytes
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;   //return value

    //set the proactive command TAG, STE_SAT_PROACTIVE_COMMAND_TAG
    *TLV_data_p = STE_SAT_PROACTIVE_COMMAND_TAG;
    TLV_data_p++;
    TLV_Length++;

    //#############################################################
    //create the 'V' part for the BER TLV
    //#############################################################
    //init the buffer
    memset(V_buffer, 0, STE_SAT_MAX_APDU_SIZE);

    STE_SAT_LOG_INFO("CreateProactiveCommandAPDU: PC Type ID: 0x%x.\n", source_apdu_info_p->type_id);

    switch (source_apdu_info_p->type_id) {
    case STE_APDU_CMD_TYPE_DISPLAY_TEXT:
        {
            if (source_apdu_info_p->apdu_data.pc_dt_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Display Text Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Display Text
            RetVal = Create_PC_APDU_DisplayText(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO:
        {
            if (source_apdu_info_p->apdu_data.pc_pli_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Provide Local Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Provide Local Info
            RetVal = Create_PC_APDU_ProvideLocalInfo(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST:
        {
            if (source_apdu_info_p->apdu_data.pc_sel_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Set Up Event List is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Set Up Event List
            RetVal = Create_PC_APDU_SetupEventList(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_MENU:
        {
            if (source_apdu_info_p->apdu_data.pc_sum_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Set Up Menu is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Set Up Menu
            RetVal = Create_PC_APDU_SetupMenu(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_GET_INKEY:
        {
            if (source_apdu_info_p->apdu_data.pc_gik_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Get Inkey Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Get Inkey
            RetVal = Create_PC_APDU_GetInkey(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;
    case STE_APDU_CMD_TYPE_GET_INPUT:
        {
            if (source_apdu_info_p->apdu_data.pc_gi_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Get Input Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Get Input
            RetVal = Create_PC_APDU_GetInput(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_MORE_TIME:
        {
            if (source_apdu_info_p->apdu_data.pc_mt_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to More Time Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC More Time
            RetVal = Create_PC_APDU_MoreTime(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_POLLING_OFF:
        {
            if (source_apdu_info_p->apdu_data.pc_po_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Polling Off Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC More Time
            RetVal = Create_PC_APDU_PollingOff(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_POLL_INTERVAL:
        {
            if (source_apdu_info_p->apdu_data.pc_pi_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Polling Interval Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Polling Interval
            RetVal = Create_PC_APDU_PollInterval(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;

    case STE_APDU_CMD_TYPE_TIMER_MANAGEMENT:
        {
            if (source_apdu_info_p->apdu_data.pc_tm_p == NULL) {
                STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: The pointer to Timer Management Info is NULL!\n");
                return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
            //create the APDU for PC Timer Management
            RetVal =Create_PC_APDU_TimerManagement(source_apdu_info_p, V_buffer, &V_Length);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                return RetVal;
            }
        }
        break;

    default:
        {
            /* Unsupported Proactive Command */
            STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: Unsupported proactive command\n");
            return STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
        }
    }                           /* end switch */
    STE_SAT_LOG_ERROR("CreateProactiveCommandAPDU: V part length: %d\n", V_Length);
    //Calculate the total length for BER TLV, two bytes or one byte
    BER_TLV_LengthLen = BuildLengthTLV(V_Length, TLV_data_p);
    TLV_data_p += BER_TLV_LengthLen;    //move to the first empty byte after Length
    TLV_Length += BER_TLV_LengthLen;

    //copy the Value part for BER TLV
    STE_SAT_MEM_COPY(TLV_data_p, V_buffer, V_Length);
    TLV_Length += V_Length;
    *dataLength_p = TLV_Length;

    return (RetVal);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_DisplayText
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
Delete_APDU_PC_DisplayText(ste_apdu_pc_display_text_t * pc_dt_p)
{
    if (pc_dt_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_DisplayText: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_dt_p->bit_mask & PC_DISPLAY_TEXT_TextString_present)
        && pc_dt_p->text_string.text_string_p != NULL) {
        STE_SAT_MEM_FREE(pc_dt_p->text_string.text_string_p);
    }
    if ((pc_dt_p->bit_mask & PC_DISPLAY_TEXT_TextAttribute_present)
        && pc_dt_p->text_attribute.text_attribute_p != NULL) {
        STE_SAT_MEM_FREE(pc_dt_p->text_attribute.text_attribute_p);
    }
    STE_SAT_MEM_FREE(pc_dt_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_SetupMenu
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Delete_APDU_PC_SetupMenu(ste_apdu_pc_set_up_menu_t * pc_sum_p)
{
    if (pc_sum_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_SetupMenu: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_sum_p->bit_mask & PC_SET_UP_MENU_AlphaIdentifier_present)
        && pc_sum_p->alpha_identifier.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_sum_p->alpha_identifier.alpha_id_p);
    }
    if ((pc_sum_p->bit_mask & PC_SET_UP_MENU_NextAction_present)
        && pc_sum_p->next_action.NextActionIndicator_p != NULL) {
        STE_SAT_MEM_FREE(pc_sum_p->next_action.NextActionIndicator_p);
    }
    if ((pc_sum_p->bit_mask & PC_SET_UP_MENU_IconIdList_present)
        && pc_sum_p->icon_id_list.IconIdList.Header_p->Data_p != NULL) {
        STE_SAT_MEM_FREE(pc_sum_p->icon_id_list.IconIdList.
                         Header_p->Data_p);
    }
    if ((pc_sum_p->bit_mask & PC_SET_UP_MENU_TextAttribute_present)
        && pc_sum_p->text_attribute.text_attribute_p != NULL) {
        STE_SAT_MEM_FREE(pc_sum_p->text_attribute.text_attribute_p);
    }
    if ( (pc_sum_p->bit_mask & PC_SET_UP_MENU_ItemList_present)
        && pc_sum_p->item_list.Header_p != NULL ) {
        ste_apdu_node_t        *temp = pc_sum_p->item_list.Header_p;
        while ( temp != NULL ) {
            ste_apdu_node_t *next = temp->Next_p;
            STE_SAT_MEM_FREE( temp->Data_p );
            STE_SAT_MEM_FREE( temp );
            temp = next;
        }
    }
    STE_SAT_MEM_FREE(pc_sum_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_GetInkey
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Delete_APDU_PC_GetInkey(ste_apdu_pc_get_inkey_t * pc_gik_p)
{
    if (pc_gik_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_GetInkey: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_gik_p->bit_mask & PC_GET_INKEY_TextString_present)
        && pc_gik_p->text_string.text_string_p != NULL) {
        STE_SAT_MEM_FREE(pc_gik_p->text_string.text_string_p);
    }
    if ((pc_gik_p->bit_mask & PC_GET_INKEY_TextAttribute_present)
        && pc_gik_p->text_attribute.text_attribute_p != NULL) {
        STE_SAT_MEM_FREE(pc_gik_p->text_attribute.text_attribute_p);
    }
    STE_SAT_MEM_FREE(pc_gik_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_GetInput
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Delete_APDU_PC_GetInput(ste_apdu_pc_get_input_t * pc_gi_p)
{
    if (pc_gi_p == NULL) {
        STE_SAT_LOG_ERROR
            ("Delete_APDU_PC_GetInput: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_gi_p->bit_mask & PC_GET_INPUT_TextString_present)
        && pc_gi_p->text_string.text_string_p != NULL) {
        STE_SAT_MEM_FREE(pc_gi_p->text_string.text_string_p);
    }
    if ((pc_gi_p->bit_mask & PC_GET_INPUT_TextAttribute_present)
        && pc_gi_p->text_attribute.text_attribute_p != NULL) {
        STE_SAT_MEM_FREE(pc_gi_p->text_attribute.text_attribute_p);
    }
    if ((pc_gi_p->bit_mask & PC_GET_INPUT_DefaultText_present)
        && pc_gi_p->default_text.text_string_p != NULL) {
        STE_SAT_MEM_FREE(pc_gi_p->default_text.text_string_p);
    }
    STE_SAT_MEM_FREE(pc_gi_p);
}


/*************************************************************************
 * Function:      Delete_APDU_PC_SendShortMessage
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
Delete_APDU_PC_SendShortMessage(ste_apdu_pc_send_short_message_t * pc_ssm_p)
{
    if (pc_ssm_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_SendShortMessage: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_ssm_p->bit_mask & PC_SEND_SMS_Address_present)
        && pc_ssm_p->address.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_ssm_p->address.dial_text_p);
    }
    if ((pc_ssm_p->bit_mask & PC_SEND_SMS_TextAttribute_present)
        && pc_ssm_p->text_attribute.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_ssm_p->text_attribute.text_attribute_p);
    }
    if ((pc_ssm_p->bit_mask & PC_SEND_SMS_AlphaIdentifier_present)
        && pc_ssm_p->alpha_identifier.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_ssm_p->alpha_identifier.alpha_id_p);
    }
    if ((pc_ssm_p->bit_mask & PC_SEND_SMS_3GPP_SMS_TPDU_present
        || pc_ssm_p->bit_mask & PC_SEND_SMS_CDMA_SMS_TPDU_present)
        && pc_ssm_p->tpdu.data_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_ssm_p->tpdu.data_p);
    }

    STE_SAT_MEM_FREE(pc_ssm_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_SendDTMF
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Delete_APDU_PC_SendDTMF(ste_apdu_pc_send_dtmf_t * pc_dtmf_p)
{
    if (pc_dtmf_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_SendDTMF: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_dtmf_p->bit_mask & PC_SEND_DTMF_TextAttribute_present)
        && pc_dtmf_p->text_attribute.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_dtmf_p->text_attribute.text_attribute_p);
    }
    if ((pc_dtmf_p->bit_mask & PC_SEND_DTMF_AlphaIdentifier_present)
        && pc_dtmf_p->alpha_identifier.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_dtmf_p->alpha_identifier.alpha_id_p);
    }
    if ((pc_dtmf_p->bit_mask & PC_SEND_DTMF_DTMF_String_present)
        && pc_dtmf_p->dtmf.data_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_dtmf_p->dtmf.data_p);
    }

    STE_SAT_MEM_FREE(pc_dtmf_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_SendSS
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Delete_APDU_PC_SendSS(ste_apdu_pc_send_ss_t * pc_ss_p)
{
    if (pc_ss_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_SendSS: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_ss_p->bit_mask & PC_SEND_SS_TextAttribute_present)
        && pc_ss_p->text_attribute.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_ss_p->text_attribute.text_attribute_p);
    }
    if ((pc_ss_p->bit_mask & PC_SEND_SS_AlphaIdentifier_present)
        && pc_ss_p->alpha_identifier.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_ss_p->alpha_identifier.alpha_id_p);
    }
    if ((pc_ss_p->bit_mask & PC_SEND_SS_SS_String_present)
        && pc_ss_p->ss_string.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_ss_p->ss_string.dial_text_p);
    }

    STE_SAT_MEM_FREE(pc_ss_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_SendUSSD
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void Delete_APDU_PC_SendUSSD(ste_apdu_pc_send_ussd_t * pc_ussd_p)
{
    if (pc_ussd_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_SendUSSD: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_ussd_p->bit_mask & PC_SEND_USSD_TextAttribute_present)
        && pc_ussd_p->text_attribute.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_ussd_p->text_attribute.text_attribute_p);
    }
    if ((pc_ussd_p->bit_mask & PC_SEND_USSD_AlphaIdentifier_present)
        && pc_ussd_p->alpha_identifier.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_ussd_p->alpha_identifier.alpha_id_p);
    }
    if ((pc_ussd_p->bit_mask & PC_SEND_USSD_USSD_String_present)
        && pc_ussd_p->ussd_string.str_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_ussd_p->ussd_string.str_p);
    }

    STE_SAT_MEM_FREE(pc_ussd_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_SetupCall
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
Delete_APDU_PC_SetupCall(ste_apdu_pc_setup_call_t * pc_sc_p)
{
    if (pc_sc_p == NULL)
    {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_SetupCall: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_Address_present)
        && pc_sc_p->address.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->address.dial_text_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_CCP_present)
        && pc_sc_p->ccp.ccp_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->ccp.ccp_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_SubAddress_present)
        && pc_sc_p->sub_address.sub_address_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->sub_address.sub_address_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_TextAttribute_1_present)
        && pc_sc_p->text_attribute_1.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->text_attribute_1.text_attribute_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_TextAttribute_2_present)
        && pc_sc_p->text_attribute_2.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->text_attribute_2.text_attribute_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_AlphaIdentifier_1_present)
        && pc_sc_p->alpha_id_1.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_sc_p->alpha_id_1.alpha_id_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_AlphaIdentifier_2_present)
        && pc_sc_p->alpha_id_2.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_sc_p->alpha_id_2.alpha_id_p);
    }

    STE_SAT_MEM_FREE(pc_sc_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_Refresh
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
Delete_APDU_PC_Refresh(ste_apdu_pc_refresh_t * pc_r_p)
{
    if (pc_r_p == NULL)
    {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_Refresh: The pointer to be deleted is NULL.\n");
        return;
    }

    if ( (pc_r_p->bit_mask & PC_REFRESH_AlphaIdentifier_present) &&
         pc_r_p->alpha_identifier.alpha_id_p != NULL )
    {
        STE_SAT_MEM_FREE(pc_r_p->alpha_identifier.alpha_id_p);
    }
    if ( (pc_r_p->bit_mask & PC_REFRESH_TextAttribute_present) &&
         pc_r_p->text_attribute.text_attribute_p != NULL )
    {
        STE_SAT_MEM_FREE(pc_r_p->text_attribute.text_attribute_p);
    }
    if ( (pc_r_p->bit_mask & PC_REFRESH_FileList_present) &&
         pc_r_p->file_list.files_p != NULL )
    {
        size_t  i;
        for ( i = 0 ; i < pc_r_p->file_list.nr_of_files ; i++ ) {
            STE_SAT_MEM_FREE(pc_r_p->file_list.files_p[i].file_path);
        }
        STE_SAT_MEM_FREE(pc_r_p->file_list.files_p);
    }


#define       PC_REFRESH_FileList_present             0x04

#if 0
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_Address_present)
        && pc_sc_p->address.dial_text_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->address.dial_text_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_CCP_present)
        && pc_sc_p->ccp.ccp_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->ccp.ccp_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_SubAddress_present)
        && pc_sc_p->sub_address.sub_address_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->sub_address.sub_address_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_TextAttribute_1_present)
        && pc_sc_p->text_attribute_1.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->text_attribute_1.text_attribute_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_TextAttribute_2_present)
        && pc_sc_p->text_attribute_2.text_attribute_p != NULL)
    {
        STE_SAT_MEM_FREE(pc_sc_p->text_attribute_2.text_attribute_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_AlphaIdentifier_1_present)
        && pc_sc_p->alpha_id_1.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_sc_p->alpha_id_1.alpha_id_p);
    }
    if ((pc_sc_p->bit_mask & PC_SETUP_CALL_AlphaIdentifier_2_present)
        && pc_sc_p->alpha_id_2.alpha_id_p != NULL) {
        STE_SAT_MEM_FREE(pc_sc_p->alpha_id_2.alpha_id_p);
    }
#endif
    STE_SAT_MEM_FREE(pc_r_p);
}

/*************************************************************************
 * Function:      Delete_APDU_PC_LanguageNotification
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
static void
Delete_APDU_PC_LanguageNotification(ste_apdu_pc_language_notification_t * pc_ln_p)
{
    if (pc_ln_p == NULL) {
        STE_SAT_LOG_ERROR("Delete_APDU_PC_LanguageNotification: The pointer to be deleted is NULL.\n");
        return;
    }
    if ((pc_ln_p->bit_mask & PC_LANGUAGE_NOTIFICATION_Language_present)
        && pc_ln_p->language.language_p != NULL) {
        STE_SAT_MEM_FREE(pc_ln_p->language.language_p);
    }
    STE_SAT_MEM_FREE(pc_ln_p);
}

//#############################################################################################################################
// internal functions for Terminal Response
//#############################################################################################################################

/*************************************************************************
 * Function:      CreateResultTLV
 *
 * Description:   Builds the Result TLV
 *
 * Input Params:  GeneralResult       Proactive command General Result
 *                AdditionalInfoSize  Additional information length in bytes
 *                AdditionalInfo_p    Pointer to additional information
 *                CompReq             Comprehension Required
 *
 * Output Params: RspData_p           Pointer to byte array to which TLV will
 *                                    be added.
 *
 * Return:        Size of Result TLV
 *
 * Notes:
 *
 *************************************************************************/
static uint8_t
CreateResultTLV(const uint8_t GeneralResult,
                const uint8_t AdditionalInfoSize,
                const uint8_t * const AdditionalInfo_p,
                const boolean CompReq,
                uint8_t * RspData_p)
{
    uint8_t                   TotalBytes = GENERAL_RESULT_SIZE;
    uint8_t                   LengthBytes = 1;    /* Initialise number of length bytes to one */
    uint8_t                   ResultTag = RESULT_TAG;
    uint8_t                   LocalAdditionalInfoSize = AdditionalInfoSize;

    if (CompReq) {              /* Comprehension Required */
        ResultTag |= COMPREHENSION_REQUIRED_MASK;
    }
    /* end Comprehension Required */

    *RspData_p++ = ResultTag;   /* TotalBytes now 1 */

    /* Check if total length needs to be coded into one or two bytes (> 0x7F) */
    if ((AdditionalInfoSize + GENERAL_RESULT_SIZE) > MAX_LENGTH_BYTE1) {        /* Data length greater than 1 byte length size */
        if (AdditionalInfoSize < (STE_SAT_MAX_APDU_SIZE - 3)) { /* Additional plus TAG plus Len will fit in max buffer */
            *RspData_p++ = TWO_BYTES_LENGTH_IND;
            LengthBytes = 2;
        } else {                /* Data oversize */
            STE_SAT_LOG_WARNING("CreateResultTLV - not enough space for Additional Info.\n");
            LocalAdditionalInfoSize = 0;
        }
    }

    /* Write total length */
    *RspData_p++ = GENERAL_RESULT_SIZE + LocalAdditionalInfoSize;
    TotalBytes += LengthBytes;

    /* Write general result */
    *RspData_p++ = GeneralResult;
    TotalBytes++;

    /* Copy in the additional info as it fits */
    if (LocalAdditionalInfoSize > 0) {
        STE_SAT_MEM_COPY(RspData_p, AdditionalInfo_p, AdditionalInfoSize);
        TotalBytes += AdditionalInfoSize;
    }

    return (TotalBytes);
}                               // CreateResultTLV

/*************************************************************************
 * Function:      CreateTerminalResponse
 *
 * Description:   Builds the Terminal Response APDU
 *
 * Input Params:  CommandInfo_p       ponter to command info
 *
 * Output Params: ResponseData_p      Pointer to byte array to which TLV will
 *                                    be added.
 *                ResponseLength_p    Size of Result TLV
 *
 * Return:        Result/Error message
 *
 * Notes:
 *
 *************************************************************************/
static ste_sat_apdu_error_t
CreateTerminalResponse(ste_command_info_t * CommandInfo_p,
                       uint8_t * ResponseData_p,
                       size_t * ResponseLength_p)
{
    ste_sat_apdu_error_t      RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                   TLVLength = 0;
    uint8_t                   ResultLength = 0;

    if (CommandInfo_p == NULL || ResponseData_p == NULL || ResponseLength_p == NULL) {
        STE_SAT_LOG_ERROR("CreateTerminalResponse: CommandInfo_p == NULL OR ResponseData_p == 0 OR ResponseLength_p == NULL.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    //create command details for Response
    RetVal = CreateCommandDetailsTLV(CommandInfo_p, ResponseData_p);
    //Add the size of command details
    TLVLength += COMMAND_DETAILS_TOTAL_SIZE;
    //move to the first empty byte
    ResponseData_p += COMMAND_DETAILS_TOTAL_SIZE;

    //create Device Identities TLV
    RetVal = CreateDeviceIdentitiesTLV(DEVICE_ME, DEVICE_SIM, FALSE, ResponseData_p);
    TLVLength += DEVICE_IDENTITIES_TOTAL_SIZE;
    //move to the first empty byte
    ResponseData_p += DEVICE_IDENTITIES_TOTAL_SIZE;

    ResultLength = CreateResultTLV(CommandInfo_p->CommandResult.general_result,
                                   CommandInfo_p->CommandResult.additional_info_size,
                                   CommandInfo_p->CommandResult.additional_info_p,
                                   TRUE,        //TODO, Comprehension required???????????
                                   ResponseData_p);

    TLVLength += ResultLength;
    //If there is furthermore info to be added to the ResponseData_p, then move the pointer...
    //move to the first empty byte
    ResponseData_p += ResultLength;

    //set the total length to output parameter
    *ResponseLength_p = TLVLength;

    return RetVal;
}                               // CreateResultTLV



//#############################################################################################################################
// Terminal Response interface functions for outside user
//#############################################################################################################################

/*************************************************************************
 * Function:      ste_apdu_terminal_response
 *
 * Description:   Create an apdu binary from the result info for one specific APDU info structure.
 *                the memory for the APDU binary data will be allocated by APDU module and should be freed by user.
 *
 * Input Params:  apdu_info_p         Pointer to the parsed apdu
 *                command_result_p    Points to the command result with the valid information
 *                                    such as general result, additional info and so.
 *
 * Output Params: apdu                Points to the apdu to be returned.
 *
 * Return:        Apdu error          ste_sat_apdu_error_t
 *
 * Notes:         The result info passed in by user will be set to the APDU info structure,
 *                that is to say the APDU info structure will be updated according to the result
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_apdu_terminal_response(ste_parsed_apdu_t * apdu_info_p,
                           ste_command_result_t * command_result_p,
                           ste_apdu_t ** apdu) {
    return ste_apdu_terminal_response_with_second_result(apdu_info_p, command_result_p, NULL, apdu);
}

/*************************************************************************
 * Function:      ste_apdu_terminal_response_with_two_general_results
 *
 * Description:   Create an apdu binary from the result info for one specific APDU info structure.
 *                the memory for the APDU binary data will be allocated by APDU module and should be freed by user.
 *
 * Input Params:  apdu_info_p               Pointer to the parsed apdu
 *                first_command_result_p    Points to the command result with the valid information
 *                                          such as general result, additional info and so.
 *                second_command_result_p   Points to the second command result with valid info, in the
 *                                          case of two Result TLVs needed.
 *
 * Output Params: apdu                      Points to the apdu to be returned.
 *
 * Return:        Apdu error                ste_sat_apdu_error_t
 *
 * Notes:         The result info passed in by user will be set to the APDU info structure,
 *                that is to say the APDU info structure will be updated according to the result
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_apdu_terminal_response_with_second_result(ste_parsed_apdu_t * apdu_info_p,
                                              ste_command_result_t * first_command_result_p,
                                              ste_command_result_t * second_command_result_p,
                                              ste_apdu_t ** apdu)
{
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                 ResponseTLV[STE_SAT_MAX_APDU_SIZE];
    size_t                  dataLength;
    ste_command_result_t*   command_result_p = NULL;

    //pointer NULL check
    if (apdu_info_p == NULL || first_command_result_p == NULL || apdu == NULL) {
        STE_SAT_LOG_ERROR("ste_apdu_terminal_response: apdu_info_p == NULL OR command_result_p == NULL OR apdu == NULL.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    //init the buffer
    memset(ResponseTLV, 0, STE_SAT_MAX_APDU_SIZE);

    //set the result info to the APDU info structure
    apdu_info_p->command_info.CommandResult.general_result = first_command_result_p->general_result;
    apdu_info_p->command_info.CommandResult.command_status = first_command_result_p->command_status;
    apdu_info_p->command_info.CommandResult.additional_info_size = first_command_result_p->additional_info_size;

    if (first_command_result_p->additional_info_size > 0
        && first_command_result_p->additional_info_p != NULL) {
        //allocate memory for *TLV_data_pp and initialize
        apdu_info_p->command_info.CommandResult.additional_info_p =
            STE_SAT_MEM_ALLOCATE(first_command_result_p->additional_info_size + 1);

        if (NULL == apdu_info_p->command_info.CommandResult.additional_info_p) {
            STE_SAT_LOG_ERROR("ste_apdu_terminal_response: memory allocation failed.\n");
            return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
        }
        //copy the additional info to this pointer
        STE_SAT_MEM_COPY(apdu_info_p->command_info.CommandResult.additional_info_p,
                         first_command_result_p->additional_info_p,
                         first_command_result_p->additional_info_size);
    }
    //create the terminal response

    RetVal = CreateTerminalResponse(&(apdu_info_p->command_info), ResponseTLV, &dataLength);

    //Check if a second general result exists, if so create a second Result TLV and point to correct command result.
    if (second_command_result_p) {
        uint8_t ResultLength = 0;

        ResultLength = CreateResultTLV(second_command_result_p->general_result,
                                       second_command_result_p->additional_info_size,
                                       second_command_result_p->additional_info_p,
                                       TRUE,
                                       ResponseTLV + dataLength);

        //Expand total length of TR with Length of this extra ResultTLV
        dataLength += ResultLength;

        command_result_p = second_command_result_p;

    } else {
        command_result_p = first_command_result_p;
    }

    if (RetVal != STE_SAT_APDU_ERROR_NONE) {
        STE_SAT_LOG_ERROR("ste_apdu_terminal_response: Failed to CreateTerminalResponse.\n");
        return RetVal;
    }

    //check if there is other information to be added
    if (command_result_p->other_data_type != STE_CMD_RESULT_NOTHING) {

        //Switch the type to add other information
        switch (command_result_p->other_data_type) {
        case STE_CMD_RESULT_IMEI:
            {
                uint8_t                   IMEI_TLV_Length;

                if (command_result_p->other_data.imei_p == NULL) {
                    STE_SAT_LOG_ERROR("ste_apdu_terminal_response: No IMEI info to be added in TR! \n");
                    return STE_SAT_APDU_ERROR_INVALID_PARAM;
                }
                //IMEI should only be for Provide Local Info
                if (apdu_info_p->type_id != STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO) {
                    STE_SAT_LOG_WARNING("ste_apdu_terminal_response: APDU type is not matching IMEI, type value: %d \n",
                                        apdu_info_p->type_id);
                }
                //create the IMEI TLV
                RetVal = CreateIMEI_TLV(command_result_p->other_data.imei_p,
                                        ResponseTLV + dataLength,
                                        &IMEI_TLV_Length);
                dataLength += IMEI_TLV_Length;
            }
            break;
        case STE_CMD_RESULT_IMEISV:
            {
                uint8_t                 IMEISV_TLV_Length;

                if(command_result_p->other_data.imeisv_p == NULL) {
                    STE_SAT_LOG_ERROR("ste_apdu_terminal_response: No IMEISV info to be added in TR! \n");
                    return STE_SAT_APDU_ERROR_INVALID_PARAM;
                }
                //IMEISV should only be for Provide Local Info
                if (apdu_info_p->type_id != STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO) {
                    STE_SAT_LOG_WARNING("ste_apdu_terminal_response: APDU type is not matching IMEISV, type value: %d \n",apdu_info_p->type_id);
                }
                //create the IMEI TLV
                RetVal = CreateIMEISV_TLV(command_result_p->other_data.imeisv_p,
                                      ResponseTLV + dataLength,
                                      &IMEISV_TLV_Length);
                dataLength += IMEISV_TLV_Length;
            }
            break;

        case STE_CMD_RESULT_DATE_TIME_TIMEZONE:
        {
            uint8_t                 Date_time_TLV_Length;

            if (command_result_p->other_data.date_time == NULL) {
                STE_SAT_LOG_ERROR("ste_apdu_terminal_response: No date object info to be added in TR! \n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }

            //Date, time and timezone should only be for Provide Local Info
            if (apdu_info_p->type_id != STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO) {
            STE_SAT_LOG_WARNING("ste_apdu_terminal_response: APDU type is not matching date,"
                                "time and timezone object, type value: %d \n",
                                apdu_info_p->type_id);
            }
            //create the Date,Time and Timezone TLV
            RetVal = CreateDateTimeTimezone_TLV(command_result_p->other_data.date_time,
                                  ResponseTLV + dataLength,
                                  &Date_time_TLV_Length);
            dataLength += Date_time_TLV_Length;
        }
        break;
        case STE_CMD_RESULT_TEXT_STRING:
            {
                uint8_t                   TLV_TextStringLen = 0;

                if (command_result_p->other_data.text_p == NULL) {
                    STE_SAT_LOG_ERROR("ste_apdu_terminal_response: No Text String info to be added in TR! \n");
                    return STE_SAT_APDU_ERROR_INVALID_PARAM;
                }
                //TODO: NOTE: Probably the conversion from key pressed to text format should be done here
                //(from pressed key to requested response type)? or by outside CATD for GET INKEY
                //create the Text String TLV
                RetVal = CreateTextStringTLV(command_result_p->other_data.text_p->text_string_p,
                                             command_result_p->other_data.text_p->text_length,
                                             command_result_p->other_data.text_p->coding_scheme,
                                             TRUE,    //Comprehension required flag
                                             ResponseTLV + dataLength,  //this points to the first empty byte
                                             &TLV_TextStringLen);       //returned total TLV length for Text String, including TAG, length, coding scheme, and text length

                if (RetVal == STE_SAT_APDU_ERROR_NONE) {
                    dataLength += TLV_TextStringLen;
                }
            }
            break;
        case STE_CMD_RESULT_CALL_CONTROL_RESULT:
        case STE_CMD_RESULT_CC_SEND_SS:
            {
                uint8_t                   total_len = 0;

                if (command_result_p->other_data.cc_result_p == NULL) {
                    STE_SAT_LOG_ERROR("ste_apdu_terminal_response: No call control result info to be added in TR! \n");
                    return STE_SAT_APDU_ERROR_INVALID_PARAM;
                }
                RetVal = CreateCallControlResultTLV(command_result_p->other_data.cc_result_p,
                                                    ResponseTLV + dataLength,  //this points to the first empty byte
                                                    &total_len);       //returned total TLV length for Text String, including TAG, length, coding scheme, and text length

                if (RetVal == STE_SAT_APDU_ERROR_NONE) {
                    dataLength += total_len;
                }
            }
            break;
        case STE_CMD_RESULT_CC_SEND_USSD:
            {
                uint8_t                   total_len = 0;

                if (command_result_p->other_data.cc_result_p == NULL) {
                    STE_SAT_LOG_ERROR("ste_apdu_terminal_response: No cc send ussd result info to be added in TR! \n");
                    return STE_SAT_APDU_ERROR_INVALID_PARAM;
                }
                RetVal = CreateSendUSSD_ResultTLV(command_result_p->other_data.cc_result_p,
                                                  ResponseTLV + dataLength,  //this points to the first empty byte
                                                  &total_len);       //returned total TLV length for Text String, including TAG, length, coding scheme, and text length
                if (RetVal == STE_SAT_APDU_ERROR_NONE) {
                    dataLength += total_len;
                }
            }
            break;
        case STE_CMD_RESULT_POLL_INTERVAL:
            {
                if (command_result_p->other_data.duration <= 0) {
                    STE_SAT_LOG_ERROR("ste_apdu_terminal_response: No duration info to be added in TR! \n");
                    return STE_SAT_APDU_ERROR_INVALID_PARAM;
                }

                if (apdu_info_p->type_id != STE_APDU_CMD_TYPE_POLL_INTERVAL) {
                    STE_SAT_LOG_WARNING("ste_apdu_terminal_response: APDU type is not matching duration, type value: %d \n",
                                        apdu_info_p->type_id);
                }

                RetVal = CreateDurationTLV(command_result_p->other_data.duration,
                                           FALSE,
                                           ResponseTLV + dataLength);
                dataLength += DURATION_TOTAL_SIZE;
            }
            break;
        case STE_CMD_RESULT_BATTERY:
            {
                //BATTERY should only be for Provide Local Info
                if (apdu_info_p->type_id != STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO) {
                    STE_SAT_LOG_WARNING("ste_apdu_terminal_response: APDU type is not matching BATTERY, type value: %d \n",
                                    apdu_info_p->type_id);
                }
                RetVal = CreateBatteryTLV(command_result_p->other_data.battery_state,
                                                           ResponseTLV + dataLength);
                dataLength += BATTERY_TOTAL_SIZE;
            }
            break;
        case STE_CMD_RESULT_LOCATION:
            {
                //Location should only be for Provide Local Info
                if (apdu_info_p->type_id == STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO) {
                    RetVal = CreateLocationTLV(command_result_p->other_data.location_info_p,
                                               ResponseTLV + dataLength);
                    if (command_result_p->other_data.location_info_p->cell_info.UseExtended) {
                        dataLength += LOCATION_EXTENDED_TOTAL_SIZE;
                    } else {
                        dataLength += LOCATION_TOTAL_SIZE;
                    }
                } else {
                    STE_SAT_LOG_ERROR("ste_apdu_terminal_response: APDU type is not matching LOCATION, type value: %d \n",
                                    apdu_info_p->type_id);
                    return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
                }
            }
            break;
        case STE_CMD_RESULT_SEARCH_MODE:
            {
                // No error checking needed, CreateSearchModeTLV always return STE_SAT_APDU_ERROR_NONE.
                RetVal = CreateSearchModeTLV(command_result_p->other_data.search_mode,
                                             FALSE,
                                             ResponseTLV + dataLength);
                dataLength += SEARCH_MODE_TOTAL_SIZE;
            }
            break;
        case STE_CMD_RESULT_TIMING_ADVANCE:
            {
                // No error checking needed, CreateSearchModeTLV always return STE_SAT_APDU_ERROR_NONE.
                RetVal = CreateTimingAdvanceTLV(command_result_p->other_data.timing_advance.status,
                                                command_result_p->other_data.timing_advance.value,
                                                FALSE,
                                                ResponseTLV + dataLength);
                dataLength += TIMING_ADVANCE_TOTAL_SIZE;
            }
            break;
        case STE_CMD_RESULT_ACCESS_TECHNOLOGY:
            {
                RetVal = CreateAccessTechnologyTLV((cn_cell_rat_t)command_result_p->other_data.rat,
                                                   ResponseTLV + dataLength);
                dataLength += ACCESS_TECHNOLOGY_TOTAL_SIZE;
            }
            break;
        case STE_CMD_RESULT_NMR:
            {
                uint8_t length = 0;
                // No error checking needed, CreateNMRTLV always return STE_SAT_APDU_ERROR_NONE.
                RetVal = CreateNMRTLV(command_result_p->other_data.nmr_info->nmr_geran.measurement_results,
                                      command_result_p->other_data.nmr_info->nmr_geran.num_of_channels,
                                      command_result_p->other_data.nmr_info->nmr_geran.ARFCN_list,
                                      command_result_p->other_data.nmr_info->nmr_utran.mm_report_len,
                                      command_result_p->other_data.nmr_info->nmr_utran.measurements_report,
                                      command_result_p->other_data.nmr_info->nmr_rat_type,
                                      &length,
                                      ResponseTLV + dataLength);
                dataLength += length;
            }
            break;
        case STE_CMD_RESULT_TIMER_MANAGEMENT:
            {
                if (apdu_info_p->type_id != STE_APDU_CMD_TYPE_TIMER_MANAGEMENT) {
                    STE_SAT_LOG_WARNING("ste_apdu_terminal_response: APDU type is not matching duration, type value: %d \n",
                                        apdu_info_p->type_id);
                }

                RetVal = CreateTimerIdentifierTLV(command_result_p->other_data.timer_value[0],
                                                  FALSE,
                                                  ResponseTLV + dataLength);
                dataLength += TIMER_IDENTIFIER_TOTAL_SIZE;
                // Check if Timer value should be included in the response.
                if (command_result_p->other_data.timer_value[1] == 1) {
                    RetVal = CreateTimerManagementTLV(command_result_p->other_data.timer_value[2],
                                                      command_result_p->other_data.timer_value[3],
                                                      command_result_p->other_data.timer_value[4],
                                                      FALSE,
                                                      ResponseTLV + dataLength);
                    dataLength += TIMER_VALUE_TOTAL_SIZE;
                }
            }
            break;
        default:
            {
                //print out a warning message. May need to return error?
                STE_SAT_LOG_WARNING("ste_apdu_terminal_response: unknown other data, Type value: %d \n", command_result_p->other_data_type);
            }
        }
    }

    *apdu = ste_apdu_new(ResponseTLV, dataLength);

    return RetVal;
}

/*************************************************************************
 * Function:      ste_apdu_tr_general_error
 *
 * Description:   Create an apdu binary from the general error result info.
 *                the memory for the APDU binary data will be allocated by APDU module and should be freed by user.
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t * ste_apdu_tr_general_error(ste_sat_apdu_error_t   error_info,
                                       ste_apdu_t            *apdu_p)
{
    ste_apdu_general_result_t   cmd_result;
    ste_apdu_additional_info_t  add_info;
    ste_command_info_t          cmd_info;
    ste_apdu_t                 *result_p = NULL;
    uint8_t                     length;
    uint8_t                     v_length;
    uint8_t                    *raw_p;
    ste_sat_apdu_error_t        rv = STE_SAT_APDU_ERROR_NONE;
    uint8_t                     ResponseTLV[STE_SAT_MAX_APDU_SIZE];
    uint8_t                    *index_p;
    uint8_t                     total_len = 0;

    if (!apdu_p)
    {
        return NULL;
    }
    memset(ResponseTLV, 0, STE_SAT_MAX_APDU_SIZE);
    index_p = ResponseTLV;

    //try to fetch the command details from the raw apdu
    length = apdu_p->raw_length;
    raw_p = apdu_p->raw_data;

    if (*raw_p != STE_SAT_PROACTIVE_COMMAND_TAG)
    {
        return NULL;
    }
    raw_p++;

    if (*raw_p == TWO_BYTES_LENGTH_IND)
    {
        raw_p++;
    }
    v_length = *raw_p;
    raw_p++;

    if ((*raw_p & ~COMPREHENSION_REQUIRED_MASK) != COMMAND_DETAILS_TAG)
    {
        return NULL;
    }
    raw_p++;

    v_length = *raw_p;
    raw_p++;

    if (v_length < 3)
    {
        return NULL;
    }
    cmd_info.CommandNumber = *raw_p;
    raw_p++;
    cmd_info.CommandType = *raw_p;
    raw_p++;
    cmd_info.CommandQualifier = *raw_p;
    raw_p++;

    switch (error_info)
    {
        case STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING:
        case STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT:
        {
            cmd_result = SAT_RES_ERROR_REQUIRED_VALUES_MISSING;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
        case STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
        case STE_SAT_APDU_ERROR_CORRUPTED_DATA:
        {
            cmd_result = SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
        case STE_SAT_APDU_ERROR_COMMAND_BEYOND_ME_CAPABILITY:
        {
            cmd_result = SAT_RES_COMMAND_BEYOND_ME_CAPABILITY;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
        case STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD:
        {
            cmd_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
        case STE_SAT_APDU_ERROR_INVALID_PARAM:
        case STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION:
        case STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE:
        case STE_SAT_APDU_ERROR_EMPTY_DATA:
        case STE_SAT_APDU_ERROR_UNKNOWN:
        default:
        {
            cmd_result = SAT_RES_COMMAND_BEYOND_ME_CAPABILITY;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
    }
    rv = CreateCommandDetailsTLV(&cmd_info, index_p);
    total_len += COMMAND_DETAILS_TOTAL_SIZE;
    index_p += COMMAND_DETAILS_TOTAL_SIZE;

    rv = CreateDeviceIdentitiesTLV(DEVICE_ME, DEVICE_SIM, FALSE, index_p);
    total_len += DEVICE_IDENTITIES_TOTAL_SIZE;
    index_p += DEVICE_IDENTITIES_TOTAL_SIZE;

    v_length = CreateResultTLV(cmd_result, 1, (uint8_t*)(&add_info), FALSE, index_p);
    total_len += v_length;
    index_p += v_length;

    result_p = ste_apdu_new(ResponseTLV, total_len);

    return result_p;
}

//#############################################################################
// APDU interface functions for outside user
//#############################################################################

/*************************************************************************
 * Function:      ste_apdu_parse
 *
 * Description:   Create an apdu info structure from a binady apdu data.
 *                the memory for the APDU info structure will be allocated
 *                by APDU module and should be freed by user.
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_apdu_parse(const ste_apdu_t * apdu,
               ste_parsed_apdu_t ** parsed_apdu_info_pp)
{
    size_t                  dataLength;
    uint8_t                 RemainingLength;
    const uint8_t          *TLV_data_temp_p;
    uint8_t                 Initial_TLV_Tag;
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;

    dataLength = apdu->raw_length;
    TLV_data_temp_p = apdu->raw_data;

    STE_SAT_LOG_INFO("ste_apdu_parse: ###### ENTER ######\n");
    //pointer NULL check
    if (TLV_data_temp_p == NULL || dataLength == 0 || parsed_apdu_info_pp == NULL) {
        STE_SAT_LOG_ERROR("ste_apdu_parse: TLV_data_p == NULL OR dataLength == 0 OR parsed_apdu_info_pp == NULL.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    //allocate memory for *parsed_apdu_info_pp and initialize
    *parsed_apdu_info_pp = (ste_parsed_apdu_t *)STE_SAT_MEM_ALLOCATE(sizeof(ste_parsed_apdu_t));
    if (NULL == *parsed_apdu_info_pp)
    {
        STE_SAT_LOG_ERROR("ste_apdu_parse: memory allocation failed.\n");
        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
    }

    STE_SAT_LOG_INFO("ste_apdu_parse: Parse BER TLV Length.\n");
  /**********************************************************************
   **         Initialise data and validate BER-TLV                     **
   **********************************************************************/
    Initial_TLV_Tag = *TLV_data_temp_p;
    if (!ParseAndValidateBER_TLV_Length((const uint8_t **) (&TLV_data_temp_p),    // Pointer to Tag
                                        (uint8_t *) &RemainingLength))   // Returned length value
    {
        STE_SAT_LOG_ERROR("ste_apdu_parse: Initial TLV Tag length is not correct, not understandable by ME\n");
        return STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }

    STE_SAT_LOG_INFO("ste_apdu_parse: BER TLV Tag: 0x%x.\n", Initial_TLV_Tag);

    switch (Initial_TLV_Tag)
    {
        case STE_SAT_PROACTIVE_COMMAND_TAG:
        {
            //SET the parsed APDU kind to PC
            (*parsed_apdu_info_pp)->apdu_kind = STE_APDU_KIND_PC;
            RetVal = ParseProactiveCommand(TLV_data_temp_p, RemainingLength, *parsed_apdu_info_pp);
        }
        break;
        case STE_SAT_MENU_SELECTION_TAG:
        {
            //SET the parsed APDU kind to EC
            (*parsed_apdu_info_pp)->apdu_kind = STE_APDU_KIND_EC;
        }
        break;
        case STE_SAT_SMS_PP_DOWNLOAD_TAG:
        case STE_SAT_CB_DOWNLOAD_TAG:
        case STE_SAT_CALL_CONTROL_TAG:
        case STE_SAT_MO_SMS_CONTROL_TAG:
        case STE_SAT_EVENT_DOWNLOAD_TAG:
        case STE_SAT_TIMER_EXPIRATION_TAG:
        {
            //TODO: currently skip
        }
        break;
        default:
        {
            STE_SAT_LOG_ERROR("ste_apdu_parse: Tag not found, Tag value: %x \n", *TLV_data_temp_p);
            STE_SAT_MEM_FREE(*parsed_apdu_info_pp);
            *parsed_apdu_info_pp = NULL;
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }
    }

    return RetVal;
}

/*************************************************************************
 * Function:      ste_parsed_apdu_delete
 *
 * Description:   Delete an apdu info structure, including the lower layer pointer parameters
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
void ste_parsed_apdu_delete(ste_parsed_apdu_t * apdu_info_p)
{

    if (apdu_info_p == NULL) {
        STE_SAT_LOG_ERROR("ste_parsed_apdu_delete: apdu_info_p == NULL.\n");
        return;
    }
    //delete command Info related pointer
    if (apdu_info_p->command_info.CommandResult.additional_info_p != NULL) {
        //free the additional info pointer
        STE_SAT_MEM_FREE(apdu_info_p->command_info.CommandResult.additional_info_p);
    }

    switch (apdu_info_p->apdu_kind) {
    case STE_APDU_KIND_PC:
        {
            switch (apdu_info_p->type_id) {
            case STE_APDU_CMD_TYPE_DISPLAY_TEXT:
                {
                    Delete_APDU_PC_DisplayText(apdu_info_p->apdu_data.pc_dt_p);
                }
                break;
            case STE_APDU_CMD_TYPE_SET_UP_MENU:
                {
                    Delete_APDU_PC_SetupMenu(apdu_info_p->apdu_data.pc_sum_p);
                }
                break;
            case STE_APDU_CMD_TYPE_GET_INKEY:
                {
                    Delete_APDU_PC_GetInkey(apdu_info_p->apdu_data.pc_gik_p);
                }
                break;
            case STE_APDU_CMD_TYPE_SET_UP_CALL:
                {
                    Delete_APDU_PC_SetupCall(apdu_info_p->apdu_data.pc_sc_p);
                }
                break;
            case STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE:
                {
                    Delete_APDU_PC_SendShortMessage(apdu_info_p->apdu_data.pc_ssm_p);
                }
                break;
            case STE_APDU_CMD_TYPE_SEND_DTMF:
                {
                    Delete_APDU_PC_SendDTMF(apdu_info_p->apdu_data.pc_dtmf_p);
                }
                break;
            case STE_APDU_CMD_TYPE_SEND_SS:
                {
                    Delete_APDU_PC_SendSS(apdu_info_p->apdu_data.pc_ss_p);
                }
                break;
            case STE_APDU_CMD_TYPE_SEND_USSD:
                {
                    Delete_APDU_PC_SendUSSD(apdu_info_p->apdu_data.pc_ussd_p);
                }
                break;
            case STE_APDU_CMD_TYPE_GET_INPUT:
                {
                    Delete_APDU_PC_GetInput(apdu_info_p->apdu_data.pc_gi_p);
                }
                break;
            case STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO:
                {
                    // This doesn't have any nested allocations, so it's easy to delete
                    STE_SAT_MEM_FREE(apdu_info_p->apdu_data.pc_pli_p);
                }
                break;
            case STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST:
                {
                    // This doesn't have any nested allocations, so it's easy to delete
                    STE_SAT_MEM_FREE(apdu_info_p->apdu_data.pc_sel_p);
                }
                break;
            case STE_APDU_CMD_TYPE_MORE_TIME:
                {
                    // This doesn't have any nested allocations, so it's easy to delete
                    STE_SAT_MEM_FREE(apdu_info_p->apdu_data.pc_mt_p);
                }
                break;
            case STE_APDU_CMD_TYPE_POLLING_OFF:
                {
                    // This doesn't have any nested allocations, so it's easy to delete
                    STE_SAT_MEM_FREE(apdu_info_p->apdu_data.pc_po_p);
                }
                break;
            case STE_APDU_CMD_TYPE_POLL_INTERVAL:
                {
                    // This doesn't have any nested allocations, so it's easy to delete
                    STE_SAT_MEM_FREE(apdu_info_p->apdu_data.pc_pi_p);
                }
                break;
            case STE_APDU_CMD_TYPE_TIMER_MANAGEMENT:
                {
                    // This doesn't have any nested allocations, so it's easy to delete
                    STE_SAT_MEM_FREE(apdu_info_p->apdu_data.pc_tm_p);
                }
                break;
            case STE_APDU_CMD_TYPE_REFRESH:
                {
                    Delete_APDU_PC_Refresh(apdu_info_p->apdu_data.pc_r_p);
                }
                break;
            case STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION:
                {
                    Delete_APDU_PC_LanguageNotification(apdu_info_p->apdu_data.pc_ln_p);
                }
                break;
            default:
                {
                    STE_SAT_LOG_ERROR("ste_apdu_delete: APDU type not found, type value: %d \n", apdu_info_p->type_id);
                    return;
                }
            }
        }
        break;
    case STE_APDU_KIND_TR:
    case STE_APDU_KIND_EC:
        {
            //TODO: currently skip
        }
        break;
    default:
        {
            STE_SAT_LOG_ERROR("ste_apdu_delete: APDU kind not found, Kind value: %d \n", apdu_info_p->apdu_kind);
            return;
        }
    }
    //free the APDU pointer
    STE_SAT_MEM_FREE(apdu_info_p);
}


/*************************************************************************
 * Function:      ste_apdu_unparse
 *
 * Description:   Write the apdu info structure to the data buffer,
 *                note the buffer memory is allocated by APDU module and should be freed by user.
 *                The function will return the actual data length of created buffer via dataLength_p.
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_apdu_unparse(ste_parsed_apdu_t * source_apdu_info_p,
                 ste_apdu_t ** apdu)
{
    size_t                  dataLength;
    ste_sat_apdu_error_t    RetVal = STE_SAT_APDU_ERROR_NONE;
    uint8_t                 TLV_buffer[STE_SAT_MAX_APDU_SIZE];

    //pointer NULL check
    if (source_apdu_info_p == NULL || apdu == NULL) {
        STE_SAT_LOG_ERROR("ste_apdu_unparse: source_apdu_info_p == NULL or apdu == NULL.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    //init the buffer
    memset(TLV_buffer, 0, STE_SAT_MAX_APDU_SIZE);

    switch (source_apdu_info_p->apdu_kind) {
    case STE_APDU_KIND_PC:
        {
            RetVal = CreateProactiveCommandAPDU(source_apdu_info_p, TLV_buffer, &dataLength);
            if (RetVal != STE_SAT_APDU_ERROR_NONE) {
                STE_SAT_LOG_ERROR("ste_apdu_unparse: Failed to CreateProactiveCommandAPDU.\n");
                return RetVal;
            }
            if (dataLength > 0) {
                *apdu = ste_apdu_new(TLV_buffer, dataLength);
                if (!apdu) {
                    STE_SAT_LOG_ERROR("ste_apdu_unparse: memory allocation failed.\n");
                    return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                }
            }
        }
        break;
    case STE_APDU_KIND_TR:
    case STE_APDU_KIND_EC:
        {
            //TODO: currently skip
        }
        break;
    default:
        {
            STE_SAT_LOG_ERROR("ste_apdu_unparse: APDU kind not found, Kind value: %d \n", source_apdu_info_p->apdu_kind);
            return STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT;
        }
    }

    return RetVal;
}

/*************************************************************************
 * Function:      ste_apdu_get_kind
 *
 * Description:   Retrieve kind of an APDU data structure
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
uint8_t ste_parsed_apdu_get_kind(const ste_parsed_apdu_t * apdu_info_p)
{
    // The kind
    if (apdu_info_p != NULL) {
        return apdu_info_p->apdu_kind;
    } else {
        STE_SAT_LOG_ERROR("ste_apdu_get_kind: apdu_info_p == NULL.\n");
        return STE_APDU_KIND_NONE;
    }
}

/*************************************************************************
 * Function:      ste_parsed_apdu_get_type
 *
 * Description:   Retrieve type of an APDU data structure
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
uint8_t ste_parsed_apdu_get_type(const ste_parsed_apdu_t * apdu_info_p)
{
    // NULL check
    if (apdu_info_p != NULL) {
        return apdu_info_p->type_id;
    } else {
        STE_SAT_LOG_ERROR("ste_parsed_apdu_get_type: apdu_info_p == NULL.\n");
        return STE_APDU_CMD_TYPE_NONE;
    }
}

/*************************************************************************
 * Function:      ste_apdu_new
 *
 * Description:
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t *ste_apdu_new(const uint8_t * raw, size_t len)
{
    ste_apdu_t             *apdu;
    apdu = calloc(1, sizeof(*apdu));
    if (apdu) {
        if (raw && len) {
            apdu->raw_data = malloc(len);
            if (apdu->raw_data) {
                memcpy(apdu->raw_data, raw, len);
                apdu->raw_length = len;
            }
        }
    }
    return apdu;
}


void ste_apdu_delete(ste_apdu_t * apdu)
{
    if (apdu) {
        if (apdu->raw_data)
            free(apdu->raw_data);
        free(apdu);
    }
}


const uint8_t *ste_apdu_get_raw(const ste_apdu_t * apdu)
{
    if (apdu)
    {
        return apdu->raw_data;
    }
    else
    {
        return 0;
    }
}

size_t ste_apdu_get_raw_length(const ste_apdu_t * apdu)
{
    if (apdu)
    {
        return apdu->raw_length;
    }
    else
    {
        return 0;
    }
}

ste_sat_apdu_error_t ste_apdu_get_cmd_details(const ste_apdu_t * apdu_p,
                                              uint8_t * cmd_type,
                                              uint8_t * cmd_qualifier,
                                              uint8_t * cmd_number)
{
    uint8_t                     v_length;
    uint8_t                    *raw_p;

    if (!apdu_p)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    //try to fetch the command details from the raw apdu
    raw_p = apdu_p->raw_data;

    if (*raw_p != STE_SAT_PROACTIVE_COMMAND_TAG)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    raw_p++;

    if (*raw_p == TWO_BYTES_LENGTH_IND)
    {
        raw_p++;
    }
    v_length = *raw_p;
    raw_p++;

    if ((*raw_p & ~COMPREHENSION_REQUIRED_MASK) != COMMAND_DETAILS_TAG)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    raw_p++;

    v_length = *raw_p;
    raw_p++;

    if (v_length < 3)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    *cmd_number = *raw_p;
    raw_p++;
    *cmd_type = *raw_p;
    raw_p++;
    *cmd_qualifier = *raw_p;
    raw_p++;

    if (*cmd_type >= STE_APDU_FIRST_RESERVED_COMMAND_TYPE)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    return STE_SAT_APDU_ERROR_NONE; //TODO should be no error
}

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
ste_sat_apdu_error_t  ste_apdu_check_event_data_available(ste_apdu_t *apdu_p)
{
    uint8_t                    *raw_p;
    raw_p = apdu_p->raw_data;

    if (*raw_p != STE_SAT_EVENT_DOWNLOAD_TAG)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    raw_p++;

    if (*raw_p == TWO_BYTES_LENGTH_IND)
    {
        raw_p++;
    }

    raw_p++;

    if ((*raw_p & ~COMPREHENSION_REQUIRED_MASK) != EVENT_LIST_TAG)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    raw_p++;

    if ((*raw_p ) != 1)/*No of the events should be one*/
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    raw_p++;

    if ((*raw_p ) != SET_UP_EVENT_LIST_DATA_AVAILABLE)/*Data available event*/
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    return STE_SAT_APDU_ERROR_NONE;

}
#endif
uint8_t ste_apdu_get_kind(const ste_apdu_t * apdu_p)
{
    uint8_t                     apdu_kind;
    uint8_t                    *raw_p;

    if (!apdu_p)
    {
        return STE_APDU_KIND_NONE;
    }

    raw_p = apdu_p->raw_data;

    switch (*raw_p)
    {
        case STE_SAT_PROACTIVE_COMMAND_TAG:
        {
            apdu_kind = STE_APDU_KIND_PC;
        }
        break;
        case STE_SAT_MENU_SELECTION_TAG:
        case STE_SAT_SMS_PP_DOWNLOAD_TAG:
        case STE_SAT_CB_DOWNLOAD_TAG:
        case STE_SAT_CALL_CONTROL_TAG:
        case STE_SAT_MO_SMS_CONTROL_TAG:
        case STE_SAT_EVENT_DOWNLOAD_TAG:
        case STE_SAT_TIMER_EXPIRATION_TAG:
        {
            apdu_kind = STE_APDU_KIND_EC;
        }
        break;
        case COMMAND_DETAILS_TAG | COMPREHENSION_REQUIRED_MASK:
        case COMMAND_DETAILS_TAG:
        {
            apdu_kind = STE_APDU_KIND_TR;
        }
        break;
        default:
        {
            STE_SAT_LOG_ERROR("ste_apdu_get_kind: Tag not found: 0x%x \n", *raw_p);
            return STE_APDU_KIND_NONE;
        }
    }

    return apdu_kind;
}




/*************************************************************************
 * Function:      ste_apdu_set
 *
 * Description:   set the specific command structure data to one APDU info structure
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_parsed_apdu_set(ste_parsed_apdu_t * apdu_info_p, const void *data_p,
                    uint8_t flags)
{
    //pointer NULL check
    if (apdu_info_p == NULL || data_p == NULL) {
        STE_SAT_LOG_ERROR("ste_apdu_set: apdu_info_p == NULL OR data_p == NULL.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    if (!ValidateDataFlag(flags)) {
        STE_SAT_LOG_ERROR("ste_apdu_set: Invalid flags.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    switch (flags) {
    case STE_APDU_CMD_TYPE_DISPLAY_TEXT:
        {
            apdu_info_p->apdu_data.pc_dt_p = (ste_apdu_pc_display_text_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO:
        {
            apdu_info_p->apdu_data.pc_pli_p = (ste_apdu_pc_provide_local_info_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST:
        {
            apdu_info_p->apdu_data.pc_sel_p = (ste_apdu_pc_set_up_event_list_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_MENU:
        {
            apdu_info_p->apdu_data.pc_sum_p = (ste_apdu_pc_set_up_menu_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_CALL:
        {
            apdu_info_p->apdu_data.pc_sc_p = (ste_apdu_pc_setup_call_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE:
        {
            apdu_info_p->apdu_data.pc_ssm_p = (ste_apdu_pc_send_short_message_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_DTMF:
        {
            apdu_info_p->apdu_data.pc_dtmf_p = (ste_apdu_pc_send_dtmf_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_SS:
        {
            apdu_info_p->apdu_data.pc_ss_p = (ste_apdu_pc_send_ss_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_USSD:
        {
            apdu_info_p->apdu_data.pc_ussd_p = (ste_apdu_pc_send_ussd_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_MORE_TIME:
        {
            apdu_info_p->apdu_data.pc_mt_p = (ste_apdu_pc_more_time_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_TIMER_MANAGEMENT:
        {
            apdu_info_p->apdu_data.pc_tm_p = (ste_apdu_pc_timer_management_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_POLL_INTERVAL:
        {
            apdu_info_p->apdu_data.pc_pi_p = (ste_apdu_pc_poll_interval_t *) data_p;
        }
        break;
    case STE_APDU_CMD_TYPE_POLLING_OFF:
        {
            apdu_info_p->apdu_data.pc_po_p = (ste_apdu_pc_polling_off_t *) data_p;
        }
        break;
    default:
        {
            STE_SAT_LOG_ERROR("ste_apdu_set: Invalid flags, flags value: %d \n", flags);
            return STE_SAT_APDU_ERROR_INVALID_PARAM;
        }
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      ste_apdu_get
 *
 * Description:   fetch the specific command structure data from one APDU info structure
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_parsed_apdu_get(const ste_parsed_apdu_t * apdu_info_p, void **data_pp, uint8_t flags)
{
    //pointer NULL check
    if (apdu_info_p == NULL || data_pp == NULL) {
        STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p == NULL OR data_pp == NULL.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    if (!ValidateDataFlag(flags)) {
        STE_SAT_LOG_ERROR("ste_parsed_apdu_get: Invalid flags.\n");
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    switch (flags) {
    case STE_APDU_CMD_TYPE_DISPLAY_TEXT:
        {
            if (apdu_info_p->apdu_data.pc_dt_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_dt_p == NULL, cannot get pointer for display text.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_dt_p;
        }
        break;

    case STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO:
        {
            if (apdu_info_p->apdu_data.pc_pli_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_pli_p == NULL, cannot get pointer for provide local information.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_pli_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST:
        {
            if (apdu_info_p->apdu_data.pc_sel_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_sel_p == NULL, cannot get pointer for set up event list.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_sel_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_MENU:
        {
            if (apdu_info_p->apdu_data.pc_sum_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_sum_p == NULL, cannot get pointer for set up menu.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_sum_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SET_UP_CALL:
        {
            if (apdu_info_p->apdu_data.pc_sc_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_sc_p == NULL, cannot get pointer for setup call.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_sc_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE:
        {
            if (apdu_info_p->apdu_data.pc_ssm_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_ssm_p == NULL, cannot get pointer for send short message.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_ssm_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_DTMF:
        {
            if (apdu_info_p->apdu_data.pc_dtmf_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_dtmf_p == NULL, cannot get pointer for send dtmf.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_dtmf_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_SS:
        {
            if (apdu_info_p->apdu_data.pc_ss_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_ss_p == NULL, cannot get pointer for send ss.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_ss_p;
        }
        break;
    case STE_APDU_CMD_TYPE_SEND_USSD:
        {
            if (apdu_info_p->apdu_data.pc_ussd_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_ussd_p == NULL, cannot get pointer for send ussd.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_ussd_p;
        }
        break;
    case STE_APDU_CMD_TYPE_MORE_TIME:
        {
            if (apdu_info_p->apdu_data.pc_mt_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_mt_p == NULL, cannot get pointer for more time.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_mt_p;
        }
        break;
    case STE_APDU_CMD_TYPE_TIMER_MANAGEMENT:
        {
            if (apdu_info_p->apdu_data.pc_tm_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_tm_p == NULL, cannot get pointer for timer management.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_tm_p;
        }
        break;
    case STE_APDU_CMD_TYPE_POLL_INTERVAL:
        {
            if (apdu_info_p->apdu_data.pc_pi_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_pi_p == NULL, cannot get pointer for poll interval.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_pi_p;
        }
        break;
    case STE_APDU_CMD_TYPE_POLLING_OFF:
        {
            if (apdu_info_p->apdu_data.pc_po_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_po_p == NULL, cannot get pointer for polling off.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_po_p;
        }
        break;
    case STE_APDU_CMD_TYPE_REFRESH:
        {
            if (apdu_info_p->apdu_data.pc_r_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_r_p == NULL, cannot get pointer for refresh.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_r_p;
        }
        break;
    case STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION:
        {
            if (apdu_info_p->apdu_data.pc_ln_p == NULL) {
                STE_SAT_LOG_ERROR("ste_parsed_apdu_get: apdu_info_p->apdu_data.pc_ln_p == NULL, cannot get pointer for language notification.\n");
                return STE_SAT_APDU_ERROR_INVALID_PARAM;
            }
            *data_pp = apdu_info_p->apdu_data.pc_ln_p;
        }
        break;
    default:
        {
            STE_SAT_LOG_ERROR("ste_parsed_apdu_get: Invalid flags, flags value: %d \n", flags);
            return STE_SAT_APDU_ERROR_INVALID_PARAM;
        }
    }

    return STE_SAT_APDU_ERROR_NONE;
}

/*************************************************************************
 * Function:      ste_apdu_call_control
 *
 * Description:   Encode the APDU binary according to the call control info structure from client
 *
 * Input Params:  cc_p:               call control input data in a strucure
 *
 * Output Params:
 *
 * Return:        The created APDU binary
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_call_control(ste_cat_call_control_t    *cc_p,
                                    ste_apdu_location_info_t  *local_info_p)
{
    ste_apdu_t                 *apdu_p;
    unsigned                    total_len = 0;
    uint8_t*                    Env_p;
    uint8_t*                    Length_p = NULL;
    uint8_t                     Length = 0;
    uint8_t                     EC_TLV_buf[STE_SAT_MAX_APDU_SIZE];
    ste_apdu_ec_call_control_t  cc_info;
    ste_sat_apdu_error_t        RetVal = STE_SAT_APDU_ERROR_NONE;

    if (cc_p == NULL)
    {
        STE_SAT_LOG_ERROR("ste_apdu_call_control: input parameters error.\n");
        return NULL;
    }

    //init the buffer
    memset(EC_TLV_buf, 0, STE_SAT_MAX_APDU_SIZE);

    Env_p = EC_TLV_buf;

    cc_info.bit_mask = 0;

    //convert the info from client to APDU internal structure ste_apdu_dial_string_t
    if (!FormatCallControlInfo(cc_p, &cc_info))
    {
        STE_SAT_LOG_ERROR("ste_apdu_call_control: convert the call control info to apdu specific format failed.\n");
        return NULL;
    }

    /* Build the data for ENVELOPE(CALL CONTROL): */
    *Env_p++ = STE_SAT_CALL_CONTROL_TAG;
    Length_p = Env_p;  // => first byte of total length of SIMPLE-TLVs

    /*
      Reserve one byte to be used if the total length exceeds 0x7F. The two
      bytes length indication will be overwritten if one byte length value.
    */

    *Length_p = TWO_BYTES_LENGTH_IND;
    Length_p++; // point to second length byte
    Env_p++;

    Env_p++; /* Move to the start of the Device Id field */

    RetVal = CreateDeviceIdentitiesTLV(DEVICE_ME, DEVICE_SIM, TRUE, Env_p);

    *Length_p = DEVICE_IDENTITIES_TOTAL_SIZE;

    Env_p += DEVICE_IDENTITIES_TOTAL_SIZE; /* Move to the start of the dial string field */

    if (!(cc_info.bit_mask & EC_CALL_CONTROL_DialString_present))
    {
        STE_SAT_LOG_ERROR("ste_apdu_call_control: no address infomation.\n");
        return NULL;
    }
    Length = CreateDialStringTLV(&(cc_info.dial_string), Env_p);
    *Length_p += Length;
    Env_p += Length; /* Move to the start of the CCP 1 field */

    if (cc_info.bit_mask & EC_CALL_CONTROL_CCP1_present)
    {
        Length = CreateCCP_TLV(cc_info.ccp_1, Env_p);
        *Length_p += Length;
        Env_p += Length; /* Move to the start of the subAddress field */
    }

    if (cc_info.bit_mask & EC_CALL_CONTROL_SubAddress_present)
    {
        Length = CreateSubAddressTLV(cc_info.sub_address, Env_p);
        *Length_p += Length;
        Env_p += Length; /* Move to the start of the Location Information field */
    }

    *Env_p++ = LOCATION_INFORMATION_TAG; /* point to Length byte */
    *Length_p += 1;
    if (local_info_p->cell_info.UseExtended)
    {
      *Env_p++ = EXTENDED_LOCATION_INFO_SIZE; /* Move to the start of the MCC/MNC/LAC */
    }
    else
    {
      *Env_p++ = LOCATION_INFO_SIZE; /* Move to the start of the MCC/MNC/LAC */
    }
    *Length_p += 1;

    memcpy(Env_p, &(local_info_p->net_info.PLMN), sizeof(local_info_p->net_info.PLMN)); /* Copy MCC i MNC data */
    *Length_p += sizeof(local_info_p->net_info.PLMN); /* Move to the start of the LAC data*/
    Env_p += sizeof(local_info_p->net_info.PLMN);

    memcpy(Env_p, &(local_info_p->net_info.LAC), sizeof(local_info_p->net_info.LAC)); /*Copy LAC data*/
    /*All LAI data have been copied, move to the start of the CellId */
    *Length_p += sizeof(local_info_p->net_info.LAC);
    Env_p += sizeof(local_info_p->net_info.LAC);

    *Env_p++ = (local_info_p->cell_info.cid >> 8) & 0xFF; //high byte;
    *Env_p++ = local_info_p->cell_info.cid & 0xFF; //low byte
    *Length_p += 2;
    if (local_info_p->cell_info.UseExtended)
    { /* Add extended CellId */
        *Env_p++ = (local_info_p->cell_info.cid >> 24) & 0x0F; //ext high byte
        *Env_p++ = (local_info_p->cell_info.cid >> 16) & 0xFF; //ext low byte
        *Length_p += 2;
    }

    if (cc_info.bit_mask & EC_CALL_CONTROL_CCP2_present)
    {
        Length = CreateCCP_TLV(cc_info.ccp_2, Env_p);
        *Length_p += Length;
        Env_p += Length; /* Move to the start of the BC repeat ind field */
    }
    if (cc_info.bit_mask & EC_CALL_CONTROL_BCRepeatInd_present)
    {
        Length = CreateBCRepeatInd_TLV(cc_info.bc_repeat_ind, Env_p);
        *Length_p += Length;
        Env_p += Length;
    }
    /* Determine if the size is contained in one or two bytes */
    if (*Length_p <= MAX_LENGTH_BYTE1)
    {
        /* Overall size is the length plus 2 bytes for the tag and length */
        total_len = (*Length_p) + 2;
        /* Remove the additional size byte */
        memmove(Length_p - 1, Length_p, *Length_p + 1);
    }
    else
    {
        /* Overall size is the length plus 3 bytes for the tag and length */
        total_len = (*Length_p) + 3;
    }

    apdu_p = ste_apdu_new(EC_TLV_buf, total_len);

    DeleteCallControlInfo(&cc_info);

    return apdu_p;
}

/*************************************************************************
 * Function:      ste_apdu_setup_call
 *
 * Description:   Encode the APDU binary according to the pc setup call apdu info structure
 *
 * Input Params:  call_apdu_p:        pc setup call apdu info structure
 *                local_info_p:       Network/operator information.
 *
 * Output Params:
 *
 * Return:        The created APDU binary
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_setup_call(ste_apdu_pc_setup_call_t      * call_apdu_p,
                                  ste_apdu_location_info_t      * local_info_p)
{
    ste_apdu_t                   *apdu_p;
    unsigned                      total_len = 0;
    uint8_t*                      Env_p;
    uint8_t*                      Length_p = NULL;
    uint8_t                       Length = 0;
    uint8_t                       EC_TLV_buf[STE_SAT_MAX_APDU_SIZE];
    ste_sat_apdu_error_t          RetVal = STE_SAT_APDU_ERROR_NONE;

    if (call_apdu_p == NULL || local_info_p == NULL)
    {
        STE_SAT_LOG_ERROR("ste_apdu_setup_call: input parameters error.\n");
        return NULL;
    }

    // Init the buffer
    memset(EC_TLV_buf, 0, STE_SAT_MAX_APDU_SIZE);

    Env_p = EC_TLV_buf;

    // Build the data for ENVELOPE(CALL CONTROL):
    *Env_p++ = STE_SAT_CALL_CONTROL_TAG;
    Length_p = Env_p;  // => first byte of total length of SIMPLE-TLVs

    // Reserve one byte to be used if the total length exceeds 0x7F. The two
    // bytes length indication will be overwritten if there is a one byte
    // length value.

    *Length_p = TWO_BYTES_LENGTH_IND;
    Length_p++; // point to second length byte
    Env_p++;

    Env_p++; /* Move to the start of the Device Id field */

    RetVal = CreateDeviceIdentitiesTLV(DEVICE_ME, DEVICE_SIM, TRUE, Env_p);

    *Length_p = DEVICE_IDENTITIES_TOTAL_SIZE;

    Env_p += DEVICE_IDENTITIES_TOTAL_SIZE; /* Move to the start of the dial string field */

    if (!(call_apdu_p->bit_mask & PC_SETUP_CALL_Address_present))
    {
        STE_SAT_LOG_ERROR("ste_apdu_setup_call: no address infomation.\n");
        return NULL;
    }
    Length = CreateAddressTLV(&(call_apdu_p->address), Env_p);
    *Length_p += Length;
    Env_p += Length; /* Move to the start of the CCP 1 field */

    if (call_apdu_p->bit_mask & PC_SETUP_CALL_CCP_present)
    {
        Length = CreateCCP_TLV(call_apdu_p->ccp, Env_p);
        *Length_p += Length;
        Env_p += Length; /* Move to the start of the subAddress field */
    }

    if (call_apdu_p->bit_mask & PC_SETUP_CALL_SubAddress_present)
    {
        Length = CreateSubAddressTLV(call_apdu_p->sub_address, Env_p);
        *Length_p += Length;
        Env_p += Length; /* Move to the start of the Location Information field */
    }

    *Env_p++ = LOCATION_INFORMATION_TAG; /* point to Length byte */
    *Length_p += 1;
    if (local_info_p->cell_info.UseExtended)
    {
      *Env_p++ = EXTENDED_LOCATION_INFO_SIZE; /* Move to the start of the MCC/MNC/LAC */
    }
    else
    {
      *Env_p++ = LOCATION_INFO_SIZE; /* Move to the start of the MCC/MNC/LAC */
    }
    *Length_p += 1;

    memcpy(Env_p, &(local_info_p->net_info.PLMN), sizeof(local_info_p->net_info.PLMN)); /* Copy MCC i MNC data */
    *Length_p += sizeof(local_info_p->net_info.PLMN); /* Move to the start of the LAC data*/
    Env_p += sizeof(local_info_p->net_info.PLMN);

    memcpy(Env_p, &(local_info_p->net_info.LAC), sizeof(local_info_p->net_info.LAC)); /*Copy LAC data*/
    /*All LAI data have been copied, move to the start of the CellId */
    *Length_p += sizeof(local_info_p->net_info.LAC);
    Env_p += sizeof(local_info_p->net_info.LAC);

    *Env_p++ = (local_info_p->cell_info.cid >> 8) & 0xFF; //high byte;
    *Env_p++ = local_info_p->cell_info.cid & 0xFF; //low byte
    *Length_p += 2;
    if (local_info_p->cell_info.UseExtended)
    { /* Add extended CellId */
        *Env_p++ = (local_info_p->cell_info.cid >> 24) & 0x0F; //ext high byte
        *Env_p++ = (local_info_p->cell_info.cid >> 16) & 0xFF; //ext low byte
        *Length_p += 2;
    }

    /* Determine if the size is contained in one or two bytes */
    if (*Length_p <= MAX_LENGTH_BYTE1)
    {
        /* Overall size is the length plus 2 bytes for the tag and length */
        total_len = (*Length_p) + 2;
        /* Remove the additional size byte */
        memmove(Length_p - 1, Length_p, *Length_p + 1);
    }
    else
    {
        /* Overall size is the length plus 3 bytes for the tag and length */
        total_len = (*Length_p) + 3;
    }

    apdu_p = ste_apdu_new(EC_TLV_buf, total_len);

    return apdu_p;
}


void Get_TLV_Length(const uint8_t* data_p,
                    uint8_t  * bytes_nr_p,
                    uint16_t * length_p)
{
    *bytes_nr_p = 1;  // Initiate for the most common case

    if (*data_p == TWO_BYTES_LENGTH_IND)      // 0x81
    {
        *bytes_nr_p = 2;
        data_p++;     /* Point to actual length in following byte */
    }

    *length_p = *data_p;
}

uint8_t get_nr_of_char_for_BCD(uint8_t * source_p, uint8_t length)
{
    uint8_t * index_p;

    index_p = source_p + length - 1; //move to the last byte
    if (((*index_p) & 0xF0) == 0xF0)
    {  //odd number
        return (length * 2 - 1);
    }
    else
    {
        return (length * 2);
    }
}


/*************************************************************************
 * Function:      GetUserDataCodingCB_Scheme
 *
 * Description:   Obtain data coding scheme in enum format used in
 *                UniversalText_t from the data coding scheme byte
 *                Ref. GSM 03.38 version 7.1.0 ch. 5, CB Data Coding Scheme
 *
 * Input Params:  DCS     Data coding scheme byte
 *
 * Output Params: None
 *
 * Return:        Text coding
 *
 * Notes:
 *
 *************************************************************************/
ste_sim_text_coding_t GetUserDataCodingCB_Scheme(const uint8_t  DataCS)
{
    uint8_t                 CodingGroup;
    ste_sim_text_coding_t   ReturnValue = STE_SIM_USSD_PackedGSM_Default;

    CodingGroup = 0xF0 & DataCS;

    if ((CodingGroup >= 0x80) && (CodingGroup <= 0xE0))
    {
        /* bits 7..4 between 1000 and 1110, reserved. Reserved codings shall be assumed to be GSM default */
        ReturnValue = STE_SIM_USSD_PackedGSM_Default;
    }
    else
    { /* Consider all other Coding groups */
        switch (CodingGroup)
        {
            case 0x10:
            {
                if (DataCS == 0x11)
                {
                    ReturnValue = STE_SIM_UCS2;
                }
                else
                {
                    ReturnValue = STE_SIM_USSD_PackedGSM_Default;
                }
            }
            break;

            case 0x40:    /* These cases handle bit 5 = 0, i.e. uncompressed text */
            /* No break! */
            case 0x50:
            {
                switch (DataCS & 0x0C)  /* Check bits 3 & 2 */
                {
                    case 0x00:
                        ReturnValue = STE_SIM_USSD_PackedGSM_Default;  /* Should be impossible, as the text is uncompressed */
                        break;
                    case 0x04:
                        ReturnValue = STE_SIM_GSM_Default;
                        break;
                    case 0x08:
                        ReturnValue = STE_SIM_UCS2;
                        break;
                    case 0x0C:
                        ReturnValue = STE_SIM_USSD_PackedGSM_Default;    /* Really a reserved value. These shall be interpreted as default */
                        break;
                    default:
                        ReturnValue = STE_SIM_USSD_PackedGSM_Default;
                        break;
                }
            }
            break;
            case 0x60:    /* These cases handle bit 5 = 1, i.e. compressed text */
            /* No break! */
            case 0x70:
            {
                switch (DataCS & 0x0C)  /* Check bits 3 & 2 */
                {
                    case 0x00:
                        ReturnValue = STE_SIM_USSD_PackedGSM_Default;
                        break;
                    case 0x04:
                         ReturnValue = STE_SIM_USSD_PackedGSM_Default;
                        break;
                    case 0x08:
                         ReturnValue = STE_SIM_CODING_UNKNOWN;  /* GSM Standard compression for UCS2 data is a contradiction in terms */
                        break;
                    case 0x0C:
                        ReturnValue = STE_SIM_USSD_PackedGSM_Default;    /* Really a reserved value. These shall be interpreted as default */
                        break;
                    default:
                        ReturnValue = STE_SIM_USSD_PackedGSM_Default;
                        break;
                }
            }
            break;
            case 0xF0:  /* Only consider bit 2. Bits 1&0 refer to the message class, which is irrelevant here */
            {
                if ((DataCS & 0x04) == 0)   /* Bit 2 = 0 */
                {
                    ReturnValue = STE_SIM_USSD_PackedGSM_Default;
                }
                else                        /* Bit 2 = 1, 8 bit data */
                {
                    ReturnValue = STE_SIM_GSM_Default;
                }
            }
            break;
            default:
                ReturnValue = STE_SIM_USSD_PackedGSM_Default;
        } /* end switch CodingGroup */
    } /* end Consider all other Coding groups */

    return (ReturnValue);
} /* end GetUserDataCodingCB_Scheme */

static void ConvertAddressTLV_To_DialledAddress(uint8_t * TLV_Data_p,
                                                ste_cat_cc_dialled_address_t* address_p)
{
    uint8_t    bytes_nr;
    uint16_t   length;
    int        rv;

    TLV_Data_p++;        // point to first (only?) Length byte
    Get_TLV_Length(TLV_Data_p, &bytes_nr, &length);

    if (length >= 2)
    { /* minumum length OK */
        TLV_Data_p += bytes_nr;      // point to first value byte i.e the TON and NPI byte

        /* Extract TON and convert to enum - See GSM 03.40 for coding */
        rv = sim_decode_ton_npi(*TLV_Data_p, &address_p->ton, &address_p->npi);

        TLV_Data_p++;   // point to start of BCD number
        address_p->dialled_string_p = malloc(sizeof(ste_sim_text_t));
        if (!address_p->dialled_string_p)
        {
            return;
        }
        memset(address_p->dialled_string_p, 0, sizeof(ste_sim_text_t));

        address_p->dialled_string_p->text_coding = STE_SIM_BCD;
        address_p->dialled_string_p->no_of_characters = get_nr_of_char_for_BCD(TLV_Data_p, length - 1);
        address_p->dialled_string_p->text_p = malloc(length);
        if (!address_p->dialled_string_p->text_p)
        {
            free(address_p->dialled_string_p);
            return;
        }
        memset(address_p->dialled_string_p->text_p, 0, length);
        memcpy(address_p->dialled_string_p->text_p, TLV_Data_p, length - 1);
    } /* End minimum length OK */
}

static void Convert_TLV_To_USSD_info(uint8_t * TLV_Data_p,
                                     ste_cat_cc_ussd_data_t * ussd_p)
{
    uint8_t    bytes_nr;
    uint16_t   length;

    TLV_Data_p++;                 /* point to Start of length data */
    Get_TLV_Length(TLV_Data_p, &bytes_nr, &length);

    if (length >= 2)   /* if it's 0 or 1 then there's no data! */
    {
        ussd_p->dialled_string_p = malloc(sizeof(ste_sim_string_t));
        if (!ussd_p->dialled_string_p)
        {
            return;
        }
        memset(ussd_p->dialled_string_p, 0, sizeof(ste_sim_string_t));

        ussd_p->dialled_string_p->str_p = malloc(length);
        if (!ussd_p->dialled_string_p->str_p)
        {
            free(ussd_p->dialled_string_p);
            return;
        }
        memset(ussd_p->dialled_string_p->str_p, 0, length);

        ussd_p->dialled_string_p->no_of_bytes = length - 1;     /* Subtract 1 byte for the data coding scheme */
        TLV_Data_p += bytes_nr;    /* => Data coding scheme byte */

        ussd_p->dialled_string_p->text_coding = GetUserDataCodingCB_Scheme(*TLV_Data_p);      /* Interpret coding scheme to give enum */
        ussd_p->dcs = *TLV_Data_p;
        TLV_Data_p++;                                             /* point to Start of text */

        memcpy(ussd_p->dialled_string_p->str_p, TLV_Data_p, length - 1);   /* Copy text */
    }
}

static void Convert_TLV_To_PDP_info(uint8_t * TLV_Data_p,
                                    ste_sim_string_t * pdp_context_p)
{
    uint8_t    bytes_nr;
    uint16_t   length;

    TLV_Data_p++;                 /* point to Start of length data */
    Get_TLV_Length(TLV_Data_p, &bytes_nr, &length);

    if (length >= 2)   /* if it's 0 or 1 then there's no data! */
    {
        pdp_context_p->str_p = malloc(length + 1);
        if (!pdp_context_p->str_p)
        {
            return;
        }
        memset(pdp_context_p->str_p, 0, length + 1);

        pdp_context_p->text_coding = STE_SIM_CODING_UNKNOWN;      /* For PDP context activation, the coding is unknown  */
        pdp_context_p->no_of_bytes = length;
        TLV_Data_p += bytes_nr;    /* => pdp context params */

        memcpy(pdp_context_p->str_p, TLV_Data_p, length);   /* Copy text */
    }
}

static void Convert_TLV_To_String(uint8_t *  TLV_Data_p,
                                  uint8_t ** result_p,
                                  uint8_t *  result_len_p)
{
    uint8_t    bytes_nr;
    uint16_t   length;

    TLV_Data_p++;  /* => Start of length data (first byte is TAG value) */
    Get_TLV_Length(TLV_Data_p, &bytes_nr, &length);
    *result_len_p = length;
    TLV_Data_p += bytes_nr; /* => Start of text */
    if (length > 0)
    {
        *result_p = malloc(length);
        if (!(*result_p))
        {
            return;
        }
        /* Copy text */
        memcpy(*result_p, TLV_Data_p, length);
    }
}


/*************************************************************************
 * Function:      GetCodingOfAlphaField
 *
 * Description:   Obtains the data coding scheme from the first byte of
 *                a data object supplied by the SIM in EF ADN format.
 *                Ref. GSM 11.11 Annex B, Coding of Alpha Fields in the SIM
 *                for UCS2.
 *
 * Input Params:  FirstByte              First byte of data object
 *
 * Output Params: None
 *
 * Return:        Text coding scheme
 *
 *
 *************************************************************************/
ste_sim_text_coding_t  GetCodingOfAlphaField (const uint8_t FirstByte)
{
    ste_sim_text_coding_t Coding;

    switch (FirstByte)
    {
        case UCS2_FORMAT:
            Coding = STE_SIM_UCS2;
            break;

        case UCS2_81_PREFIX_FORMAT:
            Coding = STE_SIM_UCS2_81Prefix;
            break;

        case UCS2_82_PREFIX_FORMAT:
            Coding = STE_SIM_UCS2_82Prefix;
            break;

        default:
            Coding = STE_SIM_GSM_Default;
            break;
    }

    return (Coding);
} /* GetCodingOfAlphaField */

/*************************************************************************
 * Function:      CopyUnpackedTextFromSimpleTLV
 *
 * Description:   At entry Source_p is the start of an unpacked text.
 *                The function removes 0xFF's from the end of text
 *                (which is not in UCS2 format) as EF ADN alpha ID's may
 *                have FF pad bytes.
 *                For UCS2 data, the string will have the 80/81/82 prefix
 *                removed and the data coding scheme will be specified in
 *                TextCoding_t.
 *
 * Input Params:  Source_p            Source text
 *                TextLength          Source Text length
 *
 * Output Params: Coding_p            Text coding scheme
 *                Dest_p              Destination text
 *
 * Return:        Length of text copied
 *
 * Notes: It should have already been checked that the text length > 0.
 *
 *************************************************************************/
uint8_t CopyUnpackedTextFromSimpleTLV( const uint8_t*               Source_p,
                                       const uint8_t                TextLength,
                                       ste_sim_text_coding_t* const Coding_p,
                                       uint8_t*               const Dest_p)
{
    uint8_t Size = TextLength;

    /* Obtain the coding scheme. This is only possible if Size > 0 or a
       Purify "Array Bounds Read" error will occur. */
    if (Size > 0)
    {/* Determine coding type */
        *Coding_p = GetCodingOfAlphaField(*Source_p);
    } /* End determine coding type */
    else
    { /* Set Coding scheme to invalid */
        *Coding_p = STE_SIM_CODING_UNKNOWN;
    } /* end Set Coding scheme to invalid */

    if (Size != 0)
    { /* Check if the text ends with 0xFF characters only if not USC2 coded: */

        switch (*Source_p)                /* check first byte */
        {
            case UCS2_FORMAT:
            case UCS2_81_PREFIX_FORMAT:
            case UCS2_82_PREFIX_FORMAT:
            {
                /* Copy text from next byte as the UCS2 format byte is not required in the string */
                Source_p++;
                Size--;
            }
            break;

            default:
            {
                uint8_t  Index;

                for (Index = 0; Index < Size; Index++)
                {
                    if (*(Source_p + Index) == 0xFF) /* 0xFF to indicate end of valid data */
                    {
                        Size = Index;
                        break;
                    }
                }
            }
            break;
        } /* end switch */
    } /* end Check if the text ends with 0xFF characters only if not USC2 coded: */

    if (Size > 0)
    { /* Copy text string */
        memcpy(Dest_p, Source_p, Size);
    } /* end Copy text string */
    else
    { /* Set Coding scheme to invalid */
        *Coding_p = STE_SIM_CODING_UNKNOWN;
    } /* end Set Coding scheme to invalid */

    return(Size);

}  /* end CopyUnpackedTextFromSimpleTLV */


static void ConvertAlphaIdTLV_To_UserInd(uint8_t          * TLV_Data_p,
                                         ste_sim_string_t * user_ind_p)
{
    if (TLV_Data_p != NULL)
    { /* i.e. alpha ID Tag is present */
        uint8_t  Length = 0;

        TLV_Data_p++;  /* point to length */

        if (*TLV_Data_p == TWO_BYTES_LENGTH_IND)
        {
            TLV_Data_p++;  /* point to second length bytes (Length on two bytes) */
        }
        Length = *TLV_Data_p;

        TLV_Data_p++;  /* point to Value */
        if (Length == 0)
        {
            return;
        }
        user_ind_p->str_p = malloc(Length + 1);
        if (!user_ind_p->str_p)
        {
            return;
        }
        memset(user_ind_p->str_p, 0, Length + 1);

        user_ind_p->no_of_bytes = CopyUnpackedTextFromSimpleTLV( TLV_Data_p,
                                                                 Length,
                                                                 &user_ind_p->text_coding,
                                                                 user_ind_p->str_p);
    }
}

/*************************************************************************
 * Function:      ste_apdu_parse_call_control_response
 *
 * Description:   Decode the cc response APDU binary to the call control info structure
 *
 * Input Params:  apdu:            response APDU binary to be decoded.
 *
 * Output Params: cc_rsp_p:        call control response data strucure
 *
 * Return:        RetVal           Returns a valid error code in case of
 *                                 failure.
 *
 * Notes:
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_apdu_parse_call_control_response(ste_cat_call_control_response_t * cc_rsp_p,
                                     ste_apdu_t  * apdu,
                                     ste_cat_call_control_result_t * cc_result_p)
{
    uint8_t  * TLV_Data_p;
    uint8_t    TagValue = 0;
    uint8_t    Length = 0;
    uint8_t    TLVLength = 0;
    uint8_t    RemainingLength = 0;
    boolean    ccp1_exist = FALSE;
    boolean    Valid = TRUE;
    ste_sat_apdu_error_t rv = STE_SAT_APDU_ERROR_NONE;

    if (!cc_rsp_p || !cc_result_p)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    memset(cc_rsp_p, 0, sizeof(ste_cat_call_control_response_t));
    /* If any errors are found in response structure or format. The ME
     allow the user request to continue with no modification. */
    cc_rsp_p->cc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
    *cc_result_p = STE_CAT_CC_ALLOWED_NO_MODIFICATION;

    if (!apdu)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }
    if (!apdu->raw_data)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }
    TLV_Data_p = apdu->raw_data;

    if (*TLV_Data_p > STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }

    /* The first byte is the call control result */
    cc_rsp_p->cc_result = *TLV_Data_p;
    *cc_result_p = *TLV_Data_p;

    /* The next 1 or 2 bytes contain the length */
    TLV_Data_p++;
    if (*TLV_Data_p == TWO_BYTES_LENGTH_IND)
    {
        TLV_Data_p++;

        if (*TLV_Data_p < MIN_LENGTH_BYTE2)
        {
            STE_SAT_LOG_ERROR("Call Control envelope response error in length byte 2: 0x%x\n", *TLV_Data_p);
            return STE_SAT_APDU_ERROR_CORRUPTED_DATA;
        }
    }
    else if (*TLV_Data_p > MAX_LENGTH_BYTE1)
    {
        STE_SAT_LOG_ERROR("Call Control envelope response error in length byte 1: 0x%x\n", *TLV_Data_p);
        return STE_SAT_APDU_ERROR_CORRUPTED_DATA;
    }

    Length = *TLV_Data_p;

    TLV_Data_p++;

    RemainingLength = Length;
    if (RemainingLength <= 2)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }

    while (RemainingLength > 0)
    { /* Loop through the response envelope and parse each TLV */
        // Remove ComprehensionRequired Flag
        TagValue = *TLV_Data_p & ~COMPREHENSION_REQUIRED_MASK;

        TLVLength = *(TLV_Data_p + 1);

        switch (TagValue)
        {
            case ADDRESS_TAG:
            {
                cc_rsp_p->cc_info.cc_type = STE_CAT_CC_CALL_SETUP;
                if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                {
                    cc_rsp_p->cc_info.cc_data.call_setup_p = malloc(sizeof(ste_cat_cc_call_setup_t));
                    if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                    {
                        STE_SAT_LOG_ERROR("memory allocation failed in APDU.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                    memset(cc_rsp_p->cc_info.cc_data.call_setup_p, 0, sizeof(ste_cat_cc_call_setup_t));
                }
                ConvertAddressTLV_To_DialledAddress(TLV_Data_p, &(cc_rsp_p->cc_info.cc_data.call_setup_p->address));
            }
            break;

            case SS_STRING_TAG:
            {
                cc_rsp_p->cc_info.cc_type = STE_CAT_CC_SS;
                if (!cc_rsp_p->cc_info.cc_data.ss_p)
                {
                    cc_rsp_p->cc_info.cc_data.ss_p = malloc(sizeof(ste_cat_cc_ss_t));
                    if (!cc_rsp_p->cc_info.cc_data.ss_p)
                    {
                        STE_SAT_LOG_ERROR("memory allocation failed in APDU.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                ConvertAddressTLV_To_DialledAddress(TLV_Data_p, &(cc_rsp_p->cc_info.cc_data.ss_p->address));
            }
            break;

            case USSD_STRING_TAG:
            {
                cc_rsp_p->cc_info.cc_type = STE_CAT_CC_USSD;
                if (!cc_rsp_p->cc_info.cc_data.ussd_p)
                {
                    cc_rsp_p->cc_info.cc_data.ussd_p = malloc(sizeof(ste_cat_cc_ussd_t));
                    if (!cc_rsp_p->cc_info.cc_data.ussd_p)
                    {
                        STE_SAT_LOG_ERROR("memory allocation failed in APDU.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                Convert_TLV_To_USSD_info(TLV_Data_p, &(cc_rsp_p->cc_info.cc_data.ussd_p->ussd_data));
            }
            break;

            case PDP_CONTEXT_ACTIVATION_TAG:
            {
                cc_rsp_p->cc_info.cc_type = STE_CAT_CC_PDP;
                if (!cc_rsp_p->cc_info.cc_data.pdp_p)
                {
                    cc_rsp_p->cc_info.cc_data.pdp_p = malloc(sizeof(ste_cat_cc_pdp_t));
                    if (!cc_rsp_p->cc_info.cc_data.pdp_p)
                    {
                        STE_SAT_LOG_ERROR("memory allocation failed in APDU.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                }
                Convert_TLV_To_PDP_info(TLV_Data_p, &(cc_rsp_p->cc_info.cc_data.pdp_p->pdp_context));
            }
            break;

            case CAPABILITY_CONFIG_PARAMS_TAG:
            {
                if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                {
                    cc_rsp_p->cc_info.cc_data.call_setup_p = malloc(sizeof(ste_cat_cc_call_setup_t));
                    if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                    {
                        STE_SAT_LOG_ERROR("memory allocation failed in APDU.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                    memset(cc_rsp_p->cc_info.cc_data.call_setup_p, 0, sizeof(ste_cat_cc_call_setup_t));
                }
                if (!ccp1_exist)
                {
                    if (*(TLV_Data_p + 1) > 0)
                    {
                        Convert_TLV_To_String(TLV_Data_p,
                                              &(cc_rsp_p->cc_info.cc_data.call_setup_p->ccp_1.ccp_p),
                                              &(cc_rsp_p->cc_info.cc_data.call_setup_p->ccp_1.len));
                    }
                    // To indicate that the first CCP TLV was found
                    ccp1_exist = TRUE;
                }
                else
                {
                    if (*(TLV_Data_p + 1) > 0)
                    {
                        Convert_TLV_To_String(TLV_Data_p,
                                              &(cc_rsp_p->cc_info.cc_data.call_setup_p->ccp_2.ccp_p),
                                              &(cc_rsp_p->cc_info.cc_data.call_setup_p->ccp_2.len));
                    }
                }
            }
            break;

            case CALLED_PARTY_SUBADDRESS_TAG:
            {
                if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                {
                    cc_rsp_p->cc_info.cc_data.call_setup_p = malloc(sizeof(ste_cat_cc_call_setup_t));
                    if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                    {
                        STE_SAT_LOG_ERROR("memory allocation failed in APDU.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                    memset(cc_rsp_p->cc_info.cc_data.call_setup_p, 0, sizeof(ste_cat_cc_call_setup_t));
                }
                Convert_TLV_To_String(TLV_Data_p,
                                      &(cc_rsp_p->cc_info.cc_data.call_setup_p->sub_address.sub_address_p),
                                      &(cc_rsp_p->cc_info.cc_data.call_setup_p->sub_address.len));
            }
            break;

            case ALPHA_IDENTIFIER_TAG:
            {
                ConvertAlphaIdTLV_To_UserInd(TLV_Data_p, &(cc_rsp_p->user_indication));
            }
            break;

            case BC_REPEAT_INDICATOR_TAG:
            {
                if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                {
                    cc_rsp_p->cc_info.cc_data.call_setup_p = malloc(sizeof(ste_cat_cc_call_setup_t));
                    if (!cc_rsp_p->cc_info.cc_data.call_setup_p)
                    {
                        STE_SAT_LOG_ERROR("memory allocation failed in APDU.\n");
                        return STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE;
                    }
                    memset(cc_rsp_p->cc_info.cc_data.call_setup_p, 0, sizeof(ste_cat_cc_call_setup_t));
                }

                cc_rsp_p->cc_info.cc_data.call_setup_p->bc_repeat_ind = *(TLV_Data_p + 2);
            }
            break;

            default:
            {
                // An invalid tag type has been found, do not act on SIM response
                STE_SAT_LOG_ERROR("Call Control envelope error, invalid tag found: 0x%x\n", *TLV_Data_p);
                cc_rsp_p->cc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
                *cc_result_p = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
                Valid = FALSE;
                /* Possible error in TLV length, Abort further processing */
                RemainingLength = 0;
                rv = STE_SAT_APDU_ERROR_CORRUPTED_DATA;
            }
        } /* end switch */

        if (Valid)
        {
            RemainingLength -= (TLVLength + 2);
            TLV_Data_p += (TLVLength + 2);
        }

    } /* end Loop through the response envelope and parse each TLV */

    return rv;
}


/*************************************************************************
 * Function:      ste_apdu_sms_control
 *
 * Description:   Encode the APDU binary according to the sms control info structure from client
 *
 * Input Params:  sc_p:               sms control input data in a strucure
 *
 * Output Params:
 *
 * Return:        The created APDU binary
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_sms_control(ste_cat_sms_control_t     * sc_p,
                                   ste_apdu_location_info_t  * local_info_p)
{
    ste_apdu_t                 *apdu_p = NULL;
    unsigned                    total_len = 0;
    uint8_t*                    Env_p;
    uint8_t*                    Length_p = NULL;
    uint8_t                     Length = 0;
    uint8_t                     EC_TLV_buf[STE_SAT_MAX_APDU_SIZE];
    ste_apdu_ec_sms_control_t   sc_info;
    ste_sat_apdu_error_t        RetVal = STE_SAT_APDU_ERROR_NONE;

    if (sc_p == NULL)
    {
        STE_SAT_LOG_ERROR("ste_apdu_sms_control: input parameters error.\n");
        return NULL;
    }

    //init the buffer
    memset(EC_TLV_buf, 0, STE_SAT_MAX_APDU_SIZE);

    Env_p = EC_TLV_buf;

    sc_info.bit_mask = 0;

    //convert the info from client to APDU internal structure ste_apdu_dial_string_t
    if (!FormatSmsControlInfo(sc_p, &sc_info))
    {
        STE_SAT_LOG_ERROR("ste_apdu_sms_control: convert the sms control info to apdu specific format failed.\n");
        return NULL;
    }

    /* Build the data for ENVELOPE(SMS CONTROL): */
    *Env_p++ = STE_SAT_MO_SMS_CONTROL_TAG;
    Length_p = Env_p;  // => first byte of total length of SIMPLE-TLVs

    /*
      Reserve one byte to be used if the total length exceeds 0x7F. The two
      bytes length indication will be overwritten if one byte length value.
    */

    *Length_p = TWO_BYTES_LENGTH_IND;
    Length_p++; // point to second length byte
    Env_p++;

    Env_p++; /* Move to the start of the Device Id field */

    RetVal = CreateDeviceIdentitiesTLV(DEVICE_ME, DEVICE_SIM, TRUE, Env_p);

    *Length_p = DEVICE_IDENTITIES_TOTAL_SIZE;

    Env_p += DEVICE_IDENTITIES_TOTAL_SIZE; /* Move to the start of the dial string field */

    if (!(sc_info.bit_mask & EC_SMS_CONTROL_SMSC_Address_present))
    {
        STE_SAT_LOG_ERROR("ste_apdu_sms_control: no SMSC address infomation.\n");
        return NULL;
    }
    Length = CreateAddressTLV(&(sc_info.smsc), Env_p);
    *Length_p += Length;
    Env_p += Length; /* Move to the start of the destination address field */

    if (!(sc_info.bit_mask & EC_SMS_CONTROL_DestAddress_present))
    {
        STE_SAT_LOG_ERROR("ste_apdu_sms_control: no destination address infomation.\n");
        return NULL;
    }
    Length = CreateAddressTLV(&(sc_info.destination), Env_p);
    *Length_p += Length;
    Env_p += Length; /* Move to the start of the Location Information field */

    *Env_p++ = LOCATION_INFORMATION_TAG; /* point to Length byte */
    *Length_p += 1;
    if (local_info_p->cell_info.UseExtended)
    {
        *Env_p++ = EXTENDED_LOCATION_INFO_SIZE; /* Move to the start of the MCC/MNC/LAC */
    }
    else
    {
        *Env_p++ = LOCATION_INFO_SIZE; /* Move to the start of the MCC/MNC/LAC */
    }
    *Length_p += 1;

    memcpy(Env_p, &(local_info_p->net_info.PLMN), sizeof(local_info_p->net_info.PLMN)); /* Copy MCC i MNC data */
    *Length_p += sizeof(local_info_p->net_info.PLMN); /* Move to the start of the LAC data*/
    Env_p += sizeof(local_info_p->net_info.PLMN);

    memcpy(Env_p, &(local_info_p->net_info.LAC), sizeof(local_info_p->net_info.LAC)); /*Copy LAC data*/
    /*All LAI data have been copied, move to the start of the CellId */
    *Length_p += sizeof(local_info_p->net_info.LAC);
    Env_p += sizeof(local_info_p->net_info.LAC);

    *Env_p++ = (local_info_p->cell_info.cid >> 8) & 0xFF; //high byte;
    *Env_p++ = local_info_p->cell_info.cid & 0xFF; //low byte
    *Length_p += 2;
    if (local_info_p->cell_info.UseExtended)
    { /* Add extended CellId */
        *Env_p++ = (local_info_p->cell_info.cid >> 24) & 0x0F; //ext high byte
        *Env_p++ = (local_info_p->cell_info.cid >> 16) & 0xFF; //ext low byte
        *Length_p += 2;
    }

    /* Determine if the size is contained in one or two bytes */
    if (*Length_p <= MAX_LENGTH_BYTE1)
    {
        /* Overall size is the length plus 2 bytes for the tag and length */
        total_len = (*Length_p) + 2;
        /* Remove the additional size byte */
        memmove(Length_p - 1, Length_p, *Length_p + 1);
    }
    else
    {
        /* Overall size is the length plus 3 bytes for the tag and length */
        total_len = (*Length_p) + 3;
    }

    apdu_p = ste_apdu_new(EC_TLV_buf, total_len);

    DeleteSmsControlInfo(&sc_info);

    return apdu_p;
}

/*************************************************************************
 * Function:      ste_apdu_copy
 *
 * Description:   clone an APDU data structure
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t * ste_apdu_copy(ste_apdu_t *source_p)
{
    ste_apdu_t * dst_p = NULL;

    if (!source_p)
    {
        return NULL;
    }
    dst_p = malloc(sizeof(ste_apdu_t));
    if (!dst_p)
    {
        return NULL;
    }

    memcpy(dst_p, source_p, sizeof(ste_apdu_t));
    if (source_p->raw_length > 0 && source_p->raw_data)
    {
        dst_p->raw_data = malloc(source_p->raw_length + 1);
        if (!dst_p->raw_data)
        {
            free(dst_p);
            return NULL;
        }
        memset(dst_p->raw_data, 0, source_p->raw_length + 1);
        memcpy(dst_p->raw_data, source_p->raw_data, source_p->raw_length);
    }
    return dst_p;
}

uint8_t  ste_apdu_get_catd_status_words(const ste_apdu_t * apdu_p,
                                        ste_apdu_status_words_t * status_words)
{
    if (status_words == NULL) {
        return 1; // Failure
    }

    status_words->sw1 = 0;
    status_words->sw2 = 0;

    if (apdu_p && apdu_p->raw_length >= 2) {
        status_words->sw1 = apdu_p->raw_data[apdu_p->raw_length-2];
        status_words->sw2 = apdu_p->raw_data[apdu_p->raw_length-1];
    } else {
        return 1; // Status words not available
    }

    return 0;
}
/*************************************************************************
 * Function:      ste_apdu_decode_alhpa_id
 *
 * Description:   At entry ste_apdu_alpha_identifier_t is the APDU structure of an alpha id.
 *                The function removes 0xFF's from the end of text
 *                (which is not in UCS2 format) as EF ADN alpha ID's may
 *                have FF pad bytes.
 *                For UCS2 data, the string will have the 80/81/82 prefix
 *                removed and the data coding scheme will be specified in
 *                ste_sim_text_coding_t.
 *
 * Input Params:  alpha_p             Source alpha id structure
 *
 * Output Params: coding_p            Text coding scheme
 *                result_len_p        The length of the result
 *
 * Return:        The pointer to the converted result
 *
 * Notes: It should have already been checked that the text length > 0.
 *
 *************************************************************************/
uint8_t * ste_apdu_decode_alhpa_id( ste_apdu_alpha_identifier_t * alpha_p,
                                    ste_sim_text_coding_t       * coding_p,
                                    size_t                      * result_len_p)
{
    uint8_t * result_p = NULL;
    uint8_t * source_p = NULL;
    size_t    length = 0;

    if (!alpha_p || !coding_p || !result_len_p)
    {
        STE_SAT_LOG_ERROR("ste_apdu_decode_alhpa_id: incorrect input params.\n");
        return NULL;
    }

    if (!alpha_p->alpha_id_p || alpha_p->alpha_id_length == 0)
    {
        STE_SAT_LOG_ERROR("ste_apdu_decode_alhpa_id: alpha id has no data.\n");
        return NULL;
    }

    source_p = alpha_p->alpha_id_p;
    length = alpha_p->alpha_id_length;

    /* Determine coding type from the first byte*/
    *coding_p = GetCodingOfAlphaField(*source_p);

    /* Check if the text ends with 0xFF characters only if not USC2 coded: */
    switch (*source_p)                /* check first byte */
    {
        case UCS2_FORMAT:
        case UCS2_81_PREFIX_FORMAT:
        case UCS2_82_PREFIX_FORMAT:
        {
            /* Copy text from next byte as the UCS2 format byte is not required in the string */
            source_p++;
            length--;
        }
        break;

        default:
        {
            uint8_t  Index;

            for (Index = 0; Index < length; Index++)
            {
                if (*(source_p + Index) == 0xFF) /* 0xFF to indicate end of valid data */
                {
                    length = Index;
                    break;
                }
            }
        }
        break;
    } /* end switch */

    if (length > 0)
    { /* Copy text string */
        result_p = malloc(length + 1);
        if (!result_p)
        {
            STE_SAT_LOG_ERROR("ste_apdu_decode_alhpa_id: memory allocation failure.\n");
            return NULL;
        }
        memset(result_p, 0, length + 1);
        memcpy(result_p, source_p, length);
    } /* end Copy text string */
    else
    { /* Set Coding scheme to invalid */
        *coding_p = STE_SIM_CODING_UNKNOWN;
    } /* end Set Coding scheme to invalid */

    return (result_p);

}

/*************************************************************************
 * Function:      ste_apdu_parse_sms_control_response
 *
 * Description:   Decode the sc response APDU binary to the sms control info structure
 *
 * Input Params:  apdu:            response APDU binary to be decoded.
 *
 * Output Params: sc_rsp_p:        sms control response data strucure
 *
 * Return:        RetVal           Returns a valid error code in case of
 *                                 failure.
 *
 * Notes:
 *
 *************************************************************************/
ste_sat_apdu_error_t
ste_apdu_parse_sms_control_response(ste_cat_sms_control_response_t * sc_rsp_p,
                                    ste_apdu_t  * apdu,
                                    ste_cat_sms_control_result_t * sc_result_p)
{
    uint8_t  * TLV_Data_p;
    uint8_t    TagValue = 0;
    uint8_t    Length = 0;
    uint8_t    TLVLength = 0;
    uint8_t    RemainingLength = 0;
    boolean    smsc_exist = FALSE;
    boolean    Valid = TRUE;
    ste_sat_apdu_error_t rv = STE_SAT_APDU_ERROR_NONE;

    if (!sc_rsp_p || !sc_result_p)
    {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }

    memset(sc_rsp_p, 0, sizeof(ste_cat_sms_control_response_t));
    /* If any errors are found in response structure or format. The ME
     allow the user request to continue with no modification. */
    sc_rsp_p->sc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
    *sc_result_p = STE_CAT_CC_ALLOWED_NO_MODIFICATION;

    if (!apdu)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }
    if (!apdu->raw_data)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }
    TLV_Data_p = apdu->raw_data;

    if (*TLV_Data_p > STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }

    /* The first byte is the sms control result */
    sc_rsp_p->sc_result = *TLV_Data_p;
    *sc_result_p = *TLV_Data_p;

    /* The next 1 or 2 bytes contain the length */
    TLV_Data_p++;
    if (*TLV_Data_p == TWO_BYTES_LENGTH_IND)
    {
        TLV_Data_p++;

        if (*TLV_Data_p < MIN_LENGTH_BYTE2)
        {
            STE_SAT_LOG_ERROR("sms Control envelope response error in length byte 2: 0x%x\n", *TLV_Data_p);
            return STE_SAT_APDU_ERROR_CORRUPTED_DATA;
        }
    }
    else if (*TLV_Data_p > MAX_LENGTH_BYTE1)
    {
        STE_SAT_LOG_ERROR("sms Control envelope response error in length byte 1: 0x%x\n", *TLV_Data_p);
        return STE_SAT_APDU_ERROR_CORRUPTED_DATA;
    }

    Length = *TLV_Data_p;

    TLV_Data_p++;

    RemainingLength = Length;
    if (RemainingLength <= 2)
    {
        return STE_SAT_APDU_ERROR_EMPTY_DATA;
    }

    while (RemainingLength > 0)
    { /* Loop through the response envelope and parse each TLV */
        // Remove ComprehensionRequired Flag
        TagValue = *TLV_Data_p & ~COMPREHENSION_REQUIRED_MASK;

        TLVLength = *(TLV_Data_p + 1);

        switch (TagValue)
        {
            case ADDRESS_TAG:
            {
                if (!smsc_exist)
                {
                    ConvertAddressTLV_To_DialledAddress(TLV_Data_p, &(sc_rsp_p->sc_info.smsc));
                    // To indicate that the first Address TLV was found
                    smsc_exist = TRUE;
                }
                else
                {
                    ConvertAddressTLV_To_DialledAddress(TLV_Data_p, &(sc_rsp_p->sc_info.dest));
                }
            }
            break;

            case ALPHA_IDENTIFIER_TAG:
            {
                ConvertAlphaIdTLV_To_UserInd(TLV_Data_p, &(sc_rsp_p->user_indication));
            }
            break;

            default:
            {
                // An invalid tag type has been found, do not act on SIM response
                STE_SAT_LOG_ERROR("SMS Control envelope error, invalid tag found: 0x%x\n", *TLV_Data_p);
                sc_rsp_p->sc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
                *sc_result_p = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
                Valid = FALSE;
                /* Possible error in TLV length, Abort further processing */
                RemainingLength = 0;
                rv = STE_SAT_APDU_ERROR_CORRUPTED_DATA;
            }
        } /* end switch */

        if (Valid)
        {
            RemainingLength -= (TLVLength + 2);
            TLV_Data_p += (TLVLength + 2);
        }

    } /* end Loop through the response envelope and parse each TLV */

    return rv;
}

static uint8_t create_event_network_search_mode_tlv(cn_registration_info_t * reg_info_p,
                                                    uint8_t                * Env_p)
{
    uint8_t   Size = 0;
    uint8_t  DeviceTag = DEVICE_IDENTITIES_TAG;

    //Device identification
    *Env_p++ = DeviceTag;            /* TAG */
    *Env_p++ = DEVICE_IDENTITIES_LENGTH;       /* LENGTH */
    *Env_p++ = DEVICE_ME;   /* VALUE */
    *Env_p++ = DEVICE_SIM;

    Size += DEVICE_IDENTITIES_TOTAL_SIZE;

    *Env_p++ = NETWORK_SEARCH_MODE_TAG;
    *Env_p++ = 1;  // Length of network search mode TLV

    if (reg_info_p->search_mode == CN_NETWORK_SEARCH_MODE_AUTOMATIC)
    {
        *Env_p++ = 1;
    }
    else if (reg_info_p->search_mode == CN_NETWORK_SEARCH_MODE_MANUAL)
    {
        *Env_p++ = 0;
    }
    Size += 3; //Network search mode TLV

    return Size;
}

static uint8_t create_event_rat_mode_tlv(cn_registration_info_t * reg_info_p,
                                         uint8_t                * Env_p)
{
    uint8_t   Size = 0;
    uint8_t  DeviceTag = DEVICE_IDENTITIES_TAG;

    //Device identification
    *Env_p++ = DeviceTag;            /* TAG */
    *Env_p++ = DEVICE_IDENTITIES_LENGTH;       /* LENGTH */
    *Env_p++ = DEVICE_ME;   /* VALUE */
    *Env_p++ = DEVICE_SIM;

    Size += DEVICE_IDENTITIES_TOTAL_SIZE;

    *Env_p++ = ACCESS_TECHNOLOGY_TAG;
    *Env_p++ = 1;

    switch (reg_info_p->rat)
    {
        case CN_RAT_TYPE_GSM:
        case CN_RAT_TYPE_GPRS:
        case CN_RAT_TYPE_EDGE:
        {
            *Env_p++ = 0x0;
        }
        break;
        case CN_RAT_TYPE_UMTS:
        case CN_RAT_TYPE_HSDPA:
        case CN_RAT_TYPE_HSUPA:
        case CN_RAT_TYPE_HSPA:
        {
            *Env_p++ = 0x03;
        }
        break;
        case CN_RAT_TYPE_UNKNOWN:
        case CN_RAT_TYPE_IS95A:
        case CN_RAT_TYPE_IS95B:
        case CN_RAT_TYPE_1xRTT:
        case CN_RAT_TYPE_EVDO_REV_0:
        case CN_RAT_TYPE_EVDO_REV_A:
        default:
        {
            STE_SAT_LOG_ERROR("create_event_rat_mode_tlv: unexpected rat:%d.\n",reg_info_p->rat);
            return 0;
        }
    }

    Size += 3; // Rat mode tlv

    return Size;
}

static uint8_t create_event_location_status_tlv(cn_cell_info_t * cell_info_p,
                                                uint8_t        * Env_p)
{
    uint8_t   Size = 0;
    uint8_t  DeviceTag = DEVICE_IDENTITIES_TAG;
    boolean ExtendedCellInfo = FALSE;

    //Device identification
    *Env_p++ = DeviceTag;            /* TAG */
    *Env_p++ = DEVICE_IDENTITIES_LENGTH;       /* LENGTH */
    *Env_p++ = DEVICE_ME;   /* VALUE */
    *Env_p++ = DEVICE_SIM;

    Size += DEVICE_IDENTITIES_TOTAL_SIZE;

    //Location status
    *Env_p++ = LOCATION_STATUS_TAG;
    *Env_p++ = 1;  //Length of status TLV

    //Location service status
    if (cell_info_p->service_status == CN_NW_SERVICE)
    {
        *Env_p++ = 0x00;
    }
    else if (cell_info_p->service_status == CN_NW_LIMITED_SERVICE)
    {
        *Env_p++ = 0x01;
    }
    else if (cell_info_p->service_status == CN_NW_NO_COVERAGE)
    {
        *Env_p++ = 0x02;
    }

    Size += 3;

    if (cell_info_p->service_status == CN_NW_SERVICE) { //Ska bara fylla i location info om Normal Service
        //Location information
        *Env_p++ = LOCATION_INFORMATION_TAG;

        //Length of location information depends on rat mode
        if(cell_info_p->rat == CN_CELL_RAT_GSM)
        {
            *Env_p++ = 0x07; //Length = 7 bytes if GERAN or GSM, otherwise 9 bytes
            ExtendedCellInfo = FALSE;
        }
        else
        {
            *Env_p++ = 0x09;
            ExtendedCellInfo = TRUE;
        }

        //MCC & MNC
        create_bcd_from_mcc_mnc((uint8_t*)cell_info_p->mcc_mnc, Env_p);
        Env_p += 3;

        //Location area code
        *Env_p++ = (uint8_t) ((cell_info_p->current_ac >> 8) & 0xff);
        *Env_p++ = (uint8_t) cell_info_p->current_ac;

        //Cell id
        *Env_p++ = (uint8_t) ((cell_info_p->current_cell_id >> 8) & 0xff);
        *Env_p++ = (uint8_t) cell_info_p->current_cell_id;

        Size += 9;

        //Extended cell info
        if (ExtendedCellInfo)
        {
            *Env_p++ = (uint8_t) ((cell_info_p->current_cell_id >> 24) & 0xff);
            *Env_p++ = (uint8_t) ((cell_info_p->current_cell_id >> 16) & 0xff);
            Size += 2;
        }
    }
    return Size;
}

static uint8_t create_event_mt_Call_tlv(cn_call_context_t * call_context_p,
                                        uint8_t           * Env_p)
{
    uint8_t Size = 0;
    uint16_t Length = 0;
    uint8_t DeviceTag = DEVICE_IDENTITIES_TAG;
    uint8_t* number_text = NULL;
    ste_sim_text_t source_text;
    int rc = -1;
    int i;

    //Device indetification
    *Env_p++ = DeviceTag;
    *Env_p++ = DEVICE_IDENTITIES_LENGTH;
    *Env_p++ = DEVICE_NETWORK;
    *Env_p++ = DEVICE_SIM;

    Size += DEVICE_IDENTITIES_TOTAL_SIZE;

    //Transaction identfier
    *Env_p++ = TRANSACTION_IDENTIFIER_TAG;
    *Env_p++ = 1;
    *Env_p++ = call_context_p->transaction_id;

    Size += 3; // Length of whole transaction part

    //Address
    if ((strlen((char *)call_context_p->number)) > 0) {
        size_t len = strlen((char *)call_context_p->number);
        *Env_p++ = ADDRESS_TAG;
        Size++;

        source_text.text_p = malloc(len + 1);

        if (!source_text.text_p) {
            STE_SAT_LOG_ERROR("create_event_mt_Call_tlv : Could not allocate memory! \n");
            return 0;
        }

        memset(source_text.text_p, 0, len + 1);

        source_text.text_coding = STE_SIM_ASCII8;
        source_text.no_of_characters = strlen((char *)call_context_p->number);
        number_text = malloc(source_text.no_of_characters + sizeof(char));
        memcpy(source_text.text_p,call_context_p->number, source_text.no_of_characters);

        rc =  utility_ASCII8_to_BCD(&source_text,
                                    number_text,
                                    10,
                                    &Length);

        if(rc!=0) {
            //error, what to do?
            STE_SAT_LOG_ERROR("create_event_mt_Call_tlv : utility_ASCII8_to_BCD : "
                    "error, couldnt convert from ASCII8 to BCD \n ");
        } else {

            *Env_p++ = (uint8_t)Length + 1;
            *Env_p++ = call_context_p->address_type;

            for(i=0;i<Length;i++) {
                *Env_p++ = *(number_text + i);
            }
            //Added size of AddressTLV
            Size += Length + 2;
        }
        //Subaddress not supported!

        free(source_text.text_p);
        free(number_text);
    }
    return Size;
}

static uint8_t create_event_call_connected_tlv(cn_call_context_t * call_context_p,
                                               uint8_t           * Env_p)
{
    uint8_t Size = 0;
    uint8_t DeviceTag = DEVICE_IDENTITIES_TAG;

    //Device indetification
    *Env_p++ = DeviceTag;
    *Env_p++ = DEVICE_IDENTITIES_LENGTH;

    if(call_context_p->is_MT)
    {
        *Env_p++ = DEVICE_ME;
        *Env_p++ = DEVICE_SIM;
    }
    else
    {
        *Env_p++ = DEVICE_NETWORK;
        *Env_p++ = DEVICE_SIM;
    }

    Size += DEVICE_IDENTITIES_TOTAL_SIZE;

    //Transaction identfier
    *Env_p++ = TRANSACTION_IDENTIFIER_TAG;
    *Env_p++ = 1;
    *Env_p++ = call_context_p->transaction_id;

    Size += 3; // Length of whole transaction part

    return Size;
}

static uint8_t create_event_call_disconnected_tlv(cn_call_context_t * call_context_p,
                                                  uint8_t           * Env_p)
{
    uint8_t Size = 0;
    uint8_t DeviceTag = DEVICE_IDENTITIES_TAG;
    int i;

    //Device indetification
    *Env_p++ = DeviceTag;
    *Env_p++ = DEVICE_IDENTITIES_LENGTH;

    if(call_context_p->call_cause.cause_type_sender == CN_CALL_CAUSE_TYPE_SENDER_NETWORK) {
        *Env_p++ = DEVICE_NETWORK;
        *Env_p++ = DEVICE_SIM;
    }
    else {
        *Env_p++ = DEVICE_ME;
        *Env_p++ = DEVICE_SIM;
    }

    Size += DEVICE_IDENTITIES_TOTAL_SIZE;

    //Transaction identfier
    *Env_p++ = TRANSACTION_IDENTIFIER_TAG;
    *Env_p++ = 1;
    *Env_p++ = call_context_p->transaction_id;

    Size += 3; // Length of whole transaction part

    // If cause length is zero and cause sender is server we assume "radio link failure"
    if (call_context_p->call_cause.detailed_cause_length > 0 ||
            (call_context_p->call_cause.cause_type_sender == CN_CALL_CAUSE_TYPE_SENDER_SERVER &&
                    call_context_p->call_cause.detailed_cause_length == 0)) {

        //Cause
        *Env_p++ = CAUSE_TAG;
        *Env_p++ = call_context_p->call_cause.detailed_cause_length;
        for(i=0; i<call_context_p->call_cause.detailed_cause_length; i++) {
            *Env_p++ = call_context_p->call_cause.detailed_cause[i];
        }

        Size += 2 + call_context_p->call_cause.detailed_cause_length;
    }

    return Size;
}




/*************************************************************************
 * Function:      ste_apdu_event_download
 *
 * Description:   to create an apdu for event download
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_event_download(cn_message_type_t event_type,
                                      void * event_data_p,
                                      ste_sat_kind_of_event_requested kind_of_event)
{
    ste_apdu_t*                 apdu_p = NULL;
    unsigned                    total_len = 0;
    uint8_t*                    Env_p;
    uint8_t*                    Length_p = NULL;
    uint8_t                     Length = 0;
    uint8_t                     EC_TLV_buf[STE_SAT_MAX_APDU_SIZE];

    //init the buffer
    memset(EC_TLV_buf, 0, STE_SAT_MAX_APDU_SIZE);

    Env_p = EC_TLV_buf;

    /* Build the data for ENVELOPE(SMS CONTROL): */
    *Env_p++ = STE_SAT_EVENT_DOWNLOAD_TAG;
    Length_p = Env_p;  // => first byte of total length of SIMPLE-TLVs

    /*
      Reserve one byte to be used if the total length exceeds 0x7F. The two
      bytes length indication will be overwritten if one byte length value.
    */

    *Length_p = TWO_BYTES_LENGTH_IND;
    Length_p++; // point to second length byte
    Env_p++; //Length can be 1 or 2 bytes!

    Env_p++; /* Move to the start of the Event list field */

    *Env_p++ = EVENT_LIST_TAG | COMPREHENSION_REQUIRED_MASK; /* => Tag */
    *Env_p++ = 1;             /* => Only one event list item Ref.TS 31.111 7.5.1 */
    *Length_p = SIZE_EVENT_LIST_TLV;

    //now based on the event type, we encode for the other bytes for different TLVs
    switch (event_type)
    {
        case CN_EVENT_MODEM_REGISTRATION_STATUS:
        {

            cn_registration_info_t * reg_info_p = (cn_registration_info_t*) event_data_p;
            if (!reg_info_p)
            {
                //something wrong with the data, return NULL
                return NULL;
            }

            if (kind_of_event == STE_SAT_KIND_OF_EVENT_REQUESTED_SEARCH_MODE)
            {
                *Env_p++ = 0x0E;
                Length = create_event_network_search_mode_tlv(reg_info_p, Env_p);
            }

            if (kind_of_event == STE_SAT_KIND_OF_EVENT_REQUESTED_RAT_MODE)
            {
                *Env_p++ = 0x0B;
                Length = create_event_rat_mode_tlv(reg_info_p, Env_p);
                if (Length == 0) //Error unexpected rat mode!
                {
                    return NULL;
                }
            }
        }
        break;

        case CN_EVENT_CELL_INFO:
        {
            cn_cell_info_t * cell_info_p = (cn_cell_info_t*) event_data_p;
            *Env_p++ = 0x03;
            Length = create_event_location_status_tlv(cell_info_p, Env_p);
        }
        break;

        case CN_EVENT_CALL_STATE_CHANGED:
        {
            cn_call_context_t * call_context_p = (cn_call_context_t*) event_data_p;

            if (kind_of_event == STE_SAT_KIND_OF_EVENT_REQUESTED_CALL_CONNECTED)
            {
                *Env_p++ = 0x01;
                Length = create_event_call_connected_tlv(call_context_p, Env_p);
            }
            else if (kind_of_event == STE_SAT_KIND_OF_EVENT_REQUESTED_CALL_DISCONNECTED)
            {
                *Env_p++ = 0x02;
                Length = create_event_call_disconnected_tlv(call_context_p, Env_p);
            }
            else if (kind_of_event == STE_SAT_KIND_OF_EVENT_REQUESTED_INCOMING)
            {
                *Env_p++ = 0x00;
                Length = create_event_mt_Call_tlv(call_context_p, Env_p);
            }
        }
        break;
        default:
        {
            return NULL;
        }
        break;
    }

    *Length_p += Length;
    Env_p += Length; /* Move to the start of next field */

    /* Determine if the size is contained in one or two bytes */
    if (*Length_p <= MAX_LENGTH_BYTE1)
    {
        /* Overall size is the length plus 2 bytes for the tag and length */
        total_len = (*Length_p) + 2;
        /* Remove the additional size byte */
        memmove(Length_p - 1, Length_p, *Length_p + 1);
    }
    else
    {
        /* Overall size is the length plus 3 bytes for the tag and length */
        total_len = (*Length_p) + 3;
    }

    apdu_p = ste_apdu_new(EC_TLV_buf, total_len);

    return apdu_p;
}



/*************************************************************************
 * Function:      ste_apdu_uicc_activate_file
 *
 * Description:   Encode the APDU binary for activating a file
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:        The created APDU binary
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_uicc_activate_file()
{
    ste_apdu_t                 *apdu_p = NULL;
    uint8_t                     APDU_buf[] = { STE_UICC_CLASS,
                                               STE_UICC_INS_ACTIVATE_FILE,
                                               STE_UICC_ACTIVATE_FILE_P1_SELECT_BY_FILE_ID,
                                               STE_UICC_ACTIVATE_FILE_P2,
                                               STE_UICC_ACTIVATE_FILE_P3 } ;

    apdu_p = ste_apdu_new(APDU_buf, sizeof(APDU_buf));

    return apdu_p;

}


/*************************************************************************
 * Function:      ste_apdu_uicc_deactivate_file
 *
 * Description:   Encode the APDU binary for deactivating a file
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:        The created APDU binary
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_uicc_deactivate_file()
{
    ste_apdu_t                 *apdu_p = NULL;
    uint8_t                     APDU_buf[] = { STE_UICC_CLASS,
                                               STE_UICC_INS_DEACTIVATE_FILE,
                                               STE_UICC_DEACTIVATE_FILE_P1_SELECT_BY_FILE_ID,
                                               STE_UICC_DEACTIVATE_FILE_P2,
                                               STE_UICC_DEACTIVATE_FILE_P3 } ;

    apdu_p = ste_apdu_new(APDU_buf, sizeof(APDU_buf));

    return apdu_p;

}



/*************************************************************************
 * Function:      ste_apdu_uicc_select_file
 *
 * Description:   Encode the APDU binary for selecting a file
 *
 * Input Params:
 *
 * Output Params:
 *
 * Return:        The created APDU binary
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_uicc_select_file()
{
    ste_apdu_t                 *apdu_p = NULL;
    uint8_t                     APDU_length = 0;

    uint8_t                     APDU_buf[] = { STE_UICC_CLASS,
                                               STE_UICC_INS_SELECT_FILE,
                                               STE_UICC_SELECT_FILE_P1_SELECT_BY_FILE_ID,
                                               STE_UICC_SELECT_FILE_P2,
                                               STE_UICC_SELECT_FILE_P3} ;


    APDU_length = sizeof(APDU_buf);

    apdu_p = ste_apdu_new(APDU_buf, APDU_length);

    return apdu_p;
}



/*************************************************************************
 * Function:      ste_apdu_uicc_read_record
 *
 * Description:   Encode the APDU binary for reading a record from a file
 *
 * Input Params:  record_no      record number to be read
 *                record_mode    specifies record to be read:
 *                               - '02' = next record;
 *                               - '03' = previous record;
 *                               - '04' = absolute mode/current mode, the record number is given by record_no with
 *                                        record_no='00' denoting the current record.
 *                data_length    length of data to be read
 *
 * Output Params:
 *
 * Return:        The created APDU binary
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t  * ste_apdu_uicc_read_record(uint8_t record_no, uint8_t record_mode, uint8_t data_length)
{
    ste_apdu_t                 *apdu_p = NULL;
    uint8_t                     APDU_length = 0;

    uint8_t                     APDU_buf[] = { STE_UICC_CLASS,
                                               STE_UICC_INS_READ_RECORD,
                                               0, 0, 0 } ;

    APDU_length = sizeof(APDU_buf);

    APDU_buf[2] = record_no;
    APDU_buf[3] = record_mode;
    APDU_buf[4] = data_length;

    apdu_p = ste_apdu_new(APDU_buf, APDU_length);

    return apdu_p;
}

/*************************************************************************
 * Function:      ste_apdu_timer_expiration_ec
 *
 * Description:   To create an apdu for timer expiration envelope command.
 *
 * Input Params:  timer_id_pc:  Timer ID given in proactive command.
 *                hours:        Number of hours.
 *                minutes:      Number of minutes.
 *                seconds:      Number of seconds.
 *
 * Output Params: None.
 *
 * Return:    Returns a valid APDU if successful, otherwise NULL.
 *
 * Notes:
 *
 *************************************************************************/
ste_apdu_t   *ste_apdu_timer_expiration_ec(const ste_apdu_timer_identifier_code_value_t  timer_id_pc,
                                           const uint8_t hours,
                                           const uint8_t minutes,
                                           const uint8_t seconds)
{
    ste_apdu_t*      apdu_p = NULL;
    uint8_t*         Env_p;
    uint8_t          EC_TLV_buf[STE_SAT_MAX_APDU_SIZE];


    // Init the buffer.
    memset(EC_TLV_buf, 0, STE_SAT_MAX_APDU_SIZE);

    Env_p = EC_TLV_buf;

    // Build the APDU for ENVELOPE COMMAND (TIMER EXPIRATION).
    *Env_p++ = STE_SAT_TIMER_EXPIRATION_TAG;
    *Env_p++ = 0x0C;
    *Env_p++ = DEVICE_IDENTITIES_TAG;
    *Env_p++ = DEVICE_IDENTITIES_LENGTH;
    *Env_p++ = DEVICE_ME;    // Source.
    *Env_p++ = DEVICE_SIM;   // Destination.
    *Env_p++ = TIMER_IDENTIFIER_TAG;
    *Env_p++ = TIMER_IDENTIFIER_LENGTH;
    *Env_p++ = timer_id_pc;
    *Env_p++ = TIMER_VALUE_TAG;
    *Env_p++ = TIMER_VALUE_LENGTH;
    *Env_p++ = ste_apdu_encode_bcd(hours);
    *Env_p++ = ste_apdu_encode_bcd(minutes);
    *Env_p++ = ste_apdu_encode_bcd(seconds);
    // Try to allocate memory for this APDU data.
    apdu_p = ste_apdu_new(EC_TLV_buf, 0x0E);
    return apdu_p;
}

int bit_buffer_init(bit_buffer_t* buffer, size_t size)
{
    if (buffer) {
        size_t bytes = (size >> 3) + ((size & 0x07) ? 1 : 0);

        if (bytes == 0) {
            bytes = 1;
        }

        buffer->storage = NULL;
        buffer->size    = 0;
        buffer->used    = 0;

        buffer->storage = calloc(sizeof(uint8_t), bytes);
        if (buffer->storage) {
            buffer->size = bytes << 3;
            return 0;
        }
    }
    return -1;
}

void bit_buffer_deinit(bit_buffer_t* buffer)
{
    if (buffer) {
        free(buffer->storage);
        buffer->storage = NULL;
        buffer->size    = 0;
        buffer->used    = 0;
    }
}

int bit_buffer_length(bit_buffer_t* buffer)
{
    if (buffer) {
        return (buffer->used >> 3) + ((buffer->used & 0x07) ? 1 : 0);
    }
    return 0;
}

uint8_t bit_buffer_append_bits(bit_buffer_t* buffer, unsigned bits, uint8_t num_bits)
{
    int      bits_size = 8 * sizeof(bits) - 1;
    unsigned mask      = ~(1 << bits_size); // All ones except the most significant bit.
    uint8_t  stored    = 0;
    uint8_t  remaining;
    uint8_t  byte;
    int      index;
    int      shift;

    if (buffer                                        &&
       (num_bits > 0)                                 &&
       (num_bits <= bits_size)                        &&
       ((buffer->used + num_bits) <= (buffer->size))) {

        shift = bits_size - num_bits;
        mask >>= shift;
        bits &= mask; // Clear the 'sign bit' as well as all bits that shouldn't be stored.

        index     = buffer->used >> 3;
        remaining = 8 - (buffer->used & 0x07);
        stored    = (remaining < num_bits) ? remaining : num_bits;

        shift = num_bits - remaining;
        if (shift < 0) {
            byte = (uint8_t) (bits << (-shift));
        } else if (shift > 0) {
            byte = (uint8_t) (bits >> shift);
        } else {
            byte = (uint8_t) (bits);
        }

        buffer->storage[index] |= byte;
        buffer->used += stored;

        // Recursively store more bits if not all has been stored...
        if (stored < num_bits) {
            stored += bit_buffer_append_bits(buffer, bits, num_bits - stored);
        }
    }
    return stored;
}

/*
 * This routine takes a letter presenting a hex digit and returns the
 * equivalent decimal number. This is not hex to decimal conversion; it is
 * mere conversion of ASCII value into a real number. For example, if the
 * letter is passed as '5', it is really a number 53 and not 5. The return
 * value in this case will be 5.
 * param [letter]   Letter as an input
 * return           Decimal value as presented by the letter;
 *                  0xFF, if input is not a valid "hex letter"
 */
static uint8_t convert_ascii_to_decimal(uint8_t letter)
{
    switch (letter) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return letter - '0';
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return letter - 'a' + 0xA;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return letter - 'A' + 0xA;

    default:
        return 0xFF;
    }
}

/*
 * This routine decodes the string containing MCC and MNC to provide
 * 3 bytes originally used to create the string.
 * param [mcc_mnc_string]   Pointer to string created
 * param [plmn]             Pointer to the result
 * return                   void
 */
void create_bcd_from_mcc_mnc(uint8_t *mcc_mnc_string, uint8_t *plmn)
{
    uint8_t nibble1 = 0;
    uint8_t nibble2 = 0;
    uint8_t nibble3 = 0;
    uint8_t nibble4 = 0;
    uint8_t nibble5 = 0;
    uint8_t nibble6 = 0;
    uint8_t len = 0;

    if (mcc_mnc_string == NULL || plmn == NULL ) {
        return;
    }

    nibble1 = convert_ascii_to_decimal(mcc_mnc_string[0]);
    nibble2 = convert_ascii_to_decimal(mcc_mnc_string[1]);
    nibble3 = convert_ascii_to_decimal(mcc_mnc_string[2]);

    len = strlen((char *)mcc_mnc_string);

    switch (len) {
    case 4:
    case 5:
        nibble4 = 0xF;
        nibble5 = convert_ascii_to_decimal(mcc_mnc_string[3]);
        nibble6 = convert_ascii_to_decimal(mcc_mnc_string[4]);
        break;
    case 6:
        nibble4 = convert_ascii_to_decimal(mcc_mnc_string[5]);
        nibble5 = convert_ascii_to_decimal(mcc_mnc_string[3]);
        nibble6 = convert_ascii_to_decimal(mcc_mnc_string[4]);
        break;
    }

    *plmn++ = (nibble2 << 4) | nibble1;
    *plmn++ = (nibble4 << 4) | nibble3;
    *plmn++ = (nibble6 << 4) | nibble5;

}

ste_sat_apdu_error_t  ste_apdu_get_event_from_event_download(ste_apdu_t *apdu_p, uint8_t* event_id_p )
{
    uint8_t                    *raw_p;
    raw_p = apdu_p->raw_data;

    if (*raw_p != STE_SAT_EVENT_DOWNLOAD_TAG) {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    raw_p++;

    if (*raw_p == TWO_BYTES_LENGTH_IND) {
        raw_p++;
    }
    raw_p++;

    if ((*raw_p & ~COMPREHENSION_REQUIRED_MASK) != EVENT_LIST_TAG) {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    raw_p++;

    if ((*raw_p ) != 1) {
        return STE_SAT_APDU_ERROR_INVALID_PARAM;
    }
    raw_p++;

    *event_id_p = *raw_p;

    return STE_SAT_APDU_ERROR_NONE;
}
