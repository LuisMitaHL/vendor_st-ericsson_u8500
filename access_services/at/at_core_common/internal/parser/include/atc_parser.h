/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_PARSER_H
#define ATC_PARSER_H

/*************************************************************************
 * Includes
 *************************************************************************/

/* ATC common related header files */
#include <atc_config.h>
#include <atc_connection.h>
#include <atc_anchor.h>
#include <atc_common.h>
#include <atc_command_list.h>
#include <exe.h>

#include <stdbool.h>
#include <stdint.h>


/*************************************************************************
 * Types, constants and external variables
 *************************************************************************/

/* remove... this does not work...typedef int AT_Command_e;  for now... (the enum is in the backend) */

/* TODO: Idea. Split AT_Command_e into commands and result codes and have a new type combining both. Could solve move to backend. */

#define MAX_NO_OF_OPEN_CHANNELS                    (MAX_NBR_CONNECTIONS)
#define PARSER_STATE_TABLE_ENTRY_NOT_FOUND      (MAX_NBR_CONNECTIONS)

#define AT_STRING_PARAM_BOUNDER 0x22 /* String parameter bounder ("). */
#define AT_COMMAND_SEPARATOR    0x3B /* Separator for AT commands (;). */
#define AT_D_COMMAND_SEPARATOR  0x23 /* Separator for AT D* commands (#). */
#define AT_PARAMETER_SEPARATOR  0x2C /* Separator for command parameters (,). */
#define AT_D_PARAMETER_SEPARATOR 0x2A /* Separator for AT D* command parameters (*). */
#define AT_COMMAND_LINE_PREFIX  "AT" /* Prefix for a command line. */
#define NO_TRANSLATION          '?'  /* Used for unknown characters. */
#define RESULT_TEXT_SIZE        2048 /* Maximum result text size (allocate worst case scenario in +CGMI, +CGMM, +CGMR, +GMM and +GMR) */
#define RSSI_VALUE_UNKNOWN      0x63 /* Value indicating rssi not known */

/* Define an enumeration of AT Command Classes. */
typedef enum /* AT Command classes: */
{
    AT_CLASS_UNDEFINED = 0x00,  /* ... undefined command class. */
    AT_CLASS_ABORTABLE = 0x01,  /* ... abortable commands. */
    AT_CLASS_PROTECTED = 0x02,  /* ... commands protected by PIN & PH-SIM PIN. */
    AT_CLASS_BASIC = 0x04,      /* ... basic modem commands. */
    AT_CLASS_ERICSSON = 0x08,   /* ... Ericsson specific commands. */
    AT_CLASS_GSM_BASIC = 0x10,  /* ... basic GSM 07.07 commands. */
    AT_CLASS_GSM_SMS = 0x20,    /* ... commands for SMS handling (GSM 07.05). */
    AT_CLASS_GSM_SS = 0x40,     /* ... commands for supplementary services. */
    AT_CLASS_DEBUG = 0x80,      /* ... commands exclusively for debugging. */
    AT_CLASS_ALL = 0xFF
                   /* ... all command classes. */
} AT_CommandClass_e;

/* Define an enumeration of AT Command Modes. */
typedef enum /* AT Command modes: */
{
    AT_MODE_UNDEFINED = 0x00,   /* ... undefined command mode. */
    AT_MODE_NONE = 0x00,        /* ... no mode. */
    AT_MODE_DO = 0x01,          /* ... action command with no parameters. */
    AT_MODE_SET = 0x02,         /* ... command with parameters. */
    AT_MODE_READ = 0x04,        /* ... the current settings shall be read. */
    AT_MODE_TEST = 0x08,        /* ... test if the command is implemented. */
    AT_MODE_ALL = 0x0F
                  /* ... all modes. */
} AT_CommandMode_e;

/* Define an enumeration of valid bases for the AT_AddBasedValue function. */
typedef enum /* Integer bases: */
{
    AT_HEX_INT = 0x10, /* ... hexadecimal integer. */
    AT_DEC_INT = 10
                 /* ... decimal integer. */
} AT_IntegerBase_e;

