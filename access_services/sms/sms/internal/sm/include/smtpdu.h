#ifndef SMTPDU_H
#define SMTPDU_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION:
 *
 *  Short Message TPDU parser include file.
 *
 *************************************************************************/

/*****************************************************************************

*  The module comprises the following sections:
*
* 1.  Public Field Names
*
* 2.  Public Data Names (processed fields)
*
* 3.  Public Data Structures
*
* 4.  Public Function Prototypes
*
**************************************************************************************/

/*************************************************************************************
**
**  Section 1. Public FieldName Constants
**
**  This section contains the recognised field name mnemonics for the TPDU_SetField( )
**  and TPDU_GetField( ) functions. Each field has a short name which fully conforms
**  and corresponds to the fields defined in GSM 03.40. Each short name also has a
**  full desciptive equivalent giving the caller a full or terse commenting option.
**
**************************************************************************************/
#include "t_smslinuxporting.h"

#include "c_sms_config.h"

typedef enum {
    TP_CD, TP_CDL, TP_CT, TP_DA, TP_DCS, TP_DT, TP_FCS, TP_MMS, TP_MN, TP_MR,
    TP_MTI, TP_OA, TP_PI, TP_PID, TP_RA, TP_RD, TP_RP, TP_SCTS, TP_SRI, TP_SRQ,
    TP_SRR, TP_ST, TP_VP, TP_VPF, TP_UD, TP_UDL, TP_UDHI, TP_UDH, TP_UDHL
} fieldName_t;

#define ANY_IE_TYPE                   0xFF
#define TPDU_MAX_FIELDS               (TP_UDHL - TP_CD + 1)

/*************************************************************************************
**
** Section 2. Public dataName Constants
**
**  This section contains the recognised data name mnemonics for the TPDU_Get( )
**  and TPDU_Set( ) functions.
**
**************************************************************************************/
typedef enum {
    ALPHABET,
    ME_DATA_DOWNLOAD,
    ME_DEPERSONALIZATION,
    CLASS,
    SIZE,
    REPLACEMENT_CODE,
    SIM_DOWNLOAD,
    STATUS,
    TRANSFER_PROTOCOL
} dataName_t;


typedef enum {
    TP_VPF_ABSOLUTE_FORMAT = 0x18,
    TP_VPF_ENHANCED_FORMAT = 0x08,
    TP_VPF_RELATIVE_FORMAT = 0x10,
    TP_NO_VP_FIELD = 0
} formatVPF_t;


/*************************************************************************************
**
**  Section 3. Public Data Structures and Declarations
**
*************************************************************************************/
#define SMS_DELIVER           0x00
#define SMS_RESERVED_MESSAGE  0x03
#define SMS_STATUS_REPORT     0x02

#define MAX_SM_ADDRESS_LENGTH    SMR_MAX_SM_ADDRESS_LENGTH      // digits

#define MAX_LENGTH_OF_MSG_WAITING_IND_LIST (6)
#define INVALID_MESSAGE_WAITING_TYPE (0x7f)
#define MAX_LENGTH_OF_MSG_WAITING_TEXT (160)

/*
 * TPDU Definition
 */
#define MAXIMUM_TPDU_SIZE 165

typedef uint8_t Tpdu_t[MAXIMUM_TPDU_SIZE];

typedef uint8_t *TPDU_p;

#define MAX_TP_USER_DATA_LENGTH             (140)

typedef enum {
    CONCAT_SM_8BIT_REF = 0x00,
    SPECIAL_SMS_INDICATION = 0x01,
    RESERVED_FOR_FUTURE_IEI = 0x02,
    NOT_USED = 0x03,
    APPLIC_PORT_ADDR_8BIT = 0x04,
    APPLIC_PORT_ADDR_16BIT = 0x05,
    SMSC_CONTROL_PARAMS = 0x06,
    UDH_SOURCE_INDICATOR = 0x07,
    CONCAT_SM_16BIT_REF = 0x08,
    WAP_WCMP = 0x09,
    TEXT_FORMATTING = 0x0A,
    PREDEFINED_SOUND = 0x0B,
    USER_DEFINED_SOUND = 0x0C,
    PREDEFINED_ANIMATION = 0x0D,
    LARGE_ANIMATION = 0x0E,
    SMALL_ANIMATION = 0x0F,
    LARGE_PICTURE = 0x10,
    SMALL_PICTURE = 0x11,
    VARIABLE_PICTURE = 0x12,
    USER_PROMPT_INDICATOR = 0x13,
    EXTENDED_OBJECT = 0x14,
    REUSED_EXTENDED_OBJECT = 0x15,
    COMPRESSION_CONTROL = 0x16,
    OBJECT_DISTRIBUTION_INDICATOR = 0x17,
    STANDARD_WVG_OBJECT = 0x18,
    CHARACTER_SIZE_WVG_OBJECT = 0x19,
    EXTENDED_OBJECT_DATA_REQUEST_CMD = 0x1A,
    // Reserved for future ESMS features 0x1B - 0x1F
    RFC822_EMAIL_HEADER = 0x20,
    HYPERLINK_FORMAT_ELEMENT = 0x21,
    REPLY_ADDRESS_ELEMENT = 0x22
        // Reserved for future use           0x23 - 0x6F
        // (U)SIM Toolkit Security Headers   0x70 - 0x7F
        // SME to SME specific use           0x80 - 0x9F
        // Reserved for future use           0xA0 - 0xBF
        // SC specific use                   0xC0 - 0xDF
        // Reserved for future use           0xE0 - 0xFF
} eSMS_IEI_codes_t;


