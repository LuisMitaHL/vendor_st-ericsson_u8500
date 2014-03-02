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
 *  The Short Message Object and it's methods.
 *
 *************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"

/* Requester Interface definitions */
//#include "r_sys.h"

/* The Message Transport Server Interface */
#include "g_sms.h"
#include "r_sms.h"

/* Message Module Include Files */
#include "sm.h"
#include "smsevnthndr.h"
#ifndef REMOVE_SMSTRG_LAYER
#include "smstrg.h"
#endif                          //REMOVE_SMSTRG_LAYER
#include "smtpdu.h"
#include "smrouter.h"
#include "smutil.h"
#include "smslist.h"

/* Configuration */
#include "c_sms_config.h"

// START LINUX PORTING
// Linux Porting. Code to be moved to header files at later point.

typedef enum {
    SMS_TYPE_OF_NUMBER_UNKNOWN = 0x0,
    SMS_TYPE_OF_NUMBER_INTERNATIONAL = 0x1,
    SMS_TYPE_OF_NUMBER_NATIONAL = 0x2,
    SMS_TYPE_OF_NUMBER_NETWORK_SPECIFIC = 0x3,
    SMS_TYPE_OF_NUMBER_DEDICATED_PAD_ACCESS = 0x4,
    SMS_TYPE_OF_NUMBER_ALPHANUMERIC = 0x5,
    SMS_TYPE_OF_NUMBER_ABBREVIATED = 0x6
} SMS_TypeOfNumber_e;

typedef enum {
    SMS_NUMBERING_PLAN_ID_UNKNOWN = 0x0,
    SMS_NUMBERING_PLAN_ID_ISDN = 0x1,
    SMS_NUMBERING_PLAN_ID_DATA = 0x3,
    SMS_NUMBERING_PLAN_ID_TELEX = 0x4,
    SMS_NUMBERING_PLAN_ID_NATIONAL = 0x8,
    SMS_NUMBERING_PLAN_ID_PRIVATE = 0x9
} SMS_NumberingPlanId_e;

#define TON_UNKNOWN        SMS_TYPE_OF_NUMBER_UNKNOWN
#define TON_ALPHANUMERIC   SMS_TYPE_OF_NUMBER_ALPHANUMERIC
#define TON_ABBREVIATED    SMS_TYPE_OF_NUMBER_ABBREVIATED
#define NPI_UNKNOWN        SMS_NUMBERING_PLAN_ID_UNKNOWN

// END LINUX PORTING


#define ASCII_STAR_CHARACTER (0x2A)
#define ASCII_HASH_CHARACTER (0x23)
#define ASCII_p_CHARACTER    (0x70)
// Semi-octet decoding as defined in 3GPP 23.040 section 9.1.2.3 required
// to solve IR99934 however this was Rejected by customer
//#define ASCII_a_CHARACTER    (0x61)
//#define ASCII_b_CHARACTER    (0x62)
//#define ASCII_c_CHARACTER    (0x63)

#define BCD_STAR_CHARACTER (0x3A)
#define BCD_HASH_CHARACTER (0x3B)
#define BCD_p_CHARACTER    (0x3C)
// Semi-octet decoding as defined in 3GPP 23.040 section 9.1.2.3 required
// to solve IR99934 however this was Rejected by customer
//#define BCD_a_CHARACTER    (0x3C)
//#define BCD_b_CHARACTER    (0x3D)
//#define BCD_c_CHARACTER    (0x3E)

// Used as Limit in lookup table
#define NO_BCD_CHARACTERS 3

#define INVALID_TPDU_CACHE_REF 0

#define EODRC_USER_DATA_BYTES   3       // UDHL + IEI + IEIL = 3 bytes
#define EODRC_USER_DATA_SEPTETS 4       // UDHL + IEI + IEIL = 3 bytes = 4 septets


// Used to be in p_types.h
#define MS_MASK  0xF0           /* 1111 0000 */
#define LS_MASK  0x0F           /* 0000 1111 */

#define MAX_STATUS_REPORT_OK_BITVALUE       (0x02)
#define MIN_STATUS_REPORT_UNKNOWN_BITVAL    (0x10)
#define MAX_STATUS_REPORT_UNKNOWN_BITVAL    (0x1F)
#define MIN_STATUS_REPORT_TEMP_ERROR_BITVAL (0x20)
#define MAX_STATUS_REPORT_TEMP_ERROR_BITVAL (0x3F)

#define DCS_GENERAL_CODING_CLASS2_8BIT  (0x16)
#define DCS_CLASS2_8BIT                 (0xf6)

/**
 * Maximum validity period
 */
#define SMS_VALIDPER_MAXIMUM_PERIOD 255


//Type definitions
typedef struct TPDUObject {
    Tpdu_t Tpdu;
    ShortMessagePackedAddress_t TpduAddress;
} TPDUObject_t;

typedef struct ShortMessageObject {
    ShortMessageType_t ShortMessageType;
    SMS_Status_t Status;
    SMS_Position_t StoragePosition;
    TPDUObject_t *TPDUObject_p;
    uint8_t TpduLength;
    uint8_t AcknowledgeRequired;
    uint8_t SequenceNumber;
    DeliveryStatus_t DeliveryStatus;
    MoreToSendState_t MoreToSendState;
    uint8_t StatusReportRecNum;
} ShortMessage_t;

typedef struct {
    uint8_t InUse;
    uint32_t RefNum;
    ShortMessage_p ShortMessage;
    TPDUObject_t *TPDUObject_p;
} TPDUCacheEntry_t;

typedef MSG_List *TPDUPool;

// Fucntion Prototypes
//ShortMessage Dynamic Memory Management
static void ShortMessage_LoadTPDU(
    ShortMessage_p);
static void ShortMessage_UnloadTPDU(
    ShortMessage_p,
    uint8_t);

//Local Static Functions
static uint8_t ShortMessage_PackGSMFormatAddress(
    uint8_t,
    ShortMessageSubscriberNumber_t *,
    ShortMessagePackedAddress_t *);
static uint16_t ShortMessage_GetTPDUCacheSize(
    void);
static void ShortMessage_InitialiseTPDUCache(
    TPDUPool * theTPDUPool);
static TPDUCacheEntry_t *ShortMessage_TPDUCache_IterateFromStart(
    TPDUPool theTPDUPool,
    TPDUCacheEntry_t * PreviousTPDU_p);
static TPDUObject_t *ShortMessage_AllocateTpduCacheEntry(
    TPDUPool theTPDUPool,
    ShortMessage_p ShortMessage);
static void ShortMessage_RemoveTPDUFromCache(
    TPDUPool theTPDUPool,
    TPDUObject_t * TPDUObject_p,
    ShortMessage_p ShortMessage);

static ShortMessage_p ShortMessage_IterateFromStart(
    MSG_List * thisShortMessageList,
    ShortMessage_p previousMessage);

//Local Data Declarations
MSG_List *ShortMessageMSG_List_p;
TPDUPool TPDUCache_List;

static uint8_t BCD_Lookup[NO_BCD_CHARACTERS][2] = { {BCD_STAR_CHARACTER, ASCII_STAR_CHARACTER},
{BCD_HASH_CHARACTER, ASCII_HASH_CHARACTER},
{BCD_p_CHARACTER, ASCII_p_CHARACTER}
};

// Semi-octet decoding as defined in 3GPP 23.040 section 9.1.2.3 required
// to solve IR99934 however this was Rejected by customer
//                                {BCD_a_CHARACTER,     ASCII_a_CHARACTER},
//                                {BCD_b_CHARACTER,     ASCII_b_CHARACTER},
//                                {BCD_c_CHARACTER,     ASCII_c_CHARACTER} };


/********************************************************************/
/**
 *
 * @function     ShortMessage_Initialise
 *
 * @description  Initialise the Short Message Object
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_Initialise(
    void)
{
    // Create the Short Message MSG_List
    ShortMessageMSG_List_p = MSG_ListCreate();
    ShortMessage_InitialiseTPDUCache(&TPDUCache_List);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_Deinitialise
 *
 * @description  Deinitialise the Short Message Object
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_Deinitialise(
    void)
{
    if (NULL != ShortMessageMSG_List_p) {
        ShortMessage_t *NextMSG_p = MSG_ListGetFirst(ShortMessageMSG_List_p);
        while (NULL != NextMSG_p) {
            (void) MSG_ListRemove(ShortMessageMSG_List_p, NextMSG_p);
            SMS_HEAP_FREE(&NextMSG_p);
            NextMSG_p = (ShortMessage_t *) MSG_ListGetFirst(ShortMessageMSG_List_p);
        }
        MSG_ListDestroy(ShortMessageMSG_List_p);
    }

    if (NULL != TPDUCache_List) {
        TPDUCacheEntry_t *NextTPDU_p = MSG_ListGetFirst(TPDUCache_List);
        while (NULL != NextTPDU_p) {
            (void) MSG_ListRemove(TPDUCache_List, NextTPDU_p);
            if(NULL != NextTPDU_p->TPDUObject_p) {
                SMS_HEAP_FREE(&NextTPDU_p->TPDUObject_p);
            }
            SMS_HEAP_FREE(&NextTPDU_p);
            NextTPDU_p = (TPDUCacheEntry_t *) MSG_ListGetFirst(TPDUCache_List);
        }
        MSG_ListDestroy(TPDUCache_List);
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_Exists
 *
 * @description  Determines if the specified short message still
 *               exists in the list of short messages
 *
 * @param        ShortMessage_p
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_Exists(
    ShortMessage_p theShortMessage)
{
    return MSG_ListFindItem(ShortMessageMSG_List_p, theShortMessage) == NOPOS ? FALSE : TRUE;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_Reinitialise
 *
 * @description  Initialise the Short Message Object
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_Reinitialise(
    void)
{
    ShortMessage_p NextMessage = NULL;
    uint8_t ListDestroyed = FALSE;

    while (!ListDestroyed) {
        NextMessage = (ShortMessage_p) MSG_ListGetFirst(ShortMessageMSG_List_p);

        if (NULL == NextMessage) {
            ListDestroyed = TRUE;
        } else {
            // Remove the entry from the msg list
            (void) MSG_ListRemove(ShortMessageMSG_List_p, NextMessage);

            // If the TPDU memory was dynamically allocated, we need to
            // ensure this memory is freed otherwise we will leak it
            if (NextMessage->TPDUObject_p != NULL) {
                // This is a bit of a cheat but serves as an ends to a means.
                // The TPDU Cache memory is allocated at start up and never
                // freed, not even as part of reinitialisation.  However, on
                // receipt of a message, the TPDU doesn't go into the cache
                // until the message is stored, so the TPDU in this instance
                // would be alloc'd.  To prevent leaking this TPDU (although
                // it should never happen) we attempt to remove it from the
                // cache.  If the TPDU pointer is still non null after this,
                // it indicates that the TPDU wasn't cached and hence was
                // alloc'd so we free up the TPDU memory instead.
                ShortMessage_RemoveTPDUFromCache(TPDUCache_List, NextMessage->TPDUObject_p, NextMessage);

                if (NULL != NextMessage->TPDUObject_p) {
                    SMS_HEAP_FREE(&NextMessage->TPDUObject_p);
                    NextMessage->TPDUObject_p = NULL;
                    SMS_B_(SMS_LOG_D("sm.c: Deallocated short message TPDU from dynamic ram"));
                }
            }
            SMS_HEAP_FREE(&NextMessage);
        }
    }
    // Don't need to destroy the ShortMessageList as it was created statically
}

/********************************************************************/
/**
 *
 * @function     *ShortMessage_IterateFromStart
 *
 * @description  <Detailed description of the method>
 *
 * @param        thisShortMessageList : The short message list.
 * @param        previousMessage :      The previous sm in the list
 *
 * @return       static ShortMessage_p
 */