/* Define a type for AT command lines. */
typedef unsigned char *AT_CommandLine_t;

/* Define a general type for AT command response message pointers. */
struct AT_ResponseMessage_s {
    exe_request_result_t exe_result_code;
    void *data_p;
    int len;
};
typedef struct AT_ResponseMessage_s *AT_ResponseMessage_t;

#define AT_RESULT_UNDEFINED 255      /* Used with AT_CommandResult_type defined in AT module.
Use 255 to avoid conflict. */

typedef enum {
    ETZR_DISABLED                                = 0,
    ETZR_TIMEZONE                                = 1,
    ETZR_TIMEZONE_TIME_TIMESTAMP                 = 2,
    ETZR_TIMEZONE_TIME_TIMESTAMP_DAYLIGHT_SAVING = 3
} ETZR_format_t;

typedef enum {
    NET_REG_IND_DISABLED,
    NET_REG_IND_NETWORK_STATUS,
    NET_REG_IND_LAC_AND_CELL_ID
} net_reg_ind_type_t;

typedef enum {
    ATC_CHARSET_UTF8 = 0,
    ATC_CHARSET_HEX,
    ATC_CHARSET_GSM7,
    ATC_CHARSET_GSM8,
    ATC_CHARSET_IRA,
    ATC_CHARSET_8859_1,
    ATC_CHARSET_UCS2,
    ATC_CHARSET_UNDEF_BINARY,
    ATC_CHARSET_MAX /* The number of different character sets. */
} atc_charset_t;

#define CHARACTER_SET_DEFAULT ATC_CHARSET_UTF8

typedef struct { /* Structure for character set lookup table entries: */
    atc_charset_t cscs_charsetval; /* Index of character set. */
    char *cscs_charsetstring_p;                  /* Name of character set. */
} CSCS_CharSetLookupTableRecord_s;

/* Define an enumeration of ME and network facility lock codes. */
typedef enum {                  /* Lock codes for GSM facility locks: */
    ATC_FAC_CNTRL,                  /*   Lock CoNTRoL surface (e.g. phone keyboard). */
    ATC_FAC_PH_SIM,                 /*   Lock PHone to SIM card, ME asks password when other */
    /*   than current SIM card inserted. */
    ATC_FAC_PH_FSIM,                /*   Lock PHone to the very first inserted SIM card, ME asks */
    /*   password when other than the first SIM card is inserted. */
    ATC_FAC_SIM_PIN,                /*   Lock SIM card, SIM asks password in ME power-up and */
    /*   when this lock command issued. */
    ATC_FAC_SIM_PIN2,               /*   Enable PIN2 authentication. */
    /* Barring services refers to GSM 02.88 clause 1 and 2: */
    ATC_FAC_BAOC,                   /*   Barr All Outgoing Calls. */
    ATC_FAC_BOIC,                   /*   Barr Outgoing International Calls. */
    ATC_FAC_BOIC_exHC,              /*   Barr Outgoing International Calls except to home country. */
    ATC_FAC_BAIC,                   /*   Barr All Incoming Calls. */
    ATC_FAC_BAIC_Roam,              /*   BIC-Roam, Barr Incoming Calls when Roaming outside the home country. */
    /* Barr incoming calls from: */
    ATC_FAC_BInotTA,                /*   Numbers Not stored to TA memory. */
    ATC_FAC_BInotME,                /*   Numbers Not stored to ME memory. */
    ATC_FAC_BInotSIM,               /*   Numbers Not stored to SIM memory. */
    ATC_FAC_BInotAny,               /*   Numbers Not stored in Any memory. */
    /* For the following services refer to GSM 02.30: */
    ATC_FAC_BAll,                   /*   All Barring services. */
    ATC_FAC_BAllOut,                /*   All outGoing barring services. */
    ATC_FAC_BAllIn,                 /*   All inComing barring services. */
    ATC_FAC_FDN,                    /*   SIM fixed dialing memory feature, if PIN2. */
    /*   authentication has not been done during the current. */
    /*   session, PIN2 is required as <passwd>. */
    ATC_FAC_ESL,                    /*   Extended SimLock */
    ATC_FAC_ACL,                    /*   APN Control List. */
    ATC_FAC_NetPer,                 /*   Network Personalizations. */
    ATC_FAC_NetUbPer,               /*   Network sUbset Personalizations. */
    ATC_FAC_SerPer,                 /*   Service Provider Personalizations. */
    ATC_FAC_CorPer,                 /*   Corporate Personalizations. */
    ATC_FAC_LL,                     /*   LineLock for ALS (Orange specific SIM field, see *ELIN). */
    ATC_FAC_NR_FACILLITIES,         /*   This is the number of valid codes. */
    ATC_FAC_ERROR
    /*   The given facility code was invalid. */
} atc_facility_locks_t;

