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
 *  The Short Message Storage Object and its methods.
 *
 *************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#ifdef PLATFORM_ANDROID
#include <sys/statfs.h>
#include <sys/stat.h>
#else
#include <sys/statvfs.h>
#endif

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"

/* The Message Transport Server Interface */
#include "g_sms.h"
#include "r_sms.h"
#include "d_sms.h"

/* Message Module Include Files */
#include "sm.h"
#include "smsevnthndr.h"
#include "smstrg.h"
#include "smbearer.h"
#include "smrouter.h"
#include "smtpdu.h"
#include "smutil.h"
#include "smslist.h"
#include "smsmain.h"

/* Configuration */
#include "c_sms_config.h"

#include "sim.h"




/*
 * SMSTORAGE OBJECT: DATA CONFIGURATION DEFINITIONS
 *
 * NOTES:
 * #1: Pending User Option support defined by product Control Board
 */

/*
 * BIT Definitions
 */
#define BIT0_MASK (0x01)
#define BIT1_MASK (0x02)
#define BIT2_MASK (0x04)
#define BIT3_MASK (0x08)
#define BIT4_MASK (0x10)
#define BIT5_MASK (0x20)
#define BIT6_MASK (0x40)
#define BIT7_MASK (0x80)

/*
 **********************************************************************************************
 * Partitioned Linear Slot Range:
 *
 * The following illustrates the available slot allocation per message storage area.
 * (Note that this example assumes 20 slots for Class0, Temp MO and SM Control. The actual
 * value is determined by SMS_MAX_SHORT_MESSAGE_PARTS.)
 *
 * |<--1000 slots--->|<--10000 slots-->|<----20 slots--->|<----20 slots--->|<----20 slots--->|
 * |<---SM RANGE---->|<---ME RANGE---->|<-CLASS 0 RANGE->|<-TEMP MO RANGE->|<---SM CONTROL-->|
 * | 0x0001 - 0x03E8 | 0x03E9 - 0x2AF8 | 0x2AF9 - 0x2B0C | 0x2B0D - 0x2B20 | 0x2B21 - 0x2B34 |
 * |<-------------MT RANGE------------>|<--------------------TEMP RANGE--------------------->|
 * |<-----------11000 slots----------->|<---------------------60 slots---------------------->|
 * |<------------------------------TOTAL ALLOCATED RANGE------------------------------------>|
 * |<-----------------------------------11060 slots----------------------------------------->|
 *
 * MAX       = 65534 (LAST_SLOT, 0xFFFE)
 * ALLOCATED = 11060
 * FREE      = 54474
 **********************************************************************************************
 */
#define FIRST_SLOT           (0x0001)   // First slot number     total = 65534 slots
#define LAST_SLOT            (0xFFFE)   // Last slot number

#define SMSTORAGE_MAX_SM_SLOTS (1000)
#define SMSTORAGE_MAX_ME_SLOTS (10000)

#define SMS_MAX_SHORT_MESSAGE_PARTS (255)

// Note that the numbers of slots is based on arbitary values which
// can, and do, change in "real" builds; e.g. number of ME slots.
#define FIRST_SLOT_SIM       (FIRST_SLOT)       // First SIM slot        0x0001(00001) ( 1000 slots, arbitrary value SMSTORAGE_MAX_SM_SLOTS)
#define FIRST_SLOT_ME        (FIRST_SLOT + SMSTORAGE_MAX_SM_SLOTS)      // First ME slot         0x03E9(01001) (10000 slots, arbitrary value SMSTORAGE_MAX_ME_SLOTS)
#define FIRST_SLOT_TEMP      (FIRST_SLOT_ME + SMSTORAGE_MAX_ME_SLOTS)   // First TEMP slot       0x2AF9(11001) (   60 slots, based on [2 x SMS_MAX_SHORT_MESSAGE_PARTS] + SMS_CONTROL_DIR_MAX_SIZE)
#define FIRST_SLOT_CLASS0    (FIRST_SLOT_TEMP)  // First CLASS 0 slot    0x2AF9(11001) (   20 slots, based on SMS_MAX_SHORT_MESSAGE_PARTS)
#define FIRST_SLOT_TEMPMO    (FIRST_SLOT_CLASS0 + SMS_MAX_SHORT_MESSAGE_PARTS)  // First TEMP MO slot    0x2B0D(11021) (   20 slots, based on SMS_MAX_SHORT_MESSAGE_PARTS)
#define FIRST_SLOT_SMCONTROL (FIRST_SLOT_TEMPMO + SMS_MAX_SHORT_MESSAGE_PARTS)  // First SM CONTROL slot 0x2B21(11041) (   20 slots, based on SMS_CONTROL_DIR_MAX_SIZE)

#define LAST_SLOT_SIM        (FIRST_SLOT_ME - 1)        // Last SIM slot         0x03E8(01000)
#define LAST_SLOT_ME         (FIRST_SLOT_TEMP - 1)      // Last ME slot          0x2AF8(11000)
#define LAST_SLOT_TEMP       (FIRST_SLOT_SMCONTROL + SMS_CONTROL_DIR_MAX_SIZE - 1)      // Last TEMP slot        0x2B34(11060)
#define LAST_SLOT_CLASS0     (FIRST_SLOT_TEMPMO - 1)    // Last CLASS 0 slot     0x2B0C(11020)
#define LAST_SLOT_TEMPMO     (FIRST_SLOT_SMCONTROL - 1) // Last TEMP MO slot     0x2B20(11040)
#define LAST_SLOT_SMCONTROL  (LAST_SLOT_TEMP)   // Last SM CONTROL slot  0x2B34(11060)

#define SMS_MAX_STORAGE_ME_CAPACITY (FIRST_SLOT_TEMP - FIRST_SLOT_ME)

/*
 * EfSmsRecord: EfSmsRecord Status Masks
 */
#define EFSMS_RECORD_STATUS_FREE_MASK   (0x00)
#define EFSMS_RECORD_STATUS_UNREAD_MASK (BIT1_MASK | BIT0_MASK)
#define EFSMS_RECORD_STATUS_READ_MASK   (BIT0_MASK)
#define EFSMS_RECORD_STATUS_SENT_MASK   (BIT2_MASK | BIT0_MASK)
#define EFSMS_RECORD_STATUS_UNSENT_MASK (BIT2_MASK | BIT1_MASK | BIT0_MASK)

/*
 * EfSmsrRecord: EfSmsrRecord defines
 */
#define EFSMSR_RECORD_STATUS_FREE_MASK   (0x00)
#define SMSR_TPDU_OFFSET                 (1)
#define STATUS_REPORT_REC_SIZE           (30)   //size of a record in ef_smsr

/*
 * EfSmsRecord: EfSmsRecord Status Group Masks
 */
#define EFSMS_RECORD_STATUS_MASK        (BIT2_MASK | BIT1_MASK | BIT0_MASK)
#define EFSMS_RECORD_STATUS_TPSR_MASK   (BIT4_MASK | BIT3_MASK)

/*
 * EfSmssFile: EfSmssFile defines
 */
#define EFSMSS_FILE_READ_OFFSET               (0)
#define EFSMSS_FILE_MEM_CAPACITY_EXC_OFFSET   (1)
#define EFSMSS_FILE_MIN_LENGTH                (2)       // Memory capacity available is in 2nd octet. Only first 2 octets are mandatory.
#define EFSMSS_FILE_MEM_CAPACITY_EXC_MASK     (BIT0_MASK)

// DCS Values identifying a class2 8Bit message
#define DCS_GENERAL_CODING_CLASS2_8BIT  (0x16)
#define DCS_CLASS2_8BIT                 (0xf6)

/*
 * SMSTORAGE OBJECT DATA MACRO DEFINITIONS
 */
#define ShortMessageStorage_WriteState(State)           ShortMessageStorage.ShortMessageStorageState = State
#define ShortMessageStorage_ReadState()                 ShortMessageStorage.ShortMessageStorageState
#define SmStorage_SIMSlotLogicalCapacityWr(SmCapacity)  ShortMessageStorage.SlotLogicalSimCapacity = SmCapacity
#define SmStorage_SIMSlotLogicalCapacityRd()            ShortMessageStorage.SlotLogicalSimCapacity
#define SmStorage_SIMSlotVerifiedCapacityWr(SmCapacity) ShortMessageStorage.SlotVerifiedSimCapacity = SmCapacity
#define SmStorage_SIMSlotVerifiedCapacityRd()           ShortMessageStorage.SlotVerifiedSimCapacity
#define SmStorage_SIMSlotSimUsedSpaceInc()              ShortMessageStorage.SimUsedSpace++
#define SmStorage_SIMSlotSimUsedSpaceDec()              ShortMessageStorage.SimUsedSpace--

#define SmStorage_SIM_SR_LogicalCapacityWr(SrCapacity)  ShortMessageStorage.SRLogicalSimCapacity = SrCapacity
#define SmStorage_SIM_SR_LogicalCapacityRd()            ShortMessageStorage.SRLogicalSimCapacity
#define SmStorage_SIM_SR_VerifiedCapacityWr(SrCapacity) ShortMessageStorage.SRVerifiedSimCapacity = SrCapacity
#define SmStorage_SIM_SR_VerifiedCapacityRd()           ShortMessageStorage.SRVerifiedSimCapacity

/*
 * EFSMS RECORD MANIPULATION UTILITY MACRO
 */
#define SmStorage_EfSmsRecordScAddress(EfSmsRecord) ((ShortMessagePackedAddress_t*)&(EfSmsRecord)[1])

#define MAX_SM_FILENAME         (14)    // /sm_SSSSS.sms where SSSSS is the slot number + Null terminator
#define STORAGE_INDEX_OFFSET    (4)     // offset to the start of SSSSS in the filename above
#define MAX_SM_FILENAME_BYTES   (sizeof(char) * MAX_SM_FILENAME)
#define INDEX_FILENAME_SIZE     (20)    // /sm_store_index.txt 20 character including Null terminator
#define MAX_SM_PATHNAME         (160)   // Currently the longest path under a mount point (eg /ifs is)
                                // /mts_data/11223344556677889900/sm/sm_delivery_status which is 52 characters
#define SM_FULL_FILENAME_BYTES  (sizeof(char) * (SMS_STORAGE_MEDIA_LENGTH + MAX_SM_PATHNAME + MAX_SM_FILENAME))

#define SHORT_MESSAGE_FILE_ENTRY_LENGTH (EFSMS_RECORD_SIZE + sizeof(struct stat))
#define STATUS_REPORT_FILE_ENTRY_LENGTH (sizeof(DeliveryStatus_t) + sizeof(SMS_StatusReport_t))

#define SM_FILE_SUFFIX_LEN ((sizeof SmFileSuffix  / sizeof SmFileSuffix[0])  - 1)
#define SM_FILE_PREFIX_LEN ((sizeof SmFile_Prefix / sizeof SmFile_Prefix[0]) - 1)
#define SR_FILE_SUFFIX_LEN ((sizeof SrFileSuffix  / sizeof SrFileSuffix[0])  - 1)

#define EF_SMS_FILENAME  0x6F3C
#define EF_SMSR_FILENAME 0x6F47
#define EF_SMSS_FILENAME 0x6F43
#define EF_SMS_FILEPATH  NULL

#define SMSTORAGE_FILEINDEX_INVALID (0xFFFF)

#ifndef CFG_MSG_FSU_BLOCK_OVERHEAD
#define CFG_MSG_FSU_BLOCK_OVERHEAD 10
#endif



#ifndef REMOVE_SMSTRG_LAYER

/*
 * TYPEDEF
 */

/*
 * STORAGE TYPE DEFINITIONS
 *   + Storage Media Status: Management Data for SmStorage Object
 *   + Storage State       : State of SmStorage Object
 *   + Storage Information : Information detailing the status of the SmStorage
 *                           Object
 *   + Storage Status      : Storage Status Full/Not Full
 *                           NOTE: Media Storage Status due to Legacy
 */

typedef enum {
    SMSTORAGESTATE_VALIDATE_SM,
    SMSTORAGESTATE_INITIALISE_COMPLETE,
    SMSTORAGESTATE_SIM_REFRESH, // SAT Refresh
    SMSTORAGESTATE_SIM_RESTART, // SIM Power Off
    SMSTORAGESTATE_LAST
} ShortMessageStorageState_t;

typedef enum {
    RESTORE_NOT_STARTED,
    RESTORING_SM_DIR,
    INVALID_RESTORE_STATE
} RestoreSequence_t;

typedef enum {
    SMSTORAGECONTAINER_SM = 0,  // SMS container
    SMSTORAGECONTAINER_SR,      // Status Report container

    SMSTORAGECONTAINER_INVALID
} SmStorageContainer_t;

typedef struct {
    ShortMessage_p ShortMessage;
    uint16_t FileIndex;
    uint8_t StorageIndex;
    uint8_t PendingFlag;
} SmStorageObject_t;

typedef MSG_List *SmStorageMSG_List_p;

typedef struct ShortMessageStorageObject {
    ShortMessageStorageState_t ShortMessageStorageState;
    uint8_t StorageListRetrieved;
    uint8_t SlotLogicalSimCapacity;
    uint8_t SlotVerifiedSimCapacity;
    uint8_t SimUsedSpace;
    uint16_t MEUsedSpace;       // This is the number of Messages stored on the ME.
    // This is a cache of the persistent value SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT

    uint16_t MEInitialisationCount;     // At initialisation we count the number of messages actually
    // stored on the ME, as they are read back. This count is
    // then checked to make sure that it is equal to MEUsedSpace.
    SmStorageMSG_List_p SmStorageMSG_List;
    MSG_StorageMedia_t ChangedStatusVolume;
    uint8_t SRLogicalSimCapacity;       // Max Status Report records on the SIM
    uint8_t SRVerifiedSimCapacity;
} ShortMessageStorage_t;


/**
 * SMS_DeliveryStatus_t is a short form of Status Report indicating
 * the status of a short message.
 *
 * @param SMS_DELIVERY_STATUS_MO_SMS_DELIVERED             MO short message delivered
 * @param SMS_DELIVERY_STATUS_MO_SMS_NOT_YET_DELIVERED     MO short message not yet delivered
 * @param SMS_DELIVERY_STATUS_MO_SMS_NOT_DELIVERED         MO short message not delivered
 * @param SMS_DELIVERY_STATUS_MO_SMS_STATUS_NOT_REQUESTED  MO short message status not requested
 * @param SMS_DELIVERY_STATUS_MT_SMS_HAS_NOT_STATUS        MT short message has no status
 */
enum {
    SMS_DELIVERY_STATUS_MO_SMS_DELIVERED,
    SMS_DELIVERY_STATUS_MO_SMS_NOT_YET_DELIVERED,
    SMS_DELIVERY_STATUS_MO_SMS_NOT_DELIVERED,
    SMS_DELIVERY_STATUS_MO_SMS_STATUS_NOT_REQUESTED,
    SMS_DELIVERY_STATUS_MT_SMS_HAS_NOT_STATUS
};
typedef int8_t SMS_DeliveryStatus_t;    // (signed type)


//FUNCTION PROTOTYPES
/*
 * EFSMS and EFSMSR RECORD UTILITY FUNCTION PROTOTYPES
 */
static uint8_t SmStorage_SimEfFileSizeRd(
    int filename);
static uint8_t *SmStorage_EfSmsRecordTPDU(
    EfSmsRecord_t);
static void SmStorage_EfSmsRecordInitialise(
    EfSmsRecord_t);
static uint8_t SmStorage_EfSmsRecordStatusCalculate(
    const SMS_Status_t);
static EfSmsRecordStatus_t SmStorage_EfSmsRecordStatusGet(
    const EfSmsRecord_t);
static void SmStorage_EfSmsRecordStatusSet(
    EfSmsRecord_t,
    const uint8_t);
static SMS_Status_t SmStorage_ShortMessageStatusCalculate(
    const uint8_t fEfSmsRecordStatus);
static void SmStorage_EfSmsRecordPack(
    ShortMessage_p,
    EfSmsRecord_t);
static ShortMessageType_t SmStorage_EfSmsRecordType(
    EfSmsRecord_t EfSmsRecord);

static uint8_t SmStorage_SIM_ShortMessage_Write(
    const uint8_t * const SM_Data_p,
    uint8_t * const RecordNumber_p);

/*
 * SLOT OBJECT OPERATIONS
 */
static void SmStorage_SlotAllocate(
    const SMS_Position_t,
    SmStorageObject_t *);
static SmStorageObject_t *SmStorage_SlotAccess(
    const SMS_Position_t SlotNo);
static void SmStorage_SlotDeallocate(
    const SMS_Position_t);
static uint8_t SmStorage_SlotRead(
    const SMS_Position_t,
    EfSmsRecord_t);
static SmStoreErrorCode_t SmStorage_SlotWrite(
    const SMS_Position_t SlotNumber,
    EfSmsRecord_t EfSmsRecord,
    const MSG_StorageMedia_t * StorageVolume_p,
    ShortMessage_p const SmInstance,
    const uint8_t AllocateSlot);
static uint8_t SmStorage_SlotDelete(
    const SMS_Position_t);
static SMS_Position_t SmStorage_SlotRangeValidation(
    const SMS_Storage_t StorageMedia,
    const SmStorageTempVolume_t TempVolume,
    SMS_Position_t Slot);
static uint16_t SmStorage_SlotToIndex(
    const SMS_Position_t Slot);

// Short Message Utility Functions
static uint8_t SmStorage_SmInvalidTPDU(
    EfSmsRecord_t EfSmsRecord);
static ShortMessage_p SmStorage_SmRestore(
    const SMS_Position_t SlotNumber,
    EfSmsRecord_t EfSmsRecord);

// SmStorage Function prototypes
static void SmStorage_InitControlVolume(
    void);
static void SmStorage_EventTimeout_SmValidate(
    void);
static void SmStorage_EventTimeout_SIMRefresh(
    void);
static void SmStorage_EventTimeout_SIMRestart(
    void);
static ShortMessage_p SmStorage_ShortMessageInitialise(
    ShortMessageType_t const,
    EfSmsRecord_t,
    const SMS_Position_t);

static uint8_t SmStorage_WriteSmToFile(
    const SMS_Position_t SlotNumber,
    const EfSmsRecord_t EfSmsRecord,
    const MSG_StorageMedia_t * StorageVolume_p,
    ShortMessage_p const SmInstance,
    const uint8_t AllocateSlot);

static SmStoreErrorCode_t SmStorage_WriteSmToSIM(
    uint8_t SlotNumber,
    EfSmsRecord_t EfSmsRecord,
    ShortMessage_p SmInstance,
    const uint8_t AllocateSlot);

static uint8_t SmStorage_ReadSmFromFile(
    const char *const SmPathFilename_p,
    const uint16_t SmFileIndex,
    EfSmsRecord_t EfSmsRecord);

static uint8_t SmStorage_DeleteSmFromSim(
    const SMS_Position_t SlotNumber);

static uint8_t SmStorage_DeleteSmFromVolume(
    const SMS_Position_t SlotNumber,
    const char *const SmPathFilename_p,
    const uint16_t SmFileIndex);

static uint8_t SmStorage_SmToBeCached(
    ShortMessage_p SmInstance);

static void SmStorage_ValidateSmInSimCache(
    const EfSmsRecord_t EfSmsRecord,
    const uint8_t SlotNumber,
    ShortMessage_p SmInstance);

static void SmStorage_IncrementInfoCount(
    SmStorageObject_t * const storedObject_p,
    SMS_SlotInformation_t * const SmStorageInfo_p);

static uint8_t isUniqueStorageNumber(
    char *Pathname_p,
    char *FilenameProposed_p);

static SmStorageTempVolume_t SmStorage_VolumeIsTemporary(
    const MSG_StorageMedia_t * Volume_p);

static struct stat SmStorage_GetFileEntryTimeStamp(
    void);


#ifdef TEST_MSG_LIST
static void SmStorage_PrintMSG_List(
    void);
#endif

static void SmStorage_Index_Entry(
    const SmStorageContainer_t Container,
    FILE * File_p,
    const uint16_t FileIndex);

static SMS_Position_t SmStorage_GetContainerFilename(
    const SmStorageContainer_t Container,
    const MSG_StorageMedia_t * StorageMedia_p,
    char *SmPathname_p);

static uint8_t SmStorage_GetStorageDetails(
    const SmStorageContainer_t Container,
    const SMS_Position_t SlotNumber,
    char *PathFilename_p,
    uint16_t * FileIndex_p);

static uint8_t SmStorage_ReadStatusReport(
    const SMS_Position_t SlotNumber,
    DeliveryStatus_t * const DeliveryStatus_p,
    SMS_StatusReport_t * const StatusReport_p);

static uint8_t SmStorage_DeleteStatusReportFromVolume(
    const SMS_Position_t SlotNumber);

/* Storage MSG_List Manipulation function prototpyes */
static void SmStorage_InitStorageMSG_List(
    SmStorageMSG_List_p * thisMessageStorageMSG_List_p);
static void SmStorage_DestroyStorageMSG_List(
    SmStorageMSG_List_p thisMessageStorageMSG_List_p);
static void SmStorage_RemoveStoredItem(
    const SmStorageMSG_List_p thisMessageStorageMSG_List,
    const SmStorageObject_t * thisStoredSm_p);
static void SmStorage_InsertStoredItem(
    const SmStorageMSG_List_p thisMessageStorageMSG_List,
    const SmStorageObject_t * thisStoredSm_p);
static int SmStorage_CmpStoragePos(
    const SmStorageObject_t * newStoredSm_p,
    const SmStorageObject_t * StoredSmInMSG_List_p);

static SmStorageObject_t *SmStorage_IterateFromStart(
    SmStorageMSG_List_p thisStorageMSG_List,
    SmStorageObject_t * previousMessage_p);

static void SmStorage_RestoreSmFromVolume(
    const MSG_StorageMedia_t * const StorageVolume_p,
    ShortMessage_p * SmInstance_p,
    uint8_t * RestoreComplete_p);

static void SmStorage_ProcessPendingReplaceTypes(
    void);

static void SmStorage_GetStatusReportForSlotNo(
    const SMS_Position_t SlotNumber,
    uint8_t * SMS_SMRStatusReport_p,
    ShortMessage_p SmInstance);

static uint8_t SmStorage_StatusReportDeleteFromSIM(
    const uint8_t RecordNumber);

static uint8_t SmStorage_SIMFreeObsoleteSR(
    const SMS_Position_t SlotNumber);

static void SmStorage_CacheSRsFromSIM(
    uint8_t * CacheSIM_SRsComplete_p);

static void SmStorage_RestoreSIMVolume(
    ShortMessage_p * SmInstance_p,
    uint8_t * SIMRestoreComplete_p);

static void SmStorage_ValidateDeliveryStatus(
    const SMS_Position_t SlotNumber,
    ShortMessage_p SmInstance);

static uint8_t SmStorage_MoveSRBetweenContainerFiles(
    const char *const SrcFilename_p,
    const uint16_t SrcFileIndex,
    const char *const DstFilename_p,
    const uint16_t DstFileIndex);

static uint8_t SmStorage_GetFileIndex(
    char *SmFilename_p,
    uint16_t * FileIndex_p);
static uint8_t SmStorage_SetFileIndex(
    const uint16_t SlotNumber,
    const uint16_t FileIndex);
static void SmStorage_AnalyseStorage(
    SMS_StorageStatus_t * NewStorageStatus_p);
static void SMStorage_GetTimestampFromFileTime(
    ShortMessage_p SmInstance,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p);
static void SMStorage_GetTimestampFromServiceCentre(
    ShortMessage_p SmInstance,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p);

static uint8_t SmStorage_SIM_SMR_ShortMsg_Read_Available_Get(
    void);
static void SmStorage_SIM_SMR_ShortMsg_Read_Available_Set(
    const uint8_t ServiceAvailable);

static void SmStorage_CreateUniqueSmFilename(
    uint16_t * const FileIndex_p,
    char *SmPathname_p,
    char *SmFilename_p);
static void SmStorage_Construct_SM_Filename(
    const uint16_t SlotNumber,
    char *const SMFilename_p);

static uint8_t SmStorage_RestoreSIMDeliveryStatus(
    const SMS_Position_t fSlotNumber,
    ShortMessage_p SmInstance);
static uint8_t SmStorage_GetNextFreeSRRecord(
    void);

//********************* BEGIN BELONGS IN NEW FILE smstrg_utils *************************
#define CR_CHARACTER      '\r'
#define LF_CHARACTER      '\n'

#define RELATIVE_PATH_SIZE         (30)
#define NUM_OF_RELATIVE_PATH_SIZE  (3)
#define LINE_LENGTH                (MSG_STORAGE_MEDIA_LENGTH+3)
#define SIM_NOT_IN_PREFERRED_INDEX (0xFF)       // used to index the SIM volume if it is not in preferred storage


// These will be derived from the absolute pathname in c_filepaths.c
static char MESSAGING_SUB_DIR[RELATIVE_PATH_SIZE];
static char SMS_SUB_DIR[RELATIVE_PATH_SIZE];
//static char              MSG_SUB_DIR      [RELATIVE_PATH_SIZE];
static char SIMCACHE_SUB_DIR[RELATIVE_PATH_SIZE];

const char SM_SUB_DIR[] = "/sm";
const char SM_STATUS_REPORT_SUB_DIR[] = "/sr";
static const char PARENT_DIR[] = "..";


// Local Functions
static void SmStorage_GetICCIDPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p);
static uint8_t SmStorage_Set_FS_Working_Dir(
    void);
static void SmStorage_CreateFixedDirectories(
    const MSG_StorageMedia_t * const Volume_p);
static void SmStorage_CreateRelativeDirectories(
    const MSG_StorageMedia_t * const Volume_p);
static void SmStorage_ConfigureRelativePaths(
    char **RelativePathList);


static const char NullString = '\0';
static const char DirSeparator[] = "/";
static const char SmFileSuffix[] = ".sms";
static const char SmFile_UC_Suffix[] = ".SMS";
static const char SmContainerFilename[] = "/shortmessages.tpdu";
static const char SrContainerFilename[] = "/statusreports.tpdu";

// Note that some FS paths are defined in smutil.c. This is because they are also required for the persistent
// data storage in the FS.
const char SMS_FS_PATH_SIM_CACHE[] = SMS_FS_DEFAULT_TOP_MOUNT_POINT "/messaging/sms/simcache";
const char SMS_FS_DIR_SIM_CACHE[] = "/simcache";
const char SMS_FS_DIR_SMS[] = "/sms";

static char *SMS_FS_Working_Dir_p = NULL;

static const char SmTimeStampDummyFile[] = SMS_FS_DEFAULT_TOP_MOUNT_POINT "/tim.tmp";

