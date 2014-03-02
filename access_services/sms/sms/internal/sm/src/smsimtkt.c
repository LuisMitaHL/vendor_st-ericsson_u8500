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
 *  Short Message SimToolkit Object.
 *
 *************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"


/* The Message Transport Server Interface */
#include "g_sms.h"
#include "r_sms.h"

/* Dependant Modules */
#include "sim.h"

/* Message Module Include Files */
#include "sm.h"
#include "smstrg.h"
#include "smsevnthndr.h"
#include "smsimtkt.h"
#include "smutil.h"
#include "smsmain.h"
#include "c_sms_config.h"


#define SMS_SAT_MAX_SIZE_SMS_TPDU 165
#define SMS_SAT_MAX_SIZE_CB_APDU               128

typedef struct {
    uint8_t TPDU_Length;
    uint8_t TPDU[SMS_SAT_MAX_SIZE_SMS_TPDU];
} SMS_SAT_TP_UserData_t;

// Device Identities
#define DEVICE_SIM                             0x81     // 129
#define DEVICE_NETWORK                         0x83     // 131

// Constants for unpacking/packing
#define MAX_APDU_ENVELOPE_DATA_LENGTH          0xFF     // 255
#define MAX_LENGTH_BYTE1                       0x7F     // 127
#define TWO_BYTES_LENGTH_IND                   0x81     // 129
#define COMPREHENSION_REQUIRED_MASK            0x80     // 128

// SIMPLE-TLV tags
#define DEVICE_IDENTITIES_TAG                  0x02
#define ADDRESS_TAG                            0x06
#define SMS_TPDU_TAG                           0x0B     // 11

// NPI TON bit decoding constants
#define NPI_MASK                               0x0F     // 15       // Masks NPI bits out of a byte
#define TON_MASK                               0x70     // 112      // Masks TON bits out of a byte
#define TON_LSB_POS                            4        // used to move bits into position
#define NPI_TON_FIXED_BITS                     0x80     // 128      // Bit8 should always be set

// Response TDPU
#define STATUS_WORDS_SIZE                      2        // ETSI 102 223 8.36
#define STATUS_WORD1_OFFSET_FROM_END           2
#define STATUS_WORD2_OFFSET_FROM_END           1
#define R_TPDU_MIN_SIZE                        STATUS_WORDS_SIZE

typedef enum {
    SIMTKT_IDLE = 1
} SIMTKT_State_t;

typedef struct SIMTKTSystemObject {
    SIMTKT_State_t SIMTKT_State;
    uint8_t StoredProtocolIdentifier;
    uint8_t StoredDataCodingScheme;
    uint8_t StoredAlphabet;
    uint8_t SMS_DownloadActivated;
} SIMTKTSystem_t;

typedef struct {
    uint8_t NoOfBytes;          // Number of bytes for dialled string
    void *Text_p;               // Dialled string
    SMS_TypeOfNumber_t TON;     // Type of number
    SMS_NumberingPlanId_t NPI;  // Numbering plan ID
} SMS_SAT_DialledString_t;

// SMS SAT Event call back data, used to build delivery report
typedef struct {
    uint8_t Status;
    uint8_t StatusWord1;
    uint8_t StatusWord2;
    uint8_t APDULength;
    uint8_t *APDU_p;
} SMS_SAT_EnvelopeCB_t;

//Local Prototypes

//Local Event Handler Functions
static void HandleSIMTKT_Message(
    void *,
    EventData_p);
static void HandleSIMTKT_DeliveryReport(
    void *,
    EventData_p);

//Message Event, State & Handler Table
static EventAction_t SIMTKT_StateEventTable[3] = {
    {SIMTKT_NEW_SHORT_MESSAGE, SIMTKT_IDLE, HandleSIMTKT_Message},
    {SIMTKT_BUILD_DELIVERY_REPORT, SIMTKT_IDLE, HandleSIMTKT_DeliveryReport},
    {END_OF_TABLE, END_OF_TABLE, END_OF_TABLE_FUNCTION}
};