typedef struct atc_pending_queue_s atc_pending_queue_t;

struct atc_pending_queue_s {
    AT_Command_e override;
    AT_CommandLine_t response;
    atc_pending_queue_t *next;
};

typedef struct cpms_msg_storage_s {
    msg_storage_t mem1;
    msg_storage_t mem2;
} cpms_msg_storage_t;

typedef enum {
    ATC_SMS_INDICATION_NONE,
    ATC_SMS_INDICATION_CMT,
    ATC_SMS_INDICATION_CMTI
} atc_sms_indication_t;

typedef enum {
    COLP_DISABLED = 0,
    COLP_ENABLED = 1,
} colp_state_e;

typedef struct cnmi_message_subscribe_s {
    bool mt;
    bool bm;
} cnmi_message_subscribe_t;

typedef struct cnmi_message_indication_s {
    unsigned char mode;
    unsigned char mt;
    unsigned char bm;
    unsigned char ds;
    unsigned char bfr;
} cnmi_message_indication_t;


typedef enum {
    ATD_CALL_TYPE_NONE,
    ATD_CALL_TYPE_CS_VOICE,
    ATD_CALL_TYPE_PS_DATA
} atd_call_type_t;

typedef enum {
    CMER_MODE_BUFFER  = 0,
    CMER_MODE_FORWARD = 3
} cmer_mode_t;

typedef enum {
    CMER_IND_DEACTIVATED = 0,
    CMER_IND_ACTIVATED = 1
} cmer_ind_t;

/* Used in the translation from the cn_signal_info_t's rssi_dbm to the +CIEV signal quality (0-5) */
/* Data to the border dBm levels is manufacturer specific and based on GSM 07.07 */
typedef enum {
    CIEV_SIGNAL_DBM_BORDER_0 = 108,   /* Border in dBm between 0 and 1 bar */
    CIEV_SIGNAL_DBM_BORDER_1 = 104,   /* Border in dBm between 1 and 2 bars */
    CIEV_SIGNAL_DBM_BORDER_2 = 98,    /* Border in dBm between 2 and 3 bars */
    CIEV_SIGNAL_DBM_BORDER_3 = 90,    /* Border in dBm between 3 and 4 bars */
    CIEV_SIGNAL_DBM_BORDER_4 = 80,    /* Border in dBm between 4 and 5 bars */
    CIEV_SIGNAL_DBM_UNDEFINED = 0,    /* No signal, 0 bars */
} ciev_signal_dbm_borders_t;

typedef enum {
    CIEV_0_BARS = 0,
    CIEV_1_BAR,
    CIEV_2_BARS,
    CIEV_3_BARS,
    CIEV_4_BARS,
    CIEV_5_BARS,
    CIEV_UNKNOWN_BARS = 0xFF,
} ciev_signal_bars_t;

typedef enum {
    CUUS1_MSG_ANY = 0,
    CUUS1_MSG_SETUP = 1,
    CUUS1_MSG_ALERT = 2,
    CUUS1_MSG_CONNECT = 3,
    CUUS1_MSG_DISCONNECT = 4,
    CUUS1_MSG_RELEASE = 5,
    CUUS1_MSG_RELEASE_COMPLETE = 6
} cuus1_message_type_t;

