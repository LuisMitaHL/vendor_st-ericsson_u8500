#ifndef SM_H
#define SM_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
 * DESCRIPTION:
 *
 *  Defines the external types and function prototypes for the
 *  short message object.
 *
 *************************************************************************/

#include <time.h>

#include "t_smslinuxporting.h"
#include "t_sms.h"

//#include "t_rms.h"
//#include "t_smr_si.h"

#include "smtpdu.h"
#include "c_sms_config.h"

#define DATE_AND_TIME_LENGTH     (12)

#define PACKED_TIME_STAMP_LENGTH 7

//Constant definitions
#define ASCII_ZERO '0'
#define ASCII_CHAR_SIZE 2

#define MAX_TP_CHARACTERS ((MAX_TP_USER_DATA_LENGTH / 7) * 8)

// Length representation, used in pack/unpack addresses
#define ADDRESS_LENGTH_IN_DIGITS 0
#define ADDRESS_LENGTH_IN_BYTES  1

#define MSG_MAXIMUM_BYTES_IN_SM           (140)
#define MSG_MAXIMUM_OBJECTS_PER_SM        (35)

//Maximum Length of a Short Message Character + EMS buffer
//This arises because each object can potentially take an extra byte for expansion
//of the position information by the message manager. If we have a full message with
// the maximum number of objects then the size will expand as follows
#define MSG_MAX_SHORT_MESSAGE_LEN         (MSG_MAXIMUM_BYTES_IN_SM + MSG_MAXIMUM_OBJECTS_PER_SM)

#define MAX_MSG_ADDRESS_LENGTH            (10)  /* Digits */
#define MAX_DIGITS_IN_MSG_NUMBER          (MAX_MSG_ADDRESS_LENGTH * 2)

// Indication to the Short Message Bearer that no Deliver Report
// (i.e. TPDU) is to be sent
#define NO_DELIVERY_REPORT 0

#define RMS_RP_NO_CAUSE   0x00

typedef enum {
    EXTENDED_PREDEFINED_SOUND = 0x00,
    EXTENDED_IMELODY = 0x01,
    EXTENDED_BW_BITMAP = 0x02,
    EXTENDED_2_BIT_GREY_BITMAP = 0x03,
    EXTENDED_6_BIT_COLOUR_BITMAP = 0x04,
    EXTENDED_PREDEFINED_ANIM = 0x05,
    EXTENDED_BW_ANIM = 0x06,
    EXTENDED_2_BIT_GREY_ANIM = 0x07,
    EXTENDED_6_BIT_COLOUR_ANIM = 0x08,
    EXTENDED_VCARD = 0x09,
    EXTENDED_VCALENDAR = 0x0A,
    EXTENDED_VECT_GRAPHIC = 0x0B,
    EXTENDED_POLY_MELODY = 0x0C,
    // Reserved 0x0D - 0xFE
    DATA_FORMAT_DELIVERY_REQUEST = 0xFF,
    LAST_VALID_EXTENDED_IEI_VALUE
} eSMS_Extended_IEI_codes_t;

typedef enum {
    SCtoMS = 0,
    MStoSC,
    MAX_SM_MESSAGE_DIRECTION
} SmMessageDirection_t;

typedef enum {
    SM_STORE_NO_ERROR,
    SM_STORE_FAILED,
    SM_STORE_DATA_ERROR,
    SM_STORE_FILE_NOT_FOUND
} SmStoreErrorCode_t;


/**
 * Number of characters in the File System volume name
 */
#define SMS_STORAGE_MEDIA_LENGTH  (25)  // Android path /data/app-private/

/**
 * MSG_StorageMedia_t describes the media type a message is stored on, or shall be moved to.
 * Use "/sim" for SIM
 * For other storage media, use the string for the mounting-point.
 *
 * @param StorageMediaName  The storage media name
 */
typedef struct {
    char StorageMediaName[SMS_STORAGE_MEDIA_LENGTH + 1];        // number of chars + a NULL terminator
} MSG_StorageMedia_t;