//This object is static
static SIMTKTSystem_t thisSIMTKTSystem = {
    SIMTKT_IDLE,                // SIMTKT_State
    0,                          // StoredProtocolIdentifier
    0,                          // StoredDataCodingScheme
    0,                          // StoredAlphabet
    0                           // SMS_DownloadActivated
};

SIMTKTSystem_p theSIMTKTSystem = &thisSIMTKTSystem;

static SMS_SAT_EnvelopeCB_t EnvelopeCB = {
    0,                          // CB Status
    0x00,                       // CB Status Word 1
    0x00,                       // CB Status Word 2
    0,                          // CB APDU Length
    NULL                        // CB APDU Data
};

static uint16_t SIMTKT_PP_GetDownloadLength(
    const uint8_t,
    const uint8_t);

static uint8_t SIMTKT_PP_BuildSimpleTLV_DeviceIdentities(
    const uint8_t,
    const uint8_t,
    const uint8_t,
    char *);

static uint8_t SIMTKT_PP_NPI_TON_ToADN_Byte(
    const ste_sim_numbering_plan_id_t,
    const ste_sim_type_of_number_t);

static uint8_t SIMTKT_PP_SendDownloadEnvelope(
    const SMS_SAT_TP_UserData_t * const,
    const SMS_SAT_DialledString_t * const);


/********************************************************************/
/**
 *
 * @function     SIMTKT_HandleEvent
 *
 * @description  Function to handle SIMTKT event
 *
 * @param        event :     as type of SIMTKT_Event_t.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void SIMTKT_HandleEvent(
    SIMTKT_Event_t event,
    EventData_p eventData)
{
    EventHandlerFunction_p eventHandler;

#ifdef SMS_PRINT_B_
    char *event_p = NULL;
    char *state_p = NULL;
    switch (event) {
    case SIMTKT_NEW_SHORT_MESSAGE:
        event_p = "SIMTKT_NEW_SHORT_MESSAGE";
        break;
    case SIMTKT_BUILD_DELIVERY_REPORT:
        event_p = "SIMTKT_BUILD_DELIVERY_REPORT";
        break;
    default:
        event_p = "UNKNOWN EVENT";
        break;
    }
    switch (thisSIMTKTSystem.SIMTKT_State) {
    case SIMTKT_IDLE:
        state_p = "SIMTKT_IDLE";
        break;
    default:
        state_p = "UNKNOWN STATE";
        break;
    }
    SMS_LOG_D("smsimtkt.c: SIMTKT : %s in %s", event_p, state_p);
#endif

    eventHandler = EventHandler_Find(theSIMTKTSystem, SIMTKT_StateEventTable, event, theSIMTKTSystem->SIMTKT_State);

    (eventHandler) (theSIMTKTSystem, eventData);
}


/********************************************************************/
/**
 *
 * @function     SIMTKT_SetEnvelopeData
 *
 * @description  Stores the SMS SAT Event call back data.
 *
 * @param        EnvelopeDataLength : Envelope data length
 * @param        EnvelopeData_p     : Envelope data
 *
 * @return       None
 *
 */
