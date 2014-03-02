/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd_tpl.h
 * Description     : Terminal Profile download header file
 *
 * Author          : Jessica Nilsson <jessica.j.nilsson@stericsson.com>
 *
 */

#ifndef __catd_tpl_h__
#define __catd_tpl_h__ (1)


/*  This is copied from the module cnh160625 file si_sat_util.h */

/* Definitions for masks to set individual bits in terminal profile bytes */
/* First byte - note that bits 1 and 5 are always set and are not customer */
/* configurable */
#define  BIT_PROFILE_DOWNLOAD_SUPPORT                         (0x01)
#define  BIT_CFG_SAT_SMS_PP_DATA_DOWNLOAD_SUPPORT             (0x02)
#define  BIT_CFG_SAT_CB_DATA_DOWNLOAD_SUPPORT                 (0x04)
#define  BIT_CFG_SAT_MENU_SELECTION_SUPPORT                   (0x08)
#define  BIT_CFG_SAT_SMS_PP_DATA_DOWNLOAD_SUPPORT2            (0x10)
#define  BIT_CFG_SAT_TIMER_EXPIRATION_SUPPORT                 (0x20)
#define  BIT_CFG_SAT_GSM_USSD_IN_CALL_CONTROL_SUPPORT         (0x40)
#define  BIT_CFG_SAT_GSM_ENVELOPE_REDIAL_SUPPORT              (0x80)
/* Second byte */
#define  BIT_CFG_SAT_COMMAND_RESULT_SUPPORT                   (0x01)
#define  BIT_CFG_SAT_CALL_CONTROL_SUPPORT                     (0x02)
#define  BIT_CFG_SAT_GSM_CELL_ID_IN_CALL_CONTROL_SUPPORT      (0x04)
#define  BIT_CFG_SAT_MO_SMS_SIM_CONTROL_SUPPORT               (0x08)
#define  BIT_CFG_SAT_GSM_HANDLING_OF_ALPHA_ID_SUPPORT         (0x10)
#define  BIT_CFG_SAT_UCS2_ENTRY_SUPPORT                       (0x20)
#define  BIT_CFG_SAT_UCS2_DISPLAY_SUPPORT                     (0x40)
#define  BIT_CFG_SAT_GSM_DISPLAY_EXTENSION_TEXT_SUPPORT       (0x80)
/* Third byte */
#define  BIT_CFG_SAT_CMD_DISPLAY_TEXT_SUPPORT                 (0x01)
#define  BIT_CFG_SAT_CMD_GET_INKEY_SUPPORT                    (0x02)
#define  BIT_CFG_SAT_CMD_GET_INPUT_SUPPORT                    (0x04)
#define  BIT_CFG_SAT_CMD_MORE_TIME_SUPPORT                    (0x08)
#define  BIT_CFG_SAT_CMD_PLAY_TONE_SUPPORT                    (0x10)
#define  BIT_CFG_SAT_CMD_POLL_INTERVAL_SUPPORT                (0x20)
#define  BIT_CFG_SAT_CMD_POLLING_OFF_SUPPORT                  (0x40)
#define  BIT_CFG_SAT_CMD_REFRESH_SUPPORT                      (0x80)
/* Fourth byte */
#define  BIT_CFG_SAT_CMD_SELECT_ITEM_SUPPORT                  (0x01)
#define  BIT_CFG_SAT_CMD_SEND_SHORT_MESSAGE_SUPPORT           (0x02)
#define  BIT_CFG_SAT_CMD_SEND_SS_SUPPORT                      (0x04)
#define  BIT_CFG_SAT_CMD_SEND_USSD_SUPPORT                    (0x08)
#define  BIT_CFG_SAT_CMD_SET_UP_CALL_SUPPORT                  (0x10)
#define  BIT_CFG_SAT_CMD_SET_UP_MENU_SUPPORT                  (0x20)
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_MCC_SUPPORT               (0x40)
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_NMR_SUPPORT               (0x80)
/* Fifth byte */
#define  BIT_CFG_SAT_CMD_SET_EVENT_LIST_SUPPORT               (0x01)
#define  BIT_CFG_SAT_EVENT_MT_CALL_SUPPORT                    (0x02)
#define  BIT_CFG_SAT_EVENT_CALL_CONNECTED_SUPPORT             (0x04)
#define  BIT_CFG_SAT_EVENT_CALL_DISCONNECTED_SUPPORT          (0x08)
#define  BIT_CFG_SAT_EVENT_LOCATION_STATUS_SUPPORT            (0x10)
#define  BIT_CFG_SAT_EVENT_USER_ACTIVITY_SUPPORT              (0x20)
#define  BIT_CFG_SAT_EVENT_IDLE_SCREEN_AVAILABLE_SUPPORT      (0x40)
#define  BIT_CFG_SAT_EVENT_CARD_READER_STATUS_SUPPORT         (0x80)
/* Sixth byte */
#define  BIT_CFG_SAT_EVENT_LANGUAGE_SELECTION_SUPPORT         (0x01)
#define  BIT_CFG_SAT_EVENT_BROWSER_TERMINATION_SUPPORT        (0x02)
#define  BIT_CFG_SAT_EVENT_DATA_AVAILABLE_SUPPORT             (0x04)
#define  BIT_CFG_SAT_EVENT_CHANNEL_STATUS_SUPPORT             (0x08)
#define  BIT_CFG_SAT_EVENT_ACCESS_TECHNOLOGY_CHANGE_SUPPORT   (0x10)
#define  BIT_CFG_SAT_EVENT_DISPLAY_PARAMETERS_CHANGED_SUPPORT (0x20)
#define  BIT_CFG_SAT_EVENT_LOCAL_CONNECTION_SUPPORT           (0x40)
#define  BIT_CFG_SAT_EVENT_LOCAL_SEARCH_MODE_SUPPORT          (0x80)
/* Seventh byte */
#define  BIT_CFG_SAT_CMD_POWER_ON_CARD_SUPPORT                (0x01)
#define  BIT_CFG_SAT_CMD_POWER_OFF_CARD_SUPPORT               (0x02)
#define  BIT_CFG_SAT_CMD_PERFORM_CARD_APDU_SUPPORT            (0x04)
#define  BIT_CFG_SAT_CMD_GET_READER_STATUS_SUPPORT            (0x08)
#define  BIT_CFG_SAT_CMD_GET_READER_IDENTIFIER_SUPPORT        (0x10)
#define  BIT_CFG_SAT_RFU1                                     (0x20)
#define  BIT_CFG_SAT_RFU2                                     (0x40)
#define  BIT_CFG_SAT_RFU3                                     (0x80)
/* Eighth byte */
#define  BIT_CFG_SAT_CMD_TIMER_START_STOP_SUPPORT             (0x01)
#define  BIT_CFG_SAT_CMD_TIMER_GET_STATUS_SUPPORT             (0x02)
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_TIME_DATE_SUPPORT         (0x04)
#define  BIT_CFG_SAT_GSM_GET_INKEY_BINARY_CHOICE_SUPPORT      (0x08)
#define  BIT_CFG_SAT_SET_IDLE_TEXT_SUPPORT                    (0x10)
#define  BIT_CFG_SAT_RUN_AT_COMMAND_SUPPORT                   (0x20)
#define  BIT_CFG_SAT_GSM_SET_UP_CALL_ALPHA_ID_2_SUPPORT       (0x40)
#define  BIT_CFG_SAT_GSM_CAPABILITY_CONFIGURATION_2_SUPPORT   (0x80)
/* Nineth byte */
#define  BIT_CFG_SAT_GSM_SUSTAINED_DISPLAY_TEXT_SUPPORT       (0x01)
#define  BIT_CFG_SAT_CMD_SEND_DTMF_SUPPORT                    (0x02)
#define  BIT_CFG_SAT_GSM_CMD_LOCAL_INFO_BCCH_LIST_SUPPORT     (0x04)
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_LANGUAGE_SUPPORT          (0x08)
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_TIMING_ADVANCE_SUPPORT    (0x10)
#define  BIT_CFG_SAT_CMD_LANGUAGE_NOTIFICATION_SUPPORT        (0x20)
#define  BIT_CFG_SAT_CMD_LAUNCH_BROWSER_SUPPORT               (0x40)
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_ACCESS_TECHNOLOGY_SUPPORT (0x80)
/* Tenth byte */
#define  BIT_CFG_SAT_SOFTKEYS_SELECT_ITEM_SUPPORT             (0x01)
#define  BIT_CFG_SAT_SOFTKEYS_SETUP_MENU_SUPPORT              (0x02)


