/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Balaji Natakala
 *  Email : balaji.natakala@stericsson.com
 *****************************************************************************/


#define SMSCLIENT_SMSHANDLING_C

#include "smsClientSmsHandling.h"


static bool v_RegisteredSmsPort = FALSE;      /* application port registered for MT SMS triggers */
static t_smsClient_MessageList v_MessageList;
static uint16_t v_SmsPort      = K_SMSCLIENT_SUPL_SMS_PORT_ID;
static uint16_t v_MessageTimer = K_SMSCLIENT_MESSAGE_TIMER_VALUE;


/*
 **************************************************************************
 * Function: smsClient2_01RegisterApplicationPort
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: bool
 *
 * Description: Add application port subscription to receive SUPL MT SMS
 *              messages
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
bool smsClient2_01RegisterApplicationPort(void)
{
    SMS_RequestStatus_t        vl_ReqStatus;

    SMS_ApplicationPortRange_t vl_ApplicationPortRange;
    SMS_Error_t vl_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;

    bool vl_PassedOk = FALSE;
    SMS_RequestControlBlock_t vl_ReqCtrlBlk;

    // Use WAIT_MODE to subscribe to application ports.
    if (!smsClient1_1ServerReqCtrlBlkWaitModeInit(&vl_ReqCtrlBlk))
        return FALSE;

    vl_ApplicationPortRange.ApplicationPort = SMS_APPLICATION_PORT_DESTINATION;
    vl_ApplicationPortRange.LowerPort = v_SmsPort;
    vl_ApplicationPortRange.UpperPort = v_SmsPort;

    //add application port subscription
    vl_ReqStatus = Request_SMS_ApplicationPortSubscribe(&vl_ReqCtrlBlk, &vl_ApplicationPortRange, &vl_ErrorCode);

    if (vl_ReqStatus == SMS_REQUEST_OK) {
        if (vl_ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            INF("Request_SMS_ApplicationPortSubscribe, SUCCESS!\n");

            v_RegisteredSmsPort = TRUE;

            // Ensure that message list has been initialised before anything uses it.
            smsClient2_04InitialiseMessageList();
            vl_PassedOk = TRUE;
        } else {
            ERR("Request_SMS_ApplicationPortSubscribe, vl_Error Code = %d !\n", vl_ErrorCode);
        }
    } else {
        ERR("Request_SMS_ApplicationPortSubscribe, Request Status = %d, vl_Error Code = %d !\n", vl_ReqStatus, vl_ErrorCode);
    }

    return (vl_PassedOk);
}



/*
 **************************************************************************
 * Function: smsClient2_02DeregisterApplicationPort
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: bool
 *
 * Description: Remove application port subscription to receive SUPL MT SMS
 *              messages
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
bool smsClient2_02DeregisterApplicationPort(void)
{
    bool vl_PassedOk = FALSE;

    if (v_RegisteredSmsPort) {
        SMS_RequestStatus_t vl_ReqStatus;

        SMS_ApplicationPortRange_t vl_ApplicationPortRange;
        SMS_Error_t                vl_ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
        SMS_RequestControlBlock_t  vl_ReqCtrlBlk;

        // Use WAIT_MODE to unsubscribe from application ports.
        if (!smsClient1_1ServerReqCtrlBlkWaitModeInit(&vl_ReqCtrlBlk))
            return FALSE;

        vl_ApplicationPortRange.ApplicationPort = SMS_APPLICATION_PORT_DESTINATION;
        vl_ApplicationPortRange.LowerPort = v_SmsPort;
        vl_ApplicationPortRange.UpperPort = v_SmsPort;

        //remove application port subscription
        vl_ReqStatus = Request_SMS_ApplicationPortUnsubscribe(&vl_ReqCtrlBlk, &vl_ApplicationPortRange, &vl_ErrorCode);

        if (vl_ReqStatus == SMS_REQUEST_OK) {
            if (vl_ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
                // TODO should v_RegisteredSmsPort be set to FALSE here?
                vl_PassedOk = TRUE;
            } else {
                ERR("Request_SMS_ApplicationPortUnsubscribe, vl_Error Code = %d !\n", vl_ErrorCode);
            }
        } else {
            ERR("Request_SMS_ApplicationPortUnsubscribe, Request Status = %d, vl_Error Code = %d !\n", vl_ReqStatus, vl_ErrorCode);
        }
    }

    return (vl_PassedOk);
}



/*
 **************************************************************************
 * Function: smsClient2_03SmsPortRegistered
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: void
 *
 * Description: SMS Port has been registered
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
void smsClient2_03SmsPortRegistered(void)
{
    // TODO Does this function need to be global / is it even needed?
    v_RegisteredSmsPort = TRUE;
}


/*
 **************************************************************************
 * Function: smsClient2_04InitialiseMessageList
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Initialise message list
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
void smsClient2_04InitialiseMessageList(void)
{
    v_MessageList.p_Head = NULL;
    v_MessageList.v_MessageCount = 0;
    v_MessageList.v_MessageTimerRunning = FALSE;
}



/*
 **************************************************************************
 * Function: smsClient2_05SetSmsPort
 *
 * Parameters:
 *   [In] uint16_t
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Interactive Debug function for setting SUPL SMS port
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
void smsClient2_05SetSmsPort(const uint16_t vp_SmsPort)
{
    v_SmsPort = vp_SmsPort;
}



/*
 **************************************************************************
 * Function: smsClient2_06SetMessageTimer
 *
 * Parameters:
 *   [In] uint16_t
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Function for setting message timer value
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
void smsClient2_06SetMessageTimer(const uint16_t vp_NewTimerVal)
{
    v_MessageTimer = vp_NewTimerVal;
}



/*
 **************************************************************************
 * AMIL SMS Scope External Functions
 **************************************************************************
*/