#define FIRST_EMS_IEI        (TEXT_FORMATTING)
#define LAST_VALID_IEI_VALUE (REPLY_ADDRESS_ELEMENT)

// User Data Header length
#define CONCATENATED_8BIT_USER_DATA_HEADER_SIZE   5     // bytes
#define CONCATENATED_16BIT_USER_DATA_HEADER_SIZE  6     // bytes

typedef uint8_t SMS_DataCodingScheme_t;

#define ALPHABET_7_BIT   0x00
#define ALPHABET_8_BIT   0x04
#define ALPHABET_16_BIT  0x08

// values for the text coding of the Short message
#define SM_DEFAULT_CODING  0    // Using GSM 7 bit default alphabet
#define SM_8BIT_CODING     1    // Using 8 bit data, currently not supprted in the phone.
#define SM_UCS2_CODING     2    // Using UCS2 coding

typedef enum {
    DATA_CODING_7_BITS,
    DATA_CODING_8_BITS,
    DATA_CODING_16_BITS
} DataCoding_t;

//Maximum Number of Characters in a Short Message (depends on alphabet)
#define MSG_DEFAULT_ALPHABET_MAX_SHORT_MESSAGE_CHARACTERS   (160)
#define MSG_EIGHTBIT_ALPHABET_MAX_SHORT_MESSAGE_CHARACTERS  (140)
#define MSG_UNICODE_ALPHABET_MAX_SHORT_MESSAGE_CHARACTERS    (70)

#define MAX_USER_DATA_LENGTH  (160)

/*
 * SHORT MESSAGE TYPE DEFINITIONS
 */
typedef uint32_t SMID_t;

/*
 * Definition of Short Message Handle
 */
typedef struct ShortMessageObject *ShortMessage_p;

/*
 * Short Message Direction
 */
typedef enum {
    SMDIRECTION_MO,
    SMDIRECTION_MT,
    SMDIRECTION_LAST
} SMDirection_t;

typedef enum {
    STATUS_REPORT_NOT_OK,       //not delivered (e.g. validity period expired)
    STATUS_REPORT_OK,           //delivered
    STATUS_REPORT_STILL_TRYING, //still in service centre
    STATUS_REPORT_UNKNOWN,      //unknown indication received from the network
    STATUS_REPORT_RESET         // used to indicate an initialised location (e.g. after SIM Change)
        // or when Status Report Has not been requested.
        //NOTE: STATUS_REPORT_RESET must stay as highest value in enum
} DeliveryStatus_t;

typedef enum {
    STATE_DISABLED,             // Radio link not being preserved by MoreToSend
    STATE_OPENING,              // MoreToSend opening persistent radio link
    STATE_OPENED,               // Radio link opened and being preserved
    STATE_CLOSING               // Radio link to be closed
} MoreToSendState_t;

#define SMS_TIME_STAMP_LENGTH               (7)
#define SMS_TIME_STAMP_TIME_ZONE_POSITION   (6)

typedef unsigned char TimeStamp_t[SMS_TIME_STAMP_LENGTH];

typedef struct {
    time_t Time;
    int8_t Zone;
} ShortMessageTimeStamp_t;

typedef SMS_ConcatParameters_t *ShortMessageConcatParams_p;

typedef struct {
    uint16_t DestinationPort;
    uint16_t OriginatorPort;
    uint8_t UseApplicationPorts;
} ShortMessageApplicationPorts_t;

typedef ShortMessageApplicationPorts_t *ShortMessageApplicationPorts_p;

typedef enum {
    SMTYPE_STANDARD = 0x00,
    SMTYPE_FAX_GROUP3 = 0x22,
    SMTYPE_FAX_GROUP4 = 0x23,
    SMTYPE_VOICE = 0x24,
    SMTYPE_ERMES = 0x25,
    SMTYPE_NATIONAL_PAGING = 0x26,
    SMTYPE_EMAIL = 0x32
} SMTransferProtocol_t;


typedef uint8_t UnpackedTimeStamp_t[DATE_AND_TIME_LENGTH];      // 12 bytes
typedef uint8_t PackedTimeStamp_t[SMS_TIME_STAMP_LENGTH];       //  7 bytes

