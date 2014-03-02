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
 *  The Short Message Router Object and its methods.
 *
 *************************************************************************/

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"


/* Utility Include Files */
#include <string.h>
#include <stdint.h>


/* Message Module Include Files */
#include "d_sms.h"
#include "g_sms.h"
#include "t_sms.h"
#include "smsevnthndr.h"
#include "c_sms_config.h"


#include "sm.h"
#include "smbearer.h"
#include "smrouter.h"
#include "smmngr.h"
#include "smsmain.h"

#include "smotadp.h"
#include "smsimtkt.h"
#ifndef REMOVE_SMSTRG_LAYER
#include "smstrg.h"
#endif                          //REMOVE_SMSTRG_LAYER
#include "smutil.h"             //HPLMN

/* List Support - Application Port Subscription */
#include "smslist.h"


typedef struct {
    SMS_PROCESS ProcessID;
    SMS_ClientTag_t ClientTag;
    SMS_ApplicationPortRange_t ApplicationPortRange;
} MSG_App_Port_List_t;

typedef struct ShortMessageRouterObject {
    CallbackFunction_p CallbackFunction;
    SMS_Storage_t PreferredStorage;
    SMS_NetworkAcknowledge_t NetworkAcknowledge;
} ShortMessageRouter_t;


//Local Prototypes
#define NO_CALLBACK ((CallbackFunction_p) NULL)
#define DEFAULT_PID (1)         // Use this as the default PID for STATUS-REPORTS when no PID is provided


static MSG_List *MSG_Application_Port_List_p = NULL;


#ifndef REMOVE_SMSTRG_LAYER
static SmStoreErrorCode_t ShortMessageRouter_ReplaceShortMessage(
    EventData_p,
    uint8_t);
#endif                          // REMOVE_SMSTRG_LAYER

static uint8_t ShortMessageRouter_GetCurrentNumberOfRules(
    void);
static uint8_t ShortMessageRouter_GetRoutingRule(
    ShortMessageRule_t * routingRule_p,
    uint8_t previousRule);

static int ShortMessageRouter_Port_Compare_Complete(
    void *elt1,
    void *elt2);
static int ShortMessageRouter_Port_Compare_PortRange(
    void *elt1,
    void *elt2);
static int ShortMessageRouter_Port_Compare_InsertSorted(
    void *elt1,
    void *elt2);

static SMS_Error_t ShortMessageRouter_Port_Find(
    SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_PROCESS * const Process_p,
    SMS_ClientTag_t * const ClientTag_p);
static SMS_Error_t ShortMessageRouter_Port_GetSubscriber(
    ShortMessage_p thisShortMessage,
    SMS_PROCESS * const Process_p,
    SMS_ClientTag_t * const ClientTag_p,
    SMS_ApplicationPortRange_t * ApplicationPortRange_p);


//Local Event Handler Functions
static void ShortMessageRouter_NewShortMessage(
    void *object,
    EventData_p eventData);
static void ShortMessageRouter_ProcessReceivedStatusReports(
    void *object,
    EventData_p eventData);
static void ShortMessageRouter_ShortMessageSend(
    void *object,
    EventData_p eventData);
static void ShortMessageRouter_ShortMessageSendAcknowledge(
    void *object,
    EventData_p eventData);
static void ShortMessageRouter_ApplicationAcknowledgeWithConfirm(
    void *object,
    EventData_p eventData);
static void ShortMessageRouter_DeliverReportConfirm(
    void *object,
    EventData_p eventData);
static void ShortMessageRouter_MemoryCapacityAvailableSent(
    void *object,
    EventData_p eventData);


//Message Event, State & Handler Table
static EventAction_t ShortMessageRouterStateEventTable[] = {
    {SHORTMESSAGEROUTER_NEW_SHORT_MESSAGE, ALL_STATES, ShortMessageRouter_NewShortMessage},
    {SHORTMESSAGEROUTER_APPLICATION_ACKNOWLEDGE_WITH_CNF, ALL_STATES, ShortMessageRouter_ApplicationAcknowledgeWithConfirm},
    {SHORTMESSAGEROUTER_PROCESS_RECEIVED_STATUS_REPORTS, ALL_STATES, ShortMessageRouter_ProcessReceivedStatusReports},
    {SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND, ALL_STATES, ShortMessageRouter_ShortMessageSend},
    {SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND_ACKNOWLEDGE, ALL_STATES, ShortMessageRouter_ShortMessageSendAcknowledge},
    {SHORTMESSAGEROUTER_DELIVERREPORT_CONFIRM, ALL_STATES, ShortMessageRouter_DeliverReportConfirm},
    {SHORTMESSAGEROUTER_MEMCAPACITYAVAILABLE_SENT, ALL_STATES, ShortMessageRouter_MemoryCapacityAvailableSent},
    {END_OF_TABLE, END_OF_TABLE, END_OF_TABLE_FUNCTION}
};

//This object is static
static ShortMessageRouter_t thisShortMessageRouter = { NO_CALLBACK, SMS_STORAGE_MT, SMS_NETWORK_ACKNOWLEDGE_CLIENT };

extern ShortMessageManager_p theShortMessageManager;

#define MSG_TIMER_PROCESS_STATUS_REPORTS  (10)  // 10 millisecond



/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_HandleEvent
 *
 * @description  Function for the short message router to handle event
 *
 * @param        event :     as type of ShortMessageRouterEvent_t.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageRouter_HandleEvent(
    ShortMessageRouterEvent_t event,
    EventData_p eventData)
{
    EventHandlerFunction_p eventHandler;

#ifdef SMS_PRINT_B_
    char *event_p = NULL;
    switch (event) {
    case SHORTMESSAGEROUTER_NEW_SHORT_MESSAGE:
        event_p = "new_short_message";
        break;
    case SHORTMESSAGEROUTER_APPLICATION_ACKNOWLEDGE_WITH_CNF:
        event_p = "application_acknowledge_with_cnf";
        break;
    case SHORTMESSAGEROUTER_PROCESS_RECEIVED_STATUS_REPORTS:
        event_p = "process_received_status_reports";
        break;
    case SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND:
        event_p = "short_message_send";
        break;
    case SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND_ACKNOWLEDGE:
        event_p = "short_message_send_acknowledge";
        break;
    case SHORTMESSAGEROUTER_DELIVERREPORT_CONFIRM:
        event_p = "deliverreport_confirm";
        break;
    case SHORTMESSAGEROUTER_MEMCAPACITYAVAILABLE_SENT:
        event_p = "memcapacityavailable_sent";
        break;
    default:
        event_p = "unknown event";
        break;
    }
    SMS_LOG_D("smrouter.c: ShortMessageRouter_HandleEvent: %s", event_p);
#endif                          //PRINT_B_
    eventHandler = EventHandler_Find(&thisShortMessageRouter, ShortMessageRouterStateEventTable, event, 0 /*this object has not states */ );
    (eventHandler) (&thisShortMessageRouter, eventData);
}

