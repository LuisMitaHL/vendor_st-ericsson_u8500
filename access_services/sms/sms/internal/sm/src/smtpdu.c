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
 *  The Short Message TPDU Object and it's methods.
 *
 *************************************************************************/
 /*
  **********************************************************************************
  * The module comprises four parts each divided into a number of separate sections.
  * The first part contains module data definitions and constants. The second part
  * implements the TPDU object for the SMS as defined in GSM 03.40 version 7.4.0
  * Release 1998 and consists of sections 4 to 6 inclusive. The third part holds
  * specific SMS utilities and comprises sections 7 to 17. The fourth part contains
  * the error reporting and the debug (TRACE) functions. The last part contains
  * a full regression test harness which is conditionally compiled. In general terms
  * the second part operates directly on complete SMS fields and the third part accesses
  * those fields for more specific operations. The final part (from section 30. onwards)
  * is the test harness and associated support utilities.
  *
  * Conditional compilation switches are defined in section 1.1  and control:
  *
  *     - inbuilt test harness    (WIN32 builds only)
  *     - full error reporting    (all builds)
  *     - brief error reporting   (all builds)
  *     - real time trace facility  (all builds)
  *
  * Note that functions that are called from outside the module are denoted by names
  * beginning with 'TPDU_ ..', internal functions are named otherwise.
  *
  * Also note the term "soctet" / "SOCTET" this is used where a variable may be in either
  * octets or septets dependent upon the message alphabet.
  *
  * The individual sections reflect the software structure and are given below.
  *
  * 0.  Compile Time Switch Notes
  *
  * 1.  Include Files
  *
  * 2.  Local Constants and Definitions
  *   2.1 General Constant Definitions
  *   2.2 Field Characteristics Definitions
  *   2.3 Macro Definitions
  *   2.4 Data Definitions
  *   2.5 TPDU Error Definitions
  *
  * 3.  Local Function Prototypes
  *   3.1 MWI Order definition array
  *
  * 4.  TPDU Parse Functions
  *   4.1 parseTPDU( )
  *     4.1.1 parse_SM_DELIVER( )
  *     4.1.2 parse_SM_DELIVER_REPORT_ERROR( )
  *     4.1.3 parse_SM_DELIVER_REPORT_ACK( )
  *     4.1.4 parse_SM_SUBMIT( )
  *     4.1.5 parse_SM_SUBMIT_REPORT_ERROR( )
  *     4.1.6 parse_SM_SUBMIT_REPORT_ACK( )
  *     4.1.7 parse_SM_STATUS_REPORT( )
  *     4.1.8 parse_SM_COMMAND( )
  *   4.2 Support Functions for parseTPDU( )
  *     4.2.1 parsePIField( )
  *     4.2.2 parsePID_DCSFields( )
  *     4.2.3 parseUDFields( )
  *     4.2.3aparseUDHFields( )
  *     4.2.4 resetParseData( )
  *     4.2.5 getVPFieldSize( )
  *     4.2.6 parseAddressField( )
  *
  * 5.  TPDU Get Field Functions
  *   5.1 TPDU_GetFieldPtr( )
  *   5.2 TPDU_GetField( )
  *   5.3 TPDU_GetFieldXfr( )
  *   5.4 parseField( )
  *   5.5 processField( )
  *   5.6 fieldValid( )
  *
  * 6.  TPDU_SetField( )
  *   6.1 checkNewField( )
  *   6.2 replaceBitField( )
  *   6.3 replaceOctetField( )
  *   6.4 setDerivedFields( )
  *
  * 7.  TPDU Pack Header Notes
  *   7.1 TPDU_DELIVER_PackHdr( )
  *   7.2 TPDU_DELIVER_REPORT_PackHdr( )
  *   7.3 TPDU_SUBMIT_PackHdr( )
  *   7.4 TPDU_SUBMIT_REPORT_PackHdr( )
  *   7.5 TPDU_STATUS_REPORT_PackHdr( )
  *   7.6 TPDU_COMMAND_PackHdr( )
  *   7.7 packField( )
  *
  * 8.  TPDU_PackText( )
  *   8.1 packSeptets( )
  *   8.2 memcpyUNICODE( )
  *
  * 9.  TPDU_UnpackText( )
  *   9.1 extractText( )
  *   9.2 unpackSeptets( )
  *
  * 10. TPDU_GetVoiceMailInfo( )
  *   10.1 getSpecialMessageInfo( )
  *   10.2 getVoiceMailInfo_PCN( )
  *   10.3 getVoiceMailInfo_DCS( )
  *   10.4 msgWaitingType( )
  *   10.5 isOrangeVoiceMail( )
  *
  * 11. TPDU_Get( )
  *   11.1 extractAlphabetFromDCS( )
  *   11.2 extractMesssageClass( )
  *   11.3 extractTransferProtocol( )
  *   11.4 isOBEX( )
  *   11.5 isWAP( )
  *   11.6 isPictureMessage( )
  *   11.7 isChat( )
  *   11.8 isEmailClientProvisioning( )
  *
  * 12. TPDU Bit Functions
  *   12.1 TPDU_GetBit( )
  *   12.2 TPDU_SetBit( )
  *   12.3 TPDU_TestBit( )
  *
  * 13. TPDU_GetConcatInfo( )
  *
  * 14. TPDU_AddUserDataIE( )
  *   14.1 setAlphabet( )
  *
  * 15. TPDU_GetPackedAddress( )
  *
  * 16. TPDU_GetUserDataIE( )
  *   16.1 extractUDH_IE( )
  *
  * 17. TPDU_GetApplicationPorts( )
  *   17.1 findApplicationPorts( )
  *
  *
  *
  **************************************************************************************/


/*************************************************************************************
**
**  Section 0.  Compile Time Switch Notes.
**
**    The overall operating mode is controlled by the following compiler switches:
**
**    PRINT_A_  Fatal/normal error reporting with major system functions
**
**    Note only one compiler switch is required at any one time. It is possible to
**    compile without any switches with no error reporting at all.
**
**************************************************************************************/

/*************************************************************************************
**
**  Section 1. Include Files
**
**************************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"

/* Requester Interface definitions */
//#include "r_sys.h"

/* The Message Transport Server Interface */
#include "g_sms.h"
#include "t_sms.h"
#include "r_sms.h"

/* Message Module Include Files */
#include "smutil.h"
#include "sm.h"
#include "smtpdu.h"


/*************************************************************************************
**
**  Section 2.1 General Constant Definitions
**
**************************************************************************************/

#define ADDRESS_FIELD               0x90
#define BIT_ALIGN_MASK              0x07


#define DCS_CLASS1_ME_SPECIFIC      0x11
#define DCS_CODING_GROUP_MASK       0xF0
#define DCS_DATA_CODING_MSG_CLASS   0xF0
#define DCS_AUTO_DELETE_COMP_CODE   0x70
#define DCS_AUTO_DELETE_UCOMP_CODE  0x50
#define DCS_GEN_COMP_CODE           0x30
#define DCS_GEN_COMP_CODE_NO_CLASS  0x20
#define DCS_GEN_UCOMP_CODE          0x10
#define DCS_GEN_UCOMP_CODE_NO_CLASS 0x00
#define DCS_MWI_STORE_16BIT_MASK    0xE0

#define FIELD_ADDRESS_MASK          0x90

#define MAX_PI                      10    /** Note well NOT defined in GSM 03.40 **/
#define MIN_FIELD_MASK              0x0F

#define SINGLE_BIT                  1
#define SINGLE_OCTET                1

#define TP_DT_SIZE                  7
#define TP_SCTS_SIZE                7
#define TP_MR_SIZE                  1
#define TP_ST_SIZE                  1
#define TP_VP_ENHAN_ABS_FIELD_SIZE  7
#define TP_VPF_FORMAT_MASK          0x18

#define TPDU_OCTET_FIELD            0x80
#define TPDU_PI_DCS_BIT             0x02
#define TPDU_PI_PID_BIT             0x01
#define TPDU_PI_UDL_BIT             0x04
#define TPDU_UDHI_BIT               0x40

#define TWO_BITS                    2

#define TPDU_MAX_CD_FIELD_SIZE      0x9C
#define TPDU_PI_EXTENSION_BIT       0x80
#define GSM_COMPRESSED_FORMAT       0x20
#define MAX_ADDRESS_SEMI_OCTETS     20
#define UPDATE_PROHIBITED_BIT       0x40
#define DERIVED_FIELDS_BIT          0x20
#define NOT_SPECIAL_MSG             FALSE
#define IEI_SPECIAL_MSG             0x01
#define SPECIAL_MSG_TYPE_MASK       0x7F
#define SPECIAL_MSG_STORE_BIT       0x80
#define PCN_NOT_VOICE_MAIL          FALSE
#define PCN_VOICE_MAIL              TRUE
#define PCN_VMWI_INDICATOR_MASK     0x01
#define PCN_SPACE_IN_SHORT_MESSAGE  0x20
#define DCS_CODING_GROUP_WAITING    0xF0
#define DCS_MWI_WAIT_DISCARD        0xC0
#define DCS_MWI_ACTIVE_BIT          0x08
#define DCS_MSG_WAIT_COMPRESSED     0xD0
#define DCS_MSG_WAIT_UNCOMPRESSED   0xE0
#define DCS_NO_MESSAGE_WAITING      FALSE
#define DCS_VOICE_MAIL_WAITING      0x00
#define DCS_FAX_WAITING             0x01
#define DCS_EMAIL_WAITING           0x02
#define DCS_VIDEO_MAIL_WAITING      0x07
#define PCN_VMWI_ADDRESS_LENGTH     0x04
#define PCN_VMWI_TYPE_OF_ADDRESS    0xD0
#define PCN_VMWI_ADDRESS_1          0x10
#define PCN_VMWI_ADDRESS_2          0x00
#define PCN_VMWI_ADDRESS_1_MASK     0x7E
#define PCN_VMWI_ADDRESS_2_MASK     0x3F
#define TP_PID_ME_DATA_DOWNLOAD     0x7D
#define TP_PID_ME_DEPERSONALIZE_SM  0x7E
#define TP_PID_REPLACE_SM_TYPE_1    0x41
#define TP_PID_REPLACE_SM_TYPE_7    0x47
#define TP_PID_SIM_DATA_DOWNLOAD    0x7F
#define DCS_CODING_CLASS_MASK       0x03
#define CONCAT_8BIT_REF             0x00
#define CONCAT_16BIT_REF            0x08
#define APP_PORT_8_BIT_ADDR         0x04
#define APP_PORT_16_BIT_ADDR        0x05
#define APP_PORT_8_BIT_ADDR_LENGTH  0x02
#define APP_PORT_16_BIT_ADDR_LENGTH 0x04


/*************************************************************************************
*
* Section 2.2 Field Characteristics Structure and Definitions
*
* The characteristics and hence field dependent processing are contained within the
* structure FieldType[ ] and consists of three bytes per field. The field description
* is in either bit or octet format as follows:
*
*  Bit format:  byte 0: bit  7    field designator  // 0 = bit format
*           bit  6    update disable    // 1 = update prohibited
*           bits 5-3  not used
*           bits 0-2  bit alignment   // ie. number of shifts needed
*
*       byte 1: bits 0-7  aligned bit mask
*
*  Octet format:byte 0: bit 7   field designator  // 1 = octet format
*           bit 6   update prohibit   // 1 = derived field
*           bit 5   dependent field   // 1 = dependent fields
*           bit 4   address field   // 1 = address field
*           bits 0-3  minimum field value
*
*       byte 1: bits 0-7  maximum field value
*
*  Both formats:byte 2: bit 7   1 = field valid for SM_DELIVER_REPORT_ACK message
*           bit 6   1 = field valid for SM_COMMAND message
*           bit 5   1 = field valid for SM_SUBMIT message
*           bit 4   1 = field valid for SM_DELIVER_REPORT_ERROR message
*           bit 3   1 = field valid for SM_SUBMIT_REPORT_ACK message
*           bit 2   1 = field valid for SM_STATUS_REPORT message
*           bit 1   1 = field valid for SM_SUBMIT_REPORT_ERROR message
*           bit 0   1 = field valid for SM_DELIVER message
*
**************************************************************************************/

struct fieldCharacteristics_t {
    uint8_t a;
    uint8_t b;
    uint8_t c;
};

#define addressField  0x90
#define octetField    0x80
#define octetFieldDep 0xA0
#define bitField      0x00
#define bitFieldDep   0x20
#define bitFieldPro   0x40
#define octetFieldPro 0xC0
#define octetFieldDP  0xE0

const struct fieldCharacteristics_t FieldType[] = {
//  type        minLen. maxLen. validity   mnemonic  <- for none bit fields
//  type        align.  mask    validity   mnemonic  <- for bit fields
    {octetFieldDep + 0, 157, 0x40},     //  TP_CD
    {octetFieldPro + 1, 1, 0x40},       //  TP_CDL
    {octetField + 1, 1, 0x40},  //  TP_CT
    {addressField + 2, 12, 0x60},       //  TP_DA
    {octetFieldDep + 0, 1, 0xBF},       //  TP_DCS
    {octetField + 7, 7, 0x04},  //  TP_DT
    {octetField + 1, 1, 0x12},  //  TP_FCS
    {bitField + 2, 0x04, 0x05}, //  TP_MMS
    {octetField + 1, 1, 0x40},  //  TP_MN
    {octetField + 1, 1, 0x64},  //  TP_MR
    {bitFieldPro + 0, 0x03, 0xFF},      //  TP_MTI
    {addressField + 2, 12, 0x01},       //  TP_OA
    {octetFieldPro + 1, MAX_PI, 0x9E},  //  TP_PI
    {octetFieldDep + 0, 1, 0xFF},       //  TP_PID
    {addressField + 2, 12, 0x04},       //  TP_RA
    {bitField + 2, 0x04, 0x20}, //  TP_RD
    {bitField + 7, 0x80, 0x21}, //  TP_RP
    {octetField + 7, 7, 0x0F},  //  TP_SCTS
    {bitField + 5, 0x20, 0x01}, //  TP_SRI
    {bitField + 5, 0x20, 0x04}, //  TP_SRQ
    {bitField + 5, 0x20, 0x60}, //  TP_SRR
    {octetField + 1, 1, 0x04},  //  TP_ST
    {octetFieldDep + 0, 1, 0x20},       //  TP_VP
    {bitFieldDep + 3, 0x18, 0x20},      //  TP_VPF
    {octetFieldDep + 0, 143, 0xBF},     //  TP_UD
    {octetFieldPro + 0, 1, 0xBF},       //  TP_UDL
    {bitFieldPro + 6, 0x40, 0xFF},      //  TP_UDHI
    {octetFieldDep + 0, 140, 0xFF},     //  TP_UDH
    {octetFieldPro + 0, 1, 0xFF}        //  TP_UDHL
};

// The TP_UD field has a maximum value which is dependent upon the thisParse->type
static const uint8_t maxOctetsUD[] = { 140, 151, 143, 152, 158, 140, 156, 159 };
static const uint8_t BitMask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

static const uint8_t DCSClassDecode[] = {
    SM_CLASS_0,
    SM_CLASS_1_ME_SPECIFIC,
    SM_CLASS_2_SIM_SPECIFIC,
    SM_CLASS_3_TE_SPECIFIC
};


/*************************************************************************************
**
**  Section 2.3 Macro Definitions
**
*************************************************************************************/

// these sections are purely for readability
#define fieldType         FieldType[thisField].a
#define fieldMinLength(A) (FieldType[(A)].a & MIN_FIELD_MASK)
#define fieldMaxLength(A) FieldType[(A)].b
#define bitMask(A)        FieldType[(A)].b

#define thisDCS       thisTPDU[thisParse->index[TP_DCS]]
#define thisPI        thisTPDU[thisParse->index[TP_PI]]
#define thisPID       thisTPDU[thisParse->index[TP_PID]]
#define thisUDL       thisTPDU[thisParse->index[TP_UDL]]
#define thisUDHL      thisTPDU[thisParse->index[TP_UDHL]]


#define PACK_OPTIONAL_PARAMETERS                    \
    if (PID_p)  { *PI_p |= TPDU_PI_PID_BIT;         \
            thisTPDU[sizeTPDU++] = *PID_p;  }       \
    if (DCS_p)  { *PI_p |= TPDU_PI_DCS_BIT;         \
            thisTPDU[sizeTPDU++] = *DCS_p;  }

#define ROUND_UP_TO_SEPTETS(A)   (A) = (uint8_t)((8*(uint16_t)(A)+6) / 7);

#define ROUND_DOWN_TO_SEPTETS(A) (A) = (uint8_t)((8*(uint16_t)(A)) / 7);

#define USER_HEADER     (thisTPDU[0] & TPDU_UDHI_BIT)

#define OCTET_FIELD(A)    (FieldType[(A)].a & TPDU_OCTET_FIELD)

#define BIT_FIELD(A)    (!(FieldType[(A)].a & TPDU_OCTET_FIELD))

#define MAX_UD_SEPTETS(A) (uint8_t)(((uint16_t)(maxOctetsUD[(A)]) * 8) / 7)

#define NO_TEXT       (!thisUDL || ((USER_HEADER) && (thisUDL == (thisUDHL+1))))

#define PARSE_OK            0x00
#define PARSE_UDL_ERR       0x01
#define PARSE_UDHL_ERR      0x02
#define PARSE_UDH_ERR       0x04
#define PARSE_ANO_ERR       0x08

/*************************************************************************************
**
**  Section 2.4 Data Definitions
**
*************************************************************************************/

typedef struct parse_t *parse_p;

struct parse_t {
    uint8_t index[TPDU_MAX_FIELDS];     // Offset index for specific field
    uint8_t size[TPDU_MAX_FIELDS];      // Field size in octets addressed by index
    uint8_t idx;                // Accessed as "runningIndex" by #define
    uint8_t length;             // Accessed as "MessageLength" by #define
    uint8_t alphabet;           // Accessed as "thisParse->alphabet" by #define
    ShortMessageType_t type;    // Accessed as "thisParse->type" by #define
    uint8_t parseStatus;        // accessed as "thisParse->parseStatus" by #define
};

typedef uint8_t(
    *MWIHandlerFunction_p) (
    const TPDU_p thisTPDU,
    MWIList_p thisMsg,
    parse_p thisParse);

