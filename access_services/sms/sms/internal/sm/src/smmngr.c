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
 *  The Short Message Manager Object and it's methods.
 *
 *************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"

/* The Message Transport Server Interface */
#include "d_sms.h"
#include "g_sms.h"
#include "t_sms.h"
#include "r_sms.h"


/* Message Module Include Files */
#include "sm.h"
#include "smsevnthndr.h"
#include "smmngr.h"
#include "smutil.h"
#ifndef REMOVE_SMSTRG_LAYER
#include "smstrg.h"
#endif                          //REMOVE_SMSTRG_LAYER
#include "smbearer.h"
#include "smrouter.h"
#include "smsmain.h"

#include "c_sms_config.h"

// External Modules
#include "cn_client.h"


/*
**========================================================================
** Defines.
**========================================================================
*/
#define MSG_TIMER_S_INTO_MS 1000


typedef enum {
    MO_SMS_ALLOWED,
    MO_SMS_NOT_ALLOWED,
    MO_SMS_ALLOWED_MODIFICATIONS
} MO_SMS_Control_t;

static ShortMessageManager_t thisShouldNeverBeUsedByAnyone = {
    {SMS_NO_PROCESS, SMS_CLIENT_TAG_NOT_USED}, // MoreToSendSignalInfo
    {SMS_NO_PROCESS, SMS_CLIENT_TAG_NOT_USED}, // MessageSenderSignalInfo
    {SMS_NO_PROCESS, SMS_CLIENT_TAG_NOT_USED}, // DeliverReportSenderSignalInfo
    0, // SM_ReferenceNumber
    0, // LastReceived_SMS_PID
    SMS_STORAGE_POSITION_INVALID, // LastReceived_SMS_Position
    STATE_DISABLED, // MoreToSendState
    FALSE, // ClientMemoryCapacityFull
    {FALSE, FALSE, 0, 0, {0, 0, 0}}, // CN_RegistrationInfo
    NULL, // MO_SMS_InProgress
    FALSE // MoreToSend
 };

ShortMessageManager_p theShortMessageManager = &thisShouldNeverBeUsedByAnyone;


static void ShortMessageManager_SmInstanceSend(
    ShortMessageManager_p thisShortMessageManager,
    ShortMessage_p SmInstance,
    const uint8_t MSG_MoreToSend,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p);


