/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : apdu.h
 * Description     : apdu interface
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */


// FIXME: Move internal definitions to apdu_utilities.h

#ifndef __apdu_h__
#define __apdu_h__ (1)

#include "sim.h"
#include "cn_message_types.h"
#include <sys/types.h>
#include <stdint.h>

typedef int             boolean;
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif



//#############################################################################
// Temporary definitions for some structures that should be done
// outside of SAT module
//#############################################################################

#define MAX_SIZE_IMEI 15
#define MAX_SIZE_IMEISV 16
#define SVN_SIZE 2

/** This struct contains the IMEI.
  * @param Length                 The length of the IMEI that is transmited over the air
  *                               Note, this is not the length of the Digit field.
  * @param TypeOfIdentity         The type of ID that should be transmitted over the air.
  * @param OddEvenIndication      The odd/even indication that should be transmitted over the air.
  * @param Digit                  The actual IMEI in binary format, needs to be
  *                               packed to be transmitted over the air
  */
typedef struct {
    uint8_t                   Length;
    uint8_t                   TypeOfIdentity;
    uint8_t                   OddEvenIndication;
    uint8_t                   Digit[MAX_SIZE_IMEI];
} IMEI_t;

typedef IMEI_t          ste_cmd_result_imei_t;

/** This struct contains the IMEISV.
  * @param Length                 The length of the IMEISV that is transmited over the air
  *                               Note, this is not the length of the Digit field.
  * @param TypeOfIdentity         The type of ID that should be transmitted over the air.
  * @param OddEvenIndication      The odd/even indication that should be transmitted over the air.
  * @param SVN                    Software Version Number of the mobile equipment.
  * @param Digit                  The actual IMEISV in binary format, needs to be
  *                               packed to be transmitted over the air
  */
typedef struct {
    uint8_t                   Length;
    uint8_t                   TypeOfIdentity;
    uint8_t                   OddEvenIndication;
    uint8_t                   SVN[SVN_SIZE];
    uint8_t                   Digit[MAX_SIZE_IMEISV];
} IMEISV_t;

typedef IMEISV_t          ste_cmd_result_imeisv_t;
//#############################################################################


typedef enum {
    STE_SAT_APDU_ERROR_NONE = 0,
    STE_SAT_APDU_ERROR_INVALID_PARAM,
    STE_SAT_APDU_ERROR_CORRUPTED_DATA,
    STE_SAT_APDU_ERROR_MEMORY_ALLOCATION_FAILURE,
    STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING,
    STE_SAT_APDU_ERROR_REQUIRED_VALUES_INCORRECT,
    STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
    STE_SAT_APDU_ERROR_COMMAND_BEYOND_ME_CAPABILITY,
    STE_SAT_APDU_ERROR_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
    STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION,
    STE_SAT_APDU_ERROR_EMPTY_DATA,
    STE_SAT_APDU_ERROR_UNKNOWN,
    STE_SAT_APDU_ERROR_TERMINATED
} ste_sat_apdu_error_t;

typedef enum {
    STE_SAT_KIND_OF_EVENT_REQUESTED_SEARCH_MODE,
    STE_SAT_KIND_OF_EVENT_REQUESTED_RAT_MODE,
    STE_SAT_KIND_OF_EVENT_REQUESTED_INCOMING,
    STE_SAT_KIND_OF_EVENT_REQUESTED_CALL_CONNECTED,
    STE_SAT_KIND_OF_EVENT_REQUESTED_CALL_DISCONNECTED,
    STE_SAT_KIND_OF_EVENT_REQUESTED_LOCATION_STATUS
} ste_sat_kind_of_event_requested;

// The apdu data.
struct ste_apdu_s {
    size_t                  raw_length;
    uint8_t                *raw_data;
};

// Abstract representation of an APDU
typedef struct ste_apdu_s ste_apdu_t;

// Abstract representation of a parsed APDU
typedef struct ste_parsed_apdu_s ste_parsed_apdu_t;


#define STE_APDU_KIND_NONE      (0)
#define STE_APDU_KIND_PC        (1)
#define STE_APDU_KIND_TR        (2)
#define STE_APDU_KIND_EC        (3)
#define STE_APDU_KIND__MAX      (4)

/* Command Types, 3GPP TS 31.111 and ETSI TS 102 223 v5.11.0 section 9.4  */
#define STE_APDU_CMD_TYPE_NONE                   (0x00)
#define STE_APDU_CMD_TYPE_REFRESH                (0x01)
#define STE_APDU_CMD_TYPE_MORE_TIME              (0x02)
#define STE_APDU_CMD_TYPE_POLL_INTERVAL          (0x03)
#define STE_APDU_CMD_TYPE_POLLING_OFF            (0x04)
#define STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST      (0x05)
#define STE_APDU_CMD_TYPE_SET_UP_CALL            (0x10)
#define STE_APDU_CMD_TYPE_SEND_SS                (0x11)
#define STE_APDU_CMD_TYPE_SEND_USSD              (0x12)
#define STE_APDU_CMD_TYPE_SEND_SHORT_MESSAGE     (0x13)
#define STE_APDU_CMD_TYPE_SEND_DTMF              (0x14)
#define STE_APDU_CMD_TYPE_LAUNCH_BROWSER         (0x15)
#define STE_APDU_CMD_TYPE_PLAY_TONE              (0x20)
#define STE_APDU_CMD_TYPE_DISPLAY_TEXT           (0x21)
#define STE_APDU_CMD_TYPE_GET_INKEY              (0x22)
#define STE_APDU_CMD_TYPE_GET_INPUT              (0x23)
#define STE_APDU_CMD_TYPE_SELECT_ITEM            (0x24)
#define STE_APDU_CMD_TYPE_SET_UP_MENU            (0x25)
#define STE_APDU_CMD_TYPE_PROVIDE_LOCAL_INFO     (0x26)
#define STE_APDU_CMD_TYPE_TIMER_MANAGEMENT       (0x27)
#define STE_APDU_CMD_TYPE_SET_UP_IDLE_MODE_TEXT  (0x28)
#define STE_APDU_CMD_TYPE_RUN_AT                 (0x34)
#define STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION  (0x35)
#define STE_APDU_CMD_TYPE_OPEN_CHANNEL           (0x40)
#define STE_APDU_CMD_TYPE_CLOSE_CHANNEL          (0x41)
#define STE_APDU_CMD_TYPE_RECEIVE_DATA           (0x42)
#define STE_APDU_CMD_TYPE_SEND_DATA              (0x43)
#define STE_APDU_CMD_TYPE_GET_CHANNEL_STATUS     (0x44)
#define STE_APDU_CMD_TYPE_SERVICE_SEARCH         (0x45)
#define STE_APDU_CMD_TYPE_GET_SERVICE_INFOMATION (0x46)
#define STE_APDU_CMD_TYPE_DECLARE_SERVICE        (0x47)
#define STE_APDU_FIRST_RESERVED_COMMAND_TYPE     (0x48) /* Value from which SAT_RES_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME generated */

// Get/Set detailed data per type
/************************************************************************
 * APDU common used structures
 ***********************************************************************/
typedef struct {
    void                   *Data_p;     //real data, can be cast to the real data type
    void                   *Next_p;     //pointer to next in the list
} ste_apdu_node_t;

typedef struct {
    uint8_t                 NumberOfNodes;      //number of nodes
    ste_apdu_node_t        *Header_p;   //header
} ste_apdu_list_t;

typedef struct {
    uint8_t                 source_ID;
    uint8_t                 destination_ID;
} ste_apdu_device_id_t;

typedef struct {
    size_t                  alpha_id_length;
    uint8_t                *alpha_id_p;
} ste_apdu_alpha_identifier_t;

typedef struct {
    uint8_t                 icon_identifier;
    uint8_t                 icon_qualifier;
} ste_apdu_icon_identifier_t;