typedef const struct {
    MWIHandlerFunction_p firstMWIFunction;
    MWIHandlerFunction_p secondMWIFunction;
    MWIHandlerFunction_p thirdMWIFunction;
} orderedMWIFunction_t;

/************************************************************************************
**
**  Section 2.5 Error Definitions. within this software there are four error levels
**        with the following significance:
**
**        - Catastrophic errors are those errors that could cause an MT to crash
**        - Fatal errors are those errors that prevent the requested operation
**        - Normal errors are errors that may prevent the requested operation
**        - Warnings are errors that are often recoverable
**
*************************************************************************************/

#define   SYSTEM_ERROR        0xC0
#define   FATAL_ERROR         0x80
#define   NORMAL_ERROR        0x40
#define   WARNING_ERROR       0x01
#define   NO_ERROR            0

enum {
    // Warning errors
    EXCESS_CD_FIELD_TRUNCATED = 1,      // section 4.1.8
    GSM_COMPRESSED_MSG,         // section 4.2.3
    SF_FIELD_TOO_LONG,          // section 6.1
    SF_INVALID_ADDRESS_LENGTH,  // section 6.1
    SF_INVALID_ADDRESS_TYPE,    // section 6.1
    TRUNCATED_CD_UD_FIELD,      // section 6.1
    USER_DATA_TRUNCATION,       // section 4.2.3
    ODD_UNICODE_TEXT_LENGTH,    // section 8.

    // Normal errors
    ADDRESS_FIELD_TOO_LONG = 0x40,      // section 4.2.6
    ADDRESS_TYPE_MS_BIT_NOT_SET,        // section 4.2.6
    OVERSIZE_TP_PI_FIELD,       // section 4.2.1
    UNKNOWN_MESSAGE_TYPE,       // section 4.1
    UNKNOWN_SDF_FIELD_TYPE,     // section 6.4
    NONE_BIT_FIELD_GET,         // section 12.1
    NONE_BIT_FIELD_TEST,        // section 12.3
    NONE_BIT_FIELD_UPDATE,      // section 12.2
    WRONG_APP_PORT_ADDR_IE_LENGTH,      // section 17.1

    // Fatal errors
    FIELD_INVALID_FOR_MSG_TYPE = 0x80,  // section 5.4
    FIELD_PARSE_ERROR,          // section 5.4
    PROHIBITED_FIELD_UPDATE,    // section 6.1
    SF_INVALID_FIELD,           // section 6.1
    SF_FIELD_TOO_SHORT,         // section 6.1
    UNKNOWN_GET_DATA_TYPE,      // section 11.
    GET_FIELD_PARSE_FAILURE,    // section 5.2
    GET_FIELD_PTR_PARSE_FAILURE,        // section 5.1
    GET_FIELD_XFR_PARSE_FAILURE,        // section 5.3
    NULL_PKTEXT_TEXT_PTR,       // section 8.

    // Catastrophic (system) errors
    GET_FIELD_NULL_DEST_PTR = 0xC0      // section 5.4
};



/*************************************************************************************
**
**  Section 3. Local Function Prototypes
**
*************************************************************************************/
static uint8_t checkNewField(
    fieldName_t thisField,
    uint8_t * newField_p,
    uint8_t * newFieldSize_p,
    parse_p thisParse);

static uint8_t extractAlphabetFromDCS(
    uint8_t tpDCS);
static uint8_t extractMessageClass(
    uint8_t tpDCS);
static uint8_t extractText(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    uint8_t * dest_p,
    uint8_t textLength);

static uint8_t fieldValid(
    fieldName_t field,
    ShortMessageType_t msgType);

static uint8_t findApplicationPorts(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    uint16_t * destPort_p,
    uint16_t * origPort_p);
static uint8_t *findUserDataIE(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    uint8_t typeIE,
    uint8_t thisInstance);

static uint8_t getVoiceMailInfo_DCS(
    const TPDU_p thisTPDU,
    MWIList_p thisMsg,
    parse_p thisParse);
static uint8_t getSpecialMessageInfo(
    const TPDU_p thisTPDU,
    MWIList_p thisMsg,
    parse_p thisParse);
static uint8_t getVoiceMailInfo_PCN(
    const TPDU_p thisTPDU,
    MWIList_p thisMsg,
    parse_p thisParse);
static uint8_t getVPFieldSize(
    const TPDU_p thisTPDU);

static uint8_t isOrangeVoiceMail(
    uint8_t * thisOPA_p);

static void memcpyUNICODE(
    uint8_t * out_p,
    uint8_t * in_p,
    uint8_t size);
static MWI_Type_t msgWaitingType(
    uint8_t type,
    uint8_t * waiting);

static void packField(
    uint8_t * newField,
    TPDU_p thisTPDU,
    uint8_t * index,
    uint8_t sizeNewField);
static void packSeptets(
    uint8_t * out_p,
    uint8_t * text_p,
    uint8_t * endByte,
    uint8_t shift);

