/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup ldr_boot_records_handler
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "r_boot_records_handler.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "flash_manager.h"
#include "boot_area_management.h"
#include "r_serialization.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/* Minimum size of one boot record. */
#define MIN_BOOTRECORD_SIZE   8

static BootRecord_t *BootRecordsContainer_pp[MAX_NUMBER_OF_BOOTRECORDS] = {NULL,};

static BootRecord_t *Domain_BootRecord_p = NULL;
/*******************************************************************************
 * Definition of file internal functions
 ******************************************************************************/
static ErrorCode_e Fill_BootRecordsContainer
(
    uint32 *BootRecordsImage_p,
    uint32 BootRecordsSize,
    boolean MemconfOK,
    boolean DomainBR_Exist
);

static ErrorCode_e CheckIfExistDomainBR
(
    void
);

static ErrorCode_e WriteBootRecordsToFlash
(
    uint32 *BootRecordsImage_p,
    uint32 BootRecordsSize
);

static ErrorCode_e ReadBootRecordsFromFlashAndFillContainers
(
    void
);

static ErrorCode_e SynchronizeContainersWithFlash
(
    void
);

static ErrorCode_e AdjustMemconfSetings
(
    uint8 *ReceivedMemconf_p,
    uint32 MemconfSize
);

static ErrorCode_e CollectBootRecordAndGenerateImage
(
    uint8 **GeneratedImage_pp,
    uint32 *GeneratedImageSize_p
);

/*******************************************************************************
 * Definition of file internal functions
 ******************************************************************************/
