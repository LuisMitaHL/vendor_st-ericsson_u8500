#ifndef SMSTRG_H
#define SMSTRG_H
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
 *  Defines all the external types and function prototypes for the
 *  short message object.
 *
 *************************************************************************/

#include "sm.h"

#define SMS_SIM_SIZE (176)      /* Length of a Short Message */


/*
 * MASK Values for use in Search Functionality
 */
#define STATUSMASK_UNKNOWN 0x00
#define STATUSMASK_FREE    0x01
#define STATUSMASK_READ    0x02
#define STATUSMASK_UNREAD  0x04
#define STATUSMASK_SENT    0x08
#define STATUSMASK_UNSENT  0x10

typedef enum {
    SMSTORAGEEVENT_TIMEOUT,
    SMSTORAGEEVENT_STATUSUPDATE
} SmStorageEvent_t;

typedef enum {
    SMSTORAGETEMPVOLUME_SMCONTROL = 0,
    SMSTORAGETEMPVOLUME_INVALID
} SmStorageTempVolume_t;


/*
 * EFSMS RECORD TYPE DEFINITIONS
 *
 *   + EfSmsRecord: Storage Format of a Short message within the Storage media
 *   + EfSmsRecord Status: Reference GSM11.11
 *   + EfSmsRecordFormat: Generic Data Structure encapsulating a EfSmsRecord
 *                        for use within SmStorage
 */
#define EFSMS_RECORD_SIZE   SMS_SIM_SIZE
typedef uint8_t EfSmsRecord_t[EFSMS_RECORD_SIZE];
typedef uint8_t EfSmsRecordStatus_t;

// Note that some FS paths are defined in smutil.h. This is because they are also required for the persistent
// data storage in the FS.
extern const char SM_SUB_DIR[];
extern const char SM_STATUS_REPORT_SUB_DIR[];
extern const char SMS_FS_PATH_SIM_CACHE[];
extern const char SMS_FS_DIR_SIM_CACHE[];

#define SMS_FS_DIR_USE_SIM_CACHE   SMS_FS_DIR_SIM_CACHE // Used to indicate that the path used is to map to the SIM CACHE.


#define SM_CONTROL_DIR  "/control"
#define SIM_VOL         "/sim"

#define MID_FILENAME_SIZE         20


/**
 * MSG_StoreList_t describes the list of media types used to store a message
 *
 * @param NumStorageMedia  Indicates how many entries are in the store list
 * @param MSGStorageList_p A pointer to an array of storage volume names
 */
typedef struct {
    uint8_t NumStorageMedia;
    MSG_StorageMedia_t *MSGStorageList_p;
} MSG_StoreList_t;

void SmStorage_Clear_FS_Working_Dir(
    void);

void SmStorage_Initialise(
    void);

void SmStorage_Deinitialise(
    void);

// Used to derive FS paths for messages
void SmStorage_GetSMPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p);
void SmStorage_GetDeliveryStatusPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p);
SMS_Position_t SmStorage_GetSMPendingPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p);
void SmStorage_GetSMControlPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p);


uint8_t SmStorage_GetStorageList(
    MSG_StoreList_t * MSG_StoreList_p);
uint8_t SmStorage_SetStorageList(
    const MSG_StoreList_t * const MSG_StoreList_p);
uint16_t SmStorage_GetAvailableMECapacity(
    void);

void SmStorage_ClearDir(
    char *Directory_Buffer_p);

void SmStorage_DeleteAllThisICCID_Messages(
    MSG_StorageMedia_t * StorageVolume_p);
void SmStorage_DeleteAllOtherICCID_Messages(
    MSG_StorageMedia_t * StorageVolume_p);

uint8_t SmStorage_FindSIMCapacity(
    SMS_StorageCapacity_t * MessageStorageCapacity_p);

// Sm Storage Management functions
void SmStorage_EventHandler(
    SmStorageEvent_t);
void SmStorage_ReinitialiseStorage(
    void);
SMS_Storage_t SmStorage_SlotType(
    SMS_Position_t);
ShortMessage_p SmStorage_GetNextMessage(
    ShortMessage_p thisShortMessage);
ShortMessage_p SmStorage_GetPreviousMessage(
    ShortMessage_p thisShortMessage);
ShortMessage_p SmStorage_SmIdentify(
    const SMS_Position_t Position);
void SmStorage_SlotStatus(
    SMS_Storage_t const Storage,
    SMS_SlotInformation_t * const SmStorageInfo_p);
void SmStorage_StatusGet(
    SMS_StorageStatus_t * StorageStatus_p);
void SmStorage_GetMessageTimestamp(
    ShortMessage_p SmInstance,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p);

void SmStorage_StartSimRefresh(
    void);
void SmStorage_StartSimRestart(
    void);


// Short message functions
uint8_t SmStorage_SmStatusUpdate(
    ShortMessage_p const,
    SMS_Position_t const,
    SMS_Status_t const);
uint8_t SmStorage_SmUpdate(
    ShortMessage_p const,
    SMS_Position_t const);