/********************************************************************/
void SIMTKT_SetEnvelopeData(
    uint8_t EnvelopeDataLength,
    char *EnvelopeData_p)
{
    SMS_A_(SMS_LOG_I("smsimtkt.c: SIMTKT_SetEnvelopeData"));

    if (EnvelopeDataLength >= R_TPDU_MIN_SIZE && EnvelopeData_p != NULL) {
        EnvelopeCB.Status = TRUE;
        EnvelopeCB.APDULength = EnvelopeDataLength - STATUS_WORDS_SIZE;

        // Status words are last two bytes of data.
        EnvelopeCB.StatusWord1 = (uint8_t) EnvelopeData_p[EnvelopeDataLength - STATUS_WORD1_OFFSET_FROM_END];
        EnvelopeCB.StatusWord2 = (uint8_t) EnvelopeData_p[EnvelopeDataLength - STATUS_WORD2_OFFSET_FROM_END];

        if (EnvelopeCB.APDULength > SMS_SAT_MAX_SIZE_CB_APDU) {
            EnvelopeCB.APDULength = SMS_SAT_MAX_SIZE_CB_APDU;
        }

        if (EnvelopeCB.APDU_p != NULL) {
            SMS_HEAP_FREE(&EnvelopeCB.APDU_p);
            EnvelopeCB.APDU_p = NULL;
        }

        EnvelopeCB.APDU_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(uint8_t) * (SMS_SAT_MAX_SIZE_CB_APDU));

        if (EnvelopeCB.APDU_p != NULL) {
            memset(EnvelopeCB.APDU_p, 0x00, (SMS_SAT_MAX_SIZE_CB_APDU) * (sizeof(uint8_t)));
            memcpy(EnvelopeCB.APDU_p, (uint8_t *) EnvelopeData_p, EnvelopeCB.APDULength);
        } else {
            SMS_A_(SMS_LOG_E("smsimtkt.c: SIMTKT_SetEnvelopeData: Memory allocation for APDU CB Envelope FAILED"));
            EnvelopeCB.Status = FALSE;
        }
    }
}                               /* SIMTKT_SetEnvelopeData */

/********************************************************************/
/**
 *
 * @function     SIMTKT_GetEnvelopeStatus
 *
 * @description  Retrieves the current status SMS SAT Event call back data.
 *
 * @param        EnvelopeCBData_p : Envelope call back data
 *
 * @return       TRUE is SMS SAT Event call back data is present;
 *               FALSE otherwise
 */
/********************************************************************/
uint8_t SIMTKT_GetEnvelopeStatus(
    void)
{
    SMS_B_(SMS_LOG_D("smsimtkt.c: SIMTKT_GetEnvelopeStatus"));

    if (EnvelopeCB.Status == TRUE) {
        EnvelopeCB.Status = FALSE;
        SMS_A_(SMS_LOG_I("smsimtkt.c: SIMTKT_GetEnvelopeStatus: Envelope CB valid"));

        return (TRUE);
    }

    return (FALSE);
}                               /* SIMTKT_GetEnvelopeStatus */

/********************************************************************/
/**
 *
 * @function     SIMTKT_PP_GetDownloadLength
 *
 * @description  Determine if the length of the SMS_PP_Download envelope
 *               is contained in 1 or 2 bytes.
 *
 * @param        TPDU_DataLength :  Length of the SMS element TS 11.14 12.13
 * @param        SCA_Length :       Length of the SCA element TS 11.14 12.1
 *
 * @return       Required length of PP download buffer.
 */
/********************************************************************/
static uint16_t SIMTKT_PP_GetDownloadLength(
    const uint8_t TPDU_DataLength,
    const uint8_t SCA_Length)
{
    uint8_t TPDU_Length = 0;
    uint16_t PP_Envelope_Length = 0;

    SMS_C_(SMS_LOG_V("smsimtkt.c: SIMTKT_PP_GetDownloadLength: TPDU_DataLength=%d, SCA_Length=%d", TPDU_DataLength, SCA_Length));

    /* Add in SMS_TPDU length TS 11.14 12.13 */
    /* Add on tag length to TPDU size */
    TPDU_Length = TPDU_DataLength + 1;

    /* One size byte is always included */
    PP_Envelope_Length = TPDU_Length + 1;

    if (TPDU_DataLength > MAX_LENGTH_BYTE1) {
        /* Add additional size byte */
        PP_Envelope_Length += 1;
    }

    /* Add in Address length TS 11.14 12.1 */
    /* Add on tag length + TON/NPI length + one size byte + SCA length */
    PP_Envelope_Length += SCA_Length + 3;

    if (SCA_Length > MAX_LENGTH_BYTE1) {
        /* Add additional size byte */
        PP_Envelope_Length += 1;
    }

    /* Add in Device Identities Length TS 11.14 12.7 */
    PP_Envelope_Length += 4;

    return (PP_Envelope_Length);
}                               /* SIMTKT_PP_GetDownloadLength */