ErrorCode_e Do_WriteCompleteBootRecordsImage(uint32 *BootRecordsImage_p, uint32 BootRecordsSize)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 *GeneratedImage_p = NULL;
    uint32 GeneratedImageSize = 0;

    VERIFY(NULL != BootRecordsImage_p, E_INVALID_INPUT_PARAMETERS);
    IS_ALIGNED((uint32)BootRecordsImage_p);

    VERIFY(0 != BootRecordsSize, E_INVALID_INPUT_PARAMETERS);
    IS_ALIGNED(BootRecordsSize);

    if (!(MEMCONF_1_ID  == BootRecordsImage_p[0] ||
            MEMCONF_4_ID  == BootRecordsImage_p[0] ||
            MEMCONF_8_ID  == BootRecordsImage_p[0] ||
            MEMCONF_16_ID == BootRecordsImage_p[0] ||
            MEMCONF_32_ID == BootRecordsImage_p[0])) {
        ReturnValue = E_INVALID_BOOTRECORD_IMAGE;
        goto ErrorExit;
    }

    if (NULL == Domain_BootRecord_p) {
        ReturnValue = CheckIfExistDomainBR();
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    Do_BootRecordsContainer_Destroy();

    ReturnValue = Fill_BootRecordsContainer(BootRecordsImage_p, BootRecordsSize, FALSE, FALSE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = CollectBootRecordAndGenerateImage((uint8 **)&GeneratedImage_p, &GeneratedImageSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = WriteBootRecordsToFlash(GeneratedImage_p, GeneratedImageSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        Do_BootRecordsContainer_Destroy();
    }

    BUFFER_FREE(GeneratedImage_p);

    return ReturnValue;
}

ErrorCode_e Do_Read_BootRecordIndexed(uint32 BootRecordId,
                                      uint32 BootPartitionIndex,
                                      uint32 **BootRecord_pp,
                                      uint32 *BootRecordSize_p)

{
    ErrorCode_e ReturnValue = E_BOOTRECORD_NOT_EXIST;
    uint32 i = 0;

    VERIFY(NULL != BootRecordSize_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != BootRecord_pp, E_INVALID_INPUT_PARAMETERS);

    *BootRecordSize_p = 0;

    if (NULL == BootRecordsContainer_pp[0]) {
        ReturnValue = ReadBootRecordsFromFlashAndFillContainers();
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    ReturnValue = E_BOOTRECORD_NOT_EXIST;

    for (i = 0; i < MAX_NUMBER_OF_BOOTRECORDS; i++) {
        if (BootRecordId == BootRecordsContainer_pp[i]->BR_Id &&
                BootPartitionIndex == BootRecordsContainer_pp[i]->PartitionIndex) {
            *BootRecordSize_p = BootRecordsContainer_pp[i]->BR_Size;
            *BootRecord_pp = BootRecordsContainer_pp[i]->BRData_p;
            ReturnValue = E_SUCCESS;
            goto ErrorExit;
        }
    }

ErrorExit:
    return ReturnValue;
}

ErrorCode_e Do_Write_BootRecordIndexed(uint32 BootRecordId,
                                       uint32 BootPartitionIndex,
                                       uint32 *BootRecordData_p,
                                       uint32 BootRecordSize)
{
    ErrorCode_e ReturnValue = E_BOOTRECORD_WRITE_FAILED;
    uint32 i = 0;

    //Sanity check of input parameters.
    VERIFY(0 != BootRecordId, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != BootRecordData_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 != BootRecordSize, E_INVALID_INPUT_PARAMETERS);
    IS_ALIGNED(BootRecordSize);

    //If containers are empty try to read them from flash.
    if (NULL == BootRecordsContainer_pp[0]) {
        ReturnValue = ReadBootRecordsFromFlashAndFillContainers();
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    ReturnValue = E_BOOTRECORD_WRITE_FAILED;

    //Loop until appropriate place is found.
    for (i = 0; i < (MAX_NUMBER_OF_BOOTRECORDS - 1); i++) {
        if (BootRecordId == BootRecordsContainer_pp[i]->BR_Id &&
                BootPartitionIndex == BootRecordsContainer_pp[i]->PartitionIndex) {
            //Free previous allocated buffer.
            BUFFER_FREE(BootRecordsContainer_pp[i]->BRData_p);
            ReturnValue = E_SUCCESS;
            break;
        } else if (NULL == BootRecordsContainer_pp[i]) {
            //Allocate  space for new container.
            BootRecordsContainer_pp[i] = (BootRecord_t *)malloc(sizeof(BootRecord_t));
            VERIFY(NULL != BootRecordsContainer_pp[i], E_ALLOCATE_FAILED);
            ReturnValue = E_SUCCESS;
            break;
        }
    }

    VERIFY(E_SUCCESS == ReturnValue, E_BOOTRECORD_FULL);

    BootRecordsContainer_pp[i]->BR_Id = BootRecordId;
    BootRecordsContainer_pp[i]->PartitionIndex = BootPartitionIndex;
    BootRecordsContainer_pp[i]->BR_Size = BootRecordSize;

    //Prepare buffer for new record
    BootRecordsContainer_pp[i]->BRData_p = (uint32 *)malloc(BootRecordSize);
    VERIFY(NULL != BootRecordsContainer_pp[i]->BRData_p, E_ALLOCATE_FAILED);

    //Copy received contents into new buffer
    memcpy(BootRecordsContainer_pp[i]->BRData_p, BootRecordData_p, BootRecordSize);

    //Synchronize new RAM data with Flash data
    ReturnValue = SynchronizeContainersWithFlash();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    //Copy Domain boot record in local buffer
    if (DOMAIN_DATA_ID == BootRecordsContainer_pp[i]->BR_Id) {
        if (NULL != Domain_BootRecord_p) {
            BUFFER_FREE(Domain_BootRecord_p->BRData_p);
        } else {
            Domain_BootRecord_p = (BootRecord_t *)malloc(sizeof(BootRecord_t));
            ASSERT(NULL != Domain_BootRecord_p);
        }

        Domain_BootRecord_p->BR_Id = BootRecordId;
        Domain_BootRecord_p->PartitionIndex = BootPartitionIndex;
        Domain_BootRecord_p->BR_Size = BootRecordSize;
        Domain_BootRecord_p->BRData_p = (uint32 *)malloc(BootRecordSize);
        ASSERT(NULL != Domain_BootRecord_p->BRData_p);

        memcpy(Domain_BootRecord_p->BRData_p, BootRecordsContainer_pp[i]->BRData_p, BootRecordSize);
    }

ErrorExit:
    B_(printf("boot_records_handler.c (%d): Do_Write_BootRecordIndexed return (%d)\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

void Do_BootRecordsContainer_Destroy(void)
{
    uint32 i = 0;

    if (NULL != BootRecordsContainer_pp[0]) {
        for (i = 0; i < MAX_NUMBER_OF_BOOTRECORDS; i++) {
            if (NULL != BootRecordsContainer_pp[i]) {
                BUFFER_FREE(BootRecordsContainer_pp[i]->BRData_p);
                BUFFER_FREE(BootRecordsContainer_pp[i]);
            }
        }
    }
}

/*******************************************************************************
 * File Internal functions
 ******************************************************************************/
static ErrorCode_e Fill_BootRecordsContainer(uint32 *BootRecordsImage_p,
        uint32 BootRecordsSize,
        boolean MemconfOK,
        boolean DomainBR_Exist)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 BootRecordId = 0;
    uint32 BR_Size = 0;
    uint32 NrOfPartitions = 0;

    VERIFY(((NULL != BootRecordsImage_p) && (0 != BootRecordsSize)), E_INVALID_INPUT_PARAMETERS);

    if (FALSE == MemconfOK) {
        BootRecordsContainer_pp[BootRecordId] = (BootRecord_t *)malloc(sizeof(BootRecord_t));
        VERIFY(NULL != BootRecordsContainer_pp[BootRecordId], E_ALLOCATE_FAILED);
        memset(BootRecordsContainer_pp[BootRecordId], 0x00, sizeof(BootRecord_t));

        BootRecordsContainer_pp[BootRecordId]->BR_Id = BootRecordsImage_p[0];
        BootRecordsContainer_pp[BootRecordId]->BR_Size = BootRecordsImage_p[1];

        BR_Size = BootRecordsContainer_pp[BootRecordId]->BR_Size;
        VERIFY(((BR_Size <= BootRecordsSize) && (MIN_BOOTRECORD_SIZE < BR_Size)), E_BOOTRECORDS_MISMATCH);

        BootRecordsContainer_pp[BootRecordId]->BRData_p = (uint32 *)malloc(BR_Size);
        VERIFY(NULL != BootRecordsContainer_pp[BootRecordId]->BRData_p, E_ALLOCATE_FAILED);

        memcpy(BootRecordsContainer_pp[BootRecordId]->BRData_p, BootRecordsImage_p, BR_Size);

        ReturnValue = AdjustMemconfSetings((uint8 *)(BootRecordsContainer_pp[BootRecordId]->BRData_p), BR_Size);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        BootRecordsContainer_pp[BootRecordId]->BR_Id = BootRecordsContainer_pp[BootRecordId]->BRData_p[0];

        BootRecordsImage_p += (BR_Size / sizeof(uint32));
        //(*(uint8 **)&BootRecordsImage_p) += BR_Size;
        BootRecordsSize -= BR_Size;
        BootRecordId++;
    }

    while (BootRecordsSize) {
        BootRecordsContainer_pp[BootRecordId] = (BootRecord_t *)malloc(sizeof(BootRecord_t));
        VERIFY(NULL != BootRecordsContainer_pp[BootRecordId], E_ALLOCATE_FAILED);
        memset(BootRecordsContainer_pp[BootRecordId], 0x00, sizeof(BootRecord_t));

        BootRecordsContainer_pp[BootRecordId]->BR_Id = BootRecordsImage_p[0];

        if (STOP_ID == BootRecordsContainer_pp[BootRecordId]->BR_Id) {
            BUFFER_FREE(BootRecordsContainer_pp[BootRecordId]);
            break;
        }

        BootRecordsContainer_pp[BootRecordId]->BR_Size = BootRecordsImage_p[1];
        BR_Size = BootRecordsContainer_pp[BootRecordId]->BR_Size;
        VERIFY(((BR_Size <= BootRecordsSize) && (MIN_BOOTRECORD_SIZE < BR_Size)), E_BOOTRECORDS_MISMATCH);

        BootRecordsContainer_pp[BootRecordId]->BRData_p = (uint32 *)malloc(BR_Size);
        VERIFY(NULL != BootRecordsContainer_pp[BootRecordId]->BRData_p, E_ALLOCATE_FAILED);

        memcpy(BootRecordsContainer_pp[BootRecordId]->BRData_p, BootRecordsImage_p, BR_Size);

        if (PARTITIONS_ID == BootRecordsContainer_pp[BootRecordId]->BR_Id) {
            NrOfPartitions = BootRecordsContainer_pp[BootRecordId]->BRData_p[3];
        }

        if (PARTITION_ID == BootRecordsContainer_pp[BootRecordId]->BR_Id) {
            if (NrOfPartitions > 0) {
                BootRecordsContainer_pp[BootRecordId]->PartitionIndex = BootRecordsContainer_pp[BootRecordId]->BRData_p[3];
                NrOfPartitions--;
            } else {
                Do_BootRecordsContainer_Destroy();
                ReturnValue = E_INVALID_BOOTRECORD_IMAGE;
                goto ErrorExit;
            }
        } else {
            BootRecordsContainer_pp[BootRecordId]->PartitionIndex = 0;
        }

        BootRecordsImage_p += (BR_Size / sizeof(uint32));
        //(*(uint8 **)&BootRecordsImage_p) += BR_Size;
        BootRecordsSize -= BR_Size;
        BootRecordId++;
    }

    /*
     * When Bind Properties command is performed Domain boot record is created. We
     * should always keep this record and treat as valid.
     */
    if ((NULL != Domain_BootRecord_p) && (FALSE == DomainBR_Exist)) {
        uint32 i = 0;

        for (i = 0; i < BootRecordId; i++) {
            /* If Domain boot record exist in boot record image from flash archive we
             * treat as invalid and replace with existing one.
             */
            if (DOMAIN_DATA_ID == BootRecordsContainer_pp[i]->BR_Id) {
                BootRecordId = i;
                BUFFER_FREE(BootRecordsContainer_pp[BootRecordId]->BRData_p);
                break;
            }
        }

        if (NULL == BootRecordsContainer_pp[BootRecordId]) {
            BootRecordsContainer_pp[BootRecordId] = (BootRecord_t *)malloc(sizeof(BootRecord_t));
            VERIFY(NULL != BootRecordsContainer_pp[BootRecordId], E_ALLOCATE_FAILED);
        }

        memset(BootRecordsContainer_pp[BootRecordId], 0x00, sizeof(BootRecord_t));

        BootRecordsContainer_pp[BootRecordId]->BR_Id = Domain_BootRecord_p->BR_Id;
        BootRecordsContainer_pp[BootRecordId]->BR_Size = Domain_BootRecord_p->BR_Size;
        BootRecordsContainer_pp[BootRecordId]->PartitionIndex = Domain_BootRecord_p->PartitionIndex;
        BootRecordsContainer_pp[BootRecordId]->BRData_p = (uint32 *)malloc(Domain_BootRecord_p->BR_Size);
        VERIFY(NULL != BootRecordsContainer_pp[BootRecordId]->BRData_p, E_ALLOCATE_FAILED);

        memcpy(BootRecordsContainer_pp[BootRecordId]->BRData_p, Domain_BootRecord_p->BRData_p, Domain_BootRecord_p->BR_Size);
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e CheckIfExistDomainBR(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 ContainerNr = 0;

    //Containers copy in RAM are empty, check if exist boot record image written on Flash.
    if (NULL == BootRecordsContainer_pp[0]) {
        ReturnValue = ReadBootRecordsFromFlashAndFillContainers();

        if ((E_BAM_NOT_CONFIGURED == ReturnValue) || (E_INVALID_BOOTRECORD_IMAGE == ReturnValue)) {
            //Domain boot record not created, there is no boot record image written on Flash.
            ReturnValue = E_SUCCESS;
            goto ErrorExit;
        }

        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    //Check if Domain boot record exist on flash.
    while (NULL != BootRecordsContainer_pp[ContainerNr]) {
        if (DOMAIN_DATA_ID == BootRecordsContainer_pp[ContainerNr]->BR_Id) {
            Domain_BootRecord_p = (BootRecord_t *)malloc(sizeof(BootRecord_t));
            ASSERT(NULL != Domain_BootRecord_p);

            Domain_BootRecord_p->BR_Id = BootRecordsContainer_pp[ContainerNr]->BR_Id;
            Domain_BootRecord_p->PartitionIndex = BootRecordsContainer_pp[ContainerNr]->PartitionIndex;
            Domain_BootRecord_p->BR_Size = BootRecordsContainer_pp[ContainerNr]->BR_Size;
            Domain_BootRecord_p->BRData_p = (uint32 *)malloc(Domain_BootRecord_p->BR_Size);
            ASSERT(NULL != Domain_BootRecord_p->BRData_p);

            memcpy(Domain_BootRecord_p->BRData_p, BootRecordsContainer_pp[ContainerNr]->BRData_p, Domain_BootRecord_p->BR_Size);
            break;
        }

        ContainerNr++;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e SynchronizeContainersWithFlash(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 *BootRecordsImage_p = NULL;
    uint32 BootRecordsImageSize = 0;
    uint32 i = 0;
    uint32 Offset = 0;

    VERIFY(NULL != BootRecordsContainer_pp[0], E_BOOTRECORD_EMPTY);

    while (NULL != BootRecordsContainer_pp[i]) {
        BootRecordsImageSize += BootRecordsContainer_pp[i]->BR_Size;
        i++;
    }

    BootRecordsImage_p = (uint32 *)malloc(BootRecordsImageSize);
    VERIFY(NULL != BootRecordsImage_p, E_ALLOCATE_FAILED);

    i = 0;

    while (NULL != BootRecordsContainer_pp[i]) {
        memcpy(BootRecordsImage_p + Offset / sizeof(uint32), BootRecordsContainer_pp[i]->BRData_p, BootRecordsContainer_pp[i]->BR_Size);
        Offset += BootRecordsContainer_pp[i]->BR_Size;
        i++;
    }

    ReturnValue = WriteBootRecordsToFlash(BootRecordsImage_p, BootRecordsImageSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    BUFFER_FREE(BootRecordsImage_p);

    return ReturnValue;
}

static ErrorCode_e WriteBootRecordsToFlash(uint32 *BootRecordsImage_p, uint32 BootRecordsSize)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_BAM_WriteBootArea((uint8 *)BootRecordsImage_p, BootRecordsSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:

    if (E_SUCCESS != ReturnValue) {
        Do_BootRecordsContainer_Destroy();
    }

    return ReturnValue;
}

static ErrorCode_e ReadBootRecordsFromFlashAndFillContainers(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 BootRecordsSize = 0;
    uint32 *BootRecordsInFlash_p = NULL;

    ReturnValue = Do_BAM_UnregisteredReadBootArea((uint8 **)&BootRecordsInFlash_p, &BootRecordsSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    if (!(MEMCONF_1_ID  == BootRecordsInFlash_p[0] ||
            MEMCONF_4_ID  == BootRecordsInFlash_p[0] ||
            MEMCONF_8_ID  == BootRecordsInFlash_p[0] ||
            MEMCONF_16_ID == BootRecordsInFlash_p[0] ||
            MEMCONF_32_ID == BootRecordsInFlash_p[0])) {
        ReturnValue = E_INVALID_BOOTRECORD_IMAGE;
        goto ErrorExit;
    }

    ReturnValue = Fill_BootRecordsContainer(BootRecordsInFlash_p, BootRecordsSize, TRUE, TRUE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    BUFFER_FREE(BootRecordsInFlash_p);

    return ReturnValue;
}

static void DeserializeMemconfBootRecord(Memconf_BootRecord_t *Memconf_BootRecord_p, void *Data_p)
{
    void *Iter_p = Data_p;

    Memconf_BootRecord_p->MemconfId = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->MemconfSize = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->Magic = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->PageSize = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->BlockSize = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->ColumnBits = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->AddrCycles = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->Type = (FlashTypes_t)get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->TotalSize = get_uint32_le(&Iter_p);
    Memconf_BootRecord_p->RedundantAreaSize = get_uint32_le(&Iter_p);
}
static ErrorCode_e AdjustMemconfSetings(uint8 *ReceivedMemconf_p, uint32 MemconfSize)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    Memconf_BootRecord_t NewMemconf = {0};
    FPD_Config_t Configuration = {0};

    VERIFY(MemconfSize == sizeof(Memconf_BootRecord_t), E_INVALID_BOOTRECORD_IMAGE);

    DeserializeMemconfBootRecord(&NewMemconf, ReceivedMemconf_p);
    //NewMemconf_p = (Memconf_BootRecord_t*)ReceivedMemconf_p;

    ReturnValue = Do_FPD_GetInfo(&Configuration);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    if (0x10 == Configuration.BusWidth) {
        NewMemconf_p->MemconfId = 0x10;
        NewMemconf_p->ColumnBits = 0x0b;
        NewMemconf_p->TotalSize = 0x20000;
    } else if (0x8 == Configuration.BusWidth) {
        NewMemconf_p->MemconfId = 0x08;
        NewMemconf_p->ColumnBits = 0x0c;
        NewMemconf_p->TotalSize = 0x10000;
    } else if (0x0 == Configuration.BusWidth) {
        NewMemconf_p->MemconfId = 0x10;
        NewMemconf_p->ColumnBits = 0x00;
        NewMemconf_p->TotalSize = 0x20000;
    } else {
        A_(printf("boot_records_handler.c (%d): Unknown memory type!!!", __LINE__);)
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    VERIFY(Configuration.DeviceSize >= FlashSizeREQbyArchive, E_INVALID_SIZE_IN_MEMCONF);
    VERIFY(FlashSizeREQbyArchive > 0, E_INVALID_SIZE_IN_MEMCONF);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e CollectBootRecordAndGenerateImage(uint8 **GeneratedImage_pp, uint32 *GeneratedImageSize_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *GeneratedImage_p = NULL;
    uint32 GeneratedImageSize = 0;
    uint32 InImageOffset = 0;
    Memconf_BootRecord_t *Memconf_p = NULL;
    Signature_t *Signature_p = NULL;
    uint32 AccumulatedCRC = 0;
    uint32 i = 0;

    // Calculate SIGNATURE boot record data which is Modulo 2^32 of the sum of all words up to the checksum itself
    Memconf_p = (Memconf_BootRecord_t *)BootRecordsContainer_pp[0]->BRData_p;

    AccumulatedCRC += Memconf_p->MemconfId;
    AccumulatedCRC += Memconf_p->MemconfSize;
    AccumulatedCRC += Memconf_p->Magic;
    AccumulatedCRC += Memconf_p->PageSize;
    AccumulatedCRC += Memconf_p->BlockSize;
    AccumulatedCRC += Memconf_p->ColumnBits;
    AccumulatedCRC += Memconf_p->AddrCycles;
    AccumulatedCRC += Memconf_p->Type;
    AccumulatedCRC += Memconf_p->TotalSize;
    AccumulatedCRC += Memconf_p->RedundantAreaSize;

    Signature_p = (Signature_t *)BootRecordsContainer_pp[1]->BRData_p;

    AccumulatedCRC += Signature_p->Signature_Id;
    AccumulatedCRC += Signature_p->Signature_Size;

    //Update Signature field
    Signature_p->Signature = AccumulatedCRC;

    C_(printf("\n\n boot_record_handler.c %d: Calculatd SIGNATURE *** %x ***\n\n", __LINE__, AccumulatedCRC);)

    while (NULL != BootRecordsContainer_pp[i]) {
        GeneratedImageSize += BootRecordsContainer_pp[i]->BR_Size;

        GeneratedImage_p = (uint8 *)realloc(GeneratedImage_p, GeneratedImageSize);

        if (NULL == GeneratedImage_p) {
            A_(printf("boot_records_handler.c %d: Allocate failed!!!", __LINE__);)
            ReturnValue = E_ALLOCATE_FAILED;
            goto ErrorExit;
        }

        memcpy(GeneratedImage_p + InImageOffset, (uint8 *)BootRecordsContainer_pp[i]->BRData_p, BootRecordsContainer_pp[i]->BR_Size);

        InImageOffset = GeneratedImageSize;
        i++;
    }

    *GeneratedImage_pp = GeneratedImage_p;
    *GeneratedImageSize_p = GeneratedImageSize;

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/** @} */
/** @} */