/* Eleventh byte */
#define  MAX_SIZE_CFG_SAT_SOFTKEYS_AVAILABLE_MAX              (0x0A)
#define  DEFAULT_CFG_SAT_SOFTKEYS_AVAILABLE_MAX               (0x00)
/* Twelfth byte */
#define  BIT_CFG_SAT_CMD_OPEN_CHANNEL_SUPPORT                 (0x01)
#define  BIT_CFG_SAT_CMD_CLOSE_CHANNEL_SUPPORT                (0x02)
#define  BIT_CFG_SAT_CMD_RECEIVE_DATA_SUPPORT                 (0x04)
#define  BIT_CFG_SAT_CMD_SEND_DATA_SUPPORT                    (0x08)
#define  BIT_CFG_SAT_CMD_GET_CHANNEL_STATUS_SUPPORT           (0x10)
#define  BIT_CFG_SAT_SERVICE_SEARCH_SUPPORT                   (0x20)
#define  BIT_CFG_SAT_GET_SERVICE_INFOMATION_SUPPORT           (0x40)
#define  BIT_CFG_SAT_DECLARE_SERVICE_SUPPORT                  (0x80)
/* Thirteenth byte */
#define  BIT_CFG_SAT_CSD_SUPPORT                              (0x01)
#define  BIT_CFG_SAT_GPRS_SUPPORT                             (0x02)
#define  BIT_CFG_SAT_BLUETOOTH_SUPPORT                        (0x04)
#define  BIT_CFG_SAT_IRDA_SUPPORT                             (0x08)
#define  BIT_CFG_SAT_RS232_SUPPORT                            (0x10)
#define  MAX_CFG_SAT_BEARER_CHANNELS_SUPPORT_NUMBER           (0xE0)
#define  DEFAULT_CFG_SAT_BEARER_CHANNELS_SUPPORT_NUMBER       (0x00)
/* Fourteenth byte */
#define  MAX_SIZE_CFG_SAT_V_CHARACTERS_DISPLAY_NUMBER         (0x1F)
#define  DEFAULT_CFG_SAT_V_CHARACTERS_DISPLAY_NUMBER          (0x03)
#define  BIT_CFG_SAT_SCREEN_SIZING_PARAMS_SUPPORT             (0x80)
/* Fifteenth byte */
#define  MAX_SIZE_CFG_SAT_H_CHARACTERS_DISPLAY_NUMBER         (0x7F)
#define  DEFAULT_CFG_SAT_H_CHARACTERS_DISPLAY_NUMBER          (0x10)
#define  BIT_CFG_SAT_VARIABLE_SIZE_FONTS_SUPPORT              (0x80)
/* Sixteenth byte */
#define  BIT_CFG_SAT_DISPLAY_CAN_BE_RESIZED_SUPPORT           (0x01)
#define  BIT_CFG_SAT_TEXT_WRAPPING_SUPPORT                    (0x02)
#define  BIT_CFG_SAT_TEXT_SCROLLING_SUPPORT                   (0x04)
#define  OFFSET_CFG_SAT_WIDTH_REDUCTION_WHEN_IN_MENU_NUMBER   (0x05)
#define  DEFAULT_CFG_SAT_WIDTH_REDUCTION_WHEN_IN_MENU_NUMBER  (0x02)
#define  MAX_SIZE_CFG_SAT_WIDTH_REDUCTION_WHEN_IN_MENU_NUMBER (0x07)
/* Seventeenth byte */
#define  BIT_CFG_SAT_TCP_CLIENT_REMOTE_SUPPORT                (0x01)
#define  BIT_CFG_SAT_UDP_CLIENT_REMOTE_SUPPORT                (0x02)
#define  BIT_CFG_SAT_TCP_SERVER_SUPPORT                       (0x04)
#define  BIT_CFG_SAT_TCP_CLIENT_LOCAL_SUPPORT                 (0x08)
#define  BIT_CFG_SAT_UDP_CLIENT_LOCAL_SUPPORT                 (0x10)
/* Eighteenth byte */
#define  BIT_CFG_SAT_DISPLAY_TEXT_VARIABLE_TIMEOUT_SUPPORT    (0x01)
#define  BIT_CFG_SAT_GET_INKEY_HELP_WHILE_WAITING_SUPPORT     (0x02)
#define  BIT_CFG_SAT_USB_SUPPORT                              (0x04)
#define  BIT_CFG_SAT_GET_INKEY_VARIABLE_TIMEOUT_SUPPORT       (0x08)
#define  BIT_CFG_SAT_PROVIDE_LOCAL_INFO_ESN_SUPPORT           (0x10)
#define  BIT_CFG_SAT_CALL_CONTROL_ON_GPRS_SUPPORT             (0x20)
#define  BIT_CFG_SAT_PROVIDE_LOCAL_INFO_IMEISV_SUPPORT        (0x40)
#define  BIT_CFG_SAT_PROVIDE_LOCAL_INFO_SEARCH_MODE_CHANGE_SUPPORT    (0x80)
/* Nineteenth byte, reserved for TIA/EIA-136-C facilities */
/* Twentieth byte, reserved for TIA/EIA-820-A facilities */
/* Twenty-first to twenty-seventh bytes: reserved for release 6 and
 *  later features
 */