typedef struct {
    uint8_t                 item_identifier;
    uint8_t                 item_str_len;
    uint8_t                *item_str_p;
} ste_apdu_item_t;

typedef struct {
    uint8_t                 NextActionLen;
    uint8_t                *NextActionIndicator_p;
} ste_apdu_items_next_action_t;

typedef struct {
    uint8_t                  length;
    uint8_t                 *language_p;
} ste_apdu_language_t;

/**
 * Icon Identifier List
 *
 * @param DisplayWithAlphaId    Indicates whether the Icon should be displayed
 *                              with the text in the user indication or whether
 *                              it is self-explanatory.
 * @param IconIdList            Icon Id List
 */
typedef struct {
    boolean                 DisplayWithAlphaId;
    ste_apdu_list_t         IconIdList;
} ste_apdu_icon_id_list_t;

typedef struct {
    size_t                  text_attribute_length;
    uint8_t                *text_attribute_p;
} ste_apdu_text_attribute_t;

typedef struct {
    size_t                  text_length;
    uint8_t                 coding_scheme;
    uint8_t                *text_string_p;
} ste_apdu_text_string_t;

typedef struct {
    uint8_t                 ton_npi;
    size_t                  str_length;  // This is the SIZE of the coded dialing string
    uint8_t                *dial_text_p; // This is the dialing string coded according to the extended BCD coding described in TS 31.102, table 4.4 (same as for EFadn).
} ste_apdu_address_t;

typedef struct {
    uint8_t                 ccp_length;
    uint8_t                *ccp_p;
} ste_apdu_ccp_t;

typedef struct {
    uint8_t                 sub_address_len;
    uint8_t                *sub_address_p;
} ste_apdu_sub_address_t;

typedef struct {
    uint8_t                 min_length;
    uint8_t                 max_length;
} ste_apdu_response_length_t;

typedef struct {
    uint8_t                *data_p;
    size_t                  data_len;
} ste_apdu_sms_tpdu_t;

typedef struct {
    uint8_t                *data_p;
    size_t                  data_len;
} ste_apdu_dtmf_string_t;

/************************************************************************
 * DISPLAY TEXT
 ***********************************************************************/
/** Priority for Display Text */
typedef enum {
    SAT_NORMAL_PRIORITY = 0,
    SAT_HIGH_PRIORITY
} ste_apdu_text_priority_t;

/** Clear text mode */
typedef enum {
    SAT_CLEAR_AFTER_DELAY = 0,
    SAT_WAIT_FOR_USER_TO_CLEAR
} ste_apdu_clear_text_t;

typedef struct {
    ste_apdu_text_priority_t  priority;
    ste_apdu_clear_text_t     clear_text;
} ste_apdu_display_text_details_t;

typedef struct {
    uint8_t                          bit_mask;
#define       PC_DISPLAY_TEXT_TextString_present           0x01
#define       PC_DISPLAY_TEXT_IconIdentifier_present       0x02
#define       PC_DISPLAY_TEXT_ImmediateResponse_present    0x04
#define       PC_DISPLAY_TEXT_Duration_present             0x08
#define       PC_DISPLAY_TEXT_TextAttribute_present        0x10
#define       PC_DISPLAY_TEXT_FrameIdentifier_present      0x20
#define       PC_DISPLAY_TEXT_DeviceID_present             0x40
#define       PC_DISPLAY_TEXT_CommandDetails_present       0x80

    ste_apdu_text_string_t           text_string;
    ste_apdu_icon_identifier_t       icon_id;
    uint8_t                          immediate_response;
    unsigned int                     duration;   //time unit: TENS_OF_SECOND
    ste_apdu_text_attribute_t        text_attribute;
    uint8_t                          frame_identifier;
    ste_apdu_device_id_t             device_id;
    ste_apdu_display_text_details_t  command_details;
} ste_apdu_pc_display_text_t;

/************************************************************************
 * POLL INTERVAL
 ***********************************************************************/
typedef struct {
    uint8_t                   bit_mask;
#define       PC_POLL_INTERVAL_Duration_present            0x01
#define       PC_POLL_INTERVAL_DeviceID_present            0x02
    unsigned int              duration;
    ste_apdu_device_id_t      device_id;
} ste_apdu_pc_poll_interval_t;


/************************************************************************
 * PROVIDE LOCAL INFOMATION
 ***********************************************************************/

/** Provide local info type */
//NOTE: The numbering of ste_apdu_provide_local_info_type_t exactly matches the numbering of PROVIDE_LOCATION_INFORMATION, etc.,
// which is exactly the binary value of command qualifier. So if this is changed in one place, the other places need to be updated.
typedef enum {
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LOCATION_INFO = 0,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEI,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_NW_MEASURE_RESULT,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_DATE_TIME_ZONE,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_LANGUAGE,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_TIMING_ADVANCE,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ACCESS_TECH,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ESN,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_ME_IMEISV,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_SEARCH_MODE,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_BATTERY,
    STE_SAT_PROVIDE_LOCAL_INFO_TYPE_END
} ste_apdu_provide_local_info_type_t;

typedef struct {
    uint8_t                              bit_mask;
#define       PC_PROVIDE_LOCAL_INFO_Type_present           0x01
#define       PC_PROVIDE_LOCAL_INFO_DeviceID_present       0x02

    ste_apdu_provide_local_info_type_t   info_type;
    ste_apdu_device_id_t                 device_id;
    cn_nmr_rat_type_t                    nmr_rat_type;
    uint8_t                              nmr_utran_type;
} ste_apdu_pc_provide_local_info_t;

/* Battery states, Ref. TS 31.111 section 8.76 */
typedef enum {
    STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_VERY_LOW = 0x00,
    STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_LOW = 0x01,
    STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_AVERAGE = 0x02,
    STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_GOOD = 0x03,
    STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_FULL = 0x04,
    STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_NO_BATTERY = 0xFE, //Status not applicable - Not powered by a Battery
    STE_SAT_PROVIDE_LOCAL_INFO_BATTERY_STATE_UNKNOWN = 0xFF //Status Unknown - e.g. battery is charging.
} ste_apdu_provide_local_info_battery_state_t;

typedef struct {
    uint8_t                             nmr_rat_type;
    ste_cat_nmr_geran_t                 nmr_geran;
    ste_cat_nmr_utran_t                 nmr_utran;
} ste_apdu_pc_provide_local_nmr_info_t;

/* Date, Time and Timezone */

typedef struct {
    uint8_t             hours;
    uint8_t             minutes;
    uint8_t             seconds;
} ste_cat_time_t;

typedef struct {
    uint8_t             year;
    uint8_t             month;
    uint8_t             day;
} ste_cat_date_t;

typedef struct {
    int8_t              timezone;
    uint8_t             daylight;
} ste_cat_timezone_t;

typedef struct {
    ste_cat_date_t      date;
    ste_cat_time_t      time;
    ste_cat_timezone_t  timezone;
} ste_apdu_date_time_t;

/************************************************************************
 * SET UP EVENT LIST
 ***********************************************************************/

// 'Event' definitions for Set Up Event List command
#define STE_SAT_SET_UP_EVENT_LIST_BIT_SEED                              (0x00000001)