/********************************************************************/
/**
 *
 * @function     SIMTKT_PP_BuildSimpleTLV_DeviceIdentities
 *
 * @description  Builds the Device Identities simple TLV.
 *
 * @param        SourceDevice :    Source device ID
 * @param        DestDevice :      Destination device ID
 * @param        CompReq :         Comprehension Required
 *
 * @output param Dest_p :          Pointer to byte array to which TLV will
 *                                 be added
 *
 * @return       Size of Device Identities TLV.
 */
/********************************************************************/
static uint8_t SIMTKT_PP_BuildSimpleTLV_DeviceIdentities(
    const uint8_t SourceDevice,
    const uint8_t DestDevice,
    const uint8_t CompReq,
    char *Dest_p)
{
#define DEVICE_IDENTITIES_TOTAL_SIZE  4 // Ref. TS 31.111 section 8.7
#define DEVICE_IDENTITIES_LENGTH      2 // Ref. TS 31.111 section 8.7

    uint8_t Tag = DEVICE_IDENTITIES_TAG;

    if (CompReq == TRUE) {      // Comprehension Required
        Tag |= COMPREHENSION_REQUIRED_MASK;
    }

    *Dest_p++ = Tag;            // TAG

    *Dest_p++ = DEVICE_IDENTITIES_LENGTH;       // LENGTH
    *Dest_p++ = SourceDevice;   // VALUE
    *Dest_p = DestDevice;

    return (DEVICE_IDENTITIES_TOTAL_SIZE);
}                               /* SIMTKT_PP_BuildSimpleTLV_DeviceIdentities */

/********************************************************************/
/**
 *
 * @function     SIMTKT_PP_NPI_TON_ToADN_Byte
 *
 * @description  Converts the enum values of NPI & TON into a Byte
 *
 * @param        NPI :  SMS TPDU Data
 * @param        TON :  SMS TPDU Data
 *
 * @return       Byte to be stored
 */
/********************************************************************/
static uint8_t SIMTKT_PP_NPI_TON_ToADN_Byte(
    const ste_sim_numbering_plan_id_t NPI,
    const ste_sim_type_of_number_t TON)
{
    uint8_t ADNByte = NPI_TON_FIXED_BITS;

    ADNByte = ADNByte | ((TON << TON_LSB_POS) & TON_MASK);
    ADNByte |= (NPI & NPI_MASK);

    return (ADNByte);
}                               /* SIMTKT_PP_NPI_TON_ToADN_Byte */

/********************************************************************/
/**
 *
 * @function     SIMTKT_PP_SendDownloadEnvelope
 *
 * @description  Build and sends the PP download envelope
 *
 * @param        SAT_TP_UserData_p :            SMS TPDU Data
 * @param        SAT_DialledString_p :          Service Centre Address
 *
 * @return       TRUE if SMS PP Download Envelope was built successfully;
 *               FALSE otherwise.
 */
