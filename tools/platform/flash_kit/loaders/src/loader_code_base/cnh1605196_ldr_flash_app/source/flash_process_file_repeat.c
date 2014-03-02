/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_flash_app
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "error_codes.h"
#include "fcntl.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "r_flash_applications.h"
#include "r_command_exec_service.h"
#include "r_zip_parser.h"
#include "r_manifest_parser.h"
#include "r_flashlayout_parser.h"
#include "r_bulk_protocol.h"
#include "r_io_subsystem.h"
#include "io_subsystem.h"
#include "command_ids.h"
#include "commands.h"
#include "r_serialization.h"
#include "r_basicdefinitions.h"
#include "r_path_utilities.h"
#include "flash_applications.h"
#include "r_service_management.h"
#include "r_loader_sec_lib.h"

#include "r_boot_records_handler.h"
#include "r_communication_abstraction.h"
#include "flash_manager.h"
#include "block_device_management.h"
#include "chipid.h"

#ifdef MACH_TYPE_STN8500
#include "bam_emmc.h"
#endif

#include "boot_area_management.h"
#include "toc_handler.h"

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
#include "r_time_utilities.h"
#include "r_measurement_tool.h"
#endif

#include "flash_process_file_repeat.h"
#include "flash_process_file_info.h"
#include "versions_parser.h"
#include "fm_emmc.h"
#include "r_security_algorithms.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

#define FILL_BUFFER_SIZE         (0x80000)  /* must be uint32 aligned */

#ifdef FAILSAFE_FLASHING
#define FAILSAFE_MAGIC     0xFA175AFEFA175AFE
#define FAILSAFE_AMOUNT    0x20000000   /* 512MB */
#endif

// From flash_applications_functions.c
extern const char *SignatureString;
extern const char *ArchiveString;
extern const char *ContentsString;
extern const char *ManifestFileName;
extern const char *FlashlayoutFileName;
extern const char *VersionsFileName;

extern const char *BDMUnitString;
extern const char *BAMUnitString;

extern const char *BootString;
extern const char *FlashString;
extern const char *FlashDeviceString;

extern const char *ContentTypeImageString;
extern const char *ContentTypeSparseImageString;
extern const char *ContentTypeBootImageString;
extern const char *ContentTypeEmptyPartitionString;
extern const char *ContentTypeFlashLayoutString;

extern const char *BDMDeviceString;
extern const char *BAMDeviceString;

extern const char *PartSizeString;

// These content types in flash layout will create a TOC entry that describes the partition. If an image is present 2 TOC entries will be created
const char *const FlashLayoutPartitionEntryString1 = "x-empflash/TOC-Partition-Entry";
const char *const FlashLayoutPartitionEntryString2 = "x-steflash/TOC-Partition-Entry";

static const char *SkipTypes[] = {
    "x-empflash/flash-layout",
    NULL // array terminator
};

static const char *SupportedArchiveNames[] = {
    "contents.zip",
    "archive.zip",
    NULL  // array terminator
};

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/

static ProcessFileParameters_t *CreateInitProcessFileParam(void);