//The bit position for each event in event list, ex. the first bit
//(event_list &  (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED << STE_SAT_SET_UP_EVENT_LIST_MT_CALL)) of event_list is for MT CALL.
//Bit position value exactly matches the binary value in APDU
#define STE_SAT_SET_UP_EVENT_LIST_MT_CALL                               (0)
#define STE_SAT_SET_UP_EVENT_LIST_CALL_CONNECTED                        (1)
#define STE_SAT_SET_UP_EVENT_LIST_CALL_DISCONNECTED                     (2)
#define STE_SAT_SET_UP_EVENT_LIST_LOCATION_STATUS                       (3)
#define STE_SAT_SET_UP_EVENT_LIST_USER_ACTIVITY                         (4)
#define STE_SAT_SET_UP_EVENT_LIST_IDLE_SCREEN_AVAILABLE                 (5)
#define STE_SAT_SET_UP_EVENT_LIST_CARD_READER_STATUS                    (6)
#define STE_SAT_SET_UP_EVENT_LIST_LANGUAGE_SELECTION                    (7)
#define STE_SAT_SET_UP_EVENT_LIST_BROWSER_TERMINATED                    (8)
#define STE_SAT_SET_UP_EVENT_LIST_DATA_AVAILABLE                        (9)
#define STE_SAT_SET_UP_EVENT_LIST_CHANNEL_STATUS                        (10)
#define STE_SAT_SET_UP_EVENT_LIST_ACCESS_TECHNOLOGY_CHANGE              (11)
#define STE_SAT_SET_UP_EVENT_LIST_DISPLAY_PARAMETERS_CHANGED            (12)
#define STE_SAT_SET_UP_EVENT_LIST_LOCAL_CONNECTION                      (13)
#define STE_SAT_SET_UP_EVENT_LIST_NW_SEARCH_MODE_CHANGE                 (14)
#define STE_SAT_SET_UP_EVENT_LIST_BROWSING_STATUS                       (15)
#define STE_SAT_SET_UP_EVENT_LIST_FRAMES_INFO_CHANGE                    (16)
#define STE_SAT_SET_UP_EVENT_LIST_HCI_CONNECTIVITY_EVENT                (19)
#define STE_SAT_SET_UP_EVENT_LIST_END                                   (20)    //END should always be the last one

typedef struct {
    uint8_t                   bit_mask;
#define       PC_SET_UP_EVENT_LIST_Event_present           0x01
#define       PC_SET_UP_EVENT_LIST_DeviceID_present        0x02

    unsigned int              event_list;
    ste_apdu_device_id_t      device_id;
} ste_apdu_pc_set_up_event_list_t;


/************************************************************************
 * GET INPUT
 ***********************************************************************/
//Get Input Response
typedef enum {
    STE_APDU_GET_INPUT_DIGITS_AST_HASH_PLUS_ONLY,
    STE_APDU_GET_INPUT_ENTIRE_SMS_DEFAULT_ALPHABET,
    STE_APDU_GET_INPUT_UCS2_CODED       //if UCS2 is set, then PackFormat is ignored. PackFormat is not for UCS2
} ste_apdu_get_input_response_t;

//Get Input SMS packed/unpacked format
typedef enum {
    STE_APDU_GET_INPUT_UNPACKED,
    STE_APDU_GET_INPUT_SMS_PACKED
} ste_apdu_get_input_pack_format_t;

//Get Input Command Details TLV
typedef struct {
    ste_apdu_get_input_response_t ResponseType;
    ste_apdu_get_input_pack_format_t PackFormat;        //This pack format is not relevant to UCS2 coding.
    boolean                 EchoAllowed;
    boolean                 HelpInfoAvailable;
} ste_apdu_get_input_details_t;

typedef struct {
    uint8_t                        bit_mask;
#define       PC_GET_INPUT_DeviceID_present             0x01
#define       PC_GET_INPUT_TextString_present           0x02
#define       PC_GET_INPUT_ResponseLength_present       0x04
#define       PC_GET_INPUT_DefaultText_present          0x08
#define       PC_GET_INPUT_IconIdentifier_present       0x10
#define       PC_GET_INPUT_TextAttribute_present        0x20
#define       PC_GET_INPUT_FrameIdentifier_present      0x40
#define       PC_GET_INPUT_CommandDetails_present       0x80
    ste_apdu_device_id_t           device_id;
    ste_apdu_text_string_t         text_string;
    ste_apdu_response_length_t     response_length;
    ste_apdu_text_string_t         default_text;
    ste_apdu_icon_identifier_t     icon_id;
    ste_apdu_text_attribute_t      text_attribute;
    uint8_t                        frame_identifier;
    ste_apdu_get_input_details_t   command_details;
} ste_apdu_pc_get_input_t;

/************************************************************************
 * GET INKEY
 ***********************************************************************/
//Get Inkey Response
typedef enum {
    STE_APDU_GET_INKEY_YES_NO,
    STE_APDU_GET_INKEY_DIGITS_AST_HASH_PLUS_ONLY,
    STE_APDU_GET_INKEY_ENTIRE_SMS_DEFAULT_ALPHABET,
    STE_APDU_GET_INKEY_UCS2_CODED
} ste_apdu_get_inkey_response_t;

//Get Inkey Mode
typedef enum {
    STE_APDU_GET_INKEY_DISPLAY_RESPONSE,
    STE_APDU_GET_INKEY_IMMEDIATE_RESPONSE
} ste_apdu_get_inkey_mode_t;

//Get Inkey Command Details TLV
typedef struct {
    ste_apdu_get_inkey_response_t  ResponseType;
    ste_apdu_get_inkey_mode_t      Mode;
    boolean                        HelpInfoAvailable;
} ste_apdu_get_inkey_details_t;

typedef struct {
    uint8_t                       bit_mask;
#define       PC_GET_INKEY_DeviceID_present             0x01
#define       PC_GET_INKEY_TextString_present           0x02
#define       PC_GET_INKEY_IconIdentifier_present       0x04
#define       PC_GET_INKEY_Duration_present             0x08
#define       PC_GET_INKEY_TextAttribute_present        0x10
#define       PC_GET_INKEY_FrameIdentifier_present      0x20
#define       PC_GET_INKEY_CommandDetails_present       0x40
    ste_apdu_device_id_t          device_id;
    ste_apdu_text_string_t        text_string;
    ste_apdu_icon_identifier_t    icon_id;
    unsigned int                  duration;   //time unit: TENS_OF_SECOND
    ste_apdu_text_attribute_t     text_attribute;
    uint8_t                       frame_identifier;
    ste_apdu_get_inkey_details_t  command_details;
} ste_apdu_pc_get_inkey_t;

/************************************************************************
 * SET UP MENU
 ***********************************************************************/


/** Preference for Menu Selection */
typedef enum {
    PREFERRED_SELECTION_NONE = 0,
    PREFERRED_SELECTION_SOFT_KEY
} ste_apdu_sum_preferred_selection_t;

/** Help Information Availability for Menu Selection */
typedef enum {
    HELP_INFO_UNAVAILABLE = 0,
    HELP_INFO_AVAILABLE
} ste_apdu_sum_help_info_available_t;

/** Setup Menu Command Details TLV
 *
 * @param SoftKeySelectionPreferred   Soft Key Selection Preferred flag
 * @param HelpInfoAvailable           Help Info Available flag
 */
typedef struct {
    ste_apdu_sum_preferred_selection_t PreferredSelection;
    ste_apdu_sum_help_info_available_t HelpInfoAvailable;
} ste_apdu_set_up_menu_details_t;

typedef struct {
    unsigned int                    bit_mask;   //use unsigned int for this bit mask because this structure has more members
#define       PC_SET_UP_MENU_DeviceID_present            0x0001
#define       PC_SET_UP_MENU_CommandDetails_present      0x0002
#define       PC_SET_UP_MENU_AlphaIdentifier_present     0x0004
#define       PC_SET_UP_MENU_ItemList_present            0x0008
#define       PC_SET_UP_MENU_NextAction_present          0x0010
#define       PC_SET_UP_MENU_IconIdentifier_present      0x0020
#define       PC_SET_UP_MENU_IconIdList_present          0x0040
#define       PC_SET_UP_MENU_TextAttribute_present       0x0080
#define       PC_SET_UP_MENU_TextAttributeList_present   0x0100
    ste_apdu_device_id_t            device_id;
    ste_apdu_set_up_menu_details_t  command_details;
    ste_apdu_alpha_identifier_t     alpha_identifier;
    ste_apdu_list_t                 item_list;
    ste_apdu_items_next_action_t    next_action;
    ste_apdu_icon_identifier_t      icon_id;
    ste_apdu_icon_id_list_t         icon_id_list;
    ste_apdu_text_attribute_t       text_attribute;
    ste_apdu_list_t                 text_attribute_list;
} ste_apdu_pc_set_up_menu_t;

