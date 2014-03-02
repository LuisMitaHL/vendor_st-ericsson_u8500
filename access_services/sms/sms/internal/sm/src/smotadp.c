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
 *  Short Message Over The Air De-Personalisation Object.
 *
 *************************************************************************/

/*
 * INCLUDE FILES
 */
#include <stdint.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"
//#include "r_timerserver.h"

/* Message Module Include Files */
#include "sm.h"
#include "smsevnthndr.h"
#include "smotadp.h"
#include "smutil.h"

#include "cops.h"

/* Configuration */
#include "c_sms_config.h"

typedef enum {
    OTADP_IDLE = 1,
} OTADP_State_t;

typedef struct OTADPSystemObject {
    OTADP_State_t OTADP_State;
    uint8_t StoredProtocolIdentifier;
    uint8_t StoredDataCodingScheme;
    uint8_t StoredAlphabet;
} OTADPSystem_t;

typedef struct {
    uint8_t DeliverLength;
    uint8_t Deliver[MAXIMUM_TPDU_SIZE];
} SMS_DataOTADP_Format_t;

typedef struct {
    uint8_t Length;
    uint8_t Data[COPS_SIMLOCK_OTA_MESSAGE_MAX_SIZE];
} SIMLock_OTADP_Message_t;

static cops_return_code_t OTADP_ConnectServer(
    cops_context_id_t ** Context_pp);
static void OTADP_CloseServer(
    cops_context_id_t ** Context_pp);

//Local Event Handler Functions
static void OTADP_HandleNewShortMessage(
    void *object,
    EventData_p eventData);

//Message Event, State & Handler Table
static EventAction_t OTADP_StateEventTable[2] = {
    {OTADP_NEW_SHORT_MESSAGE, OTADP_IDLE, OTADP_HandleNewShortMessage},

    {END_OF_TABLE, END_OF_TABLE, END_OF_TABLE_FUNCTION}
};

//This object is static
static OTADPSystem_t thisOTADPSystem = { OTADP_IDLE, 0, 0, 0 };

OTADPSystem_p theOTADPSystem = &thisOTADPSystem;


/********************************************************************/
/**
 *
 * @function     OTADP_HandleEvent
 *
 * @description  Handle OTADP event
 *
 * @param        event     : as type of OTADP_Event_t.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void OTADP_HandleEvent(
    OTADP_Event_t event,
    EventData_p eventData)
{
    EventHandlerFunction_p eventHandler;

#ifdef SMS_PRINT_B_
    char *event_p = NULL;
    char *state_p = NULL;
    switch (event) {
    case OTADP_NEW_SHORT_MESSAGE:
        event_p = "OTADP_NEW_SHORT_MESSAGE";
        break;
    default:
        event_p = "UNKNOWN EVENT";
        break;
    }
    switch (thisOTADPSystem.OTADP_State) {
    case OTADP_IDLE:
        state_p = "OTADP_IDLE";
        break;
    default:
        state_p = "UNKNOWN STATE";
        break;
    }
    SMS_LOG_D("smotadp.c: OTADP : %s in %s", event_p, state_p);
#endif

    eventHandler = EventHandler_Find(theOTADPSystem, OTADP_StateEventTable, event, theOTADPSystem->OTADP_State);

    (eventHandler) (theOTADPSystem, eventData);

}

/********************************************************************/
/**
 *
 * @function     OTADP_ConnectServer
 *
 * @description  Function creates a new communication channel with
 *               the COPS daemon
 *
 * @param        Context_pp : Pointer to a pointer to the context ID.
 *
 * @return       ReturnCode : cops_return_code_t
 * @retval       COPS_RC_OK The function completed successfully
 * @retval       COPS_RC_UNSPECIFIC_ERROR An unspecific error has occured
 * @retval       COPS_RC_INTERNAL_ERROR An internal error has occured
 */
