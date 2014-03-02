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
 *  The Short Message Bearer Object Utility.
 *
 *************************************************************************/

/* Utility Include Files */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"

/* Message Module Include Files */
#include "smutil.h"
#include "smsmain.h"


/* The Message Transport Server Interface */
#include "g_sms.h"
#include "r_sms.h"

#include "sim.h"

/* Dependant Modules */
#ifdef SMS_USE_MFL_UICC
#include "t_sim.h"              // For UICC_ServiceHandle_t
#include "r_sim.h"
#include "r_smr.h"
#endif                          // SMS_USE_MFL_UICC

/* Configuration */
#include "c_sms_config.h"


#ifndef SMS_USE_MFL_UICC

#define SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE 10
#define SMS_IMSI_LEN                        9
typedef unsigned char SMS_IMSI_t[SMS_IMSI_LEN];

#endif                          // SMS_USE_MFL_UICC

typedef struct {
    uint8_t ICCID_Changed;
    char Current_ICCID[(SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE * 2) + 1];       // *2 as 2 characters per byte
} Messaging_USIM_t;

static Messaging_USIM_t Messaging_USIM = {
    FALSE,                      // ICCID_Changed
    {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 0x00}  // Current_ICCID
};


// Path used to store SMS' Persistent Data is: /<mountpoint>/messaging/pstore/
const char SMS_FS_DIR_TOP_MOUNT_POINT[] = "/data/misc";
const char SMS_FS_DIR_MESSAGING[] = "/messaging";
static const char SMS_FS_DIR_PSTORE[] = "/pstore";
static const char SMS_FS_FILENAME_MEMORY_CAPACITY_EXCEEDED_FLAG[] = "/memcap.dat";
static const char SMS_FS_FILENAME_MESSAGE_REFERENCE[] = "/messref.dat";
static const char SMS_FS_FILENAME_STORED_SM_COUNT[] = "/storedsm.dat";
static const char SMS_FS_FILENAME_ICCID[] = "/iccid.dat";

static SMS_Error_t SmUtil_PersistentStorageData_ReadFile(
    const SMS_PersistentStorageDataType_t DataType,
    const size_t ReadSize,
    void *const Data_p,
    uint8_t * const FileExists_p);

static FILE *SmUtil_PersistentStorageData_FileOpen(
    const SMS_PersistentStorageDataType_t DataType,
    const uint8_t Create);

static uint8_t SmUtil_PersistentStorageData_CreateFileName(
    const SMS_PersistentStorageDataType_t DataType,
    const uint8_t Create,
    char *const FileNameBuffer_p);

static uint8_t SmUtil_PersistentStorageData_CreateAndCheckDirectory(
    const uint8_t Create,
    const char *const DirectoryName_p,
    char *const Buffer_p);


// When writing Message Waiting data to the SIM, use this Record Id
#define SIM_MWI_RECORD_ID    1

#define MCC_CANADA 302


//Local Const data
// According to 23.040, MTI of binary 00000011 shall be treated as a deliver
static const uint8_t ShortMessageTypes[MAX_SM_MESSAGE_DIRECTION][(SM_TYPE_LAST / 2)] = {
    {SM_DELIVER, SM_SUBMIT_REPORT_ERROR, SM_STATUS_REPORT, SM_DELIVER},
    {SM_DELIVER_REPORT_ERROR, SM_SUBMIT, SM_COMMAND, SM_DELIVER}
};

//Local function
static uint16_t SmUtil_GetPackedSizeOfAddress(
    uint8_t LengthInDigits,
    uint8_t * Addr_p);

#ifdef SMS_HEAP_CHECK
typedef unsigned short MSG_AllocListIndex;
#define NOPOS 0xffff

typedef struct MSG_AllocListItem {
    void *data;
} MSG_AllocListItem;

typedef struct MSG_AllocList {
    MSG_AllocListIndex maxCount;
    MSG_AllocListIndex count;
    MSG_AllocListIndex lastIndex;
    MSG_AllocListItem *item;
} MSG_AllocList;

void *MSG_AllocListGetFirst(
    MSG_AllocList *);
void *MSG_AllocListGetNext(
    MSG_AllocList *,
    const void *);
void *MSG_AllocListRemove(
    MSG_AllocList *,
    const void *);

static MSG_AllocList *HeapAllocList_p = NULL;


/********************************************************************/
/**
 *
 * @function     *MSG_HeapListCreate
 *
 * @description  Creates an item in the list
 *
 * @param        void :
 *
 * @return       MSG_AllocList
 */
/********************************************************************/
MSG_AllocList *MSG_HeapListCreate(
    void)
{
    MSG_AllocList *this_p = (MSG_AllocList *) malloc(sizeof(MSG_AllocList) * 1);

    if (NULL == this_p) {
        SMS_A_(SMS_LOG_E("ERROR: MSG_AllocListCreate failed to allocate memory for list"));
    } else {
        this_p->maxCount = 0;
        this_p->count = 0;
        this_p->lastIndex = NOPOS;
        this_p->item = NULL;
    }

    return this_p;
}


/********************************************************************/
/**
 *
 * @function     *MSG_AllocListRemoveItem
 *
 * @description  Removes an item from the list
 *
 * @param        *this_p : Identifies the list itself
 * @param        index :   Index of the item to be delete from the list
 *
 * @return       void
 */
/********************************************************************/
void *MSG_AllocListRemoveItem(
    MSG_AllocList * this_p,
    MSG_AllocListIndex index)
{
    MSG_AllocListIndex i = 0;
    void *data = NULL;

    // Store data before it is overwritten - support out-of-range index
    if (index < this_p->count) {
        data = this_p->item[index].data;
        for (i = index + 1; i < this_p->count; i++) {
            this_p->item[i - 1] = this_p->item[i];
        }
        this_p->count--;

        if (this_p->count == 0) {
            // List is empty so reset last index
            this_p->lastIndex = NOPOS;
        } else if (this_p->lastIndex != NOPOS && this_p->lastIndex != 0 && this_p->lastIndex >= index) {
            // An item before the last indexed item has been removed
            // so adjust the last index
            this_p->lastIndex--;
        }
    }

    return data;
}


/********************************************************************/
/**
 *
 * @function     MSG_AllocListFindItem
 *
 * @description  Seaches for an item in the list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Identifies the item to find in the list
 *
 * @return       MSG_AllocListIndex
 */
/********************************************************************/
MSG_AllocListIndex MSG_AllocListFindItem(
    const MSG_AllocList * this_p,
    const void *data)
{
    MSG_AllocListIndex resultIndex = NOPOS;

    // First check if the item is at the last cached position
    if (this_p->lastIndex != NOPOS && this_p->count != 0 && this_p->item[this_p->lastIndex].data == data) {
        resultIndex = this_p->lastIndex;
    } else {
        MSG_AllocListIndex index;

        // Item not at cached position so search the whole list
        for (index = 0; index < this_p->count; index++) {
            if (this_p->item[index].data == data) {
                // Item found
                resultIndex = index;
                break;
            }
        }
    }

    return resultIndex;
}


/********************************************************************/
/**
 *
 * @function     MSG_AllocListRemove
 *
 * @description  Removes an item from the list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Item to be removed from the list
 *
 * @return       void*
 */
/********************************************************************/
void *MSG_AllocListRemove(
    MSG_AllocList * this_p,
    const void *data)
{
    // Note: MSG_AllocListFindItem returns NOPOS if data not found.
    //       MSG_AllocListRemoveItem returns NULL if index is out of range (e.g. NOPOS).
    return MSG_AllocListRemoveItem(this_p, MSG_AllocListFindItem(this_p, data));
}


/********************************************************************/
/**
 *
 * @function     MSG_AllocListGetCount
 *
 * @description  Gets the number of items in the list
 *
 * @param        *this_p : Identifies the list
 *
 * @return       MSG_AllocListIndex
 */
/********************************************************************/
MSG_AllocListIndex MSG_AllocListGetCount(
    const MSG_AllocList * this_p)
{
    return this_p->count;
}


/********************************************************************/
/**
 *
 * @function     *MSG_AllocListGetFirst
 *
 * @description  Gets first item fro the list
 *
 * @param        *this_p : Identifies the list.
 *
 * @return       void
 */
/********************************************************************/
void *MSG_AllocListGetFirst(
    MSG_AllocList * this_p)
{
    if (this_p->count == 0) {
        // List is empty so reset last index
        this_p->lastIndex = NOPOS;

        return NULL;
    } else {
        this_p->lastIndex = 0;

        return this_p->item[0].data;
    }
}


/********************************************************************/
/**
 *
 * @function     *MSG_AllocListGet
 *
 * @description  Gets item at specified index from the list
 *
 * @param        *this_p : Identifies the list.
 * @param        index :   Index of item within the list to get
 *
 * @return       void
 */
/********************************************************************/
void *MSG_AllocListGet(
    MSG_AllocList * this_p,
    MSG_AllocListIndex index)
{
    if (index < this_p->count) {
        this_p->lastIndex = index;

        return this_p->item[index].data;
    } else {
        return NULL;
    }
}


/********************************************************************/
/**
 *
 * @function     *MSG_AllocListGetNext
 *
 * @description  Get the next item in a list given a previous item
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Item in the list to get the next one off
 *
 * @return       void
 */
/********************************************************************/
void *MSG_AllocListGetNext(
    MSG_AllocList * this_p,
    const void *data)
{
    MSG_AllocListIndex index;

    index = MSG_AllocListFindItem(this_p, data);

    if (index != NOPOS && index < this_p->count - 1) {
        this_p->lastIndex = index + 1;

        return this_p->item[index + 1].data;
    } else {
        return NULL;
    }
}

static MSG_AllocListIndex HeapFindNextSize(
    MSG_AllocListIndex oldSize)
{

    switch (oldSize) {
    case 0:
        return 200;
    case 200:
        return 400;
    case 400:
        return 600;
    default:
        return oldSize + 200;
    }
}