/************************************************************************
 * CALL CONTROL
 ***********************************************************************/

typedef enum {
    STE_APDU_CC_TYPE_UNKNOWN,
    STE_APDU_CC_CALL_SETUP,
    STE_APDU_CC_SS,
    STE_APDU_CC_USSD,
    STE_APDU_CC_PDP
} ste_apdu_dial_string_type_t;


typedef struct {
    ste_apdu_dial_string_type_t    cc_type;        /* Called Party, SS or USSD */
    ste_apdu_address_t             address;        /* The TON and NPI fields are only used for SS */
    uint8_t                        dcs;            /* This is only used for USSD strings */
} ste_apdu_dial_string_t;

typedef struct
{
    uint32_t cid; //Cell id
    uint8_t  UseExtended; //boolean set to true if rat is UMTS, HSDPA, HSUPA or HSPA
} ste_apdu_cell_id_t;

/**
 * Defines size of a PLMN (Public Land Mobile Network)
 */
#define STE_APDU_PLMN_LENGTH   3

/**
 * Type used for representing a PLMN. The format is:
 *                7654   3210
 *              ---------------
 *   index 0:   | MCC2 | MCC1 |
 *   index 1:   | MNC3 | MCC3 |
 *   index 2:   | MNC2 | MNC1 |
 *
 * (MCC = Mobile Country Code, MNC = Mobile Network Code)
 *
 * @param PLMN      The PLMN code in octet array format.
 */
typedef struct
{
    uint8_t     PLMN[STE_APDU_PLMN_LENGTH];
} ste_apdu_plmn_t;

/**
 * Defines size of an LAC (Location Area Code)
 * This element is coded according to TS 24.008, i.e. bit 8 of the octet with index 0
 * is the most significant bit, and bit 1 of the octet with index 1 is the least
 * significant bit.
 */
#define STE_APDU_LAC_LENGTH    2

/**
 * Type used for representing a Location Area Code
 *
 * @param LAC       The LAC code in octet array format.
 */
typedef struct
{
    uint8_t LAC[STE_APDU_LAC_LENGTH];
} ste_apdu_lac_t;

/** ste_cat_net_operator_info_t */
typedef struct
{
    ste_apdu_plmn_t          PLMN;
    ste_apdu_lac_t           LAC;
} ste_apdu_net_operator_info_t;

typedef struct
{
    ste_apdu_cell_id_t              cell_info;
    ste_apdu_net_operator_info_t    net_info;
} ste_apdu_location_info_t;

typedef struct {
    uint8_t                          bit_mask;
#define       EC_CALL_CONTROL_DeviceID_present            0x01
#define       EC_CALL_CONTROL_DialString_present          0x02
#define       EC_CALL_CONTROL_CCP1_present                0x04
#define       EC_CALL_CONTROL_SubAddress_present          0x08
#define       EC_CALL_CONTROL_LocationInfo_present        0x10
#define       EC_CALL_CONTROL_CCP2_present                0x20
#define       EC_CALL_CONTROL_BCRepeatInd_present         0x40
    ste_apdu_device_id_t             device_id;
    ste_apdu_dial_string_t           dial_string;
    ste_apdu_ccp_t                   ccp_1;
    ste_apdu_sub_address_t           sub_address;
    ste_apdu_location_info_t         location_info;
    ste_apdu_ccp_t                   ccp_2;
    uint8_t                          bc_repeat_ind;
} ste_apdu_ec_call_control_t;

/************************************************************************
 * CALL CONTROL RESPONSE
 ***********************************************************************/
/** Call Control result */
typedef enum
{
    STE_APDU_ALLOWED_NO_MODIFICATION    = 0,
    STE_APDU_NOT_ALLOWED                = 1,
    STE_APDU_ALLOWED_WITH_MODIFICATIONS = 2
} ste_apdu_call_control_result_t;

typedef struct
{
    uint8_t                          bit_mask;
#define       EC_CALL_CONTROL_RSP_Reulst_present            0x01
#define       EC_CALL_CONTROL_RSP_DialString_present        0x02
#define       EC_CALL_CONTROL_RSP_CCP1_present              0x04
#define       EC_CALL_CONTROL_RSP_SubAddress_present        0x08
#define       EC_CALL_CONTROL_RSP_AlphaIdentifier_present   0x10
#define       EC_CALL_CONTROL_RSP_BCRepeatInd_present       0x20
#define       EC_CALL_CONTROL_RSP_CCP2_present              0x40
  ste_apdu_call_control_result_t     cc_result;
  ste_apdu_dial_string_t             rsp_dial_string;
  ste_apdu_ccp_t                     rsp_ccp_1;
  ste_apdu_sub_address_t             rsp_sub_address;
  ste_apdu_alpha_identifier_t        alpha_id;
  uint8_t                            rsp_bc_repeat_ind;
  ste_apdu_ccp_t                     rsp_ccp_2;
} ste_apdu_call_control_response_t;

/************************************************************************
 * SMS CONTROL
 ***********************************************************************/

typedef struct {
    uint8_t                          bit_mask;
#define       EC_SMS_CONTROL_DeviceID_present            0x01
#define       EC_SMS_CONTROL_SMSC_Address_present        0x02
#define       EC_SMS_CONTROL_DestAddress_present         0x04
#define       EC_SMS_CONTROL_LocationInfo_present        0x08
    ste_apdu_device_id_t             device_id;
    ste_apdu_address_t               smsc;
    ste_apdu_address_t               destination;
    ste_apdu_location_info_t         location_info;
} ste_apdu_ec_sms_control_t;

/************************************************************************
 * SMS CONTROL RESPONSE
 ***********************************************************************/
/** SMS Control result */
typedef ste_apdu_call_control_result_t   ste_apdu_sms_control_result_t;

typedef struct
{
    uint8_t                          bit_mask;
#define       EC_SMS_CONTROL_RSP_Reulst_present            0x01
#define       EC_SMS_CONTROL_RSP_SMSC_Address_present      0x02
#define       EC_SMS_CONTROL_RSP_DestAddress_present       0x04
#define       EC_SMS_CONTROL_RSP_AlphaIdentifier_present   0x08
    ste_apdu_sms_control_result_t    sc_result;
    ste_apdu_address_t               smsc;
    ste_apdu_address_t               destination;
    ste_apdu_alpha_identifier_t      alpha_id;
} ste_apdu_sms_control_response_t;


/************************************************************************
 * MORE TIME
 ***********************************************************************/

typedef struct {
    uint8_t                 bit_mask;
#define       PC_MORE_TIME_DeviceID_present            0x01
    ste_apdu_device_id_t    device_id;
} ste_apdu_pc_more_time_t;


/************************************************************************
 * POLLING OFF
 ***********************************************************************/

typedef struct {
    uint8_t                 bit_mask;
#define       PC_POLLING_OFF_DeviceID_present          0x01
    ste_apdu_device_id_t    device_id;
} ste_apdu_pc_polling_off_t;


/************************************************************************
 * TIMER MANAGEMENT
 ***********************************************************************/

typedef enum {
    TIMER_START = 0,
    TIMER_DEACTIVATE,
    TIMER_GET_CURRENT_VALUE,
    TIMER_NONE
} ste_apdu_timer_action_t;

typedef struct {
    ste_apdu_timer_action_t timer_action;
} ste_apdu_timer_management_details_t;