#ifndef REMOVE_SMSTRG_LAYER
/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_StorageFailedAck
 *
 * @description  Function to send the appropriate acknowledge when
 *               the short message storage has failed.
 *
 * @param        theShortMessage : the short message.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_StorageFailedAck(
    ShortMessage_p theShortMessage)
{
    //Excerpt from GSM 03.38:
    //When a mobile terminated message is Class 2 (SIM-specific), a phase 2 (or later) MS shall ensure that the message has been
    //transferred to the SMS data field in the SIM before sending an acknowledgement to the SC. The MS shall return a
    //"protocol error, unspecified" error message (see GSM TS 04.11) if the short message cannot be stored in the SIM and there
    //is other short message storage available at the MS. If all the short message storage at the MS is already in use, the MS
    //shall return "memory capacity exceeded". This behaviour applies in all cases except for phase 2+ MS supporting SIM
    //Application Toolkit when the Protocol Identifier (TP-PID) of the mobile terminated message is set to "SIM Data download"
    //(see GSM 03.40 [14])
    if (ShortMessage_GetMessageClass(theShortMessage) == SM_CLASS_2_SIM_SPECIFIC) {
        // Is there space in the file system?
        uint8_t spaceFound;
        MSG_StorageMedia_t *availableMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

        if (availableMedia_p != NULL) {
            //Find space in the preferred store list - but not on the SIM
            spaceFound = SmStorage_GetAvailableVolume(availableMedia_p, 1, TRUE);

            if (spaceFound == 0 || theShortMessageManager->ClientMemoryCapacityFull) {
                ShortMessage_Acknowledge(theShortMessage, TP_MEMORY_CAPACITY_EXCEEDED);
            } else {
                ShortMessage_Acknowledge(theShortMessage, TP_UNSPECIFIED_ERROR_CAUSE);
            }

            SMS_HEAP_FREE(&availableMedia_p);
        }
    } else if (ShortMessage_GetMessageClass(theShortMessage) == SM_CLASS_0) {
        // Can't ever send memory capcity exceed for class0 as they are not stored
        ShortMessage_Acknowledge(theShortMessage, TP_NO_CAUSE);
    } else {
        ShortMessage_Acknowledge(theShortMessage, TP_MEMORY_CAPACITY_EXCEEDED);
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ProcessReplaceType
 *
 * @description  Function to handle replacing a short message
 *               in storage that has the same replace type as
 *               the short message in the given event data.
 *
 *               Returns the result of the attempted storage.
 *
 * @param        eventData                : event data containing the short message.
 * @param        currentReplacedErrorCode : current value of the replaced error code.
 * @param        replaceType              : the replace type of the short message in the event data.
 * @param        storageRule              : the storage rule to be applied.
 *
 * @return       SmStoreErrorCode_t
 */
/********************************************************************/
static SmStoreErrorCode_t ShortMessageRouter_ProcessReplaceType(
    const EventData_p eventData,
    const SmStoreErrorCode_t currentReplacedErrorCode,
    const uint8_t replaceType,
    const ShortMessageStorageRule_t storageRule)
{
    // Preserve the current replaced error code.
    SmStoreErrorCode_t replacedErrorCode = currentReplacedErrorCode;

    // If this is a replace type that is to be stored, replace any
    // previously stored short message that has the same replace type.
    if ((replaceType > 0) && (SHORTMESSAGEROUTER_STORAGE_NO_STORE != storageRule)) {
        // Only update the error code if a replace was attempted.
        replacedErrorCode = ShortMessageRouter_ReplaceShortMessage(eventData, replaceType);
    }

    return replacedErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ShortMessageStore
 *
 * @description  Function to handle storage of the short message
 *               depending on the storage rule.
 *
 *               Returns the result of the attempted storage.
 *
 * @param        thisShortMessage : short message to store.
 * @param        routingRule_p    : short message routing rule.
 *
 * @return       SmStoreErrorCode_t
 */
/********************************************************************/
static SmStoreErrorCode_t ShortMessageRouter_ShortMessageStore(
    const ShortMessage_p thisShortMessage,
    const ShortMessageRule_t * const routingRule_p)
{
    SmStoreErrorCode_t storeErrorCode;
    MSG_StorageMedia_t storageMedia;

    // Initialise the storage rule to the value in the current routing rule.
    ShortMessageStorageRule_t localStorageRule = routingRule_p->STORAGE;

    memset(storageMedia.StorageMediaName, 0, SMS_STORAGE_MEDIA_LENGTH);

    //Support for AT+CPMS command
    if (localStorageRule == SHORTMESSAGEROUTER_STORAGE_PREFERRED) {
        if (thisShortMessageRouter.PreferredStorage == SMS_STORAGE_ME) {
            SMS_B_(SMS_LOG_D("smrouter.c: Override Prefered storage - set to ME"));
            localStorageRule = SHORTMESSAGEROUTER_STORAGE_ME_SPECIFIED;
        } else if (thisShortMessageRouter.PreferredStorage == SMS_STORAGE_SM) {
            SMS_B_(SMS_LOG_D("smrouter.c: Override Prefered storage - set to SIM"));
            localStorageRule = SHORTMESSAGEROUTER_STORAGE_SIM_SPECIFIED;
        }
    }
    // Now decide where to store the short message
    switch (localStorageRule) {
    case SHORTMESSAGEROUTER_STORAGE_ME_SPECIFIED:      // Store only in specified storage for message class (SIM or ME), fail otherwise
    case SHORTMESSAGEROUTER_STORAGE_ME_DELETE:
        {
            storeErrorCode = ShortMessage_StoreME(thisShortMessage);
        }
        break;
    case SHORTMESSAGEROUTER_STORAGE_SIM_SPECIFIED:     // Store only in specified storage for message class (SIM or ME), fail otherwise
    case SHORTMESSAGEROUTER_STORAGE_SIM_DELETE:
        {
            (void) strcpy(storageMedia.StorageMediaName, SIM_VOL);

            storeErrorCode = ShortMessage_StoreMedia(thisShortMessage, &storageMedia);
        }
        break;
    case SHORTMESSAGEROUTER_STORAGE_PREFERRED: // Store in platform default preferred storage (SIM or ME), fallback to non-preferred, fail otherwise.
        {
            storeErrorCode = ShortMessage_Store(thisShortMessage);
        }
        break;
    case SHORTMESSAGEROUTER_STORAGE_ME_PREFERRED:      // Store in ME storage if possible, fallback to non-preferred, fail otherwise.
        {
            storeErrorCode = ShortMessage_StoreME(thisShortMessage);

            if (SM_STORE_FAILED == storeErrorCode) {
                (void) strcpy(storageMedia.StorageMediaName, SIM_VOL);

                storeErrorCode = ShortMessage_StoreMedia(thisShortMessage, &storageMedia);
            }
        }
        break;
    case SHORTMESSAGEROUTER_STORAGE_SIM_PREFERRED:     // Store in SIM storage if possible, fallback to non-preferred, fail otherwise.
        {
            (void) strcpy(storageMedia.StorageMediaName, SIM_VOL);

            storeErrorCode = ShortMessage_StoreMedia(thisShortMessage, &storageMedia);

            if (SM_STORE_FAILED == storeErrorCode) {
                storeErrorCode = ShortMessage_StoreME(thisShortMessage);
            }
        }
        break;
    case SHORTMESSAGEROUTER_STORAGE_CONTROL:
        {
            (void) strcpy(storageMedia.StorageMediaName, SM_CONTROL_DIR);

            storeErrorCode = ShortMessage_StoreMedia(thisShortMessage, &storageMedia);
        }
        break;
    case SHORTMESSAGEROUTER_STORAGE_NO_STORE:  // Do not store
    default:
        {
            storeErrorCode = SM_STORE_NO_ERROR;
        }
        break;
    }                           // switch storage

    return storeErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ReplaceShortMessage
 *
 * @description  Function for the short message router to replace a short
 *               message with the correct replace type
 *
 * @param        eventData   : the event data.
 * @param        replaceType : uint8_t.
 *
 * @return       static SmStoreErrorCode_t
 */
/********************************************************************/
static SmStoreErrorCode_t ShortMessageRouter_ReplaceShortMessage(
    EventData_p eventData,
    uint8_t replaceType)
{
    SmStoreErrorCode_t errorCode = SM_STORE_FAILED;
    ShortMessage_p thisShortMessage = eventData->ShortMessage;
    ShortMessage_p shortMessageToReplace_p = NULL;

    while (((shortMessageToReplace_p = SmStorage_GetNextMessage(shortMessageToReplace_p)) != NULL) && (SM_STORE_FAILED == errorCode)) {
        if (shortMessageToReplace_p != NULL && ShortMessage_GetReplaceType(shortMessageToReplace_p) == replaceType) {
            //there's a short message in this slot and it's of the correct replace type
            if (ShortMessage_ServiceCentreAddressCompare(thisShortMessage, shortMessageToReplace_p) && ShortMessage_OrigDestAddressCompare(thisShortMessage, shortMessageToReplace_p)) {
                //the service centres and orignating addresses match
                SMS_A_(SMS_LOG_I("smrouter.c: REPLACING SHORT MESSAGE"));
                // Okey dokey, we've found the short message, now replace it.
                if (SM_STORE_FAILED != (errorCode = SmStorage_ReplaceShortMessage(shortMessageToReplace_p, thisShortMessage))) {
                    // reset the status to unread since we have replaced the old message
                    (void) ShortMessage_SetStatus(shortMessageToReplace_p, SMS_STATUS_UNREAD);

                    // set the event data to the short message id that has been replaced
                    eventData->ShortMessage = shortMessageToReplace_p;
                }
            }
        }
    }

    return errorCode;
}
#endif                          // REMOVE_SMSTRG_LAYER


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_PreferredStorageSet
 *
 * @description  Allows +CPMS to set the prefered storage
 *
 * @param        SMS_Storage_t : prefered storage for SMS.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageRouter_PreferredStorageSet(
    SMS_Storage_t PreferredStorage)
{
    thisShortMessageRouter.PreferredStorage = PreferredStorage;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_DeliverReportControlSet
 *
 * @description
 *
 * @param        SMS_NetworkAcknowledge_t : Responsible for network ack.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageRouter_DeliverReportControlSet(
    SMS_NetworkAcknowledge_t NetworkAcknowledge)
{
    thisShortMessageRouter.NetworkAcknowledge = NetworkAcknowledge;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_DeliverReportControlGet
 *
 * @description
 *
 * @param        SMS_NetworkAcknowledge_t : Responsible for network ack.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageRouter_DeliverReportControlGet(
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p)
{
    *NetworkAcknowledge_p = thisShortMessageRouter.NetworkAcknowledge;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_CheckRule_PortRange
 *
 * @description  Function determines if there is a match Port Range
 *
 *               Returns FALSE if the rule passed in has failed to match
 *               any of it's active elements, otherwise returns TRUE.
 *
 * @param        fRoutingRule_p    : short message routing rule to match.
 * @param        fThisShortMessage : The short message to check.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessageRouter_CheckPortRange(
    const uint16_t fLowerValue,
    const uint16_t fUpperValue,
    const uint8_t fIsPositive,
    const uint16_t fPortValue)
{
    uint8_t lRuleMatch = TRUE;

    if (fPortValue < fLowerValue || fPortValue > fUpperValue) {
        lRuleMatch = FALSE;
    }

    if (!fIsPositive) {
        lRuleMatch = !lRuleMatch;
    }

    return lRuleMatch;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_CheckRule_PortRanges
 *
 * @description  Function determines if there is a match Port Range
 *
 *               Returns FALSE if the rule passed in has failed to match
 *               any of it's active elements, otherwise returns TRUE.
 *
 * @param        fRoutingRule_p    : short message routing rule to match.
 * @param        fThisShortMessage : The short message to check.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessageRouter_CheckRule_PortRanges(
    const ShortMessageRule_t * const fRoutingRule_p,
    const ShortMessage_p fThisShortMessage)
{
    uint8_t lRuleMatch = TRUE;
    ShortMessageApplicationPorts_t lApplicationPorts;

    ShortMessage_GetApplicationPorts(fThisShortMessage, &lApplicationPorts);

    if (fRoutingRule_p->ACTIVERULES.ORIG_PORT_RANGE_ACTIVE) {
        if (lApplicationPorts.UseApplicationPorts) {
            lRuleMatch = ShortMessageRouter_CheckPortRange(fRoutingRule_p->OrigPortRange.StartOfRange,
                                                           fRoutingRule_p->OrigPortRange.EndOfRange, fRoutingRule_p->ACTIVERULES.ORIG_PORT_RANGE_POSITIVE, lApplicationPorts.OriginatorPort);
        } else {
            lRuleMatch = FALSE;
        }

        SMS_B_(SMS_LOG_D("smrouter.c: routingRule.ORIG_PORT_RANGE_ACTIVE origStart = %d origEnd = %d ruleMatch = %d",
                      fRoutingRule_p->OrigPortRange.StartOfRange, fRoutingRule_p->OrigPortRange.EndOfRange, lRuleMatch));
    }
    // Check Destination Port Range
    if (lRuleMatch && fRoutingRule_p->ACTIVERULES.DEST_PORT_RANGE_ACTIVE) {
        if (lApplicationPorts.UseApplicationPorts) {
            lRuleMatch = ShortMessageRouter_CheckPortRange(fRoutingRule_p->DestPortRange.StartOfRange,
                                                           fRoutingRule_p->DestPortRange.EndOfRange, fRoutingRule_p->ACTIVERULES.DEST_PORT_RANGE_POSITIVE, lApplicationPorts.DestinationPort);
        } else {
            lRuleMatch = FALSE;
        }
    }

    if (fRoutingRule_p->ACTIVERULES.DEST_PORT_RANGE_ACTIVE) {
        SMS_B_(SMS_LOG_D("smrouter.c: routingRule.DEST_PORT_RANGE_ACTIVE destStart = %d destEnd = %d ruleMatch = %d",
                      fRoutingRule_p->DestPortRange.StartOfRange, fRoutingRule_p->DestPortRange.EndOfRange, lRuleMatch));
    }

    return lRuleMatch;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_CheckRule_ContainsIE
 *
 * @description  Function determines if there is a match between IE
 *               active rule elements and the short message attributes.
 *
 *               Returns FALSE if the rule passed in has failed to match
 *               any of it's active elements, otherwise returns TRUE.
 *
 * @param        fRoutingRule_p    : short message routing rule to match.
 * @param        fThisShortMessage : The short message to check.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessageRouter_CheckRule_ContainsIE(
    const ShortMessageRule_t * const fRoutingRule_p,
    const ShortMessage_p fThisShortMessage)
{
    uint8_t lRuleMatch = TRUE;

    if (fRoutingRule_p->ACTIVERULES.CHECK_FOR_IE_ACTIVE) {
        lRuleMatch = ShortMessage_ContainsIEType(fThisShortMessage, fRoutingRule_p->IEType);

        if (!fRoutingRule_p->ACTIVERULES.CHECK_FOR_IE_POSITIVE) {
            lRuleMatch = !lRuleMatch;
        }

        SMS_B_(SMS_LOG_D("smrouter.c: routingRule.CHECK_FOR_IE_ACTIVE ruleMatch = %d", lRuleMatch));
    }

    return lRuleMatch;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_RuleMatch
 *
 * @description  Function determines if there is a match between ALL the
 *               active rule elements and the short message attributes.
 *
 *               Returns FALSE if the rule passed in has failed to match
 *               any of it's active elements, otherwise returns TRUE.
 *
 * @param        routingRule_p         : short message routing rule to match.
 * @param        shortMessageType      : short message type.
 * @param        shortMessageAddress_p : short message origination address.
 * @param        TPPID                 : short message Protocal IDentifier.
 * @param        TPDCS                 : short message Data Coding Scheme.
 * @param        shortMessageClass     : short message class.
 * @param        isDepersonalisation   : short message is depersonalisation.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessageRouter_RuleMatch(
    const ShortMessageRule_t * const routingRule_p,
    const ShortMessage_p thisShortMessage,
    const ShortMessageType_t shortMessageType,
    const ShortMessagePackedAddress_t * const shortMessageAddress_p,
    const uint8_t TPPID,
    const uint8_t TPDCS,
    const SMClass_t shortMessageClass,
    const uint8_t isDepersonalisation)
{
    uint8_t ruleMatch = TRUE;
    uint8_t addressSize;

    // Check the Default (match everything) flag first -
    // no point checking the other fields if this matches
    if (routingRule_p->ACTIVERULES.DEFAULT_ACTIVE && routingRule_p->ACTIVERULES.DEFAULT_POSITIVE) {
        SMS_B_(SMS_LOG_D("smrouter.c: routingRule.ACTIVERULES.DEFAULT_ACTIVE and POSITIVE"));
    } else {
        // Check message type
        if (ruleMatch && routingRule_p->ACTIVERULES.TPMTI_ACTIVE) {
            if (routingRule_p->TPMTI == shortMessageType) {
                ruleMatch = routingRule_p->ACTIVERULES.TPMTI_POSITIVE;
            } else {
                ruleMatch = !(routingRule_p->ACTIVERULES.TPMTI_POSITIVE);
            }

            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.TPMTI       = 0x%02x SM TPMTI = 0x%02x ruleMatch = %d", routingRule_p->TPMTI, shortMessageType, ruleMatch));
        }
        // Check origination address
        if (ruleMatch && routingRule_p->ACTIVERULES.TPOA_ACTIVE) {
            // Only compare the correct number of digits
            // Calculate number of bytes from semi-octects
            addressSize = (shortMessageAddress_p->Length + 1) / 2;

            if (memcmp(shortMessageAddress_p->AddressValue, routingRule_p->TPOA.AddressValue, addressSize) == 0) {
                ruleMatch = routingRule_p->ACTIVERULES.TPOA_POSITIVE;
            } else {
                ruleMatch = !(routingRule_p->ACTIVERULES.TPOA_POSITIVE);
            }

            SMS_B_(SMS_LOG_D("smrouter.c: TPOA                                           ruleMatch = %d", ruleMatch));
        }
        // Check protocol identifier
        if (ruleMatch && routingRule_p->ACTIVERULES.TPPID_ACTIVE) {
            if (routingRule_p->TPPID == TPPID) {
                ruleMatch = routingRule_p->ACTIVERULES.TPPID_POSITIVE;
            } else {
                ruleMatch = !(routingRule_p->ACTIVERULES.TPPID_POSITIVE);
            }

            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.TPPID       = 0x%02x SM TPPID = 0x%02x ruleMatch = %d", routingRule_p->TPPID, TPPID, ruleMatch));
        }
        // Check data coding scheme
        if (ruleMatch && routingRule_p->ACTIVERULES.TPDCS_ACTIVE) {
            if (routingRule_p->TPDCS == TPDCS) {
                ruleMatch = routingRule_p->ACTIVERULES.TPDCS_POSITIVE;
            } else {
                ruleMatch = !(routingRule_p->ACTIVERULES.TPDCS_POSITIVE);
            }

            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.TPDCS       = 0x%02x SM TPDCS = 0x%02x ruleMatch = %d", routingRule_p->TPDCS, TPDCS, ruleMatch));
        }
        // Check over the air depersonalisation
        if (ruleMatch && routingRule_p->ACTIVERULES.OTADP_ACTIVE) {
            if (routingRule_p->Flags.OTADP == isDepersonalisation) {
                ruleMatch = routingRule_p->ACTIVERULES.OTADP_POSITIVE;
            } else {
                ruleMatch = !(routingRule_p->ACTIVERULES.OTADP_POSITIVE);
            }

            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.Flags.OTADP = %d    SM OTADP = %d    ruleMatch = %d", routingRule_p->Flags.OTADP, isDepersonalisation, ruleMatch));
        }
        // Check HomePLMN
        if (ruleMatch && routingRule_p->ACTIVERULES.HomePLMN_ACTIVE) {
            MSG_PLMN_t SIM_HPLMN;

            // Get Home PLMN
            if (SmUtil_Get_MSG_Home_PLMN(&SIM_HPLMN) == TRUE) {
                if (SmUtil_Same_PLMN(&SIM_HPLMN, &routingRule_p->HomePLMN) == TRUE) {
                    ruleMatch = routingRule_p->ACTIVERULES.HomePLMN_POSITIVE;
                } else {
                    ruleMatch = !(routingRule_p->ACTIVERULES.HomePLMN_POSITIVE);
                }
            }

            SMS_B_(SMS_LOG_D("smrouter.c: HomePLMN                                       ruleMatch = %d", ruleMatch));
        }
        // Check message class
        if (ruleMatch && routingRule_p->ACTIVERULES.Class_ACTIVE) {
            if (routingRule_p->Class == shortMessageClass) {
                ruleMatch = routingRule_p->ACTIVERULES.Class_POSITIVE;
            } else {
                ruleMatch = !(routingRule_p->ACTIVERULES.Class_POSITIVE);
            }

            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.Class       = %d    SM Class = %d    ruleMatch = %d", routingRule_p->Class, shortMessageClass, ruleMatch));
        }
        // Check Origination/Destination Port Range
        if (ruleMatch) {
            ruleMatch = ShortMessageRouter_CheckRule_PortRanges(routingRule_p, thisShortMessage);
        }

        if (ruleMatch) {
            ruleMatch = ShortMessageRouter_CheckRule_ContainsIE(routingRule_p, thisShortMessage);
        }
    }                           // If not the default rule

    return ruleMatch;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_PackShortMessageEventData
 *
 * @description  This function extracts the storage location and TPDU
 *               from a short message in preparation for a dispatch
 *               or explicit dispatch.
 *
 * @param        SMS_Category_t               : short message category.
 * @param        ShortMessage_p               : short message.
 * @param        SMS_ShortMessageReceived_t   : short message event data.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_PackShortMessageEventData(
    const SMS_Category_t messageCategory,
    const ShortMessage_p newShortMessage,
    SMS_ShortMessageReceived_t * const IndicationSMS_Data_p)
{
    if (NULL != IndicationSMS_Data_p) {
        Tpdu_t *bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

        if (NULL != bufferTPDU_p) {
            IndicationSMS_Data_p->Category = messageCategory;
            IndicationSMS_Data_p->IsReplaceType = (0 == ShortMessage_GetReplaceType(newShortMessage) ? FALSE : TRUE);

            ShortMessage_GetPackedServiceCentreAddress(newShortMessage, (ShortMessagePackedAddress_t *) & IndicationSMS_Data_p->SMSC_Address_TPDU.ServiceCenterAddress);
            // Extract the TPDU from the short message holding the status report
            (void) ShortMessage_GetTpdu(newShortMessage, &IndicationSMS_Data_p->SMSC_Address_TPDU.TPDU.Length, (uint8_t *) bufferTPDU_p);
            // Protect against Status Report TPDU being greater than SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH
            if (SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH > IndicationSMS_Data_p->SMSC_Address_TPDU.TPDU.Length) {
            memset(IndicationSMS_Data_p->SMSC_Address_TPDU.TPDU.Data, 0, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);
            } else {
            IndicationSMS_Data_p->SMSC_Address_TPDU.TPDU.Length = SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH;
            }
            memcpy(IndicationSMS_Data_p->SMSC_Address_TPDU.TPDU.Data, bufferTPDU_p, IndicationSMS_Data_p->SMSC_Address_TPDU.TPDU.Length);

            if (messageCategory == SMS_CATEGORY_WAITING_IN_SERVICE_CENTER) {
                // Indicate only storage type and status
                IndicationSMS_Data_p->Slot.Storage = SMS_STORAGE_MT;
                IndicationSMS_Data_p->Slot.Position = 0;
            } else {
#ifdef REMOVE_SMSTRG_LAYER
                IndicationSMS_Data_p->Slot.Storage = SMS_STORAGE_MT;
                IndicationSMS_Data_p->Slot.Position = 0;
#else
                {
                    SMS_Position_t SlotNumber;

                    SlotNumber = ShortMessage_GetStoragePosition(newShortMessage);

                    SmStorage_SlotToMediaAndPosition(SlotNumber, &IndicationSMS_Data_p->Slot.Storage, &IndicationSMS_Data_p->Slot.Position);
                }
#endif                          // REMOVE_SMSTRG_LAYER
            }

            SMS_HEAP_FREE(&bufferTPDU_p);
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_DispatchShortMessage
 *
 * @description  This function extracts the storage location and TPDU
 *               from a short message and dispatches this information.
 *
 * @param        SMS_Category_t : short message category.
 * @param        ShortMessage_p : short message.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_DispatchShortMessage(
    const SMS_Category_t messageCategory,
    const ShortMessage_p newShortMessage)
{
    SMS_ShortMessageReceived_t *IndicationSMS_Data_p = (SMS_ShortMessageReceived_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_ShortMessageReceived_t));

    ShortMessageRouter_PackShortMessageEventData(messageCategory, newShortMessage, IndicationSMS_Data_p);
    if (NULL != IndicationSMS_Data_p) {
        SMS_A_(SMS_LOG_I("smrouter.c: DISPATCH Dispatch_MSG_ShortMessage"));

        Dispatch_SMS_ShortMessageReceived(IndicationSMS_Data_p);
        SMS_HEAP_FREE(&IndicationSMS_Data_p);
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_SendShortMessage
 *
 * @description  This function extracts the storage location and TPDU
 *               from a short message and sends this information to a
 *               specific Process.
 *
 * @param        SMS_Category_t              : short message category.
 * @param        ShortMessage_p              : short message.
 * @param        PROCESS                     : destination Process
 * @param        ClientTag_t                 : client tag data
 * @param        SMS_ApplicationPortRange_t* : application port range SM received via
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_SendShortMessage(
    const SMS_Category_t messageCategory,
    const ShortMessage_p newShortMessage,
    const SMS_PROCESS ProcessId,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p)
{
    SMS_ShortMessageReceived_t *IndicationSMS_Data_p = (SMS_ShortMessageReceived_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_ShortMessageReceived_t));

    ShortMessageRouter_PackShortMessageEventData(messageCategory, newShortMessage, IndicationSMS_Data_p);

    if (NULL != IndicationSMS_Data_p) {
        Event_SMS_ApplicationPortMatched_t *SendBuf_p;

        SendBuf_p = (Event_SMS_ApplicationPortMatched_t *) SMS_SIGNAL_ALLOC(Event_SMS_ApplicationPortMatched_t, EVENT_SMS_APPLICATIONPORTMATCHED);

        if (SendBuf_p != SMS_NIL) {
            SendBuf_p->ApplicationPortRange.ApplicationPort = ApplicationPortRange_p->ApplicationPort;
            SendBuf_p->ApplicationPortRange.LowerPort = ApplicationPortRange_p->LowerPort;
            SendBuf_p->ApplicationPortRange.UpperPort = ApplicationPortRange_p->UpperPort;

            SendBuf_p->Data.Category = IndicationSMS_Data_p->Category;
            SendBuf_p->Data.IsReplaceType = IndicationSMS_Data_p->IsReplaceType;
            SendBuf_p->Data.SMSC_Address_TPDU = IndicationSMS_Data_p->SMSC_Address_TPDU;
            SendBuf_p->Data.Slot = IndicationSMS_Data_p->Slot;

            // Set the client tag in the return signal
            Do_SMS_ClientTag_Set(SendBuf_p, ClientTag);

            SMS_SEND(&SendBuf_p, ProcessId);
        }

        SMS_HEAP_FREE(&IndicationSMS_Data_p);
    }
}

extern uint8_t Blocked_ShortMessageDispatch;

/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ApplicationNotify
 *
 * @description  Function to handle the notification to applications
 *
 * @param        eventData            : event data to notify to the applications.
 * @param        applicationNotify    : notification rule.
 * @param        shortMessageType     : short message type.
 * @param        storeErrorCode       : the current storage error code.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_ApplicationNotify(
    const EventData_p eventData,
    const ShortMessageRouterNotify_t applicationNotify,
    const ShortMessageType_t shortMessageType,
    const SmStoreErrorCode_t storeErrorCode)
{
    switch (applicationNotify) {
    case SHORTMESSAGEROUTER_NOTIFY_NORMAL:
        {
            if (SM_STATUS_REPORT == shortMessageType) {
                // Store the Status Report PID and storage position in case it has to be NACKed
                theShortMessageManager->LastReceived_SMS_PID = ShortMessage_GetProtocolIdentifier(eventData->ShortMessage);
                theShortMessageManager->LastReceived_SMS_Position = ShortMessage_GetStoragePosition(eventData->ShortMessage);

                // If no PID is set in the TPDU, set a default one to ensure the
                // send delivery report conditions can be met
                if (!theShortMessageManager->LastReceived_SMS_PID) {
                    theShortMessageManager->LastReceived_SMS_PID = DEFAULT_PID;
                }
                // Send notification to short message manager
                ShortMessageManager_HandleEvent(SHORTMESSAGEMANAGER_RECEIVE_NEW_STATUS_REPORT, eventData);

            } else {
                SMS_PROCESS Process = SMS_NO_PROCESS;
                SMS_ClientTag_t ClientTag = 0;
                SMS_ApplicationPortRange_t ApplicationPortRange;

                // Initialise messageCategory to SMS_CATEGORY_WAITING_IN_SERVICE_CENTER, to indicate that
                // storage failed but inform client that message is waiting in service centre
                SMS_Category_t messageCategory = SMS_CATEGORY_WAITING_IN_SERVICE_CENTER;

                if (SM_STORE_FAILED != storeErrorCode &&  !theShortMessageManager->ClientMemoryCapacityFull) {
                    // Store the Short Message PID and storage position in case it has to be NACKed
                    theShortMessageManager->LastReceived_SMS_PID = ShortMessage_GetProtocolIdentifier(eventData->ShortMessage);
                    theShortMessageManager->LastReceived_SMS_Position = ShortMessage_GetStoragePosition(eventData->ShortMessage);

                    messageCategory = ShortMessage_GetCategoryFromClass(eventData->ShortMessage);
                }
                // Dispatch to subscribed process
                if (SMS_ERROR_INTERN_NO_ERROR == ShortMessageRouter_Port_GetSubscriber(eventData->ShortMessage, &Process, &ClientTag, &ApplicationPortRange)) {
                    // Send the event to a specific client rather than dispatching the event to all subscribed clients
                    ShortMessageRouter_SendShortMessage(messageCategory, eventData->ShortMessage, Process, ClientTag, &ApplicationPortRange);
                } else {
                    ShortMessageRouter_DispatchShortMessage(messageCategory, eventData->ShortMessage);
                }
            }
        }
        break;
    case SHORTMESSAGEROUTER_NOTIFY_COPS:       // Send to COPS module
        {
            OTADP_HandleEvent(OTADP_NEW_SHORT_MESSAGE, eventData);
        }
        break;
    case SHORTMESSAGEROUTER_NOTIFY_SAT:        // Send to SIM Toolkit
        {
            SIMTKT_HandleEvent(SIMTKT_NEW_SHORT_MESSAGE, eventData);
        }
        break;
    case SHORTMESSAGEROUTER_NOTIFY_NONE:       // No notification
    default:
        {
            // Do nothing here
        }
        break;
    }                           // switch APPNOTIFY
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_NetworkAcknowledge
 *
 * @description  Function to handle network acknowledgement
 *
 * @param        thisShortMessage   : short message to acknowledge.
 * @param        networkAcknowledge : acknowledge rule.
 * @param        ackValue           : positive acknowledge value.
 * @param        nackValue          : negative acknowledge value.
 * @param        shortMessageType   : short message type.
 * @param        storeErrorCode     : short message storage error code.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_NetworkAcknowledge(
    const ShortMessage_p thisShortMessage,
    const ShortMessageRouterNetAck_t networkAcknowledge,
    const uint8_t ackValue,
    const uint8_t nackValue,
    const ShortMessageType_t shortMessageType,
    const SmStoreErrorCode_t storeErrorCode)
{
    ShortMessageRouterNetAck_t localNetworkAcknowledge = networkAcknowledge;
    if (networkAcknowledge == SHORTMESSAGEROUTER_NETACK_CLIENT && thisShortMessageRouter.NetworkAcknowledge == SMS_NETWORK_ACKNOWLEDGE_NORMAL) {
        localNetworkAcknowledge = SHORTMESSAGEROUTER_NETACK_NORMAL;
    }

    switch (localNetworkAcknowledge) {
    case SHORTMESSAGEROUTER_NETACK_NORMAL:     // Send normal ack/nack to network
        {
#ifndef REMOVE_SMSTRG_LAYER
            if (SM_STATUS_REPORT == shortMessageType) {
                ShortMessage_Acknowledge(thisShortMessage, TP_NO_CAUSE);
            } else {
                if (theShortMessageManager->ClientMemoryCapacityFull) {
                    ShortMessageRouter_StorageFailedAck(thisShortMessage);
                } else if (SM_STORE_NO_ERROR == storeErrorCode) {
                    ShortMessage_Acknowledge(thisShortMessage, TP_NO_CAUSE);
                } else if (SM_STORE_DATA_ERROR == storeErrorCode) {
                    // Special case of OTA 8-bit message that may be stored
                    // although the storage media (SIM) returned an Unspecified Error
                    // due to erroneous data
                    ShortMessage_Acknowledge(thisShortMessage, TP_UNSPECIFIED_ERROR_CAUSE);
                } else {
                    ShortMessageRouter_StorageFailedAck(thisShortMessage);
                }
            }
#endif                          // REMOVE_SMSTRG_LAYER
            break;
        }
    case SHORTMESSAGEROUTER_NETACK_SPECIAL:    // Send the ACK/NACK specified in NETACKVAL/NETNACKVAL to the network
        {
            if (SM_STATUS_REPORT == shortMessageType) {
                ShortMessage_Acknowledge(thisShortMessage, TP_NO_CAUSE);
            } else {
                if (SM_STORE_FAILED == storeErrorCode) {
                    ShortMessage_Acknowledge(thisShortMessage, nackValue);
                } else {
                    ShortMessage_Acknowledge(thisShortMessage, ackValue);
                }
            }
            break;
        }
    case SHORTMESSAGEROUTER_NETACK_CLIENT:     // The ACK/NACK to the network is handled by the client EXCEPT memory capacity exceeded
        {
#ifndef REMOVE_SMSTRG_LAYER
            if (SM_STATUS_REPORT != shortMessageType && SM_STORE_FAILED == storeErrorCode) {
                ShortMessageRouter_StorageFailedAck(thisShortMessage);
            } else if (theShortMessageManager->ClientMemoryCapacityFull && ShortMessage_GetMessageClass(thisShortMessage) != SM_CLASS_0) {
#else
            // Class0 SMS are not stored and must be ACKed by the client
            if (theShortMessageManager->ClientMemoryCapacityFull && ShortMessage_GetMessageClass(thisShortMessage) != SM_CLASS_0) {
#endif                          // REMOVE_SMSTRG_LAYER
                ShortMessage_Acknowledge(thisShortMessage, TP_MEMORY_CAPACITY_EXCEEDED);
            }
            break;
        }
    case SHORTMESSAGEROUTER_NETACK_NONE:       // Send no ack/nack to network
    default:
        {
        }
        break;

    }                           // switch NETACK
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ContinueParsing
 *
 * @description  Function to determine whether or not to continue
 *               parsing the next short message routing rule.
 *
 * @param        applicationNotify   : notification rule.
 * @param        currentContinue     : current state of continue flag.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessageRouter_ContinueParsing(
    const ShortMessageRouterNotify_t applicationNotify,
    const uint8_t currentContinue)
{
    // Preserve the current "continue parsing" flag.
    uint8_t continueParsing = currentContinue;

    switch (applicationNotify) {
    case SHORTMESSAGEROUTER_NOTIFY_COPS:
        {
            continueParsing = FALSE;    // short message handled by COPS so do not continue

            SMS_B_(SMS_LOG_D("smrouter.c: SHORTMESSAGEROUTER_NOTIFY_COPS stop short message routing"));
        }
        break;
    case SHORTMESSAGEROUTER_NOTIFY_NORMAL:
    case SHORTMESSAGEROUTER_NOTIFY_SAT:
    case SHORTMESSAGEROUTER_NOTIFY_NONE:
    default:
        {
            // Do nothing here
        }
        break;
    }                           // switch APPNOTIFY

    return continueParsing;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ShortMessageProcessDiscard
 *
 * @description  Function to determine whether or not to discard the
 *               new short message instance. This may be due to the
 *               short message being replaced, passed to another
 *               module for processing or because the storage failed.
 *
 * @param        thisShortMessage    : short message to acknowledge.
 * @param        routingRule_p       : short message routing rule.
 * @param        shortMessageType    : short message type.
 * @param        replacedErrorCode   : short message replaced error code.
 * @param        currentDiscard      : current state of discard flag.
 * @param        continueParsing     : whether or not routing rule parsing will continue.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessageRouter_ShortMessageProcessDiscard(
    const ShortMessage_p thisShortMessage,
    const ShortMessageRule_t * const routingRule_p,
    const ShortMessageType_t shortMessageType,
    const SmStoreErrorCode_t replacedErrorCode,
    const uint8_t currentDiscard,
    const uint8_t continueParsing)
{
    // Preserve the current discard flag.
    uint8_t discardShortMessage = currentDiscard;

    // Determine if the short message should be discarded.
    // NOTE: The discard flag should ONLY be set to TRUE and never be cleared to FALSE.
    //
    switch (routingRule_p->APPNOTIFY) {
    case SHORTMESSAGEROUTER_NOTIFY_COPS:
        {
            discardShortMessage = TRUE; // short message handled by COPS so discard

            SMS_B_(SMS_LOG_D("smrouter.c: SHORTMESSAGEROUTER_NOTIFY_COPS discard short message"));
        }
        break;
    case SHORTMESSAGEROUTER_NOTIFY_NORMAL:
    case SHORTMESSAGEROUTER_NOTIFY_SAT:
    case SHORTMESSAGEROUTER_NOTIFY_NONE:
    default:
        {
            // Do nothing here
        }
        break;
    }                           // switch APPNOTIFY

    // Discard the short message if it was stored successfully and the storage rule
    // indicates it should now be deleted
    //
    switch (routingRule_p->STORAGE) {
    case SHORTMESSAGEROUTER_STORAGE_SIM_DELETE:
    case SHORTMESSAGEROUTER_STORAGE_ME_DELETE:
    case SHORTMESSAGEROUTER_STORAGE_DELETE:
        {
            discardShortMessage = TRUE;
            SMS_B_(SMS_LOG_D("smrouter.c: storage indicates delete so discard short message"));
        }
        break;
    case SHORTMESSAGEROUTER_STORAGE_ME_SPECIFIED:
    case SHORTMESSAGEROUTER_STORAGE_SIM_SPECIFIED:
    case SHORTMESSAGEROUTER_STORAGE_PREFERRED:
    case SHORTMESSAGEROUTER_STORAGE_ME_PREFERRED:
    case SHORTMESSAGEROUTER_STORAGE_SIM_PREFERRED:
    case SHORTMESSAGEROUTER_STORAGE_NO_STORE:
    case SHORTMESSAGEROUTER_STORAGE_CONTROL:
    default:
        {
            // Do nothing;
        }
        break;
    }                           // switch storage

    // Discard the short message if routing rule parsing will not continue.
    if (!continueParsing) {
        if (SM_STORE_FAILED != replacedErrorCode) {
            discardShortMessage = TRUE; // replaced existing short message so discard

            SMS_B_(SMS_LOG_D("smrouter.c: discard replaced short message"));
        } else if (SM_STATUS_REPORT == shortMessageType) {
            discardShortMessage = TRUE;
            SMS_B_(SMS_LOG_D("smrouter.c: Status Report so discard short message"));
        }
        // Storage position may be invalid if SM was not persistantly stored
        // and must be deleted here to remove SM from short message list.
        else if (SMS_STORAGE_POSITION_INVALID == ShortMessage_GetStoragePosition(thisShortMessage)) {
            discardShortMessage = TRUE;
            SMS_B_(SMS_LOG_D("smrouter.c: storage position invalid discard short message"));
        }

        if (discardShortMessage) {
            ShortMessage_Delete(thisShortMessage);
            SMS_B_(SMS_LOG_D("smrouter.c: hardcoded short message DISCARD"));
        }
    }

    return discardShortMessage;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_HandleReservedMTI
 *
 * @description  Inspect TP-MTI for reserved value and change rule if required
 *
 * @param thisShortMessage  To enable TP-MTI to be inspected
 * @param StorageRule_p     To determine whether to store the msg.
 * @param AppNofityRule_p   What to notify
 * @param NetAckRule_p      Whether to send an ack.
 *
 * @return void
 */
/********************************************************************/
static void ShortMessageRouter_HandleReservedMTI(
    ShortMessage_p thisShortMessage,
    ShortMessageStorageRule_t * StorageRule_p,
    ShortMessageRouterNotify_t * AppNotifyRule_p,
    ShortMessageRouterNetAck_t * NetAckRule_p)
{
    // If we have received a SAT SMS PP Data Download request check the MTI for reserved value
    // If the reserved value is detected perform actions as described in TS 23.040
    //  TS 23.040 9.2.3.1
    //  If an MS receives a TPDU with a "Reserved" value in the TP MTI it shall process the message as if
    //  it were an "SMS DELIVER" but store the message exactly as received.

    if (*AppNotifyRule_p == SHORTMESSAGEROUTER_NOTIFY_SAT) {
        // The rules table has allocated this to be a SAT message now check the TP-MTI value
        uint8_t TpMtiBitfield;
        Tpdu_t *BufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));
        uint8_t TpduLength;

        if (BufferTPDU_p != NULL) {
            SMS_A_(SMS_LOG_I("smrouter.c: ShortMessageRouter_HandleReservedMTI Found SAT message"));

            (void) ShortMessage_GetTpdu(thisShortMessage, &TpduLength, (uint8_t *) BufferTPDU_p);

            TpMtiBitfield = TPDU_GetBit(TP_MTI, (uint8_t *) BufferTPDU_p);

            if (TpMtiBitfield == SMS_RESERVED_MESSAGE) {
                // TP-MTI has the reserved value re-write the output of the rule matching to be a normal msg.
                *StorageRule_p = SHORTMESSAGEROUTER_STORAGE_SIM_SPECIFIED;
                *AppNotifyRule_p = SHORTMESSAGEROUTER_NOTIFY_NORMAL;
                *NetAckRule_p = SHORTMESSAGEROUTER_NETACK_NORMAL;

                SMS_A_(SMS_LOG_I("smrouter.c: ShortMessageRouter_HandleReservedMTI Found Reserved"));
            }
            if (NULL != BufferTPDU_p) {
                SMS_HEAP_FREE(&BufferTPDU_p);
            }
        } else {
            SMS_A_(SMS_LOG_E("smrouter.c: Failed mem alloc ShortMessageRouter_HandleReservedMTI"));
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_NewShortMessage
 *
 * @description  Function to handle SHORTMESSAGEROUTER_NEW_SHORT_MESSAGE event
 *
 * @param        *object   : void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_NewShortMessage(
    void *object,
    EventData_p eventData)
{
    ShortMessage_p thisShortMessage = eventData->ShortMessage;

    uint8_t routingRuleID = 0;
    ShortMessageRule_t routingRule;
    SmStoreErrorCode_t storeErrorCode;
    uint8_t replaceType;
    uint8_t TPPID;
    uint8_t TPDCS;
    ShortMessagePackedAddress_t shortMessageAddress;
    ShortMessageType_t shortMessageType;
    SMClass_t shortMessageClass;
    uint8_t isDepersonalisation;

    // The following cached variables should NOT be reset before each rule is parsed.
    uint8_t routed = FALSE;
    uint8_t continueParsing = TRUE;
    SmStoreErrorCode_t replacedErrorCode = SM_STORE_FAILED;
    uint8_t discardShortMessage = FALSE;

    SMS_IDENTIFIER_NOT_USED(object);

    // Retrieve the short message routing match criteria
    //
    // NOTE: The SM's original match criteria is cached here since SM's may have
    //       their criteria changed if stored on certain SIM's
    //
    shortMessageType = ShortMessage_GetType(thisShortMessage);
    SMS_B_(SMS_LOG_D("smrouter.c: Cached SM TPMTI        = 0x%02x", shortMessageType));

    ShortMessage_GetPackedOrigOrDestAddress(thisShortMessage, &shortMessageAddress);

    TPPID = ShortMessage_GetProtocolIdentifier(thisShortMessage);
    SMS_B_(SMS_LOG_D("smrouter.c: Cached SM TPPID        = 0x%02x", TPPID));

    TPDCS = ShortMessage_GetDataCodingScheme(thisShortMessage);
    SMS_B_(SMS_LOG_D("smrouter.c: Cached SM TPDCS        = 0x%02x", TPDCS));

    isDepersonalisation = ShortMessage_IsDepersonalisation(thisShortMessage);
    SMS_B_(SMS_LOG_D("smrouter.c: Cached SM OTADP        = %d", isDepersonalisation));

    shortMessageClass = ShortMessage_GetMessageClass(thisShortMessage);
    SMS_B_(SMS_LOG_D("smrouter.c: Cached SM Class        = %d", shortMessageClass));

    replaceType = ShortMessage_GetReplaceType(thisShortMessage);
    SMS_B_(SMS_LOG_D("smrouter.c: Cached SM Replace Type = %d", replaceType));


    SMS_B_(SMS_LOG_D("smrouter.c: ************************************************************"));

    while (continueParsing && ((routingRuleID = ShortMessageRouter_GetRoutingRule(&routingRule, routingRuleID)) != 0)) {
        SMS_B_(SMS_LOG_D("smrouter.c: BEGIN parsing routingRuleID: %02d", routingRuleID));
        // If this rule is a match carry out the actions associated with this rule.
        if (ShortMessageRouter_RuleMatch(&routingRule, thisShortMessage, shortMessageType, &shortMessageAddress, TPPID, TPDCS, shortMessageClass, isDepersonalisation)) {
            routed = TRUE;      // Found at least one rule which matches the incoming message

            // Reserved MTI cannot be determined from shortMessageType since ShortMessageType_t
            // contains no value for reserved since SMS of type reserved are converted to type
            // SM_DELIVER for other internal processing
            ShortMessageRouter_HandleReservedMTI(thisShortMessage, &routingRule.STORAGE, &routingRule.APPNOTIFY, &routingRule.NETACK);

#ifdef REMOVE_SMSTRG_LAYER
            SMS_IDENTIFIER_NOT_USED(replaceType);

            storeErrorCode = SM_STORE_NO_ERROR;
#else
            storeErrorCode = SM_STORE_FAILED;

            // If the short message is a replace type, replace an existing short message
            // in storage with the same replace type with the short message in the event data.
            replacedErrorCode = ShortMessageRouter_ProcessReplaceType(eventData, replacedErrorCode, replaceType, routingRule.STORAGE);

            if (SM_STORE_FAILED != replacedErrorCode) {
                storeErrorCode = replacedErrorCode;     // allows ack/nack as appropriate
            } else if (routingRule.STORAGE == SHORTMESSAGEROUTER_STORAGE_NO_STORE) {
                storeErrorCode = SM_STORE_NO_ERROR;
            } else {
                // Attempt to store the message (if requested)
                storeErrorCode = ShortMessageRouter_ShortMessageStore(thisShortMessage, &routingRule);
            }
#endif                          // REMOVE_SMSTRG_LAYER

            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.STORAGE     = %d   storeErrorCode = %d", routingRule.STORAGE, storeErrorCode));
            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.APPNOTIFY   = %d", routingRule.APPNOTIFY));
            SMS_B_(SMS_LOG_D("smrouter.c: routingRule.NETACK      = %d", routingRule.NETACK));
            // Notify the applications
            ShortMessageRouter_ApplicationNotify(eventData, routingRule.APPNOTIFY, shortMessageType, storeErrorCode);

            // Acknowledge the short message
            ShortMessageRouter_NetworkAcknowledge(thisShortMessage, routingRule.NETACK, routingRule.NETACKVAL, routingRule.NETNACKVAL, shortMessageType, storeErrorCode);

            // Determine if short message routing rule parsing should continue.
            continueParsing = ShortMessageRouter_ContinueParsing(routingRule.APPNOTIFY, routingRule.Flags.CONTINUE);    // If the rule's CONTINUE flag is set, continue parsing

            // The SM may be discarded by one of the routing handlers which has 'hardcoded'
            // functionality - MWI discarding is an example of this.
            // The routing storage rule may also specify discarding of the short message
            // after processing.
            discardShortMessage = ShortMessageRouter_ShortMessageProcessDiscard(thisShortMessage, &routingRule, shortMessageType, replacedErrorCode, discardShortMessage, continueParsing);


        }                       // if (ShortMessageRouter_RuleMatch())

        SMS_B_(SMS_LOG_D("smrouter.c: END   parsing routingRuleID: %02d", routingRuleID));
        SMS_B_(SMS_LOG_D("smrouter.c: ************************************************************"));
    }                           // while (continue parsing and valid rule read)

    if (routed == FALSE) {
        SMS_A_(SMS_LOG_E("smrouter.c: ERROR: No matching short message routing rule delete and NACK SM"));
        // No matching rules were found, nothing has been done with the message
        // so send back an unspecified error to the network and delete it.
        ShortMessage_Acknowledge(thisShortMessage, TP_UNSPECIFIED_ERROR_CAUSE);
        ShortMessage_Delete(thisShortMessage);
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ProcessReceivedStatusReports
 *
 * @description  Function to handle SHORTMESSAGEROUTER_PROCESS_RECEIVED_STATUS_REPORTS event
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageRouter_ProcessReceivedStatusReports(
    void *object,
    EventData_p eventData)
{
    ShortMessage_p oldStatusReport = NULL;

    SMS_IDENTIFIER_NOT_USED(oldStatusReport);   //stop compiler moan
    SMS_IDENTIFIER_NOT_USED(object);
    SMS_IDENTIFIER_NOT_USED(eventData);

#ifndef REMOVE_SMSTRG_LAYER
    while (!SmStorage_RestoreStatusReportSM(&oldStatusReport, (MSG_StorageMedia_t *) SMS_FS_DIR_USE_SIM_CACHE)) {
        if (NULL != oldStatusReport) {
            eventData->ShortMessage = oldStatusReport;

            // Send notification to short message manager
            ShortMessageManager_HandleEvent(SHORTMESSAGEMANAGER_PROCESS_RECEIVED_STATUS_REPORTS, eventData);

            // If the short message has not been deallocated by some other part
            // of messaging then we'll do it ourseleves here.
            if (ShortMessage_Exists(eventData->ShortMessage)) {
                ShortMessage_DeallocateTPDUStorage(eventData->ShortMessage);

                if (ShortMessage_GetType(eventData->ShortMessage) == SM_STATUS_REPORT) {
                    ShortMessage_Delete(eventData->ShortMessage);
                }
            }
        }
    }
#endif                          // REMOVE_SMSTRG_LAYER
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ShortMessageSend
 *
 * @description  Function to handle SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND event
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageRouter_ShortMessageSend(
    void *object,
    EventData_p eventData)
{
    ShortMessageRouter_p thisRouter = (ShortMessageRouter_p) object;

    if (thisRouter->CallbackFunction == NO_CALLBACK) {
        SMS_B_(SMS_LOG_D("smrouter.c: ShortMessageRouter_ShortMessageSend: saving callback "));

        thisRouter->CallbackFunction = eventData->CallbackFunction;

        ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_COMMAND_REQ, eventData);
    } else {
        SMS_B_(SMS_LOG_D("smrouter.c: ShortMessageRouter_ShortMessageSend: calling callback "));
        (eventData->CallbackFunction) (eventData->ShortMessage, SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED);
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_DeliverReportConfirm
 *
 * @description  This function forwards the result of sending deliver report to the message manager.
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageRouter_DeliverReportConfirm(
    void *object,
    EventData_p eventData)
{
    SMS_IDENTIFIER_NOT_USED(object);    //stop compiler moan

    ShortMessageManager_HandleEvent(SHORTMESSAGEMANAGER_DELIVERREPORT_CONFIRM, eventData);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_MemoryCapacityAvailableSent
 *
 * @description  This function forwards the command to handle Memory Capacity Available sent.
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageRouter_MemoryCapacityAvailableSent(
    void *object,
    EventData_p eventData)
{
    SMS_IDENTIFIER_NOT_USED(object);    //stop compiler moan

    ShortMessageManager_HandleEvent(SHORTMESSAGEMANAGER_MEMCAPACITYAVAILABLE_SENT, eventData);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ShortMessageSendAcknowledge
 *
 * @description  Function to handle SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND_ACKNOWLEDGE event
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageRouter_ShortMessageSendAcknowledge(
    void *object,
    EventData_p eventData)
{
    ShortMessageRouter_p thisRouter = (ShortMessageRouter_p) object;
    CallbackFunction_p callbackFunction_p;

    if (thisRouter->CallbackFunction != NO_CALLBACK) {
        SMS_B_(SMS_LOG_D("smrouter.c: ShortMessageRouter_ShortMessageSendAcknowledge: calling callback "));

        callbackFunction_p = thisRouter->CallbackFunction;
        thisRouter->CallbackFunction = NO_CALLBACK;

        (callbackFunction_p) (eventData->ShortMessage, eventData->RMS_V2_ErrorCode);
    } else {
        SMS_B_(SMS_LOG_D("smrouter.c: ShortMessageRouter_ShortMessageSent:"));
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_ApplicationAcknowledgeWithConfirm
 *
 * @description  Function to handle SHORTMESSAGEROUTER_APPLICATION_ACKNOWLEDGE_WITH_CNF event
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageRouter_ApplicationAcknowledgeWithConfirm(
    void *object,
    EventData_p eventData)
{
    SMS_IDENTIFIER_NOT_USED(object);    //stop compiler moan

    ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_APPLICATION_ACK_WITH_CNF, eventData);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_GetRoutingRule
 *
 * @description  Read a routing rule from the SM routing table
 *
 * @param        routingRule_p: The routing rule structure to be filled in
 * @param        previousRule : The previous rule returned by this function (used to get next rule)
 *                              Set to 0 to get first rule.
 *
 * @return       static uint8_t : The current routing rule number. 0 if the call fails (i.e. the last rule
 *                              has been read or other file access error).
 */
/********************************************************************/
static uint8_t ShortMessageRouter_GetRoutingRule(
    ShortMessageRule_t * routingRule_p,
    uint8_t previousRule)
{
    uint8_t returnRule = 0;
    uint8_t noOfRules;

    // Get the current number of routing rules
    noOfRules = ShortMessageRouter_GetCurrentNumberOfRules();

    SMS_B_(SMS_LOG_D("smrouter.c: ShortMessageRouter_GetRoutingRule: previousRule: %d noOfRules; %d", previousRule, noOfRules));

    if (previousRule < noOfRules)       //successful access
    {
        returnRule = previousRule + 1;
        memcpy(routingRule_p, &SMRuleTable[previousRule], sizeof(ShortMessageRule_t));
    } else                      // failure to access
    {
        SMS_A_(SMS_LOG_E("smrouter.c: ***************************************************************************"));
        SMS_A_(SMS_LOG_E("smrouter.c: ** failed to access SM routing rule. The short message can not be stored **"));
        SMS_A_(SMS_LOG_E("smrouter.c: ***************************************************************************"));
    }

    return returnRule;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Port_Compare_Complete
 *
 * @description  Helper function for MSG_ListFind to provide
 *               comparison on complete list data
 *               USE: When deleting an application port subscription
 *                    all application port range elements and processId
 *                    must match
 *
 * @param        elt1 : First list data to compare  [from list ]
 * @param        elt2 : Second list data to compare [find input]
 *
 * @return       int  : 0 when we have a match otherwise return -1
 */
/********************************************************************/
static int ShortMessageRouter_Port_Compare_Complete(
    void *elt1,
    void *elt2)
{
    // we only return 0 when we have a match otherwise return -1
    int RetVal = -1;

    if (((MSG_App_Port_List_t *) elt2)->ProcessID == ((MSG_App_Port_List_t *) elt1)->ProcessID
        && ((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.ApplicationPort == ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.ApplicationPort
        && ((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.LowerPort == ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.LowerPort
        && ((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.UpperPort == ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.UpperPort) {
        RetVal = 0;
    }

    return RetVal;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Port_Compare_PortRange
 *
 * @description  Helper function for MSG_ListFind to provide
 *               comparison on list data ProcessID
 *               USE: When adding a new application port subscription,
 *                    to check if the supplied range lies within a
 *                    range already subscribed to by another process
 *               USE: When attempting to send a Rx SM to clients, to
 *                    determine which process is subscribed to this
 *                    application port range
 *               NOTE: There will only ever be a single process
 *                     subscribed to an application port so the find
 *                     only needs to find the first match
 *
 * @param        elt1 : First list data to compare  [from list ]
 * @param        elt2 : Second list data to compare [find input]
 *
 * @return       int  : 0 when we have a match otherwise return -1
 */
/********************************************************************/
static int ShortMessageRouter_Port_Compare_PortRange(
    void *elt1,
    void *elt2)
{
    // we only return 0 when we have a match otherwise return -1
    int RetVal = -1;

    // if the supplied lower or upper ports are within the list supplied port range
    if ((((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.LowerPort >= ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.LowerPort
         && ((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.LowerPort <= ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.UpperPort)
        || (((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.UpperPort >= ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.LowerPort
            && ((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.UpperPort <= ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.UpperPort)) {
        // match the application port type (destination/origination)
        if (((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.ApplicationPort == ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.ApplicationPort) {
            RetVal = 0;
        }
    }

    return RetVal;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Port_Compare_InsertSorted
 *
 * @description  Helper function for MSG_ListInsertSorted to provide
 *               comparison on list data Application Port Range
 *               (LowerPort) to determine the correct sorted insertion
 *               index
 *               USE: When adding a new application port subscription,
 *                    to ensure that the new subscription is added to
 *                    the list in a sorted order
 *
 * @param        elt1 : First list data to compare  [from list ]
 * @param        elt2 : Second list data to compare [find input]
 *
 * @return       int  : "> 0"  means "elt1 > elt2",
 *                      "== 0" means equal,
 *                      "< 0"  means "elt1 < elt2"
 */
/********************************************************************/
static int ShortMessageRouter_Port_Compare_InsertSorted(
    void *elt1,
    void *elt2)
{
    // we only return 0 when we have a match otherwise return -1
    int RetVal = 0;

    if (((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.LowerPort < ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.LowerPort) {
        RetVal = -1;
    } else if (((MSG_App_Port_List_t *) elt2)->ApplicationPortRange.LowerPort > ((MSG_App_Port_List_t *) elt1)->ApplicationPortRange.LowerPort) {
        RetVal = 1;
    }

    return RetVal;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Port_Add
 *
 * @description  Add a subscription to an application port range
 *
 * @param        PROCESS                     : The ProcessID of the subscriber
 * @param        ClientTag_t                 : The ClientTag information which will be used when notifying the Process of an incoming message
 * @param        SMS_ApplicationPortRange_t* : The application port range and type to subscribe to
 *
 * @return       SMS_Error_t             : SMS_ERROR_INTERN_NO_ERROR if OK
 */
/********************************************************************/
SMS_Error_t ShortMessageRouter_Port_Add(
    const SMS_PROCESS Process,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;

    if (NULL == ApplicationPortRange_p) {
        MSG_ErrorCode = SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT;
    }
    // In order for the searching/sorting against existing entries the lower port
    // must be less than or equal to the upper port
    else if (ApplicationPortRange_p->LowerPort > ApplicationPortRange_p->UpperPort) {
        MSG_ErrorCode = SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT;
    } else {
        // Create the application port list index table if necessary
        if (NULL == MSG_Application_Port_List_p) {
            MSG_Application_Port_List_p = MSG_ListCreate();
        }

        if (NULL == MSG_Application_Port_List_p) {
            MSG_ErrorCode = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
        } else {
            MSG_App_Port_List_t *AppPortListData_p = (MSG_App_Port_List_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_App_Port_List_t));

            if (NULL == AppPortListData_p) {
                MSG_ErrorCode = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
            } else {
                MSG_ListIndex ListIndex = NOPOS;

                AppPortListData_p->ProcessID = Process;
                AppPortListData_p->ClientTag = ClientTag;
                memcpy(&AppPortListData_p->ApplicationPortRange, ApplicationPortRange_p, sizeof(SMS_ApplicationPortRange_t));

                // We should get back NOPOS to indicate that no match against the port range was found
                ListIndex = MSG_ListFind(MSG_Application_Port_List_p, (void *) AppPortListData_p, ShortMessageRouter_Port_Compare_PortRange);

                // If no match was found then we can add this port subscription
                if (NOPOS == ListIndex) {
                    ListIndex = MSG_ListInsertSorted(MSG_Application_Port_List_p, (void *) AppPortListData_p, ShortMessageRouter_Port_Compare_InsertSorted);
                } else {
                    // Clear up, as we're unable to add this subscription
                    SMS_HEAP_FREE(&AppPortListData_p);

                    // It is possible for a duplicate application port subscription to be received
                    // in which case the second request will be rejected with error SMS_ERROR_INTERN_APP_PORT_IN_USE
                    MSG_ErrorCode = SMS_ERROR_INTERN_APP_PORT_IN_USE;
                }
            }
        }
    }

    return MSG_ErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Port_Delete
 *
 * @description  Delete a subscription to an application port range
 *
 * @param        PROCESS                     : The ProcessID of the subscriber
 * @param        ClientTag_t                 : The ClientTag information which will be used when notifying the Process of an incoming message
 * @param        SMS_ApplicationPortRange_t* : The application port range and type to unsubscribe from
 *
 * @return       SMS_Error_t             : SMS_ERROR_INTERN_NO_ERROR if OK
 */
/********************************************************************/
SMS_Error_t ShortMessageRouter_Port_Delete(
    const SMS_PROCESS Process,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    if (NULL == ApplicationPortRange_p) {
        MSG_ErrorCode = SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT;
    }
    // In order for the searching/sorting against existing entries the lower port
    // must be less than or equal to the upper port
    else if (ApplicationPortRange_p->LowerPort > ApplicationPortRange_p->UpperPort) {
        MSG_ErrorCode = SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT;
    }
    // First check if the application port list has been initialised
    else {
        if (NULL != MSG_Application_Port_List_p) {
            MSG_App_Port_List_t AppPortListData;
            MSG_ListIndex ListIndex = NOPOS;

            AppPortListData.ProcessID = Process;
            AppPortListData.ClientTag = ClientTag;
            memcpy(&AppPortListData.ApplicationPortRange, ApplicationPortRange_p, sizeof(SMS_ApplicationPortRange_t));

            // We should get back the list index of the application port subscription to delete
            ListIndex = MSG_ListFind(MSG_Application_Port_List_p, (void *) &AppPortListData, ShortMessageRouter_Port_Compare_Complete);

            if (NOPOS != ListIndex) {
                // Get the list item data
                MSG_App_Port_List_t *AppPortListData_p = (MSG_App_Port_List_t *) MSG_ListGet(MSG_Application_Port_List_p, ListIndex);
                // Remove the list entry
                (void) MSG_ListRemove(MSG_Application_Port_List_p, (void *) AppPortListData_p);
                // Free the list item data
                SMS_HEAP_FREE(&AppPortListData_p);
                MSG_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            }
        }
    }

    return MSG_ErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Port_Find
 *
 * @description  Find the subscribing process of an application port range
 *
 * @param        SMS_ApplicationPortRange_t : The application port range and type to find
 * @param        PROCESS*                   : The ProcessID of the subscriber, if the application port range is being subscribed to
 * @param        ClientTag_t*               : Client tag data associated with process
 *
 * @return       SMS_Error_t            : SMS_ERROR_INTERN_NO_ERROR if OK
 */
/********************************************************************/
SMS_Error_t ShortMessageRouter_Port_Find(
    SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_PROCESS * const Process_p,
    SMS_ClientTag_t * const ClientTag_p)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    if (NULL == ApplicationPortRange_p || NULL == Process_p) {
        MSG_ErrorCode = SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT;
    } else {
        if (NULL != MSG_Application_Port_List_p) {
            MSG_App_Port_List_t AppPortListData;
            MSG_ListIndex ListIndex = NOPOS;

            memcpy(&AppPortListData.ApplicationPortRange, ApplicationPortRange_p, sizeof(SMS_ApplicationPortRange_t));

            // We should get back the list index of a match to the application port range, from which we can extract the subscribing ProcessId
            ListIndex = MSG_ListFind(MSG_Application_Port_List_p, (void *) &AppPortListData, ShortMessageRouter_Port_Compare_PortRange);

            if (NOPOS != ListIndex) {
                // Get the list item data
                MSG_App_Port_List_t *AppPortListData_p = (MSG_App_Port_List_t *) MSG_ListGet(MSG_Application_Port_List_p, ListIndex);
                ApplicationPortRange_p->ApplicationPort = AppPortListData_p->ApplicationPortRange.ApplicationPort;
                *Process_p = AppPortListData_p->ProcessID;
                *ClientTag_p = AppPortListData_p->ClientTag;
                MSG_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            }
        }
    }

    return MSG_ErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Port_GetSubscriber
 *
 * @description  Find the subscribing process of an application port range
 *
 * @param        ShortMessage_p   : The short message from which the application ports will be extracted
 * @param        PROCESS*         : The ProcessID of the subscriber, if the application port range is being subscribed to
 * @param        ClientTag_t*     : The client tag data to be retrieved from the find operation
 *
 * @return       SMS_Error_t  : SMS_ERROR_INTERN_NO_ERROR if OK
 */
/********************************************************************/
SMS_Error_t ShortMessageRouter_Port_GetSubscriber(
    ShortMessage_p thisShortMessage,
    SMS_PROCESS * const Process_p,
    SMS_ClientTag_t * const ClientTag_p,
    SMS_ApplicationPortRange_t * ApplicationPortRange_p)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    ShortMessageApplicationPorts_t ApplicationPorts;

    // Extract application ports from short message
    ShortMessage_GetApplicationPorts(thisShortMessage, &ApplicationPorts);

    if (ApplicationPorts.UseApplicationPorts) {
        // Prioritize the matching.  First check for a subscription against
        // the destination port.
        ApplicationPortRange_p->ApplicationPort = SMS_APPLICATION_PORT_DESTINATION;
        ApplicationPortRange_p->LowerPort = ApplicationPorts.DestinationPort;
        ApplicationPortRange_p->UpperPort = ApplicationPorts.DestinationPort;

        // Find a subscriber of the destination port
        MSG_ErrorCode = ShortMessageRouter_Port_Find(ApplicationPortRange_p, Process_p, ClientTag_p);

        // Otherwise check for a subscription against the origination port.
        if (SMS_ERROR_INTERN_NO_ERROR != MSG_ErrorCode) {
            ApplicationPortRange_p->ApplicationPort = SMS_APPLICATION_PORT_ORIGINATION;
            ApplicationPortRange_p->LowerPort = ApplicationPorts.OriginatorPort;
            ApplicationPortRange_p->UpperPort = ApplicationPorts.OriginatorPort;

            // Find a subscriber of the origination port
            MSG_ErrorCode = ShortMessageRouter_Port_Find(ApplicationPortRange_p, Process_p, ClientTag_p);
        }
    }

    return MSG_ErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_GetCurrentNumberOfRules
 *
 * @description  Find the current number of routing rules in the SMRuleTable.
 *
 * @return       static uint8_t : The current number of routing rules.
 */
/********************************************************************/
static uint8_t ShortMessageRouter_GetCurrentNumberOfRules(
    void)
{
    static uint8_t numberOfRules = 0;

    if (!numberOfRules) {
        ShortMessageRouterRulesUsed_t emptyRulesUsed;
        ShortMessageRouterRulesUsed_t *currentRulesUsed_p = NULL;
        uint8_t i;

        memset(&emptyRulesUsed, 0x00, sizeof(ShortMessageRouterRulesUsed_t));

        for (i = 0; i < MAX_RULES; i++) {
            currentRulesUsed_p = &SMRuleTable[i].ACTIVERULES;

            if (memcmp(currentRulesUsed_p, &emptyRulesUsed, sizeof(ShortMessageRouterRulesUsed_t)) == 0) {
                break;
            }

            numberOfRules += 1;
        }
    }

    return numberOfRules;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageRouter_Deinitialise
 *
 * @description  Deinitialise the Short Message Router
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageRouter_Deinitialise(
    void)
{
    if (NULL != MSG_Application_Port_List_p) {
        MSG_App_Port_List_t *NextAppPortListData_p = MSG_ListGetFirst(MSG_Application_Port_List_p);
        while (NULL != NextAppPortListData_p) {
            (void) MSG_ListRemove(MSG_Application_Port_List_p, NextAppPortListData_p);
            SMS_HEAP_FREE(&NextAppPortListData_p);
            NextAppPortListData_p = MSG_ListGetFirst(MSG_Application_Port_List_p);
        }
        MSG_ListDestroy(MSG_Application_Port_List_p);
    }
}