typedef enum {
    CUUS1I_MSG_ANY = 0,
    CUUS1I_MSG_ALERT = 1,
    CUUS1I_MSG_PROGRESS = 2,
    CUUS1I_MSG_CONNECT = 3,
    CUUS1I_MSG_RELEASE = 4
} cuus1i_message_type_t;

typedef enum {
    CUUS1U_MSG_ANY = 0,
    CUUS1U_MSG_SETUP = 1,
    CUUS1U_MSG_DISCONNECT = 2,
    CUUS1U_MSG_RELEASE_COMPLETE = 3
} cuus1u_message_type_t;

/* 3GPP TS 24.008 10.5.4.25 states max is 35 of 131 bytes
 * Modem could have other limits.
 * Just use the maximum possible and let the modem/network decide.
 */
#define MAX_CUUS1_MSG_SIZE (UINT8_MAX)

/* The UUS Information Element Identifier */
#define CUUS1_IEI (0x7E)

/*************************************************************************
 *
 * Name: AT_ParserState_s
 *
 * Description: This type contains the current state of the parser.
 *
 *
 * Last change: 020208  QCSPARD Imported from SB
 *
 **************************************************************************/

typedef struct AT_ParserState_struct { /* Structure for AT parser state: */
    ContextId_t ContextID; /* ... context id for the channel on */

    atc_connection_type_t connection_type;

    unsigned char AndC;   /* ... carrier detect signal behavior. */
    unsigned char CR;     /* ... service reporting control. */
    unsigned char CRC;    /* ... cellular result codes. */
    unsigned char CLIP;   /* Calling Line Identification Presentation */
    unsigned char CNAP;   /* Calling Name Identification Presentation */
    unsigned char ECME;   /* CME unsolicited error code reporting enable and disable. */
    unsigned char ECRAT;  /* CME unsolicited error code reporting of RAT name indications */
    unsigned char ENNIR;  /* Network Name Information */
    unsigned char ELAT;   /* Enable/disable unsolicited reporting of local tone generation indications */
    unsigned char ERFSTATE;/* RF state unsolicited reporting, enable and disable */
    unsigned char AndD;   /* ... DCE response control. */
    unsigned char S0;     /* ... automatic answer control character. */
    char S3; /* ... termination character. */
    char S4; /* ... response formatting character. */
    char S5; /* ... deletion character. */
    unsigned char S6; /* ... pause before blind dialling. */
    unsigned char S7; /* ... connection completion timeout. */
    unsigned char S8; /* ... comma dial modifier time. (dummy) */
    unsigned char S10; /* ... automatic disconnect delay control. */
    unsigned char X;  /* ... call progress monitoring control. */
    BitField_t Echo : 1; /* ... echo mode flag. */ /* SHOULD BE REMOVED FOR PAS */

    BitField_t VerboseResponse : 1; /* ... verbose response mode flag. */
    BitField_t SuppressResponse : 1; /* ... suppress response mode flag. */

    IntParam_t at_plus_cops_format; /* ... operator format ... */
    IntParam_t CUSD; /* ... code presentation enabler ... */

    BitField_t CMEE : 2; /* ... GSM error response mode. */
    BitField_t IsPending : 1; /* ... there are pending AT command(s). */
    bool isWaiting; /* Waiting for AtService answer */
    BitField_t NoMoreParams : 1; /* ... the command has no more params. */
    size_t CommandLineLength;     /* ... length of current command line */
    AT_CommandLine_t CommandLine; /* ... current unfinished command line. */
    AT_CommandLine_t Pos; /* ... position in the command line. */
    AT_Command_e Command; /* ... currently executed command. */
    int CommandTableIndex; /* ... index in command table for currently executed command. */

    BitField_t Mode : 4; /* ... operation mode for the command. */
    BitField_t EnabledCommandClasses : 8; /* ... command classes enabled. */

    unsigned int CMEE_ErrorType; /* ... last gsm specific error code. */
    unsigned int CMS_ErrorType; /* ... last gsm specific error code. */

    atc_pending_queue_t *pending_queue_p;

    BitField_t EASY : 1; /* System Event Alert */
    BitField_t ECAM : 1; /* Extended call monitoring */
    BitField_t BSIR : 1; /* In-band ring tone setting */

    uint16_t PDU_Length;   /* .. Length of PDU data given in AT command */
    uint8_t Stat;          /* Status for sms to store */
    uint8_t sub_state;     /* Substate of AT Command. Used by CGCMOD, CPIN, ECEXPIN and CLCK. */
    uint8_t sms_ack_type;  /* Type of message acknowledgement to send */

    /* Used by the CSSN command */
    BitField_t            CSSI               : 1;  /* Presentation of CSSI.   */
    BitField_t            CSSU               : 1;  /* Presentation of CSSU.   */
    BitField_t            CGEV               : 1;  /* PS data information, used by the CGEREP command */
    BitField_t            ESIMSR             : 1;  /* Ericsson SIM State Reporting. */
    BitField_t            ESIMRF             : 1;  /* Ericsson SIM Refresh State Reporting. */
    BitField_t            EPEE               : 1;  /* Ericsson SIM State Reporting. */

    /* Used by the CCWA command */
    BitField_t            CCWA_n             : 1;  /* Presentation of CCWA unsolicited events. 1 is enabled  */

    /* Used by the AT+CMER command */
    ciev_signal_bars_t    CIEV_signal;             /* Last issued signal quality +CIEV: 2,<n> unsolicited event */
    uint8_t               CMER_mode;               /* Presentation or hiding of Mobile Equipment Event Reporting. Values 0 and 3 are supported.  */
    BitField_t            CMER_ind           : 1;  /* Presentation or hiding of the +CIEV unsolicited event. 1 is enabled  */

    /* Used by the CUUS1 command */
    BitField_t            CUUS1I             : 1;  /* Presentation of CUUS1I.   */
    BitField_t            CUUS1U             : 1;  /* Presentation of CUUS1U.   */
    cuus1_message_t       CUUS1_setup_msg;         /* Cached outgoing CUUS SETUP message*/

    bool abort;
    int request_handle; /* request handle (needed in abort scenarios) */

    AT_Command_e(*HandlerFunction)(struct AT_ParserState_struct *,
                                   AT_CommandLine_t, AT_ResponseMessage_t);
    net_reg_ind_type_t CREG; /* Storage for CREG value from set command */
    net_reg_ind_type_t EREG; /* Storage for EREG value from set command */
    net_reg_ind_type_t CGREG; /* Storage for CGREG value from set command */

    atc_charset_t cscs_charset;

    exe_stkc_pc_request_mode_t stkc_pc_enable;

    ETZR_format_t ETZR;
    BitField_t EPSB;
    atc_facility_locks_t facility_code;
    exe_clck_mode_t      clck_mode;
    unsigned char    clck_class;
#ifdef AT_PLUS_CPIN_ENABLED
    bool get_sim_state; /*In the process of getting the current sim state*/
    exe_cpin_code_t cpin_code; /* Store the SIM state code */
#endif
    cpms_msg_storage_t cpms_msg_storage;
    cnmi_message_indication_t cnmi_message_indication;
    AT_Command_e SuccessResult; /* Result to report upon a handler completed with EXE_SUCCESS */
    colp_state_e colp_state;
    atd_call_type_t atd_call_type;
    BitField_t connecting_voice_call : 1; /* Set to true if MO call was initiated */
    exe_empage_state_t empage_state;
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_atc_extended_at_parser_state_s.h>
#endif
} AT_ParserState_s;