typedef enum {
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_UNKNOWN = 0,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_ONE,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_TWO,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_THREE,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FOUR,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_FIVE,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SIX,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_SEVEN,
    STE_APDU_TIMER_IDENTIFIER_CODE_VALUE_TIMER_EIGHT
} ste_apdu_timer_identifier_code_value_t;

typedef struct {
    size_t                  timer_value_length;
    uint32_t                timer_value_data;
} ste_apdu_timer_value_t;

typedef struct {
    size_t                                  timer_identifier_length;
    ste_apdu_timer_identifier_code_value_t  timer_code;
} ste_apdu_timer_identifier_t;

typedef struct {
    uint8_t                              bit_mask;
#define       PC_TIMER_MANAGEMENT_DeviceID_present            0x01
#define       PC_TIMER_MANAGEMENT_CommandDetails_present      0x02
#define       PC_TIMER_MANAGEMENT_TimerIdentifier_present     0x04
#define       PC_TIMER_MANAGEMENT_TimerValue_present          0x08
    ste_apdu_device_id_t                 device_id;
    ste_apdu_timer_management_details_t  command_details;
    ste_apdu_timer_identifier_t          timer_identifier;
    ste_apdu_timer_value_t               timer_value;
} ste_apdu_pc_timer_management_t;


/************************************************************************
 * SEND SHORT MESSAGE
 ***********************************************************************/
typedef struct {
    boolean packing_required;
} ste_apdu_send_sms_details_t;

typedef struct {
    uint16_t                      bit_mask;
#define       PC_SEND_SMS_CommandDetails_present       0x0001
#define       PC_SEND_SMS_DeviceID_present             0x0002
#define       PC_SEND_SMS_AlphaIdentifier_present      0x0004
#define       PC_SEND_SMS_Address_present              0x0008
#define       PC_SEND_SMS_3GPP_SMS_TPDU_present        0x0010
#define       PC_SEND_SMS_CDMA_SMS_TPDU_present        0x0020
#define       PC_SEND_SMS_IconIdentifier_present       0x0040
#define       PC_SEND_SMS_TextAttribute_present        0x0080
#define       PC_SEND_SMS_FrameIdentifier_present      0x0100

    ste_apdu_send_sms_details_t   command_details;
    ste_apdu_device_id_t          device_id;
    ste_apdu_alpha_identifier_t   alpha_identifier;
    ste_apdu_address_t            address;
    ste_apdu_sms_tpdu_t           tpdu;
    ste_apdu_icon_identifier_t    icon_id;
    ste_apdu_text_attribute_t     text_attribute;
    uint8_t                       frame_identifier;
} ste_apdu_pc_send_short_message_t;


/************************************************************************
 * SEND DTMF
 ***********************************************************************/
typedef struct {
    uint8_t                       bit_mask;
#define       PC_SEND_DTMF_CommandDetails_present       0x01
#define       PC_SEND_DTMF_DeviceID_present             0x02
#define       PC_SEND_DTMF_AlphaIdentifier_present      0x04
#define       PC_SEND_DTMF_DTMF_String_present          0x08
#define       PC_SEND_DTMF_IconIdentifier_present       0x10
#define       PC_SEND_DTMF_TextAttribute_present        0x20
#define       PC_SEND_DTMF_FrameIdentifier_present      0x40
    ste_apdu_device_id_t          device_id;
    ste_apdu_alpha_identifier_t   alpha_identifier;
    ste_apdu_dtmf_string_t        dtmf;
    ste_apdu_icon_identifier_t    icon_id;
    ste_apdu_text_attribute_t     text_attribute;
    uint8_t                       frame_identifier;
} ste_apdu_pc_send_dtmf_t;

/************************************************************************
 * SEND SS
 ***********************************************************************/
typedef struct {
    uint8_t                       bit_mask;
#define       PC_SEND_SS_CommandDetails_present       0x01
#define       PC_SEND_SS_DeviceID_present             0x02
#define       PC_SEND_SS_AlphaIdentifier_present      0x04
#define       PC_SEND_SS_SS_String_present            0x08
#define       PC_SEND_SS_IconIdentifier_present       0x10
#define       PC_SEND_SS_TextAttribute_present        0x20
#define       PC_SEND_SS_FrameIdentifier_present      0x40
    ste_apdu_device_id_t          device_id;
    ste_apdu_alpha_identifier_t   alpha_identifier;
    ste_apdu_address_t            ss_string;
    ste_apdu_icon_identifier_t    icon_id;
    ste_apdu_text_attribute_t     text_attribute;
    uint8_t                       frame_identifier;
} ste_apdu_pc_send_ss_t;

/************************************************************************
 * SEND USSD
 ***********************************************************************/
typedef struct {
    uint8_t   dcs;
    uint8_t  *str_p;
    size_t    str_len;
} ste_apdu_ussd_string_t;

typedef struct {
    uint8_t                       bit_mask;
#define       PC_SEND_USSD_CommandDetails_present       0x01
#define       PC_SEND_USSD_DeviceID_present             0x02
#define       PC_SEND_USSD_AlphaIdentifier_present      0x04
#define       PC_SEND_USSD_USSD_String_present          0x08
#define       PC_SEND_USSD_IconIdentifier_present       0x10
#define       PC_SEND_USSD_TextAttribute_present        0x20
#define       PC_SEND_USSD_FrameIdentifier_present      0x40
    ste_apdu_device_id_t          device_id;
    ste_apdu_alpha_identifier_t   alpha_identifier;
    ste_apdu_ussd_string_t        ussd_string;
    ste_apdu_icon_identifier_t    icon_id;
    ste_apdu_text_attribute_t     text_attribute;
    uint8_t                       frame_identifier;
} ste_apdu_pc_send_ussd_t;

/************************************************************************
 * REFRESH
 ***********************************************************************/

/** Maximum length of Application Id (AID) in bytes */
#define STE_APDU_MAX_AID_LENGTH                      (20)

//this enum directly maps to the value in the binary according to the specification,
//please do not change the order of this enum
typedef enum
{
    STE_APDU_REFRESH_INIT_AND_FULL_FILE_CHANGE = 0x00,
    STE_APDU_REFRESH_FILE_CHANGE,
    STE_APDU_REFRESH_INIT_AND_FILE_CHANGE,
    STE_APDU_REFRESH_INIT,
    STE_APDU_REFRESH_UICC_RESET,
    STE_APDU_REFRESH_3G_APP_RESET,
    STE_APDU_REFRESH_3G_SESSION_RESET,
    STE_APDU_REFRESH_RESERVED
} ste_apdu_refresh_type_t;

typedef struct {
    ste_apdu_refresh_type_t   refresh_type;
} ste_apdu_refresh_details_t;

typedef struct {
    uint8_t    *file_path;  /**< raw bytes - eg. 3F002FE2 is 4 bytes */
    uint8_t     path_len;   /**< number of bytes in the file path */
} ste_apdu_file_name_t;

typedef struct {
    uint8_t                 nr_of_files;
    ste_apdu_file_name_t   *files_p;
} ste_apdu_file_list_t;

typedef struct {
    uint8_t  len;
    uint8_t  app[STE_APDU_MAX_AID_LENGTH];
} ste_apdu_application_id_t;

typedef struct {
    uint8_t                       bit_mask;
#define       PC_REFRESH_CommandDetails_present       0x01
#define       PC_REFRESH_DeviceID_present             0x02
#define       PC_REFRESH_FileList_present             0x04
#define       PC_REFRESH_AID_present                  0x08
#define       PC_REFRESH_AlphaIdentifier_present      0x10
#define       PC_REFRESH_IconIdentifier_present       0x20
#define       PC_REFRESH_TextAttribute_present        0x40
#define       PC_REFRESH_FrameIdentifier_present      0x80
    ste_apdu_refresh_details_t    command_details;
    ste_apdu_device_id_t          device_id;
    ste_apdu_file_list_t          file_list;
    ste_apdu_application_id_t     app_id;
    ste_apdu_alpha_identifier_t   alpha_identifier;
    ste_apdu_icon_identifier_t    icon_id;
    ste_apdu_text_attribute_t     text_attribute;
    uint8_t                       frame_identifier;
} ste_apdu_pc_refresh_t;