/********************************************************************/
static cops_return_code_t OTADP_ConnectServer(
    cops_context_id_t ** Context_pp)
{
    cops_event_callbacks_t EventCallbacks;
    cops_return_code_t ReturnCode;

    SMS_A_(SMS_LOG_I("smotadp.c: OTADP_ConnectServer: Connecting to server"));

    memset(&EventCallbacks, '\0', sizeof(cops_event_callbacks_t));

    ReturnCode = cops_context_create(Context_pp, &EventCallbacks, NULL);

    if (ReturnCode == COPS_RC_OK) {
        SMS_A_(SMS_LOG_I("smotadp.c: OTADP_ConnectServer: Connected to server"));
    }

    return (ReturnCode);
}

/********************************************************************/
/**
 *
 * @function     OTADP_CloseServer
 *
 * @description  Destroys an established communication with the 
 *               COPS daemon 
 *
 * @param        Context_pp : Pointer to a pointer to the context ID.
 *
 * @return       ReturnCode : cops_return_code_t
 * @retval       COPS_RC_OK The function completed successfully
 * @retval       COPS_RC_UNSPECIFIC_ERROR An unspecific error has occured
 * @retval       COPS_RC_INTERNAL_ERROR An internal error has occured
 */
/********************************************************************/
static void OTADP_CloseServer(
    cops_context_id_t ** Context_pp)
{
    SMS_A_(SMS_LOG_I("smotadp.c: OTADP_CloseServer: Closing connection to server"));

    cops_context_destroy(Context_pp);

    SMS_A_(SMS_LOG_I("smotadp.c: OTADP_CloseServer: Connection to server closed"));
}