void MSG_HeapListInsertItem(
    MSG_AllocList * this_p,
    MSG_AllocListIndex index,
    const void *data)
{
    MSG_AllocListIndex i;

    // Make sure that there is space
    if (this_p->count == this_p->maxCount) {
        // allocate more space - this is only done here
        MSG_AllocListItem *newItem;
        MSG_AllocListIndex oldMaxCount;
        MSG_AllocListIndex newMaxCount;

        oldMaxCount = this_p->maxCount;
        newMaxCount = HeapFindNextSize(oldMaxCount);

        // allocate a new list
        newItem = (MSG_AllocListItem *) malloc(sizeof(MSG_AllocListItem) * newMaxCount);

        // Copy the old list
        for (i = 0; i < oldMaxCount; i++) {
            newItem[i] = this_p->item[i];
        }

        // Free the old list
        if (this_p->item != NULL) {
            free(this_p->item);
        }
        // Set the list
        this_p->item = newItem;

        // Update list
        this_p->maxCount = newMaxCount;
    }

    for (i = this_p->count; i > index; i--) {
        this_p->item[i] = this_p->item[i - 1];
    }

    this_p->item[index].data = (void *) data;
    this_p->count++;

    if (this_p->lastIndex != NOPOS && this_p->lastIndex >= index) {
        // An item before the last indexed item has been added
        // so adjust the last index
        this_p->lastIndex++;
    }
}

static uint32_t HeapAllocTotal = 0;
static uint32_t HeapAllocPeak = 0;

typedef struct {
    void *memoryPointer;
    uint32_t size;
    const char *filename;
    size_t line;
} HeapEntry_t;


/********************************************************************/
/**
 *
 * @function     SmUtil_HeapStatusDump
 *
 * @description  Iterates through all of the currently allocated blocks
 *               and prints to the log the size of the block and the
 *               file and line number where it was allocated.
 *
 * @param        None.
 *
 * @return       void
 */
/********************************************************************/
static void SmUtil_HeapStatusDump(
    void)
{
    uint32_t BlockCount = 1;
    HeapEntry_t *HeapEntry_p = (HeapEntry_t *) MSG_AllocListGetFirst(HeapAllocList_p);

    while (HeapEntry_p != NULL) {
        SMS_A_(SMS_LOG_I("smutil.c: %4d BufSize %4d %s %d ", BlockCount, HeapEntry_p->size, HeapEntry_p->filename, HeapEntry_p->line));
        BlockCount++;
        HeapEntry_p = (HeapEntry_t *) MSG_AllocListGetNext(HeapAllocList_p, HeapEntry_p);
    }
}


/********************************************************************/
/**
 *
 * @function     SmUtil_HeapStatus
 *
 * @description  Function for returning the status of the Memory
 *               used for testing only
 *
 * @param        None.
 *
 * @return       uint32_t (number of buffers)
 */