/********************************************************************/
static uint8_t SIMTKT_PP_SendDownloadEnvelope(
    const SMS_SAT_TP_UserData_t * const SAT_TP_UserData_p,
    const SMS_SAT_DialledString_t * const SAT_DialledString_p)
{
    uint8_t Result = FALSE;
    char *Envelope_p = NULL;
    uint16_t TotalLength = SIMTKT_PP_GetDownloadLength(SAT_TP_UserData_p->TPDU_Length,
                                                       SAT_DialledString_p->NoOfBytes);

    if (TotalLength <= MAX_APDU_ENVELOPE_DATA_LENGTH) { // Space for Envelope so allocate storage
        Envelope_p = (char *) SMS_HEAP_UNTYPED_ALLOC(sizeof(char) * (MAX_APDU_ENVELOPE_DATA_LENGTH));

        if (Envelope_p == NULL) {       // Memory allocation failed
            SMS_A_(SMS_LOG_E("smsimtkt.c: SIMTKT_PP_SendDownloadEnvelope: Memory allocation for APDU PP Download Envelope Data FAILED"));
        } else {
            // Build SMS PP download envelope as structures fit and memory alloc'd
            uint32_t EnvelopeSize = 0;  // including Tag & length bytes
            uint8_t DeviceIdLength = 0;
            char *Env_p = Envelope_p;

            *Env_p++ = SMS_PP_DOWNLOAD_TAG;     // SMS PP Download Tag

            // Set overall envelope size to include length and SMS PP tag
            EnvelopeSize = TotalLength + 2;

            if (EnvelopeSize > MAX_LENGTH_BYTE1) {
                *Env_p++ = TWO_BYTES_LENGTH_IND;
                EnvelopeSize++; // Include extra length byte in overall envelope size
            }
            // Fill in length field
            *Env_p++ = (uint8_t) TotalLength;   // Must be less than 256

            // Build device identities TLV
            DeviceIdLength = SIMTKT_PP_BuildSimpleTLV_DeviceIdentities(DEVICE_NETWORK, DEVICE_SIM, TRUE, Env_p);

            Env_p += DeviceIdLength;    // => Service Centre Address

            // Build service centre address TLV
            *Env_p++ = (ADDRESS_TAG | COMPREHENSION_REQUIRED_MASK);

            // Length may be variable size
            // Include TON/NPI byte in length calculation
            if ((SAT_DialledString_p->NoOfBytes + 1) > MAX_LENGTH_BYTE1) {
                *Env_p++ = TWO_BYTES_LENGTH_IND;
            }
            // Add in TON/NPI byte to length
            *Env_p++ = SAT_DialledString_p->NoOfBytes + 1;
            *Env_p++ = SIMTKT_PP_NPI_TON_ToADN_Byte(SAT_DialledString_p->NPI, SAT_DialledString_p->TON);

            memcpy(Env_p, SAT_DialledString_p->Text_p, SAT_DialledString_p->NoOfBytes);

            Env_p += SAT_DialledString_p->NoOfBytes;

            // Build SMS TPDU TLV
            *Env_p++ = (SMS_TPDU_TAG | COMPREHENSION_REQUIRED_MASK);    // => Tag

            if (SAT_TP_UserData_p->TPDU_Length > MAX_LENGTH_BYTE1) {
                *Env_p++ = TWO_BYTES_LENGTH_IND;
            }

            *Env_p++ = SAT_TP_UserData_p->TPDU_Length;

            memcpy(Env_p, SAT_TP_UserData_p->TPDU, SAT_TP_UserData_p->TPDU_Length);

            if (EnvelopeSize > 2) {
                int UICC_Status;
                ste_sim_t *UICC_p = MessagingSystem_UICC_SimAsyncGet();

                if (!UICC_p) {
                    SMS_A_(SMS_LOG_E("smsimtkt.c: SIMTKT_PP_SendDownloadEnvelope: SIM UICC Server Object FAILED"));
                } else {
                    // We need to distinguish, in the CAT callback, whether the envelope download came from here or the
                    // equivalent CBS data download. We use the Client Tag to check which is which.
                    uintptr_t UICC_Client_Tag = (uintptr_t) SMS_PP_DOWNLOAD_TAG;

                    SMS_A_(SmUtil_HexPrint_Raw((const uint8_t *) Envelope_p, EnvelopeSize, "SMS: smstkt.c: Downloaded Envelope_p"));

                    UICC_Status = ste_cat_envelope_command(UICC_p, UICC_Client_Tag, Envelope_p, EnvelopeSize);

                    if (UICC_Status) {
                        SMS_A_(SMS_LOG_E("smsimtkt.c: SIMTKT_PP_SendDownloadEnvelope: ste_cat_envelope_command FAILED"));
                    } else {
                        Result = TRUE;
                    }
                }
            }

            SMS_HEAP_FREE(&Envelope_p);
        }
    } else {                    // Indicate error as size too great
        SMS_A_(SMS_LOG_E("smsimtkt.c: SIMTKT_PP_SendDownloadEnvelope: APDU PP Download Envelope parameters too large %d", TotalLength));
    }

    return (Result);
}                               /* SIMTKT_PP_SendDownloadEnvelope */