static const uint8_t DefaultShortMessageFileEntry[SHORT_MESSAGE_FILE_ENTRY_LENGTH] = {
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

static uint8_t SIM_SMR_Service_Available = TRUE;

// Local RAM preferred storage list
static MSG_List *MSG_PrefStoreList_p = NULL;

static ShortMessageStorage_t ShortMessageStorage = { SMSTORAGESTATE_VALIDATE_SM,        // ShortMessageStorageState
    FALSE,                      // StorageListRetrieved
    0,                          // SlotLogicalSimCapacity
    0,                          // SlotVerifiedSimCapacity
    0,                          // SimUsedSpace
    0,                          // MEUsedSpace
    0,                          // MEInitialisationCount
    NULL,                       // SmStorageMSG_List
    {{0}},                      // ChangedStatusVolume (Double braces needed to silence warnings.)
    0,                          // SRLogicalSimCapacity
    0
};                              // SRVerifiedSimCapacity

static uint8_t AllSRsCached;    // this is set TRUE when all possible SR's have been cached from the SIM
static uint8_t *SIM_StatusReports_p = NULL;     // for temporary storage of SR's read from the SIM

#define EF_SMSR_MAX_RECS (254)
#define ARRAY_OFFSET     (1)    // StatusReportIsEmpty[0] holds Record 1 status etc
#define ArrayIndex(RecNum)  RecNum - ARRAY_OFFSET
static uint8_t StatusReportIsEmpty[EF_SMSR_MAX_RECS];






//#define DUMP_FS_ACTIVITY_TO_LOG_ENABLE
#ifdef DUMP_FS_ACTIVITY_TO_LOG_ENABLE
/*
 * If this code is enabled, it will replace the standard versions of fopen, fclose, fwrite and fread with debug versions which print
 * data to the log.
 * fopen will print filename, mode and descriptor.
 * fclose will print descriptor.
 * fwrite will print the raw hex data, which is written to the file, to the log.
 * fread will print the raw dex data, which is read from the file, to the log.
 *
 * The debug functions are very basic and contain no error handling. They should be transparent to any code which uses them.
 */

// Function to dump a block of hex data to the log in a format which is easy to read.
static void SMS_Util_dump_hex_data_to_log(
    const char *const hex_data,
    const int32_t hex_data_length,
    const int32_t isWriteData,
    const int32_t line_num,
    const char *const file_p)
{
#define NUM_CONVERTED_BYTES_IN_OUTPUT_STRING  16        // each line of data printed to log has this number of converted bytes within it
#define NUM_CHARS_PER_CONVERTED_BYTE           3        // each byte of data is represented by 2 char digits + 1 space; e.g. 0x3E becomes "3E "
    char *buf = SMS_HEAP_UNTYPED_ALLOC((NUM_CONVERTED_BYTES_IN_OUTPUT_STRING * NUM_CHARS_PER_CONVERTED_BYTE) + 1);

    if (buf != NULL) {
        int32_t bytes_remaining = hex_data_length;
        const char *from = hex_data;

        SMS_LOG_D("%s Data Message Length=%d", isWriteData ? "Written" : "Read", hex_data_length);
        SMS_LOG_D("%s %d", file_p, line_num);
        SMS_LOG_D("DATA START:");

        while (bytes_remaining > 0) {
            uint32_t i;
            char *to = buf;
            const uint32_t bytes_to_convert = (bytes_remaining > NUM_CONVERTED_BYTES_IN_OUTPUT_STRING) ? NUM_CONVERTED_BYTES_IN_OUTPUT_STRING : bytes_remaining;

            for (i = 0; i < bytes_to_convert; i++) {
                (void) sprintf(to, "%02X ", *from);
                from++;
                to += NUM_CHARS_PER_CONVERTED_BYTE;
            }
            bytes_remaining -= bytes_to_convert;
            SMS_LOG_D("DATA: %s", buf);
        }
        SMS_LOG_D("DATA COMPLETE");
        SMS_HEAP_FREE(&buf);
    } else
        SMS_LOG_E("malloc failed");
}

// Function to read from a file and dump data to log.
static size_t fread_and_dump(
    void *ptr,
    size_t size,
    size_t nmemb,
    FILE * stream,
    const char *filename_p,
    const int lineNum)
{
    size_t r = fread(ptr, size, nmemb, stream);
    SMS_Util_dump_hex_data_to_log((const char *) ptr, size * nmemb, FALSE, lineNum, filename_p);
    if (r < 1)
        SMS_LOG_E("fread() returned %d!!", r);

    return r;
}

// Function to write to a file and dump data to log.
static size_t fwrite_and_dump(
    const void *ptr,
    size_t size,
    size_t nmemb,
    FILE * stream,
    const char *filename_p,
    const int lineNum)
{
    size_t r = fwrite(ptr, size, nmemb, stream);
    SMS_Util_dump_hex_data_to_log((const char *) ptr, size * nmemb, TRUE, lineNum, filename_p);
    if (r < 1)
        SMS_LOG_E("fwrite() returned %d!!", r);

    return r;
}

// Function to open file and print its name, mode and descriptor to the log.
static FILE *fopen_and_dump(
    const char *path,
    const char *mode,
    const char *filename_p,
    const int lineNum)
{
    FILE *fp = fopen(path, mode);

    SMS_LOG_I("%s %d\nOpening %s in mode %s. Descriptor = 0x%08X", filename_p, lineNum, path, mode, (unsigned int) fp);
    return fp;
}

// Function to close file and print its descriptor to log.
static int fclose_and_dump(
    FILE * fp,
    const char *filename_p,
    const int lineNum)
{
    int r = fclose(fp);
    SMS_LOG_D("%s %d\nClosing File Descriptor = 0x%08X", filename_p, lineNum, (unsigned int) fp);
    if (r != 0)
        SMS_LOG_E("fclose() returned %d!!", r);

    return r;
}

// #defines to replace standard versions for fread, fwrite and fopen with debug versions.
#define fread(a, b, c, d)  fread_and_dump((a), (b), (c), (d), __FILE__, __LINE__)
#define fwrite(a, b, c, d) fwrite_and_dump((a), (b), (c), (d), __FILE__, __LINE__)
#define fopen(a, b)        fopen_and_dump((a), (b), __FILE__, __LINE__)
#define fclose(a)          fclose_and_dump((a), __FILE__, __LINE__)
#endif                          // DUMP_FS_ACTIVITY_TO_LOG_ENABLE





/********************************************************************/
/**
 *
 * @function     SmStorage_Clear_FS_Working_Dir
 *
 * @description  Clears the current working directory
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_Clear_FS_Working_Dir(
    void)
{
    // Clear the current working directory
    if (SMS_FS_Working_Dir_p != NULL) {
        free(SMS_FS_Working_Dir_p);
        SMS_FS_Working_Dir_p = NULL;
    }
}

/********************************************************************/
/**
 *
 * @function     SmStorage_Initialise
 *
 * @description  Identify the SmStorage Capacity for each Storage Media with the
 *               capabilty of storing Short Messages.
 *               Set the Memory Capacity Exceeded flag, prior to initiating
 *               the initialisation of the Storage Media.
 *
 * Important !!!!!!!
 * We assume that the SIM state is active and we have a ICCID in the Messaging System
 *
 * NOTES:        The Short Messages will be Asynchronously Retrieved from SIM Storage
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_Initialise(
    void)
{
    static uint8_t initialised = FALSE;

    char *DirectoryBuffer_p;
    MSG_StoreList_t *MSG_StorageList_p = NULL;
    uint16_t loop;
    MSG_StorageMedia_t *CurrentVolume_p;
    char *RelativePathList[NUM_OF_RELATIVE_PATH_SIZE] = { SIMCACHE_SUB_DIR,
        SMS_SUB_DIR,
        MESSAGING_SUB_DIR
    };

    SMS_C_(SMS_LOG_V("smstrg.c SmStorage_Initialise commencing"));

    SMS_C_(SMS_LOG_V("RelativePathList[0]=%s", RelativePathList[0]));
    SMS_C_(SMS_LOG_V("RelativePathList[1]=%s", RelativePathList[1]));
    SMS_C_(SMS_LOG_V("RelativePathList[2]=%s", RelativePathList[2]));
    // Get some working memory
    DirectoryBuffer_p = (char *) SMS_HEAP_UNTYPED_ALLOC(sizeof(char) * FILENAME_MAX);

    // The configuration filepaths are all absolute, but we need relative paths
    SmStorage_ConfigureRelativePaths(RelativePathList);

    SMS_C_(SMS_LOG_V("RelativePathList[0]=%s", RelativePathList[0]));
    SMS_C_(SMS_LOG_V("RelativePathList[1]=%s", RelativePathList[1]));
    SMS_C_(SMS_LOG_V("RelativePathList[2]=%s", RelativePathList[2]));

    // Store the current working directory
    SmStorage_Set_FS_Working_Dir();

    // The FS may not contain the directory structure required by SMS.
    // Therefore create it.
    // Now for each volume in the preferred storage list make the messaging directories
    MSG_StorageList_p = (MSG_StoreList_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StoreList_t));

    (void) SmStorage_GetStorageList(MSG_StorageList_p);

    CurrentVolume_p = MSG_StorageList_p->MSGStorageList_p;

    for (loop = 0; loop < MSG_StorageList_p->NumStorageMedia; loop++) {
        SMS_C_(SMS_LOG_V("CurrentVolume_p->StorageMediaName=%s", CurrentVolume_p->StorageMediaName));
        // We do not want to create a sim volume
        if (strcmp(CurrentVolume_p->StorageMediaName, SIM_VOL)) {
            SmStorage_CreateFixedDirectories(CurrentVolume_p);
            SmStorage_CreateRelativeDirectories(CurrentVolume_p);
        }
        // Move onto the next storage media
        CurrentVolume_p++;
    }

    // This is allocated in SmStorage_GetStorageList
    SMS_HEAP_FREE(&MSG_StorageList_p->MSGStorageList_p);
    SMS_HEAP_FREE(&MSG_StorageList_p);

    // Now we need to create the SIM Cache directories
    SmStorage_CreateRelativeDirectories((MSG_StorageMedia_t *) SMS_FS_DIR_USE_SIM_CACHE);


    // Free some working memory
    SMS_HEAP_FREE(&DirectoryBuffer_p);

    SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_Initialise: Logical Slot Capacity %d", SmStorage_SIMSlotLogicalCapacityRd()));
    //Initialise SmStorage Capacity
    SmStorage_SIMSlotLogicalCapacityWr(SmStorage_SimEfFileSizeRd(EF_SMS_FILENAME));
    ShortMessageStorage.SimUsedSpace = 0;
    ShortMessageStorage.MEInitialisationCount = 0;

    //Initialise SrStorage Capacity
    SmStorage_SIM_SR_LogicalCapacityWr(SmStorage_SimEfFileSizeRd(EF_SMSR_FILENAME));
    AllSRsCached = FALSE;
    memset(StatusReportIsEmpty, FALSE, EF_SMSR_MAX_RECS);

    // Read persistent storage to determine the number of messages stored on the ME
    if (SMS_ERROR_INTERN_NO_ERROR != SmUtil_PersistentStorageData_Read(SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT, sizeof(uint16_t), &ShortMessageStorage.MEUsedSpace)) {
        ShortMessageStorage.MEUsedSpace = 0;
    }

    SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_Initialise: Logical Slot Capacity %d", SmStorage_SIMSlotLogicalCapacityRd()));

    if (initialised) {
        // if we've already been initialised, destroy the old list so we
        // we dont cause a memory leak - Now why do we get 2 inits?
        SmStorage_DestroyStorageMSG_List(ShortMessageStorage.SmStorageMSG_List);
    }
    // Init the linked list of short messages
    SmStorage_InitStorageMSG_List(&ShortMessageStorage.SmStorageMSG_List);

    initialised = TRUE;

    // Commence retrieval of EfSms Record(s) from Pd
    ShortMessageStorage_WriteState(SMSTORAGESTATE_VALIDATE_SM);

    Do_SMS_TimerSet_1_MS_WCT(MSG_SMSTORAGE_INIT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_STORAGE_INIT_TIMER_VALUE);

    SMS_B_(SMS_LOG_D("smstrg.c SmStorage_Initialise complete"));
}

void SmStorage_Deinitialise(
    void)
{
    if (NULL != ShortMessageStorage.SmStorageMSG_List) {
        SmStorage_DestroyStorageMSG_List(ShortMessageStorage.SmStorageMSG_List);
    }

    if (NULL != MSG_PrefStoreList_p) {
        MSG_StorageMedia_t *NextPreferredStorage_p = MSG_ListGetFirst(MSG_PrefStoreList_p);
        while (NULL != NextPreferredStorage_p) {
            (void) MSG_ListRemove(MSG_PrefStoreList_p, NextPreferredStorage_p);
            SMS_HEAP_FREE(&NextPreferredStorage_p);
            NextPreferredStorage_p = MSG_ListGetFirst(MSG_PrefStoreList_p);
        }
        MSG_ListDestroy(MSG_PrefStoreList_p);
    }
}

/********************************************************************/
/**
 *
 * @function     SmStorage_Set_FS_Working_Dir
 *
 * @description  Sets the current working directory
 *
 * @param        void
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_Set_FS_Working_Dir(
    void)
{
    if (SMS_FS_Working_Dir_p == NULL) {
        SMS_FS_Working_Dir_p = (char *) malloc(FILENAME_MAX);
    }

    if (SMS_FS_Working_Dir_p != NULL) {
        if (getcwd(SMS_FS_Working_Dir_p, FILENAME_MAX) != NULL) {
            return (TRUE);
        } else {
            free(SMS_FS_Working_Dir_p);
            SMS_FS_Working_Dir_p = NULL;
        }
    }

    return (FALSE);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_CreateFixedDirectories
 *
 * @description  Create the /messaging/sms/msg diorectory
 *
 * @param        CurrentVolume_p->StorageMediaName
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_CreateFixedDirectories(
    const MSG_StorageMedia_t * const Volume_p)
{
    char *Buffer_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX * sizeof(char));

    SMS_C_(SMS_LOG_V("smstrg.c SmStorage_CreateFixedDirectories()"));

    if (Buffer_p != NULL) {
        SMS_C_(SMS_LOG_V("Volume_p->StorageMediaName=%s", Volume_p->StorageMediaName));
        // Get into the volume name
        if (chdir(Volume_p->StorageMediaName) == 0) {
            // Make the /.../messaging directory
            (void) strcpy(Buffer_p, MESSAGING_SUB_DIR);
            SMS_C_(SMS_LOG_V("\tBuffer_p=%s", Buffer_p));

            if (access(Buffer_p + 1, F_OK) != 0) {
                (void) mkdir(Buffer_p + 1, S_IRWXU);
            }
            // Make the /.../messaging/sms directory
            (void) strcat(Buffer_p, SMS_SUB_DIR);
            SMS_C_(SMS_LOG_V("\tBuffer_p=%s", Buffer_p));

            if (access(Buffer_p + 1, F_OK) != 0) {
                (void) mkdir(Buffer_p + 1, S_IRWXU);
            }
            // Make the /../messaging/simcache directory
            (void) strcpy(Buffer_p, MESSAGING_SUB_DIR);
            (void) strcat(Buffer_p, SIMCACHE_SUB_DIR);
            SMS_C_(SMS_LOG_V("\tBuffer_p=%s", Buffer_p));

            if (access(Buffer_p + 1, F_OK) != 0) {
                (void) mkdir(Buffer_p + 1, S_IRWXU);
            }
            // Restore current working directory
            if (chdir(SMS_FS_Working_Dir_p) != 0) {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_CreateFixedDirectories() Failed to chdir into %s", SMS_FS_Working_Dir_p));
            }
        } else {
            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_CreateFixedDirectories() Failed to chdir into %s", Volume_p->StorageMediaName));
        }

        SMS_HEAP_FREE(&Buffer_p);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_CreateRelativeDirectories
 *
 * @description  Create the ICCID directry and those below it.
 *
 * @param        Volume_p : .
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_CreateRelativeDirectories(
    const MSG_StorageMedia_t * const Volume_p)
{
    char *Buffer_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX * sizeof(char));

    SMS_C_(SMS_LOG_V("smstrg.c SmStorage_CreateRelativeDirectories()"));

    if (Buffer_p != NULL) {
        // Now we need to create the iccid directory and below
        // Make the /.../messaging/sms/<iccid> directory
        SmStorage_GetICCIDPath(Volume_p, Buffer_p);

        SMS_C_(SMS_LOG_V("\tBuffer_p=%s", Buffer_p));
        if (access(Buffer_p, F_OK)) {
            (void) mkdir(Buffer_p, S_IRWXU);
        }
        // Make the /.../messaging/sms/<iccid>/sm directory
        SmStorage_GetSMPath(Volume_p, Buffer_p);
        SMS_C_(SMS_LOG_V("\tBuffer_p=%s", Buffer_p));

        if (access(Buffer_p, F_OK)) {
            (void) mkdir(Buffer_p, S_IRWXU);
        }
        // Make the /.../messaging/sms/<iccid>/sr directory
        SmStorage_GetDeliveryStatusPath(Volume_p, Buffer_p);
        SMS_C_(SMS_LOG_V("\tBuffer_p=%s", Buffer_p));

        if (access(Buffer_p, F_OK)) {
            (void) mkdir(Buffer_p, S_IRWXU);
        }
        // Make the /.../messaging/simcache/<iccid>/control directory
        SmStorage_GetSMControlPath(Volume_p, Buffer_p);
        SMS_C_(SMS_LOG_V("\tBuffer_p=%s", Buffer_p));

        if (access(Buffer_p, F_OK)) {
            (void) mkdir(Buffer_p, S_IRWXU);
        }

        SMS_HEAP_FREE(&Buffer_p);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ConfigureRelativePaths
 *
 * @description
 * Copies the SimCache, SMS and Messaging directory names into buffers
 * pointed to by RelativePathList_pp.
 *
 * @param        RelativePathList_pp    Resultant Directory List
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_ConfigureRelativePaths(
    char **RelativePathList_pp)
{
    if (RelativePathList_pp != NULL) {
        // Copy path for SMS SIM Cache
        (void) strncpy(RelativePathList_pp[0], SMS_FS_DIR_SIM_CACHE, RELATIVE_PATH_SIZE);

        // Copy path for SMS Subdir
        (void) strncpy(RelativePathList_pp[1], SMS_FS_DIR_SMS, RELATIVE_PATH_SIZE);

        // Copy path for Messaging Subdir
        (void) strncpy(RelativePathList_pp[2], SMS_FS_DIR_MESSAGING, RELATIVE_PATH_SIZE);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetICCIDPath
 *
 * @description  Gets path to the ICCID directoty
 *
 * @param        DirectoryBuffer_p : Holds the ICCID directory
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_GetICCIDPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p)
{
    char *Buffer_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX * sizeof(char));

    if (Buffer_p) {
        const char *TopDirectory_p;
        uint8_t IsSimCache;

        if (strcmp(Volume_p->StorageMediaName, SMS_FS_DIR_USE_SIM_CACHE) == 0) {
            // Using SIM volume so need to setup folder in FS to support caching of SIM data.
            TopDirectory_p = SMS_FS_DIR_TOP_MOUNT_POINT;
            IsSimCache = TRUE;
        } else {
            // Using SIM 
            TopDirectory_p = Volume_p->StorageMediaName;
            IsSimCache = FALSE;
        }

        (void) strcpy(DirectoryBuffer_p, TopDirectory_p);
        (void) strcat(DirectoryBuffer_p, MESSAGING_SUB_DIR);
        if (IsSimCache) {
            (void) strcat(DirectoryBuffer_p, SIMCACHE_SUB_DIR);
        } else {
            (void) strcat(DirectoryBuffer_p, SMS_SUB_DIR);
        }
        (void) strcat(DirectoryBuffer_p, "/");

        SmUtil_ICCID_Get(Buffer_p);

        (void) strcat(DirectoryBuffer_p, Buffer_p);

        SMS_HEAP_FREE(&Buffer_p);
    }
}



/********************************************************************/
/**
 *
 * @function     SmStorage_GetSMPath
 *
 * @description  Gets the path to the short message directory on
 *               the specified volume
 *
 * @param        Volume_p : Volume to get the sm path for
 * @param        DirectoryBuffer_p : Holds the sm path
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_GetSMPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p)
{
    char *Buffer_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX * sizeof(char));

    if (Buffer_p) {
        SmStorage_GetICCIDPath(Volume_p, Buffer_p);
        (void) strcpy(DirectoryBuffer_p, Buffer_p);
        (void) strcat(DirectoryBuffer_p, SM_SUB_DIR);

        SMS_HEAP_FREE(&Buffer_p);
    } else {
        memset(DirectoryBuffer_p, 0, sizeof(char) * FILENAME_MAX);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetSMControlPath
 *
 * @description  Gets the sm control directory
 *
 * @param        DirectoryBuffer_p : Holds the required path name
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_GetSMControlPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p)
{
    char *Buffer_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX * sizeof(char));

    if (Buffer_p) {
        SmStorage_GetICCIDPath(Volume_p, Buffer_p);
        (void) strcpy(DirectoryBuffer_p, Buffer_p);
        (void) strcat(DirectoryBuffer_p, SM_CONTROL_DIR);

        SMS_HEAP_FREE(&Buffer_p);
    } else {
        memset(DirectoryBuffer_p, 0, sizeof(char) * FILENAME_MAX);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetDeliveryStatusPath
 *
 * @description  Gets tha pathname of the delivery status directory
 *
 * @param        Volume_p : Volume to get the path for
 * @param        DirectoryBuffer_p : Holds delivery status pathname
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_GetDeliveryStatusPath(
    const MSG_StorageMedia_t * const Volume_p,
    char *DirectoryBuffer_p)
{
    char *Buffer_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX * sizeof(char));

    if (Buffer_p != NULL) {
        SmStorage_GetICCIDPath(Volume_p, Buffer_p);
        (void) strcpy(DirectoryBuffer_p, Buffer_p);
        (void) strcat(DirectoryBuffer_p, SM_STATUS_REPORT_SUB_DIR);

        SMS_HEAP_FREE(&Buffer_p);
    } else {
        memset(DirectoryBuffer_p, 0, sizeof(char) * FILENAME_MAX);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ClearDir
 *
 * @description  Clear the specified directoy
 *
 * @param        Directory_Buffer_p : Holds the directory path
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_ClearDir(
    char *DirectoryBuffer_p)
{
    struct dirent *DirectoryEntry_p = NULL;
    // Open the directory
    DIR *Directory_p = opendir(DirectoryBuffer_p);

    if (Directory_p) {
        // Get to the first entry
        rewinddir(Directory_p);

        //change to the directory
        if (chdir(DirectoryBuffer_p) == 0) {
            while ((DirectoryEntry_p = readdir(Directory_p))) {
                // Resursive, but do not go into . or ..
                if (DirectoryEntry_p->d_name[0] != '.') {
                    // Assume its a file so attempt to remove it
                    if (remove(DirectoryEntry_p->d_name) == -1) {
                        // Empty the directory - May not be a directory
                        SmStorage_ClearDir(DirectoryEntry_p->d_name);
                        (void) rmdir(DirectoryEntry_p->d_name);
                    }
                }
            }
        } else {
            SMS_A_(SMS_LOG_E("smstrg.c: chdir(%s) failed.", DirectoryBuffer_p));
        }

        // Now close the directory
        (void) closedir(Directory_p);

        //change to the Parent directory
        if (chdir(PARENT_DIR) != 0) {
            SMS_A_(SMS_LOG_E("smstrg.c: chdir(PARENT_DIR) failed."));
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetStorageList
 *
 * @description  Get the list of storage medias
 *
 * @param        MSG_StoreList_p : must be a NULL pointer, and is allocted
 *                                 within here, and freed by the caller.
 *
 * @return       void
 */
/********************************************************************/
uint8_t SmStorage_GetStorageList(
    MSG_StoreList_t * MSG_StoreList_p)
{
    MSG_ListIndex NumOfPrefStoreMedia;
    MSG_StorageMedia_t *CurrentPrefStorage_p;
    MSG_StorageMedia_t *PreferredStorage_p;

    SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_GetStorageList() called"));
    // Determine whether or not a RAM copy of the preferred storage list exists
    if (!MSG_PrefStoreList_p) {
        unsigned char idx = 0;

        // Create RAM copy of preferred storage list
        MSG_PrefStoreList_p = MSG_ListCreate();

        while (Preferred_Storage_Default[idx] && strlen(Preferred_Storage_Default[idx])) {
            PreferredStorage_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));
            (void) strcpy(PreferredStorage_p->StorageMediaName, Preferred_Storage_Default[idx]);
            (void) MSG_ListInsertLast(MSG_PrefStoreList_p, PreferredStorage_p);
            idx++;
        }

        PreferredStorage_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));
        (void) strcpy(PreferredStorage_p->StorageMediaName, SM_CONTROL_DIR);
        (void) MSG_ListInsertLast(MSG_PrefStoreList_p, PreferredStorage_p);
    }

    NumOfPrefStoreMedia = MSG_ListGetCount(MSG_PrefStoreList_p) - 1;    // -1 to remove control storage area
    CurrentPrefStorage_p = MSG_ListGetFirst(MSG_PrefStoreList_p);

    // Load the data from the RAM copy of the preferred storage list into the output list
    MSG_StoreList_p->MSGStorageList_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t) * NumOfPrefStoreMedia);
    MSG_StoreList_p->NumStorageMedia = (uint8_t) NumOfPrefStoreMedia;

    if (!MSG_StoreList_p->MSGStorageList_p) {
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_GetStorageList: Failed memory allocation!"));
        return FALSE;
    } else {
        int i;
        MSG_StorageMedia_t *Media_p = MSG_StoreList_p->MSGStorageList_p;

        for (i = 0; i < NumOfPrefStoreMedia; i++) {
            // reset contents of the buffer to NULL
            memset(Media_p->StorageMediaName, 0x00, sizeof(Media_p->StorageMediaName));
            // Insert the media name in the output list. The the RAM copy of the
            // preferred storage list is sorted in priority order.
            (void) strcpy(Media_p->StorageMediaName, CurrentPrefStorage_p->StorageMediaName);
            CurrentPrefStorage_p = MSG_ListGetNext(MSG_PrefStoreList_p, CurrentPrefStorage_p);
            Media_p++;
        }
    }

    return TRUE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_CmpStorageMedia
 *
 * @description  Determine if the given preferred storage item has the
 *               given storage media name.
 *
 * @param        PrefStoreItem_p : The preferred storage media name
 * @param        Volume_p        : The storage media name to match
 *
 * @return       uint8_t
 */