/************************************************************************
 * SETUP CALL
 ***********************************************************************/
/** Call setup details */
typedef enum
{
    STE_APDU_ONLY_IF_NOT_BUSY,
    STE_APDU_PUT_ALL_OTHER_CALLS_ON_HOLD,
    STE_APDU_DISCONNECT_ALL_OTHER_CALLS
} ste_apdu_setup_call_option_t;

typedef struct
{
    ste_apdu_setup_call_option_t   option;
    boolean                        redial;
} ste_apdu_setup_call_details_t;


typedef struct {
    uint16_t                      bit_mask;
#define       PC_SETUP_CALL_CommandDetails_present         0x0001
#define       PC_SETUP_CALL_DeviceID_present               0x0002
#define       PC_SETUP_CALL_AlphaIdentifier_1_present      0x0004
#define       PC_SETUP_CALL_Address_present                0x0008
#define       PC_SETUP_CALL_CCP_present                    0x0010
#define       PC_SETUP_CALL_SubAddress_present             0x0020
#define       PC_SETUP_CALL_Duration_present               0x0040
#define       PC_SETUP_CALL_IconIdentifier_1_present       0x0080
#define       PC_SETUP_CALL_AlphaIdentifier_2_present      0x0100
#define       PC_SETUP_CALL_IconIdentifier_2_present       0x0200
#define       PC_SETUP_CALL_TextAttribute_1_present        0x0400
#define       PC_SETUP_CALL_TextAttribute_2_present        0x0800
#define       PC_SETUP_CALL_FrameIdentifier_present        0x1000
    ste_apdu_setup_call_details_t command_details;
    ste_apdu_device_id_t          device_id;
    ste_apdu_alpha_identifier_t   alpha_id_1;  //for confirmation phase
    ste_apdu_address_t            address;
    ste_apdu_ccp_t                ccp;
    ste_apdu_sub_address_t        sub_address;
    unsigned int                  duration;   //time unit: TENS_OF_SECOND
    ste_apdu_icon_identifier_t    icon_id_1;  //for confirmation phase
    ste_apdu_alpha_identifier_t   alpha_id_2;  //for call setup phase
    ste_apdu_icon_identifier_t    icon_id_2;  //for call setup phase
    ste_apdu_text_attribute_t     text_attribute_1;  //for confirmation phase
    ste_apdu_text_attribute_t     text_attribute_2;  //for call setup phase
    uint8_t                       frame_identifier;
} ste_apdu_pc_setup_call_t;

/************************************************************************
 * LANGUAGE NOTIFICATION
 ***********************************************************************/

typedef struct {
    uint8_t                 bit_mask;
#define       PC_LANGUAGE_NOTIFICATION_DeviceID_present             0x01
#define       PC_LANGUAGE_NOTIFICATION_Language_present             0x02
    ste_apdu_device_id_t    device_id;
    ste_apdu_language_t     language;
} ste_apdu_pc_language_notification_t;

//#############################################################################
// APDU related interface functions
//#############################################################################

// Create an apdu info structure from a binady apdu data.
// the memory for the APDU info structure will be allocated by APDU module and
// should be freed by user.
ste_sat_apdu_error_t    ste_apdu_parse(const ste_apdu_t   *apdu,
                                       ste_parsed_apdu_t **parsed_apdu_info_pp);

// Write the apdu info structure to the data buffer, note the buffer memory is
// allocated by APDU module and should be freed by user.  The function will
// return the actual data length of created buffer via dataLength_p.
ste_sat_apdu_error_t    ste_apdu_unparse(ste_parsed_apdu_t *source_apdu_info_p,
                                         ste_apdu_t       **apdu);

// Delete an apdu info structure, including the lower layer pointer parameters
void                    ste_parsed_apdu_delete(ste_parsed_apdu_t *apdu_info_p);

// Retrieve kind of an APDU data structure
uint8_t                 ste_parsed_apdu_get_kind(const ste_parsed_apdu_t *apdu_info_p);

// Retrieve type of an APDU data structure
uint8_t                 ste_parsed_apdu_get_type(const ste_parsed_apdu_t *apdu_info_p);

// create a new empty APDU info structure, based on kind and type
ste_apdu_t             *ste_apdu_new(const uint8_t *raw, size_t len);

void                    ste_apdu_delete(ste_apdu_t *apdu);

const uint8_t          *ste_apdu_get_raw(const ste_apdu_t *apdu);
size_t                  ste_apdu_get_raw_length(const ste_apdu_t *apdu);

ste_sat_apdu_error_t    ste_apdu_get_cmd_details(const ste_apdu_t * apdu_p,
                                                 uint8_t * cmd_type,
                                                 uint8_t * cmd_qualifier,
                                                 uint8_t * cmd_number);
uint8_t                 ste_apdu_get_kind(const ste_apdu_t *apdu_p);

ste_apdu_t *            ste_apdu_copy(ste_apdu_t *source_p);

typedef struct {
    uint8_t sw1;
    uint8_t sw2;
} ste_apdu_status_words_t;

uint8_t ste_apdu_get_catd_status_words(const ste_apdu_t * apdu_p,
                                       ste_apdu_status_words_t * status_words);

// set the specific command structure data to one APDU info structure
ste_sat_apdu_error_t    ste_parsed_apdu_set(ste_parsed_apdu_t *apdu_info_p,
                                            const void        *data_p,
                                            uint8_t            flags);

// fetch the specific command structure data from one APDU info structure
ste_sat_apdu_error_t    ste_parsed_apdu_get(const ste_parsed_apdu_t *apdu_info_p,
                                            void                   **data_pp,
                                            uint8_t                  flags);

ste_apdu_t  * ste_apdu_call_control(ste_cat_call_control_t        * cc_p,
                                    ste_apdu_location_info_t      * local_info_p);

ste_apdu_t  * ste_apdu_setup_call(ste_apdu_pc_setup_call_t      * call_apdu_p,
                                  ste_apdu_location_info_t      * local_info_p);

ste_sat_apdu_error_t
ste_apdu_parse_call_control_response(ste_cat_call_control_response_t * cc_rsp_p,
                                     ste_apdu_t  * apdu,
                                     ste_cat_call_control_result_t * cc_result_p);

ste_apdu_t  * ste_apdu_sms_control(ste_cat_sms_control_t        * cc_p,
                                   ste_apdu_location_info_t     * local_info_p);

ste_sat_apdu_error_t
ste_apdu_parse_sms_control_response(ste_cat_sms_control_response_t * sc_rsp_p,
                                    ste_apdu_t  * apdu,
                                    ste_cat_sms_control_result_t * sc_result_p);

uint8_t * ste_apdu_decode_alhpa_id( ste_apdu_alpha_identifier_t * alpha_p,
                                    ste_sim_text_coding_t       * coding_p,
                                    size_t                      * result_len_p);

ste_apdu_t  * ste_apdu_event_download(cn_message_type_t event_type, void * event_data_p, ste_sat_kind_of_event_requested kind_of_event);

void catd_main_event_download(ste_apdu_t * apdu_p, int fd, uintptr_t client_tag);

ste_apdu_t   *ste_apdu_timer_expiration_ec(const ste_apdu_timer_identifier_code_value_t  timer_id_pc,
                                           const uint8_t hours,
                                           const uint8_t minutes,
                                           const uint8_t seconds);

void create_bcd_from_mcc_mnc(uint8_t *mcc_mnc_string, uint8_t *plmn);