/********************************************************************/
static ShortMessage_p ShortMessage_IterateFromStart(
    MSG_List * thisShortMessageList,
    ShortMessage_p previousMessage)
{
    ShortMessage_p nextMessage = NULL;

    if (previousMessage == NULL) {
        //if previousMessage has been set to NULL we must be looking
        //for the first message so don't bother looking for it
        //first time so find the first bit we've got
        nextMessage = (ShortMessage_p) MSG_ListGetFirst(thisShortMessageList);
    } else {
        //not the first time so find one that is next after previousMessage
        nextMessage = (ShortMessage_p) MSG_ListGetNext(thisShortMessageList, previousMessage);
    }
    return (nextMessage);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_Create
 *
 * @description  Create an Instance of a Short Message Object
 *
 * @param        shortMessageType : as type of ShortMessageType_t.
 *
 * @return       ShortMessage_p
 */
/********************************************************************/
ShortMessage_p ShortMessage_Create(
    ShortMessageType_t shortMessageType)
{
    ShortMessage_t *NewShortMessage_p;

    // Create an smInstance
    NewShortMessage_p = (ShortMessage_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(ShortMessage_t));

    if (NewShortMessage_p != NULL) {
        // Add an instance into the MSG_List
        (void) MSG_ListInsertLast(ShortMessageMSG_List_p, NewShortMessage_p);

        SMS_B_(SMS_LOG_D("sm.c: ShortMessageCreate - Adding SM 0x%08x to SM Manager MSG_List 0x%08x", (int) NewShortMessage_p, (int) ShortMessageMSG_List_p));

        NewShortMessage_p->ShortMessageType = shortMessageType;
        NewShortMessage_p->TPDUObject_p = NULL;
        NewShortMessage_p->TpduLength = 0;
        NewShortMessage_p->StoragePosition = SMS_STORAGE_POSITION_INVALID;
        NewShortMessage_p->AcknowledgeRequired = FALSE;
        NewShortMessage_p->SequenceNumber = 1;
        NewShortMessage_p->DeliveryStatus = STATUS_REPORT_RESET;
        NewShortMessage_p->MoreToSendState = STATE_DISABLED;
        NewShortMessage_p->StatusReportRecNum = SMS_STORAGE_POSITION_INVALID;

        switch (shortMessageType) {
        case SM_DELIVER:       // SC to MS
        case SM_SUBMIT_REPORT_ACK:     // SC to MS
        case SM_SUBMIT_REPORT_ERROR:   // SC to MS
        case SM_STATUS_REPORT: // SC to MS
            NewShortMessage_p->Status = SMS_STATUS_UNREAD;
            break;
        case SM_DELIVER_REPORT_ACK:    // MS to SC
        case SM_DELIVER_REPORT_ERROR:  // MS to SC
        case SM_SUBMIT:        // MS to SC
        case SM_COMMAND:       // MS to SC
            NewShortMessage_p->Status = SMS_STATUS_UNSENT;
            break;
        default:
            {
                // Do not know what type - so delte it.
                ShortMessage_Delete(NewShortMessage_p);
                NewShortMessage_p = NULL;
            }
            break;
        }                       // end switch
    }                           // end if NULL

    return (NewShortMessage_p);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_Delete
 *
 * @description  Function to delete a short message
 *
 * @param        shortMessage : as type of ShortMessage_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_Delete(
    ShortMessage_p shortMessage)
{
#ifndef REMOVE_SMSTRG_LAYER
    if (shortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID) {
        (void) SmStorage_SmDelete(shortMessage->StoragePosition, TRUE);
    }
#endif                          // REMOVE_SMSTRG_LAYER

    // Find the Short Message Instance in the MSG_List. MSG_ListRemove does a Find and then delete
    (void) MSG_ListRemove(ShortMessageMSG_List_p, shortMessage);

    SMS_B_(SMS_LOG_D("sm.c: ShortMessageDelete - Removing SM 0x%08x from SM Manager MSG_List 0x%08x", (int) shortMessage, (int) ShortMessageMSG_List_p));

    if (NULL != shortMessage->TPDUObject_p) {
        ShortMessage_DeallocateTPDUStorage(shortMessage);
    }
    // Now free up the data
    SMS_HEAP_FREE(&shortMessage);

}


/********************************************************************/
/**
 *
 * @function     ShortMessage_DeletePart
 *
 * @description  Function to delete a short message, can only be called
 * from the MSG layer, and is a special function which does not tell the
 * MSG layer that a SM has been deleted as it is assumed the MSG manager
 * already knows this
 *
 * @param        shortMessage      : as type of ShortMessage_p.
 * @param        deleteFromStorage : whether of not to delete the SM from presistant storage.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_DeletePart(
    ShortMessage_p shortMessage,
    uint8_t deleteFromStorage)
{

#ifdef REMOVE_SMSTRG_LAYER
    SMS_IDENTIFIER_NOT_USED(deleteFromStorage)
#else
    if (shortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID) {
        (void) SmStorage_SmDelete(shortMessage->StoragePosition, deleteFromStorage);
    }
#endif                          // REMOVE_SMSTRG_LAYER

    // Find the Short Message Instance in the MSG_List. MSG_ListRemove does a Find and then delete
    (void) MSG_ListRemove(ShortMessageMSG_List_p, shortMessage);

    SMS_B_(SMS_LOG_D("sm.c: ShortMessageDeletePart - Removing SM 0x%08x from SM Manager MSG_List 0x%08x", (int) shortMessage, (int) ShortMessageMSG_List_p));

    if (NULL != shortMessage->TPDUObject_p) {
        ShortMessage_DeallocateTPDUStorage(shortMessage);
    }
    // Now free up the data
    SMS_HEAP_FREE(&shortMessage);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetType
 *
 * @description  Function to obtain the short message type for a short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       ShortMessageType_t
 */
/********************************************************************/
ShortMessageType_t ShortMessage_GetType(
    ShortMessage_p thisShortMessage)
{
    return ((ShortMessageType_t) thisShortMessage->ShortMessageType);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_Print
 *
 * @description  Function to print an instance of a short message object
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_Print(
    ShortMessage_p thisShortMessage)
{
    uint8_t digitBuffer[SMS_MAX_DIGITS_IN_NUMBER + 1] = { '\0' };
    SMS_TypeOfNumber_t typeOfNumber;
    uint8_t npi;
    uint8_t ton;
    SMS_NumberingPlanId_t numberingPlanId;
    ShortMessageType_t smType;

    static const char *ShortMessageTypeString[] = {
        "DELIVER",              // SC to MS
        "SUBMIT_REPORT_ERROR",  // SC to MS
        "STATUS_REPORT",        // SC to MS
        "SUBMIT_REPORT_ACK",    // SC to MS
        "DELIVER_REPORT_ERROR", // MS to SC
        "SUBMIT",               // MS to SC
        "COMMAND",              // MS to SC
        "DELIVER_REPORT_ACK",   // MS to SC
        "UNKNOWN"
    };

    if (NULL != thisShortMessage) {
        size_t pos = 0;
        char *PrintBuffer = (char *)SMS_HEAP_UNTYPED_ALLOC((3 * sizeof(digitBuffer)) +    /* Digit buffers */
                                                           (6 * 10) +                     /* Integers */
                                                           (9 * 10) +                     /* Static text */
                                                           20 +                           /* Longest message type string */
                                                           12);                           /* ShortMessage pointer */

        if (NULL != PrintBuffer) {
            smType = ShortMessage_GetType(thisShortMessage);
            pos = sprintf(PrintBuffer, " Type = %s", ShortMessageTypeString[SMS_MIN(smType, SM_TYPE_LAST)]);
            pos += sprintf(PrintBuffer+pos, " Stat = %d", ShortMessage_GetStatus(thisShortMessage));
            pos += sprintf(PrintBuffer+pos, " Pos = %d", thisShortMessage->StoragePosition);

            //ShortMessage originating or destination address
            ShortMessage_GetUnpackedOrigOrDestAddress(thisShortMessage, digitBuffer, &ton, &npi);
            pos += sprintf(PrintBuffer+pos, " Add = %s", digitBuffer);
            pos += sprintf(PrintBuffer+pos, " TON: %d ", ton);
            pos += sprintf(PrintBuffer+pos, " NPI: %d ", npi);

            //Service centre address
            ShortMessage_GetUnpackedServiceCentreAddress(thisShortMessage, digitBuffer, &typeOfNumber, &numberingPlanId);
            pos += sprintf(PrintBuffer+pos, " SCAdd = %s", digitBuffer);
            pos += sprintf(PrintBuffer+pos, " TON: %d ", typeOfNumber);
            pos += sprintf(PrintBuffer+pos, " NPI: %d ", numberingPlanId);
        }

        SMS_LOG_D("0x%08x %s", (unsigned int) (uint32_t) thisShortMessage, PrintBuffer);      // double cast required.

        SMS_HEAP_FREE(&PrintBuffer);
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_HexPrint
 *
 * @description  Prints the Hex bytes of the short message
 *
 * @param        SC_Address_p :
 * @param        UserData_p :
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_HexPrint(
    ShortMessagePackedAddress_t * SC_Address_p,
    SMS_TPDU_t * UserData_p)
{
    uint16_t loop;
    uint8_t *CharBuffer = NULL;
    uint8_t *PrintBuffer = NULL;
    uint16_t BufferSize;

    // To reduce the number of prints, we print to a buffer fisrt - Allocate some memory :-
    // *2 for ascii representation of character in print buffer and
    // +1 for the space char between SC and UD
    // +1 for the Null terminator
    BufferSize = 1 + 1;
    if (UserData_p) {
        BufferSize += ASCII_CHAR_SIZE * UserData_p->Length;
    }
    if (SC_Address_p) {
        BufferSize += ASCII_CHAR_SIZE * SC_Address_p->Length;
    }

    PrintBuffer = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(BufferSize);
    CharBuffer = PrintBuffer;

    if (SC_Address_p) {
        for (loop = 0; loop < SC_Address_p->Length; loop++) {
            sprintf((char *) CharBuffer, "%02X", SC_Address_p->AddressValue[loop]);
            CharBuffer += ASCII_CHAR_SIZE;
        }
    }

    // Separate SC Address from User Data
    sprintf((char *) CharBuffer, " ");
    CharBuffer++;

    if (UserData_p) {
        for (loop = 0; loop < UserData_p->Length; loop++) {
            sprintf((char *) CharBuffer, "%02X", UserData_p->Data[loop]);
            CharBuffer += ASCII_CHAR_SIZE;
        }
    }

    // Now null terminate it
    *CharBuffer = 0x00;

    if (SC_Address_p) {
        SMS_LOG_I("sm.c: TPDU Data= %02X%02X%s", SC_Address_p->Length, SC_Address_p->TypeOfAddress, PrintBuffer);
    } else {
        SMS_LOG_D("sm.c: TPDU Data= <NULL>%s", PrintBuffer);
    }

    SMS_HEAP_FREE(&PrintBuffer);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_IsReplyRequested
 *
 * @description  Function to check if the short message is requested to reply
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsReplyRequested(
    ShortMessage_p thisShortMessage)
{
    uint8_t isReplyRequested = FALSE;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        isReplyRequested = TPDU_TestBit(TP_RP, thisShortMessage->TPDUObject_p->Tpdu);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (isReplyRequested);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetReplyRequested
 *
 * @description  Function to set the short message as reply requested
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        newSetting :       auint8_t type.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetReplyRequested(
    ShortMessage_p thisShortMessage,
    uint8_t newSetting)
{
    uint8_t alteredTPDU = FALSE;
    uint8_t replyPathSet = FALSE;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        replyPathSet = TPDU_TestBit(TP_RP, thisShortMessage->TPDUObject_p->Tpdu);

        if (replyPathSet != newSetting) {
            TPDU_SetBit(TP_RP, thisShortMessage->TPDUObject_p->Tpdu, newSetting);
            alteredTPDU = TRUE;
        }

        ShortMessage_UnloadTPDU(thisShortMessage, alteredTPDU);
    }

}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetAcknowledgeRequired
 *
 * @description  Function to set the short message as acknowledgement requested
 *
 * @param        thisShortMessage :    as type of ShortMessage_p.
 * @param        AcknowledgeRequired : uint8_t type.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetAcknowledgeRequired(
    ShortMessage_p thisShortMessage,
    uint8_t AcknowledgeRequired)
{
    if (thisShortMessage != NULL) {
        thisShortMessage->AcknowledgeRequired = AcknowledgeRequired;
    } else {
        SMS_B_(SMS_LOG_E("sm.c: ShortMessage_SetAcknowledgeRequired: error in ShortMessage_p"));
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetAcknowledgeRequired
 *
 * @description  Function to obtain the uint8_t value of if the short message  is acknowledgement requested
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetAcknowledgeRequired(
    ShortMessage_p thisShortMessage)
{
    uint8_t AckRequired = TRUE; //most cases need acknowledgement back

    if (thisShortMessage != NULL) {
        AckRequired = thisShortMessage->AcknowledgeRequired;
    } else {
        SMS_B_(SMS_LOG_E("sm.c: ShortMessage_GetAcknowledgeRequired: error in ShortMessage_p"));
    }
    return (AckRequired);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetStatus
 *
 * @description  Set the status of the specified Short Message, to the specified status
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        Status :           as type of ShortMessageStatus_t.
 *
 * @return       void
 */
/********************************************************************/
uint8_t ShortMessage_SetStatus(
    ShortMessage_p const thisShortMessage,
    SMS_Status_t const Status)
{
    uint8_t Result = FALSE;

    switch (ShortMessage_GetType(thisShortMessage)) {
    case SM_DELIVER:           // SC to MS
    case SM_SUBMIT_REPORT_ACK: // SC to MS
    case SM_SUBMIT_REPORT_ERROR:       // SC to MS
    case SM_STATUS_REPORT:     // SC to MS
        if ((Status == SMS_STATUS_READ) || (Status == SMS_STATUS_UNREAD)) {
            // set the parent message status before updating the individual short message
            thisShortMessage->Status = Status;
            Result = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("sm.c: Error Trying to set wrong status: %d to SC to MS short message type", Status));
        }
        break;
    case SM_DELIVER_REPORT_ACK:        // MS to SC
    case SM_DELIVER_REPORT_ERROR:      // MS to SC
    case SM_SUBMIT:            // MS to SC
    case SM_COMMAND:           // MS to SC
        if ((Status == SMS_STATUS_SENT) || (Status == SMS_STATUS_UNSENT)) {
            // set the parent message status before updating the individual short message
            thisShortMessage->Status = Status;
            Result = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("sm.c: Error Trying to set wrong status: %d to MS to SC short message type", Status));
        }
        break;
    default:
        {
            // Do not know what short message type
            SMS_A_(SMS_LOG_E("sm.c: unknown short message type"));
        }
        break;
    }

#ifndef REMOVE_SMSTRG_LAYER
    /*
     * Update Short Message within Storage if Necessary
     */
    if ((thisShortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID) && Result) {
        Result = SmStorage_SmStatusUpdate(thisShortMessage, thisShortMessage->StoragePosition, Status);
    }
#endif                          // REMOVE_SMSTRG_LAYER

    return (Result);
}


/********************************************************************/
/**
 * @function     ShortMessage_SetRejectDuplicates
 *
 * @description  Sets the TP_RD for the specified Short Message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessage_SetRejectDuplicates(
    ShortMessage_p const thisShortMessage)
{
    uint8_t Result = FALSE;

    if (thisShortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID) {
        if (ShortMessage_GetType(thisShortMessage) == SM_SUBMIT) {
            ShortMessage_LoadTPDU(thisShortMessage);

            if (thisShortMessage->TPDUObject_p != NULL) {
                // TPDU_SetBit needs looking at as currently it requires the mask for the bit it is setting!
                TPDU_SetBit(TP_RD, thisShortMessage->TPDUObject_p->Tpdu, 0x04);

                ShortMessage_UnloadTPDU(thisShortMessage, TRUE);

                Result = TRUE;
            }
        }
    }

    if (!Result) {
        SMS_A_(SMS_LOG_E("sm.c: Unable to set TP-RD"));
    }

    return (Result);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetStatus
 *
 * @description  Get the status of the specified Short Message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       SMS_Status_t
 */
/********************************************************************/
SMS_Status_t ShortMessage_GetStatus(
    ShortMessage_p const thisShortMessage)
{
    return ((SMS_Status_t) thisShortMessage->Status);
}


/********************************************************************/
/**
 *
 * @function     TPStatusToStatus
 *
 * @description  Converts TP status to a DeliveryStatus_t
 *
 * @param        status : DeliveryStatus.
 *
 * @return       DeliveryStatus_t
 */
/********************************************************************/
DeliveryStatus_t TPStatusToStatus(
    uint8_t status)
{
    DeliveryStatus_t DeliveryStatus = STATUS_REPORT_NOT_OK;

    if (status <= MAX_STATUS_REPORT_OK_BITVALUE) {
        DeliveryStatus = STATUS_REPORT_OK;
    } else if ((status >= MIN_STATUS_REPORT_UNKNOWN_BITVAL) && (status <= MAX_STATUS_REPORT_UNKNOWN_BITVAL)) {
        DeliveryStatus = STATUS_REPORT_UNKNOWN;
    } else if ((status >= MIN_STATUS_REPORT_TEMP_ERROR_BITVAL) && (status <= MAX_STATUS_REPORT_TEMP_ERROR_BITVAL)) {
        return (STATUS_REPORT_STILL_TRYING);
    }

    return DeliveryStatus;
}


#ifndef REMOVE_SMSTRG_LAYER
/********************************************************************/
/**
 *
 * @function     ShortMessage_SetDeliveredStatus
 *
 * @description  Set the delivery status for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        status :           as type of DeliveryStatus_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetDeliveredStatus(
    ShortMessage_p const thisShortMessage,
    DeliveryStatus_t status)
{
    if (ShortMessage_GetType(thisShortMessage) == SM_SUBMIT &&
        ShortMessage_GetStatus(thisShortMessage) == SMS_STATUS_SENT && thisShortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID && status != STATUS_REPORT_RESET) {
        thisShortMessage->DeliveryStatus = status;
        (void) SmStorage_WriteDeliveryStatus(thisShortMessage->StoragePosition, status);
    } else {
        SMS_B_(SMS_LOG_D("sm.c: ShortMessage_SetDeliveredStatus: either not submit, not sent or not stored"));
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetDeliveredStatus
 *
 * @description  Get the delivery status of the specified sm
 *
 * @param        thisShortMessage : Identifies the sm
 *
 * @return       DeliveryStatus_t
 */
/********************************************************************/
DeliveryStatus_t ShortMessage_GetDeliveredStatus(
    ShortMessage_p const thisShortMessage)
{
    DeliveryStatus_t DeliveryStatus = STATUS_REPORT_UNKNOWN;

    if (thisShortMessage->StoragePosition == SMS_STORAGE_POSITION_INVALID) {
        SMS_A_(SMS_LOG_E("sm.c: ShortMessage_GetDeliveredStatus: slotnumber invalid"));
    } else if (ShortMessage_GetType(thisShortMessage) == SM_SUBMIT && ShortMessage_GetStatus(thisShortMessage) == SMS_STATUS_SENT) {
        DeliveryStatus = thisShortMessage->DeliveryStatus;
    }

    return (DeliveryStatus);
}
#endif                          // REMOVE_SMSTRG_LAYER


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetCacheDeliveryStatus
 *
 * @description  Set the cached delivery status for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        status :           as type of DeliveryStatus_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetCacheDeliveryStatus(
    ShortMessage_p const thisShortMessage,
    DeliveryStatus_t status)
{
    thisShortMessage->DeliveryStatus = status;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetDeliveryStatus
 *
 * @description  Obtain the delivery status of a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       DeliveryStatus_t
 */
/********************************************************************/
DeliveryStatus_t ShortMessage_GetDeliveryStatus(
    ShortMessage_p const thisShortMessage)
{
    DeliveryStatus_t status = STATUS_REPORT_UNKNOWN;
    uint8_t TPStatus = 0x10;

    if (ShortMessage_GetType(thisShortMessage) == SM_STATUS_REPORT) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            TPStatus = TPDU_GetField(TP_ST, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

            status = TPStatusToStatus(TPStatus);

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }
    }
    return (status);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetUnpackedServiceCentreAddress
 *
 * @description  Function to obtain the unpacked service centre address of a speicifc short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *digits :          as uint8_t.
 * @param        *typeOfNumber :    as type of SMS_TypeOfNumber_t.
 * @param        *numberingPlanId : as type of SMS_NumberingPlanId_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_GetUnpackedServiceCentreAddress(
    ShortMessage_p thisShortMessage,
    uint8_t * digits,
    SMS_TypeOfNumber_t * typeOfNumber,
    SMS_NumberingPlanId_t * numberingPlanId)
{
    ShortMessagePackedAddress_t packedAddress;
    ShortMessageSubscriberNumber_t unpackedAddress;

    // Initialise digits
    if (digits != NULL) {
        memset(digits, 0x00, SMS_MAX_DIGITS_IN_NUMBER + 1);
    }

    *typeOfNumber = TON_UNKNOWN;
    *numberingPlanId = NPI_UNKNOWN;
    ShortMessage_GetPackedServiceCentreAddress(thisShortMessage, &packedAddress);
    UnpackServiceCentreAddress(&packedAddress, &unpackedAddress);

    if (digits != NULL) {
        strcpy((char *) digits, (char *) unpackedAddress.Digits);
    }

    *typeOfNumber = unpackedAddress.TypeOfNumber;
    *numberingPlanId = unpackedAddress.NumberingPlanId;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetPackedServiceCentreAddress
 *
 * @description  Get the Service Centre Address of the specified Short Message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *ScAddress_p :     as type of ShortMessagePackedAddress_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_GetPackedServiceCentreAddress(
    ShortMessage_p const thisShortMessage,
    ShortMessagePackedAddress_t * ScAddress_p)
{
    uint8_t ScAddressLength = 0;

    ScAddress_p->Length = 0;

    if (thisShortMessage == NULL) {
        SMS_A_(SMS_LOG_E("sm.c: Error - input param of ShortMessage_GetPackedServiceCentreAddress is NULL!"));
        return;
    }

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        /*
         * Determine Length of ScAddress Data Field.
         * NOTE:
         * Ensure Service Centre address does not exceed the Maximum
         * Service Centre Address Length
         */
        ScAddressLength = SMS_MIN((unsigned) (thisShortMessage->TPDUObject_p->TpduAddress.Length + 1), sizeof(ShortMessagePackedAddress_t));
        memset(ScAddress_p, 0x00, sizeof(ShortMessagePackedAddress_t));
        memcpy(ScAddress_p, &thisShortMessage->TPDUObject_p->TpduAddress, ScAddressLength);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetPackedServiceCentreAddress
 *
 * @description  Set the Service Centre Address of the specified Short Message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *ScAddress_p     : as type of ShortMessagePackedAddress_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetPackedServiceCentreAddress(
    ShortMessage_p const thisShortMessage,
    ShortMessagePackedAddress_t * ScAddress_p)
{
    uint8_t ScAddressLength;

    ScAddressLength = 0;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        /*
         * Determine Length of ScAddress Data Field.
         * NOTE:
         * Ensure Service Centre address does not exceed the Maximum
         * Service Centre Address Length
         */
        ScAddressLength = SMS_MIN((unsigned) (ScAddress_p->Length + 1), sizeof(ShortMessagePackedAddress_t));
        memset(&thisShortMessage->TPDUObject_p->TpduAddress, 0x00, sizeof(ShortMessagePackedAddress_t));
        memcpy(&thisShortMessage->TPDUObject_p->TpduAddress, ScAddress_p, ScAddressLength);

        ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
    }

}


#define TP_VP_MAX_FIELD_SIZE  7

/********************************************************************/
/**
 *
 * @function     ShortMessage_SetPackedDestinationAddress
 *
 * @description  Set the Destination Address of the specified Short Message
 *               NOTE: For use with Submit SM only
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *packedAddress_p : as type of ShortMessagePackedAddress_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetPackedDestinationAddress(
    ShortMessage_p const thisShortMessage,
    ShortMessagePackedAddress_t * packedAddress_p)
{

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        // TPDU header must be reset since Destination Address may be
        // different length than current value. All header elements must
        // therefore be cached and reset.
        uint8_t rejectDuplicates = TPDU_GetField(TP_RD, thisShortMessage->TPDUObject_p->Tpdu, SM_SUBMIT);
        uint8_t validityPeriodFormat = TPDU_GetField(TP_VPF, thisShortMessage->TPDUObject_p->Tpdu, SM_SUBMIT);
        uint8_t statusReportRequest = TPDU_GetField(TP_SRR, thisShortMessage->TPDUObject_p->Tpdu, SM_SUBMIT);
        uint8_t replyPath = TPDU_GetField(TP_RP, thisShortMessage->TPDUObject_p->Tpdu, SM_SUBMIT);
        uint8_t protocolIdentifier = TPDU_GetField(TP_PID, thisShortMessage->TPDUObject_p->Tpdu, SM_SUBMIT);
        uint8_t dataCodingScheme = TPDU_GetField(TP_DCS, thisShortMessage->TPDUObject_p->Tpdu, SM_SUBMIT);
        uint8_t validityPeriod[TP_VP_MAX_FIELD_SIZE];

        (void) TPDU_GetFieldXfr(TP_VP, thisShortMessage->TPDUObject_p->Tpdu, SM_SUBMIT, validityPeriod);

        thisShortMessage->TpduLength = TPDU_SUBMIT_PackHdr(thisShortMessage->TPDUObject_p->Tpdu, rejectDuplicates,      // TP_RD  (Reject Duplicates)
                                   validityPeriodFormat,        // TP_VPF (Validity Period Format)
                                   statusReportRequest, // TP_SRR (Status Report Request)
                                   replyPath,   // TP_RP  (Reply Path)
                                   packedAddress_p,     // TP_DA  (Destination Address)
                                   &protocolIdentifier, // TP_PID (Protocol Identifier)
                                   &dataCodingScheme,   // TP_DCS (Data Coding Scheme)
                                   validityPeriod);     // TP_VP  (Validity Period)

        ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
    }

}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetUnPackedServiceCentreAddress
 *
 * @description  Sets the sm sc address from given unpacked sc address
 *
 * @param        thisShortMessage               : Identifies the sm
 * @param        unpackedServiceCentreAddress_p : the unpacked sc address
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetUnPackedServiceCentreAddress(
    ShortMessage_p const thisShortMessage,
    ShortMessageSubscriberNumber_t * unpackedServiceCentreAddress_p)
{
    ShortMessagePackedAddress_t packedServiceCentreAddress;
    (void) ShortMessage_PackGSMFormatAddress(ADDRESS_LENGTH_IN_BYTES, unpackedServiceCentreAddress_p, &packedServiceCentreAddress);
    ShortMessage_SetPackedServiceCentreAddress(thisShortMessage, &packedServiceCentreAddress);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_SetTPDU
 *
 * @description  From the given Tpdu Unit, populate the following data fields within the specified short message instance.
 *               + Called/Calling Line Identity
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        TPDU_Data_p :      as type of Tpdu_t.
 * @param        TPDU_Length :      as uint8_t.
 *
 * @return       void
 *
 *  Notes:
 *PROBLEM: When you receive a status report the parameter indicator field is optional and the only way to identify
 *it's presence is to know the length of the TPDU.
 *SOLUTION: This function when copying in the TPDU shall initialise the buffer to all zero's which means
 *then it is not a problem for the TPDU parsing functions in smtpdu.c to assume that the TP-PI field is always present.
 *When it it not received it will appear to be zero which means no other fields are present.
 */
/********************************************************************/
void ShortMessage_SetTPDU(
    ShortMessage_p const thisShortMessage,
    Tpdu_t TPDU_Data_p,
    uint8_t const TPDU_Length)
{
    uint8_t VerifiedTpduLength;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        // Copy the Tpdu Unit into the Short Message.
        // DO NOT exceed the length of the TPDU unit allocated
        VerifiedTpduLength = SMS_MIN(TPDU_Length, sizeof(Tpdu_t));
        memset(thisShortMessage->TPDUObject_p->Tpdu, 0, sizeof(Tpdu_t));
        memcpy(thisShortMessage->TPDUObject_p->Tpdu, TPDU_Data_p, VerifiedTpduLength);

        // Initialise the TPDULength of ShortMessage
        thisShortMessage->TpduLength = VerifiedTpduLength;

        ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
    }

}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetMessageReference
 *
 * @description  This function retreives the message reference number from the appropriate position
 *               within the TPDU related to the short message object.
 *
 * @param        thisShortMessage :    as type of ShortMessage_p.
 * @param        *messageReference_p : as uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetMessageReference(
    ShortMessage_p const thisShortMessage,
    uint8_t * messageReference_p)
{
    uint8_t success = FALSE;

    *messageReference_p = 0;

    if (ShortMessage_GetType(thisShortMessage) == SM_SUBMIT || ShortMessage_GetType(thisShortMessage) == SM_STATUS_REPORT) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            *messageReference_p = TPDU_GetField(TP_MR, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));
            success = TRUE;

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }
    }
    return (success);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetMessageReference
 *
 * @description  This function sets the TP-MR field within the TPDU, only valid to do
 *               on certain MO short messages SM_SUBMIT, SM_COMMAND
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        newMsgRef : as uint8_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetMessageReference(
    ShortMessage_p const thisShortMessage,
    uint8_t newMsgRef)
{
    if (ShortMessage_GetType(thisShortMessage) == SM_SUBMIT || ShortMessage_GetType(thisShortMessage) == SM_COMMAND) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            (void) TPDU_SetField(TP_MR, thisShortMessage->TPDUObject_p->Tpdu, thisShortMessage->ShortMessageType, &newMsgRef, 1);

            ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetTpdu
 *
 * @description  Function to retrieve the Tpdu unit
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *TpduLength_p :    as uint8_t.
 * @param        Tpdu_p :           as type of Tpdu_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetTpdu(
    ShortMessage_p thisShortMessage,
    uint8_t * TpduLength_p,
    Tpdu_t Tpdu_p)
{
    uint8_t success = FALSE;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        *TpduLength_p = SMS_MIN(MAXIMUM_TPDU_SIZE, thisShortMessage->TpduLength);
        memcpy(Tpdu_p, thisShortMessage->TPDUObject_p->Tpdu, *TpduLength_p);
        success = (*TpduLength_p > 0);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }
    return (success);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetStoragePosition
 *
 * @description  Set the Slot Number in which the specified short message is stored
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        storagePosition : as uint8_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetStoragePosition(
    ShortMessage_p thisShortMessage,
    SMS_Position_t storagePosition)
{
    thisShortMessage->StoragePosition = storagePosition;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetStoragePosition
 *
 * @description  Get the Slot Number in which the specified short message is stored
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
SMS_Position_t ShortMessage_GetStoragePosition(
    ShortMessage_p thisShortMessage)
{
    return (thisShortMessage->StoragePosition);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_SetStatusReportRecNum
 *
 * @description  Set the record number the associated Status Report
 *               is saved on the SIM in EF_SMSR
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        RecordNumber : storage location in EF_SMSR on the SIM
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetStatusReportRecNum(
    ShortMessage_p thisShortMessage,
    const uint8_t RecordNumber)
{
    thisShortMessage->StatusReportRecNum = RecordNumber;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetStatusReportRecNum
 *
 * @description  Get the record number the associated Status Report
 *               is saved on the SIM in EF_SMSR
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t storage location in EF_SMSR on the SIM
 */
/********************************************************************/
uint8_t ShortMessage_GetStatusReportRecNum(
    ShortMessage_p thisShortMessage)
{
    return (thisShortMessage->StatusReportRecNum);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetCategoryFromClass
 *
 * @description  <Detailed description of the method>
 *
 * @param        thisShortMessage : <add description>.
 *
 * @return       SMS_Category_t
 */
/********************************************************************/
SMS_Category_t ShortMessage_GetCategoryFromClass(
    ShortMessage_p thisShortMessage)
{
    SMS_Category_t MessageCategory;

    switch (ShortMessage_GetMessageClass(thisShortMessage)) {
    case SM_CLASS_0:
        MessageCategory = SMS_CATEGORY_CLASS_ZERO;
        break;
    case SM_CLASS_1_ME_SPECIFIC:
        MessageCategory = SMS_CATEGORY_STANDARD;
        break;
    case SM_CLASS_2_SIM_SPECIFIC:
        MessageCategory = SMS_CATEGORY_SIM_SPECIFIC;
        break;
    case SM_CLASS_3_TE_SPECIFIC:
        MessageCategory = SMS_CATEGORY_TE_SPECIFIC;
        break;
    default:
        MessageCategory = SMS_CATEGORY_STANDARD;
        break;
    }

    return MessageCategory;
}


#ifndef REMOVE_SMSTRG_LAYER
#ifdef CODE_NOT_CURRENTLY_USED  // Kept for reference.
/********************************************************************/
/**
 *
 * @function     ShortMessage_GetVolume
 *
 * @description  Function to see where the short message is stored
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        Volume_p         : ptr to volume string
 *
 * @return       uint8_t
 */
/********************************************************************/
void ShortMessage_GetVolume(
    ShortMessage_p thisShortMessage,
    MSG_StorageMedia_t * Volume_p)
{
    SMS_Position_t StoragePosition;

    StoragePosition = ShortMessage_GetStoragePosition(thisShortMessage);

    if (SMS_STORAGE_POSITION_INVALID != StoragePosition) {
        (void) SmStorage_GetStorageVolume(StoragePosition, Volume_p);
    }
}
#endif
#endif                          // REMOVE_SMSTRG_LAYER


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetProtocolIdentifier
 *
 * @description  Get the protocol identifier for a short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetProtocolIdentifier(
    ShortMessage_p thisShortMessage)
{
    uint8_t protocolIdentifier = 0;     // represents unsupported value for TP-PID

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        protocolIdentifier = TPDU_GetField(TP_PID, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (protocolIdentifier);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetReplaceType
 *
 * @description  Get the replace type for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetReplaceType(
    ShortMessage_p thisShortMessage)
{
    uint8_t replaceType = 0;

    if (ShortMessage_GetType(thisShortMessage) == SM_DELIVER) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            replaceType = TPDU_Get(REPLACEMENT_CODE, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }
    }
    return (replaceType);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetTransferProtocol
 *
 * @description  This function returns the transfer protocol indicated in the TP_Protocol_Identifier IE.
 *               The PID value may contain other info apart from the transfer protocol (for telematic interworking)
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       SMTransferProtocol_t
 */
/********************************************************************/
SMTransferProtocol_t ShortMessage_GetTransferProtocol(
    ShortMessage_p thisShortMessage)
{
    SMTransferProtocol_t transferProtocol = SMTYPE_STANDARD;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        transferProtocol = TPDU_Get(TRANSFER_PROTOCOL, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }
    return (transferProtocol);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetAlphabet
 *
 * @description  Get Alphabet for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetAlphabet(
    ShortMessage_p thisShortMessage)
{
    uint8_t alphabet = SM_DEFAULT_CODING;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        alphabet = TPDU_Get(ALPHABET, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));
        switch (alphabet) {
        default:               //deliberate drop through
        case ALPHABET_7_BIT:
            alphabet = SM_DEFAULT_CODING;
            break;
        case ALPHABET_8_BIT:
            alphabet = SM_8BIT_CODING;
            break;
        case ALPHABET_16_BIT:
            alphabet = SM_UCS2_CODING;
            break;
        }

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (alphabet);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetDataCodingScheme
 *
 * @description  Get the data coding scheme for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetDataCodingScheme(
    ShortMessage_p thisShortMessage)
{
    uint8_t datacodingscheme = 0;       // represents default 7-bit coding

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        datacodingscheme = TPDU_GetField(TP_DCS, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (datacodingscheme);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetMessageClass
 *
 * @description  TPDU_GetMessageClass returns the short message class as indicated in TP_Data_Coding_Scheme.
 *               TPDU_p points to the first octet of the TPDU
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       SMClass_t
 */
/********************************************************************/
SMClass_t ShortMessage_GetMessageClass(
    ShortMessage_p thisShortMessage)
{
    SMClass_t messageClass = SM_CLASS_UNKNOWN;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        messageClass = TPDU_Get(CLASS, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return messageClass;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_Derive_RP_ErrorCause
 *
 * @description  This function derives an RP Error Cause from a TP failure cause.
 *
 * @param        TP_FailureCause : as type of TP_FailureCause_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
static SMS_Error_t ShortMessage_Derive_RP_ErrorCause(
    const TP_FailureCause_t TP_FailureCause)
{
    SMS_Error_t DerivedErrorCause = SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED;

    switch (TP_FailureCause) {
    case TP_NO_CAUSE:
        {
            DerivedErrorCause = SMS_ERROR_INTERN_NO_ERROR;
        }
        break;
    case TP_MEMORY_CAPACITY_EXCEEDED:
        {
            DerivedErrorCause = SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED;
        }
        break;
    case TP_UNSPECIFIED_ERROR_CAUSE:   // deliberate drop through
    default:
        break;
    }                           //if it's any others convertedErrorCause will remain as SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED

    return DerivedErrorCause;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_Acknowledge
 *
 * @description  This function sends an application acknowledgement to the Short Message Router
 *
 * @param        thisShortMessage  : as type of ShortMessage_p.
 * @param        TP_FailureCause   : as type of TP_FailureCause_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_Acknowledge(
    ShortMessage_p thisShortMessage,
    TP_FailureCause_t TP_FailureCause)
{
    ShortMessage_p deliveryShortMessage;
    EventData_t eventData;
    uint8_t protocolIdentifier = 0;     // represents unsupported value for TP-PID
    uint8_t dataCodingScheme = 0;       // represents unsupported value for TP-DCS

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        protocolIdentifier = TPDU_GetField(TP_PID, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        dataCodingScheme = TPDU_GetField(TP_DCS, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    if (TP_FailureCause == TP_NO_CAUSE) {
        deliveryShortMessage = ShortMessage_Create(SM_DELIVER_REPORT_ACK);
    } else {
        deliveryShortMessage = ShortMessage_Create(SM_DELIVER_REPORT_ERROR);
    }
    if (deliveryShortMessage != NULL) {
        // Return the original TP-PID and TP-DCS values
        (void) ShortMessage_PackTPDUHeader(deliveryShortMessage, TP_FailureCause,       // TP-FCS  (Failure Cause)
                                           &protocolIdentifier, // TP-PID  (Protocol Identifier)
                                           &dataCodingScheme,   // TP_DCS  (Data Coding Scheme)
                                           FALSE,       // TP_SRR  (Status Report Request)
                                           FALSE,       // TP_RP   (Reply Path Set)
                                           NULL,        // TP_VP   (Validity Period)
                                           NULL,        // TP_DA   (Destination Address)
                                           NULL,        // TP_CT   (Command type)
                                           NULL // TP_MN   (Message Number)
            );

        deliveryShortMessage->AcknowledgeRequired = thisShortMessage->AcknowledgeRequired;
        eventData.ShortMessage = deliveryShortMessage;
    } else {
        // No space to create the TPDU, but we can still send the Deliver Report
        // without TPDU as it is optional
        eventData.ShortMessage = NULL;
    }
    eventData.RMS_V2_ErrorCode = ShortMessage_Derive_RP_ErrorCause(TP_FailureCause);
    ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_APPLICATION_ACKNOWLEDGE_WITH_CNF, &eventData);

}


/********************************************************************/
/**
 *
 * @function     ShortMessage_AcknowledgementWithConfirm
 *
 * @description  This function sends an application acknowledgement to the Short Message Router.
 *               It is called with a full acknowledgement message.
 *               An asynchronous response will be provided by Network Systems.
 *
 * @param        deliveryShortMessage : as type of ShortMessage_p.
 * @param        RMS_V2_ErrorCode     : as type of SMS_RP_ErrorCause_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessage_AcknowledgementWithConfirm(
    ShortMessage_p deliveryShortMessage,
    const SMS_RP_ErrorCause_t RMS_V2_ErrorCode)
{
    EventData_t eventData;

    eventData.RMS_V2_ErrorCode = RMS_V2_ErrorCode;
    eventData.ShortMessage = deliveryShortMessage;
    eventData.ErrorCode = SMS_ERROR_INTERN_NO_ERROR;

    ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_APPLICATION_ACKNOWLEDGE_WITH_CNF, &eventData);

    return eventData.ErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_IsStatusReportRequested
 *
 * @description  Function to check if the Status Report is requested for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsStatusReportRequested(
    ShortMessage_p const thisShortMessage)
{
    uint8_t statusReportRequested = FALSE;

    if (ShortMessage_GetType(thisShortMessage) == SM_SUBMIT) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            statusReportRequested = TPDU_TestBit(TP_SRR, thisShortMessage->TPDUObject_p->Tpdu);

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }
    }

    return (statusReportRequested);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetStatusReportRequest
 *
 * @description  Set the Status Report Request for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        newSetting : uint8_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetStatusReportRequest(
    ShortMessage_p const thisShortMessage,
    uint8_t newSetting)
{
    uint8_t alteredTPDU = FALSE;
    uint8_t statusReportRequested = FALSE;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        statusReportRequested = TPDU_TestBit(TP_SRR, thisShortMessage->TPDUObject_p->Tpdu);

        if (statusReportRequested != newSetting) {
            TPDU_SetBit(TP_SRR, thisShortMessage->TPDUObject_p->Tpdu, newSetting);
            alteredTPDU = TRUE;
        }

        ShortMessage_UnloadTPDU(thisShortMessage, alteredTPDU);
    }

}



/********************************************************************/
/**
 *
 * @function     ShortMessage_GetValidityPeriod
 *
 * @description  Get the validity period for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetValidityPeriod(
    ShortMessage_p const thisShortMessage)
{
    uint8_t validityPeriod = SMS_VALIDPER_MAXIMUM_PERIOD;

    if (ShortMessage_GetType(thisShortMessage) == SM_SUBMIT) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            if (TPDU_GetBit(TP_VPF, thisShortMessage->TPDUObject_p->Tpdu) == TP_VPF_RELATIVE_FORMAT) {
                validityPeriod = TPDU_GetField(TP_VP, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));
            }

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }
    }

    return (validityPeriod);
}



/********************************************************************/
/**
 *
 * @function     ShortMessage_Send
 *
 * @description  This function triggers the ShortMessageBearer object to send a short message
 *               or a command to the network. You supply a function pointer of the type
 *               CallbackFunction_p which will get called with FailureCause and ShortMessage_p.
 *               If FailureCause is == 0 then that ShortMessage_p has been successfully sent.
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        callbackFunction : as type of CallbackFunction_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_Send(
    ShortMessage_p thisShortMessage,
    CallbackFunction_p callbackFunction)
{
    EventData_t eventData;
    uint8_t refNo = 0;

    eventData.ShortMessage = thisShortMessage;
    eventData.CallbackFunction = callbackFunction;

    if ((ShortMessage_GetType(thisShortMessage) == SM_SUBMIT || ShortMessage_GetType(thisShortMessage) == SM_COMMAND) && (ShortMessage_GetStatus(thisShortMessage) == SMS_STATUS_UNSENT)) {
        if (SMS_MO_RETRY_IF_MESSAGE_REFERENCE_SET == 1) {
            uint8_t currentRefNo;

            (void) ShortMessage_GetMessageReference(thisShortMessage, &currentRefNo);

            if (currentRefNo == SMS_DEFAULT_TP_MR) {
                // Attempt to send the short message so set the TP-MR
                refNo = SmUtil_TP_MessageReferenceReadAndIncrement();
                ShortMessage_SetMessageReference(thisShortMessage, refNo);
            } else {
                // This is a retry of a failed send so set TP-RD
                (void) ShortMessage_SetRejectDuplicates(thisShortMessage);
            }
        } else {
            // Attempt to send the short message so set the TP-MR
            refNo = SmUtil_TP_MessageReferenceReadAndIncrement();
            ShortMessage_SetMessageReference(thisShortMessage, refNo);
        }

#ifndef REMOVE_SMSTRG_LAYER
        if (ShortMessage_IsStatusReportRequested(thisShortMessage)) {
            ShortMessage_SetDeliveredStatus(thisShortMessage, STATUS_REPORT_UNKNOWN);
        } else {
            ShortMessage_SetDeliveredStatus(thisShortMessage, STATUS_REPORT_RESET);
        }
#endif                          // REMOVE_SMSTRG_LAYER

        SMS_B_(SMS_LOG_D("sm.c: ShortMessage_Send: Sending"));
        ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND, &eventData);
    } else {
        SMS_A_(SMS_LOG_E("sm.c: ShortMessage_Send: failed type test not sending type %d status %d", ShortMessage_GetType(thisShortMessage), ShortMessage_GetStatus(thisShortMessage)));
        (callbackFunction) (thisShortMessage, SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MO);
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_PackTPDUHeader
 *
 * @description  This function packs the TPDU header according to the short message type.
 *
 * @param        thisShortMessage :      as type of ShortMessage_p.
 * @param        failureCause :          as type of TP_FailureCause_t.
 * @param        *protocolIdentifier_p : uint8_t type.
 * @param        *dataCodingScheme_p :   as type of .
 * @param        requestStatusReport :   uint8_t.
 * @param        replyPathSet :          uint8_t.
 * @param        *validityPeriod_p :     uint8_t type.
 * @param        destAddress_p :         as type of ShortMessageSubscriberNumber_t.
 * @param        *commandType_p :        uint8_t type.
 * @param        *messageNumber_p :      uint8_t type.
 *
 * @return       uint8_t
 *
 * Notes:
 * 1) The presence of mandatory elements is checked before the TPDU_PackHeaderIEs is called.
 *    If no mandatory values are supplied, the function returns 0 as TPDULength.
 * 2) If the message header was successfully packed, the function returns the TPDU length.
 *    A different function will deal with packing of User Data, where TPDULength will be updated.
 */
/********************************************************************/
uint8_t ShortMessage_PackTPDUHeader(
    ShortMessage_p thisShortMessage,
    TP_FailureCause_t failureCause,
    uint8_t * protocolIdentifier_p,
    SMS_DataCodingScheme_t * dataCodingScheme_p,
    uint8_t requestStatusReport,
    uint8_t replyPathSet,
    uint8_t * validityPeriod_p,
    ShortMessageSubscriberNumber_t * destAddress_p,
    uint8_t * commandType_p,
    uint8_t * messageNumber_p)
{
    ShortMessagePackedAddress_t destAddressGSM;

    uint8_t TPDULength = 0;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        switch (ShortMessage_GetType(thisShortMessage)) {
        case SM_DELIVER_REPORT_ACK:
            // Positive ack to network
            failureCause = TP_NO_CAUSE;
            // deliberate drop through on case
        case SM_DELIVER_REPORT_ERROR:
            {
                // Negative ack to network
                // The only mandatory field in this case is TP_Parameter_Indicator
                // and it will be dealt with on the TPDU level
                TPDULength = TPDU_DELIVER_REPORT_PackHdr(thisShortMessage->TPDUObject_p->Tpdu, failureCause,    // TP_FCS (Failure Cause)
                                                         protocolIdentifier_p,  // TP_PID (Protocol Identifier)
                                                         dataCodingScheme_p);   // TP_DCS (Data Coding Schems)
            }
            break;

        case SM_SUBMIT:
            {
                // Mobile originated short message
                if ((destAddress_p != NULL) && (protocolIdentifier_p != NULL) && (dataCodingScheme_p != NULL) && (strlen((char *) destAddress_p->Digits) <= SMS_MAX_DIGITS_IN_NUMBER)) {
                    (void) ShortMessage_PackGSMFormatAddress(ADDRESS_LENGTH_IN_DIGITS, destAddress_p, &destAddressGSM);

                    TPDULength = TPDU_SUBMIT_PackHdr(thisShortMessage->TPDUObject_p->Tpdu, FALSE,       // TP_RD  (Reject Duplicates)
                                                     TP_VPF_RELATIVE_FORMAT,    // TP_VPF (Validity Period Format)
                                                     requestStatusReport,       // TP_SRR (Status Report Request)
                                                     replyPathSet,      // TP_RP  (Reply Path)
                                                     (ShortMessagePackedAddress_t *) & destAddressGSM,  // TP_DA  (Destination Address)
                                                     protocolIdentifier_p,      // TP_PID (Protocol Identifier)
                                                     dataCodingScheme_p,        // TP_DCS (Data Coding Scheme)
                                                     validityPeriod_p); // TP_VP  (Validity Period)
                }               //else TPDULength == 0 i.e. the function will fail
            }
            break;

        case SM_COMMAND:
            {
                // Mobile originated command (SMS_COMMAND)
                if ((destAddress_p != NULL) &&
                    (protocolIdentifier_p != NULL) && (commandType_p != NULL) && (messageNumber_p != NULL) && (strlen((char *) destAddress_p->Digits) <= SMS_MAX_DIGITS_IN_NUMBER)) {
                    // Before we call the TPDU object operation, let's pack the
                    // destination address and validity period into the GSM format
                    (void) ShortMessage_PackGSMFormatAddress(ADDRESS_LENGTH_IN_DIGITS, destAddress_p, &destAddressGSM);

                    TPDULength = TPDU_COMMAND_PackHdr(thisShortMessage->TPDUObject_p->Tpdu, requestStatusReport,        // TP_SRR (Status Report Request)
                                                      protocolIdentifier_p,     // TP_PID (Protocol Identifier)
                                                      commandType_p,    // TP_CT  (Command Type)
                                                      messageNumber_p,  // TP_MN  (Message Number)
                                                      (ShortMessagePackedAddress_t *) & destAddressGSM);        // TP_DA  (Destination Address)
                }
            }
            break;
        default:
            break;
        }                       // switch

        thisShortMessage->TpduLength = TPDULength;

        ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
    }

    return (TPDULength);
}                               // ShortMessage_PackTPDUHeader


/********************************************************************/
/**
 *
 * @function     AlphabetToDCS
 *
 * @description  Function to convert Alphabet to Data Coding Scheme (DCS)
 *
 * @param        alphabet : uint8_t type.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t AlphabetToDCS(
    uint8_t alphabet)
{
    const uint8_t alphabetToDCS[] = { ALPHABET_7_BIT, ALPHABET_8_BIT, ALPHABET_16_BIT };

    if (alphabet > 2) {
        alphabet = 0;
    }

    return (alphabetToDCS[alphabet]);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetObject
 *
 * @description  Function to get objects from a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *buffer_p :        uint8_t type.
 * @param        objectNumber :     uint8_t type.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetObject(
    ShortMessage_p thisShortMessage,
    uint8_t * buffer_p,
    uint8_t objectNumber)
{
    uint8_t ieBytes = 0;
    uint8_t ieInstance = 0;
    uint8_t objectCounter = 0;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        while ((objectCounter != (objectNumber + 1))    //plus one because object number is 0-n
               && ((ieBytes = TPDU_GetUserDataIE(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), ANY_IE_TYPE,     // Want to parse all IE's
                                                 ieInstance++, buffer_p)) > 0)) {
            if ((eSMS_IEI_codes_t) * buffer_p >= FIRST_EMS_IEI && (eSMS_IEI_codes_t) * buffer_p <= LAST_VALID_IEI_VALUE) {
                objectCounter++;
            } else {
                ieBytes = 0;
            }
        }

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (ieBytes);
}



/********************************************************************/
/**
 *
 * @function     ShortMessage_GetIE
 *
 * @description  Function to get IE from a specific short message
 *
 * @param        thisShortMessage
 * @param        buffer_p
 * @param        objectNumber
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetIE(
    ShortMessage_p thisShortMessage,
    uint8_t * buffer_p,
    uint8_t objectNumber)
{
    uint8_t ieBytes = 0;
    uint8_t ieInstance = 0;
    uint8_t objectCounter = 0;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        while ((objectCounter != (objectNumber + 1))    //plus one because object number is 0-n
               && ((ieBytes = TPDU_GetUserDataIE(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), ANY_IE_TYPE,     // Want to parse all IE's
                                                 ieInstance++, buffer_p)) > 0)) {
            objectCounter++;
        }

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (ieBytes);
}



/********************************************************************/
/**
 *
 * @function     ShortMessage_PackMessageBody
 *
 * @description  Function to pack the message body for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        textLength :       uint8_t.
 * @param        alphabet :         uint8_t.
 * @param        *userDataText_p :  uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_PackMessageBody(
    ShortMessage_p thisShortMessage,
    uint8_t textLength,
    uint8_t alphabet,
    uint8_t * userDataText_p)
{
    uint8_t TPDULength = 0;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        TPDULength = TPDU_PackText(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), textLength, AlphabetToDCS(alphabet), userDataText_p);

        thisShortMessage->TpduLength = TPDULength;

        ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
    }

    return TPDULength;
}                               // ShortMessage_PackMessageBody


/********************************************************************/
/**
 *
 * @function     ShortMessage_CopyMessageBody
 *
 * @description  Function to copy the message body for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        textLength :       uint8_t.
 * @param        alphabet :         uint8_t.
 * @param        *userDataText_p :  uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_CopyMessageBody(
    ShortMessage_p thisShortMessage,
    uint8_t textLength,
    uint8_t alphabet,
    uint8_t * userDataText_p)
{
    uint8_t TPDULength = 0;
    uint8_t textAlphabet = ALPHABET_7_BIT;      // default value

    switch (alphabet) {
    case SM_DEFAULT_CODING:
        {
            textAlphabet = ALPHABET_7_BIT;
        }
        break;

    case SM_8BIT_CODING:
        {
            textAlphabet = ALPHABET_8_BIT;
        }
        break;

    case SM_UCS2_CODING:
        {
            textAlphabet = ALPHABET_16_BIT;
        }
        break;

    default:
        {
            SMS_A_(SMS_LOG_E("sm.c: Error - Unknown DCS received in ShortMessage_CopyMessageBody"));
            // textAlphabet will remain at its initialised value
        }
        break;
    }

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        //clear UDHI & UDL
        TPDU_SetBit(TP_UDHI, thisShortMessage->TPDUObject_p->Tpdu, 0);

        TPDULength = TPDU_CopyText(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), textLength, textAlphabet, userDataText_p);

        thisShortMessage->TpduLength = TPDULength;

        ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
    }

    return TPDULength;
}                               // ShortMessage_CopyMessageBody


/********************************************************************/
/**
 *
 * @function     ShortMessage_ResetUserDataHeader
 *
 * @description  Reset the user data header ready to construct a new message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_ResetUserDataHeader(
    ShortMessage_p thisShortMessage)
{
    uint8_t TPDULength = 0;

    if (thisShortMessage->ShortMessageType == SM_SUBMIT) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            TPDULength = TPDU_ResetUserDataIE(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

            // Set the TPDULength in the Short Message Object
            thisShortMessage->TpduLength = TPDULength;

            ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
        }
    }
}                               // ShortMessage_ResetUserDataHeader



/********************************************************************/
/**
 *
 * @function     ShortMessage_AddIEToUserDataHeader
 *
 * @description  Function to add IE to the user data header for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        alphabet :         uint8_t.
 * @param        ieType :           uint8_t.
 * @param        ieLength :         uint8_t.
 * @param        *ieData_p :        uint8_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_AddIEToUserDataHeader(
    ShortMessage_p thisShortMessage,
    uint8_t alphabet,
    uint8_t ieType,
    uint8_t ieLength,
    uint8_t * ieData_p)
{
    uint8_t TPDULength = 0;

    if (thisShortMessage->ShortMessageType == SM_SUBMIT || thisShortMessage->ShortMessageType == SM_DELIVER_REPORT_ACK) {
        // Convert dataCodingScheme to the DCS value first
        uint8_t dcs = AlphabetToDCS(alphabet);

        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            TPDULength = TPDU_AddUserDataIE(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), dcs, ieType, ieLength, ieData_p);

            // Set the TPDULength in the Short Message Object
            thisShortMessage->TpduLength = TPDULength;

            ShortMessage_UnloadTPDU(thisShortMessage, TRUE);
        }
    }
}


#ifndef REMOVE_SMSTRG_LAYER
/********************************************************************/
/**
 *
 * @function     ShortMessage_Store
 *
 * @description  Save the short message to a free position in memory
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       SmStoreErrorCode_t
 */
/********************************************************************/
SmStoreErrorCode_t ShortMessage_Store(
    ShortMessage_p thisShortMessage)
{
    SmStoreErrorCode_t ErrorCode = SM_STORE_FAILED;
    uint8_t SpaceFound = FALSE;
    MSG_StorageMedia_t *AvailableMedia_p = NULL;

    if (ShortMessage_GetType(thisShortMessage) == SM_DELIVER || ShortMessage_GetType(thisShortMessage) == SM_SUBMIT) {
        AvailableMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

        //Find space in the preferred store list
        SpaceFound = SmStorage_GetAvailableVolume(AvailableMedia_p, 1, FALSE);

        if (SpaceFound == TRUE) {
            // Now we have the media - save onto it.
            (void) SmStorage_SmSaveInVolume(thisShortMessage, AvailableMedia_p, &ErrorCode);
        }

        SMS_HEAP_FREE(&AvailableMedia_p);
    }

    return (ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_StoreME
 *
 * @description  Stores the sm in ME storage
 *
 * @param        thisShortMessage : The short mesasge to store.
 *
 * @return       SmStoreErrorCode_t
 */
/********************************************************************/
SmStoreErrorCode_t ShortMessage_StoreME(
    ShortMessage_p thisShortMessage)
{
    SmStoreErrorCode_t ErrorCode = SM_STORE_FAILED;
    uint8_t SpaceFound = FALSE;
    MSG_StorageMedia_t *AvailableMedia_p = NULL;

    if (ShortMessage_GetType(thisShortMessage) == SM_DELIVER || ShortMessage_GetType(thisShortMessage) == SM_SUBMIT) {
        AvailableMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

        //Find space in the preferred store list - but not on the SIM
        SpaceFound = SmStorage_GetAvailableVolume(AvailableMedia_p, 1, TRUE);

        if (SpaceFound == TRUE) {
            // Now we have the media - save onto it.
            (void) SmStorage_SmSaveInVolume(thisShortMessage, AvailableMedia_p, &ErrorCode);
        }

        SMS_HEAP_FREE(&AvailableMedia_p);
    }

    return (ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_StoreMedia
 *
 * @description  Save the Specified Short message into the Specified Media
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        MediaType :        uint8_t.
 *
 * @return       SmStoreErrorCode_t
 */
/********************************************************************/
SmStoreErrorCode_t ShortMessage_StoreMedia(
    ShortMessage_p const thisShortMessage,
    MSG_StorageMedia_t * Volume_p)
{
    SmStoreErrorCode_t ErrorCode = SM_STORE_FAILED;
    SMS_Position_t Slot = SMS_STORAGE_POSITION_INVALID;

    if (thisShortMessage->StoragePosition == SMS_STORAGE_POSITION_INVALID && (ShortMessage_GetType(thisShortMessage) == SM_DELIVER || ShortMessage_GetType(thisShortMessage) == SM_SUBMIT)) {

        Slot = SmStorage_SmSaveInVolume(thisShortMessage, Volume_p, &ErrorCode);
        thisShortMessage->StoragePosition = Slot;
    }

    return (ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_Move
 *
 * @description  Save the Specified Short message into the Specified Media
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        MediaType :        uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_Move(
    ShortMessage_p const thisShortMessage,
    MSG_StorageMedia_t * Volume_p)
{
    return (SmStorage_MoveStoredShortMessage(thisShortMessage, Volume_p));
}
#endif                          // REMOVE_SMSTRG_LAYER


/********************************************************************/
/**
 *
 * @function     ShortMessage_IsStatusReportMatch
 *
 * @description  Determines if the given Status Report matches the given
 *               short message.
 *
 * @param        ShortMessage_p : new short message status report.
 * @param        ShortMessage_p : short message address to populate for Caller.
 *
 * @return       static uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsStatusReportMatch(
    const ShortMessage_p shortMessage,
    const ShortMessage_p newStatusReport)
{
    uint8_t messageReference;
    uint8_t oldMessageReference;
    uint8_t StatusReportMatch = FALSE;

    if (ShortMessage_GetMessageReference(newStatusReport, &messageReference)) {
        SMS_Status_t status = ShortMessage_GetStatus(shortMessage);

        if (status == SMS_STATUS_SENT) {
            uint8_t success = ShortMessage_GetMessageReference(shortMessage, &oldMessageReference);

            if (success) {
                SMS_B_(SMS_LOG_D("sm.c: RECEIVESTATUSREPORT: slot %d msgref %d-%d", ShortMessage_GetStoragePosition(shortMessage), oldMessageReference, messageReference));

                if (messageReference == oldMessageReference) {
                    // Found the match so set flag to stop us going round and
                    // round the while loop anymore
                    StatusReportMatch = TRUE;
                }
            }
        }
    }

    return StatusReportMatch;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_IsConcatenated
 *
 * @description  Function to get the concatenation information for a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        ConcatParams_p :   as type of SMS_ConcatParameters_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_IsConcatenated(
    ShortMessage_p thisShortMessage,
    ShortMessageConcatParams_p ConcatParams)
{

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        TPDU_GetConcatInfo(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), ConcatParams);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    } else {
        ConcatParams->ConcatMessageRefNumber = 0;
        ConcatParams->MaxNumberOfConcatMsgs = 0;
        ConcatParams->MsgSequenceNumber = 0;
        ConcatParams->IsConcatenated = FALSE;
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_IsDepersonalisation
 *
 * @description  Function to determine if the specific short message is for depersonalisation
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsDepersonalisation(
    ShortMessage_p thisShortMessage)
{
    uint8_t isDepersonalisation = FALSE;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        isDepersonalisation = TPDU_Get(ME_DEPERSONALIZATION, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return isDepersonalisation;
}



/********************************************************************/
/**
 *
 * @function     ShortMessage_IsSIMDataDownload
 *
 * @description  Function to determine if the specific short message is for SIM data download
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsSIMDataDownload(
    ShortMessage_p thisShortMessage)
{
    uint8_t isSimdatadownload = FALSE;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        isSimdatadownload = TPDU_Get(SIM_DOWNLOAD, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return isSimdatadownload;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_IsEnhanced
 *
 * @description  Tests to see if there are any IEI's that fall within the range specified for eSMS             .
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsEnhanced(
    ShortMessage_p thisShortMessage)
{
    uint8_t isEnhanced = FALSE;
    uint8_t ieInstance = 0;
    TPDU_p buffer_p = NULL;

    //allocate a temporary buffer
    buffer_p = SMS_HEAP_UNTYPED_ALLOC(MAXIMUM_TPDU_SIZE);

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        while (TPDU_GetUserDataIE(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), ANY_IE_TYPE,    // Want to parse all IE's
                                  ieInstance++, buffer_p)
               && !isEnhanced) {
            if ((*buffer_p >= FIRST_EMS_IEI) && (*buffer_p <= LAST_VALID_IEI_VALUE)) {
                isEnhanced = TRUE;
            }
        }

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }
    //free the temporary buffer
    SMS_HEAP_FREE(&buffer_p);

    return (isEnhanced);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_ContainsIEType
 *
 * @description  Tests to see if there are any IEI's of a specific type
 *
 * @param        fThisShortMessage : as type of ShortMessage_p.
 * @param        fIEType : type of IE to check forShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_ContainsIEType(
    ShortMessage_p fThisShortMessage,
    const uint8_t fIEType)
{
    uint8_t lIsType = FALSE;
    TPDU_p lBuffer_p = NULL;

    //allocate a temporary buffer
    lBuffer_p = SMS_HEAP_UNTYPED_ALLOC(MAXIMUM_TPDU_SIZE);

    ShortMessage_LoadTPDU(fThisShortMessage);

    if (fThisShortMessage->TPDUObject_p != NULL) {
        if (TPDU_GetUserDataIE(fThisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(fThisShortMessage), fIEType, 0, lBuffer_p)) {
            lIsType = TRUE;
        }

        ShortMessage_UnloadTPDU(fThisShortMessage, FALSE);
    }
    //free the temporary buffer
    SMS_HEAP_FREE(&lBuffer_p);

    return (lIsType);
}



/********************************************************************/
/**
 *
 * @function     ShortMessage_IsMessageWaitingIndication
 *
 * @description  Function to determine if the specific short message is of Message Waiting indication
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        messageWaiting :   as type of MWI_List_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsMessageWaitingIndication(
    ShortMessage_p thisShortMessage,
    MWIList_p messageWaiting)
{
    uint8_t i;
    uint8_t isMsgWaiting = FALSE;

    // Initialise the Message Waiting MSG_List (empty at this point)
    for (i = 0; i < MAX_LENGTH_OF_MSG_WAITING_IND_LIST; i++) {
        messageWaiting->MWI_Indication[i].Type = INVALID_MESSAGE_WAITING_TYPE;
        messageWaiting->MWI_Indication[i].Count = 0;
    }

    messageWaiting->MWIList_Index = 0;
    messageWaiting->ALS_LineDetermined = FALSE;
    messageWaiting->TextAssociated = FALSE;
    messageWaiting->DiscardShortMessage = TRUE;

    // First check the specific PCN (DCS1800) voice mail info
    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        isMsgWaiting = TPDU_GetVoiceMailInfo(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), messageWaiting);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }
    // If no message waiting indication present, set the DiscardShortMessage to FALSE
    if (isMsgWaiting == FALSE) {
        messageWaiting->DiscardShortMessage = FALSE;
    }
    return (isMsgWaiting);
}                               // ShortMessage_IsMessageWaitingIndication()




/********************************************************************/
/**
 *
 * @function     ShortMessage_IsStored
 *
 * @description  Finds out whether a short message is stored
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsStored(
    ShortMessage_p thisShortMessage)
{
    uint8_t SmStored = FALSE;
    if (thisShortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID) {
        SmStored = TRUE;
    }
    return SmStored;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_IsExtendedObjectDataReqCmd
 *
 * @description  Finds out whether a short message is contains an
 *               Extended Object Data Request Command
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        DataPresent_p    : assigned TRUE if there is any data in the SM.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsExtendedObjectDataReqCmd(
    ShortMessage_p thisShortMessage,
    uint8_t * const DataPresent_p)
{
    uint8_t isExtendedObjectDataReqCmd = FALSE;
    uint8_t userDataLength = 0;
    uint8_t alphabet = SM_DEFAULT_CODING;
    TPDU_p buffer_p = NULL;

    //allocate a temporary buffer
    if ((buffer_p = SMS_HEAP_UNTYPED_ALLOC(MAXIMUM_TPDU_SIZE)) == NULL) {
        SMS_A_(SMS_LOG_E("sm.c: Error - ShortMessage_IsExtendedObjectDataReqCmd: Memory Allocation Failed"));
    } else {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            if (TPDU_GetUserDataIE(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), EXTENDED_OBJECT_DATA_REQUEST_CMD, 0, buffer_p)) {
                isExtendedObjectDataReqCmd = TRUE;
            }
            // Determine if the TPDU contains any data
            userDataLength = TPDU_GetField(TP_UDL, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

            alphabet = TPDU_Get(ALPHABET, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

            if (alphabet == ALPHABET_7_BIT) {
                *DataPresent_p = (userDataLength > EODRC_USER_DATA_SEPTETS);
            } else {
                *DataPresent_p = (userDataLength > EODRC_USER_DATA_BYTES);
            }

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }
        //free the temporary buffer
        SMS_HEAP_FREE(&buffer_p);
    }

    return (isExtendedObjectDataReqCmd);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_ServiceCentreAddressCompare
 *
 * @description  Function to compare the service centre address of two specific short messages
 *
 * @param        shortMessageOne : as type of ShortMessage_p.
 * @param        shortMessageTwo : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_ServiceCentreAddressCompare(
    ShortMessage_p shortMessageOne,
    ShortMessage_p shortMessageTwo)
{
    uint8_t match = FALSE;
    uint8_t numberOfBytes = 0;
    ShortMessagePackedAddress_t serviceCentreAddressOne;

    // Initialise Length
    serviceCentreAddressOne.Length = 0;
    serviceCentreAddressOne.TypeOfAddress = 0;
    memset(serviceCentreAddressOne.AddressValue, 0, SMS_MAX_SM_ADDRESS_LENGTH);

    //get service centre address from first shortMessage
    ShortMessage_LoadTPDU(shortMessageOne);

    if (shortMessageOne->TPDUObject_p != NULL) {
        memcpy(&serviceCentreAddressOne, &shortMessageOne->TPDUObject_p->TpduAddress, sizeof(ShortMessagePackedAddress_t));

        ShortMessage_UnloadTPDU(shortMessageOne, FALSE);
    }
    //get the second one
    ShortMessage_LoadTPDU(shortMessageTwo);

    if (shortMessageTwo->TPDUObject_p != NULL) {
        //get longest of the two address lengths
        numberOfBytes = SMS_MAX(serviceCentreAddressOne.Length, shortMessageTwo->TPDUObject_p->TpduAddress.Length);

        //add one for the length of the length (with protection)
        numberOfBytes = SMS_MIN((unsigned) (numberOfBytes + 1), sizeof(ShortMessagePackedAddress_t));

        if (memcmp(&serviceCentreAddressOne, &shortMessageTwo->TPDUObject_p->TpduAddress, numberOfBytes) == 0) {
            match = TRUE;
        }

        ShortMessage_UnloadTPDU(shortMessageTwo, FALSE);
    }

    return (match);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetPackedTimeStamp
 *
 * @description  Function to get packed Time Stamp for a specific short message
 *
 * @param        thisShortMessage :  as type of ShortMessage_p.
 * @param        packedTimeStamp_p : as type of PackedTimeStamp_t.
 *
 * @return       static void
 */
/********************************************************************/
void ShortMessage_GetPackedTimeStamp(
    ShortMessage_p thisShortMessage,
    PackedTimeStamp_t packedTimeStamp_p)
{
    memset(packedTimeStamp_p, 0, SMS_TIME_STAMP_LENGTH);
    if ((ShortMessage_GetType(thisShortMessage) == SM_DELIVER) || (ShortMessage_GetType(thisShortMessage) == SM_STATUS_REPORT)) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            (void) TPDU_GetFieldXfr(TP_SCTS, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), packedTimeStamp_p);

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetMessageDirection
 *
 * @description  ShortMessage_GetMessageDirection returns the short message
 *               direction based on the Message Type
 *
 *               ShortMessage_p points to the first octet of the short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       SMClass_t
 */
/********************************************************************/
SmMessageDirection_t ShortMessage_GetMessageDirection(
    ShortMessage_p thisShortMessage)
{

    ShortMessageType_t ShortMessageType;
    SmMessageDirection_t MessageDirection;

    ShortMessageType = ShortMessage_GetType(thisShortMessage);

    switch (ShortMessageType) {
    case SM_DELIVER:
    case SM_SUBMIT_REPORT_ERROR:
    case SM_STATUS_REPORT:
    case SM_SUBMIT_REPORT_ACK:
        {
            MessageDirection = SCtoMS;
            break;
        }

    case SM_DELIVER_REPORT_ERROR:
    case SM_SUBMIT:
    case SM_COMMAND:
    case SM_DELIVER_REPORT_ACK:
        {
            MessageDirection = MStoSC;
            break;
        }

    default:
        {
            // must be SM_TYPE_LAST or some other invalid value
            SMS_A_(SMS_LOG_E("sm.c: ShortMessage_GetMessageDirection: invalid Short Message type"));
            MessageDirection = MAX_SM_MESSAGE_DIRECTION;
        }
    }

    return MessageDirection;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetText
 *
 * @description  Function to get text from a specific short message
 *
 * @param        thisShortMessage :        as type of ShortMessage_p.
 * @param        *thisShortMessageText_p : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_GetText(
    ShortMessage_p thisShortMessage,
    uint8_t * thisShortMessageText_p)
{
    uint8_t numberOfBytes = 0;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        numberOfBytes = TPDU_UnpackText(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), thisShortMessageText_p);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (numberOfBytes);
}                               // ShortMessage_GetText


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetUnpackedOrigOrDestAddress
 *
 * @description  Function to get the unpacked origination or destination address from a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *digits :          uint8_t.
 * @param        *typeOfNumber :    uint8_t.
 * @param        *numberingPlanId : uint8_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_GetUnpackedOrigOrDestAddress(
    ShortMessage_p thisShortMessage,
    uint8_t * digits,
    uint8_t * typeOfNumber,
    uint8_t * numberingPlanId)
{
    ShortMessageSubscriberNumber_t unpackedAddress;
    ShortMessagePackedAddress_t packedAddress;

    // Initialise digits
    memset(digits, 0x00, SMS_MAX_DIGITS_IN_NUMBER + 1);

    *typeOfNumber = 0;
    *numberingPlanId = 0;
    if (thisShortMessage->ShortMessageType == SM_STATUS_REPORT ||
        thisShortMessage->ShortMessageType == SM_DELIVER || thisShortMessage->ShortMessageType == SM_SUBMIT || thisShortMessage->ShortMessageType == SM_COMMAND) {
        ShortMessage_GetPackedOrigOrDestAddress(thisShortMessage, &packedAddress);
        UnpackAddressField(&packedAddress, &unpackedAddress);
        strcpy((char *) digits, (char *) unpackedAddress.Digits);
        *typeOfNumber = unpackedAddress.TypeOfNumber;
        *numberingPlanId = unpackedAddress.NumberingPlanId;
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetPackedOrigOrDestAddress
 *
 * @description  Function to get packed origination or destination address of a specific short message
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *packedAddress_p : as type of ShortMessagePackedAddress_t.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_GetPackedOrigOrDestAddress(
    ShortMessage_p thisShortMessage,
    ShortMessagePackedAddress_t * packedAddress_p)
{
    packedAddress_p->Length = 0;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        (void) TPDU_GetPackedAddress(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), (ShortMessagePackedAddress_t *) packedAddress_p);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }
}



/********************************************************************/
/**
 *
 * @function     ShortMessage_GetApplicationPorts
 *
 * @description  Function to get application ports of a specific short message
 *
 * @param        thisShortMessage : .
 * @param        applicationPorts : .
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_GetApplicationPorts(
    ShortMessage_p thisShortMessage,
    ShortMessageApplicationPorts_p applicationPorts)
{
    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        applicationPorts->UseApplicationPorts = TPDU_GetApplicationPorts(thisShortMessage->TPDUObject_p->Tpdu,
                                                                         ShortMessage_GetType(thisShortMessage), &applicationPorts->DestinationPort, &applicationPorts->OriginatorPort);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    } else {
        applicationPorts->DestinationPort = 0;
        applicationPorts->OriginatorPort = 0;
        applicationPorts->UseApplicationPorts = FALSE;
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_LoadTPDU
 *
 * @description  This function laod the TPDU from storage into RAM and
 * associates it with the SMINstance
 *
 * @param        thisShortMessage : .
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessage_LoadTPDU(
    ShortMessage_p thisShortMessage)
{
    if (thisShortMessage == NULL) {
        SMS_A_(SMS_LOG_E("sm.c: Error - input param of ShortMessage_LoadTPDU is NULL!"));
        return;
    }

    // Check if we are calling Load with an already loaded SMInstance and no storage
    if (thisShortMessage->TPDUObject_p == NULL) {
        // Allocate some memory to load in the TPDU. This memory will be released by the
        // ShortMessage_UnloadTPDU function
        // Save the address of the memory in the SMInstance
        thisShortMessage->TPDUObject_p = ShortMessage_AllocateTpduCacheEntry(TPDUCache_List, thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            // Initialise the entry to all zeros
            thisShortMessage->TPDUObject_p->TpduAddress.Length = 0;
            thisShortMessage->TPDUObject_p->TpduAddress.TypeOfAddress = 0;
            memset(thisShortMessage->TPDUObject_p->TpduAddress.AddressValue, 0, SMS_MAX_PACKED_ADDRESS_LENGTH);
            memset(thisShortMessage->TPDUObject_p->Tpdu, 0, MAXIMUM_TPDU_SIZE);

#ifndef REMOVE_SMSTRG_LAYER
            if (thisShortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID) {
                uint8_t TpduLength;

                if (SmStorage_SmRead(thisShortMessage->StoragePosition, &thisShortMessage->TPDUObject_p->TpduAddress, &TpduLength, (uint8_t *) thisShortMessage->TPDUObject_p->Tpdu)) {
                    uint16_t Len = SmUtil_CalculateSizeOf_SM(thisShortMessage->TPDUObject_p->Tpdu);
                    thisShortMessage->TpduLength = SMS_MIN(Len, MAXIMUM_TPDU_SIZE);
                } else {
                    ShortMessage_RemoveTPDUFromCache(TPDUCache_List, thisShortMessage->TPDUObject_p, thisShortMessage);
                }
            }
#endif                          // REMOVE_SMSTRG_LAYER
        } else {
            SMS_A_(SMS_LOG_E("sm.c: Error - failed to get a TPDU cache slot"));
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_UnloadTPDU
 *
 * @description  this function frees the TPDU RAM copy associated with an
 * SmImstance.
 *
 * @param        thisShortMessage :
 * @param        UpdateStorage : If TRUE, the Storage will be updated as the
 *                               RAM copy has changed.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessage_UnloadTPDU(
    ShortMessage_p thisShortMessage,
    uint8_t UpdateStorage)
{
#ifdef REMOVE_SMSTRG_LAYER
    SMS_IDENTIFIER_NOT_USED(thisShortMessage)
        SMS_IDENTIFIER_NOT_USED(UpdateStorage)
#else
    if (thisShortMessage && thisShortMessage->StoragePosition != SMS_STORAGE_POSITION_INVALID) {
        uint8_t DCS_Byte;
        SMClass_t messageClass;

        // Check if we need to update the storage. Remove from cache if stored.
        if (UpdateStorage == TRUE) {
            // Update
            (void) SmStorage_SmWrite(thisShortMessage,
                                     thisShortMessage->StoragePosition, &thisShortMessage->TPDUObject_p->TpduAddress, thisShortMessage->TpduLength, (uint8_t *) thisShortMessage->TPDUObject_p->Tpdu);
        }
        // Need to Load TPDU before we parse TPDU
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            DCS_Byte = TPDU_GetField(TP_DCS, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

            messageClass = TPDU_Get(CLASS, thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage));

            // Don't cache the message if it is SIM Specific, and 8-bit Data message.
            if ((messageClass == SM_CLASS_2_SIM_SPECIFIC) && ((DCS_Byte == DCS_GENERAL_CODING_CLASS2_8BIT) || (DCS_Byte == DCS_CLASS2_8BIT))) {
                // If the TPDU pointer is still non null after this,
                // it indicates that the TPDU wasn't cached and hence was
                // alloc'd so we free up the TPDU memory instead.
                ShortMessage_RemoveTPDUFromCache(TPDUCache_List, thisShortMessage->TPDUObject_p, thisShortMessage);

                if (NULL != thisShortMessage->TPDUObject_p) {
                    SMS_HEAP_FREE(&thisShortMessage->TPDUObject_p);
                    thisShortMessage->TPDUObject_p = NULL;
                    SMS_B_(SMS_LOG_D("sm.c: Deallocated short message TPDU from dynamic ram"));
                }
            }
        }
    }
#endif                          // REMOVE_SMSTRG_LAYER
}


/********************************************************************/
/**
 *
 * @function     UnpackServiceCentreAddress
 *
 * @description  Unpacks the SC address
 *
 * @param        *inAddress_p :
 * @param        outAddress_p :
 *
 * @return       void
 */
/********************************************************************/
void UnpackServiceCentreAddress(
    ShortMessagePackedAddress_t * inAddress_p,
    ShortMessageSubscriberNumber_t * outAddress_p)
{
    uint8_t numberOfBytes = 0;
    ShortMessagePackedAddress_t inAddress = *inAddress_p;

    outAddress_p->Digits[0] = '\0';
    outAddress_p->TypeOfNumber = TON_UNKNOWN;
    outAddress_p->NumberingPlanId = NPI_UNKNOWN;
    numberOfBytes = SMS_MIN(inAddress.Length, SMS_MAX_PACKED_ADDRESS_LENGTH + 1);

    // Convert from number of Bytes to number of Digits
    if (numberOfBytes > 0) {
        uint8_t lastDigit = 0x00;

        // take off one for TON/NPI byte which is included in length
        numberOfBytes--;

        if (numberOfBytes > 0) {
            // assign last digit of Address
            lastDigit = (inAddress.AddressValue[numberOfBytes - 1] & 0xF0) >> 4;
        }
        // convert Length from Bytes to Digits
        inAddress.Length = numberOfBytes * 2;

        // if the last digit is 0x0f then the number of digits
        // is odd so decrement the Length by 1 to represent the exact number of digits.
        if (lastDigit == 0x0F) {
            inAddress.Length--;
        }

        UnpackAddressField(&inAddress, outAddress_p);
    }
}


/********************************************************************/
/**
 *
 * @function     UnpackAddressField
 *
 * @description  UnPacks the Originator or Destination address
 *
 * @param        *inAddress_p :
 * @param        outAddress_p :
 *
 * @return       void
 */
/********************************************************************/
void UnpackAddressField(
    ShortMessagePackedAddress_t * inAddress_p,
    ShortMessageSubscriberNumber_t * outAddress_p)
{
    uint8_t numberOfNibbles = 0;
    uint8_t numberOfSevenBitChars = 0;

    outAddress_p->TypeOfNumber = ((inAddress_p->TypeOfAddress >> 4) & 0x07);
    outAddress_p->NumberingPlanId = (inAddress_p->TypeOfAddress & 0x0F);
    numberOfNibbles = SMS_MIN(inAddress_p->Length, SMS_MAX_DIGITS_IN_NUMBER);

    if (outAddress_p->TypeOfNumber == TON_ALPHANUMERIC) {
        //numberOfSevenBitCharacters = (numberOfNibbles * numberOfBitsInNibbble) / numberOfBitsInCharacter
        numberOfSevenBitChars = (numberOfNibbles * 4) / 7;
        numberOfSevenBitChars = SmUtil_UnpackTo8Bits(outAddress_p->Digits, inAddress_p->AddressValue, numberOfSevenBitChars, DATA_CODING_7_BITS);
        outAddress_p->Digits[numberOfSevenBitChars] = '\0';
    } else {
        uint8_t i = 0;

        for (i = 0; i < ((numberOfNibbles + 1) / 2); i++) {
            outAddress_p->Digits[i * 2] = (inAddress_p->AddressValue[i] & 0x0F) + ASCII_ZERO;
            outAddress_p->Digits[i * 2 + 1] = ((inAddress_p->AddressValue[i] & 0xF0) >> 4) + ASCII_ZERO;
        }

        // BCD number of *, #, a, b and c have been coded according to 3GPP 23.040
        // Decode now
        for (i = 0; i < numberOfNibbles; i++) {
            uint8_t bcd;

            for (bcd = 0; bcd < NO_BCD_CHARACTERS; bcd++) {
                if (outAddress_p->Digits[i] == BCD_Lookup[bcd][0]) {
                    outAddress_p->Digits[i] = BCD_Lookup[bcd][1];
                    break;
                }
            }
        }

        //Terminates
        outAddress_p->Digits[numberOfNibbles] = '\0';
    }
}


/********************************************************************/
/**
 *
 * @function     PackServiceCentreAddress
 *
 * @description  Packs supplied SC address into GSM format
 *
 * @param        inAddress_p   : Unpacked SC Address
 * @param        *outAddress_p : Packed SC address
 *
 * @return       void
 */
/********************************************************************/
void PackServiceCentreAddress(
    ShortMessageSubscriberNumber_t * inAddress_p,
    ShortMessagePackedAddress_t * outAddress_p)
{
    (void) ShortMessage_PackGSMFormatAddress(ADDRESS_LENGTH_IN_BYTES, inAddress_p, outAddress_p);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_OrigDestAddressCompare
 *
 * @description  Compares the packed address of two short messages
 *
 * @param        shortMessageOne : First Short Message.
 * @param        shortMessageTwo : Second Short Message
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_OrigDestAddressCompare(
    ShortMessage_p shortMessageOne,
    ShortMessage_p shortMessageTwo)
{
    uint8_t Status = FALSE;
    ShortMessagePackedAddress_t addressOne, addressTwo;
    uint8_t numberOfBytes = 0;
    ShortMessage_GetPackedOrigOrDestAddress(shortMessageOne, &addressOne);
    ShortMessage_GetPackedOrigOrDestAddress(shortMessageTwo, &addressTwo);
    numberOfBytes = SMS_MAX(addressOne.Length, addressTwo.Length);
    //convert nibbles to bytes (with protection)
    numberOfBytes = SMS_MIN((unsigned) (((numberOfBytes + 1) / 2) + 2), sizeof(ShortMessagePackedAddress_t));
    if (memcmp(&addressOne, &addressTwo, numberOfBytes) == 0) {
        Status = TRUE;
    }
    return Status;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_PackedTimeStampCompare
 *
 * @description  Compares the packed timestamp of two short messages
 *
 * @param        shortMessageOne : First Short Message.
 * @param        shortMessageTwo : Second Short Message
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_PackedTimeStampCompare(
    ShortMessage_p shortMessageOne,
    ShortMessage_p shortMessageTwo)
{
    uint8_t Status = FALSE;
    uint8_t packedTimeStamp1[SMS_TIME_STAMP_LENGTH];
    uint8_t packedTimeStamp2[SMS_TIME_STAMP_LENGTH];

    ShortMessage_GetPackedTimeStamp(shortMessageOne, packedTimeStamp1);
    ShortMessage_GetPackedTimeStamp(shortMessageTwo, packedTimeStamp2);

    if (memcmp(packedTimeStamp1, packedTimeStamp2, SMS_TIME_STAMP_LENGTH) == 0) {
        Status = TRUE;
    }

    return Status;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_PackGSMFormatAddress
 *
 * @description  Converts an address of type SM_SubscriberNumber_t to an address of 3.40 or 4.08 type
 *               (described as the ShortMessageAddress_t in our system).
 *
 *               Returns the size of the packed address, the Length byte included.
 *               (LengthPresentation == ADDRESS_LENGTH_IN_DIGITS) => GSM 3.40 type.
 *
 *               If the number is of alphanumeric or abbreviated type the address is packed to 7 bit characters
 *               and but the length is set to number of semi-octets instead of no of characters (P.Edlund)!
 *
 * @param        LengthPresentation : uint8_t.
 * @param        InAddr_p :           as type of ShortMessageSubscriberNumber_t.
 * @param        *OutAddr_p :         as type of ShortMessagePackedAddress_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t ShortMessage_PackGSMFormatAddress(
    uint8_t LengthPresentation,
    ShortMessageSubscriberNumber_t * InAddr_p,
    ShortMessagePackedAddress_t * OutAddr_p)
{
    uint8_t Length;
    uint8_t NoOfDigits;
    uint8_t j;
    NoOfDigits = Length = strlen((char *) InAddr_p->Digits);
    if (Length != 0) {
        OutAddr_p->TypeOfAddress = (InAddr_p->TypeOfNumber << 4) | (InAddr_p->NumberingPlanId) | 0x80;
    } else {
        OutAddr_p->TypeOfAddress = 0x80;
    }
    if (InAddr_p->TypeOfNumber == TON_ALPHANUMERIC || InAddr_p->TypeOfNumber == TON_ABBREVIATED) {
        // Check if the number of packed digits would exceed
        // the OutAddr_p->AddressValue buffer (out of bounds
        // writing)
        if (NoOfDigits > SMS_MAX_PACKED_ADDRESS_LENGTH + 1) {
            // This is the case so to prevent buffer overflow/out of bounds writing
            // force the NoOfDigits to the maximum permissible, which using 7bit
            // encoding in a 10byte buffer should be 11 digits.
            NoOfDigits = SMS_MAX_PACKED_ADDRESS_LENGTH + 1;
            SMS_A_(SMS_LOG_E("sm.c: ShortMessage_PackGSMFormatAddress +++ SUBSCRIBER NUMBER OF DIGITS EXCEEDS MAX SIZE, TRUNCATING TO PREVENT BUFFER OVERFLOW +++"));
        }
        Length = SmUtil_PackFrom8Bits(OutAddr_p->AddressValue, InAddr_p->Digits, NoOfDigits, DATA_CODING_7_BITS);
        NoOfDigits = 2 * Length;        // NOTE: In this case the length shall be
        // given in semi-octets although this is not
        // the same as number of 'digits' as the 'digits'
        // are packed 7 bit characters.
    } else {
        if (Length % 2 != 0) {
            // Prepare for assigning 0x0F to the last, unused semi-octet:
            InAddr_p->Digits[Length] = 0x0F + ASCII_ZERO;
            Length++;           // Half of the last byte will be used
        }
        // BCD number of *, #, a, b and c have been coded according to 3GPP 23.040
        // Decode now
        for (j = 0; j < NoOfDigits; j++) {
            uint8_t bcd;

            for (bcd = 0; bcd < NO_BCD_CHARACTERS; bcd++) {
                if (InAddr_p->Digits[j] == BCD_Lookup[bcd][1]) {
                    InAddr_p->Digits[j] = BCD_Lookup[bcd][0];
                    break;
                }
            }
        }

        Length /= 2;            // Number of bytes needed for the digits
        for (j = 0; j < Length; j++) {
            OutAddr_p->AddressValue[j] = InAddr_p->Digits[2 * j] - ASCII_ZERO + ((InAddr_p->Digits[2 * j + 1] - ASCII_ZERO) << 4);
        }
    }
    if (LengthPresentation == ADDRESS_LENGTH_IN_DIGITS) {
        OutAddr_p->Length = NoOfDigits;
    } else {
        OutAddr_p->Length = Length + 1; // Number of bytes following the Length byte
    }

    return (Length + 2);        // Size of digit bytes, Length and TypeOfAddress bytes
}                               // ShortMessage_PackGSMFormatAddress


/********************************************************************/
/**
 *
 * @function     ShortMessage_CreateForStorage
 *
 * @description  This function creates an SM from a TPDU.
 *               Cannot protect against storing an invalid message type since SM-SUBMIT,
 *               SM-COMMAND and SM-DELIVER message types must all be stored. We can at
 *               least attempt to the ensure that the initial status of the stored
 *               short message is consistent with the direction ie MS->SC or SC->MS.
 *
 *               WARNING : There is a potential for error since we do not know if this is an
 *                         MS->SC or SC->MS message eg an SC->MS STATUS-REPORT will be stored as
 *                         an MS->SC SMS-COMMAND.
 *
 *               NOTE    : The tpdu memory allocated by this function must be freed.
 *
 * @param        MsgTpdu_p : The message data
 *
 * @return       ShortMessage_p
 */
/********************************************************************/
ShortMessage_p ShortMessage_CreateForStorage(
    const SMS_SMSC_Address_TPDU_t * const MsgTpdu_p)
{
    ShortMessage_p NewShortMessage = NULL;
    ShortMessageType_t ShortMessageType;
    uint8_t TpMtiBitfield;

    TpMtiBitfield = TPDU_GetBit(TP_MTI, (const TPDU_p) MsgTpdu_p->TPDU.Data);

    // Cannot protect against storing an invalid message type since SM-SUBMIT,
    // SM-COMMAND and SM-DELIVER message types must all be stored. We can at
    // least attempt to the ensure that the initial status of the stored
    // short message is consistent with the direction ie MS->SC or SC->MS.
    //
    // WARNING : There is a potential for error since we do not know if this is an
    //           MS->SC or SC->MS message eg an SC->MS STATUS-REPORT will be stored as
    //           an MS->SC SMS-COMMAND.
    //
    // Convert the MTI into an MO short message type.
    // i.e. DELIVER-REPORT, SMS-COMMAND or SMS-SUBMIT
    ShortMessageType = SmUtil_ConvertMtiToSmMessageType(TpMtiBitfield, MStoSC);

    // Determine if MO short message type is invalid for storage
    if (ShortMessageType != SM_SUBMIT && ShortMessageType != SM_COMMAND) {
        // Convert the MTI into an MT short message type.
        // i.e. DELIVER, SUBMIT-REPORT or STATUS-REPORT
        ShortMessageType = SmUtil_ConvertMtiToSmMessageType(TpMtiBitfield, SCtoMS);
    }
    // TPDU is valid for storage if short message type is SM-SUBMIT, SM-COMMAND or SM-DELIVER
    if (ShortMessageType == SM_SUBMIT || ShortMessageType == SM_COMMAND || ShortMessageType == SM_DELIVER) {
        NewShortMessage = ShortMessage_Create(ShortMessageType);

        if (NewShortMessage != NULL) {
            // Manually allocate storage for TPDU, We can't use the load tpdu function
            // as we haven't yet stored the TPDU.
            NewShortMessage->TPDUObject_p = (TPDUObject_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(TPDUObject_t));

            if (NewShortMessage->TPDUObject_p == NULL) {
                SMS_HEAP_FREE(&NewShortMessage);
                NewShortMessage = NULL;
            } else {
                // Populate the short message from the TPDU
                NewShortMessage->TPDUObject_p->TpduAddress.Length = MsgTpdu_p->ServiceCenterAddress.Length;
                NewShortMessage->TPDUObject_p->TpduAddress.TypeOfAddress = MsgTpdu_p->ServiceCenterAddress.TypeOfAddress;

                memcpy(NewShortMessage->TPDUObject_p->TpduAddress.AddressValue, MsgTpdu_p->ServiceCenterAddress.AddressValue, SMS_MAX_PACKED_ADDRESS_LENGTH);

                NewShortMessage->TpduLength = SMS_MIN(MsgTpdu_p->TPDU.Length, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);

                memcpy(NewShortMessage->TPDUObject_p->Tpdu, MsgTpdu_p->TPDU.Data, NewShortMessage->TpduLength);
            }
        }
    } else {
        SMS_A_(SMS_LOG_E("sm.c: ShortMessage_CreateForStorage - wrong short message type"));
    }

    return (NewShortMessage);
}


#ifndef REMOVE_SMSTRG_LAYER
/********************************************************************/
/**
 *
 * @function     ShortMessage_CreateAndStoreFromTpdu
 *
 * @description  This function creates an SM from TPDU.
 *               So if there is a Slot number specified it will
 +               create a Sm from the TPDU . If the slotnumber is 0
 *               then it will store the TPDU in available slot in the
 *               volume.
 *
 *               So if the Slot = NULL TPDU stored in available free slot in
 *               volume storage otherwise the slot overwrites the slot
 *               specified.
 *
 * @param        MsgTpdu_p :  The message data
 * @param        Volume_p  :  Storage volume to store on
 *
 * @return       ShortMessage_p
 */
/********************************************************************/
ShortMessage_p ShortMessage_CreateAndStoreFromTpdu(
    SMS_SMSC_Address_TPDU_t * MsgTpdu_p,
    MSG_StorageMedia_t * Volume_p)
{
    ShortMessage_p NewShortMessage = NULL;
    SMS_Position_t SlotNumber;

    // Create an SM from a TPDU and attempt to associate it with the correct
    // short message type depending on whether the TPDU is MO or MT.
    NewShortMessage = ShortMessage_CreateForStorage(MsgTpdu_p);

    if (NewShortMessage != NULL) {
        SmStoreErrorCode_t ErrorCode;

        // Store the sm
        SlotNumber = SmStorage_SmSaveInVolume(NewShortMessage, Volume_p, &ErrorCode);

        if (SMS_STORAGE_POSITION_INVALID != SlotNumber) {
            // Update the linked list
            ShortMessage_SetStoragePosition(NewShortMessage, SlotNumber);
            ShortMessage_SetSequenceNumber(NewShortMessage);

            // Now its stored, we need to free up the tpdu memory allocated
            SMS_HEAP_FREE(&NewShortMessage->TPDUObject_p);
            NewShortMessage->TPDUObject_p = NULL;
        } else {
            ShortMessage_Delete(NewShortMessage);
            NewShortMessage = NULL;
        }
    }

    return (NewShortMessage);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_CreateAndStoreInSlot
 *
 * @description  Create an SM from the TPDU and stores in specified slot
 *
 * @param        MsgTpdu_p  : The message data
 * @param        Volume_p   : Storage volume to store on
 * @param        SlotNumber : Storage slot for the short message
 *
 * @return       ShortMessage_p
 */
/********************************************************************/
ShortMessage_p ShortMessage_CreateAndStoreInSlot(
    const SMS_SMSC_Address_TPDU_t * const MsgTpdu_p,
    const MSG_StorageMedia_t * const Volume_p,
    const SMS_Position_t SlotNumber)
{
    ShortMessage_p NewShortMessage = NULL;

    // Create an SM from a TPDU and attempt to associate it with the correct
    // short message type depending on whether the TPDU is MO or MT.
    NewShortMessage = ShortMessage_CreateForStorage(MsgTpdu_p);

    if (NewShortMessage != NULL) {
        // Store the sm
        (void) SmStorage_SmSaveSlotNumber(NewShortMessage, SlotNumber, Volume_p);

        // Update the linked list
        ShortMessage_SetStoragePosition(NewShortMessage, SlotNumber);
        ShortMessage_SetSequenceNumber(NewShortMessage);

        // Now its stored, we need to free up the tpdu memory allocated
        SMS_HEAP_FREE(&NewShortMessage->TPDUObject_p);
        NewShortMessage->TPDUObject_p = NULL;
    }

    return (NewShortMessage);
}
#endif                          // REMOVE_SMSTRG_LAYER


/********************************************************************/
/**
 *
 * @function     ShortMessage_IsUDHISet
 *
 * @description  Function to check if the user data header indicator is set
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_IsUDHISet(
    ShortMessage_p thisShortMessage)
{
    uint8_t IsUDHISet = FALSE;

    ShortMessage_LoadTPDU(thisShortMessage);

    if (thisShortMessage->TPDUObject_p != NULL) {
        IsUDHISet = TPDU_TestBit(TP_UDHI, thisShortMessage->TPDUObject_p->Tpdu);

        ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
    }

    return (IsUDHISet);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_HasIEInRange
 *
 * @description  Function to check if a specific short message has an IE in the supplied range
 *
 * @param        thisShortMessage
 * @param        minIE
 * @param        maxIE
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_HasIEInRange(
    ShortMessage_p thisShortMessage,
    uint8_t minIE,
    uint8_t maxIE)
{
    uint8_t ieInstance = 0;
    uint8_t IEIInRange = FALSE;
    uint8_t *buffer_p = SMS_HEAP_UNTYPED_ALLOC(MSG_MAX_SHORT_MESSAGE_LEN + 1);  //this is max size of TPDU plus terminator

    if (buffer_p != NULL) {
        ShortMessage_LoadTPDU(thisShortMessage);

        if (thisShortMessage->TPDUObject_p != NULL) {
            while ((!IEIInRange)
                   && (TPDU_GetUserDataIE(thisShortMessage->TPDUObject_p->Tpdu, ShortMessage_GetType(thisShortMessage), ANY_IE_TYPE,    // Want to parse all IE's
                                          ieInstance++, buffer_p) > 0)) {
                if (*buffer_p >= minIE && *buffer_p < maxIE) {
                    IEIInRange = TRUE;
                }
            }

            ShortMessage_UnloadTPDU(thisShortMessage, FALSE);
        }

        SMS_HEAP_FREE(&buffer_p);
    }

    return (IEIInRange);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_ShortMessageTypeMatch
 *
 * @description  Function to decide if the two short message status types match with each other
 *
 * @param        shortmessageTypeOne : as the type of SMS_Status_t.
 * @param        shortmessageTypeTwo : as the type of SMS_Status_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_ShortMessageTypeMatch(
    SMS_Status_t shortmessageTypeOne,
    SMS_Status_t shortmessageTypeTwo)
{
    uint8_t typeMatch = FALSE;

    if (shortmessageTypeOne == shortmessageTypeTwo) {
        typeMatch = TRUE;
    }

    return (typeMatch);
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetSlotType
 *
 * @description  Obtain the type of a short message
 *
 * @param        shortMessage : as the type of ShortMessage_p.
 *
 * @return       SMS_Status_t
 */
/********************************************************************/
SMS_Status_t ShortMessage_GetSlotType(
    ShortMessage_p shortMessage)
{

    SMS_Status_t shortmessageType = SMS_STATUS_UNKNOWN;

    shortmessageType = ShortMessage_GetStatus(shortMessage);

    return (shortmessageType);

}

/********************************************************************/
/**
 *
 * @function     ShortMessage_AllocateTPDUStorage
 *
 * @description  Allocate storage for a TPDU
 *
 * @param        ShortMessage_p
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_AllocateTPDUStorage(
    ShortMessage_p theShortMessage)
{
    uint8_t status = FALSE;

    theShortMessage->TPDUObject_p = (TPDUObject_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(TPDUObject_t));

    if (NULL != theShortMessage->TPDUObject_p) {
        status = TRUE;
    } else {
        SMS_B_(SMS_LOG_E("sm.c: Failed to allocate TPDU storage"));
    }
    return status;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_DeallocateTPDUStorage
 *
 * @description  Deallocate storage for a short message TPDU
 *
 * @param        ShortMessage_p
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_DeallocateTPDUStorage(
    ShortMessage_p theShortMessage)
{
    if (NULL != theShortMessage->TPDUObject_p) {
        // Attempt to remove it from the cache first
        ShortMessage_RemoveTPDUFromCache(TPDUCache_List, theShortMessage->TPDUObject_p, theShortMessage);

        // if it wasn't cached but then the TPDU pointer will be non-NULL
        // so it will be safe to free it.  We must do it in this order or
        // we will potentially start freeing the cache memory allocated
        // when the messaging software starts
        if (NULL != theShortMessage->TPDUObject_p) {
            SMS_HEAP_FREE(&theShortMessage->TPDUObject_p);
            theShortMessage->TPDUObject_p = NULL;
            SMS_B_(SMS_LOG_D("sm.c: Deallocated short message TPDU from dynamic ram"));
        } else {
            SMS_B_(SMS_LOG_D("sm.c: Deallocated short message TPDU from cache"));
        }
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetTPDUCacheSize
 *
 * @description  Initielises the TPDU Cache pool
 *
 * @return       void
 */
/********************************************************************/
static uint16_t ShortMessage_GetTPDUCacheSize(
    void)
{
    return SMS_TPDU_CACHE_ENTRIES;
}

/********************************************************************/
/**
 *
 * @function     GetNextTPDUCacheRefNum
 *
 * @description  Initielises the TPDU Cache pool
 *
 * @return       void
 */
/********************************************************************/
static uint32_t GetNextTPDUCacheRefNum(
    void)
{
    static uint32_t NextRefNum = INVALID_TPDU_CACHE_REF + 1;
    return NextRefNum++;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_InitialiseTPDUCache
 *
 * @description  Initialises the TPDU Cache pool
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessage_InitialiseTPDUCache(
    TPDUPool * theTPDUPool_p)
{
    TPDUCacheEntry_t *EmptyTPDU_p = NULL;
    MSG_ListIndex i;
    uint16_t NumCacheEntries = ShortMessage_GetTPDUCacheSize();
    *theTPDUPool_p = MSG_ListCreate();

    for (i = 0; i < NumCacheEntries; i++) {
        EmptyTPDU_p = (TPDUCacheEntry_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(TPDUCacheEntry_t));
        EmptyTPDU_p->InUse = FALSE;
        EmptyTPDU_p->RefNum = INVALID_TPDU_CACHE_REF;
        EmptyTPDU_p->ShortMessage = NULL;
        EmptyTPDU_p->TPDUObject_p = (TPDUObject_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(TPDUObject_t));

        // Initialise the entry to all zeros
        EmptyTPDU_p->TPDUObject_p->TpduAddress.Length = 0;
        EmptyTPDU_p->TPDUObject_p->TpduAddress.TypeOfAddress = 0;
        memset(EmptyTPDU_p->TPDUObject_p->TpduAddress.AddressValue, 0, SMS_MAX_PACKED_ADDRESS_LENGTH);
        memset(EmptyTPDU_p->TPDUObject_p->Tpdu, 0, MAXIMUM_TPDU_SIZE);

        MSG_ListInsertItem(*theTPDUPool_p, i, EmptyTPDU_p);
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_TPDUCache_IterateFromStart
 *
 * @description  Iterates through the TPUDCache Pool
 *
 * @param        TPDUPool
 * @param        PreviousTPDU
 *
 * @return       Pointer to the next TPDU in the pool
 */
/********************************************************************/
static TPDUCacheEntry_t *ShortMessage_TPDUCache_IterateFromStart(
    TPDUPool theTPDUPool,
    TPDUCacheEntry_t * PreviousTPDU_p)
{
    TPDUCacheEntry_t *NextTPDU_p = NULL;

    if (PreviousTPDU_p == NULL) {
        //if previousMessage has been set to NULL we must be looking
        //for the first message so don't bother looking for it
        //first time so find the first bit we've got
        NextTPDU_p = (TPDUCacheEntry_t *) MSG_ListGetFirst(theTPDUPool);
    } else {
        //not the first time so find one that is next after previousMessage
        NextTPDU_p = MSG_ListGetNext(theTPDUPool, PreviousTPDU_p);
    }

    return (NextTPDU_p);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_AllocateTpduCacheEntry
 *
 * @description  Destroys the TPDU Cache pool
 *
 * @return       void
 */
/********************************************************************/
static TPDUObject_t *ShortMessage_AllocateTpduCacheEntry(
    TPDUPool theTPDUPool,
    ShortMessage_p ShortMessage)
{
    // Cycle round the cache on a FIFO basis so the 'oldest' TPDU will
    // be pushed out of the cached TPDUs
    TPDUCacheEntry_t *TPDUEntry_p = NULL;
    TPDUCacheEntry_t *ReplaceTPDUEntry_p = NULL;

    while ((TPDUEntry_p = ShortMessage_TPDUCache_IterateFromStart(theTPDUPool, TPDUEntry_p)) != NULL) {
        // Initialise the entry to all zeros
        if (TPDUEntry_p->InUse == FALSE) {
            // An empty cache slot has been found, so use it
            ReplaceTPDUEntry_p = TPDUEntry_p;
            break;
        }
    }

    if (NULL == ReplaceTPDUEntry_p) {
        // Ok, an empty slot was not found so we'd better kick the oldest
        // one out of the cache instead
        TPDUEntry_p = NULL;

        while ((TPDUEntry_p = ShortMessage_TPDUCache_IterateFromStart(theTPDUPool, TPDUEntry_p)) != NULL) {
            if (NULL == ReplaceTPDUEntry_p) {
                // Set the replace entry to first in the list
                ReplaceTPDUEntry_p = TPDUEntry_p;
            } else if (ReplaceTPDUEntry_p->RefNum > TPDUEntry_p->RefNum) {
                // This is older than the currently oldest identified slot
                ReplaceTPDUEntry_p = TPDUEntry_p;
            }
        }                       // End of while loop

        // Now we have the entry to be ejected from the cache, reset the TPDU
        // pointer back to NULL so that this short messages TPDU is no longer
        // deemed to be in the TPDU cache
        ReplaceTPDUEntry_p->ShortMessage->TPDUObject_p = NULL;
    }
    // Ok - Now we should be accessing the slot we need to make use of
    // regardless of whether its empty or being re-used
    ReplaceTPDUEntry_p->InUse = TRUE;
    ReplaceTPDUEntry_p->RefNum = GetNextTPDUCacheRefNum();
    ReplaceTPDUEntry_p->ShortMessage = ShortMessage;
    return ReplaceTPDUEntry_p->TPDUObject_p;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_RemoveTPDUFromCache
 *
 * @description  Destroys the TPDU Cache pool
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessage_RemoveTPDUFromCache(
    TPDUPool theTPDUPool,
    TPDUObject_t * TPDUObj_p,
    ShortMessage_p ShortMessage)
{
    TPDUCacheEntry_t *TPDUEntry_p = NULL;

    while ((TPDUEntry_p = ShortMessage_TPDUCache_IterateFromStart(theTPDUPool, TPDUEntry_p)) != NULL) {
        // Check if this is the entry we are looking for
        if ((TPDUEntry_p->InUse == TRUE) && (TPDUEntry_p->TPDUObject_p == TPDUObj_p) && (TPDUEntry_p->ShortMessage == ShortMessage)) {
            // The entry has been found so clear it out
            TPDUEntry_p->InUse = FALSE;
            TPDUEntry_p->RefNum = INVALID_TPDU_CACHE_REF;
            TPDUEntry_p->ShortMessage->TPDUObject_p = NULL;
            TPDUEntry_p->ShortMessage = NULL;
            TPDUEntry_p->TPDUObject_p->TpduAddress.Length = 0;
            TPDUEntry_p->TPDUObject_p->TpduAddress.TypeOfAddress = 0;
            memset(TPDUEntry_p->TPDUObject_p->TpduAddress.AddressValue, 0, SMS_MAX_PACKED_ADDRESS_LENGTH);
            memset(TPDUEntry_p->TPDUObject_p->Tpdu, 0, MAXIMUM_TPDU_SIZE);
            break;
        }
    }
}


#ifndef REMOVE_SMSTRG_LAYER
/********************************************************************/
/**
 *
 * @function     ShortMessage_GetStorageTime
 *
 * @description  Determines the timestamp for the specified short
 *               message
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_GetStorageTime(
    ShortMessage_p theShortMessage,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p)
{
    // This is where the guts of the timestamp stuff is implemented
    SmStorage_GetMessageTimestamp(theShortMessage, ShortMessageTimestamp_p);
}
#endif                          // REMOVE_SMSTRG_LAYER


/********************************************************************/
/**
 *
 * @function     ShortMessage_DeleteDuplicate
 *
 * @description  Locates the sm from the slot number and silently
 *               deletes it
 *
 * @param        SlotNumber : Slot number of the short message
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_DeleteDuplicate(
    SMS_Position_t SlotNumber)
{
    uint8_t Status = FALSE;
    ShortMessage_p theShortMessage = NULL;

    while ((theShortMessage = ShortMessage_IterateFromStart(ShortMessageMSG_List_p, theShortMessage)) != NULL) {
        if (theShortMessage->StoragePosition == SlotNumber) {
            break;
        }
    }

    if (NULL != theShortMessage) {
        ShortMessage_DeletePart(theShortMessage, TRUE);
        Status = TRUE;
    }

    return Status;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_GetSequenceNumber
 *
 * @description  Reads the RAM based sequence number from the Short Message
 *               object, so that we do not need to read the file
 *
 * @param        theShortMessage : The short message
 * @param        SequenceNumber : The returned sequence number - ? if not concatenated
 *
 * @return       uint8_t
 */
/********************************************************************/
void ShortMessage_GetSequenceNumber(
    ShortMessage_p theShortMessage,
    uint8_t * SequenceNumber)
{
    *SequenceNumber = theShortMessage->SequenceNumber;
}

/********************************************************************/
/**
 *
 * @function     ShortMessage_SetSequenceNumber
 *
 * @description  Sets the RAM based sequence number from the Short Message
 *               object, so that we do not need to read the file
 *
 * @param        theShortMessage : The short message
 *
 * @return       uint8_t
 */
/********************************************************************/
void ShortMessage_SetSequenceNumber(
    ShortMessage_p theShortMessage)
{
    SMS_ConcatParameters_t concatDetails;

    ShortMessage_IsConcatenated(theShortMessage, &concatDetails);

    if (concatDetails.IsConcatenated) {
        theShortMessage->SequenceNumber = concatDetails.MsgSequenceNumber;
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_isKeep_MWIs_Present
 *
 * @description  Check all MWIs and indicate if any are set to keep the short message.
 *
 * @param        theShortMessage : The short message
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessage_isKeep_MWIs_Present(
    MWIList_t * messageWaiting_p)
{
    uint8_t KeepMWIs = FALSE;

    if (messageWaiting_p->DiscardShortMessage == FALSE) {
        // A MWI which has been set to keep has been found.
        KeepMWIs = TRUE;
    }

    return (KeepMWIs);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_SetMoreToSend
 *
 * @description  Set the MoreToSend state (used to maintain the radio link)
 *               attribute of the short message.
 *
 * @param        theShortMessage : The short message
 * @param        MoreToSendState : MoreToSend state value to apply
 *
 * @return       void
 */
/********************************************************************/
void ShortMessage_SetMoreToSend(
    const ShortMessage_p thisShortMessage,
    const MoreToSendState_t MoreToSendState)
{
    thisShortMessage->MoreToSendState = MoreToSendState;
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_GetMoreToSend
 *
 * @description  Get the MoreToSend state (used to maintain the radio link)
 *               attribute of the short message.
 *
 * @param        theShortMessage : The short message
 *
 * @return       uint8_t
 */
/********************************************************************/
MoreToSendState_t ShortMessage_GetMoreToSend(
    const ShortMessage_p thisShortMessage)
{
    return (thisShortMessage->MoreToSendState);
}