/********************************************************************/
/**
 *
 * @function     HandleSIMTKT_Message
 *
 * @description  This function extracts the Service Center Address and
 *               TPDU data from the SIMTKT message and downloads them
 *               to the SIM Application Toolkit. If the SIM Toolkit is
 *               activated for download the message is stored in the
 *               SIM.
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
static void HandleSIMTKT_Message(
    void *object,
    EventData_p eventData)
{
    ShortMessage_p NewShortMessage;
    SIMTKTSystem_p SIMTKTSystem = (SIMTKTSystem_p) object;
    SMS_SAT_TP_UserData_t *SAT_TPU_Data_p = (SMS_SAT_TP_UserData_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_SAT_TP_UserData_t));

    if (SAT_TPU_Data_p != NULL) {       /*  Short message memory allocation ok */

        SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_Message: ShortMessage is not NULL"));
        NewShortMessage = (ShortMessage_p) eventData->ShortMessage;

        // Initialise unused TPDU bytes
        memset(SAT_TPU_Data_p->TPDU, 0xFF, SMS_SAT_MAX_SIZE_SMS_TPDU);

        if (ShortMessage_GetTpdu(NewShortMessage, &SAT_TPU_Data_p->TPDU_Length, SAT_TPU_Data_p->TPDU) == TRUE) {        /* TPDU data extracted ok */
            SIMTKTSystem->StoredProtocolIdentifier = ShortMessage_GetProtocolIdentifier(NewShortMessage);
            SIMTKTSystem->StoredDataCodingScheme = ShortMessage_GetDataCodingScheme(NewShortMessage);
            SIMTKTSystem->StoredAlphabet = ShortMessage_GetAlphabet(NewShortMessage);
            SIMTKTSystem->SMS_DownloadActivated = SmUtil_UICC_ServiceEnabled(SIM_SERVICE_TYPE_SMS_PP);

            if (!SIMTKTSystem->SMS_DownloadActivated) {
                /* PP download not active on SIM store msg */
                SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_Message: SIMTOOL not supported Store message"));

                ShortMessage_Acknowledge(NewShortMessage, TP_NO_CAUSE);

                // SIM Toolkit functionality not enabled in the service table, store the message.
                // and tell the message manager, as we may end up witha n oprhaned short message
#ifndef REMOVE_SMSTRG_LAYER
                if (SM_STORE_FAILED != ShortMessage_StoreMedia(NewShortMessage, (MSG_StorageMedia_t *) SIM_VOL)) {
                    SMS_A_(SMS_LOG_I("smsimtkt.c: HandleSIMTKT_Message: SIMTOOL message Stored"));
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!THIS NEEDS TO BE HANDLED IN SHORT MESSAGE MODE!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    //MessageManager_HandleEvent(MESSAGEMANAGER_RECEIVE_NEW_SHORT_MESSAGE,eventData);
                } else
#endif                          // REMOVE_SMSTRG_LAYER
                {
                    SMS_A_(SMS_LOG_I("smsimtkt.c: HandleSIMTKT_Message: SIMTOOL message Not Stored"));
                }
            }
            /* end PP download not active on SIM store msg */
            else {              /* PP Download active on SIM */
                // According to 03.40 section 9.2.3.9 the message encoding has to be 8-bit.
                // However, TA test 27.22.5.1 uses 7-bit format in some tests so do not check encoding here.
                ShortMessagePackedAddress_t *MessageAddress_p = (ShortMessagePackedAddress_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(ShortMessagePackedAddress_t));
                uint8_t *DummyDigits_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(uint8_t) * (SMS_MAX_DIGITS_IN_NUMBER + 1));
                SMS_SAT_DialledString_t *DialledString_p = (SMS_SAT_DialledString_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_SAT_DialledString_t));

                SMS_B_(SMS_LOG_D("smsimtkt.c: Handle Simtoolkit message"));

                if ((MessageAddress_p != NULL) && (DummyDigits_p != NULL) && (DialledString_p != NULL)) {       /* Memory allocation passed for SIM requester parameters */
                    memset(DummyDigits_p, 0x00, (SMS_MAX_DIGITS_IN_NUMBER + 1) * (sizeof(uint8_t)));
                    memset(DialledString_p, 0x00, (sizeof(SMS_SAT_DialledString_t)));

                    // Only use this function to get the TON/NPI as it returns ASCII data characters not BCD.
                    ShortMessage_GetUnpackedServiceCentreAddress(NewShortMessage, DummyDigits_p, &DialledString_p->TON, &DialledString_p->NPI);

                    // Get the Service Centre Address in raw BCD format.
                    ShortMessage_GetPackedServiceCentreAddress(NewShortMessage, MessageAddress_p);

                    DialledString_p->Text_p = MessageAddress_p->AddressValue;
                    DialledString_p->NoOfBytes = 0;

                    // Protect MessageAddress_p->Length-1 against overflow
                    if (MessageAddress_p->Length > 0) {
                        DialledString_p->NoOfBytes = MessageAddress_p->Length - 1;      // -1 to exclude TON/NPI byte from dialled string length.
                    }
                    // Build and send the PP download envelope
                    if (SIMTKT_PP_SendDownloadEnvelope(SAT_TPU_Data_p, DialledString_p) == TRUE) {
                        SMS_B_(SMS_LOG_D("smsimtkt.c: SIMTKT_PP_SendDownloadEnvelope: Build and send PP download envelope PASSED"));
                    } else {
                        SMS_A_(SMS_LOG_E("smsimtkt.c: SIMTKT_PP_SendDownloadEnvelope: Build and send PP download envelope FAILED"));
                    }
                } else {        /* Memory allocation failed for SIM requester parameters */
                    SMS_A_(SMS_LOG_E("smsimtkt.c: Memory allocation for PP Download FAILED"));
                }               /* Memory allocation failed for SIM requester parameters */

                SMS_HEAP_FREE(&MessageAddress_p);
                SMS_HEAP_FREE(&DummyDigits_p);
                SMS_HEAP_FREE(&DialledString_p);
            }                   /* end PP Download active on SIM */
        } /* end TPDU data extracted ok */
        else {
            SMS_A_(SMS_LOG_E("smsimtkt.c: Could not create TPDU from Message"));
        }

        SMS_HEAP_FREE(&SAT_TPU_Data_p);
    } /* end Short message memory allocation ok */
    else {
        SMS_A_(SMS_LOG_E("smsimtkt.c: Error allocating received short message data "));
    }

}                               /* HandleSIMTKT_Message */

