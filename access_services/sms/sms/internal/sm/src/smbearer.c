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
 * SMS Server smbearer.c implementation file.
 *
 *************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"


/* The Message Transport Server Interface */
#include "g_sms.h"
#include "r_sms.h"
#include "d_sms.h"

/* The Cell Broadcast Server Interface */
#include "t_cbs.h"

/* Dependant Modules */
#ifdef SMS_USE_MFL_UICC
#include "r_sim.h"
#endif                          // SMS_USE_MFL_UICC

#ifdef SMS_USE_MFL_MODEM
#include "r_rms.h"
#include "r_dispatch.h"
#else
#include <stdint.h>
#include "mal_sms.h"
#endif                          // SMS_USE_MFL_MODEM

/* Message Module Include Files */
#include "sm.h"
#include "smsevnthndr.h"
#include "smbearer.h"
#include "smrouter.h"
#include "smtpdu.h"
#include "smutil.h"
#include "smutil.h"
#include "smsmain.h"
#include "smstrg.h"

/* Cell Broadcast Include Files */
#include "cbmngr.h"

/* Configuration */
#include "c_sms_config.h"

#define TP_UDHI_BIT       0x40
#define TP_MTI_MASK       0x03
#define CONCAT_8_BIT_REF  0x00
#define CONCAT_16_BIT_REF 0x08

#define CONCATENATED_8BIT_PARAMS_SIZE  3        // bytes
#define CONCATENATED_16BIT_PARAMS_SIZE 4        // bytes

#define SMS_RELAY_LINK_CONTROL_TIMEOUT (5)      //seconds

#ifndef SMS_USE_MFL_MODEM
#define SMS_MAL_CONFIGURE_RETRY_TIMEOUT (5)     //seconds
#endif

typedef enum {
    SHORTMESSAGEBEARER_IDLE = 1,        // doing nothing or sending right now
    SHORTMESSAGEBEARER_RECEIVING        // waiting for ack from application from router
} ShortMessageBearerState_t;

typedef struct {
    SMS_RelayControl_t RelayLinkControl_Setting;
    uint8_t SeqNumToSend;
} SMBearer_LinkControlSetting_t;

typedef struct ShortMessageBearerObject {
    ShortMessageBearerState_t ShortMessageBearerState;
    ShortMessage_p SendingShortMessage;
    SMBearer_LinkControlSetting_t SmBearer_LinkControl;
    uint8_t Sending;
    uint8_t MemoryCapacityAvailablePending;
} ShortMessageBearer_t;


//-------------------------------------------------------------------------------
// Local Event Handler Functions
//-------------------------------------------------------------------------------
static void ShortMessageBearer_TransmitSM_MO(
    ShortMessage_p);
static SMS_Error_t ShortMessageBearer_SendReport(
    ShortMessageBearer_p,
    ShortMessage_p,
    SMS_RP_ErrorCause_t,
    uint8_t);
static ShortMessageType_t ShortMessageBearer_CheckMsgType_MT_SMS(
    ShortMessageBearer_p,
    uint8_t *);
static void FixUserDataHeaderIndication(
    uint8_t *);
static void ShortMessageBearer_InitShortMessage(
    ShortMessage_p,
    ShortMessagePackedAddress_t *,
    uint8_t,
    uint8_t *);

// EFSMSS FILE UTILITY FUNCTION PROTOTYPE
static void ShortMessageBearer_MemCapacityExWr(
    const uint8_t);
// End local functions prototypes

static void ShortMessageBearer_ReceiveShortMessage(
    void *,
    EventData_p);
static void ShortMessageBearer_ReceiveFailureCause(
    void *,
    EventData_p);
static void ShortMessageBearer_ApplicationAck_Idle(
    void *,
    EventData_p);
static void ShortMessageBearer_ApplicationAckWithConfirm_Receiving(
    void *,
    EventData_p);
static void ShortMessageBearer_Command(
    void *,
    EventData_p);
static void ShortMessageBearer_MemoryAvailableNotification(
    void *,
    EventData_p);
static void ShortMessageBearer_HandlePendingMemoryNotification(
    void *,
    EventData_p);
static void ShortMessageBearer_NetworkAcknowledgement(
    void *,
    EventData_p);
static void ShortMessageBearer_RelayLinkTimeout(
    void *,
    EventData_p);
static void ShortMessageBearer_DeliverReportConfirm(
    void *,
    EventData_p);
static void ShortMessageBearer_Initialise_ModemFds(
    int *const Modem_fd_p
#ifndef SMS_USE_MFL_MODEM
    ,
    int *const Netlnk_fd_p
#endif
    );
static void ShortMessageBearer_TPDUDeliver(
    void *,
    EventData_p);

#ifndef SMS_USE_MFL_MODEM
static EventData_t ShortMessageBearer_EventData;

static void ShortMessageBearer_MAL_Callback(
    int event_id,
    void *data);
#endif                          // SMS_USE_MFL_MODEM

// Message Event, State & Handler Table
//=====================================================================================================================
static EventAction_t ShortMessageBearerStateEventTable[] = {
    {SHORTMESSAGEBEARER_RECEIVE_SHORT_MESSAGE, SHORTMESSAGEBEARER_IDLE, ShortMessageBearer_ReceiveShortMessage},
    {SHORTMESSAGEBEARER_RECEIVE_FAILURE, ALL_STATES, ShortMessageBearer_ReceiveFailureCause},
    {SHORTMESSAGEBEARER_APPLICATION_ACK_WITH_CNF, SHORTMESSAGEBEARER_IDLE, ShortMessageBearer_ApplicationAck_Idle},
    {SHORTMESSAGEBEARER_APPLICATION_ACK_WITH_CNF, SHORTMESSAGEBEARER_RECEIVING, ShortMessageBearer_ApplicationAckWithConfirm_Receiving},
    {SHORTMESSAGEBEARER_COMMAND_REQ, ALL_STATES, ShortMessageBearer_Command},
    {SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, ALL_STATES, ShortMessageBearer_MemoryAvailableNotification},
    {SHORTMESSAGEBEARER_NETWORK_ACK, ALL_STATES, ShortMessageBearer_NetworkAcknowledgement},
    {SHORTMESSAGEBEARER_RELAY_LINK_TIMEOUT, ALL_STATES, ShortMessageBearer_RelayLinkTimeout},
    {SHORTMESSAGEBEARER_DELIVERREPORT_CONFIRM, ALL_STATES, ShortMessageBearer_DeliverReportConfirm},
    {SHORTMESSAGEBEARER_TPDUDELIVER, ALL_STATES, ShortMessageBearer_TPDUDeliver},
    {END_OF_TABLE, END_OF_TABLE, END_OF_TABLE_FUNCTION}
};

// This object is static
static ShortMessageBearer_t thisShortMessageBearer = {
    SHORTMESSAGEBEARER_IDLE,    // ShortMessageBearerState
    NULL,                       // SendingShortMessage
    {SMS_RELAY_CONTROL_DISABLED, 1},    // SmBearer_LinkControl
    FALSE,                      // Sending
    FALSE
};

#ifdef SMS_ENABLE_FEATURE_ROUTE_PS_ONLY
static SMS_MO_Route_t mo_route_setting = SMS_MO_ROUTE_PS;
#elif SMS_ENABLE_FEATURE_ROUTE_CS_ONLY
static SMS_MO_Route_t mo_route_setting = SMS_MO_ROUTE_CS;
#else
static SMS_MO_Route_t mo_route_setting = SMS_MO_ROUTE_CS_PS;
#endif

static mal_sms_route_info_t ShortMessageBearer_Translate_SMS_RouteInfo(
    const SMS_MO_Route_t sms_route_info);

ShortMessageBearer_p theShortMessageBearer_p = &thisShortMessageBearer;