/*************************************************************************
 * Declaration of functions
 **************************************************************************/
void AT_AddRange(AT_CommandLine_t info_text, char *Format, unsigned char Min,
                 unsigned char Max);

void AT_AddBasedValue(AT_CommandLine_t info_text, char *Format,
                      signed long Value, AT_IntegerBase_e Base);

#define AT_AddValue(INFOTEXT, FORMAT, VALUE) \
    AT_AddBasedValue( (INFOTEXT), (FORMAT), (VALUE), AT_DEC_INT )

bool AT_AddString(AT_CommandLine_t InfoText, char *buffer_p);

char_param_t parser_get_char_param(AT_ParserState_s *parser_p,
                                   bool *error_p, char_param_t *default_p);

IntParam_t Parser_GetIntParam(AT_ParserState_s *parser_p, bool *Error,
                              IntParam_t *Default);

IntParam_t Parser_GetDIntParam(AT_ParserState_s *parser_p, bool *Error,
                               IntParam_t *Default);

unsigned long Parser_GetLongIntParam(AT_ParserState_s *parser_p,
                                     bool *Error, unsigned long *Default, unsigned long Limit);

void Parser_SendResponse(AT_ParserState_s *parser_p, AT_Command_e Override,
                         AT_CommandLine_t ParamString);