/* Twentyfirst byte */
#define  BIT_CFG_SAT_WML                                      (0x01)
#define  BIT_CFG_SAT_XHTML                                    (0x02)
#define  BIT_CFG_SAT_HTML                                     (0x04)
#define  BIT_CFG_SAT_CHTML                                    (0x08)
#define  BIT_CFG_SAT_BROWSER_CAP_RFU1                         (0x10)
#define  BIT_CFG_SAT_BROWSER_CAP_RFU2                         (0x20)
#define  BIT_CFG_SAT_BROWSER_CAP_RFU3                         (0x40)
#define  BIT_CFG_SAT_BROWSER_CAP_RFU4                         (0x80)

/* Twentysecond byte */
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_BATTERY_STATE_SUPPORT     (0x02)
/* Twentythird byte */
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_MEID_UTRAN_SUPPORT        (0x20)
#define  BIT_CFG_SAT_CMD_LOCAL_INFO_NMR_UTRAN_SUPPORT         (0x40)

/* Twenty-fifth byte */
#define  BIT_CFG_SAT_EVENT_HCI_CONNECTIVITY_EVENT_SUPPORT     (0x20)
/* Twenty-eighth byte */
#define  BIT_CFG_SAT_ALIGNMENT_LEFT_SUPPORT                   (0x01)
#define  BIT_CFG_SAT_ALIGNMENT_CENTER_SUPPORT                 (0x02)
#define  BIT_CFG_SAT_ALIGNMENT_RIGHT_SUPPORT                  (0x04)
#define  BIT_CFG_SAT_FONT_SIZE_NORMAL_SUPPORT                 (0x08)
#define  BIT_CFG_SAT_FONT_SIZE_LARGE_SUPPORT                  (0x10)
#define  BIT_CFG_SAT_FONT_SIZE_SMALL_SUPPORT                  (0x20)
/* Twenty-ninth byte */
#define  BIT_CFG_SAT_STYLE_NORMAL_SUPPORT                     (0x01)
#define  BIT_CFG_SAT_STYLE_BOLD_SUPPORT                       (0x02)
#define  BIT_CFG_SAT_STYLE_ITALIC_SUPPORT                     (0x04)
#define  BIT_CFG_SAT_STYLE_UNDERLINED_SUPPORT                 (0x08)
#define  BIT_CFG_SAT_STYLE_STRIKETHROUGH_SUPPORT              (0x10)
#define  BIT_CFG_SAT_STYLE_TEXT_FOREGROUND_COLOUR_SUPPORT     (0x20)
#define  BIT_CFG_SAT_STYLE_TEXT_BACKGROUND_COLOUR_SUPPORT     (0x40)