static ErrorCode_e Handle_ProcessInit(ProcessFileParameters_t *Param_p, void *ExecutionContext_p);
static ErrorCode_e Handle_ProcessParseZip(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessOpenSignature(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessReadSignature(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessParseZipInZip(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessOpenManifest(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessReadManifest(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessOpenVersions(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessReadVersions(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessOpenFlashlayout(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessReadFlashlayout(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessReadTOCandSUBTOC(ProcessFileParameters_t *Param_p);
#ifdef FAILSAFE_FLASHING
static ErrorCode_e Handle_ProcessFailSafeUpdate(ProcessFileParameters_t *Param_p);
#endif
static ErrorCode_e Handle_ProcessAllocateFile(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessReadFile(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessWriteData(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessWriteDataLoop(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessWriteEmptyPartitions(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessWritePartitionEntries(ProcessFileParameters_t *params_p);
static ErrorCode_e Handle_ProcessWriteBootArea(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessWriteBootArea_Layout_2_0(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessWriteBootArea_Layout_3_0(ProcessFileParameters_t *Param_p);
static ErrorCode_e Handle_ProcessFileFinalize(ProcessFileParameters_t *Param_p);

static ErrorCode_e CompatibilityCheckManifestVsFlashlayout(ProcessFileParameters_t *Param_p);
static ErrorCode_e CompatibilityCheckManifestVsTOC(ProcessFileParameters_t *Param_p, const uint8 ExistingTOCDevice);
static ErrorCode_e PartitionsOverlapAndSpaceCheck(ManifestParser_t *ManifestParser_p, uint32 NrPartitions, FlashLayoutParser_t *Flashlayout_p, uint32 FlashlayoutImages);
static boolean     IsContentTypeSkippable(const char *Type_p);
static boolean     IsFirstEntryInTOC_Partition(ProcessFileParameters_t *Param_p, char *TocID, uint32 TargetStartAddress);
static boolean     IsLastEntryInTOC_Partition(ProcessFileParameters_t *Param_p, char *TocID, uint32 TargetStartAddress);
static ErrorCode_e UpdateSparseImagesParameters(ProcessFileParameters_t *Param_p);
static ErrorCode_e ReadOldBootArea(ProcessFileParameters_t *Param_p);
static ErrorCode_e VerifySpaceInPartition(FlashLayoutParser_t *Flashlayout_p, uint32 NbrFlashlayoutEntries, ManifestParser_t *ManifestParserEntry_p, uint64 FileSize);
static ErrorCode_e CheckEnhancedSettings(ManifestParser_t *ManifestEntry_p);

#ifdef FAILSAFE_FLASHING
static ErrorCode_e UpdateFlashProgressInfo(ProcessFileParameters_t *Param_p);
static ErrorCode_e ReflashInProgressCheck(ProcessFileParameters_t *Param_p);
#endif

#ifdef MACH_TYPE_STN8500
static ErrorCode_e Update_TOC_List(ProcessFileParameters_t *Param_p);
static ErrorCode_e PrepareTOC_Data(ProcessFileParameters_t *Param_p, TOC_Entry_t *TmpEntry_p);
#endif

#ifdef PRINT_LEVEL_C_
static void PrintManifestEntries(ManifestParser_t *Manifest_p, uint32 ImagesNr);
#endif

// From flash_applications_functions.c
SourceType_t GetImageType(ManifestParser_t *ManifestParser_p);
uint32 GetDeviceNumber(void *Device_p);
uint16 ExtractType(char *Type);
ErrorCode_e Read_SubTOC_Data(uint32 Offset, uint32 Size, uint8 *Data_p, uint32 BufferSize, FPD_Config_t *const FPDConfig_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
ErrorCode_e Flash_ProcessFileRepeat(void *ExecutionContext_p)
{
    ErrorCode_e                       ReturnValue = E_GENERAL_FATAL_ERROR;
    ErrorCode_e                       TempReturnValue = E_GENERAL_FATAL_ERROR;
    static ProcessFileParameters_t   *Param_p = NULL;

    VERIFY(NULL != ExecutionContext_p, E_INVALID_INPUT_PARAMETERS);

    if (Param_p == NULL) {
        Param_p = CreateInitProcessFileParam();
        VERIFY(NULL != Param_p, E_ALLOCATE_FAILED);
    }

    switch (Param_p->State) {
    case PROCESS_INIT:
#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
        ReturnValue = MP(Measurement_p, 0, START_COMMAND_TIME);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif
        ReturnValue = Handle_ProcessInit(Param_p, ExecutionContext_p);
        break;

    case PROCESS_PARSE_ZIP:
        ReturnValue = Handle_ProcessParseZip(Param_p);
        break;

    case PROCESS_OPEN_SIGNATURE:
        AddFlashInfoRecord("DONE!\n");
        AddFlashInfoRecord("Opening signature... ");
        ReturnValue = Handle_ProcessOpenSignature(Param_p);
        AddFlashInfoRecord("DONE!\n");
        AddFlashInfoRecord("Reading signature... ");
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_READ_SIGNATURE:
        ReturnValue = Handle_ProcessReadSignature(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_PARSE_ZIP_IN_ZIP:
        ReturnValue = Handle_ProcessParseZipInZip(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_OPEN_MANIFEST:
        AddFlashInfoRecord("Opening manifest... ");
        ReturnValue = Handle_ProcessOpenManifest(Param_p);
        AddFlashInfoRecord("DONE!\n");
        AddFlashInfoRecord("Reading manifest... ");
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_READ_MANIFEST:
        ReturnValue = Handle_ProcessReadManifest(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_OPEN_VERSIONS:
        AddFlashInfoRecord("DONE!\n");
        ReturnValue = Handle_ProcessOpenVersions(Param_p);
        break;

    case PROCESS_READ_VERSIONS:
        ReturnValue = Handle_ProcessReadVersions(Param_p);
        break;

    case PROCESS_OPEN_FLASHLAYOUT:
        ReturnValue = Handle_ProcessOpenFlashlayout(Param_p);
        break;

    case PROCESS_READ_FLASHLAYOUT:
        ReturnValue = Handle_ProcessReadFlashlayout(Param_p);
        break;

    case PROCESS_READ_TOC_AND_SUBTOC:
        ReturnValue = Handle_ProcessReadTOCandSUBTOC(Param_p);
        break;

#ifdef FAILSAFE_FLASHING
    case PROCESS_FILESAFE_UPDATE:
        ReturnValue = Handle_ProcessFailSafeUpdate(Param_p);
        break;
#endif

    case PROCESS_ALLOCATE_FILE:
        ReturnValue = Handle_ProcessAllocateFile(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_READ_FILE:
        ReturnValue = Handle_ProcessReadFile(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_WRITE_DATA:
        ReturnValue = Handle_ProcessWriteData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_WRITE_DATA_LOOP:
        ReturnValue = Handle_ProcessWriteDataLoop(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case PROCESS_WRITE_EMPTY_PARTITIONS:
        ReturnValue = Handle_ProcessWriteEmptyPartitions(Param_p);
        break;

    case PROCESS_WRITE_PARTITION_ENTRIES:
        ReturnValue = Handle_ProcessWritePartitionEntries(Param_p);
        break;

    case PROCESS_WRITE_BOOT_AREA:
        ReturnValue = Handle_ProcessWriteBootArea(Param_p);
        break;

    case PROCESS_END:
        // Fall through
    default:
        ReturnValue = Handle_ProcessFileFinalize(Param_p);
        AddFlashInfoRecord("\n<END OF FLASHING REPORT>");
        goto ErrorExit;
        break;
    }

    if (ReturnValue == E_SUCCESS) {
        return ReturnValue;
    }

ErrorExit:
    //save errorcode so it will not be overwritten by macros
    TempReturnValue = ReturnValue;

    (void)Do_Stop_Service(NUMBER_OF_SERVICES);

    BUFFER_FREE(Param_p->FileData_p);
    DESTROY_VECTOR(Param_p->VectorIndex);
    Param_p->VectorIndex = IO_INVALID_HANDLE;
    DESTROY_VECTOR(Param_p->NextVectorIndex);
    Param_p->NextVectorIndex = IO_INVALID_HANDLE;

    CLOSE_HANDLE(Param_p->ReadHandle);
    CLOSE_HANDLE(Param_p->ZipReadHandleIn);
    CLOSE_HANDLE(Param_p->ZipReadHandle);

    DESTROY_PARSER_CONTEXT(Param_p->ZipHandle_p);
    DESTROY_PARSER_CONTEXT(Param_p->ZipHandleIn_p);

    //restore errorcode when we call Done...
    ReturnValue = TempReturnValue;
    ReturnValue = Done_Flash_ProcessFile(Param_p->ExeContext_p->Received.SessionNr, ReturnValue);

    (void)Do_LoaderSecLib_Verify_Release(&(Param_p->VerifyContext));

    Param_p->TotalBytesWritten = 0;
    Param_p->ExeContext_p->Progress = 100;
    Param_p->ExeContext_p->Running = FALSE;
    Param_p->State = PROCESS_INIT;

    Param_p->SubTOC_List = Do_TOC_DestroyTOCList(Param_p->SubTOC_List);
    Param_p->TOC_List    = Do_TOC_DestroyTOCList(Param_p->TOC_List);
    BUFFER_FREE(Param_p->BootBuffer_p);

    BUFFER_FREE(Param_p);

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = MP(Measurement_p, 0, END_COMMAND_TIME);
    ReturnValue = MFINISH(Measurement_p);
    Do_Measurement_DestroyMeasurementData(&Measurement_p);
    //Do_Measurement_DestroySessionMeasurementData(&Measurement_p->MeasurementData_p->SessionMeasurement_p);
    //Do_Measurement_DestroyMeasurementData(&Measurement_p);
#endif

    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

static ProcessFileParameters_t *CreateInitProcessFileParam(void)
{
    ProcessFileParameters_t *Param_p = NULL;

    Param_p = (ProcessFileParameters_t *)malloc(sizeof(ProcessFileParameters_t));
    ASSERT(NULL != Param_p);

    Param_p->State                 = PROCESS_INIT;         // ProcessFile_t
#ifdef NONSIGNED_ARCHIVE_ENABLED
    Param_p->ArchiveType           = SIGNED_FLASH_ARCHIVE; //Regular signed flasharchive is default
#endif
    Param_p->ZipReadHandle         = IO_INVALID_HANDLE;    // IO_Handle_t
    Param_p->ZipReadHandleIn       = IO_INVALID_HANDLE;    // IO_Handle_t
    Param_p->ReadHandle            = IO_INVALID_HANDLE;    // IO_Handle_t
    Param_p->WriteHandle           = IO_INVALID_HANDLE;    // IO_Handle_t
    memset(&(Param_p->CommandParameters), 0x00, sizeof(FlashCommandPayLoad_t));
    Param_p->ZipHandle_p           = NULL;                 // void*
    Param_p->ZipHandleIn_p         = NULL;                 // void*
    Param_p->ExeContext_p          = NULL;                 // ExecutionContext_t
    memset(&(Param_p->VerifyContext), 0x00, sizeof(VerifyContext_t));
    Param_p->ReadSize              = 0;                    // uint64
    Param_p->NextReadSize          = 0;                    // uint64
    Param_p->FileSize              = 0;                    // uint64
    Param_p->FileData_p            = NULL;                  // char
    Param_p->ManifestParser_p      = NULL;                 // ManifestParser_t
    Param_p->NoOfImages            = 0;                    // uint32
    Param_p->VectorLength          = 0;                    // uint32
    Param_p->NoOfParsedImages      = 0;                    // uint32
    Param_p->VectorIndex           = IO_INVALID_HANDLE;    // uint32
    Param_p->NextVectorIndex       = IO_INVALID_HANDLE;    // uint32
    Param_p->ImageType             = DEVICE_UNKNOWN;       // SourceType_t
    Param_p->BytesReadPosition     = 0;                    // uint64
    Param_p->BytesWritePosition    = 0;                    // uint64
    Param_p->BytesWritten          = 0;                    // uint64
    Param_p->HashBlockSize         = 0;                    // uint64
    Param_p->TotalLength           = 0;                    // uint64
    Param_p->TotalBytesWritten     = 0;                    // uint64
    Param_p->DeviceNumber          = 0;                    // uint32
    Param_p->FlashingSameFile      = TRUE;                 // boolean
    Param_p->MaxVectorLength       = 0;                    // uint64
    memset(&(Param_p->Configuration), 0x00, sizeof(FPD_Config_t));
    memset(&(Param_p->BootAreaConfig), 0x00, sizeof(FPD_Config_t));
    Param_p->BootBuffer_p          = NULL;                 // uint32
    Param_p->BootBufferSize        = 0;                    // uint32
    Param_p->TheTransportIsBulk    = FALSE;                // boolean
#ifdef FAILSAFE_FLASHING
    Param_p->FS_ProgressData_p     = NULL;                 // FlashingProgressInfo_t
    Param_p->ReflashInProgress     = FALSE;                // boolean
    Param_p->FailSafeSize          = 0;                    // uint64
#endif

#ifdef MACH_TYPE_STN8500
    Param_p->Flashlayout_p         = NULL;                 // FlashLayoutParser_t
    Param_p->FlashlayoutImages     = 0;                    // uint32
    Param_p->TOC_List              = NULL;                 // TOC_List_t
    Param_p->SubTOC_List           = NULL;                 // TOC_List_t
    Param_p->SubTOC_List_ID[0]     = '\0';
    Param_p->SubTOC_Offset         = 0xFFFFFFFF;           // uint32
    Param_p->SubTOC_Size           = 0;                    // uint32
#endif

    return Param_p;
}


static ErrorCode_e Handle_ProcessInit(ProcessFileParameters_t *Param_p, void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    char *Type_p = NULL;
    char *SourcePath_p = NULL;
    void *Data_p = NULL;

    InitializeFlashInfo();
    AddFlashInfoRecord("<START OF FLASHING REPORT>\n\n");
    AddFlashInfoRecord("##### INITIALIZING FLASHING #####\n\n");
    AddFlashInfoRecord("HW DB Cutid 0x%08X AB Cutid 0x%08X\n\n", GetDBAsicChipID(), GetABAsicChipID());

    Param_p->ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    Param_p->ExeContext_p->Progress = 0;
    Data_p = Param_p->ExeContext_p->Received.Payload.Data_p;
    Param_p->CommandParameters.Length = get_uint64_le((void **)&Data_p);
    Type_p = skip_str((void **)&Data_p);
    Param_p->CommandParameters.FileType = ExtractType(Type_p);
    SourcePath_p = skip_str((void **)&Data_p);

    IDENTIFIER_NOT_USED(Type_p);
    IDENTIFIER_NOT_USED(SourcePath_p);

    VERIFY(0 != Param_p->CommandParameters.Length, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = E_SUCCESS;
    Param_p->State = PROCESS_PARSE_ZIP;

    AddFlashInfoRecord("Opening flash archive... ");
ErrorExit:
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessParseZip(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ZipState_t ZipState = ZIP_INIT_EOCDR;

    /* Open IO handle for flash archive */
    Param_p->ZipReadHandle = Do_IOW_BulkOpen(Param_p->CommandParameters.Session, BULK_RECEIVE, Param_p->CommandParameters.Length);
    VERIFY(IO_INVALID_HANDLE != Param_p->ZipReadHandle, E_IO_FAILED_TO_OPEN);

    do {
        ReturnValue = Do_Zip_CreateParserContext(Param_p->ZipReadHandle, &(Param_p->ZipHandle_p), &ZipState, ZIP_USE_IO_WRAP, NULL);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    } while (ZIP_FINISHED != ZipState);

    Param_p->State = PROCESS_OPEN_SIGNATURE;
    Param_p->ReadSize = 0;
    Param_p->NextReadSize = 0;
#ifdef CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = MP(Measurement_p, 0, END_PARSEZIP_TIME);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to create ZIP parser context", ReturnValue);
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessOpenSignature(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    // Archive type, detection code snippet
    // If this archive contains signature.sf then this is a Signed-flasharchive, otherwise non-signed flasharchive

    ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandle_p,
                                  SignatureString,
                                  strlen(SignatureString),
                                  &Param_p->ReadHandle);

    if (ReturnValue == E_SUCCESS) {
#ifdef NONSIGNED_ARCHIVE_ENABLED
        //this is a regular flashimage archive thus we should consider signature verification
        Param_p->ArchiveType = SIGNED_FLASH_ARCHIVE;
#endif
        Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
        VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);

        Param_p->FileData_p = (char *) malloc((uint32)Param_p->FileSize);
        ASSERT(NULL != Param_p->FileData_p);

        Param_p->State = PROCESS_READ_SIGNATURE;
    } else if (E_ZIP_PARSER_FILE_NOT_FOUND == ReturnValue) {

#ifdef NONSIGNED_ARCHIVE_ENABLED
        // This is a non-signed flashimage archive, so we are about to skip signature stuff and process this archive
        Param_p->ArchiveType = NON_SIGNED_FLASH_ARCHIVE;
        Param_p->State = PROCESS_OPEN_MANIFEST;
        Param_p->HashBlockSize = BULK_BUFFER_SIZE - (3 * ALIGNED_HEADER_LENGTH);
        Param_p->MaxVectorLength = Param_p->HashBlockSize * OPTIMAL_NR_OF_BUFFERS_PROCESS;

        ReturnValue = E_SUCCESS; //since this is expected behavior for detecting non-signed flasharchive, we set the exit status to E_SUCCESS
        A_(printf("flash_process_file_repeat.c(%d): Warning: Detected NON_SIGNED_FLASH_ARCHIVE\n", __LINE__);)
#else
        // If non-signed archive feature disabled, report error.
        goto ErrorExit;
#endif
    } else {
        A_(printf("flash_process_file_repeat.c(%d):Problem with opening the zip archive. Reason=%d \n", __LINE__, ReturnValue);)
        goto ErrorExit;
    }

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to open signature", ReturnValue);
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessReadSignature(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    Param_p->ReadSize = Do_IO_PositionedRead(Param_p->ReadHandle, Param_p->FileData_p, Param_p->FileSize, Param_p->FileSize, Param_p->ReadSize);
    VERIFY(IO_ERROR != Param_p->ReadSize, IO_ErrorCode);

    if (Param_p->ReadSize == Param_p->FileSize) {
        C_(printf("flash_process_file_repeat.c (%d): reading file:    0x%llx     0x%llx\n", __LINE__, Param_p->FileSize, Param_p->ReadSize);)

#ifndef DISABLE_SECURITY
        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Verify_Initialize(&(Param_p->VerifyContext));
        VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to initialize verification", ReturnValue);
        VERIFY((E_SUCCESS == ReturnValue), ReturnValue);

        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Verify_Header(&(Param_p->VerifyContext), (uint8 *)(Param_p->FileData_p), (uint32)Param_p->FileSize);
        VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to verify header", ReturnValue);
        VERIFY((E_SUCCESS == ReturnValue), ReturnValue);

        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Verify_Hashlist(&(Param_p->VerifyContext), (uint8 *)(Param_p->FileData_p), (uint32)Param_p->FileSize);
        VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to verify payload block", ReturnValue);
        VERIFY((E_SUCCESS == ReturnValue), ReturnValue);

        ReturnValue = (ErrorCode_e)Do_LoaderSecLib_Get_HashlistBlockSize(&(Param_p->VerifyContext), &(Param_p->HashBlockSize));
        VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to get hash block size", ReturnValue);
        VERIFY((E_SUCCESS == ReturnValue), ReturnValue);

        // Calculate max. number of bytes that can be transfered in one bulk session
        Param_p->MaxVectorLength = Param_p->HashBlockSize * OPTIMAL_NR_OF_BUFFERS_PROCESS;
#endif

#ifdef  FAILSAFE_FLASHING
        ReturnValue = ReflashInProgressCheck(Param_p);
        VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
#endif

        C_(printf("flash_process_file_repeat.c (%d): signature.sf verified\n", __LINE__);)
        Param_p->State = PROCESS_PARSE_ZIP_IN_ZIP;
        BUFFER_FREE(Param_p->FileData_p);

        CLOSE_HANDLE(Param_p->ReadHandle);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

ErrorExit:
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessParseZipInZip(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ZipState_t ZipState = ZIP_INIT_EOCDR;
    uint32 i = 0;

    //open inner zip
    for (i = 0; NULL != SupportedArchiveNames[i]; i++) {
        ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandle_p,
                                      SupportedArchiveNames[i],
                                      strlen(SupportedArchiveNames[i]),
                                      &(Param_p->ZipReadHandleIn));

        if (E_SUCCESS == ReturnValue) {
            break;
        } else if (E_ZIP_PARSER_FILE_NOT_FOUND == ReturnValue) {
            continue;
        } else {
            A_(printf("flash_process_file_repeat.c(%d): Inner zip cannot be opened!!!\n", __LINE__);)
            goto ErrorExit;
        }
    }

    do {
#ifdef DISABLE_SECURITY
        ReturnValue = Do_Zip_CreateParserContext(Param_p->ZipReadHandleIn,
                      &Param_p->ZipHandleIn_p,
                      &ZipState,
                      ZIP_USE_IO_WRAP,
                      &(Param_p->VerifyContext));
#else
        ReturnValue = Do_Zip_CreateParserContext(Param_p->ZipReadHandleIn,
                      &(Param_p->ZipHandleIn_p),
                      &ZipState,
                      ZIP_USE_IO_SEC_WRAP,
                      &(Param_p->VerifyContext));
#endif
        VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to create ZIP context for the content of the flash archive", ReturnValue);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    } while (ZIP_FINISHED != ZipState);

    /* Get information about total non redundant bytes in inner zip.*/

    ReturnValue = Do_Zip_GetTotalFilesSize(Param_p->ZipHandleIn_p, &Param_p->TotalLength);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    Param_p->State = PROCESS_OPEN_MANIFEST;

#ifdef CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = MP(Measurement_p, 0, END_PARSEZIPINZIP_TIME);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif

ErrorExit:
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessOpenManifest(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint64 VectorChunkSize = 0;
#ifdef NONSIGNED_ARCHIVE_ENABLED

    if (Param_p->ArchiveType == NON_SIGNED_FLASH_ARCHIVE) {
        //non-signed flash archives do not have any inner zip archives, therefore we bypass the inner zip file handler
        Param_p->ZipHandleIn_p = Param_p->ZipHandle_p;
        Param_p->ZipHandle_p = NULL;
    }

#endif
    ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandleIn_p, ManifestFileName, strlen(ManifestFileName), &(Param_p->ReadHandle));
    VERIFY(E_SUCCESS == ReturnValue, E_ZIP_FAILED_TO_OPEN_FILE);

    Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
    VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);

    //Subtract manifest file size from total length of inner zip
    Param_p->TotalLength -= Param_p->FileSize;
    Param_p->BytesReadPosition = 0;
    Param_p->ReadSize = 0;

#ifdef DISABLE_SECURITY
    Do_IO_CalculateVectorParameters(&Param_p->VectorLength, (uint32 * const)&VectorChunkSize, 0, Param_p->FileSize, TRUE);
#else
    Param_p->VectorLength = MIN(Param_p->FileSize,      Param_p->MaxVectorLength);
    VectorChunkSize       = MIN(Param_p->HashBlockSize, Param_p->VectorLength);
#endif

    Param_p->VectorIndex = Do_IO_CreateVector(Param_p->ReadHandle, Param_p->VectorLength, VectorChunkSize);
    VERIFY(IO_VECTOR_INVALID_HANDLE != Param_p->VectorIndex, E_VECTOR_CREATE_FAIL);
    VERIFY(E_INVALID_INPUT_PARAMETERS != Param_p->VectorIndex, E_VECTOR_CREATE_FAIL);
    VERIFY(E_VECTOR_CREATE_FAIL != Param_p->VectorIndex, E_VECTOR_CREATE_FAIL);

    Param_p->State = PROCESS_READ_MANIFEST;

    ReturnValue = E_SUCCESS;
ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to open Manifest", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e Handle_ProcessReadManifest(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 NrOfEntries = 0;

    Param_p->ReadSize = Do_IO_ScatteredPositionedRead(Param_p->ReadHandle,
                        Param_p->VectorIndex,
                        Param_p->VectorLength,
                        Param_p->BytesReadPosition);
    VERIFY(IO_ERROR != Param_p->ReadSize, IO_ErrorCode);

    if (Param_p->ReadSize == Param_p->FileSize) {
        Param_p->FileData_p = (char *)malloc((uint32)Param_p->FileSize);
        ASSERT(NULL != Param_p->FileData_p);

        ReturnValue = Do_IO_CopyVectorToBuffer((void *)Param_p->FileData_p, Param_p->VectorIndex, Param_p->FileSize);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = Do_Zip_GetTotalNrOfEntries(Param_p->ZipHandleIn_p, &NrOfEntries);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = Do_ManifestParser_Create(Param_p->FileData_p, (uint32)Param_p->FileSize, &Param_p->ManifestParser_p, &Param_p->NoOfImages, NrOfEntries);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#ifdef PRINT_LEVEL_C_
        PrintManifestEntries(Param_p->ManifestParser_p, Param_p->NoOfImages);
#endif
        BUFFER_FREE(Param_p->FileData_p);
        CLOSE_HANDLE(Param_p->ReadHandle);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        DESTROY_VECTOR(Param_p->VectorIndex);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        Param_p->State = PROCESS_OPEN_VERSIONS;
#ifdef CFG_ENABLE_MEASUREMENT_TOOL
        ReturnValue = MP(Measurement_p, 0, END_MANIFEST_TIME);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif

    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Error while reading Manifest", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e Handle_ProcessOpenVersions(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    BUFFER_FREE(Param_p->FileData_p);

    ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandleIn_p, VersionsFileName, strlen(VersionsFileName), &(Param_p->ReadHandle));

    if (E_ZIP_PARSER_FILE_NOT_FOUND == ReturnValue) {
        AddFlashInfoRecord("No versioning information.\n");
        Param_p->State = PROCESS_OPEN_FLASHLAYOUT;
        ReturnValue = E_SUCCESS;
        goto ErrorExit;
    }

    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    Param_p->ReadSize = 0;
    Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
    VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);

    if (Param_p->FileSize != 0) {
        AddFlashInfoRecord("Versioning information found.\n");

        Param_p->FileData_p = (char *)malloc((uint32)Param_p->FileSize);
        ASSERT(NULL != Param_p->FileData_p);
        Param_p->State = PROCESS_READ_VERSIONS;

    } else {
        CLOSE_HANDLE(Param_p->ReadHandle);
        Param_p->State = PROCESS_OPEN_FLASHLAYOUT;
    }

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Error while opening the versions file", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e Handle_ProcessReadVersions(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    Param_p->ReadSize += Do_IO_PositionedRead(Param_p->ReadHandle, Param_p->FileData_p, Param_p->FileSize, Param_p->FileSize - Param_p->ReadSize, Param_p->ReadSize);
    VERIFY(IO_ERROR != Param_p->ReadSize, IO_ErrorCode);

    if (Param_p->ReadSize == Param_p->FileSize) {
        CLOSE_HANDLE(Param_p->ReadHandle);

        Param_p->FileData_p[Param_p->FileSize - 1] = 0;
        Do_ReadVersionsInfo(Param_p->FileData_p, Param_p->FileSize);

        Do_PrintVersionsInfo();
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->State = PROCESS_OPEN_FLASHLAYOUT;
    }

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to read versions file", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e Handle_ProcessOpenFlashlayout(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    BUFFER_FREE(Param_p->FileData_p);

    ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandleIn_p, FlashlayoutFileName, strlen(FlashlayoutFileName), &(Param_p->ReadHandle));

    if (E_SUCCESS == ReturnValue) {
        Param_p->ReadSize = 0;
        Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
        VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);

        Param_p->FileData_p = (char *)malloc((uint32)Param_p->FileSize);
        ASSERT(NULL != Param_p->FileData_p);

        Param_p->State = PROCESS_READ_FLASHLAYOUT;
        AddFlashInfoRecord("\nFlash layout found.\n");
    } else {
        B_(printf("flash_process_file_repeat.c(%d): Flashlayout not found! (%d)\n", __LINE__, ReturnValue);)
        Param_p->State = PROCESS_READ_TOC_AND_SUBTOC;
        ReturnValue = E_SUCCESS;
    }

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to open Flash layout", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e Handle_ProcessReadFlashlayout(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    FlashLayoutParser_t *Flashlayout_Temp_p = NULL;
    uint32 Index = 0;

    Param_p->ReadSize += Do_IO_PositionedRead(Param_p->ReadHandle, Param_p->FileData_p, Param_p->FileSize, Param_p->FileSize - Param_p->ReadSize, Param_p->ReadSize);
    VERIFY(IO_ERROR != Param_p->ReadSize, IO_ErrorCode);

    if (Param_p->ReadSize == Param_p->FileSize) {
        ReturnValue = Do_FlashLayoutParser_Create(Param_p->FileData_p, (uint32)Param_p->FileSize, &(Param_p->Flashlayout_p), &Param_p->FlashlayoutImages);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

#ifdef PRINT_LEVEL_C_
        {
            FlashLayoutParser_t *Flashlayout_Temp_p = NULL;
            uint32 Index = 0;

            Flashlayout_Temp_p = Param_p->Flashlayout_p;
            printf("\nflash_process_file_repeat.c (%d): No. of flashlayout files: %d\n", __LINE__, Param_p->FlashlayoutImages);

            for (Index = 1; Index <= Param_p->FlashlayoutImages; Index++) {
                printf("\nFile No.:     %d\n", Index);
                printf("Path:         %s\n", Flashlayout_Temp_p->Path);
                printf("ContentType:  %s\n", Flashlayout_Temp_p->ContentType);
                printf("Description:  %s\n", Flashlayout_Temp_p->Description);
                printf("TOC_ID:       %s\n", Flashlayout_Temp_p->TOC_ID);
                printf("SubTOC_Of:    %s\n", Flashlayout_Temp_p->SubTOC_Of);
                printf("StartAddress: 0x%08X\n", (uint32)(Flashlayout_Temp_p->StartAddress));
                printf("EndAddress:   0x%08X\n", (uint32)(Flashlayout_Temp_p->EndAddress));
                printf("\n");
                Flashlayout_Temp_p++;
            }
        }
#endif //PRINT_LEVEL_C_

        CLOSE_HANDLE(Param_p->ReadHandle);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        /*
         * Search for SUB TOC Offset
         */
        VERIFY(Param_p->Flashlayout_p != NULL, E_FLASHLAYOUT_NULL);

        Flashlayout_Temp_p = Param_p->Flashlayout_p;

        for (Index = 1; Index <= Param_p->FlashlayoutImages; Index++) {
            if (Flashlayout_Temp_p->SubTOC_Of[0] != '\0') {
                C_(printf("flash_process_file_repeat.c(%d): SUBTOC ID %s found in flashlayout!\n", __LINE__, Flashlayout_Temp_p->SubTOC_Of);)
                strncpy(Param_p->SubTOC_List_ID, Flashlayout_Temp_p->SubTOC_Of, MAX_TOC_ID_SIZE);
                break;
            }

            Flashlayout_Temp_p++;
        }

        Flashlayout_Temp_p = Param_p->Flashlayout_p;

        if (Param_p->SubTOC_List_ID[0] != '\0') {
            for (Index = 1; Index <= Param_p->FlashlayoutImages; Index++) {
                if (0 == strncmp(Flashlayout_Temp_p->TOC_ID, Param_p->SubTOC_List_ID, TOC_ID_LENGTH)) {
                    C_(printf("flash_process_file_repeat.c(%d): %s TOC ID found in flashlayout! Offset 0x%08X\n", __LINE__, Param_p->SubTOC_List_ID, (uint32)Flashlayout_Temp_p->StartAddress);)
                    Param_p->SubTOC_Offset = (uint32)Flashlayout_Temp_p->StartAddress;
                    Param_p->SubTOC_Size   = (uint32)(Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress);
                    break;
                }

                Flashlayout_Temp_p++;
            }
        }

        /* Compare data from Manifest and from Flashlayout */
        ReturnValue = CompatibilityCheckManifestVsFlashlayout(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->State = PROCESS_READ_TOC_AND_SUBTOC;
    }

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Error while reading Flash layout", ReturnValue);
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessReadTOCandSUBTOC(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue     = E_GENERAL_FATAL_ERROR;
    uint8      *DataBuffer_p    = NULL;
    TOC_List_t *SubTOC_Node  = NULL;
    uint8 i, ExistingTOCDevice = 0xFF;
    uint8 available_flash_devices = Do_FPD_GetNrOfAvailableFlashDevices();

    /*
     * Read TOC
     */
    /*
     * Read boot area. Check all flash devices
     * Read second copy. First copy may contain MBR if device has been started.
     */
    for (i = 0; i < available_flash_devices; i++) {
        FPD_Config_t FPD_Config  = {0};
        uint32 RootTocNeededSize;

        ReturnValue = Do_FPD_GetInfo(i, &FPD_Config);
        VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

        RootTocNeededSize = GetSizeForRootToc(FPD_Config.PageSize);
        DataBuffer_p = (uint8 *)malloc(RootTocNeededSize);
        VERIFY(NULL != DataBuffer_p, E_GENERAL_FATAL_ERROR);

        ReturnValue = Do_BAM_Read(&FPD_Config, BOOT_IMAGE_SECOND_COPY_OFFSET, DataBuffer_p, RootTocNeededSize);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        /* Extract TOC from boot area */
        // This is the second copy. TOC must be reformatted to look like first!
        ReturnValue = Do_TOC_Create_RootList(&Param_p->TOC_List, DataBuffer_p, RootTocNeededSize, TOC_BAMCOPY_1);

        BUFFER_FREE(DataBuffer_p);

        if ((E_SUCCESS == ReturnValue) && (Param_p->TOC_List != NULL)) {
            C_(printf("flash_process_file_repeat.c(%d): TOC list found on flash!\n", __LINE__);)
            /* Read SUBTOC */
            /* Find SUB TOC location */

            SubTOC_Node = Do_TOC_GetNext_SubTocNode(Param_p->TOC_List);

            if (SubTOC_Node != NULL) {
                C_(printf("flash_process_file_repeat.c(%d): SUB TOC list found on flash!\n", __LINE__);)

                if (Param_p->SubTOC_Offset != 0xFFFFFFFF) {
                    VERIFY(Param_p->SubTOC_Offset == SubTOC_Node->entry.TOC_Offset, E_INVALID_SUBTOC_PARAMETERS);
                } else {
                    Param_p->SubTOC_Offset = SubTOC_Node->entry.TOC_Offset;
                    Param_p->SubTOC_Size   = SubTOC_Node->entry.TOC_Size;
                }

                if ((Param_p->SubTOC_Offset != 0xFFFFFFFF) && (Param_p->SubTOC_Size != 0)) {
                    DataBuffer_p = (uint8 *)malloc(Param_p->SubTOC_Size);
                    VERIFY(NULL != DataBuffer_p, E_GENERAL_FATAL_ERROR);

                    ReturnValue = Read_SubTOC_Data(Param_p->SubTOC_Offset, Param_p->SubTOC_Size, DataBuffer_p, Param_p->SubTOC_Size, &FPD_Config);
                    VERIFY(ReturnValue == E_SUCCESS, E_FAILED_TO_READ_SUBTOC);

                    /* Extract SUB TOC list and link it with root list */
                    if (!Do_TOC_Add_SubTocList(Param_p->TOC_List, DataBuffer_p, Param_p->SubTOC_Size, SubTOC_Node->entry.TOC_Id, Param_p->SubTOC_Offset)) {
                        A_(printf("flash_process_file_repeat.c(%d): Failed to add SubTOC list!\n");)
                        ReturnValue  = E_INVALID_SUBTOC_PARAMETERS;
                        goto ErrorExit;
                    }

                    Param_p->SubTOC_List = Do_TOC_GetNext_SubTocList(Param_p->TOC_List);
                }
            }

            ExistingTOCDevice = i;

            ReturnValue = Do_FPD_GetInfo(ExistingTOCDevice, &(Param_p->BootAreaConfig));
            VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

            break;
        } else {
            AddFlashInfoRecord("TOC list NOT found on flash%d!", i);
            B_(printf("flash_process_file_repeat.c(%d): TOC list NOT found on flash%d!\n", __LINE__, i);)
        }
    }

    /* Compare data from Manifest and from TOCs */
    ReturnValue = CompatibilityCheckManifestVsTOC(Param_p, ExistingTOCDevice);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue  = E_SUCCESS;

#ifdef FAILSAFE_FLASHING

    if (Param_p->ReflashInProgress == FALSE) {
        Param_p->State = PROCESS_ALLOCATE_FILE;
    } else {
        Param_p->State = PROCESS_FILESAFE_UPDATE;
    }

#else
    Param_p->State = PROCESS_ALLOCATE_FILE;
#endif

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to read TOC list", ReturnValue);
    BUFFER_FREE(DataBuffer_p);

    return ReturnValue;
}


#ifdef FAILSAFE_FLASHING
static ErrorCode_e Handle_ProcessFailSafeUpdate(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 i = 0;
    uint8 *File_p = NULL;
    uint32 OffsetInBootBuffer = 0;
    uint32 VectorChunkSize = 0;
    FPD_Config_t FPDConfig;
    FlashLayoutRevision_t FlashLayoutRevision;

    C_(printf("flash_process_file_repeat.c(%d): FilesafeUpdate \n", __LINE__);)

    if (Param_p->BootBuffer_p == NULL) {
        ReturnValue = ReadOldBootArea(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    while (i < Param_p->FS_ProgressData_p->ParsedImageNum) {
        if ((0 != strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeFlashLayoutString)) &&
                (0 != strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeEmptyPartitionString))) {

            ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandleIn_p, Param_p->ManifestParser_p->Name, strlen(Param_p->ManifestParser_p->Name), &(Param_p->ReadHandle));
            VERIFY(E_SUCCESS == ReturnValue, E_FAILED_TO_START_BULK_SESSION);

            Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
            VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);
            VERIFY(0 != Param_p->FileSize, E_EMPTY_FILE_IN_ARCHIVE);
            C_(printf("flash_process_file_repeat.c(%d): FileName = %s \n", __LINE__, Param_p->ManifestParser_p->Name);)

            Param_p->ManifestParser_p->PartitionContentSize = Param_p->FileSize;

            ReturnValue = Update_TOC_List(Param_p);
            VERIFY(ReturnValue == E_SUCCESS, ReturnValue);

            // now update the BootBuffer
            if (GetImageType(Param_p->ManifestParser_p) == DEVICE_BAM) {

                Param_p->BytesWritePosition = Param_p->ManifestParser_p->TargetStartAddress;

                File_p = (uint8 *)malloc(Param_p->FileSize);
                ASSERT(NULL != File_p);

                /* Get flash configuration for current entry in manifest */
                ReturnValue = Do_FPD_GetInfo(Param_p->ManifestParser_p->TargetId, &FPDConfig);
                VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

                do {
                    Param_p->ReadSize = Do_IO_PositionedRead(Param_p->ReadHandle, File_p, Param_p->FileSize, Param_p->FileSize, 0);
                } while (Param_p->ReadSize == 0);

                VERIFY(Param_p->ReadSize != IO_ERROR, E_IO_FAILED_TO_READ);
                VERIFY(Param_p->ReadSize == Param_p->FileSize, E_GENERAL_FATAL_ERROR);

                Do_FlashLayoutParser_GetRevision(&FlashLayoutRevision);

                if ((FLASHLAYOUT_3_0_REVISION_MAJOR == FlashLayoutRevision.RevNrMajor) && (FLASHLAYOUT_3_0_REVISION_MINOR == FlashLayoutRevision.RevNrMinor)) {
                    VERIFY(Param_p->BytesWritePosition >= Param_p->SubTOC_Offset, E_FILE_OUT_OF_BOOT_AREA);
                    OffsetInBootBuffer = Param_p->BytesWritePosition - Param_p->SubTOC_Offset;
                } else {
                    OffsetInBootBuffer = Param_p->BytesWritePosition;
                }

                memcpy(((uint8 *)Param_p->BootBuffer_p) + OffsetInBootBuffer, File_p, Param_p->FileSize);
                BUFFER_FREE(File_p);
            }

            Param_p->ManifestParser_p++;
            i++;
            CLOSE_HANDLE(Param_p->ReadHandle);
            VERIFY(IO_VECTOR_INVALID_HANDLE == Param_p->ReadHandle, ReturnValue);

        } else {
            Param_p->ManifestParser_p++;
            C_(printf("flash_process_file_repeat.c(%d): Other type \n", __LINE__);)
        }

        Param_p->NoOfParsedImages++;
        C_(printf("flash_process_file_repeat.c(%d): NoOfParsed_Image= 0x%x\n", __LINE__, Param_p->NoOfParsedImages);)
    }

    Param_p->NoOfParsedImages--;
    Param_p->ManifestParser_p--; //adjust manifest on non-finished image

    Param_p->State = PROCESS_ALLOCATE_FILE;

ErrorExit:
    return ReturnValue;
}
#endif


static ErrorCode_e Handle_ProcessAllocateFile(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPDConfig;
    uint32 VectorChunkSize = 0;

    /* Open File*/
    Param_p->TotalLength = 0;

    while (Param_p->NoOfParsedImages < Param_p->NoOfImages) {
        if (!IsContentTypeSkippable(Param_p->ManifestParser_p->ContentType)) {
            if (0 != strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeEmptyPartitionString)) {
                ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandleIn_p, Param_p->ManifestParser_p->Name, strlen(Param_p->ManifestParser_p->Name), &(Param_p->ReadHandle));
                VERIFY(E_SUCCESS == ReturnValue, E_FAILED_TO_START_BULK_SESSION);

                Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
                VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);

                Param_p->ManifestParser_p->PartitionContentSize = Param_p->FileSize;
            } else {
                Param_p->FileSize = Param_p->ManifestParser_p->PartitionContentSize;
            }

            VERIFY(0 != Param_p->FileSize, E_EMPTY_FILE_IN_ARCHIVE);

            Param_p->TotalLength += Param_p->FileSize;

            CLOSE_HANDLE(Param_p->ReadHandle);
            VERIFY(IO_VECTOR_INVALID_HANDLE == Param_p->ReadHandle, ReturnValue);
        }

        Param_p->ManifestParser_p++;
        Param_p->NoOfParsedImages++;
    }

    Param_p->ManifestParser_p = Param_p->ManifestParser_p - Param_p->NoOfParsedImages;

#ifdef FAILSAFE_FLASHING

    if (Param_p->ReflashInProgress == TRUE) {
        Param_p->ManifestParser_p +=  Param_p->FS_ProgressData_p->ParsedImageNum;
        Param_p->NoOfParsedImages = Param_p->FS_ProgressData_p->ParsedImageNum;
    } else {
#endif
        Param_p->NoOfParsedImages = 0;
        ReturnValue = PartitionsOverlapAndSpaceCheck(Param_p->ManifestParser_p, Param_p->NoOfImages, Param_p->Flashlayout_p, Param_p->FlashlayoutImages);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#ifdef FAILSAFE_FLASHING
    }

#endif

    while ((0 == strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeFlashLayoutString))    ||
            (0 == strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeEmptyPartitionString))
          ) {
        C_(printf("--- Skipping manifest entry ---\n");)
        Param_p->NoOfParsedImages++;

        if (Param_p->NoOfParsedImages >= Param_p->NoOfImages) {
            Param_p->State              = PROCESS_WRITE_EMPTY_PARTITIONS;
            Param_p->ManifestParser_p   = Param_p->ManifestParser_p - (Param_p->NoOfParsedImages - 1);
            Param_p->NoOfParsedImages   = 0;
            Param_p->BytesWritePosition = Param_p->ManifestParser_p->TargetStartAddress;
            Param_p->FileSize           = Param_p->ManifestParser_p->PartitionContentSize;

            goto ErrorExit;
        }

        Param_p->ManifestParser_p++;
    }

    if (Param_p->NoOfParsedImages < Param_p->NoOfImages) {
        ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandleIn_p, Param_p->ManifestParser_p->Name, strlen(Param_p->ManifestParser_p->Name), &(Param_p->ReadHandle));
        VERIFY(E_SUCCESS == ReturnValue, E_FAILED_TO_START_BULK_SESSION);

        Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
        VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);

        Param_p->BytesWritePosition = Param_p->ManifestParser_p->TargetStartAddress;

#ifdef MACH_TYPE_STN8500
        ReturnValue = Update_TOC_List(Param_p);
        VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
#endif

    } else {
        goto ErrorExit;
    }

    /* Allocate File */

    ReturnValue = Do_FPD_GetInfo(Param_p->ManifestParser_p->TargetId, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

#ifdef FAILSAFE_FLASHING
    Param_p->FS_ProgressData_p->ParsedImageNum = Param_p->NoOfParsedImages;

    if (Param_p->ReflashInProgress == TRUE) {

#ifndef DISABLE_SECURITY
        Do_IOW_SecureWrap_DestroyCachedData();
        Do_IOW_SecureWrap_UpdateInternalParameters(Param_p->FS_ProgressData_p->FileOffset);
#endif
        Param_p->BytesReadPosition = Param_p->FS_ProgressData_p->FileOffset;
        Param_p->FileSize = Param_p->FileSize - Param_p->FS_ProgressData_p->FileOffset;
        Param_p->BytesWritePosition += Param_p->FS_ProgressData_p->FileOffset;
        Param_p->ReflashInProgress = FALSE; //not needed anymore
    } else {
#endif
        Param_p->BytesReadPosition = 0;
#ifdef FAILSAFE_FLASHING
    }

#endif

#ifdef DISABLE_SECURITY
    Do_IO_CalculateVectorParameters(&Param_p->VectorLength, &VectorChunkSize, FPDConfig.PageSize, Param_p->FileSize, TRUE);
#else
    Param_p->VectorLength = MIN(Param_p->FileSize,      Param_p->MaxVectorLength);
    VectorChunkSize       = MIN(Param_p->HashBlockSize, Param_p->VectorLength);
#endif

    Param_p->VectorIndex = Do_IO_CreateVector(Param_p->ReadHandle, Param_p->VectorLength, VectorChunkSize);
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to allocate vector for file", ReturnValue);
    VERIFY(IO_VECTOR_INVALID_HANDLE != Param_p->VectorIndex, E_VECTOR_CREATE_FAIL);
    VERIFY(E_INVALID_INPUT_PARAMETERS != Param_p->VectorIndex, E_VECTOR_CREATE_FAIL);
    VERIFY(E_VECTOR_CREATE_FAIL != Param_p->VectorIndex, E_VECTOR_CREATE_FAIL);

    Param_p->State = PROCESS_READ_FILE;
    Param_p->ReadSize = 0;

ErrorExit:
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessReadFile(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue               = E_SUCCESS;
    ErrorCode_e TempResult                = E_GENERAL_FATAL_ERROR;
    uint32 PageSize;
    uint32 VectorChunkSize = 0;

    // Get flash configuration for current entry in manifest
    ReturnValue = Do_FPD_GetInfo(Param_p->ManifestParser_p->TargetId, &(Param_p->Configuration));
    VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);
    PageSize = Param_p->Configuration.PageSize;

    Param_p->ReadSize = Do_IO_ScatteredPositionedRead(Param_p->ReadHandle, Param_p->VectorIndex, Param_p->VectorLength, Param_p->BytesReadPosition);
    VERIFY(IO_ERROR != Param_p->ReadSize, IO_ErrorCode);

    if (0 < Param_p->ReadSize) {
        Param_p->BytesReadPosition += Param_p->ReadSize;

        if (Param_p->ReadSize > Param_p->FileSize) {
            Param_p->FileSize = 0;
        } else {
            Param_p->FileSize -= Param_p->ReadSize;
        }

        Param_p->State = PROCESS_WRITE_DATA;
        Param_p->ImageType = GetImageType(Param_p->ManifestParser_p);
        Param_p->DeviceNumber = GetDeviceNumber(Param_p->ManifestParser_p->TargetType);

        if (Param_p->FileSize == 0) {
            Param_p->FlashingSameFile = FALSE; /* moved to the next file */

            do {
                Param_p->NoOfParsedImages++;

                if (Param_p->NoOfParsedImages >= Param_p->NoOfImages) {
                    break;
                }

                Param_p->ManifestParser_p++;

            } while ((0 == strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeFlashLayoutString))    ||
                     (0 == strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeEmptyPartitionString))
                    );

            if (Param_p->NoOfParsedImages < Param_p->NoOfImages) {
                Param_p->BytesReadPosition = 0;
            } else {
                if (Param_p->ReadHandle != IO_INVALID_HANDLE) {
                    TempResult = (ErrorCode_e)Do_IO_Close(Param_p->ReadHandle);
                    Param_p->ReadHandle = IO_INVALID_HANDLE;

                    VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to close file handle", ReturnValue);
                    VERIFY(E_SUCCESS == TempResult, E_IO_FAILED_TO_CLOSE);
                }

                //break;
                return E_SUCCESS;
            }
        }

        if (Param_p->FileSize == 0) {
            FPD_Config_t FPDConfig;

            TempResult = (ErrorCode_e)Do_IO_Close(Param_p->ReadHandle);
            Param_p->ReadHandle = IO_INVALID_HANDLE;

            VerifyAndRecord(E_SUCCESS == ReturnValue, "Failed to close file handle", ReturnValue);
            VERIFY(E_SUCCESS == TempResult, E_IO_FAILED_TO_CLOSE);

            ReturnValue = Do_Zip_OpenFile(Param_p->ZipHandleIn_p, Param_p->ManifestParser_p->Name, strlen(Param_p->ManifestParser_p->Name), &Param_p->ReadHandle);
            VERIFY(E_SUCCESS == ReturnValue, E_FAILED_TO_START_BULK_SESSION);

            Param_p->FileSize = Do_IO_GetLength(Param_p->ReadHandle);
            VERIFY(IO_ERROR != Param_p->FileSize, E_IO_FAILED_TO_GET_LENGTH);

            // Get flash configuration for next entry in manifest
            ReturnValue = Do_FPD_GetInfo(Param_p->ManifestParser_p->TargetId, &FPDConfig);
            VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);
            PageSize = FPDConfig.PageSize;

#ifdef MACH_TYPE_STN8500
            ReturnValue = Update_TOC_List(Param_p);
            VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
#endif
        }

#ifdef DISABLE_SECURITY
        Do_IO_CalculateVectorParameters(&Param_p->VectorLength, &VectorChunkSize, PageSize, Param_p->FileSize, TRUE);
#else
        Param_p->VectorLength = MIN(Param_p->FileSize,      Param_p->MaxVectorLength);
        VectorChunkSize    = MIN(Param_p->HashBlockSize, Param_p->VectorLength);
#endif

        Param_p->NextVectorIndex = Do_IO_CreateVector(Param_p->ReadHandle, Param_p->VectorLength, VectorChunkSize);
        VERIFY(IO_VECTOR_INVALID_HANDLE != Param_p->NextVectorIndex, E_VECTOR_CREATE_FAIL);
        VERIFY(E_INVALID_INPUT_PARAMETERS != Param_p->NextVectorIndex, E_VECTOR_CREATE_FAIL);
        VERIFY(E_VECTOR_CREATE_FAIL != Param_p->NextVectorIndex, E_VECTOR_CREATE_FAIL);

        Param_p->NextReadSize = Do_IO_ScatteredPositionedRead(Param_p->ReadHandle, Param_p->NextVectorIndex, Param_p->VectorLength, Param_p->BytesReadPosition);
        VERIFY(IO_ERROR != Param_p->NextReadSize, IO_ErrorCode);
    }

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Error while reading file", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e Handle_ProcessWriteData(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    FlashLayoutRevision_t FlashLayoutRevision;
    uint32 OffsetInBootBuffer = 0;

    switch (Param_p->ImageType) {
    case DEVICE_BDM:
        C_(printf("flash_process_file_repeat.c(%d): DEVICE_BDM !\n", __LINE__);)
        Param_p->WriteHandle = Do_IOW_BDMOpen(&(Param_p->Configuration));
        break;

    case DEVICE_BAM:
        C_(printf("flash_process_file_repeat.c(%d): DEVICE_BAM !\n", __LINE__);)

        if (Param_p->BootBuffer_p == NULL) {
            ReturnValue = ReadOldBootArea(Param_p);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        }

        Do_FlashLayoutParser_GetRevision(&FlashLayoutRevision);

        if ((FLASHLAYOUT_3_0_REVISION_MAJOR == FlashLayoutRevision.RevNrMajor) && (FLASHLAYOUT_3_0_REVISION_MINOR == FlashLayoutRevision.RevNrMinor)) {
            VERIFY(Param_p->BytesWritePosition >= Param_p->SubTOC_Offset, E_FILE_OUT_OF_BOOT_AREA);
            OffsetInBootBuffer = Param_p->BytesWritePosition - Param_p->SubTOC_Offset;
        } else {
            OffsetInBootBuffer = Param_p->BytesWritePosition;
        }

        ReturnValue = Do_IO_CopyVectorToBuffer(((uint8 *)Param_p->BootBuffer_p) + OffsetInBootBuffer, Param_p->VectorIndex, (uint32)Param_p->ReadSize);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->BytesWritten = Param_p->ReadSize;
        Param_p->TotalBytesWritten += Param_p->BytesWritten;
        Param_p->ExeContext_p->Progress = (uint32)(Param_p->TotalBytesWritten * 100 / Param_p->TotalLength);
        break;

    default:
        /* make the default actions here */
        A_(printf("flash_process_file_repeat.c(%d): Unknown ImageType! (Type=%d)\n", __LINE__, Param_p->ImageType);)
        AddFlashInfoRecord("\nUnknown image type\n");
        ReturnValue = E_GENERAL_FATAL_ERROR;
        Param_p->State = PROCESS_INIT;
        Param_p->ExeContext_p->Running = FALSE;
        goto ErrorExit;
    }

    Param_p->State = PROCESS_WRITE_DATA_LOOP;

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Error while writing data", ReturnValue);
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessWriteDataLoop(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ErrorCode_e TempResult  = E_GENERAL_FATAL_ERROR;

    if (IO_INVALID_HANDLE != Param_p->WriteHandle) {
        Param_p->BytesWritten = Do_IO_GatheredPositionedWrite(Param_p->WriteHandle, Param_p->VectorIndex, Param_p->ReadSize, Param_p->BytesWritePosition);
        VERIFY(IO_ERROR != Param_p->BytesWritten, IO_ErrorCode);
    } else {
        if (0 == Param_p->BytesWritten) {
            Param_p->BytesWritten = 1;
        }
    }

    if ((0 < Param_p->BytesWritten) || (Param_p->ReadSize == Param_p->BytesWritten)) {
        Param_p->TotalBytesWritten += Param_p->ReadSize;

#ifdef FAILSAFE_FLASHING
        Param_p->FS_ProgressData_p->FileOffset = Param_p->BytesReadPosition;  // when write is done, update offset
        Param_p->FailSafeSize += Param_p->BytesWritten;  //increase the failsafe written data size
        C_(printf("flash_process_file_repeat.c(%d): FailSafeSize = 0x%x \n", __LINE__, Param_p->FailSafeSize);)
        Param_p->FS_ProgressData_p->ParsedImageNum = Param_p->NoOfParsedImages;

        if (Param_p->FailSafeSize > FAILSAFE_AMOUNT) {
            /* store the image info into progress structure, and reset amount */
            Param_p->FailSafeSize = 0;

            memset(Param_p->FS_ProgressData_p->ImageName, 0x00, MAX_FILENAME_SIZE); //keep the current image name
            strcpy(Param_p->FS_ProgressData_p->ImageName, Param_p->ManifestParser_p->Name);

            ReturnValue = UpdateFlashProgressInfo(Param_p);
            VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
        }

#endif

        Param_p->ExeContext_p->Progress = (uint32)(Param_p->TotalBytesWritten * 100 / Param_p->TotalLength);
        Param_p->BytesWritePosition += Param_p->BytesWritten;

        ReturnValue = (ErrorCode_e)Do_IO_DestroyVector(Param_p->VectorIndex);
        Param_p->VectorIndex = IO_VECTOR_INVALID_HANDLE;
        VERIFY(E_SUCCESS == ReturnValue, E_VECTOR_DESTROY_FAIL);

        if (Param_p->WriteHandle != IO_INVALID_HANDLE) {
            TempResult = (ErrorCode_e)Do_IO_Close(Param_p->WriteHandle);
            Param_p->WriteHandle = IO_INVALID_HANDLE;
            VERIFY(E_SUCCESS == TempResult, E_IO_FAILED_TO_CLOSE);
        }

        Param_p->VectorIndex = Param_p->NextVectorIndex;
        Param_p->NextVectorIndex = IO_INVALID_HANDLE;

        Param_p->ReadSize = Param_p->NextReadSize;
        Param_p->State = PROCESS_READ_FILE;

        /* common part for all writes */
        if (FALSE == Param_p->FlashingSameFile) {
            Param_p->FlashingSameFile = TRUE; /* still on same file */
            Param_p->BytesWritePosition = Param_p->ManifestParser_p->TargetStartAddress;

            if (Param_p->NoOfParsedImages >= Param_p->NoOfImages) {
                Param_p->State = PROCESS_WRITE_EMPTY_PARTITIONS;
                Param_p->ManifestParser_p   = Param_p->ManifestParser_p - (Param_p->NoOfParsedImages - 1);
                Param_p->NoOfParsedImages   = 0;

                Param_p->FileSize           = Param_p->ManifestParser_p->PartitionContentSize;
            }
        }
    }

ErrorExit:

    return ReturnValue;
}


static ErrorCode_e Handle_ProcessWriteEmptyPartitions(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue    = E_GENERAL_FATAL_ERROR;
    TOC_List_t *Temp_TOC_Entry = NULL;
    uint32 VectorChunkSize = 0;

    while (Param_p->NoOfParsedImages < Param_p->NoOfImages) {
        if (0 == strcmp(Param_p->ManifestParser_p->ContentType, ContentTypeEmptyPartitionString)) {
            C_(printf("flash_process_file_repeat.c(%d): Empty partition found!\n", __LINE__);)

            // Get flash configuration for current entry in manifest
            ReturnValue = Do_FPD_GetInfo(Param_p->ManifestParser_p->TargetId, &(Param_p->Configuration));
            VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

            /*
             * Check if partition is present in TOC list.
             * If it is not than flash it, else skip it.
             */
            Temp_TOC_Entry = Do_TOC_LocateEntryByNodeId(Param_p->TOC_List, Param_p->ManifestParser_p->TocID);

            if (Temp_TOC_Entry == NULL) {
                boolean      Use_Erase_Function = FALSE;

                if (Param_p->ManifestParser_p->Fill.Set) {
                    if (((Param_p->Configuration.ErasedMemContent == 0x00) && (Param_p->ManifestParser_p->Fill.Pattern == 0x00000000)) ||
                            ((Param_p->Configuration.ErasedMemContent == 0xFF) && (Param_p->ManifestParser_p->Fill.Pattern == 0xFFFFFFFF))) {
                        /* If erased MMC content is same as requested fill data, use erase function */
                        Use_Erase_Function = TRUE;
                    }
                } else {
                    /* Always use erase function for initialized area */
                    Use_Erase_Function = TRUE;
                }

                if (Use_Erase_Function) {
                    ReturnValue = Do_EraseAreaWithDriver(Param_p->ManifestParser_p->TargetId, Param_p->BytesWritePosition, Param_p->FileSize);
                    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

                    Param_p->TotalBytesWritten     += Param_p->FileSize;
                    Param_p->ExeContext_p->Progress = (uint32)(Param_p->TotalBytesWritten * 100 / Param_p->TotalLength);

                    ReturnValue = Update_TOC_List(Param_p);
                    VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
                } else {
                    uint32 *Fill_Buffer_p  = NULL;
                    uint64  Written_Bytes  = 0;
                    uint64  Bytes_To_Write = 0;

                    Fill_Buffer_p = (uint32 *)memalign(sizeof(uint32), FILL_BUFFER_SIZE);
                    ASSERT(Fill_Buffer_p != NULL);

                    for (Bytes_To_Write = 0; Bytes_To_Write < (FILL_BUFFER_SIZE / sizeof(uint32)); Bytes_To_Write++) {
                        *(Fill_Buffer_p + Bytes_To_Write) =  Param_p->ManifestParser_p->Fill.Pattern;
                    }

                    /* To allow parallel execution, if partition size is large, don't write everything at once */
                    Do_IO_CalculateVectorParameters(&Param_p->VectorLength, &VectorChunkSize, Param_p->Configuration.PageSize, Param_p->FileSize, TRUE);

                    Param_p->DeviceNumber    = GetDeviceNumber(Param_p->ManifestParser_p->TargetType);
                    Param_p->WriteHandle = Do_IOW_BDMOpen(&(Param_p->Configuration));

                    Param_p->BytesWritten = 0;
                    Bytes_To_Write      = 0;

                    do {
                        Bytes_To_Write = MIN(FILL_BUFFER_SIZE, Param_p->VectorLength - Param_p->BytesWritten);
                        Written_Bytes = Do_IO_PositionedWrite(Param_p->WriteHandle, Fill_Buffer_p, FILL_BUFFER_SIZE, Bytes_To_Write, Param_p->BytesWritePosition + Param_p->BytesWritten);
                        VERIFY(IO_ERROR != Written_Bytes, E_IO_FAILED_TO_WRITE);

                        Param_p->BytesWritten += Written_Bytes;
                    } while (Param_p->BytesWritten < Param_p->VectorLength);

                    Param_p->TotalBytesWritten     += Param_p->VectorLength;
                    Param_p->ExeContext_p->Progress = (uint32)(Param_p->TotalBytesWritten * 100 / Param_p->TotalLength);

                    BUFFER_FREE(Fill_Buffer_p);
                    CLOSE_HANDLE(Param_p->WriteHandle);

                    if (Param_p->FileSize > Param_p->VectorLength) {
                        Param_p->FileSize           -= Param_p->VectorLength;
                        Param_p->BytesWritePosition += Param_p->VectorLength;
                        break;
                    } else {
                        ReturnValue = Update_TOC_List(Param_p);
                        VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
                    }
                }
            } else {
                C_(printf("flash_process_file_repeat.c(%d): Partition is already on flash!\n", __LINE__);)
            }
        }

        Param_p->NoOfParsedImages++;

        if (Param_p->NoOfParsedImages >= Param_p->NoOfImages) {
            C_(printf("PROCESS_WRITE_PARTITION_ENTRIES\n");)
            Param_p->State = PROCESS_WRITE_PARTITION_ENTRIES;
            break;
        }

        Param_p->ManifestParser_p++;
        Param_p->BytesWritePosition = Param_p->ManifestParser_p->TargetStartAddress;
        Param_p->FileSize           = Param_p->ManifestParser_p->PartitionContentSize;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}


static ErrorCode_e Handle_ProcessWritePartitionEntries(ProcessFileParameters_t *params_p)
{
    uint32 index1;
    uint32 index2;
    FlashLayoutParser_t *flash_layout_p;

    params_p->ManifestParser_p = params_p->ManifestParser_p - (params_p->NoOfParsedImages - 1);

    for (index1 = 0;
            index1 < params_p->NoOfImages;
            index1++, params_p->ManifestParser_p++) {

        for (index2 = 0, flash_layout_p = params_p->Flashlayout_p;
                index2 < params_p->FlashlayoutImages;
                index2++, flash_layout_p++) {

            if (0 == strcmp(params_p->ManifestParser_p->TocID, flash_layout_p->TOC_ID) &&
                    (0 == strcmp(flash_layout_p->ContentType, FlashLayoutPartitionEntryString1) ||
                     0 == strcmp(flash_layout_p->ContentType, FlashLayoutPartitionEntryString2))) {
                if (params_p->SubTOC_List_ID[0] == '\0') {
                    AddFlashInfoRecord("\nNo SubTOC available, cannot write partition entries...");
                    A_(printf("No SubTOC available, cannot write partition entries...\n");)
                    return E_GENERAL_FATAL_ERROR;
                } else {
                    strncpy(params_p->ManifestParser_p->SubTOC_Of, params_p->SubTOC_List_ID, MAX_TOC_ID_SIZE - 1);
                    params_p->ManifestParser_p->SubTOC_Of[MAX_TOC_ID_SIZE - 1] = '\0';
                    params_p->ManifestParser_p->PartitionContentSize = flash_layout_p->EndAddress - flash_layout_p->StartAddress;
                    params_p->ManifestParser_p->TocFlags = TOC_FLAG_PARTITION_ENTRY;
                    (void)Update_TOC_List(params_p);
                    break;
                }
            }
        }
    }

    C_(printf("PROCESS_WRITE_BOOT_AREA\n");)
    params_p->State = PROCESS_WRITE_BOOT_AREA;

    return E_SUCCESS;
}

static ErrorCode_e Handle_ProcessWriteBootArea(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FlashLayoutRevision_t FlashLayoutRevision;

    Do_FlashLayoutParser_GetRevision(&FlashLayoutRevision);

    if ((FLASHLAYOUT_3_0_REVISION_MAJOR == FlashLayoutRevision.RevNrMajor) && (FLASHLAYOUT_3_0_REVISION_MINOR == FlashLayoutRevision.RevNrMinor)) {
        ReturnValue = Handle_ProcessWriteBootArea_Layout_3_0(Param_p);
    } else {
        ReturnValue = Handle_ProcessWriteBootArea_Layout_2_0(Param_p);
    }

    VerifyAndRecord(E_SUCCESS == ReturnValue, "Error writing data to flash", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e Handle_ProcessWriteBootArea_Layout_2_0(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8      *TOC_p          = NULL;
    uint8      *SubTOC_p       = NULL;
    uint32      TOC_Size       = 0;
    uint32      SubTOC_Size    = 0;
    uint64      IO_RetVal      = IO_ERROR;
    boolean     UpdateBootArea = FALSE;
    IO_Handle_t IO_Handle      = IO_INVALID_HANDLE;
    TOC_List_t *It = NULL;

    // Write bootimage last, to be sure to have collected all TOC content
    // Only write complete bootimage. BAM handles duplication of blocks
    C_(printf("\nflash_process_file_repeat.c(%d): Write TOC, SUB TOC and BootArea!\n", __LINE__);)

    AddFlashInfoRecord("\nWriting TOC, SubTOC and Boot Image...");

    if (Param_p->BootBuffer_p != NULL) {
        UpdateBootArea = TRUE;
    }

    // Convert TOC to data vector
    if (Param_p->TOC_List != NULL) {
        TOC_p = Do_TOC_CreateTOCDataFromList(Param_p->TOC_List, &TOC_Size);
        VERIFY(NULL != TOC_p, E_GENERAL_FATAL_ERROR);
        VERIFY(TOC_Size <= (MAX_ROOT_TOC_ENTRIES * sizeof(TOC_Entry_t)), E_FLASH_ARCHIVE_MISMATCH); /* TODO: Add more descriptive error */

        if (Param_p->SubTOC_List == NULL) {
            Param_p->SubTOC_List = Do_TOC_GetNext_SubTocList(Param_p->TOC_List);
        }
    }

    if (TOC_p != NULL) {
        if (Param_p->BootBuffer_p == NULL) {
            ReturnValue = ReadOldBootArea(Param_p);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        }

        UpdateBootArea = TRUE;
        memcpy(Param_p->BootBuffer_p, TOC_p, TOC_Size);
    }

    if (Param_p->SubTOC_List != NULL) {
        SubTOC_p = Do_TOC_CreateTOCDataFromList(Param_p->SubTOC_List, &SubTOC_Size);
        VERIFY(NULL != SubTOC_p, E_GENERAL_FATAL_ERROR);
    } else {
        A_(printf("\nflash_process_file_repeat.c(%d): SUB TOC list is NULL!\n", __LINE__);)
    }

    if (SubTOC_p != NULL) {
        if (Param_p->SubTOC_Offset < BAM_BOOT_IMAGE_COPY_SIZE) {
            C_(printf("\nflash_process_file_repeat.c(%d): SubTOC list in BootArea!\n", __LINE__);)

            if (Param_p->BootBuffer_p == NULL) {
                ReturnValue = ReadOldBootArea(Param_p);
                VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            }

            UpdateBootArea = TRUE;
            memcpy(Param_p->BootBuffer_p + Param_p->SubTOC_Offset, SubTOC_p, SubTOC_Size);
        } else {
            C_(printf("\nflash_process_file_repeat.c(%d): SubTOC list in BDM!\n", __LINE__);)
            IO_Handle = Do_IOW_BDMOpen(&(Param_p->BootAreaConfig));
            VERIFY(IO_Handle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);

            IO_RetVal = Do_IO_PositionedWrite(IO_Handle, SubTOC_p, SubTOC_Size, SubTOC_Size, Param_p->SubTOC_Offset);
            VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

            CLOSE_HANDLE(IO_Handle);
        }
    }

    if (TRUE == UpdateBootArea) {
        IO_Handle = Do_IOW_BAMOpen(&(Param_p->BootAreaConfig));
        VERIFY(IO_Handle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);

        IO_RetVal = Do_IO_PositionedWrite(IO_Handle, Param_p->BootBuffer_p, BAM_BOOT_IMAGE_COPY_SIZE, BAM_BOOT_IMAGE_COPY_SIZE, 0);
        VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

        CLOSE_HANDLE(IO_Handle);
    }

    It = Param_p->TOC_List;

    AddFlashInfoRecord("DONE!\n");
    AddFlashInfoRecord("\nFlashing completed successfully!\n");
    AddFlashInfoRecord("\n##### Displaying TOC structure #####\n");
    AddFlashInfoRecord("\n[/TOC]\n");

    PrintTOCInfo(It, Param_p->ManifestParser_p);

    It = Param_p->SubTOC_List;

    if (It) {
        AddFlashInfoRecord("\n[/TOC/%s]\n", It->Parent->entry.TOC_Id);
        PrintTOCInfo(It, Param_p->ManifestParser_p);
    } else {
        AddFlashInfoRecord("\nSub TOC not found.\n");
    }

    Param_p->SubTOC_List = Do_TOC_DestroyTOCList(Param_p->SubTOC_List);
    Param_p->TOC_List    = Do_TOC_DestroyTOCList(Param_p->TOC_List);

    Param_p->State = PROCESS_END;

    ReturnValue = E_SUCCESS;
ErrorExit:
    BUFFER_FREE(Param_p->BootBuffer_p);
    BUFFER_FREE(TOC_p);
    BUFFER_FREE(SubTOC_p);

    return ReturnValue;
}

static ErrorCode_e Handle_ProcessWriteBootArea_Layout_3_0(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e      ReturnValue           = E_GENERAL_FATAL_ERROR;
    SourceType_t     ImageType             = DEVICE_UNKNOWN;
    IO_Handle_t      IO_Handle             = IO_INVALID_HANDLE;
    uint64           IO_RetVal             = IO_ERROR;
    ManifestParser_t *ManifestParserTemp_p = NULL;
    TOC_List_t       *SubTocNode           = NULL;
    uint8            *TOC_p                = NULL;
    uint8            *SubTOC_p             = NULL;
    uint32            TOC_Size             = 0;
    uint32            SubTOC_Size          = 0;
    uint32            index                = 0;

    AddFlashInfoRecord("\nWriting TOC, SubTOC and Boot Image...");

    if (Param_p->SubTOC_List == NULL) {
        Param_p->SubTOC_List = Do_TOC_GetNext_SubTocList(Param_p->TOC_List);
    }

    if ((Param_p->TOC_List == NULL) || (Param_p->SubTOC_List == NULL)) {
        A_(printf("flash_process_file_repeat.c(%d): TOC and SUBTOC List must be present!!!\n", __LINE__);)
        goto ErrorExit;
    }

    if (Param_p->BootBuffer_p == NULL) {
        ReturnValue = ReadOldBootArea(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    IO_Handle = Do_IOW_BDMOpen(&(Param_p->BootAreaConfig));
    VERIFY(IO_Handle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);

    //
    // WRITE ROOT TOC AREA
    // TODO: Check how this can be avoided in cases where no changes are
    //       applied in TOC list
    //
    TOC_p = Do_TOC_CreateTOCDataFromList(Param_p->TOC_List, &TOC_Size);
    VERIFY(NULL != TOC_p, E_GENERAL_FATAL_ERROR);

    UpdateTOCInWriteData((uint8 *)TOC_p, Param_p->TOC_List, 1, 0);

    // Write first copy of ROOT TOC
    IO_RetVal = Do_IO_PositionedWrite(IO_Handle, TOC_p, TOC_Size, TOC_Size, BOOT_IMAGE_SECOND_COPY_OFFSET);
    VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

    UpdateTOCInWriteData((uint8 *)TOC_p, Param_p->TOC_List, 2, BAM_BOOT_BLOCK_SIZE);

    // Write second copy of ROOT TOC
    IO_RetVal = Do_IO_PositionedWrite(IO_Handle, TOC_p, TOC_Size, TOC_Size, BOOT_IMAGE_THIRD_COPY_OFFSET);
    VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

    //
    // WRITE BOOT BLOCK AREA
    //
    ManifestParserTemp_p = Param_p->ManifestParser_p - (Param_p->NoOfParsedImages - 1);

    for (index = 0; index < Param_p->NoOfImages; index++) {

        ImageType = GetImageType(ManifestParserTemp_p);

        if (ImageType == DEVICE_BAM) {
            SubTocNode = Do_TOC_LocateEntryByNodeId(Param_p->SubTOC_List, ManifestParserTemp_p->TocID);

            if (SubTocNode != NULL) {
                SubTocNode->entry.TOC_Offset -= Param_p->SubTOC_Offset;
            }
        }

        ManifestParserTemp_p++;
    }

    SubTOC_p = Do_TOC_CreateTOCDataFromList(Param_p->SubTOC_List, &SubTOC_Size);
    VERIFY(NULL != SubTOC_p, E_GENERAL_FATAL_ERROR);

    memcpy(Param_p->BootBuffer_p, SubTOC_p, SubTOC_Size);

    IO_RetVal = Do_IO_PositionedWrite(IO_Handle, Param_p->BootBuffer_p, Param_p->BootBufferSize, Param_p->BootBufferSize, Param_p->SubTOC_Offset);
    VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

    IO_RetVal = Do_IO_PositionedWrite(IO_Handle, Param_p->BootBuffer_p, Param_p->BootBufferSize, Param_p->BootBufferSize, Param_p->SubTOC_Offset + BAM_BOOT_BLOCK_SIZE);
    VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

    AddFlashInfoRecord("DONE!\n");
    AddFlashInfoRecord("\nFlashing completed successfully!\n");
    AddFlashInfoRecord("\n##### Displaying TOC structure #####\n");
    AddFlashInfoRecord("\n[/TOC]\n");

    PrintTOCInfo(Param_p->TOC_List, Param_p->ManifestParser_p);

    if (Param_p->SubTOC_List) {
        AddFlashInfoRecord("\n[/TOC/%s]\n", Param_p->SubTOC_List->Parent->entry.TOC_Id);
        PrintTOCInfo(Param_p->SubTOC_List, Param_p->ManifestParser_p);
    } else {
        AddFlashInfoRecord("\nSub TOC not found.\n");
    }

    Param_p->SubTOC_List = Do_TOC_DestroyTOCList(Param_p->SubTOC_List);
    Param_p->TOC_List    = Do_TOC_DestroyTOCList(Param_p->TOC_List);

    Param_p->State = PROCESS_END;

    ReturnValue = E_SUCCESS;
ErrorExit:
    CLOSE_HANDLE(IO_Handle);
    BUFFER_FREE(Param_p->BootBuffer_p);
    BUFFER_FREE(SubTOC_p);
    BUFFER_FREE(TOC_p);

    return ReturnValue;
}


static ErrorCode_e Handle_ProcessFileFinalize(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    if (Param_p->Flashlayout_p != NULL) {
        ReturnValue = Do_FlashLayoutParser_Destroy(&Param_p->Flashlayout_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    Param_p->ManifestParser_p = Param_p->ManifestParser_p - Param_p->NoOfParsedImages;

    ReturnValue = Do_ManifestParser_Destroy(&Param_p->ManifestParser_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    Param_p->State = PROCESS_INIT;
    Param_p->ExeContext_p->Running = FALSE;
    C_(printf("flash_process_file_repeat.c(%d): Process File Finished!\n", __LINE__);)

#ifdef FAILSAFE_FLASHING
    BUFFER_FREE(Param_p->FS_ProgressData_p);
#endif

ErrorExit:
    VerifyAndRecord(E_SUCCESS == ReturnValue, "Error while ending flashing process", ReturnValue);
    return ReturnValue;
}

static ErrorCode_e CompatibilityCheckManifestVsFlashlayout(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e          ReturnValue        = E_GENERAL_FATAL_ERROR;
    FlashLayoutParser_t *Flashlayout_Temp_p = NULL;
    ManifestParser_t    *ManifestTemp_p     = NULL;
    boolean              EntryFound         = FALSE;
    uint32               Counter_1          = 0;
    uint32               Counter_2          = 0;

    ManifestTemp_p = Param_p->ManifestParser_p;

    for (Counter_1 = 1; Counter_1 <= Param_p->NoOfImages; Counter_1++) {
        Flashlayout_Temp_p = Param_p->Flashlayout_p;
        EntryFound = FALSE;
        Counter_2 = 0;

        if (IsContentTypeSkippable(ManifestTemp_p->ContentType)) {
            ManifestTemp_p++;
            continue;
        }

        do {
            if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                C_(printf("flash_process_file_repeat.c(%d): TOC_ID %s found\n", __LINE__, ManifestTemp_p->TocID);)
                EntryFound = TRUE;
            } else {
                Flashlayout_Temp_p++;
                Counter_2++;
            }
        } while ((!EntryFound) && (Counter_2 < Param_p->FlashlayoutImages));

        if (!EntryFound) {
            // Previously a BAM image didn't generate a TOC_ID in FlashLayoutParser_t but now
            // it is possible to specify an image in the bootarea which will get a TOC_ID
            if (GetImageType(ManifestTemp_p) == DEVICE_BAM) {
                ManifestTemp_p++;
                continue;
            }

            A_(printf("flash_process_file_repeat.c(%d): TOC_ID %s not found in flashlayout!\n", __LINE__, ManifestTemp_p->TocID);)
            ReturnValue = E_ENTRY_NOT_FOUND_IN_FLASHLAYOUT;
            goto ErrorExit;
        }

        if (ManifestTemp_p->TargetStartAddress == 0) {
            if (0 == strncmp(ManifestTemp_p->ContentType, ContentTypeSparseImageString, strlen(ContentTypeSparseImageString))) {

#ifdef FAILSAFE_FLASHING

                if (Param_p->ReflashInProgress == FALSE) {
#endif
                    ReturnValue = Do_EraseAreaWithDriver(ManifestTemp_p->TargetId, Flashlayout_Temp_p->StartAddress, Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress);
                    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#ifdef FAILSAFE_FLASHING
                }

#endif

                if (IsLastEntryInTOC_Partition(Param_p, ManifestTemp_p->TocID, (uint32)ManifestTemp_p->TargetStartAddress)) {
                    ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION_LAST;
                } else {
                    ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION_FIRST;
                }
            } else {
                ManifestTemp_p->TargetStartAddress = Flashlayout_Temp_p->StartAddress;
            }
        } else {
            // Make sure that images start at very beginning of TOC partition (except when they are TOC partition extensions)
            VERIFY(0 == strncmp(ManifestTemp_p->ContentType, ContentTypeSparseImageString, strlen(ContentTypeSparseImageString)), E_MISMATCH_MANIFEST_FLASHLAYOUT);
            VERIFY(!IsFirstEntryInTOC_Partition(Param_p, ManifestTemp_p->TocID, (uint32)ManifestTemp_p->TargetStartAddress), E_MISMATCH_MANIFEST_FLASHLAYOUT);

            if (IsLastEntryInTOC_Partition(Param_p, ManifestTemp_p->TocID, (uint32)ManifestTemp_p->TargetStartAddress)) {
                ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION_LAST;
            } else {
                ManifestTemp_p->FlashingFlags |= PARTITION_ENTRY_EXTENSION;
            }
        }

        if (Flashlayout_Temp_p->EndAddress > Flashlayout_Temp_p->StartAddress) {
            ManifestTemp_p->PartitionContentSize = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;
        } else {
            A_(printf("flash_process_file_repeat.c(%d): Invalid Start and End address in flashlayout!!\n", __LINE__);)
        }

        /* Check some other parameters? */
        ManifestTemp_p++;
    }

    ReturnValue = UpdateSparseImagesParameters(Param_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e UpdateSparseImagesParameters(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e          ReturnValue        = E_GENERAL_FATAL_ERROR;
    FlashLayoutParser_t *Flashlayout_Temp_p = NULL;
    ManifestParser_t    *ManifestTemp_p     = NULL;
    boolean              EntryFound         = FALSE;
    uint32 i = 1;
    uint32 j = 1;

    ManifestTemp_p = Param_p->ManifestParser_p;

    for (i = 1; i <= Param_p->NoOfImages; i++) {
        Flashlayout_Temp_p = Param_p->Flashlayout_p;
        j = 1;

        if (IsContentTypeSkippable(ManifestTemp_p->ContentType)) {
            ManifestTemp_p++;
            continue;
        }

        if ((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST) ||
                (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_LAST) ||
                (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION)) {
            do {
                if (0 == strncmp(ManifestTemp_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                    ManifestTemp_p->TargetStartAddress += Flashlayout_Temp_p->StartAddress;
                    EntryFound = TRUE;
                    break;
                } else {
                    Flashlayout_Temp_p++;
                    j++;
                }
            } while (j <= Param_p->FlashlayoutImages);

            VERIFY(TRUE == EntryFound, E_MISMATCH_MANIFEST_FLASHLAYOUT);
        }

        ManifestTemp_p++;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

static boolean IsFirstEntryInTOC_Partition(ProcessFileParameters_t *Param_p, char *TocID, uint32 TargetStartAddress)
{
    ManifestParser_t *ManifestTemp_p = NULL;
    uint32 i = 1;

    ManifestTemp_p = Param_p->ManifestParser_p;

    for (i = 1; i <= Param_p->NoOfImages; i++) {
        if (0 == strncmp(ManifestTemp_p->TocID, TocID, MAX_TOC_ID_SIZE)) {
            if (ManifestTemp_p->TargetStartAddress < (uint64)TargetStartAddress) {
                return FALSE;
            }
        }

        ManifestTemp_p++;
    }

    return TRUE;
}

static boolean IsLastEntryInTOC_Partition(ProcessFileParameters_t *Param_p, char *TocID, uint32 TargetStartAddress)
{
    ManifestParser_t *ManifestTemp_p = NULL;
    uint32 i = 1;

    ManifestTemp_p = Param_p->ManifestParser_p;

    for (i = 1; i <= Param_p->NoOfImages; i++) {
        if (0 == strncmp(ManifestTemp_p->TocID, TocID, MAX_TOC_ID_SIZE)) {
            if (ManifestTemp_p->TargetStartAddress > (uint64)TargetStartAddress) {
                return FALSE;
            }
        }

        ManifestTemp_p++;
    }

    return TRUE;
}

static ErrorCode_e CompatibilityCheckManifestVsTOC(ProcessFileParameters_t *Param_p, const uint8 ExistingTOCDevice)
{
    ErrorCode_e       ReturnValue      = E_GENERAL_FATAL_ERROR;
    ManifestParser_t *ManifestTemp_p   = NULL;
    TOC_List_t       *TOC_List_Entry_p = NULL;
    uint32            Counter          = 0;
    static boolean    BootAreaFound    = FALSE;

    ManifestTemp_p = Param_p->ManifestParser_p;

    for (Counter = 1; Counter <= Param_p->NoOfImages; Counter++) {
        if (IsContentTypeSkippable(ManifestTemp_p->ContentType)) {
            ManifestTemp_p++;
            continue;
        }

        if (GetImageType(ManifestTemp_p) == DEVICE_BAM) {
            if (ManifestTemp_p->TargetId != ExistingTOCDevice && ExistingTOCDevice != 0xFF) {
                A_(
                    printf("flash_process_file_repeat.c(%d): ", __LINE__);
                    printf("TOC is located on flash%d! manifest suggests TOC to be on flash%d\n", ExistingTOCDevice, ManifestTemp_p->TargetId);
                )
                ReturnValue = E_MISMATCH_MANIFEST_TOC;
                goto ErrorExit;
            }

            ReturnValue = Do_FPD_GetInfo(ManifestTemp_p->TargetId, &(Param_p->BootAreaConfig));
            VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

            ManifestTemp_p++;
            BootAreaFound = TRUE;
            continue;
        }

        TOC_List_Entry_p = Do_TOC_LocateEntryByNodeId(Param_p->TOC_List, ManifestTemp_p->TocID);

        if (TOC_List_Entry_p == NULL) {
            C_(printf("flash_process_file_repeat.c(%d): TOC ID %s not found!\n", __LINE__, ManifestTemp_p->TocID);)
            ManifestTemp_p++;
            continue;
        } else {
            if (ManifestTemp_p->TargetStartAddress == 0xFFFFFFFFFFFFFFFF) {
                ManifestTemp_p->TargetStartAddress = (uint64)TOC_List_Entry_p->entry.TOC_Offset;
            } else {
                if ((ManifestTemp_p->TargetStartAddress != (uint64)TOC_List_Entry_p->entry.TOC_Offset) &&
                        !((ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION) ||
                          (ManifestTemp_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_LAST))) {
                    A_(printf("flash_process_file_repeat.c(%d): TOC ID %s has different start address in manifest and TOC!\n", __LINE__, ManifestTemp_p->TocID);)
                    ReturnValue = E_MISMATCH_MANIFEST_TOC;
                    goto ErrorExit;
                }
            }

            /* Check some other parameters? */
        }

        ManifestTemp_p++;
    }

    if (TRUE == BootAreaFound || (NULL != Param_p->TOC_List)) {
        ReturnValue = E_SUCCESS;
    } else {
        ReturnValue = E_BOOT_AREA_NOT_FOUND;
    }

ErrorExit:
    BootAreaFound = FALSE;
    return ReturnValue;
}

static boolean IsContentTypeSkippable(const char *Type_p)
{
    for (int i = 0; NULL != SkipTypes[i]; ++i) {
        if (0 == strcmp(Type_p, SkipTypes[i])) {
            return TRUE;
        }
    }

    return FALSE;
}

#ifdef MACH_TYPE_STN8500
static ErrorCode_e Update_TOC_List(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e  ReturnValue    = E_GENERAL_FATAL_ERROR;
    TOC_Entry_t  TmpEntry;

    if (Param_p->ManifestParser_p->TocID[0] != '\0') {
        if ((Param_p->ManifestParser_p->FlashingFlags & PARTITION_ENTRY_EXTENSION) || (Param_p->ManifestParser_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_FIRST)) {
            C_(printf("flash_process_file_repeat.c(%d): Extension manifest entry. No need of TOC list update\n", __LINE__);)
            ReturnValue = E_SUCCESS;
            goto ErrorExit;
        }

        ReturnValue = PrepareTOC_Data(Param_p, &TmpEntry);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        if (Param_p->ManifestParser_p->SubTOC_Of[0] != '\0') {
            C_(printf("flash_process_file_repeat.c(%d): Adding SUB TOC entry %s-", __LINE__, Param_p->ManifestParser_p->SubTOC_Of);)
            C_(printf("%s; ", Param_p->ManifestParser_p->TocID);)
            C_(printf("SubTOC offset 0x%08X\n", Param_p->SubTOC_Offset);)
            VERIFY(Do_TOC_Add_SubTocList_Entry(Param_p->TOC_List, Param_p->ManifestParser_p->SubTOC_Of, Param_p->SubTOC_Offset, &TmpEntry), E_GENERAL_FATAL_ERROR);

            if (PROCESS_WRITE_DATA == Param_p->State || PROCESS_ALLOCATE_FILE == Param_p->State) {
                AddFlashInfoRecord("# Created SubTOC entry: \"%s\"\n", Param_p->ManifestParser_p->TocID);
            }
        } else {
            C_(printf("flash_process_file_repeat.c(%d): Adding TOC entry %s\n", __LINE__, Param_p->ManifestParser_p->TocID);)
            VERIFY(Do_TOC_Add_RootList_Entry(&Param_p->TOC_List, &TmpEntry), E_GENERAL_FATAL_ERROR);

            if (PROCESS_WRITE_DATA == Param_p->State || PROCESS_ALLOCATE_FILE == Param_p->State) {
                AddFlashInfoRecord("# Created TOC entry \"%s\"\n", Param_p->ManifestParser_p->TocID);
            }
        }

        if (PROCESS_WRITE_DATA == Param_p->State) {
            PrintPhysicalAddress(&TmpEntry, Param_p->Flashlayout_p);
        }

    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e PrepareTOC_Data(ProcessFileParameters_t *Param_p, TOC_Entry_t *TmpEntry_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FlashLayoutParser_t *Flashlayout_Temp_p = NULL;
    SourceType_t Temp_ImageType = DEVICE_UNKNOWN;
    FlashLayoutRevision_t FlashLayoutRevision;
    boolean EntryFound = FALSE;
    uint64 TOC_Offset = 0;
    uint64 TOC_Size = 0;
    uint32 TOC_Flags = 0;
    uint32 i = 1;
    uint8 Counter = 0;

    VERIFY(NULL != TmpEntry_p, E_INVALID_INPUT_PARAMETERS);

    Flashlayout_Temp_p = Param_p->Flashlayout_p;
    Temp_ImageType = GetImageType(Param_p->ManifestParser_p);

    TOC_Offset = Param_p->ManifestParser_p->TargetStartAddress;
    TOC_Size  = Param_p->ManifestParser_p->PartitionContentSize; //Set Default TOC Size (FileSize)
    TOC_Flags = Param_p->ManifestParser_p->TocFlags + (Param_p->ManifestParser_p->TargetId << 16);

    Do_FlashLayoutParser_GetRevision(&FlashLayoutRevision);

    if ((0 == strncmp(Param_p->ManifestParser_p->SizeMode, PartSizeString, strlen(PartSizeString)))) {

        Counter = 0;
        Flashlayout_Temp_p = Param_p->Flashlayout_p;

        do {
            if (0 == strncmp(Flashlayout_Temp_p->TOC_ID, Param_p->ManifestParser_p->TocID, MAX_TOC_ID_SIZE)) {
                TOC_Size = (uint32)(Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress); //Set TOC PartitionSize
                break;
            }

            Flashlayout_Temp_p++;
            Counter++;
        } while (Counter < Param_p->FlashlayoutImages);

    }

    Do_TOC_FillEntry(TmpEntry_p, TOC_Offset, TOC_Size, TOC_Flags);

    if (Temp_ImageType == DEVICE_BAM) {
        TmpEntry_p->TOC_Align       = 0xffffffff;
        TmpEntry_p->TOC_LoadAddress = 0xffffffff;
    } else {
        TmpEntry_p->TOC_Align       = Param_p->ManifestParser_p->TocEntryPoint;
        TmpEntry_p->TOC_LoadAddress = Param_p->ManifestParser_p->TocLoadAddress;
    }

    if (Param_p->ManifestParser_p->FlashingFlags & PARTITION_ENTRY_EXTENSION_LAST) {
        Flashlayout_Temp_p = Param_p->Flashlayout_p;

        do {
            if (0 == strncmp(Param_p->ManifestParser_p->TocID, Flashlayout_Temp_p->TOC_ID, MAX_TOC_ID_SIZE)) {
                TOC_Offset = Flashlayout_Temp_p->StartAddress;
                TOC_Size = Flashlayout_Temp_p->EndAddress - Flashlayout_Temp_p->StartAddress;

                Do_TOC_FillEntry(TmpEntry_p, TOC_Offset, TOC_Size, TOC_Flags);

                EntryFound = TRUE;
                break;
            } else {
                Flashlayout_Temp_p++;
                i++;
            }
        } while (i <= Param_p->FlashlayoutImages);

        VERIFY(TRUE == EntryFound, E_GENERAL_FATAL_ERROR);
    }

    memcpy(TmpEntry_p->TOC_Id, Param_p->ManifestParser_p->TocID, TOC_ID_LENGTH);

    if (PROCESS_WRITE_DATA == Param_p->State || PROCESS_ALLOCATE_FILE == Param_p->State) {
        AddFlashInfoRecord("\nHandling %s\n", Param_p->ManifestParser_p->Name);

        if ('\0' != Param_p->ManifestParser_p->SubTOC_Of[0]) {
            AddFlashInfoRecord("# Writing image to target: /%s/TOC/%s/%s, 0x%08X%08X\n",
                               Param_p->ManifestParser_p->TargetType,
                               Param_p->ManifestParser_p->SubTOC_Of,
                               Param_p->ManifestParser_p->TocID,
                               (uint32)(Param_p->ManifestParser_p->TargetStartAddress >> 32),
                               (uint32)(Param_p->ManifestParser_p->TargetStartAddress));
        } else {
            AddFlashInfoRecord("# Writing image to target: /%s/TOC/%s, 0x%08X%08X\n",
                               Param_p->ManifestParser_p->TargetType,
                               Param_p->ManifestParser_p->TocID,
                               (uint32)(Param_p->ManifestParser_p->TargetStartAddress >> 32),
                               (uint32)(Param_p->ManifestParser_p->TargetStartAddress));
        }
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}
#endif //MACH_TYPE_STN8500

static ErrorCode_e PartitionsOverlapAndSpaceCheck(ManifestParser_t *ManifestParser_p, uint32 NrPartitions, FlashLayoutParser_t *Flashlayout_p, uint32 FlashlayoutImages)
{
    int i, j;
    uint64 Start1, End1; // These specify start and end of first partition
    uint64 Start2, End2; // These specify start and end of second partition
    ErrorCode_e ReturnValue = E_SUCCESS;

    // Overlap check is done by comparing start and end of each partition against start and end of all other partitions specified in the manifest file.
    // Target type isn't checked for now.

    for (i = 0; i < NrPartitions; i++) {
        if (IsContentTypeSkippable(ManifestParser_p[i].ContentType)) {
            continue;
        }

        if (!DisabledEnhancedAreaCheck) {
            ReturnValue = CheckEnhancedSettings(&ManifestParser_p[i]);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        }

        if (0 == ManifestParser_p[i].PartitionContentSize) {
            A_(printf("flash_process_file_repeat.c (%d): File with size 0 bytes found in the archive\n", __LINE__);)
        }

        Start1 = ManifestParser_p[i].TargetStartAddress;
        End1   = Start1 + ManifestParser_p[i].PartitionContentSize;

        for (j = i + 1; j < NrPartitions; j++) {
            if (IsContentTypeSkippable(ManifestParser_p[j].ContentType)) {
                continue;
            }

            Start2 = ManifestParser_p[j].TargetStartAddress;
            End2   = Start2 + ManifestParser_p[j].PartitionContentSize;

            if (ManifestParser_p[i].TargetId == ManifestParser_p[j].TargetId &&  !((Start1 >= End2) || (Start2 >= End1))) {
#ifdef PRINT_LEVEL_A_
                printf("**ERR Partitions Overlap [%s ", ManifestParser_p[i].Name);
                printf("0x%08x--", (uint32)Start1);
                printf("0x%08x] ", (uint32)End1);
                printf("and [%s ", ManifestParser_p[j].Name);
                printf("0x%08x--", (uint32)Start2);
                printf("0x%08x]\n", (uint32)End2);
#endif
                ReturnValue = E_OVERLAPPING_PARTITIONS_FOUND;
            }

        }

        ReturnValue = VerifySpaceInPartition(Flashlayout_p, FlashlayoutImages, &ManifestParser_p[i], ManifestParser_p[i].PartitionContentSize);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e ReadOldBootArea(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FlashLayoutRevision_t FlashLayoutRevision;

    Do_FlashLayoutParser_GetRevision(&FlashLayoutRevision);

    if ((FLASHLAYOUT_3_0_REVISION_MAJOR == FlashLayoutRevision.RevNrMajor) && (FLASHLAYOUT_3_0_REVISION_MINOR == FlashLayoutRevision.RevNrMinor)) {
        Param_p->BootBuffer_p = (uint32 *)malloc(BAM_BOOT_BLOCK_SIZE);
        VERIFY(NULL != Param_p->BootBuffer_p, E_ALLOCATE_FAILED);

        ReturnValue = Do_BAM_Read(&Param_p->BootAreaConfig, Param_p->SubTOC_Offset, (uint8 *)Param_p->BootBuffer_p, BAM_BOOT_BLOCK_SIZE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->BootBufferSize = BAM_BOOT_BLOCK_SIZE;
    } else {
        Param_p->BootBuffer_p = (uint32 *)malloc(BAM_BOOT_IMAGE_COPY_SIZE);
        VERIFY(NULL != Param_p->BootBuffer_p, E_ALLOCATE_FAILED);

        ReturnValue = Do_BAM_Read(&Param_p->BootAreaConfig, BOOT_IMAGE_SECOND_COPY_OFFSET, (uint8 *)Param_p->BootBuffer_p, BAM_BOOT_IMAGE_COPY_SIZE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->BootBufferSize = BAM_BOOT_IMAGE_COPY_SIZE;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e VerifySpaceInPartition(FlashLayoutParser_t *Flashlayout_p, uint32 NbrFlashlayoutEntries, ManifestParser_t *ManifestParserEntry_p, uint64 FileSize)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint32 index;

    switch (GetImageType(ManifestParserEntry_p)) {
    case DEVICE_BDM:

        for (index = 0; index < NbrFlashlayoutEntries; index++) {
            if (0 == strncmp(Flashlayout_p->TOC_ID, ManifestParserEntry_p->TocID, MAX_TOC_ID_SIZE)) {

                C_(printf("flash_process_file_repeat.c(%d): TOC_ID Manifest:         %s \n", __LINE__, ManifestParserEntry_p->TocID);)
                C_(printf("flash_process_file_repeat.c(%d): TOC_ID FlashLayout:      %s \n", __LINE__, Flashlayout_p->TOC_ID);)
                C_(printf("flash_process_file_repeat.c(%d): Partition start address: 0x%08X%08X\n", __LINE__, (uint32)(Flashlayout_p->StartAddress >> 32), (uint32)(Flashlayout_p->StartAddress));)
                C_(printf("flash_process_file_repeat.c(%d): Partition end address:   0x%08X%08X\n", __LINE__, (uint32)(Flashlayout_p->EndAddress >> 32), (uint32)(Flashlayout_p->EndAddress));)

                C_(printf("flash_process_file_repeat.c (%d): VerifySpaceInPartition: Matching Entries found.\n", __LINE__);)

                if (Flashlayout_p->EndAddress < (ManifestParserEntry_p->TargetStartAddress + FileSize)) {
                    A_(printf("flash_process_file_repeat.c (%d): File for %s TOC entry too large\n", __LINE__, ManifestParserEntry_p->TocID);)
                    ReturnValue = E_FILE_TOO_LARGE;
                }
                C_(else {
                    printf("flash_process_file_repeat.c (%d): VerifySpaceInPartition: File will fit.\n", __LINE__);
                })
            }

            Flashlayout_p++;
        }

        break;

    case DEVICE_BAM:
        C_(printf("BAM Image at: 0x%08X\n", (uint32)ManifestParserEntry_p->TargetStartAddress);)
        //        if(!Generic) {
        //            if (BAM_BOOT_IMAGE_COPY_SIZE < (ManifestParserEntry_p->TargetStartAddress + FileSize)) {
        //                A_(printf("flash_process_file_repeat.c (%d): File for %s TOC entry out of boot area\n", __LINE__, ManifestParserEntry_p->TocID);)
        //                ReturnValue = E_FILE_OUT_OF_BOOT_AREA;
        //            }
        //            C_(else {
        //                printf("flash_process_file_repeat.c (%d): VerifySpaceInPartition: File will fit\n", __LINE__);
        //            })
        //            break;
        //        } else {
        //            if (BAM_BLOCK_SIZE < (ManifestParserEntry_p->TargetStartAddress + FileSize)) {
        //                A_(printf("flash_process_file_repeat.c (%d): File for %s TOC entry out of boot area\n", __LINE__, ManifestParserEntry_p->TocID);)
        //                ReturnValue = E_FILE_OUT_OF_BOOT_AREA;
        //            }
        //            C_(else {
        //                printf("flash_process_file_repeat.c (%d): VerifySpaceInPartition: File will fit\n", __LINE__);
        //            })
        //            break;
        //        }

    case DEVICE_PARTITION:
        /*
        * We only have to check for overlap when the image type is partition,
        *  since there is no actual file for it.
        */
        break;

    case DEVICE_UNKNOWN:
    default:
        A_(printf("flash_process_file_repeat.c (%d): %s TOC entry has unknown or unsupported image type!\n", __LINE__, ManifestParserEntry_p->TocID));
        break;
    }

    return ReturnValue;
}

static ErrorCode_e CheckEnhancedSettings(ManifestParser_t *ManifestEntry_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (ManifestEntry_p->Enhanced) {
        FPD_Config_t FlashInfo = {0};
        uint64 TargetEndAddress = 0;

        ReturnValue = Do_FPD_GetInfo(ManifestEntry_p->TargetId, &FlashInfo);
        VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

        VERIFY(0x0 != FlashInfo.EnhancedAreaEnd, E_ENHANCED_AREA_NOT_SET);

        TargetEndAddress = ManifestEntry_p->TargetStartAddress + ManifestEntry_p->PartitionContentSize;
        VERIFY((FlashInfo.EnhancedAreaStart <= ManifestEntry_p->TargetStartAddress) && (TargetEndAddress <= FlashInfo.EnhancedAreaEnd), E_ENHANCED_IMAGE_OUT_OF_ENHANCED_AREA);
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

#ifdef PRINT_LEVEL_C_
static void PrintManifestEntries(ManifestParser_t *Manifest_p, uint32 ImagesNr)
{
    uint32 Index = 0;

    printf("\nflash_applications_functions.c (%d): No. of files: %d\n", __LINE__, ImagesNr);

    for (Index = 1; Index <= ImagesNr; Index++) {
        printf("flash_process_file_repeat.c(%d): File No.:             %d\n", __LINE__, Index);
        printf("flash_process_file_repeat.c(%d): Name:                 %s\n", __LINE__, Manifest_p->Name);
        printf("flash_process_file_repeat.c(%d): ContentType:          %s\n", __LINE__, Manifest_p->ContentType);
        printf("flash_process_file_repeat.c(%d): TargetType:           %s\n", __LINE__, Manifest_p->TargetType);
        printf("flash_process_file_repeat.c(%d): Id:                   %d\n", __LINE__, Manifest_p->TargetId);
        printf("flash_process_file_repeat.c(%d): StartAddress:         0x%08X\n", __LINE__, (uint32)(Manifest_p->TargetStartAddress));
        printf("flash_process_file_repeat.c(%d): TOC_ID:               %s\n", __LINE__, Manifest_p->TocID);
        printf("flash_process_file_repeat.c(%d): SubTOC_Of:            %s\n", __LINE__, Manifest_p->SubTOC_Of);
        printf("flash_process_file_repeat.c(%d): TocLoadAddress:       0x%08X\n", __LINE__, Manifest_p->TocLoadAddress);
        printf("flash_process_file_repeat.c(%d): TocEntryPoint:        0x%08X\n", __LINE__, Manifest_p->TocEntryPoint);
        printf("flash_process_file_repeat.c(%d): TocFlags:             0x%08X\n", __LINE__, Manifest_p->TocFlags);
        printf("flash_process_file_repeat.c(%d): PartitionContentSize: 0x%08X\n", __LINE__, (uint32)(Manifest_p->PartitionContentSize));
        printf("flash_process_file_repeat.c(%d): Enhanced:             %s\n", __LINE__, (Manifest_p->Enhanced ? "TRUE" : "FALSE"));
        printf("\n");
        Manifest_p++;
    }
}
#endif

#ifdef FAILSAFE_FLASHING
static ErrorCode_e ReflashInProgressCheck(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 HashXOR_SF = 0;
    uint8 Temp_HashProgressInfo = 0;
    uint8 HashOfProgress = 0;
    uint8 i = 0;
    uint8 *PageBuffer_p = NULL;
    FPD_Config_t FPD_Config  = {0};

    Param_p->FS_ProgressData_p = (FlashingProgressInfo_t *)malloc(sizeof(FlashingProgressInfo_t));
    VERIFY(NULL != Param_p->FS_ProgressData_p, E_GENERAL_FATAL_ERROR);

    ReturnValue = Do_FPD_GetInfo(i, &FPD_Config);
    VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

    PageBuffer_p = (uint8 *)malloc(FPD_Config.PageSize);
    VERIFY(NULL != PageBuffer_p, E_GENERAL_FATAL_ERROR);
    memset(PageBuffer_p, 0xAA, FPD_Config.PageSize);

    ReturnValue = Do_BAM_Read(&FPD_Config, BOOT_IMAGE_THIRD_COPY_OFFSET,
                              PageBuffer_p, FPD_Config.PageSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    memcpy((uint8 *)Param_p->FS_ProgressData_p, PageBuffer_p, sizeof(FlashingProgressInfo_t));

    Temp_HashProgressInfo = Param_p->FS_ProgressData_p->HashProgressInfo;
    C_(printf("flash_process_file_repeat.c(%d): Temp_HashProgressInfo = 0x%x \n", __LINE__, Temp_HashProgressInfo);)
    Param_p->FS_ProgressData_p->HashProgressInfo = 0; //reset its value

    /* Calculate protecting HASH of the FlashingProgressInfo data */
    Do_Crypto_AsynchronousHash(NULL, HASH_SIMPLE_XOR, (uint8 *)Param_p->FS_ProgressData_p,
                               sizeof(FlashingProgressInfo_t), &HashOfProgress, NULL, NULL);

    C_(printf("flash_process_file_repeat.c(%d): HashOfProgress = 0x%x \n", __LINE__, HashOfProgress);)

    /* Calculate sf hash of the current Flasharchive */
    Do_Crypto_AsynchronousHash(NULL, HASH_SIMPLE_XOR, (uint8 *)(Param_p->FileData_p),
                               (uint32)Param_p->FileSize, &HashXOR_SF, NULL, NULL);

    if ((FAILSAFE_MAGIC == Param_p->FS_ProgressData_p->Magic) &&
            (Temp_HashProgressInfo == HashOfProgress) &&
            (HashXOR_SF == Param_p->FS_ProgressData_p->Hash_SF)) {

        Param_p->ReflashInProgress = TRUE;
        Param_p->FS_ProgressData_p->HashProgressInfo = HashOfProgress; //write its value back
    } else {
        memset(Param_p->FS_ProgressData_p, 0xFF, sizeof(FlashingProgressInfo_t));
    }

    C_(printf("flash_process_file_repeat.c(%d): FailSafe = %d \n", __LINE__, Param_p->ReflashInProgress);)

    if (Param_p->ReflashInProgress == FALSE) {
        /* Initialize ProgressInfo Data */
        Param_p->FS_ProgressData_p->Magic = FAILSAFE_MAGIC;
        Param_p->FS_ProgressData_p->Hash_SF = HashXOR_SF;
        Param_p->FS_ProgressData_p->ParsedImageNum = 0;
        Param_p->FS_ProgressData_p->FileOffset = 0;
    }

ErrorExit:
    BUFFER_FREE(PageBuffer_p);
    return ReturnValue;
}


static ErrorCode_e UpdateFlashProgressInfo(ProcessFileParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 HashOfProgress = 0;
    uint8 *WriteBuffer_p = NULL;
    uint32 PageOffset = 0;
    FPD_Config_t FPD_Config  = {0};

    /* Calculate protecting HASH of the FlashingProgressInfo data */
    Param_p->FS_ProgressData_p->HashProgressInfo = 0;

    Do_Crypto_AsynchronousHash(NULL, HASH_SIMPLE_XOR, (uint8 *)(Param_p->FS_ProgressData_p),
                               sizeof(FlashingProgressInfo_t), &HashOfProgress, NULL, NULL);

    Param_p->FS_ProgressData_p->HashProgressInfo = HashOfProgress;

    /* Store the ProgressInfo */
    ReturnValue = Do_FPD_GetInfo(0, &FPD_Config);
    VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

    WriteBuffer_p = (uint8 *)malloc(FPD_Config.PageSize);
    VERIFY(NULL != WriteBuffer_p, E_GENERAL_FATAL_ERROR);
    memcpy(WriteBuffer_p, (uint8 *)Param_p->FS_ProgressData_p, sizeof(FlashingProgressInfo_t));

    PageOffset = BOOT_IMAGE_THIRD_COPY_OFFSET / FPD_Config.PageSize;

    ReturnValue = Do_EMMC_WritePage(PageOffset,
                                    WriteBuffer_p,
                                    NULL,
                                    &FPD_Config);

    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);


#ifdef PRINT_LEVEL_C_
    printf("flash_process_file_repeat.c(%d): MAGIC = 0x%x \n", __LINE__, Param_p->FS_ProgressData_p->Magic);
    printf("flash_process_file_repeat.c(%d): FileOffset = 0x%x \n", __LINE__, Param_p->FS_ProgressData_p->FileOffset);
    printf("flash_process_file_repeat.c(%d): ParsedImageNum = 0x%x \n", __LINE__, Param_p->FS_ProgressData_p->ParsedImageNum);
    printf("flash_process_file_repeat.c(%d): Name: %s\n", __LINE__, Param_p->FS_ProgressData_p->ImageName);
    printf("flash_process_file_repeat.c(%d): Hash_SF = 0x%x \n", __LINE__, Param_p->FS_ProgressData_p->Hash_SF);
    printf("flash_process_file_repeat.c(%d): HashProgressInfo = 0x%x \n", __LINE__, Param_p->FS_ProgressData_p->HashProgressInfo);
#endif

ErrorExit:
    BUFFER_FREE(WriteBuffer_p);
    return ReturnValue;
}
#endif
/** @} */