// Remove FSU_VolumeStatusInformation_t. Create SMS internal version.
// This is an enum in old code but maybe not all values are needed here?
typedef int8_t SMS_FSU_VolumeStatusInformation_t;



//===================================================================================
// Short Message Object 'public' operations
//===================================================================================

extern void ShortMessage_Initialise(
    void);

void ShortMessage_Deinitialise(
    void);

ShortMessage_p ShortMessage_Create(
    ShortMessageType_t);

void ShortMessage_Reinitialise(
    void);

void ShortMessage_Delete(
    ShortMessage_p);

void ShortMessage_DeletePart(
    ShortMessage_p,
    uint8_t);

void ShortMessage_Print(
    ShortMessage_p);
void ShortMessage_HexPrint(
    ShortMessagePackedAddress_t * SC_Address_p,
    SMS_TPDU_t * UserData_p);

uint8_t ShortMessage_isKeep_MWIs_Present(
    MWIList_t *);


// Initialising Short Message object attributes
//----------------------------------------------------------------------------------
void ShortMessage_SetPackedServiceCentreAddress(
    ShortMessage_p const,
    ShortMessagePackedAddress_t *);
void ShortMessage_SetPackedDestinationAddress(
    ShortMessage_p const,
    ShortMessagePackedAddress_t *);

void ShortMessage_SetUnPackedServiceCentreAddress(
    ShortMessage_p const,
    ShortMessageSubscriberNumber_t *);

void ShortMessage_SetTPDU(
    ShortMessage_p const,
    Tpdu_t,
    uint8_t const);
uint8_t ShortMessage_GetTpdu(
    ShortMessage_p,
    uint8_t *,
    Tpdu_t);

void ShortMessage_SetStoragePosition(
    ShortMessage_p,
    SMS_Position_t);

void ShortMessage_SetStatusReportRecNum(
    ShortMessage_p,
    uint8_t const);

uint8_t ShortMessage_SetStatus(
    ShortMessage_p const,
    SMS_Status_t const);

void ShortMessage_SetMessageReference(
    ShortMessage_p const,
    uint8_t);
uint8_t ShortMessage_GetMessageReference(
    ShortMessage_p const,
    uint8_t *);

void ShortMessage_SetReplyRequested(
    ShortMessage_p,
    uint8_t);
void ShortMessage_SetAcknowledgeRequired(
    ShortMessage_p,
    uint8_t);

void ShortMessage_SetDeliveredStatus(
    ShortMessage_p const,
    DeliveryStatus_t);
void ShortMessage_SetCacheDeliveryStatus(
    ShortMessage_p const,
    DeliveryStatus_t);

void ShortMessage_SetStatusReportRequest(
    ShortMessage_p const,
    uint8_t);

void ShortMessage_GetSequenceNumber(
    ShortMessage_p messagePart,
    uint8_t * SequenceNumber);
void ShortMessage_SetSequenceNumber(
    ShortMessage_p messagePart);

uint8_t ShortMessage_PackTPDUHeader(
    ShortMessage_p,
    TP_FailureCause_t,          // Failure cause
    uint8_t *,                  // Protocol Identifier
    SMS_DataCodingScheme_t *,   // Data Coding Scheme
    uint8_t,                    // Status Report requested
    uint8_t,                    // Reply path set
    uint8_t *,                  // Validity Period
    ShortMessageSubscriberNumber_t *,   // Destination address
    uint8_t *,                  // Command type
    uint8_t *);                 // Message Ref Number

void ShortMessage_ResetUserDataHeader(
    ShortMessage_p thisShortMessage);
void ShortMessage_AddIEToUserDataHeader(
    ShortMessage_p,
    uint8_t alphabet,
    uint8_t IEType,
    uint8_t IESize,
    uint8_t * IEDate);

uint8_t ShortMessage_PackMessageBody(
    ShortMessage_p,
    uint8_t textLength,
    uint8_t alphabet,
    uint8_t * text);

uint8_t ShortMessage_CopyMessageBody(
    ShortMessage_p,
    uint8_t,                    // text length
    uint8_t,                    // text alphabet
    uint8_t *);                 // text buffer