/* Thirtieth byte */
#define  BIT_CFG_SAT_TERMINAL_APPLICATIONS                    (0x04)
#define  BIT_CFG_SAT_PROACTIVE_UICC_ACTIVATE                  (0x10)

/* End  This is copied from the module cnh160625 file si_sat_util.h */



/* Terminal Profile Download structure */
typedef struct {
    /* BYTE 1 */
    /* BIT_PROFILE_DOWNLOAD_SUPPORT
       CFG_SAT_SMS_PP_DATA_DOWNLOAD_SUPPORT,
       CFG_SAT_CB_DATA_DOWNLOAD_SUPPORT,
       CFG_SAT_MENU_SELECTION_SUPPORT,
       CFG_SAT_SMS_PP_DATA_DOWNLOAD_SUPPORT2,
       CFG_SAT_TIMER_EXPIRATION_SUPPORT,
       CFG_SAT_GSM_USSD_IN_CALL_CONTROL_SUPPORT,
       CFG_SAT_GSM_ENVELOPE_REDIAL_SUPPORT   */

    uint8_t                 firstbyte_download;

    /* BYTE 2 */
    /* CFG_SAT_COMMAND_RESULT_SUPPORT,
       CFG_SAT_CALL_CONTROL_SUPPORT,
       CFG_SAT_GSM_CELL_ID_IN_CALL_CONTROL_SUPPORT,
       CFG_SAT_MO_SMS_SIM_CONTROL_SUPPORT,
       CFG_SAT_GSM_HANDLING_OF_ALPHA_ID_SUPPORT,
       CFG_SAT_UCS2_ENTRY_SUPPORT,
       CFG_SAT_UCS2_DISPLAY_SUPPORT,
       CFG_SAT_GSM_DISPLAY_EXTENSION_TEXT_SUPPORT,     */

    uint8_t                 secondbyte_other;


    /* BYTE 3 */
    /*    CFG_SAT_CMD_DISPLAY_TEXT_SUPPORT,
       CFG_SAT_CMD_GET_INKEY_SUPPORT,
       CFG_SAT_CMD_GET_INPUT_SUPPORT,
       CFG_SAT_CMD_MORE_TIME_SUPPORT,
       CFG_SAT_CMD_PLAY_TONE_SUPPORT,
       CFG_SAT_CMD_POLL_INTERVAL_SUPPORT,
       CFG_SAT_CMD_POLLING_OFF_SUPPORT,
       CFG_SAT_CMD_REFRESH_SUPPORT,    */

    uint8_t                 thirdbyte_proactive_uicc;


    /* BYTE 4 */
    /*  CFG_SAT_CMD_SELECT_ITEM_SUPPORT,
       CFG_SAT_CMD_SEND_SHORT_MESSAGE_SUPPORT,
       CFG_SAT_CMD_SEND_SS_SUPPORT,
       CFG_SAT_CMD_SEND_USSD_SUPPORT,
       CFG_SAT_CMD_SET_UP_CALL_SUPPORT,
       CFG_SAT_CMD_SET_UP_MENU_SUPPORT,
       CFG_SAT_CMD_LOCAL_INFO_MCC_SUPPORT,
       CFG_SAT_CMD_LOCAL_INFO_NMR_SUPPORT,    */

    uint8_t                 fourthbyte_proactive_uicc;

    /* BYTE 5 */
    /* CFG_SAT_CMD_SET_EVENT_LIST_SUPPORT,
       CFG_SAT_EVENT_MT_CALL_SUPPORT,
       CFG_SAT_EVENT_CALL_CONNECTED_SUPPORT,
       CFG_SAT_EVENT_CALL_DISCONNECTED_SUPPORT,
       CFG_SAT_EVENT_LOCATION_STATUS_SUPPORT,
       CFG_SAT_EVENT_USER_ACTIVITY_SUPPORT,
       CFG_SAT_EVENT_IDLE_SCREEN_AVAILABLE_SUPPORT,     */
/* Not Supported by Platform:
CFG_SAT_EVENT_CARD_READER_STATUS_SUPPORT,     */

    uint8_t                 fifthbyte_event_driven_info;

    /* BYTE 6 */
    /* CFG_SAT_EVENT_LANGUAGE_SELECTION_SUPPORT,
       CFG_SAT_EVENT_BROWSER_TERMINATION_SUPPORT,
       CFG_SAT_EVENT_DATA_AVAILABLE_SUPPORT,
       CFG_SAT_EVENT_CHANNEL_STATUS_SUPPORT,
       CFG_SAT_EVENT_ACCESS_TECHNOLOGY_CHANGE_SUPPORT,
       CFG_SAT_EVENT_DISPLAY_PARAMETERS_CHANGED_SUPPORT,
       CFG_SAT_EVENT_LOCAL_CONNECTION_SUPPORT,    */

    uint8_t                 sixthbyte_event_driven_info_ext;

    /* BYTE 7 */
/* Not Supported by Platform:  CFG_SAT_CMD_POWER_ON_CARD_SUPPORT,   + 6)); */
/* Not Supported by Platform:  CFG_SAT_CMD_POWER_OFF_CARD_SUPPORT,   + 6)); */
/* Not Supported by Platform:  CFG_SAT_CMD_PERFORM_CARD_APDU_SUPPORT,   + 6)); */
/* Not Supported by Platform:  CFG_SAT_CMD_GET_READER_STATUS_SUPPORT,   + 6)); */
/* Not Supported by Platform:  CFG_SAT_CMD_GET_READER_IDENTIFIER_SUPPORT,   + 6)); */

    uint8_t                 seventhbyte_multiple_card_proactive_command;

    /* BYTE 8 */
    /* CFG_SAT_CMD_TIMER_START_STOP_SUPPORT,
       CFG_SAT_CMD_TIMER_GET_STATUS_SUPPORT,
       CFG_SAT_CMD_LOCAL_INFO_TIME_DATE_SUPPORT,
       CFG_SAT_GSM_GET_INKEY_BINARY_CHOICE_SUPPORT,
       CFG_SAT_SET_IDLE_TEXT_SUPPORT,
       CFG_SAT_RUN_AT_COMMAND_SUPPORT,
       CFG_SAT_GSM_SET_UP_CALL_ALPHA_ID_2_SUPPORT,
       CFG_SAT_GSM_CAPABILITY_CONFIGURATION_2_SUPPORT,   */
    uint8_t                 eigthbyte_proactive_uicc;

    /* BYTE 9 */
    /*
       CFG_SAT_GSM_SUSTAINED_DISPLAY_TEXT_SUPPORT,
       CFG_SAT_CMD_SEND_DTMF_SUPPORT,
       CFG_SAT_GSM_CMD_LOCAL_INFO_BCCH_LIST_SUPPORT,
       CFG_SAT_CMD_LOCAL_INFO_LANGUAGE_SUPPORT,
       CFG_SAT_CMD_LOCAL_INFO_TIMING_ADVANCE_SUPPORT,
       CFG_SAT_CMD_LANGUAGE_NOTIFICATION_SUPPORT,
       CFG_SAT_CMD_LAUNCH_BROWSER_SUPPORT,
       CFG_SAT_CMD_LOCAL_INFO_ACCESS_TECHNOLOGY_SUPPORT,   */

    uint8_t                 ninthbyte;

    /* BYTE 10 */
    /*  CFG_SAT_SOFTKEYS_SELECT_ITEM_SUPPORT,
       CFG_SAT_SOFTKEYS_SETUP_MENU_SUPPORT,   */

    uint8_t                 tenthbyte_soft_keys_support;


    /* BYTE 11 */

    uint8_t                 eleventhbyte_soft_keys_info;

    /* BYTE 12 */
    /*  CFG_SAT_CMD_OPEN_CHANNEL_SUPPORT,
       CFG_SAT_CMD_CLOSE_CHANNEL_SUPPORT,
       CFG_SAT_CMD_RECEIVE_DATA_SUPPORT,
       CFG_SAT_CMD_SEND_DATA_SUPPORT,
       CFG_SAT_CMD_GET_CHANNEL_STATUS_SUPPORT,
       CFG_SAT_SERVICE_SEARCH_SUPPORT,
       CFG_SAT_GET_SERVICE_INFOMATION_SUPPORT,
       CFG_SAT_DECLARE_SERVICE_SUPPORT,    */

    uint8_t                 twelfthbyte;

    /* BYTE 13 */
    /* CFG_SAT_CSD_SUPPORT,
       CFG_SAT_GPRS_SUPPORT,
       CFG_SAT_BLUETOOTH_SUPPORT,
       CFG_SAT_IRDA_SUPPORT,
       CFG_SAT_RS232_SUPPORT,
       OFFSET_CFG_SAT_BEARER_CHANNELS_SUPPORT_NUMBER,
       MAX_SIZE_CFG_SAT_BEARER_CHANNELS_SUPPORT_NUMBER,
       DEFAULT_CFG_SAT_BEARER_CHANNELS_SUPPORT_NUMBER */

    uint8_t                 thirteenthbyte;

    /* BYTE 14 */
    uint8_t                 fourteenthbyte_screen_height;

    /* BYTE 15 */
    uint8_t                 fifteenthbyte_screen_width;

    /* BYTE 16 */
    /*
       CFG_SAT_DISPLAY_CAN_BE_RESIZED_SUPPORT,
       CFG_SAT_TEXT_WRAPPING_SUPPORT,
       CFG_SAT_TEXT_SCROLLING_SUPPORT,    */
    uint8_t                 sixteenthbyte_screen_effects;

    /* BYTE 17 */
    /*
       CFG_SAT_TCP_SUPPORT,
       CFG_SAT_UDP_SUPPORT,
       CFG_SAT_TCP_SERVER_SUPPORT,   */

    uint8_t                 seventeenthbyte;

    /* BYTE 18 */
    /* CFG_SAT_DISPLAY_TEXT_VARIABLE_TIMEOUT_SUPPORT,
       CFG_SAT_GET_INKEY_HELP_WHILE_WAITING_SUPPORT,
       CFG_SAT_USB_SUPPORT,
       CFG_SAT_GET_INKEY_VARIABLE_TIMEOUT_SUPPORT,
       CFG_SAT_PROVIDE_LOCAL_INFORMATION_ESN_SUPPORT,
       CFG_SAT_CALL_CONTROL_ON_GPRS_SUPPORT,   */

    uint8_t                 eighteenthbyte;

    /* Nineteenth byte, reserved for TIA/EIA-136-C facilities */
    uint8_t                 nineteenthbyte;
    /* Twentieth byte, reserved for TIA/EIA-820-A facilities */
    uint8_t                 twentiethbyte;
    /* Twenty-first to twenty-seventh bytes: reserved for release 6 and later features */
    /* BYTE 23 */
    uint8_t                 twentyfirstbyte_ext_lanch_browser_cap;
    uint8_t                 twentysecondbyte;
    uint8_t                 twentythirdbyte;
    uint8_t                 twentyfourthbyte_class_i;

    /* BYTE 25 Event driven information extensions */
    /*     CFG_SAT_EVENT_HCI_CONNECTIVITY_EVENT_SUPPORT */
    uint8_t                 twentyfifthbyte_event_driven_info_ext;

    uint8_t                 twentysixthbyte_event_driven_info_ext;
    uint8_t                 twentyseventhbyte_eventdriven_info_ext;

    /* BYTE 28 text attributes */
/*           CFG_SAT_ALIGNMENT_LEFT_SUPPORT,
           CFG_SAT_ALIGNMENT_CENTER_SUPPORT,
           CFG_SAT_ALIGNMENT_RIGHT_SUPPORT,
           CFG_SAT_FONT_SIZE_NORMAL_SUPPORT,
           CFG_SAT_FONT_SIZE_LARGE_SUPPORT,
     CFG_SAT_FONT_SIZE_SMALL_SUPPORT,      */
    uint8_t                 twentyeighthbyte_text_attr;

    /* BYTE 29 text attributes */
    /* CFG_SAT_STYLE_NORMAL_SUPPORT,
       CFG_SAT_STYLE_BOLD_SUPPORT,
       CFG_SAT_STYLE_ITALIC_SUPPORT,
       CFG_SAT_STYLE_UNDERLINED_SUPPORT,
       CFG_SAT_STYLE_STRIKETHROUGH_SUPPORT,
       CFG_SAT_STYLE_TEXT_FOREGROUND_COLOUR_SUPPORT,
       CFG_SAT_STYLE_TEXT_BACKGROUND_COLOUR_SUPPORT,    */
    uint8_t                 twentyninthbyte_text_attr;
    uint8_t                 thirtiethbyte;
    uint8_t                 thirtyfirstbyte;

} catd_terminal_profile_download_t;