/********************************************************************/
static int SmStorage_CmpStorageMedia(
    const MSG_StorageMedia_t * const PrefStoreItem_p,
    const MSG_StorageMedia_t * const Volume_p)
{
    return strcmp(PrefStoreItem_p->StorageMediaName, Volume_p->StorageMediaName);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetVolumeIndex
 *
 * @description  Returns the index used to reference the given volume.
 *
 * @param        Volume_p      : The volume whose index index is required
 * @param        VolumeIndex_p : The index of the given volume
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_GetVolumeIndex(
    const MSG_StorageMedia_t * const Volume_p,
    uint8_t * const VolumeIndex_p)
{
    uint8_t RetVal = FALSE;
    MSG_ListIndex ListIndex = MSG_ListFind(MSG_PrefStoreList_p, Volume_p, (MSG_ListCompareFunc) SmStorage_CmpStorageMedia);

    if (NOPOS == ListIndex) {
        // Check if this is the special case of the SIM volume which may not be in preferred storage
        if (!strcmp(Volume_p->StorageMediaName, SIM_VOL)) {
            // Allocate a special SIM index. Will fail if there are 255 genuine volumes in preferred storage
            ListIndex = SIM_NOT_IN_PREFERRED_INDEX;
            RetVal = TRUE;
        }
    }
    {
        *VolumeIndex_p = (uint8_t) ListIndex;
        RetVal = TRUE;
    }

    return RetVal;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetVolume
 *
 * @description  Returns the volume for the given volume index.
 *
 * @param        VolumeIndex : The index of the required volume
 * @param        Volume_p    : The volume referenced by the given index
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_GetVolume(
    const uint8_t VolumeIndex,
    MSG_StorageMedia_t * const Volume_p)
{
    if (SIM_NOT_IN_PREFERRED_INDEX == VolumeIndex) {
        // Copy volume found to return parameter
        (void) strcpy(Volume_p->StorageMediaName, SIM_VOL);
        return TRUE;
    } else {
        MSG_StorageMedia_t *IndexedVolume_p = MSG_ListGet(MSG_PrefStoreList_p, VolumeIndex);

        if (IndexedVolume_p) {
            // Copy volume found to return parameter
            (void) strcpy(Volume_p->StorageMediaName, IndexedVolume_p->StorageMediaName);
            return TRUE;
        }
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetAvailableVolume
 *
 * @description  Get the available storage volumes
 *
 * @param        AvailableMedia_p : Holds the available storage media
 * @param        RequiredSize : The size we need to store
 * @param        MEOnly       : TRUE if only check ME storage
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_GetAvailableVolume(
    MSG_StorageMedia_t * AvailableMedia_p,
    uint8_t RequiredSize,
    uint8_t MEOnly)
{
    MSG_StoreList_t *MSG_StoreList_p;
    uint8_t RetVal = FALSE;

    // Now for each volume in the preferred storage list make sure there is space available
    MSG_StoreList_p = (MSG_StoreList_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StoreList_t));

    if (SmStorage_GetStorageList(MSG_StoreList_p)) {
        MSG_StorageMedia_t *LocalMedia_p = MSG_StoreList_p->MSGStorageList_p;
        uint16_t AvailableSize;
        uint8_t loop;

        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_GetAvailableVolume: NumStorageMedia = %d", MSG_StoreList_p->NumStorageMedia));

        for (loop = 0; loop < MSG_StoreList_p->NumStorageMedia; loop++) {
            // We do not check the SIM if MEOnly storage is TRUE
            if (strcmp(LocalMedia_p->StorageMediaName, SIM_VOL) || !MEOnly) {
                RetVal = SmStorage_FindSpace(LocalMedia_p, RequiredSize, SMS_MAX_STORAGE_ME_SHORT_MESSAGES, &AvailableSize);

                if (RetVal) {
                    // We have found some room, do not continue
                    memcpy(AvailableMedia_p, LocalMedia_p, sizeof(MSG_StorageMedia_t));
                    break;
                }
            }
            // No room at the inn - keep looking
            LocalMedia_p++;
        }
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: Unable to determine prefered store list"));
    }

    // This is allocated in SmStorage_GetStorageList
    SMS_HEAP_FREE(&MSG_StoreList_p->MSGStorageList_p);
    SMS_HEAP_FREE(&MSG_StoreList_p);

    return RetVal;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetAvailableCapacity
 *
 * @description  Get the available storage capacity
 *
 * @param        void
 *
 * @return       uint16_t
 */
/********************************************************************/
uint16_t SmStorage_GetAvailableMECapacity(
    void)
{
    MSG_StoreList_t *MSG_StoreList_p;
    uint16_t TotalAvailableSize = 0;

    // Now for each volume in the preferred storage
    MSG_StoreList_p = (MSG_StoreList_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StoreList_t));

    if (SmStorage_GetStorageList(MSG_StoreList_p)) {
        MSG_StorageMedia_t *LocalMedia_p = MSG_StoreList_p->MSGStorageList_p;
        uint8_t loop;
        uint16_t AvailableSize;

        for (loop = 0; loop < MSG_StoreList_p->NumStorageMedia; loop++) {
            // Dont include the SIM
            if (strcmp(LocalMedia_p->StorageMediaName, SIM_VOL)) {
                // NOTE : Do not consider maximum short messages in storage since
                //        since the total available size across all ME volumes will
                //        be limited before this function returns.
                (void) SmStorage_FindSpace(LocalMedia_p, 1, 0, &AvailableSize);

                TotalAvailableSize += AvailableSize;

                if (SMS_MAX_STORAGE_ME_SHORT_MESSAGES && TotalAvailableSize > (SMS_MAX_STORAGE_ME_SHORT_MESSAGES - SmStorage_MEUsedSpaceRd())) {
                    TotalAvailableSize = SMS_MAX_STORAGE_ME_SHORT_MESSAGES - SmStorage_MEUsedSpaceRd();
                    break;
                }
            }
            // keep looking
            LocalMedia_p++;
        }
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: Unable to determine prefered store list"));
    }

    // This is allocated in SmStorage_GetStorageList
    SMS_HEAP_FREE(&MSG_StoreList_p->MSGStorageList_p);
    SMS_HEAP_FREE(&MSG_StoreList_p);

    return TotalAvailableSize;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ME_StorageFull
 *
 * @description  Determine if ME storage is available depending on
 *               the maximum number of short messages allowed in
 *               storage.
 *
 * @param        MaximumSize  : Maximum short messages in storage (ignored if set to 0)
 * @param        RequiredSize : The size of the candidate message.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_ME_StorageFull(
    const uint16_t MaximumSize,
    const uint16_t RequiredSize)
{
    if (!MaximumSize) {
        return FALSE;
    } else {
        uint16_t UsedSpace = SmStorage_MEUsedSpaceRd();

        if ((UsedSpace + RequiredSize) <= MaximumSize) {
            return FALSE;
        } else {
            SMS_A_(SMS_LOG_W("smstrg.c: Request exceeded max msg limit.  used = %d requested = %d max = %d", UsedSpace, RequiredSize, MaximumSize));
        }
    }

    return TRUE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_FindSpace
 *
 * @description  Search for space on specified volume
 *
 * @param        Volume_p        : Holds the volume name
 * @param        RequiredSize    : The amount of storage we need
 * @param        MaximumSize     : Maximum short messages in storage (ignored if set to 0)
 * @param        AvailableSize_p : The amount of storage available (valid for ME volumes only)
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_FindSpace(
    const MSG_StorageMedia_t * const Volume_p,
    const uint8_t RequiredSize,
    const uint16_t MaximumSize,
    uint16_t * const AvailableSize_p)
{
#ifdef PLATFORM_ANDROID
    // Android build.  This does not support POSIX statvfs. Instead the deprecated statfs has to be used.
    struct statfs VolumeStat;
#else
    // Not an Android build.  This does support POSIX statvfs.
    struct statvfs VolumeStat;
#endif
    uint8_t RetVal = FALSE;

    // Initialise available size
    *AvailableSize_p = 0;

    // If we are asking for space on the SIM we do things a bit differently
    if (!strcmp(Volume_p->StorageMediaName, SIM_VOL)) {
        // Find out how many slots are free on the sim
        *AvailableSize_p = (uint16_t) (SmStorage_SIMSlotSimTotalSpaceRd() - SmStorage_SIMSlotSimUsedSpaceRd());

        if (*AvailableSize_p >= RequiredSize) {
            // We have found some room
            RetVal = TRUE;
        }
    } else {
        if (!SmStorage_ME_StorageFull(MaximumSize, RequiredSize)) {
#ifdef PLATFORM_ANDROID
            // Determine the free space on the specified Volume
            if (statfs(Volume_p->StorageMediaName, &VolumeStat))
#else
            if (statvfs(Volume_p->StorageMediaName, &VolumeStat))
#endif
            {
                SMS_A_(SMS_LOG_I("smstrg.c: SmStorage_FindSpace statvfs failed free blocks set to 0"));
                VolumeStat.f_bfree = 0;
            }
            // Is there enough room one message (176 byte) requires one block (512 byte) size
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_FindSpace RequiredBlocks = 0x%x", RequiredSize));
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_FindSpace VolumeStat : FreeBlock 0x%x, BlockSize 0x%x", (unsigned int) VolumeStat.f_bfree, (unsigned int) VolumeStat.f_bsize));

            if (CFG_MSG_FSU_BLOCK_OVERHEAD < VolumeStat.f_bfree && (uint32_t) (RequiredSize + CFG_MSG_FSU_BLOCK_OVERHEAD) <= VolumeStat.f_bfree) {
                // We have found some spare blocks
                RetVal = TRUE;
            }

            if (VolumeStat.f_bfree > CFG_MSG_FSU_BLOCK_OVERHEAD) {
                *AvailableSize_p = (uint16_t) (VolumeStat.f_bfree - CFG_MSG_FSU_BLOCK_OVERHEAD);
            }
        }
    }

    return RetVal;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_FindSIMCapacity
 *
 * @description  Find SIM capacity information.
 *
 * @param        MessageStorageCapacity_p
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_FindSIMCapacity(
    SMS_StorageCapacity_t * MessageStorageCapacity_p)
{
    if (MessageStorageCapacity_p) {
        MessageStorageCapacity_p->UsedSpace = SmStorage_SIMSlotSimUsedSpaceRd();
        MessageStorageCapacity_p->FreeSpace = SmStorage_SIMSlotSimTotalSpaceRd() - SmStorage_SIMSlotSimUsedSpaceRd();

        SMS_B_(SMS_LOG_D("smstrg.c: SimFreeSpace %d SimUsedSpace %d", MessageStorageCapacity_p->FreeSpace, MessageStorageCapacity_p->UsedSpace));

        return TRUE;
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DeleteAllThisICCID_Messages
 *
 * @description  Delete all messages for this ICCID on the storage volume
 *
 * @param        *StorageVolume_p : Storage volume name
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_DeleteAllThisICCID_Messages(
    MSG_StorageMedia_t * StorageVolume_p)
{
    char *Pathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(sizeof(char) * FILENAME_MAX);

    // Delete the SM Directory
    SmStorage_GetSMPath(StorageVolume_p, Pathname_p);
    SmStorage_ClearDir(Pathname_p);

    // Clear the /ifs/messaging/sms/<iccid>/control directory
    SmStorage_GetSMControlPath(StorageVolume_p, Pathname_p);
    SmStorage_ClearDir(Pathname_p);

    // Now delete the Delivery Status Directory
    SmStorage_GetDeliveryStatusPath(StorageVolume_p, Pathname_p);
    SmStorage_ClearDir(Pathname_p);

    if (!strcmp(StorageVolume_p->StorageMediaName, SIM_VOL)) {
        SmStorage_DeleteAllSIMShortMessages();
    }

    SMS_HEAP_FREE(&Pathname_p);

    // Restore current working directory
    if (chdir(SMS_FS_Working_Dir_p) != 0) {
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_DeleteAllThisICCID_Messages() Failed to chdir into %s", SMS_FS_Working_Dir_p));
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DeleteAllOtherICCID_Messages
 *
 * @description  Delete all mesasges for other ICCIDs on the media
 *
 * @param        *StorageVolume_p : The storage media name
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_DeleteAllOtherICCID_Messages(
    MSG_StorageMedia_t * StorageVolume_p)
{
    DIR *Directory_p = NULL;
    struct dirent *DirectoryEntry_p = NULL;
    char *Pathname_p = NULL;
    char *Dirname_p = NULL;
    char *ICCID_p = NULL;

    if (!strcmp(StorageVolume_p->StorageMediaName, SIM_VOL)) {
        SMS_A_(SMS_LOG_W("smstrg.c: Can not delete all other ICCID for SIM volume"));
    } else {
        // Need to determine all the ICCID directories that exist on the specified
        // volume excluding the current ICCID
        Dirname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(sizeof(char) * FILENAME_MAX);

        (void) strcpy(Dirname_p, StorageVolume_p->StorageMediaName);
        (void) strcat(Dirname_p, MESSAGING_SUB_DIR);
        (void) strcat(Dirname_p, SMS_SUB_DIR);

        // Determine this ICCID so we don't splat that directory
        ICCID_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX * sizeof(char));
        SmUtil_ICCID_Get(ICCID_p);

        // Open the directory
        Directory_p = opendir(Dirname_p);

        if (Directory_p) {
            // Get to the first entry
            rewinddir(Directory_p);

            //change to the directory
            if (chdir(Dirname_p) == 0) {
                Pathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(sizeof(char) * FILENAME_MAX);

                while ((DirectoryEntry_p = readdir(Directory_p))) {
                    // Resursive, but do not go into . or ..
                    if (DirectoryEntry_p->d_name[0] != '.') {
                        // Confirm its not this ICCID but is another ICCID
                        if (strcmp(DirectoryEntry_p->d_name, ICCID_p)) {
                            memset(Pathname_p, 0, (sizeof(char) * FILENAME_MAX));
                            // Delete the SM directory
                            (void) strcpy(Pathname_p, Dirname_p);
                            (void) strcat(Pathname_p, "/");
                            (void) strcat(Pathname_p, DirectoryEntry_p->d_name);
                            (void) strcat(Pathname_p, SM_SUB_DIR);
                            SmStorage_ClearDir(Pathname_p);

                            // Now delete the Delivery Status Directory
                            memset(Pathname_p, 0, (sizeof(char) * FILENAME_MAX));
                            (void) strcpy(Pathname_p, Dirname_p);
                            (void) strcat(Pathname_p, "/");
                            (void) strcat(Pathname_p, DirectoryEntry_p->d_name);
                            (void) strcat(Pathname_p, SM_STATUS_REPORT_SUB_DIR);
                            SmStorage_ClearDir(Pathname_p);

                            // Note we dont need to delete the msg directory as it just
                            // contains this ICCIDs messages
                        }
                    }
                }

                // Restore current working directory
                if (chdir(SMS_FS_Working_Dir_p) != 0) {
                    SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_DeleteAllOtherICCID_Messages() Failed to chdir into %s", SMS_FS_Working_Dir_p));
                }
            }

            SMS_HEAP_FREE(&Pathname_p);

            // Now close the directory
            (void) closedir(Directory_p);
        }

        SMS_HEAP_FREE(&Dirname_p);
        SMS_HEAP_FREE(&ICCID_p);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SIMSlotSimUsedSpaceRd
 *
 * @description  get number of used SIM slots
 *
 * @param        void
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_SIMSlotSimUsedSpaceRd(
    void)
{
    return ShortMessageStorage.SimUsedSpace;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SIMSlotSimTotalSpaceRd
 *
 * @description  get number of Total SIM slots
 *
 * @param        void
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_SIMSlotSimTotalSpaceRd(
    void)
{
    return SmStorage_SIMSlotLogicalCapacityRd();
}


/********************************************************************/
/**
 *
 * @function     SmStorage_MEUsedSpaceRd
 *
 * @description  get the total number of ME used slots
 *
 * @param        void
 *
 * @return       uint8_t
 */
/********************************************************************/
uint16_t SmStorage_MEUsedSpaceRd(
    void)
{
    return ShortMessageStorage.MEUsedSpace;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_InitStorageMSG_List
 *
 * @description  Initialise the list of stored message details
 *
 * @param        SmStorageMSG_List_p : MSG_List of all stored short messages.
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_InitStorageMSG_List(
    SmStorageMSG_List_p * thisMessageStorageMSG_List_p)
{
    *thisMessageStorageMSG_List_p = MSG_ListCreate();
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DestroyStorageMSG_List
 *
 * @description  Destroy the list of stored message details
 *
 * @param        SmStorageMSG_List_p : MSG_List of all stored short messages.
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_DestroyStorageMSG_List(
    SmStorageMSG_List_p thisMessageStorageMSG_List_p)
{
    SmStorageObject_t *NextMessage_p = NULL;
    uint8_t ListDestroyed = FALSE;

    // Need to iterate through the list, however, if we attempt to delete
    // each entry as we iterate through the list using the normal list
    // iterator this will break.   This is because the normal list iterator
    // does a get first for the first entry in the list, followed by a get
    // next given the previous entry for all the other entries. However if
    // the previous has just been deleted, the get next will fail as it
    // wont be able to locate the previous entry in the list..Instead, just
    // need to keep doing get first, delete. get first delete etc until get
    // first returns NULL for the short messag indicating there is nothing
    // left in the list.to be deleted.  Note that all we are doing here is
    // removing the storage for each list element (ShortMessage_p) from the
    // list. The storage allocated for the ShortMessage is free in sm.c
    while (!ListDestroyed) {
        NextMessage_p = (SmStorageObject_t *) MSG_ListGetFirst(thisMessageStorageMSG_List_p);

        if (!NextMessage_p) {
            ListDestroyed = TRUE;
        } else {
            SmStorage_RemoveStoredItem(thisMessageStorageMSG_List_p, NextMessage_p);
            SMS_HEAP_FREE(&NextMessage_p);
        }
    }

    // Now destroy the message list itself now that all its entries have
    // been removed
    MSG_ListDestroy(thisMessageStorageMSG_List_p);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_RemoveStoredItem
 *
 * @description  Removes the specifed item from the list
 *
 * @param        SmStorageMSG_List_p : MSG_List of all stored short messages.
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_RemoveStoredItem(
    const SmStorageMSG_List_p thisMessageStorageMSG_List,
    const SmStorageObject_t * thisStoredSm_p)
{
    (void) MSG_ListRemove(thisMessageStorageMSG_List, thisStoredSm_p);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_InsertStoredItem
 *
 * @description  This function inserts a short message details into
 *               the list sorted on the storage postion (logical slot)
 *
 * @param        SmStorageMSG_List_p : MSG_List of all stored short messages.
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_InsertStoredItem(
    const SmStorageMSG_List_p thisMessageStorageMSG_List,
    const SmStorageObject_t * thisStoredSm_p)
{
    (void) MSG_ListInsertSorted(thisMessageStorageMSG_List, thisStoredSm_p, (MSG_ListCompareFunc) SmStorage_CmpStoragePos);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_CmpStoragePos
 *
 * @description  Compares the storage postion of a mesasge that is
 *               allready stored in the list with a new message to be
 *               added to this list.  If the new message slot number
 *               is less than the existing one, this funciton returns
 *               a negative value, otherwise 0 is returned.
 *
 * @param        SmStorageMSG_List_p : Stored message in the message list
 *               SmStorageMSG_List_p : Short Mesasge to be inserted
 *
 * @return       If the new message slot number
 *               is less than the existing one, this funciton returns
 *               a negative value, otherwise 0 is returned.
 *
 */
/********************************************************************/
static int SmStorage_CmpStoragePos(
    const SmStorageObject_t * newStoredSm_p,
    const SmStorageObject_t * StoredSmInMSG_List_p)
{
    if (ShortMessage_GetStoragePosition(StoredSmInMSG_List_p->ShortMessage) > ShortMessage_GetStoragePosition(newStoredSm_p->ShortMessage)) {
        return -1;
    }

    return 1;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_IterateFromStart
 *
 * @description  Iterates through the entire message details list
 *
 * @param        SmStorageMSG_List_p    : The list of message details to
 *               SmStorageDetails_t : Last entry checked
 *
 * @return       SmStorageDetails_t* pointer to stored message detail
 */
/********************************************************************/
static SmStorageObject_t *SmStorage_IterateFromStart(
    SmStorageMSG_List_p thisStorageMSG_List,
    SmStorageObject_t * previousMessage_p)
{
    SmStorageObject_t *nextMessage_p = NULL;

    if (thisStorageMSG_List != NULL) {
        if (previousMessage_p == NULL) {
            //if previousMessage has been set to NULL we must be looking
            //for the first message so don't bother looking for it
            //first time so find the first bit we've got
            nextMessage_p = (SmStorageObject_t *) MSG_ListGetFirst(thisStorageMSG_List);
        } else {
            //not the first time so find one that is next after previousMessage
            nextMessage_p = MSG_ListGetNext(thisStorageMSG_List, previousMessage_p);
        }
    }

    return (nextMessage_p);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_IterateFromEnd
 *
 * @description  Function to iterate the message detials from the end for a message
 *
 * @param        thisMessagePool : as type of MessagePool_p.
 * @param        previousMessage : as type of Message_p.
 *
 * @return       static Message_p
 */
/********************************************************************/
static ShortMessage_p SmStorage_IterateFromEnd(
    SmStorageMSG_List_p thisStorageMSG_List,
    ShortMessage_p previousMessage)
{
    if (!previousMessage) {
        //if previousMessage has been set to NULL we must be looking
        //for the first message so don't bother looking for it
        return (ShortMessage_p) MSG_ListGetLast(thisStorageMSG_List);
    }
    //not the first time so find one that is before after previousMessage
    return MSG_ListGetPrev(thisStorageMSG_List, previousMessage);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetNextMessage
 *
 * @description  Use to get the message following this message. This
 *               is effectively a wrapper for the IterateFromStart
 *               function to allow clients external to message storage
 *               to loop through all of the stored short messages.
 *
 * @param        ShortMessage_p    : Current short message
 *
 * @return       ShortMessage_p    : The following short message
 */
/********************************************************************/
ShortMessage_p SmStorage_GetNextMessage(
    ShortMessage_p thisShortMessage)
{
    SmStorageObject_t *StorageObject_p = NULL;

    // The list is not a list of short messages anymore, but a list of
    // Storage Objects (each of which cotains a short message pointer).
    // So before we call the iterator we need to determine the entry
    // that the short message is in

    if (!thisShortMessage) {
        // Just get the first one
        StorageObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StorageObject_p);
    } else {
        // Loop until we find the StorageObject for the supplied short message
        while ((StorageObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StorageObject_p))) {
            if ((StorageObject_p->ShortMessage == thisShortMessage)) {
                // Then get the next one and quit
                StorageObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StorageObject_p);
                break;
            }
        }
    }

    return (!StorageObject_p ? NULL : StorageObject_p->ShortMessage);

}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetPreviousMessage
 *
 * @description  Use to get the message from end of message list. This
 *               is effectively a wrapper for the IterateFromEnd
 *               function to allow clients external to message storage
 *               to loop through all of the stored short messages from
 *               the end of short message list.
 *
 * @param        ShortMessage_p    : Current short message
 *
 * @return       ShortMessage_p    : The following short message
 */
/********************************************************************/
ShortMessage_p SmStorage_GetPreviousMessage(
    ShortMessage_p thisShortMessage)
{
    return SmStorage_IterateFromEnd(ShortMessageStorage.SmStorageMSG_List, thisShortMessage);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_StatusUpdate
 *
 * @description  Inform the phone System of SmStorage Status
 *
 * @param        void
 *
 * @return       void
 *
 * NOTES         System operates on a media basis, thus required to determine
 *               the status of each Sms Storage Media
 */
/********************************************************************/
void SmStorage_StatusGet(
    SMS_StorageStatus_t * StorageStatus_p)
{
    uint16_t AvailableSize;
    MSG_StorageMedia_t *AvailableMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

    if (AvailableMedia_p) {
        StorageStatus_p->StorageFullME = !SmStorage_GetAvailableVolume(AvailableMedia_p, 1, TRUE);
        StorageStatus_p->StorageFullSIM = !SmStorage_FindSpace((MSG_StorageMedia_t *) SIM_VOL, 1, 0, &AvailableSize);

        SMS_HEAP_FREE(&AvailableMedia_p);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ReinitialiseStorage
 *
 * @description  Reinitialised storage by re-analyising the all the
 *               short messages in the preferred storaage list
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
void SmStorage_ReinitialiseStorage(
    void)
{
    ShortMessageStorage.StorageListRetrieved = FALSE;
    ShortMessageStorage.SimUsedSpace = 0;
    ShortMessageStorage.MEUsedSpace = 0;

    (void) SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT, sizeof(uint16_t), &ShortMessageStorage.MEUsedSpace);

    ShortMessageStorage.MEInitialisationCount = 0;

    //Reinitialise SmStorage Capacity
    SmStorage_SIMSlotLogicalCapacityWr(SmStorage_SimEfFileSizeRd(EF_SMS_FILENAME));
    SmStorage_DestroyStorageMSG_List(ShortMessageStorage.SmStorageMSG_List);

    //Reinitialise SR Storage Capacity and cached data
    SmStorage_SIM_SR_LogicalCapacityWr(SmStorage_SimEfFileSizeRd(EF_SMSR_FILENAME));
    AllSRsCached = FALSE;       // this will enable caching of EF_SMSR if present
    memset(StatusReportIsEmpty, FALSE, EF_SMSR_MAX_RECS);
    if (NULL != SIM_StatusReports_p) {  // Free temporary SR storage
        SMS_HEAP_FREE(&SIM_StatusReports_p);
        SIM_StatusReports_p = NULL;
    }
    // Now create a new message list to hold the new list of messages
    // determined by the reinitialiseation procedure
    SmStorage_InitStorageMSG_List(&ShortMessageStorage.SmStorageMSG_List);

    //Commence retrieval of EfSms Record(s) from storage
    Do_SMS_TimerSet_1_MS_WCT(MSG_SMSTORAGE_INIT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_STORAGE_INIT_TIMER_VALUE);
    ShortMessageStorage_WriteState(SMSTORAGESTATE_VALIDATE_SM);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EventTimeout_SmValidate
 *
 * @description  Verifies the Contents of the next EfSms Record within the next Slot,
 *               awaiting verification. In the event that the EfSmsRecord encapsulates
 *               a Short Message, a Short Message Object will be created.
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_EventTimeout_SmValidate(
    void)
{
    ShortMessage_p SmInstance;

    static MSG_StoreList_t PreferredStorageList = { 0, NULL };
    static uint8_t SimValidationComplete;
    static uint8_t ControlValidationComplete;
    static MSG_StorageMedia_t StorageVolume;
    static uint8_t StorageListOffset;
    static uint8_t ValidationComplete;
    static uint8_t MediaRestored;

    if (!ShortMessageStorage.StorageListRetrieved) {
        // Only want to get the storage list once as it allocates memory. Also
        // need to free it if we are re-initialising it.
        if (PreferredStorageList.MSGStorageList_p) {
            SMS_HEAP_FREE(&PreferredStorageList.MSGStorageList_p);
        }

        ShortMessageStorage.StorageListRetrieved = SmStorage_GetStorageList(&PreferredStorageList);
        (void) strcpy(StorageVolume.StorageMediaName, SIM_VOL);

        ValidationComplete = FALSE;

        if (SMS_VALIDATE_SIM_STORED_SMS) {
            SimValidationComplete = FALSE;
            SmStorage_SIMSlotVerifiedCapacityWr(0);
            SmStorage_SIM_SR_VerifiedCapacityWr(0);
        } else {
            SimValidationComplete = TRUE;
            SmStorage_SIMSlotVerifiedCapacityWr(SmStorage_SIMSlotLogicalCapacityRd());
            SmStorage_SIM_SR_VerifiedCapacityWr(SmStorage_SIM_SR_LogicalCapacityRd());
        }

        // Initialise Control file if required
        SmStorage_InitControlVolume();
        ControlValidationComplete = FALSE;
        StorageListOffset = 0;
    }

    SmInstance = NULL;

    // Validation is done in the order of Control directory, SIM followed by preferred storage
    // list (excluding SIM)
    if (!ValidationComplete) {
        if (!ControlValidationComplete) {
            // Validate Control Directory
            SmStorage_RestoreSmFromVolume((MSG_StorageMedia_t *) SM_CONTROL_DIR, &SmInstance, &MediaRestored);

            if (MediaRestored) {
                ControlValidationComplete = TRUE;
            }
        } else if (!SimValidationComplete) {    // Validate SIM
            SmStorage_RestoreSIMVolume(&SmInstance, &MediaRestored);

            if (MediaRestored) {
                SimValidationComplete = TRUE;
            }
        } else if (!ValidationComplete) {
            // Validate all the file based storage
            if (ShortMessageStorage.StorageListRetrieved) {
                // Need to determine the next storage media to validate
                if (MediaRestored) {
                    // We need to get a new media volume to validate before progressing
                    uint8_t NewMediaFound = FALSE;

                    MediaRestored = FALSE;

                    while (!NewMediaFound) {
                        if (StorageListOffset >= PreferredStorageList.NumStorageMedia) {
                            // We have done all the volumes in the preferred storage list, were done
                            NewMediaFound = TRUE;
                            ValidationComplete = TRUE;

                            SMS_HEAP_FREE(&PreferredStorageList.MSGStorageList_p);
                            PreferredStorageList.NumStorageMedia = 0;
                            PreferredStorageList.MSGStorageList_p = NULL;
                        }

                        if (!ValidationComplete) {
                            // Don't process SIM volume here as we've already done SIM
                            if (strcmp(PreferredStorageList.MSGStorageList_p[StorageListOffset].StorageMediaName, SIM_VOL)) {
                                (void) strcpy(StorageVolume.StorageMediaName, PreferredStorageList.MSGStorageList_p[StorageListOffset].StorageMediaName);

                                NewMediaFound = TRUE;
                            }
                        }

                        StorageListOffset++;
                    }
                }
            } else {
                ValidationComplete = TRUE;
            }

            if (!ValidationComplete) {
                // Restore the next short message from the volume
                SmStorage_RestoreSmFromVolume(&StorageVolume, &SmInstance, &MediaRestored);

                if (MediaRestored) {
                    SMS_B_(SMS_LOG_D("smstrg.c: SmStorageInitialise: Validation complete, trying next volume"));
                }
            }
        }

        ShortMessageStorage_WriteState(SMSTORAGESTATE_VALIDATE_SM);

        //Set timer to initialise next Slot
        Do_SMS_TimerSet_1_MS_WCT(MSG_SMSTORAGE_INIT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_STORAGE_INIT_TIMER_VALUE);
    } else {
        SMS_StorageStatus_t NewStorageStatus;

        //EfSms File Initialised.
        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_EventTimeout_SmValidate - SM Initialisation Complete"));
        SMS_B_(SMS_LOG_D("smstrg.c: SIM STORAGE CAPACITY %d SLOTS", SmStorage_SIMSlotVerifiedCapacityRd()));

        // Process replace type SMS received during initialisation
        SmStorage_ProcessPendingReplaceTypes();

        // No more messages to check
        MessagingSystem_SetState(MESSAGING_SYSTEM_IDLE);
        ShortMessageStorage_WriteState(SMSTORAGESTATE_INITIALISE_COMPLETE);

        // SM storage initialised so instruct upper layers to process any received Status Reports.
        ShortMessageRouter_HandleEvent(SHORTMESSAGEROUTER_PROCESS_RECEIVED_STATUS_REPORTS, NULL);

        // Inform clients if the storage status has changed
        SmStorage_AnalyseStorage(&NewStorageStatus);

        if (!SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND && !(NewStorageStatus.StorageFullSIM && NewStorageStatus.StorageFullME)) {
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_EventTimeout_SmValidate: Network Memory Capacity Available"));
            ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, NULL);
        }
        // We have completed restoring the messages, now check that the number actually
        // stored on the ME matches SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT
        if (ShortMessageStorage.MEUsedSpace != ShortMessageStorage.MEInitialisationCount) {
            (void) SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT, sizeof(uint16_t), &ShortMessageStorage.MEInitialisationCount);
            ShortMessageStorage.MEUsedSpace = ShortMessageStorage.MEInitialisationCount;
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EventTimeout_SIMRefresh
 *
 * @description  Handling the SIM SM storage reinitialisation after a SIM
 *               SAT Refresh and SIM active state changed event
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_EventTimeout_SIMRefresh(
    void)
{
    uint8_t Complete = FALSE;
    ShortMessage_p SmInstance = NULL;

    SmStorage_RestoreSIMVolume(&SmInstance, &Complete);

    if (Complete) {
        SMS_StorageStatus_t NewStorageStatus;

        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_EventTimeout_SIMRefresh:complete"));

        MessagingSystem_SetState(MESSAGING_SYSTEM_IDLE);
        ShortMessageStorage_WriteState(SMSTORAGESTATE_INITIALISE_COMPLETE);

        // Inform clients if the storage status has changed
        SmStorage_AnalyseStorage(&NewStorageStatus);

        if (!SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND && !(NewStorageStatus.StorageFullSIM && NewStorageStatus.StorageFullME)) {
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_EventTimeout_SIMRefresh: Network Memory Capacity Available"));
            ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, NULL);
        }
    } else {
        if (SmStorage_SIM_SMR_ShortMsg_Read_Available_Get()) {
            // Try again if the service is available but failed
            Do_SMS_TimerSet_1_MS_WCT(MSG_SMSTORAGE_INIT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_STORAGE_INIT_TIMER_VALUE);
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EventTimeout_SIMRestart
 *
 * @description  Handling the SIM SM storage reinitialisation after a SIM
 *               Power Off state changed event
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_EventTimeout_SIMRestart(
    void)
{
    uint8_t Complete = FALSE;
    ShortMessage_p SmInstance = NULL;

    SmStorage_RestoreSIMVolume(&SmInstance, &Complete);

    if (Complete) {
        SMS_StorageStatus_t NewStorageStatus;

        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_EventTimeout_SIMRestart:complete"));

        MessagingSystem_SetState(MESSAGING_SYSTEM_IDLE);
        ShortMessageStorage_WriteState(SMSTORAGESTATE_INITIALISE_COMPLETE);

        // Inform clients if the storage status has changed
        SmStorage_AnalyseStorage(&NewStorageStatus);

        if (!SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND && !(NewStorageStatus.StorageFullSIM && NewStorageStatus.StorageFullME)) {
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_EventTimeout_SIMRestart: Network Memory Capacity Available"));
            ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, NULL);
        }
    } else {
        if (SmStorage_SIM_SMR_ShortMsg_Read_Available_Get()) {
            // Try again if the service is available but failed
            Do_SMS_TimerSet_1_MS_WCT(MSG_SMSTORAGE_INIT_TIMER_IND, SMS_TIMER_DEFAULT_CLIENT_TAG, SMS_STORAGE_INIT_TIMER_VALUE);
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_RestoreSmFromVolume
 *
 * @description  Restores a short message from the specified volume
 *
 * @param        *StorageVolume_p   : The volume that this directory is on
 * @param        *SmInstance_p      : Holds the sm that is restored
 * @param        *RestoreComplete_p : Indicates if all short message restored
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_RestoreSmFromVolume(
    const MSG_StorageMedia_t * const StorageVolume_p,
    ShortMessage_p * SmInstance_p,
    uint8_t * RestoreComplete_p)
{
    FILE *File_p = NULL;
    static uint16_t FileIndex = 0;
    static SMS_Position_t SlotRangeOffset = SMS_STORAGE_POSITION_INVALID;
    char *SmPathFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);
    EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;
    uint8_t SmRestored = FALSE;
    SmStorageObject_t *StorageObject_p = NULL;
#ifdef PLATFORM_ANDROID
    // Android compiler complains about initialising the stat structures using the (valid)
    // C constrution in the #else case. Therefore we have to do it another way instead.
    // Android compiler warnings are (#### is line where struct stat TimeStamp = {0}; was located):
    // smstrg.c:####: warning: missing initializer
    // smstrg.c:####: warning: (near initialization for 'TimeStamp.__pad0')
    struct stat TimeStamp;
    memset(&TimeStamp, 0x00, sizeof TimeStamp);
#else
    struct stat TimeStamp = { 0 };
#endif

    *RestoreComplete_p = FALSE;

//////////////////////////////////////////////////////////////////////
// First establish if we have no pre-existing sm container file from
// which we can restore messaging's ram list. In this case we should
// try to create a new container file. This section should only be
// be executed the first time a new directory structure is passed in
//////////////////////////////////////////////////////////////////////
    //If the current file handle is not valid assume its the first time for this directory structure
    if (!File_p) {
        //Build the full path name for our container file in the passed in directory structure
        SlotRangeOffset = SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, StorageVolume_p, SmPathFilename_p);
        SMS_B_(SMS_LOG_D("smstrg.c SmStorage_RestoreSmFromVolume: SmPathFilename_p=%s SlotRangeOffset=%d", SmPathFilename_p, SlotRangeOffset));

        //Try to open our short message container file "shortmessages.tpdu"
        if (!(File_p = fopen(SmPathFilename_p, "rb"))) {
            //If we have failed to open it signal there is nothing to restore...
            *RestoreComplete_p = TRUE;

            //...and try to create/intialise a new short message container file "shortmessages.tpdu"
            if (!(File_p = fopen(SmPathFilename_p, "wb"))) {
                //we have failed to create the file so we must at least report it through the debug system
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreSmFromVolume - Failed to create sm container file \"%s\"", SmPathFilename_p));
            } else {
                //we have created the new container file so just close it and get out of restore
                (void) fclose(File_p);
            }

            File_p = NULL;
        }
    }
//////////////////////////////////////////////////////////////////////
// Second if we have a pre-existing sm container file from
// which we can restore messaging's ram list we should start the
// process to restore all short messages from persistant memory
// to the messaging system's ram list. This section should be
// executed as many times as necessary to extract all the TPDUs
// from the container file
//////////////////////////////////////////////////////////////////////
    //If we have a pre existing short message container file with messages to restore
    if (File_p && !(*RestoreComplete_p)) {
        uint8_t RestoreStorageIndex;

        (void) SmStorage_GetVolumeIndex(StorageVolume_p, &RestoreStorageIndex);
        SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, FileIndex);

        //Try to restore the short message list from the file (ie. persistant memory)
        //Get the next tpdu entry from the container file until eof reached
        if (fread(EfSmsRecord_p, EFSMS_RECORD_SIZE, 1, File_p)
            && fread(&TimeStamp, sizeof(struct stat), 1, File_p)) {
            SmStorageObject_t *LocalStorageObject_p = NULL;
            //Scan the restored sm list for a match...
            while ((LocalStorageObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, LocalStorageObject_p))) {
                //If matched in the sm list we ignore it
                if (LocalStorageObject_p->FileIndex == FileIndex && LocalStorageObject_p->StorageIndex == RestoreStorageIndex) {
                    // Signal this restore is not required as already restored in sm list
                    SmRestored = TRUE;
                    break;
                }
            }
            //If not already matched in the sm list and it is a valid sm...
            if (!SmRestored && !SmStorage_SmInvalidTPDU(*EfSmsRecord_p)) {      //...then restore it
                // Use the FileIndex and the SlotRangeOffset to determine the previous slot
                // number used as this is an exising cached record
                if (SMS_STORAGE_POSITION_INVALID != SlotRangeOffset) {
                    SlotNumber = FileIndex + SlotRangeOffset;
                } else {
                    SMS_A_(SMS_LOG_E
                           ("smstrg.c: SmStorage_RestoreSmFromVolume: SlotRangeOffset has not been set for FileIndex %d, \"%s\"", FileIndex,
                            (SmPathFilename_p ? SmPathFilename_p : "- ERROR NO PATH -")));
                }

                // Restore It
                *SmInstance_p = SmStorage_SmRestore(SlotNumber, *EfSmsRecord_p);

                if (*SmInstance_p) {
                    StorageObject_p = (SmStorageObject_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SmStorageObject_t));

                    StorageObject_p->StorageIndex = RestoreStorageIndex;

                    // set the file index
                    StorageObject_p->FileIndex = FileIndex;
                    StorageObject_p->ShortMessage = *SmInstance_p;
                    StorageObject_p->PendingFlag = FALSE;

                    SmStorage_SlotAllocate(SlotNumber, StorageObject_p);
                    ShortMessage_SetStoragePosition(StorageObject_p->ShortMessage, SlotNumber);
                    SmStorage_ValidateDeliveryStatus(SlotNumber, *SmInstance_p);

                    SMS_B_(SMS_LOG_D
                           ("smstrg.c: SmStorage_RestoreSmFromVolume: Restored SM with Slot %d from FileIndex %d, \"%s\"", SlotNumber, FileIndex,
                            (SmPathFilename_p ? SmPathFilename_p : "- ERROR NO PATH -")));
                }
            }
            //Increment the file index
            FileIndex++;
        } else {
            //we have failed to read the next tpdu entry so assume there is nothing else to restore
            *RestoreComplete_p = TRUE;

            // Reset the index for this container
            FileIndex = 0;

        }
        // Always close the file as SMS received during initialisaton need to be written to it
        (void) fclose(File_p);
        File_p = NULL;
    }

    SMS_HEAP_FREE(&EfSmsRecord_p);
    SMS_HEAP_FREE(&SmPathFilename_p);
}


/********************************************************************/
/**
 *
 * @function     ShortMessage_DeleteMatchingReplaceTypeMessage
 *
 * @description  Deletes a short message that matches the given replace
 *               type and the SC and origination addresses of the given
 *               short message
 *
 * @param        SmInstance  : short message to match.
 * @param        ReplaceType : uint8_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t ShortMessage_DeleteMatchingReplaceTypeMessage(
    ShortMessage_p SmInstance,
    uint8_t replaceType)
{
    ShortMessage_p ShortMessageToReplace_p = NULL;

    while ((ShortMessageToReplace_p = SmStorage_GetNextMessage(ShortMessageToReplace_p))) {
        if (ShortMessage_GetReplaceType(ShortMessageToReplace_p) == replaceType) {
            // a short message in this slot is of the correct replace type
            if (ShortMessage_ServiceCentreAddressCompare(SmInstance, ShortMessageToReplace_p)
                && ShortMessage_OrigDestAddressCompare(SmInstance, ShortMessageToReplace_p)
                && ShortMessage_GetStoragePosition(SmInstance) != ShortMessage_GetStoragePosition(ShortMessageToReplace_p)) {
                // service centres and orignating addresses match and it is different
                // from the short message that is to be matched
                SMS_A_(SMS_LOG_I("smstrg.c: REPLACING SHORT MESSAGE"));
                // delete the matching replace type short message
                ShortMessage_Delete(ShortMessageToReplace_p);
                return TRUE;
            }
        }
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ProcessPendingReplaceTypes
 *
 * @description  Process any Replace Type SMS received during initialisation
 *               by deleting any matching previously received SMS from storage
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_ProcessPendingReplaceTypes(
    void)
{
    SmStorageObject_t *StoredObject_p = NULL;

    // First off, locate a pending entry
    while ((StoredObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StoredObject_p))) {
        if (StoredObject_p->PendingFlag) {
            uint8_t ReplaceType = ShortMessage_GetReplaceType(StoredObject_p->ShortMessage);

            if (ReplaceType) {
                // this is a replace type so delete any matching short message which has the same replace type
                (void) ShortMessage_DeleteMatchingReplaceTypeMessage(StoredObject_p->ShortMessage, ReplaceType);
            }

            StoredObject_p->PendingFlag = FALSE;
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_CreateUniqueSmFilename
 *
 * @description  Generates a filename for a short message
 *               by searching the given path for an available
 *               filename base on a unique Storage Index.
 *
 * @param        FileIndex_p  : File Index used in SM filename
 * @param        SmPathname_p : Pathname for the short message
 * @param        SmFilename_p : Filename for the short mesasge
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_CreateUniqueSmFilename(
    uint16_t * const FileIndex_p,
    char *SmPathname_p,
    char *SmFilename_p)
{
    uint8_t UniqueFilenameFound = FALSE;
    uint16_t LastUsedFileNumber = 0;
    uint16_t LastFreeFileNumber = 0;

    while (!UniqueFilenameFound) {
        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_CreateUniqueSmFilename - searching for FileIndex = %05d...", *FileIndex_p));
        // create a default filename to start the search with.
        SmStorage_Construct_SM_Filename(*FileIndex_p, SmFilename_p);

        if (isUniqueStorageNumber(SmPathname_p, SmFilename_p))  // this Storage Index is unique
        {
            if (!LastUsedFileNumber) {
                UniqueFilenameFound = TRUE;     //stop searching as the target Storage Index provided is available
            } else if (*FileIndex_p == LastUsedFileNumber + 1) {
                UniqueFilenameFound = TRUE;     //stop searching this is the next available Storage Index
            } else {
                SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_CreateUniqueSmFilename - Overshot FileIndex = %05d", *FileIndex_p));
                // found a unique filename but overshot the first available Storage Index
                LastFreeFileNumber = *FileIndex_p;

                *FileIndex_p = LastFreeFileNumber - ((LastFreeFileNumber - LastUsedFileNumber) / 2);
            }
        } else {
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_CreateUniqueSmFilename - Duplicate FileIndex = %05d", *FileIndex_p));

            LastUsedFileNumber = *FileIndex_p;

            if (LastFreeFileNumber) {
                if (LastUsedFileNumber + 1 == LastFreeFileNumber) {
                    (*FileIndex_p)++;
                } else {
                    *FileIndex_p = LastUsedFileNumber + ((LastFreeFileNumber - LastUsedFileNumber) / 2);
                }
            } else {
                *FileIndex_p = LastUsedFileNumber * 2;  //search again using a new Storage Index
            }
        }
    }

    SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_CreateUniqueSmFilename - Allocated FileIndex = %05d", *FileIndex_p));
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetNextFreeSRRecord
 *
 * @description  Returns the record number
 *
 * @param        fSlotNumber         : The SIM slot number
 * @param        fStatusReportTPDU_p : The Status report to write
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_GetNextFreeSRRecord(
    void)
{
    uint8_t RecordNumber = ARRAY_OFFSET;        // there is no record 0
    uint8_t MatchFound = FALSE;

    while (SmStorage_SIM_SR_LogicalCapacityRd() >= RecordNumber && !MatchFound) {
        if (StatusReportIsEmpty[ArrayIndex(RecordNumber)] == TRUE) {    // we found a free SR record on the SIM
            MatchFound = TRUE;
        } else {                // see if next record is free
            RecordNumber++;
        }
    }

    if (!MatchFound) {          // There were no free SR records on the SIM
        RecordNumber = 0;
    }

    return RecordNumber;
}

/********************************************************************/
/**
 *
 * @function     SmStorage_StoreSIMDeliveryStatus
 *
 * @description  Store a SR to the SIM
 *
 * @param        fSlotNumber         : The SIM slot number
 * @param        fStatusReportTPDU_p : The Status report to write
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_StoreSIMDeliveryStatus(
    const SMS_Position_t fSlotNumber,
    SMS_Position_t * const fSRSlotNo,
    SMS_StatusReport_t * const fStatusReportTPDU_p)
{
    uint8_t lResult = FALSE;

    *fSRSlotNo = SMS_STORAGE_POSITION_INVALID;

    if (SMS_STORAGE_POSITION_INVALID != fSlotNumber && NULL != fStatusReportTPDU_p) {   // input parameters okay
        uint8_t RecordNumber = SmStorage_GetNextFreeSRRecord();
        uint8_t Data[STATUS_REPORT_REC_SIZE];
        ste_uicc_update_sim_file_record_response_t update_result = {
            STE_UICC_STATUS_CODE_FAIL,
            STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
            {0x00, 0x00}
        };

        if (0 != RecordNumber) {
            ste_sim_t *uicc = MessagingSystem_UICC_SimSyncGet();

            // valid record ID obtained
            // Copy SR into SIM interface type
            Data[0] = (uint8_t) fSlotNumber;
            memcpy(&Data[SMSR_TPDU_OFFSET], fStatusReportTPDU_p->StatusReport, STATUS_REPORT_REC_SIZE - SMSR_TPDU_OFFSET);

            if (uicc) {
                uint8_t retval = ste_uicc_sim_file_update_record_sync(uicc,
                                                                      0,        /*!! client tag */
                                                                      EF_SMSR_FILENAME,
                                                                      RecordNumber,
                                                                      STATUS_REPORT_REC_SIZE,
                                                                      EF_SMS_FILEPATH,
                                                                      Data,
                                                                      &update_result);
                if (retval == 0 && update_result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
                    lResult = TRUE;
                    StatusReportIsEmpty[ArrayIndex(RecordNumber)] = FALSE;      // mark SR record as written
                    *fSRSlotNo = RecordNumber;

                    SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_StoreSIMDeliveryStatus - TPDU written to SIM, SM Slot No = %d EF_SMSR rec %d", fSlotNumber, RecordNumber));
                } else {
                    SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_StoreSIMDeliveryStatus slot = %d EF_SMSR rec = %d", fSlotNumber, RecordNumber));
                }
            }
        }
    }

    return lResult;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ExtendStatusReportFile
 *
 * @description  Extend the Status Report container file (if required)
 *               to hold the given number of entries if required
 *
 * @param        File_p            : Status Report container file
 * @param        NumberOfSrEntries : Number of entries to extend to
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_ExtendStatusReportFile(
    FILE * File_p,
    const uint16_t NumberOfSrEntries)
{
    // Setup to measure the length of the container file
    if (!fseek(File_p, 0, SEEK_END)) {
        uint32_t FileLen = ftell(File_p);
        uint32_t DummyEntries = 0;
        uint32_t NumOfEntries = 0;

        // Is the container file big enough to contain the current slot number
        if (FileLen < NumberOfSrEntries * STATUS_REPORT_FILE_ENTRY_LENGTH) {
            uint8_t *lDummyStatus_p;

            if (FileLen) {
                NumOfEntries = (uint32_t) (FileLen / STATUS_REPORT_FILE_ENTRY_LENGTH);  //Number of SR entries in container file now
            }

            DummyEntries = NumberOfSrEntries - NumOfEntries;    //Number of dummy SRs to write to make container big enough

            // Set up a dummy block of data for expanding the container file.
            lDummyStatus_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(STATUS_REPORT_FILE_ENTRY_LENGTH);

            memset(lDummyStatus_p, 0xff, STATUS_REPORT_FILE_ENTRY_LENGTH);
            memset(lDummyStatus_p, STATUS_REPORT_UNKNOWN, 1);

            // Write the dummy block as many times as is necessary to make the container big enough
            // to contain the current slot number starting from the end of the last valid file slot
            // (the effective end of file in case any corruption lies beyond)
            if (!fseek(File_p, (NumOfEntries * STATUS_REPORT_FILE_ENTRY_LENGTH), SEEK_SET)) {
                while (DummyEntries--) {
                    if (fwrite(lDummyStatus_p, STATUS_REPORT_FILE_ENTRY_LENGTH, 1, File_p) != 1) {
                        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_ExtendStatusReportFile Failed to extend the container file for correct position!"));
                        break;
                    }
                }
            } else {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_ExtendStatusReportFile: Failed to locate the start position for dummy entries"));
            }

            SMS_HEAP_FREE(&lDummyStatus_p);
        } else {
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_ExtendStatusReportFile container file length %d, required entries %d", FileLen, NumberOfSrEntries));
        }
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_ExtendStatusReportFile: Failed to locate end of container file!"));
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_WriteStatusReportToCache
 *
 * @description  Copy a SIM stored SR to the cache , if one exists
 *
 * @param        SlotNumber          : The Slot Number for the SR
 * @param        fStatusReportTPDU_p : The SR to write
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_WriteStatusReportToCache(
    const SMS_Position_t fSlotNumber,
    SMS_StatusReport_t * const fStatusReportTPDU_p)
{
    uint8_t lResult = FALSE;
    ///////////////////////////////////////////////////////////////////////////////////
    // First get the container file name for the status report tpdus and make sure it
    // can be opened for writing and have its insertion point set to the correct
    // position according to the status reports slot number.
    ///////////////////////////////////////////////////////////////////////////////////
    char *lFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    if (!lFilename_p) {
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteStatusReportToCache - Failed to allocate filename!"));
        lResult = FALSE;
    } else {
        FILE *lFile_p = NULL;
        uint16_t FileSlotIndex = SMSTORAGE_FILEINDEX_INVALID;

        memset(lFilename_p, 0, SM_FULL_FILENAME_BYTES);

        (void) SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SR, fSlotNumber, lFilename_p, &FileSlotIndex);

        //////////////////////////////////////////////////////
        // First see if we can open an existing file to add to...
        //////////////////////////////////////////////////////
        if (!(lFile_p = fopen(lFilename_p, "rb+"))) {
            SMS_B_(SMS_LOG_E("smstrg.c: Failed to open file for adding. Will try to create one!"));
        }
        //////////////////////////////////////////////////////
        // ...If file didn't exist try to create a new one
        ///////////////////////////////////////////////////////
        if (!lFile_p && !(lFile_p = fopen(lFilename_p, "wb"))) {
            //We don't have a file to work with
            SMS_A_(SMS_LOG_E("smstrg.c: Failed to create \"%s\" file for writing!", lFilename_p));
        } else {
            //We have a file to work with
            SMS_DeliveryStatus_t lTPDUDeliveryStatus;
            DeliveryStatus_t lDeliveryStatus = STATUS_REPORT_UNKNOWN;
            Tpdu_t *lBufferTPDU_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

            SMS_B_(SMS_LOG_D("smstrg.c: Successfully create/opened file for writing"));
            // Extend the Status Report container file (if required) to be able to hold
            // FileSlotIndex number of entries
            SmStorage_ExtendStatusReportFile(lFile_p, FileSlotIndex);

            // Index the Status Report
            SmStorage_Index_Entry(SMSTORAGECONTAINER_SR, lFile_p, FileSlotIndex);

            // Status Report container file should be open, extended to contain
            // FileSlotIndex number of status report entries and ready to write
            // the new entry
            if (lBufferTPDU_p) {
                // TPDU_GetField requires a TPDU buffer so copy the Status Report into a TPDU buffer
                memset(lBufferTPDU_p, 0x00, sizeof(Tpdu_t));
                memcpy(lBufferTPDU_p, fStatusReportTPDU_p->StatusReport, SMS_STATUS_REPORT_TPDU_MAX_LENGTH);

                // contains the SMS_DeliveryStatus_t short version of the delivery
                // status and then write the 29-byte TPDU after this.
                lTPDUDeliveryStatus = TPDU_GetField(TP_ST, (uint8_t *) lBufferTPDU_p, SM_STATUS_REPORT);
                lDeliveryStatus = TPStatusToStatus(lTPDUDeliveryStatus);
                SMS_HEAP_FREE(&lBufferTPDU_p);
            }
            ///////////////////////////////////////////////////////////////////////////////////
            // Now the delivery status and the Status report tpdu are formatted and
            // ready to be written to the container file
            ///////////////////////////////////////////////////////////////////////////////////
            if (fwrite(&lDeliveryStatus, sizeof(DeliveryStatus_t), 1, lFile_p)
                && fwrite(fStatusReportTPDU_p, sizeof(SMS_StatusReport_t), 1, lFile_p)) {
                if (!fclose(lFile_p)) {
                    lResult = TRUE;
                }
            } else {
                SMS_A_(SMS_LOG_E("smstrg.c: Failed to write status report to file!"));
                (void) fclose(lFile_p);
            }
        }

        SMS_HEAP_FREE(&lFilename_p);
    }

    return lResult;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_RestoreSIMDeliveryStatus
 *
 * @description  Copy a SIM stored SR to the cache , if one exists
 *
 * @param        SlotNumber :         The SIM slot number
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_RestoreSIMDeliveryStatus(
    const SMS_Position_t fSlotNumber,
    ShortMessage_p SmInstance)
{
    uint8_t lResult = TRUE;
    uint8_t *lSMS_SMRStatusReport_p = NULL;

    lSMS_SMRStatusReport_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(STATUS_REPORT_REC_SIZE);

    if (lSMS_SMRStatusReport_p != NULL) {
        // initialised record data
        memset(lSMS_SMRStatusReport_p, 0xFF, STATUS_REPORT_REC_SIZE);
        *lSMS_SMRStatusReport_p = EFSMSR_RECORD_STATUS_FREE_MASK;

        SmStorage_GetStatusReportForSlotNo(fSlotNumber, lSMS_SMRStatusReport_p, SmInstance);

        // Check TPDU Read back is not empty
        if (EFSMSR_RECORD_STATUS_FREE_MASK != *lSMS_SMRStatusReport_p && 0xff != *(lSMS_SMRStatusReport_p + SMSR_TPDU_OFFSET)) {
            SMS_StatusReport_t *lMSGTpduStatusReport_p = (SMS_StatusReport_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_StatusReport_t));

            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_GetStatusReportForSlotNo - SR Read for Slot Number = %d", fSlotNumber));

            if (lMSGTpduStatusReport_p != NULL) {
                // Copy SR into SMS_StatusReport_t type
                memcpy(lMSGTpduStatusReport_p->StatusReport, lSMS_SMRStatusReport_p + SMSR_TPDU_OFFSET, SMS_STATUS_REPORT_TPDU_MAX_LENGTH);

                lResult = SmStorage_WriteStatusReportToCache(fSlotNumber, lMSGTpduStatusReport_p);

                if (lResult) {  // Read the delivery status from SR
                    SMS_DeliveryStatus_t TPDUDeliveryStatus;
                    DeliveryStatus_t DeliveryStatus = STATUS_REPORT_RESET;

                    TPDUDeliveryStatus = TPDU_GetField(TP_ST, lMSGTpduStatusReport_p->StatusReport, SM_STATUS_REPORT);
                    DeliveryStatus = TPStatusToStatus(TPDUDeliveryStatus);

                    // update the short message delivery status based on the status report delivery status
                    ShortMessage_SetCacheDeliveryStatus(SmInstance, DeliveryStatus);
                }

                SMS_HEAP_FREE(&lMSGTpduStatusReport_p);
            } else {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreSIMDeliveryStatus - Failed to allocate SMS_StatusReport_t"));
                lResult = FALSE;
            }
        }

        SMS_HEAP_FREE(&lSMS_SMRStatusReport_p);
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreSIMDeliveryStatus - Failed to allocate memory for status report"));
        lResult = FALSE;
    }

    return lResult;
}

/********************************************************************/
/**
 *
 * @function     SmStorage_StatusReportDeleteFromSIM
 *
 * @description  Delete a Status Report from the specified position
 *               within EF_SMSR on the SIM
 *
 * @param        RecordNumber - Record number of EF_SMSR to free
 *
 * @return       uint8_t TRUE if successful
 */
/********************************************************************/
static uint8_t SmStorage_StatusReportDeleteFromSIM(
    const uint8_t RecordNumber)
{
    uint8_t Result = FALSE;

    if (SmStorage_SIM_SR_LogicalCapacityRd() >= RecordNumber && 0 != RecordNumber) {
        if (TRUE == StatusReportIsEmpty[ArrayIndex(RecordNumber)]) {    // Record is already empty, no need to delete
            Result = TRUE;
        } else {
            uint16_t Length = STATUS_REPORT_REC_SIZE;
            ste_sim_t *sim_sync;
            sim_record_id_t RecordId = (sim_record_id_t) RecordNumber;
            uint8_t retval = -1;
            uintptr_t client_tag;
            ste_uicc_update_sim_file_record_response_t result = {
                STE_UICC_STATUS_CODE_FAIL,
                STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                {0x00, 0x00}
            };
            uint8_t Data[STATUS_REPORT_REC_SIZE];

            // initialised record data
            memset(Data, 0xFF, STATUS_REPORT_REC_SIZE);
            Data[0] = EFSMSR_RECORD_STATUS_FREE_MASK;

            sim_sync = MessagingSystem_UICC_SimSyncGet();
            client_tag = (uintptr_t) sim_sync;

            if (sim_sync) {
                retval = ste_uicc_sim_file_update_record_sync(sim_sync, client_tag, EF_SMS_FILENAME, RecordId, Length, EF_SMS_FILEPATH, Data, &result);
            }

            if (retval == 0 && result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
                Result = TRUE;
            } else {
                // EF_SMS not updated
                SMS_A_(SMS_LOG_E("smstrg.c:  SmStorage_SIM_ShortMessage_Write failed to update EF_SMS record %d", RecordId));
            }
        }
    }

    return Result;
}

/********************************************************************/
/**
 *
 * @function     SmStorage_SIMFreeObsoleteSR
 *
 * @description  Delete Status Report associated with an empty Short
 *               Message record from the SIM
 *
 * @param        SlotNumber - The empty SM record number in EF_SMS
 *
 * @return       uint8_t TRUE if successful
 */
/********************************************************************/
static uint8_t SmStorage_SIMFreeObsoleteSR(
    const SMS_Position_t SlotNumber)
{
    uint8_t Result = TRUE;      // return TRUE if record deleted or if no match found (already deleted).

    if (0 != SmStorage_SIM_SR_LogicalCapacityRd()) {    // check EF_SMSR available and enabled on the SIM
        if (NULL != SIM_StatusReports_p) {      // We're in the initialisation phase and can find the associated SR via SIM_StatusReports_p
            uint16_t BufferOffset = 0;
            uint8_t Loop = 0;
            const uint8_t RecordOffset = 1;     // Status Report record is always loop + 1 since there's no record 0

            while (Loop < SmStorage_SIM_SR_VerifiedCapacityRd()) {      // search stored SR records for one that references the empty SM
                if (SlotNumber == *(SIM_StatusReports_p + BufferOffset)) {      // we have found a match
                    // delete Status Report associated with empty Short Message record from the SIM
                    if (SmStorage_StatusReportDeleteFromSIM((Loop + RecordOffset))) {   // update record in temp RAM store, set as empty record
                        *(SIM_StatusReports_p + BufferOffset) = EFSMSR_RECORD_STATUS_FREE_MASK;
                    } else {    // Error, unable to delete record
                        Result = FALSE;
                    }
                    /* even though we found the match, we keep looking in case there are any other SR's that
                     * are referencing the obsolete SM - this should not happen except in the case of a card
                     * with corrupt SR data in which case this will tidy it up.
                     */
                }
                Loop++;
                BufferOffset = (STATUS_REPORT_REC_SIZE * Loop);
            }
        }                       // end check there are Status Reports on the SIM and that we've already stored them
        else {
            SmStorageObject_t *StoredObject_p = SmStorage_SlotAccess(SlotNumber);

            if (StoredObject_p) {
                uint8_t RecordNumber = ShortMessage_GetStatusReportRecNum(StoredObject_p->ShortMessage);        // record number in EF_SMSR

                if (SMS_STORAGE_POSITION_INVALID != RecordNumber) {
                    if (SmStorage_StatusReportDeleteFromSIM(RecordNumber)) {    // update stored SR record number
                        ShortMessage_SetStatusReportRecNum(StoredObject_p->ShortMessage, SMS_STORAGE_POSITION_INVALID);
                    } else {    // Error, unable to delete record
                        Result = FALSE;
                    }
                }
            }
        }
    }
    return Result;
}

/********************************************************************/
/**
 *
 * @function     SmStorage_GetStatusReportForSlotNo
 *
 * @description  Searches SR's previously read from the SIM temporarily
 *               cached in RAM for a SR that corresponds with the supplied
 *               SM slot number.
 *
 * @param        SlotNumber - the SM slot number
 *               StatusReport - the status report record as stored on the SIM
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_GetStatusReportForSlotNo(
    const SMS_Position_t SlotNumber,
    uint8_t * SMS_SMRStatusReport_p,
    ShortMessage_p SmInstance)
{
    if (NULL != SMS_SMRStatusReport_p && NULL != SIM_StatusReports_p && SMS_STORAGE_POSITION_INVALID != SlotNumber) {
        uint8_t MatchFound = FALSE;
        uint16_t BufferOffset = 0;
        uint8_t Loop = 0;
        const uint8_t RecordOffset = 1; // Status Report record is always loop + 1 since there's no record 0

        // initialise record
        memset(SMS_SMRStatusReport_p, 0xFF, STATUS_REPORT_REC_SIZE);
        *SMS_SMRStatusReport_p = EFSMSR_RECORD_STATUS_FREE_MASK;

        while (Loop < SmStorage_SIM_SR_VerifiedCapacityRd() && !MatchFound) {   // check SR records for a match
            if (SlotNumber == *(SIM_StatusReports_p + BufferOffset)) {  // we have found a match
                if (NULL == SmInstance) {       // no short message so we should delete associated status report from the SIM
                    (void) SmStorage_StatusReportDeleteFromSIM((Loop + RecordOffset));
                } else if (ShortMessage_GetType(SmInstance) == SM_SUBMIT && ShortMessage_GetStatus(SmInstance) == SMS_STATUS_SENT) {    /* This is a sent message. If this is stored on SIM then we need to
                                                                                                                                         * determine if a delivery status report is present for this slot and
                                                                                                                                         * then verify if the delivery status report is for this short message.)                */

                    uint8_t DeliveryStatus_TP_MR = 0;
                    uint8_t ShortMessage_TP_MR = 0;
                    // do message ref match i.e. is this SR really for this SM
                    DeliveryStatus_TP_MR = TPDU_GetField(TP_MR, SIM_StatusReports_p + BufferOffset, SM_STATUS_REPORT);
                    // Read the Message Reference from the short message itself
                    (void) ShortMessage_GetMessageReference(SmInstance, &ShortMessage_TP_MR);

                    if (DeliveryStatus_TP_MR == ShortMessage_TP_MR) {   // The SR message reference matches that of the SM
                        MatchFound = TRUE;
                        SMS_A_(SMS_LOG_I("smstrg.c: SmStorage_GetStatusReportForSlotNo SM = %d matched to EF_SMSR rec = %d.", SlotNumber, (Loop + RecordOffset)));
                        memcpy(SMS_SMRStatusReport_p, SIM_StatusReports_p + BufferOffset, STATUS_REPORT_REC_SIZE);
                        // store original record location of SR on the SIM
                        ShortMessage_SetStatusReportRecNum(SmInstance, (Loop + RecordOffset));
                    } else {    // Message Reference doesn't match the SM Message reference so we should delete this SR from the SIM
                        (void) SmStorage_StatusReportDeleteFromSIM((Loop + RecordOffset));      // delete erroneous SR from SIM
                    }
                } else {
                    (void) SmStorage_StatusReportDeleteFromSIM((Loop + RecordOffset));  // delete erroneous SR from SIM
                }
            }
            Loop++;
            BufferOffset = (STATUS_REPORT_REC_SIZE * Loop);
        }                       // end check SR records for a match
    } else {
        // N.B. If SMS_STATUS_REPORTS_MATCH_STORE is 0 then SIM_StatusReports_p will not have been initialised.
        if (SMS_STATUS_REPORTS_MATCH_STORE > 0) {
            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_GetStatusReportForSlotNo Parameter error"));
        }
        SMS_B_(SMS_LOG_D
               ("smstrg.c: SmStorage_GetStatusReportForSlotNo: SMS_SMRStatusReport_p=0x%08X, SIM_StatusReports_p=0x%08X, SlotNumber=%d.", (uint32_t) SMS_SMRStatusReport_p,
                (uint32_t) SIM_StatusReports_p, SlotNumber));
    }
}                               // end of SmStorage_GetStatusReportForSlotNo

/********************************************************************/
/**
 *
 * @function     SmStorage_CacheSRsFromSIM
 *
 * @description  Read EF_SMSR if present on the SIM and cache all
 *               valid status reports
 *
 * @param        *CacheSIM_SRsComplete_p : TRUE if entire EF_SMSR cached
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_CacheSRsFromSIM(
    uint8_t * CacheSIM_SRsComplete_p)
{
    if (0 == SmStorage_SIM_SR_LogicalCapacityRd() || SmStorage_SIM_SR_VerifiedCapacityRd() >= SmStorage_SIM_SR_LogicalCapacityRd()) {   // no SMSR records to read
        *CacheSIM_SRsComplete_p = TRUE;
    } else {                    // more SMSR records to be read off the SIM
        uint8_t RecordNumber = SmStorage_SIM_SR_VerifiedCapacityRd() + 1;

        if (NULL == SIM_StatusReports_p) {      /* allocate temp RAM cache for SR's to read off the SIM in an efficient manor, to be freed later
                                                 * in SmStorage_RestoreSIMVolume() once all SM's and SR's have been restored from the SIM
                                                 **/
            const uint16_t BufferSize = (STATUS_REPORT_REC_SIZE * SmStorage_SIM_SR_LogicalCapacityRd());

            SIM_StatusReports_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(BufferSize);

            if (NULL != SIM_StatusReports_p) {
                memset(SIM_StatusReports_p, 0xFF, BufferSize);
            } else {            // out of memory
                *CacheSIM_SRsComplete_p = TRUE;
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_CacheSRsFromSIM Failed to allocate memory"));
            }
        }

        if (SmStorage_SIM_SR_LogicalCapacityRd() >= RecordNumber && NULL != SIM_StatusReports_p) {
            uint16_t BufferOffset = (STATUS_REPORT_REC_SIZE * SmStorage_SIM_SR_VerifiedCapacityRd());
            uintptr_t client_tag;
            ste_sim_t *sim_sync;
            uicc_request_status_t status = UICC_REQUEST_STATUS_FAILED_STATE;
            ste_uicc_sim_file_read_record_response_t read_result = {
                STE_UICC_STATUS_CODE_FAIL,
                STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                {0x00, 0x00}
                ,
                0,
                0
            };

            // request EfSMSrRecord storage to the SIM
            sim_sync = MessagingSystem_UICC_SimSyncGet();
            client_tag = (uintptr_t) sim_sync;

            if (sim_sync) {
                status = ste_uicc_sim_file_read_record_sync(sim_sync, client_tag, EF_SMSR_FILENAME, RecordNumber, STATUS_REPORT_REC_SIZE, EF_SMS_FILEPATH, &read_result);
            }

            if (status == 0 && read_result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
                memcpy(SIM_StatusReports_p + BufferOffset, read_result.data, STATUS_REPORT_REC_SIZE);
                SMS_A_(SMS_LOG_I("smstrg.c: SmStorage_CacheSRsFromSIM EF_SMSR rec %d status %d", RecordNumber, read_result.data[0]));

                if (EFSMSR_RECORD_STATUS_FREE_MASK == read_result.data[0]) {    // This record is marked as being free
                    StatusReportIsEmpty[ArrayIndex(RecordNumber)] = TRUE;
                }
                //Increment Verified SIM Capacity
                SmStorage_SIM_SR_VerifiedCapacityWr((uint8_t) RecordNumber);
            } else {            // error, cannot read record
                *CacheSIM_SRsComplete_p = TRUE; // end cache of EF_SMSR records
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_CacheSRsFromSIM Failed to read EF_SMSR rec %d", RecordNumber));
            }
        }
        if (RecordNumber >= SmStorage_SIM_SR_LogicalCapacityRd()) {     // no more SR records to read off the SIM
            *CacheSIM_SRsComplete_p = TRUE;
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_CacheSRsFromSIM complete"));
        }
    }                           // end more SMSR records to be read off the SIM
}                               // end SmStorage_CacheSRsFromSIM

/********************************************************************/
/**
 *
 * @function     SmStorage_RestoreSIMVolume
 *
 * @description  Restore the short messages on the SIM volume
 *
 * @param        *SmInstance_p :         Holds the sm instnace
 * @param        *SIMRestoreComplete_p : TRUE if completed
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_RestoreSIMVolume(
    ShortMessage_p * SmInstance_p,
    uint8_t * SIMRestoreComplete_p)
{
    *SIMRestoreComplete_p = FALSE;

    if (SMS_STATUS_REPORTS_MATCH_STORE && !AllSRsCached) {      // Read and cache Status Reports from the SIM
        SmStorage_CacheSRsFromSIM(&AllSRsCached);
    } else {                    // Restore SIM volume from the SIM to the FS
        uint8_t SlotNumber = SmStorage_SIMSlotVerifiedCapacityRd() + 1;
        EfSmsRecord_t *EfSmsRecord_p = NULL;
        SmStorageObject_t *StoredObject_p;

        EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

        if (EfSmsRecord_p) {
            SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

            if (SmStorage_SIMSlotLogicalCapacityRd() >= SlotNumber) {
                // Read and Restore this SIM slot
                if (SmStorage_SlotRead(SlotNumber, *EfSmsRecord_p)) {
                    if (!SmStorage_SmInvalidTPDU(*EfSmsRecord_p)) {
                        *SmInstance_p = SmStorage_SmRestore(SlotNumber, *EfSmsRecord_p);

                        if (*SmInstance_p) {
                            /* Assign Storage Slot to ShortMessage Instance */
                            StoredObject_p = (SmStorageObject_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SmStorageObject_t));

                            (void) SmStorage_GetVolumeIndex((MSG_StorageMedia_t *) SIM_VOL, &StoredObject_p->StorageIndex);

                            // File index will be set if the sm gets cached
                            StoredObject_p->FileIndex = SMSTORAGE_FILEINDEX_INVALID;
                            StoredObject_p->ShortMessage = *SmInstance_p;
                            StoredObject_p->PendingFlag = FALSE;

                            SmStorage_SlotAllocate(SlotNumber, StoredObject_p);
                            ShortMessage_SetStoragePosition(StoredObject_p->ShortMessage, SlotNumber);
                        }
                        // Validate SIM cache copy for this slot
                        SmStorage_ValidateSmInSimCache(*EfSmsRecord_p, (uint8_t) SlotNumber, *SmInstance_p);

                        // get SR associated with SM and cache it
                        (void) SmStorage_RestoreSIMDeliveryStatus(SlotNumber, *SmInstance_p);
                    } else {
                        //Delete the invalid TPDU from SIM
                        uint8_t RecordNumber = SlotNumber;

                        SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

                        if (SmStorage_SIM_ShortMessage_Write(*EfSmsRecord_p, &RecordNumber) == 0) {     //Blank out any associated SR
                            (void) SmStorage_SIMFreeObsoleteSR(SlotNumber);
                            SMS_A_(SMS_LOG_I("smstrg.c: SmStorage_RestoreSIMVolume - Invalid TPDU deleted from slot = %d", SlotNumber));
                        } else {
                            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreSIMVolume slot = %d", SlotNumber));
                        }
                    }

                    //Increment Verified SIM Capacity
                    SmStorage_SIMSlotVerifiedCapacityWr((uint8_t) SlotNumber);

                    if (SlotNumber >= SmStorage_SIMSlotLogicalCapacityRd()) {
                        *SIMRestoreComplete_p = TRUE;
                        SMS_B_(SMS_LOG_D("smstrg.c: SmStorageInitialise: SMS Validation complete"));
                    }
                } else {
                    // Failed SmStorage_SlotRead()
                    SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_SlotRead Failed to read SlotNumber = %d", SlotNumber));
                    *SIMRestoreComplete_p = TRUE;
                }
            } else {
                *SIMRestoreComplete_p = TRUE;
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorageInitialise: Attempted to init more than %d SIM Slots", SmStorage_SIMSlotLogicalCapacityRd()));
            }

            SMS_HEAP_FREE(&EfSmsRecord_p);
        }
    }
    if (TRUE == *SIMRestoreComplete_p) {        // SIM caching is complete so free memory used for temporary storage of status report records
        if (NULL != SIM_StatusReports_p) {
            SMS_HEAP_FREE(&SIM_StatusReports_p);
            SIM_StatusReports_p = NULL;
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ValidateDeliveryStatus
 *
 * @description  Validates any delivery status for the slot/sm instance
 *
 * @param        SlotNumber : Slot number of the short message
 * @param        SmInstance : Identifies the short message
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_ValidateDeliveryStatus(
    const SMS_Position_t SlotNumber,
    ShortMessage_p SmInstance)
{
    DeliveryStatus_t DeliveryStatus;
    uint8_t DeliveryStatus_TP_MR = 0;
    uint8_t ShortMessage_TP_MR = 0;

    // Read the delivery status
    (void) SmStorage_ReadStatusReport(SlotNumber, &DeliveryStatus, NULL);

    if (!SmInstance) {
        // If there is no SM_Instance, make sure that there is no
        // corresponding delivery report. If there is, remove it
        if (STATUS_REPORT_RESET != DeliveryStatus) {
            // This slot is empty so cant be a valid status report
            (void) SmStorage_DeleteStatusReportFromVolume(SlotNumber);
        }
    } else {
        if (SMS_STORAGE_POSITION_INVALID != SlotNumber) {
            if (ShortMessage_GetType(SmInstance) == SM_SUBMIT && ShortMessage_GetStatus(SmInstance) == SMS_STATUS_SENT) {
                // This is a sent message. If this is stored on SIM then we need to
                // determine if a delivery status report is present for this slot and
                // then verify if the delivery status report is for this short message.
                if (SmStorage_SIMSlotLogicalCapacityRd() >= SlotNumber) {
                    Tpdu_t *StatusReport_p = (Tpdu_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(Tpdu_t));

                    if (StatusReport_p) {
                        memset(StatusReport_p, 0x00, sizeof(Tpdu_t));

                        // This is a SIM Slot, is there a delivery status report for it
                        if (SmStorage_ReadStatusReport(SlotNumber, NULL, (SMS_StatusReport_t *) StatusReport_p)) {
                            // A status report exists for this slot get the message reference
                            DeliveryStatus_TP_MR = TPDU_GetField(TP_MR, (uint8_t *) StatusReport_p, SM_STATUS_REPORT);
                            // Read the Message Reference from the short message itself
                            (void) ShortMessage_GetMessageReference(SmInstance, &ShortMessage_TP_MR);

                            if (DeliveryStatus_TP_MR == ShortMessage_TP_MR) {
                                // Update the delivery status cache since this Deliver Report matches the SIM stored SM
                                ShortMessage_SetCacheDeliveryStatus(SmInstance, DeliveryStatus);
                            } else {
                                (void) SmStorage_DeleteStatusReportFromVolume(SlotNumber);
                            }
                        }

                        SMS_HEAP_FREE(&StatusReport_p);
                    }
                } else {
                    // Update the delivery status cache since this Deliver Report matches the ME stored SM.
                    // No need to check TP-MR's since ME stored SM's and Deliver Reports are synchronised.
                    ShortMessage_SetCacheDeliveryStatus(SmInstance, DeliveryStatus);
                }
            } else              // this message has not been sent
            {
                // not expecting to find a valid status
                if (DeliveryStatus != STATUS_REPORT_RESET) {
                    // Delete the status report as it can't be for this short message
                    (void) SmStorage_DeleteStatusReportFromVolume(SlotNumber);
                }
            }
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ShortMessageInitialise
 *
 * @description  Initialise a ShortMessage Object from the ShortMessage retrieved from Storage.
 *
 * @param        SmType :      as type of ShortMessageType_t.
 * @param        EfSmsRecord : as type of EfSmsRecord_t.
 * @param        SlotNumber :  SMS_Position_t
 *
 * @return       static ShortMessage_p
 */
/********************************************************************/
static ShortMessage_p SmStorage_ShortMessageInitialise(
    ShortMessageType_t const SmType,
    EfSmsRecord_t EfSmsRecord,
    const SMS_Position_t SlotNumber)
{
    ShortMessage_p SmInstance = NULL;
    uint16_t TpduLength = 0;
    /*
     * Create a Short Message instance for the retrieved Short message
     */
    SmInstance = ShortMessage_Create(SmType);

    /*
     * Determine if Short Message Instance Created
     */
    if (SmInstance) {
        SMS_Status_t SmStatus;
        uint8_t *Tpdu;

        /*
         * Set Short Message Status
         */
        SmStatus = SmStorage_ShortMessageStatusCalculate(SmStorage_EfSmsRecordStatusGet(EfSmsRecord));

        (void) ShortMessage_SetStatus(SmInstance, SmStatus);

        /*
         * Set Short Message Service Centre Address
         */
        (void) ShortMessage_SetPackedServiceCentreAddress(SmInstance, SmStorage_EfSmsRecordScAddress(EfSmsRecord));

        /*
         * Populate Short Message with Data.
         * Note: Ensure Tpdu Unit is destroyed
         */
        Tpdu = SmStorage_EfSmsRecordTPDU(EfSmsRecord);
        TpduLength = SmUtil_CalculateSizeOf_SM(Tpdu);
        TpduLength = SMS_MIN(TpduLength, MAXIMUM_TPDU_SIZE);
        ShortMessage_SetTPDU(SmInstance, Tpdu, (uint8_t) TpduLength);
        ShortMessage_SetSequenceNumber(SmInstance);

        /*
         * Populate ShortMessage Instance with Slot Number
         */
        ShortMessage_SetStoragePosition(SmInstance, SlotNumber);

        /*
         *  Initialise SR Record Number
         */
        ShortMessage_SetStatusReportRecNum(SmInstance, SMS_STORAGE_POSITION_INVALID);

        /*
         * DEBUG:
         */
        SMS_B_(SMS_LOG_D("smstrg.c: ShortMessageInitialise: Short Message "));
        SMS_B_(SMS_LOG_D(" Slot Number %d", SlotNumber));
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: ShortMessageInitialise: Fail To Create SmInstance "));
    }

    return SmInstance;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ValidateSmInSimCache
 *
 * @description  Validates the SIM cache is up-to-date for a given
 *               SIM slot
 *
 * @param        EfSmsRecord: SM Record as read from the SIM
 * @param        SlotNumber :   uint8_t (as it has to fit on the SIM)
 * @param        ShortMessage_p Short mesasge to be saved
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_ValidateSmInSimCache(
    const EfSmsRecord_t EfSmsRecord,
    const uint8_t SlotNumber,
    ShortMessage_p SmInstance)
{
    FILE *File_p = NULL;
    uint8_t WriteToSIMCache = FALSE;
    char *SmPathFilename_p = NULL;
    uint8_t *EfSmsRecord_p = NULL;
    MSG_StorageMedia_t *StorageMedia_p = NULL;


    // SIM cache has a maximum size to preserve File System space
    if (SMS_SIM_CACHE_MAX_SIZE >= SlotNumber) {
        // Allocate memory for the tpdu copy from the SIM cache
        SmPathFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);
        EfSmsRecord_p = SMS_HEAP_UNTYPED_ALLOC(EFSMS_RECORD_SIZE);
        StorageMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

        if (SmPathFilename_p && EfSmsRecord_p && StorageMedia_p) {
            (void) strcpy(StorageMedia_p->StorageMediaName, SIM_VOL);

            //...build them into a full filename
            (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, StorageMedia_p, SmPathFilename_p);

            // Try to open our short message container file
            if (!(File_p = fopen(SmPathFilename_p, "rb+"))
                // ...or create a new short message container file
                && !(File_p = fopen(SmPathFilename_p, "wb"))) {
                // Failed to create the file so we must at least report it
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_ValidateSmInSimCache - Failed to create sm container file \"%s\"", SmPathFilename_p));
            }
            // Provided we have been able to open/create the tpdu container file
            // we should either re-build or restore the SIM cache from the SIM slots
            // by copying everything valid or just copying any SIM slots that are
            // that are different and valid.
            // Invalid SIM slots should have the corresponding SIM Cache entries set
            // to empty.
            if (File_p) {
                // Move the file index to corresponding SIM cache file position
                if (fseek(File_p, ((SlotNumber - 1) * SHORT_MESSAGE_FILE_ENTRY_LENGTH), SEEK_SET)) {
                    SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_ValidateSmInSimCache - Failed to seek current position in sm container file"));
                } else {
#ifdef PLATFORM_ANDROID
                    // Android compiler complains about initialising the stat structures using the (valid)
                    // C constrution in the #else case. Therefore we have to do it another way instead.
                    // Android compiler warnings are (#### is line where struct stat TimeStamp = {0}; was located):
                    // smstrg.c:####: warning: missing initializer
                    // smstrg.c:####: warning: (near initialization for 'TimeStamp.__pad0')
                    struct stat TimeStamp;
                    memset(&TimeStamp, 0x00, sizeof TimeStamp);
#else
                    struct stat TimeStamp = { 0 };
#endif
                    // Get the tpdu from the SIM cache if it exists
                    if (fread(EfSmsRecord_p, EFSMS_RECORD_SIZE, 1, File_p)
                        && fread(&TimeStamp, sizeof(TimeStamp), 1, File_p)) {
                        // Compare the SIM Efsms record with the SIM Cache entry.
                        if (memcmp(EfSmsRecord_p, EfSmsRecord, EFSMS_RECORD_SIZE) == 0) {
                            // Although the SIM cache SM entry matches the SIM Ef record entry it could be empty
                            if (SmInstance) {
                                (void) SmStorage_SetFileIndex(SlotNumber, (uint16_t) (SlotNumber - 1));
                            }
                        } else {
                            WriteToSIMCache = TRUE;
                        }
                    } else {
                        // SIM cache entry does not exist
                        WriteToSIMCache = TRUE;
                    }

                    if (WriteToSIMCache) {
                        (void) fseek(File_p, ((SlotNumber - 1) * SHORT_MESSAGE_FILE_ENTRY_LENGTH), SEEK_SET);

                        // Check that it is not a Class2 8Bit message, these must not be cached
                        // as they can be modified by the SIM and the cache would be innacurate
                        if (SmInstance && SmStorage_SmToBeCached(SmInstance)) {

                            SmStorage_EfSmsRecordPack(SmInstance, EfSmsRecord_p);
                            TimeStamp = SmStorage_GetFileEntryTimeStamp();

                            if (fwrite(EfSmsRecord_p, EFSMS_RECORD_SIZE, 1, File_p)
                                && fwrite(&TimeStamp, sizeof(TimeStamp), 1, File_p)) {
                                (void) SmStorage_SetFileIndex(SlotNumber, (uint16_t) (SlotNumber - 1));
                            } else {
                                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_MoveSmFiles failed to write to destination file"));
                            }
                        } else {
                            // SmInstance is NULL or short message should not be cached
                            (void) fwrite(DefaultShortMessageFileEntry, sizeof(DefaultShortMessageFileEntry), 1, File_p);
                        }
                    }
                }

                // Close the short message container file
                (void) fclose(File_p);
            }
            // Clear up all file pointers and memory
            SMS_HEAP_FREE(&SmPathFilename_p);
            SMS_HEAP_FREE(&StorageMedia_p);
            SMS_HEAP_FREE(&EfSmsRecord_p);
        } else {
            // alloc failed, might result in 1 or more need freeing
            if (SmPathFilename_p)
                SMS_HEAP_FREE(&SmPathFilename_p);
            if (StorageMedia_p)
                SMS_HEAP_FREE(&StorageMedia_p);
            if (EfSmsRecord_p)
                SMS_HEAP_FREE(&EfSmsRecord_p);
        }
    }
}

/********************************************************************/
/**
 *
 * @function     SmStorage_SmSaveInVolume
 * @description  Save the specified Short message in a free slot in
 *               the specified volume.  If the volume is not specified,
 *               then it will be saved according to the prefered
 *               storage list
 *
 *               In the event a free slot is identified the information within a
 *               EfSmsRecord is populated from information extracted from the
 *               Short Message Instance. The EfSmsRecord is then written to the
 *               Slot identified.
 *
 * @param        SmInstance    : as type of ShortMessage_p.
 * @param        StoreVolume_p : MSG_StorageMedia_t
 * @param        ErrorCode_p   : Storage error code
 *
 * @return       SMS_Position_t
 */
/********************************************************************/
SMS_Position_t SmStorage_SmSaveInVolume(
    const ShortMessage_p SmInstance,
    const MSG_StorageMedia_t * StorageVolume_p,
    SmStoreErrorCode_t * const ErrorCode_p)
{
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;
    SMS_Storage_t mediaType;

    // Initialise the returned error code
    *ErrorCode_p = SM_STORE_FAILED;

    // We use this just to get the slot number first as all slots between
    // 1 and SIM Capacity are SIM slots.  All slots from SIM Capacity + 1
    // to the max slot number are available for other storage media
    if (!strcmp(StorageVolume_p->StorageMediaName, SIM_VOL)) {
        mediaType = SMS_STORAGE_SM;
    } else {
        mediaType = SMS_STORAGE_ME;
    }

    //find a free slot
    SlotNumber = SmStorage_SlotSearch(mediaType, StorageVolume_p);

    SMS_C_(SMS_LOG_V("smstrg.c: SmStorage_SmSaveMedia: After SlotSearch"));
    SMS_C_(SMS_LOG_V("Logical Slot Number is %d", SlotNumber));

    if (SMS_STORAGE_POSITION_INVALID != SlotNumber) {
        // Attempts to store the Short Message within the slot and volume identified
        if (SM_STORE_FAILED == (*ErrorCode_p = SmStorage_SmSaveSlotNumber(SmInstance, SlotNumber, StorageVolume_p))) {
            // We found a slot, but could not write to it (TA testcase 34.2.5.3 Test d)
            SlotNumber = SMS_STORAGE_POSITION_INVALID;
        }
    }
    SMS_C_(SMS_LOG_V("smstrg.c: SmStorage_SmSaveMedia: %s", SMS_STORAGE_POSITION_INVALID == SlotNumber ? "FAILED" : "PASSED"));

    return SlotNumber;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmSaveSlotNumber
 *
 * @description  Store the specified Short message to the specified slot:
 *               The information within the EfSmsRecord is populated from
 *               information extracted from the specified Short Message Instance.
 *               The EfSmsRecord is then written to the specified Slot.
 *
 * @param        SmInstance : as type of ShortMessage_p.
 * @param        SlotNumber : SMS_Position_t.
 *
 *
 * @return       SmStoreErrorCode_t
 *
 * NOTES:        The Operation prevents Short Message Overwrite
 */
/********************************************************************/
SmStoreErrorCode_t SmStorage_SmSaveSlotNumber(
    const ShortMessage_p SmInstance,
    const SMS_Position_t SlotNumber,
    const MSG_StorageMedia_t * StorageVolume_p)
{
    SmStoreErrorCode_t StoreErrorCode = SM_STORE_FAILED;
    EfSmsRecord_t *EfSmsRecord_p;
    /*
     * Short Message Overwrite Prevention
     */
    ShortMessage_p theSlotsShortMessage = SmStorage_SmIdentify(SlotNumber);

    SMS_C_(SMS_LOG_V("smstrg.c: ShortMessageSaveSlotNumber: Slot No: %d", SlotNumber));

    if (theSlotsShortMessage != SmInstance) {
        if (theSlotsShortMessage) {
            SMS_A_(SMS_LOG_E("smstrg.c: ShortMessageSaveSlotNumber: Slot %d FAIL - SLOT OVERWRITE ATTEMPT", SlotNumber));
            return StoreErrorCode;
        }
    } else {
        SMS_B_(SMS_LOG_D("smstrg.c: ShortMessageSaveSlotNumber: Slot %d Update", SlotNumber));
    }

    /*
     * Compose and Store an EfSmsRecord
     */
    EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

    if (EfSmsRecord_p) {
        SMS_StorageStatus_t NewStorageStatus;

        SmStorage_EfSmsRecordPack(SmInstance, *EfSmsRecord_p);

        /* Write EfSmsRecord to the Slot identified */
        StoreErrorCode = SmStorage_SlotWrite(SlotNumber, *EfSmsRecord_p, StorageVolume_p, SmInstance, TRUE);

        // Inform clients if the storage status has changed
        SmStorage_AnalyseStorage(&NewStorageStatus);

        /* Release Memory for EfSmsRecord  */
        SMS_HEAP_FREE(&EfSmsRecord_p);
    }

    return StoreErrorCode;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmInvalidTPDU
 *
 * @description  Find mismatch between short message type and status
 *
 * @param        EfSmsRecord : as type of EfSmsRecord_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_SmInvalidTPDU(
    EfSmsRecord_t EfSmsRecord)
{
    uint8_t Result = TRUE;
    uint8_t EfSmsStatus = SmStorage_EfSmsRecordStatusGet(EfSmsRecord);

    //default status 0xff and 0x00 as an empty TPDU
    if (0xff == EfSmsStatus || !EfSmsStatus) {
        Result = FALSE;
    } else {
        SMS_Status_t SmStatus = SmStorage_ShortMessageStatusCalculate(EfSmsStatus);

        switch (SmStorage_EfSmsRecordType(EfSmsRecord)) {
        case SM_DELIVER:       // SC to MS
        case SM_SUBMIT_REPORT_ACK:     // SC to MS
        case SM_SUBMIT_REPORT_ERROR:   // SC to MS
        case SM_STATUS_REPORT: // SC to MS
            if (SMS_STATUS_READ == SmStatus || SMS_STATUS_UNREAD == SmStatus) {
                Result = FALSE;
            } else {
                SMS_B_(SMS_LOG_D("smstrg.c: Error set wrong status: %d to SC to MS short message type", SmStatus));
            }
            break;
        case SM_DELIVER_REPORT_ACK:    // MS to SC
        case SM_DELIVER_REPORT_ERROR:  // MS to SC
        case SM_SUBMIT:        // MS to SC
        case SM_COMMAND:       // MS to SC
            if (SMS_STATUS_SENT == SmStatus || SMS_STATUS_UNSENT == SmStatus) {
                Result = FALSE;
            } else {
                SMS_B_(SMS_LOG_D("smstrg.c: Error set wrong status: %d to MS to SC short message type", SmStatus));
            }
            break;
        default:
            {
                // Do not know what short message type
                SMS_A_(SMS_LOG_E("smstrg.c: unknown short message type %d.", SmStorage_EfSmsRecordType(EfSmsRecord)));
            }
            break;
        }                       // end switch
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EfSmsRecordType
 *
 * @description  Find short message type from the TPDU record
 *
 * @param        EfSmsRecord : as type of EfSmsRecord_t.
 *
 * @return       static ShortMessageType_t
 */
/********************************************************************/
static ShortMessageType_t SmStorage_EfSmsRecordType(
    EfSmsRecord_t EfSmsRecord)
{
    EfSmsRecordStatus_t EfSmsRecordStatus = SmStorage_EfSmsRecordStatusGet(EfSmsRecord);
    ShortMessageType_t SmType = SM_TYPE_LAST;
    uint8_t *Tpdu;
    uint8_t msgType;

    if (EfSmsRecordStatus & BIT0_MASK) {
        /*
         * NOTE:
         * SMS_DELIVER/SMS_SUBMIT_REPORT have two disting primitives:
         *  + SMS_DELIVER_REPORT_ACK
         *  + SMS_DELIVER_REPORT_ERROR
         *  + SMS_SUBMIT_REPORT_ACK
         *  + SMS_SUBMIT_REPORT_ERROR
         * The primitive type cannot be determined without the RP-Message.
         */
        ShortMessageType_t Tpdu_SmType[SMDIRECTION_LAST][3] = { {SM_TYPE_LAST, SM_SUBMIT, SM_COMMAND}
        ,                       /* MO */
        {SM_DELIVER, SM_TYPE_LAST, SM_STATUS_REPORT}
        };                      /* MT */

        /*
         * Determine if Short Message exists in EfSmsRecord
         */
        Tpdu = SmStorage_EfSmsRecordTPDU(EfSmsRecord);
        msgType = Tpdu[0] & 0x03;       // get TP_MTI

        if (msgType == SMS_RESERVED_MESSAGE) {
            msgType = SMS_DELIVER;
        }

        switch ((SMS_Status_t) (EfSmsRecordStatus) & EFSMS_RECORD_STATUS_MASK) {
        case EFSMS_RECORD_STATUS_UNREAD_MASK:
        case EFSMS_RECORD_STATUS_READ_MASK:
            SmType = Tpdu_SmType[SMDIRECTION_MT][msgType];
            break;

        case EFSMS_RECORD_STATUS_UNSENT_MASK:
        case EFSMS_RECORD_STATUS_SENT_MASK:
            SmType = Tpdu_SmType[SMDIRECTION_MO][msgType];
            break;

        default:
            /*
             * Short message Type Unknown.
             */
            SmType = SM_TYPE_LAST;
            break;
        }
    }

    return SmType;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmRestore
 *
 * @description  Restore the Short message within the Specified Slot
 *
 * @param        SlotNumber : uint8_t.
 *
 * @return       static ShortMessage_p
 */
/********************************************************************/
static ShortMessage_p SmStorage_SmRestore(
    const SMS_Position_t SlotNumber,
    EfSmsRecord_t EfSmsRecord)
{
    ShortMessage_p SmInstance = NULL;
    ShortMessageType_t SmType;

    SMS_B_(SMS_LOG_D("smstrg.c: Analysing Slot %d - ", SlotNumber));

    if (EfSmsRecord) {
        SmType = SmStorage_EfSmsRecordType(EfSmsRecord);

        switch (SmType) {
        case SM_SUBMIT:
        case SM_DELIVER:
        case SM_COMMAND:
        case SM_STATUS_REPORT:
            // Create/Initilaise Short Message Instance
            SmInstance = SmStorage_ShortMessageInitialise(SmType, EfSmsRecord, SlotNumber);
            break;

        default:
            SMS_B_(SMS_LOG_D("smstrg.c: ShortMessageRestore: Unknown Short Message Type in Slot %d: SmType %d", SlotNumber, SmType));
            break;
        }
    }

    if (SmInstance) {
        SMS_B_(SMS_LOG_D(" restored"));
    } else {
        SMS_B_(SMS_LOG_D(" ignored"));
    }

    return SmInstance;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmRead
 *
 * @description  Read the Specified Short message.
 *
 * @param        SlotNumber :    SMS_Position_t
 * @param        *ScAddress_p :  as type of ShortMessagePackedAddress_t.
 * @param        *TpduLength_p : uint8_t.
 * @param        *Tpdu_p :       uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_SmRead(
    SMS_Position_t const SlotNumber,
    ShortMessagePackedAddress_t * const ScAddress_p,
    uint8_t * const TpduLength_p,
    uint8_t * const Tpdu_p)
{
    uint8_t Result = FALSE;
    /* Allocate Memory for EfSmsRecord */
    EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

    SMS_C_(SMS_LOG_V("smstrg.c: ShortMessageRead: Slot %d", SlotNumber));

    if (EfSmsRecord_p) {
        /*
         * Initilaise the EfSmsRecord
         */
        SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

        /*
         * Retrieve the Slot Information
         */
        Result = SmStorage_SlotRead(SlotNumber, *EfSmsRecord_p);
        if (Result) {
            uint8_t *Tpdu;

            /* Copy Service Centre Address */
            memcpy(ScAddress_p, SmStorage_EfSmsRecordScAddress(*EfSmsRecord_p), sizeof(ShortMessagePackedAddress_t));

            /*
             * Extract Tpdu From EfSmsRecord and Insert into SmStorageRecord
             */
            Tpdu = SmStorage_EfSmsRecordTPDU(*EfSmsRecord_p);
            *TpduLength_p = SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH;
            memcpy(Tpdu_p, Tpdu, *TpduLength_p);
        } else {
            *TpduLength_p = 0;
        }

        /* Release Memory for EfSmsRecord  */
        SMS_HEAP_FREE(&EfSmsRecord_p);
    }
    /* ENDIF: Memory Allocated for EfSmsRecord */
    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmWrite
 *
 * @description  Write the Specified Short message.
 *
 * @param        SmInstance :    as type of ShortMessage_p.
 * @param        SlotNumber :    SMS_Position_t
 * @param        *ScAddress_p :  as type of ShortMessagePackedAddress_t.
 * @param        TpduLength_p :  uint8_t.
 * @param        *Tpdu_p :       uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_SmWrite(
    ShortMessage_p const SmInstance,
    SMS_Position_t const SlotNumber,
    ShortMessagePackedAddress_t * const ScAddress_p,
    uint8_t TpduLength,
    uint8_t * const Tpdu_p)
{
    uint8_t Result = FALSE;
    /* Allocate Memory for EfSmsRecord */
    EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

    SMS_IDENTIFIER_NOT_USED(SmInstance);

    SMS_C_(SMS_LOG_V("smstrg.c: SmStorage_SmWrite"));

    if (EfSmsRecord_p) {
        uint8_t SCAddsize = SMS_MIN((unsigned) (ScAddress_p->Length + 1), sizeof(ShortMessagePackedAddress_t));

        /*
         * Initilaise the EfSmsRecord
         */
        SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

        /* Copy Service Centre Address, First find the length to copy */
        memcpy(SmStorage_EfSmsRecordScAddress(*EfSmsRecord_p), ScAddress_p, SCAddsize);

        /* Copy TPDU */
        memcpy(SmStorage_EfSmsRecordTPDU(*EfSmsRecord_p), Tpdu_p, TpduLength);

        /* Now write the slot - storage volume is NULL since writing to existing SlotNumber */
        if (SM_STORE_FAILED != SmStorage_SlotWrite(SlotNumber, *EfSmsRecord_p, NULL, SmInstance, FALSE)) {
            Result = TRUE;
        }

        SMS_HEAP_FREE(&EfSmsRecord_p);
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmStatusUpdate
 *
 * @description  Update the Specified Short message status within Storage:
 *               The specified Short message has been updated.
 *               Update the EfSmsRecord within the specified Slot
 *
 * @param        SmInstance : as type of ShortMessage_p.
 * @param        SlotNumber : SMS_Position_t
 *
 * @return       uint8_t
 *
 * NOTES:        Persistent Data enables X bytes of the EfSmsRecord to be updated.
 *               However, SIM only allows a complete EfSmsRecord to be written.
 *               To Aid understanding, the minimum functionality, that of the SIM
 *               is applied to all storage Media Data Thus, the Short Message is
 *               simply stored AGAIN.
 */
/********************************************************************/
uint8_t SmStorage_SmStatusUpdate(
    ShortMessage_p const SmInstance,
    const SMS_Position_t SlotNumber,
    const SMS_Status_t Status)
{
    uint8_t Result = FALSE;
    /* Allocate Memory for EfSmsRecord (Formatted) */
    EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

    SMS_IDENTIFIER_NOT_USED(SmInstance);

    SMS_C_(SMS_LOG_V("smstrg.c: SmStorage_SmStatusUpdate"));

    if (EfSmsRecord_p) {
        /*
         * Retrieve EfSmsRecord from within the specified Slot
         */
        if (SmStorage_SlotRead(SlotNumber, *EfSmsRecord_p)) {
            /*
             * Extract the Information from within the specified Short Message Instance
             */
            SmStorage_EfSmsRecordStatusSet(*EfSmsRecord_p, SmStorage_EfSmsRecordStatusCalculate(Status));

            /*
             * Write EfSmsRecord to the Slot identified - storage volume is NULL since updating existing SlotNumber
             */
            if (SM_STORE_FAILED != SmStorage_SlotWrite(SlotNumber, *EfSmsRecord_p, NULL, SmInstance, FALSE)) {
                Result = TRUE;
            }
        }

        /* Release Memory for EfSmsRecord */
        SMS_HEAP_FREE(&EfSmsRecord_p);

    }
    /* ENDIF: Memory Allocate for EfSmsRecord */
    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DeleteSmFromSimCache
 *
 * @description  Deletes the specified SIM cache file from the File System.
 *
 * @param        SlotNumber : Slot number of the SIM cache file to be deleted
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_DeleteSmFromSimCache(
    SMS_Position_t const SlotNumber)
{
    FILE *File_p = NULL;
    uint8_t status = FALSE;
    char *SmPathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    SMS_C_(SMS_LOG_V("smstrg.c: SmStorage_DeleteSmFromSimCache"));

    if (SmPathname_p) {
        // Get the filename, stored in the sm directory
        (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, (MSG_StorageMedia_t *) SIM_VOL, SmPathname_p);

        if (!(File_p = fopen(SmPathname_p, "rb+"))) {
            SMS_A_(SMS_LOG_E("smstrg.c: Failed to open file for SIM cache SM delete!"));
        } else {
            SMS_B_(SMS_LOG_D("smstrg.c: Successfully opened file for SIM cache SM delete"));

            SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, (uint16_t) (SlotNumber - 1));  // SlotNumber/FileIndex mapping predicatable on SIM

            if (fwrite(DefaultShortMessageFileEntry, sizeof(DefaultShortMessageFileEntry), 1, File_p) == 1) {
                status = TRUE;
            }

            (void) fclose(File_p);
        }

        SMS_HEAP_FREE(&SmPathname_p);
    }

    if (!status) {
        SMS_A_(SMS_LOG_E("smstrg.c: Failed to delete SIM cache SM for slot %d!", SlotNumber));
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmDelete
 *
 * @description  Delete the Specified Short message from within Storage.
 *
 * @param        SlotNumber        : SMS_Position_t
 * @param        deleteFromStorage : whether of not to delete the SM from presistant storage.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_SmDelete(
    SMS_Position_t const SlotNumber,
    const uint8_t deleteFromStorage)
{
    uint8_t Result = FALSE;

    SMS_C_(SMS_LOG_V("smstrg.c: SmStorage_SmDelete"));

    SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_SmDelete: Short Message in Slot Number %d", SlotNumber));

    if (deleteFromStorage) {
        Result = SmStorage_SlotDelete(SlotNumber);
    } else if (SlotNumber <= SmStorage_SIMSlotLogicalCapacityRd()) {
        // Delete SIM cache file if applicable
        SmStorage_DeleteSmFromSimCache(SlotNumber);
        Result = TRUE;
    }

    if (Result) {
        SMS_StorageStatus_t NewStorageStatus;

        // Slot Erased, Deallocate Slot
        SmStorage_SlotDeallocate(SlotNumber);

        // Now that the slot has been deleted AND deallocated, we can inform
        // inform clients if the storage status has changed
        SmStorage_AnalyseStorage(&NewStorageStatus);

        if (!SMS_MEM_CAPACITY_AVAILABLE_CLIENT_SEND && !(NewStorageStatus.StorageFullSIM && NewStorageStatus.StorageFullME)) {
            SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_SmDelete: Network Memory Capacity Available"));
            ShortMessageBearer_HandleEvent(SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, NULL);
        }

    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotPositionIsTemporary
 *
 * @description  Determine whether or not the slot storage position
 *               value resides within the overall temporary slot range
 *               (CLASS 0/TEMP MO/SM CONTROL DIR)
 *
 *
 * @param        StoragePosition : The slot storage position being checked
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_SlotPositionIsTemporary(
    const SMS_Position_t StoragePosition)
{
    if ((SMS_Position_t)FIRST_SLOT_TEMP <= StoragePosition &&
        (SMS_Position_t)LAST_SLOT_TEMP >= StoragePosition) {
        return TRUE;
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotIsTemporary
 *
 * @description  Determine whether or not the slot storage is in one
 *               of the temporary directories
 *
 *
 * @param        StoredObject_p : The object being stored / removed
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_SlotIsTemporary(
    const SmStorageObject_t * const StoredObject_p)
{
    if (StoredObject_p) {
        MSG_StorageMedia_t StorageMedia;

        if (SmStorage_GetVolume(StoredObject_p->StorageIndex, &StorageMedia)) {
            if (!strcmp(StorageMedia.StorageMediaName, SM_CONTROL_DIR)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotAllocate
 *
 * @description  Allocate the Specified Short message to the Specified Slot
 *
 *
 * @param        SlotNo     : SMS_Position_t
 * @param        SmInstance : as type of ShortMessage_p.
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_SlotAllocate(
    const SMS_Position_t SlotNo,
    SmStorageObject_t * StorageObject_p)
{
    ShortMessage_SetStoragePosition(StorageObject_p->ShortMessage, SlotNo);

    if (!MSG_ListCheckItemData(ShortMessageStorage.SmStorageMSG_List, StorageObject_p->ShortMessage)) {
        SmStorage_InsertStoredItem(ShortMessageStorage.SmStorageMSG_List, StorageObject_p);

        if (SlotNo <= SmStorage_SIMSlotSimTotalSpaceRd()) {
            //one slot used in sim
            SmStorage_SIMSlotSimUsedSpaceInc();
        } else if (!SmStorage_SlotIsTemporary(StorageObject_p)) {
            if (SMSTORAGESTATE_INITIALISE_COMPLETE == ShortMessageStorage_ReadState()) {
                ShortMessageStorage.MEUsedSpace++;
                (void) SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT, sizeof(uint16_t), &ShortMessageStorage.MEUsedSpace);
            } else {
                ShortMessageStorage.MEInitialisationCount++;
            }
        }

        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_SlotAllocate: Slot Number %d", SlotNo));
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotAccess
 *
 * @description  Access the ShortMessage Identity associated with the Specified Slot
 *
 * @param        SlotNo : SMS_Position_t
 *
 * @return       static ShortMessage_p
 */
/********************************************************************/
static SmStorageObject_t *SmStorage_SlotAccess(
    const SMS_Position_t SlotNo)
{
    SmStorageObject_t *StoredObject_p = NULL;

    while ((StoredObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StoredObject_p))) {
        if (SlotNo == ShortMessage_GetStoragePosition(StoredObject_p->ShortMessage)) {
            break;
        }
    }

    return StoredObject_p;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmIdentify
 *
 * @description  Wrapper for slot access. returns the sm of the slot number
 *
 * @param        SlotNo : Slot to get sm for
 *
 * @return       ShortMessage_p
 */
/********************************************************************/
ShortMessage_p SmStorage_SmIdentify(
    SMS_Position_t const SlotNo)
{
    SmStorageObject_t *StoredObject_p = SmStorage_SlotAccess(SlotNo);

    if (StoredObject_p) {
        return StoredObject_p->ShortMessage;
    }

    return NULL;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotDeallocate
 *
 * @description  Deallocate the Specified Slot
 *
 * @param        SlotNo : SMS_Position_t
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_SlotDeallocate(
    const SMS_Position_t SlotNo)
{
    SmStorageObject_t *StoredObject_p = SmStorage_SlotAccess(SlotNo);

    if (StoredObject_p) {
        if (!SmStorage_SlotIsTemporary(StoredObject_p)) {
            // Decrement the used space counter for the media type
            if (SlotNo > SmStorage_SIMSlotSimTotalSpaceRd()) {
                ShortMessageStorage.MEUsedSpace--;
                (void) SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT, sizeof(uint16_t), &ShortMessageStorage.MEUsedSpace);
            } else {
                if (SmStorage_SIMSlotSimUsedSpaceRd() > 0) {
                    SmStorage_SIMSlotSimUsedSpaceDec();
                }
            }
        }

        SmStorage_RemoveStoredItem(ShortMessageStorage.SmStorageMSG_List, StoredObject_p);
        SMS_HEAP_FREE(&StoredObject_p);
    }

    SMS_B_(SMS_LOG_D("smstrg.c: SlotDeallocate: Slot Number %d", SlotNo));
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotWrite
 *
 * @description  Write the given EfSms Record into the specified Slot
 *
 * @param        SlotNumber :       SMS_Position_t
 * @param        EfSmsRecord :      EfSmsRecord_t.
 * @param        *StorageVolume_p : MSG_StorageMedia_t
 * @param        SmInstance :       ShortMessage_p
 * @param        AllocateSlot :     uint8_t
 *
 * @return       static SmStoreErrorCode_t
 *
 * NOTES:        Persistent Data enables X bytes of the EfSmsRecord to be updated.
 *               However, SIM only allows a complete EfSmsRecord to be written.
 *               To Aid understanding, the minimum functionality, that of the SIM
 *               is applied to all storage Media Data
 */
/********************************************************************/
static SmStoreErrorCode_t SmStorage_SlotWrite(
    const SMS_Position_t SlotNumber,
    EfSmsRecord_t EfSmsRecord,
    const MSG_StorageMedia_t * StorageVolume_p,
    ShortMessage_p const SmInstance,
    const uint8_t AllocateSlot)
{
    SmStoreErrorCode_t StoreErrorCode = SM_STORE_NO_ERROR;

    SMS_C_(SMS_LOG_V("smstrg.c: SlotWrite: Slot Number %d", SlotNumber));

    if (StorageVolume_p) {
        SMS_C_(SMS_LOG_V("smstrg.c: SlotWrite: StorageVolume_p %s", (char *) StorageVolume_p));
        // Check for storage volume and slot number inconsistency
        if (!strcmp(StorageVolume_p->StorageMediaName, SIM_VOL)) {
            if (SmStorage_SIMSlotVerifiedCapacityRd() < SlotNumber) {
                StoreErrorCode = SM_STORE_FAILED;
                SMS_A_(SMS_LOG_E("smstrg.c: Attempt to write FS volume slot number %d to SIM", SlotNumber));
            }
        } else {
            if (SmStorage_SIMSlotLogicalCapacityRd() >= SlotNumber) {
                StoreErrorCode = SM_STORE_FAILED;
                SMS_A_(SMS_LOG_E("smstrg.c: Attempt to write a SIM slot number %d to FS volume", SlotNumber));
            }
        }
    }

    if (SM_STORE_NO_ERROR == StoreErrorCode) {
        // request EfSmsRecord storage to the SIM
        if (SmStorage_SIMSlotVerifiedCapacityRd() >= SlotNumber) {
            StoreErrorCode = SmStorage_WriteSmToSIM((uint8_t) SlotNumber, EfSmsRecord, SmInstance, AllocateSlot);

            // Handle write to SIM errors
            if (SM_STORE_NO_ERROR != StoreErrorCode) {
                SMS_A_(SMS_LOG_E("smstrg.c: failed to write slot number %d to SIM", SlotNumber));
            }
        } else if (FIRST_SLOT_ME <= SlotNumber && !SmStorage_WriteSmToFile(SlotNumber, EfSmsRecord, StorageVolume_p, SmInstance, AllocateSlot)) {
            StoreErrorCode = SM_STORE_FAILED;
            SMS_A_(SMS_LOG_E("smstrg.c: failed to write slot number %d to FS volume", SlotNumber));
        }

    }

    SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_SIMSlotLogicalCapacityRd = %d ", SmStorage_SIMSlotLogicalCapacityRd()));
    SMS_B_(SMS_LOG_D("SlotNumber = %d ", SlotNumber));
    SMS_B_(SMS_LOG_D("StoreErrorCode = %04x", StoreErrorCode));

    return StoreErrorCode;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetFileEntryTimeStamp
 *
 * @description  Get filesystem time by means of dummy write to a file
 *
 * @return       struct stat
 */
/********************************************************************/
struct stat SmStorage_GetFileEntryTimeStamp(
    void)
{
    FILE *File_p = NULL;
#ifdef PLATFORM_ANDROID
    // Android compiler complains about initialising the stat structures using the (valid)
    // C constrution in the #else case. Therefore we have to do it another way instead.
    // Android compiler warnings are (#### is line where struct stat VariableName = {0}; was located):
    // smstrg.c:####: warning: missing initializer
    // smstrg.c:####: warning: (near initialization for 'FileStats.__pad0')
    // smstrg.c:####: warning: missing initializer
    // smstrg.c:####: warning: (near initialization for 'PrevFileStats.__pad0')
    struct stat FileStats;
    static struct stat PrevFileStats;
    static uint8_t PrevFileStatsInitialised = FALSE;

    if (!PrevFileStatsInitialised) {
        memset(&PrevFileStats, 0x00, sizeof PrevFileStats);
        PrevFileStatsInitialised = TRUE;
    }
    memset(&FileStats, 0x00, sizeof FileStats);
#else
    struct stat FileStats = { 0 };
    static struct stat PrevFileStats = { 0 };
#endif

    if (!(File_p = fopen(SmTimeStampDummyFile, "wb"))) {
        SMS_A_(SMS_LOG_E("smstrg.c: Failed to get correct timestamp!"));
    } else {
        (void) fwrite(&PrevFileStats, sizeof(PrevFileStats), 1, File_p);
        (void) fclose(File_p);
        (void) stat(SmTimeStampDummyFile, &FileStats);
        (void) remove(SmTimeStampDummyFile);
    }

    if (FileStats.st_atime <= PrevFileStats.st_atime || FileStats.st_mtime <= PrevFileStats.st_mtime || FileStats.st_ctime <= PrevFileStats.st_ctime) {
        FileStats.st_atime = PrevFileStats.st_atime + 1;
        FileStats.st_mtime = PrevFileStats.st_mtime + 1;
        FileStats.st_ctime = PrevFileStats.st_ctime + 1;
    }

    PrevFileStats = FileStats;

    return FileStats;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_WriteSmToFile
 *
 * @description  Write the Specified Short Message to the file system
 *
 * @param        SlotNumber      :  SMS_Position_t
 * @param        EfSmsRecord     :  EfSmsRecord_t.
 * @param        *StorageVolume_p:  MSG_StorageMedia_t.
 * @param        ShortMessage_p  :  SmInstance
 * @param        AllocateSlot    :  uint8_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_WriteSmToFile(
    const SMS_Position_t SlotNumber,
    const EfSmsRecord_t EfSmsRecord,
    const MSG_StorageMedia_t * StorageVolume_p,
    ShortMessage_p const SmInstance,
    const uint8_t AllocateSlot)
{
    /* Note: This function may be extended to handle writing of SM to */
    /* files on external media once this interface is better defined. */
    char *SmPathname_p = NULL;
    SmStorageObject_t *StoredObject_p = NULL;

    FILE *File_p = NULL;
    uint8_t status = FALSE;
    uint8_t Pending = FALSE;

    struct stat TimeStamp;

    // If storage initialisation is not complete then write to the pending directory
    if (ShortMessageStorage_ReadState() != SMSTORAGESTATE_INITIALISE_COMPLETE) {
        // The pending directory is for MT sm's only
        if (ShortMessage_GetType(SmInstance) == SM_DELIVER || ShortMessage_GetType(SmInstance) == SM_STATUS_REPORT) {
            Pending = TRUE;
        }
    }

    SmPathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    if (SmPathname_p) {
        uint8_t FoundSmStorageDetails;
        uint16_t FileIndex = 0;

        if (!(FoundSmStorageDetails = SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SM, SlotNumber, SmPathname_p, &FileIndex))) {
            (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, StorageVolume_p, SmPathname_p);
        }

        if (!(File_p = fopen(SmPathname_p, "rb+"))
            && !(File_p = fopen(SmPathname_p, "wb"))) {
            SMS_A_(SMS_LOG_E("smstrg.c: Failed to open \"%s\" file for writing!", SmPathname_p));
        }

        if (File_p) {
            SMS_B_(SMS_LOG_D("smstrg.c: Successfully opened file for writing"));

            if (!FoundSmStorageDetails) {
                // Tie the slot number to the file index here
                FileIndex = SmStorage_SlotToIndex(SlotNumber);
            }

            SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, FileIndex);

            if (fwrite(EfSmsRecord, EFSMS_RECORD_SIZE, 1, File_p)) {
                TimeStamp = SmStorage_GetFileEntryTimeStamp();
                (void) fwrite(&TimeStamp, sizeof(TimeStamp), 1, File_p);
                if (!fclose(File_p)) {
                    status = TRUE;

                    if (AllocateSlot
                        /* Assign Storage Slot to ShortMessage Instance */
                        && (StoredObject_p = (SmStorageObject_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SmStorageObject_t)))) {
                        (void) SmStorage_GetVolumeIndex(StorageVolume_p, &StoredObject_p->StorageIndex);

                        StoredObject_p->FileIndex = (uint16_t) FileIndex;
                        StoredObject_p->ShortMessage = SmInstance;
                        StoredObject_p->PendingFlag = Pending;

                        SmStorage_SlotAllocate(SlotNumber, StoredObject_p);
                    }
                }
            } else {
                (void) fclose(File_p);
            }

            if (!status) {
                SMS_A_(SMS_LOG_E("smstrg.c: Failed to write to file"));
            }
        }
        // Free up dynamically allocated memory
        SMS_HEAP_FREE(&SmPathname_p);
    }

    return status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_WriteSmToSIM
 *
 * @description  Write the Specified Short Message to the SIM
 *
 * @param        SlotNumber   : uint8_t
 * @param        EfSmsRecord    as type of EfSmsRecored_t.
 * @param        SmInstance   : pointer to the short message.
 * @param        AllocateSlot : AllocateSlot
 *
 * @return       static SmStoreErrorCode_t
 */
/********************************************************************/
static SmStoreErrorCode_t SmStorage_WriteSmToSIM(
    uint8_t SlotNumber,
    EfSmsRecord_t EfSmsRecord,
    ShortMessage_p SmInstance,
    const uint8_t AllocateSlot)
{
    SmStoreErrorCode_t StoreErrorCode = SM_STORE_FAILED;
    uint16_t FileIndex = SMSTORAGE_FILEINDEX_INVALID;
    SmStorageObject_t *StoredObject_p = NULL;
    uint8_t RecordNumber = SlotNumber;
    MSG_StorageMedia_t StorageMedia;
    char *SmPathname_p = NULL;
    FILE *File_p = NULL;

    if (SmStorage_SIM_ShortMessage_Write(EfSmsRecord, &RecordNumber) == 0) {
        StoreErrorCode = SM_STORE_NO_ERROR;

        // Set up the storage media to SIM
        (void) strcpy(StorageMedia.StorageMediaName, SIM_VOL);

        if (SmStorage_SmToBeCached(SmInstance) && SMS_SIM_CACHE_MAX_SIZE >= SlotNumber) {
            SmPathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

            if (SmPathname_p) {
                // Get the filename, stored in the sm directory
                (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, &StorageMedia, SmPathname_p);

                if (!(File_p = fopen(SmPathname_p, "rb+"))
                    && !(File_p = fopen(SmPathname_p, "wb"))) {
                    SMS_A_(SMS_LOG_E("smstrg.c: Failed to create sim_cache file \"%s\" for writing!", SmPathname_p));
                } else {
                    SMS_B_(SMS_LOG_D("smstrg.c: Successfully created sim_cache file for writing"));

                    FileIndex = (uint16_t) (SlotNumber - 1);    // SlotNumber/FileIndex mapping predicatable on SIM

                    SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, FileIndex);

                    if (fwrite(EfSmsRecord, EFSMS_RECORD_SIZE, 1, File_p) == 1) {
                        struct stat TimeStamp;

                        TimeStamp = SmStorage_GetFileEntryTimeStamp();
                        (void) fwrite(&TimeStamp, sizeof(TimeStamp), 1, File_p);
                        (void) fclose(File_p);
                    } else {
                        SMS_A_(SMS_LOG_E("smstrg.c: Failed to write to sim_cache file"));
                        (void) fclose(File_p);
                    }
                }
                // Free the dynamically allocated memory for the filename
                SMS_HEAP_FREE(&SmPathname_p);
            }
        }
    }
    // Check for special case of OTA 8-bit message that may be stored
    // although the SIM returned an Unspecified Error due to erroneous data
#ifdef SMS_USE_MFL_UICC
    else
        // TODO
    if (SIM_ISO_ERROR_UNSPECIFIED == ErrorCause && FALSE == SmStorage_SmToBeCached(SmInstance)) {
        SMS_B_(SMS_LOG_E("smstrg.c: SmStorage_WriteSmToSIM SM_STORE_DATA_ERROR: Error Cause = %d", ErrorCause));
        StoreErrorCode = SM_STORE_DATA_ERROR;
    } else {
        SMS_B_(SMS_LOG_E("smstrg.c: SmStorage_WriteSmToSIM Error Cause = %d", ErrorCause));
    }
#endif

    if (AllocateSlot && (SM_STORE_NO_ERROR == StoreErrorCode || SM_STORE_DATA_ERROR == StoreErrorCode)) {
        StoredObject_p = (SmStorageObject_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SmStorageObject_t));
        (void) SmStorage_GetVolumeIndex((MSG_StorageMedia_t *) SIM_VOL, &StoredObject_p->StorageIndex);

        // Filename and slot number match for SIM Slots
        StoredObject_p->FileIndex = FileIndex;
        StoredObject_p->ShortMessage = SmInstance;
        StoredObject_p->PendingFlag = FALSE;

        SmStorage_SlotAllocate(SlotNumber, StoredObject_p);
    }

    return StoreErrorCode;
}

/********************************************************************/
/**
 * @function     SmStorage_SlotRead
 *
 * @description  Read the Specified Short Message
 *
 * @param        SlotNumber :  SMS_Position_t
 * @param        EfSmsRecord : as type of EfSmsRecored_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_SlotRead(
    const SMS_Position_t SlotNumber,
    EfSmsRecord_t EfSmsRecord)
{
    uint8_t Result = FALSE;
    char *SmFilename_p = NULL;

    SMS_C_(SMS_LOG_V("smstrg.c: SlotRead: Slot Number %d", SlotNumber));

    SmFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    if (SmFilename_p) {
        uint16_t SmFileIndex;

        if (!SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SM, SlotNumber, SmFilename_p, &SmFileIndex)
            || !strncmp(SmFilename_p, &NullString, 1)) {
            if (SmStorage_SlotType(SlotNumber) == SMS_STORAGE_SM) {
                ste_sim_t *sim_sync;
                uicc_request_status_t status = UICC_REQUEST_STATUS_FAILED_STATE;
                ste_uicc_sim_file_read_record_response_t read_result = {
                    STE_UICC_STATUS_CODE_FAIL,
                    STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                    {0x00, 0x00}
                    ,
                    0,
                    0
                };
                uint8_t length = EFSMS_RECORD_SIZE;
                sim_ef_t file_name = EF_SMS_FILENAME;
                sim_record_id_t record_id = (sim_record_id_t) SlotNumber;
                uintptr_t client_tag;

                SMS_C_(SMS_LOG_V("smstrg.c: SlotRead: Slot Number %d", SlotNumber));
                // request EfSmsRecord storage to the SIM
                sim_sync = MessagingSystem_UICC_SimSyncGet();
                client_tag = (uintptr_t) sim_sync;

                if (sim_sync) {
                    status = ste_uicc_sim_file_read_record_sync(sim_sync, client_tag, file_name, record_id, length, EF_SMS_FILEPATH, &read_result);
                }

                if (status == 0 && read_result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
                    if (read_result.length <= EFSMS_RECORD_SIZE) {
                        memcpy(EfSmsRecord, read_result.data, read_result.length);
                        Result = TRUE;
                    } else {
                        Result = FALSE;
                    }
                    if (read_result.data != NULL) {
                        free(read_result.data);
                    }
                } else {
                    SMS_A_(SMS_LOG_E("smstrg.c: ste_uicc_sim_file_read_record uicc_status_code:%d", read_result.uicc_status_code));
                }
            }
        } else {
            Result = SmStorage_ReadSmFromFile(SmFilename_p, SmFileIndex, EfSmsRecord);
        }

        SMS_HEAP_FREE(&SmFilename_p);
    }

    return Result;
}


/********************************************************************/
/**
 * @function     SmStorage_SIM_SMR_ShortMsg_Read_Available_Get
 *
 * @description  Get flag to indicate SIM_SMR_ShortMessage_Read is available
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_SIM_SMR_ShortMsg_Read_Available_Get(
    void)
{
    return SIM_SMR_Service_Available;
}


/********************************************************************/
/**
 * @function     SmStorage_SIM_SMR_ShortMsg_Read_Available_Set
 *
 * @description  Set flag to if SIM_SMR_ShortMessage_Read is available
 *
 * @return
 */
/********************************************************************/
static void SmStorage_SIM_SMR_ShortMsg_Read_Available_Set(
    const uint8_t ServiceAvailable)
{
    SIM_SMR_Service_Available = ServiceAvailable;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ReadSmFromFile
 *
 * @description  Read the Specified Short Message
 *
 * @param        SmPathFilename_p : name of the Short Message container file
 * @param        SmFileIndex      : index of Short Message to read from container file
 * @param        EfSmsRecord      : as type of EfSmsRecored_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_ReadSmFromFile(
    const char *const SmPathFilename_p,
    const uint16_t SmFileIndex,
    EfSmsRecord_t EfSmsRecord)
{
    FILE *File_p = NULL;
    uint8_t Result = FALSE;

    if (!(File_p = fopen(SmPathFilename_p, "rb"))) {
        SMS_A_(SMS_LOG_E("smstrg.c: Failed to open short message file for reading for SmFileIndex %d!", SmFileIndex));
        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_ReadSmFromFile: %s", SmPathFilename_p));
    } else {
        SMS_B_(SMS_LOG_D("smstrg.c: Successfully opened file for reading"));

        SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, SmFileIndex);

        if (fread(EfSmsRecord, EFSMS_RECORD_SIZE, 1, File_p) == 1) {
            Result = TRUE;
        }

        (void) fclose(File_p);
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_Index_Entry
 *
 * @description  Move the file cursor to an indexed entry
 *
 * @param        Container : SM/SR
 * @param        File_p    : Container file
 * @param        FileIndex : Index of entry
 *
 * @return
 */
/********************************************************************/
static void SmStorage_Index_Entry(
    const SmStorageContainer_t Container,
    FILE * File_p,
    const uint16_t FileIndex)
{
    uint16_t FileRecordLength = 0;

    switch (Container) {
    case SMSTORAGECONTAINER_SM:{
            FileRecordLength = SHORT_MESSAGE_FILE_ENTRY_LENGTH;
            break;
        }
    case SMSTORAGECONTAINER_SR:{
            FileRecordLength = STATUS_REPORT_FILE_ENTRY_LENGTH;
            break;
        }
    default:{
            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_Index_Entry: Undefined Container type %d", Container));
            break;
        }
    }

    if (FileRecordLength) {
        (void) fseek(File_p, (FileIndex * FileRecordLength), SEEK_SET);
        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_Index_Entry: Container = %d, file index = %d, file position = %d", Container, FileIndex, (FileIndex * FileRecordLength)));
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetContainerFilename
 *
 * @description  Get the path & filename of the appropriate SM/SR container
 *
 * @param        Container       : SM/SR
 * @param        StorageVolume_p : FS volume to search
 * @param        SmPathname_p    : [out] Container path & filename
 *
 * @return       SMS_Position_t : The slot range offset for this volume
 */
/********************************************************************/
static SMS_Position_t SmStorage_GetContainerFilename(
    const SmStorageContainer_t Container,
    const MSG_StorageMedia_t * StorageVolume_p,
    char *SmPathname_p)
{
    SMS_Position_t SlotRangeOffset = SMS_STORAGE_POSITION_INVALID;

    SMS_B_(SMS_LOG_D("SmStorage_GetContainerFilename: StorageVolume_p->StorageMediaName=%s", StorageVolume_p != NULL ? StorageVolume_p->StorageMediaName : "NULL"));

    memset(SmPathname_p, 0, SM_FULL_FILENAME_BYTES);

    if (StorageVolume_p != NULL && Container < SMSTORAGECONTAINER_INVALID) {
        if (SMSTORAGECONTAINER_SM == Container && !strcmp(StorageVolume_p->StorageMediaName, SM_CONTROL_DIR)) {
            SmStorage_GetSMControlPath( /*StorageVolume_p */ (MSG_StorageMedia_t *) SMS_FS_DIR_USE_SIM_CACHE, SmPathname_p);
            SlotRangeOffset = FIRST_SLOT_SMCONTROL;
        } else {
            if (SMSTORAGECONTAINER_SR == Container) {
                SmStorage_GetDeliveryStatusPath((MSG_StorageMedia_t *) StorageVolume_p, SmPathname_p);  // Cast away const
            } else {
                SmStorage_GetSMPath((MSG_StorageMedia_t *) StorageVolume_p, SmPathname_p);      // Cast away const
            }

            SlotRangeOffset = FIRST_SLOT_ME;
        }

        //Build up the complete file name and path
        switch (Container) {
        case SMSTORAGECONTAINER_SM:{
                (void) strcat(SmPathname_p, SmContainerFilename);
                break;
            }
        case SMSTORAGECONTAINER_SR:{
                (void) strcat(SmPathname_p, SrContainerFilename);
                break;
            }
        default:{
                memset(SmPathname_p, 0, SM_FULL_FILENAME_BYTES);
                break;
            }
        }
    }

    return SlotRangeOffset;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetStorageDetails
 *
 * @description  Get storage details from sm/sr index file for a
 *               particular short message
 *
 * @param        Container        : SM/SR
 * @param        SlotNumber       : SMS_Position_t
 * @param        Filename_p       : Full path name of file where SMS is stored
 * @param        FileIndex_p      : Index in file for Slot Number given
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_GetStorageDetails(
    const SmStorageContainer_t Container,
    const SMS_Position_t SlotNumber,
    char *PathFilename_p,
    uint16_t * FileIndex_p)
{
    SmStorageObject_t *StoredObject_p = SmStorage_SlotAccess(SlotNumber);

    if (StoredObject_p) {
        MSG_StorageMedia_t StorageMedia;

        (void) SmStorage_GetVolume(StoredObject_p->StorageIndex, &StorageMedia);

        if (SMSTORAGE_FILEINDEX_INVALID == StoredObject_p->FileIndex) {
            memset(PathFilename_p, 0, SM_FULL_FILENAME_BYTES);
            *FileIndex_p = 0;
        } else {
            (void) SmStorage_GetContainerFilename(Container, &StorageMedia, PathFilename_p);
            *FileIndex_p = StoredObject_p->FileIndex;
        }

        return TRUE;
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetSmStorageDetails
 *
 * @description  Get storage details from sm index file for a
 *               particular short message
 *
 * @param        SlotNumber       : SMS_Position_t
 * @param        SmFilename_p     : Full path name of file where SMS is stored
 * @param        SmFileIndex_p    : Index in file for Slot Number given
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_GetSmStorageDetails(
    const SMS_Position_t SlotNumber,
    char *SmPathFilename_p,
    uint16_t * SmFileIndex_p)
{
    return SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SM, SlotNumber, SmPathFilename_p, SmFileIndex_p);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetStorageVolume
 *
 * @description  Gets the storage volume for a particular short
 *
 * @param        SlotNumber : SMS_Position_t
 *
 * @return       static uint8_t
 */
/********************************************************************/
uint8_t SmStorage_GetStorageVolume(
    const SMS_Position_t SlotNumber,
    MSG_StorageMedia_t * const StorageVolume_p)
{
    SmStorageObject_t *StoredObject_p = SmStorage_SlotAccess(SlotNumber);

    if (StoredObject_p) {
        return SmStorage_GetVolume(StoredObject_p->StorageIndex, StorageVolume_p);
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotDelete
 *
 * @description  Delete the specified Short Message Slot
 *
 * @param        SlotNumber : SMS_Position_t
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_SlotDelete(
    const SMS_Position_t SlotNumber)
{
    uint8_t status = FALSE;
    // Allocate storage for this filename
    char *SmFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    SMS_B_(SMS_LOG_D("smstrg.c: SlotDelete: Slot Number %d", SlotNumber));

    if (SmFilename_p) {
        uint16_t SmFileIndex;

        // Get the details and filename for this short message slot
        if (SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SM, SlotNumber, SmFilename_p, &SmFileIndex)) {
            if (SlotNumber <= SmStorage_SIMSlotLogicalCapacityRd()) {
                if (SmStorage_DeleteSmFromSim(SlotNumber)) {
                    SMS_B_(SMS_LOG_D("smstrg.c: Successfully deleted SIM slot %d!", SlotNumber));
                    status = TRUE;
                } else {
                    SMS_A_(SMS_LOG_E("smstrg.c: Failed to delete SIM slot %d!", SlotNumber));
                }
            } else {
                if (SmStorage_DeleteSmFromVolume(SlotNumber, SmFilename_p, SmFileIndex)) {
                    SMS_B_(SMS_LOG_D("smstrg.c: Successfully deleted slot %d", SlotNumber));
                    status = TRUE;
                } else {
                    SMS_A_(SMS_LOG_E("smstrg.c: Failed to delete slot %d", SlotNumber));
                }
            }
        } else {
            // Couldn't determine filname for this short message slot
            SMS_A_(SMS_LOG_E("smstrg.c: Couldn't determine filename for slot %d!", SlotNumber));
        }

        SMS_HEAP_FREE(&SmFilename_p);
    }

    return status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DeleteSmFromSim
 *
 * @description  Delete the specified Short Message Slot
 *
 * @param        SlotNumber : SMS_Position_t
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_DeleteSmFromSim(
    const SMS_Position_t SlotNumber)
{
    uint8_t Status = FALSE;
    EfSmsRecord_t *EfSmsRecord_p;

    if ((EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t)))) {
        uint8_t RecordNumber = (uint8_t) SlotNumber;

        SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

        if (SmStorage_SIM_ShortMessage_Write(*EfSmsRecord_p, &RecordNumber) == 0) {
            Status = TRUE;
            (void) SmStorage_DeleteStatusReportFromVolume(SlotNumber);
        } else {
            SMS_B_(SMS_LOG_E("smstrg.c: SmStorage_DeleteSmFromSim"));
        }

        SMS_HEAP_FREE(&EfSmsRecord_p);
    }
    // Delete SIM cache file if applicable
    SmStorage_DeleteSmFromSimCache(SlotNumber);

    return Status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DeleteSmFromVolume
 *
 * @description  Delete the specified Short Message from the internal
 *               file system
 *
 * @param        SlotNumber       : SMS_Position_t
 * @param        SmPathFilename_p :  name of the Short Message container file
 * @param        SmFileIndex      :  index of Short Message to read from container file
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_DeleteSmFromVolume(
    const SMS_Position_t SlotNumber,
    const char *const SmPathFilename_p,
    const uint16_t SmFileIndex)
{
    FILE *File_p = NULL;
    uint8_t status = FALSE;

    if (!(File_p = fopen(SmPathFilename_p, "rb+"))) {
        SMS_A_(SMS_LOG_E("smstrg.c: Failed to open file for SM delete!"));
    } else {
        SMS_B_(SMS_LOG_D("smstrg.c: Successfully opened file for SM delete (%s)", SmPathFilename_p));

        SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, SmFileIndex);

        if (fwrite(DefaultShortMessageFileEntry, sizeof(DefaultShortMessageFileEntry), 1, File_p) == 1) {
            status = TRUE;
        }

        (void) fclose(File_p);
    }

    if (status) {
        (void) SmStorage_DeleteStatusReportFromVolume(SlotNumber);
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: Failed to delete sm file for slot %d!", SlotNumber));
    }

    return status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_IncrementInfoCount
 *
 * @description  Increment Info count based on the message status
 *
 * @param        storedObject_p  : as type of SmStorageObject_t.
 * @param        SmStorageInfo_p : as type of SMS_SlotInformation_t.
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_IncrementInfoCount(
    SmStorageObject_t * const storedObject_p,
    SMS_SlotInformation_t * const SmStorageInfo_p)
{
    if (storedObject_p) {
        switch (ShortMessage_GetStatus(storedObject_p->ShortMessage)) {
        case SMS_STATUS_READ:{
                SmStorageInfo_p->ReadCount++;
                break;
            }
        case SMS_STATUS_UNREAD:{
                SmStorageInfo_p->UnreadCount++;
                break;
            }
        case SMS_STATUS_SENT:{
                SmStorageInfo_p->SentCount++;
                break;
            }
        case SMS_STATUS_UNSENT:{
                SmStorageInfo_p->UnsentCount++;
                break;
            }
        default:
            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_IncrementInfoCount: ERROR unknown case"));
            break;
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotStatus
 *
 * @description  Derive the Status of the Short Messages within the
 *               Storage Slots specified
 *
 * @param        Storage         : as type of SMS_Storage_t.
 * @param        SmStorageInfo_p : as type of SmStorageStatusInformation_t.
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_SlotStatus(
    SMS_Storage_t const Storage,
    SMS_SlotInformation_t * const SmStorageInfo_p)
{
    SmStorageObject_t *StoredObject_p = NULL;
    SMS_Position_t SlotNumber = 1;
    SMS_StorageCapacity_t MessageStorageCapacity;

    SmStorageInfo_p->ReadCount = 0;
    SmStorageInfo_p->UnreadCount = 0;
    SmStorageInfo_p->SentCount = 0;
    SmStorageInfo_p->UnsentCount = 0;
    SmStorageInfo_p->FreeCount = 0;

    switch (Storage) {
        //SIM slot number count starts from 1
    case SMS_STORAGE_SM:

        while ((StoredObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StoredObject_p))) {
            SlotNumber = ShortMessage_GetStoragePosition(StoredObject_p->ShortMessage);

            if (SlotNumber <= SmStorage_SIMSlotLogicalCapacityRd()) {
                SmStorage_IncrementInfoCount(StoredObject_p, SmStorageInfo_p);
            }
        }

        //Now load up the free space on this volume
        (void) SmStorage_FindSIMCapacity(&MessageStorageCapacity);
        SmStorageInfo_p->FreeCount = MessageStorageCapacity.FreeSpace;
        break;

        //FLASH slot number count starts from total number of sim slots
    case SMS_STORAGE_ME:
        //can be optimised using IterateFromEnd
        while ((StoredObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StoredObject_p))) {
            SlotNumber = ShortMessage_GetStoragePosition(StoredObject_p->ShortMessage);

            if (!SmStorage_SlotIsTemporary(StoredObject_p)
                && SlotNumber > SmStorage_SIMSlotLogicalCapacityRd()) {
                SmStorage_IncrementInfoCount(StoredObject_p, SmStorageInfo_p);
            }
        }

        //Now load up the free space on this volume
        SmStorageInfo_p->FreeCount = SmStorage_GetAvailableMECapacity();
        break;

        //All storage medium
    case SMS_STORAGE_MT:

        while ((StoredObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StoredObject_p))) {
            if (!SmStorage_SlotIsTemporary(StoredObject_p)) {
                SmStorage_IncrementInfoCount(StoredObject_p, SmStorageInfo_p);
            }
        }

        // Add up free space on SIM and ME volumes for MT
        SmStorageInfo_p->FreeCount = SmStorage_GetAvailableMECapacity();
        (void) SmStorage_FindSIMCapacity(&MessageStorageCapacity);
        SmStorageInfo_p->FreeCount += MessageStorageCapacity.FreeSpace;
        break;

    default:
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_SlotStatus: ERROR unknown case"));
        break;

    }                           //ENDCASE: Storage
}


/********************************************************************/
/**
 *
 * @function     SmStorage_VolumeIsTemporary
 *
 * @description  Determine if the supplied storage media information
 *               refers to a temporary volume
 *
 * @param        Volume_p : The storage media to be checked
 *
 * @return       SmStorageTempVolume_t
 */
/********************************************************************/
static SmStorageTempVolume_t SmStorage_VolumeIsTemporary(
    const MSG_StorageMedia_t * Volume_p)
{
    if (Volume_p) {
        if (!strcmp(Volume_p->StorageMediaName, SM_CONTROL_DIR)) {
            return SMSTORAGETEMPVOLUME_SMCONTROL;
        }
    }

    return SMSTORAGETEMPVOLUME_INVALID;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotRangeGet
 *
 * @description  Return the first & last possible slot numbers for a
 *               particular volume
 *
 * @param        StorageMedia: Identifies storage media as SIM only,
 *                             FileSystem only or SIM and FileSystem
 * @param        TempVolume  : Identifies the sub-dividion of the temp
 *                             range as CLASS 0, TEMP MO, or SM CONTROL DIR
 * @param        FirstSlot_p : (optional) To return the first slot in
 *                             the range
 * @param        LastSlot_p  : (optional) To return the last slot in
 *                             the range
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_SlotRangeGet(
    const SMS_Storage_t StorageMedia,
    const SmStorageTempVolume_t TempVolume,
    SMS_Position_t * const FirstSlot_p,
    SMS_Position_t * const LastSlot_p)
{
    SMS_Position_t FirstSlot = SMS_STORAGE_POSITION_INVALID;
    SMS_Position_t LastSlot = SMS_STORAGE_POSITION_INVALID;

    if (SMS_STORAGE_ME == StorageMedia) {
        // The option to use temporary slot indices is only available with the SMS_STORAGE_ME storage option
        // Initialise to the correct slot range (ME/TEMP)
        switch (TempVolume) {
            // Initialise to SM CONTROL DIR slot range
        case SMSTORAGETEMPVOLUME_SMCONTROL:
            {
                FirstSlot = FIRST_SLOT_SMCONTROL;
                LastSlot = LAST_SLOT_SMCONTROL;
            }
            break;

            // Initialise to ME slot range
        case SMSTORAGETEMPVOLUME_INVALID:
        default:
            {
                FirstSlot = FIRST_SLOT_ME;
                LastSlot = LAST_SLOT_ME;
            }
            break;
        }
    } else {
        // Initialise to first possible SIM slot
        // The SIM slot range is also checked with the SMS_STORAGE_MT storage option set
        FirstSlot = FIRST_SLOT_SIM;     // Initialise to start of SIM slot range
        LastSlot = LAST_SLOT_SIM;       // Initialise to end of SIM slot range
    }

    if (FirstSlot_p) {
        *FirstSlot_p = FirstSlot;
    }

    if (LastSlot_p) {
        *LastSlot_p = LastSlot;
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotToIndex
 *
 * @description  Convert the Slot Index to a File Index, based on where the Slot
 *               Index resides in the Slot Range we can determine the offset to
 *               subtract, which will give the File Index.
 *
 * @param        Slot          : The slot number to be converted
 *
 * @return       uint16_t
 */
/********************************************************************/
static uint16_t SmStorage_SlotToIndex(
    const SMS_Position_t Slot)
{
    if ((SMS_Position_t)FIRST_SLOT <= Slot && (SMS_Position_t)LAST_SLOT >= Slot) {
        if ((SMS_Position_t)FIRST_SLOT_SIM <= Slot && (SMS_Position_t)LAST_SLOT_SIM >= Slot) {
            return (uint16_t) (Slot - FIRST_SLOT_SIM);
        } else if ((SMS_Position_t)FIRST_SLOT_ME <= Slot && (SMS_Position_t)LAST_SLOT_ME >= Slot) {
            return (uint16_t) (Slot - FIRST_SLOT_ME);
        } else if ((SMS_Position_t)FIRST_SLOT_SMCONTROL <= Slot && (SMS_Position_t)LAST_SLOT_SMCONTROL >= Slot) {
            return (uint16_t) (Slot - FIRST_SLOT_SMCONTROL);
        }
    }

    SMS_A_(SMS_LOG_E("smstrg.c: ERROR Slot Index %d (0x%04x) falls outside known range!!!", Slot, Slot));

    return SMSTORAGE_FILEINDEX_INVALID;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotRangeValidation
 *
 * @description  Verify that the slot specified falls within the range
 *               limits for the specified storage media or temporary
 *               volume
 *
 * @param        StorageMedia  : Identifies storage media as SIM only,
 *                               FileSystem only or SIM and FileSystem
 * @param        TempVolume    : Identifies the sub-dividion of the temp
 *                               range as CLASS 0, TEMP MO, or SM CONTROL DIR
 * @param        SlotIdentified: The slot number to be verified
 *
 * @return       SMS_Position_t
 */
/********************************************************************/
static SMS_Position_t SmStorage_SlotRangeValidation(
    const SMS_Storage_t StorageMedia,
    const SmStorageTempVolume_t TempVolume,
    SMS_Position_t SlotIdentified)
{
    if ((SMS_STORAGE_SM == StorageMedia || SMS_STORAGE_MT == StorageMedia) && SMSTORAGETEMPVOLUME_INVALID != TempVolume) {
        // TempVolume not permitted with storage media SMS_STORAGE_SM or SMS_STORAGE_MT
        SlotIdentified = SMS_STORAGE_POSITION_INVALID;
    } else {
        SMS_Position_t LastSlot_ME = LAST_SLOT_ME;

        if (SMS_MAX_STORAGE_ME_SHORT_MESSAGES) {
            LastSlot_ME = FIRST_SLOT_ME + SMS_MAX_STORAGE_ME_SHORT_MESSAGES - 1;
        }
        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_SlotRangeValidation LastSlot_ME %d", LastSlot_ME));

        switch (StorageMedia) {
        case SMS_STORAGE_SM:
            {
                // Inhibit an ME slot being assigned if media is SIM only
                if (SlotIdentified > SmStorage_SIMSlotVerifiedCapacityRd()) {
                    // All SIM slots are full
                    SlotIdentified = SMS_STORAGE_POSITION_INVALID;
                }
            }
            break;

        case SMS_STORAGE_MT:
            {
                // For SMS_STORAGE_MT case we will scan the SIM then ME slots
                if (SlotIdentified > SmStorage_SIMSlotVerifiedCapacityRd() && SlotIdentified < FIRST_SLOT_ME) {
                    // All SIM slots are full so check the ME slot range
                    /* NOTE: SmStorage_SlotRangeValidation is only called from within
                     *       SmStorage_SlotSearch, whilst it is in the process of determining
                     *       a free slot to allocate.  Therefore by setting the SlotIdentified
                     *       to the first in the ME range here, we are simply asking the
                     *       SlotSearch method to resume the search at a point later in
                     *       the slot range.  Purely so that we can omit searching through
                     *       the potentially invalid range that lies between the max sim
                     *       capacity and the max sim slot range. */
                    SlotIdentified = FIRST_SLOT_ME;
                } else if (SlotIdentified > LastSlot_ME) {
                    // All ME slots are full
                    SlotIdentified = SMS_STORAGE_POSITION_INVALID;
                }
            }
            break;

        case SMS_STORAGE_ME:
            {
                switch (TempVolume) {
                case SMSTORAGETEMPVOLUME_SMCONTROL:
                    {
                        if (SlotIdentified < (SMS_Position_t)FIRST_SLOT_SMCONTROL ||
                            SlotIdentified > (SMS_Position_t)LAST_SLOT_SMCONTROL ||
                            SlotIdentified >= ((SMS_Position_t)FIRST_SLOT_SMCONTROL + SMS_CONTROL_DIR_MAX_SIZE)) {
                            SlotIdentified = SMS_STORAGE_POSITION_INVALID;
                        }
                    }
                    break;

                case SMSTORAGETEMPVOLUME_INVALID:
                default:
                    {
                        if (SlotIdentified < FIRST_SLOT_ME || SlotIdentified > LastSlot_ME) {
                            SlotIdentified = SMS_STORAGE_POSITION_INVALID;
                        }
                    }
                    break;
                }
            }
            break;

        default:
            break;
        }
    }

    return SlotIdentified;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotSearch
 *
 * @description  Identify a free Slot within the specified range that meets
 *               the specifed search criteria
 *
 * @param        SearchMedia : Identifies storage media as SIM only,
 *                             FileSystem only or SIM and FileSystem
 * @param        Volume_p    : Identifies the sub-dividion of the temp
 *                             range as CLASS 0, TEMP MO, or SM CONTROL DIR
 *
 * @return       SMS_Position_t
 */
/********************************************************************/
SMS_Position_t SmStorage_SlotSearch(
    const SMS_Storage_t SearchMedia,
    const MSG_StorageMedia_t * Volume_p)
{
    SmStorageTempVolume_t TempVolume = SmStorage_VolumeIsTemporary(Volume_p);
    SMS_Position_t SlotIdentified = SMS_STORAGE_POSITION_INVALID;
    SMS_Position_t SmFileNumber = SMS_STORAGE_POSITION_INVALID;
    SMS_Position_t CurrentSlotNumber = SMS_STORAGE_POSITION_INVALID;
    SmStorageObject_t *StoredObject_p = NULL;

    SmStorage_SlotRangeGet(SearchMedia, TempVolume, &SlotIdentified, NULL);

    // Storage initialising so determine slot by searching sm container file for free file index
    if (ShortMessageStorage_ReadState() != SMSTORAGESTATE_INITIALISE_COMPLETE && SMSTORAGETEMPVOLUME_INVALID == TempVolume) {
        char *DstFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

        if (DstFilename_p) {
            FILE *File_p = NULL;
            memset(DstFilename_p, 0, SM_FULL_FILENAME_BYTES);
            (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, Volume_p, DstFilename_p);

            if (!(File_p = fopen(DstFilename_p, "rb"))) {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_SlotSearch failed to open/create destination file \"%s\"", DstFilename_p));
            } else {
                uint8_t EfSmsStatus = 0;

                SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_SlotSearch successfully opened/created destination file for writing"));
                // Search FileIndex of first empty sms record in the file
                while (fread(&EfSmsStatus, sizeof(uint8_t), 1, File_p)) {
                    if (EFSMS_RECORD_STATUS_FREE_MASK == EfSmsStatus) {
                        break;
                    }

                    SmFileNumber++;

                    // move the file pointer to the next sms record
                    (void) fseek(File_p, (EFSMS_RECORD_SIZE + sizeof(uint32_t) + sizeof(struct stat) - sizeof(uint8_t)), SEEK_CUR);
                }

                (void) fclose(File_p);

                SlotIdentified += SmFileNumber;
                SMS_A_(SMS_LOG_I("smstrg.c: SmStorage_SlotSearch found free FileIndex %d, SlotNumber %d for writing", SmFileNumber, SlotIdentified));
            }

            SMS_HEAP_FREE(&DstFilename_p);
        }
    } else {
        // For all SMS_Storage_t & MSG_StorageMedia_t types always do RAM list search first
        while ((StoredObject_p = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, StoredObject_p))) {
            // Determine the slot number for this shortmessage. The sm storage
            // list is ordered on the logical slot number.
            CurrentSlotNumber = ShortMessage_GetStoragePosition(StoredObject_p->ShortMessage);

            // Identify if the identified slot is already taken
            if (SlotIdentified < CurrentSlotNumber) {
                // There is a gap in allocated slots in the SM storage list therefore
                // the slot identified is available for use
                break;
            } else if (SlotIdentified == CurrentSlotNumber) {
                // Identified slot is already taken, try next slot
                SlotIdentified++;
                // Ensure the slot identified resides within the correct slot range
                SlotIdentified = SmStorage_SlotRangeValidation(SearchMedia, TempVolume, SlotIdentified);
                if (SlotIdentified == SMS_STORAGE_POSITION_INVALID) {
                    break;
                }
            }
        }                       // End of While loop
    }

    SMS_B_(if (SlotIdentified == SMS_STORAGE_POSITION_INVALID) {
           SMS_LOG_E("smstrg.c: Cannot identify slot within range for SearchMedia = %d", SearchMedia);
           if (SearchMedia == SMS_STORAGE_SM) {
               SMS_StorageStatus_t NewStorageStatus;

               // Inform clients if the storage status has changed
               SmStorage_AnalyseStorage(&NewStorageStatus);
               }
           }
           else
           {
           SMS_LOG_I("smstrg.c: SlotIdentified = %d within range for SearchMedia = %d", SlotIdentified, SearchMedia);}
    );

    return SlotIdentified;
}


/*
 * EFSMS FILE UTILITY FUNCTIONS
 */

/********************************************************************/
/** TODO This is the old stuff. Should not be used.
 * Kept for reference for a while.
 *
 * @function     SmStorage_SIM_ShortMessage_Write
 *
 * @description  Writes the supplied data in the Short Messages file
 *               (EF 6F3C) on the SIM or USIM.
 *
 * @param        SM_Data_p       SM message data to write to the file
 *               RecordNumber_p  The record number to update
 *               ErrorCause_p    Error code returned
 *
 * @return       RequestStatus
 */
/********************************************************************/
static uint8_t SmStorage_SIM_ShortMessage_Write(
    const uint8_t * const SM_Data_p,
    uint8_t * const RecordNumber_p)
{
    uint16_t Length = EFSMS_RECORD_SIZE;
    ste_sim_t *sim;
    sim_record_id_t RecordId = (sim_record_id_t) * RecordNumber_p;
    uint8_t retval = -1;
    uintptr_t client_tag = 0;   // dummy TODO
    ste_uicc_update_sim_file_record_response_t result = {
        STE_UICC_STATUS_CODE_FAIL,
        STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
        {0x00, 0x00}
    };

    sim = MessagingSystem_UICC_SimSyncGet();

    if (sim) {
        retval = ste_uicc_sim_file_update_record_sync(sim, client_tag, EF_SMS_FILENAME, RecordId, Length, EF_SMS_FILEPATH, SM_Data_p, &result);
    }

    if (retval != 0 || result.uicc_status_code != STE_UICC_STATUS_CODE_OK) {
        // EF_SMS not updated
        SMS_A_(SMS_LOG_E("smstrg.c:  SmStorage_SIM_ShortMessage_Write failed to update EF_SMS record %d", *RecordNumber_p));
        retval = -1;
    }
    return retval;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SimEfFileSizeRd
 *
 * @description  Returns the number of records (if present)
 *
 * @param        void
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_SimEfFileSizeRd(
    int fileName)
{
    uint8_t NoOfRecords = 0;
    int result = -1;
    ste_sim_t *sim_sync;
    ste_uicc_sim_file_get_format_response_t file_format = {
        STE_UICC_STATUS_CODE_FAIL,
        STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
        {0x00, 0x00},
        SIM_FILE_STRUCTURE_UNKNOWN,
        0,
        0,
        0
    };

    uintptr_t client_tag;

    // Initialise output parameter
    memset(&file_format, 0, sizeof(ste_uicc_sim_file_get_format_response_t));

    sim_sync = MessagingSystem_UICC_SimSyncGet();
    client_tag = (uintptr_t) sim_sync;

    result = ste_uicc_sim_file_get_format_sync(sim_sync, client_tag, fileName, EF_SMS_FILEPATH, &file_format);

    if (result == 0 && STE_UICC_STATUS_CODE_OK == file_format.uicc_status_code) {
        NoOfRecords = file_format.num_records;
        SMS_A_(SMS_LOG_I("smstrg.c:SmStorage_SimEfFileSizeRd file = 0x%08x, file_format.num_records = %d ", fileName, file_format.num_records));
    } else {
        SMS_A_(SMS_LOG_I
               ("smstrg.c:SmStorage_SimEfFileSizeRd Error file = 0x%08x, result = %d, file_format.status_code_fail_details = %d ", fileName, result, file_format.uicc_status_code_fail_details));
    }

    return NoOfRecords;
}

/********************************************************************/
/**
 *
 * @function     * SmStorage_EfSmsRecordTPDU
 *
 * @description  Identify the Tpdu from the specified EfSmsRecord
 *
 * @param        EfSmsRecord : as type of EfSmsRecord_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t *SmStorage_EfSmsRecordTPDU(
    EfSmsRecord_t EfSmsRecord)
{
    /*
     * Skip Service Centre Address
     */
    uint8_t EfSmsRecordPosition = SMS_MIN((unsigned) (EfSmsRecord[1] + 1), sizeof(ShortMessagePackedAddress_t));

    /*
     * Increment EfSmsRecordPosition to reference Tpdu Field
     */
    EfSmsRecordPosition++;
    /*
     * Extract RP-User Data (Tpdu) From EfSmsRecord
     */
    return &EfSmsRecord[EfSmsRecordPosition];
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EfSmsRecordInitialise
 *
 * @description  Initialise the given EfSmsRecord
 *
 * @param        EfSmsRecord : as type of EfSmsRecord_t.
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_EfSmsRecordInitialise(
    EfSmsRecord_t EfSmsRecord)
{
    /* Initialise EfSmsRecord: 3GPP ST 31.102 defines this as 00 FF .... FF */
    EfSmsRecord[0] = EFSMS_RECORD_STATUS_FREE_MASK;
    memset(&EfSmsRecord[1], 0xFF, (sizeof(EfSmsRecord_t) - 1));
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EfSmsRecordStatusCalculate
 *
 * @description  Calculate the EfSmsRecordStatus
 *
 * @param        SmStatus : as type of SMS_Status_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_EfSmsRecordStatusCalculate(
    const SMS_Status_t SmStatus)
{
    switch (SmStatus) {
    case SMS_STATUS_UNREAD:{
            return EFSMS_RECORD_STATUS_UNREAD_MASK;
        }
    case SMS_STATUS_READ:{
            return EFSMS_RECORD_STATUS_READ_MASK;
        }
    case SMS_STATUS_UNSENT:{
            return EFSMS_RECORD_STATUS_UNSENT_MASK;
        }
    case SMS_STATUS_SENT:{
            return EFSMS_RECORD_STATUS_SENT_MASK;
        }
    default:{
            return EFSMS_RECORD_STATUS_FREE_MASK;
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EfSmsRecordStatusGet
 *
 * @description  get EfSmsRecordStatus
 *
 * @param        EfSmsRecord_t EfSmsRecord
 *
 * @return       EfSmsRecordStatus_t*
 */
/********************************************************************/
static EfSmsRecordStatus_t SmStorage_EfSmsRecordStatusGet(
    const EfSmsRecord_t EfSmsRecord)
{
    return (EfSmsRecordStatus_t) EfSmsRecord[0];
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EfSmsRecordStatusSet
 *
 * @description  Assigns the EfSmsRecordStatus a new value
 *
 * @param        EfSmsRecord_t  EfSmsRecord
 *               const uint8_t    newFTpduStatus
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_EfSmsRecordStatusSet(
    EfSmsRecord_t EfSmsRecord,
    const uint8_t newFTpduStatus)
{
    EfSmsRecord[0] = newFTpduStatus;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EfSmsRecordStatusCalculate
 *
 * @description  Calculate the EfSmsRecordStatus
 *
 * @param        SmStatus : as type of ShortMessageStatus_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static SMS_Status_t SmStorage_ShortMessageStatusCalculate(
    const uint8_t fEfSmsRecordStatus)
{
    switch (fEfSmsRecordStatus & EFSMS_RECORD_STATUS_MASK) {
    case EFSMS_RECORD_STATUS_UNREAD_MASK:{
            return SMS_STATUS_UNREAD;
        }
    case EFSMS_RECORD_STATUS_READ_MASK:{
            return SMS_STATUS_READ;
        }
    case EFSMS_RECORD_STATUS_UNSENT_MASK:{
            return SMS_STATUS_UNSENT;
        }
    case EFSMS_RECORD_STATUS_SENT_MASK:{
            return SMS_STATUS_SENT;
        }
    default:{
            return SMS_STATUS_UNKNOWN;
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EfSmsRecordPack
 *
 * @description  Compose the EfSmsRecord for the given Short Message Instance
 *
 * @param        SmInstance :    as type of ShortMessage_p.
 * @param        EfSmsRecord_p : as type of EfSmsRecord_t.
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_EfSmsRecordPack(
    ShortMessage_p SmInstance,
    EfSmsRecord_t EfSmsRecord)
{
    uint8_t TpduLength;         /* Dummy Variable */

    /*
     * Extract Data Required within EfSmsRecord From the
     * Short Message Instance
     *  + Short Message Status
     *  + Service Centre Address
     *  + Tpdu Unit
     *
     * Initilaise the EfSmsRecord
     */
    SmStorage_EfSmsRecordInitialise(EfSmsRecord);

    /*
     * Set Short Message Status
     */
    SmStorage_EfSmsRecordStatusSet(EfSmsRecord, SmStorage_EfSmsRecordStatusCalculate(ShortMessage_GetStatus(SmInstance)));

    ShortMessage_GetPackedServiceCentreAddress(SmInstance, SmStorage_EfSmsRecordScAddress(EfSmsRecord));

    (void) ShortMessage_GetTpdu(SmInstance, &TpduLength, SmStorage_EfSmsRecordTPDU(EfSmsRecord));
}


/********************************************************************/
/**
 *
 * @function     SmStorage_EventHandler
 *
 * @description  Processes Events for the SmStorage Object
 *
 * @param        SmStorageEvent : as type of SmStorageEvent_t.
 *
 * @return       void
 *
 * NOTES:        As the SmStorageObject receives only one Event, SMSTORAGEEVENT_#.
 *               As a result, the event handler does not utilise pointer arithmatic
 *               to process the events. The Event Handler is kept simple for
 *               maintenance purposes only.
 */
/********************************************************************/
void SmStorage_EventHandler(
    SmStorageEvent_t SmStorageEvent)
{
    /*
     * Determine Event
     */
    switch (ShortMessageStorage.ShortMessageStorageState) {
    case SMSTORAGESTATE_VALIDATE_SM:
        switch (SmStorageEvent) {
        case SMSTORAGEEVENT_TIMEOUT:
            SmStorage_EventTimeout_SmValidate();
            break;

        case SMSTORAGEEVENT_STATUSUPDATE:
            SMS_B_(SMS_LOG_D("smstrg.c: StateMachine: EVENT IGNORED: State SMSTORAGEEVENT_STATUSUPDATE: Event %d:", SmStorageEvent));
            break;

        default:
            SMS_A_(SMS_LOG_E("smstrg.c: UNKNOWN EVENT: STATE VALIDATE SM"));
            break;
        }
        break;

    case SMSTORAGESTATE_INITIALISE_COMPLETE:
        switch (SmStorageEvent) {
        case SMSTORAGEEVENT_TIMEOUT:
            SMS_B_(SMS_LOG_D("smstrg.c: StateMachine: EVENT IGNORED: State SMSTORAGESTATE_INITIALISE_COMPLETE: Event %d:", SmStorageEvent));
            break;

        case SMSTORAGEEVENT_STATUSUPDATE:
            {
                SMS_StorageStatus_t NewStorageStatus;

                /* Update Status */
                SmStorage_AnalyseStorage(&NewStorageStatus);
            }
            break;

        default:
            SMS_A_(SMS_LOG_E("smstrg.c: UNKNOWN EVENT: STATE INITIALISE COMPLETE"));
            break;
        }
        break;

    case SMSTORAGESTATE_SIM_REFRESH:
        switch (SmStorageEvent) {
        case SMSTORAGEEVENT_TIMEOUT:
            SmStorage_EventTimeout_SIMRefresh();
            break;

        case SMSTORAGEEVENT_STATUSUPDATE:
            SMS_B_(SMS_LOG_D("smstrg.c: StateMachine: EVENT IGNORED: State SMSTORAGESTATE_SIM_REFRESH: Event %d:", SmStorageEvent));
            break;

        default:
            SMS_A_(SMS_LOG_E("smstrg.c: UNKNOWN EVENT: STATE VALIDATE SM"));
            break;
        }
        break;

    case SMSTORAGESTATE_SIM_RESTART:
        switch (SmStorageEvent) {
        case SMSTORAGEEVENT_TIMEOUT:
            SmStorage_EventTimeout_SIMRestart();
            break;

        case SMSTORAGEEVENT_STATUSUPDATE:
            SMS_B_(SMS_LOG_D("smstrg.c: StateMachine: EVENT IGNORED: State SMSTORAGESTATE_SIM_RESTART: Event %d:", SmStorageEvent));
            break;

        default:
            SMS_A_(SMS_LOG_E("smstrg.c: UNKNOWN EVENT: STATE VALIDATE SM"));
            break;
        }
        break;

    default:
        SMS_A_(SMS_LOG_E("smstrg.c: STATE UNKNOWN"));
        break;
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotType
 *
 * @description  Get the short message storage slot type
 *
 * @param        SlotNumber : uint16_t
 *
 * @return       uint8_t
 */
/********************************************************************/
SMS_Storage_t SmStorage_SlotType(
    SMS_Position_t SlotNumber)
{
    if (SlotNumber && SlotNumber <= SmStorage_SIMSlotLogicalCapacityRd()) {
        return SMS_STORAGE_SM;
    }

    return SMS_STORAGE_ME;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_WriteDeliveryStatus
 *
 * @description  Write the delivery status for the specified slot
 *
 * @param        SlotNumber     : SMS_Position_t.
 * @param        DeliveryStatus : uint8_t.
 *
 * @return       void
 */
/********************************************************************/
uint8_t SmStorage_WriteDeliveryStatus(
    const SMS_Position_t SlotNumber,
    const DeliveryStatus_t DeliveryStatus)
{
    // This function writes the single byte short version of a status
    // report to the first byte of the status report file.
    FILE *File_p = NULL;
    uint8_t status = FALSE;
    uint8_t RebuildStatusReport = FALSE;
    /* Get the pathname for this file */
    char *Filename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    if (Filename_p) {
        uint16_t fileIndexPosition = SMSTORAGE_FILEINDEX_INVALID;

        memset(Filename_p, 0, SM_FULL_FILENAME_BYTES);

        (void) SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SR, SlotNumber, Filename_p, &fileIndexPosition);

        if (!(File_p = fopen(Filename_p, "rb+"))) {
            //The status report container file does not exist so signal we need a new container file
            RebuildStatusReport = TRUE;
            SMS_B_(SMS_LOG_D("smstrg.c: Status report file does not already exist for this slot"));
        } else {
            //We succeded in opening the status report container
            SMS_B_(SMS_LOG_D("smstrg.c: Status report file already exist for this slot"));
        }

        //Do we have the signal to try to rebuild the status report container file
        if (RebuildStatusReport) {
            // File doesn't already exist
            // If the file won't open in "wb" mode...
            if (!(File_p = fopen(Filename_p, "wb"))) {
                //...report it to the debug log
                SMS_A_(SMS_LOG_E("smstrg.c: Failed to create delivery status report file \"%s\"", Filename_p));
            } else {
                // Extend the Status Report container file (if required) to be able to hold
                // FileSlotIndex number of entries
                SmStorage_ExtendStatusReportFile(File_p, fileIndexPosition);
            }
        }
        //If the file has been opened sucesfully...
        if (File_p) {
            //...we can write the status report to it
            SMS_B_(SMS_LOG_D("smstrg.c: Successfully create/opened file for writing"));
            // Index the Status Report
            SmStorage_Index_Entry(SMSTORAGECONTAINER_SR, File_p, fileIndexPosition);

            if (fwrite(&DeliveryStatus, sizeof(DeliveryStatus), 1, File_p)) {
                //...status report was written sucessfully
                if (!fclose(File_p)) {
                    status = TRUE;
                }
            } else {
                (void) fclose(File_p);
            }

            if (!status) {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteDeliveryStatus: Failed to write delivery status report file"));
            }
        }
        // Free up dynamically allocated memory
        SMS_HEAP_FREE(&Filename_p);
    }

    return status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ReadDeliveryStatus
 *
 * @description  Function to read delivery status from FS.
 *
 * @param        SlotNumber       : SMS_Position_t
 * @param        DeliveryStatus_p : DeliveryStatus_t*
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_ReadDeliveryStatus(
    const SMS_Position_t SlotNumber,
    DeliveryStatus_t * const DeliveryStatus_p)
{
    return SmStorage_ReadStatusReport(SlotNumber, DeliveryStatus_p, NULL);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ReadStatusReportTPDU
 *
 * @description  Function to read status report TPDU from FS.
 *
 * @param        SlotNumber       : SMS_Position_t
 * @param        StatusReport_p   : SMS_StatusReport_t*
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_ReadStatusReportTPDU(
    const SMS_Position_t SlotNumber,
    SMS_StatusReport_t * const StatusReport_p)
{
    return SmStorage_ReadStatusReport(SlotNumber, NULL, StatusReport_p);
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ReadStatusReport
 *
 * @description  Function to read delivery status and/or status report TPDU from
 *               FS.
 *               NOTE: Both delivery status and status report parameters cannot
 *                     be NULL.  Either or both must be supplied.
 *
 * @param        SlotNumber       : SMS_Position_t       (mandatory - in )
 * @param        DeliveryStatus_p : DeliveryStatus_t*        (optional  - out)
 * @param        StatusReport_p   : SMS_StatusReport_t* (optional  - out)
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_ReadStatusReport(
    const SMS_Position_t SlotNumber,
    DeliveryStatus_t * const DeliveryStatus_p,
    SMS_StatusReport_t * const StatusReport_p)
{
    uint8_t status = FALSE;

    if (DeliveryStatus_p || StatusReport_p) {
        FILE *File_p = NULL;
        /* Get the pathname for this file */
        char *Filename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

        if (Filename_p) {
            uint16_t fileIndexPosition = SMSTORAGE_FILEINDEX_INVALID;

            memset(Filename_p, 0, SM_FULL_FILENAME_BYTES);

            if (SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SR, SlotNumber, Filename_p, &fileIndexPosition)) {
                if (!(File_p = fopen(Filename_p, "rb"))) {
                    SMS_A_(SMS_LOG_E("smstrg.c: Failed to open status report file for reading"));

                    if (DeliveryStatus_p) {
                        *DeliveryStatus_p = STATUS_REPORT_RESET;
                    }
                } else {
                    DeliveryStatus_t DeliveryStatus;

                    SMS_B_(SMS_LOG_D("smstrg.c: Successfully opened status report file for reading"));
                    SmStorage_Index_Entry(SMSTORAGECONTAINER_SR, File_p, fileIndexPosition);

                    if (fread(&DeliveryStatus, sizeof(DeliveryStatus_t), 1, File_p)) {
                        if (DeliveryStatus_p) {
                            *DeliveryStatus_p = DeliveryStatus;
                            status = TRUE;
                        }
                    }

                    if (StatusReport_p) {
                        status = FALSE;

                        if (STATUS_REPORT_UNKNOWN != DeliveryStatus && fread(StatusReport_p, sizeof(SMS_StatusReport_t), 1, File_p)) {
                            status = TRUE;
                        }
                    }

                    (void) fclose(File_p);
                }
            }
            // Free up dynamically allocated memory
            SMS_HEAP_FREE(&Filename_p);
        }
    }

    return status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_WriteStatusReportTPDU
 *
 * @description  Write the raw stored delivery status TPDU for the
 *               specified slot number
 *
 * @param        SlotNumber   const SMS_Position_t
 * @param        StatusReport const SMS_StatusReport_t
 *
 * @return       void
 */
/********************************************************************/
uint8_t SmStorage_WriteStatusReportTPDU(
    const SMS_Position_t SlotNumber,
    SMS_Position_t * const fSRSlotNo,
    const SMS_StatusReport_t * StatusReport_p)
{
    uint8_t status = FALSE;
    MSG_StorageMedia_t StorageVolume;
    SMS_Position_t SRSlotNo = SMS_STORAGE_POSITION_INVALID;

    /* Get the pathname for this file */
    SMS_StatusReport_t *StatusReportTPDU_p = (SMS_StatusReport_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_StatusReport_t));

    *fSRSlotNo = SMS_STORAGE_POSITION_INVALID;


    if (StatusReportTPDU_p) {
        memset(StatusReportTPDU_p, 0, sizeof(SMS_StatusReport_t));

        // get the volume for the existing message from it's slot number.
        // this can be used to determine if the Status Report should be stored
        // on the SIM
        if (SmStorage_GetStorageVolume(SlotNumber, &StorageVolume)) {
            // Determine how long the status report is and copy it into the buffer to write
            // We always write the full 29byte status report tpdu so if the recevied tpdu is
            // less than 29 bytes all the trailing bytes are set to zeros.
            (void) GetStatusReportTPDU(StatusReport_p->StatusReport, StatusReportTPDU_p->StatusReport);

            // store the status report in the cache
            status = SmStorage_WriteStatusReportToCache(SlotNumber, StatusReportTPDU_p);

            // If the respective short message was stored on the SIM store the status report to the SIM also.
            if (!strcmp(SIM_VOL, StorageVolume.StorageMediaName)) {
                if (SmStorage_StoreSIMDeliveryStatus(SlotNumber, &SRSlotNo, StatusReportTPDU_p)) {
                    *fSRSlotNo = SRSlotNo;
                } else {
                    status = FALSE;
                }
            }
        }

        SMS_HEAP_FREE(&StatusReportTPDU_p);
    }

    return status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DeleteStatusReportFromVolume
 *
 * @description  Function to delete the delivery status report file
 *               for the specified slot number on the specified volume
 *
 * @param        SlotNumber :  SMS_Position_t
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_DeleteStatusReportFromVolume(
    const SMS_Position_t SlotNumber)
{
    FILE *File_p = NULL;
    char *Filename_p = NULL;
    uint8_t status = TRUE;

    /* Get the pathname for this file */
    Filename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    if (Filename_p) {
        uint16_t fileIndexPosition = SMSTORAGE_FILEINDEX_INVALID;

        memset(Filename_p, 0, SM_FULL_FILENAME_BYTES);

        (void) SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SR, SlotNumber, Filename_p, &fileIndexPosition);

        // Determine if this file already exists
        if (!(File_p = fopen(Filename_p, "rb+"))) {
            status = FALSE;
            SMS_B_(SMS_LOG_D("smstrg.c: Status report container file does not exist!"));
        } else {
            ///////////////////////////////////////////////////////////////
            //Set up a dummy block of data for expanding the container file.
            ///////////////////////////////////////////////////////////////
            uint8_t *lDummyStatus_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(STATUS_REPORT_FILE_ENTRY_LENGTH);

            memset(lDummyStatus_p, 0xff, STATUS_REPORT_FILE_ENTRY_LENGTH);
            memset(lDummyStatus_p, STATUS_REPORT_UNKNOWN, 1);

            //////////////////////////////////////////////////////////////////////////////////////
            //Write a dummy block over the Status Report entry in the container file for deletion
            //////////////////////////////////////////////////////////////////////////////////////
            SmStorage_Index_Entry(SMSTORAGECONTAINER_SR, File_p, fileIndexPosition);

            if (fwrite(lDummyStatus_p, STATUS_REPORT_FILE_ENTRY_LENGTH, 1, File_p)) {
                SMS_B_(SMS_LOG_D("smstrg.c: Successfully deleted status report for slot number %d", SlotNumber));
            } else {
                SMS_A_(SMS_LOG_E("smstrg.c: Failed to delete status report for slot number %d", SlotNumber));
                status = FALSE;
            }

            SMS_HEAP_FREE(&lDummyStatus_p);
            (void) fclose(File_p);
        }

        // if this is stored on a SIM volume, delete it from the SIM
        // by writing an empty report to it's location
        if (SmStorage_SIMSlotLogicalCapacityRd() >= SlotNumber) {
            if (!SmStorage_SIMFreeObsoleteSR(SlotNumber)) {
                status = FALSE;
            }
        }
        // Free up dynamically allocated memory
        SMS_HEAP_FREE(&Filename_p);
    }

    return status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_WriteStatusReportSM
 *
 * @description  Write a temporary Status Report Short Message to the FS
 *
 * @param        SmInstance      : The Status Report Sm to store
 * @param        StorageVolume_p : The volume to store on
 *
 * @return       SmStoreErrorCode_t
 */
/********************************************************************/
SmStoreErrorCode_t SmStorage_WriteStatusReportSM(
    const ShortMessage_p SmInstance,
    const MSG_StorageMedia_t * StorageVolume_p)
{
    SmStoreErrorCode_t StoreErrorCode = SM_STORE_FAILED;
    char *SmFilename_p = NULL;
    char *SmPathname_p = NULL;
    FILE *File_p = NULL;

    if (ShortMessage_GetType(SmInstance) != SM_STATUS_REPORT) {
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteStatusReportSM: ERROR - Short Message is not a Status Report"));
    } else {
        // Compose and Store an EfSmsRecord
        EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

        if (!EfSmsRecord_p) {
            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteStatusReportSM: Failed to malloc EfSmsRecord_p"));
        } else {
            SmStorage_EfSmsRecordPack(SmInstance, *EfSmsRecord_p);

            SmPathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX);

            if (!SmPathname_p) {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteStatusReportSM: Failed to malloc SmPathname_p!"));
            } else {
                memset(SmPathname_p, 0, FILENAME_MAX);
                SmFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(MAX_SM_FILENAME_BYTES);

                if (!SmFilename_p) {
                    SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteStatusReportSM: Failed to malloc SmFilename_p!"));
                } else {
                    // start by trying to create sm_CCCCCCCC_00001.sms
                    uint16_t SmFileNumber = 1;

                    memset(SmFilename_p, 0, MAX_SM_FILENAME_BYTES);
                    // Get the filename, stored in the sm directory
                    SmStorage_GetDeliveryStatusPath(StorageVolume_p, SmPathname_p);
                    SmStorage_CreateUniqueSmFilename(&SmFileNumber, SmPathname_p, SmFilename_p);
                    (void) strcat(SmPathname_p, SmFilename_p);

                    if (!(File_p = fopen(SmPathname_p, "wb"))) {
                        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteStatusReportSM: Failed to create Status Report temp sm file \"%s\" for writing!", SmPathname_p));
                    } else {
                        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_WriteStatusReportSM: Successfully created Status Report temp sm file for writing"));
                        // Write Status Report EfSmsRecord to the file
                        if (fwrite(EfSmsRecord_p, EFSMS_RECORD_SIZE, 1, File_p)) {
                            if (!fclose(File_p)) {
                                StoreErrorCode = SM_STORE_NO_ERROR;
                            }
                        } else {
                            (void) fclose(File_p);
                        }

                        if (SM_STORE_NO_ERROR != StoreErrorCode) {
                            (void) remove(SmPathname_p);
                            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_WriteStatusReportSM: Failed to write to Status Report temp sm file!"));
                        }
                    }

                    // Free up dynamically allocated memory
                    SMS_HEAP_FREE(&SmFilename_p);
                }

                // Free up dynamically allocated memory
                SMS_HEAP_FREE(&SmPathname_p);
            }

            // Free up dynamically allocated memory
            SMS_HEAP_FREE(&EfSmsRecord_p);
        }
    }

    return StoreErrorCode;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_RestoreStatusReportSM
 *
 * @description  Restore a Status Report Short Message from the FS
 *
 * @param        SmInstance      : The Status Report Sm restored
 * @param        StorageVolume_p : The volume to restore from
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_RestoreStatusReportSM(
    ShortMessage_p * SmInstance_p,
    const MSG_StorageMedia_t * StorageVolume_p)
{
    char *SrcDirName_p = (char *) SMS_HEAP_UNTYPED_ALLOC(sizeof(char) * FILENAME_MAX);
    static uint8_t RestoreCompleted = FALSE;
    static DIR *Directory_p = NULL;

    *SmInstance_p = NULL;

    if (SrcDirName_p) {
        SmStorage_GetDeliveryStatusPath(StorageVolume_p, SrcDirName_p);

        if (!Directory_p) {
            Directory_p = opendir(SrcDirName_p);

            if (!Directory_p) {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreStatusReportSM: - failed to open sr directory"));
                RestoreCompleted = TRUE;
            } else {
                rewinddir(Directory_p);
            }
        }

        if (Directory_p) {
            struct dirent *DirectoryEntry_p = NULL;

            // The directory is open search for a short message file
            if (!(DirectoryEntry_p = readdir(Directory_p))) {
                RestoreCompleted = TRUE;
                (void) closedir(Directory_p);
                Directory_p = NULL;
            } else {
                if (DirectoryEntry_p->d_name[0] != '.') {
                    // Check the length is correct for an sm, subtract 2 for the / and NULL characters
                    if (strlen(DirectoryEntry_p->d_name) == (MAX_SM_FILENAME - 2)) {
                        // Does it look like a short message file
                        // Files created on the MMC card currently have upper case filenames hence check for
                        // both lower case and upper case file suffixes
                        if (!strcmp(&DirectoryEntry_p->d_name[(strlen(DirectoryEntry_p->d_name) - SM_FILE_SUFFIX_LEN)], SmFileSuffix)
                            || !strcmp(&DirectoryEntry_p->d_name[(strlen(DirectoryEntry_p->d_name) - SM_FILE_SUFFIX_LEN)], SmFile_UC_Suffix)) {
                            char *SmFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

                            if (SmFilename_p) {
                                EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

                                if (EfSmsRecord_p) {
                                    FILE *File_p = NULL;

                                    SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

                                    (void) strcpy(SmFilename_p, SrcDirName_p);
                                    (void) strcat(SmFilename_p, DirSeparator);
                                    (void) strcat(SmFilename_p, DirectoryEntry_p->d_name);

                                    // We have a short message filename, now we have to read it
                                    if (!(File_p = fopen(SmFilename_p, "rb"))) {
                                        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreStatusReportSM: Failed to open short message file!"));
                                    } else {
                                        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_RestoreStatusReportSM: Successfully opened file for reading"));

                                        if (fread(EfSmsRecord_p, EFSMS_RECORD_SIZE, 1, File_p)) {
                                            if (!SmStorage_SmInvalidTPDU(*EfSmsRecord_p)) {
                                                // Restore the short message, slot number not required, hence 0
                                                *SmInstance_p = SmStorage_SmRestore(0, *EfSmsRecord_p);
                                            }
                                        } else {
                                            SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreStatusReportSM: Failed to read Status Report SM file"));
                                        }

                                        (void) fclose(File_p);
                                    }

                                    // The Status Report has been read so delete the temporary SM file
                                    if (!remove(SmFilename_p)) {
                                        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_RestoreStatusReportSM: Status Report SM file deleted"));
                                    } else {
                                        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_RestoreStatusReportSM: Failed to delete Status Report SM file"));
                                    }

                                    SMS_HEAP_FREE(&EfSmsRecord_p);
                                }

                                SMS_HEAP_FREE(&SmFilename_p);
                            }
                        }
                    }
                } else {
                    RestoreCompleted = TRUE;
                    (void) closedir(Directory_p);
                    Directory_p = NULL;
                }
            }
        }
        // Free up dynamically allocated memory
        SMS_HEAP_FREE(&SrcDirName_p);
    }

    return RestoreCompleted;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotCapacity_Get
 *
 * @description  Get the short message storage slot capacity
 *
 * @param        SlotLogicalSimCapacity : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
#ifdef CODE_NOT_CURRENTLY_USED  // Kept for future reference.
uint8_t SmStorage_SlotCapacity_Get(
    void)
{
    return ShortMessageStorage.SlotLogicalSimCapacity;
}
#endif


/********************************************************************/
/**
 * Function: SmStorage_MediaAndPositionToSlotAndVolume
 * Function Overview: Calculate the SlotNumber from MediaType and Position.
 * Function Description: As Above
 * Function Parameters:
 *   Input: Param#1: MediaType: Is the Storage Media
 *          Param#2: Position : Is the Slot Position.
 *   Output: None
 *  Return Value: uint 16     :Slot Number
 *  NOTES:
 *  #1: The function will operate within the limits of the system as currently
 *      perceived by the SmStorage Object ie.Verified Capacity
 */
/********************************************************************/
SMS_Position_t SmStorage_MediaAndPositionToSlotAndVolume(
    const SMS_Storage_t Storage,
    const SMS_Position_t Position,
    MSG_StorageMedia_t * SmVolume_p)
{
    SMS_Position_t SlotNumber = SMS_STORAGE_POSITION_INVALID;
    MSG_StorageMedia_t *AvailableMedia_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));

    // Verify Position Minimum Value
    if (SMS_STORAGE_POSITION_INVALID != Position) {
        // Determine what storage media is going to SlotNumber
        switch (Storage) {
        case SMS_STORAGE_SM:
            {
                if (SmStorage_SIMSlotSimTotalSpaceRd() >= Position) {
                    SlotNumber = Position;

                    if (SmVolume_p) {
                        (void) strcpy(SmVolume_p->StorageMediaName, SIM_VOL);
                    }
                }
            }
            break;
        case SMS_STORAGE_MT:
            {
                /* NOTE: IF SMS_STORAGE_MT HAS BEEN SPECIFIED THEN IT IS LEFT UP TO US TO
                 *       DETERMINE WHAT A PARTICULAR POSITION RELATES TO.  THEREFORE
                 *       THE SIMPLEST SOLUTION IS TO USE A SIMPLE LINEAR RANGE WHERE
                 *       WE EFFECTIVELY MAP THE POSITION ON TO A SLOT.  THE LINEAR
                 *       RANGE STARTS WITH SIM THEN ME.
                 *       EXAMPLE 1:
                 *        - POSITION = 3 (MAX SIM CAPACITY = 5)
                 *        - SLOTNUMBER = 3
                 *        - THIS IS A DIRECT MAPPING ON THE SIM RANGE
                 *       EXAMPLE 2:
                 *        - POSITION = 8 (MAX SIM CAPACITY = 5, LAST_SIM_SLOT = 10)
                 *        - SLOTNUMBER = (POSITION - MAX SIM CAPACITY) + LAST_SIM_SLOT
                 *                     = (8 - 5) + 10
                 *                     = 13
                 *        - THIS POSITION EXCEEDS THE SIM RANGE, THE AMOUNT BY WHICH
                 *          IT EXCEEDS THE SIM RANGE IS MAPPED DIRECTLY ON TO THE ME
                 *          RANGE
                 *       IF THE MAX SIM CAPACITY = 5 AND LAST_SIM_SLOT = 10 THEN THE
                 *       FOLLOWING DEPICTS THE MAPPING
                 *
                 * 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20
                 * |<--------sim slots-------->|<----------me slots--------->|
                 * |<max sim cp>|<invalidrange>|                               << there is an invalid slot range
                 *                                                                as we allocate more slots than
                 *       ^^                            ^^                         available sim capacity for future
                 *       example 1                     example 2                  expansion
                 *
                 *
                 *       THE PURPOSE OF THIS CALCULATION IS TO OMIT THE INVALID RANGE
                 *       SO:
                 *        - POSITION 1-5  MAP TO SIM RANGE 1-5
                 *        - POSITION 6-10 MAP TO ME  RANGE 11-15
                 */

                if (SmVolume_p) {
                    // If the position lies within the SIM capacity (and slot range) then simply set the slot number to the position
                    if (SmStorage_SIMSlotSimTotalSpaceRd() >= Position && LAST_SLOT_SIM >= Position) {
                        (void) strcpy(SmVolume_p->StorageMediaName, SIM_VOL);
                        SlotNumber = Position;
                    }
                    // Otherwise determine the available media
                    else {
                        if (SmStorage_GetAvailableVolume(AvailableMedia_p, 1, TRUE)) {
                            (void) strcpy(SmVolume_p->StorageMediaName, AvailableMedia_p->StorageMediaName);
                            //           |<-------this will give us the ME index------>|<-then add offset->|
                            SlotNumber = (Position - SmStorage_SIMSlotSimTotalSpaceRd()) + LAST_SLOT_SIM;
                        }
                    }
                } else {
                    // If the position lies within the SIM capacity (and slot range) then simply set the slot number to the position
                    if (SmStorage_SIMSlotSimTotalSpaceRd() > Position && LAST_SLOT_SIM >= Position) {
                        SlotNumber = Position;
                    }
                    // The position is outside the SIM capacity, so must be an ME position
                    else {
                        //           |<-------this will give us the ME index------>|<-then add offset->|
                        SlotNumber = (Position - SmStorage_SIMSlotSimTotalSpaceRd()) + LAST_SLOT_SIM;
                    }
                }
            }
            break;
        case SMS_STORAGE_ME:
            {
                if (SmVolume_p) {
                    if (SmStorage_GetAvailableVolume(AvailableMedia_p, 1, TRUE)) {
                        (void) strcpy(SmVolume_p->StorageMediaName, AvailableMedia_p->StorageMediaName);
                        SlotNumber = Position + LAST_SLOT_SIM;
                    }
                } else {
                    SlotNumber = Position + LAST_SLOT_SIM;
                }
            }
            break;
        default:
            {
                SMS_A_(SMS_LOG_E("smstrg.c: Unknown Storage type %d", Storage));
            }
            break;
        }
    }

    SMS_HEAP_FREE(&AvailableMedia_p);

    return SlotNumber;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SlotToMediaAndPosition
 *
 * @description  Converts a slot number to a storage media nd position
 *
 * @param        SlotNumber : <add description>.
 * @param        Storage : <add description>.
 * @param        Position : <add description>.
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_SlotToMediaAndPosition(
    const SMS_Position_t SlotNumber,
    SMS_Storage_t * Storage,
    SMS_Position_t * Position)
{
    if (LAST_SLOT_SIM >= SlotNumber) {
        *Storage = SMS_STORAGE_SM;
        *Position = SlotNumber;
    } else                      // this will handle all non-SIM slot numbers
    {
        *Storage = SMS_STORAGE_ME;
        *Position = SlotNumber - LAST_SLOT_SIM;
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_InitControlVolume
 *
 * @description  Initialises the SM_CONTROL_DIR volume.
 *
 * @param        none
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_InitControlVolume(
    void)
{
    // Get some working memory
    char *SmPathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);
    FILE *File_p = NULL;
    uint16_t FileIndex;

    if (!SmPathname_p) {
        SMS_A_(SMS_LOG_E("smstrg.c - SmStorage_InitControlVolume, pathname memory allocation failed"));
    } else {
        (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, (MSG_StorageMedia_t *) SM_CONTROL_DIR, SmPathname_p);

        if ((File_p = fopen(SmPathname_p, "rb+"))
            || (File_p = fopen(SmPathname_p, "wb"))) {
            uint8_t EfSmsStatus = 0;

            for (FileIndex = 0; FileIndex < SMS_CONTROL_DIR_MAX_SIZE; FileIndex++) {
                SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, FileIndex);

                if (fread(&EfSmsStatus, sizeof(uint8_t), 1, File_p) != 1) {
                    // Failed to read the SM Status so assume entry does not exist and container file must be expanded
                    if (fwrite(DefaultShortMessageFileEntry, sizeof(DefaultShortMessageFileEntry), 1, File_p) != 1) {
                        //Report failure of entry to be written
                        SMS_A_(SMS_LOG_E("smstrg.c: Failed to create default SM entry in control container file!"));
                    }
                }
            }

            (void) fclose(File_p);
        } else {
            SMS_A_(SMS_LOG_E("smstrg.c: Failed to create file \"%s\" for writing!", SmPathname_p));
        }

        if (SmPathname_p) {
            SMS_HEAP_FREE(&SmPathname_p);
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_DeleteAllSIMShortMessages
 *
 * @description  Delete all the short messages from the SIM
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_DeleteAllSIMShortMessages(
    void)
{
    EfSmsRecord_t *EfSmsRecord_p;
    uint8_t SlotNumber;

    EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

    if (EfSmsRecord_p) {
        SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

        for (SlotNumber = 1; SlotNumber <= SmStorage_SIMSlotLogicalCapacityRd(); SlotNumber++) {
            if (SmStorage_SIM_ShortMessage_Write(*EfSmsRecord_p, &SlotNumber) == 0) {
                //one message deleted from the sim slot
                SmStorage_SIMSlotSimUsedSpaceDec();

                (void) SmStorage_SIMFreeObsoleteSR(SlotNumber); // delete associated Status Report from EF_SMSR

            } else {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_DeleteAllSIMShortMessages"));
            }
        }

        SMS_HEAP_FREE(&EfSmsRecord_p);
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SmToBeCached
 *
 * @description  Delete all the short messages from the SIM
 *
 * @return       void
 */
/********************************************************************/
static uint8_t SmStorage_SmToBeCached(
    ShortMessage_p SmInstance)
{
    uint8_t DCS = ShortMessage_GetDataCodingScheme(SmInstance);

    // Check that it is not a Class2 8Bit message, these must not be cached
    // as they can be modified by the SIM and the cache would be innacurate
    if (SM_CLASS_2_SIM_SPECIFIC == ShortMessage_GetMessageClass(SmInstance)
        && (DCS_GENERAL_CODING_CLASS2_8BIT == DCS || DCS_CLASS2_8BIT == DCS)) {
        return FALSE;
    }

    return TRUE;
}


/********************************************************************/
/**
 *
 * @function     SMStorage_GetTimestampFromFileTime
 *
 * @description  Gets the filesystem timestamp for a particular short
 *               message
 *
 * @return       void
 */
/********************************************************************/
static void SMStorage_GetTimestampFromFileTime(
    ShortMessage_p SmInstance,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p)
{
    FILE *File_p = NULL;
    char *SmPathname_p = NULL;
    struct stat FileStats;

    ShortMessageTimestamp_p->Time = 0;
    ShortMessageTimestamp_p->Zone = 0;

    if (ShortMessage_GetStoragePosition(SmInstance) != SMS_STORAGE_POSITION_INVALID) {
        SmPathname_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

        if (SmPathname_p) {
            uint16_t SmFileIndex = SMSTORAGE_FILEINDEX_INVALID;

            if (SmStorage_GetStorageDetails(SMSTORAGECONTAINER_SM, ShortMessage_GetStoragePosition(SmInstance), SmPathname_p, &SmFileIndex)) {
                if (strncmp(SmPathname_p, &NullString, 1)) {
                    SmFileIndex *= SHORT_MESSAGE_FILE_ENTRY_LENGTH;
                    SmFileIndex += (EFSMS_RECORD_SIZE + sizeof(SMS_ClientTag_t));

                    if ((File_p = fopen(SmPathname_p, "rb+"))) {
                        if (!fseek(File_p, SmFileIndex, SEEK_SET)) {
                            if (fread(&FileStats, sizeof(FileStats), 1, File_p) != 0) {
                                if ((ShortMessage_GetType(SmInstance) == SM_SUBMIT)
                                    && (ShortMessage_GetStatus(SmInstance) == SMS_STATUS_SENT)) {
                                    ShortMessageTimestamp_p->Time = FileStats.st_mtime;
                                } else {
                                    ShortMessageTimestamp_p->Time = FileStats.st_ctime;
                                }
                            }
                        }

                        (void) fclose(File_p);
                    }
                } else {
                    // The SM is not stored in the filesystem - maybe a Class 2 SMS?
                    // So use the Service Centre timestamp instead
                    SMStorage_GetTimestampFromServiceCentre(SmInstance, ShortMessageTimestamp_p);
                }
            } else {
                SMS_A_(SMS_LOG_W("smstrg.c: SMStorage_GetTimestampFromFileTime: Failed to get storage details for slot number %d", ShortMessage_GetStoragePosition(SmInstance)));
            }

            SMS_HEAP_FREE(&SmPathname_p);
        }
    } else {
        // No file available to determine the system time, set it to 0
        SMS_C_(SMS_LOG_V("smstrg.c: SMStorage_GetTimestampFromFileTime: short message not stored, timestamp of 0 used"));
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_GetMessageTimestamp
 *
 * @description  Gets the storage timestamp for a particular short
 *               message, along with the time zone
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_GetMessageTimestamp(
    ShortMessage_p SmInstance,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p)
{
    if (0 == SMS_USER_DEFINED_TIMESTAMP) {
        // Returns the time stamp from the file created
        SMStorage_GetTimestampFromFileTime(SmInstance, ShortMessageTimestamp_p);
    } else {
        if (ShortMessage_GetMessageDirection(SmInstance) == SCtoMS) {
            // Mobile Terminated Message, so use Service Centre Timestamp & TimeZone
            SMStorage_GetTimestampFromServiceCentre(SmInstance, ShortMessageTimestamp_p);
        } else {
            // Mobile Originated Message (no SC timestamp), so use File 'last changed' time
            SMStorage_GetTimestampFromFileTime(SmInstance, ShortMessageTimestamp_p);
        }
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_PackTimestamp
 *
 * @description  Pack a ShortMessageTimeStamp_t into a MSG_DatAndTime_t
 *
 * @param        ShortMessageTimestamp_p : unpacked timestamp.
 * @param        timeStamp_p : resulting packed timestamp as type of uint8_t.
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_PackTimestamp(
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p,
    uint8_t * timeStamp_p)
{
    struct tm *LocalTime;
    uint8_t UnpackedTimeStamp[(SMS_TIME_STAMP_LENGTH * 2) + 1]; // 2 chars for each byte + string terminator
    uint8_t loop;
    uint8_t *CharBuffer = NULL;

    // Convert to tm structure
    LocalTime = localtime(&ShortMessageTimestamp_p->Time);

    CharBuffer = &UnpackedTimeStamp[0];

    sprintf((char *) CharBuffer, "%02i", LocalTime->tm_year - 100);     // Years minus 1900
    CharBuffer += ASCII_CHAR_SIZE;
    sprintf((char *) CharBuffer, "%02i", LocalTime->tm_mon + 1);        // Months start at 0
    CharBuffer += ASCII_CHAR_SIZE;
    sprintf((char *) CharBuffer, "%02i", LocalTime->tm_mday);
    CharBuffer += ASCII_CHAR_SIZE;
    sprintf((char *) CharBuffer, "%02i", LocalTime->tm_hour);
    CharBuffer += ASCII_CHAR_SIZE;
    sprintf((char *) CharBuffer, "%02i", LocalTime->tm_min);
    CharBuffer += ASCII_CHAR_SIZE;
    sprintf((char *) CharBuffer, "%02i", LocalTime->tm_sec);
    CharBuffer += ASCII_CHAR_SIZE;
    sprintf((char *) CharBuffer, "%02i", abs(ShortMessageTimestamp_p->Zone));

    // The return value must be packed in semi octects.
    for (loop = 0; loop < SMS_TIME_STAMP_LENGTH; loop++) {
        timeStamp_p[loop] = (UnpackedTimeStamp[2 * loop] - ASCII_ZERO) + ((UnpackedTimeStamp[2 * loop + 1] - ASCII_ZERO) << 4);
    }

    if (ShortMessageTimestamp_p->Zone < 0) {
        timeStamp_p[SMS_TIME_STAMP_TIME_ZONE_POSITION] |= 0x08;
    }
}


/********************************************************************/
/**
 *
 * @function     SMStorage_GetTimestampFromServiceCentre
 *
 * @description  Gets the and unpacks the Service Centre timestamp
 *               for a particular short message
 *
 * @return       void
 */
/********************************************************************/
static void SMStorage_GetTimestampFromServiceCentre(
    ShortMessage_p SmInstance,
    ShortMessageTimeStamp_t * ShortMessageTimestamp_p)
{
    struct tm LocalTime;
    uint8_t packedTimeStamp[SMS_TIME_STAMP_LENGTH];
    uint8_t loop;

    // Get packed SC timestamp from SM header
    ShortMessage_GetPackedTimeStamp(SmInstance, packedTimeStamp);

    // unpack the timestamp
    for (loop = 0; loop < SMS_TIME_STAMP_LENGTH; loop++) {
        // For the time zone byte, we need special handling of the sign bit (bit3) if it's set
        if (SMS_TIME_STAMP_TIME_ZONE_POSITION == loop && (packedTimeStamp[loop] & 0x08)) {
            packedTimeStamp[loop] = ~((packedTimeStamp[loop] & 0x07) * 10 + (packedTimeStamp[loop] >> 4 & 0x0f)) + 1;
        } else {
            packedTimeStamp[loop] = (packedTimeStamp[loop] & 0x0f) * 10 + (packedTimeStamp[loop] >> 4 & 0x0f);
        }
    }

    LocalTime.tm_year = packedTimeStamp[0] + 100;
    LocalTime.tm_mon = packedTimeStamp[1] - 1;
    LocalTime.tm_mday = packedTimeStamp[2];
    LocalTime.tm_hour = packedTimeStamp[3];
    LocalTime.tm_min = packedTimeStamp[4];
    LocalTime.tm_sec = packedTimeStamp[5];
    LocalTime.tm_isdst = 0;

    ShortMessageTimestamp_p->Time = mktime(&LocalTime);
    ShortMessageTimestamp_p->Zone = packedTimeStamp[6];

    if (ShortMessageTimestamp_p->Time == (time_t) - 1) {
        ShortMessageTimestamp_p->Time = 0;
        ShortMessageTimestamp_p->Zone = 0;
    }
}


/********************************************************************/
/**
 *
 * @function     void SmStorage_GetFileIndex
 *
 * @description  <Detailed description of the method>
 *
 * @param        *SmFilename_p : <add description>.
 * @param        FileIndex : <add description>.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_GetFileIndex(
    char *SmFilename_p,
    uint16_t * FileIndex_p)
{
    uint8_t Status = FALSE;
    int Index = SMSTORAGE_FILEINDEX_INVALID;

    if (!strcmp(&SmFilename_p[(strlen(SmFilename_p) - SM_FILE_SUFFIX_LEN)], SmFileSuffix)
        || !strcmp(&SmFilename_p[(strlen(SmFilename_p) - SM_FILE_SUFFIX_LEN)], SmFile_UC_Suffix)) {
        // Looks like an sms filename to me, get the index
        (void) sscanf(SmFilename_p + STORAGE_INDEX_OFFSET, "%05d", &Index);     // skip over /sm_CCCCCCCC_ before the index
        Status = TRUE;
    }

    *FileIndex_p = (uint16_t) Index;

    return Status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_SetFileIndex
 *
 * @description  Sets storage file index for given short message slot
 *               NOTE: THIS IS CURRENTLY ONLY USED FOR SIM SLOTS THAT'S WHY NO
 *                     SLOT OFFSETS ARE BEING USED
 *
 * @param        SlotNumber : Identifies an sm by its slot
 * @param        FileIndex  : File index for stored sm
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmStorage_SetFileIndex(
    const uint16_t SlotNumber,
    const uint16_t FileIndex)
{
    SmStorageObject_t *StorageObject_p = SmStorage_SlotAccess(SlotNumber);

    if (StorageObject_p) {
        StorageObject_p->FileIndex = FileIndex;
        ShortMessage_SetStoragePosition(StorageObject_p->ShortMessage, SlotNumber);
        return TRUE;
    }

    return FALSE;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_MoveStoredShortMessage
 *
 * @description  Moves a stored Short Message and associated Status
 *               Report if one exists to the volume specified
 *
 *               The design of this function is tied directly to the
 *               storage architecture and execution must take place
 *               in the following order:
 *
 *               1/ Read the Status Report TPDU before the SM referenced by
 *                  the old storage object has it's slot number changed
 *
 *               2/ Store the SM on the new volume
 *
 *               3/ Since the SM is effectively stored twice delete the old
 *                  stored object and adjust the SM ME count if applicable
 *                  WARNING : At this point although the old storage object
 *                            is still available the SM object to which it points
 *                            has been updated with a new slot number so a call to
 *                            SmStorage_SlotAccess using the old slot number
 *                            will return the wrong volume and file index
 *
 *               4/ Delete the original ME storage (or SIM cache) SM file
 *
 *               5/ Move the associated Status Report to the new volume
 *
 *               6/ If the respective short message was stored on the SIM and a valid status report exists
 *                  store the status report in the same respective slot on the SIM.
 *
 *               7/ If SM originated on sim, erase SM and corresponding SR record
 *
 * @param        theShortMessage  : ShortMessage_p.
 * @param        *Volume_p        : MSG_StorageMedia_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmStorage_MoveStoredShortMessage(
    ShortMessage_p thisShortMessage,
    MSG_StorageMedia_t * Volume_p)
{
    FILE *File_p = NULL;
    uint8_t Status = TRUE;
    uint8_t SRRecordNumber = SMS_STORAGE_POSITION_INVALID;
    DeliveryStatus_t DeliveryStatus = STATUS_REPORT_RESET;

    MSG_StorageMedia_t *OldVolume_p = NULL;
    char *OldFilename_p = NULL; // path/filename
    char *NewFilename_p = NULL; // path/filename
    char *SmFilename_p = NULL;  // filename

    SmStorageObject_t *OldStoredObject_p = NULL;
    SmStorageObject_t *NewStoredObject_p = NULL;

    OldVolume_p = (MSG_StorageMedia_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_StorageMedia_t));
    SmFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(MAX_SM_FILENAME_BYTES);
    OldFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);
    NewFilename_p = (char *) SMS_HEAP_UNTYPED_ALLOC(SM_FULL_FILENAME_BYTES);

    if (OldVolume_p && SmFilename_p && OldFilename_p && NewFilename_p) {
        SMS_Position_t OldSlot = ShortMessage_GetStoragePosition(thisShortMessage);
        SMS_Position_t NewSlot;

        if ((OldStoredObject_p = SmStorage_SlotAccess(OldSlot))) {
            uint16_t OldFileIndex = OldStoredObject_p->FileIndex;

            (void) SmStorage_GetVolume(OldStoredObject_p->StorageIndex, OldVolume_p);

            //Build the full path name for our container file in the passed in directory structure
            (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SM, OldVolume_p, OldFilename_p);

            if (strcmp(OldVolume_p->StorageMediaName, Volume_p->StorageMediaName)) {
                SMS_StatusReport_t *StatusReport_p = NULL;
                SmStoreErrorCode_t ErrorCode;
                uint8_t SRExists = FALSE;

                if ((StatusReport_p = (SMS_StatusReport_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(SMS_StatusReport_t)))) {
                    //****************************************************************
                    // 1/ Read the Status Report TPDU before the SM referenced by the
                    // old storage object has it's slot number changed
                    //****************************************************************
                    SRExists = SmStorage_ReadStatusReport(OldSlot, &DeliveryStatus, StatusReport_p);
                    SRRecordNumber = ShortMessage_GetStatusReportRecNum(OldStoredObject_p->ShortMessage);       // record number in EF_SMSR

                    //****************************************************************
                    // 2/ Store the SM on the new volume
                    //****************************************************************
                    NewSlot = SmStorage_SmSaveInVolume(thisShortMessage, Volume_p, &ErrorCode);

                    if (SMS_STORAGE_POSITION_INVALID != NewSlot) {
                        //****************************************************************
                        // 3/ WARNING - because we used SmStorage_SmSaveInVolume to store the SM
                        // in its new location, this has allocated a storage object so we've now
                        // got 2 storage objects pointing to one short message
                        //
                        // Since the SM is effectively stored twice delete the old
                        // stored object and adjust the SM ME count if applicable
                        // At this point although the old storage object
                        // is still available the SM object to which it points
                        // has been updated with a new slot number so a call to
                        // SmStorage_SlotAccess using the old slot number
                        // will return the wrong volume and file index
                        //****************************************************************
                        if (!SmStorage_SlotIsTemporary(OldStoredObject_p) && OldSlot > SmStorage_SIMSlotSimTotalSpaceRd()) {
                            ShortMessageStorage.MEUsedSpace--;
                            (void) SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT, sizeof(uint16_t), &ShortMessageStorage.MEUsedSpace);
                        }

                        SmStorage_RemoveStoredItem(ShortMessageStorage.SmStorageMSG_List, OldStoredObject_p);

                        //****************************************************************
                        // 4/ Delete the original ME storage (or SIM cache) SM file
                        //****************************************************************
                        if (strncmp(OldFilename_p, &NullString, 1)) {
                            EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

                            // Delete the old short message entry in the container file
                            if (EfSmsRecord_p) {
                                uint8_t *DummyEntry_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(SHORT_MESSAGE_FILE_ENTRY_LENGTH);

                                memset(DummyEntry_p, 0x00, SHORT_MESSAGE_FILE_ENTRY_LENGTH);
                                memset(DummyEntry_p + 1, 0xFF, EFSMS_RECORD_SIZE - 1);

                                SmStorage_EfSmsRecordInitialise(*EfSmsRecord_p);

                                //Write a dummy block over the SM entry for deletion in its container file
                                if ((File_p = fopen(OldFilename_p, "rb+"))) {
                                    SmStorage_Index_Entry(SMSTORAGECONTAINER_SM, File_p, OldFileIndex);

                                    if (fwrite(DummyEntry_p, SHORT_MESSAGE_FILE_ENTRY_LENGTH, 1, File_p) != 1) {
                                        SMS_A_(SMS_LOG_E("smstrg.c: Failed to delete SM in slot %d during a move", OldSlot));
                                        Status = FALSE;
                                    } else {
                                        SMS_B_(SMS_LOG_D("smstrg.c: Successfully deleted SM for slot number %d", OldSlot));
                                    }

                                    (void) fclose(File_p);

                                    File_p = NULL;
                                } else {
                                    SMS_A_(SMS_LOG_E("smstrg.c: Failed to open container file during a Move"));
                                    Status = FALSE;
                                }

                                SMS_HEAP_FREE(&EfSmsRecord_p);
                                SMS_HEAP_FREE(&DummyEntry_p);
                            }
                            //****************************************************************
                            // 5/ Move the associated Status Report to the new volume
                            //****************************************************************
                            if (SRExists) {
                                memset(OldFilename_p, 0, SM_FULL_FILENAME_BYTES);
                                (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SR, OldVolume_p, OldFilename_p);

                                //Check the entry in the container here is valid

                                if (STATUS_REPORT_RESET != DeliveryStatus) {
                                    // WARNING - The old storage object must have been deleted
                                    //           before accessing the new stored object via the
                                    //           new slot number. This is because the both stored
                                    //           objects reference the same SM so the old stored
                                    //           object may be returned even when using the new
                                    //           slot number if it is still present in the stored
                                    //           object list
                                    NewStoredObject_p = SmStorage_SlotAccess(NewSlot);

                                    if (NewStoredObject_p) {
                                        (void) SmStorage_GetContainerFilename(SMSTORAGECONTAINER_SR, Volume_p, NewFilename_p);
                                        Status = SmStorage_MoveSRBetweenContainerFiles(OldFilename_p, OldStoredObject_p->FileIndex, NewFilename_p, NewStoredObject_p->FileIndex);
                                    } else {
                                        Status = FALSE;
                                    }

                                    //****************************************************************
                                    // 6/ If the respective short message was stored on the SIM
                                    //    and a valid status report exists store the status report in
                                    //    the same respective slot on the SIM.
                                    //****************************************************************
                                    if (!strcmp(SIM_VOL, Volume_p->StorageMediaName)) {
                                        SMS_Position_t SRSlotNo = SMS_STORAGE_POSITION_INVALID;

                                        if (SmStorage_StoreSIMDeliveryStatus(NewSlot, &SRSlotNo, StatusReport_p)) {
                                            if (NewStoredObject_p) {
                                                ShortMessage_SetStatusReportRecNum(NewStoredObject_p->ShortMessage, (uint8_t) SRSlotNo);
                                                SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_MoveStoredShortMessage - SR TPDU written to EF_SMSR rec = %d", SRSlotNo));
                                            } else {
                                                SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_MoveStoredShortMessage - SR TPDU NOT written to EF_SMSR NewStoredObject_p = NULL"));
                                            }
                                        } else {
                                            Status = FALSE;
                                        }
                                    }
                                }       // end if DeliveryStatus != STATUS_REPORT_RESET
                            }
                        }       // end if strncmp(OldFilename_p, &NullString, 1)) != 0
                    }           // end if SMS_STORAGE_POSITION_INVALID

                    SMS_HEAP_FREE(&StatusReport_p);

                    //****************************************************************
                    // 7/ If SM originated on sim, erase SM and corresponding SR record
                    //****************************************************************
                    if (SmStorage_SIMSlotLogicalCapacityRd() >= OldSlot) {
                        uint8_t SIMStatus = FALSE;
                        uint8_t *EfSmsRecord_p = NULL;

                        // Overwrite the SIM SMS record
                        if ((EfSmsRecord_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t)))) {
                            uint8_t OldRecordNumber = (uint8_t) OldSlot;

                            SmStorage_EfSmsRecordInitialise(EfSmsRecord_p);

                            if (SmStorage_SIM_ShortMessage_Write(EfSmsRecord_p, &OldRecordNumber) == 0) {
                                SmStorage_SIMSlotSimUsedSpaceDec();

                                if (!SRExists) {
                                    // No associated Status Report and clearing SIM record successful
                                    SIMStatus = TRUE;
                                } else if (SRRecordNumber != SMS_STORAGE_POSITION_INVALID) {    // delete the associated SR from the SIM
                                    if (SmStorage_StatusReportDeleteFromSIM(SRRecordNumber)) {
                                        // No need to update OldStoredObject_p since it has already been removed
                                        SMS_B_(SMS_LOG_D("smstrg.c: SmStorage_MoveStoredShortMessage - SR TPDU written to SIM, slot = %d", OldSlot));
                                        SIMStatus = TRUE;
                                    } else {
                                        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_MoveStoredShortMessage Failed to delete SR slot = %d", OldSlot));
                                    }
                                }
                            } else {
                                SMS_B_(SMS_LOG_E("smstrg.c: SmStorage_MoveStoredShortMessage"));
                            }

                            SMS_HEAP_FREE(&EfSmsRecord_p);

                            if (Status) {
                                Status = SIMStatus;
                            }
                        }       // end if EfSmsRecord_p malloc
                    }           // end if old sm originates on sim
                }               // StatusReport_p MALLOC failed
            }                   // end if ensure move to new media

            // Finished with all the information from the old stored object
            SMS_HEAP_FREE(&OldStoredObject_p);
        }                       // end if OldStoredObject_p == NULL

    }                           // end if NewFilename_p == NULL && OldFilename_p == NULL && SmFilename_p == NULL && OldVolume_p == NULL

    SMS_HEAP_FREE(&NewFilename_p);
    SMS_HEAP_FREE(&OldFilename_p);
    SMS_HEAP_FREE(&SmFilename_p);
    SMS_HEAP_FREE(&OldVolume_p);

    return Status;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_MoveSRBetweenContainerFiles
 *
 * @description  Moves SRs between container files
 *
 * @param        char *SrcFilename_p the source container file for the Status Report
 * @param        uint16_t  SrcFileIndex   the file index to locate the source Status Report
 * @param        char *DstFilename_p the destination container file for the Status Report
 * @param        uint16_t  DstFileIndex   the file index to write the destination Status Report
 *
 * @return       uint8_t Succes         returned TRUE if the operation succeds.
 */
/********************************************************************/
static uint8_t SmStorage_MoveSRBetweenContainerFiles(
    const char *const SrcFilename_p,
    const uint16_t SrcFileIndex,
    const char *const DstFilename_p,
    const uint16_t DstFileIndex)
{
    FILE *SrcFile_p = NULL;
    FILE *DstFile_p = NULL;
    uint8_t *ContainerEntry_p = NULL;
    uint8_t Success = FALSE;

    SrcFile_p = fopen(SrcFilename_p, "rb+");

    if (!(DstFile_p = fopen(DstFilename_p, "rb+"))) {
        if ((DstFile_p = fopen(DstFilename_p, "wb"))) {
            // Extend the Status Report container file (if required) to be able to hold
            // FileSlotIndex number of entries
            SmStorage_ExtendStatusReportFile(DstFile_p, DstFileIndex);
        } else {
            SMS_A_(SMS_LOG_E("smstrg.c: Failed to create sr container file \"%s\" for writing!", DstFilename_p));
        }
    }

    ContainerEntry_p = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(STATUS_REPORT_FILE_ENTRY_LENGTH);

    if (SrcFile_p && DstFile_p && ContainerEntry_p) {
        /////////////////////////////////////////
        //Read Source slot entry from source file
        /////////////////////////////////////////
        SmStorage_Index_Entry(SMSTORAGECONTAINER_SR, SrcFile_p, SrcFileIndex);

        //Get the whole entry in ram
        if (fread(ContainerEntry_p, STATUS_REPORT_FILE_ENTRY_LENGTH, 1, SrcFile_p)) {
            /////////////////////////////////////////////
            //Write Source slot entry to Destination slot
            /////////////////////////////////////////////
            SmStorage_Index_Entry(SMSTORAGECONTAINER_SR, DstFile_p, DstFileIndex);

            //Set the whole entry in the destination file
            if (fwrite(ContainerEntry_p, STATUS_REPORT_FILE_ENTRY_LENGTH, 1, DstFile_p)) {
                Success = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("smstrg.c: could not write SR to destination during a move operation"));
            }
        } else {
            SMS_A_(SMS_LOG_E("smstrg.c: could not read original SR for a move operation"));
        }

        //////////////////////////
        //Delete source slot entry
        //////////////////////////
        memset(ContainerEntry_p, 0xff, STATUS_REPORT_FILE_ENTRY_LENGTH);
        memset(ContainerEntry_p, STATUS_REPORT_UNKNOWN, 1);

        SmStorage_Index_Entry(SMSTORAGECONTAINER_SR, SrcFile_p, SrcFileIndex);

        if (fwrite(ContainerEntry_p, STATUS_REPORT_FILE_ENTRY_LENGTH, 1, SrcFile_p) != 1) {
            SMS_A_(SMS_LOG_E("smstrg.c: failed to delete original SR during a move operation"));
            Success = FALSE;
        }
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_MoveSRBetweenContainerFiles could not get file/resources"));
    }

    ////////////////////////////
    //Tidy up after ourselves
    ////////////////////////////
    if (SrcFile_p) {
        (void) fclose(SrcFile_p);
        SrcFile_p = NULL;
    }
    if (DstFile_p) {
        (void) fclose(DstFile_p);
        DstFile_p = NULL;
    }
    if (ContainerEntry_p) {
        SMS_HEAP_FREE(&ContainerEntry_p);
        ContainerEntry_p = NULL;
    }

    return Success;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_StartSimRefresh
 *
 * @description  Refreshes the short messages stored on the SIM
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_StartSimRefresh(
    void)
{
    // Set the local sm storage state machine up
    ShortMessageStorage_WriteState(SMSTORAGESTATE_SIM_REFRESH);

    // Reset the SIM used space
    ShortMessageStorage.SimUsedSpace = 0;

    // Reinitialise SR Storage Capacity and cached data
    SmStorage_SIM_SR_LogicalCapacityWr(SmStorage_SimEfFileSizeRd(EF_SMSR_FILENAME));
    SmStorage_SIMSlotVerifiedCapacityWr(0);
    SmStorage_SIM_SR_VerifiedCapacityWr(0);
    AllSRsCached = FALSE;       // this will enable caching of EF_SMSR if present
    memset(StatusReportIsEmpty, FALSE, EF_SMSR_MAX_RECS);
    if (NULL != SIM_StatusReports_p) {  // Free temporary SR storage
        SMS_HEAP_FREE(&SIM_StatusReports_p);
        SIM_StatusReports_p = NULL;
    }

    SmStorage_SIM_SMR_ShortMsg_Read_Available_Set(TRUE);

    // Call the SIM Refresh event timeout function to start of processing
    // the SIM on a timer
    SmStorage_EventTimeout_SIMRefresh();
}


/********************************************************************/
/**
 *
 * @function     SmStorage_StartSimRestart
 *
 * @description  Refreshes the short messages stored on the SIM
 *               (varies from the SmStorage_StartSimRefresh only in that this is
 *                used to handle the SIM power off state changed event)
 *
 * @param        void
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_StartSimRestart(
    void)
{
    // Set the local sm storage state machine up
    ShortMessageStorage_WriteState(SMSTORAGESTATE_SIM_RESTART);

    // Reset the SIM used space
    ShortMessageStorage.SimUsedSpace = 0;

    // Reinitialise SR Storage Capacity and cached data
    SmStorage_SIM_SR_LogicalCapacityWr(SmStorage_SimEfFileSizeRd(EF_SMSR_FILENAME));
    SmStorage_SIMSlotVerifiedCapacityWr(0);
    SmStorage_SIM_SR_VerifiedCapacityWr(0);
    AllSRsCached = FALSE;       // this will enable caching of EF_SMSR if present
    memset(StatusReportIsEmpty, FALSE, EF_SMSR_MAX_RECS);
    if (NULL != SIM_StatusReports_p) {  // Free temporary SR storage
        SMS_HEAP_FREE(&SIM_StatusReports_p);
        SIM_StatusReports_p = NULL;
    }

    SmStorage_SIM_SMR_ShortMsg_Read_Available_Set(TRUE);

    // Call the SIM Restart event timeout function to start of processing
    // the SIM on a timer
    SmStorage_EventTimeout_SIMRestart();
}


/********************************************************************/
/**
 *
 * @function     SmStorage_ReplaceShortMessage
 *
 * @description  Replaces short message contents with that of another short message
 *
 * @param        OldShortMessage : The sm to be replaced
 * @param        NewShortMesasge : Contents of new sm replacing old one
 *
 * @return       SmStoreErrorCode_t
 */
/********************************************************************/
SmStoreErrorCode_t SmStorage_ReplaceShortMessage(
    ShortMessage_p OldShortMessage,
    ShortMessage_p NewShortMesasge)
{
    SmStoreErrorCode_t ErrorCode = SM_STORE_FAILED;
    /* Allocate Memory for EfSmsRecord */
    EfSmsRecord_t *EfSmsRecord_p = (EfSmsRecord_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(EfSmsRecord_t));

    if (EfSmsRecord_p) {
        SMS_Position_t SlotNumber = ShortMessage_GetStoragePosition(OldShortMessage);

        SmStorage_EfSmsRecordPack(NewShortMesasge, *EfSmsRecord_p);

        if (SMS_STORAGE_POSITION_INVALID != SlotNumber) {
            if (!SmStorage_SlotDelete(SlotNumber)) {
                SMS_A_(SMS_LOG_E("smstrg.c: SmStorage_ReplaceShortMessage Unable to delete Slot %d", SlotNumber));
            }

            /* Write EfSmsRecord to the Slot identified, don't alloc a slot, we've already got one
               storage volume is NULL since updating existing SlotNumber */
            if (SM_STORE_FAILED != (ErrorCode = SmStorage_SlotWrite(SlotNumber, *EfSmsRecord_p, NULL, NewShortMesasge, FALSE))) {
                // Remove old short message from TPDU cache, it will re-enter the cache on the next read
                ShortMessage_DeallocateTPDUStorage(OldShortMessage);
            }
        }

        /* Release Memory for EfSmsRecord  */
        SMS_HEAP_FREE(&EfSmsRecord_p);
    }

    return ErrorCode;
}


/********************************************************************/
/**
 *
 * @function     SmStorage_AnalyseStorage
 *
 * @description  Send the Dispatch if the status has changed
 *
 * @return       void
 */
/********************************************************************/
static void SmStorage_AnalyseStorage(
    SMS_StorageStatus_t * NewStorageStatus_p)
{
    static SMS_StorageStatus_t ExistingStorageStatus;

    SmStorage_StatusGet(NewStorageStatus_p);

    if (memcmp(NewStorageStatus_p, &ExistingStorageStatus, sizeof(SMS_StorageStatus_t)) || NewStorageStatus_p->StorageFullSIM == 1 ) {
        // some useful debug?
        SMS_A_(SMS_LOG_I("smstrg.c: Dispatch_MSG_SMS_StorageStatusChanged: SIM full=%d ME full=%d", NewStorageStatus_p->StorageFullSIM, NewStorageStatus_p->StorageFullME));
        // The status is not identical so send a dispatch
        Dispatch_SMS_StorageStatusChanged(*NewStorageStatus_p);

        // and update the existing status
        memcpy(&ExistingStorageStatus, NewStorageStatus_p, sizeof(SMS_StorageStatus_t));
    }
}


/********************************************************************/
/**
 *
 * @function     SmStorage_Construct_SM_Filename
 *
 * @description  Create an SM Filename of the format: -
 *
 *               /sm_SSSSS.sms
 *
 *               where SSSSS is the slot number.
 *
 *               e.g  /sm_00001.sms for slot 1 etc.
 *
 * @param        uint16_t StorageIndex : Storage Index
 *
 * @return       void
 */
/********************************************************************/
void SmStorage_Construct_SM_Filename(
    const uint16_t StorageIndex,
    char *const SMFilename_p)
{
    if (!StorageIndex) {
        memset(SMFilename_p, 0, MAX_SM_FILENAME_BYTES);
    } else {
        // Construct the short message filename.
        (void) snprintf(SMFilename_p,   // destination
                        MAX_SM_FILENAME,        // max chars in destination string
                        "/sm_%05d.sms", // /sm_SSSSS.sms
                        StorageIndex);  // %d
    }
}


/********************************************************************/
/**
 *
 * @function     isUniqueStorageNumber
 *
 * @description  Extract the Storage Number from the filename specified and check
 *               whether this has been used before. (i.e there are any other files
 *               in the directory that use the same Storage Index). Filename format is: -
 *
 *               /sm_SSSSS.sms
 *
 *               where SSSSS is the Storage Index number.
 *
 * @param        char*  :  path to be searched
 * @param        char*  :  Proposed filename
 *
 * @return       uint8_t : True if this Storage Index number does not appear in
 *                         another filename (in the directory specified).
 */
/********************************************************************/
static uint8_t isUniqueStorageNumber(
    char *Pathname_p,
    char *FilenameProposed_p)
{
    uint8_t FileIsUnique = TRUE;
    uint16_t StorageNumberProposed = 0;
    uint16_t ExistingStorageNumber = 0;
    struct dirent *DirectoryEntry_p = NULL;
    static DIR *Directory_p = NULL;

    // extract the Storage Index information from the filename given.
    (void) SmStorage_GetFileIndex(FilenameProposed_p, &StorageNumberProposed);

    // open the directory specified
    if (!(Directory_p = opendir(Pathname_p))) {
        SMS_A_(SMS_LOG_E("smstrg.c: isUniqueStorageNumber - failed to open directory"));
    } else {                    // directory opened ok, so read all the filenames contained in it.
        rewinddir(Directory_p);

        // While there are more entries in this directory & a match has not been found:
        while ((DirectoryEntry_p = readdir(Directory_p)) &&     // if more files to examine
               FileIsUnique)    // stop searching as soon as a match is found
        {
            // Do not examine "." or ".." directory entries.
            if (DirectoryEntry_p->d_name[0] != '.') {
                // Extract the Storage Index from the filename of this directory entry.
                (void) SmStorage_GetFileIndex(&(DirectoryEntry_p->d_name[0]), &ExistingStorageNumber);

                // Compare the two storage Indexes.
                if (ExistingStorageNumber == StorageNumberProposed) {   // this Storage Index has already been used, so stop searching any more filenames.
                    FileIsUnique = FALSE;
                }
            }
        }                       // end while

        (void) closedir(Directory_p);
    }                           //end if

    return FileIsUnique;
}

#endif                          //#ifndef REMOVE_SMSTRG_LAYER


uint8_t SmStorage_ReadMemoryCapacityExceeded(
    uint8_t * const MemCapacityExceeded_p)
{
    uintptr_t client_tag;
    ste_sim_t *sim_sync;
    int read_status;
    sim_ef_t file_name = EF_SMSS_FILENAME;
    uint8_t Success = FALSE;
    ste_uicc_sim_file_read_binary_response_t read_result = {
        STE_UICC_STATUS_CODE_FAIL,
        STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
        {0x00, 0x00},
        NULL,
        0
    };

    SMS_C_(SMS_LOG_V("smstrg.c: ReadMemoryCapacityExceeded"));

    sim_sync = MessagingSystem_UICC_SimSyncGet();
    client_tag = (uintptr_t) sim_sync;

    read_status = ste_uicc_sim_file_read_binary_sync(sim_sync, client_tag, file_name, EFSMSS_FILE_READ_OFFSET, EFSMSS_FILE_MIN_LENGTH, EF_SMS_FILEPATH, &read_result);

    if (read_status == 0 && read_result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
        SMS_C_(SMS_LOG_V("smstrg.c: ReadMemoryCapacityExceeded: ste_uicc_sim_file_read_binary_sync: Len = %d, data %sNULL", read_result.length, read_result.data != NULL ? "NOT " : ""));

        if (read_result.length >= EFSMSS_FILE_MIN_LENGTH && read_result.data != NULL) {
            Success = TRUE;
            SMS_B_(SMS_LOG_D("smstrg.c: ReadMemoryCapacityExceeded: ste_uicc_sim_file_read_binary_sync: MemCapExc Octet 0x%02X", read_result.data[EFSMSS_FILE_MEM_CAPACITY_EXC_OFFSET]));

            if (read_result.data[EFSMSS_FILE_MEM_CAPACITY_EXC_OFFSET] & EFSMSS_FILE_MEM_CAPACITY_EXC_MASK) {
                // Bit 1, as defined by 3GPP, set; therefore Memory capacity available
                *MemCapacityExceeded_p = FALSE;
            } else {
                // Bit 1, as defined by 3GPP, unset; therefore Memory capacity not available
                *MemCapacityExceeded_p = TRUE;
            }
        } else {
            SMS_A_(SMS_LOG_E("smstrg.c: ReadMemoryCapacityExceeded: ste_uicc_sim_file_read_binary_sync: Len = %d, data %sNULL", read_result.length, read_result.data != NULL ? "NOT " : ""));
        }
        if (read_result.data != NULL) {
            free(read_result.data);
        }
    } else {
        SMS_A_(SMS_LOG_E("smstrg.c: ReadMemoryCapacityExceeded: ste_uicc_sim_file_read_binary_sync: read_status %d, uicc_status_code %d.", read_status, read_result.uicc_status_code));
    }

    SMS_C_(SMS_LOG_V("smstrg.c: ReadMemoryCapacityExceeded: Success %d, MemCapEx %d", Success, *MemCapacityExceeded_p));
    return Success;
}



#ifdef TEST_MSG_LIST
/********************************************************************/
/**
 *
 * @function     SmStorage_PrintMSG_List
 *
 * @description  <Detailed description of the method>
 *
 * @param        void
 *
 * @return       static void
 */
/********************************************************************/
static void SmStorage_PrintMSG_List(
    void)
{
    uint16_t Index = 0;
    ShortMessage_p ShortMessage = NULL;
    SMS_Position_t Position = 0;

    SMS_A_(SMS_LOG_I("smstrg.c: SmStorage_PrintMSG_List"));
    SMS_A_(SMS_LOG_I("               Index    Position    AddressData"));

    while ((ShortMessage = SmStorage_IterateFromStart(ShortMessageStorage.SmStorageMSG_List, ShortMessage))) {
        Position = ShortMessage_GetStoragePosition(ShortMessage);
        SMS_A_(SMS_LOG_I("             [%2d]     [%2d]        %0x (%d)", Index, Position, ShortMessage, ShortMessage));
        Index++;
    }
}
#endif