void ShortMessage_SetMoreToSend(
    const ShortMessage_p,
    const MoreToSendState_t);
MoreToSendState_t ShortMessage_GetMoreToSend(
    const ShortMessage_p);


// Getting the values of the short message attributes
//----------------------------------------------------------------------------------
uint8_t ShortMessage_Exists(
    ShortMessage_p theShortMessage);

void ShortMessage_GetPackedServiceCentreAddress(
    ShortMessage_p const,
    ShortMessagePackedAddress_t *);
void ShortMessage_GetPackedOrigOrDestAddress(
    ShortMessage_p,
    ShortMessagePackedAddress_t *);

void ShortMessage_GetUnpackedServiceCentreAddress(
    ShortMessage_p,
    uint8_t *,
    SMS_TypeOfNumber_t *,
    SMS_NumberingPlanId_t *);
void ShortMessage_GetUnpackedOrigOrDestAddress(
    ShortMessage_p,
    uint8_t *,
    uint8_t *,
    uint8_t *);

void ShortMessage_GetPackedTimeStamp(
    ShortMessage_p,
    PackedTimeStamp_t);

SMS_Status_t ShortMessage_GetStatus(
    ShortMessage_p const);

SMS_Position_t ShortMessage_GetStoragePosition(
    ShortMessage_p);

uint8_t ShortMessage_GetStatusReportRecNum(
    ShortMessage_p);

SMS_Category_t ShortMessage_GetCategoryFromClass(
    ShortMessage_p);

uint8_t ShortMessage_IsMessageWaitingIndication(
    ShortMessage_p,
    MWIList_p);

uint8_t ShortMessage_IsDepersonalisation(
    ShortMessage_p);

uint8_t ShortMessage_IsSIMDataDownload(
    ShortMessage_p);

uint8_t ShortMessage_IsMEDataDownload(
    ShortMessage_p);

uint8_t ShortMessage_IsEnhanced(
    ShortMessage_p);

uint8_t ShortMessage_IsExtendedObjectDataReqCmd(
    ShortMessage_p thisShortMessage,
    uint8_t * const DataPresent_p);

uint8_t ShortMessage_IsStatusReportMatch(
    const ShortMessage_p shortMessage,
    const ShortMessage_p newStatusReport);

void ShortMessage_IsConcatenated(
    ShortMessage_p,
    ShortMessageConcatParams_p);

uint8_t ShortMessage_GetText(
    ShortMessage_p,
    uint8_t *);
uint8_t ShortMessage_GetAlphabet(
    ShortMessage_p);
uint8_t ShortMessage_GetDataCodingScheme(
    ShortMessage_p);
SMClass_t ShortMessage_GetMessageClass(
    ShortMessage_p);
uint8_t ShortMessage_GetProtocolIdentifier(
    ShortMessage_p);
SMTransferProtocol_t ShortMessage_GetTransferProtocol(
    ShortMessage_p);
uint8_t ShortMessage_IsUDHISet(
    ShortMessage_p thisShortMessage);
uint8_t ShortMessage_HasIEInRange(
    ShortMessage_p thisShortMessage,
    uint8_t minIE,
    uint8_t maxIE);
uint8_t ShortMessage_ShortMessageTypeMatch(
    SMS_Status_t shortmessageTypeOne,
    SMS_Status_t shortmessageTypeTwo);
SMS_Status_t ShortMessage_GetSlotType(
    ShortMessage_p shortMessage);
uint8_t ShortMessage_ContainsIEType(
    ShortMessage_p fThisShortMessage,
    const uint8_t fIEType);

void ShortMessage_GetApplicationPorts(
    ShortMessage_p,
    ShortMessageApplicationPorts_p);

SmMessageDirection_t ShortMessage_GetMessageDirection(
    ShortMessage_p);

ShortMessageType_t ShortMessage_GetType(
    ShortMessage_p);
uint8_t ShortMessage_IsReplyRequested(
    ShortMessage_p);
uint8_t ShortMessage_GetAcknowledgeRequired(
    ShortMessage_p);