/********************************************************************/
/**
 *
 * @function     OTADP_HandleNewShortMessage
 *
 * @description  This function unpacks the SIM lock data from the MT
 *               Over The Air Depersonalisation message and passes the
 *               data to the COPS module.
 *
 * @param        *object   : void.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
static void OTADP_HandleNewShortMessage(
    void *object,
    EventData_p eventData)
{
    OTADPSystem_p OTADPSystem = (OTADPSystem_p) object;
    ShortMessage_p NewShortMessage = (ShortMessage_p) eventData->ShortMessage;

    SMS_DataOTADP_Format_t *SMSData_p = (SMS_DataOTADP_Format_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_DataOTADP_Format_t));

    if (SMSData_p != NULL) {
        cops_simlock_ota_reply_message_t SIMLock_OTADP_Reply;
        SIMLock_OTADP_Message_t *SIMLock_OTADP_Message_p = (SIMLock_OTADP_Message_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SIMLock_OTADP_Message_t));

        if (SIMLock_OTADP_Message_p != NULL) {
            cops_return_code_t ReturnCode;
            cops_context_id_t *Context_p;

            SMS_A_(SMS_LOG_I("smotadp.c: OTADP_HandleNewShortMessage: OTA ShortMessage is not NULL"));

            // Initialise OTA message data.
            memset(SIMLock_OTADP_Message_p->Data, 0x00, COPS_SIMLOCK_OTA_MESSAGE_MAX_SIZE);

            SMSData_p->DeliverLength = ShortMessage_GetText(NewShortMessage, SMSData_p->Deliver);

            if (SMSData_p->DeliverLength > sizeof(SIMLock_OTADP_Message_p->Data)) {
                SMSData_p->DeliverLength = sizeof(SIMLock_OTADP_Message_p->Data);
            }

            SIMLock_OTADP_Message_p->Length = SMSData_p->DeliverLength;

            memcpy(SIMLock_OTADP_Message_p->Data, SMSData_p->Deliver, SMSData_p->DeliverLength);

            SMS_B_(SmUtil_HexPrint_Raw((const uint8_t *) SIMLock_OTADP_Message_p, sizeof(SIMLock_OTADP_Message_t), "SMS: smotadp.c: SIMLock_OTADP_Message_p"));

            // Store the SM data for use in the acknowledge.
            OTADPSystem->StoredProtocolIdentifier = ShortMessage_GetProtocolIdentifier(NewShortMessage);
            OTADPSystem->StoredDataCodingScheme = ShortMessage_GetDataCodingScheme(NewShortMessage);
            OTADPSystem->StoredAlphabet = ShortMessage_GetAlphabet(NewShortMessage);

            // Connect to COPS server; hence create communication channel.
            ReturnCode = OTADP_ConnectServer(&Context_p);

            if (ReturnCode == COPS_RC_OK) {     // Unlock the ME over the air (OTA), by sending the ME a SMS with the
                // IMEI of the ME and the control keys.
                ReturnCode = cops_simlock_ota_unlock(Context_p, // COPS communication channel.
                                                     SIMLock_OTADP_Message_p->Data,     // Unlock data.
                                                     COPS_SIMLOCK_OTA_MESSAGE_MAX_SIZE, // Unlock data length.
                                                     &SIMLock_OTADP_Reply);     // Reply data used to build acknowledgement data.

                if (ReturnCode == COPS_RC_OK) {
                    ShortMessage_p DeliveryShortMessage = ShortMessage_Create(SM_DELIVER_REPORT_ACK);

                    if (DeliveryShortMessage == NULL) { // Failed to create delivery report.
                        SMS_A_(SMS_LOG_E("smotadp.c: OTADP_HandleNewShortMessage: Create delivery report FAILED"));
                    } else {
                        ShortMessage_SetAcknowledgeRequired(DeliveryShortMessage, TRUE);

                        // 3GPP TS 22.022 V5.0.0 Personalisation of Mobile Equipment (ME) states that:
                        //
                        // "The ME specific de personalisation SMS messages sent by the network to
                        //  de personalise the ME shall be coded according to TS 23.040...
                        //
                        //  All characters are coded according to the default alphabet described in TS 23.038...
                        //
                        //  The acknowledgement to the ME De personalisation Short Message shall be a
                        //  SMS DELIVER REPORT for RP ACK as described in TS  23.040 with the
                        //  TP User Data coded according to the default alphabet described in TS 23.038"
                        //
                        //  The COPS module always returns the response as 8-bit data which is converted
                        //  to the default alphabet by the MTS if the depersonalisation SM was default encoded.
                        //  In the unlikely event that the depersonalisation SM is NOT default encoded Oyster
                        //  have requested the response is 8-bit encoded. A cleaner solution may have
                        //  been to convert the 8-bit response to UCS2 if the depersonalisation SM was
                        //  UCS2 encoded but this is not what was asked for.
                        if (OTADPSystem->StoredAlphabet != SM_DEFAULT_CODING) {
                            OTADPSystem->StoredDataCodingScheme = ALPHABET_8_BIT;
                            OTADPSystem->StoredAlphabet = SM_8BIT_CODING;
                        }

                        (void) ShortMessage_PackTPDUHeader(DeliveryShortMessage,
                                                           TP_NO_CAUSE, &OTADPSystem->StoredProtocolIdentifier, &OTADPSystem->StoredDataCodingScheme, FALSE, FALSE, NULL, NULL, NULL, NULL);

                        (void) ShortMessage_PackMessageBody(DeliveryShortMessage, COPS_SIMLOCK_OTA_REPLY_MESSAGE_SIZE, OTADPSystem->StoredAlphabet, &SIMLock_OTADP_Reply.data[0]);

                        // Send delivery report.
                        (void) ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, RMS_RP_NO_CAUSE);
                    }
                } else {
                    SMS_A_(SMS_LOG_E("smotadp.c: OTADP_HandleNewShortMessage: Unlock the ME over the air (OTA) FAILED: Return Code = %d ", ReturnCode));
                    ShortMessage_Acknowledge(NewShortMessage, TP_UNSPECIFIED_ERROR_CAUSE);
                }

                // Close connection to COPS server.
                OTADP_CloseServer(&Context_p);
            } else {
                SMS_A_(SMS_LOG_E("smotadp.c: OTADP_HandleNewShortMessage: Connecting to server FAILED"));
            }

            SMS_HEAP_FREE(&SIMLock_OTADP_Message_p);
        } else {
            SMS_A_(SMS_LOG_E("smotadp.c: OTADP_HandleNewShortMessage: Error allocating received short message data "));
        }

        SMS_HEAP_FREE(&SMSData_p);
    } else {
        SMS_A_(SMS_LOG_E("smotadp.c: OTADP: Error allocating dispatch data"));
    }
}