/*
 **************************************************************************
 * Function: smsClient2_07HandleSmsEventApplicationPortMatched
 *
 * Parameters:
 *   [In] void*  Event signal data
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Handle incoming signal EVENT_MSG_SMS_APPLICATIONPORTMATCHED
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
void smsClient2_07HandleSmsEventApplicationPortMatched(const void *const RecPrim_p)
{
    SMS_ShortMessageReceived_t *pl_MessageData;

    // We have received a port-matched SMS. Send a deliver report to the network acknowledging it as we are going to process it.
    smsClient2_28SendDeliverReport();

    pl_MessageData = (SMS_ShortMessageReceived_t *) OSA_Malloc(sizeof(SMS_ShortMessageReceived_t));

    if (pl_MessageData != NULL) {
        SMS_EventStatus_t vl_EventStatus;

        SMS_ApplicationPortRange_t vl_ApplicationPortRange;

        vl_EventStatus = Event_SMS_ApplicationPortMatched(RecPrim_p, &vl_ApplicationPortRange, pl_MessageData);

        if (vl_EventStatus == SMS_EVENT_OK) {
            if (smsClient2_09ApplicationPortMatch(&vl_ApplicationPortRange)) {
                SMS_ConcatMessageRefNumber_t v_MessageReference = 0;

                INF("SUPL MT SMS, Category = %d, Replacement = %s, Storage = %d, Position = %d TPDU Length = %d !\n",
                    pl_MessageData->Category, (pl_MessageData->IsReplaceType ? "TRUE" : "FALSE"), pl_MessageData->Slot.Storage, pl_MessageData->Slot.Position, pl_MessageData->SMSC_Address_TPDU.TPDU.Length);

                //if not already received, store user data from SMS TPDU in message list
                if (smsClient2_11StoreMessage(pl_MessageData, &v_MessageReference)) {
                    //is message now a complete message
                    if (smsClient2_13MessageComplete(v_MessageReference)) {
                        uint8_t *pl_PushBody;

                        INF("SUPL Push Body Length = %d !\n", smsClient2_12MessageDataLength(v_MessageReference));

                        pl_PushBody = (uint8_t *) OSA_Malloc(K_SMSCLIENT_SUPL_PUSH_BODY_MAX_LENGTH);

                        if (pl_PushBody != NULL) {
                            //pack SUPL message from user data in each SMS TPDU
                            if (smsClient2_15ExtractMessage(v_MessageReference, pl_PushBody)) {
                                uint32_t vl_Index  = 0;
                                uint16_t vl_pduLen = 0;
                                uint8_t *pl_SuplData = NULL;

                                memcpy(&vl_pduLen, &(pl_PushBody[0]), sizeof(vl_pduLen));
                                pl_SuplData = OSA_Malloc(vl_pduLen+1);
                                memcpy(pl_SuplData, &(pl_PushBody[2]),  vl_pduLen);
                                pl_SuplData[vl_pduLen] = '\0'; /*Setting last byte as NULL*/

                                INF("SUPL Data Length = %d", vl_pduLen);
                                for ( vl_Index = 0 ;vl_Index < vl_pduLen ; vl_Index++ )
                                {
                                   INF( "SMS Data Parameters : String = %X", pl_SuplData[vl_Index]);
                                }

#ifdef AGPS_INJECT_MOBILE_INFO_TEST
                                {
                                    s_gnsSUPL_MobileInfo vl_MobileInfo;
                                    memset (&vl_MobileInfo, 0, sizeof(vl_MobileInfo));
                                    uint8_t pl_Imsi[] = "001010123456789";

                                    vl_MobileInfo.v_CellInfoType = E_gnsSUPL_GSM_CELL;
                                    memcpy(vl_MobileInfo.a_IMSI, pl_Imsi, K_gnsSUPL_MAX_IMSI_LENGTH);
                                    vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MCC = 1;
                                    vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_MNC = 1;
                                    vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_CellIdentity = 1;
                                    vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_LAC = 1;
                                    vl_MobileInfo.v_CellInfo.v_GsmCellInfo.v_NumNeighbouringCells = 0;
                                    vl_MobileInfo.v_CellInfoStatus = E_gnsSUPL_CELL_INFO_CURRENT;
                                    vl_MobileInfo.v_RegistrationState = E_gnsSUPL_REGISTRATION_SERVICE_ALLOWED;

                                    GNS_SuplMobileInfoInd(&vl_MobileInfo);
                                }
#endif

                                // Send Push data to SUPL STELP API.
                                GNS_SuplSmsPushInd(pl_SuplData, vl_pduLen, NULL, 0);
                            }

                            OSA_Free(pl_PushBody);
                        } else {
                            ERR("Heap allocation error (%d bytes) !\n", K_SMSCLIENT_SUPL_PUSH_BODY_MAX_LENGTH);
                        }
                    }
                    //do incomplete messages still exist
                    if (smsClient2_16AnyMessagesIncomplete()) {
                        smsClient2_17StartMessageTimer();
                    } else if (smsClient2_10MessageCount() == 0) {
                        smsClient2_14DestroyMessageList();
                    }
                }
            } else {
                ERR("Event_MSG_SMS_ApplicationPortMatched, Invalid application port range, lower = %d, upper = %d !\n", vl_ApplicationPortRange.LowerPort, vl_ApplicationPortRange.UpperPort);
            }
        } else {
            ERR("Event_MSG_SMS_ApplicationPortMatched, Event Status = %d !\n", vl_EventStatus);
        }

        OSA_Free(pl_MessageData);
    } else {
        ERR("Heap allocation error (SMS_ShortMessageReceived_t) !\n");
    }
}


/*
 **************************************************************************
 * Function: smsClient2_08TimerExpired
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Called when the timer has expired. Any incomplete concatenated
 *              SMS messages which have been partially received are destroyed
 *              as it is assumed that the missing parts will not now arrive.
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
void smsClient2_08TimerExpired(void)
{
    INF("\n");

    // Timer has expired so destroy the sms message list.
    smsClient2_14DestroyMessageList();
}



/*
 **************************************************************************
 * Internal Functions
 **************************************************************************
*/