DeliveryStatus_t ShortMessage_GetDeliveredStatus(
    ShortMessage_p const);
DeliveryStatus_t ShortMessage_GetDeliveryStatus(
    ShortMessage_p const);

uint8_t ShortMessage_IsStatusReportRequested(
    ShortMessage_p const);

uint8_t ShortMessage_GetValidityPeriod(
    ShortMessage_p const);
uint8_t ShortMessage_GetReplaceType(
    ShortMessage_p thisShortMessage);

void ShortMessage_GetStorageTime(
    ShortMessage_p theShortMessage,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p);

uint8_t ShortMessage_GetObject(
    ShortMessage_p,
    uint8_t * dst,
    uint8_t objInstance);

uint8_t ShortMessage_GetIE(
    ShortMessage_p thisShortMessage,
    uint8_t * buffer_p,
    uint8_t objectNumber);


//TX/RX functions
//----------------------------------------------------------------------------------
typedef void (
    *CallbackFunction_p) (
    ShortMessage_p,
    SMS_Error_t);
void ShortMessage_Send(
    ShortMessage_p,
    CallbackFunction_p);

void ShortMessage_Acknowledge(
    ShortMessage_p,
    TP_FailureCause_t);
SMS_Error_t ShortMessage_AcknowledgementWithConfirm(
    ShortMessage_p,
    const SMS_RP_ErrorCause_t);


//Utility functions
//----------------------------------------------------------------------------------
void UnpackAddressField(
    ShortMessagePackedAddress_t *,
    ShortMessageSubscriberNumber_t *);
void UnpackServiceCentreAddress(
    ShortMessagePackedAddress_t *,
    ShortMessageSubscriberNumber_t *);
void PackServiceCentreAddress(
    ShortMessageSubscriberNumber_t *,
    ShortMessagePackedAddress_t *);
void ShortMessage_PackTimeStamp(
    uint8_t * unpackedTime_p,
    uint8_t * packedTime_p);

DeliveryStatus_t TPStatusToStatus(
    uint8_t status);

uint8_t AlphabetToDCS(
    uint8_t alphabet);

uint8_t ShortMessage_OrigDestAddressCompare(
    ShortMessage_p,
    ShortMessage_p);
uint8_t ShortMessage_ServiceCentreAddressCompare(
    ShortMessage_p,
    ShortMessage_p);
uint8_t ShortMessage_PackedTimeStampCompare(
    ShortMessage_p shortMessageOne,
    ShortMessage_p shortMessageTwo);

uint8_t ShortMessage_DeleteDuplicate(
    SMS_Position_t SlotNumber);


// Storage functions
//----------------------------------------------------------------------------------
uint8_t ShortMessage_IsStored(
    ShortMessage_p);
SmStoreErrorCode_t ShortMessage_Store(
    ShortMessage_p);
SmStoreErrorCode_t ShortMessage_StoreME(
    ShortMessage_p);
SmStoreErrorCode_t ShortMessage_StoreMedia(
    ShortMessage_p const thisShortMessage,
    MSG_StorageMedia_t * Volume_p);
uint8_t ShortMessage_Move(
    ShortMessage_p const thisShortMessage,
    MSG_StorageMedia_t * Volume_p);

ShortMessage_p ShortMessage_CreateAndStoreFromTpdu(
    SMS_SMSC_Address_TPDU_t * MsgTpdu_p,
    MSG_StorageMedia_t * Volume_p);
ShortMessage_p ShortMessage_CreateForStorage(
    const SMS_SMSC_Address_TPDU_t * const MsgTpdu_p);
uint8_t ShortMessage_AllocateTPDUStorage(
    ShortMessage_p theShortMessage);
void ShortMessage_DeallocateTPDUStorage(
    ShortMessage_p theShortMessage);

ShortMessage_p ShortMessage_CreateAndStoreInSlot(
    const SMS_SMSC_Address_TPDU_t * const MsgTpdu_p,
    const MSG_StorageMedia_t * const Volume_p,
    const SMS_Position_t SlotNumber);

#endif                          // SM_H