/********************************************************************/
uint32_t SmUtil_HeapStatus(
    void)
{
    uint32_t NumBlocks = MSG_AllocListGetCount(HeapAllocList_p);
    SMS_A_(SMS_LOG_I("smutil.c: Total  %5d bytes allocated", HeapAllocTotal));
    SMS_A_(SMS_LOG_I("smutil.c: Peak   %5d bytes allocated", HeapAllocPeak));
    SMS_A_(SMS_LOG_I("smutil.c: Blocks %5d blocks allocated", NumBlocks));

    SmUtil_HeapStatusDump();

    return NumBlocks;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_HeapUntypedAlloc
 *
 * @description  Function for messaging system to allocate memory
 *
 * @param        sizeToMalloc : uint32_t type.
 *
 * @return       void *
 */
/********************************************************************/
void *SmUtil_HeapUntypedAlloc(
    const uint32_t sizeToMalloc,
    const char *filename,
    const size_t line)
{
    void *memoryPointer = NULL;

#ifdef SMS_HEAP_CHECK_ENHANCED_DEBUG
    SMS_A_(SMS_LOG_I("SmUtil_HeapUntypedAlloc: %s %d, size = %d.", filename, line, sizeToMalloc));
#endif

    //actually do the allocation
    memoryPointer = malloc(sizeToMalloc);

    if (memoryPointer == NULL) {
        SMS_A_(SMS_LOG_E("smutil.c: %d bytes alloc failed ", sizeToMalloc));
    } else {
        HeapEntry_t *HeapEntry_p = (HeapEntry_t *) malloc(sizeof(HeapEntry_t));

        // Initialise the memory
        memset(memoryPointer, 0xAA, sizeToMalloc);

        HeapEntry_p->memoryPointer = memoryPointer;
        HeapEntry_p->size = sizeToMalloc;
        HeapEntry_p->filename = filename;
        HeapEntry_p->line = line;

        if (HeapAllocList_p == NULL) {
            // Create RAM copy of allocated Heap
            HeapAllocList_p = MSG_HeapListCreate();
        }

        MSG_HeapListInsertItem(HeapAllocList_p, HeapAllocList_p->count, HeapEntry_p);

        HeapAllocTotal += sizeToMalloc;

        if (HeapAllocTotal > HeapAllocPeak) {
            HeapAllocPeak = HeapAllocTotal;
            SMS_A_(SMS_LOG_I("HEAP: New Peak %d bytes alloced", HeapAllocPeak));
        }
    }

#ifdef SMS_HEAP_CHECK_ENHANCED_DEBUG
    SMS_A_(SMS_LOG_I("SmUtil_HeapUntypedAlloc: buffer=0x%08X.", (unsigned int) memoryPointer));
#endif
    return (memoryPointer);
}


/********************************************************************/
/**
 *
 * @function     SmUtil_HeapFree
 *
 * @description  Function for messaging system to free the memory
 *
 * @param        *memoryToFree : void type.
 *
 * @return       void
 */
/********************************************************************/
void SmUtil_HeapFree(
    void *memoryToFree,
    const char *filename,
    const size_t line)
{
#ifdef SMS_HEAP_CHECK_ENHANCED_DEBUG
    SMS_A_(SMS_LOG_I("SmUtil_HeapFree: %s %d. Buffer=0x%08X", filename, line, (unsigned int) memoryToFree));
#endif
    SMS_IDENTIFIER_NOT_USED(filename);
    SMS_IDENTIFIER_NOT_USED(line);

    if (memoryToFree != NULL) {
        HeapEntry_t *HeapEntry_p = (HeapEntry_t *) MSG_AllocListGetFirst(HeapAllocList_p);

        while (HeapEntry_p) {
            if (HeapEntry_p->memoryPointer == memoryToFree) {
                HeapAllocTotal -= HeapEntry_p->size;
                (void) MSG_AllocListRemove(HeapAllocList_p, HeapEntry_p);
                free(HeapEntry_p);
                break;
            } else {
                HeapEntry_p = (HeapEntry_t *) MSG_AllocListGetNext(HeapAllocList_p, HeapEntry_p);
            }
        }

        free(memoryToFree);
    } else {
        SMS_A_(SMS_LOG_E("smutil.c: Free NULL pointer?"));
    }
}
#endif                          //SMS_HEAP_CHECK


/********************************************************************/
/**
 *
 * @function     SmUtil_UnpackTo8Bits
 *
 * @description  Unpack packed data of 7 or 8 bits formats to 8 bits
 *
 * @param        *Out_p :          as type of uint8_t.
 * @param        *In_p :           uint8_t.
 * @param        NoOfPackedUnits : uint8_t.
 * @param        PackedFormat :    as type of DataCoding_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_UnpackTo8Bits(
    uint8_t * Out_p,
    uint8_t * In_p,
    uint8_t NoOfPackedUnits,
    DataCoding_t PackedFormat)
{
    uint8_t NoOfUnpackedOctets = 0;

    if (NoOfPackedUnits > 0) {
        switch (PackedFormat) {
        case DATA_CODING_7_BITS:

            {
                uint8_t shift = 0;

                *Out_p = 0;

                while (NoOfPackedUnits > 0) {
                    *Out_p = (*Out_p | (*In_p << shift)) & 0x7F;
                    *++Out_p = *In_p++ >> (7 - shift);

                    NoOfUnpackedOctets++;
                    NoOfPackedUnits--;

                    if (NoOfPackedUnits == 0) {
                        break;
                    }

                    shift++;
                    shift = (shift % 7);

                    if (shift == 0) {
                        Out_p++;
                        *Out_p = 0;
                        NoOfUnpackedOctets++;
                        NoOfPackedUnits--;
                    }
                }
            }

            break;

        case DATA_CODING_8_BITS:

            NoOfUnpackedOctets = NoOfPackedUnits;
            memcpy(Out_p, In_p, NoOfPackedUnits);
            break;

        default:

            SMS_B_(SMS_LOG_D("smutil.c: UTIL: Unsupported Data Coding"));
            NoOfUnpackedOctets = 0;
            break;

        }                       /* switch */

    }
    /* if (NoOfPackedUnits ...) */
    return NoOfUnpackedOctets;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_PackFrom8Bits
 *
 * @description  Pack unpacked data of 7 or 8 bits formats
 *
 * @param        *Out_p           : uint8_t.
 * @param        *In_p            : uint8_t.
 * @param        NoOfOctetsToPack : uint8_t.
 * @param        FormatToPackTo   : as type of DataCoding_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_PackFrom8Bits(
    uint8_t * Out_p,
    uint8_t * In_p,
    uint8_t NoOfOctetsToPack,
    DataCoding_t FormatToPackTo)
{
    uint8_t NoOfOctetsOfPackedData = 0;

    if (NoOfOctetsToPack > 0) {
        switch (FormatToPackTo) {
        case DATA_CODING_7_BITS:
            {
                uint8_t *Stop_p;
                uint8_t shift = 0;

                *Out_p = 0;
                Stop_p = In_p + NoOfOctetsToPack;

                do {
                    *Out_p = (*In_p++ >> shift);

                    if (In_p == Stop_p) {
                        break;
                    }

                    *Out_p = *Out_p | (*In_p << (7 - shift));
                    Out_p++;

                    shift++;
                    shift = (shift % 7);

                    if (shift == 0) {
                        In_p++;
                    }

                } while (In_p < Stop_p);

                NoOfOctetsOfPackedData = (NoOfOctetsToPack - NoOfOctetsToPack / 8);
            }

            break;

        case DATA_CODING_8_BITS:

            memcpy(Out_p, In_p, NoOfOctetsToPack);
            NoOfOctetsOfPackedData = NoOfOctetsToPack;
            break;

        case DATA_CODING_16_BITS:

            {
                uint16_t i;
                //Need to swap the bytes around
                for (i = 0; i < NoOfOctetsToPack; i += 2) {
                    *Out_p++ = *(In_p + 1);
                    *(Out_p++) = *(In_p++);
                    In_p++;
                }
            }
            NoOfOctetsOfPackedData = NoOfOctetsToPack;
            break;

        default:

            SMS_B_(SMS_LOG_D("smutil.c: UTIL: Unsupported Data Coding"));
            NoOfOctetsOfPackedData = 0;
            break;

        }                       /* switch */

    }
    /* if (NoOfOctetsToPack ...) */
    return NoOfOctetsOfPackedData;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_GetPackedSizeOfAddress
 *
 * @description  Function to calculate the packed size of an address
 *
 * @param        LengthInDigits : uint8_t.
 * @param        *Addr_p :        uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
static uint16_t SmUtil_GetPackedSizeOfAddress(
    uint8_t LengthInDigits,
    uint8_t * Addr_p)
{
    uint16_t Size;

    if (!LengthInDigits) {
        Size = *Addr_p + 1;     // SC address
    } else {
        Size = 2;               // Length byte + TypeOfAddress;
        Size += (*Addr_p + 1) / 2;      // Bytes needed for the digits
    }

    return Size;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_CalculateSizeOf_SM
 *
 * @description  This routine calculates the real length of a message that has been
 *               read from SMH (which always send the maximum size)
 *
 * @param        *Data_p : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint16_t SmUtil_CalculateSizeOf_SM(
    uint8_t * Data_p)
{

    uint8_t DefaultAlphabet = 0;
    uint8_t TimeSize = SMS_TIME_STAMP_LENGTH;
    uint16_t MessageSize = 0;
    uint8_t UserDataPresent = TRUE;
    uint8_t TPDUType;

    // TODO - Big health warning with this funciton - it doesn't work properly,
    // it never did.  It did work with Deliver and Submit, Its now been extended
    // for now to work with Status Reports too.  However with the fact that we
    // now have as TPDU store/read interace there is potential for this to be
    // called with any valid TPDU.  However the TP-MTI only has 2 bits, requiring
    // the  knowledge of whether this is an MO or MT message before a TPDU can be
    // identified and we dont have this information.  So for now the assumption
    // is as follows,
    //  _____________________________
    // | TP-MTI Value | Assumed TPDU |
    // |    0x00      | Deliver      |
    // |    0x01      | Submit       |
    // |    0x02      | Status Report|
    // |    0x03      | Deliver      |
    // |-----------------------------|
    //
    // This will need looking at again!!!

    TPDUType = (*Data_p & MTI_MASK);

    switch (TPDUType) {
    case SMS_SUBMIT_MTI:
        {
            switch (*Data_p & VPF_MASK) {
            case TP_VPF_MISSING:
                TimeSize = 0;
                break;

            case TP_VPF_INTEGER_REPRESENTATION:
                TimeSize = 1;
                break;

            default:
                break;          /* Already initiated */
            }
            MessageSize = 2;    // MTI + MR
        }
        break;

    case SMS_DELIVER_MTI:
    case SMS_RESERVED_MTI:
        MessageSize = 1;        // MTI
        break;

    case SMS_STATUS_REPORT_MTI:
        MessageSize = 2;        // MTI + MR
        break;

    default:
        SMS_A_(SMS_LOG_E("smutil.c: ERROR: Invalid TP-MTI for message"));
        break;
    }

    MessageSize += SmUtil_GetPackedSizeOfAddress(TRUE,  // semi-octets
                                                 Data_p + MessageSize); // OA or DA or RA

    if (SMS_STATUS_REPORT_MTI == TPDUType) {
        // Process status reports very differently from the others
        uint8_t TpPi = 0;
        MessageSize += (TimeSize * 2);  // TP-SCTS and TP-DT
        MessageSize++;          // TP-ST

        // Check if TP-PI is set
        TpPi = *(Data_p + MessageSize++);

        if (TpPi != 0) {
            // The TP-PI field is set
            if ((TpPi & TP_PID_MASK) != 0) {
                MessageSize++;  // TP-PID
            }

            if ((TpPi & TP_DCS_MASK) != 0) {
                DefaultAlphabet = ((*(Data_p + MessageSize) & DCS_MC_MASK) == 0xF0) || ((*(Data_p + MessageSize) & GENERAL_DCS_MASK) == 0x00);

                MessageSize++;  // TP-DCS
            }

            if ((TpPi & TP_UDL_MASK) == 0) {
                UserDataPresent = FALSE;
            }
        } else {
            UserDataPresent = FALSE;
        }

    } else {
        MessageSize++;          // PID

        // Assign TRUE to DefaultAlphabet if 7 bit data in user data:

        DefaultAlphabet = ((*(Data_p + MessageSize) & DCS_MC_MASK) == 0xF0) || ((*(Data_p + MessageSize) & GENERAL_DCS_MASK) == 0x00);

        MessageSize++;          // DCS

        MessageSize += TimeSize;        // SCTS or VP
    }

    if (UserDataPresent) {
        uint16_t Len;

        if (DefaultAlphabet) {
            // 7 Bit Data
            uint16_t NoOfBits = *(Data_p + MessageSize) * 7;

            if (NoOfBits % 8) {
                Len = NoOfBits / 8 + 1;
            } else {
                Len = NoOfBits / 8;
            }
        } else {
            // 8 or 16 Bit Data
            Len = *(Data_p + MessageSize);
        }

        MessageSize += Len;

        MessageSize++;          // byte for UDL, User Data Length
    }

    return MessageSize;

}                               /* CalculateSizeOfSM */


/********************************************************************/
/**
 *
 * @function     SmUtil_ConvertMtiToSmMessageType
 *
 * @description  return standard reference (23.040) values from const data
 *
 * @param        TpMti : uint8_t
 * @param        Direction : type SmMessageDirection_t
 *
 * @return       ShortMessageType_t
 */
/********************************************************************/
ShortMessageType_t SmUtil_ConvertMtiToSmMessageType(
    uint8_t TpMti,
    SmMessageDirection_t Direction)
{
    return (ShortMessageTypes[Direction][TpMti]);
}


/********************************************************************/
/**
 *
 * @function     SmUtil_Get_GD_MSG_IMSI
 *
 * @description  Function to get IMSI from Persistent Storage
 *
 * @param        *MSG_IMSI_p : as type of MSG_IMSI_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_Get_GD_MSG_ICCID(
    uint8_t * const MSG_ICCID_p)
{
    SMS_Error_t ErrorCode;
    uint8_t Result = TRUE;

    ErrorCode = SmUtil_PersistentStorageData_Read(SMS_PERSISTENT_STORAGE_DATA_TYPE_ICCID, SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE, MSG_ICCID_p);

    if (ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_E("smgutil.c: SmUtil_Get_GD_MSG_ICCID: Failure to read persistent storage 0x%04X", ErrorCode));
        Result = FALSE;
    }

    return Result;
}                               /* END FUNCTION: () */


/********************************************************************/
/**
 *
 * @function     SmUtil_Put_GD_MSG_IMSI
 *
 * @description  Function to put IMSI to GD
 *
 * @param        *MSG_IMSI_p : as type of MSG_IMSI_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_Put_GD_MSG_ICCID(
    uint8_t * const MSG_ICCID_p)
{
    SMS_Error_t ErrorCode;
    uint8_t Result = TRUE;

    ErrorCode = SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_ICCID, SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE, MSG_ICCID_p);

    if (ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_E("smutil.c: SmUtil_Put_GD_MSG_ICCID: Failure to write persitent storage 0x%04X", ErrorCode));
        Result = FALSE;
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_TP_MessageReferenceReadAndIncrement
 *
 * @description  Function to read and then increase the short message reference number (TP-MR)
 *
 * @param        void
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_TP_MessageReferenceReadAndIncrement(
    void)
{
    uint8_t RefNo = 0;

#ifdef SMS_USE_MFL_UICC
    SIM_ISO_ErrorCause_t ErrorCause;
    SMS_Error_t ErrorCode = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    SMS_TP_MessageReference_t MSG_MessageReference = 0;
    RequestStatus_t RequestStatus = REQUEST_FAILED;
    UICC_ServiceHandle_t ServiceHandle = SmUtil_UICC_ServiceHandleGet();

    RequestStatus = R_Req_SIM_SMR_MessageRef_Read(WAIT_RESPONSE, ServiceHandle, &RefNo, &ErrorCause);

    //It failed to read it from SIM then get the RefNo from Persistent Storage
    if (RequestStatus != REQUEST_OK) {
        SMS_A_(SMS_LOG_E("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: failed SIM, Persistent Data instead"));

        ErrorCode = SmUtil_PersistentStorageData_Read(SMS_PERSISTENT_STORAGE_DATA_TYPE_MESSAGE_REFERENCE, sizeof(SMS_TP_MessageReference_t), &MSG_MessageReference);

        if (ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            //save this value for later on
            RefNo = MSG_MessageReference;
        } else {
            SMS_A_(SMS_LOG_E("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: Failure to read Persistent Data 0x%04X", ErrorCode));
        }
    }
    //Increment the Reference Number only if SIM read or Persistent Storage read is sucessful
    if ((RequestStatus == REQUEST_OK) || (ErrorCode == SMS_ERROR_INTERN_NO_ERROR)) {
        //increment the reference number
        RefNo++;

        if (SMS_MO_RETRY_IF_MESSAGE_REFERENCE_SET == 1 && RefNo == 0) {
            // Inhibit allocating a TP-MR of 0 due to overflow as this indicates
            // that a new TP-MR must be allocated. ie if TP-MR of 0 was assigned
            // to an SM that failed to be sent then an attempt to retry sending
            // would allocate a new TP-MR instead of setting TP-RD.
            RefNo++;
        }

        SMS_B_(SMS_LOG_D("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: new reference number %d", RefNo));
        //write the counter value to the SIM, don't really care if this fails
        (void) R_Req_SIM_SMR_MessageRef_Write(WAIT_RESPONSE, ServiceHandle, RefNo, &ErrorCause);

        MSG_MessageReference = RefNo;

        ErrorCode = SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_MESSAGE_REFERENCE, sizeof(SMS_TP_MessageReference_t), &MSG_MessageReference);

        if (ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_E("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: Failure to write Persistent Storage 0x%04X", ErrorCode));
        }
    }
#else
    // Support TP-MRs without using SIM.
    SMS_Error_t ErrorCode;
    SMS_TP_MessageReference_t MSG_MessageReference = 0;

    SMS_B_(SMS_LOG_D("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: Read Persistent Data"));

    ErrorCode = SmUtil_PersistentStorageData_Read(SMS_PERSISTENT_STORAGE_DATA_TYPE_MESSAGE_REFERENCE, sizeof(SMS_TP_MessageReference_t), &MSG_MessageReference);

    if (ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        //increment the reference number
        MSG_MessageReference++;

        if (SMS_MO_RETRY_IF_MESSAGE_REFERENCE_SET == 1 && MSG_MessageReference == 0) {
            // Inhibit allocating a TP-MR of 0 due to overflow as this indicates
            // that a new TP-MR must be allocated. ie if TP-MR of 0 was assigned
            // to an SM that failed to be sent then an attempt to retry sending
            // would allocate a new TP-MR instead of setting TP-RD.
            MSG_MessageReference++;
        }
        //setup return value
        RefNo = MSG_MessageReference;

        SMS_B_(SMS_LOG_D("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: new reference number %d", RefNo));

        ErrorCode = SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_MESSAGE_REFERENCE, sizeof(SMS_TP_MessageReference_t), &MSG_MessageReference);

        if (ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
            SMS_A_(SMS_LOG_E("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: Failure to write Persistent Storage 0x%04X", ErrorCode));
        }
    } else {
        SMS_A_(SMS_LOG_E("smutil.c: SmUtil_TP_MessageReferenceReadAndIncrement: Failure to read Persistent Data 0x%04X", ErrorCode));
    }

#endif

    return (RefNo);
}


/********************************************************************/
/**
 *
 * @function     SmUtil_Get_MSG_MemoryCapacityExceededFlag
 *
 * @description  Get the Memory Capacity Exceeded Flag
 *
 * @param        MemoryCapacityExceededNotificationPtr : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_Get_MSG_MemoryCapacityExceededFlag(
    uint8_t * const MemoryCapacityExceededNotificationPtr)
{
    SMS_Error_t ErrorCode;
    uint8_t Result = FALSE;
    MSG_MemoryCapacityExceededFlag_t *MSG_MemoryCapacityExceededFlag_p = (MSG_MemoryCapacityExceededFlag_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_MemoryCapacityExceededFlag_t));

    if (MSG_MemoryCapacityExceededFlag_p != NULL) {
        ErrorCode = SmUtil_PersistentStorageData_Read(SMS_PERSISTENT_STORAGE_DATA_TYPE_MEMORY_CAPACITY_EXCEEDED_FLAG, sizeof(MSG_MemoryCapacityExceededFlag_t), MSG_MemoryCapacityExceededFlag_p);

        if (ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
            if (MSG_MemoryCapacityExceededFlag_p->SMS_MemoryCapacityExceededFlag) {
                *MemoryCapacityExceededNotificationPtr = TRUE;
            } else {
                *MemoryCapacityExceededNotificationPtr = FALSE;
            }

            Result = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("smutil.c: Get_MSG_MemoryCapacityExceededFlag: Failure to read Persistent Storage 0x%04X", ErrorCode));
        }

        SMS_HEAP_FREE(&MSG_MemoryCapacityExceededFlag_p);
    }

    SMS_B_(SMS_LOG_D("smutil.c: Get_MSG_MemoryCapacityExceededFlag: MemoryCapacityExceededNotification %d", *MemoryCapacityExceededNotificationPtr));

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_Put_MSG_MemoryCapacityExceededFlag
 *
 * @description  Put the Memory Capacity Exceeded Flag
 *
 * @param        MemoryCapacityExceededNotification : uint8_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_Put_MSG_MemoryCapacityExceededFlag(
    const uint8_t MemoryCapacityExceededNotification)
{
    SMS_Error_t ErrorCode;
    uint8_t Result = TRUE;
    MSG_MemoryCapacityExceededFlag_t *MSG_MemoryCapacityExceededFlag_p = (MSG_MemoryCapacityExceededFlag_t *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_MemoryCapacityExceededFlag_t));

    MSG_MemoryCapacityExceededFlag_p->SMS_MemoryCapacityExceededFlag = MemoryCapacityExceededNotification;

    ErrorCode = SmUtil_PersistentStorageData_Write(SMS_PERSISTENT_STORAGE_DATA_TYPE_MEMORY_CAPACITY_EXCEEDED_FLAG, sizeof(MSG_MemoryCapacityExceededFlag_t), MSG_MemoryCapacityExceededFlag_p);

    SMS_HEAP_FREE(&MSG_MemoryCapacityExceededFlag_p);

    if (ErrorCode != SMS_ERROR_INTERN_NO_ERROR) {
        SMS_A_(SMS_LOG_E("smutil.c: Put_MSG_MemoryCapacityExceededFlag: Failure to write Persistent Storage 0x%04X", ErrorCode));
        Result = FALSE;
    }

    SMS_B_(SMS_LOG_D("smutil.c: Put_MSG_MemoryCapacityExceededFlag: MemoryCapacityExceededNotification %d", MemoryCapacityExceededNotification));

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_Derive_HPLMN
 *
 * @description  Extract Home PLMN from IMSI
 *
 * @param        *IMSI_p : pointer to IMSI_t.
 * @param        *HPLMN : pointer to MSG_PLMN_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
#ifdef SMS_USE_MFL_UICC         // Function has been ported, but its only caller is currently not ported. Therefore removed to prevent compiler warnings.
static uint8_t SmUtil_Derive_HPLMN(
    const SMS_IMSI_t * IMSI_p,
    MSG_PLMN_t * HPLMN)
{
    uint8_t Result = TRUE;

    typedef struct {
        unsigned char Length;
        ubitfield TypeOfIdentity:3;
        ubitfield OddEvenIndication:1;
        ubitfield IdentityDigit_01:4;
        ubitfield IdentityDigit_02:4;
        ubitfield IdentityDigit_03:4;
        ubitfield IdentityDigit_04:4;
        ubitfield IdentityDigit_05:4;
        ubitfield IdentityDigit_06:4;
    } IMSI_PLMN_s;              /* Used only for deriving HPLMN from IMSI */

    IMSI_PLMN_s *IMSI_temp_p;

    IMSI_temp_p = (IMSI_PLMN_s *) IMSI_p;

    if ((IMSI_temp_p->Length > (SMS_IMSI_LEN - 1)) || (IMSI_temp_p->Length < 3)) {
        Result = FALSE;
    }

    HPLMN->MCC1 = IMSI_temp_p->IdentityDigit_01;
    HPLMN->MCC2 = IMSI_temp_p->IdentityDigit_02;
    HPLMN->MCC3 = IMSI_temp_p->IdentityDigit_03;
    HPLMN->MNC1 = IMSI_temp_p->IdentityDigit_04;
    HPLMN->MNC2 = IMSI_temp_p->IdentityDigit_05;
    HPLMN->MNC3 = IMSI_temp_p->IdentityDigit_06;

    return Result;
}
#endif                          // SMS_USE_MFL_UICC


/********************************************************************/
/**
 *
 * @function     SmUtil_Get_MSG_Home_PLMN
 *
 * @description  Get Home PLMN from SIM
 *
 * @param        *HPLMN : pointer to MSG_PLMN_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_Get_MSG_Home_PLMN(
    MSG_PLMN_t * HPLMN)
{
#ifdef SMS_USE_MFL_UICC
    uint8_t Result = TRUE;
    SMS_UICC_ServiceHandle_t ServiceHandle = SmUtil_UICC_ServiceHandleGet();
    SIM_ISO_ErrorCause_t SimErrorDetails;
    uint8_t SIM_IMSI[SIM_IMSI_LEN];

    // Read the current IMSI
    if (Request_SIM_IMSI_Read(WAIT_RESPONSE, ServiceHandle, SIM_IMSI, &SimErrorDetails) == REQUEST_OK) {
        (void) SmUtil_Derive_HPLMN((const IMSI_t *) SIM_IMSI, HPLMN);   //extract PLMN from IMSI
    } else {
        Result = FALSE;
        SMS_A_(SMS_LOG_E("ERROR: smutil.c: IMSI read failed"));
    }

    return Result;
#else
    SMS_A_(SMS_LOG_E("smutil.c: SmUtil_Get_MSG_Home_PLMN: *********** NOT PORTED TO LINUX ***********"));
    SMS_IDENTIFIER_NOT_USED(HPLMN);
    return FALSE;
#endif                          // SMS_USE_MFL_UICC
}


/********************************************************************/
/**
 *
 * @function     SmUtil_PLMN_MCC
 *
 * @description  Calculates the MCC of a PLMN
 *
 * @param        *PLMN_p : pointer to MSG_PLMN_t.
 *
 * @return       static int
 */
/********************************************************************/
static int SmUtil_PLMN_MCC(
    const MSG_PLMN_t * PLMN_p)
{
    int Result = 0;

    if (PLMN_p->MCC1 == 0xf || PLMN_p->MCC2 == 0xf || PLMN_p->MCC3 == 0xf) {
        //return Result = 0;
    } else {
        Result = PLMN_p->MCC1 * 100 + PLMN_p->MCC2 * 10 + PLMN_p->MCC3;
    }

    return Result;
}


/********************************************************************/
/**
 *
 * @function     SmUtil_PLMN_NA
 *
 * @description  Calculates the NA of a PLMN
 *
 * @param        *PLMN_p : pointer to MSG_PLMN_t.
 *
 * @return       static uint8_t
 */
/********************************************************************/
static uint8_t SmUtil_PLMN_NA(
    const MSG_PLMN_t * PLMN_p)
{
    unsigned char i;
    static const int MCC_USA[] = { 310, 311, 312, 313, 314, 315, 316 };
    uint8_t Result = FALSE;

    for (i = 0; i < (sizeof(MCC_USA) / sizeof(int)); i++) {
        if (SmUtil_PLMN_MCC(PLMN_p) == MCC_USA[i]) {
            Result = TRUE;
        }
    }
    return (Result);
}


/********************************************************************/
/**
 *
 * @function     SmUtil_Same_PLMN
 *
 * @description  <Detailed description of the method>
 *
 * @param        *PLMN1_p : pointer to MSG_PLMN_t.
 * @param        *PLMN2_p : pointer to MSG_PLMN_t.
 *
 * @return       uint8_t
 */
/********************************************************************/
uint8_t SmUtil_Same_PLMN(
    const MSG_PLMN_t * PLMN1_p,
    const MSG_PLMN_t * PLMN2_p)
{
    uint8_t Result = FALSE;

    if (memcmp(PLMN1_p, PLMN2_p, sizeof(MSG_PLMN_t)) == 0) {
        Result = TRUE;
    } else {
        if ((PLMN1_p->MNC3 == 0xF) || (PLMN2_p->MNC3 == 0xF))   //only one of PLMNs contains F
        {
            if ((SmUtil_PLMN_MCC(PLMN2_p) == SmUtil_PLMN_MCC(PLMN1_p)) && (PLMN2_p->MNC1 == PLMN1_p->MNC1) && (PLMN2_p->MNC2 == PLMN1_p->MNC2)
                ) {
                if ((!SmUtil_PLMN_NA(PLMN1_p)) && (SmUtil_PLMN_MCC(PLMN1_p) != MCC_CANADA)) {
                    Result = TRUE;
                } else {        // 0 suffix
                    if ((PLMN2_p->MNC3 == 0) || (PLMN1_p->MNC3 == 0)) {
                        Result = TRUE;
                    }
                }
            }
        }
    }
    return (Result);
}


/********************************************************************/
/**
 *
 * @function     SmUtil_ICCID_Changed
 *
 * @description  Determine whether IMSI has changed
 *
 * @param        void
 *
 * @return       extern uint8_t
 */
/********************************************************************/
uint8_t SmUtil_ICCID_Changed(
    void)
{
    return (Messaging_USIM.ICCID_Changed);
}


/********************************************************************/
/**
 *
 * @function     SmUtil_ICCID_Get
 *
 * @description  Guess what - get the current ICCID
 *
 * @param        ICCID_p
 *
 * @return       void
 */
/********************************************************************/
void SmUtil_ICCID_Get(
    char *const ICCID_p)
{
    (void) strcpy(ICCID_p, Messaging_USIM.Current_ICCID);
}


#ifdef SMS_USE_MFL_UICC
/********************************************************************/
/**
 *
 * @function     SmUtil_UICC_ServiceHandle_Get
 *
 * @description  Guess what - get the current ICCID Service Handle
 *
 * @param        void
 *
 * @return       extern SMS_UICC_ServiceHandle_t
 */
/********************************************************************/
SMS_UICC_ServiceHandle_t SmUtil_UICC_ServiceHandleGet(
    void)
{
    return (Messaging_USIM.UICC_ServiceHandle);
}
#endif


/********************************************************************/
/**
 *
 * @function     SmUtil_USIM_Initialise
 *
 * @description  The ICCID is read from the USIM, and compared with the Previous ICCID
 *               used within the Messaging System
 *
 * @param        void
 *
 * @return       uint8_t
 *
 * NOTES:        The Messaging System Only Requires the ICCID to identify if the ICCID
 *               has changed. To avoid
 *               i.  Continual Reading of ICCID Values
 *               ii. Storage Capacity
 *               Only the fact that the ICCID has changed is maintained within the MsgSystem
 */
/********************************************************************/
uint8_t SmUtil_USIM_Initialise(
    uint8_t * eventData_p)
{
#ifdef SMS_USE_MFL_UICC
    uint8_t USIM_Initialised = FALSE;
    SIM_ISO_ErrorCause_t SimErrorDetails;
    UICC_ServiceHandle_t ServiceHandle;
    uint8_t Previous_ICCID[SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE];
    uint8_t Current_ICCID[SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE];
    SIM_PinRefDataId_t NotUsed;
    SIM_State_t SIMState;

    // Unpack the event to get the SIM state and ServiceHandle.
    if (GS_EVENT_OK == Event_SIM_State_Changed(eventData_p, &ServiceHandle, &NotUsed, &SIMState)) {
        // Only do something when the SIM is active
        if (SIMState == SIM_STATE_ACTIVE) {
            SMS_A_(SMS_LOG_I("smutil.c: SIM_STATE Active"));
            // Read the current ICCID
            if (Request_SIM_ICC_ID_Read(WAIT_RESPONSE, ServiceHandle, Current_ICCID, &SimErrorDetails) == REQUEST_OK) {
                if (SMS_STORAGE_IS_ICCID_RELATED == 1) {
                    uint8_t Index;

                    // Store the UCCID as UCS2 in the Messagesystem object
                    // *2 as 2 characters per byte
                    for (Index = 0; Index < SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE; Index++) {
/*#ifdef MSG_NO_SIZE_SWPRINTF_PROTOTYPE
            (void)swprintf (&Messaging_USIM.Current_ICCID[Index*2], L"%02x", Current_ICCID[Index]);
#else
            (void)swprintf (&Messaging_USIM.Current_ICCID[Index*2], sizeof(wchar_t)*2, L"%02x", Current_ICCID[Index]);
#endif*/
                        (void) snprintf(&Messaging_USIM.Current_ICCID[Index * 2], sizeof(char) * 2, "%02x", Current_ICCID[Index]);
                    }

                    // Null terminate
                    Messaging_USIM.Current_ICCID[(SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE * 2)] = 0x00;
                }
                // Store the Service Handle in the MessageSystem object
                Messaging_USIM.UICC_ServiceHandle = ServiceHandle;

                // Read the previous ICCID
                if (!SmUtil_Get_GD_MSG_ICCID(&Previous_ICCID[0])) {
                    SMS_A_(SMS_LOG_E("smutil.c Error getting MSG ICCID from Persistent Storage"));
                }
                // Are they the same?
                if (memcmp(Previous_ICCID, Current_ICCID, SMS_SIM_SMS_IDENTIFICATION_NUMBER_SIZE)) {
                    /*
                     * ICCID Changed.
                     * Update ICCID within Messaging System
                     */
                    Messaging_USIM.ICCID_Changed = TRUE;

                    // Store the new one in global data
                    if (!SmUtil_Put_GD_MSG_ICCID(&Current_ICCID[0])) {
                        SMS_A_(SMS_LOG_E("smutil.c: Put_GD_MSG_ICCID, failed to write to Persistent Storage"));
                    } else {
                        SMS_A_(SMS_LOG_I("smutil.c: ICCID CHANGE"));
                    }
                } else {
                    Messaging_USIM.ICCID_Changed = FALSE;
                    SMS_A_(SMS_LOG_I("smutil.c: ICCID NOT CHANGED"));
                }

                USIM_Initialised = TRUE;
            } else {
                SMS_A_(SMS_LOG_E("smutil.c: UCCID read failed"));
            }
        } else {
            SMS_A_(SMS_LOG_I("smutil.c: SIM_STATE NOT Active"));
        }
    } else {
        SMS_A_(SMS_LOG_E("smutil.c: Call to \"Event_SIM_State_Changed (...)\" failed"));
    }

    return USIM_Initialised;
#else
    SMS_A_(SMS_LOG_E("smutil.c: SmUtil_USIM_Initialise: *********** NOT PORTED TO LINUX ***********"));
    SMS_IDENTIFIER_NOT_USED(eventData_p);
    return FALSE;
#endif                          // SMS_USE_MFL_UICC
}

#ifdef SMS_USE_MFL_MODEM
/********************************************************************/
/**
 *
 * @function     SmUtil_TranslateErrorCodes
 *
 * @description  Map RMS_V2_ErrorCode_t error causes to SMS_Error_t
 *               The error code values 0x0101 to 0x0396 are the same for SMS_Error_t and
 *               RMS_V2_ErrorCode_t.
 *
 * @param        RMS_V2_ErrorCode : RMS RP Error Cause
 *
 * @return       SMS_Error_t
 */
/********************************************************************/
SMS_Error_t SmUtil_TranslateErrorCodes(
    const RMS_V2_ErrorCode_t RMS_V2_ErrorCode)
{
    SMS_Error_t SMS_Error = SMS_ERROR_RMS_INTERN_CONNECTION_ESTABLISHMENT_FAILED;

    if (RMS_V2_ErrorCode == RMS_RP_NO_CAUSE || RMS_V2_ErrorCode == RMS_V2_ERROR_CODE_INTERN_NO_ERROR) {
        SMS_Error = SMS_ERROR_INTERN_NO_ERROR;
    } else {
        SMS_Error = (SMS_Error_t) RMS_V2_ErrorCode;
    }

    return SMS_Error;
}
#endif                          // SMS_USE_MFL_MODEM




#ifndef SMS_USE_MFL_MODEM
#define SMS_ERROR_RP_NO_ERROR 0x0000
/********************************************************************/
/**
 *
 * @function     SmUtil_TranslateErrorCodesToMal
 *
 * @description  Covert SMS RP Error Cause value to MAL RP Error Cause
 *               value.
 *
 * @param        SmsRP_Error : SMS_RP_ErrorCause_t
 *
 * @return       MAL_SMS_RP_ErrorCause_t
 */
/********************************************************************/
MAL_SMS_RP_ErrorCause_t SmUtil_TranslateErrorCodesToMal(
    const SMS_RP_ErrorCause_t SmsRP_Error)
{
    MAL_SMS_RP_ErrorCause_t MalRP_Error;

    switch (SmsRP_Error) {
        // Moved common cases to top of switch for faster run-time processing.
    case SMS_ERROR_INTERN_NO_ERROR:
    case SMS_ERR_OK:
    case SMS_ERROR_RP_NO_ERROR:
        MalRP_Error = MAL_SMS_OK;
        break;
    case SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED:
        MalRP_Error = MAL_SMS_EXT_ERR_MEMORY_CAPACITY_EXC;
        break;
    case SMS_ERROR_RP_UNALLOCATED_NUMBER:
        MalRP_Error = MAL_SMS_EXT_ERR_UNASSIGNED_NUMBER;
        break;
    case SMS_ERROR_RP_OPERATOR_DETERMINED_BARRING:
        MalRP_Error = MAL_SMS_EXT_ERR_OPER_DETERMINED_BARR;
        break;
    case SMS_ERROR_RP_CALL_BARRED:
        MalRP_Error = MAL_SMS_EXT_ERR_CALL_BARRED;
        break;
    case SMS_ERROR_RP_RESERVED:
        MalRP_Error = MAL_SMS_EXT_ERR_RESERVED;
        break;
    case SMS_ERROR_RP_SMS_TRANSFER_REJECTED:
        MalRP_Error = MAL_SMS_EXT_ERR_MSG_TRANSFER_REJ;
        break;
    case SMS_ERROR_RP_DESTINATION_OUT_OF_ORDER:
        MalRP_Error = MAL_SMS_EXT_ERR_DEST_OUT_OF_ORDER;
        break;
    case SMS_ERROR_RP_UNIDENTIFIED_SUBSCRIBER:
        MalRP_Error = MAL_SMS_EXT_ERR_UNDEFINED_SUBSCRIBER;
        break;
    case SMS_ERROR_RP_FACILITY_REJECTED:
        MalRP_Error = MAL_SMS_EXT_ERR_FACILITY_REJECTED;
        break;
    case SMS_ERROR_RP_UNKNOWN_SUBSCRIBER:
        MalRP_Error = MAL_SMS_EXT_ERR_UNKNOWN_SUBSCRIBER;
        break;
    case SMS_ERROR_RP_NETWORK_OUT_OF_ORDER:
        MalRP_Error = MAL_SMS_EXT_ERR_NETW_OUT_OF_ORDER;
        break;
    case SMS_ERROR_RP_TEMPORARY_FAILURE:
        MalRP_Error = MAL_SMS_EXT_ERR_TEMPORARY_FAILURE;
        break;
    case SMS_ERROR_RP_CONGESTION:
        MalRP_Error = MAL_SMS_EXT_ERR_CONGESTION;
        break;
    case SMS_ERROR_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED:
        MalRP_Error = MAL_SMS_EXT_ERR_RESOURCE_UNAVAILABLE;
        break;
    case SMS_ERROR_RP_REQUESTED_FACILITY_NOT_SUBSCRIBED:
        MalRP_Error = MAL_SMS_EXT_ERR_REQ_FACILITY_NOT_SUB;
        break;
    case SMS_ERROR_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED:
        MalRP_Error = MAL_SMS_EXT_ERR_REQ_FACILITY_NOT_IMP;
        break;
    case SMS_ERROR_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE:
        MalRP_Error = MAL_SMS_EXT_ERR_INVALID_REFERENCE;
        break;
    case SMS_ERROR_RP_SEMANTICALLY_INCORRECT_MESSAGE:
        MalRP_Error = MAL_SMS_EXT_ERR_INCORRECT_MESSAGE;
        break;
    case SMS_ERROR_RP_INVALID_MANDATORY_INFORMATION:
        MalRP_Error = MAL_SMS_EXT_ERR_INVALID_MAND_INFO;
        break;
    case SMS_ERROR_RP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
        MalRP_Error = MAL_SMS_EXT_ERR_INVALID_MSG_TYPE;
        break;
    case SMS_ERROR_RP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE:
        MalRP_Error = MAL_SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST;
        break;
    case SMS_ERROR_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED:
        MalRP_Error = MAL_SMS_EXT_ERR_INVALID_INFO_ELEMENT;
        break;
    case SMS_ERROR_RP_INTERWORKING_UNSPECIFIED_PROBLEM:
        MalRP_Error = MAL_SMS_EXT_ERR_INTERWORKING;
        break;
    case SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED:
        MalRP_Error = MAL_SMS_EXT_ERR_PROTOCOL_ERROR;
        break;
    case SMS_ERROR_CP_CONGESTION:
        MalRP_Error = MAL_SMS_EXT_ERR_LOW_LAYER_CONGESTION;
        break;
    case SMS_ERROR_CP_INVALID_MANDATORY_INFORMATION:
        MalRP_Error = MAL_SMS_EXT_ERR_LOW_LAYER_INVALID_MAND_INFO;
        break;
    case SMS_ERROR_CP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
        MalRP_Error = MAL_SMS_EXT_ERR_LOW_LAYER_INVALID_MSG_TYPE;
        break;
    case SMS_ERROR_CP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE:
        MalRP_Error = MAL_SMS_EXT_ERR_LOW_LAYER_MSG_NOT_COMP_WITH_ST;
        break;
    case SMS_ERROR_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED:
        MalRP_Error = MAL_SMS_EXT_ERR_LOW_LAYER_INVALID_INFO_ELEMENT;
        break;
    default:
        {
            // The different RP Error cause types are 16 bit values.
            // There is common mapping between the bottom byte, however the top byte value varies
            // depending in which enum it is defined.  This conversion function handles the
            // "standard" values explicitly, however if a "non-standard" value is passed in we map
            // map the top byte to match that used by the MAL_SMS_RP_ErrorCause_t data structure.

            // Get the top byte used by the MAL_SMS_RP_ErrorCause_t data structure.
            const uint16_t Mal_Error_Mask = MAL_SMS_EXT_ERR_MEMORY_CAPACITY_EXC & 0xFF00;

            // Convert the top byte of the SMS_RP_ErrorCause_t to be that used by MAL_SMS_RP_ErrorCause_t.
            MalRP_Error = Mal_Error_Mask | (0x00FF & SmsRP_Error);
        }
        break;
    }

    SMS_C_(SMS_LOG_V("smutil.c: SmUtil_TranslateErrorCodesToMal: SMS RP = 0x%04X, MAL RP = 0x%04X", SmsRP_Error, MalRP_Error));

    return MalRP_Error;
}
#endif


/*
 * Handle Persistent Data Storage in FS
 */

#ifdef SMS_PRINT_B_
static const char *SmUtil_PersistentStorageData_DataTypeToString(
    const SMS_PersistentStorageDataType_t DataType)
{
    switch (DataType) {
    case SMS_PERSISTENT_STORAGE_DATA_TYPE_MEMORY_CAPACITY_EXCEEDED_FLAG:
        return "Memory Capacity Exceeded";
    case SMS_PERSISTENT_STORAGE_DATA_TYPE_MESSAGE_REFERENCE:
        return "Message Reference";
    case SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT:
        return "Stored SM Count";
    case SMS_PERSISTENT_STORAGE_DATA_TYPE_ICCID:
        return "ICCID";
    default:
        return "Unknown DataType";
    }
}

static void SmUtil_PersistentStorage_Data_DumpToLog(
    const SMS_PersistentStorageDataType_t DataType,
    const size_t Size,
    const uint8_t * Data_p,
    const uint8_t WriteData)
{
    SMS_LOG_D("smutil.c: SmUtil_PersistentStorage_Data_DumpToLog()");
    SMS_LOG_D("DataType = %d (%s)", DataType, SmUtil_PersistentStorageData_DataTypeToString(DataType));
    SMS_LOG_D("Size = %d", Size);
    SMS_LOG_D("%s Data =", WriteData ? "Written" : "Read");

    if (Data_p != NULL) {
        size_t Index;
        size_t pos = 0;
        char *PrintBuffer = malloc(16 * 3 + 3);

        if (NULL != PrintBuffer) {
            for (Index = 1; Index <= Size; Index++, Data_p++)       // Using 1-based so that Index % 16 is not TRUE for Index==0.
            {
                pos += sprintf(PrintBuffer+pos, "%02X ", *Data_p);

                if (Index % 16 == 0) {
                    SMS_LOG_D("\t%s", PrintBuffer);
                    pos = 0;
                }
            }

            if (pos != 0) {
                SMS_LOG_D("\t%s", PrintBuffer);
            }

            free(PrintBuffer);
        }
    }
}
#endif                          // #ifdef SMS_PRINT_B_

/********************************************************************/
/**
 *
 * @function     SmUtil_PersistentStorageData_CreateAndCheckDirectory
 *
 * @description  Creates a directory name and then, if necessary,
 *               checks to see if it is present. If it does not exist,
 *               try and create it.
 *
 * @param        Create : TRUE directory should be created.
 * @param        DirectoryName_p : Name of directory which should be
 *                                 checked.
 * @param        Buffer_p : Buffer in which directory name should be
 *                          created.
 *
 * @return       uint8_t : TRUE if directory exists after function has
 *                         executed.
 */
/********************************************************************/
static uint8_t SmUtil_PersistentStorageData_CreateAndCheckDirectory(
    const uint8_t Create,
    const char *const DirectoryName_p,
    char *const Buffer_p)
{
    uint8_t Success = TRUE;

    (void) strcat(Buffer_p, DirectoryName_p);

    if (Create) {
        // See if directory exists.
        // If it doesn't, try and create it.
        if (access(Buffer_p, F_OK) != 0) {
            if (mkdir(Buffer_p, S_IRWXU) != 0) {
                Success = FALSE;
            }
        }
    }

    return Success;
}

/********************************************************************/
/**
 *
 * @function     SmUtil_PersistentStorageData_CreateFileName
 *
 * @description  Creates a persistent storage filename. If flag is set,
 *               all necessary sub-directories for the filename will
 *               also be created.
 *
 * @param        DataType : Type of persistent data to be stored.
 * @param        Create : TRUE if necessary directories should be created.
 * @param        FileNameBuffer_p : Buffer containing file name created
 *                                  by this function.
 *
 * @return       uint8_t : TRUE if filename created successfully.
 */
/********************************************************************/
static uint8_t SmUtil_PersistentStorageData_CreateFileName(
    const SMS_PersistentStorageDataType_t DataType,
    const uint8_t Create,
    char *const FileNameBuffer_p)
{
    uint8_t Success = TRUE;

    // Create top level directory.
    (void) strcpy(FileNameBuffer_p, SMS_FS_DIR_TOP_MOUNT_POINT);

    // This is the top-level folder and should already exist and be able to cd into.
    // We do not create this folder if it doesn't exist.  However if Create is not set,
    // there is no need to validate whether it exists, or not.
    if (Create) {
        if (chdir(FileNameBuffer_p) != 0) {
            SMS_A_(SMS_LOG_E("smutil.c: SmUtil_PersistentStorageData_CreateFileName() Failed to chdir into %s", FileNameBuffer_p));
            Success = FALSE;
        }
    }

    if (Success) {
        // Add ./<toplevel>/messaging folder
        Success = SmUtil_PersistentStorageData_CreateAndCheckDirectory(Create, SMS_FS_DIR_MESSAGING, FileNameBuffer_p);
    }
    if (Success) {
        // add ./<toplevel>/messaging/pstore folder
        Success = SmUtil_PersistentStorageData_CreateAndCheckDirectory(Create, SMS_FS_DIR_PSTORE, FileNameBuffer_p);
    }
    if (Success) {
        // Add filename for datatype
        switch (DataType) {
        case SMS_PERSISTENT_STORAGE_DATA_TYPE_MEMORY_CAPACITY_EXCEEDED_FLAG:
            (void) strcat(FileNameBuffer_p, SMS_FS_FILENAME_MEMORY_CAPACITY_EXCEEDED_FLAG);
            break;

        case SMS_PERSISTENT_STORAGE_DATA_TYPE_MESSAGE_REFERENCE:
            (void) strcat(FileNameBuffer_p, SMS_FS_FILENAME_MESSAGE_REFERENCE);
            break;

        case SMS_PERSISTENT_STORAGE_DATA_TYPE_STORED_SM_COUNT:
            (void) strcat(FileNameBuffer_p, SMS_FS_FILENAME_STORED_SM_COUNT);
            break;

        case SMS_PERSISTENT_STORAGE_DATA_TYPE_ICCID:
            (void) strcat(FileNameBuffer_p, SMS_FS_FILENAME_ICCID);
            break;

        default:
            Success = FALSE;
            break;
        }
    }

    if (!Success) {
        // Something has gone wrong so NUL the string being returned.
        *FileNameBuffer_p = '\0';
    }

    SMS_B_(SMS_LOG_D("SmUtil_PersistentStorageData_CreateFileName: File=%s", FileNameBuffer_p));
    return Success;
}

/********************************************************************/
/**
 *
 * @function     SmUtil_PersistentStorageData_FileOpen
 *
 * @description  Opens the file associated with supplied DataType. If
 *               flag is set, the file will (re-)created.
 *
 * @param        DataType : Type of persistent data to be stored.
 * @param        Create : TRUE if file should be (re-)created.
 *
 * @return       FILE* : File descriptor information.
 */
/********************************************************************/
static FILE *SmUtil_PersistentStorageData_FileOpen(
    const SMS_PersistentStorageDataType_t DataType,
    const uint8_t Create)
{
    FILE *File_p = NULL;
    char *FileNameBuffer_p = SMS_HEAP_UNTYPED_ALLOC(FILENAME_MAX + 1);

    if (FileNameBuffer_p != NULL) {
        if (SmUtil_PersistentStorageData_CreateFileName(DataType, Create, FileNameBuffer_p)) {

            // If Create is set, we are writing data to the file system.
            if (Create) {
                // Open file so that it is writeable but removes existing data.
                File_p = fopen(FileNameBuffer_p, "wb");
            } else {
                // Open File (read only). Note that this will deliberately fail if the file is not present.
                File_p = fopen(FileNameBuffer_p, "rb");
            }
        }

        SMS_HEAP_FREE(&FileNameBuffer_p);
    }

    return File_p;
}

/********************************************************************/
/**
 *
 * @function     SmUtil_PersistentStorageData_Write
 *
 * @description  Writes the supplied data to the file associated with
 *               DataType.
 *
 * @param        DataType : Type of persistent data to be stored.
 * @param        WriteSize : Size of data to be written to file.
 * @param        Data_p : Data to be written to file.
 *
 * @return       SMS_Error_t : Error value.
 */
/********************************************************************/
SMS_Error_t SmUtil_PersistentStorageData_Write(
    const SMS_PersistentStorageDataType_t DataType,
    const size_t WriteSize,
    const void *const Data_p)
{
    SMS_Error_t SMS_Error = SMS_ERROR_INTERN_NO_ERROR;

    SMS_B_(SMS_LOG_D("SmUtil_PersistentStorageData_Write: DataType = %d", DataType));

    if (Data_p != NULL) {

        FILE *File_p = SmUtil_PersistentStorageData_FileOpen(DataType, TRUE);

        if (File_p != NULL) {
            if (fwrite(Data_p, WriteSize, 1, File_p) == 0) {
                SMS_Error = SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED;
            }
#ifdef SMS_PRINT_B_
            else {
                SmUtil_PersistentStorage_Data_DumpToLog(DataType, WriteSize, Data_p, TRUE);
            }
#endif

            (void) fclose(File_p);
        } else {
            SMS_A_(SMS_LOG_E("SmUtil_PersistentStorageData_Write: Data does not exist"));
            SMS_Error = SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED;
        }
    } else {
        SMS_Error = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    }

    SMS_B_(SMS_LOG_D("SmUtil_PersistentStorageData_Write: Error = 0x%04X", SMS_Error));
    return SMS_Error;
}

/********************************************************************/
/**
 *
 * @function     SmUtil_PersistentStorageData_ReadFile
 *
 * @description  Reads the data from the file associated with
 *               DataType.
 *
 * @param        DataType : Type of persistent data to be read.
 * @param        ReadSize : Size of data to be read from file.
 * @param        Data_p : Buffer for data to be read from file.
 * @param        FileExists_p : TRUE if file exists, FALSE if file
 *                              does not exist in FS.
 *
 * @return       SMS_Error_t : Error value.
 */
/********************************************************************/
static SMS_Error_t SmUtil_PersistentStorageData_ReadFile(
    const SMS_PersistentStorageDataType_t DataType,
    const size_t ReadSize,
    void *const Data_p,
    uint8_t * const FileExists_p)
{
    SMS_Error_t SMS_Error = SMS_ERROR_INTERN_NO_ERROR;

    SMS_B_(SMS_LOG_D("SmUtil_PersistentStorageData_ReadFile: DataType = %d", DataType));

    if (Data_p != NULL && FileExists_p != NULL) {
        FILE *File_p = SmUtil_PersistentStorageData_FileOpen(DataType, FALSE);

        if (File_p != NULL) {
            *FileExists_p = TRUE;

            if (fread(Data_p, ReadSize, 1, File_p) == 0) {
                SMS_Error = SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED;
            }
#ifdef SMS_PRINT_B_
            else {
                SmUtil_PersistentStorage_Data_DumpToLog(DataType, ReadSize, Data_p, FALSE);
            }
#endif

            (void) fclose(File_p);
        } else {
            SMS_A_(SMS_LOG_E("SmUtil_PersistentStorageData_ReadFile: Data does not exist"));
            SMS_Error = SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED;
        }
    } else {
        SMS_Error = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    }

    SMS_B_(SMS_LOG_D("SmUtil_PersistentStorageData_ReadFile: Error = 0x%04X", SMS_Error));
    return SMS_Error;
}

/********************************************************************/
/**
 *
 * @function     SmUtil_PersistentStorageData_Read
 *
 * @description  Reads the data from the file associated with
 *               DataType.  If the file does not exist, e.g. because
 *               the phone is newly flashed and this is the first time
 *               the persistent storage is used, then the file will be
 *               created containing default data (all 0x00). This default
 *               data will also be returned to the caller.
 *
 * @param        DataType : Type of persistent data to be read.
 * @param        ReadSize : Size of data to be read from file.
 * @param        Data_p : Buffer for data to be read from file.
 *
 * @return       SMS_Error_t : Error value.
 */
/********************************************************************/
SMS_Error_t SmUtil_PersistentStorageData_Read(
    const SMS_PersistentStorageDataType_t DataType,
    const size_t ReadSize,
    void *const Data_p)
{
    SMS_Error_t SMS_Error;
    uint8_t FileExists = FALSE;

    SMS_B_(SMS_LOG_D("SmUtil_PersistentStorageData_Read: DataType = %d", DataType));

    SMS_Error = SmUtil_PersistentStorageData_ReadFile(DataType, ReadSize, Data_p, &FileExists);

    if (SMS_Error != SMS_ERROR_INTERN_NO_ERROR && !FileExists) {
        // Reading the data from the file was not successful and the file does not exist in the FS.
        // Therefore try and create an new file containing some default data.  (The default data is
        // created by filling the file with 0x00.)
        // We also return this default data to the caller.
        SMS_A_(SMS_LOG_I("SmUtil_PersistentStorageData_Read: Create Default Data: DataType = %d", DataType));
        (void) memset(Data_p, 0x00, ReadSize);
        SMS_Error = SmUtil_PersistentStorageData_Write(DataType, ReadSize, Data_p);
    }

    SMS_B_(SMS_LOG_D("SmUtil_PersistentStorageData_Read: Error = 0x%04X", SMS_Error));
    return SMS_Error;
}

/********************************************************************/
/**
 *
 * @function     SmUtil_HexPrint_Raw
 *
 * @description  Prints the Hex bytes of the data to the log
 *
 * @param [in]   data_p : Raw data to print to log.
 * @param [in]   length : Length of raw data to print to log.
 * @param [in]   debugtag_p : Debug string to print to log to state
 *                            what data is being printed.
 *
 * @return       void
 */
/********************************************************************/
#define CONVERTED_ASCII_CHAR_SIZE          3    // Each byte is converted to 2 ascii chars and 1 space, i.e. 0x3A is converted to "3A "
#define NUMBER_OF_CONVERTED_BYTES_PER_LINE 16
void SmUtil_HexPrint_Raw(
    const uint8_t * data_p,
    const size_t length,
    const char *debugtag_p)
{
    char *PrintBuffer_p = SMS_HEAP_UNTYPED_ALLOC((NUMBER_OF_CONVERTED_BYTES_PER_LINE * CONVERTED_ASCII_CHAR_SIZE) + 1);

    if (PrintBuffer_p != NULL) {
        size_t remaining = length;

        SMS_A_(SMS_LOG_I("%s (%d bytes) =", debugtag_p, length));

        while (remaining > 0) {
            size_t bytes_to_convert;
            char *CharBuffer_p = PrintBuffer_p;

            if (remaining > NUMBER_OF_CONVERTED_BYTES_PER_LINE) {
                bytes_to_convert = NUMBER_OF_CONVERTED_BYTES_PER_LINE;
            } else {
                bytes_to_convert = remaining;
            }
            remaining -= bytes_to_convert;

            while (bytes_to_convert--) {
                sprintf(CharBuffer_p, "%02X ", *data_p);
                CharBuffer_p += CONVERTED_ASCII_CHAR_SIZE;
                data_p++;
            }

            SMS_A_(SMS_LOG_I("<DATA> %s</DATA>", PrintBuffer_p));
        }
        SMS_HEAP_FREE(&PrintBuffer_p);
    }
}

/********************************************************************/
/**
 *
 * @function     SmUtil_UICC_ServiceEnabled
 *
 * @description  Check if the service is enabled on the SIM.
 *
 * @param        ServiceType Service to check.
 *
 * @return       TRUE if service is enabled, else FALSE.
 */
/********************************************************************/
uint8_t SmUtil_UICC_ServiceEnabled(
    const sim_service_type_t ServiceType)
{
    ste_sim_t *UICC_p;
    uint8_t ServiceAvailable = FALSE;
    uint8_t ServiceEnabled = FALSE;

    SMS_B_(SMS_LOG_D("SmUtil_Check_UICC_ServiceEnabled: Enter with ServiceType %d", ServiceType));

    UICC_p = MessagingSystem_UICC_SimSyncGet();

    if (UICC_p != NULL) {
        uintptr_t UICC_Client_Tag = (uintptr_t) ServiceType;
        ste_uicc_get_service_availability_response_t uicc_service_availability_response = {
            STE_UICC_STATUS_CODE_FAIL,
            STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
            STE_UICC_SERVICE_AVAILABILITY_UNKNOWN
        };
        ste_uicc_get_service_table_response_t uicc_service_table_response = {
            STE_UICC_STATUS_CODE_FAIL,
            STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
            {0, 0}
            ,
            STE_UICC_SERVICE_STATUS_UNKNOWN
        };

        if (ste_uicc_get_service_availability_sync(UICC_p, UICC_Client_Tag, ServiceType, &uicc_service_availability_response) == 0) {
            SMS_B_(SMS_LOG_D("SmUtil_Check_UICC_ServiceEnabled: status code %d, fail details %d, availability %d",
                          uicc_service_availability_response.uicc_status_code, uicc_service_availability_response.uicc_status_code_fail_details,
                          uicc_service_availability_response.service_availability));

            if (uicc_service_availability_response.uicc_status_code == STE_UICC_STATUS_CODE_OK && uicc_service_availability_response.service_availability == STE_UICC_SERVICE_AVAILABLE) {
                ServiceAvailable = TRUE;
            }
        } else {
            SMS_A_(SMS_LOG_E("SmUtil_Check_UICC_ServiceEnabled: ste_uicc_get_service_availability_sync FAILED"));
        }

        if (ServiceAvailable) {
            if (ServiceType == SIM_SERVICE_TYPE_SMS_PP ||
                ServiceType == SIM_SERVICE_TYPE_SMS_CB ||
                ServiceType == SIM_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM) {
                SMS_A_(SMS_LOG_E("SmUtil_Check_UICC_ServiceEnabled: ServiceType %d always enabled if available", ServiceType));
                ServiceEnabled = TRUE;
            } else {
                if (ste_uicc_get_service_table_sync(UICC_p, UICC_Client_Tag, ServiceType, &uicc_service_table_response) == 0) {
                    SMS_B_(SMS_LOG_D("SmUtil_Check_UICC_ServiceEnabled: status code %d, fail details %d, status %d",
                                  uicc_service_table_response.uicc_status_code, uicc_service_table_response.uicc_status_code_fail_details, uicc_service_table_response.service_status));

                    if ((uicc_service_table_response.uicc_status_code == STE_UICC_STATUS_CODE_OK && uicc_service_table_response.service_status == STE_UICC_SERVICE_STATUS_ENABLED) ||
                        uicc_service_table_response.uicc_status_code_fail_details == STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED) {
                        ServiceEnabled = TRUE;
                    }
                } else {
                    SMS_A_(SMS_LOG_E("SmUtil_Check_UICC_ServiceEnabled: ste_uicc_get_service_table_sync FAILED"));
                }
            }
        }
    } else {
        SMS_A_(SMS_LOG_E("SmUtil_Check_UICC_ServiceEnabled: SIM UICC Server Object FAILED"));
    }

    SMS_B_(SMS_LOG_D("SmUtil_Check_UICC_ServiceEnabled Exit with ServiceAvailable %d, ServiceEnabled %d", ServiceAvailable, ServiceEnabled));
    return ServiceEnabled;
}

#define EF_FDN_FILENAME 0x6F3B
#define EF_FDN_FILEPATH NULL

/********************************************************************/
/**
 *
 * @function     SmUtil_FixedDiallingNumbersMatched
 *
 * @description  Check if the SMSC and Destination Addresses are matched
 *               with an FDN stored on the USIM.
 *
 * @param        SMSC_Address_p: SMSC Address to be checked.
 * @param        DestinationAddress_p: Destination Address to be checked.
 *
 * @return       TRUE if both SMSC and Destination Addresses match FDN USIM list.
 */
/********************************************************************/
uint8_t SmUtil_FixedDiallingNumbersMatched(
    const ShortMessagePackedAddress_t * const SMSC_Address_p,
    const ShortMessagePackedAddress_t * const DestinationAddress_p)
{
    uint8_t FDN_SMSC_Matched = FALSE;
    uint8_t FDN_DestinationMatched = FALSE;

    uint8_t NoOfRecords = 0;
    uint8_t RecordLength = 0;
    uint8_t FDN_Index = 0;
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

    sim_sync = MessagingSystem_UICC_SimSyncGet();

    client_tag = (uintptr_t) sim_sync;

    // Get the EFfdn file format
    result = ste_uicc_sim_file_get_format_sync(sim_sync, client_tag, EF_FDN_FILENAME, EF_FDN_FILEPATH, &file_format);

    if (result == 0 && STE_UICC_STATUS_CODE_OK == file_format.uicc_status_code) {
        NoOfRecords = file_format.num_records;
        RecordLength = file_format.record_len;
        SMS_B_(SMS_LOG_D("smutil.c:SmUtil_FixedDiallingNumbersMatched num_records = %d, record_len = %d ", file_format.num_records, file_format.record_len));
        SMS_B_(SmUtil_HexPrint_Raw((uint8_t *) SMSC_Address_p, sizeof(SMS_PackedAddress_t), "SMS: smutil.c: SMSC Address"));
        SMS_B_(SmUtil_HexPrint_Raw((uint8_t *) DestinationAddress_p, sizeof(ShortMessagePackedAddress_t), "SMS: smutil.c: Destination Address"));
    } else {
        SMS_A_(SMS_LOG_E("smutil.c: ERROR: ste_uicc_sim_file_get_format result = %d ", result));
        SMS_A_(SMS_LOG_E("smutil.c: ERROR: ste_uicc_sim_file_get_format status = %d, fail_details = %d ", file_format.uicc_status_code, file_format.uicc_status_code_fail_details));
    }

    // Check each FDN Address in EFfdn
    for (FDN_Index = 0; FDN_Index < NoOfRecords; FDN_Index++) {
        ste_uicc_sim_file_read_record_response_t read_record = {
            STE_UICC_STATUS_CODE_FAIL,
            STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
            {0x00, 0x00}
            ,
            0,
            0
        };

        result = -1;

        result = ste_uicc_sim_file_read_record_sync(sim_sync, client_tag, EF_FDN_FILENAME, FDN_Index+1, RecordLength, EF_FDN_FILEPATH, &read_record);

        if (result == 0 && read_record.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
            if (read_record.length <= RecordLength) {
                uint8_t FDN_FirstByte = read_record.length - (sizeof(ShortMessagePackedAddress_t) + 2); // + 2 for Capability and Extension bytes

                SMS_B_(SMS_LOG_D("smutil.c: FDN Read: Index %d, record length %d", FDN_Index, read_record.length));

                SMS_B_(SmUtil_HexPrint_Raw(&read_record.data[FDN_FirstByte], sizeof(ShortMessagePackedAddress_t), "SMS: smutil.c: FDN Address"));

                // Check FDN Address valid
                if (read_record.data[FDN_FirstByte] != 0xFF && read_record.data[FDN_FirstByte] > 1) {
                    uint8_t FDN_AddressIndex = FDN_FirstByte;
                    uint8_t AddressLength = (uint8_t) SMS_MIN(read_record.data[FDN_FirstByte] - 1, SMS_MAX_PACKED_ADDRESS_LENGTH);      // -1 exclude TON/NPI

                    if (SMSC_Address_p->Length == read_record.data[FDN_AddressIndex++]) {
                        if (SMSC_Address_p->TypeOfAddress == read_record.data[FDN_AddressIndex++]) {
                            if (memcmp(SMSC_Address_p->AddressValue, &read_record.data[FDN_AddressIndex], AddressLength) == 0) {
                                SMS_B_(SMS_LOG_D("smutil.c: FDN Address %d matched SMSC Address", FDN_Index));
                                FDN_SMSC_Matched = TRUE;
                            }
                        }
                    }
                    // Reset index to check Destination Address
                    FDN_AddressIndex = FDN_FirstByte + 1;       // Skip length check

                    // Don't check Destination Address length as different format to FDN address length
                    if (DestinationAddress_p->TypeOfAddress == read_record.data[FDN_AddressIndex++]) {

                        if (memcmp(DestinationAddress_p->AddressValue, &read_record.data[FDN_AddressIndex], AddressLength) == 0) {
                            SMS_B_(SMS_LOG_D("smutil.c: FDN Address %d matched Destination Address", FDN_Index));
                            FDN_DestinationMatched = TRUE;
                        }
                    }

                    if (FDN_SMSC_Matched && FDN_DestinationMatched) {
                        // No need to continue once both addresses find a match in FDN list
                        break;
                    }
                } else {
                    SMS_B_(SMS_LOG_D("smutil.c: FDN Address %d empty", FDN_Index));
                }
            } else {
                SMS_A_(SMS_LOG_E("smutil.c: ERROR: ste_uicc_sim_file_read_record length:%d, expected:%d", read_record.length, RecordLength));
            }
        } else {
            SMS_A_(SMS_LOG_E("smutil.c: ERROR: ste_uicc_sim_file_read_record result = %d ", result));
            SMS_A_(SMS_LOG_E("smutil.c: ERROR: ste_uicc_sim_file_read_record status = %d, fail_details = %d ", file_format.uicc_status_code, file_format.uicc_status_code_fail_details));
        }
    }

    SMS_B_(SMS_LOG_D("smutil.c: FDN_SMSC_Matched = %d FDN_DestinationMatched = %d", FDN_SMSC_Matched, FDN_DestinationMatched));
    return FDN_SMSC_Matched && FDN_DestinationMatched;
}