/************************************************************************
 * Typedefs for RESULT TLV in Terminal Response
 ***********************************************************************/
typedef enum {
    STE_SAT_PC_CMD_STATUS_NONE = 0,
    STE_SAT_PC_CMD_STATUS_PENDING,
    STE_SAT_PC_CMD_STATUS_COMPLETED,
    STE_SAT_PC_CMD_STATUS_TERMINATED
} ste_apdu_pc_cmd_status_t;


/** General Result values */
typedef enum {
    SAT_RES_CMD_PERFORMED_SUCCESSFULLY = 0x00,
    SAT_RES_CMD_PERFORMED_WITH_PART_COMP = 0x01,
    SAT_RES_CMD_PERFORMED_WITH_MISSING_INFO = 0x02,
    SAT_RES_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS = 0x03,
    SAT_RES_CMD_PERFORMED_SUCC_WITHOUT_REQ_ICON = 0x04,
    SAT_RES_CMD_PERFORMED_WITH_CALL_CONTROL_MOD = 0x05,
    SAT_RES_CMD_PERFORMED_SUCC_LIMITED_SERVICE = 0x06,
    SAT_RES_CMD_PERFORMED_WITH_MODIFICATION = 0x07,
    SAT_RES_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE = 0x08,

    SAT_RES_USER_TERMINATED_SESSION = 0x10,
    SAT_RES_USER_REQUESTS_BACKWARDS_MOVE = 0x11,
    SAT_RES_NO_RESPONSE_FROM_USER = 0x12,
    SAT_RES_HELP_INFO_REQUIRED_BY_USER = 0x13,
    SAT_RES_USSD_TRANS_TERMINATED_BY_USER = 0x14,

    SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD = 0x20,
    SAT_RES_NETWORK_CURRENTLY_UNABLE = 0x21,
    SAT_RES_USER_REJECTED_PROACTIVE_CMD = 0x22,
    SAT_RES_USER_CLEARED_DOWN_OR_NET_RELEASE = 0x23,
    SAT_RES_ACTION_CONTRADICTS_CURRENT_TIMER_STATE = 0x24,
    SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_TEMP = 0x25,
    SAT_RES_LAUNCH_BROWSER_GENERIC_ERROR = 0x26,

    SAT_RES_COMMAND_BEYOND_ME_CAPABILITY = 0x30,
    SAT_RES_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME = 0x31,
    SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME = 0x32,
    SAT_RES_COMMAND_NUMBER_NOT_KNOWN_BY_ME = 0x33,
    SAT_RES_SS_RETURN_ERROR = 0x34,
    SAT_RES_SMS_RP_ERROR = 0x35,
    SAT_RES_ERROR_REQUIRED_VALUES_MISSING = 0x36,
    SAT_RES_USSD_RETURN_ERROR = 0x37,
    SAT_RES_MULTIPLE_COMMANDS_ERROR = 0x38,
    SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_PERM = 0x39,
    SAT_RES_BEARER_INDEPENDENT_PROTOCOL_ERROR = 0x3A
} ste_apdu_general_result_t;


/** Additional Information value (complete list) */
typedef enum {
    SAT_RES_ADD_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN = 0x00,

    // SAT_ME_ProblemAdditionalInfo_t
    SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN = 0x00,
    SAT_RES_MEP_SCREEN_IS_BUSY = 0x01,
    SAT_RES_MEP_CURRENTLY_BUSY_ON_CALL = 0x02,
    SAT_RES_MEP_CURRENTLY_BUSY_ON_SS_TRANSACTION = 0x03,
    SAT_RES_MEP_NO_SERVICE = 0x04,
    SAT_RES_MEP_ACCESS_CONTROL_CLASS_BAR = 0x05,
    SAT_RES_MEP_RADIO_RESOURCE_NOT_GRANTED = 0x06,
    SAT_RES_MEP_NOT_IN_SPEECH_CALL = 0x07,
    SAT_RES_MEP_CURRENTLY_BUSY_ON_USSD_TRANSACTION = 0x08,
    SAT_RES_MEP_CURRENTLY_BUSY_ON_SEND_DTMF = 0x09,
    SAT_RES_MEP_NO_USIM_ACTIVE = 0x0A,

    // SAT_InteractionWithCC_AdditionalInfo_t
    SAT_RES_CC_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN = 0x00,
    SAT_RES_CC_ACTION_NOT_ALLOWED = 0x01,
    SAT_RES_CC_TYPE_OF_REQUEST_CHANGED = 0x02,

    // SAT_LaunchBrAdditionalInfo_t
    SAT_RES_LBR_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN = 0x00,
    SAT_RES_LBR_BEARER_UNAVAILABLE = 0x01,
    SAT_RES_LBR_BROWSER_UNAVAILABLE = 0x02,
    SAT_RES_LBR_ME_UNABLE_TO_READ_PROVISIONING_DATA = 0x03,

    // SAT_BearerIndProtAdditionalInfo_t
    SAT_RES_BIP_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN = 0x00,
    SAT_RES_BIP_NO_CHANNEL_AVAILABLE = 0x01,
    SAT_RES_BIP_CHANNEL_CLOSED = 0x02,
    SAT_RES_BIP_CHANNEL_IDENTIFIER_NOT_VALID = 0x03,
    SAT_RES_BIP_REQUESTED_BUFFER_SIZE_UNAVAILABLE = 0x04,
    SAT_RES_BIP_SECURITY_ERROR = 0x05,
    SAT_RES_BIP_REQUESTED_SIM_ME_INTERFACE_ERROR = 0x06
} ste_apdu_additional_info_t;

//#############################################################################
// Terminal Response interface functions
//#############################################################################

//Other data type inside of a command result
typedef enum {
    STE_CMD_RESULT_NOTHING = 0,
    STE_CMD_RESULT_IMEI,
    STE_CMD_RESULT_IMEISV,
    STE_CMD_RESULT_TEXT_STRING,
    STE_CMD_RESULT_CALL_CONTROL_RESULT,
    STE_CMD_RESULT_CC_SEND_SS,
    STE_CMD_RESULT_CC_SEND_USSD,
    STE_CMD_RESULT_POLL_INTERVAL,
    STE_CMD_RESULT_BATTERY,
    STE_CMD_RESULT_LOCATION,
    STE_CMD_RESULT_SEARCH_MODE,
    STE_CMD_RESULT_TIMER_MANAGEMENT,
    STE_CMD_RESULT_TIMING_ADVANCE,
    STE_CMD_RESULT_ACCESS_TECHNOLOGY,
    STE_CMD_RESULT_NMR,
    STE_CMD_RESULT_DATE_TIME_TIMEZONE
} ste_cmd_result_other_data_type_t;

//the union structure for different additional information
typedef union {
    ste_cmd_result_imei_t                 *imei_p;          //IMEI information, used for Provide Local Information
    ste_cmd_result_imeisv_t               *imeisv_p;        //IMEISV information, used for Provide Local Information
    ste_apdu_text_string_t                *text_p;          //Text String, used for get input, get inkey, etc.
    ste_cat_call_control_response_t       *cc_result_p;     //call control result, used for setup call, send ss, send ussd.
    unsigned int                           duration;        //duration result
    uint8_t                                battery_state;   //Battery charge state result
    ste_apdu_location_info_t              *location_info_p; //Location information, used for Provide Local Information
    uint8_t                                search_mode;     //Search mode as defined by TS 102.223 sect 8.75
    uint8_t                                rat;             //Access technology, used for Provide Local Information
    uint8_t                                timer_value[5];  //Timer ID, flag, timer value used for Timer Management proactive command.
    ste_cat_timing_advance_response_t      timing_advance;
    ste_apdu_pc_provide_local_nmr_info_t  *nmr_info;        //Network measurement report (NMR), used for Provide Local Information
    ste_apdu_date_time_t                  *date_time;       //Date, Time and Timezone, used for Provide Local Information
} ste_cmd_result_other_data_t;