void Parser_SendMultipleResponses(AT_ParserState_s *parser_p, AT_Command_e Override,
                                  AT_CommandLine_t ParamString, bool LastResponse);

bool Parser_ParseCommandLine(AT_ParserState_s *parser_p,
                             AT_ResponseMessage_t Message);

AT_ParserState_s *ParserStateTable_GetParserState_ByEntry(unsigned char Entry);

char *atc_get_response(AT_Command_e response, bool verbose);

unsigned char ParserStateTable_GetFreeEntry(void);
void ParserStateTable_ResetEntry(unsigned char Entry);
void ParserStateTable_UpdateEntry(unsigned char Entry, ContextId_t ContextID);

void Parser_AT(uint8_t *MsgData_p, AT_LineLength_t MsgDataLength,
               AT_DataType_t TypeOfData, AT_ParserState_s *parser_p);

void init_parser_states(void);
char Parser_GetS3(AT_ParserState_s *parser_p);
char parser_get_s4(AT_ParserState_s *parser_p);
char parser_get_s5(AT_ParserState_s *parser_p);

AT_Command_e AT_Flag_Handle(AT_ParserState_s *parser_p,
                            AT_CommandLine_t info_text_p, unsigned char *Flag, unsigned char Limit);

AT_Command_e AT_Flag_Handle_Default(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t InfoText,
                                    unsigned char *Flag,
                                    unsigned char Limit,
                                    unsigned char *Default);

AT_Command_e parser_parse_int_param(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t InfoText, IntParam_t *Value_p,
                                    IntParam_t LoLim, IntParam_t UpLim, IntParam_t Default);

int parser_write_formatted_text(AT_ParserState_s *parser_p, unsigned char *data,
                                unsigned int length);

int parser_write_unformatted_text(AT_ParserState_s *parser_p, unsigned char *data,
                                  unsigned int length);

void Parser_SetConnectionType(AT_ParserState_s *parser_p,
                              atc_connection_type_t connection_type);

void Parser_SetDefaultState(AT_ParserState_s *parser_p);

void Parser_ResetCommandSettings(AT_ParserState_s *parser_p);

StrParam_t Parser_GetHexParamAsStr(AT_ParserState_s *parser_p,
                                   bool *Error_p, StrParam_t Default_p);
StrParam_t Parser_GetBinParamAsStr(AT_ParserState_s *parser_p,
                                   bool *Error_p, StrParam_t Default_p);

StrParam_t Parser_GetStrParam(AT_ParserState_s *parser_p, bool *Error,
                              StrParam_t Default);

void Parser_SetCMEE_ErrorType(AT_ParserState_s *parser_p, exe_cmee_error_t CMEE_ErrorType);
void Parser_SetCMS_ErrorType(AT_ParserState_s *parser_p,
                             unsigned int CMS_ErrorType);
void Parser_SetSuccessResult(AT_ParserState_s *parser_p,  AT_Command_e result);
bool parser_supports_async_cgact(AT_ParserState_s *parser_p);
bool Parser_DecodeEscapeSequences(AT_CommandLine_t StartOfParam);

/*************************************************************************/
#endif /* ATC_PARSER_H */