#define ADAPT_CFG_SCAN_CONTROL_APP_UNSUPPORTED      (0x00)
#define ADAPT_CFG_SCAN_CONTROL_APP_SUPPORTED        (0xFF)
#define ADAPT_CFG_NETWORK_SELECTION_APP_UNSUPPORTED (0x00)
#define ADAPT_CFG_NETWORK_SELECTION_APP_SUPPORTED   (0xFF)
#define ADAPT_CFG_ACTING_HPLMN_UNSUPPORTED          (0x00)
#define ADAPT_CFG_ACTING_HPLMN_SUPPORTED            (0xFF)
#define ADAPT_CFG_3G_UNSUPPORTED                    (0x00)
#define ADAPT_CFG_3G_SUPPORTED                      (0xFF)
#define ADAPT_CFG_RAT_BALANCING_UNSUPPORTED         (0x00)
#define ADAPT_CFG_RAT_BALANCING_SUPPORTED           (0xFF)
#define ADAPT_CFG_BIP_UNSUPPORTED                   (0x00)
#define ADAPT_CFG_BIP_SUPPORTED                     (0xFF)
#define ADAPT_CFG_LTE_UNSUPPORTED                   (0x00)
#define ADAPT_CFG_LTE_DATA_ONLY                     (0x01)
#define ADAPT_CFG_LTE_DATA_VOICE_CSFB               (0x02)
#define ADAPT_CFG_LTE_DATA_VOICE_IMS_CSFB           (0x03)
#define ADAPT_CFG_RFU_NULL                          (0x00)