/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_Initialise
 *
 * @description  Initialise short message bearer
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageBearer_Initialise(
    int *Modem_fd_p
#ifndef SMS_USE_MFL_MODEM
    ,
    int *Netlnk_fd_p
#endif
    )
{
    // Initialise Modem File Descriptors
    ShortMessageBearer_Initialise_ModemFds(Modem_fd_p
#ifndef SMS_USE_MFL_MODEM
                                           , Netlnk_fd_p
#endif
        );
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_MemCapacityInitialise
 *
 * @description  Initialise short message bearer
 *
 * @param        void
 *
 * @return       void
 *
 * NOTES:
 *               Initialise the Memory Capacity Exceeded Notification Flag
 *               within Persistent Data from the field stored on the SIM (if it exists).
 *               In the event that the new SIM does not support the EfSmssRecord,
 *               the Memory Capacity Exceeded Notification Flag from Persistent Data
 *               will be used. If Memory Capacity Exceeded is set or there is a SIM
 *               other than unsupported service a memory available notification will
 *               be sent to the network when free capacity is detected.
 */
/********************************************************************/
void ShortMessageBearer_MemCapacityInitialise(
    void)
{
    uint8_t MemoryCapacityExceededNotification = FALSE;

#ifdef SMS_USE_MFL_UICC
    UICC_ServiceHandle_t ServiceHandle = SmUtil_UICC_ServiceHandleGet();
    SIM_ISO_ErrorCause_t ErrorCause;
    SMR_MemCap_t SMR_MemCap;
    RequestStatus_t RequestStatus;
    uint8_t Available = FALSE;
    uint8_t Enabled = FALSE;


    SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemCapacityInitialise: thisShortMessageBearer.MemoryCapacityAvailablePending = %d", thisShortMessageBearer.MemoryCapacityAvailablePending));

    RequestStatus = Request_SIM_Service_GetStatus(WAIT_RESPONSE, ServiceHandle, SIM_SRV_SHORT_MESSAGE, &Available, &Enabled, &ErrorCause);

    if (REQUEST_OK == RequestStatus && TRUE == Available && TRUE == Enabled) {
        // Read memory capacity exceeded field from SIM
        if (R_Req_SIM_SMR_MemCapExcStatus_Read(WAIT_RESPONSE, ServiceHandle, &SMR_MemCap, &ErrorCause) == REQUEST_OK) {
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemCapacityInitialise: R_Req_SIM_SMR_MemCapExcStatus_Read MemoryExceeded: %d", SMR_MemCap));

            switch (SMR_MemCap) {
            case SMR_MEM_EXCEEDED:
                MemoryCapacityExceededNotification = TRUE;
                break;

            case SMR_MEM_AVAILABLE:
            default:
                MemoryCapacityExceededNotification = FALSE;
                break;
            }
        } else if (SIM_ISO_ERROR_APPL_NO_SERVICE == ErrorCause || SIM_ISO_ERROR_6A82_FILE_NOT_FOUND == ErrorCause) {
            // SMS not supported on SIM so read Memory Capacity Exceeded from Persistent Data.
            // This should not be reached since Request_SIM_Service_GetStatus should have returned
            // not available.
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemCapacityInitialise: R_Req_SIM_SMR_MemCapExcStatus_Read SIM ErrorCause: %d", ErrorCause));
            (void) SmUtil_Get_MSG_MemoryCapacityExceededFlag(&MemoryCapacityExceededNotification);
        } else {
            // The memory capacity exceeded field could not be read from the SIM.
            // A memory available notification will be sent to the network when free capacity is detected.
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemCapacityInitialise: R_Req_SIM_SMR_MemCapExcStatus_Read SIM ErrorCause: %d", ErrorCause));
            MemoryCapacityExceededNotification = TRUE;
        }
    } else {
        // SMS not supported on SIM or requester error so read Memory Capacity Exceeded from Persistent Data
        SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemCapacityInitialise: Request_SIM_Service_GetStatus SIM ErrorCause: %d Available: %d Enabled: %d", ErrorCause, Available, Enabled));
        (void) SmUtil_Get_MSG_MemoryCapacityExceededFlag(&MemoryCapacityExceededNotification);
    }
#else
    // Using MAL Modem.
    // Read SIM EFsmss file to get Memory Capacity Exceeded value.
    if (SmStorage_ReadMemoryCapacityExceeded(&MemoryCapacityExceededNotification)) {
        // Memory capacity exceeded flag read from SIM card.
        SMS_A_(SMS_LOG_W("smbearer.c: ShortMessageBearer_MemCapacityInitialise: SmStorage_ReadMemoryCapacityExceeded MemoryExceeded: %d", MemoryCapacityExceededNotification));
    } else {
        SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MemCapacityInitialise: SmStorage_ReadMemoryCapacityExceeded FAILED"));
        // Memory capacity exceeded flag not read from SIM card.
        // Read memory capacity exceeded flag from persistent storage.
        (void) SmUtil_Get_MSG_MemoryCapacityExceededFlag(&MemoryCapacityExceededNotification);
    }
#endif                          //SMS_USE_MFL_UICC

    // Cache the memory capacity exceeded flag in Persistent Data
    (void) SmUtil_Put_MSG_MemoryCapacityExceededFlag(MemoryCapacityExceededNotification);
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_MO_RouteSet
 *
 * @description  Set MO Routing
 *
 * @param        Route : SMS MO Routing.
 *
 * @return       void
 *
 */
/********************************************************************/
void ShortMessageBearer_MO_RouteSet(
    SMS_MO_Route_t Route)
{
    switch (Route) {
        case SMS_MO_ROUTE_PS:
        case SMS_MO_ROUTE_CS:
        case SMS_MO_ROUTE_CS_PS:
            mo_route_setting = Route;
            break;
        default:
            mo_route_setting = SMS_MO_ROUTE_CS_PS;
            break;
    }

    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_MO_RouteSet: mo_route_setting=%d", mo_route_setting));
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_MO_RouteGet
 *
 * @description  Retreive MO Routing
 *
 * @param        void
 *
 * @return       SMS MO Routing (enum value)
 *
 */
/********************************************************************/
SMS_MO_Route_t ShortMessageBearer_MO_RouteGet(
    void)
{
    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_MO_RouteGet: mo_route_setting=%d", mo_route_setting));

    return mo_route_setting;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_HandleEvent
 *
 * @description  Function for the short message bearer to handle event
 *
 * @param        event :     as type of ShortMessageBearerEvent_t.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageBearer_HandleEvent(
    ShortMessageBearerEvent_t event,
    EventData_p eventData)
{
    EventHandlerFunction_p eventHandler;

#ifdef SMS_PRINT_B_
    char *event_p = NULL;
    char *state_p = NULL;
    switch (event) {
    case SHORTMESSAGEBEARER_RECEIVE_SHORT_MESSAGE:
        event_p = "receive_short_message";
        break;
    case SHORTMESSAGEBEARER_APPLICATION_ACK_WITH_CNF:
        event_p = "application_ack";
        break;
    case SHORTMESSAGEBEARER_COMMAND_REQ:
        event_p = "command_req";
        break;
    case SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ:
        event_p = "memory_notification_req";
        break;
    case SHORTMESSAGEBEARER_NETWORK_ACK:
        event_p = "network_ack";
        break;
    case SHORTMESSAGEBEARER_RECEIVE_FAILURE:
        event_p = "receive_failure";
        break;
    default:
        event_p = "unknown event";
        break;
    }
    switch (theShortMessageBearer_p->ShortMessageBearerState) {
    case SHORTMESSAGEBEARER_IDLE:
        state_p = "idle";
        break;
    case SHORTMESSAGEBEARER_RECEIVING:
        state_p = "receiving";
        break;
    default:
        state_p = "unknown state";
        break;
    }
    SMS_LOG_D("smbearer.c: ShortMessageBearer_HandleEvent: %s in %s", event_p, state_p);
#endif                          //PRINT_B_

    eventHandler = EventHandler_Find(theShortMessageBearer_p, ShortMessageBearerStateEventTable, event, theShortMessageBearer_p->ShortMessageBearerState);

    (eventHandler) (theShortMessageBearer_p, (EventData_p) eventData);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_ApplicationAck_Idle
 *
 * @description  This function deletes a received delivery report that
 *               is not required to be sent to the network.
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_ApplicationAck_Idle(
    void *object,
    EventData_p eventData)
{
    SMS_IDENTIFIER_NOT_USED(object);

    if (eventData->ShortMessage != NULL) {
        // Delete the deliver report
        ShortMessage_Delete(eventData->ShortMessage);
    }

    eventData->ErrorCode = SMS_ERR_RECEPTION_FAILED;
    SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_ApplicationAck_Idle: invalid ShortMessageBearerState: SHORTMESSAGEBEARER_IDLE"));
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_ApplicationAckWithConfirm_Receiving
 *
 * @description  This function sends report to the network on behalf of the application which triggered it
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_ApplicationAckWithConfirm_Receiving(
    void *object,
    EventData_p eventData)
{
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;
    uint8_t sendDeliverReport = TRUE;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_NO_ERROR;

    if (eventData->ShortMessage == NULL) {
        sendDeliverReport = FALSE;
    }

    ErrorCode = ShortMessageBearer_SendReport(thisBearer, eventData->ShortMessage, eventData->RMS_V2_ErrorCode, sendDeliverReport);
    eventData->ErrorCode = ErrorCode;

    if (eventData->ShortMessage != NULL) {
        //don't keep deliver reports
        ShortMessage_Delete(eventData->ShortMessage);
    }

    thisBearer->ShortMessageBearerState = SHORTMESSAGEBEARER_IDLE;

    ShortMessageBearer_HandlePendingMemoryNotification(object, eventData);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_HandlePendingMemoryNotification
 *
 * @description  This function checks if there was a memory available notification
 *               from the storage entity while another MO activity was being processed.
 *               If the bearer is in idle state and the pending flag is on
 *               then send the memory available notification to the network
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_HandlePendingMemoryNotification(
    void *object,
    EventData_p eventData)
{
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;

    SMS_IDENTIFIER_NOT_USED(eventData);

    if (thisBearer->MemoryCapacityAvailablePending == TRUE) {
        ShortMessageBearer_MemoryAvailableNotification(object, NULL);
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_Command
 *
 * @description  Function to handle the event of SHORTMESSAGEBEARER_COMMAND_REQ
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_Command(
    void *object,
    EventData_p eventData)
{
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;

    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_Command: "));
    thisBearer->SendingShortMessage = eventData->ShortMessage;

    // Send the message
    ShortMessageBearer_TransmitSM_MO(eventData->ShortMessage);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_CheckMsgType_MT_SMS
 *
 * @description  The bearer has received indication from the SMR process about a new incoming
 *               (mobile terminated) short message.
 *               The incoming message type should be either SMS_DELIVER or  SMS_STATUS_REPORT.
 *               If it is neither, discard the whole TPDU and send error report (MANDATORY_IE_ERROR) to SMR
 *
 * @param        thisBearer  : as type of ShortMessageBearer_p.
 * @param        *UserData_p : TPDU data.
 *
 * @return       static ShortMessageType_t
 */
/********************************************************************/
static ShortMessageType_t ShortMessageBearer_CheckMsgType_MT_SMS(
    ShortMessageBearer_p thisBearer,
    uint8_t * UserData_p)
{

    ShortMessageType_t shortMessageType = SM_TYPE_LAST;
    uint8_t userDataHeaderLength;
    uint8_t *userDataFirstIEI_p;
    uint8_t msgType;
    uint8_t validMsgType = FALSE;

    SMS_IDENTIFIER_NOT_USED(thisBearer);

    msgType = UserData_p[0] & TP_MTI_MASK;

    if (msgType == SMS_RESERVED_MESSAGE) {
        msgType = SMS_DELIVER;
    }

    switch (msgType) {
    case SMS_DELIVER:
        {
            SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_CheckMsgType_MT_SMS: SMS_DELIVER "));
            shortMessageType = SM_DELIVER;
            FixUserDataHeaderIndication(UserData_p);
            validMsgType = TRUE;
            break;
        }
    case SMS_STATUS_REPORT:
        {
            SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_CheckMsgType_MT_SMS: SMS_STATUS_REPORT "));
            shortMessageType = SM_STATUS_REPORT;
            validMsgType = TRUE;
            break;
        }
    default:
        {
            ShortMessage_p DeliveryShortMessage;

            SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_CheckMsgType_MT_SMS: PROTOCOL ERROR UNSPECIFIED"));
            // Create the delivery report Short Message.
            DeliveryShortMessage = ShortMessage_Create(SM_DELIVER_REPORT_ERROR);

            if (DeliveryShortMessage == NULL) {
                // Failed to create delivery report.
                SMS_A_(SMS_LOG_E("smbearer.c: Failed to create delivery report"));
            } else {
                ShortMessage_SetAcknowledgeRequired(DeliveryShortMessage, TRUE);

                (void) ShortMessage_PackTPDUHeader(DeliveryShortMessage, TP_UNSPECIFIED_ERROR_CAUSE, NULL, NULL, FALSE, FALSE, NULL, NULL, NULL, NULL);

                // Send Protocol error unspecified report
                (void) ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED);
            }

            validMsgType = FALSE;
            break;
        }
    }                           // end switch

    if (validMsgType) {
        // Find the start of the User Data, and derive from this the first IEI position & the UHDL
        userDataHeaderLength = TPDU_GetField(TP_UDHL, UserData_p, shortMessageType);

        userDataFirstIEI_p = TPDU_GetFieldPtr(TP_UDH, UserData_p, shortMessageType);

        // Verify message integrity and set UDH bit if necessary - this is because some nets
        // do not set the UDH bit correctly!!!
        if (CheckTPUserDataIntegrity(userDataFirstIEI_p, userDataHeaderLength)) {
            SMS_B_(SMS_LOG_D("smbearer.c: Setting UDHI bit"));
            UserData_p[0] = UserData_p[0] | TP_UDHI_BIT;
        } else {
            SMS_B_(SMS_LOG_D("smbearer.c: Not Setting UDHI bit"));
        }
    }

    return (shortMessageType);
}

/********************************************************************/
/**
 *
 * @function     FixUserDataHeaderIndication
 *
 * @description  This function checks if the User Data Header is present, even when UDH
 *               Indication bit in the first octet of the relevant TPDU is NOT set.
 *               The current experience (Jan, 2000) shows that this is a very
 *               likely scenario, i.e. even when set by a sending entity, the UDH
 *               indication bit appears as NOT set at the destination side.
 *
 * @param        *TPDUStart_p : uint8_t.
 *
 * @return       static void
 *
 * NOTE:         This bodge (sorry, check) will ONLY work in detecting concatenated
 *               short messages! (Providing the UDH contains the concatenated msg
 *               parameters only, and no other possible UDH IEs .)
 */
/********************************************************************/
static void FixUserDataHeaderIndication(
    uint8_t * TPDUStart_p)
{
    uint8_t *userDataLength_p;
    uint8_t *userDataStart_p;

    if (!(*TPDUStart_p & TP_UDHI_BIT)) {
        SMS_B_(SMS_LOG_D("smbearer.c: FixUserDataHeaderIndication: UDHI not present"));
        // don't trust the network
        userDataLength_p = TPDU_GetFieldPtr(TP_UDL, TPDUStart_p, SM_DELIVER);
        userDataStart_p = userDataLength_p + 1;

        if ((userDataLength_p != NULL) && (*userDataLength_p != 0)) {
            if ((*(userDataStart_p) == CONCATENATED_8BIT_USER_DATA_HEADER_SIZE)
                && (*(userDataStart_p + 1) == CONCAT_8_BIT_REF)
                && (*(userDataStart_p + 2) == CONCATENATED_8BIT_PARAMS_SIZE)) {
                // Looks like a 8-bit ref concatenated message set the UDH Indicator bit
                *TPDUStart_p |= TP_UDHI_BIT;
                SMS_B_(SMS_LOG_D(" - Fixed 8 bit concat refNum"));
            } else if ((*(userDataStart_p) == CONCATENATED_16BIT_USER_DATA_HEADER_SIZE)
                       && (*(userDataStart_p + 1) == CONCAT_16_BIT_REF)
                       && (*(userDataStart_p + 2) == CONCATENATED_16BIT_PARAMS_SIZE)) {
                // Looks like a 16-bit ref concatenated message set the UDH Indicator bit
                *TPDUStart_p |= TP_UDHI_BIT;
                SMS_B_(SMS_LOG_D(" - Fixed 16 bit concat refNum"));
            }
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_InitShortMessage
 *
 * @description  This function initialises a short message object
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 * @param        *SCAddress_p : as type of ShortMessagePackedAddress_t.
 * @param        TPDU_Length : uint8_t.
 * @param        *TPDU_Data_p : uint8_t.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_InitShortMessage(
    ShortMessage_p thisShortMessage,
    ShortMessagePackedAddress_t * SCAddress_p,
    uint8_t TPDU_Length,
    uint8_t * TPDU_Data_p)
{
    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_InitShortMessage: "));
    ShortMessage_SetPackedServiceCentreAddress(thisShortMessage, SCAddress_p);
    ShortMessage_SetTPDU(thisShortMessage, TPDU_Data_p, TPDU_Length);
    (void) ShortMessage_SetStatus(thisShortMessage, SMS_STATUS_UNREAD);
    ShortMessage_SetSequenceNumber(thisShortMessage);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_RelayLinkControl_Set
 *
 * @description  Sets the relay link control setting
 *
 * @param        RelayLinkControl : SMS Relay Link Control setting
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageBearer_RelayLinkControl_Set(
    const SMS_RelayControl_t RelayLinkControl)
{
    theShortMessageBearer_p->SmBearer_LinkControl.RelayLinkControl_Setting = RelayLinkControl;
    theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend = 1;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_RelayLinkControl_Get
 *
 * @description  Gets the relay link control setting
 *
 * @param        RelayLinkControl_p : Poitner to place the SMS Relay Link Control setting
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageBearer_RelayLinkControl_Get(
    SMS_RelayControl_t * const RelayLinkControl_p)
{
    *RelayLinkControl_p = theShortMessageBearer_p->SmBearer_LinkControl.RelayLinkControl_Setting;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_RelayLinkTimeout
 *
 * @description  Handles the link relay timeout event
 *
 * @param        object :    void ptr to smbearer object
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageBearer_RelayLinkTimeout(
    void *object,
    EventData_p eventData)
{
    SMS_IDENTIFIER_NOT_USED(object);
    SMS_IDENTIFIER_NOT_USED(eventData);

    // Link timer has expired if set to enabled_timeout, set to disabled
    if (SMS_RELAY_CONTROL_TIMEOUT_ENABLED == theShortMessageBearer_p->SmBearer_LinkControl.RelayLinkControl_Setting) {
        SMS_B_(SMS_LOG_D
               ("smbearer.c: ShortMessageBearer_RelayLinkTimeout: Timeout expired RLC closing STATE %d -> %d", theShortMessageBearer_p->SmBearer_LinkControl.RelayLinkControl_Setting,
                SMS_RELAY_CONTROL_DISABLED));
        theShortMessageBearer_p->SmBearer_LinkControl.RelayLinkControl_Setting = SMS_RELAY_CONTROL_DISABLED;
    }
    // The radio link will be closing now, so set the sequence number to indicate that the next time
    // we send a short message we will be sending the first message for that opening of the
    // radio link.  See ShortMessageBearer_TransmitSM_MO for how this flag is used to control
    // the SMR radio link settings.
    theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend = 1;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_ReceiveFailureCause
 *
 * @description  <Detailed description of the method>
 *
 * @param        *object   : not used.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_ReceiveFailureCause(
    void *object,
    EventData_p eventData)
{
#ifndef SMS_USE_MFL_MODEM
    SMS_IDENTIFIER_NOT_USED(object);
    SMS_IDENTIFIER_NOT_USED(eventData);
#else
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;
    RMS_V2_Status_t FailureCause;

    if (Event_RMS_V2_FailureReceivedInd(eventData->DataPointer, &FailureCause) == GS_EVENT_OK) {
        if (!FailureCause.SuccessfulOutcome) {
            SMS_A_(SMS_LOG_I("smbearer.c: NW->SMS Failure Cause = 0x%04x, dispatching event to subscribers.", FailureCause.ErrorCode));

            Dispatch_SMS_ErrorOccurred(FailureCause.ErrorCode);

            // Reset state
            if (thisBearer->ShortMessageBearerState == SHORTMESSAGEBEARER_RECEIVING) {
                SMS_B_(SMS_LOG_D("smbearer.c: Resetting state to IDLE."));
                thisBearer->ShortMessageBearerState = SHORTMESSAGEBEARER_IDLE;
            }
        }
    }
#endif                          //SMS_USE_MFL_MODEM
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_ProcessShortMessage
 *
 * @description  Function to process a received Short Message
 *
 * @param        *object             :   void.
 * @param        eventData           : as type of EventData_p.
 * @param        SMSC_Address_TPDU_p : as type of SMS_SMSC_Address_TPDU_t.
 * @param        AcknowledgeRequired : Whether or not an acknowledge is required.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_ProcessShortMessage(
    void *object,
    EventData_p eventData,
    SMS_SMSC_Address_TPDU_t * SMSC_Address_TPDU_p,
    const uint8_t AcknowledgeRequired)
{
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;
    ShortMessageType_t shortMessageType;

    if (SMSC_Address_TPDU_p == NULL) {
        // alloc failed
        SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_ProcessShortMessage: SMSC_Address_TPDU_p == NULL"));
    } else {
        thisBearer->ShortMessageBearerState = SHORTMESSAGEBEARER_RECEIVING;

        shortMessageType = ShortMessageBearer_CheckMsgType_MT_SMS(thisBearer, SMSC_Address_TPDU_p->TPDU.Data);

        if (shortMessageType < SM_TYPE_LAST) {
            eventData->ShortMessage = ShortMessage_Create(shortMessageType);

            if (eventData->ShortMessage != NULL) {
                // Allocate storage for the TPDU as it can't go into the cache yet
                if (ShortMessage_AllocateTPDUStorage(eventData->ShortMessage)) {
                    ShortMessageBearer_InitShortMessage(eventData->ShortMessage,
                                                        (ShortMessagePackedAddress_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress, SMSC_Address_TPDU_p->TPDU.Length, SMSC_Address_TPDU_p->TPDU.Data);

                    SMS_A_(SMS_LOG_I("smbearer.c: RX "));
                    SMS_A_(ShortMessage_Print(eventData->ShortMessage));
                    SMS_A_(ShortMessage_HexPrint((ShortMessagePackedAddress_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress, &SMSC_Address_TPDU_p->TPDU));

                    ShortMessage_SetAcknowledgeRequired(eventData->ShortMessage, AcknowledgeRequired);

                    // Set up a timer that will trigger a NACK to the network upon expire
                    Do_SMS_TimerSet_1_MS_WCT(MSG_SMS_DELIVER_REPORT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_DELIVER_REPORT_TIMEOUT_VALUE * 1000);

                    // Get the Router to route this message
                    ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_NEW_SHORT_MESSAGE, eventData);

                    // If the short message has not been deallocated by some other part
                    // of messaging then we'll do it ourseleves here.
                    if (ShortMessage_Exists(eventData->ShortMessage)) {
                        ShortMessage_DeallocateTPDUStorage(eventData->ShortMessage);
                    }
                } else {
                    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_ProcessShortMessage: Allocate TPDU storage Failed"));
                    thisBearer->ShortMessageBearerState = SHORTMESSAGEBEARER_IDLE;
                }
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_ProcessShortMessage: Memory Alloc Failed"));
                thisBearer->ShortMessageBearerState = SHORTMESSAGEBEARER_IDLE;
            }
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_ReceiveShortMessage
 *
 * @description  Function to handle the event of SHORTMESSAGEBEARER_RECEIVE_SHORT_MESSAGE
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_ReceiveShortMessage(
    void *object,
    EventData_p eventData)
{
#ifndef SMS_USE_MFL_MODEM
    // MT SMS Receive event is handled by ShortMessageBearer_MAL_Callback()
    SMS_IDENTIFIER_NOT_USED(object);
    SMS_IDENTIFIER_NOT_USED(eventData);
#else
    SMS_SMSC_Address_TPDU_t *SMSC_Address_TPDU_p = (SMS_SMSC_Address_TPDU_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_SMSC_Address_TPDU_t));
    uint8_t AcknowledgeRequired = FALSE;
    RMS_V2_UserData_t *UserData_p = (RMS_V2_UserData_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(RMS_V2_UserData_t));

    if (SMSC_Address_TPDU_p == NULL || UserData_p == NULL) {
        SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_ReceiveShortMessage: Memory Alloc Failed"));
    } else if (Event_RMS_V2_MessageReceivedInd(eventData->DataPointer, (RMS_V2_Address_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress, UserData_p, &AcknowledgeRequired) == GS_EVENT_OK) {
        SMSC_Address_TPDU_p->TPDU.Length = UserData_p->UserDataLength;
        memcpy(SMSC_Address_TPDU_p->TPDU.Data, UserData_p->UserDataBody, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);
    }

    ShortMessageBearer_ProcessShortMessage(object, eventData, SMSC_Address_TPDU_p, (uint8_t) AcknowledgeRequired);

    // Cleanup
    if (UserData_p != NULL) {
        SMS_HEAP_FREE(&UserData_p);
    }

    if (SMSC_Address_TPDU_p != NULL) {
        SMS_HEAP_FREE(&SMSC_Address_TPDU_p);
    }
#endif                          // SMS_USE_MFL_MODEM
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_MemoryAvailableNotification
 *
 * @description  This function sends memory available notification to the network.
 *               The appropriate flag is set to TRUE to indicate that the next incoming
 *               RESPONSE_RMS_V2_MESSAGE_SEND is the answer to this request.
 *
 *         NOTE: If eventData is NULL this is an MTS internally generated (rather than
 *               client controlled) Memory Capacity Available notification. The request
 *               will therefore only be sent if the Network has already been informed by
 *               the platform that Memory Capacity was exceeded.
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_MemoryAvailableNotification(
    void *object,
    EventData_p eventData)
{
    uint8_t MemoryCapacityEx = TRUE;
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;

    if (eventData == NULL) {
        // Detemine if Network has been informed that Memory is not Available
        if (!SmUtil_Get_MSG_MemoryCapacityExceededFlag(&MemoryCapacityEx)) {
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: Get_MSG_MemoryCapacityExceededFlag failed"));
            MemoryCapacityEx = FALSE;
        }
    }

    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: Get_MSG_MemoryCapacityExceededFlag=%d", MemoryCapacityEx));

    if (MemoryCapacityEx == TRUE) {
        SMS_B_(SMS_LOG_D
               ("smbearer.c: ShortMessageBearer_MemoryAvailableNotification, thisBearer->ShortMessageBearerState = %d, thisBearer->Sending = %d", thisBearer->ShortMessageBearerState,
                thisBearer->Sending));

        if (thisBearer->ShortMessageBearerState == SHORTMESSAGEBEARER_IDLE && thisBearer->Sending == FALSE) {
            // Network Informed that memory exceeded. Thus it is valid to inform the newtork that memory has now become available
#ifdef SMS_USE_MFL_MODEM
            RequestControlBlock_t *ReqCtrlBlk_p;

            ReqCtrlBlk_p = mfl_request_control_block_alloc(MessagingSystem_GetModemSessionData(), SMS_MFL_DEFAULT_CLIENT_TAG);

            if (ReqCtrlBlk_p != NULL) {
                if (R_Req_RMS_V2_Message_SendMemoryAvailable(ReqCtrlBlk_p, NULL, NULL) != REQUEST_PENDING) {
                    SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: failed to send memory available"));
                } else {
                    SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: waiting for NW acknowledgement"));

                    thisBearer->Sending = TRUE;
                }

                mfl_request_control_block_free(ReqCtrlBlk_p);
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: mfl_request_control_block_alloc failed"));
            }
#else                           // SMS_USE_MFL_MODEM

            if (mal_sms_request_set_memory_status(TRUE) == MAL_SMS_CBS_SUCCESS) {       // we are in the Ex state and need to notify modem we now have memory available
                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: sent to NW"));
                thisBearer->Sending = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: failed to send memory available"));
            }

#endif                          // SMS_USE_MFL_MODEM
        } else {
            thisBearer->MemoryCapacityAvailablePending = TRUE;
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemoryAvailableNotification: thisBearer->MemoryCapacityAvailablePending = %d SET?", thisBearer->MemoryCapacityAvailablePending));
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_ProcessNetworkAck
 *
 * @description  Function to process acknowledgement of a previously sent SMS
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_ProcessNetworkAck(
    void *object,
    EventData_p eventData,
    SMS_TPDU_t * userData_p)
{
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;
    MoreToSendState_t MoreToSendState = STATE_DISABLED;
    eventData->ShortMessage = NULL;

    if (eventData->RMS_V2_ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_I("smbearer.c: NW->MSG ACK OK"));
    } else if (userData_p != NULL && userData_p->Length) {
        uint8_t TP_FailureCause = TPDU_GetField(TP_FCS,
                                                userData_p->Data,
                                                SM_SUBMIT_REPORT_ERROR);

        // User Data present so use the TP Failure Cause
        //
        // Convert TP-FCS byte to SMS_Error_t.
        eventData->RMS_V2_ErrorCode = 0x1000;
        eventData->RMS_V2_ErrorCode |= TP_FailureCause;
        SMS_A_(SMS_LOG_I("smbearer.c: NW->MSG NACK TP Failure Cause 0x%04x", eventData->RMS_V2_ErrorCode));
    } else {
        // No user data so use the RP Failure Cause
        SMS_A_(SMS_LOG_I("smbearer.c: NW->MSG NACK RP Failure Cause 0x%04x", eventData->RMS_V2_ErrorCode));
    }

    if (thisBearer->SendingShortMessage == NULL) {
        /*
         * REFERENCE GSM03.40 Section 10.3
         * Upon acknowledgement to the Memory Capacity Available Notification
         * (Positive or Negative) the memoryCapacity Exceeded Notification
         * indication will be cleared
         */
        thisBearer->MemoryCapacityAvailablePending = FALSE;

        SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: thisBearer->MemoryCapacityAvailablePending = %d CLEAR?", thisBearer->MemoryCapacityAvailablePending));
        // Update Memory Capacity Status
        ShortMessageBearer_MemCapacityExWr(FALSE);

        // If memory capacity available was triggered via the swbp requester rather than
        // internally by the platform the message manager will send a response signal.
        ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_MEMCAPACITYAVAILABLE_SENT, eventData);

        SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: MEMORY CAPACITY AVAILABLE"));
    } else {
        eventData->ShortMessage = thisBearer->SendingShortMessage;
        thisBearer->SendingShortMessage = NULL;
        MoreToSendState = ShortMessage_GetMoreToSend(eventData->ShortMessage);
        ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND_ACKNOWLEDGE, eventData);
    }

    thisBearer->Sending = FALSE;
    ShortMessageBearer_HandlePendingMemoryNotification(object, eventData);

    if (thisBearer->SmBearer_LinkControl.RelayLinkControl_Setting != SMS_RELAY_CONTROL_DISABLED) {
        SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: Restart RLC timer."));
        // Stop the relay link timer and restart it
        Do_SMS_TimerReset_1_MS_WCT(MSG_RELAY_LINK_CONTROL_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);
        Do_SMS_TimerSet_1_MS_WCT(MSG_RELAY_LINK_CONTROL_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_RELAY_LINK_CONTROL_TIMEOUT * 1000);
    }

    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: ShortMessage 0x%08x MoreToSend STATE %d.", (int) eventData->ShortMessage, MoreToSendState));
    if (STATE_OPENING == MoreToSendState || STATE_OPENED == MoreToSendState) {
        SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: Restarting MoreToSend RLC timer, after Network Acknowledgement"));
        // Stop the MoreToSend RLC timer and restart it
        Do_SMS_TimerReset_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);
        Do_SMS_TimerSet_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_MORETOSEND_TIMEOUT_VALUE * 1000);
    } else {
        SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: MoreToSend not requested, timer was not restarted."));
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_NetworkAcknowledgement
 *
 * @description  Function to handle the event of SHORTMESSAGEBEARER_NETWORK_ACK
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_NetworkAcknowledgement(
    void *object,
    EventData_p eventData)
{
#ifndef SMS_USE_MFL_MODEM
    // Response to SMS Send event is handled by ShortMessageBearer_MAL_Callback()
    SMS_IDENTIFIER_NOT_USED(object);
    SMS_IDENTIFIER_NOT_USED(eventData);
#else
    ShortMessageBearer_p thisBearer = (ShortMessageBearer_p) object;
    RMS_V2_UserData_t *UserData_p = (RMS_V2_UserData_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(RMS_V2_UserData_t));
    RMS_V2_Status_t *Status_p = (RMS_V2_Status_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(RMS_V2_Status_t));

    if ((Status_p != NULL) && (UserData_p != NULL)) {
        if (R_Resp_RMS_V2_Message_Send(eventData->DataPointer, UserData_p, Status_p) != REQUEST_OK) {
            // Failure in the response handler
            SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: Response_RMS_V2_Message_Send Failed: %d", Status_p->ErrorCode));
        } else {
            if (Status_p->SuccessfulOutcome) {
                // This is an ack for the previously submitted MO SMS
                eventData->RMS_V2_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;        // indicate success
            } else {
                // No user data so use the RP Failure Cause
                eventData->RMS_V2_ErrorCode = SmUtil_TranslateErrorCodes(Status_p->ErrorCode);
            }

            ShortMessageBearer_ProcessNetworkAck(thisBearer, eventData, (SMS_TPDU_t *) UserData_p);
        }
    }

    if (UserData_p != NULL) {
        SMS_HEAP_FREE(&UserData_p);
    }

    if (Status_p != NULL) {
        SMS_HEAP_FREE(&Status_p);
    }
#endif                          // SMS_USE_MFL_MODEM
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_TransmitSM_MO
 *
 * @description  Send an SM_MO to the SMR_MO_Process_.
 *
 * @param        thisShortMessage : as type of ShortMessage_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_TransmitSM_MO(
    ShortMessage_p thisShortMessage)
{
    SMS_ConcatParameters_t concatDetails;
    SMS_SMSC_Address_TPDU_t *SMSC_Address_TPDU_p = (SMS_SMSC_Address_TPDU_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_SMSC_Address_TPDU_t));
#ifndef SMS_USE_MFL_MODEM
    uint8_t MoreToSend = FALSE;
#else
    RequestControlBlock_t *ReqCtrlBlk_p;
    RMS_V2_TransmissionParams_t TxParams;
    RMS_V2_UserData_t *RequestData_p = (RMS_V2_UserData_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(RMS_V2_UserData_t));
    RMS_V2_Status_t Status;

    // Initialise TxParams for single part SMS send.
    TxParams.MsgSequence.SeqNum = 1;
    TxParams.MsgSequence.Total = 1;
#endif                          // SMS_USE_MFL_MODEM

    if (SMSC_Address_TPDU_p == NULL
#ifdef SMS_USE_MFL_MODEM
        || RequestData_p == NULL
#endif                          // SMS_USE_MFL_MODEM
        ) {
        // alloc failed
        SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_TransmitSM_MO: Memory Alloc Failed"));
    } else {
        ShortMessage_GetPackedServiceCentreAddress(thisShortMessage, (ShortMessagePackedAddress_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress);

        if (SMS_ENABLE_CONCAT_SMS_SEND == 1) {
            // Determine if we should keep the radio link open
            // Get Concatenation (TxParams) details
            ShortMessage_IsConcatenated(thisShortMessage, &concatDetails);
#ifdef SMS_USE_MFL_MODEM
            TxParams.MsgSequence.SeqNum = concatDetails.MsgSequenceNumber;
            TxParams.MsgSequence.Total = concatDetails.MaxNumberOfConcatMsgs;
#else
            if (concatDetails.MsgSequenceNumber == concatDetails.MaxNumberOfConcatMsgs) {
                // The SMS is not a concatenated part or it is the last part of the sequence
                MoreToSend = FALSE;
            } else {
                MoreToSend = TRUE;
            }
#endif                          // SMS_USE_MFL_MODEM

            if (theShortMessageBearer_p->SmBearer_LinkControl.RelayLinkControl_Setting != SMS_RELAY_CONTROL_DISABLED) {
                // When a client has explicitly requested for the link to be preserved (via MoreToSend flag)
                // send message 1/3, then continuously send 2/3
                // When MoreToSend is eventually set to FALSE, no action is performed as the message sequence
                // number should match the total number of message parts which should indicate that the radio
                // link shoule close
                MoreToSendState_t MoreToSendState = ShortMessage_GetMoreToSend(thisShortMessage);

                if (STATE_DISABLED == MoreToSendState) {
                    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_NetworkAcknowledgement: Reset RLC timer."));
                    // Reset the relay link timer
                    Do_SMS_TimerReset_1_MS_WCT(MSG_RELAY_LINK_CONTROL_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);

                    // If this is not a concatenated message, but we should be keeping the link open according
                    // to the RelayLinkControl_Setting, then we adjust the TxParams to make it look like a concat
                    // message, thus fooling the short message relay layer to keep the radio link open expecting
                    // to get another msg part to send.
                    if (FALSE == concatDetails.IsConcatenated) {
#ifdef SMS_USE_MFL_MODEM
                        TxParams.MsgSequence.Total = 3;
                        TxParams.MsgSequence.SeqNum = theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend;
#else
                        MoreToSend = TRUE;
#endif                          // SMS_USE_MFL_MODEM

                        //*************************************************************************************
                        //!!!!!!!!!      TO IMPLEMENT AT+CMMS CORRECTLY REVERSE THE CHANGES MADE     !!!!!!!!!!
                        //!!!!!!!!!                    IN THIS VERSION OF THIS FILE                  !!!!!!!!!!
                        //*************************************************************************************
                        //
                        // WARNING - ER93698, temporary correction to allow a TA: 34.123 TC 16.1.9.1 to pass.
                        //           The TA test maintains the radio link for 3 SMS and the link must be
                        //           shutdown in a controlled manner after the third SMS is sent.
                        //           This effectively breaks the functionality of AT+CMMS which does not
                        //           close the radio link after a specfic number of SMS have been sent.
                        //           This should be reworked to a solution that provides AT+CMMS functionality
                        //           and allows the TA test to pass.
                        //*************************************************************************************
                        if (theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend == 3) {
                            theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend = 1;
#ifndef SMS_USE_MFL_MODEM
                            MoreToSend = FALSE;
#endif                          // SMS_USE_MFL_MODEM
                        } else {
                            theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend++;
                        }
                    }
                } else {
                    switch (MoreToSendState) {
                    case STATE_OPENING:{
                            theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend = 1;
                            break;
                        }
                    case STATE_OPENED:{
                            theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend = 2;
                            break;
                        }
                    case STATE_CLOSING:{
                            theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend = 3;
                            break;
                        }
                    default:{
                            break;
                        }
                    }

#ifdef SMS_USE_MFL_MODEM
                    TxParams.MsgSequence.Total = 3;
                    TxParams.MsgSequence.SeqNum = theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend;
#else
                    if (theShortMessageBearer_p->SmBearer_LinkControl.SeqNumToSend == 3) {
                        MoreToSend = FALSE;
                    } else {
                        MoreToSend = TRUE;
                    }
#endif                          // SMS_USE_MFL_MODEM
                }
            }
        }                       // end Determine if we should keep the radio link open

        (void) ShortMessage_GetTpdu(thisShortMessage, &(SMSC_Address_TPDU_p->TPDU.Length), SMSC_Address_TPDU_p->TPDU.Data);

        SMS_A_(SMS_LOG_I("smbearer.c:  TX "));
        SMS_A_(ShortMessage_Print(thisShortMessage));
        SMS_A_(ShortMessage_HexPrint((ShortMessagePackedAddress_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress, &SMSC_Address_TPDU_p->TPDU));
#ifdef SMS_USE_MFL_MODEM
        SMS_A_(SMS_LOG_I("smbearer.c:  TxParams Total = %d,  SeqNum = %d", TxParams.MsgSequence.Total, TxParams.MsgSequence.SeqNum));
        ReqCtrlBlk_p = mfl_request_control_block_alloc(MessagingSystem_GetModemSessionData(), SMS_MFL_DEFAULT_CLIENT_TAG);

        if (ReqCtrlBlk_p != NULL) {
            RequestData_p->UserDataLength = SMSC_Address_TPDU_p->TPDU.Length;
            memset(RequestData_p->UserDataBody, 0, RMS_V2_MAX_RP_USER_DATA_LENGTH + 1);
            memcpy(RequestData_p->UserDataBody, SMSC_Address_TPDU_p->TPDU.Data, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);

            if (R_Req_RMS_V2_Message_Send(ReqCtrlBlk_p, (RMS_V2_Address_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress, &TxParams, RequestData_p, &Status) != REQUEST_PENDING) {
                SMS_A_(SMS_LOG_W("smbearer.c: ShortMessageBearer_TransmitSM_MO: failed to send. Error code :%d", Status.ErrorCode));
            } else {
                SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_TransmitSM_MO: waiting for SMR ack/nack"));

                thisShortMessageBearer.Sending = TRUE;
            }

            mfl_request_control_block_free(ReqCtrlBlk_p);
        } else {
            SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_TransmitSM_MO: mfl_request_control_block_alloc failed"));
        }

        SMS_HEAP_FREE(&RequestData_p);
#else
        {
            MAL_SMS_SMSC_Address_TPDU_t MAL_SMS_SMSC_Address_TPDU;
            int ErrorCode;

            MAL_SMS_SMSC_Address_TPDU.ServiceCenterAddress.Length = SMSC_Address_TPDU_p->ServiceCenterAddress.Length;
            MAL_SMS_SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress = SMSC_Address_TPDU_p->ServiceCenterAddress.TypeOfAddress;
            memcpy(MAL_SMS_SMSC_Address_TPDU.ServiceCenterAddress.AddressValue, SMSC_Address_TPDU_p->ServiceCenterAddress.AddressValue, MAL_SMS_MAX_PACKED_ADDRESS_LENGTH);

            MAL_SMS_SMSC_Address_TPDU.TPDU.Length = SMSC_Address_TPDU_p->TPDU.Length;
            memcpy(MAL_SMS_SMSC_Address_TPDU.TPDU.Data, SMSC_Address_TPDU_p->TPDU.Data, MAL_SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);

            if ((ErrorCode = mal_sms_request_send_tpdu(&MAL_SMS_SMSC_Address_TPDU, ShortMessageBearer_Translate_SMS_RouteInfo(mo_route_setting), MoreToSend)) != MAL_SMS_CBS_SUCCESS) {
                SMS_A_(SMS_LOG_W("smbearer.c: mal_sms_request_send_tpdu: failed to send. Error Code :%d", ErrorCode));
            } else {
                SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_TransmitSM_MO: More_SMS_ToSend:%d waiting for SMR ack/nack", MoreToSend));

                thisShortMessageBearer.Sending = TRUE;
            }
        }
#endif                          // SMS_USE_MFL_MODEM

        if (SMSC_Address_TPDU_p != NULL) {
            SMS_HEAP_FREE(&SMSC_Address_TPDU_p);
        }
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_SendReport
 *
 * @description  This function sends report to the SMR entity
 *               The cause parameter specifies the reason of failure.
 *               The sendDeliverReport parameter indicates whether the application
 *               acknowledging the message wants the TPDU included in the report.
 *
 * @param        thisBearer :        as type of ShortMessageBearer_p.
 * @param        thisShortMessage :  as type of ShortMessage_p.
 * @param        failureCause :      as type of SMS_RP_ErrorCause_t.
 * @param        sendDeliverReport : uint8_t.
 *
 * @return       static SMS_Error_t
 */
/********************************************************************/
static SMS_Error_t ShortMessageBearer_SendReport(
    ShortMessageBearer_p thisBearer,
    ShortMessage_p thisShortMessage,
    SMS_RP_ErrorCause_t failureCause,
    uint8_t sendDeliverReport)
{
    SMS_Error_t SMS_Error = SMS_ERROR_INTERN_NO_ERROR;

#ifdef SMS_USE_MFL_MODEM
    RMS_V2_UserData_t *UserData_p = (RMS_V2_UserData_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(RMS_V2_UserData_t));
    RMS_V2_Status_t ReportData, ResponseData;
    RequestStatus_t RequestStatus;

    SMS_IDENTIFIER_NOT_USED(thisBearer);

    if (UserData_p != NULL) {
        if (failureCause == RMS_RP_NO_CAUSE) {
            ReportData.SuccessfulOutcome = TRUE;
        } else {
            ReportData.SuccessfulOutcome = FALSE;
        }

        ReportData.ErrorCode = failureCause;

        // Clear the timer that would trigger a NACK to this Short Message if it expired
        Do_SMS_TimerReset_1_MS_WCT(MSG_SMS_DELIVER_REPORT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);

        if (failureCause == SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED) {
            // If there is a Memory Capacity Available Command in pending, waiting to be sent to the
            // Network, it should be cleared here since a Memory Capacity Exceeded state has just been entered
            //
            // NOTE: This could happen if an SMS was received and the memory capacity was exceeded before the
            //       MTS had sent the pending Memory Capacity Available command.
            //
            //       The theoretical scenario would be:
            //
            //       1/ Memory is full
            //       2/ Memory becomes available but Memory Capacity Available command cannot be sent and is set to pending
            //       3/ A Class2 SMS could still be received or an SMS from another Service Centre so memory becomes full again
            //       4/ The pending Memory Capacity Available command should therefore be cleared
            //
            thisBearer->MemoryCapacityAvailablePending = FALSE;
            SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_SendReport: thisBearer->MemoryCapacityAvailablePending = %d CLEAR?", thisBearer->MemoryCapacityAvailablePending));
            // Update the Memory Capacity Available Status
            ShortMessageBearer_MemCapacityExWr(TRUE);
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_SendReport: MEMORY CAPACITY EXCEEDED"));
        }
        if (sendDeliverReport) {
            // If a short message (SMS_DELIVER_REPORT) was created for this report,
            // use the TPDULength calculated at the relevant TPDU set-up
            // If no short message object was created, as in case of
            // MANDATORY_IE_MISSING, set reportLength to 0, as no
            // DeliverReport (i.e. TPDU) is to be sent to SMR
            if (thisShortMessage) {
                (void) ShortMessage_GetTpdu(thisShortMessage, &(UserData_p->UserDataLength), UserData_p->UserDataBody);
            } else {
                UserData_p->UserDataLength = 0;
            }
        } else {
            UserData_p->UserDataLength = 0;
        }

        //Acknowledgement is sent only if requested in dispatcher
        //MMS does not need an acknowledgement back from SMS for the SMS transport over MMS
        if (ShortMessage_GetAcknowledgeRequired(thisShortMessage)) {
            RequestControlBlock_t *ReqCtrlBlk_p;

            SMS_A_(SMS_LOG_I("smbearer.c:  TX DELIVER REPORT "));
            SMS_A_(ShortMessage_Print(thisShortMessage));
            //SMS_A_(ShortMessage_HexPrint(NULL,UserData_p));
            ReqCtrlBlk_p = mfl_request_control_block_alloc(MessagingSystem_GetModemSessionData(), SMS_MFL_DEFAULT_CLIENT_TAG);

            if (ReqCtrlBlk_p != NULL) {
                RequestStatus = R_Req_RMS_V3_Message_SendAcknowledgement(ReqCtrlBlk_p, UserData_p, &ReportData, &ResponseData);

                if (RequestStatus != REQUEST_PENDING) {
                    SMS_B_(SMS_LOG_D("smbearer.c: SMB_SendReport: Send Ack Error. Error Code %d", ResponseData.ErrorCode));
                    SMS_Error = ResponseData.ErrorCode;
                } else if (failureCause == RMS_RP_NO_CAUSE) {
                    SMS_A_(SMS_LOG_I("smbearer.c: MSG->NW ACK OK"));
                } else {
                    SMS_A_(SMS_LOG_I("smbearer.c: MSG->NW NACK Cause 0x%04x", failureCause));
                }

                mfl_request_control_block_free(ReqCtrlBlk_p);
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_SendReport: mfl_request_control_block_alloc failed"));
                SMS_Error = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
            }
        }

        SMS_HEAP_FREE(&UserData_p);
    } else {
        // alloc failed
        SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_SendReport: Memory Alloc Failed (UserData_p)"));
        SMS_Error = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    }

#else
    SMS_SMSC_Address_TPDU_t *SMSC_Address_TPDU_p = (SMS_SMSC_Address_TPDU_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_SMSC_Address_TPDU_t));

    SMS_IDENTIFIER_NOT_USED(thisBearer);

    if (SMSC_Address_TPDU_p != NULL) {
        int ErrorCode;

        // Initialise data structure
        memset(SMSC_Address_TPDU_p, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));
        SMSC_Address_TPDU_p->ServiceCenterAddress.Length = 0;
        SMSC_Address_TPDU_p->TPDU.Length = 0;

        // Clear the timer that would trigger a NACK to this Short Message if it expired
        Do_SMS_TimerReset_1_MS_WCT(MSG_SMS_DELIVER_REPORT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);

        if (failureCause == SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED) {
            // If there is a Memory Capacity Available Command in pending, waiting to be sent to the
            // Network, it should be cleared here since a Memory Capacity Exceeded state has just been entered
            //
            // NOTE: This could happen if an SMS was received and the memory capacity was exceeded before the
            //       MTS had sent the pending Memory Capacity Available command.
            //
            //       The theoretical scenario would be:
            //
            //       1/ Memory is full
            //       2/ Memory becomes available but Memory Capacity Available command cannot be sent and is set to pending
            //       3/ A Class2 SMS could still be received or an SMS from another Service Centre so memory becomes full again
            //       4/ The pending Memory Capacity Available command should therefore be cleared
            //
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_SendReport: MEMORY CAPACITY EXCEEDED"));
            SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_SendReport: thisBearer->MemoryCapacityAvailablePending = %d CLEAR?", thisBearer->MemoryCapacityAvailablePending));
            thisBearer->MemoryCapacityAvailablePending = FALSE;
        }
        if (sendDeliverReport) {
            // If a short message (SMS_DELIVER_REPORT) was created for this report,
            // use the TPDULength calculated at the relevant TPDU set-up
            // If no short message object was created, as in case of
            // MANDATORY_IE_MISSING, set reportLength to 0, as no
            // DeliverReport (i.e. TPDU) is to be sent to SMR
            if (thisShortMessage) {
                (void) ShortMessage_GetTpdu(thisShortMessage, &(SMSC_Address_TPDU_p->TPDU.Length), SMSC_Address_TPDU_p->TPDU.Data);
            } else {
                SMSC_Address_TPDU_p->TPDU.Length = 0;
            }
        } else {
            SMSC_Address_TPDU_p->TPDU.Length = 0;
        }

        //Acknowledgement is sent only if requested in dispatcher
        //MMS does not need an acknowledgement back from SMS for the SMS transport over MMS
        if (ShortMessage_GetAcknowledgeRequired(thisShortMessage)) {
            MAL_SMS_SMSC_Address_TPDU_t MAL_SMS_SMSC_Address_TPDU;

            SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_MAX_PACKED_ADDRESS_LENGTH == MAL_SMS_MAX_PACKED_ADDRESS_LENGTH);
            SMS_COMPILE_TIME_ASSERT_STATEMENT(SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH == MAL_SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);

            MAL_SMS_SMSC_Address_TPDU.ServiceCenterAddress.Length = SMSC_Address_TPDU_p->ServiceCenterAddress.Length;
            MAL_SMS_SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress = SMSC_Address_TPDU_p->ServiceCenterAddress.TypeOfAddress;
            memcpy(MAL_SMS_SMSC_Address_TPDU.ServiceCenterAddress.AddressValue, SMSC_Address_TPDU_p->ServiceCenterAddress.AddressValue, MAL_SMS_MAX_PACKED_ADDRESS_LENGTH);

            MAL_SMS_SMSC_Address_TPDU.TPDU.Length = SMSC_Address_TPDU_p->TPDU.Length;
            memcpy(MAL_SMS_SMSC_Address_TPDU.TPDU.Data, SMSC_Address_TPDU_p->TPDU.Data, MAL_SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);

            ShortMessage_GetPackedServiceCentreAddress(thisShortMessage, (ShortMessagePackedAddress_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress);

            SMS_A_(SMS_LOG_I("smbearer.c:  TX DELIVER REPORT "));
            SMS_A_(ShortMessage_Print(thisShortMessage));
            SMS_A_(ShortMessage_HexPrint((ShortMessagePackedAddress_t *) & SMSC_Address_TPDU_p->ServiceCenterAddress, &SMSC_Address_TPDU_p->TPDU));

            if ((ErrorCode = mal_sms_request_send_deliver_report(SmUtil_TranslateErrorCodesToMal(failureCause), &MAL_SMS_SMSC_Address_TPDU)) != MAL_SMS_CBS_SUCCESS) {
                SMS_A_(SMS_LOG_E("smbearer.c: mal_sms_request_send_deliver_report: Error Code %d", ErrorCode));
                SMS_Error = SMS_ERR_SERVER_NOT_READY;
            } else if (failureCause == SMS_ERROR_INTERN_NO_ERROR || failureCause == RMS_RP_NO_CAUSE) {
                SMS_A_(SMS_LOG_I("smbearer.c: MSG->NW ACK OK"));
            } else {
                SMS_A_(SMS_LOG_I("smbearer.c: MSG->NW NACK Cause 0x%04x", failureCause));
            }
        }

        SMS_HEAP_FREE(&SMSC_Address_TPDU_p);
    } else {
        // alloc failed
        SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_SendReport: Memory Alloc Failed (SMSC_Address_TPDU_p)"));
        SMS_Error = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    }
#endif                          // SMS_USE_MFL_MODEM

    return SMS_Error;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_MemCapacityExWr
 *
 * @description  Set the memory Capacity Exceeded Notification flag
 *               within the EfSmss File and the value cached in Persistant Data.
 *
 * @param        MemoryCapacityEx : uint8_t.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_MemCapacityExWr(
    const uint8_t MemoryCapacityEx)
{
#ifdef SMS_USE_MFL_UICC
    SIM_ISO_ErrorCause_t ErrorCause;
    UICC_ServiceHandle_t ServiceHandle = SmUtil_UICC_ServiceHandleGet();
    RequestStatus_t RequestStatus;
    uint8_t Available = FALSE;
    uint8_t Enabled = FALSE;
    uint8_t MemoryAvailable;

    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_MemCapacityExWr: 1-True:0-False: Value %d ", MemoryCapacityEx));
    // Invert the flag
    if (MemoryCapacityEx) {
        MemoryAvailable = FALSE;
    } else {
        MemoryAvailable = TRUE;
    }

    RequestStatus = Request_SIM_Service_GetStatus(WAIT_RESPONSE, ServiceHandle, SIM_SRV_SHORT_MESSAGE, &Available, &Enabled, &ErrorCause);

    if (REQUEST_OK == RequestStatus && TRUE == Available && TRUE == Enabled) {
        // Update flag on SIM
        if (R_Req_SIM_SMR_MemCapExcStatus_Write(WAIT_RESPONSE, ServiceHandle, &MemoryAvailable, &ErrorCause) == REQUEST_OK) {
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MemCapacityExWr: R_Req_SIM_SMR_MemCapExcStatus_Write MemoryAvailable: %d", MemoryAvailable));
        } else {
            SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MemCapacityExWr: R_Req_SIM_SMR_MemCapExcStatus_Write SIM ErrorCause: %d", ErrorCause));
        }
    } else {
        // SMS not supported on SIM or requester error
        SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MemCapacityExWr: Request_SIM_Service_GetStatus SIM ErrorCause: %d Available: %d Enabled: %d", ErrorCause, Available, Enabled));
    }
#else
    // MAL modem automatically updates the EFsmss file so there is no need to do it ourselves.
#endif                          // SMS_USE_MFL_UICC

    // Update the cached EfSmss File within Persistent Data
    (void) SmUtil_Put_MSG_MemoryCapacityExceededFlag(MemoryCapacityEx);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_DeliverReportConfirm
 *
 * @description  Function to handle the event of SHORTMESSAGEBEARER_DELIVERREPORT_CONFIRM
 *
 * @param        *object :   void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_DeliverReportConfirm(
    void *object,
    EventData_p eventData)
{
#ifndef SMS_USE_MFL_MODEM
    // Response to Deliver Report event is handled by ShortMessageBearer_MAL_Callback()
    SMS_IDENTIFIER_NOT_USED(object);
#else
    // Initialise the status error code to a MTS generic error
    // to indicate an unhandled exception, in order to enable
    // the MSG layer to fail gracefully
    RMS_V2_Status_t Status = { FALSE, SMS_ERROR_INTERN_UNSPECIFIED_ERROR };

    SMS_IDENTIFIER_NOT_USED(object);

    if (R_Resp_RMS_V3_Message_SendAcknowledgement(eventData->DataPointer, &Status) != REQUEST_OK) {
        // Failure in the response handler
        SMS_A_(SMS_LOG_W("smbearer.c: ShortMessageBearer_DeliverReportConfirm: Response_RMS_V3_Message_SendAcknowledgement Failed"));
    } else {
        if (Status.SuccessfulOutcome) {
            SMS_A_(SMS_LOG_I
                   ("smbearer.c: ShortMessageBearer_DeliverReportConfirm: Response_RMS_V3_Message_SendAcknowledgement Succeeded. Successful Outcome :%d, Error code :%d",
                    Status.SuccessfulOutcome, Status.ErrorCode));
        } else {
            SMS_A_(SMS_LOG_W
                   ("smbearer.c: ShortMessageBearer_DeliverReportConfirm: Response_RMS_V3_Message_SendAcknowledgement did NOT succeed. Successful Outcome :%d, Error code :%d",
                    Status.SuccessfulOutcome, Status.ErrorCode));
        }
    }

    eventData->RMS_V2_ErrorCode = Status.ErrorCode;     // Indicate result of send of deliver report
#endif                          // SMS_USE_MFL_MODEM

    // Regardless of what happens here, we need to route some sort of response back to
    // Short Message Router to allow it to unlock
    ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_DELIVERREPORT_CONFIRM, eventData);
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_TPDUDeliver
 *
 * @description  Delivers a TPDU into the MTS via the Network Signalling
 *               dispatch mechanism.
 *
 * @param        SMS_TPDU_p : The SMS TPDU data to be delivered to the MTS.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
static void ShortMessageBearer_TPDUDeliver(
    void *object,
    EventData_p eventData)
{
    SMS_ShortMessageDeliver_Req_t *data_p = (SMS_ShortMessageDeliver_Req_t *) eventData->DataPointer;
    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageBearer_TPDUDeliver"));

    ShortMessageBearer_ProcessShortMessage((void *) theShortMessageBearer_p, eventData, &(data_p->SMSC_Address_TPDU), TRUE);
}

#ifndef SMS_USE_MFL_MODEM
/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_Translate_MAL_ErrorCode
 *
 * @description  Function to convert MAL error code to exported error code
 *
 * @param in ErrorCode: Error code from MAL SMS
 *
 * @return       static SMS_Error_t
 */
/********************************************************************/
static SMS_Error_t ShortMessageBearer_Translate_MAL_ErrorCode(
    const MAL_SMS_CBS_Error_t ErrorCode)
{
    SMS_Error_t ResultErrorCode = (SMS_Error_t) ErrorCode;

    // This is required because although the MAL returns SMS_Error_t SMS_ERR_OK
    // is used to indicate a successful transaction. SMS Server will only return
    // 1 success error code to clients and it shall be SMS_ERROR_INTERN_NO_ERROR
    if (ErrorCode == MAL_SMS_OK) {
        ResultErrorCode = SMS_ERROR_INTERN_NO_ERROR;
    }

    return ResultErrorCode;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_Translate_SMS_RouteInfo
 *
 * @description  Function to convert SMS route info value to MAL
 *               route info value.
 *
 * @param in sms_route_info: SMS Route value
 *
 * @return       mal_sms_route_info_t
 */
/********************************************************************/
static mal_sms_route_info_t ShortMessageBearer_Translate_SMS_RouteInfo(
    const SMS_MO_Route_t sms_route_info)
{
    mal_sms_route_info_t mal_sms_route_info = 0;

    switch (sms_route_info) {
    case SMS_MO_ROUTE_PS:
        mal_sms_route_info = 2;
        break;
    case SMS_MO_ROUTE_CS:
        mal_sms_route_info = 1;
        break;
    default:
        mal_sms_route_info = 0; /* default (configured on modem side) */
        break;
    }

    SMS_A_(SMS_LOG_I("ShortMessageBearer_Translate_SMS_RouteInfo: sms_route_info=%d translated to mal_sms_route_info=%d", sms_route_info, mal_sms_route_info));

    return mal_sms_route_info;
}


extern SMS_MO_RouteData_t SMS_MO_Route_ClientData;

/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_HandleMalCallbackData
 *
 * @description  Unpacks data and carries out necessary actions based
 *               on the type of data.  The data being unpacked was previously
 *               packed in ShortMessageBearer_MAL_Callback().
 *
 * @param in     MalCallbackData_p: Call back data which has been previous
 *               packed and stored by ShortMessageBearer_MAL_Callback()
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageBearer_HandleMalCallbackData(
    SMS_MAL_Callback_Data_t * MalCallbackData_p)
{
    switch (MalCallbackData_p->CallbackDataType) {
    case SMS_MAL_CALLBACK_DATA_TYPE_RECEIVED_MSG_IND:
        {
            SMS_MAL_Callback_Received_Msg_Ind_Data_t *ReceivedMsgIndData_p = &MalCallbackData_p->Data.SMS_MAL_Callback_Received_Msg_Ind_Data;

            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_HandleMalCallbackData: SMS_MAL_CALLBACK_RECEIVED_MSG_IND"));
            SMS_A_(ShortMessage_HexPrint((ShortMessagePackedAddress_t *) &ReceivedMsgIndData_p->SMSC_Address_TPDU.ServiceCenterAddress, &ReceivedMsgIndData_p->SMSC_Address_TPDU.TPDU));
            if (SHORTMESSAGEBEARER_IDLE == theShortMessageBearer_p->ShortMessageBearerState) {
                ShortMessageBearer_ProcessShortMessage((void *) theShortMessageBearer_p, &ShortMessageBearer_EventData, &ReceivedMsgIndData_p->SMSC_Address_TPDU, TRUE);
            } else {
                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_HandleMalCallbackData: Wrong ShortMessageBearerState: %d", thisShortMessageBearer.ShortMessageBearerState));
            }
        }
        break;

    case SMS_MAL_CALLBACK_DATA_TYPE_MESSAGE_SEND_RESP:
        {
            SMS_MAL_Callback_Message_Send_Resp_Data_t *MessageSendData_p = &MalCallbackData_p->Data.SMS_MAL_Callback_Message_Send_Resp_Data;

            SMS_A_(SMS_LOG_I
                   ("smbearer.c: ShortMessageBearer_HandleMalCallbackData: SMS_MAL_CALLBACK_MESSAGE_SEND_RESP Error:0x%04x, Sending:%d", MessageSendData_p->Error,
                    theShortMessageBearer_p->Sending));

            if (theShortMessageBearer_p->Sending) {
                // Update the Short Message object with the TP-MR returned from the MAL
                ShortMessage_SetMessageReference(thisShortMessageBearer.SendingShortMessage, MessageSendData_p->Message_Ref);
                ShortMessageBearer_EventData.RMS_V2_ErrorCode = MessageSendData_p->Error;
                ShortMessageBearer_ProcessNetworkAck((void *) theShortMessageBearer_p, &ShortMessageBearer_EventData, NULL);
            }
        }
        break;

    case SMS_MAL_CALLBACK_DATA_TYPE_RECEIVE_MESSAGE_RESP:
        {
            SMS_MAL_Callback_Receive_Message_Resp_Data_t *ReceivedMsgRespData_p = &MalCallbackData_p->Data.SMS_MAL_Callback_Receive_Message_Resp_Data;
            uint8_t SMS_MAL_ConfigState = MessagingSystem_Get_MAL_ConfigurationState();

            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_HandleMalCallbackData: SMS_MAL_CALLBACK_RECEIVE_MESSAGE_RESP, RSMS_Error:0x%04x", ReceivedMsgRespData_p->Error));

            if (SMS_MAL_ConfigState) {
                if (SMS_ERR_SERVER_NOT_READY == ReceivedMsgRespData_p->Error) {
                    SMS_A_(SMS_LOG_W("smbearer.c: ShortMessageBearer_HandleMalCallbackData: MAL not ready, will retry"));
                    Do_SMS_TimerSet_1_MS_WCT(MSG_MAL_CONFIGURE_RETRY_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_MAL_CONFIGURE_RETRY_TIMEOUT * 1000);
                } else {
                    MessagingSystem_Set_MAL_ConfigurationState(FALSE);
                }
            }
            // Callback for mal_sms_request_set_memory_status(memory available).
            if (theShortMessageBearer_p->Sending) {
                // Update the Short Message object with the TP-MR returned from the MAL
                ShortMessageBearer_EventData.RMS_V2_ErrorCode = ReceivedMsgRespData_p->Error;
                ShortMessageBearer_ProcessNetworkAck((void *) theShortMessageBearer_p, &ShortMessageBearer_EventData, NULL);
            }
        }
        break;

    case SMS_MAL_CALLBACK_DATA_TYPE_MEMORY_CAPACITY_EXC_IND:
        {
            SMS_MAL_Callback_Memory_Capacity_Exc_Ind_Data_t *MemoryCapacityExcIndData_p = &MalCallbackData_p->Data.SMS_MAL_Callback_Memory_Capacity_Exc_Ind_Data;

            // Callback received to indicate that a deliver report has been sent with RP Error cause
            // MAL_SMS_EXT_ERR_MEMORY_CAPACITY_EXC.
            // There is no payload data and there will be another callback (MAL_SMS_RECEIVED_MSG_REPORT_RESP)
            // which will indicate the result of the send deliver report.

            // There is no data payload for this callback; i.e. (data == NULL).
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_HandleMalCallbackData: SMS_MAL_CALLBACK_MEMORY_CAPACITY_EXC_IND."));
            SMS_IDENTIFIER_NOT_USED(MemoryCapacityExcIndData_p);
            SMS_IDENTIFIER_NOT_USED(MemoryCapacityExcIndData_p->Unused);

            // Update persistent data copy of memory capacity exceeded flag as we have received an indication that it has been
            // set on the SIM card.
            ShortMessageBearer_MemCapacityExWr(TRUE);
        }
        break;

    case SMS_MAL_CALLBACK_DATA_TYPE_RECEIVED_MSG_REPORT_RESP:
        {
            SMS_MAL_Callback_Received_Msg_Report_Resp_Data_t *ReceivedMsgReportRespData_p = &MalCallbackData_p->Data.SMS_MAL_Callback_Received_Msg_Report_Resp_Data;

            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_HandleMalCallbackData: SMS_MAL_CALLBACK_RECEIVED_MSG_REPORT_RESP, RSMS_Error:0x%04x", ReceivedMsgReportRespData_p->Error));

            // Indicate result of send of deliver report
            ShortMessageBearer_EventData.RMS_V2_ErrorCode = ReceivedMsgReportRespData_p->Error;

            // Regardless of what happens here, we need to route some sort of response back to
            // Short Message Router to allow it to unlock
            ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_DELIVERREPORT_CONFIRM, &ShortMessageBearer_EventData);
        }
        break;

    case SMS_MAL_CALLBACK_DATA_TYPE_CB_ROUTING_RESP:
        {
            SMS_MAL_Callback_CB_Routing_Resp_Data_t *CbRoutingRespData_p = &MalCallbackData_p->Data.SMS_MAL_Callback_CB_Routing_Resp_Data;

            SMS_A_(SMS_LOG_I
                   ("smbearer.c: ShortMessageBearer_HandleMalCallbackData: SMS_MAL_CALLBACK_CB_ROUTING_RESP Error: 0x%04X SubNum %d", CbRoutingRespData_p->Error,
                    CbRoutingRespData_p->SubscriptionNumber));
            if (CbRoutingRespData_p->Error == SMS_ERROR_INTERN_NO_ERROR) {
                CBS_Manager_Init_MAL_Info(CbRoutingRespData_p->SubscriptionNumber);
            } else {
                CBS_Manager_ToggleModemActivationState();
            }
        }
        break;

    case SMS_MAL_CALLBACK_DATA_TYPE_CB_ROUTING_IND:
        {
            SMS_MAL_Callback_CB_Routing_Ind_Data_t *CbRoutingIndData_p = &MalCallbackData_p->Data.SMS_MAL_Callback_CB_Routing_Ind_Data;

            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_HandleMalCallbackData: SMS_MAL_CALLBACK_CB_ROUTING_IND"));

            // Process CB routing indication
            CBS_Manager_HandleNewCbIndication((const SMS_MAL_Callback_CB_Routing_Ind_Data_t *) CbRoutingIndData_p);
        }
        break;

    default:
        SMS_A_(SMS_LOG_E("ShortMessageBearer_HandleMalCallbackData: Unexpected case %d", MalCallbackData_p->CallbackDataType));
        break;
    }
}




/********************************************************************/
/**
 *
 * @function     ShortMessageBearer_MAL_Callback
 *
 * @description  Function to handle the callback from the SMS MAL
 *
 * @param in eventID as defined in mal_sms.h
 * @param in data pointer as defined in mal_sms.h
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageBearer_MAL_Callback(
    int event_id,
    void *data)
{
    SMS_MAL_Callback_Data_t *SmsData_p;

    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_MAL_Callback: received event_id: %d, data %sNULL!", event_id, data != NULL ? "NOT " : ""));

    SmsData_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_MAL_Callback_Data_t));

    if (SmsData_p != NULL) {
        uint8_t AddToList = FALSE;

        switch (event_id) {
        case MAL_SMS_RECEIVED_MSG_IND:
        case MAL_SMS_RECEIVED_SIM_MSG_IND:
            SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MAL_Callback: MAL_SMS_RECEIVED_X_IND event_id: %d", event_id));
            if (SHORTMESSAGEBEARER_IDLE == theShortMessageBearer_p->ShortMessageBearerState) {
                if (data != NULL) {
                    SMS_MAL_Callback_Received_Msg_Ind_Data_t *ReceviedMsgData_p = &SmsData_p->Data.SMS_MAL_Callback_Received_Msg_Ind_Data;
                    const MAL_SMS_SMSC_Address_TPDU_t *MalData_p = (const MAL_SMS_SMSC_Address_TPDU_t *) data;

                    SmsData_p->CallbackDataType = SMS_MAL_CALLBACK_DATA_TYPE_RECEIVED_MSG_IND;

                    ReceviedMsgData_p->SMSC_Address_TPDU.ServiceCenterAddress.Length = SMS_MIN(MalData_p->ServiceCenterAddress.Length, SMS_MAX_PACKED_ADDRESS_LENGTH);
                    ReceviedMsgData_p->SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress = MalData_p->ServiceCenterAddress.TypeOfAddress;
                    memcpy(ReceviedMsgData_p->SMSC_Address_TPDU.ServiceCenterAddress.AddressValue, MalData_p->ServiceCenterAddress.AddressValue,
                           ReceviedMsgData_p->SMSC_Address_TPDU.ServiceCenterAddress.Length);

                    ReceviedMsgData_p->SMSC_Address_TPDU.TPDU.Length = SMS_MIN(MalData_p->TPDU.Length, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);
                    memcpy(ReceviedMsgData_p->SMSC_Address_TPDU.TPDU.Data, MalData_p->TPDU.Data, ReceviedMsgData_p->SMSC_Address_TPDU.TPDU.Length);
                    AddToList = TRUE;
                } else {
                    SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MAL_Callback: ERROR EventId: %d, data NULL", event_id));
                }
            } else {
                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MAL_Callback: Wrong ShortMessageBearerState: %d for event: %d", thisShortMessageBearer.ShortMessageBearerState, event_id));
            }
            break;

        case MAL_SMS_MESSAGE_SEND_RESP:
            if (data != NULL) {
                SMS_MAL_Callback_Message_Send_Resp_Data_t *MessageSendData_p = &SmsData_p->Data.SMS_MAL_Callback_Message_Send_Resp_Data;
                const sms_message_send_resp_t *MalData_p = (const sms_message_send_resp_t *) data;

                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MAL_Callback: MAL_SMS_MESSAGE_SEND_RESP event_id: %d, err_type:0x%04x", event_id, *MalData_p->err_type));

                SmsData_p->CallbackDataType = SMS_MAL_CALLBACK_DATA_TYPE_MESSAGE_SEND_RESP;

                MessageSendData_p->Message_Ref = (SMS_TP_MessageReference_t) * MalData_p->msg_ref;
                MessageSendData_p->Error = ShortMessageBearer_Translate_MAL_ErrorCode(*MalData_p->err_type);
                AddToList = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MAL_Callback: ERROR EventId: %d, data NULL", event_id));
            }
            break;

        case MAL_SMS_RECEIVE_MESSAGE_RESP:
            if (data != NULL) {
                SMS_MAL_Callback_Receive_Message_Resp_Data_t *ReceiveMsgData_p = &SmsData_p->Data.SMS_MAL_Callback_Receive_Message_Resp_Data;
                const MAL_SMS_CBS_Error_t *MalData_p = (const MAL_SMS_CBS_Error_t *) data;

                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MAL_Callback: MAL_SMS_RECEIVE_MESSAGE_RESP event_id: %d, RSMS_Error:0x%04x", event_id, *MalData_p));

                SmsData_p->CallbackDataType = SMS_MAL_CALLBACK_DATA_TYPE_RECEIVE_MESSAGE_RESP;

                ReceiveMsgData_p->Error = ShortMessageBearer_Translate_MAL_ErrorCode(*MalData_p);
                AddToList = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MAL_Callback: ERROR EventId: %d, data NULL", event_id));
            }
            break;

        case MAL_SMS_MEMORY_CAPACITY_EXC_IND:
            {
                // Callback received to indicate that a deliver report has been sent with RP Error cause
                // MAL_SMS_EXT_ERR_MEMORY_CAPACITY_EXC.
                // There is no payload data and there will be another callback (MAL_SMS_RECEIVED_MSG_REPORT_RESP)
                // which will indicate the result of the send deliver report.
                SMS_MAL_Callback_Memory_Capacity_Exc_Ind_Data_t *MemoryCapacityExceededData_p = &SmsData_p->Data.SMS_MAL_Callback_Memory_Capacity_Exc_Ind_Data;

                // There is no data payload for this callback; i.e. (data == NULL).
                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MAL_Callback: MAL_SMS_MEMORY_CAPACITY_EXC_IND event_id: %d.", event_id));
                SMS_IDENTIFIER_NOT_USED(MemoryCapacityExceededData_p);
                SMS_IDENTIFIER_NOT_USED(MemoryCapacityExceededData_p->Unused);

                SmsData_p->CallbackDataType = SMS_MAL_CALLBACK_DATA_TYPE_MEMORY_CAPACITY_EXC_IND;
                AddToList = TRUE;
            }
            break;

        case MAL_SMS_RECEIVED_MSG_REPORT_RESP:
            if (data != NULL) {
                SMS_MAL_Callback_Received_Msg_Report_Resp_Data_t *ReceivedMsgReportData_p = &SmsData_p->Data.SMS_MAL_Callback_Received_Msg_Report_Resp_Data;
                const MAL_SMS_CBS_Error_t *MalData_p = (const MAL_SMS_CBS_Error_t *) data;

                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MAL_Callback: MAL_SMS_RECEIVED_MSG_REPORT_RESP event_id: %d, RSMS_Error:0x%04x", event_id, *MalData_p));

                SmsData_p->CallbackDataType = SMS_MAL_CALLBACK_DATA_TYPE_RECEIVED_MSG_REPORT_RESP;

                ReceivedMsgReportData_p->Error = ShortMessageBearer_Translate_MAL_ErrorCode(*MalData_p);
                AddToList = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MAL_Callback: ERROR EventId: %d, data NULL", event_id));
            }
            break;

        case MAL_CB_ROUTING_RESP:
            if (data != NULL) {
                SMS_MAL_Callback_CB_Routing_Resp_Data_t *CbRoutingData_p = &SmsData_p->Data.SMS_MAL_Callback_CB_Routing_Resp_Data;
                const cbs_routing_resp_t *MalData_p = (const cbs_routing_resp_t *) data;

                SMS_A_(SMS_LOG_I
                       ("smbearer.c: ShortMessageBearer_MAL_Callback: MAL_CB_ROUTING_RESP event_id: %d Error: 0x%04X SubNum %d", event_id, *MalData_p->Error_p,
                        MalData_p->SubscriptionNumber_p));

                SmsData_p->CallbackDataType = SMS_MAL_CALLBACK_DATA_TYPE_CB_ROUTING_RESP;

                CbRoutingData_p->Error = ShortMessageBearer_Translate_MAL_ErrorCode(*MalData_p->Error_p);
                CbRoutingData_p->SubscriptionNumber = MalData_p->SubscriptionNumber_p;  // Not a pointer despite its name!
                AddToList = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MAL_Callback: ERROR EventId: %d, data NULL", event_id));
            }
            break;

        case MAL_CB_ROUTING_IND:
        case MAL_CB_SIM_ROUTING_IND:
            if (data != NULL) {
                SMS_MAL_Callback_CB_Routing_Ind_Data_t *CbRoutingData_p = NULL;
                const cbs_routing_ind_t *MalData_p = (const cbs_routing_ind_t *) data;
                uint16_t Size = 0;
                uint8_t i;

                SMS_A_(SMS_LOG_I("smbearer.c: ShortMessageBearer_MAL_Callback: MAL_CB_%sROUTING_IND event_id: %d", event_id == MAL_CB_SIM_ROUTING_IND ? "SIM_" : "", event_id));

                Size = sizeof(SMS_MAL_Callback_CB_Routing_Ind_Data_t) + MalData_p->number_of_messages * sizeof(SMS_MAL_CALLBACK_CB_Message_Data_t);
                SMS_HEAP_FREE(&SmsData_p);
                SmsData_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_MAL_Callback_Data_t) + Size);

                if (SmsData_p != NULL) {
                    CbRoutingData_p = &SmsData_p->Data.SMS_MAL_Callback_CB_Routing_Ind_Data;
                    SmsData_p->CallbackDataType = SMS_MAL_CALLBACK_DATA_TYPE_CB_ROUTING_IND;

                    CbRoutingData_p->number_of_messages = MalData_p->number_of_messages;

                    for (i = 0; i < CbRoutingData_p->number_of_messages; i++) {
                        CbRoutingData_p->messages[i].header.SerialNumber = MalData_p->messages[i].header.serial_number;
                        CbRoutingData_p->messages[i].header.MessageIdentifier = MalData_p->messages[i].header.message_id;
                        CbRoutingData_p->messages[i].header.PageParameter = MalData_p->messages[i].header.pages;
                        CbRoutingData_p->messages[i].header.DataCodingScheme = MalData_p->messages[i].header.dcs;
                        CbRoutingData_p->messages[i].useful_data_length = MalData_p->messages[i].useful_data_length;
                        (void) memmove(CbRoutingData_p->messages[i].data, MalData_p->messages[i].data, SMS_MAL_CB_CONTENTS_MAX_LENGTH);
                    }

                    AddToList = TRUE;
                } else {
                    SMS_A_(SMS_LOG_E("ShortMessageBearer_MAL_Callback: SmsData_p is NULL"));
                }
            } else {
                SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MAL_Callback: ERROR EventId: %d, data NULL", event_id));
            }
            break;

        default:
            SMS_A_(SMS_LOG_W("smbearer.c: ShortMessageBearer_MAL_Callback: Warning Unhandled event_id: %d", event_id));
            break;
        }

        if (AddToList) {
            // SmsData_p contains the data received from the MAL callback. Add it to a list of data
            // which is to be processed by the SMS CB Server.
            MessagingSystem_MAL_Callback_AddData(SmsData_p);
        } else {
            // Free SmsData_p as we haven't used it.
            SMS_HEAP_FREE(&SmsData_p);
        }
    } else {
        // Critical error.
        SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_MAL_Callback: SmsData_p Malloc Failed for event_id %d", event_id));
    }
}
#endif                          // #ifndef SMS_USE_MFL_MODEM

#ifdef SMS_USE_MFL_MODEM
// Temp switch to enable removing this code if using a build in which EVENT_ID_RMS_V2_MESSAGERECEIVEDIND and
// EVENT_ID_RMS_V2_FAILURERECEIVEDIND are not defined.
#ifndef SMS_TEMPORARILY_REMOVE_NS_EVENT_SUBSCRIPTION
static void ShortMessageBearer_Initialise_NSEventsSubscribe_Util(
    const ClientTag_t ClientTag,
    const EventId_t EventId,
    const char *const DebugString_p)
{
    RequestControlBlock_t *ReqCtrlBlk_p;

    IDENTIFIER_NOT_USED(DebugString_p); // Silence warnings if SMS_PRINT_A_ not enabled.

    ReqCtrlBlk_p = mfl_request_control_block_alloc(MessagingSystem_GetModemSessionData(), ClientTag);

    if (ReqCtrlBlk_p != NULL) {
        if (Request_EventChannel_Subscribe(ReqCtrlBlk_p, EVENT_SUBSCRIBE, EventId) != REQUEST_PENDING) {
            SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_Initialise_NSEventsSubscribe_Util: failed to send Request_EventChannel_Subscribe %s", DebugString_p));
        }

        mfl_request_control_block_free(ReqCtrlBlk_p);
    } else {
        SMS_A_(SMS_LOG_E("smbearer.c: ShortMessageBearer_Initialise_NSEventsSubscribe_Util: cannot subscribe to %s event", DebugString_p));
    }
}
#endif

static void ShortMessageBearer_Initialise_NSEventsSubscribe(
    void)
{
// Temp switch to enable removing this code if using a build in which EVENT_ID_RMS_V2_MESSAGERECEIVEDIND and
// EVENT_ID_RMS_V2_FAILURERECEIVEDIND are not defined.
#ifndef SMS_TEMPORARILY_REMOVE_NS_EVENT_SUBSCRIPTION
    ShortMessageBearer_Initialise_NSEventsSubscribe_Util(EVENT_RMS_V2_MESSAGERECEIVEDIND,       // Set Client tag to event ID.
                                                         EVENT_ID_RMS_V2_MESSAGERECEIVEDIND, "EVENT_RMS_V2_MESSAGERECEIVEDIND");

    ShortMessageBearer_Initialise_NSEventsSubscribe_Util(EVENT_RMS_V2_FAILURERECEIVEDIND,       // Set Client tag to event ID.
                                                         EVENT_ID_RMS_V2_FAILURERECEIVEDIND, "EVENT_RMS_V2_FAILURERECEIVEDIND");
#endif
}
#endif                          // #ifdef SMS_USE_MFL_MODEM

static void ShortMessageBearer_Initialise_ModemFds(
    int *const Modem_fd_p
#ifndef SMS_USE_MFL_MODEM
    ,
    int *const Netlnk_fd_p
#endif
    )
{
#ifdef SMS_USE_MFL_MODEM
    // Using MFL Modem

    // Begin a new modem session. This will loop and delay if there is no socket available.
    // Therefore we do not expect to receive a NULL return.
    modem_session_t *Session_p;

    SMS_B_(SMS_LOG_D("smbearer.c: call mfl_session_begin()"));
    Session_p = mfl_session_begin();

    if (Session_p != NULL) {
        SMS_B_(SMS_LOG_D("smbearer.c: mfl_session_begin() success"));
        // Get file descriptor for this session
        mfl_session_get_file_descriptor(Session_p, Modem_fd_p);
        SMS_B_(SMS_LOG_D("smbearer.c: mfl_session_get_file_descriptor() complete"));
    } else {
        SMS_A_(SMS_LOG_E("smbearer.c: mfl_session_begin() returned NULL"));
    }

    // Update value of Session in smsmain.c structure.
    MessagingSystem_SetModemSessionData(Session_p);

    // Subscribe to MFL Modem NS Asynchronous Events.
    ShortMessageBearer_Initialise_NSEventsSubscribe();

#else                           // SMS_USE_MFL_MODEM

    // Using MAL Modem
    int Result;                 // SMS_req_type

    // Initialise the SMS MAL
    if (MAL_SMS_CBS_SUCCESS != (Result = mal_sms_cbs_init(Modem_fd_p, Netlnk_fd_p))) {
        SMS_A_(SMS_LOG_E("smbearer.c: mal_sms_cbs_init ErrorCause: %d ", Result));
    }
    // Register the callback functions
    else if (MAL_SMS_CBS_SUCCESS != (Result = mal_sms_cbs_register_callback(ShortMessageBearer_MAL_Callback))) {
        SMS_A_(SMS_LOG_E("smbearer.c: mal_sms_cbs_register_callback ErrorCause: %d ", Result));
    }

    SMS_B_(SMS_LOG_D("smbearer.c: ShortMessageBearer_Initialise_ModemFds(): Exit ErrorCause: %d ", Result));
#endif                          // SMS_USE_MFL_MODEM
}