#ifndef REMOVE_SMSTRG_LAYER
/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_Read
 *
 * @description  Function for the message manager to read SM
 *
 * @param        *Slot_p               : as type of SMS_Slot_t.
 * @param        *ShortMessageStatus_p : as type of SMS_Status_t,
 * @param        *MSG_SMS_TPDU_p       : as type of SMS_SMSC_Address_TPDU_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_Read(
    const SMS_Slot_t * const Slot_p,
    SMS_Status_t * const ShortMessageStatus_p,
    SMS_SMSC_Address_TPDU_t * const SMS_Data_p)
{
    // Read the message containing the short message that we wish to read
    ShortMessage_p SmInstance = NULL;
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;
    SMS_Error_t Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    // Specifying a read storage of SMS_STORAGE_MT is meaningless
    // (to perform a read all use find first, find next, find next, etc...)
    if (Slot_p->Storage != SMS_STORAGE_MT) {
        // obtain the slotnumber
        SlotNumber = SmStorage_MediaAndPositionToSlotAndVolume(Slot_p->Storage, Slot_p->Position, NULL);
    }

    if (SlotNumber != SMS_STORAGE_POSITION_INVALID) {
        SmInstance = SmStorage_SmIdentify(SlotNumber);

        if (SmInstance) {
            // Now read this slot and copy the tpdu contents into the output
            if (SmStorage_SmRead(SlotNumber, (ShortMessagePackedAddress_t *) & SMS_Data_p->ServiceCenterAddress, &SMS_Data_p->TPDU.Length, SMS_Data_p->TPDU.Data)) {
                uint16_t Len = SmUtil_CalculateSizeOf_SM(SMS_Data_p->TPDU.Data);
                SMS_Data_p->TPDU.Length = (uint8_t) SMS_MIN(Len, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);

                // Now convert it from SM to MSG status
                *ShortMessageStatus_p = ShortMessage_GetStatus(SmInstance);

                Result = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                Result = SMS_ERROR_INTERN_MESSAGE_NOT_FOUND;
            }
        } else {
            Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
        }
    } else {
        SMS_A_(SMS_LOG_E("smmngr.c: Failed to determine short msg storage position for TPDU read"));
        Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
    }

    return (Result);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_Write
 *
 * @description  Function for the message manager to Write SM
 *  Note:  That the Position which is contained in the input Slot_p
 *  is ignored when writing the TPDU , and a new SlotNumber is assigned.
 *  where available after the Memory storage has been determined.
 *
 * @param        ShortMessageStatus : as type of MSG_ShortMessageStatus_t,
 * @param        *SMS_Data_p        : as type of SMS_SMSC_Address_TPDU_t.
 * @param        *InputSlot_p       : as type of SMS_Slot_t.
 * @param        *OutputSlot_p      : as type of SMS_Slot_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_Write(
    const SMS_Status_t ShortMessageStatus,
    const SMS_SMSC_Address_TPDU_t * const SMS_Data_p,
    SMS_Slot_t * const InputSlot_p,
    SMS_Slot_t * const OutputSlot_p)
{
    ShortMessage_p SmInstance;
    SMS_Position_t SlotNumber;
    SMS_Position_t LocalSlotNumber;
    SMS_Error_t Result = SMS_ERROR_INTERN_NO_ERROR;
    MSG_StorageMedia_t *AvailableMedia_p = NULL;

    AvailableMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

    // Specifying a position with a storage of SMS_STORAGE_MT is meaningless
    if (InputSlot_p->Storage == SMS_STORAGE_MT && InputSlot_p->Position != SMS_STORAGE_POSITION_INVALID) {
        Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    }

    if (Result == SMS_ERROR_INTERN_NO_ERROR) {
        // The client has asked for us to allocate a slot
        if (InputSlot_p->Position == SMS_STORAGE_POSITION_INVALID || InputSlot_p->Storage == SMS_STORAGE_MT) {
            LocalSlotNumber = SmStorage_SlotSearch(InputSlot_p->Storage, NULL);

            if (LocalSlotNumber == SMS_STORAGE_POSITION_INVALID) {
                // we still could not find any space on the specified media
                SMS_A_(SMS_LOG_W("smmngr.c: ShortMessageManager_SMS_Write - storage full"));
                Result = SMS_ERROR_INTERN_STORAGE_FULL;
            } else {
                // Now translate that into Storage and Offet position
                SmStorage_SlotToMediaAndPosition(LocalSlotNumber, &InputSlot_p->Storage, &InputSlot_p->Position);
            }
        }
    }

    if (Result == SMS_ERROR_INTERN_NO_ERROR) {
        // obtain the internal slotnumber
        SlotNumber = SmStorage_MediaAndPositionToSlotAndVolume(InputSlot_p->Storage, InputSlot_p->Position, AvailableMedia_p);

        // we still could not find any space on the specified media
        if (SlotNumber == SMS_STORAGE_POSITION_INVALID) {
            // we still could not find any space on the specified media
            SMS_A_(SMS_LOG_W("smmngr.c: ShortMessageManager_SMS_Write - storage full"));
            Result = SMS_ERROR_INTERN_STORAGE_FULL;
        } else {
            // Delete the one we already have
            SmInstance = SmStorage_SmIdentify(SlotNumber);

            if (SmInstance) {
                ShortMessage_Delete(SmInstance);
            }
            // StorageMedia is a dummy parameter - we are using the slot number
            SmInstance = ShortMessage_CreateAndStoreInSlot(SMS_Data_p, AvailableMedia_p, SlotNumber);

            if (SmInstance != NULL) {
                (void) ShortMessage_SetStatus(SmInstance, ShortMessageStatus);

                // Translate the slot to return the correct output slot information.
                memcpy(OutputSlot_p, InputSlot_p, sizeof(SMS_Slot_t));

                Result = SMS_ERROR_INTERN_NO_ERROR;
            } else {
                SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_SMS_Write - Failed to create short message and store in slot"));
                Result = SMS_ERROR_INTERN_STORAGE_FULL;
            }
        }
    }

    SMS_HEAP_FREE(&AvailableMedia_p);

    return (Result);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_DeletePart
 *
 * @description  Function for the short message manager to delete SM message,
 *               with the option to use the special case ShortMessage_DeletePart
 *               in order to remove the SM from the SM list and not to delete
 *               from storage (USED BY ShortMessageManager_SIMRefresh)
 *
 * @param        *InputSlot_p : as type of SMS_Slot_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_DeletePart(
    const SMS_Slot_t * const InputSlot_p,
    uint8_t deleteFromStorage)
{
    ShortMessage_p SmInstance;
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;
    SMS_Error_t Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    // Specifying a read storage of SMS_STORAGE_MT is meaningless
    //  (to perform a delete all use find first)
    if (InputSlot_p->Storage != SMS_STORAGE_MT) {
        // obtain the slotnumber
        SlotNumber = SmStorage_MediaAndPositionToSlotAndVolume(InputSlot_p->Storage, InputSlot_p->Position, NULL);
    }

    if (SlotNumber != SMS_STORAGE_POSITION_INVALID) {
        // Delete the one we already have - SmInstance not needed
        SmInstance = SmStorage_SmIdentify(SlotNumber);

        if (SmInstance) {
            if (deleteFromStorage) {
                // perform delete from sm list and storage and notify the rest of the system
                ShortMessage_Delete(SmInstance);
            } else {
                // perform delete from sm list only
                ShortMessage_DeletePart(SmInstance, deleteFromStorage);
            }
            Result = SMS_ERROR_INTERN_NO_ERROR;
        } else {
            Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
        }
    } else {
        Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
    }

    return (Result);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_Delete
 *
 * @description  Function for the short message manager to delete SM message
 *
 * @param        *InputSlot_p : as type of SMS_Slot_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_Delete(
    const SMS_Slot_t * const InputSlot_p)
{
    return ShortMessageManager_SMS_DeletePart(InputSlot_p, TRUE);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_StatusReportRead
 *
 * @description  Function for the message manager to read the status report
 *               TPDU associated with the given short message slot.
 *
 * @param        *InputSlot_p    : as type of SMS_Slot_t.
 * @param        *StatusReport_p : as type of SMS_StatusReport_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_StatusReportRead(
    const SMS_Slot_t * const InputSlot_p,
    SMS_StatusReport_t * const StatusReport_p)
{
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;
    SMS_Error_t Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    // Specifying a storage of SMS_STORAGE_MT is meaningless
    //  (to perform a status report read all use find first, find next, find next, etc...)
    if (InputSlot_p->Storage != SMS_STORAGE_MT) {
        // obtain the slotnumber
        SlotNumber = SmStorage_MediaAndPositionToSlotAndVolume(InputSlot_p->Storage, InputSlot_p->Position, NULL);
    }

    if (SlotNumber != SMS_STORAGE_POSITION_INVALID && SmStorage_ReadStatusReportTPDU(SlotNumber, StatusReport_p)) {
        Result = SMS_ERROR_INTERN_NO_ERROR;
    } else {
        // Cant get the status report for this slot
        Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
    }

    return (Result);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_StatusSet
 *
 * @description  Function for the message manager to set the Status
 *               of SM. The AT Interface provides the Slotnumber
 *               of the Short message.
 *
 * @param        *InputSlot_p       : as type of SMS_Slot_t.
 * @param        ShortMessageStatus : as type of MSG_ShortMessageStatus_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_StatusSet(
    const SMS_Slot_t * const InputSlot_p,
    const SMS_Status_t ShortMessageStatus)
{
    ShortMessage_p SmInstance = NULL;
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;
    SMS_Error_t Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    // Specifying a storage of SMS_STORAGE_MT is meaningless
    //  (to perform a status set all use find first, find next, find next, etc...)
    if (InputSlot_p->Storage != SMS_STORAGE_MT) {
        // obtain the slotnumber
        SlotNumber = SmStorage_MediaAndPositionToSlotAndVolume(InputSlot_p->Storage, InputSlot_p->Position, NULL);
    }

    if (SlotNumber != SMS_STORAGE_POSITION_INVALID) {
        // Now get the status, identify the SM that is in this slot
        SmInstance = SmStorage_SmIdentify(SlotNumber);

        if (SmInstance) {
            if (TRUE == ShortMessage_SetStatus(SmInstance, ShortMessageStatus)) {
                Result = SMS_ERROR_INTERN_NO_ERROR;
            }
        } else {
            Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
        }
    } else {
        SMS_A_(SMS_LOG_E("smmngr.c: Failed to set the short message Status "));
    }

    return (Result);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_Find
 *
 * @description  Function for the message manager to Find a Sm when
 *               the MSG type is provided . This is converted into
 *               short message type and then the message is located
 *               The output is the Slotnumber sent back to the AT
 *               Interface.
 *
 * @param        currentSlot        : as type of SMS_Slot_t,
 * @param        shortMessageStatus : as type of SMS_Status_t,
 * @param        browseOption       : as type of SMS_BrowseOption_t,
 * @param        *nextSlot          : as type of SMS_Slot_t.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_Find(
    SMS_Slot_t currentSlot,
    SMS_Status_t shortMessageStatus,
    SMS_BrowseOption_t browseOption,
    SMS_Slot_t * nextSlot)
{
    ShortMessage_p SmInstance = NULL;
    uint8_t nextShortMessageFound = FALSE;
    SMS_Position_t StorageSlot;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_NO_ERROR;

    // Some parameter parsing
    switch (browseOption) {
    case SMS_BROWSE_OPTION_FIRST:
    case SMS_BROWSE_OPTION_LAST:
        {
            // DO nothing - do not really care about slot number
            ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
            // No relative slot - so this will mean we get the first or last
            SmInstance = NULL;
        }
        break;
    case SMS_BROWSE_OPTION_NEXT:
    case SMS_BROWSE_OPTION_PREVIOUS:
        {
            // Must have a valid slot number & specific storage media
            //  (only allow SMS_STORAGE_MT for browse option first or last)
            if (currentSlot.Position == SMS_STORAGE_POSITION_INVALID || currentSlot.Storage == SMS_STORAGE_MT) {
                // Error condition
                ErrorCode = SMS_ERROR_INTERN_INVALID_BROWSE_OPTION;
            } else {
                //Obtain the slot position
                StorageSlot = SmStorage_MediaAndPositionToSlotAndVolume(currentSlot.Storage, currentSlot.Position, NULL);

                // Find the SM in that slot
                SmInstance = SmStorage_SmIdentify(StorageSlot);

                if (SmInstance == NULL) {
                    ErrorCode = SMS_ERROR_INTERN_INVALID_BROWSE_OPTION;
                }
            }
        }
        break;
    default:
        {
            ErrorCode = SMS_ERROR_INTERN_INVALID_BROWSE_OPTION;
        }
        break;
    }

    if (ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        switch (browseOption) {
        case SMS_BROWSE_OPTION_FIRST:
            SmInstance = NULL;
            // Deliberate fall thru
        case SMS_BROWSE_OPTION_NEXT:
            {
                while (nextShortMessageFound == FALSE && (SmInstance = SmStorage_GetNextMessage(SmInstance)) != NULL) {
                    if ((SMS_STATUS_ANY_STATUS == shortMessageStatus || ShortMessage_GetStatus(SmInstance) == shortMessageStatus) &&
                        // Ensure we do not return information on temporary objects (Class0, Temp MO or SM Control Dir)
                        !SmStorage_SlotPositionIsTemporary(ShortMessage_GetStoragePosition(SmInstance))) {
                        SmStorage_SlotToMediaAndPosition(ShortMessage_GetStoragePosition(SmInstance), &nextSlot->Storage, &nextSlot->Position);

                        if (nextSlot->Storage == currentSlot.Storage ||
                            // To support SMS_BROWSE_OPTION_FIRST browseoption with any storage specified
                            (SMS_STORAGE_MT == currentSlot.Storage && SMS_BROWSE_OPTION_FIRST == browseOption)) {
                            //Message found in given media
                            nextShortMessageFound = TRUE;
                        }
                    }
                }
            }
            break;
        case SMS_BROWSE_OPTION_LAST:
            SmInstance = NULL;
            // Deliberate fall thru
        case SMS_BROWSE_OPTION_PREVIOUS:
            {
                while (nextShortMessageFound == FALSE && (SmInstance = SmStorage_GetPreviousMessage(SmInstance)) != NULL) {
                    if ((SMS_STATUS_ANY_STATUS == shortMessageStatus || ShortMessage_GetStatus(SmInstance) == shortMessageStatus) &&
                        // Ensure we do not return information on temporary objects (Class0, Temp MO or SM Control Dir)
                        !SmStorage_SlotPositionIsTemporary(ShortMessage_GetStoragePosition(SmInstance))) {
                        SmStorage_SlotToMediaAndPosition(ShortMessage_GetStoragePosition(SmInstance), &nextSlot->Storage, &nextSlot->Position);

                        if (nextSlot->Storage == currentSlot.Storage ||
                            // To support SMS_BROWSE_OPTION_LAST browseoption with any storage specified
                            (SMS_STORAGE_MT == currentSlot.Storage && SMS_BROWSE_OPTION_LAST == browseOption)) {
                            //Message found in given media
                            nextShortMessageFound = TRUE;
                        }
                    }
                }
            }
            break;
        }
    }

    if (nextShortMessageFound != TRUE) {
        //No Message found in slot
        nextSlot->Storage = SMS_STORAGE_MT;
        nextSlot->Position = SMS_STORAGE_POSITION_INVALID;
        ErrorCode = SMS_ERROR_INTERN_MESSAGE_NOT_FOUND;
    }

    return (ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_CapacityGet
 *
 * @description  Returns the status of the SMS store
 *
 * @param        thisMessageManager : <add description>.
 * @param        SlotInfo_p : <add description>.
 * @param        StorageStatus_p : <add description>.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_CapacityGet(
    SMS_Storage_t Storage,
    SMS_SlotInformation_t * SlotInfo_p,
    SMS_StorageStatus_t * StorageStatus_p)
{
    // Get the Slot Information
    SmStorage_SlotStatus(Storage, SlotInfo_p);

    // Now get the Storage Status
    SmStorage_StatusGet(StorageStatus_p);

    return SMS_ERROR_INTERN_NO_ERROR;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_StorageClear
 *
 * @description  Clears out the SM and MSG storage on a particular storage for
 * either this ICCID or all others. This is a very powerful funciton,
 * and shoul be used with caution. It also invokes a reinit of the storage afterwards
 *
 * @param        Storage :
 * @param        AllThisICCID_Messages :
 * @param        AllOtherICCID_Messages :
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_StorageClear(
    const SMS_Storage_t Storage,
    const uint8_t AllThisICCID_Messages,
    const uint8_t AllOtherICCID_Messages)
{
    SMS_Error_t Result = SMS_ERROR_INTERN_NO_ERROR;
    MSG_StoreList_t *MSG_StorageList_p;
    MSG_StorageMedia_t *CurrentVolume_p;
    uint8_t loop;
    MSG_StorageMedia_t *MSG_Media_p;

    if ((!AllThisICCID_Messages) && (!AllOtherICCID_Messages)) {
        Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
        SMS_A_(SMS_LOG_E("smmngr.c: This ICCID and Other ICCID both unspecified"));
    } else if (Storage == SMS_STORAGE_SM && AllOtherICCID_Messages) {
        SMS_A_(SMS_LOG_E("smmngr.c: Request to clear other ICCID Messages for SIM"));
        Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    } else {
        MSG_StorageList_p = (MSG_StoreList_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StoreList_t));

        if (Storage == SMS_STORAGE_SM) {
            // Set up the storage list structure with the single specified
            // storage media for which the request is to apply
            MSG_StorageList_p->MSGStorageList_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));
            (void) strcpy(MSG_StorageList_p->MSGStorageList_p->StorageMediaName, SIM_VOL);
            MSG_StorageList_p->NumStorageMedia = 1;
        } else {
            // Get all media in the preferred storage list
            (void) SmStorage_GetStorageList(MSG_StorageList_p);
        }

        CurrentVolume_p = MSG_StorageList_p->MSGStorageList_p;

        for (loop = 0; loop < MSG_StorageList_p->NumStorageMedia; loop++) {
            // Clear ME storage only but the entire preferred storage list
            // was retrieved for efficiency so filter SIM if present
            if (Storage == SMS_STORAGE_ME && (strcmp(CurrentVolume_p->StorageMediaName, SIM_VOL) == 0)) {
                loop++;
                CurrentVolume_p++;
            }
            // Just check that we've not stepped out of the storage list, which is
            // possible via the previous condition
            if (loop < MSG_StorageList_p->NumStorageMedia) {
                if (AllThisICCID_Messages) {
                    SmStorage_DeleteAllThisICCID_Messages(CurrentVolume_p);
                }

                if (AllOtherICCID_Messages) {
                    SmStorage_DeleteAllOtherICCID_Messages(CurrentVolume_p);
                }

                CurrentVolume_p++;
            }
        }

        if (AllThisICCID_Messages) {
            // Re-enter the SM validate storage state.
            MessagingSystem_SetState(MESSAGING_SYSTEM_VALIDATE_SM_STORAGE);

            // Remove the current dynamic lists for Msgs, SMs and stored SMs and
            // restart the initialisation procedure
            ShortMessage_Reinitialise();
            SmStorage_ReinitialiseStorage();
        }
#if defined TEST_USING_STUBS || defined MSG_INTEGRATION_TEST
        else {
            // Indicates to test harness that storage clear has completed.
            Dispatch_SMS_ServerStatusChanged(SMS_SERVER_STATUS_MTS_READY);
        }
#endif

        MSG_Media_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));
        memset(MSG_Media_p, 0x00, sizeof(MSG_StorageMedia_t));

        if (SmStorage_GetAvailableVolume(MSG_Media_p, 1, FALSE) && !SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND) {
            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_StorageClear: Network Memory Capacity Available"));

            ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, NULL);

            //Inform the MMI if Storage Capacity Available within a given Storage Media
            SmStorage_EventHandler(SMSTORAGEEVENT_STATUSUPDATE);
        }

        SMS_HEAP_FREE(&MSG_Media_p);
        SMS_HEAP_FREE(&MSG_StorageList_p->MSGStorageList_p);
        SMS_HEAP_FREE(&MSG_StorageList_p);
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_PreferedStoreSet
 *
 * @description  Set the prefered storage for SMS
 *
 * @param        PreferedStorage    : Prefered storage .
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_PreferredStoreSet(
    SMS_Storage_t PreferredStorage)
{
    // Now set the prefered storage location
    ShortMessageRouter_PreferredStorageSet(PreferredStorage);

}
#endif                          // #ifndef REMOVE_SMSTRG_LAYER

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_DeliverReportControlSet
 *
 * @description  Set the NetWork Ack responsible for SMS
 *
 * @param        NetworkAcknowledge : Network Acknowledge responsible.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_DeliverReportControlSet(
    SMS_NetworkAcknowledge_t NetworkAcknowledge)
{
    ShortMessageRouter_DeliverReportControlSet(NetworkAcknowledge);
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_DeliverReportControlGet
 *
 * @description  Set the NetWork Ack responsible for SMS
 *
 * @param        NetworkAcknowledge_p : Network Acknowledge responsible.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_DeliverReportControlGet(
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p)
{
    ShortMessageRouter_DeliverReportControlGet(NetworkAcknowledge_p);
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_MO_RouteSet
 *
 * @description  Set the SMS MO Route
 *
 * @param        Route : SMS MO Routing.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_MO_RouteSet(
    SMS_MO_Route_t Route)
{
    ShortMessageBearer_MO_RouteSet(Route);
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_MO_RouteGet
 *
 * @description  Retreive the SMS MO Route
 *
 * @return       SMS MO Routing (enum value)
 */
/********************************************************************/
SMS_MO_Route_t ShortMessageManager_SMS_MO_RouteGet(
    void)
{
    return ShortMessageBearer_MO_RouteGet();
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_CN_CellInfoSet
 *
 * @description  Set the Call Network Cell Info
 *
 * @param        CN_CellInfo_p : CN Cell Info.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_CN_CellInfoSet(
    const ShortMessageManager_CN_CellInfo_t* const CN_CellInfo_p)
{
    if (CN_CellInfo_p) {
        memcpy(&theShortMessageManager->CN_CellInfo, CN_CellInfo_p, sizeof(ShortMessageManager_CN_CellInfo_t));
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_CN_CellInfoAvailable
 *
 * @description  Return whether or not valid CN Cell Info
 *               is awaiting processing as part of MO SMS Control by USIM.
 *
 * @return       int (boolean value)
 */
/********************************************************************/
uint8_t ShortMessageManager_CN_CellInfoAvailable(
    void)
{
    return theShortMessageManager->CN_CellInfo.data_info;
}

#ifndef REMOVE_SMSTRG_LAYER
/********************************************************************/
/**
 *
 * @function     ShortMessageManager_PollStorage
 *
 * @description  Handle the event of MESSAGEMANAGER_STORAGE_TIMEOUT
 *               This will poll the SM storage poriodically to check if someone has freed
 *               up any space in the file system.
 *
 * @param        None.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_PollStorage(
    void)
{
    uint8_t SpaceFound;
    MSG_StorageMedia_t *StorageMedia_p;

    StorageMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

    SpaceFound = SmStorage_GetAvailableVolume(StorageMedia_p, 1, FALSE);

    if (SpaceFound && !SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND) {
        SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_PollStorage: Memory Capacity Available"));
        ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, NULL);
    }
    //Inform the MMI if Storage Capacity Available within a given Storage Media
    SmStorage_EventHandler(SMSTORAGEEVENT_STATUSUPDATE);

    // Reset the timer to look again later
    Do_SMS_TimerSet_1_MS_WCT(MSG_STORAGE_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_TIMER_STORAGE_POLL_INTERVAL * MSG_TIMER_S_INTO_MS);

    SMS_HEAP_FREE(&StorageMedia_p);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_StoreMatchedStatusReport
 *
 * @description  Function attempts to match the given Status Report
 *               to the given Short Message. If there is a match
 *               the Status Report is stored.
 *
 * @param        statusReport             : short message of type Status Report.
 * @param        shortMessageToMatch_p    : short message to attempt to match.
 * @param        shortMessageSlotNumber_p : short message slot number.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t ShortMessageManager_StoreMatchedStatusReport(
    const ShortMessage_p statusReport,
    const ShortMessage_p shortMessageToMatch_p,
    SMS_Slot_t * const shortMessageSlotNumber_p)
{
    DeliveryStatus_t newStatus;
    DeliveryStatus_t oldStatus;
    uint8_t statusReportMatch = FALSE;

    if (SMS_STATUS_SENT == ShortMessage_GetStatus(shortMessageToMatch_p)) {
        if ((statusReportMatch = ShortMessage_IsStatusReportMatch(shortMessageToMatch_p, statusReport)) == TRUE) {
            newStatus = ShortMessage_GetDeliveryStatus(statusReport);
            oldStatus = ShortMessage_GetDeliveredStatus(shortMessageToMatch_p);

            SMS_B_(SMS_LOG_D("smmngr.c: RECEIVESTATUSREPORT: slot %d status %d != %d", ShortMessage_GetStoragePosition(shortMessageToMatch_p), oldStatus, newStatus));

            if (oldStatus != newStatus) {
                Tpdu_t *bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));
                SMS_StatusReport_t *statusReport_p = (SMS_StatusReport_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_StatusReport_t));
                uint8_t statusReport_TPDULen;

                if (NULL != bufferTPDU_p && statusReport_p != NULL) {
                    SMS_Position_t StatusReportSlotNo = SMS_STORAGE_POSITION_INVALID;
                    // Extract the TPDU from the short message holding the status report
                    (void) ShortMessage_GetTpdu(statusReport, &statusReport_TPDULen, (uint8_t *) bufferTPDU_p);

                    // Protect against Status Report TPDU being greater than SMS_STATUS_REPORT_TPDU_MAX_LENGTH
                    statusReport_TPDULen = SMS_MIN(SMS_STATUS_REPORT_TPDU_MAX_LENGTH, statusReport_TPDULen);

                    memcpy(statusReport_p, bufferTPDU_p, statusReport_TPDULen);

                    ShortMessage_SetCacheDeliveryStatus(shortMessageToMatch_p, newStatus);

                    shortMessageSlotNumber_p->Position = ShortMessage_GetStoragePosition(shortMessageToMatch_p);

                    if (TRUE == SmStorage_WriteStatusReportTPDU(shortMessageSlotNumber_p->Position, &StatusReportSlotNo, statusReport_p)) {
                        ShortMessage_SetStatusReportRecNum(shortMessageToMatch_p, (uint8_t) StatusReportSlotNo);

                        if (StatusReportSlotNo == 0) {
                            SMS_B_(SMS_LOG_D("smmngr.c: RECEIVESTATUSREPORT: written to ME"));
                        } else {
                            SMS_B_(SMS_LOG_D("smmngr.c: RECEIVESTATUSREPORT: written to EF_SMSR rec %d", StatusReportSlotNo));
                        }
                    } else {
                        SMS_B_(SMS_LOG_D("smmngr.c: RECEIVESTATUSREPORT: NOT written to EF_SMSR"));
                    }

                    SMS_B_(SMS_LOG_D("smmngr.c: RECEIVESTATUSREPORT: setting slot %d status %d->%d", ShortMessage_GetStoragePosition(shortMessageToMatch_p), oldStatus, newStatus));
                }

                if (NULL != bufferTPDU_p) {
                    SMS_HEAP_FREE(&bufferTPDU_p);
                }

                if (NULL != statusReport_p) {
                    SMS_HEAP_FREE(&statusReport_p);
                }
            }
        }
    } else {
        SMS_B_(SMS_LOG_D("smmngr.c: STATUS_REPORT: short message not SENT"));
    }

    return statusReportMatch;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SIMRefresh
 *
 * @description  Deletes any SIM messages from the short message list
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SIMRefresh(
    void)
{
    uint8_t allDeleted = FALSE;
    SMS_Slot_t LastSlot = { SMS_STORAGE_MT, 0 };
    SMS_Slot_t CurrentSlot = { SMS_STORAGE_MT, 0 };
    SMS_BrowseOption_t BrowseOption = SMS_BROWSE_OPTION_FIRST;

    while (!allDeleted) {
        // Get the first short message slot
        (void) ShortMessageManager_SMS_Find(LastSlot, SMS_STATUS_ANY_STATUS, BrowseOption, &CurrentSlot);

        if (CurrentSlot.Storage == SMS_STORAGE_MT && CurrentSlot.Position == SMS_STORAGE_POSITION_INVALID) {
            allDeleted = TRUE;
        } else if (CurrentSlot.Storage == SMS_STORAGE_SM) {
            // Perform a delete from sm list, without removing from storage
            (void) ShortMessageManager_SMS_DeletePart(&CurrentSlot, FALSE);
            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_SIMRefresh Class 2 short message deleted storage = %d, position = 0x%04x", CurrentSlot.Storage, CurrentSlot.Position));
            BrowseOption = SMS_BROWSE_OPTION_FIRST;
        } else {
            // Continue searching from current slot.
            LastSlot.Storage = CurrentSlot.Storage;
            LastSlot.Position = CurrentSlot.Position;
            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_SIMRefresh Non-Class 2 short message found storage = %d, position = 0x%04x", CurrentSlot.Storage, CurrentSlot.Position));
            BrowseOption = SMS_BROWSE_OPTION_NEXT;
        }
    }

    // SIM messages have now been deleted from RAM respond to the refresh event
    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_SIMRefresh: deleted all SIM short messages"));
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_InformationRead
 *
 * @description  Returns the status and concatenated parameter
 *               information for a specified SM slot
 *
 * @param        Slot_p : SM slot to retrieve information for.
 * @param        ReadInformation_p : Status and concatenated parameter
 *                                   information.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_InformationRead(
    const SMS_Slot_t * const Slot_p,
    SMS_ReadInformation_t * ReadInformation_p)
{
    ShortMessage_p SmInstance = NULL;
    SMS_Position_t SlotNumber;
    ShortMessagePackedAddress_t PackedAddress;
    SMS_Error_t Result = SMS_ERROR_INTERN_NO_ERROR;

    // obtain the slotnumber
    SlotNumber = SmStorage_MediaAndPositionToSlotAndVolume(Slot_p->Storage, Slot_p->Position, NULL);

    if (SlotNumber != SMS_STORAGE_POSITION_INVALID) {
        SmInstance = SmStorage_SmIdentify(SlotNumber);

        if (SmInstance) {
            ShortMessageTimeStamp_t ShortMessageTimestamp;

            // Get Status
            ReadInformation_p->Status = ShortMessage_GetStatus(SmInstance);

            // Get Service Centre Address
            ShortMessage_GetPackedServiceCentreAddress(SmInstance, &PackedAddress);
            // Unpack
            memset(&ReadInformation_p->SMSC_Address, 0x00, sizeof(SMS_SubscriberNumber_t));
            UnpackServiceCentreAddress((ShortMessagePackedAddress_t *) & PackedAddress, &ReadInformation_p->SMSC_Address);

            // Get MO/MT Address
            ShortMessage_GetPackedOrigOrDestAddress(SmInstance, &PackedAddress);
            // Unpack
            memset(&ReadInformation_p->Address, 0x00, sizeof(SMS_SubscriberNumber_t));
            UnpackAddressField((ShortMessagePackedAddress_t *) & PackedAddress, &ReadInformation_p->Address);

            // Get Timestamp
            SmStorage_GetMessageTimestamp(SmInstance, &ShortMessageTimestamp);
            SmStorage_PackTimestamp(&ShortMessageTimestamp, &ReadInformation_p->DateAndTime[0]);

            // Get Concat Params
            ShortMessage_IsConcatenated(SmInstance, &ReadInformation_p->ConcatParameters);
        } else {
            Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
        }
    } else {
        SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_SMS_InformationRead: Failed to determine short msg storage position for TPDU read"));
        Result = SMS_ERROR_INTERN_INVALID_MESSAGE_ID;
    }

    return (Result);
}
#endif                          // REMOVE_SMSTRG_LAYER

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_MemCapacityFullSet
 *
 * @description  Set the memory capacity full flag
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_MemCapacityFullSet(
    void)
{
    theShortMessageManager->ClientMemoryCapacityFull = TRUE;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_DispatchStatusReport
 *
 * @description  Function to extract the Status Report TPDU from the
 *               given short message and dispatch it.
 *
 * @param        statusReport_p         : short message of type Status Report.
 * @param        shortMessageSlotNumber : short message slot number.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_DispatchStatusReport(
    const ShortMessage_p statusReport_p,
    const SMS_Slot_t * const shortMessageSlotNumber_p)
{
    Tpdu_t *bufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));
    SMS_SMSC_Address_TPDU_t *statusReport_TPDU_p = (SMS_SMSC_Address_TPDU_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_SMSC_Address_TPDU_t));

    // Notify any interested client applications of a new Status Report
    if (NULL != statusReport_TPDU_p && NULL != bufferTPDU_p) {
        ShortMessage_GetPackedServiceCentreAddress(statusReport_p, (ShortMessagePackedAddress_t *) & statusReport_TPDU_p->ServiceCenterAddress);

        // Extract the TPDU from the short message holding the status report
        (void) ShortMessage_GetTpdu(statusReport_p, &statusReport_TPDU_p->TPDU.Length, (uint8_t *) bufferTPDU_p);

        // Protect against Status Report TPDU being greater than MSG_TPDU_MAX_LENGTH
        if (SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH > statusReport_TPDU_p->TPDU.Length) {
            memset(statusReport_TPDU_p->TPDU.Data, 0, SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH);
        } else {
            statusReport_TPDU_p->TPDU.Length = SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH;
        }

        memcpy(statusReport_TPDU_p->TPDU.Data, bufferTPDU_p, statusReport_TPDU_p->TPDU.Length);

        SMS_B_(SMS_LOG_D("smmngr.c: DISPATCH Dispatch_MSG_SMS_StatusReportReceived"));

        Dispatch_SMS_StatusReportReceived(*shortMessageSlotNumber_p, statusReport_TPDU_p);
    }

    if (NULL != bufferTPDU_p) {
        SMS_HEAP_FREE(&bufferTPDU_p);
    }

    if (NULL != statusReport_TPDU_p) {
        SMS_HEAP_FREE(&statusReport_TPDU_p);
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_HandleStatusReport
 *
 * @description  Match a Status Report to a stored SM and dispatch
 *               indication to clients
 *
 * @param        *thisObject : void type.
 * @param        eventData   : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_HandleStatusReport(
    void *thisObject,
    EventData_p eventData)
{
    ShortMessage_p newStatusReport = eventData->ShortMessage;
    SMS_Slot_t shortMessageSlotNumber = { SMS_STORAGE_ME, SMS_STORAGE_POSITION_INVALID };
#ifndef REMOVE_SMSTRG_LAYER
    if (SMS_STATUS_REPORTS_MATCH_STORE) {
        uint8_t statusReportMatch = FALSE;
        ShortMessage_p shortMessageToMatch_p = NULL;

        SMS_IDENTIFIER_NOT_USED(thisObject);

        while (((shortMessageToMatch_p = SmStorage_GetNextMessage(shortMessageToMatch_p)) != NULL) && statusReportMatch == FALSE) {
            statusReportMatch = ShortMessageManager_StoreMatchedStatusReport(newStatusReport, shortMessageToMatch_p, &shortMessageSlotNumber);
        }
    }
#endif                          // REMOVE_SMSTRG_LAYER

    SMS_IDENTIFIER_NOT_USED(thisObject);

    ShortMessageManager_DispatchStatusReport(newStatusReport, &shortMessageSlotNumber);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_ReceiveNewStatusReport
 *
 * @description  Handle the event of SHORTMESSAGEMANAGER_RECEIVE_NEW_STATUS_REPORT
 *
 * @param        *thisObject : void type.
 * @param        eventData   : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageManager_ReceiveNewStatusReport(
    void *thisObject,
    EventData_p eventData)
{
#ifndef REMOVE_SMSTRG_LAYER
    ShortMessage_p newStatusReport = eventData->ShortMessage;
    MessagingSystemState_t MessagingSystemState;

    MessagingSystem_GetState(&MessagingSystemState);

    if (MessagingSystemState < MESSAGING_SYSTEM_IDLE) {
        // Store Status Report SM and process later as the SM whose status is to be updated
        // cannot be identified until the SM storage is completely initialised.
        // Don't know which volume the SM associated with this Status Report is on so
        // temporarily store on \ifs. Status Reports are always acked so there is no problem
        // if the TPDU cannot be stored in the volume of the corresponding SM during processing
        // of the Status Report.
        // If there were no storage on \ifs this would be a problem since the Status Report will
        // be acked but not stored so not processed later and the SM status would not be updated.
        (void) SmStorage_WriteStatusReportSM(newStatusReport, (MSG_StorageMedia_t *) SMS_FS_DIR_USE_SIM_CACHE);
    } else
#endif                          // REMOVE_SMSTRG_LAYER
    {
        ShortMessageManager_HandleStatusReport(thisObject, eventData);
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_MemCapacityAvailableSent
 *
 * @description  If memory capacity available was initiated via the
 *               requester interface this function initiates a response
 *               to that request.
 *
 * @param        *thisObject : void type.
 * @param        eventData   : as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageManager_MemCapacityAvailableSent(
    void *thisObject,
    EventData_p eventData)
{
    ShortMessageManager_p thisShortMessageManager = (ShortMessageManager_p) thisObject;

    // The Process will be not be zero if memory capacity available was initiated
    // via the requester interface therefore a response to that request must be sent.
    if (thisShortMessageManager->MessageSenderSignalInfo.ProcessId != SMS_NO_PROCESS) {
        // Memory capacity available was triggered by a client request therefore
        // a response signal must be sent.
        SMS_MemCapacityAvailableSend_Resp_t *SendBuf_p;

        SMS_A_(SMS_LOG_I("smmngr.c: MEMCAPACITYAVAILABLE_RESPONSE: Process = 0x%08x, ClientTag = 0x%08x, Cause = 0x%04x",
                      (unsigned int) thisShortMessageManager->MessageSenderSignalInfo.ProcessId, (unsigned int) thisShortMessageManager->MessageSenderSignalInfo.ClientTag,
                      eventData->RMS_V2_ErrorCode));

        SendBuf_p = (SMS_MemCapacityAvailableSend_Resp_t *) SMS_SIGNAL_ALLOC(SMS_MemCapacityAvailableSend_Resp_t, RESPONSE_SMS_MEMCAPACITYAVAILABLESEND);

        // Set the client tag in the return signal
        Do_SMS_ClientTag_Set(SendBuf_p, thisShortMessageManager->MessageSenderSignalInfo.ClientTag);

        SendBuf_p->ErrorCode = eventData->RMS_V2_ErrorCode;

        SMS_SEND(&SendBuf_p, thisShortMessageManager->MessageSenderSignalInfo.ProcessId);
        thisShortMessageManager->MessageSenderSignalInfo.ProcessId = SMS_NO_PROCESS;
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_DeliverReportConfirm
 *
 * @description  Handle the event of SHORTMESSAGEMANAGER_DELIVERREPORT_CONFIRM
 *
 * @param        *thisObject : void type.
 * @param        eventData :   as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageManager_DeliverReportConfirm(
    void *thisObject,
    EventData_p eventData)
{
    ShortMessageManager_p thisShortMessageManager = (ShortMessageManager_p) thisObject;

    // Only proceed with confirmation if we have someone to send it to!
    if (thisShortMessageManager->DeliverReportSenderSignalInfo.ProcessId != SMS_NO_PROCESS) {
        SMS_DeliverReportSend_Resp_t *SendBuf_p = (SMS_DeliverReportSend_Resp_t *) SMS_SIGNAL_ALLOC(SMS_DeliverReportSend_Resp_t, RESPONSE_SMS_DELIVERREPORTSEND);

        SMS_A_(SMS_LOG_I("smmngr.c: DELIVERREPORTACKSEND: Process = 0x%08x, ClientTag = 0x%08x, Cause = 0x%04x",
                      (unsigned int) thisShortMessageManager->DeliverReportSenderSignalInfo.ProcessId,
                      (unsigned int) thisShortMessageManager->DeliverReportSenderSignalInfo.ClientTag, eventData->RMS_V2_ErrorCode));
        // Get the client tag to 'global' data to be able to use it in the send
        Do_SMS_ClientTag_Set(SendBuf_p, thisShortMessageManager->DeliverReportSenderSignalInfo.ClientTag);

        // Set the error code in the signal buffer from that returned by the modem
        SendBuf_p->ErrorCode = eventData->RMS_V2_ErrorCode;

        SMS_A_(SMS_LOG_I
               ("ShortMessageManager_DeliverReportConfirm sending Deliver Report Confirm Response signal to socket:0x%08x", thisShortMessageManager->DeliverReportSenderSignalInfo.ProcessId));
        SMS_SEND(&SendBuf_p, thisShortMessageManager->DeliverReportSenderSignalInfo.ProcessId);
        thisShortMessageManager->DeliverReportSenderSignalInfo.ProcessId = SMS_NO_PROCESS;
    }
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_ShortMessageSent
 *
 * @description  Handle the event of SHORTMESSAGEMANAGER_SHORT_MESSAGE_SENT
 *
 * @param        *thisObject : void type.
 * @param        eventData :   as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageManager_ShortMessageSent(
    void *thisObject,
    EventData_p eventData)
{
    ShortMessageManager_p thisShortMessageManager = (ShortMessageManager_p) thisObject;
    SMS_ShortMessageSend_Resp_t *SendBuf_p;

    SMS_A_(SMS_LOG_I("smmngr.c: ACKSEND: Process = 0x%08x, ClientTag = 0x%08x, Cause = 0x%04x, TP-MR = 0x%02x",
                  (unsigned int) thisShortMessageManager->MessageSenderSignalInfo.ProcessId,
                  (unsigned int) thisShortMessageManager->MessageSenderSignalInfo.ClientTag, eventData->RMS_V2_ErrorCode, thisShortMessageManager->SM_ReferenceNumber));

    SendBuf_p = (SMS_ShortMessageSend_Resp_t *) SMS_SIGNAL_ALLOC(SMS_ShortMessageSend_Resp_t, RESPONSE_SMS_SHORTMESSAGESEND);

    SendBuf_p->SM_Reference = thisShortMessageManager->SM_ReferenceNumber;

    // Set the client tag in the return signal
    Do_SMS_ClientTag_Set(SendBuf_p, thisShortMessageManager->MessageSenderSignalInfo.ClientTag);

    // Set the error code in the signal buffer from that returned by the modem
    SendBuf_p->ErrorCode = eventData->RMS_V2_ErrorCode;

    SMS_A_(SMS_LOG_I("ShortMessageManager_ShortMessageSent sending SMS Send Response signal to socket:0x%08x", thisShortMessageManager->MessageSenderSignalInfo.ProcessId));
    SMS_SEND(&SendBuf_p, thisShortMessageManager->MessageSenderSignalInfo.ProcessId);
    thisShortMessageManager->MessageSenderSignalInfo.ProcessId = SMS_NO_PROCESS;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_MoreToSend_IsBlocking
 *
 * @description  Determine if a previous client had requested to maintain
 *               the radio link, if there was no previous client or if this
 *               is the same (previous) client we will return FALSE
 *               otherwise if this is not the previous client we will return
 *               TRUE.
 *
 * @param        thisShortMessageManager : short message manager instance.
 * @param        senderSignalInfo_p      : sender signal info.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t ShortMessageManager_MoreToSend_IsBlocking(
    ShortMessageManager_p thisShortMessageManager,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p)
{
    uint8_t Result = FALSE;

    if (SMS_NO_PROCESS != thisShortMessageManager->MoreToSendSignalInfo.ProcessId && senderSignalInfo_p->ProcessId != thisShortMessageManager->MoreToSendSignalInfo.ProcessId) {
        // Currently being blocked by another client requesting the radio channel
        // to be kept open for multiple sends, so reject requests from other clients
        // until other client is finished or timed-out
        Result = TRUE;
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_MoreToSend_StateHandler
 *
 * @description  Function to handle the MoreToSendState state changes:
 *
 *                 STATE_DISABLED = MoreToSend has not requested the
 *                                  radio link to be preserved or CMMS
 *                                  already has the link preserved
 *                 STATE_OPENING  = MoreToSend has requested the radio
 *                                  link to be preserved and this is
 *                                  the first message part
 *                 STATE_OPENED   = MoreToSend has previously requested
 *                                  the radio link to be preserved and
 *                                  this is a subsequent message part
 *                 STATE_CLOSING  = MoreToSend has request for the radio
 *                                  link to be closed and the final part
 *                                  has already been sent
 *
 *               State changes will occur as follows:
 *
 *                 STATE_DISABLED -> STATE_OPENING (only if CMMS is not
 *                                                  preserving the link)
 *                 STATE_OPENING  -> STATE_OPENED  (if more parts are
 *                                                  being sent)
 *                 STATE_OPENING  -> STATE_CLOSING (if the last part has
 *                                                  been sent)
 *                 STATE_CLOSING  -> STATE_DISABLED(if we have reset RLC)
 *
 *               This function is called prior to ShortMessageManager_SMS_Send
 *               (once the ShortMessageobject has been created.  In order to
 *               correctly process the STATE_CLOSING state, we must call this
 *               function again after ShortMessageManager_SMS_Send to reset
 *               the RLC settings.
 *
 * @param        thisShortMessageManager : short message manager instance.
 * @param        MSG_MoreToSend          : indicates if there are more short messages
 *                                         to be sent on the same radio link
 * @param        senderSignalInfo_p      : sender signal info.
 * @param        SmInstance              : short message object.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageManager_MoreToSend_StateHandler(
    ShortMessageManager_p thisShortMessageManager,
    const uint8_t MSG_MoreToSend,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p,
    ShortMessage_p SmInstance)
{
    SMS_RelayControl_t RelayLinkControl;

    ShortMessageManager_SMS_RelayLinkControl_Get(&RelayLinkControl);

    Do_SMS_TimerReset_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);

    SMS_B_(SMS_LOG_D
           ("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Timeout reset. Current state %d, MoreToSend %d, RLC %d, Short Message 0x%08x.", thisShortMessageManager->MoreToSendState,
            MSG_MoreToSend, RelayLinkControl, (int) SmInstance));

    switch (thisShortMessageManager->MoreToSendState) {
    case STATE_DISABLED:
        {
            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Entering case STATE_DISABLED %d.", STATE_DISABLED));

            if (MSG_MoreToSend) {
                SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: MoreToSend requested."));
                // Only initiate the RLC set if the RLC is currently disabled
                if (SMS_RELAY_CONTROL_DISABLED == RelayLinkControl) {
                    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: RLC currently disabled, enabling."));
                    ShortMessageManager_SMS_RelayLinkControl_Set(SMS_RELAY_CONTROL_ENABLED);
                    // Store the client info (ProcessId), which will be used to block all other
                    // clients until this client has finished
                    thisShortMessageManager->MoreToSendSignalInfo = *senderSignalInfo_p;
                    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Setting MoreToSend state to STATE_OPENING %d.", STATE_OPENING));
                    // Identify that MoreToSend has set the RLC, not CMMS
                    // This should ensure that smbearer correctly sets this as the first message part
                    thisShortMessageManager->MoreToSendState = STATE_OPENING;
                    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Starting MoreToSend Timer %d seconds.", SMS_MORETOSEND_TIMEOUT_VALUE));
                    // Set the MoreToSend RLC timer
                    Do_SMS_TimerSet_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_MORETOSEND_TIMEOUT_VALUE * MSG_TIMER_S_INTO_MS);
                    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Update the MoreToSend state within short message."));
                    // Set the MoreToSend message attribute (as supplied by the client request),
                    // this will be used to determine the TxParams in smbearer.c
                    ShortMessage_SetMoreToSend(SmInstance, thisShortMessageManager->MoreToSendState);
                } else {
                    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: RLC enabled, nothing to do."));
                    // Set the MoreToSend RLC timer
                    Do_SMS_TimerSet_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_MORETOSEND_TIMEOUT_VALUE * MSG_TIMER_S_INTO_MS);
                }
            } else {
                SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: MoreToSend not requested."));
                // Ensure client blocking parameters are reset
                thisShortMessageManager->MoreToSendSignalInfo.ProcessId = SMS_NO_PROCESS;
                thisShortMessageManager->MoreToSendSignalInfo.ClientTag = SMS_CLIENT_TAG_NOT_USED;
            }
            break;
        }

    case STATE_OPENING:
    case STATE_OPENED:
        {
            SMS_B_(SMS_LOG_D
                   ("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Entering case %s %d.",
                    (STATE_OPENING == thisShortMessageManager->MoreToSendState ? "STATE_OPENING" : "STATE_OPENED"), thisShortMessageManager->MoreToSendState));
            // We will remain in STATE_OPENED until MSG_MoreToSend is set to FALSE, or we timeout
            if (MSG_MoreToSend) {
                SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: MoreToSend requested, updating state to STATE_OPENED %d.", STATE_OPENED));
                // This should ensure that smbearer correctly sets this as the second message part
                thisShortMessageManager->MoreToSendState = STATE_OPENED;
            } else {
                SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: MoreToSend not requested, updating state to STATE_CLOSING %d.", STATE_CLOSING));
                // This should ensure that smbearer correctly sets this as the third (last) message part
                thisShortMessageManager->MoreToSendState = STATE_CLOSING;
            }

            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Starting MoreToSend Timer %d seconds.", SMS_MORETOSEND_TIMEOUT_VALUE));
            // Set the MoreToSend RLC timer
            Do_SMS_TimerSet_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_MORETOSEND_TIMEOUT_VALUE * MSG_TIMER_S_INTO_MS);
            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Update the MoreToSend state within short message."));
            // Set the MoreToSend message attribute (as supplied by the client request),
            // this will be used to determine the TxParams in smbearer.c
            ShortMessage_SetMoreToSend(SmInstance, thisShortMessageManager->MoreToSendState);
            break;
        }

    case STATE_CLOSING:
        {
            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Entering case STATE_CLOSING %d.", STATE_CLOSING));

            if (SMS_RELAY_CONTROL_DISABLED != RelayLinkControl) {
                SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: RLC currently enabled, disabling."));
                // Reset the link control setting back to previous state
                ShortMessageManager_SMS_RelayLinkControl_Set(SMS_RELAY_CONTROL_DISABLED);
            }

            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Updating state to STATE_DISABLED %d.", STATE_DISABLED));
            thisShortMessageManager->MoreToSendState = STATE_DISABLED;
            // Unblock other clients
            thisShortMessageManager->MoreToSendSignalInfo.ProcessId = SMS_NO_PROCESS;
            thisShortMessageManager->MoreToSendSignalInfo.ClientTag = SMS_CLIENT_TAG_NOT_USED;
            break;
        }

    default:
        SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_MoreToSend_StateHandler: Entering case UNKNOWN %d.", thisShortMessageManager->MoreToSendState));
        break;
    }
}


// Defines for Card Application Toolkit (CAT) MO SMS Control
#define SMS_CAT_MO_SMS_CONTROL_MAX_LENGTH      0xFF
#define SMS_CAT_ADDRESS_TAG                    0x06
#define SMS_CAT_LOCATION_INFORMATION_TAG       0x13
#define SMS_CAT_LOCATION_INFORMATION_LENGTH    7  //  No extended Cell ID Ref. TS 31.111 section 8.19
#define SMS_CAT_LOCATION_EXT_CID_LENGTH        2
#define SMS_CAT_DEVICE_IDENTITIES_TAG          0x02
#define SMS_CAT_DEVICE_IDENTITIES_TOTAL_SIZE   4  // Ref. TS 31.111 section 8.7
#define SMS_CAT_DEVICE_IDENTITIES_LENGTH       2  // Ref. TS 31.111 section 8.7
#define SMS_CAT_DEVICE_UICC                    0x81
#define SMS_CAT_DEVICE_TERMINAL                0x82
#define SMS_CAT_COMPREHENSION_REQUIRED_MASK    0x80

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_ControlSendEnvelope
 *
 * @description  Contruct the SAT envelope for MO SMS Control by USIM
 *               and send it to SAT.
 *
 *               See 3GPP TS 31.111 v10.1.0 Section 7.3.2.2 for encoding
 *               used in this function
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_ControlSendEnvelope(
    void)
{
    uint8_t error = FALSE;
    uint8_t *Envelope_p = SMS_HEAP_UNTYPED_ALLOC(SMS_CAT_MO_SMS_CONTROL_MAX_LENGTH);
    ShortMessagePackedAddress_t SMSC_Address;
    ShortMessagePackedAddress_t DestinationAddress;

    if (theShortMessageManager->MO_SMS_InProgress) {
        if (theShortMessageManager->CN_CellInfo.data_info == SMS_CN_REG_DATA_ERROR) {
            SMS_A_(SMS_LOG_E("smmngr: SMS_ControlSendEnvelope: no valid CN registration info"));
            error = TRUE;
        }
        else {
            ShortMessage_GetPackedServiceCentreAddress(theShortMessageManager->MO_SMS_InProgress, &SMSC_Address);
            ShortMessage_GetPackedOrigOrDestAddress(theShortMessageManager->MO_SMS_InProgress, &DestinationAddress);
        }
    } else {
        SMS_A_(SMS_LOG_E("smmngr: SMS_ControlSendEnvelope: SmInstance = NULL"));
        error = TRUE;
    }

    if (!error && Envelope_p != NULL) {
        // Build and send CB download envelope
        uint32_t EnvelopeSize;
        uint8_t AddressLength;
        uint8_t *Length_p;
        uint8_t *Env_p = Envelope_p;

        *Env_p++ = SMS_CAT_MO_SMS_CONTROL_TAG;

        // Set up a pointer to the Length byte to be filled in later.
        // Note that the length for the MO SMS Control envelope will not exceed 255 and will therefore be coded on 1 byte.
        Length_p = Env_p;

        // point to start of Device identities octet
        Env_p++;

        // Build device identities TLV
        *Env_p++ = SMS_CAT_DEVICE_IDENTITIES_TAG;       // TAG
        *Env_p++ = SMS_CAT_DEVICE_IDENTITIES_LENGTH; // LENGTH of VALUE (2)
        *Env_p++ = SMS_CAT_DEVICE_TERMINAL;   // VALUE (Source)
        *Env_p++ = SMS_CAT_DEVICE_UICC;       // VALUE (Destination)
        *Length_p = SMS_CAT_DEVICE_IDENTITIES_TOTAL_SIZE;    // Length of entire Device Identities TLV

        // Build the SMSC Address TLV
        *Env_p++ = SMS_CAT_ADDRESS_TAG;     // TAG
        AddressLength = SMS_MIN(SMSC_Address.Length, SMS_MAX_PACKED_ADDRESS_LENGTH + 1); // +1 for TON/NPI byte
        *Env_p++ = AddressLength;
        if (AddressLength) {
            *Env_p++ = SMSC_Address.TypeOfAddress;
            memcpy(Env_p, SMSC_Address.AddressValue, AddressLength - 1); // -1 since length includes TON/NPI byte
            Env_p += AddressLength - 1;
        }
        *Length_p += AddressLength + 2;   // +2 for address length and address tag bytes

        // Build the Destination Address TLV
        *Env_p++ = SMS_CAT_ADDRESS_TAG;     // TAG
        if(DestinationAddress.Length) {
            // Convert destination address length from number of semi-octets to number of bytes
            AddressLength = ((DestinationAddress.Length + 1) / 2) + 1; // +1 for TON/NPI byte
            AddressLength = SMS_MIN(AddressLength, SMS_MAX_PACKED_ADDRESS_LENGTH + 1); // +1 for TON/NPI byte
            *Env_p++ = AddressLength;
            *Env_p++ = DestinationAddress.TypeOfAddress;
            memcpy(Env_p, DestinationAddress.AddressValue, AddressLength - 1); // -1 since length includes TON/NPI byte
            Env_p += AddressLength - 1;
        } else {
            *Env_p++ = 0; // Address length
        }
        *Length_p += AddressLength + 2;   // +2 for address length and address tag bytes

        // Build the location TLV
        *Env_p++ = SMS_CAT_LOCATION_INFORMATION_TAG;     // TAG
        if (theShortMessageManager->CN_CellInfo.use_extended_cid) {
            *Env_p++ = SMS_CAT_LOCATION_INFORMATION_LENGTH + SMS_CAT_LOCATION_EXT_CID_LENGTH;
        } else {
            *Env_p++ = SMS_CAT_LOCATION_INFORMATION_LENGTH;
        }
        *Env_p++ = theShortMessageManager->CN_CellInfo.mcc_mnc[0];
        *Env_p++ = theShortMessageManager->CN_CellInfo.mcc_mnc[1];
        *Env_p++ = theShortMessageManager->CN_CellInfo.mcc_mnc[2];
        *Env_p++ = (uint8_t)(0xFF & (theShortMessageManager->CN_CellInfo.lac >> 8)); // Hi byte
        *Env_p++ = (uint8_t)(0xFF & theShortMessageManager->CN_CellInfo.lac); // Lo byte

        // Cell ID is 16 LSB of UC-id TS 31.111 section 8.19
        *Env_p++ = (uint8_t)(0xFF & (theShortMessageManager->CN_CellInfo.cid >> 8)); // Hi byte
        *Env_p++ = (uint8_t)(0xFF & theShortMessageManager->CN_CellInfo.cid); // Lo byte

        if (theShortMessageManager->CN_CellInfo.use_extended_cid) {
            // Extended Cell ID (RNC-id) is 12 MSB of UC-id TS 31.111 section 8.19
            *Env_p++ = (uint8_t)(0x0F & (theShortMessageManager->CN_CellInfo.cid >> 24)); // Hi byte
            *Env_p++ = (uint8_t)(0xFF & (theShortMessageManager->CN_CellInfo.cid >> 16)); // Lo byte
            *Length_p += SMS_CAT_LOCATION_INFORMATION_LENGTH + SMS_CAT_LOCATION_EXT_CID_LENGTH + 2;   // +2 for location length and location tag bytes
        } else {
            *Length_p += SMS_CAT_LOCATION_INFORMATION_LENGTH + 2;   // +2 for location length and location tag bytes
        }

        // Clear the CN registration info so that it is not processed again
        theShortMessageManager->CN_CellInfo.data_info = SMS_CN_REG_DATA_CLEAR;
        theShortMessageManager->CN_CellInfo.use_extended_cid = FALSE;
        theShortMessageManager->CN_CellInfo.mcc_mnc[0] = 0;
        theShortMessageManager->CN_CellInfo.mcc_mnc[1] = 0;
        theShortMessageManager->CN_CellInfo.mcc_mnc[2] = 0;
        theShortMessageManager->CN_CellInfo.lac = 0;
        theShortMessageManager->CN_CellInfo.cid = 0;

        // Overall size of the entire envelope is the *Length_p +2 bytes for the MO SMS Control tag and length byte.
        EnvelopeSize = *Length_p + 2;

        // Perform envelope download
        {
            ste_sim_t *UICC_p = MessagingSystem_UICC_SimAsyncGet();

            if (UICC_p == NULL) {
                SMS_A_(SMS_LOG_E("smmngr: SMS_ControlSendEnvelope: SIM UICC Server Object FAILED"));
                error = TRUE;
            } else {
                int UICC_Status;
                uintptr_t UICC_Client_Tag;

                // The Client Tag is used by the CAT callback to distinguish between different CAT envelope responses
                UICC_Client_Tag = (uintptr_t) SMS_CAT_MO_SMS_CONTROL_TAG;

                SMS_B_(SMS_LOG_D("smmngr: SMS_ControlSendEnvelope: calling ste_cat_envelope_command()"));
                SMS_A_(SmUtil_HexPrint_Raw(Envelope_p, EnvelopeSize, "SMS: smmngr.c: MO SMS Control Envelope"));

                UICC_Status = ste_cat_envelope_command(UICC_p, UICC_Client_Tag, (const char *) Envelope_p, EnvelopeSize);

                SMS_A_(SMS_LOG_I("smmngr: SMS_ControlSendEnvelope: ste_cat_envelope_command returned %d", UICC_Status));
                SMS_IDENTIFIER_NOT_USED(UICC_Status);       // Silence warnings if A prints not enabled.
            }
        }
    } else {
        // Memory allocation failed
        SMS_A_(SMS_LOG_E("smmngr: SMS_ControlSendEnvelope: envelope not sent"));
        error = TRUE;
    }

    if (Envelope_p) {
        // Free envelope buffer memory
        SMS_HEAP_FREE(&Envelope_p);
    }

    if (error) {
        EventData_t eventData;

        theShortMessageManager->CN_CellInfo.data_info = SMS_CN_REG_DATA_CLEAR;

        // Need to send failure back to client here
        eventData.RMS_V2_ErrorCode = SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED;
        ShortMessageManager_ShortMessageSent(theShortMessageManager, &eventData);
    }
}


/*
 * Check status word in envelope response for 0x9000, 0x91xx and 0x92xx patterns.
 */
#define IS_SW_MO_SMS_ALLOWED(word) \
    word == 0x9000 || \
    (word >> 8) == 0x91 || \
    (word >> 8) == 0x92

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_ControlHandleResponse
 *
 * @description  Decode the SAT envelope response for MO SMS Control
 *               by USIM and determine whether the SMSC and Destination
 *               addresses are allowed, barred or require modification.
 *               The MO SMS send is then initiated or rejected as appropriate.
 *
 *               See 3GPP TS 31.111 v10.1.0 Section 7.3.2.2 for encoding
 *               used in this function
 *
 * @param        cat_ec_response_p: SAT envelope response
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_ControlHandleResponse(
    ste_cat_ec_response_t* cat_ec_response_p)
{
    uint8_t error = FALSE;

    if (cat_ec_response_p){
        MO_SMS_Control_t MO_SMS_Control = MO_SMS_NOT_ALLOWED;
        uint8_t apduLength = cat_ec_response_p->apdu.len;
        uint8_t* Env_p = (uint8_t*)cat_ec_response_p->apdu.buf;
        uint16_t StatusWord;

        if (Env_p) {
            // Status word is the last 2 bytes
            StatusWord = (cat_ec_response_p->apdu.buf[apduLength-2] << 8);
            StatusWord = StatusWord | cat_ec_response_p->apdu.buf[apduLength-1];

            if (IS_SW_MO_SMS_ALLOWED(StatusWord)) {
                if (apduLength == 2) { // Allowed if only status words in APDU
                    MO_SMS_Control = MO_SMS_ALLOWED;
                } else {
                    MO_SMS_Control = *Env_p++;
                }
            }

            SMS_A_(SMS_LOG_I("smmngr: SMS_ControlHandleReponse: ec_status 0x%04X, apduLength %d, StatusWord 0x%04X, MO_SMS_Control %d", cat_ec_response_p->ec_status, apduLength, StatusWord, MO_SMS_Control));
            SMS_A_(SmUtil_HexPrint_Raw((uint8_t*)cat_ec_response_p->apdu.buf, apduLength, "SMS: smmngr.c: MO SMS Control cat_ec_response_p->apdu.buf"));

            Env_p++; // Skip the response length

            switch (MO_SMS_Control) {
            case MO_SMS_ALLOWED:
                {
                    ShortMessageManager_SmInstanceSend(theShortMessageManager, theShortMessageManager->MO_SMS_InProgress, theShortMessageManager->MoreToSend, &theShortMessageManager->MessageSenderSignalInfo);
                }
                break;
            case MO_SMS_ALLOWED_MODIFICATIONS:
                {
                    ShortMessagePackedAddress_t SMSC_Address;
                    ShortMessagePackedAddress_t DestinationAddress;
                    uint8_t SemiOctetPadding = 0;

                    // Initialise address fields
                    memset(&SMSC_Address, 0x00, sizeof(ShortMessagePackedAddress_t));
                    memset(&DestinationAddress, 0x00, sizeof(ShortMessagePackedAddress_t));

                    // Extract SMSC Address
                    Env_p++; // address tag not used
                    SMSC_Address.Length = *Env_p++;
                    SMSC_Address.Length = SMS_MIN(SMSC_Address.Length, SMS_MAX_PACKED_ADDRESS_LENGTH + 1); // +1 for TON/NPI

                    if (SMSC_Address.Length) {
                        SMSC_Address.TypeOfAddress = *Env_p++;
                        memcpy(SMSC_Address.AddressValue, Env_p, SMSC_Address.Length - 1); // -1 for TON/NPI
                        Env_p += (SMSC_Address.Length - 1); // if this was adjusted by SMS_MIN the rest of the decoding will be garbage, -1 for TON/NPI
                        SMS_B_(SmUtil_HexPrint_Raw((uint8_t*)&SMSC_Address, SMSC_Address.Length + 1, "SMS: smmngr.c: MO SMS Control Adjusted SMSC Address")); // + 1 to include length
                    }

                    // Extract Destination Address
                    Env_p++; // address tag not used
                    DestinationAddress.Length = *Env_p++;
                    DestinationAddress.Length = SMS_MIN(DestinationAddress.Length, SMS_MAX_PACKED_ADDRESS_LENGTH + 1); // +1 for TON/NPI

                    if (DestinationAddress.Length) {
                        DestinationAddress.TypeOfAddress = *Env_p++;
                        memcpy(DestinationAddress.AddressValue, Env_p, DestinationAddress.Length - 1); // -1 for TON/NPI
                        SMS_B_(SmUtil_HexPrint_Raw((uint8_t*)&DestinationAddress, DestinationAddress.Length + 1, "SMS: smmngr.c: MO SMS Control Adjusted Destination Address")); // + 1 to include length

                        // Adjust Destination Address from number of bytes including TON/NPI to number of semi-octets excluding TON/NPI
                        DestinationAddress.Length--; // subtract 1 byte for TON/NPI
                        // Check last semi-octet for padding character
                        if (DestinationAddress.Length && (DestinationAddress.AddressValue[DestinationAddress.Length - 1] & 0xF0) == 0xF0) {
                            SemiOctetPadding = 1;
                        }
                        DestinationAddress.Length = (DestinationAddress.Length * 2) - SemiOctetPadding;
                    }

                    // Substitute SMSC Address if valid
                    if (SMSC_Address.Length) {
                        ShortMessage_SetPackedServiceCentreAddress(theShortMessageManager->MO_SMS_InProgress, &SMSC_Address);
                        SMS_B_(SMS_LOG_D("smmngr: SMS_ControlHandleReponse: Modified SMSC Address"));
                    }

                    // Substitute Destination Address if valid
                    if (DestinationAddress.Length) {
                        ShortMessage_SetPackedDestinationAddress(theShortMessageManager->MO_SMS_InProgress, &DestinationAddress);
                        SMS_B_(SMS_LOG_D("smmngr: SMS_ControlHandleReponse: Modified Destination Address"));
                    }

                    ShortMessageManager_SmInstanceSend(theShortMessageManager, theShortMessageManager->MO_SMS_InProgress, theShortMessageManager->MoreToSend, &theShortMessageManager->MessageSenderSignalInfo);
                }
                break;
            default: // MO_SMS_NOT_ALLOWED or unrecognised MO_SMS_Control
                {
                    error = TRUE;
                }
                break;
            }

            // Free envelope buffer memory
            SMS_HEAP_FREE(&cat_ec_response_p->apdu.buf);
            cat_ec_response_p->apdu.buf = NULL;
        } else { // Env_p == cat_ec_response_p->apdu.buf == NULL
            error = TRUE;
        }
    } else { // cat_ec_response_p == NULL
        error = TRUE;
    }

    if (error) {
        EventData_t eventData;

        // Inform client of error
        eventData.RMS_V2_ErrorCode = SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED;
        ShortMessageManager_ShortMessageSent(theShortMessageManager, &eventData);
    }
}


uint8_t ShortMessageManager_UCS2char_to_BCD (uint16_t UCS2_char)
{
    if ((UCS2_char >= '0') && (UCS2_char <= '9'))
    {
        return (uint8_t) (UCS2_char - 0x30);
    }
    switch(UCS2_char)
    {
        case '*' : return 0x0A;
        case '#' : return 0x0B;
        case 'C' : return 0x0C;
        case 'D' : return 0x0D;
        case 'E' : return 0x0E;

       default:   return (0x0F);
    }
}

int ShortMessageManager_ASCII8_to_BCD(ste_sim_text_t* source_text_p,
                                      uint8_t*        BCD_text_p,
                                      uint16_t        max_len,
                                      uint16_t*       actual_len_p)
{
    unsigned needed_size;
    uint8_t* src;
    uint8_t* end;

    if ((source_text_p == NULL)                        ||
        (source_text_p->text_p == NULL)                ||
        (source_text_p->text_coding != STE_SIM_ASCII8) ||
        (actual_len_p == NULL)                         ||
        (BCD_text_p == NULL)                            ) {
        return -1;
    }

    needed_size = ((unsigned)(source_text_p->no_of_characters + 1)) >> 1;
    if (max_len < needed_size) {
        return -1;
    }

    (*actual_len_p) = 0;

    src = source_text_p->text_p;
    end = src + source_text_p->no_of_characters;
    while (src < end)
    {
        uint8_t lsn, msn;
        lsn = ShortMessageManager_UCS2char_to_BCD((uint16_t)*src);
        src++;
        msn = (src < end) ? ShortMessageManager_UCS2char_to_BCD((uint16_t)*src) : 0x0F;

        *BCD_text_p = ((msn << 4) | lsn);
        BCD_text_p++;
        (*actual_len_p)++;

        src++;
    }
    return 0;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_Update_SMSC_Address
 *
 * @description  If the SMSC Address is not set update it with the
 *               active SMSC Address read from the USIM
 *
 * @param        SmInstance   : short message instance.
 * @param        SMSC_Address_p : SMSC address to check.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
void ShortMessageManager_Update_SMSC_Address(
    const ShortMessage_p SmInstance,
    ShortMessagePackedAddress_t* const SMSC_Address_p)
{
    // If no SMSC Address is passed from the client retrieve it from the USIM
    if (!SMSC_Address_p->Length) {
        ste_sim_t *uicc = MessagingSystem_UICC_SimSyncGet();
        ste_uicc_sim_smsc_get_active_response_t active_smsc_result = {
            STE_UICC_STATUS_CODE_FAIL,
            STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
            {0x00, 0x00},
            {{STE_SIM_INVALID_TEXT_CODING, 0, NULL}, 0, 0}
        };

        if (uicc) {
            uint8_t retval = ste_uicc_sim_smsc_get_active_sync (uicc,
                                                                0, // client tag
                                                                &active_smsc_result);

            if (retval == 0 && active_smsc_result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
                uint16_t smsc_length;

                if (!ShortMessageManager_ASCII8_to_BCD(&active_smsc_result.smsc.num_text, SMSC_Address_p->AddressValue, sizeof(SMSC_Address_p->AddressValue), &smsc_length)) {
                    SMSC_Address_p->Length = (uint8_t)smsc_length + 1; // +1 for TON/NPI
                    SMSC_Address_p->TypeOfAddress = (0x0f & active_smsc_result.smsc.npi) | ((0x07 & active_smsc_result.smsc.ton)<<4) | 0x80;

                    // Update the SMSC address in the SmInstance as well as the output paramter
                    ShortMessage_SetPackedServiceCentreAddress(SmInstance, SMSC_Address_p);

                    SMS_A_(SmUtil_HexPrint_Raw((uint8_t*)SMSC_Address_p, sizeof(ShortMessagePackedAddress_t), "SMS: smmngr.c: Active BCD SMSC read from USIM"));
                } else {
                    memset(SMSC_Address_p, 0x00, sizeof(ShortMessagePackedAddress_t)); // failed to set SMSC address so reset
                    SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_Update_SMSC_Address SMSC ascii to BCD failed"));
                }
                if (active_smsc_result.smsc.num_text.text_p != NULL) {
                    free(active_smsc_result.smsc.num_text.text_p);
                }
            } else {
                SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_Update_SMSC_Address ste_uicc_sim_smsc_get_active_sync uicc_status_code = %d", active_smsc_result.uicc_status_code));
            }
        }
    }
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_SendControl
 *
 * @description  Initiate MO SMS Control first by executing the Fixed
 *               Dialling Number check followed by requesting the
 *               Call Network Registraton Information required for
 *               the MO SMS Control by USIM SAT envelope.
 *
 * @param        SmInstance   : short message instance.
 * @param        SatInitiated : MO SMS send initiated by SAT.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
static SMS_Error_t ShortMessageManager_SMS_SendControl(
    const ShortMessage_p SmInstance,
    const uint8_t SatInitiated)
{
    SMS_Error_t Result = SMS_ERROR_INTERN_FIXED_DIALLING_NUMBER_RESTRICTED;
    ShortMessagePackedAddress_t SMSC_Address;
    ShortMessagePackedAddress_t DestinationAddress;
    cn_error_code_t cn_error_code = CN_UNKNOWN;

    ShortMessage_GetPackedServiceCentreAddress(SmInstance, &SMSC_Address);
    ShortMessage_GetPackedOrigOrDestAddress(SmInstance, &DestinationAddress);

    // Ensure SMSC Address is set
    ShortMessageManager_Update_SMSC_Address(SmInstance, &SMSC_Address);

    // FDN check should not be carried out if SMS send is SAT initiated
    if (!SatInitiated && SmUtil_UICC_ServiceEnabled(SIM_SERVICE_TYPE_FDN)) {

        // FDN is enabled so check SMSC Address and Destination address against FDN list
        if (SmUtil_FixedDiallingNumbersMatched(&SMSC_Address, &DestinationAddress)) {
            Result = SMS_ERROR_INTERN_NO_ERROR;
        }
    } else {
        // FDN disabled no check required
        Result = SMS_ERROR_INTERN_NO_ERROR;
    }

    if (Result == SMS_ERROR_INTERN_NO_ERROR && SmUtil_UICC_ServiceEnabled(SIM_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM)) {

        // Get the Call Network info asynchronously and send an evelope to the USIM in the response handler
        if ((cn_error_code = cn_request_cell_info(MessagingSystem_Get_CN_Context(), 0)) == CN_SUCCESS) {
            // Not rejected but used internally to indicate awaiting envelope

            Result = SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED;
        } else {
            EventData_t eventData;

            // Inform client of error
            eventData.RMS_V2_ErrorCode = SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED;
            ShortMessageManager_ShortMessageSent(theShortMessageManager, &eventData);

            SMS_A_(SMS_LOG_E("smmngr.c: cn_request_registration_state_normal: error %d", cn_error_code));
        }
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SmInstanceSend
 *
 * @description  Send an MO Short Message instance.
 *
 * @param        thisShortMessageManager : short message manager instance.
 * @param        SmInstance              : short message instance.
 * @param        MSG_MoreToSend          : indicates if there are more short messages
 *                                         to be sent on the same radio link
 * @param        senderSignalInfo_p      : sender signal info.
 *
 * @return       void
 */
/********************************************************************/
static void ShortMessageManager_SmInstanceSend(
    ShortMessageManager_p thisShortMessageManager,
    ShortMessage_p SmInstance,
    const uint8_t MSG_MoreToSend,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p)
 {
    uint8_t RefNo;

    // Handle the MoreToSend state
    ShortMessageManager_MoreToSend_StateHandler(thisShortMessageManager, MSG_MoreToSend, senderSignalInfo_p, SmInstance);

    // This will send the short message
    ShortMessage_Send(SmInstance, ShortMessageManager_SMS_Ack);

    // If we have moved to the STATE_CLOSING MoreToSendState then we have just sent
    // the last message and need to disable the RLC and reset the client blocking settings
    if (STATE_CLOSING == thisShortMessageManager->MoreToSendState) {
        ShortMessageManager_MoreToSend_StateHandler(thisShortMessageManager, MSG_MoreToSend, senderSignalInfo_p, SmInstance);
    } else if (STATE_DISABLED != thisShortMessageManager->MoreToSendState) {
        SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_SMS_Send: Restarting MoreToSend RLC timer, after SMS send"));
        // Stop the MoreToSend RLC timer and restart it
        Do_SMS_TimerReset_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG);
        Do_SMS_TimerSet_1_MS_WCT(MSG_SMS_MORETOSEND_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_MORETOSEND_TIMEOUT_VALUE * MSG_TIMER_S_INTO_MS);
    }
    // Store TP-MR in the message object to be used in the ack
    if (TRUE == ShortMessage_GetMessageReference(SmInstance, &RefNo)) {
        thisShortMessageManager->SM_ReferenceNumber = RefNo;
    }
    // The short message created earlier "ShortMessage_CreateForStorage" for the
    // SendNotStored scenario is no longer freed here.  As it has been passed to
    // SmBearer (via "ShortMessage_Send").  Therefore freeing it here will cause
    // heap corruption errors when we receive the NS ACK and attempt to set
    // certain parameters within the message.
    // Therefore the freeing of the (SendNotStored) short message will occur in
    // the ACK method (which is passed to "ShortMessage_Send" as a callback
    // "ShortMessageManager_SMS_Ack").  Within the ACK callback we determine a
    // SendNotStored short message as a message with no valid storage parameters
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_Send
 *
 * @description  Function for the short message manager to send a stored Sm
 *               with a given slotnumber or non-stored TPDU.
 *
 * @param        thisShortMessageManager : short message manager instance.
 * @param        MSG_SMS_TPDU_p          : as type of SMS_SMSC_Address_TPDU_t.
 * @param        MSG_SMS_Slot_p          : as type of SMS_Slot_t.
 * @param        MSG_MoreToSend          : indicates if there are more short messages
 *                                         to be sent on the same radio link
 * @param        satInitiated            : MO SMS send initiated by SAT.
 * @param        senderSignalInfo_p      : sender signal info.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SMS_Send(
    ShortMessageManager_p thisShortMessageManager,
    const SMS_SMSC_Address_TPDU_t * const MSG_SMS_TPDU_p,
    const SMS_Slot_t * const MSG_SMS_Slot_p,
    const uint8_t MSG_MoreToSend,
    const uint8_t satInitiated,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p)
{
    ShortMessage_p SmInstance = NULL;
    SMS_Error_t Result = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;

#ifdef REMOVE_SMSTRG_LAYER
    SMS_IDENTIFIER_NOT_USED(MSG_SMS_Slot_p);
    SMS_IDENTIFIER_NOT_USED(SlotNumber);
#endif                          // REMOVE_SMSTRG_LAYER
    SMS_A_(SMS_LOG_I("smmngr.c: ShortMessageManager_SMS_Send, MSG_MoreToSend = %d", MSG_MoreToSend));
    // Check if we are blocking other client send requests due to a previous send request
    // with MoreToSend set, in which case ignore this request
    if (ShortMessageManager_MoreToSend_IsBlocking(thisShortMessageManager, senderSignalInfo_p) || thisShortMessageManager->MessageSenderSignalInfo.ProcessId != SMS_NO_PROCESS) {
        Result = SMS_ERROR_INTERN_SERVER_BUSY;
        SMS_A_(SMS_LOG_E("smmngr.c: Rejecting SMS send request, current client (0x%08X) is blocking!", thisShortMessageManager->MessageSenderSignalInfo.ProcessId));
    }
    // Either we are currently not blocking or this is a subsequent request from the blocking client
    else {
#ifndef REMOVE_SMSTRG_LAYER
        // Specifying a storage of SMS_STORAGE_MT is meaningless
        //  (to perform a send all use find first, find next, find next, etc...)
        if (MSG_SMS_Slot_p->Storage != SMS_STORAGE_MT) {
            // obtain the slotnumber
            SlotNumber = SmStorage_MediaAndPositionToSlotAndVolume(MSG_SMS_Slot_p->Storage, MSG_SMS_Slot_p->Position, NULL);
            if (SlotNumber == SMS_STORAGE_POSITION_INVALID) {
                Result = SMS_ERROR_INTERN_MESSAGE_NOT_FOUND;
            }
        }

        if (SlotNumber != SMS_STORAGE_POSITION_INVALID) {
            // identify the SM that is in this slot
            SmInstance = SmStorage_SmIdentify(SlotNumber);
            if (SmInstance == NULL) {
                Result = SMS_ERROR_INTERN_MESSAGE_NOT_FOUND;
            }
        } else
#endif                          // REMOVE_SMSTRG_LAYER
        {
            if (MSG_SMS_TPDU_p->TPDU.Length > 0) {
                SmInstance = ShortMessage_CreateForStorage((SMS_SMSC_Address_TPDU_t *) MSG_SMS_TPDU_p);
            }
        }

        if (NULL != SmInstance) {
            // Assign the message sender signal info with the valid sender signal info
            thisShortMessageManager->MessageSenderSignalInfo = *senderSignalInfo_p;

            // Force the SM status to unsent so that previously sent SMs may be resent.
            (void) ShortMessage_SetStatus(SmInstance, SMS_STATUS_UNSENT);

            // Execute FDN and MO SMS Control by USIM
            Result = ShortMessageManager_SMS_SendControl(SmInstance, satInitiated);

            if (Result == SMS_ERROR_INTERN_NO_ERROR) {
                ShortMessageManager_SmInstanceSend(thisShortMessageManager, SmInstance, MSG_MoreToSend, &thisShortMessageManager->MessageSenderSignalInfo);
            } else if (Result == SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED) {
                // Either awaiting envelope response or client already informed of error
                Result = SMS_ERROR_INTERN_NO_ERROR;
                thisShortMessageManager->MO_SMS_InProgress = SmInstance;
                thisShortMessageManager->MoreToSend = MSG_MoreToSend;
                SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_SMS_Send MO SMS Control SmInstance = 0x%08x, MoreToSend = %d", (unsigned int)thisShortMessageManager->MO_SMS_InProgress, thisShortMessageManager->MoreToSend));
            } else {
                thisShortMessageManager->MessageSenderSignalInfo.ProcessId = SMS_NO_PROCESS;
            }
        }
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     ShortMessasgeManager_SMSAck
 *
 * @description  This function sends an ack back to the message manager to
 *               pass back to the caller as a result of an SMS send
 *
 * @param        theShortMessage  : The short message.
 * @param        RMS_V2_ErrorCode : RMS RP Error Cause.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_Ack(
    ShortMessage_p theShortMessage,
    const SMS_Error_t RMS_V2_ErrorCode)
{
    EventData_t eventData;

    eventData.ShortMessage = theShortMessage;
    eventData.RMS_V2_ErrorCode = RMS_V2_ErrorCode;

    SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_SMS_Ack"));

    if (RMS_V2_ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        //that part was sent so set status to sent
        (void) ShortMessage_SetStatus(theShortMessage, SMS_STATUS_SENT);

        // get the TP-MR so that it may be returned in the response
        (void) ShortMessage_GetMessageReference(theShortMessage, &theShortMessageManager->SM_ReferenceNumber);

#ifndef REMOVE_SMSTRG_LAYER
        // now that the message status is SENT we can set the delivery status to UNKNOWN until the network tells us otherwise
        if (ShortMessage_IsStatusReportRequested(theShortMessage) == TRUE) {
            ShortMessage_SetDeliveredStatus(theShortMessage, STATUS_REPORT_UNKNOWN);
        } else {
            ShortMessage_SetDeliveredStatus(theShortMessage, STATUS_REPORT_RESET);
        }
#endif                          // REMOVE_SMSTRG_LAYER
    }
    // Check if this was originally alloc'd in ShortMessageManager_SMS_Send as a
    // SendNotStored message
    if (SMS_STORAGE_POSITION_INVALID == ShortMessage_GetStoragePosition(theShortMessage)) {
        // Clean up memory after short message has been sent
        ShortMessage_DeletePart(theShortMessage, FALSE);
    }

    ShortMessageManager_HandleEvent(SHORTMESSAGEMANAGER_SHORT_MESSAGE_SENT, &eventData);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SendDeliverReport
 *
 * @description  Send a Deliver Report including the TPDU provided to
 *               Ack/Nack a Short Message.
 *
 * @param        thisShortMessageManager : As type of ShortMessageManager_p.
 * @param        RP_ErrorCause           : The RP error cause.
 * @param        TPDU_p                  : Deliver Report TPDU to send.
 * @param        senderSignalInfo_p      : sender signal info.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_SendDeliverReport(
    ShortMessageManager_p thisShortMessageManager,
    const SMS_RP_ErrorCause_t RP_ErrorCause,
    const SMS_TPDU_t * const deliverReport_TPDU_p,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p)
{
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
    ShortMessage_p DeliveryShortMessage;
    ShortMessageType_t ShortMessageType;
    uint8_t TpMtiBitfield;

    // The Process will be zero if no aysnchronous Deliver Report send is in progress.
    // While sending a DeliverReport aysnchronously the Process will not be SMS_NO_PROCESS hence it
    // will not be possible to send another DeliverReport.
    if (thisShortMessageManager->DeliverReportSenderSignalInfo.ProcessId == SMS_NO_PROCESS) {
        TpMtiBitfield = TPDU_GetBit(TP_MTI, (TPDU_p) deliverReport_TPDU_p->Data);

        // Convert the MTI into a short message type.  We've no idea if this is an
        // MS->SC or SC->MS message but we assume that it is MO and thus MS->SC
        // i.e. DELIVER-REPORT, SMS-COMMAND or SMS-SUBMIT
        ShortMessageType = SmUtil_ConvertMtiToSmMessageType(TpMtiBitfield, MStoSC);

        //if short message type is SM-COMMAND or SM-SUBMIT
        if (ShortMessageType == SM_DELIVER_REPORT_ERROR) {
            if (RP_ErrorCause == RMS_RP_NO_CAUSE) {
                ShortMessageType = SM_DELIVER_REPORT_ACK;
            } else {
#ifndef REMOVE_SMSTRG_LAYER
                ShortMessage_p ShortMessageToNack;

                // Identify the short message that is being NACKed and then delete it.
                ShortMessageToNack = SmStorage_SmIdentify(thisShortMessageManager->LastReceived_SMS_Position);

                if (ShortMessageToNack != NULL) {
                    SMS_A_(SMS_LOG_I("smmngr.c: ShortMessageManager_SendDeliverReport - deleted short message 0x%08x", (unsigned int)ShortMessageToNack));
                    ShortMessage_Delete(ShortMessageToNack);
                }
#endif                          // REMOVE_SMSTRG_LAYER

                ShortMessageType = SM_DELIVER_REPORT_ERROR;
            }

            // Create the delivery report Short Message.
            DeliveryShortMessage = ShortMessage_Create(ShortMessageType);

            if (DeliveryShortMessage == NULL) {
                // Failed to create delivery report.
                SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_SendDeliverReport: Failed to create delivery report"));

                ErrorCode = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
            } else {
                ShortMessage_SetTPDU(DeliveryShortMessage, (uint8_t *) deliverReport_TPDU_p->Data, deliverReport_TPDU_p->Length);

                ShortMessage_SetAcknowledgeRequired(DeliveryShortMessage, TRUE);

                if (NULL != senderSignalInfo_p) {
                    // Store the senderProcess to be used by the response.
                    theShortMessageManager->DeliverReportSenderSignalInfo = *senderSignalInfo_p;
                }

                ErrorCode = ShortMessage_AcknowledgementWithConfirm(DeliveryShortMessage, RP_ErrorCause);
            }
        } else {
            ErrorCode = SMS_ERROR_INTERN_INVALID_MESSAGE_TYPE;
            SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_SendDeliverReport - wrong short message type"));
        }

        // Clear the information on the last short message
        thisShortMessageManager->LastReceived_SMS_PID = 0;
        thisShortMessageManager->LastReceived_SMS_Position = SMS_STORAGE_POSITION_INVALID;
    } else {
        EventData_t EventData;

        // Already sending a Deliver Report so reject the request
        EventData.RMS_V2_ErrorCode = (SMS_RP_ErrorCause_t) SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED;

        SMS_A_(SMS_LOG_I("smmngr.c: ShortMessageManager_SendDeliverReport - RMS_RP_PROTOCOL_ERROR_UNSPECIFIED"));
        ShortMessageManager_DeliverReportConfirm(theShortMessageManager, &EventData);
    }

    if (ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        theShortMessageManager->DeliverReportSenderSignalInfo.ProcessId = SMS_NO_PROCESS;
    }

    return (ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_ShortMessageNack
 *
 * @description  Nack a Short Message.
 *
 * @param        thisShortMessageManager : as type of ShortMessageManager_p.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_ShortMessageNack(
    ShortMessageManager_p thisShortMessageManager)
{
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
    SMS_TPDU_t *MSG_TPDU_p = NULL;

    MSG_TPDU_p = (SMS_TPDU_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_TPDU_t));

    if (MSG_TPDU_p == NULL) {
        ErrorCode = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Initialise the Deliver Report TPDU
        memset(MSG_TPDU_p->Data, 0x00, sizeof(MSG_TPDU_p->Data));

        MSG_TPDU_p->Length = TPDU_DELIVER_REPORT_PackHdr(MSG_TPDU_p->Data, TP_UNSPECIFIED_ERROR_CAUSE,  // TP_FCS (Failure Cause)
                                                         &thisShortMessageManager->LastReceived_SMS_PID,        // TP_PID (Protocol Identifier)
                                                         NULL); // TP_DCS (Data Coding Schems)

        // Send the Deliver Report
        ErrorCode = ShortMessageManager_SendDeliverReport(thisShortMessageManager, SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED, MSG_TPDU_p, NULL);

        SMS_HEAP_FREE(&MSG_TPDU_p);
    }

    return (ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_MasterReset
 *
 * @description  Master Reset
 *                 1. Reset Message Waiting Indicator
 *                 2. Reset MO and MT Counter
 *                 3. Clear Storage other than SIM and Card
 *
 * @param        thisShortMessageManager : as of type ShortMessageManager_p.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_MasterReset(
    ShortMessageManager_p thisShortMessageManager)
{
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_NO_ERROR;

    SMS_IDENTIFIER_NOT_USED(thisShortMessageManager);
#ifndef REMOVE_SMSTRG_LAYER
    {
        MSG_StorageMedia_t *CurrentVolume_p;
        uint8_t Loop;
        MSG_StoreList_t *MSG_StorageList_p = (MSG_StoreList_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StoreList_t));

        //Delete messages from storages other than Sim and Card
        if (SmStorage_GetStorageList(MSG_StorageList_p)) {
            CurrentVolume_p = MSG_StorageList_p->MSGStorageList_p;

            for (Loop = 0; Loop < MSG_StorageList_p->NumStorageMedia; Loop++) {
#ifdef LINUX_PORTING_REQUIRED
                if ((strcmp(CurrentVolume_p->StorageMediaName, SIM_VOL) != 0) && (strcmp(CurrentVolume_p->StorageMediaName, FS_DIR_CARD) != 0))
#else
                SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_MasterReset: ***** LINUX PORTING REQUIRED *****"));
                if ((strcmp(CurrentVolume_p->StorageMediaName, SIM_VOL) != 0) && (strcmp(CurrentVolume_p->StorageMediaName, "~/") != 0))        // Using top of FS for current user. Not good!
#endif
                {
                    ErrorCode = ShortMessageManager_StorageClear(SMS_STORAGE_ME, TRUE, TRUE);
                }
                CurrentVolume_p++;
            }

            if (NULL != MSG_StorageList_p->MSGStorageList_p) {
                //free memory allocated in SmStorage_GetStorageList
                SMS_HEAP_FREE(&MSG_StorageList_p->MSGStorageList_p);
            }
        } else {
            ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
            SMS_A_(SMS_LOG_E("smmngr.c : ShortMessageManager_MasterReset: GetStorageList: Failed"));
        }
        SMS_HEAP_FREE(&MSG_StorageList_p);
    }
#endif                          // REMOVE_SMSTRG_LAYER

    return ErrorCode;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_RelayLinkControl_Set
 *
 * @description  This function sends the relay link control request to
 *               SM bearer to control what short message relay does with
 *               the radio link when a short message has been sent.  i.e.
 *               to control whether the radio link should stay open after
 *               a short message is sent.  This is to support the +CMMS
 *               AT Command request.
 *
 * @param        RelayLinkControl : SMS Relay Link Control setting
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_RelayLinkControl_Set(
    const SMS_RelayControl_t RelayLinkControl)
{
    ShortMessageBearer_RelayLinkControl_Set(RelayLinkControl);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_SMS_RelayLinkControl_Get
 *
 * @description  Gets the relay link control setting from SM bearer
 *
 * @param        RelayLinkControl : SMS Relay Link Control setting
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_SMS_RelayLinkControl_Get(
    SMS_RelayControl_t * const RelayLinkControl_p)
{
    ShortMessageBearer_RelayLinkControl_Get(RelayLinkControl_p);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_AddPortSubscription
 *
 * @description  Add an application port subscription
 *
 * @param        SenderProcess        : Process ID.
 * @param        ClientTag            : Client Tag.
 * @param        ApplicationPortRange : Port type and range.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_AddPortSubscription(
    const SMS_PROCESS SenderProcess,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    if (ApplicationPortRange_p != NULL) {
        MSG_ErrorCode = ShortMessageRouter_Port_Add(SenderProcess, ClientTag, ApplicationPortRange_p);
    }

    return (MSG_ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_DeletePortSubscription
 *
 * @description  Delete an application port subscription
 *
 * @param        SenderProcess        : Process ID.
 * @param        ClientTag            : Client Tag.
 * @param        ApplicationPortRange : Port type and range.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_DeletePortSubscription(
    const SMS_PROCESS SenderProcess,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p)
{
    SMS_Error_t MSG_ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

    if (ApplicationPortRange_p != NULL) {
        MSG_ErrorCode = ShortMessageRouter_Port_Delete(SenderProcess, ClientTag, ApplicationPortRange_p);
    }

    return (MSG_ErrorCode);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_Send_MemCapacityAvailable
 *
 * @description  Send Memory Capacity Available.
 *
 * @param        thisShortMessageManager : short message manager instance.
 * @param        eventData               : event data.
 * @param        senderSignalInfo_p      : sender signal info.
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t ShortMessageManager_Send_MemCapacityAvailable(
    ShortMessageManager_p thisShortMessageManager,
    EventData_p eventData,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p)
{
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_SERVER_BUSY;

    // The Client has indicated that memory is available so the
    // flag must be updated even if it is not possible to send
    // Memory Capacity Available. It could be that client memory
    // status has toggled with no MT SMS having triggered informing
    // the network of Memory Capacity Exceeded.
    thisShortMessageManager->ClientMemoryCapacityFull = FALSE;

    // The Process will be zero if no sending is in progress.
    // While sending a message the Process will not be zero hence it will not be possible to send
    // memory capacity available until Process is set to zero in SendingComplete
    if (thisShortMessageManager->MessageSenderSignalInfo.ProcessId == SMS_NO_PROCESS) {
        uint8_t MemoryCapacityEx = 0;

        if (!SmUtil_Get_MSG_MemoryCapacityExceededFlag(&MemoryCapacityEx)) {
            SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_Send_MemCapacityAvailable: Get_MSG_MemoryCapacityExceededFlag failed"));
            MemoryCapacityEx = TRUE;
        }

        if (MemoryCapacityEx == TRUE) {
            // Assign the message sender signal info with the valid sender signal info
            thisShortMessageManager->MessageSenderSignalInfo = *senderSignalInfo_p;
            SMS_B_(SMS_LOG_D("smmngr.c: ShortMessageManager_Send_MemCapacityAvailable: Network Memory Capacity Available"));
            // Only send RP-SMMA if Memory Capacity Exceeded has previuosly been sent to the network
            ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, eventData);
            ErrorCode = SMS_ERROR_INTERN_NO_ERROR;
        } else {
            // Memory Capacity Exceeded Flag was already cleared.
            // Do nothing.
            ErrorCode = SMS_ERROR_INTERN_NO_ERROR_NO_ACTION;
        }
    }

    return ErrorCode;
}

/********************************************************************/
/**
 *
 * @function     ShortMessageManager_Send_MemCapacityGetState
 *
 * @description  Get the current state of MemCapacityFULL flag.
 *
 * @return       current state of memory (1 == Full)
 */
/********************************************************************/
uint8_t ShortMessageManager_Send_MemCapacityGetState()
{
    return theShortMessageManager->ClientMemoryCapacityFull;
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_MoreToSendTimeout
 *
 * @description  Handle the event of SHORTMESSAGEMANAGER_MORETOSEND_TIMEOUT
 *
 * @param        *thisObject : void type.
 * @param        eventData :   as type of EventData_p.
 *
 * @return       static void
 */
/********************************************************************/
static void ShortMessageManager_MoreToSendTimeout(
    void *thisObject,
    EventData_p eventData)
{
    ShortMessageManager_p thisShortMessageManager = (ShortMessageManager_p) thisObject;

    SMS_IDENTIFIER_NOT_USED(eventData);

    SMS_A_(SMS_LOG_E("smmngr.c: ShortMessageManager_MoreToSendTimeout: Timeout expired closing link STATE %d -> %d", thisShortMessageManager->MoreToSendState, STATE_CLOSING));
    // Since we have timed out force the state
    thisShortMessageManager->MoreToSendState = STATE_CLOSING;
    // Call the MoreToSend state handler to reset RLC
    ShortMessageManager_MoreToSend_StateHandler(thisShortMessageManager, FALSE, NULL, NULL);
}


/********************************************************************/
/**
 *
 * @function     ShortMessageManager_HandleEvent
 *
 * @description  Handle the event of SHORTMESSAGEMANAGER_ATSHORT_MESSAGE_SENT
 *
 * @param        ShortMessageManagerEvent : event type.
 * @param        eventData :                as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void ShortMessageManager_HandleEvent(
    const ShortMessageManagerEvent_t ShortMessageManagerEvent,
    EventData_p EventData)
{
    switch (ShortMessageManagerEvent) {
    case SHORTMESSAGEMANAGER_SHORT_MESSAGE_SENT:
        ShortMessageManager_ShortMessageSent(theShortMessageManager, EventData);
        break;
    case SHORTMESSAGEMANAGER_MEMCAPACITYAVAILABLE_SENT:
        ShortMessageManager_MemCapacityAvailableSent(theShortMessageManager, EventData);
        break;
    case SHORTMESSAGEMANAGER_DELIVERREPORT_CONFIRM:
        ShortMessageManager_DeliverReportConfirm(theShortMessageManager, EventData);
        break;
    case SHORTMESSAGEMANAGER_MORETOSEND_TIMEOUT:
        ShortMessageManager_MoreToSendTimeout(theShortMessageManager, EventData);
        break;
    case SHORTMESSAGEMANAGER_RECEIVE_NEW_STATUS_REPORT:
        ShortMessageManager_ReceiveNewStatusReport(theShortMessageManager, EventData);
        break;
    case SHORTMESSAGEMANAGER_PROCESS_RECEIVED_STATUS_REPORTS:
        ShortMessageManager_HandleStatusReport(theShortMessageManager, EventData);
        break;
    default:
        break;
    }
}