/********************************************************************/
/**
 *
 * @function     HandleSIMTKT_DeliveryReport
 *
 * @description  This function builds a delivery report for
 *               downloaded Service Centre Address and TPDU data
 *               to the SIM Application Toolkit.
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
static void HandleSIMTKT_DeliveryReport(
    void *object,
    EventData_p eventData)
{
    ShortMessage_p DeliveryShortMessage;
    SIMTKTSystem_p SIMTKTSystem = (SIMTKTSystem_p) object;

    SMS_A_(SMS_LOG_I("smsimtkt.c: HandleSIMTKT_DeliveryReport: SW1 0x%02X, SW2 0x%02X", EnvelopeCB.StatusWord1, EnvelopeCB.StatusWord2));

    if ((EnvelopeCB.StatusWord1 == 0x90 && EnvelopeCB.StatusWord2 == 0x00) || EnvelopeCB.StatusWord1 == 0x91 || EnvelopeCB.StatusWord1 == 0x92) {
        DeliveryShortMessage = ShortMessage_Create(SM_DELIVER_REPORT_ACK);
    }
    else {
        DeliveryShortMessage = ShortMessage_Create(SM_DELIVER_REPORT_ERROR);
    }

    if (DeliveryShortMessage == NULL) {
        SMS_A_(SMS_LOG_E("smsimtkt.c: HandleSIMTKT_DeliveryReport: Failed to create a Deliver Report, due to lack of memory"));
    } else {
        ShortMessage_SetAcknowledgeRequired(DeliveryShortMessage, TRUE);

        // See 3GPP TS 31.111 section 7.1.1.1 for information about what to do for various status words.
        if ((EnvelopeCB.StatusWord1 == 0x90 && EnvelopeCB.StatusWord2 == 0x00) || EnvelopeCB.StatusWord1 == 0x91 || EnvelopeCB.StatusWord1 == 0x92) {
            // Success
            SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: SIMTOOL DATA DOWNLOAD SUCCESS"));
            (void) ShortMessage_PackTPDUHeader(DeliveryShortMessage, TP_NO_CAUSE, &SIMTKTSystem->StoredProtocolIdentifier, &SIMTKTSystem->StoredDataCodingScheme, FALSE, FALSE, NULL, NULL, NULL, NULL);

            if (EnvelopeCB.APDULength > 0 && EnvelopeCB.APDU_p != NULL) {
                // There is body data, so pack it up.
                SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: Create and Pack Body"));
                (void) ShortMessage_CopyMessageBody(DeliveryShortMessage, EnvelopeCB.APDULength, SIMTKTSystem->StoredAlphabet, EnvelopeCB.APDU_p);
            }

            SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: Send ACK"));
            (void) ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, RMS_RP_NO_CAUSE);
        } else if (EnvelopeCB.StatusWord1 == 0x93 && EnvelopeCB.StatusWord2 == 0x00) {
            // CAT Busy
            SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: SIMTOOL SIMTOOL BUSY"));
            (void) ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, TP_SIM_APPLICATION_TOOLKIT_BUSY);
        } else if (EnvelopeCB.StatusWord1 == 0x62 || EnvelopeCB.StatusWord1 == 0x63 || EnvelopeCB.StatusWord1 == 0x9E || EnvelopeCB.StatusWord1 == 0x9F) {
            // '62 XX' or '63 XX' error has occurred.
            SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: SIMTOOL DATA DOWNLOAD ERROR"));

            (void) ShortMessage_PackTPDUHeader(DeliveryShortMessage, TP_SIM_DATA_DOWNLOAD_ERROR, &SIMTKTSystem->StoredProtocolIdentifier, &SIMTKTSystem->StoredDataCodingScheme, FALSE,
                                               FALSE, NULL, NULL, NULL, NULL);

            if (EnvelopeCB.APDULength > 0 && EnvelopeCB.APDU_p != NULL) {
                // There is body data, so pack it up.
                SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: Create and Pack Body"));
                (void) ShortMessage_CopyMessageBody(DeliveryShortMessage, EnvelopeCB.APDULength, SIMTKTSystem->StoredAlphabet, EnvelopeCB.APDU_p);
            }

            SMS_B_(SMS_LOG_D("smsimtkt.c: Send ACK"));
            (void) ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED);
        } else if (EnvelopeCB.StatusWord1 == 0x6F) {
            // '6F XX' error has occurred.
            SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: SIMTOOL DATA DOWNLOAD ERROR 6FXX"));

            SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: Create unspecified and Pack Header"));
            (void) ShortMessage_PackTPDUHeader(DeliveryShortMessage,
                                               TP_SIM_DATA_DOWNLOAD_ERROR, &SIMTKTSystem->StoredProtocolIdentifier, &SIMTKTSystem->StoredDataCodingScheme, FALSE, FALSE, NULL, NULL, NULL, NULL);

            SMS_B_(SMS_LOG_D("smsimtkt.c: HandleSIMTKT_DeliveryReport: Send unspecified ACK"));
            (void) ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED);
        } else {
            SMS_B_(SMS_LOG_D("simtkt.c: HandleSIMTKT_DeliveryReport: SIMTOOL UNSPECIFIED ERROR"));
            (void) ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, TP_UNSPECIFIED_ERROR_CAUSE);
        }
    }

    // Free resources.
    EnvelopeCB.APDULength = 0;
    if (EnvelopeCB.APDU_p != NULL) {
        SMS_HEAP_FREE(&EnvelopeCB.APDU_p);
    }
    EnvelopeCB.APDU_p = NULL;
}                               /* HandleSIMTKT_DeliveryReport */