typedef struct {
    uint8_t firstbyte_scan_control_app;
    uint8_t secondbyte_network_selection_app;
    uint8_t thirdbyte_acting_hplmn;
    uint8_t fourthbyte_3g;
    uint8_t fifthbyte_rat_balancing;
    uint8_t sixthbyte_bip;
    uint8_t seventhbyte_lte;
    uint8_t eigthbyte_rfu;
} catd_adapt_terminal_support_table_t;

#ifdef HAVE_ANDROID_OS
    #define SAT_CONFIG_DIR "/etc/SAT/"
#else
#ifndef HOST_FS_PATH
#define HOST_FS_PATH ""
#endif
    #define SAT_CONFIG_DIR HOST_FS_PATH ""
#endif
#define SAT_TERMINAL_PROFILE_PATH SAT_CONFIG_DIR  "terminal_profile"

/*************************************************************************
 * Function:      catd_main_load_terminal_profile
 *
 * Description:   Function to get the required terminal profile to download
 *                to the modem.
 *
 * Input Params: -
 *
 * Return: pointer to the raw profile buffer.
 *
 * Notes: Caller MUST free the returned profile
 *
 *************************************************************************/
const catd_terminal_profile_download_t *catd_main_load_terminal_profile();


/*************************************************************************
 * Function:      catd_main_store_terminal_profile
 *
 * Description:   Function to store the terminal profile persistently.
 *
 * Input Params:  profile_p - Pointer to the buffer of the terminal profile
 *                            to store.
 *                profile_len Length of buffer
 *
 * Return:
 *
 * Notes:
 *
 *************************************************************************/
void catd_main_store_terminal_profile(uint8_t *profile_p, uint8_t profile_len);

/*************************************************************************
 * Function:      catd_main_load_adapt_terminal_support_table
 *
 * Description:   Function to get configuration data to update
 *                the Terminal Support Table FILE on the Adapt sim
 *
 * Input Params:  table_p, pointer to a client-provided support table where to write data. Any contents is overwritten
 *
 * Return: -
 *
 *************************************************************************/
void catd_main_load_adapt_terminal_support_table(catd_adapt_terminal_support_table_t *table_p);

#endif