static void parse_SM_COMMAND(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parse_SM_DELIVER(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parse_SM_DELIVER_REPORT_ACK(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parse_SM_DELIVER_REPORT_ERROR(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parse_SM_STATUS_REPORT(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parse_SM_SUBMIT(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parse_SM_SUBMIT_REPORT_ACK(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parseAddressField(
    const TPDU_p thisTPDU,
    uint8_t thisAddress,
    parse_p thisParse);

static TPDU_p parseField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    parse_p thisParse);
static void parsePID_DCSFields(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parsePIField(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parse_SM_SUBMIT_REPORT_ERROR(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parseTPDU(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    parse_p thisParse);
static void parseUDFields(
    const TPDU_p thisTPDU,
    parse_p thisParse);
static void parseUDHFields(
    const TPDU_p thisTPDU,
    parse_p thisParse);

static uint8_t processField(
    fieldName_t thisField,
    uint8_t * thisData_p);

static void replaceBitField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    uint8_t replacementBits,
    parse_p thisParse);
static void replaceOctetField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    uint8_t * newField_p,
    uint8_t newFieldSize,
    parse_p thisParse);
static void resetParseData(
    parse_p thisParse);

static void setAlphabet(
    TPDU_p thisTPDU,
    parse_p thisParse,
    uint8_t newAlphabet);

static void setDerivedFields(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    fieldName_t thisField);

static void unpackSeptets(
    uint8_t * out_p,
    uint8_t * text_p,
    uint8_t textLength);

/* Section 3.1 -  MWI Order definition array */

static orderedMWIFunction_t definedMWIOrder[] = {
    {getSpecialMessageInfo, getVoiceMailInfo_DCS, getVoiceMailInfo_PCN},
    {getSpecialMessageInfo, getVoiceMailInfo_PCN, getVoiceMailInfo_DCS},

    {getVoiceMailInfo_DCS, getSpecialMessageInfo, getVoiceMailInfo_PCN},
    {getVoiceMailInfo_DCS, getVoiceMailInfo_PCN, getSpecialMessageInfo},

    {getVoiceMailInfo_PCN, getSpecialMessageInfo, getVoiceMailInfo_DCS},
    {getVoiceMailInfo_PCN, getVoiceMailInfo_DCS, getSpecialMessageInfo}
};

/*************************************************************************************
**
**  Section 4.0 Parsing Status Functions
**
**  Local functions for logging parse error
**
**************************************************************************************/


/********************************************************************/
/**
 *
 * @function     setParseStatusUDLengthError
 *
 * @description  Set the parse status of UD Length error
 *
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void setParseStatusUDLengthError(
    parse_p thisParse)
{
    thisParse->parseStatus |= PARSE_UDL_ERR;
    SMS_A_(SMS_LOG_E("smtpdu.c: UDL error"));
}


/********************************************************************/
/**
 *
 * @function     setParseStatusUDHeaderLengthError
 *
 * @description  Set the parse status of UDHeader Length error
 *
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void setParseStatusUDHeaderLengthError(
    parse_p thisParse)
{
    thisParse->parseStatus |= PARSE_UDHL_ERR;
    SMS_A_(SMS_LOG_E("smtpdu.c: UDHL error"));
}


/********************************************************************/
/**
 *
 * @function     setParseStatusUDHeaderError
 *
 * @description  Set the parse status of UDHeader error
 *
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void setParseStatusUDHeaderError(
    parse_p thisParse)
{
    thisParse->parseStatus |= PARSE_UDH_ERR;
    SMS_A_(SMS_LOG_E("smtpdu.c: UDH error"));
}

/*************************************************************************************
**
**  Section 4.1 Parsing Function
**
**  This function takes TPDU parameters and sets the parsing structure accordingly.
**  This structure contains an index and a size for each message field. Additional
**  structure information is the MessageLength, thisParse->type thisParse->alphabet, thisParse->parseStatus
**  and the thisParse->idx pointer. This extra information reduces the number of transfer
**  parameters and generally simplifies the housekeeping.
**
**  The message type is a function of the TPDU MTI field and the message direction as
**  follows:
**          000 = SM_DELIVER
**          001 = SM_SUBMIT_REPORT_ERROR
**          010 = SM_STATUS_REPORT
**          011 = SM_SUBMIT_REPORT_ACK
**          100 = SM_DELIVER_REPORT_ERROR
**          101 = SM_SUBMIT
**          110 = SM_COMMAND
**          111 = SM_DELIVER_REPORT_ACK
**
**  The fields which are common to all message type are parsed at this level i.e. the
**  TP_UDHI and the TP_MTI fields.
**
**  Note the parse size array is set when fields are actually present. The parse index
**  array gives a particular field's relative location should that field be present, it
**  is not an indication of a field's presence or abscence.
**
**************************************************************************************/

typedef void (
    *ParseSubFunction_p) (
    const TPDU_p,
    parse_p);

const ParseSubFunction_p sParse[] = {
    parse_SM_DELIVER,
    parse_SM_SUBMIT_REPORT_ERROR,
    parse_SM_STATUS_REPORT,
    parse_SM_SUBMIT_REPORT_ACK,
    parse_SM_DELIVER_REPORT_ERROR,
    parse_SM_SUBMIT,
    parse_SM_COMMAND,
    parse_SM_DELIVER_REPORT_ACK
};


/********************************************************************/
/**
 *
 * @function     parseTPDU
 *
 * @description  Function to parse TPDU data
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        typeTPDU :  as type of ShortMessageType_t.
 * @param        thisParse : as type of parse_p.
 *
 * @return       void
 */
/********************************************************************/
static void parseTPDU(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    parse_p thisParse)
{
    resetParseData(thisParse);
    thisParse->type = typeTPDU;

    if (thisParse->type > (SM_TYPE_LAST - 1)) {
        if (thisTPDU[0] & MTI_MASK) {
            thisParse->type = SM_SUBMIT;
        } else {
            thisParse->type = SM_DELIVER;
        }
    }

    thisParse->size[TP_MTI] = TWO_BITS; // Mandatory field
    thisParse->size[TP_UDHI] = SINGLE_BIT;      // Optional field initial setting

    sParse[thisParse->type] (thisTPDU, thisParse);      // Parse specific message type
}


/*************************************************************************************
*
* Section 4.1.1 SM_DELIVER Header Parsing. Note TP_SRI field is optional but is always
*       parsed as present.
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_DELIVER
 *
 * @description  Function to parse the short message of delivery
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_DELIVER(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{

    // Set the parse data for the bit fields (index already zero)
    thisParse->size[TP_MMS] = SINGLE_BIT;
    thisParse->size[TP_RP] = SINGLE_BIT;
    thisParse->size[TP_SRI] = SINGLE_BIT;       // Optional but always parsed as present

    // Now parse the remainder
    parseAddressField(thisTPDU, TP_OA, thisParse);

    thisParse->index[TP_PID] = thisParse->idx++;
    thisParse->size[TP_PID] = SINGLE_OCTET;

    thisParse->index[TP_DCS] = thisParse->idx++;
    thisParse->size[TP_DCS] = SINGLE_OCTET;

    thisParse->index[TP_SCTS] = thisParse->idx;
    thisParse->size[TP_SCTS] = TP_SCTS_SIZE;
    thisParse->idx += TP_SCTS_SIZE;

    thisParse->size[TP_UDL] = SINGLE_OCTET;

    parseUDFields(thisTPDU, thisParse);
}


/*************************************************************************************
*
* Section 4.1.2 SM_DELIVER_REPORT(RP_ERROR) TPDU Parsing
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_DELIVER_REPORT_ERROR
 *
 * @description  Function to parse the delivery report error of short message
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_DELIVER_REPORT_ERROR(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    thisParse->size[TP_FCS] = SINGLE_OCTET;
    thisParse->index[TP_FCS] = thisParse->idx++;

    parsePIField(thisTPDU, thisParse);

    parsePID_DCSFields(thisTPDU, thisParse);
    parseUDFields(thisTPDU, thisParse);

    // adjust thisParse->length if optional TP_UDL not present
    if (!(thisPI & TPDU_PI_UDL_BIT)) {
        thisParse->length--;
    }
}


/*************************************************************************************
*
* Section 4.1.3 SM_DELIVER_REPORT(RP_ACK) TPDU Parsing.
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_DELIVER_REPORT_ACK
 *
 * @description  Function to parse the delivery report acknowledgement of short message
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_DELIVER_REPORT_ACK(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    parsePIField(thisTPDU, thisParse);

    parsePID_DCSFields(thisTPDU, thisParse);
    parseUDFields(thisTPDU, thisParse);

    // adjust thisParse->length if optional TP_UDL not present
    if (!(thisPI & TPDU_PI_UDL_BIT)) {
        thisParse->length--;
    }
}


/*************************************************************************************
*
* Section 4.1.4 SM_SUBMIT TPDU Parsing. Note the TP_SRR bit field is optional and is
*       alaways parsed as present.
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_SUBMIT
 *
 * @description  Function to parse a short message of SUBMIT
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_SUBMIT(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    // Set the field sizes for the bit fields (index already zero)
    thisParse->size[TP_RP] = SINGLE_BIT;
    thisParse->size[TP_RD] = SINGLE_BIT;
    thisParse->size[TP_VPF] = TWO_BITS;
    thisParse->size[TP_SRR] = SINGLE_BIT;       // optional bit always parsed as present

    // Now parse the rest of the fields
    thisParse->index[TP_MR] = thisParse->idx++;
    thisParse->size[TP_MR] = SINGLE_OCTET;

    parseAddressField(thisTPDU, TP_DA, thisParse);

    thisParse->index[TP_PID] = thisParse->idx++;
    thisParse->size[TP_PID] = SINGLE_OCTET;

    thisParse->index[TP_DCS] = thisParse->idx++;
    thisParse->size[TP_DCS] = SINGLE_OCTET;

    thisParse->index[TP_VP] = thisParse->idx;
    thisParse->size[TP_VP] = getVPFieldSize(thisTPDU);
    thisParse->idx += thisParse->size[TP_VP];

    thisParse->size[TP_UDL] = SINGLE_OCTET;

    parseUDFields(thisTPDU, thisParse);
}


/*************************************************************************************
*
* Section 4.1.5 SM_SUBMIT_REPORT(RP_ERROR) TPDU Parsing.
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_SUBMIT_REPORT_ERROR
 *
 * @description  Function to parse the submit report error of a sort message
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_SUBMIT_REPORT_ERROR(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    thisParse->index[TP_FCS] = thisParse->idx++;
    thisParse->size[TP_FCS] = SINGLE_OCTET;

    parsePIField(thisTPDU, thisParse);

    thisParse->index[TP_SCTS] = thisParse->idx;
    thisParse->size[TP_SCTS] = TP_SCTS_SIZE;
    thisParse->idx += TP_SCTS_SIZE;

    parsePID_DCSFields(thisTPDU, thisParse);
    parseUDFields(thisTPDU, thisParse);

    // adjust thisParse->length if optional TP_UDL not present
    if (!(thisPI & TPDU_PI_UDL_BIT)) {
        thisParse->length--;
    }
}


/*************************************************************************************
*
* Section 4.1.6 SM_SUBMIT_REPORT(RP_ACK) TPDU Parsing.
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_SUBMIT_REPORT_ACK
 *
 * @description  Function to parse the submit report acknowledgement of short message
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_SUBMIT_REPORT_ACK(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    parsePIField(thisTPDU, thisParse);

    thisParse->index[TP_SCTS] = thisParse->idx;
    thisParse->size[TP_SCTS] = TP_SCTS_SIZE;
    thisParse->idx += TP_SCTS_SIZE;

    parsePID_DCSFields(thisTPDU, thisParse);
    parseUDFields(thisTPDU, thisParse);

    // adjust thisParse->length if optional TP_UDL not present
    if (!(thisPI & TPDU_PI_UDL_BIT)) {
        thisParse->length--;
    }
}


/*************************************************************************************
*
* Section 4.1.7 SM_STATUS_REPORT TPDU Parsing.
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_STATUS_REPORT
 *
 * @description  Function to parse the short message status report
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_STATUS_REPORT(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    // Set the parse data for the bit fields
    thisParse->size[TP_MMS] = SINGLE_BIT;
    thisParse->size[TP_SRQ] = SINGLE_BIT;

    // Now parse the remainder
    thisParse->index[TP_MR] = thisParse->idx++;
    thisParse->size[TP_MR] = SINGLE_OCTET;

    parseAddressField(thisTPDU, TP_RA, thisParse);

    thisParse->index[TP_SCTS] = thisParse->idx;
    thisParse->size[TP_SCTS] = TP_SCTS_SIZE;
    thisParse->idx += TP_SCTS_SIZE;

    thisParse->index[TP_DT] = thisParse->idx;
    thisParse->size[TP_DT] = TP_DT_SIZE;
    thisParse->idx += TP_DT_SIZE;

    thisParse->index[TP_ST] = thisParse->idx++;
    thisParse->size[TP_ST] = SINGLE_OCTET;

    parsePIField(thisTPDU, thisParse);
    parsePID_DCSFields(thisTPDU, thisParse);
    parseUDFields(thisTPDU, thisParse);

    // adjust thisParse->length if optional TP_UDL not present and again if TP_PI is zero
    if (!(thisPI & TPDU_PI_UDL_BIT)) {
        thisParse->length--;
        if (!thisPI) {
            thisParse->length--;
            thisParse->size[TP_PI] = 0;
        }
    }
}


/*************************************************************************************
*
* Section 4.1.8 SM_COMMAND TPDU Parsing. Note the TP_SRR bit field is always parsed as
*       present.
*
*************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parse_SM_COMMAND
 *
 * @description  Function to parse the short message command
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parse_SM_COMMAND(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    // optional bit fields always parsed as present
    thisParse->size[TP_SRR] = SINGLE_BIT;

    // Now parse the rest of the fields
    thisParse->index[TP_MR] = thisParse->idx++;
    thisParse->size[TP_MR] = SINGLE_OCTET;

    thisParse->index[TP_PID] = thisParse->idx++;
    thisParse->size[TP_PID] = SINGLE_OCTET;

    thisParse->index[TP_CT] = thisParse->idx++;
    thisParse->size[TP_CT] = SINGLE_OCTET;

    thisParse->index[TP_MN] = thisParse->idx++;
    thisParse->size[TP_MN] = SINGLE_OCTET;

    parseAddressField(thisTPDU, TP_DA, thisParse);

    if (thisTPDU[thisParse->idx] > TPDU_MAX_CD_FIELD_SIZE) {
        thisTPDU[thisParse->idx] = TPDU_MAX_CD_FIELD_SIZE;
    }

    thisParse->index[TP_CDL] = thisParse->idx;
    thisParse->index[TP_UDL] = thisParse->idx;
    thisParse->size[TP_CDL] = SINGLE_OCTET;

    thisParse->index[TP_CD] = thisParse->idx + 1;
    thisParse->index[TP_UD] = thisParse->idx + 1;
    thisParse->size[TP_CD] = thisTPDU[thisParse->idx];

    thisParse->index[TP_UDHL] = thisParse->idx + 1;
    thisParse->index[TP_UDH] = thisParse->idx + 2;

    // parse the TP_UDHL and TP_UDH fields but only if the TP_UDHI bit is set
    if (USER_HEADER) {
        thisParse->size[TP_UDHL] = SINGLE_OCTET;
        thisParse->size[TP_UDH] = thisUDHL;
        parseUDHFields(thisTPDU, thisParse);
    }

    thisParse->idx += thisTPDU[thisParse->idx] + 1;

    thisParse->alphabet = ALPHABET_8_BIT;
    thisParse->length = thisParse->idx;
}


/************************************************************************************
*
* Section 4.2.1 TPDU TP_PI field parsing function. This function sets the PI index
*       and the size of the PI field. The maximum field is undefined in
*       GSM 03.40 but in this function it is determined by the constant MAX_PI.
*       If the max. field length is exceeded an error is returned.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parsePIField
 *
 * @description  Function to parse Parameter Indicator (PI) field
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parsePIField(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    uint8_t sizeTP_PI;

    sizeTP_PI = SINGLE_OCTET;   // always at least one octet in field

    thisParse->index[TP_PI] = thisParse->idx;

    // increment the PI field size for each extension bit found to a maximum of 255
    while ((thisTPDU[thisParse->idx] & TPDU_PI_EXTENSION_BIT) && (sizeTP_PI < 255)) {
        sizeTP_PI++;
        thisParse->idx++;
    }

    thisParse->idx++;

    if (sizeTP_PI > MAX_PI) {
        sizeTP_PI = MAX_PI;
    }

    thisParse->size[TP_PI] = sizeTP_PI;
}


/*************************************************************************************
*
* Section 4.2.2 Set the optional fields parse information. This function decodes the
*       Parameter Indicator (TP_PI) field and parses the TP_PID and TP_DCS
*       fields accordingly.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parsePID_DCSFields
 *
 * @description  Function to decode the Parameter Indicator field and parse PID &
 *               Data Coding Scheme (DCS) fields
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parsePID_DCSFields(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    thisParse->index[TP_PID] = thisParse->idx;
    if (thisPI & TPDU_PI_PID_BIT) {
        thisParse->idx++;
        thisParse->size[TP_PID] = SINGLE_OCTET;
    }

    thisParse->index[TP_DCS] = thisParse->idx;
    if (thisPI & TPDU_PI_DCS_BIT) {
        thisParse->idx++;
        thisParse->size[TP_DCS] = SINGLE_OCTET;
    }

    if (thisPI & TPDU_PI_UDL_BIT)
        thisParse->size[TP_UDL] = SINGLE_OCTET;
}


/************************************************************************************
*
* Section 4.2.3 TPDU User Data Field. This function parses the user data fields
*       For correct operation "thisParse->idx" must point to the User Data Length
*       field. Excessive field lengths return an error and the field length
*       is set to 0.
*               Note that the parsed field sizes are always in octets irrespective
*               of the thisParse->alphabet.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parseUDFields
 *
 * @description  Function to parse the user data fields
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parseUDFields(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    uint8_t userDataOctets = 0;

    // Set thisParse->alphabet if TP_DCS present otherwise defaults to APHABET_7_BIT
    if (thisParse->size[TP_DCS]) {
        thisParse->alphabet = extractAlphabetFromDCS(thisDCS);
    }

    thisParse->index[TP_UDL] = thisParse->idx++;
    thisParse->index[TP_UD] = thisParse->idx;
    thisParse->index[TP_UDHL] = thisParse->idx++;
    thisParse->index[TP_UDH] = thisParse->idx;

    // if there is a valid TP_UDL then read it otherwise default to zero
    if (thisParse->size[TP_PI] && (thisPI & TPDU_PI_UDL_BIT)) {
        thisParse->size[TP_UDL] = SINGLE_OCTET;
    }

    if (thisParse->size[TP_UDL]) {
        // parse the TP_UDHL and TP_UDH fields but only if the TP_UDHI bit is set
        if (USER_HEADER) {
            thisParse->size[TP_UDHL] = SINGLE_OCTET;
            thisParse->size[TP_UDH] = thisUDHL;
        }
        // calculate the number of octets in the user data
        if (thisParse->alphabet == ALPHABET_7_BIT) {
            userDataOctets = thisUDL - (thisUDL / 8);
        } else                  // 8/16 bit alphabet so UDL is number of octets  3GPP 23.040 Section 9.2.3.16
        {
            userDataOctets = thisUDL;
        }

        // check that user data length is within limits and truncate if necessary
        if (userDataOctets > maxOctetsUD[thisParse->type]) {
            uint8_t index;

            setParseStatusUDLengthError(thisParse);

            userDataOctets = maxOctetsUD[thisParse->type];
            index = thisParse->idx - 1;

            if (thisParse->alphabet == ALPHABET_7_BIT) {
                thisTPDU[index] = MAX_UD_SEPTETS(thisParse->type);
            } else {
                thisTPDU[index] = maxOctetsUD[thisParse->type];
            }
        }

        thisParse->size[TP_UD] = userDataOctets;
    } else {
        thisParse->size[TP_UDHI] = 0;
    }

    thisParse->length = thisParse->index[TP_UDL] + userDataOctets + 1;
}


/************************************************************************************
*
* Section 4.2.3aTPDU User Data Header Field. This function parses the user data header fields
*       For correct operation "thisParse->idx" must point to the User Data Header Length
*       field. Excessive field lengths return an error and the field length
*       is set to 0.
*               Note that the parsed field sizes are always in octets irrespective
*               of the thisParse->alphabet.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parseUDHFields
 *
 * @description  Function to parse user data header fields
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parseUDHFields(
    const TPDU_p thisTPDU,
    parse_p thisParse)
{
    uint8_t UDH_pos = thisParse->index[TP_UDH];
    uint8_t UDHL_pos = thisParse->index[TP_UDHL];
    uint8_t *UDH_p = &thisTPDU[UDH_pos];
    uint8_t *UDHL_p = &thisTPDU[UDHL_pos];
    uint8_t *endUDH_p = UDH_p + thisUDHL - 1;
    uint8_t thisIEISize = 0;

    // the UDH information is corrupt in some way - we didn't end where we should have, so ignore
    if (*UDHL_p > maxOctetsUD[thisParse->type]) {
        setParseStatusUDHeaderLengthError(thisParse);
    }
    // parse the whole UDH to validate it rather than exiting when required IEI is found
    while (UDH_p < endUDH_p) {
        thisIEISize = *(UDH_p + 1);     // get current IEI length
        UDH_p += thisIEISize + 2;       // start of next IEI block
    }

    // the UDH information is corrupt in some way - we didn't end where we should have, so ignore
    if (UDH_p != endUDH_p + 1) {
        setParseStatusUDHeaderError(thisParse);
    }
}

/************************************************************************************
*
* Section 4.2.4 TPDU Parsing Data Resetting.
*       all parse fields are set to zero. In terms of the initial values
*       the function returns the following:
*
*         thisParse->type   = 0 = SM_DELIVER
*         thisParse->alphabet = 0 = ALPHABET_7_BIT
*         thisParse->parseStatus      = 0 = PARSE_OK
*         thisParse->idx    = 1 = pointer to first none-bit field
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     resetParseData
 *
 * @description  Function to reset the parse data
 *
 * @param        thisParse : as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void resetParseData(
    parse_p thisParse)
{
    // Reset the parse structure, this ensures zero for unset fields.
    memset((uint8_t *) thisParse, 0, sizeof(struct parse_t));

    thisParse->idx = 1;
}

/************************************************************************************
*
* Section 4.2.5 This function returns the current size of the TP_VP field and
*       increments 'thisParse->idx' by the appropriate amount. No error
*       return is possible.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     getVPFieldSize
 *
 * @description  Function to get the current size of the TP_VP field
 *
 * @param        thisTPDU : as type of TPDU_p.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t getVPFieldSize(
    const TPDU_p thisTPDU)
{
    uint8_t fieldSize;

    switch (thisTPDU[0] & TP_VPF_FORMAT_MASK) {
    case TP_VPF_ABSOLUTE_FORMAT:
    case TP_VPF_ENHANCED_FORMAT:
        fieldSize = TP_VP_ENHAN_ABS_FIELD_SIZE;
        break;

    case TP_VPF_RELATIVE_FORMAT:
        fieldSize = SINGLE_OCTET;
        break;

    default:
        fieldSize = 0;
    }
    return fieldSize;
}


/************************************************************************************
*
* Section 4.2.6 This function parses an address field and increments 'thisParse->idx'
*       by the appropriate amount. An abnormal field size returns an error
*       and the address field size is set to the maximum/minimum allowed.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parseAddressField
 *
 * @description  Function to parse the address field
 *
 * @param        thisTPDU :    as type of TPDU_p.
 * @param        thisAddress : uint8_t.
 * @param        thisParse :   as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void parseAddressField(
    const TPDU_p thisTPDU,
    uint8_t thisAddress,
    parse_p thisParse)
{
    uint8_t semiOctets;
    uint8_t sizeOfAddressField;

    thisParse->index[thisAddress] = thisParse->idx;
    semiOctets = thisTPDU[thisParse->idx];

    if (semiOctets > MAX_ADDRESS_SEMI_OCTETS) {
        semiOctets = MAX_ADDRESS_SEMI_OCTETS;
    }

    sizeOfAddressField = (semiOctets + 5) / 2;  // overall size in octets
    thisParse->size[thisAddress] = sizeOfAddressField;
    thisParse->idx += sizeOfAddressField;
}


/*************************************************************************************
**
**  Section 5.1   TPDU Field Pointer Function
**
**  This function return a pointer to a TPDU data field octet. Attempts to read an
**  invalid field return a null pointer. Note that bit data is not normalised.
**
**  The field mnemonics are as given in GSM 03.40 and may be in abbreviated or full
**  form. The list of mnemonics is contained in the header module smtpdu.h section 1.
**
**************************************************************************************/


/********************************************************************/
/**
 *
 * @function     *TPDU_GetFieldPtr
 *
 * @description  Function to get the pointer to a TPDU data field octet
 *
 * @param        thisField : as type of fieldName_t.
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        typeTPDU :  as type of ShortMessageType_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t *TPDU_GetFieldPtr(
    fieldName_t thisField,
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU)
{
    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer
    uint8_t *ptr_p;

    ptr_p = (uint8_t *) parseField(thisField, thisTPDU, typeTPDU, thisParse);

    return ptr_p;
}


/*************************************************************************************
**
**  Section 5.2   TPDU Field Extraction Function
**
**  This function returns the first octet of a TPDU data field. Attempts to read an
**  invalid field return zero. Bit data is right adjusted and any excess bits are
**  masked out.
**
**  The field mnemonics are as given in GSM 03.40 and may be in abbreviated or full
**  form. The list of mnemonics is contained in the header module smtpdu.h section 1.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetField
 *
 * @description  Function to get the first octet of a TPDU data field
 *
 * @param        thisFieldName : as type of fieldName_t.
 * @param        thisTPDU :      as type of TPDU_p.
 * @param        typeTPDU :      as type of ShortMessageType_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_GetField(
    fieldName_t thisFieldName,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU)
{
    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    uint8_t *field_p;
    uint8_t field = 0;

    field_p = parseField(thisFieldName, thisTPDU, typeTPDU, thisParse);
    if (field_p) {
        field = processField(thisFieldName, field_p);
    }

    return field;
}


/*************************************************************************************
**
**  Section 5.3   TPDU Field Transfer Function
**
**   Attempts to read an invalid field return zero and
**  the successful extraction of a bit field always returns a one. Bit data is right
**  adjusted and any excess bits are masked out.
**
**  The field mnemonics are as given in GSM 03.40 and may be in abbreviated or full
**  form. The list of mnemonics is contained in the header module smtpdu.h section 1.
**
**  Note the calling function must ensure that the destination memory allocation is
**  adequate.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetFieldXfr
 *
 * @description  Function to get field Xfr: copy a TPDU data field to a designated
 *               address and returns the number of bytes transferred.
 *
 * @param        thisField: as type of fieldName_t.
 * @param        thisTPDU : as type of TPDU_p.
 * @param        typeTPDU : as type of ShortMessageType_t.
 * @param        *out_p   : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_GetFieldXfr(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t * out_p)
{
    uint8_t *source_p;
    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    source_p = parseField(thisField, thisTPDU, typeTPDU, thisParse);

    if (source_p) {
        if ((thisParse->size[thisField] == 1) || BIT_FIELD(thisField)) {
            // single octet check for bit field and align/mask as necessary
            *out_p = processField(thisField, source_p);
        }
        // multiple octet so transfer the approriate number of bytes
        else
            memcpy(out_p, source_p, thisParse->size[thisField]);
    }

    return thisParse->size[thisField];  // Number of bytes transferred
}


/*************************************************************************************
**
**  Section 5.4   TPDU Field Parse Function
**
**  This function parses TPDU data and returns a pointer to the designated field.
**  Attempts to read an invalid field return a null pointer.
**
**  The field mnemonics are as given in GSM 03.40 and may be in abbreviated or full
**  form. The list of mnemonics is contained in the header module smtpdu.h section 1.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     parseField
 *
 * @description  This function parses TPDU data and returns a pointer to the designated field
 *
 * @param        thisField : as type of fieldName_t.
 * @param        thisTPDU  : as type of TPDU_p.
 * @param        typeTPDU  : as type of ShortMessageType_t.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static TPDU_p
 */
/********************************************************************/
static TPDU_p parseField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    parse_p thisParse)
{

    parseTPDU(thisTPDU, typeTPDU, thisParse);   // Find all fields

    // check if the field is valid for this type of message
    if (!fieldValid(thisField, thisParse->type)) {
        return NULL;            // Invalid field for message type
    }
    // now check if has been set
    if (thisParse->size[thisField] == 0) {
        if (((thisField >= TP_UD) && (thisField <= TP_UDHL))
            || (thisField == TP_PI) || (thisField == TP_DCS) || (thisField == TP_PID)) {
            // the field is optional so return the default value of zero and a warning
            return &thisParse->size[thisField];
        } else {
            // mandatory field so flag a fatal error - something is very wrong
            return NULL;
        }
    }
    return (thisTPDU + thisParse->index[thisField]);

}


/*************************************************************************************
**
**  Section 5.5 Single Octet Processing Function. This function operated on single
**        octet fields returning zero for a null pointer. The type and details
**        of the processing are governed by the contents of FieldType in section
**        2.2  If the field is a bit field then the data is aligned and masked
**        prior to being returned.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     processField
 *
 * @description  This is a single  octet processing function, which operates on single
 *               octet fields returning zero for a null pointer
 *
 * @param        thisField :   as type of fieldName_t.
 * @param        *thisData_p : uint8_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t processField(
    fieldName_t thisField,
    uint8_t * thisData_p)
{
    uint8_t field;

    if (!thisData_p) {
        return 0;               // dont process null pointer
    }

    field = *thisData_p;

    if (BIT_FIELD(thisField))   // only process bit fields
    {
        // Bit field processing so mask the field and right adjust
        field &= bitMask(thisField);    // get rid of unwanted bits
        field >>= (fieldType & BIT_ALIGN_MASK); // and right adjust
    }

    return field;
}


/*************************************************************************************
**
**  Section 5.6 This function checks whether a field is valid for a specific message
**        type. If it is then TRUE is returned otherwise FALSE.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     fieldValid
 *
 * @description  Function to check if a field is valid for a specific message type
 *
 * @param        field :   as type of fieldName_t.
 * @param        msgType : as type of ShortMessageType_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t fieldValid(
    fieldName_t field,
    ShortMessageType_t msgType)
{
    if (FieldType[field].c & BitMask[msgType]) {
        return TRUE;
    } else {
        return FALSE;
    }
}


/*************************************************************************************
**
**  Section 6.  TPDU Field Setting Function
**
**  This function replaces the designated TPDU data field. Aborted attempts to update
**  a field return a zero otherwise the number of update bytes is returned.
**
**  Where necessary dependent fields are adjusted to conform with the new replacement.
**
**  The field mnemonics are as given in GSM 03.40 and may be in abbreviated or full
**  form. The list of mnemonics is contained in the header module smtpdu.h
**
**  Note the calling function must ensure that the destination memory allocation is
**  sufficient for both the extant and the new TPDU messages.
**
**  For bit fields the penultimate parameter points to the right adjusted replacement
**  data. If these data exceed the bit field width the excess bits are ignored.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_SetField
 *
 * @description  This function replaces the designated TPDU data field. Aborted attempts to
 *               update  a field return a zero otherwise the number of update bytes is returned
 *
 *               Where necessary dependent fields are adjusted to conform with the new replacement.
 *
 * @param        thisField    : as type of fieldName_t.
 * @param        thisTPDU     : as TPDU_p.
 * @param        typeTPDU     : as type of ShortMessageType_t.
 * @param        *newField_p  : uint8_t.
 * @param        newFieldSize : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_SetField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t * newField_p,
    uint8_t newFieldSize)
{
    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Parse structure pointer

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    // check the new field, a failed check gives none zero then return
    if (checkNewField(thisField, newField_p, &newFieldSize, thisParse)) {
        newFieldSize = 0;
    } else {
        // check if it is a bit field and process appropriately
        if (BIT_FIELD(thisField)) {
            replaceBitField(thisField, thisTPDU, *newField_p, thisParse);
        } else {
            replaceOctetField(thisField, thisTPDU, newField_p, newFieldSize, thisParse);
        }

        setDerivedFields(thisTPDU, thisParse, thisField);       // and set any dependent fields
    }

    return newFieldSize;
}


/*************************************************************************************
**
**  Section 6.1 Replacement field checking function. The error type is returned if the
*       a check fails otherwise zero. Note that for address fields if the self
**        derived field length is different to the requested update field length
**        the derived field length is used provided it is valid.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     checkNewField
 *
 * @description  This is a replacement field checking function, which returns the error
 *               type if the check fails otherwise zero
 *
 * @param        thisField :       as type of fieldName_t.
 * @param        *newField_p :     uint8_t.
 * @param        *newFieldSize_p : uint8_t.
 * @param        thisParse :       as type of parse_p.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t checkNewField(
    fieldName_t thisField,
    uint8_t * newField_p,
    uint8_t * newFieldSize_p,
    parse_p thisParse)
{
    uint8_t derivedAddressLength;

    // Check if this field is valid for this message type.
    if (!fieldValid(thisField, thisParse->type)) {
        return FATAL_ERROR;     // Invalid field
    }
    // make sure field update is not prohibited
    if (fieldType & UPDATE_PROHIBITED_BIT) {
        return FATAL_ERROR;
    }
    // There are only tests for octet fields remaining so exit if a bit field
    if (BIT_FIELD(thisField)) {
        return NO_ERROR;
    }
    // Check that the transferred field length is not too long
    if (*newFieldSize_p > fieldMaxLength(thisField)) {
        if ((thisField == TP_UD) || (thisField == TP_CD)) {
            *newFieldSize_p = maxOctetsUD[thisParse->type];
        }
    }
    // Check the transferred field is not too short
    if (*newFieldSize_p < fieldMinLength(thisField)) {
        return FATAL_ERROR;
    }
    // Check that any replacement addresses make sense
    if ((fieldType & FIELD_ADDRESS_MASK) == ADDRESS_FIELD) {
        if (!(*(newField_p + 1) & 0x80))        // MS bit of type must be set
        {
            return NORMAL_ERROR;
        }

        derivedAddressLength = (*newField_p + 5) / 2;
        if (derivedAddressLength != *newFieldSize_p) {
            if (derivedAddressLength <= 22) {
                *newFieldSize_p = derivedAddressLength;
            }
            return NORMAL_ERROR;
        }
    }
    return NO_ERROR;
}


/*************************************************************************************
**
**  Section 6.2 Bit field replacement function
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     replaceBitField
 *
 * @description  Function to replace the bit field of TPDU data
 *
 * @param        thisField :       as type of fieldName_t.
 * @param        thisTPDU :        as type of TPDU_p.
 * @param        replacementBits : uint8_t.
 * @param        thisParse :       as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void replaceBitField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    uint8_t replacementBits,
    parse_p thisParse)
{
    // align replacement bits and mask to avoid overspill
    replacementBits <<= (fieldType & BIT_ALIGN_MASK);   // align
    replacementBits &= bitMask(thisField);      // mask

    thisTPDU[thisParse->index[thisField]] &= ~bitMask(thisField);       // clear
    thisTPDU[thisParse->index[thisField]] |= replacementBits;   // update
}


/*************************************************************************************
**
**  Section 6.3 Octet field replacement function
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     replaceOctetField
 *
 * @description  Function to replace the octet field of TPDU data
 *
 * @param        thisField :    as type of fieldName_t.
 * @param        thisTPDU :     as type of TPDU_p.
 * @param        *newField_p :  uint8_t.
 * @param        newFieldSize : uint8_t.
 * @param        thisParse :    as type of parse_p.
 *
 * @return       static void
 */
/********************************************************************/
static void replaceOctetField(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    uint8_t * newField_p,
    uint8_t newFieldSize,
    parse_p thisParse)
{
    if ((thisParse->length > thisParse->index[thisField]) && (newFieldSize != thisParse->size[thisField]))      // new & old field same sizes ?
    {
        // Their sizes are different so move all the fields above the replaced one
        memmove(thisTPDU + thisParse->index[thisField] + newFieldSize, thisTPDU + thisParse->index[thisField] + thisParse->size[thisField], (thisParse->length - thisParse->index[thisField]));
    }
    // Now insert the new field
    memcpy(thisTPDU + thisParse->index[thisField], newField_p, newFieldSize);

    thisParse->size[thisField] = newFieldSize;
}


/************************************************************************************
**
**  Section 6.4 TPDU Derived Field Setting. This ensures that any derived fields
**        are set appropriately. A field that effects a derived field has
**        the DERIVED_FIELDS_BIT set in the FieldType structure.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     setDerivedFields
 *
 * @description  TPDU Derived Field Setting. This ensures that any derived fields
 *               are set appropriately. A field that effects a derived field has
 *               the DERIVED_FIELDS_BIT set in the FieldType structure.
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisParse : as type of parse_p.
 * @param        thisField : as type of fieldName_t.
 *
 * @return       static void
 */
/********************************************************************/
static void setDerivedFields(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    fieldName_t thisField)
{
    uint8_t indexPI = thisParse->index[TP_PI];

    if (!(fieldType & DERIVED_FIELDS_BIT)) {
        return;
    }

    switch (thisField) {
    case TP_CD:
        // ensure that the TP_CDL field reflects the new size
        thisTPDU[thisParse->index[TP_CDL]] = thisParse->size[TP_CD];
        thisTPDU[0] &= ~TPDU_UDHI_BIT;
        break;

    case TP_DCS:
        // ensure TP_PI reflects the presence of TP_DCS field but only if there!
        if (indexPI) {
            thisTPDU[indexPI] |= TPDU_PI_DCS_BIT;
        }
        break;

    case TP_PID:
        // ensure TP_PI reflects the presence of TP_PID field but only if there!
        if (indexPI) {
            thisTPDU[indexPI] |= TPDU_PI_PID_BIT;
        }
        break;

    case TP_UD:
        // ensure the TP_UDL, the TP_UDHI bit and TP_PI fields are updated
        thisTPDU[0] &= ~TPDU_UDHI_BIT;
        if (indexPI) {
            thisTPDU[indexPI] |= TPDU_PI_UDL_BIT;
        }
        break;

    case TP_VP:
        // If the TP_VP field has been updated and the field size has changed
        // then the format bits must be changed accordingly. Note there is
        // ambiguity between enhanced and absolute formats - ideally the format
        // should be changed first.
        if (thisParse->size[TP_VP] != getVPFieldSize(thisTPDU)) {
            switch (thisParse->size[TP_VP]) {
            case 0:
                thisTPDU[0] &= ~TP_VPF_FORMAT_MASK;
                break;

            case 1:
                thisTPDU[0] &= ~TP_VPF_FORMAT_MASK;
                thisTPDU[0] |= TP_VPF_RELATIVE_FORMAT;
                break;

            case TP_VP_ENHAN_ABS_FIELD_SIZE:
                thisTPDU[0] |= ~TP_VPF_FORMAT_MASK;
                break;

            default:
                break;
            };
        }
        break;

    case TP_VPF:
        {
            uint8_t zeroVPField[] = { 0, 0, 0, 0, 0, 0, 0 };    // only use stack if needed

            // if VP format has changed the TP_VP field size then clear the TP_VP field
            if (thisParse->size[TP_VP] != getVPFieldSize(thisTPDU)) {
                replaceOctetField(TP_VP, thisTPDU, zeroVPField, getVPFieldSize(thisTPDU), thisParse);
            }
            break;
        }

    case TP_UDH:
        // ensure that the TP_UDHI bit is set and that TP_UDHL is correct
        thisTPDU[0] |= TPDU_UDHI_BIT;
        thisUDHL = thisParse->size[TP_UDH];
        thisUDL = thisParse->size[TP_UDH] + 1;
        if (indexPI) {
            thisTPDU[indexPI] |= TPDU_PI_UDL_BIT;
        }
        break;

    default:
        break;
    }
}

/*************************************************************************************
**
** Section 7. TPDU Message Header Creation Functions - Common Notes.
**
**  These functions create new messages of the specified type using the supplied field
**  values. Note that it is the responsibility of the caller to ensure sufficient
**  memory has been allocated. The functions return the number of bytes in the TPDUs,
**  a NULL TPDU pointer results in a zero return.
**
**  Note that there are two types of fields, user fields and derived fields. User fields
**  may be set directly, derived fields are set by the software and access is read only.
**  Attempts to write/set a derived field return an error. The read only fields are:
**
**      TP_CDL    - derived by setting TP_CD
**      TP_UDL    - derived by setting TP_UD directly or indirectly
**      TP_UDHI   - derived by setting TP_UDH directly of indirectly
**      TP_UDHL   - derived by setting TP_UDH directly of indirectly
**      TP_MTI    - implicit in the pack header instruction
**      TP_PI   - derived from TP_DCS, TP_UDL and TP_PID
**
**  Note also that the value of TP_UDL or TP_CDL after a header creation is zero.
**
**************************************************************************************/
/*************************************************************************************
**
** Section 7.1  TPDU Pack SMS_DELIVER Message Header.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_DELIVER_PackHdr
 *
 * @description  Function to pack SMS_DELIVER message header TPDU
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        Tp_MMS :   uint8_t.
 * @param        Tp_SRI :   uint8_t.
 * @param        Tp_RP :    uint8_t.
 * @param        OA_p :     as type of ShortMessagePackedAddress_t.
 * @param        *PID_p :   uint8_t.
 * @param        *DCS_p :   uint8_t.
 * @param        *SCTS_p :  uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_DELIVER_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_MMS,
    uint8_t Tp_SRI,
    uint8_t Tp_RP,
    ShortMessagePackedAddress_t * OA_p,
    uint8_t * PID_p,
    uint8_t * DCS_p,
    uint8_t * SCTS_p)
{
    uint8_t sizeTPDU = 1;
    Tpdu_t *bufferTPDU_p;
    uint8_t numberofbytes;

    bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

    // We need to ensure that any body data is preserved.
    // copy the input data
    // This stops the Parser complaining that is it parsing a TPDU that has not been set up.
    // with a valid Address TON. Does not affect operation - just prevents lots of false errors on the log.
    thisTPDU[3] = 0x80;
    numberofbytes = TPDU_GetFieldXfr(TP_UD, thisTPDU, SM_DELIVER, (uint8_t *) bufferTPDU_p);

    // Set the bit fields in the first octet
    thisTPDU[0] = 0x00;         // set TP_MTI field

    if (Tp_MMS) {
        thisTPDU[0] |= bitMask(TP_MMS);
    }

    if (Tp_SRI) {
        thisTPDU[0] |= bitMask(TP_SRI);
    }

    if (Tp_RP) {
        thisTPDU[0] |= bitMask(TP_RP);
    }
    // now set the remaining fields
    packField((uint8_t *) OA_p, thisTPDU, &sizeTPDU, (uint8_t) (((*(uint8_t *) OA_p) + 5) / 2));

    if (PID_p) {
        thisTPDU[sizeTPDU++] = *PID_p;
    } else {
        thisTPDU[sizeTPDU++] = 0;
    }

    if (DCS_p) {
        thisTPDU[sizeTPDU++] = *DCS_p;
    } else {
        thisTPDU[sizeTPDU++] = DCS_CLASS1_ME_SPECIFIC;
    }

    packField(SCTS_p, thisTPDU, &sizeTPDU, TP_SCTS_SIZE);
    thisTPDU[sizeTPDU] = 0;

    // Now set up the User Data previously preserved.
    numberofbytes = TPDU_SetField(TP_UD, thisTPDU, SM_DELIVER, (uint8_t *) bufferTPDU_p, numberofbytes);
    sizeTPDU = sizeTPDU + numberofbytes;

    SMS_HEAP_FREE(&bufferTPDU_p);

    return sizeTPDU;
}


/*************************************************************************************
**
** Section 7.2  TPDU Pack SMS_DELIVER_REPORT Message Header. Note this function
**        produces either (RP_ERROR) or (RP_ACK) messages depending on whether
**        Tp_FCS has been set.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_DELIVER_REPORT_PackHdr
 *
 * @description  Function to pack SMS_DELIVER_REPORT message header TPDU.
 *               This functionproduces either RP_ERROR or RP_ACK messages
 *               depending on whether Tp_FCS has been set.
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        Tp_FCS   : as type of TP_FailureCause_t.
 * @param        *PID_p   : uint8_t.
 * @param        *DCS_p   : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_DELIVER_REPORT_PackHdr(
    const TPDU_p thisTPDU,
    TP_FailureCause_t Tp_FCS,
    uint8_t * PID_p,
    uint8_t * DCS_p)
{
    uint8_t sizeTPDU = 1;
    uint8_t *PI_p;
    Tpdu_t *bufferTPDU_p;
    uint8_t numberofbytes;

    bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

    // RWe need to ensure that any body data is preserved.
    // copy the input data
    numberofbytes = TPDU_GetFieldXfr(TP_UD, thisTPDU, SM_DELIVER_REPORT_ACK, (uint8_t *) bufferTPDU_p);

    // Set the bit fields in the first octet
    thisTPDU[0] = 0x00;         // set TP_MTI field

    // now set the remaining fields
    if (Tp_FCS) {
        thisTPDU[sizeTPDU++] = Tp_FCS;  // only packed if error present
    }

    PI_p = thisTPDU + sizeTPDU; // pointer to TP_PI
    thisTPDU[sizeTPDU++] = 0;

    PACK_OPTIONAL_PARAMETERS    // macro also updates TP_PI
        if (numberofbytes > 0) {
        // Now set up the User Data previously preserved.
        numberofbytes = TPDU_SetField(TP_UD, thisTPDU, SM_DELIVER_REPORT_ACK, (uint8_t *) bufferTPDU_p, numberofbytes);
        sizeTPDU = sizeTPDU + numberofbytes;
    }

    SMS_HEAP_FREE(&bufferTPDU_p);

    return sizeTPDU;
}


/*************************************************************************************
**
** Section 7.3  TPDU Pack SMS_SUBMIT Message Header Function.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_SUBMIT_PackHdr
 *
 * @description  Function to pack SMS_SUBMIT message header TPDU
 *
 * @param        thisTPDU : TPDU_p.
 * @param        Tp_RD :    uint8_t.
 * @param        Tp_VPF :   as type of formatVPF_t.
 * @param        Tp_SRR :   uint8_t.
 * @param        Tp_RP :    uint8_t.
 * @param        DA_p :     as type of ShortMessagePackedAddress_t.
 * @param        *PID_p :   uint8_t.
 * @param        *DCS_p :   uint8_t.
 * @param        *Tp_VP :   uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_SUBMIT_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_RD,
    formatVPF_t Tp_VPF,
    uint8_t Tp_SRR,
    uint8_t Tp_RP,
    ShortMessagePackedAddress_t * DA_p,
    uint8_t * PID_p,
    uint8_t * DCS_p,
    uint8_t * Tp_VP)
{
    uint8_t sizeTPDU = 1;
    Tpdu_t *bufferTPDU_p;
    uint8_t numberofbytes;
    uint8_t UDHISet = FALSE;
    uint8_t UDL = 0;

    // This stops the Parser complaining that is it parsing a TPDU that has not been set up.
    // with a valid Address TON. Does not affect operation - just prevents lots of false errors on the log.
    thisTPDU[3] = 0x80;

    // Preserve UDHI as this gets trahsed when setting the UD field again
    bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

    UDHISet = USER_HEADER ? TRUE : FALSE;

    // Get the UDL and preserve it as it was correctly cacluated
    // at the point of packing the SM data
    UDL = TPDU_GetField(TP_UDL, thisTPDU, SM_SUBMIT);

    // We need to ensure that any body data is preserved.
    // copy the input data
    numberofbytes = TPDU_GetFieldXfr(TP_UD, thisTPDU, SM_SUBMIT, (uint8_t *) bufferTPDU_p);

    // Set the bit fields in the first octet
    thisTPDU[0] = 0x01;

    if (Tp_RD) {
        thisTPDU[0] |= bitMask(TP_RD);
    }

    if (Tp_VPF) {
        thisTPDU[0] |= (bitMask(TP_VPF) & Tp_VPF);
    }

    if (Tp_SRR) {
        thisTPDU[0] |= bitMask(TP_SRR);
    }

    if (Tp_RP) {
        thisTPDU[0] |= bitMask(TP_RP);
    }
    // now set the remaining fields
    thisTPDU[sizeTPDU++] = SMS_DEFAULT_TP_MR;   // initialise TP-MR field this is overwritten during the send

    packField((uint8_t *) DA_p, thisTPDU, &sizeTPDU, (uint8_t) (((*(uint8_t *) DA_p) + 5) / 2));

    if (PID_p) {
        thisTPDU[sizeTPDU++] = *PID_p;
    } else {
        thisTPDU[sizeTPDU++] = 0;
    }

    if (DCS_p) {
        thisTPDU[sizeTPDU++] = *DCS_p;
    } else {
        thisTPDU[sizeTPDU++] = DCS_CLASS1_ME_SPECIFIC;
    }

    packField(Tp_VP, thisTPDU, &sizeTPDU, getVPFieldSize(thisTPDU));

    // Now set up the User Data previously preserved.
    thisTPDU[sizeTPDU++] = UDL;
    numberofbytes = TPDU_SetField(TP_UD, thisTPDU, SM_SUBMIT, (uint8_t *) bufferTPDU_p, numberofbytes);
    sizeTPDU = sizeTPDU + numberofbytes;

    if (UDHISet) {
        thisTPDU[0] = thisTPDU[0] | TPDU_UDHI_BIT;
    }

    SMS_HEAP_FREE(&bufferTPDU_p);
    return sizeTPDU;
}

/*************************************************************************************
**
** Section 7.4  TPDU Pack SMS_SUBMIT_REPORT Message Header Function. Note this function
**        produces either (RP_ERROR) or (RP_ACK) messages depending on whether
**        Tp_FCS has been set.
**
**************************************************************************************/


/********************************************************************/
/**
 *
 * @function     TPDU_SUBMIT_REPORT_PackHdr
 *
 * @description  Function to pack SMS_SUBMIT_REPORT message header  TPDU,
 *               which produces either RP_ERROR or RP_ACK message depending
 *               on whether Tp_FCS has been set
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        Tp_FCS   : as type of TP_FailureCause_t.
 * @param        *SCTS_p  : uint8_t.
 * @param        *PID_p   : uint8_t.
 * @param        *DCS_p   : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_SUBMIT_REPORT_PackHdr(
    const TPDU_p thisTPDU,
    TP_FailureCause_t Tp_FCS,
    uint8_t * SCTS_p,
    uint8_t * PID_p,
    uint8_t * DCS_p)
{
    uint8_t sizeTPDU = 1;
    uint8_t *PI_p;
    Tpdu_t *bufferTPDU_p;
    uint8_t numberofbytes;

    // We need to ensure that any body data is preserved.
    // copy the input data
    bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));
    numberofbytes = TPDU_GetFieldXfr(TP_UD, thisTPDU, SM_SUBMIT_REPORT_ACK, (uint8_t *) bufferTPDU_p);

    thisTPDU[0] = 0x01;         // set TP_MTI field

    if (Tp_FCS) {
        thisTPDU[sizeTPDU++] = Tp_FCS;  // only packed if error present
    }

    PI_p = thisTPDU + sizeTPDU; // TP_PI pointer
    thisTPDU[sizeTPDU++] = 0;   // TP_PI location
    packField(SCTS_p, thisTPDU, &sizeTPDU, TP_SCTS_SIZE);

    PACK_OPTIONAL_PARAMETERS    // macro also updates TP_PI
        // Now set up the User Data previously preserved.
        numberofbytes = TPDU_SetField(TP_UD, thisTPDU, SM_SUBMIT_REPORT_ACK, (uint8_t *) bufferTPDU_p, numberofbytes);
    sizeTPDU = sizeTPDU + numberofbytes;

    SMS_HEAP_FREE(&bufferTPDU_p);

    return sizeTPDU;
}


/*************************************************************************************
**
** Section 7.5 TPDU Pack SMS_STATUS_REPORT Message Header Function.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_STATUS_REPORT_PackHdr
 *
 * @description  Function to pack SMS_STATUS_REPORT message header TPDU
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        Tp_MMS :   uint8_t.
 * @param        Tp_SRQ :   uint8_t.
 * @param        RA_p :     as type of ShortMessagePackedAddress_t.
 * @param        *SCTS_p :  uint8_t.
 * @param        *Tp_DT :   uint8_t.
 * @param        Tp_ST :    uint8_t.
 * @param        *PID_p :   uint8_t.
 * @param        *DCS_p :   uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_STATUS_REPORT_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_MMS,
    uint8_t Tp_SRQ,
    ShortMessagePackedAddress_t * RA_p,
    uint8_t * SCTS_p,
    uint8_t * Tp_DT,
    uint8_t Tp_ST,
    uint8_t * PID_p,
    uint8_t * DCS_p)
{
    uint8_t sizeTPDU = 1;
    uint8_t *PI_p;
    Tpdu_t *bufferTPDU_p;
    uint8_t numberofbytes;

    // We need to ensure that any body data is preserved.
    // copy the input data
    // This stops the Parser complaining that is it parsing a TPDU that has not been set up.
    // with a valid Address TON. Does not affect operation - just prevents lots of false errors on the log.
    bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

    thisTPDU[3] = 0x80;
    numberofbytes = TPDU_GetFieldXfr(TP_UD, thisTPDU, SM_STATUS_REPORT, (uint8_t *) bufferTPDU_p);

    // Set the bit fields in the first octet
    thisTPDU[0] = 0x02;         // set TP_MTI field

    if (Tp_MMS) {
        thisTPDU[0] |= bitMask(TP_MMS);
    }

    if (Tp_SRQ) {
        thisTPDU[0] |= bitMask(TP_SRQ);
    }
    // now set the remaining mandatory fields
    thisTPDU[sizeTPDU++] = 0;   // initialise TP-MR field this is overwritten during the send

    packField((uint8_t *) RA_p, thisTPDU, &sizeTPDU, (uint8_t) (((*(uint8_t *) RA_p) + 5) / 2));
    packField(SCTS_p, thisTPDU, &sizeTPDU, TP_SCTS_SIZE);
    packField(Tp_DT, thisTPDU, &sizeTPDU, TP_DT_SIZE);
    thisTPDU[sizeTPDU++] = Tp_ST;
    PI_p = thisTPDU + sizeTPDU; // pointer to TP_PI
    thisTPDU[sizeTPDU++] = 0;

    PACK_OPTIONAL_PARAMETERS    // macro also updates TP_PI
        if (!*PI_p) {
        sizeTPDU -= 1;          // no optional parameters no TP_PI
    }
    // Now set up the User Data previously preserved.
    numberofbytes = TPDU_SetField(TP_UD, thisTPDU, SM_STATUS_REPORT, (uint8_t *) bufferTPDU_p, numberofbytes);
    sizeTPDU = sizeTPDU + numberofbytes;

    SMS_HEAP_FREE(&bufferTPDU_p);

    return sizeTPDU;
}


/*************************************************************************************
**
** Section 7.6 TPDU Pack SMS_COMMAND Message Header Function.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_COMMAND_PackHdr
 *
 * @description  Function to pack the SMS_COMMAND message header TPDU
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        Tp_SRR :   uint8_t.
 * @param        *PID_p :   uint8_t.
 * @param        *CT_p :    uint8_t.
 * @param        *MN_p :    uint8_t.
 * @param        DA_p :     as type of ShortMessagePackedAddress_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_COMMAND_PackHdr(
    const TPDU_p thisTPDU,
    uint8_t Tp_SRR,
    uint8_t * PID_p,
    uint8_t * CT_p,
    uint8_t * MN_p,
    ShortMessagePackedAddress_t * DA_p)
{
    uint8_t sizeTPDU = 1;
    Tpdu_t *bufferTPDU_p;
    uint8_t numberofbytes;

    // We need to ensure that any body data is preserved.
    // copy the input data
    // This stops the Parser complaining that is it parsing a TPDU that has not been set up.
    // with a valid Address TON. Does not affect operation - just prevents lots of false errors on the log.
    bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

    thisTPDU[3] = 0x80;
    numberofbytes = TPDU_GetFieldXfr(TP_UD, thisTPDU, SM_COMMAND, (uint8_t *) bufferTPDU_p);

    thisTPDU[0] = 0x02;         // set TP_MTI field
    if (Tp_SRR) {
        thisTPDU[0] |= bitMask(TP_SRR); // set first octet bit field
    }
    // now set the remaining fields - all mandatory
    thisTPDU[sizeTPDU++] = 0;   // initialise TP-MR field this is overwritten during the send

    if (PID_p) {
        thisTPDU[sizeTPDU++] = *PID_p;
    } else {
        thisTPDU[sizeTPDU++] = 0;
    }

    if (CT_p) {
        thisTPDU[sizeTPDU++] = *CT_p;
    } else {
        thisTPDU[sizeTPDU++] = 0;
    }

    if (MN_p) {
        thisTPDU[sizeTPDU++] = *MN_p;
    } else {
        thisTPDU[sizeTPDU++] = 0;
    }

    packField((uint8_t *) DA_p, thisTPDU, &sizeTPDU, (uint8_t) (((*(uint8_t *) DA_p) + 5) / 2));
    thisTPDU[sizeTPDU++] = 0;

    // Now set up the User Data previously preserved.
    numberofbytes = TPDU_SetField(TP_UD, thisTPDU, SM_COMMAND, (uint8_t *) bufferTPDU_p, numberofbytes);
    sizeTPDU = sizeTPDU + numberofbytes;

    SMS_HEAP_FREE(&bufferTPDU_p);

    return sizeTPDU;
}


/*************************************************************************************
**
**  Section 7.7 TPDU Pack Field Function. This function adds a field to the TPDU, if
**        the field pointer is NULL then the field is set to zero.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     packField
 *
 * @description  TPDU Pack Field Function. This function adds a field to the TPDU,
 *               if the field pointer is NULL then the field is set to zero.
 *
 * @param        *newField :    uint8_t.
 * @param        thisTPDU :     as type of TPDU_p.
 * @param        *index :       uint8_t.
 * @param        sizeNewField : uint8_t.
 *
 * @return       static void
 */
/********************************************************************/
static void packField(
    uint8_t * newField,
    TPDU_p thisTPDU,
    uint8_t * index,
    uint8_t sizeNewField)
{
    uint8_t idx;

    for (idx = 0; idx < sizeNewField; idx++) {
        if (newField) {
            thisTPDU[*index] = newField[idx];
        } else {
            thisTPDU[*index] = 0;
        }

        *index += 1;
    }
}


/*************************************************************************************
**
**  Section 8. TPDU Pack Text Function
**
**  This function adds text to the user data area where necessary converting the format
**  to match that defined by TP_DCS. Note that the text is added after any user data
**  header and replaces any text that may already be there. If necessary the data
**  coding schems (TP_DCS) is adjusted to accord with the alphabet parameter.
**
**  The overall TPDU size (in octets) is returned.  A NULL TPDU pointer returns zero.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_PackText
 *
 * @description  This function adds text to the user data area where necessary
 *               converting the format to match that defined by TP_DCS
 *
 * @param        thisTPDU     : as type of TPDU_p.
 * @param        typeTPDU     : as type of ShortMessageType_t.
 * @param        textLength   : uint8_t.
 * @param        textAlphabet : uint8_t.
 * @param        *text_p      : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_PackText(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t textLength,
    uint8_t textAlphabet,
    uint8_t * text_p)
{
    uint8_t *out_p;
    uint8_t packedOctets = 0;
    uint8_t fillBits = 0;
    uint16_t requiredBits = 0;

    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    // do not skip this bit if no text as it convert UDL to correct alphabet meaning
    {
        if (textAlphabet != thisParse->alphabet) {
            setAlphabet(thisTPDU, thisParse, textAlphabet);
        }
        // Set a text pointer to the next free TPDU byte and set pakedOctets
        out_p = thisTPDU + thisParse->index[TP_UDL] + 1;        // pointer to user data

        // Set the user data length (TP_UDL), the thisParse->length and the textLength in packedOctets
        if (textAlphabet == ALPHABET_7_BIT) {
            if (USER_HEADER) {
                packedOctets = thisUDHL + 1;
                out_p += packedOctets;  // allow for user data header
                fillBits = (7 - ((8 * (uint16_t) (packedOctets)) % 7)) % 7;
            }

            requiredBits = (8 * packedOctets) + (7 * textLength) + fillBits;
            requiredBits = SMS_MIN((maxOctetsUD[thisParse->type] * 8), requiredBits);
            packedOctets = (7 + requiredBits) / 8;
            thisUDL = (6 + requiredBits) / 7;
        } else                  // 8-bit alphabet and 16-bit UCS2
        {
            if (USER_HEADER) {
                packedOctets = thisUDHL + 1;
                out_p += packedOctets;  // allow for user data header
            }
            // Check for overrun
            textLength = SMS_MIN((maxOctetsUD[thisParse->type] - packedOctets), textLength);

            if (textAlphabet == ALPHABET_16_BIT) {
                // Check if header is odd and message exceeds maximum size (TP-UD = 139 bytes total)
                // If so, exclude last whole character by reducing length count by 2
                if ((packedOctets & 0x01) && (packedOctets + textLength > 0x8b)) {
                    // Odd number of bytes in TP-UD so lets discard the last two from the UCS2 content
                    textLength -= 2;
                }
            }
            packedOctets += textLength;
            thisUDL = packedOctets;
        }

        thisParse->length = thisParse->index[TP_UDL] + 1 + packedOctets;

        // mark the TP_UDL as present in the TP_DCS - if it is there
        if (thisParse->index[TP_PI]) {
            thisPI |= TPDU_PI_UDL_BIT;
        }
        // convert and transfer the text to the TPDU
        switch (textAlphabet) {
        case ALPHABET_7_BIT:
            {
                if (textLength) {
                    // To prevent unitialised memory reads, copy to a buffer one character
                    // larger than needed. This is needed due to a boundary error in packspetets
                    // that has been there for a long time.
                    // This is a compromise interim siolution before a re-write of pack septets
                    uint8_t *text_ptr = SMS_HEAP_UNTYPED_ALLOC(textLength + 1);
                    // Set the last byte to 0
                    memset(text_ptr + textLength, 0x00, 1);
                    memcpy(text_ptr, text_p, textLength);

                    packSeptets(out_p, text_ptr, (thisTPDU + thisParse->length), fillBits);

                    SMS_HEAP_FREE(&text_ptr);
                } else {
                    // To prevent unitialised memory reads, copy to a buffer one character
                    // larger than needed. This is needed due to a boundary error in packspetets
                    // that has been there for a long time.
                    // This is a compromise interim siolution before a re-write of pack septets
                    uint8_t fillChar[MAXIMUM_TPDU_SIZE];

                    memset(fillChar, 0xFF, MAXIMUM_TPDU_SIZE);
                    packSeptets(out_p, fillChar, (thisTPDU + thisParse->length), fillBits);
                }
                break;
            }

        case ALPHABET_8_BIT:
            memcpy(out_p, text_p, textLength);
            break;

        case ALPHABET_16_BIT:
            memcpyUNICODE(out_p, text_p, textLength);
            break;

        default:
            break;
        }
    }

    return thisParse->length;
}

/*************************************************************************************
**
**  Section 8a. TPDU Copy Text Function (Only used for packing RP_ACK body from
**              SIMTOOL SMS-PP Data Download, as we ignore syntax.
**
**  This function adds text to the user data area. Note that the text is added after
**  any user data header and replaces any text that may already be there.
**
**  The overall TPDU size (in octets) is returned.  A NULL TPDU pointer returns zero.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_CopyText
 *
 * @description  Function to copy text TPDU
 *
 * @param        thisTPDU     : as type of TPDU_p.
 * @param        typeTPDU     : as type of ShortMessageType_t.
 * @param        textLength   : uint8_t.
 * @param        textAlphabet : uint8_t.
 * @param        *text_p      : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_CopyText(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t textLength,
    uint8_t textAlphabet,
    uint8_t * text_p)
{
    uint8_t *out_p;
    uint8_t packedOctets = 0;

    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    SMS_IDENTIFIER_NOT_USED(textAlphabet);

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    if (textLength) {
        // Set a text pointer to the next free TPDU byte and set pakedOctets
        out_p = thisTPDU + thisParse->index[TP_UDL] + 1;        // pointer to user data

        // Set the user data length (TP_UDL), the MessageSize and the textLength in packedOctets
        textLength = SMS_MIN((maxOctetsUD[thisParse->type] - packedOctets), textLength);
        packedOctets += textLength;
        thisUDL = packedOctets;

        thisParse->length = thisParse->index[TP_UDL] + 1 + packedOctets;

        // mark the TP_UDL as present in the TP_DCS - if it is there
        if (thisParse->index[TP_PI]) {
            thisPI |= TPDU_PI_UDL_BIT;
        }
        // Copy the body supplied into the TPDU
        memcpy(out_p, text_p, textLength);
    }

    return thisParse->length;
}

/*************************************************************************************
*
*  Section 8.1  Septet packing function, note the actual packed septets are dependent on
*       the setting of shift which is initialised to the number of fill bits.
*       Note also that 'end_p' points to the first byte after the message.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     packSeptets
 *
 * @description  Function to pack Septets, where the actual packed septets are dependent
 *               on the setting of shift which is initialised to the number of fill bits.
 *               Note also that 'end_p' points to the first byte after the message.
 *
 * @param        *out_p :  uint8_t.
 * @param        *text_p : uint8_t.
 * @param        *end_p :  uint8_t.
 * @param        shift :   uint8_t.
 *
 * @return       static void
 */
/********************************************************************/
static void packSeptets(
    uint8_t * out_p,
    uint8_t * text_p,
    uint8_t * end_p,
    uint8_t shift)
{

    // first set the initial conditions
    if (shift) {
        *out_p++ = ((*text_p) & 0x7F) << shift;
        shift = (8 - shift) % 7;
        if (!shift)
            text_p++;
    }
    // Turn off lint warnings 661 and 662 as we know there is a bounds problem with
    // this function - however, it is too high risk to re-write this.

    // now translate the bulk of the message
    while (out_p < end_p) {
        /*lint --e{661} --e{662} */
        *out_p = (((*(text_p++)) & 0x7F) >> shift);
        *(out_p++) |= (((*text_p) & 0x7F) << (7 - shift));
        if (++shift >= 7) {
            shift = 0;
            text_p++;
        }
    }
}


/*************************************************************************************
**
**  Section 8.2 This function carries out a memory copy for UNICODE data which is the
**        same as a normal copy with adjacent bytes swapped over.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     memcpyUNICODE
 *
 * @description  Function of memory copy for UNICODE data which is the same as a
 *               normal copy with adjacent bytes swapped over.
 *
 * @param        *out_p :     uint8_t.
 * @param        *in_p :      uint8_t.
 * @param        textLength : uint8_t.
 *
 * @return       static void
 */
/********************************************************************/
static void memcpyUNICODE(
    uint8_t * out_p,
    uint8_t * in_p,
    uint8_t textLength)
{
    uint8_t i;

    for (i = 0; i < textLength; i += 2) {
        // Swap the bytes around for 16 bit format
        *out_p++ = *(in_p + 1);
        *(out_p++) = *(in_p++);
        in_p++;
    }
}


/*************************************************************************************
**
**  Section 9. TPDU text unpacking function
**
**  This function transfers to a designated address the user data text. The number of
**  text bytes is returned.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_UnpackText
 *
 * @description  Function to unpack text TPDU
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        typeTPDU : as type of ShortMessageType_t.
 * @param        *dest_p :  uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_UnpackText(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t * dest_p)
{

    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer
    uint8_t textBytes = 0;

    parseTPDU(thisTPDU, typeTPDU, thisParse);
    textBytes = extractText(thisTPDU, thisParse, dest_p, thisUDL);

    return textBytes;
}


/*************************************************************************************
**
**  Section 9.1 Text Extraction Function. Extract the text from the TP_UD to the
**        designated destination and return the number of text bytes. Note for
**        septet characters the whole of the user data area is unpacked and then
**        the user data header is removed - only the actual text is required.
**
**  WARNINGS:   Assumes that the text length also includes the length of header, and
**              that the destination buffer has enough room to hold the header and the
**              required text.
**
**              Should not use this function for getting the first X bytes of text into
**              a buffer of size X.
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     extractText
 *
 * @description  Function to extract text from the TP_UD to the designated
 *               destination and return the number of text bytes.
 *
 * @param        thisTPDU :       as type of TPDU_p.
 * @param        thisParse :      as type of parse_p.
 * @param        *dest_p :        uint8_t.
 * @param        UserDataLength : uint8_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t extractText(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    uint8_t * dest_p,
    uint8_t UserDataLength)
{
    uint8_t *TPDUData_p;
    uint8_t *in_p;
    uint8_t headerFields = 0;
    uint8_t headerSeptets = 0;
    uint8_t textLength = 0;

    if (UserDataLength) {
        TPDUData_p = thisTPDU + thisParse->index[TP_UDL] + 1;   // point to user data

        if (USER_HEADER) {
            headerFields = thisParse->size[TP_UDH] + 1;
        }

        if (headerFields > maxOctetsUD[thisParse->type])        // can't have more user data headers than will fit in a message
        {
            // the TPDU is corrupt so let's return nothing
            textLength = 0;
        } else {
            textLength = UserDataLength;
            in_p = TPDUData_p + headerFields;

            switch (thisParse->alphabet) {
            case ALPHABET_7_BIT:
                if (textLength > MAX_UD_SEPTETS(thisParse->type))       // can't have more text than will fit in a message
                {
                    // the TPDU is corrupt so let's return nothing
                    textLength = 0;
                } else {
                    headerSeptets = ROUND_UP_TO_SEPTETS(headerFields);
                    if (headerSeptets > textLength) {
                        // UDH is corrupt so let's return nothing
                        textLength = 0;
                    } else {
                        unpackSeptets(dest_p, TPDUData_p, textLength);
                        textLength = textLength - headerSeptets;
                        memmove(dest_p, dest_p + headerFields, textLength);
                    }
                }
                break;

            case ALPHABET_8_BIT:
                if (textLength > maxOctetsUD[thisParse->type])  // can't have more text than will fit in a message
                {
                    // the TPDU is corrupt so let's return nothing
                    textLength = 0;
                }

                if (headerFields > textLength) {
                    // UDH is corrupt so let's return nothing
                    textLength = 0;
                } else {
                    textLength = textLength - headerFields;
                    memcpy(dest_p, in_p, textLength);
                }
                break;

            case ALPHABET_16_BIT:
                if (textLength > maxOctetsUD[thisParse->type])  // can't have more text than will fit in a message
                {
                    // the TPDU is corrupt so let's return nothing
                    textLength = 0;
                }

                if (headerFields > textLength) {
                    // UDH is corrupt so let's return nothing
                    textLength = 0;
                } else {
                    textLength = textLength - headerFields;
                    in_p = TPDUData_p + headerFields;
                    memcpyUNICODE(dest_p, in_p, textLength);
                }
                break;

            default:
                textLength = 0;
            }
        }
    }
    return textLength;
}


/*************************************************************************************
**
**  Section 9.2 Septet unpacking function.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     unpackSeptets
 *
 * @description  Function to unpack Septets
 *
 * @param        *out_p :     uint8_t.
 * @param        *text_p :    uint8_t.
 * @param        textLength : uint8_t.
 *
 * @return       static void
 */
/********************************************************************/
static void unpackSeptets(
    uint8_t * out_p,
    uint8_t * text_p,
    uint8_t textLength)
{
    uint8_t shift = 0;
    uint8_t *end_p;

    end_p = out_p + textLength;

    *out_p = 0;

    while (out_p < end_p) {
        *out_p |= (*text_p << shift) & 0x7F;
        out_p++;

        if (out_p >= end_p) {
            break;
        }
        *out_p = *text_p++ >> (7 - shift);

        if (++shift >= 7) {
            shift = 0;
            out_p++;

            if (out_p >= end_p) {
                break;
            }
            *out_p = 0;
        }
    }
}


/*************************************************************************************
**
**  Section 10. TPDU Get Voice Mail Information Function
**
**  This function returns TRUE if voice mail is found otherwise FALSE. When found the
**  associated message waiting list is updated.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetVoiceMailInfo
 *
 * @description  Function to get voice mail information TPDU
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        typeTPDU : as type of ShortMessageType_t.
 * @param        thisMsg :  as type of MWIList_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_GetVoiceMailInfo(
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    MWIList_p thisMsg)
{
    uint8_t isVoiceMail = FALSE;        // assume no voice mail at the start
    MWIHandlerFunction_p MWIFunction;   // function pointer to appropriate MWI function
    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    MWIList_t DummyMsgWaitingList;
    MWIList_p MsgWaitingList_p;

    MsgWaitingList_p = thisMsg;

    // Ensure that the discard flag has defaulted to TRUE, it will be set to false
    // if any of the MWI indicates that the message should be stored
    thisMsg->DiscardShortMessage = TRUE;
    DummyMsgWaitingList.DiscardShortMessage = TRUE;

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    // Check the first MWI function
    MWIFunction = definedMWIOrder[0].firstMWIFunction;
    isVoiceMail = (MWIFunction) (thisTPDU, MsgWaitingList_p, thisParse);

    // Even if we have found voicemail information, we need to continue and check the storage
    // does not confilct between DCS and UDH

    if (isVoiceMail) {
        // If we found a voicemail indication then thisMsg will contain the information we
        // need.  However, we need to still check the other forms of MWI to see if they exist
        // and have different discard settings.  Therefore we continue to call the remaining
        // MWI search funtaion but we now pass in a dummy MWIList so as not to overwrite the
        // one we have just got.
        MsgWaitingList_p = &DummyMsgWaitingList;
    }
    // Check the next MWI function, MsgWaitingList_p will be pointing to dummy data if
    // the isVoiceMail parameter is TRUE.
    MWIFunction = definedMWIOrder[0].secondMWIFunction;
    isVoiceMail = (isVoiceMail | (MWIFunction) (thisTPDU, MsgWaitingList_p, thisParse));

    if (isVoiceMail) {
        MsgWaitingList_p = &DummyMsgWaitingList;
    }
    // Check the last MWI function, MsgWaitingList_p will be pointing to dummy data if
    // the isVoiceMail parameter is TRUE.
    MWIFunction = definedMWIOrder[0].thirdMWIFunction;
    isVoiceMail = (isVoiceMail | (MWIFunction) (thisTPDU, MsgWaitingList_p, thisParse));

    // If the Dummy MWI indicates not to discard the message, update the proper
    // MWIList information accordingly
    if (FALSE == DummyMsgWaitingList.DiscardShortMessage) {
        thisMsg->DiscardShortMessage = FALSE;
    }

    return isVoiceMail;
}


/*************************************************************************************
*
*  Section 10.1 The TP_UserDataHeader may contain special message information as
*       defined in GSM 03.40, 9.2.3.24.2. The function returns TRUE if a
*       special message is found and updates the waiting list appropriately.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     getSpecialMessageInfo
 *
 * @description  Function to get special message information if the TP_UserDataHeader
 *               contains special message information.
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisMsg :   as type of MWIList_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t getSpecialMessageInfo(
    const TPDU_p thisTPDU,
    MWIList_p thisMsg,
    parse_p thisParse)
{
    uint8_t MICount;
    uint8_t MIType;
    uint8_t listIndex = 0;
    uint8_t *UDH_p;
    uint8_t isMsgWaiting = FALSE;
    uint8_t ieInstance = 0;
    uint8_t Lcl_TextAssociated = FALSE;

    // if there is no user data header for whatever reason cannot be special message
    if (!USER_HEADER || !thisUDL) {
        return NOT_SPECIAL_MSG;
    }

    UDH_p = thisTPDU + thisParse->index[TP_UDL] + 2;

    // get the IEI
    while ((UDH_p != NULL) && (ieInstance < MAX_LENGTH_OF_MSG_WAITING_IND_LIST - 1)) {
        // e.g. Voice Message Waiting, Store message, Message Indication Count 01
        //      IEI: 01  IEIDL: 02  Special Indication Data: 80  01
        UDH_p = findUserDataIE(thisTPDU, thisParse, IEI_SPECIAL_MSG, ieInstance);

        if (UDH_p && (*(++UDH_p) == 2)) // Check 2 byte IEI found
        {
            MIType = *(++UDH_p);        // Message Indication Type
            MICount = *(++UDH_p);       // Message Indication Count

            {
                uint8_t Lcl_isMsgWaiting = FALSE;       // Local copy of IsMsgWaiting to prevent risk of over-writing previous 'TRUE' during while-loop iterations

                thisMsg->MWI_Indication[listIndex].Type = msgWaitingType((uint8_t) (MIType & 0x7F), &Lcl_isMsgWaiting);

                if (Lcl_isMsgWaiting) {
                    isMsgWaiting = Lcl_isMsgWaiting;
                }
            }

            // set the discard flags
            // Assume that if the message is only stored if there is text in it.

            // Note: Discard Message is defaulted to true, so don't need to ever set it
            // to true, therfore if it is ever set to FALSE the FALSE value will prevail, even
            // if a subsequent IE inidicated that it was not to be stored
            if ((MIType & SPECIAL_MSG_STORE_BIT) && (thisUDL > (thisUDHL + 1))) {
                Lcl_TextAssociated = TRUE;
                thisMsg->DiscardShortMessage = FALSE;
            }
            // Now overwrite the count and set the icon control flag
            thisMsg->MWI_Indication[listIndex].Count = MICount;

            // Increment Index Count
            listIndex++;
        }
        // Increment Total Number of IEI count
        ieInstance++;
    }

    // Set Index
    thisMsg->MWIList_Index = listIndex;

    // Determine whether there is any MWI data which should be stored.  If there is, keep the text.
    if (Lcl_TextAssociated) {
        thisMsg->TextAssociated = Lcl_TextAssociated;
    }

    return isMsgWaiting;
}


/*************************************************************************************
*
*  Section 10.2 This function checks for the presence of voice mail and decodes the
*       originating address in accordance with the Common Handset Specification
*       Phase 2 (version 4.2). The message waiting list is updated accordingly.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     getVoiceMailInfo_PCN
 *
 * @description  This function checks for the presence of voice mail and decodes the
 *               originating address in accordance with the Common Handset Specification
 *               Phase 2. The message waiting list is updated accordingly.
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisMsg :   as type of MWIList_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t getVoiceMailInfo_PCN(
    const TPDU_p thisTPDU,
    MWIList_p thisMsg,
    parse_p thisParse)
{
    uint8_t *thisOA_p;

    // cannot be Orange voice mail if TP_OA not present
    if (!thisParse->size[TP_OA]) {
        return PCN_NOT_VOICE_MAIL;
    }
    // set pointer to originating address (TP_OA) field
    thisOA_p = thisTPDU + thisParse->index[TP_OA];

    // decode the TP_OA field and check for Orange voice mail
    if (!isOrangeVoiceMail(thisOA_p)) {
        return PCN_NOT_VOICE_MAIL;
    }
    // Process Orange PCN Voice Mail
    // Set the icon control flag
    {
        uint8_t IndicationOn = (*(thisOA_p + 2) & PCN_VMWI_INDICATOR_MASK);

        if (IndicationOn) {
            // Count is used to indicate the presence, or absence, of messages.  As there is no 'real' count value,
            // set it to 1.
            thisMsg->MWI_Indication[0].Count = 1;
        } else {
            thisMsg->MWI_Indication[0].Count = 0;
        }
    }

    // Set Line 1 or Line 2
    {
        uint8_t Line = 1 + (*(thisOA_p + 2) >> 7);

        if (Line == 1) {
            thisMsg->MWI_Indication[0].Type = MWI_VOICE_MAIL_LINE1;
        } else {
            thisMsg->MWI_Indication[0].Type = MWI_VOICE_MAIL_LINE2;
        }

        // ALS has been determined, rather than assumed.
        thisMsg->ALS_LineDetermined = TRUE;
    }

    // Set the message text and message discard flags
    if (((thisUDL == 1) && (thisTPDU[thisParse->index[TP_UD]] == PCN_SPACE_IN_SHORT_MESSAGE)) || NO_TEXT) {
        thisMsg->TextAssociated = FALSE;
    } else if ((thisUDL > 1) && USER_HEADER) {
        uint8_t *text_p = SMS_HEAP_UNTYPED_ALLOC(MSG_DEFAULT_ALPHABET_MAX_SHORT_MESSAGE_CHARACTERS);
        uint8_t textLength = 0;

        if (text_p != NULL) {
            textLength = extractText(thisTPDU, thisParse, text_p, thisUDL);
        }

        if ((textLength == 0) || ((textLength == 1) && text_p && text_p[0] == PCN_SPACE_IN_SHORT_MESSAGE))      // if the UDL is > 1 and there is a User Data Header
        {
            thisMsg->TextAssociated = FALSE;
        } else {
            thisMsg->TextAssociated = TRUE;
            thisMsg->DiscardShortMessage = FALSE;
        }

        if (text_p != NULL) {
            SMS_HEAP_FREE(&text_p);
        }
    } else {
        thisMsg->TextAssociated = TRUE;
        thisMsg->DiscardShortMessage = FALSE;
    }

    thisMsg->MWIList_Index = 1;

    return PCN_VOICE_MAIL;
}


/*************************************************************************************
*
* Section 10.3  This function checks the DCS and the associated short message for
*               voice message waiting indication. No line information is available
*               for GSM, always use line 1.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     getVoiceMailInfo_DCS
 *
 * @description  This function checks the DCS and the associated short message for
 *               voice message waiting indication. No line information is available
 *               for GSM, always use line 1
 *
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        thisMsg :   as type of MWIList_p.
 * @param        thisParse : as type of parse_p.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t getVoiceMailInfo_DCS(
    const TPDU_p thisTPDU,
    MWIList_p thisMsg,
    parse_p thisParse)
{
    uint8_t thisGroup;
    uint8_t isMsgWaiting;

    // cannot be DCS derived voice mail if TP_DCS not present
    if (!thisParse->size[TP_DCS]) {
        return DCS_NO_MESSAGE_WAITING;
    }

    thisGroup = thisDCS & DCS_CODING_GROUP_WAITING;

    if (!((thisGroup == DCS_MWI_WAIT_DISCARD)
          || (thisGroup == DCS_MSG_WAIT_COMPRESSED)
          || (thisGroup == DCS_MSG_WAIT_UNCOMPRESSED))) {
        return DCS_NO_MESSAGE_WAITING;
    }
    // Set the waiting message type
    thisMsg->MWI_Indication[0].Type = msgWaitingType((uint8_t) (thisDCS & 0x03), &isMsgWaiting);

    // Set the icon control flag
    if ((thisDCS & DCS_MWI_ACTIVE_BIT) == DCS_MWI_ACTIVE_BIT) {
        // Count is used to indicate the presence, or absence, of messages.  As there is no 'real' count value,
        // set it to 1.
        thisMsg->MWI_Indication[0].Count = 1;
    } else {
        thisMsg->MWI_Indication[0].Count = 0;
    }

    // Set the message text control flag
    if (NO_TEXT) {
        thisMsg->TextAssociated = FALSE;
    } else {
        thisMsg->TextAssociated = TRUE;
    }

    // Set the message discard flags
    // Note: Discard Message is defaulted to true, so don't need to it to TRUE
    if (thisGroup != DCS_MWI_WAIT_DISCARD) {
        thisMsg->DiscardShortMessage = FALSE;
    }

    thisMsg->MWIList_Index = 1;

    return isMsgWaiting;
}


/*************************************************************************************
*
* Section 10.4  This function converts the encoded waiting type into a system type and
*       sets the waiting flag appropriately
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     msgWaitingType
 *
 * @description  This function converts the encoded waiting type into a system
 *               type and sets the waiting flag appropriately
 *
 * @param        type :     uint8_t.
 * @param        *waiting : uint8_t.
 *
 * @return       static MWI_Type_t
 *
 * NOTE:         This function should not be used when handling PCN MWIs
 *               which can support VOICEMAIL_WAITING for LINE2.
 *
 *               bit4  bit3  bit2  bit1  bit0   uint8_t  Type
 *
 *               0     0     0     0     0      0      MSG_VOICE_MAIL_WAITING
 *               0     0     0     0     1      1      MSG_FAX_WAITING
 *               0     0     0     1     0      2      MSG_EMAIL_WAITING
 *               0     0     0     1     1      3      MSG_OTHER_MAIL_WAITING
 *               0     0     1     1     1      7      MSG_VIDEO_MAIL_WAITING
 *
 */
/********************************************************************/

static MWI_Type_t msgWaitingType(
    uint8_t type,
    uint8_t * waiting)
{
    MWI_Type_t waitingType = INVALID_MESSAGE_WAITING_TYPE;

    switch (type) {
    case DCS_VOICE_MAIL_WAITING:
        waitingType = MWI_VOICE_MAIL_LINE1;
        break;

    case DCS_FAX_WAITING:
        waitingType = MWI_FAX;
        break;

    case DCS_EMAIL_WAITING:
        waitingType = MWI_EMAIL;
        break;

    case DCS_VIDEO_MAIL_WAITING:
        waitingType = MWI_VIDEO_MAIL;
        break;

    default:
        waitingType = MWI_OTHER_MAIL;
        break;
    }

    *waiting = TRUE;

    return waitingType;
}

/*************************************************************************************
*
* Section 10.5  This function decodes the TP_OA field to see if an ORANGE voice mail
*       is indicated and returns either TRUE or FALSE accordingly.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     isOrangeVoiceMail
 *
 * @description  This function decodes the TP_OA field to see if an ORANGE voice
 *               mail is indicated and returns either TRUE or FALSE accordingly.
 *
 * @param        *thisOA_p : uint8_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/

static uint8_t isOrangeVoiceMail(
    uint8_t * thisOA_p)
{
    if ((*thisOA_p == PCN_VMWI_ADDRESS_LENGTH)
        && (*(thisOA_p + 1) == PCN_VMWI_TYPE_OF_ADDRESS)
        && ((*(thisOA_p + 2) & PCN_VMWI_ADDRESS_1_MASK) == PCN_VMWI_ADDRESS_1)
        && ((*(thisOA_p + 3) & PCN_VMWI_ADDRESS_2_MASK) == PCN_VMWI_ADDRESS_2)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*************************************************************************************
**
**  Section 11. TPDU Get Data Function
**
**  This function return (decoded) TPDU data. Attempts to read invalid fields return
**  a value of zero. The return data is right adjusted and where appropriate masked.
**
**  A list of valid data mnemonics is given in the header module "smtpdu.h" section 1.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_Get
 *
 * @description  Function to get TPDU data
 *
 * @param        thisData : as type of dataName_t.
 * @param        thisTPDU : as type of TPDU_p.
 * @param        typeTPDU : as type of ShortMessageType_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_Get(
    dataName_t thisData,
    TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU)
{
    uint8_t getData = 0;

    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    switch (thisData) {
    case ALPHABET:
        getData = thisParse->alphabet;
        break;

    case CLASS:
        getData = extractMessageClass(thisDCS);
        break;

    case ME_DATA_DOWNLOAD:
        if ((fieldValid(TP_PID, thisParse->type)) && (thisPID == TP_PID_ME_DATA_DOWNLOAD)) {
            getData = 0x01;
        }
        break;

    case ME_DEPERSONALIZATION:
        if (fieldValid(TP_PID, thisParse->type) && (thisPID == TP_PID_ME_DEPERSONALIZE_SM)) {
            getData = 0x01;
        }
        break;

    case REPLACEMENT_CODE:
        if ((thisPID >= TP_PID_REPLACE_SM_TYPE_1) && (thisPID <= TP_PID_REPLACE_SM_TYPE_7)) {
            getData = thisPID & 0x07;
        }
        break;

    case SIM_DOWNLOAD:
        if (fieldValid(TP_PID, thisParse->type) && (thisPID == TP_PID_SIM_DATA_DOWNLOAD)) {
            getData = 0x01;
        }
        break;

    case SIZE:
        getData = thisParse->length;
        break;

    case STATUS:
        getData = thisParse->parseStatus;
        break;

    case TRANSFER_PROTOCOL:
        getData = thisPID;
        break;

    default:
        break;
    }

    return getData;
}


/*************************************************************************************
*
* Section 11.1 Alphabet Extraction Function
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     extractAlphabetFromDCS
 *
 * @description  Function to extract Alphabet from DCS
 *
 * @param        tpduDCS : uint8_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t extractAlphabetFromDCS(
    uint8_t tpduDCS)
{
    uint8_t alphabet = ALPHABET_7_BIT;

    // The alphabet field is dependent upon the coding group
    switch (tpduDCS & DCS_CODING_GROUP_MASK) {
    case DCS_GEN_COMP_CODE:
    case DCS_GEN_UCOMP_CODE:
    case DCS_GEN_COMP_CODE_NO_CLASS:
    case DCS_GEN_UCOMP_CODE_NO_CLASS:
        alphabet = tpduDCS & 0x0C;      // The alphabet set by bits 2/3
        if (alphabet == 0x0C) {
            alphabet = ALPHABET_7_BIT;
        }
        break;

    case DCS_DATA_CODING_MSG_CLASS:
        if (tpduDCS & 0x04) {
            alphabet = ALPHABET_8_BIT;
        }
        break;

    case DCS_MWI_STORE_16BIT_MASK:
        alphabet = ALPHABET_16_BIT;
        break;

    default:
        break;
    }

    return alphabet;
}


/*************************************************************************************
*
* Section 11.2  Extract Message Class Function. This function returns the message
*       class as defined in GSM 03.38 version 5.3.0 July 1996 section 4.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     extractMessageClass
 *
 * @description  Function to extract message class function
 *
 * @param        tpduDCS : uint8_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t extractMessageClass(
    uint8_t tpduDCS)
{
    uint8_t messageClass = SM_CLASS_UNKNOWN;

    switch (tpduDCS & DCS_CODING_GROUP_MASK) {
    case DCS_GEN_COMP_CODE:
    case DCS_GEN_UCOMP_CODE:
    case DCS_DATA_CODING_MSG_CLASS:
    case DCS_AUTO_DELETE_COMP_CODE:
    case DCS_AUTO_DELETE_UCOMP_CODE:
        messageClass = DCSClassDecode[tpduDCS & DCS_CODING_CLASS_MASK];
        break;

    default:
        // No class information
        break;
    }

    return messageClass;
}



/*************************************************************************************
**
**  Section 12. TPDU bit functions. These functions enable the direct addressing of
**        bit fields using GSM 03.40 field names. There are three functions in
**        all enabling the extraction of the bit(s), the setting of the bit(s)
**        and the testing of the bit(s) to see if one or more have been set.
**
**        Note that returned bit fields are not right adjusted or checked to make
**        sure they are appropriate to the message type (unlike access by the
**        TPDU_GetField( ) function). Any attempt to access a none bit field will
**        cause an error to be flagged.
**
**************************************************************************************
*
* Section 12.1  This function tests the designated bit and returns the masked bits. If
*       an attempt to address a none bit field is made an error is flagged and
*       a zero return is made.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetBit
 *
 * @description  This function tests the designated bit and returns the masked bits.
 *
 * @param        thisField : as type of fieldName_t.
 * @param        thisTPDU :  as type of TPDU_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_GetBit(
    fieldName_t thisField,
    const TPDU_p thisTPDU)
{
    uint8_t rt = 0;

    if (BIT_FIELD(thisField)) {
        rt = thisTPDU[0] & bitMask(thisField);
    }

    return rt;
}


/*************************************************************************************
*
* Section 12.2  This function sets the designated bit. If the field is not a bit field
*       then an error is flagged.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_SetBit
 *
 * @description  Function to set the designated bit for TPDU data
 *
 * @param        thisField : as type of fieldName_t.
 * @param        thisTPDU :  as type of TPDU_p.
 * @param        newBit :    uint8_t.
 *
 * @return       void
 */
/********************************************************************/
void TPDU_SetBit(
    fieldName_t thisField,
    const TPDU_p thisTPDU,
    uint8_t newBit)
{
    if (BIT_FIELD(thisField)) {
        newBit &= bitMask(thisField);   // mask new data
        thisTPDU[0] &= ~bitMask(thisField);     // clear bits to be replaced
        thisTPDU[0] |= newBit;  // add the new bits
    }
}


/*************************************************************************************
*
* Section 12.3  This function tests the designated bit and return either TRUE or FALSE
*       If a none bit field is addressed the functions returns FALSE and an
*       error is flagged.
*
**************************************************************************************/


/********************************************************************/
/**
 *
 * @function     TPDU_TestBit
 *
 * @description  Function to test the designated bit
 *
 * @param        thisField : as  type of fieldName_t.
 * @param        thisTPDU : as   type of TPDU_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_TestBit(
    fieldName_t thisField,
    const TPDU_p thisTPDU)
{
    uint8_t rt = FALSE;

    if (BIT_FIELD(thisField)) {
        if (thisTPDU[0] & bitMask(thisField)) {
            rt = TRUE;
        }
    }

    return rt;
}


/*************************************************************************************
*
* Section 13. Concatenation Information Function.
*
* This function sets the number of concatination parts, the message, reference number
* and the sequence number in the passed concatination structure.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetConcatInfo
 *
 * @description  Function to get concatenation information in TPDU data
 *
 * @param        thisTPDU   : as type of TPDU_p.
 * @param        typeTPDU   : as type of ShortMessageType_t.
 * @param        thisConcat : as type of SMS_ConcatParameters_t.
 *
 * @return       void
 */
/********************************************************************/
void TPDU_GetConcatInfo(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    SMS_ConcatParameters_t * thisConcat)
{
    uint16_t msgRefNumber = 0;
    uint8_t *UDH_p;

    struct parse_t Parse;
    parse_p thisParse = &Parse; // Parse structure & pointer

    thisConcat->ConcatMessageRefNumber = 0;     // Default no concatenation
    thisConcat->IsConcatenated = FALSE; // Default no concatenation
    thisConcat->MsgSequenceNumber = 1;  // Default no concatenation
    thisConcat->MaxNumberOfConcatMsgs = 1;      // Default no concatenation

    if (USER_HEADER) {
        parseTPDU(thisTPDU, typeTPDU, thisParse);

        UDH_p = findUserDataIE(thisTPDU, thisParse, CONCAT_16BIT_REF, 0);

        if (UDH_p) {
            UDH_p += 2;
            msgRefNumber = 256 * (*UDH_p++);
            msgRefNumber += *UDH_p++;
        } else {
            UDH_p = findUserDataIE(thisTPDU, thisParse, CONCAT_8BIT_REF, 0);
            if (UDH_p) {
                UDH_p += 2;
                msgRefNumber = *UDH_p++;
            }
        }

        if (!UDH_p) {
            return;
        }

        thisConcat->ConcatMessageRefNumber = msgRefNumber;
        thisConcat->MaxNumberOfConcatMsgs = *UDH_p++;
        thisConcat->MsgSequenceNumber = *UDH_p++;

        if ((thisConcat->MaxNumberOfConcatMsgs > 1) && (thisConcat->MsgSequenceNumber >= 1) && (thisConcat->MsgSequenceNumber <= thisConcat->MaxNumberOfConcatMsgs)) {
            thisConcat->IsConcatenated = TRUE;
        } else {
            thisConcat->IsConcatenated = FALSE;
        }
    }
}


/*************************************************************************************
*
* Section 14. Addition of IEI block to UDH Function.
*
* This function adds an IEI data block to the User Data Header (UDH) and if necessary
* adjusts the data coding scheme (TP_DCS) to accord with the alphabet parameter.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_ResetUserDataIE
 *
 * @description  Function to reset the user data IE.
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        typeTPDU : as type of ShortMessageType_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_ResetUserDataIE(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU)
{
    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    thisTPDU[0] &= ~TPDU_UDHI_BIT;      // clear the TP_UDHI bit
    thisUDHL = 0;               // reset TP_UDHL to zero
    thisUDL = 0;                // new UDL size in octets

    // number of bytes in TPDU = bytes up to TP_UDHL as now reset
    thisParse->length = thisParse->index[TP_UDHL];

    return thisParse->length;
}

/*************************************************************************************
*
* Section 14. Addition of IEI block to UDH Function.
*
* This function adds an IEI data block to the User Data Header (UDH) and if necessary
* adjusts the data coding scheme (TP_DCS) to accord with the alphabet parameter.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_AddUserDataIE
 *
 * @description  Function to add an IEI data block to the user data header and if necessary
 *               adjusts the data coding scheme (TP_DCS) to accord with the alphabet parameter
 * @param        thisTPDU   : as type of TPDU_p.
 * @param        typeTPDU   : as type of ShortMessageType_t.
 * @param        alphabet   : uint8_t.
 * @param        typeIEI    : uint8_t.
 * @param        lengthIEI  : uint8_t.
 * @param        *dataIEI_p : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_AddUserDataIE(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t alphabet,
    uint8_t typeIEI,
    uint8_t lengthIEI,
    uint8_t * dataIEI_p)
{
    uint8_t *newIEI_p;

    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    if (alphabet != thisParse->alphabet) {
        setAlphabet(thisTPDU, thisParse, alphabet);
    }

    if (!thisUDL || !USER_HEADER) {
        thisTPDU[0] |= TPDU_UDHI_BIT;   // set the TP_UDHI bit
        thisUDHL = 0;           // set TP_UDHL to zero

        // If the PI is present set UDL bit. Also set when adding User Data text
        // but there may be a UDH with no following text so it must be set here
        // to cover this case.
        if (thisParse->size[TP_PI] > 0) {
            thisPI |= TPDU_PI_UDL_BIT;
        }
    }
    // This bit of code is included to make sure that the user data header fields are
    // reset if ever we ask for a concatenation IE to be added.
    if (typeIEI == 0x00 || typeIEI == 0x08) {
        thisTPDU[0] |= TPDU_UDHI_BIT;   // set the TP_UDHI bit
        thisUDHL = 0;           // set TP_UDHL to zero
    }
    // find start address of new IEI and update thisTPDU
    newIEI_p = thisTPDU + thisParse->index[TP_UDHL] + 1 + thisUDHL;
    *newIEI_p++ = typeIEI;
    *newIEI_p++ = lengthIEI;
    memcpy(newIEI_p, dataIEI_p, lengthIEI);

    thisUDHL += lengthIEI + 2;  // new user data header size

    thisParse->size[TP_UD] = thisUDHL + 1;      // size of TP_UD always octets

    if (thisParse->alphabet == ALPHABET_7_BIT) {
        thisUDL = (uint8_t) ((8 * (uint16_t) (thisParse->size[TP_UD]) + 6) / 7);        // new UDL size in septets
    } else {
        thisUDL = thisUDHL + 1; // new UDL size in octets
    }

    // number of bytes in TPDU = bytes up to and including TP_UDHL + bytes in UDH
    thisParse->length = thisParse->index[TP_UDHL] + 1 + thisUDHL;

    return thisParse->length;
}


/*************************************************************************************
*
* Section 14.1  Alphabet Setting Function. This function sets TP_DCS and thisParse->alphabet
*       to the designated alphabet. Where class conflicts occur the DCS is set
*       to a class 1 ME specific message with the new alphabet.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     setAlphabet
 *
 * @description  Function to set Alphabet.This function sets TP_DCS and
 *               thisParse->alphabet to the designated alphabet. Where class conflicts
 *               occur the DCS is set to a class 1 ME specific message with the new alphabet.
 *
 * @param        thisTPDU :    as type of TPDU_p.
 * @param        thisParse :   as type of parse_p.
 * @param        newAlphabet : uint8_t.
 *
 * @return       static void
 */
/********************************************************************/
static void setAlphabet(
    TPDU_p thisTPDU,
    parse_p thisParse,
    uint8_t newAlphabet)
{

    uint8_t defaultDCS = DCS_CLASS1_ME_SPECIFIC;        // class 1 with 7 bit alphabet

    thisParse->alphabet = newAlphabet;  // update parse information
    newAlphabet &= 0x0C;        // mask out any unwanted bits
    defaultDCS |= newAlphabet;  // default DCS with new alphabet

    // The alphabet is coding group dependent
    switch (thisDCS & DCS_CODING_GROUP_MASK) {
    case DCS_GEN_COMP_CODE:
    case DCS_GEN_UCOMP_CODE:
    case DCS_GEN_COMP_CODE_NO_CLASS:
    case DCS_GEN_UCOMP_CODE_NO_CLASS:
        thisDCS &= ~0x0C;       // Clear the alphabet field
        thisDCS |= newAlphabet; // and add the new bits
        break;

    case DCS_DATA_CODING_MSG_CLASS:
        if (newAlphabet == ALPHABET_8_BIT) {
            thisDCS |= 0x04;
        } else if (newAlphabet == ALPHABET_7_BIT) {
            thisDCS &= 0xFB;
        } else {
            thisDCS = defaultDCS;
        }
        break;

    case DCS_MWI_STORE_16BIT_MASK:
        if (newAlphabet != ALPHABET_16_BIT) {
            thisDCS = defaultDCS;
        }
        break;

    default:
        thisDCS = defaultDCS;
        break;
    }
}


/*************************************************************************************
*
* Section 15. Get Packed Address
*
* This function transfers an address to the designated location and returns the
* number of bytes in the address field. The actual address field depends upon the
* message type - there is never more than one address per message.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetPackedAddress
 *
 * @description  Function to get packed address from the TPDU.
 *               This function transfers an address to the designated location and
 *               returns the number of bytes in the address field. The actual address
 *               field depends upon the message type
 *
 * @param        thisTPDU : as type of TPDU_p.
 * @param        typeTPDU : as type of ShortMessageType_t.
 * @param        out_p    : as type of ShortMessagePackedAddress_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_GetPackedAddress(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    ShortMessagePackedAddress_t * out_p)
{
    uint8_t thisField;

    struct parse_t Parse;       // Create parse structure on stack
    parse_p thisParse = &Parse; // Structure pointer

    parseTPDU(thisTPDU, typeTPDU, thisParse);

    switch (thisParse->type) {
    case SM_COMMAND:
    case SM_SUBMIT:
        thisField = TP_DA;
        break;

    case SM_STATUS_REPORT:
        thisField = TP_RA;
        break;

    case SM_DELIVER:
        thisField = TP_OA;
        break;

    default:
        memset(out_p, 0, sizeof(ShortMessagePackedAddress_t));
        return 0;
    }

    memcpy(out_p, thisTPDU + thisParse->index[thisField], thisParse->size[thisField]);

    return thisParse->size[thisField];
}


/*************************************************************************************
**
**  Section 16. Get IE Block from User Data Header
**
**  This function copies the designated IE from the TP_UDH and returns the number of
**  bytes transferred. Failure to find the specified IE returns a zero. The IE is
**  defined by 'typeIE' and the specific instance of that type. If 'typeIE' equals
**  ANY_IE_TYPE then the absolute instance of the IE will be returned invarient of
**  the type.
**
**  Note a failure to find an IE does not result in an error being flagged.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetUserDataIE
 *
 * @description  Function to get user data IE from TPDU
 *
 * @param        thisTPDU   : as type of TPDU_p.
 * @param        typeTPDU   : as type of ShortMessageType_t.
 * @param        typeIE     : uint8_t.
 * @param        instanceIE : uint8_t.
 * @param        *dest_p    : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_GetUserDataIE(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint8_t typeIE,
    uint8_t instanceIE,
    uint8_t * dest_p)
{
    uint8_t lengthIE = 0;
    uint8_t *UDH_p;

    if (USER_HEADER) {
        struct parse_t Parse;   // Create parse structure on stack
        parse_p thisParse = &Parse;     // Parse structure pointer

        parseTPDU(thisTPDU, typeTPDU, thisParse);

        UDH_p = findUserDataIE(thisTPDU, thisParse, typeIE, instanceIE);

        if (UDH_p) {
            lengthIE = *(UDH_p + 1) + 2;
            memcpy(dest_p, UDH_p, lengthIE);
        }
    }

    return lengthIE;
}

/*************************************************************************************
*
* Section 16.1  Find IE in User Data Header
*       This function returns the address of the required IE in the User Data
*       Header if it exists, otherwise a NULL is returned. The IE is defined
*       by 'typeIE' and the specific instance of that type. If IE type is
*       ANY_IE_TYPE then the absolute instance of the IE will be found
*       independent of what IE types are involved.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     findUserDataIE
 *
 * @description  Function to find the user data IE by returning the address of the
 *               required IE in the user data header if it exists or NULL otherwise.
 *
 * @param        thisTPDU :     as type of TPDU_p.
 * @param        thisParse :    as type of parse_p.
 * @param        typeIE :       uint8_t.
 * @param        thisInstance : uint8_t.
 *
 * @return       static uint8_t*
 */
/********************************************************************/
static uint8_t *findUserDataIE(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    uint8_t typeIE,
    uint8_t thisInstance)
{
    uint8_t found = FALSE;
    uint8_t thisIEISize = 0;
    uint8_t thisIEIType = 0;
    uint8_t *UDH_p = NULL;
    uint8_t *endUDH_p = NULL;
    uint8_t *ReturnUDH_p = NULL;
    uint8_t instanceCount = 0;

    if (USER_HEADER && thisUDHL) {
        UDH_p = thisTPDU + thisParse->index[TP_UDL] + 2;
        endUDH_p = UDH_p + thisUDHL - 1;

        // parse the whole UDH to validate it rather than exiting when required IEI is found
        while (UDH_p < endUDH_p) {
            // endUDH_p points at the last byte of UDH data
            thisIEIType = *UDH_p;
            thisIEISize = *(UDH_p + 1); // get current IEI length

            if ((thisIEIType == typeIE) || (typeIE == ANY_IE_TYPE)) {
                if (instanceCount++ == thisInstance) {
                    found = TRUE;
                    ReturnUDH_p = UDH_p;
                }
            }
            UDH_p += thisIEISize + 2;   // start of next IEI block
        }

        // the UDH information is corrupt in some way - we didn't end where we should have (just past end of UDH),
        // so ignore the UDH
        if (UDH_p != endUDH_p + 1) {
            found = FALSE;
        }
    }

    if (found) {
        return ReturnUDH_p;
    } else {
        return NULL;
    }
}




/********************************************************************/
/**
 *
 * @function     CheckTPUserDataIntegrity
 *
 * @description  Function to check if any IE's data is outside it's valid range or
 *               the UDHL does not seem to match the IE's detected in the UserData array.
 *
 * @param        *UserData_p : uint8_t.
 * @param        UDHL_size :   uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t CheckTPUserDataIntegrity(
    const uint8_t * UserData_p,
    const uint8_t UDHL_size)
{

    uint16_t CharIndex = 0;
    uint8_t InconsistancyFound = FALSE, TestStatus = FALSE;
    uint16_t ObjectStartPos;


    // search the Data array until either:
    //   1. The end of the array is reached or,
    //   2. An inconsistancy is found

    CharIndex = 0;
    while (CharIndex < UDHL_size && !InconsistancyFound) {
        if (CheckValidObjectIEI(UserData_p[CharIndex])) // No! - now always accept
        {
            // all IEIs as valid valid object detected so look for the next one
            CharIndex += UserData_p[CharIndex + 1] + 2; // 2 allows for IEI & IEDL
        } else {
            // something has gone wrong
            InconsistancyFound = TRUE;
        }
    };

    // If we have a valid list of objects, examine each object for consistancy, else we have already "failed"
    if (CharIndex == UDHL_size && !InconsistancyFound) {
        // check each object's data is valid
        for (ObjectStartPos = 0; ObjectStartPos < UDHL_size && !TestStatus;) {
            TestStatus = CheckObjectDataIntegrity(&UserData_p[ObjectStartPos]);
            // Point to the next object.
            ObjectStartPos += UserData_p[ObjectStartPos + 1] + 2;       // 2 allows for IEI + IEDL
        }
    }

    return (TestStatus);
}                               // end of CheckTPUserDataIntegrity


/********************************************************************/
/**
 *
 * @function     CheckValidObjectIEI
 *
 * @description  Function to check if the object IEI is valid
 *
 * @param        ObjectIEI : as type of eSMS_IEI_codes_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t CheckValidObjectIEI(
    const eSMS_IEI_codes_t ObjectIEI)
{
    uint8_t ObjectValid = FALSE;

    switch (ObjectIEI) {
    case (CONCAT_SM_8BIT_REF):
    case (SPECIAL_SMS_INDICATION):
    case (APPLIC_PORT_ADDR_8BIT):
    case (APPLIC_PORT_ADDR_16BIT):
    case (CONCAT_SM_16BIT_REF):
    case (WAP_WCMP):
    case (TEXT_FORMATTING):
    case (PREDEFINED_SOUND):
    case (USER_DEFINED_SOUND):
    case (PREDEFINED_ANIMATION):
    case (LARGE_ANIMATION):
    case (SMALL_ANIMATION):
    case (LARGE_PICTURE):
    case (SMALL_PICTURE):
    case (VARIABLE_PICTURE):
    case (USER_PROMPT_INDICATOR):
    case (OBJECT_DISTRIBUTION_INDICATOR):
    case (EXTENDED_OBJECT):
    case (REUSED_EXTENDED_OBJECT):
        ObjectValid = TRUE;
        break;

    default:
        // all other values return false
        ObjectValid = FALSE;
        break;
    }

    return (ObjectValid);
}

/********************************************************************/
/**
 *
 * @function     GetStatusReportTPDU
 *
 * @description  Extracts up to 29bytes out of a received status report
 *               to be stored.  The fields are (TP-MTI, SRQ etc). TP-MR,
 *               TP-RA, TP-SCTS, TP-DT and TP_ST.  i.e TP-PI and any
 *               corresponding UD are not stored for status reports.
 *               See 3GPP TS 23.040, Section 9.2.2.3 for a breakdown
 *               of the status report TPDU.
 *
 * @param        *Object_p : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t GetStatusReportTPDU(
    const uint8_t * const SrcTPDU,
    uint8_t * const DestTPDU)
{
    uint8_t NumBytes = 0;
    uint8_t AddressLen = 0;

    NumBytes = TP_MR_SIZE + 1;  // Add 1 for 1st field containing MTI etc

    // The address length is the number of digits not bytes so calculate how
    // many bytes the address comprising (Address Len (1byte), Ton-NPI (1byte)
    // and the digits themselves occupy.
    // Num Bytes for digits + Length Byte + TonNpi Byte
    AddressLen = ((SrcTPDU[NumBytes] + 1) / 2) + 2;

    NumBytes += AddressLen + TP_SCTS_SIZE + TP_DT_SIZE + TP_ST_SIZE;

    memcpy(DestTPDU, SrcTPDU, NumBytes);

    return (NumBytes);
}

/********************************************************************/
/**
 *
 * @function     CheckObjectDataIntegrity
 *
 * @description  Function to check if the object's data is  consistant with
 *               that type of object or if any data field is outside its valid range
 *
 * @param        *Object_p : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t CheckObjectDataIntegrity(
    const uint8_t * Object_p)
{
    uint8_t ObjectIntegrityOK = FALSE;
    uint16_t destinationPortNumber, originatingPortNumber;

    switch (*Object_p)          // switch on the IEI for this object
    {
    case (TEXT_FORMATTING):
        if (*(Object_p + 1) == 3 && *(Object_p + 2) <= MAX_USER_DATA_LENGTH && *(Object_p + 3) <= MAX_USER_DATA_LENGTH) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (PREDEFINED_SOUND):
        if (*(Object_p + 1) == 2 && *(Object_p + 2) <= MAX_USER_DATA_LENGTH && *(Object_p + 3) <= 9) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (USER_DEFINED_SOUND):
        if (*(Object_p + 1) >= 2 && *(Object_p + 1) <= MAX_USER_DATA_LENGTH) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (PREDEFINED_ANIMATION):
        if (*(Object_p + 1) == 1 && *(Object_p + 2) <= 5) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (LARGE_ANIMATION):
        if (*(Object_p + 1) == 129 && *(Object_p + 2) <= MAX_USER_DATA_LENGTH) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (SMALL_ANIMATION):
        if (*(Object_p + 1) == 65 && *(Object_p + 2) <= MAX_USER_DATA_LENGTH) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (LARGE_PICTURE):
        if (*(Object_p + 1) == 129 && *(Object_p + 2) <= MAX_USER_DATA_LENGTH) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (SMALL_PICTURE):
        if (*(Object_p + 1) == 33 && *(Object_p + 2) <= MAX_USER_DATA_LENGTH) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (VARIABLE_PICTURE):
        if (*(Object_p + 1) >= 1 && *(Object_p + 1) <= 131 && *(Object_p + 2) <= MAX_USER_DATA_LENGTH && *(Object_p + 3) >= 1   //Min picture width (Bytes)
            && *(Object_p + 3) <= 32    //Max picture width (Bytes)
            && *(Object_p + 4) >= 1     //Min picture height (Pixels)
            && *(Object_p + 4) <= 128)  //Max picture height (Pixels)
        {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (APPLIC_PORT_ADDR_16BIT):
        // Take the value in the first octet of the 16-bit
        // ref number and multiply it by 256
        destinationPortNumber = *(Object_p + 1) * 256;
        // Now add the value found in the second octet
        destinationPortNumber += *(Object_p + 2);
        // Now do a similar decode for the Originating Port Number
        originatingPortNumber = *(Object_p + 3) * 256;
        originatingPortNumber += *(Object_p + 4);

        if ((destinationPortNumber == 2948) || (destinationPortNumber == 2949)) {
            ObjectIntegrityOK = TRUE;   // (WAP_MESSAGE)
        } else if (destinationPortNumber == 49999) {
            ObjectIntegrityOK = TRUE;   // (WAP_PROVISIONING_MESSAGE)
        } else if ((originatingPortNumber >= 9200) && (originatingPortNumber <= 9207)) {
            ObjectIntegrityOK = TRUE;   // (WAP_MESSAGE)
        }
        break;

    case (CONCAT_SM_8BIT_REF):
        if (*(Object_p + 1) == 3) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (CONCAT_SM_16BIT_REF):
        if (*(Object_p + 1) == 4) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (SPECIAL_SMS_INDICATION):
        if (*(Object_p + 1) == 2) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (REUSED_EXTENDED_OBJECT):
        if (*(Object_p + 1) == 3) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (USER_PROMPT_INDICATOR):
        if (*(Object_p + 1) == 1) {
            ObjectIntegrityOK = TRUE;
        }
        break;

    case (APPLIC_PORT_ADDR_8BIT):      // only data after the IEI so no further check possible
    case (WAP_WCMP):           // only data after the IEI so no further check possible
    default:
        ObjectIntegrityOK = TRUE;       // default to TRUE for all other types (?)
    }                           // end switch

    return (ObjectIntegrityOK);
}


#ifdef CODE_CURRENTLY_UNUSED    // Kept for future reference.
/********************************************************************/
/**
 *
 * @function     CheckEMSR5ObjectDataIntegrity
 *
 * @description  Function to check if the object's data is  consistant with
 *               that type of object or if any data field is outside its valid range
 *
 * @param        *Object_p : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t CheckEMSR5ObjectDataIntegrity(
    const uint8_t * Object_p)
{
    uint8_t ObjectIntegrityOK = FALSE;
    uint16_t ExtendedObjectLength, DataLength;
    uint8_t bitfields = 0;

    switch (*Object_p)          // switch on the IEI for this object
    {
    case (EXTENDED_OBJECT):
        {
            //for an extended object, we need to validate the object type
            //contained within it, hence switch on Octet 5, the extended Object Type
            ExtendedObjectLength = 256 * (*(Object_p + 2)) + (*(Object_p + 3));

            //Calculate number of bitfields for B&W / grey / colour objects
            switch (*(Object_p + 5)) {
            case (EXTENDED_BW_BITMAP):
            case (EXTENDED_BW_ANIM):
                bitfields = 1;
                break;

            case (EXTENDED_2_BIT_GREY_BITMAP):
            case (EXTENDED_2_BIT_GREY_ANIM):
                bitfields = 2;
                break;

            case (EXTENDED_6_BIT_COLOUR_BITMAP):
            case (EXTENDED_6_BIT_COLOUR_ANIM):
                bitfields = 6;
                break;

            default:
                break;
            }

            switch (*(Object_p + 5)) {
            case (EXTENDED_PREDEFINED_SOUND):
                //octet 8 = the predefined sound number
                if (ExtendedObjectLength == 1 && *(Object_p + 8) <= 9) {
                    ObjectIntegrityOK = TRUE;
                }
                break;

            case (EXTENDED_BW_BITMAP):
            case (EXTENDED_2_BIT_GREY_BITMAP):
            case (EXTENDED_6_BIT_COLOUR_BITMAP):
                // octet 8 = horizontal pixels
                // octet 9 = vertical pixels
                // octet 10.. = packed data, no fill bits except in final octet

                // Test if horizontal * vertical == the length of image data

                DataLength = bitfields * (*(Object_p + 8)) * (*(Object_p + 9)); // data length in bits
                DataLength = (DataLength + 7) / 8;      //data length in bytes, rounded up

                if (ExtendedObjectLength == (DataLength + 2))   //add 2 bytes for the x/y size in octets 8 and 9
                {
                    ObjectIntegrityOK = TRUE;
                }
                break;

            case (EXTENDED_PREDEFINED_ANIM):
                //octet 8 = the predefined animation number
                if (ExtendedObjectLength == 1 && *(Object_p + 8) <= 14) {
                    ObjectIntegrityOK = TRUE;
                }
                break;

            case (EXTENDED_BW_ANIM):
            case (EXTENDED_2_BIT_GREY_ANIM):
            case (EXTENDED_6_BIT_COLOUR_ANIM):
                // octet 8 = horizontal pixels
                // octet 9 = vertical pixels
                // octet 10 = no. of frames
                // octet 11 control byte
                // octet 12.. = packed data, fill bits at end of each frame octet
                //              so each frame starts on an octet boundary

                // Test if horizontal * vertical * number of frames == the length of image data

                DataLength = bitfields * (*(Object_p + 8)) * (*(Object_p + 9)); // frame length in bits
                DataLength = (DataLength + 7) / 8;      //frame length in bytes, rounded up
                DataLength = DataLength * (*(Object_p + 10));   // total image data length
                DataLength += 4;        //add in the x/y size, frame count and control byte

                if (ExtendedObjectLength == DataLength) {
                    ObjectIntegrityOK = TRUE;
                }
                break;

                //no further checking is done on the following types as their data is outside the scope
                //of messaging to understand
            case (EXTENDED_IMELODY):
            case (EXTENDED_VCARD):
            case (EXTENDED_VCALENDAR):
            case (EXTENDED_VECT_GRAPHIC):
            case (EXTENDED_POLY_MELODY):
            case (DATA_FORMAT_DELIVERY_REQUEST):
            default:
                ObjectIntegrityOK = TRUE;       // default to TRUE for all other types (?)
                break;
            }                   // end switch on object type
        }                       // end case (EXTENDED_OBJECT):

    case (REUSED_EXTENDED_OBJECT):
    default:
        break;
    }                           // end   switch (*Object_p) // switch on the IEI for this object

    return (ObjectIntegrityOK);
}
#endif                          // #ifdef CODE_CURRENTLY_UNUSED


/*************************************************************************************
**
**  Section 17. Get Application Port Addresses
**
**  This function searches the user data area for application port addresses (both 16
**  and 8 bit addresses) and returns then to the designated locations. If no application
**  ports are found then FALSE is returned otherwise TRUE is returned.
**
**  A failure to find an application port IE does not result in an error being flagged.
**
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     TPDU_GetApplicationPorts
 *
 * @description  Function to get application port address from TPDU by searching
 *               the user data area for application port addresses (both 16 and 8
 *               bit addresses) and returns then to the designated locations.
 *
 * @param        thisTPDU    : as type of TPDU_p.
 * @param        typeTPDU    : as type of ShortMessageType_t.
 * @param        *destPort_p : uint16_t.
 * @param        *origPort_p : uint16_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t TPDU_GetApplicationPorts(
    const TPDU_p thisTPDU,
    ShortMessageType_t typeTPDU,
    uint16_t * destPort_p,
    uint16_t * origPort_p)
{

    uint8_t found = FALSE;

    if (USER_HEADER) {
        struct parse_t Parse;   // Create parse structure on stack
        parse_p thisParse = &Parse;     // Parse structure pointer

        parseTPDU(thisTPDU, typeTPDU, thisParse);

        found = findApplicationPorts(thisTPDU, thisParse, destPort_p, origPort_p);

    }

    return found;
}


/*************************************************************************************
*
* Section 17.1  Find Application Port Addresses
*       This function searches the user data for any application port addresses.
*       A succsessful search returns TRUE otherwise a FALSE is returned. Note
*       the application ports are always sent as uint16_t values irrespective of
*       their original format.
*
**************************************************************************************/

/********************************************************************/
/**
 *
 * @function     findApplicationPorts
 *
 * @description  Function to find the application port addresses by searching
 *               the user data for any application port address.
 *
 * @param        thisTPDU :    as type of TPDU_p.
 * @param        thisParse :   as type of parse_p.
 * @param        *destPort_p : uint16_t.
 * @param        *origPort_p : uint16_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t findApplicationPorts(
    const TPDU_p thisTPDU,
    parse_p thisParse,
    uint16_t * destPort_p,
    uint16_t * origPort_p)
{

    uint8_t found = FALSE;
    uint8_t *UDH_p;

    UDH_p = findUserDataIE(thisTPDU, thisParse, APP_PORT_16_BIT_ADDR, 0);
    if (UDH_p++) {
        if (*UDH_p++ == APP_PORT_16_BIT_ADDR_LENGTH) {
            found = TRUE;       // 16 bit application port addresses found
            *destPort_p = (uint16_t) (256 * (*UDH_p++));
            *destPort_p += (uint16_t) * UDH_p++;
            *origPort_p = (uint16_t) (256 * (*UDH_p++));
            *origPort_p += (uint16_t) * UDH_p++;
        }
    } else {
        UDH_p = findUserDataIE(thisTPDU, thisParse, APP_PORT_8_BIT_ADDR, 0);
        if (UDH_p++) {
            if (*UDH_p++ == APP_PORT_8_BIT_ADDR_LENGTH) {
                found = TRUE;   // 8 bit application port addresses found
                *destPort_p = (uint16_t) * UDH_p++;
                *origPort_p = (uint16_t) * UDH_p++;
            }
        }
    }

    return found;
}

/************************ End of Source Module smtpdu.c *****************************/