uint8_t SmStorage_SmRead(
    SMS_Position_t const,
    ShortMessagePackedAddress_t * const,
    uint8_t * const,
    uint8_t * const);
uint8_t SmStorage_SmWrite(
    ShortMessage_p const,
    SMS_Position_t const,
    ShortMessagePackedAddress_t * const,
    uint8_t,
    uint8_t * const);
uint8_t SmStorage_SmDelete(
    SMS_Position_t const SlotNumber,
    const uint8_t);
SmStoreErrorCode_t SmStorage_ReplaceShortMessage(
    ShortMessage_p OldShortMessage,
    ShortMessage_p NewShortMesasge);

// Delivery status functions
uint8_t SmStorage_WriteDeliveryStatus(
    const SMS_Position_t SlotNumber,
    const DeliveryStatus_t StatusReport);
uint8_t SmStorage_ReadDeliveryStatus(
    const SMS_Position_t SlotNumber,
    DeliveryStatus_t * const DeliveryStatus_p);

uint8_t SmStorage_WriteStatusReportTPDU(
    const SMS_Position_t SlotNumber,
    SMS_Position_t * const fSRSlotNo,
    const SMS_StatusReport_t * StatusReport_p);
uint8_t SmStorage_ReadStatusReportTPDU(
    const SMS_Position_t SlotNumber,
    SMS_StatusReport_t * const StatusReport_p);

SmStoreErrorCode_t SmStorage_WriteStatusReportSM(
    const ShortMessage_p SmInstance,
    const MSG_StorageMedia_t * StorageVolume_p);
uint8_t SmStorage_RestoreStatusReportSM(
    ShortMessage_p * SmInstance_p,
    const MSG_StorageMedia_t * StorageVolume_p);

// Volume based interface
uint8_t SmStorage_GetVolumeIndex(
    const MSG_StorageMedia_t * const Volume_p,
    uint8_t * const VolumeIndex_p);
uint8_t SmStorage_GetVolume(
    const uint8_t VolumeIndex,
    MSG_StorageMedia_t * const Volume_p);
uint8_t SmStorage_GetAvailableVolume(
    MSG_StorageMedia_t * AvailableMedia_p,
    uint8_t RequiredSize,
    uint8_t MEOnly);
uint8_t SmStorage_FindSpace(
    const MSG_StorageMedia_t * const Volume_p,
    const uint8_t RequiredSize,
    const uint16_t MaximumSize,
    uint16_t * const AvailableSize_p);

uint8_t SmStorage_GetStorageVolume(
    const SMS_Position_t,
    MSG_StorageMedia_t * const);
SMS_Position_t SmStorage_SmSaveInVolume(
    const ShortMessage_p SmInstance,
    const MSG_StorageMedia_t * StorageVolume_p,
    SmStoreErrorCode_t * const ErrorCode_p);

SMS_Position_t SmStorage_MediaAndPositionToSlotAndVolume(
    const SMS_Storage_t Storage,
    const SMS_Position_t Position,
    MSG_StorageMedia_t * SmVolume_p);

uint8_t SmStorage_GetSmStorageDetails(
    const SMS_Position_t SlotNumber,
    char *SmPathFilename_p,
    uint16_t * SmFileIndex_p);

void SmStorage_SlotToMediaAndPosition(
    const SMS_Position_t,
    SMS_Storage_t *,
    SMS_Position_t *);
SmStoreErrorCode_t SmStorage_SmSaveSlotNumber(
    const ShortMessage_p SmInstance,
    const SMS_Position_t SlotNumber,
    const MSG_StorageMedia_t * StorageVolume_p);
SMS_Position_t SmStorage_SlotSearch(
    const SMS_Storage_t searchMedia,
    const MSG_StorageMedia_t * Volume_p);
uint8_t SmStorage_SlotPositionIsTemporary(
    const SMS_Position_t StoragePosition);
void SmStorage_SlotRangeGet(
    const SMS_Storage_t StorageMedia,
    const SmStorageTempVolume_t TempVolume,
    SMS_Position_t * const FirstSlot_p,
    SMS_Position_t * const LastSlot_p);

void SmStorage_VolumeStatusChanged(
    const MSG_StorageMedia_t * const MSG_Media_p,
    const SMS_FSU_VolumeStatusInformation_t FSU_VolumeStatusInformation);       //Linux porting

void SmStorage_DeleteAllSIMShortMessages(
    void);
uint8_t SmStorage_SIMSlotSimUsedSpaceRd(
    void);
uint8_t SmStorage_SIMSlotSimTotalSpaceRd(
    void);
uint16_t SmStorage_MEUsedSpaceRd(
    void);

void SmStorage_PackTimestamp(
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p,
    uint8_t * timeStamp_p);
uint8_t SmStorage_MoveStoredShortMessage(
    ShortMessage_p theShortMessage,
    MSG_StorageMedia_t * Volume_p);

uint8_t SmStorage_ReadMemoryCapacityExceeded(
    uint8_t * const MemCapacityExceeded_p);


#endif                          // SMSTRG_H