//general structure for Command Result information.
typedef struct {
    ste_apdu_general_result_t         general_result;       // General result
    ste_apdu_pc_cmd_status_t          command_status;       // Command status for the current command
    uint8_t                           additional_info_size; // Size of additional information
    uint8_t                          *additional_info_p;    // Pointer to additional information.
    ste_cmd_result_other_data_type_t  other_data_type;      // Data type classifier for other data
    ste_cmd_result_other_data_t       other_data;           // Pointer to other data
} ste_command_result_t;

// Create an apdu binary from the result info for one specific APDU info
// structure. the memory for the APDU binary data will be allocated by APDU
// module and should be freed by user.
ste_sat_apdu_error_t ste_apdu_terminal_response(ste_parsed_apdu_t    *apdu_info_p,
                                                ste_command_result_t *command_result_p,
                                                ste_apdu_t          **apdu);

// Create an apdu binary from the result info for one specific APDU info
// structure. the memory for the APDU binary data will be allocated by APDU
// module and should be freed by user. In this case we will have two general results
// as input params, which will generate a terminal response with 2 Result TLVS included.
ste_sat_apdu_error_t
ste_apdu_terminal_response_with_second_result(ste_parsed_apdu_t * apdu_info_p,
                                              ste_command_result_t * first_command_result_p,
                                              ste_command_result_t * second_command_result_p,
                                              ste_apdu_t ** apdu);

// Create an apdu binary from the general error result.
// the memory for the APDU binary data will be allocated by APDU
// module and should be freed by user.
ste_apdu_t * ste_apdu_tr_general_error(ste_sat_apdu_error_t   error_info,
                                       ste_apdu_t            *apdu_p);



/************************************************************************
 * UICC APDU specific deinitions
 ***********************************************************************/
typedef enum {
    STE_UICC_CMD_NONE = 0,
    STE_UICC_CMD_SELECT,
    STE_UICC_CMD_ACTIVATE,
    STE_UICC_CMD_DEACTIVATE
} ste_apdu_uicc_command_t;

#define STE_UICC_CLASS (0xA0)

#define STE_UICC_INS_SELECT_FILE     (0xA4)
#define STE_UICC_INS_READ_RECORD     (0xB2)
#define STE_UICC_INS_ACTIVATE_FILE   (0x44)
#define STE_UICC_INS_DEACTIVATE_FILE (0x04)


#define STE_UICC_SELECT_FILE_P1_SELECT_BY_FILE_ID  (0x00)
#define STE_UICC_SELECT_FILE_P1_SELECT_BY_MF_PATH  (0x08)
#define STE_UICC_SELECT_FILE_P1_SELECT_BY_DF_PATH  (0x09)

#define STE_UICC_SELECT_FILE_P2  (0x00)
#define STE_UICC_SELECT_FILE_P3  (0x02)

#define STE_UICC_ACTIVATE_FILE_P1_SELECT_BY_FILE_ID  (0x00)
#define STE_UICC_ACTIVATE_FILE_P1_SELECT_BY_MF_PATH  (0x08)
#define STE_UICC_ACTIVATE_FILE_P1_SELECT_BY_DF_PATH  (0x09)

#define STE_UICC_ACTIVATE_FILE_P2  (0x00)
#define STE_UICC_ACTIVATE_FILE_P3  (0x00)

#define STE_UICC_DEACTIVATE_FILE_P1_SELECT_BY_FILE_ID  (0x00)
#define STE_UICC_DEACTIVATE_FILE_P1_SELECT_BY_MF_PATH  (0x08)
#define STE_UICC_DEACTIVATE_FILE_P1_SELECT_BY_DF_PATH  (0x09)

#define STE_UICC_DEACTIVATE_FILE_P2  (0x00)
#define STE_UICC_DEACTIVATE_FILE_P3  (0x00)


ste_apdu_t  * ste_apdu_uicc_select_file();

ste_apdu_t  * ste_apdu_uicc_read_record(uint8_t record_no, uint8_t record_mode, uint8_t data_length);

ste_apdu_t  * ste_apdu_uicc_activate_file();

ste_apdu_t  * ste_apdu_uicc_deactivate_file();

ste_sat_apdu_error_t  ste_apdu_get_event_from_event_download(ste_apdu_t *apdu_p, uint8_t* event_id_p );


/************************************************************************
 * Bit buffer utility functions.
 ***********************************************************************/

/**
 * This data type represents a bit string.
 *
 * The bit string is stored in an unsigned byte array.
 * The bit string is stored from low index to high array index.
 * Within the last used byte of the storage, the bits are stored from MSb to LSb,
 * with the free space residing in the LSb part. The free bits are always set to zero.
 *
 * Example: The bit string '1001110110011' (13 bits) is represented as
 *
 *     storage[0] = 0x9D ('10011101')
 *     storage[1] = 0x98 ('10011000', i.e. '10011' with three trailing zero bits)
 *     used       = 13
 */
typedef struct {
    uint8_t* storage;   // Storage for the bit string
    size_t   size;      // Max number of bits that will fit in the storage
    unsigned used;      // Bits used in the storage.
} bit_buffer_t;

/**
 * Initiates a bit_buffer_t structure and allocates memory for the bit storage.
 *
 * @param   buffer  The bit_buffer_t struct to initialize.
 * @param   size    The number of bit (repeat BITS) the bit bufefr should have memory allocated for.
 *
 * @return  Zero on success and non-zero on failure.
 *
 * NOTE: Do not initiate an already initiated bit_buffer_t struct, it will lead to memory leaks...
 */
int bit_buffer_init(bit_buffer_t* buffer, size_t size);

/**
 * De-initiates a bit_buffer_t structure and de-allocates memory for the bit storage.
 *
 * @param   buffer  The bit_buffer_t struct to initialize.
 */
void bit_buffer_deinit(bit_buffer_t* buffer);


/**
 * Returns the number of bytes (that has been used within the bit buffer's storage.
 *
 * @param   buffer  The bit_buffer_t struct to initialize.
 *
 * @return  The number of bytes (repeat BYTES) that has been used within the bit buffer's storage.
 */
int bit_buffer_length(bit_buffer_t* buffer);

/**
 * Appends bits to the bit buffer.
 *
 * The bits to append must be 'right-justified' within the 'bits' parameter. In other words,
 * if ten bits are to be stored, the ten least significant bits in the 'bits' parameter will
 * be appended to the bit buffer.
 *
 * @param   buffer      Pointer to the bit buffer to append to.
 * @param   bits        An unsigned containing the bits to append. A maximum of (8 * sizeof(unsigned) - 1)
 *                      bits may be appended.
 * @param   num_bits    The number of bits to append
 *
 * @return The number of bits appended, may be less than num_bits in case of error.
 *
 * Example:
 *
 *   Pre append: The bit buffer contains '1001110110011' (13 bits) represented as
 *
 *     storage[0] = 0x9D ('10011101')
 *     storage[1] = 0x98 ('10011000', eg '10011' with three trailing zero bits)
 *     used       = 13
 *
 *   Append the bit string '1110001010' (10 bits) using:
 *
 *     bit_buffer_append_bits(buffer, 0x8F8A, 10)
 *     // 0x8F8A is 16 bits ('1000111110001010'), but we only care about the ten rightmost ones, i.e. '1110001010'
 *
 *   Post append: The bit buffer contains '10011101100111110001010' (23 bits) represented as
 *
 *     storage[0] = 0x9D ('10011101')
 *     storage[1] = 0x9F ('10011111')
 *     storage[1] = 0x14 ('00010100', i.e. '0001010' with one trailing zero bit)
 *     used       = 23
 */
uint8_t bit_buffer_append_bits(bit_buffer_t* buffer, unsigned bits, uint8_t num_bits);

#endif