/*
 **************************************************************************
 * Function: smsClient2_09ApplicationPortMatch
 *
 * Parameters:
 *   [In] SMS_ApplicationPortRange_t*
 *   [Out] none
 *
 * Return value: bool
 *
 * Description: Compare application ports for match
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
static bool smsClient2_09ApplicationPortMatch(const SMS_ApplicationPortRange_t * const pp_ApplicationPortRange)
{
    bool vl_PassedOk = FALSE;

    if ((pp_ApplicationPortRange->LowerPort == v_SmsPort) && (pp_ApplicationPortRange->UpperPort == v_SmsPort)) {
        vl_PassedOk = TRUE;
    }

    return (vl_PassedOk);
}

/*
 **************************************************************************
 * Function: smsClient2_10MessageCount
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: uint8_t
 *
 * Description: Return the number of message elements listed
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
static uint8_t smsClient2_10MessageCount(void)
{
    return (v_MessageList.v_MessageCount);
}



/*
 **************************************************************************
 * Function: smsClient2_11StoreMessage
 *
 * Parameters:
 *   [In] SMS_ShortMessageReceived_t*
 *   [Out] SMS_ConcatMessageRefNumber_t*
 *
 * Return value: bool
 *
 * Description: Read and store TPDU data into message list
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
static bool smsClient2_11StoreMessage(const SMS_ShortMessageReceived_t * const pp_MessageData, SMS_ConcatMessageRefNumber_t * const pp_MessageReference)
{
    SMS_ConcatMessageRefNumber_t vl_RetMessageReference = 0;
    bool vl_PassedOk = FALSE;
    t_smsClient_MessageElement *pl_MessageElement;
    uint16_t vl_ConcatMessRefNum = 1;
    uint8_t  vl_MaxNumConcatMsgs = 1;
    uint8_t  vl_MsgSeqNum = 1;

    (void) smsClient2_27TPDUConcatInfoRead(pp_MessageData->SMSC_Address_TPDU.TPDU.Length,
                                        &(pp_MessageData->SMSC_Address_TPDU.TPDU.Data[0]),
                                        &vl_ConcatMessRefNum,
                                        &vl_MaxNumConcatMsgs,
                                        &vl_MsgSeqNum);

    INF("SUPL MT SMS, message %d; part %d of %d !\n", vl_ConcatMessRefNum, vl_MsgSeqNum, vl_MaxNumConcatMsgs);

    //add message element if not already existing
    pl_MessageElement = smsClient2_23MessageListMessageElementAdd(vl_ConcatMessRefNum, vl_MaxNumConcatMsgs);

    if (pl_MessageElement != NULL) {
        //add TPDU if not already existing
        if (smsClient2_18MessageListTPDUAdd(pl_MessageElement,
                                          vl_MsgSeqNum,
                                          pp_MessageData->IsReplaceType,
                                          &(pp_MessageData->Slot), pp_MessageData->SMSC_Address_TPDU.TPDU.Length, &(pp_MessageData->SMSC_Address_TPDU.TPDU.Data[0]))) {
            vl_RetMessageReference = vl_ConcatMessRefNum;

            vl_PassedOk = TRUE;
        }
    } else {
        ERR("SUPL MT SMS, unable to add message %d to list !\n", vl_ConcatMessRefNum);
    }

    *pp_MessageReference = vl_RetMessageReference;

    return (vl_PassedOk);
}

/*
 **************************************************************************
 * Function: smsClient2_12MessageDataLength
 *
 * Parameters:
 *   [In] SMS_ConcatMessageRefNumber_t
 *   [Out] none
 *
 * Return value: uint16_t
 *
 * Description: Return the total user data length of one message element
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
static uint16_t smsClient2_12MessageDataLength(const SMS_ConcatMessageRefNumber_t vp_MessageReference)
{
    t_smsClient_MessageElement *pl_MessageElement;

    uint16_t RetMessageLength = 0;

    pl_MessageElement = smsClient2_25MessageListMessageElementFind(vp_MessageReference, NULL);

    if (pl_MessageElement != NULL) {
        RetMessageLength = pl_MessageElement->v_TotalTpduLength;
    }

    return (RetMessageLength);
}



/*
 **************************************************************************
 * Function: smsClient2_13MessageComplete
 *
 * Parameters:
 *   [In] SMS_ConcatMessageRefNumber_t
 *   [Out] none
 *
 * Return value: bool
 *
 * Description: Check to see message elements make a complete message
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
static bool smsClient2_13MessageComplete(const SMS_ConcatMessageRefNumber_t vp_MessageReference)
{
    bool vl_MessageComplete = FALSE;

    if (v_MessageList.v_MessageCount > 0) {
        t_smsClient_MessageElement *pl_MessageElement = smsClient2_25MessageListMessageElementFind(vp_MessageReference,
                                                                                             NULL);

        if (pl_MessageElement != NULL) {
            if (pl_MessageElement->v_TpduCount == pl_MessageElement->v_MaxTpdu) {
                vl_MessageComplete = TRUE;
            }
        }
    }

    return (vl_MessageComplete);
}



/*
 **************************************************************************
 * Function: smsClient2_14Destroyv_MessageList
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Free all message elements & associated TPDU elements and
 *              free list
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
static void smsClient2_14DestroyMessageList(void)
{
    if (v_MessageList.p_Head != NULL) {
        t_smsClient_MessageElement *pl_CurMessageElement = v_MessageList.p_Head;
        t_smsClient_MessageElement *pl_NextMessageElement = NULL;

        //check for & free each message element
        while (pl_CurMessageElement != NULL) {
            pl_NextMessageElement = pl_CurMessageElement->p_Next;

            smsClient2_20MessageListTPDUDeleteAll(pl_CurMessageElement->p_Head);

            //free message element
            OSA_Free(pl_CurMessageElement);

            pl_CurMessageElement = pl_NextMessageElement;
        }

        if (v_MessageList.v_MessageTimerRunning) {
            smsClient1_3SelectTimeoutSet(0);
        }

        smsClient2_04InitialiseMessageList();
    }
}

/*
 **************************************************************************
 * Function: smsClient2_15ExtractMessage
 *
 * Parameters:
 *   [In] SMS_ConcatMessageRefNumber_t
 *   [Out] uint8_t*
 *
 * Return value: bool
 *
 * Description: Extract the message from TPDU user data and once complete
 *              remove message element from list
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15
static bool smsClient2_15ExtractMessage(const SMS_ConcatMessageRefNumber_t vp_MessageReference, uint8_t * const CompleteUserData_p)
{
    t_smsClient_MessageElement *pl_MessageElement;

    bool vl_PassedOk = FALSE;

    pl_MessageElement = smsClient2_25MessageListMessageElementFind(vp_MessageReference, NULL);

    if (pl_MessageElement != NULL) {
        t_smsClient_TpduElement *pl_TPDU_Element;

        uint8_t vl_SequenceNo;

        uint8_t *pl_PartUserData = CompleteUserData_p;

        //3GPP TS 23.040; sequence number starts at 1
        for (vl_SequenceNo = 1; vl_SequenceNo <= pl_MessageElement->v_TpduCount; vl_SequenceNo++) {
            pl_TPDU_Element = smsClient2_21MessageListTPDUFind(pl_MessageElement, vl_SequenceNo, NULL);

            memcpy(pl_PartUserData, &pl_TPDU_Element->v_UserDataLength, sizeof(pl_TPDU_Element->v_UserDataLength));

            memcpy(pl_PartUserData+sizeof(pl_TPDU_Element->v_UserDataLength), 
                   pl_TPDU_Element->p_UserData, 
                   pl_TPDU_Element->v_UserDataLength);
        }

        smsClient2_24MessageListMessageElementDelete(vp_MessageReference);

        vl_PassedOk = TRUE;
    }

    return (vl_PassedOk);
}

/*
 **************************************************************************
 * Function: smsClient2_16AnyMessagesIncomplete
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: bool
 *
 * Description: Check if any list of message elements make an incomplete
 *              message
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 16
static bool smsClient2_16AnyMessagesIncomplete(void)
{
    bool vl_MessageInComplete = FALSE;

    if (v_MessageList.v_MessageCount > 0) {
        t_smsClient_MessageElement *pl_MessageElement = v_MessageList.p_Head;

        while ((pl_MessageElement != NULL) && !vl_MessageInComplete) {
            if (pl_MessageElement->v_TpduCount != pl_MessageElement->v_MaxTpdu) {
                vl_MessageInComplete = TRUE;
            } else {
                pl_MessageElement = pl_MessageElement->p_Next;
            }
        }
    }

    return (vl_MessageInComplete);
}

/*
 **************************************************************************
 * Function: smsClient2_17StartMessageTimer
 *
 * Parameters:
 *   [In] none
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Start message timer
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 17
static void smsClient2_17StartMessageTimer(void)
{
    //restart timer if running
    if (v_MessageList.v_MessageTimerRunning) {
        smsClient1_3SelectTimeoutSet(0);
    }

    smsClient1_3SelectTimeoutSet(v_MessageTimer);

    v_MessageList.v_MessageTimerRunning = TRUE;
}

/*
 **************************************************************************
 * Function: smsClient2_18MessageListTPDUAdd
 *
 * Parameters:
 *   [In] t_smsClient_MessageElement*
 *        uint8_t
 *        bool
 *        uint16_t
 *        uint8_t*
 *   [Out] none
 *
 * Return value: bool
 *
 * Description: Check if a TPDU element with matching sequence number
 *              exists, if so return that element.  Otherwise create a new
 *              element and add it to the head of the list
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 17
static bool smsClient2_18MessageListTPDUAdd(t_smsClient_MessageElement * const pp_MessageElement,
                                             const uint8_t vp_SequenceNo, const bool vp_IsReplace, const SMS_Slot_t * const pp_SMSSlot, const uint16_t vp_TPDUDataLength, const uint8_t * const pp_TPDUData)
{
    t_smsClient_TpduElement *pl_TPDU_Element;

    bool vl_PassedOk = FALSE;

    //check for matching TPDU before creating new one
    pl_TPDU_Element = smsClient2_21MessageListTPDUFind(pp_MessageElement, vp_SequenceNo, NULL);

    //if this is a replacement part and the original part has been found
    if ((pl_TPDU_Element != NULL) && vp_IsReplace) {
        //delete the original
        smsClient2_19MessageListTPDUDelete(pp_MessageElement, vp_SequenceNo);

        //reset pl_TPDU_Element so that we fall into the following "create new" condition
        pl_TPDU_Element = NULL;
    }
    //no match found, create new element
    if (pl_TPDU_Element == NULL) {
        pl_TPDU_Element = (t_smsClient_TpduElement *) OSA_Malloc(sizeof(t_smsClient_TpduElement));

        //initialise new TPDU element
        if (pl_TPDU_Element != NULL) {
            uint16_t vl_UserDataLength = 0;
            uint8_t *pl_UserData = NULL;

            //link to head of TPDU list
            pl_TPDU_Element->p_Next = pp_MessageElement->p_Head;
            pp_MessageElement->p_Head = pl_TPDU_Element;

            pl_TPDU_Element->v_SequenceNo = vp_SequenceNo;
            pl_TPDU_Element->v_SMSSlot.Storage = pp_SMSSlot->Storage;
            pl_TPDU_Element->v_SMSSlot.Position = pp_SMSSlot->Position;

            INF("TPDU_DataLen=%d, SeqNo=%d, Storage=%d, Pos=%d\n", vp_TPDUDataLength, vp_SequenceNo, pp_SMSSlot->Storage, pp_SMSSlot->Position);

            if (smsClient2_22MessageListTPDUUserDataRead(vp_TPDUDataLength, pp_TPDUData, (bool) (vp_SequenceNo == 1), &vl_UserDataLength, &pl_UserData)) {
                pl_TPDU_Element->v_UserDataLength = vl_UserDataLength;
                pl_TPDU_Element->p_UserData = pl_UserData;

                INF("UserDataLen=%d\n", vl_UserDataLength);

                //update the TPDU count
                pp_MessageElement->v_TpduCount++;

                //update the TPDU user data length
                pp_MessageElement->v_TotalTpduLength += vl_UserDataLength;

                vl_PassedOk = TRUE;
            }
        } else {
            ERR("Heap allocation error (t_smsClient_TpduElement) !\n");
        }
    }

    return (vl_PassedOk);
}

/*
 **************************************************************************
 * Function: smsClient2_19MessageListTPDUDelete
 *
 * Parameters:
 *   [In] t_smsClient_MessageElement*
 *        uint16_t
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Find the element matching the supplied sequence number.
 *              If found, free the TPDU element and relink list TPDU elements
 *              if required.
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 19
static void smsClient2_19MessageListTPDUDelete(t_smsClient_MessageElement * const pp_MessageElement, const uint8_t vp_SequenceNo)
{
    if (pp_MessageElement->p_Head != NULL) {
        t_smsClient_TpduElement *pl_TPDU_Element;
        t_smsClient_TpduElement *pl_PrevTPDUElement = NULL;

        pl_TPDU_Element = smsClient2_21MessageListTPDUFind(pp_MessageElement, vp_SequenceNo, &pl_PrevTPDUElement);

        if (pl_TPDU_Element != NULL) {
            //if previous TPDU element exists relink remaining list there
            if (pl_PrevTPDUElement != NULL) {
                pl_PrevTPDUElement->p_Next = pl_TPDU_Element->p_Next;
            }
            //otherwise relink to list head
            else {
                pp_MessageElement->p_Head = pl_TPDU_Element->p_Next;
            }

            //free TPDU data
            OSA_Free(pl_TPDU_Element->p_UserData);

            //free TPDU element
            OSA_Free(pl_TPDU_Element);

            //decrement TPDU list counter
            pp_MessageElement->v_TpduCount--;
        }
    }
}

/*
 **************************************************************************
 * Function: smsClient2_20MessageListTPDUDeleteAll
 *
 * Parameters:
 *   [In] t_smsClient_TpduElement *
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Free all TPDU elements within a message element.
 *              The TPDU Element supplied should be the head of the TPDU list.
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 20
static void smsClient2_20MessageListTPDUDeleteAll(t_smsClient_TpduElement * pp_TPDUElement)
{
    t_smsClient_TpduElement *pl_NextTPDUElement = NULL;

    //check for & free each TPDU element within the message element
    while (pp_TPDUElement != NULL) {
        pl_NextTPDUElement = pp_TPDUElement->p_Next;

        //free TPDU data
        OSA_Free(pp_TPDUElement->p_UserData);

        //free TPDU element
        OSA_Free(pp_TPDUElement);

        pp_TPDUElement = pl_NextTPDUElement;
    }
}

/*
 **************************************************************************
 * Function: smsClient2_21MessageListTPDUFind
 *
 * Parameters:
 *   [In] t_smsClient_MessageElement*
 *        uint8_t
 *   [Out] t_smsClient_TpduElement**
 *
 * Return value: t_smsClient_TpduElement*
 *
 * Description: Find the TPDU matching the supplied TPDU sequence number.
 *              Return the TPDU element if found, otherwise NULL.
 *              OPTIONALLY: If pp_PrevTPDU_Element is provided (i.e. != NULL)
 *                          populate with previous list element, or NULL if
 *                          current element is list head.
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 21
static t_smsClient_TpduElement* smsClient2_21MessageListTPDUFind(const t_smsClient_MessageElement * const pp_MessageElement, const uint8_t vp_SequenceNo, t_smsClient_TpduElement * *const pp_PrevTPDU_Element)
{
    t_smsClient_TpduElement *pl_RetTPDU_Element = NULL;
    t_smsClient_TpduElement *pl_TPDUElement = pp_MessageElement->p_Head;
    t_smsClient_TpduElement *pl_PrevTPDUElement = NULL;

    //reset (optional) previous TPDU element
    if (pp_PrevTPDU_Element != NULL) {
        *pp_PrevTPDU_Element = NULL;
    }

    while ((pl_TPDUElement != NULL) && (pl_RetTPDU_Element == NULL)) {
        //match TPDU sequence number
        if (vp_SequenceNo == pl_TPDUElement->v_SequenceNo) {
            //supply (optional) previous TPDU element
            if (pp_PrevTPDU_Element != NULL) {
                *pp_PrevTPDU_Element = pl_PrevTPDUElement;
            }

            pl_RetTPDU_Element = pl_TPDUElement;
        } else {
            pl_PrevTPDUElement = pl_TPDUElement;
            pl_TPDUElement = pl_TPDUElement->p_Next;
        }
    }

    return (pl_RetTPDU_Element);
}

/*
 **************************************************************************
 * Function: smsClient2_22MessageListTPDUUserDataRead
 *
 * Parameters:
 *   [In] uint16_t,
 *        uint8_t*,
 *        bool
 *   [Out] uint16_t*,
 *         uint8_t**
 *
 * Return value: bool
 *
 * Description: Read user data from TPDU
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 22
static bool smsClient2_22MessageListTPDUUserDataRead(const uint16_t vp_TPDUDataLength,
                                                      const uint8_t * const pp_TPDUData, const bool vp_FirstPart, uint16_t * const vp_UserDataLength, uint8_t * *const pp_UserData)
{
    uint16_t vl_TPDUMarker = 0;

    uint16_t vl_RetUserDataLength = 0;
    uint8_t *pl_RetUserData = NULL;

    bool vl_PassedOk = FALSE;

    if (vp_TPDUDataLength > 0) {
        uint16_t vl_SkipBytes = 0;

        //TPDU Unpacked according to 3GPP 23.040 section 9.2.2.1

        //parse the MT SMS TPDU and extract the user data
        //skip 1st byte: TP-MTI, TP-MMS, TP-SRI, TP-UDHI, TP-RP
        vl_TPDUMarker++;

        //read originating address length
        vl_SkipBytes = ((*(pp_TPDUData + vl_TPDUMarker) / 2) + (*(pp_TPDUData + vl_TPDUMarker) % 2));

        //skip next n bytes: originating address + one length byte + TON NPI byte
        vl_TPDUMarker += vl_SkipBytes + 2;

        //skip two bytes: TP-PID byte + TP-DCS byte
        vl_TPDUMarker += 2;

        //skip seven bytes: TP-SCTS (fixed length time stamp)
        vl_TPDUMarker += 7;

        //read user data length
        vl_RetUserDataLength = *(pp_TPDUData + vl_TPDUMarker);

        //skip one byte: user data length
        vl_TPDUMarker++;

        //read user data header length
        vl_SkipBytes = *(pp_TPDUData + vl_TPDUMarker);

        //adjust user data length by subtracting user data header bytes + one length byte
        vl_RetUserDataLength -= (vl_SkipBytes + 1);

        //skip next n bytes: user data header + one length byte
        vl_TPDUMarker += vl_SkipBytes + 1;

        //WAP Push over SMS
        //functionality provided by the application, it is included here for testing purposes only
        if ((v_SmsPort == K_SMSCLIENT_WAP_PUSH_PORT_ID) && vp_FirstPart) {
            //skip one byte: TID
            vl_TPDUMarker++;

            //skip one byte: PDU type (push)
            vl_TPDUMarker++;

            //read WAP Push header length
            vl_SkipBytes = *(pp_TPDUData + vl_TPDUMarker);

            //skip next n bytes: WAP header + one length byte
            vl_TPDUMarker += vl_SkipBytes + 1;

            //adjust user data length by subtracting WAP Push header + TID, Push and length bytes
            vl_RetUserDataLength -= vl_SkipBytes + 3;
        }

        if (vl_RetUserDataLength <= K_SMSCLIENT_SUPL_PUSH_BODY_MAX_LENGTH) {
            pl_RetUserData = (uint8_t *) OSA_Malloc(vl_RetUserDataLength);

            if (pl_RetUserData != NULL) {
                uint8_t *pl_StartUserData = (uint8_t *) (pp_TPDUData + vl_TPDUMarker);

                memcpy(pl_RetUserData, pl_StartUserData, vl_RetUserDataLength);

                vl_PassedOk = TRUE;
            } else {
                ERR("Heap allocation error (%d bytes) !\n", vl_RetUserDataLength);

                vl_RetUserDataLength = 0;
            }
        } else {
            ERR("Invalid MT SMS User Data Length = %d !\n", vl_RetUserDataLength);

            vl_RetUserDataLength = 0;
        }
    }

    *pp_UserData = pl_RetUserData;
    *vp_UserDataLength = vl_RetUserDataLength;

    return (vl_PassedOk);
}

/*
 **************************************************************************
 * Function: smsClient2_23MessageListMessageElementAdd
 *
 * Parameters:
 *   [In] SMS_ConcatMessageRefNumber_t,
 *        uint8_t
 *   [Out] none
 *
 * Return value: t_smsClient_MessageElement*
 *
 * Description: Check if a message element with matching message reference
 *              exists, if so return that element.  Otherwise create a new
 *              element and add it to the head of the list
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 23
static t_smsClient_MessageElement* smsClient2_23MessageListMessageElementAdd(const SMS_ConcatMessageRefNumber_t vp_MessageReference, const uint8_t vp_MaxTpdu)
{
    t_smsClient_MessageElement *RetMessageElement_p = NULL;

    //check for matching message before creating new one
    RetMessageElement_p = smsClient2_25MessageListMessageElementFind(vp_MessageReference, NULL);

    //no matching message found so create a new one, otherwise return the existing one
    if (RetMessageElement_p == NULL) {
        RetMessageElement_p = (t_smsClient_MessageElement *) OSA_Malloc(sizeof(t_smsClient_MessageElement));

        //initialise new message element
        if (RetMessageElement_p != NULL) {
            //link to head of message list
            RetMessageElement_p->p_Next = v_MessageList.p_Head;
            v_MessageList.p_Head = RetMessageElement_p;

            RetMessageElement_p->v_MessageReference = vp_MessageReference;

            RetMessageElement_p->p_Head = NULL;
            RetMessageElement_p->v_TotalTpduLength = 0;
            RetMessageElement_p->v_TpduCount = 0;
            RetMessageElement_p->v_MaxTpdu = vp_MaxTpdu;

            //update the overall message count
            v_MessageList.v_MessageCount++;
        } else {
            ERR("Heap allocation error (t_smsClient_MessageElement) !\n");
        }
    }

    return (RetMessageElement_p);
}

/*
 **************************************************************************
 * Function: smsClient2_24MessageListMessageElementDelete
 *
 * Parameters:
 *   [In] SMS_ConcatMessageRefNumber_t
 *   [Out] none
 *
 * Return value: void
 *
 * Description: Find the element matching the supplied message reference.
 *              If found, free the message element and all associated TPDU
 *              elements and relink list message elements if required.
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 24
static void smsClient2_24MessageListMessageElementDelete(const SMS_ConcatMessageRefNumber_t vp_MessageReference)
{
    t_smsClient_MessageElement *pl_PrevMessageElement = NULL;

    t_smsClient_MessageElement *pl_MessageElement = smsClient2_25MessageListMessageElementFind(vp_MessageReference,
                                                                                         &pl_PrevMessageElement);

    if (pl_MessageElement != NULL) {
        //remove all TPDU elements from message element
        smsClient2_20MessageListTPDUDeleteAll(pl_MessageElement->p_Head);

        //if previous message element exists relink remaining list there
        if (pl_PrevMessageElement != NULL) {
            pl_PrevMessageElement->p_Next = pl_MessageElement->p_Next;
        }
        //otherwise relink to list head
        else {
            v_MessageList.p_Head = pl_MessageElement->p_Next;
        }

        //free message element
        OSA_Free(pl_MessageElement);

        //decrement overall message list counter
        v_MessageList.v_MessageCount--;
    }
}

/*
 **************************************************************************
 * Function: smsClient2_25MessageListMessageElementFind
 *
 * Parameters:
 *   [In] SMS_ConcatMessageRefNumber_t
 *   [Out] t_smsClient_MessageElement**
 *
 * Return value: t_smsClient_MessageElement*
 *
 * Description: Find the message matching the supplied message reference.
 *              Return the message element if found, otherwise NULL.
 *              OPTIONALLY: If PrevMessageElement_pp is provided (i.e. != NULL)
 *                          populate with previous list element, or NULL if
 *                          current element is list head.
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 25
static t_smsClient_MessageElement* smsClient2_25MessageListMessageElementFind(const SMS_ConcatMessageRefNumber_t vp_MessageReference, t_smsClient_MessageElement * *const pp_PrevMessageElement)
{
    t_smsClient_MessageElement *pl_RetMessageElement = NULL;
    t_smsClient_MessageElement *pl_MessageElement = v_MessageList.p_Head;
    t_smsClient_MessageElement *pl_PrevMessageElement = NULL;

    //reset (optional) previous message element
    if (pp_PrevMessageElement != NULL) {
        *pp_PrevMessageElement = NULL;
    }

    while ((pl_MessageElement != NULL) && (pl_RetMessageElement == NULL)) {
        //match message reference
        if (vp_MessageReference == pl_MessageElement->v_MessageReference) {
            //supply (optional) previous message element
            if (pp_PrevMessageElement != NULL) {
                *pp_PrevMessageElement = pl_PrevMessageElement;
            }

            pl_RetMessageElement = pl_MessageElement;
        }
        //or move to next message element
        else {
            pl_PrevMessageElement = pl_MessageElement;
            pl_MessageElement = pl_MessageElement->p_Next;
        }
    }

    return (pl_RetMessageElement);
}


/*
 **************************************************************************
 * Function: smsClient2_26TPDUUserDataHeaderRead
 *
 * Parameters:
 *   [In] uint8_t,
 *        uint8_t*
 *   [Out] uint16_t*,
 *         uint8_t*,
 *         uint8_t*
 *
 * Return value: bool - TRUE if concatentated information exists.
 *
 * Description: Read Concatenated Information from User Data Header of TPDU
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 26
static bool smsClient2_26TPDUUserDataHeaderRead(const uint8_t vp_TPDUUserHeaderDataLength,
                                                const uint8_t* const pp_TPDUUserHeaderData,
                                                      uint16_t* const pp_ConcatMessRefNum,
                                                      uint8_t* const pp_MaxNumConcatMsgs,
                                                      uint8_t* const pp_MsgSeqNum)
{
    bool vl_ConcatInfoRead = FALSE;
    uint8_t vl_TPDUMarker = 0;

    INF("\n");

    while (vl_TPDUMarker < vp_TPDUUserHeaderDataLength) {
        // Step through any IEIs found in UserDataHeader.  See 3GPP TS 23.040 section 9.2.3.24
        uint8_t vl_IEIValue;
        uint8_t vl_IEILength;
        #define TP_UDH_IEI_CONCAT 0x00 // See 3GPP TS 23.040 section 9.2.3.24

        vl_IEIValue = *(pp_TPDUUserHeaderData + vl_TPDUMarker);
        vl_TPDUMarker++;

        vl_IEILength = *(pp_TPDUUserHeaderData + vl_TPDUMarker);
        vl_TPDUMarker++;

        if (vl_IEIValue == TP_UDH_IEI_CONCAT) {
            // Here is a Concatenation IEI.  Unpack according to 3GPP TS 23.040 section 9.2.3.24.1.
            uint8_t vl_ConcatMessRefNum; // Octet 1
            uint8_t vl_MaxNumConcatMsgs; // Octet 2
            uint8_t vl_MsgSeqNum;        // Octet 3

            // Extract Concatenated Short Message Reference Number
            vl_ConcatMessRefNum = *(pp_TPDUUserHeaderData + vl_TPDUMarker);
            vl_TPDUMarker++;

            // Extract Maximum Number of Short Messages in Concatenated Short Message
            vl_MaxNumConcatMsgs = *(pp_TPDUUserHeaderData + vl_TPDUMarker);
            vl_TPDUMarker++;

            // Extract Sequence Number of the Current Short Message
            vl_MsgSeqNum = *(pp_TPDUUserHeaderData + vl_TPDUMarker);
            vl_TPDUMarker++;

            // According to 3GPP TS 23.040 section 9.2.3.24.1, if the Max Number of Concatenated Messages is 0 then the whole IEI should be ignored.
            // Also, if the Sequence Number is 0, or greater than the Max Number of Concatenated Messages, then the whole IEI should be ignored.
            if (vl_MaxNumConcatMsgs == 0 || vl_MsgSeqNum == 0 || vl_MsgSeqNum > vl_MaxNumConcatMsgs) {
                ERR("Ignoring invalid concatenation data: MessRefNum=%d MaxNum=%d SeqNum=%d\n", vl_ConcatMessRefNum, vl_MaxNumConcatMsgs, vl_MsgSeqNum);
            } else {
                INF("Concatenation Info: MessRefNum=%d MaxNum=%d SeqNum=%d\n", vl_ConcatMessRefNum, vl_MaxNumConcatMsgs, vl_MsgSeqNum);
                *pp_ConcatMessRefNum = vl_ConcatMessRefNum;
                *pp_MaxNumConcatMsgs = vl_MaxNumConcatMsgs;
                *pp_MsgSeqNum = vl_MsgSeqNum;
                vl_ConcatInfoRead = TRUE;

                // There should only be 1 concatenation IEI in the TPDU, so break out of the while loop now we have decoded the data.
                break;
            }
        }
        else {
            // No concatenation info in this IEI, step over and try again
            vl_TPDUMarker += vl_IEILength;
        }
    }

    return vl_ConcatInfoRead;
}

/*
 **************************************************************************
 * Function: smsClient2_27TPDUConcatInfoRead
 *
 * Parameters:
 *   [In] uint16_t,
 *        uint8_t*
 *   [Out] uint16_t*,
 *         uint8_t*,
 *         uint8_t*
 *
 * Return value: bool - TRUE if concatentated information exists.
 *
 * Description: Read Concatenated Information from SMS TPDU
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 27
static bool smsClient2_27TPDUConcatInfoRead(const uint16_t vp_TPDUDataLength,
                                            const uint8_t * const pp_TPDUData,
                                                  uint16_t* const pp_ConcatMessRefNum,
                                                  uint8_t* const pp_MaxNumConcatMsgs,
                                                  uint8_t* const pp_MsgSeqNum)
{
    uint16_t vl_TPDUMarker = 0;
    uint16_t vl_RetUserDataLength = 0;
    uint8_t *pl_RetUserData = NULL;
    bool     vl_PassedOk = FALSE;

    INF("\n");

    if (vp_TPDUDataLength > 0) {
        uint16_t vl_SkipBytes = 0;
        bool     vl_TP_UDHI;
        uint8_t  vl_UserDataHeaderLength;

        //TPDU Unpacked according to 3GPP TS 23.040 section 9.2.2.1

        //parse the MT SMS TPDU and extract the user data
        //get TP-UDHI from 1st byte: TP-MTI, TP-MMS, TP-SRI, TP-UDHI, TP-RP
        #define TP_UDHI_MASK 0x40 // Bit 6 01000000. See 3GPP TS 23.040 section 9.2.3.23
        vl_TP_UDHI = (bool) (*(pp_TPDUData + vl_TPDUMarker) & TP_UDHI_MASK);
        vl_TPDUMarker++;

        if (vl_TP_UDHI) {
            //read originating address length
            vl_SkipBytes = ((*(pp_TPDUData + vl_TPDUMarker) / 2) + (*(pp_TPDUData + vl_TPDUMarker) % 2));

            //skip next n bytes: originating address + one length byte + TON NPI byte
            vl_TPDUMarker += vl_SkipBytes + 2;

            //skip two bytes: TP-PID byte + TP-DCS byte
            vl_TPDUMarker += 2;

            //skip seven bytes: TP-SCTS (fixed length time stamp)
            vl_TPDUMarker += 7;

            //read user data length (This is the length of the entire user data block including the header which vl_TP_UDHI confirms exists.)
            vl_RetUserDataLength = *(pp_TPDUData + vl_TPDUMarker);
            //skip one byte: user data length
            vl_TPDUMarker++;

            //read user data header length (This is the length of user data header information which we need to parse looking for concatenation information.)
            vl_UserDataHeaderLength = *(pp_TPDUData + vl_TPDUMarker);
            vl_TPDUMarker++;

            vl_PassedOk = smsClient2_26TPDUUserDataHeaderRead(vl_UserDataHeaderLength,
                                                       pp_TPDUData + vl_TPDUMarker,
                                                       pp_ConcatMessRefNum, pp_MaxNumConcatMsgs, pp_MsgSeqNum);

            if (vl_PassedOk) {
                // Concatenation information unpacked successfully
                INF("Concatenation info unpacked\n");
            } else {
                // Concatenation information not unpacked successfully. This is likely to be because
                // the TPDU does not contain any concatenation information.
                INF("Concatenation info not available\n");
            }
        } else {
            INF("TPDU does not support concatenation\n");
        }
    }

    return (vl_PassedOk);
}


/*
 **************************************************************************
 * Function: smsClient2_28SendDeliverReport
 *
 * Description: Send RP_ACK Deliver Report to network to acknowledge successful
 *              delivery of SMS which has been port-matched and dispatched
 *              to posman.
 **************************************************************************
*/
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 28
static void smsClient2_28SendDeliverReport(void)
{
    SMS_RequestStatus_t       vl_ReqStatus;
    SMS_RequestControlBlock_t vl_ReqCtrlBlk;
    SMS_Error_t               vl_Error;
    static const SMS_TPDU_t   vl_DeliverReportTPDU = {0x02, {0x00, 0x00}};
    #define DELIVER_REPORT_RP_ACK_ERROR_CAUSE 0x0000 // RP_ErrorCause. 0 means that the deliver report is treated as being of type RP_ACK, i.e. is not a failure case.

    INF("\n");

    if (!smsClient1_1ServerReqCtrlBlkWaitModeInit(&vl_ReqCtrlBlk))
        return;

    vl_ReqStatus = Request_SMS_DeliverReportSend(&vl_ReqCtrlBlk,
                                                  DELIVER_REPORT_RP_ACK_ERROR_CAUSE,
                                                  &vl_DeliverReportTPDU,
                                                  &vl_Error);

    if (vl_ReqStatus != SMS_REQUEST_OK || vl_Error != SMS_ERROR_INTERN_NO_ERROR)
        ERR("Send Deliver Report Failed. ReqStat=%d Err=%d\n", vl_ReqStatus, vl_Error);
}