typedef enum {
    MWI_VOICE_MAIL_LINE1,
    MWI_VOICE_MAIL_LINE2,
    MWI_FAX,
    MWI_EMAIL,
    MWI_OTHER_MAIL,
    MWI_VIDEO_MAIL
} MWI_Type_t;

typedef struct {
    MWI_Type_t Type;
    uint8_t Count;
} MessageWaitingIndication_t;


typedef struct {
    MessageWaitingIndication_t MWI_Indication[MAX_LENGTH_OF_MSG_WAITING_IND_LIST];
    uint8_t MWIList_Index;
    uint8_t ALS_LineDetermined; // If this is set, then we have positively determined the line number. Otherwise it has defaulted to line 1.
    uint8_t TextAssociated;
    uint8_t DiscardShortMessage;
} MWIList_t;

typedef MWIList_t *MWIList_p;


typedef SMS_SubscriberNumber_t ShortMessageSubscriberNumber_t;


/*************************************************************************************
**
**  Section 4. Public Function Prototypes
**
*************************************************************************************/

uint8_t TPDU_ResetUserDataIE(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU);

uint8_t TPDU_AddUserDataIE(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t alphabet,
    uint8_t typeIEI,
    uint8_t lengthIEI,
    uint8_t * dataIEI_p);

uint8_t TPDU_COMMAND_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_SRR,
    uint8_t * PID_p,
    uint8_t * CT_p,
    uint8_t * MN_p,
    ShortMessagePackedAddress_t * DA_p);

uint8_t TPDU_DELIVER_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_MMS,
    uint8_t Tp_SRI,
    uint8_t Tp_RP,
    ShortMessagePackedAddress_t * OA_p,
    uint8_t * PID_p,
    uint8_t * DCS_p,
    uint8_t * SCTS_p);

uint8_t TPDU_DELIVER_REPORT_PackHdr(
    const TPDU_p thisTPDU,
    TP_FailureCause_t Tp_FCS,
    uint8_t * PID_p,
    uint8_t * DCS_p);

uint8_t TPDU_Get(
    dataName_t thisData,
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU);

uint8_t TPDU_GetApplicationPorts(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint16_t * destPort_p,
    uint16_t * origPort_p);

uint8_t TPDU_GetBit(
    fieldName_t thisField,
    const TPDU_p thisTPDU);

void TPDU_GetConcatInfo(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    SMS_ConcatParameters_t * thisConcat);

uint8_t TPDU_GetField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU);

uint8_t TPDU_GetFieldXfr(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t * out_p);

uint8_t *TPDU_GetFieldPtr(
    fieldName_t thisField,
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU);


uint8_t TPDU_GetPackedAddress(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    ShortMessagePackedAddress_t * out_p);

uint8_t TPDU_GetPtr(
    dataName_t thisData,
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU);

uint8_t TPDU_GetVoiceMailInfo(
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    MWIList_p thisMsg);

uint8_t TPDU_GetUserDataIE(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t typeIE,
    uint8_t instanceIE,
    uint8_t * dest_p);

uint8_t TPDU_PackText(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t textLength,
    uint8_t textAlphabet,
    uint8_t * text_p);

uint8_t TPDU_CopyText(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t textLength,
    uint8_t textAlphabet,
    uint8_t * text_p);

uint8_t TPDU_Set(
    dataName_t thisData,
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU);

void TPDU_SetBit(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    uint8_t newBit);

uint8_t TPDU_SetField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t * newField,
    uint8_t newFieldSize);

uint8_t TPDU_STATUS_REPORT_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_MMS,
    uint8_t Tp_SRQ,
    ShortMessagePackedAddress_t * RA_p,
    uint8_t * SCTS_p,
    uint8_t * Tp_DT,
    uint8_t Tp_ST,
    uint8_t * PID_p,
    uint8_t * DCS_p);

uint8_t TPDU_SUBMIT_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_RD,
    formatVPF_t Tp_VPF,
    uint8_t Tp_SRR,
    uint8_t Tp_RP,
    ShortMessagePackedAddress_t * DA_p,
    uint8_t * PID_p,
    uint8_t * DCS_p,
    uint8_t * VP_p);

uint8_t TPDU_SUBMIT_REPORT_PackHdr(
    const TPDU_p thisTPDU,
    TP_FailureCause_t Tp_FCS,
    uint8_t * SCTS_p,
    uint8_t * PID_p,
    uint8_t * DCS_p);

uint8_t TPDU_TestBit(
    fieldName_t thisField,
    const TPDU_p thisTPDU);

uint8_t TPDU_UnpackText(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t * dest_p);

uint8_t TPDU_PackUserData(
    const TPDU_p,
    uint8_t,
    uint8_t,
    uint8_t,
    uint8_t *);

uint8_t TPDU_AddIEToUserDataHeader(
    const TPDU_p,
    uint8_t,
    uint8_t,
    uint8_t,
    uint8_t,
    uint8_t *);

uint8_t CheckTPUserDataIntegrity(
    const uint8_t *,
    const uint8_t);

uint8_t CheckObjectDataIntegrity(
    const uint8_t *);

uint8_t CheckValidObjectIEI(
    const eSMS_IEI_codes_t);

uint8_t GetStatusReportTPDU(
    const uint8_t * const SrcTPDU,
    uint8_t * const DestTPDU);

/******************************* end of smTPDU.h *************************************/

#endif                          // SMTPDU_H
