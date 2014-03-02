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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
//#include "types.h"
//#include "stat.h"
#include "fcntl.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "r_flash_applications.h"
#include "r_communication_service.h"
#include "r_command_exec_service.h"
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

#include "r_boot_records_handler.h"
#include "r_communication_abstraction.h"
#include "flash_manager.h"
#include "block_device_management.h"

#include "t_manifest_parser.h"

#include "bam_emmc.h"

#include "boot_area_management.h"
#include "toc_handler.h"
#include "io_subsystem.h"
#include "watchdog.h"

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
#include "r_time_utilities.h"
#include "r_measurement_tool.h"
#endif

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/** Don't use alignment on page size, when you calculate vector parameters.*/
#define USE_NO_PAGE_ALIGNMENT    (0)

//#define FLASH_PROGRESS_INTERVAL  (10)
#define MAX_NUMBER_OF_DEVICES    (64)
#define MAX_DEVICE_PATH_LENGTH   (256)
#define BULK_VECTOR_DIVISOR    (24)

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
extern  Measurement_t *Measurement_p;
#endif

/* Context for verification of hash of archive.zip */
//VerifyContext_t VerifyContext;
uint8 *BDM_TempBuffer_p = NULL;

static char *DevUART0   = "/uart0";
static char *DevUART1   = "/uart1";
static char *DevUART2   = "/uart2";
static char *DevUSB     = "/usb";
static char *CommDevice = "comm";

const char *SignatureString     = "signature.sf";
const char *ArchiveString       = "archive.zip";
const char *ContentsString      = "contents.zip";
const char *ManifestFileName    = "manifest.txt";
const char *VersionsFileName    = "version.txt";
const char *FlashlayoutFileName = "flashlayout.txt";

const char *ContentTypeBootImageString       = "x-empflash/bootimage";
const char *ContentTypeImageString           = "x-empflash/image";
const char *ContentTypeSparseImageString     = "x-steflash/sparse-image";
const char *ContentTypeEmptyPartitionString  = "x-empflash/initilized-area";
const char *ContentTypeFlashLayoutString     = "x-empflash/flash-layout";


const char *BDMUnitString = "bdm";
const char *BAMUnitString = "boot";

const char *FlashString       = "flash";
const char *FlashDeviceString = "/flash";

const char *BDMDeviceString = "/bdm";
const char *BAMDeviceString = "/boot";

extern const char *ISSWString;
extern const char *CSPSAString;
extern const char *ARB_TBL_String;

typedef struct EraseRegion_s {
    uint64 StartAddress;
    uint64 LengthBytes;
    struct EraseRegion_s *next;
} EraseRegion_t;

typedef struct device_path_table {
    uint8 path_len;
    char *path_p;
    struct device_path_table *next;
} device_path_table_t;

//Variables that enables various loader options
uint32 DisabledEnhancedAreaCheck = 0;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
/*static*/
uint16 ExtractType(char *Type);
/*static*/
SourceType_t GetImageType(ManifestParser_t *ManifestParser_p);
/*static*/
SourceType_t GetDeviceType(void *Device_p);
/*static*/
uint32 GetDeviceNumber(void *Device_p);
static boolean GetDeviceInfo(const char *DevicePath_p, DeviceInfo_t *DeviceInfo);
static ErrorCode_e FillAreaWithContent(uint64 BytesOffset, uint64 BytesLength, uint8 AreaContent, FPD_Config_t *const FPDConfig_p);
static void ListDevices_FreeAllocatedSpace(ListDevice_t *Devices_p);
static ErrorCode_e GetDevicePathForTOCEntry(TOC_List_t *Entry_p, char *Path_p, const uint8 FlashDevice);
static ErrorCode_e ReadSubTOCFromFlash(TOC_List_t *List_p, FPD_Config_t *const FPDConfig_p);
static ErrorCode_e EraseTOCPartition(DeviceInfo_t *DeviceInfo, uint64 Start, uint64 Length, boolean RemovePartitionFromTOC);

static ErrorCode_e Conditionaly_EraseFPDDevice(const uint8 FlashDevice, uint64 AddressOffset, uint64 LengthBytes);
static ErrorCode_e Get_TA_ARB_TOCs(TOC_List_t **Trim_ARB_TOC_List_pp, FPD_Config_t *const FPDConfig_p);
static  EraseRegion_t *Create_EraseRegionsList(uint64 AddressOffset, uint64 LengthBytes, TOC_List_t *TOC_Entry_p);
static boolean EraseRegions_Add_Entry(EraseRegion_t **List_pp, EraseRegion_t *NewData_p);
static void Destroy_EraseRegionsList(EraseRegion_t *ListHead);
static ErrorCode_e DetectBootAreaLayout(DeviceInfo_t *DeviceInfo, BootAreaLayout_e *BootAreaLayout_p);
static ErrorCode_e UpdateBootAreaV2(DeviceInfo_t *DeviceInfo, TOC_List_t *TocList_p, uint8 *BootArea_p);
static ErrorCode_e UpdateBootAreaV3(DeviceInfo_t *DeviceInfo, TOC_List_t *TocList_p, char *TocId_p);
static ErrorCode_e Fetch_Device_Attributes(void *, FetchDeviceMode_t);
static ErrorCode_e Delete_Device_Path_Table(device_path_table_t *);
static ErrorCode_e Insert_path_in_device_tbl(device_path_table_t *, char *);
static boolean Is_path_in_device_tbl(device_path_table_t *, char *);
C_(static ErrorCode_e  Print_paths_from_device_tbl(device_path_table_t *);)

ErrorCode_e Read_SubTOC_Data(uint32 Offset, uint32 Size, uint8 *Data_p, uint32 BufferSize, FPD_Config_t *const FPDConfig_p);
static ErrorCode_e SelectLoaderOptions(uint32 Property, uint32 Value);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/* snprintf declared as external function to fix compile warning */
extern int snprintf(char *, size_t, const char *, ...);

ErrorCode_e Do_Flash_ProcessFileImpl(uint16 Session, const uint64 Length, const char *Type_p, const char *SourcePath_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    void *Data_p = NULL;
    uint32 TypeLength = 0;
    uint32 SourcePathLength = 0;
    CommandData_t CmmData   = {0};
    CmmData.Payload.Data_p = NULL;

    /* Part of structure must be initialized before checking of input parameters */
    CmmData.CommandNr = COMMAND_FLASH_PROCESSFILE;
    CmmData.ApplicationNr = GROUP_FLASH;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(0 != Length, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != Type_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != SourcePath_p, E_INVALID_INPUT_PARAMETERS);

    TypeLength = get_uint32_string_le((void **)&Type_p);
    VERIFY(TypeLength > 0, E_INVALID_INPUT_PARAMETERS);
    SourcePathLength = get_uint32_string_le((void **)&SourcePath_p);
    VERIFY(SourcePathLength > 0, E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = sizeof(uint64) + TypeLength + SourcePathLength;
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    memset(CmmData.Payload.Data_p, 0x00, CmmData.Payload.Size);
    Data_p = CmmData.Payload.Data_p;
    put_uint64_le((void **)&Data_p, Length);
    put_block((void **)&Data_p, Type_p, TypeLength);
    put_block((void **)&Data_p, SourcePath_p, SourcePathLength);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = Do_Measurement_CreateMeasurementData(NULL, &Measurement_p, FLASH_VIA_BULK_TRANSFER);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif //CFG_ENABLE_MEASUREMENT_TOOL

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("flash_applications_functions.c (%d): Do_Flash_ProcessFileImpl......(%x)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_Flash_DumpAreaImpl(uint16 Session, const char *Path_p, const uint64 Start, const uint64 Length, const char *TargetPath_p, const uint32 RedundantArea)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    void *Data_p = NULL;
    uint32 PathLength = 0;
    uint32 TargetPathLength = 0;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    /* Part of structure must be initialized before checking of input parameters */
    CmmData.CommandNr = COMMAND_FLASH_DUMPAREA;
    CmmData.ApplicationNr = GROUP_FLASH;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != Path_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != TargetPath_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 != Length, E_INVALID_INPUT_PARAMETERS);

    PathLength = get_uint32_string_le((void **)&Path_p);
    VERIFY(PathLength > 0, E_INVALID_INPUT_PARAMETERS);
    TargetPathLength = get_uint32_string_le((void **)&TargetPath_p);
    VERIFY(TargetPathLength > 0, E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = PathLength + sizeof(uint64) + sizeof(uint64) + TargetPathLength + sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    memset(CmmData.Payload.Data_p, 0x00, CmmData.Payload.Size);
    Data_p = CmmData.Payload.Data_p;
    put_block((void **)&Data_p, Path_p, PathLength);
    put_uint64_le((void **)&Data_p, Start);
    put_uint64_le((void **)&Data_p, Length);
    put_block((void **)&Data_p, TargetPath_p, TargetPathLength);
    put_uint32_le((void **)&Data_p, RedundantArea);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = Do_Measurement_CreateMeasurementData(NULL, &Measurement_p, FLASH_VIA_BULK_TRANSFER);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("flash_applications_functions.c (%d): Do_Flash_DumpAreaImpl......(%x)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_Flash_ListDevicesImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_FLASH_LISTDEVICES;
    CmmData.ApplicationNr = GROUP_FLASH;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("flash_applications_functions.c (%d): Do_Flash_ListDevicesImpl......(%x)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_Flash_EraseAreaImpl(uint16 Session, const char *Path_p, const uint64 Start, const uint64 Length)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 PathLen = 0;
    void *Data_p = NULL;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    B_(printf("flash_applications_functions.c(%d): Do_Flash_EraseAreaImpl: Path %s,", __LINE__, (Path_p + 4));)
    B_(printf(" Start %lld,", Start);)
    B_(printf(" Length %lld\n", Length);)

    /* Part of structure must be initialized before checking of input parameters */
    CmmData.CommandNr = COMMAND_FLASH_ERASEAREA;
    CmmData.ApplicationNr = GROUP_FLASH;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != Path_p, E_INVALID_INPUT_PARAMETERS);

    PathLen = get_uint32_string_le((void **)&Path_p);
    VERIFY(PathLen > 0, E_INVALID_INPUT_PARAMETERS);
    VERIFY(Path_p[4] == '/', E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = PathLen + sizeof(uint64) + sizeof(uint64);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    memset(CmmData.Payload.Data_p, 0x00, CmmData.Payload.Size);
    Data_p = CmmData.Payload.Data_p;
    put_block(&Data_p, Path_p, PathLen);
    put_uint64_le((void **)&Data_p, Start);
    put_uint64_le((void **)&Data_p, Length);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("flash_applications_functions.c (%d): Do_Flash_EraseAreaImpl......(%x)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_Flash_SetEnhancedAreaImpl(uint16 Session, const char *Path_p, const uint64 Start, const uint64 Length)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    void *Data_p = NULL;
    uint32 PathLength = 0;
    CommandData_t CmmData = {0};
    CmmData.Payload.Data_p = NULL;

    /* Part of structure must be initialized before checking of input parameters */
    CmmData.CommandNr = COMMAND_FLASH_SETENHANCEDAREA;
    CmmData.ApplicationNr = GROUP_FLASH;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != Path_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 != Length, E_INVALID_INPUT_PARAMETERS);

    PathLength = get_uint32_string_le((void **)&Path_p);
    VERIFY(PathLength > 0, E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = PathLength + sizeof(uint64) + sizeof(uint64);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    memset(CmmData.Payload.Data_p, 0x00, CmmData.Payload.Size);
    Data_p = CmmData.Payload.Data_p;
    put_block((void **)&Data_p, Path_p, PathLength);
    put_uint64_le((void **)&Data_p, Start);
    put_uint64_le((void **)&Data_p, Length);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    B_(printf("flash_applications_functions.c (%d): Do_Flash_SetEnhancedAreaImpl......(%x)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Do_Flash_SelectLoaderOptionsImpl(uint16 Session, const uint32 Property, const uint32 Value)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};
    void *Data_p = NULL;

    CmmData.CommandNr = COMMAND_FLASH_SELECTLOADEROPTIONS;
    CmmData.ApplicationNr = GROUP_FLASH;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Data_p = NULL;

    CmmData.Payload.Size = sizeof(uint32) + sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    memset(CmmData.Payload.Data_p, 0x00, CmmData.Payload.Size);
    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Property);
    put_uint32_le((void **)&Data_p, Value);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    B_(printf("flash_applications_functions.c (%d): Do_Flash_SelectLoaderOptionsImpl......(%x)\n\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*******************************************************************************
 *  Long running commands
 ******************************************************************************/

ErrorCode_e Flash_ListDevicesRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;

    ReturnValue = Fetch_Device_Attributes((void *) ExeContext_p, DEST_SERIALIZED_TO_PC);

ErrorExit:
    return ReturnValue;
}

ErrorCode_e Flash_EraseAreaRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *Data_p = NULL;
    char *Path = NULL;
    char *Path_p = NULL;
    uint32 PathLen = 0;
    uint64 Start = 0;
    uint64 Length = 0;
    DeviceInfo_t DeviceInfo;
    boolean RemovePartitionFromTOC = FALSE;
    device_path_table_t *device_path_table_p = NULL;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    Path = skip_str((void **)&Data_p);
    Start = get_uint64_le((void **)&Data_p);
    Length = get_uint64_le((void **)&Data_p);

    PathLen = get_uint32_le((void **)&Path);
    Path_p = (char *)malloc(PathLen + sizeof(char) + sizeof(char));
    ASSERT(NULL != Path_p);
    memcpy(Path_p, Path, PathLen);
    Path_p[PathLen] = '\0';

    /*
     *  Now we are about to create a table with all device paths included
     */

    device_path_table_p = (device_path_table_t *)malloc(sizeof(device_path_table_t));
    VERIFY(NULL != device_path_table_p, E_ALLOCATE_FAILED);

    device_path_table_p->path_p = NULL;
    device_path_table_p->next = NULL;
    Fetch_Device_Attributes((void *) device_path_table_p, DEST_STORE_PATHS_IN_TABLE);
    C_(
        ReturnValue = Print_paths_from_device_tbl(device_path_table_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    )

    /*
     *  Add trailing slash at the end of the path
     */
    if (Path_p[PathLen - 1] != '/') {
        PathLen++;
        Path_p[PathLen - 1] = '/';
        Path_p[PathLen] = '\0';
    } else {
        RemovePartitionFromTOC = TRUE;
    }

    if (Is_path_in_device_tbl(device_path_table_p, Path_p)) {
        C_(printf("flash_applications_functions.c (%d): Path FOUND in device path table\n", __LINE__);)
    } else {
        A_(printf("flash_applications_functions.c (%d): **ERROR: Path NOT FOUND in device path table \n", __LINE__);)
        ReturnValue = E_SUCCESS;
        goto ErrorExit;
    }

    C_(printf("flash_applications_functions.c (%d): **Flash_EraseAreaRepeat** \n", __LINE__);)

#ifdef ENABLE_LOADER_HWI2C_WD
    //Disable watchdog prior to erase
    HWI2C_Do_WatchDog_Disable();
#endif

    /* Input parameters type is uint64 and in all places where it is used
     * it is typecasted to uint32. This is temporary till PD_NAND, CABS
     * and MBBS API make support for parameters of type uint64.
     */

    if (!GetDeviceInfo(Path_p, &DeviceInfo)) {
        A_(printf("flash_applications_functions (%d): Failed to get DeviceInfo\n", __LINE__);)
        goto ErrorExit;
    }

    switch (DeviceInfo.DeviceType) {
    case DEVICE_FPD: {
        BootAreaLayout_e BootAreaLayout = BOOT_AREA_LAYOUT_UNKNOWN;

        VERIFY(0 != Length, E_INVALID_INPUT_PARAMETERS);

        ReturnValue = Do_Stop_Service(NUMBER_OF_SERVICES); /* Stop All Services */
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        //
        // Boot area can not be partially erased!!!
        //
        (void)DetectBootAreaLayout(&DeviceInfo, &BootAreaLayout);

        switch (BootAreaLayout) {
        case BOOT_AREA_LAYOUT_V2:
            VERIFY((Start == 0) || (Start >= (BOOT_IMAGE_FORTH_COPY_OFFSET + BAM_BOOT_IMAGE_COPY_SIZE)), E_INVALID_INPUT_PARAMETERS);
            VERIFY((Start + Length) >= (BOOT_IMAGE_FORTH_COPY_OFFSET + BAM_BOOT_IMAGE_COPY_SIZE), E_INVALID_INPUT_PARAMETERS);
            break;

        case BOOT_AREA_LAYOUT_V3:
            VERIFY((Start == 0) || (Start >= BAM_BOOT_AREA_V3_MAX_SIZE), E_INVALID_INPUT_PARAMETERS);
            VERIFY((Start + Length) >= BAM_BOOT_AREA_V3_MAX_SIZE, E_INVALID_INPUT_PARAMETERS);
            break;

        case BOOT_AREA_EMPTY:
        case BOOT_AREA_LAYOUT_UNKNOWN:

            if (Start < BAM_BOOT_AREA_V3_MAX_SIZE) {
                Length += Start;
                Start = 0;
            }

            if ((Start + Length) < BAM_BOOT_AREA_V3_MAX_SIZE) {
                Length = BAM_BOOT_AREA_V3_MAX_SIZE;
            }

            break;
        }

        //
        // Erase required region but take care about trim area data and TOCs.
        //
        ReturnValue = Conditionaly_EraseFPDDevice(DeviceInfo.DeviceId, Start, Length);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        break;
    }

    case DEVICE_BDM: {
        BDM_Unit_Config_t BDM_Config = {0};
        uint32 SizeOfLogSectorInBytes = 0;
        uint32 FirstBad = 0;
        uint32 Position = 0;
        uint32 NrOfSectors = 0;
        FPD_Config_t FPDConfig = {0};

        VERIFY(0 != Length, E_INVALID_INPUT_PARAMETERS);

        ReturnValue = Do_FPD_GetInfo(DeviceInfo.DeviceId, &FPDConfig);
        VERIFY(E_SUCCESS == ReturnValue, E_INVALID_INPUT_PARAMETERS);

        ReturnValue = Do_BDM_GetInfo(&FPDConfig, &BDM_Config);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        VERIFY((Length <= (BDM_Config.EndBlock - BDM_Config.StartBlock)), E_INVALID_INPUT_PARAMETERS);

        SizeOfLogSectorInBytes = BDM_Config.PageSizeInBytes;

        /* Check if input parameters are multiply by device logical block (sector). */
        VERIFY(!((0 != ((uint32)Start % SizeOfLogSectorInBytes)) || (0 != ((uint32)Length % SizeOfLogSectorInBytes))), E_INVALID_INPUT_PARAMETERS);

        Position = (uint32)Start / SizeOfLogSectorInBytes;
        NrOfSectors = (uint32)Length / SizeOfLogSectorInBytes;

        ReturnValue = Do_BDM_Junk(&FPDConfig, Position, NrOfSectors, &FirstBad);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;
    }
    case DEVICE_BAM: {
        FPD_Config_t FPDConfig = {0,};
        ReturnValue = Do_FPD_GetInfo(DeviceInfo.DeviceId, &FPDConfig);
        VERIFY(E_SUCCESS == ReturnValue, E_INVALID_INPUT_PARAMETERS);

        ReturnValue = Do_BAM_CompleteErase(&FPDConfig);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;
    }
    case DEVICE_PARTITION: {
        ReturnValue = EraseTOCPartition(&DeviceInfo, Start, Length, RemovePartitionFromTOC);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;
    }
    default:
        ReturnValue = E_INVALID_INPUT_PARAMETERS;
        B_(printf("flash_applications_functions.c (%d): Unknown device for path %s\n", __LINE__, Path_p);)
        goto ErrorExit;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    BUFFER_FREE(Path_p);
    ExeContext_p->Running = FALSE;
    Delete_Device_Path_Table(device_path_table_p);
#ifdef ENABLE_LOADER_HWI2C_WD
    (void)HWI2C_Enabe_WD(WATCHDOG_TIMEOUT);
#endif
    B_(printf("flash_applications_functions.c (%d): Do_Flash_EraseAreaImpl......(%x)\n\n", __LINE__, ReturnValue);)
    ReturnValue = Done_Flash_EraseArea(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

ErrorCode_e Flash_SetEnhancedAreaRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *Data_p   = NULL;
    char *Path_p   = NULL;
    char *Device_p = NULL;
    uint64 Start   = 0;
    uint64 Length  = 0;
    uint32 DeviceNumber;
    FPD_Config_t FPDConfig = {0};

    ASSERT(NULL != ExeContext_p);

    Data_p   = ExeContext_p->Received.Payload.Data_p;
    Path_p   = skip_str(&Data_p);
    Start    = get_uint64_le(&Data_p);
    Length   = get_uint64_le(&Data_p);
    Device_p = ExtractDevice(Path_p);
    VERIFY(NULL != Device_p, E_INVALID_INPUT_PARAMETERS);

    DeviceNumber = GetDeviceNumber(Device_p);
    BUFFER_FREE(Device_p);

    ReturnValue = Do_FPD_GetInfo(DeviceNumber, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, E_SERVICE_NOT_SUPPORTED);

    VERIFY((Start % FPDConfig.WriteProtectedSize) == 0, E_INVALID_INPUT_PARAMETERS_ENHANCED_AREA);
    VERIFY((Length % FPDConfig.WriteProtectedSize) == 0, E_INVALID_INPUT_PARAMETERS_ENHANCED_AREA);
    VERIFY((Start + Length) <= FPDConfig.DeviceSize, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = Do_FPD_SetEnhancedArea(Start, Length, DeviceNumber);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;

ErrorExit:

    ReturnValue = Done_Flash_SetEnhancedArea(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    ExeContext_p->Running = FALSE;

    return ReturnValue;
}

ErrorCode_e Flash_SelectLoaderOptionsRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *TempData_p = NULL;
    uint32 Property = 0;
    uint32 Value = 0;

    ASSERT(NULL != ExeContext_p);

    TempData_p = ExeContext_p->Received.Payload.Data_p;
    Property = get_uint32_le((void **)&TempData_p);
    Value = get_uint32_le((void **)&TempData_p);

    ReturnValue = SelectLoaderOptions(Property, Value);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;

ErrorExit:

    ReturnValue = Done_Flash_SelectLoaderOptions(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    ExeContext_p->Running = FALSE;

    return ReturnValue;
}

ErrorCode_e Flash_ErrorHandlerRepeat(void *ExecutionContext_p)
{
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint8 *Data_p = NULL;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 CommandNr = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    ReturnValue = (ErrorCode_e)get_uint32_le((void **)&Data_p);
    CommandNr = get_uint16_le((void **)&Data_p);

    switch (CommandNr) {
    case COMMAND_FLASH_PROCESSFILE:
        ReturnValue = Done_Flash_ProcessFile(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
    case COMMAND_FLASH_LISTDEVICES:
        ReturnValue = Done_Flash_ListDevices(ExeContext_p->Received.SessionNr, ReturnValue, 0, NULL);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
    case COMMAND_FLASH_DUMPAREA:
        ReturnValue = Done_Flash_DumpArea(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
    case COMMAND_FLASH_ERASEAREA:
        ReturnValue = Done_Flash_EraseArea(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
    case COMMAND_FLASH_SETENHANCEDAREA:
        ReturnValue = Done_Flash_SetEnhancedArea(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
    case COMMAND_FLASH_SELECTLOADEROPTIONS:
        ReturnValue = Done_Flash_SelectLoaderOptions(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
    default:
        break;
    }

    if (0 != ExeContext_p->Received.Payload.Size) {
        BUFFER_FREE(ExeContext_p->Received.Payload.Data_p);
    }

    ExeContext_p->Running = FALSE;
    ReturnValue = E_SUCCESS;
    return ReturnValue;

}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*static*/ uint16 ExtractType(char *Type)
{
    return 0;
}

SourceType_t GetImageType(ManifestParser_t *ManifestParser_p)
{
    if ((0 == strncmp(ContentTypeImageString, ManifestParser_p->ContentType, strlen(ContentTypeImageString))) ||
            (0 == strncmp(ContentTypeSparseImageString, ManifestParser_p->ContentType, strlen(ContentTypeSparseImageString)))) {
        if (0 == strncmp(BAMUnitString, ManifestParser_p->TargetType, strlen(BAMUnitString))) {
            C_(printf("flash_applications_functions.c (%d): DEVICE_BAM\n", __LINE__);)
            return DEVICE_BAM;
        } else if (0 == strncmp(BDMUnitString, ManifestParser_p->TargetType, strlen(BDMUnitString))) {
            C_(printf("flash_applications_functions.c (%d): DEVICE_BDM\n", __LINE__);)
            return DEVICE_BDM;
        } else {
            C_(printf("flash_applications_functions.c (%d): DEVICE_BDM\n", __LINE__);)
            return DEVICE_BDM;
        }
    } else if (0 == strncmp(ContentTypeBootImageString, ManifestParser_p->ContentType, strlen(ContentTypeBootImageString))) {
        C_(printf("flash_applications_functions.c (%d): DEVICE_BAM\n", __LINE__);)
        return DEVICE_BAM;
    } else if (0 == strncmp(ContentTypeEmptyPartitionString, ManifestParser_p->ContentType, strlen(ContentTypeEmptyPartitionString))) {
        C_(printf("flash_applications_functions.c (%d): Empty Partition DEVICE_PARTITION\n", __LINE__);)
        return DEVICE_PARTITION;
    } else {
        C_(printf("flash_applications_functions.c (%d): DEVICE_UNKNOWN\n", __LINE__);)
        return DEVICE_UNKNOWN;
    }
}

SourceType_t GetDeviceType(void *Device_p)
{
    unsigned char *LastWord_p = NULL;
    SourceType_t ImageType = DEVICE_UNKNOWN;

    LastWord_p = (unsigned char *) GetLastWord(Device_p);

    if (NULL != LastWord_p) {
        if (0 == strncmp(BAMUnitString, (const char *) LastWord_p, strlen(BAMUnitString))) {
            ImageType = DEVICE_BAM;
        } else if (0 == strncmp(BDMUnitString, (const char *) LastWord_p, strlen(BDMUnitString))) {
            ImageType = DEVICE_BDM;
        } else if (0 == strncmp(FlashString, (const char *) LastWord_p, strlen(FlashString))) {
            ImageType = DEVICE_FPD;
        } else {
            ImageType = DEVICE_UNKNOWN;
        }

        free(LastWord_p);
    }

    return ImageType;
}

static boolean GetDeviceInfo(const char *DevicePath_p, DeviceInfo_t *DeviceInfo)
{
    boolean ReturnValue = TRUE;
    uint8 PathSlashCounter = 0;
    uint32 i = 0;
    size_t len = 0;

    uint8 token_num = 0;
    char *token_p = NULL;
    boolean isFetchedLastToken = FALSE;
    char *term_str = NULL;
    char *tmp = NULL;
    char *Path_p = NULL;
    char *save_p = NULL;

    if (NULL == DevicePath_p) {
        ReturnValue = FALSE;
        goto Exit;
    }

    memset(DeviceInfo, '\0', sizeof(DeviceInfo));
    DeviceInfo->DeviceType = DEVICE_UNKNOWN;
    DeviceInfo->DeviceId = flash0; //initialize default flash device
    DeviceInfo->TOCId[0] = '\0';

    Path_p = (char *) malloc(strlen(DevicePath_p) + 1);

    if (NULL == Path_p) {
        A_(printf("flash_application_functions.c(%d): **ERR Memory allocation fail **\n", __LINE__);)
        ReturnValue = FALSE;
        goto Exit;
    }

    memset(Path_p , '\0', strlen(DevicePath_p) + 1);
    strncpy(Path_p,  DevicePath_p, strlen(DevicePath_p));

    len = strlen(DevicePath_p);

    for (i = 0; i < len; i++) {
        if (DevicePath_p[i] == '/') {
            PathSlashCounter++;
        }
    }

    if ((PathSlashCounter < MIN_NUM_SLASHES) || (PathSlashCounter > MAX_NUM_SLASHES)) {
        A_(printf("flash_applications_functions.c(%d): Invalid path\n", __LINE__);)
        DeviceInfo->DeviceType = DEVICE_UNKNOWN;
        goto Exit;
    }

    // Path Tokenizer
    // We're about to break the path into tokens in order to process them separately
    // example: Path=/token1/token2/token3/token4/
    token_num = 1;
    memset(DeviceInfo->TOCId, '\0', TOC_ID_LENGTH + 1);
    memset(DeviceInfo->SUBTOCId, '\0', TOC_ID_LENGTH + 1);

    do {
        switch (token_num) {
        case 1:  //fetch first token
            token_p = (char *) strtok_r(Path_p, "/", &save_p);

            if (NULL != (tmp = strstr(token_p, "flash"))) {
                //make sure the symbol after "flash" is a number
                if ((*(tmp + strlen("flash")) < '0') || (*(tmp + strlen("flash")) > '9')) {
                    //non-numeric value <n> found in "flash<n>"
                    A_(printf("flash_applications_functions.c(%d): Can not find valid flash device in path\n", __LINE__);)
                    DeviceInfo->DeviceType = DEVICE_UNKNOWN;
                    goto Exit;
                }

                DeviceInfo->DeviceId = strtol(token_p + strlen("flash"), &term_str, 10) ;

                if (DeviceInfo->DeviceId >= MAX_FLASH_DEVICES) {
                    A_(printf("flash_applications_functions.c(%d): Flash device /flash%d not supported \n", __LINE__, DeviceInfo->DeviceId);)
                    goto Exit;
                }

                //make sure make sure that "flash<n>" is null terminated
                if (*term_str != '\0') {
                    A_(printf("flash_applications_functions.c(%d): Can not find valid flash device in path\n", __LINE__);)
                    DeviceInfo->DeviceType = DEVICE_UNKNOWN;
                    goto Exit;
                }
            } else {
                A_(printf("flash_applications_functions.c(%d): Can not find valid flash device in path\n", __LINE__);)
                DeviceInfo->DeviceType = DEVICE_UNKNOWN;
                goto Exit;
            }

            if (PathSlashCounter == 2) {
                DeviceInfo->DeviceType = DEVICE_FPD;
                goto Exit; //we fetch all we need for this kinda DeviceType, so we exit.
            }

            break;
        case 2:  //fetch second token

            if (!strncmp(token_p, "TOC", strlen("TOC") + 1)) {
                DeviceInfo->DeviceType = DEVICE_PARTITION;
            } else if (!strncmp(token_p, "boot0", strlen("boot0") + 1)) {
                //Specific code to do something when boot<n> exist
                DeviceInfo->DeviceType = DEVICE_BAM;
            } else if (!strncmp(token_p, BDMUnitString, strlen(BDMUnitString) + 1)) {
                DeviceInfo->DeviceType = DEVICE_BDM;
            } else {
                DeviceInfo->DeviceType = DEVICE_UNKNOWN;
                goto Exit;
            }

            break;
        case  3: //fetch third token

            if (!strncmp(token_p, "TOC", strlen("TOC") + 1)) {
                DeviceInfo->DeviceType = DEVICE_PARTITION;
            } else  if (token_num < (PathSlashCounter - 1)) { //fetch SUBTOC string
                strncpy(DeviceInfo->SUBTOCId, token_p, strlen(token_p));
            }

            break;
        case  4: //fetch fourth token
            // Specific code to do something with 4th token (The last one)
            // Info: The TOCid will be fetched outside of the switch
            break;
        default:
            //Currently the maximum supported token number is 4
            A_(printf("flash_applications_functions.c(%d): Path too long. NOT supported\n", __LINE__);)
            DeviceInfo->DeviceType = DEVICE_UNKNOWN;
            goto Exit;
        }

        if (token_num == (PathSlashCounter - 1)) { //we have reached the last token
            //fetch TOCId
            strncpy(DeviceInfo->TOCId, token_p, strlen(token_p));
            isFetchedLastToken = TRUE;
        } else { // keep tokenizing the rest of the path
            token_p = (char *) strtok_r(NULL, "/", &save_p);
            token_num++;
        }

        if (strlen(token_p) == 0) {
            //we are not allowing using double slashes in path
            A_(printf("flash_applications_functions.c(%d): Double-backslashes can not be used in path\n", __LINE__);)
            DeviceInfo->DeviceType = DEVICE_UNKNOWN;
            goto Exit;
        }

    } while (!isFetchedLastToken);

Exit:
    BUFFER_FREE(Path_p);
    return ReturnValue;
}

static ErrorCode_e Conditionaly_EraseFPDDevice(const uint8 FlashDevice, uint64 AddressOffset, uint64 LengthBytes)
{
#define COMPLETE_FLASH_ERASE 0xffffffffffffffff
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    EraseRegion_t *EraseRegionList_p = NULL;
    TOC_List_t *Trim_ARB_TOC_List_p = NULL;
    TOC_List_t *SubTocNode = NULL;
    uint8 *TA_ARB_TOC_Data_p = NULL;
    uint8 *SubTOC_BB1_p = NULL;
    FPD_Config_t FPDConfig = {0,};
    uint32 SubTocListSize = 0;

    ReturnValue = Do_FPD_GetInfo(FlashDevice, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, E_INVALID_INPUT_PARAMETERS);

    if (COMPLETE_FLASH_ERASE == LengthBytes) {
        LengthBytes = FPDConfig.DeviceSize;
        AddressOffset = 0;
    }

    //
    // Get TRIM TOC's (if any)
    //
    ReturnValue = Get_TA_ARB_TOCs(&Trim_ARB_TOC_List_p, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    if (NULL != Trim_ARB_TOC_List_p) {
        EraseRegion_t *EraseRegionEntry_p = NULL;
        uint32 TA_ARB_TOC_DataSize = 0;

        EraseRegionList_p = Create_EraseRegionsList(AddressOffset, LengthBytes, Trim_ARB_TOC_List_p);
        VERIFY(NULL != EraseRegionList_p, ReturnValue);

        EraseRegionEntry_p = EraseRegionList_p;

        do {
            ReturnValue = Do_EraseAreaWithDriver(FlashDevice, EraseRegionEntry_p->StartAddress, EraseRegionEntry_p->LengthBytes);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            EraseRegionEntry_p = EraseRegionEntry_p->next;
        } while (NULL != EraseRegionEntry_p);

        //
        // If boot area is erased write back Trim Area TOCs
        //
        if (0 == AddressOffset) {
            IO_Handle_t IO_Handle      = IO_INVALID_HANDLE;
            uint64      IO_RetVal      = IO_ERROR;

            TA_ARB_TOC_Data_p = Do_TOC_CreateTOCDataFromList(Trim_ARB_TOC_List_p, &TA_ARB_TOC_DataSize);
            VERIFY(NULL != TA_ARB_TOC_Data_p, E_GENERAL_FATAL_ERROR);

            SubTocNode = Do_TOC_GetNext_SubTocNode(Trim_ARB_TOC_List_p);

            if (SubTocNode != NULL) {
                IO_Handle = Do_IOW_BDMOpen(&FPDConfig);
                VERIFY(IO_Handle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);

                UpdateTOCInWriteData((uint8 *)TA_ARB_TOC_Data_p, Trim_ARB_TOC_List_p, 1, 0);

                // Write first copy of ROOT TOC
                IO_RetVal = Do_IO_PositionedWrite(IO_Handle, TA_ARB_TOC_Data_p, TA_ARB_TOC_DataSize, TA_ARB_TOC_DataSize, BOOT_IMAGE_SECOND_COPY_OFFSET);
                VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

                UpdateTOCInWriteData((uint8 *)TA_ARB_TOC_Data_p, Trim_ARB_TOC_List_p, 2, BAM_BOOT_BLOCK_SIZE);

                // Write second copy of ROOT TOC
                IO_RetVal = Do_IO_PositionedWrite(IO_Handle, TA_ARB_TOC_Data_p, TA_ARB_TOC_DataSize, TA_ARB_TOC_DataSize, BOOT_IMAGE_THIRD_COPY_OFFSET);
                VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

                SubTOC_BB1_p = Do_TOC_CreateTOCDataFromList(SubTocNode->SubItems, &SubTocListSize);
                VERIFY(NULL != SubTOC_BB1_p, E_GENERAL_FATAL_ERROR);

                // Write first copy of boot block
                IO_RetVal = Do_IO_PositionedWrite(IO_Handle, SubTOC_BB1_p, SubTocListSize, SubTocListSize, SubTocNode->entry.TOC_Offset);
                VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);

                // Write second copy of boot block
                IO_RetVal = Do_IO_PositionedWrite(IO_Handle, SubTOC_BB1_p, SubTocListSize, SubTocListSize, SubTocNode->entry.TOC_Offset + BAM_BOOT_BLOCK_SIZE);
                VERIFY(IO_RetVal != IO_ERROR, E_IO_FAILED_TO_READ);
            } else {

                IO_Handle = Do_IOW_BAMOpen(&FPDConfig);
                VERIFY(IO_Handle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);

                IO_RetVal = Do_IO_PositionedWrite(IO_Handle, TA_ARB_TOC_Data_p, (uint64)TA_ARB_TOC_DataSize, (uint64)TA_ARB_TOC_DataSize, 0);
                CLOSE_HANDLE(IO_Handle);

                if (IO_RetVal == IO_ERROR) {
                    A_(printf("flash_applications_functions.c(%d): Do_IO_PositionedWrite failed !!!\n", __LINE__);)
                    ReturnValue = E_IO_FAILED_TO_READ;
                    goto ErrorExit;
                }
            }
        }
    } else {
        C_(printf("flash_applications_functions.c(%d): There are not any regions that needs to be protected.\n", __LINE__);)
        ReturnValue = Do_EraseAreaWithDriver(FlashDevice, AddressOffset, LengthBytes);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

ErrorExit:
    Destroy_EraseRegionsList(EraseRegionList_p);
    (void)Do_TOC_DestroyTOCList(Trim_ARB_TOC_List_p);
    BUFFER_FREE(TA_ARB_TOC_Data_p);
    BUFFER_FREE(SubTOC_BB1_p);

    return ReturnValue;

#undef COMPLETE_FLASH_ERASE
}

static ErrorCode_e Get_TA_ARB_TOCs(TOC_List_t **Trim_ARB_TOC_List_pp, FPD_Config_t *const FPDConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    TOC_List_t *TOC_List_p = NULL;
    TOC_List_t *SubTocList_p = NULL;
    TOC_List_t *SubTocNode_p = NULL;
    TOC_List_t *TOC_Entry_p = NULL;
    TOC_List_t *Trim_ARB_TOC_List_p = NULL;
    uint8 *BootArea_p = NULL;
    uint16 BootAreaSize = GetSizeForRootToc(FPDConfig_p->PageSize);

    //
    // Read the boot area and create TOC list
    //
    BootArea_p = malloc(BootAreaSize);
    VERIFY(NULL != BootArea_p, E_ALLOCATE_FAILED);

    ReturnValue = Do_BAM_Read(FPDConfig_p, BAM_BOOT_IMAGE_COPY_SIZE, BootArea_p, BootAreaSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    (void)Do_TOC_Create_RootList(&TOC_List_p, BootArea_p, BootAreaSize, TOC_BAMCOPY_1);

    if (NULL != TOC_List_p) {

        TOC_Entry_p = TOC_List_p;

        do {
            if ((strncmp(CSPSAString, TOC_Entry_p->entry.TOC_Id, strlen(CSPSAString)) == 0) ||
                    (strncmp(ARB_TBL_String, TOC_Entry_p->entry.TOC_Id, strlen(ARB_TBL_String)) == 0)) {
                VERIFY(Do_TOC_Add_RootList_Entry(&Trim_ARB_TOC_List_p, &TOC_Entry_p->entry), E_GENERAL_FATAL_ERROR);
            }
        } while ((TOC_Entry_p = Do_TOC_FindNextEntry(TOC_Entry_p)) != NULL);

        ReturnValue = ReadSubTOCFromFlash(TOC_List_p, FPDConfig_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        SubTocNode_p = Do_TOC_GetNext_SubTocNode(TOC_List_p);

        if (NULL != SubTocNode_p) {
            SubTocList_p = SubTocNode_p->SubItems;

            TOC_Entry_p = SubTocList_p;

            do {
                if ((strncmp(CSPSAString, TOC_Entry_p->entry.TOC_Id, strlen(CSPSAString)) == 0) ||
                        (strncmp(ARB_TBL_String, TOC_Entry_p->entry.TOC_Id, strlen(ARB_TBL_String)) == 0)) {

                    if (NULL == Trim_ARB_TOC_List_p) {
                        VERIFY(Do_TOC_Add_RootList_Entry(&Trim_ARB_TOC_List_p, &SubTocNode_p->entry), E_GENERAL_FATAL_ERROR);
                    }

                    VERIFY(Do_TOC_Add_SubTocList_Entry(Trim_ARB_TOC_List_p, SubTocNode_p->entry.TOC_Id, SubTocNode_p->entry.TOC_Offset, &TOC_Entry_p->entry), E_GENERAL_FATAL_ERROR);
                }
            } while ((TOC_Entry_p = TOC_Entry_p->next) != NULL);
        }
    }

ErrorExit:
    *Trim_ARB_TOC_List_pp = Trim_ARB_TOC_List_p;

    (void)Do_TOC_DestroyTOCList(TOC_List_p);
    BUFFER_FREE(BootArea_p);

    return ReturnValue;
}

static EraseRegion_t *Create_EraseRegionsList(uint64 AddressOffset, uint64 LengthBytes, TOC_List_t *TOC_Entry_p)
{
    EraseRegion_t *EraseRegionsList_p = NULL;
    EraseRegion_t EraseRegion = {0, };

    while (NULL != TOC_Entry_p) {
        if ((TOC_Entry_p->entry.TOC_Offset >= AddressOffset) && (TOC_Entry_p->entry.TOC_Offset < (AddressOffset + LengthBytes))) {
            EraseRegion.StartAddress = AddressOffset;
            EraseRegion.LengthBytes = TOC_Entry_p->entry.TOC_Offset - AddressOffset;

            if (FALSE == EraseRegions_Add_Entry(&EraseRegionsList_p, &EraseRegion)) {
                A_(printf("flash_applications_functions.c(%d): Failed to put new entry in Erase Region list!!!\n", __LINE__);)
                Destroy_EraseRegionsList(EraseRegionsList_p);
                EraseRegionsList_p = NULL;
                goto Exit;
            }

            LengthBytes -= MIN((EraseRegion.LengthBytes + TOC_Entry_p->entry.TOC_Size), LengthBytes);
            AddressOffset = TOC_Entry_p->entry.TOC_Offset + TOC_Entry_p->entry.TOC_Size;

            if (LengthBytes == 0) {
                C_(printf("flash_applications_functions.c(%d): Address offset placed on Trim Area region.\n", __LINE__);)
                goto Exit;
            }
        }

        TOC_Entry_p = Do_TOC_FindNextEntry(TOC_Entry_p);
    }

    EraseRegion.StartAddress = AddressOffset;
    EraseRegion.LengthBytes = LengthBytes;

    if (FALSE == EraseRegions_Add_Entry(&EraseRegionsList_p, &EraseRegion)) {
        A_(printf("flash_applications_functions.c(%d): Failed to put new entry in Erase Region list!!!\n", __LINE__);)
        Destroy_EraseRegionsList(EraseRegionsList_p);
        EraseRegionsList_p = NULL;
        goto Exit;
    }

Exit:
    return EraseRegionsList_p;
}

static boolean EraseRegions_Add_Entry(EraseRegion_t **List_pp, EraseRegion_t *NewData_p)
{
    EraseRegion_t **iter;
    EraseRegion_t *newest;

    if (!NewData_p) {
        return FALSE;
    }

    newest = (EraseRegion_t *)malloc(sizeof(EraseRegion_t));

    if (NULL == newest) {
        A_(printf("flash_application_functions.c(%d): **ERR Memory allocation fail **\n", __LINE__);)
        return FALSE;
    }

    iter = List_pp;

    while (NULL != (*iter)) {
        iter = &((*iter)->next);
    }

    // Now iter points on a pointer that points to NULL.
    *iter = newest;

    (*iter)->LengthBytes = NewData_p->LengthBytes;
    (*iter)->StartAddress = NewData_p->StartAddress;
    (*iter)->next = NULL;

    return TRUE;
}

static void Destroy_EraseRegionsList(EraseRegion_t *ListHead)
{
    EraseRegion_t *head;
    EraseRegion_t *iter;

    head = ListHead;

    while (NULL != head) {
        iter = head->next;
        free(head);
        head = iter;
    }
}

ErrorCode_e Do_EraseAreaWithDriver(const uint8 FlashDevice, uint64 AddressOffset, uint64 LengthBytes)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint64 EndAddress = 0;
    uint64 AlignmentMask = 0;
    uint64 AlignedAddressOffset = 0;
    uint64 AlignedEndAddress = 0;
    uint32 StartPage = 0;
    uint32 NrOfEraseBlocks = 0;
    FPD_Config_t FPDConfig = {0,};

    ReturnValue = Do_FPD_GetInfo(FlashDevice, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, E_INVALID_INPUT_PARAMETERS);

    C_(printf("flash_applications_functions.c (%d): ** Do_EraseAreaWithDriver **\n", __LINE__);)
    C_(printf("flash_applications_functions.c (%d): AddressOffset = 0x%08x\n", __LINE__, (uint32)AddressOffset);)
    C_(printf("flash_applications_functions.c (%d): Length = 0x%08x\n", __LINE__, (uint32)LengthBytes);)
    C_(printf("flash_applications_functions.c (%d): PageSize = %d\n", __LINE__, FPDConfig.PageSize);)
    C_(printf("flash_applications_functions.c (%d): EraseBlockSize = %d\n", __LINE__, FPDConfig.EraseBlockSize);)

    AlignmentMask = ~((uint64)FPDConfig.EraseBlockSize - 1);

    AlignedAddressOffset = (AddressOffset + FPDConfig.EraseBlockSize - 1) & AlignmentMask;
    EndAddress = AddressOffset + LengthBytes;
    AlignedEndAddress = EndAddress & AlignmentMask;
    C_(printf("Aligned parameters AddressOffset = 0x%08x, EndAddress = 0x%08x\n", (uint32)AlignedAddressOffset, (uint32)AlignedEndAddress);)

    ReturnValue = FillAreaWithContent(AddressOffset, MIN(EndAddress, AlignedAddressOffset) - AddressOffset, FPDConfig.ErasedMemContent, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    /* TODO: In NAND case, implement erasing block by block in FPD to handle bad blocks */
    if (AlignedAddressOffset < AlignedEndAddress) {
        /* Convert parameters expressed in bytes to parameters expressed into number of pages. */
        StartPage = AlignedAddressOffset / FPDConfig.PageSize;
        NrOfEraseBlocks = (AlignedEndAddress - AlignedAddressOffset) / FPDConfig.EraseBlockSize;
        C_(printf("Aligned FPD erase from page 0x%08x, 0x%08x blocks!\n", (uint32)StartPage, (uint32)NrOfEraseBlocks);)
        ReturnValue = Do_FPD_Erase(StartPage, NrOfEraseBlocks, FlashDevice);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    if (EndAddress != 0 && AlignedEndAddress >= AlignedAddressOffset) {
        ReturnValue = FillAreaWithContent(AlignedEndAddress, EndAddress - AlignedEndAddress, FPDConfig.ErasedMemContent, &FPDConfig);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

ErrorExit:
    C_(printf("flash_applications_functions.c (%d): ** Exit Do_EraseAreaWithDriver Result(%d) **\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

static ErrorCode_e FillAreaWithContent(uint64 BytesOffset, uint64 BytesLength, uint8 AreaContent, FPD_Config_t *const FPDConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *DataBuffer_p = NULL;
    IO_Handle_t WriteHandle = IO_INVALID_HANDLE;
    uint64 BytesWritten = IO_ERROR;

    if (0 == BytesLength) {
        C_(printf("flash_applications_functions.c (%d): Fill area length is 0, nothing to fill\n", __LINE__);)
        ReturnValue = E_SUCCESS;
        goto ErrorExit;
    }

    C_(printf("Filling area before aligned area with %02X\n", AreaContent);)
    C_(printf("Filling area: Start = 0x%08x; Length = 0x%08x; Content = 0x%02x\n", (uint32)BytesOffset, (uint32)BytesLength, AreaContent);)

    DataBuffer_p = (uint8 *)malloc(BytesLength);
    VERIFY(NULL != DataBuffer_p, E_ALLOCATE_FAILED);
    memset(DataBuffer_p, AreaContent, BytesLength);

    WriteHandle = Do_IOW_BDMOpen(FPDConfig_p);
    VERIFY(IO_INVALID_HANDLE != WriteHandle, E_IO_FAILED_TO_OPEN);

    BytesWritten = Do_IO_PositionedWrite(WriteHandle, DataBuffer_p, BytesLength, BytesLength, BytesOffset);
    VERIFY(BytesLength == BytesWritten, E_IO_FAILED_TO_WRITE);

    ReturnValue = E_SUCCESS;

ErrorExit:
    CLOSE_HANDLE(WriteHandle);
    BUFFER_FREE(DataBuffer_p);

    return ReturnValue;
}

void ListDevices_FreeAllocatedSpace(ListDevice_t *Devices_p)
{
    uint32 i = 0;

    for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++) {
        BUFFER_FREE(Devices_p[i].Path_p);
        BUFFER_FREE(Devices_p[i].Type_p);
    }
}

static ErrorCode_e GetDevicePathForTOCEntry(TOC_List_t *Entry_p, char *Path_p, const uint8 FlashDevice)
{
    uint8 Flash;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    int amount_of_char = 0;
    char Temp_TOC_ID[TOC_ID_LENGTH + 1];

    VERIFY(NULL != Entry_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != Path_p, E_INVALID_INPUT_PARAMETERS);

    if (Entry_p->Parent && Entry_p->SubItems) {
        A_(printf("flash_applications_functions.c (%d): More than two levels of TOCs detected!\n", __LINE__));
        goto ErrorExit;
    }

    Flash = ((Entry_p->entry.TOC_Flags >> 16) & 0xFF);

    strncpy(Temp_TOC_ID, Entry_p->entry.TOC_Id, TOC_ID_LENGTH);
    Temp_TOC_ID[TOC_ID_LENGTH] = '\0';

    if (Entry_p->entry.TOC_Offset < BAM_BOOT_IMAGE_COPY_SIZE && Flash == FlashDevice) {
        amount_of_char = snprintf(Path_p, MAX_DEVICE_PATH_LENGTH, "/flash%d/boot0/TOC/%s", Flash, Temp_TOC_ID);
        VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
        Path_p[amount_of_char] = '\0';
    } else {
        if (!Entry_p->Parent) {
            amount_of_char = snprintf(Path_p, MAX_DEVICE_PATH_LENGTH, "/flash%d/TOC/%s", Flash, Temp_TOC_ID);
            VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
            Path_p[amount_of_char] = '\0';
        } else {
            char Temp_SubTOC_ID[TOC_ID_LENGTH + 1];
            strncpy(Temp_SubTOC_ID, Entry_p->Parent->entry.TOC_Id, TOC_ID_LENGTH);
            Temp_SubTOC_ID[TOC_ID_LENGTH] = '\0';

            amount_of_char = snprintf(Path_p, MAX_DEVICE_PATH_LENGTH, "/flash%d/TOC/%s/%s", Flash, Temp_SubTOC_ID, Temp_TOC_ID);
            VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
            Path_p[amount_of_char] = '\0';
        }
    }

    ReturnValue = E_SUCCESS;

ErrorExit:

    return ReturnValue;
}

ErrorCode_e Read_SubTOC_Data(uint32 Offset, uint32 Size, uint8 *Data_p, uint32 BufferSize, FPD_Config_t *const FPDConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    IO_Handle_t ReadHandle  = IO_INVALID_HANDLE;
    uint64      BytesRead   = IO_ERROR;

    VERIFY(BufferSize >= Size, E_INVALID_INPUT_PARAMETERS);

    /* Read SUB TOC data */
    if (Offset < BAM_BOOT_IMAGE_COPY_SIZE) {
        ReadHandle = Do_IOW_BAMOpen(FPDConfig_p);
        VERIFY(ReadHandle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);
    } else {
        ReadHandle = Do_IOW_BDMOpen(FPDConfig_p);
        VERIFY(ReadHandle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);
    }

    BytesRead = Do_IO_PositionedRead(ReadHandle, Data_p, Size, Size, Offset);
    VERIFY(BytesRead != IO_ERROR, E_IO_FAILED_TO_READ);

    ReturnValue = E_SUCCESS;

ErrorExit:

    CLOSE_HANDLE(ReadHandle);

    return ReturnValue;
}

static ErrorCode_e ReadSubTOCFromFlash(TOC_List_t *List_p, FPD_Config_t *const FPDConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    IO_Handle_t ReadHandle  = IO_INVALID_HANDLE;
    TOC_List_t *Node_p      = NULL;
    uint32      Size        = 0;
    uint32      Offset      = 0;
    uint8      *Data_p      = NULL;

    VERIFY(NULL != List_p, E_INVALID_INPUT_PARAMETERS);

    C_(printf("flash_applications_functions.c (%d): TOC list found on flash\n", __LINE__);)

    // Get SubTOC entry from list
    Node_p = Do_TOC_GetNext_SubTocNode(List_p);

    if (Node_p != NULL) {
        B_(printf("flash_applications_functions.c (%d): SubTOC entry found, reading SubTOC\n", __LINE__);)
        Offset = Node_p->entry.TOC_Offset;
        Size   = Node_p->entry.TOC_Size;

        if ((Offset != 0xFFFFFFFF) && (Size != 0)) {
            Data_p = (uint8 *)malloc(Size);
            VERIFY(NULL != Data_p, E_ALLOCATE_FAILED);

            ReturnValue = Read_SubTOC_Data(Offset, Size, Data_p, Size, FPDConfig_p);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            /* Extract SUB TOC list and link it with root list */
            if (!Do_TOC_Add_SubTocList(List_p, Data_p, Size, Node_p->entry.TOC_Id, Offset)) {
                A_(printf("flash_applications_functions.c (%d): Failed to add SubTOC list\n");)
                ReturnValue  = E_INVALID_SUBTOC_PARAMETERS;
                goto ErrorExit;
            }

            B_(

            if (Do_TOC_GetNext_SubTocList(List_p) == NULL) {
            printf("flash_applications_functions.c (%d): Cannot create SubTOC from data at offset 0x%08x\n", __LINE__, Offset);
            }
            )
        }
    } else {
        B_(printf("flash_applications_functions.c (%d): SubTOC entry not found\n", __LINE__);)
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    BUFFER_FREE(Data_p);
    CLOSE_HANDLE(ReadHandle);

    return ReturnValue;
}

static ErrorCode_e EraseTOCPartition(DeviceInfo_t *DeviceInfo, uint64 Start, uint64 Length, boolean RemovePartitionFromTOC)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    BootAreaLayout_e BootAreaLayout = BOOT_AREA_LAYOUT_UNKNOWN;
    TOC_List_t *TOC_List_p = NULL;
    TOC_List_t *TOC_Entry_p = NULL;
    TOC_List_t *SubTocNode_p = NULL;
    uint64 PartitionStart = 0;
    uint64 PartitionSize = 0;
    uint8 *BootArea_p = NULL;
    FPD_Config_t FPDConfig = {0,};

    ReturnValue = Do_FPD_GetInfo(DeviceInfo->DeviceId, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = DetectBootAreaLayout(DeviceInfo, &BootAreaLayout);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    VERIFY((BOOT_AREA_LAYOUT_V2 == BootAreaLayout) || (BOOT_AREA_LAYOUT_V3 == BootAreaLayout), E_GENERAL_FATAL_ERROR);

    // read the boot area and create TOC list
    BootArea_p = malloc(BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(NULL != BootArea_p, E_ALLOCATE_FAILED);

    ReturnValue = Do_BAM_Read(&FPDConfig, BAM_BOOT_IMAGE_COPY_SIZE, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_TOC_Create_RootList(&TOC_List_p, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, TOC_BAMCOPY_1);
    VERIFY(E_TOC_HANDLER_INPUT_DATA_EMPTY != ReturnValue, E_SUCCESS);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = ReadSubTOCFromFlash(TOC_List_p, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    // First of all, check if the SUBTOCId string fetched in the path is valid SUBTOC entry
    if (strlen(DeviceInfo->SUBTOCId)) {

        TOC_Entry_p = Do_TOC_LocateEntryByNodeId(TOC_List_p, DeviceInfo->SUBTOCId);

        if (NULL == TOC_Entry_p) {
            B_(printf("flash_applications_functions.c (%d) SUBTOC:[%s] not found in the TOC list\n", __LINE__, DeviceInfo->SUBTOCId);)
            ReturnValue = E_INVALID_INPUT_PARAMETERS;
            goto ErrorExit;
        }


        if (!Do_TOC_IsEntrySubToc(&TOC_Entry_p->entry)) {
            B_(printf("flash_applications_functions.c (%d): Invalid SUBTOC:[%s] found in path\n", __LINE__, DeviceInfo->SUBTOCId);)
            ReturnValue = E_INVALID_INPUT_PARAMETERS;
            goto ErrorExit;
        }
    }

    TOC_Entry_p = Do_TOC_LocateEntryByNodeId(TOC_List_p, DeviceInfo->TOCId); // This can find an image entry or a partition entry so the size to erase can vary

    if (NULL == TOC_Entry_p) {
        //This is expected NULL value since we try to erase Entry that has been already erased, therefore we set the ReturnValue to E_SUCCESS
        ReturnValue = E_SUCCESS;
        B_(printf("flash_applications_functions.c (%d) WARNING: Trying to erase non-existing partition. \n", __LINE__);)
        goto ErrorExit;
    }

    PartitionStart = TOC_Entry_p->entry.TOC_Offset;
    PartitionSize = TOC_Entry_p->entry.TOC_Size;

    if (PartitionStart < BAM_BOOT_IMAGE_COPY_SIZE) {
        // fill the partition area with erased content
        memset(BootArea_p + PartitionStart, FPDConfig.ErasedMemContent, PartitionSize);
    } else {
        if (0 == Start && -1 == Length) {
            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, PartitionStart, PartitionSize);
        } else {
            VERIFY(Start < PartitionSize, E_INVALID_INPUT_PARAMETERS);
            VERIFY(Length < PartitionSize - Start, E_INVALID_INPUT_PARAMETERS);
            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, PartitionStart + Start, Length);
        }

        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

    if (BootAreaLayout == BOOT_AREA_LAYOUT_V3) {
        SubTocNode_p = Do_TOC_GetNext_SubTocNode(TOC_List_p);

        if ((NULL != SubTocNode_p) && (SubTocNode_p->SubItems != NULL)) {
            if ((TOC_Entry_p->entry.TOC_Offset - SubTocNode_p->entry.TOC_Offset) < BAM_BOOT_BLOCK_SIZE) {
                ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, PartitionStart + BAM_BOOT_BLOCK_SIZE, PartitionSize);
                VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            }
        } else {
            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, PartitionStart + BAM_BOOT_BLOCK_SIZE, PartitionSize);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        }
    }

    if (RemovePartitionFromTOC) {
        char TOC_Id[TOC_ID_LENGTH + 1];

        strncpy(TOC_Id, TOC_Entry_p->entry.TOC_Id, TOC_ID_LENGTH);
        TOC_Id[TOC_ID_LENGTH] = '\0';

        C_(printf("flash_applications_functions.c (%d): Removing TOC entry %s\n", __LINE__, TOC_Id);)

        VERIFY(Do_TOC_Remove_EntryByNodeId(&TOC_List_p, TOC_Id), E_GENERAL_FATAL_ERROR);

        if (!Do_TOC_Remove_EntryByNodeId(&TOC_List_p, TOC_Id)) {
            C_(printf("Optional TOC entry could not be removed\n");)
        }

        if (BootAreaLayout == BOOT_AREA_LAYOUT_V2) {
            ReturnValue = UpdateBootAreaV2(DeviceInfo, TOC_List_p, BootArea_p);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        } else {
            ReturnValue = UpdateBootAreaV3(DeviceInfo, TOC_List_p, TOC_Id);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        }
    }

ErrorExit:
    (void)Do_TOC_DestroyTOCList(TOC_List_p);
    BUFFER_FREE(BootArea_p);

    return ReturnValue;
}

static ErrorCode_e UpdateBootAreaV3(DeviceInfo_t *DeviceInfo, TOC_List_t *TocList_p, char *TocId_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    IO_Handle_t IO_Handle = IO_INVALID_HANDLE;
    TOC_List_t *TocListSecondCopy_p = NULL;
    TOC_List_t *SubTocNode_p = NULL;
    uint64 BytesWritten = IO_ERROR;
    uint8 *BootArea_p = NULL;
    uint8 *BootAreaSecondCopy_p = NULL;
    uint8 *Data_p = NULL;
    uint32 DataSize = 0;
    FPD_Config_t FPDConfig;

    ReturnValue = Do_FPD_GetInfo(DeviceInfo->DeviceId, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    IO_Handle = Do_IOW_BDMOpen(&FPDConfig);
    VERIFY(IO_INVALID_HANDLE != IO_Handle, E_IO_FAILED_TO_OPEN);

    SubTocNode_p = Do_TOC_GetNext_SubTocNode(TocList_p);

    if (SubTocNode_p != NULL) {
        //
        // If SUBTOC node doesn't contain any links to sub items, delete SUBTOC area in
        // boot blocks and move on.
        //
        if (SubTocNode_p->SubItems == NULL) {
            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, SubTocNode_p->entry.TOC_Offset, SubTocNode_p->entry.TOC_Size);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, SubTocNode_p->entry.TOC_Offset + BAM_BOOT_BLOCK_SIZE, SubTocNode_p->entry.TOC_Size);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            SubTocNode_p->entry.TOC_Size = 0;
        } else {
            //
            // Update first copy of SUBTOC
            //
            Data_p = Do_TOC_CreateTOCDataFromList(SubTocNode_p->SubItems, &DataSize);
            VERIFY((NULL != Data_p) && (DataSize > 0), E_TOC_LIST_CORRUPTED);

            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, SubTocNode_p->entry.TOC_Offset, SubTocNode_p->entry.TOC_Size);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            BytesWritten = Do_IO_PositionedWrite(IO_Handle, Data_p, DataSize, DataSize, SubTocNode_p->entry.TOC_Offset);
            VERIFY(IO_ERROR != BytesWritten, E_IO_FAILED_TO_WRITE);
            C_(printf("flash_applications_functions.c (%d): Updated SubTOC at offset 0x%08x\n", __LINE__, SubTocNode_p->entry.TOC_Offset);)

            BUFFER_FREE(Data_p);
            SubTocNode_p->entry.TOC_Size = DataSize;

            //
            // Update second copy of SUBTOC
            //
            BootAreaSecondCopy_p = malloc(BAM_BOOT_IMAGE_COPY_SIZE);
            VERIFY(NULL != BootAreaSecondCopy_p, E_ALLOCATE_FAILED);

            ReturnValue = Do_BAM_Read(&FPDConfig, BOOT_IMAGE_THIRD_COPY_OFFSET, BootAreaSecondCopy_p, BAM_BOOT_IMAGE_COPY_SIZE);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            ReturnValue = Do_TOC_Create_RootList(&TocListSecondCopy_p, BootAreaSecondCopy_p, BAM_BOOT_IMAGE_COPY_SIZE, TOC_BAMCOPY_2);
            VERIFY(E_TOC_HANDLER_INPUT_DATA_EMPTY != ReturnValue, E_TOC_LIST_CORRUPTED);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            ReturnValue = ReadSubTOCFromFlash(TocListSecondCopy_p, &FPDConfig);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            VERIFY(Do_TOC_Remove_EntryByNodeId(&TocListSecondCopy_p, TocId_p), E_GENERAL_FATAL_ERROR);

            SubTocNode_p = Do_TOC_GetNext_SubTocNode(TocListSecondCopy_p);
            VERIFY((NULL != SubTocNode_p) && (SubTocNode_p->SubItems != NULL), E_TOC_LIST_CORRUPTED);

            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, SubTocNode_p->entry.TOC_Offset, SubTocNode_p->entry.TOC_Size);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            Data_p = Do_TOC_CreateTOCDataFromList(SubTocNode_p->SubItems, &DataSize);
            VERIFY((NULL != Data_p) && (DataSize > 0), E_TOC_LIST_CORRUPTED);

            BytesWritten = Do_IO_PositionedWrite(IO_Handle, Data_p, DataSize, DataSize, SubTocNode_p->entry.TOC_Offset);
            VERIFY(IO_ERROR != BytesWritten, E_IO_FAILED_TO_WRITE);
            C_(printf("flash_applications_functions.c (%d): Updated SubTOC at offset 0x%08x\n", __LINE__, SubTocNode_p->entry.TOC_Offset);)

            BUFFER_FREE(Data_p);
        }
    }

    BootArea_p = malloc(BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(NULL != BootArea_p, E_ALLOCATE_FAILED);

    memset(BootArea_p, FPDConfig.ErasedMemContent, BAM_BOOT_IMAGE_COPY_SIZE);

    //
    // If TOC list is empty just erase root boot area
    //
    if (TocList_p) {
        // Update root toc area (both copies)
        Data_p = Do_TOC_CreateTOCDataFromList(TocList_p, &DataSize);
        VERIFY((NULL != Data_p) && (DataSize > 0), E_TOC_LIST_CORRUPTED);

        UpdateTOCInWriteData(Data_p, TocList_p, 1, 0);
        memcpy(BootArea_p, Data_p, DataSize);

        // Write first copy of ROOT TOC
        BytesWritten = Do_IO_PositionedWrite(IO_Handle, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, BAM_BOOT_IMAGE_COPY_SIZE, BOOT_IMAGE_SECOND_COPY_OFFSET);
        VERIFY(BytesWritten != IO_ERROR, E_IO_FAILED_TO_READ);

        UpdateTOCInWriteData(Data_p, TocList_p, 2, BAM_BOOT_BLOCK_SIZE);
        memcpy(BootArea_p, Data_p, DataSize);

        // Write second copy of ROOT TOC
        BytesWritten = Do_IO_PositionedWrite(IO_Handle, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, BAM_BOOT_IMAGE_COPY_SIZE, BOOT_IMAGE_THIRD_COPY_OFFSET);
        VERIFY(BytesWritten != IO_ERROR, E_IO_FAILED_TO_READ);
    } else {
        // Write first copy of ROOT TOC
        BytesWritten = Do_IO_PositionedWrite(IO_Handle, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, BAM_BOOT_IMAGE_COPY_SIZE, BOOT_IMAGE_SECOND_COPY_OFFSET);
        VERIFY(BytesWritten != IO_ERROR, E_IO_FAILED_TO_READ);

        // Write first copy of ROOT TOC
        BytesWritten = Do_IO_PositionedWrite(IO_Handle, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, BAM_BOOT_IMAGE_COPY_SIZE, BOOT_IMAGE_THIRD_COPY_OFFSET);
        VERIFY(BytesWritten != IO_ERROR, E_IO_FAILED_TO_READ);
    }

ErrorExit:
    (void)Do_TOC_DestroyTOCList(TocListSecondCopy_p);
    CLOSE_HANDLE(IO_Handle);
    BUFFER_FREE(BootAreaSecondCopy_p);
    BUFFER_FREE(BootArea_p);
    BUFFER_FREE(Data_p);

    return ReturnValue;
}

static ErrorCode_e UpdateBootAreaV2(DeviceInfo_t *DeviceInfo, TOC_List_t *TocList_p, uint8 *BootArea_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    IO_Handle_t WriteHandle = IO_INVALID_HANDLE;
    uint64 BytesWritten = IO_ERROR;
    TOC_List_t *SubTocNode_p = NULL;
    uint8 *Data_p = NULL;
    uint32 DataSize = 0;
    FPD_Config_t FPDConfig;

    ReturnValue = Do_FPD_GetInfo(DeviceInfo->DeviceId, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    SubTocNode_p = Do_TOC_GetNext_SubTocNode(TocList_p);

    if ((NULL != SubTocNode_p) && (SubTocNode_p->SubItems != NULL)) {
        Data_p = Do_TOC_CreateTOCDataFromList(SubTocNode_p->SubItems, &DataSize);
        VERIFY((NULL != Data_p) && (DataSize > 0), E_TOC_LIST_CORRUPTED);

        // SubTOC located in boot area, update existing boot area
        if (SubTocNode_p->entry.TOC_Offset < BAM_BOOT_IMAGE_COPY_SIZE) {
            // fill the SubTOC area in buffer with ErasedMemContent, and copy the updated area
            memset(BootArea_p + SubTocNode_p->entry.TOC_Offset, FPDConfig.ErasedMemContent, SubTocNode_p->entry.TOC_Size);
            memcpy(BootArea_p + SubTocNode_p->entry.TOC_Offset, Data_p, DataSize);
        } else {
            // erase SubTOC area before writing the updated SubTOC
            ReturnValue = Do_EraseAreaWithDriver(DeviceInfo->DeviceId, SubTocNode_p->entry.TOC_Offset, SubTocNode_p->entry.TOC_Size);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            WriteHandle = Do_IOW_BDMOpen(&FPDConfig);
            VERIFY(IO_INVALID_HANDLE != WriteHandle, E_IO_FAILED_TO_OPEN);

            BytesWritten = Do_IO_PositionedWrite(WriteHandle, Data_p, DataSize, DataSize, SubTocNode_p->entry.TOC_Offset);
            VERIFY(IO_ERROR != BytesWritten, E_IO_FAILED_TO_WRITE);
            B_(printf("flash_applications_functions.c (%d): Updated SubTOC at offset 0x%08x\n", __LINE__, SubTocNode_p->entry.TOC_Offset);)

            CLOSE_HANDLE(WriteHandle);
        }

        BUFFER_FREE(Data_p);

        // update the length of SubTOC in root TOC
        SubTocNode_p->entry.TOC_Size = DataSize;
    }

    Data_p = Do_TOC_CreateTOCDataFromList(TocList_p, &DataSize);

    // erase the old TOC
    memset(BootArea_p, FPDConfig.ErasedMemContent, 512);

    if (NULL != Data_p) {
        // we have a new TOC, copy the TOC to boot area
        memcpy(BootArea_p, Data_p, DataSize);
    }

    WriteHandle = Do_IOW_BAMOpen(&FPDConfig);
    VERIFY(WriteHandle != IO_INVALID_HANDLE, E_IO_FAILED_TO_OPEN);

    BytesWritten = Do_IO_PositionedWrite(WriteHandle, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, BAM_BOOT_IMAGE_COPY_SIZE, 0);
    VERIFY(IO_ERROR != BytesWritten, E_IO_FAILED_TO_WRITE);

ErrorExit:
    CLOSE_HANDLE(WriteHandle);
    BUFFER_FREE(Data_p);

    return ReturnValue;
}

static ErrorCode_e DetectBootAreaLayout(DeviceInfo_t *DeviceInfo, BootAreaLayout_e *BootAreaLayout_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    TOC_List_t *TocListBlockOne_p = NULL;
    TOC_List_t *TocListBlockTwo_p = NULL;
    TOC_List_t *TocListEntry_p = NULL;
    TOC_List_t *SubTocNodeBlockOne_p = NULL;
    TOC_List_t *SubTocNodeBlockTwo_p = NULL;
    uint8 *BootAreaCopy1_p = NULL;
    uint8 *BootAreaCopy2_p = NULL;
    FPD_Config_t FPDConfig = {0,};
    uint32 NrOfEntriesInRootToc = 0;

    *BootAreaLayout_p = BOOT_AREA_LAYOUT_UNKNOWN;

    ReturnValue = Do_FPD_GetInfo(DeviceInfo->DeviceId, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    *BootAreaLayout_p = BOOT_AREA_EMPTY;

    //
    // Read first copy of root boot area (counting from zero) and create TOC list
    //
    BootAreaCopy1_p = malloc(BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(NULL != BootAreaCopy1_p, E_ALLOCATE_FAILED);

    ReturnValue = Do_BAM_Read(&FPDConfig, BOOT_IMAGE_SECOND_COPY_OFFSET, BootAreaCopy1_p, BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_TOC_Create_RootList(&TocListBlockOne_p, BootAreaCopy1_p, BAM_BOOT_IMAGE_COPY_SIZE, TOC_BAMCOPY_1);
    VERIFY(E_TOC_HANDLER_INPUT_DATA_EMPTY != ReturnValue, E_SUCCESS);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    TocListEntry_p = TocListBlockOne_p;

    //
    // Check if root TOC contains more than tree entries. If so, we can conclude that
    // we have V2 layout of boot area.
    //
    do {
        NrOfEntriesInRootToc++;
    } while ((TocListEntry_p = Do_TOC_FindNextEntry(TocListEntry_p)) != NULL);

    if (NrOfEntriesInRootToc > MAX_ROOT_TOC_ENTRIES_IN_LAYOUT_V3) {
        C_(printf("flash_applications_functions.c (%d): %d TOC entries in root TOC list. Boot area layout V2\n", __LINE__, NrOfEntriesInRootToc);)
        *BootAreaLayout_p = BOOT_AREA_LAYOUT_V2;
        goto ErrorExit;
    }

    //
    // Check if "ISSW" TOC is present in root TOC list. If ISSW image is placed in root boot area
    // we can conclude that it is boot area layout version 2. Otherwise it is boot area layout
    // version 3.
    //
    TocListEntry_p = Do_TOC_LocateEntryByNodeId(TocListBlockOne_p, ISSWString);

    if (NULL != TocListEntry_p) {
        if (TocListEntry_p->entry.TOC_Offset < BAM_BOOT_IMAGE_COPY_SIZE) {
            C_(printf("flash_applications_functions.c (%d): ISSW TOC reside in root boot area. Boot area layout V2\n", __LINE__);)
            *BootAreaLayout_p = BOOT_AREA_LAYOUT_V2;
            goto ErrorExit;
        } else {
            C_(printf("flash_applications_functions.c (%d): ISSW TOC reside outside of root boot area. Boot area layout V3\n", __LINE__);)
            *BootAreaLayout_p = BOOT_AREA_LAYOUT_V3;
            goto ErrorExit;
        }
    }

    //
    // Check if SUBTOC node is present in TOC list. If root TOC doesn't contain SUBTOC nor ISSW
    // (which we have checked in previous section) it is save to assume that we have boot area
    // layout version 2
    //
    SubTocNodeBlockOne_p = Do_TOC_GetNext_SubTocNode(TocListBlockOne_p);

    if (SubTocNodeBlockOne_p == NULL) {
        C_(printf("flash_applications_functions.c (%d): Root TOC list doesn't contain ISSW nor SUBTOC. Boot area layout V2\\n", __LINE__);)
        *BootAreaLayout_p = BOOT_AREA_LAYOUT_V2;
        goto ErrorExit;
    }

    *BootAreaLayout_p = BOOT_AREA_LAYOUT_UNKNOWN;

    //
    // Read second copy of root boot area (counting from zero) and create TOC list
    //
    BootAreaCopy2_p = malloc(BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(NULL != BootAreaCopy2_p, E_ALLOCATE_FAILED);

    ReturnValue = Do_BAM_Read(&FPDConfig, BOOT_IMAGE_THIRD_COPY_OFFSET, BootAreaCopy2_p, BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_TOC_Create_RootList(&TocListBlockTwo_p, BootAreaCopy2_p, BAM_BOOT_IMAGE_COPY_SIZE, TOC_BAMCOPY_2);

    if (ReturnValue != E_SUCCESS) {
        B_(printf("flash_applications_functions.c (%d): Boot area layout version cannot be detected\n", __LINE__);)
        *BootAreaLayout_p = BOOT_AREA_LAYOUT_UNKNOWN;
        goto ErrorExit;
    }

    //
    // If SUBTOC node cannot be found in second root boot block than we have desynchronization between
    // root boot copies in boot area.
    //
    SubTocNodeBlockTwo_p = Do_TOC_GetNext_SubTocNode(TocListBlockTwo_p);

    if (SubTocNodeBlockTwo_p == NULL) {
        B_(printf("flash_applications_functions.c (%d): Mismatch between first and second copy of boot block\n", __LINE__);)
        B_(printf("flash_applications_functions.c (%d): Boot area layout version cannot be detected\n", __LINE__);)
        ReturnValue = E_GENERAL_FATAL_ERROR; // Add new error code E_BOOT_AREA_CORRUPTED
        *BootAreaLayout_p = BOOT_AREA_LAYOUT_UNKNOWN;
        goto ErrorExit;
    }

    //
    // Check if memory space between SUBTOCs is BAM_BOOT_BLOCK_SIZE. If yes we can safely assume that
    // that we have boot area layout version 3 and version 2 otherwise.
    //
    if ((SubTocNodeBlockTwo_p->entry.TOC_Offset - SubTocNodeBlockOne_p->entry.TOC_Offset) == BAM_BOOT_BLOCK_SIZE) {
        C_(printf("flash_applications_functions.c (%d): Boot area Layout V3 detected\n", __LINE__);)
        *BootAreaLayout_p = BOOT_AREA_LAYOUT_V3;
    } else {
        C_(printf("flash_applications_functions.c (%d): Boot area Layout V2 detected\n", __LINE__);)
        *BootAreaLayout_p = BOOT_AREA_LAYOUT_V2;
    }

ErrorExit:
    (void)Do_TOC_DestroyTOCList(TocListBlockOne_p);
    (void)Do_TOC_DestroyTOCList(TocListBlockTwo_p);
    BUFFER_FREE(BootAreaCopy1_p);
    BUFFER_FREE(BootAreaCopy2_p);
    return ReturnValue;
}

static ErrorCode_e Fetch_Device_Attributes(void *DataTypeStructure_p, FetchDeviceMode_t FetchMode)
{

    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = NULL;
    ListDevice_t Devices[MAX_NUMBER_OF_DEVICES];
    uint32 DeviceNr = 0;
    CommDevId_t DeviceType;
    uint8 *BootArea_p = NULL;
    TOC_List_t *TOC_Entry_p = NULL;
    BAM_Config_t BAM_Config = {0};
    FPD_Config_t FPD_Config;
    uint32 i = 0;
    uint8 available_flash_devices = 0;
    uint32 RootTocNeededSize;
    CommunicationDevice_t *CommunicationDevice_p = NULL;
    device_path_table_t *path_table_p = NULL;

    TOC_List_t *TOC_List_p = NULL;
    char *TempPath_p = NULL;

    /*
     *  Check the Fetch mode in which is running
     */
    if (FetchMode == DEST_SERIALIZED_TO_PC) {
        ExeContext_p = (ExecutionContext_t *) DataTypeStructure_p;
    } else if (FetchMode == DEST_STORE_PATHS_IN_TABLE) {
        path_table_p = (device_path_table_t *) DataTypeStructure_p;
    }

    for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++) {
        memset(&Devices[i], 0x00, sizeof(ListDevice_t));
    }

    /*
     * Get communication device.
     */
    ReturnValue = Do_Communication_GetCommunicationDevice(GlobalCommunication_p, &CommunicationDevice_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    DeviceType = ((CommDev_t *)CommunicationDevice_p->Context_p)->Descriptor_p->DevType;

    switch (DeviceType) {
    case DEV_UART0:
        DEST_MODE_PC(insert_string(&Devices[DeviceNr].Path_p, DevUART0, strlen(DevUART0));)
        DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, DevUART0);)
        break;

    case DEV_UART1:
        DEST_MODE_PC(insert_string(&Devices[DeviceNr].Path_p, DevUART1, strlen(DevUART1));)
        DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, DevUART1);)
        break;

    case DEV_UART2:
        DEST_MODE_PC(insert_string(&Devices[DeviceNr].Path_p, DevUART2, strlen(DevUART2));)
        DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, DevUART2);)
        break;

    case DEV_USB:
        DEST_MODE_PC(insert_string(&Devices[DeviceNr].Path_p, DevUSB, strlen(DevUSB));)
        DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, DevUSB);)
        break;

    default:
        A_(printf("flash_applications_functions.c (%d): Unknown communication device!!!\n", __LINE__);)
        ReturnValue = E_UNKNOWN_COMM_DEVICE;
        goto ErrorExit;
    }

    DEST_MODE_PC(insert_string(&Devices[DeviceNr].Type_p, CommDevice, strlen(CommDevice));)
    DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, CommDevice);)

    /*
     * Get flash devices from services.
     */
    available_flash_devices = Do_FPD_GetNrOfAvailableFlashDevices();

    for (i = 0; i < available_flash_devices; i++) {
        char FlashPath[MAX_DEVICE_PATH_LENGTH];
        int amount_of_char = 0;

        memset(&FPD_Config, 0, sizeof(FPD_Config_t));
        ReturnValue = Do_FPD_GetInfo(i, &FPD_Config);
        VERIFY(ReturnValue == E_SUCCESS, ReturnValue);

        DeviceNr++;
        VERIFY(DeviceNr < MAX_NUMBER_OF_DEVICES, E_FLASH_APP_INTERNAL_ERROR);

        amount_of_char = snprintf(FlashPath, MAX_DEVICE_PATH_LENGTH, "/flash%ld", i);
        VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
        FlashPath[MAX_DEVICE_PATH_LENGTH - 1] = '\0';

        DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, FlashPath);)
        DEST_MODE_PC(
            insert_string(&Devices[DeviceNr].Path_p, FlashPath, strlen(FlashPath));

        switch (FPD_Config.Loader_Flash_Mem_Type) {
    case MEM_TYPE_NAND: {
        insert_string(&Devices[DeviceNr].Type_p, "Large page NAND", strlen("Large page NAND"));
        }
        break;
        case MEM_TYPE_NOR: {
            insert_string(&Devices[DeviceNr].Type_p, "NOR", strlen("NOR"));
        }
        break;
        case MEM_TYPE_EMMC: {
            insert_string(&Devices[DeviceNr].Type_p, "eMMC", strlen("eMMC"));
        }
        break;
        default: {
            insert_string(&Devices[DeviceNr].Type_p, "Unknown flash type", strlen("Unknown flash type"));
        }
        break;
        }

        Devices[DeviceNr].BlockSize = FPD_Config.EraseBlockSize;
        Devices[DeviceNr].Start = 0;
        Devices[DeviceNr].Length = FPD_Config.DeviceSize;
        )

        /*
         * Provide information for the parameter WriteProtectedGroupSize of the flash device, needed for calculating
         * SetEnhancedArea parameters. We have to register this information as a device.
         */
        DeviceNr++;
        VERIFY(DeviceNr < MAX_NUMBER_OF_DEVICES, E_FLASH_APP_INTERNAL_ERROR);

        amount_of_char = snprintf(FlashPath, MAX_DEVICE_PATH_LENGTH, "/flash%ld: WriteProtectedGroupSize", i);
        VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
        FlashPath[MAX_DEVICE_PATH_LENGTH - 1] = '\0';

        insert_string(&Devices[DeviceNr].Path_p, FlashPath, strlen(FlashPath));
        insert_string(&Devices[DeviceNr].Type_p, "Information", strlen("Information"));

        Devices[DeviceNr].BlockSize = FPD_Config.WriteProtectedSize;
        Devices[DeviceNr].Start = 0;
        Devices[DeviceNr].Length = 0;

#ifdef CNH1606644_GDFS_ENABLED
        /*
         * List GDFS as device.
         */
        DeviceNr++;
        VERIFY(DeviceNr < MAX_NUMBER_OF_DEVICES, E_FLASH_APP_INTERNAL_ERROR);

        amount_of_char = snprintf(FlashPath, MAX_DEVICE_PATH_LENGTH, "/flash%ld/GDFS", i);
        VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
        FlashPath[MAX_DEVICE_PATH_LENGTH - 1] = '\0';
        DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, FlashPath);)
        DEST_MODE_PC(
            insert_string(&Devices[DeviceNr].Path_p, FlashPath, strlen(FlashPath));
            insert_string(&Devices[DeviceNr].Type_p, "GD", strlen("GD"));

            Devices[DeviceNr].BlockSize = 1;
            Devices[DeviceNr].Start     = 0;
            Devices[DeviceNr].Length    = GDFS_MAX_PSTORE_FILES * GDFS_PARAMETERS_PER_PSTORE_FILE;
        )
#endif

        if (FPD_Config.EnhancedAreaEnd > 0) {
            DeviceNr++;
            VERIFY(DeviceNr < MAX_NUMBER_OF_DEVICES, E_FLASH_APP_INTERNAL_ERROR);

            amount_of_char = snprintf(FlashPath, MAX_DEVICE_PATH_LENGTH, "/flash%ld/enhanced", i);
            VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
            FlashPath[MAX_DEVICE_PATH_LENGTH - 1] = '\0';
            DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, FlashPath);)
            DEST_MODE_PC(
                insert_string(&Devices[DeviceNr].Path_p, FlashPath, strlen(FlashPath));
                insert_string(&Devices[DeviceNr].Type_p, "EnhancedArea", strlen("EnhancedArea"));

                Devices[DeviceNr].BlockSize = FPD_Config.PageSize;
                Devices[DeviceNr].Start     = FPD_Config.EnhancedAreaStart;
                Devices[DeviceNr].Length    = FPD_Config.EnhancedAreaEnd - FPD_Config.EnhancedAreaStart + 1;
            )
        }

        /*
         * Get boot devices from services.
         */
        Do_BAM_GetConfig(&FPD_Config, &BAM_Config);

        if ((BAM_Config.Number_Of_Units > 0) && (i == 0)) {
            DeviceNr++;
            VERIFY(DeviceNr < MAX_NUMBER_OF_DEVICES, E_FLASH_APP_INTERNAL_ERROR);

            amount_of_char = snprintf(FlashPath, MAX_DEVICE_PATH_LENGTH, "/flash%ld/boot0", i);
            VERIFY(((MAX_DEVICE_PATH_LENGTH > amount_of_char) && (0 < amount_of_char)), E_FLASH_APP_INTERNAL_ERROR);
            FlashPath[MAX_DEVICE_PATH_LENGTH - 1] = '\0';
            DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, FlashPath);)
            DEST_MODE_PC(
                insert_string(&Devices[DeviceNr].Path_p, FlashPath, strlen(FlashPath));
                insert_string(&Devices[DeviceNr].Type_p, "Partition", strlen("Partition"));

                Devices[DeviceNr].BlockSize = BAM_Config.Unit_Configuration[0].PageSize;
                Devices[DeviceNr].Start     = BAM_Config.Unit_Configuration[0].StartBlock;
                Devices[DeviceNr].Length    = BAM_Config.Unit_Configuration[0].EndBlockByteAddress;
            )
        }

        // read the second copy of boot area, first may contain MBR
        RootTocNeededSize = GetSizeForRootToc(FPD_Config.PageSize);
        BootArea_p = (uint8 *)malloc(RootTocNeededSize);
        VERIFY(NULL != BootArea_p, E_ALLOCATE_FAILED);
        ReturnValue = Do_BAM_Read(&FPD_Config, BAM_BOOT_IMAGE_COPY_SIZE, BootArea_p, RootTocNeededSize);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = Do_TOC_Create_RootList(&TOC_List_p, BootArea_p, RootTocNeededSize, TOC_BAMCOPY_1);

        if (E_SUCCESS != ReturnValue) {
            A_(printf("flash_applications_functions.c (%d): Do_TOC_Create_RootList returned %d. ", __LINE__, ReturnValue);)
            ReturnValue = E_SUCCESS;
            continue;
        }

        // we have TOC on flash, try to read SubTOC
        ReturnValue = ReadSubTOCFromFlash(TOC_List_p, &FPD_Config);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        TempPath_p = (char *)malloc(MAX_DEVICE_PATH_LENGTH);
        VERIFY(NULL != TempPath_p, E_ALLOCATE_FAILED);

        TOC_Entry_p = TOC_List_p;

        while (TOC_Entry_p) {
            DeviceNr++;
            VERIFY(DeviceNr < MAX_NUMBER_OF_DEVICES, E_FLASH_APP_INTERNAL_ERROR);

            ReturnValue = GetDevicePathForTOCEntry(TOC_Entry_p, TempPath_p, i);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            DEST_MODE_TBL(Insert_path_in_device_tbl(path_table_p, TempPath_p);)
            DEST_MODE_PC(
                insert_string(&Devices[DeviceNr].Path_p, TempPath_p, strlen(TempPath_p));

            if (Do_TOC_IsEntrySubToc(&TOC_Entry_p->entry)) {
            insert_string(&Devices[DeviceNr].Type_p, "SubTOC", strlen("SubTOC"));
            } else if (Do_TOC_IsEntryPartitionEntry(&TOC_Entry_p->entry)) {
            insert_string(&Devices[DeviceNr].Type_p, "PartitionEntry", strlen("PartitionEntry"));
            } else {
                insert_string(&Devices[DeviceNr].Type_p, "Partition", strlen("Partition"));
            }

            Devices[DeviceNr].BlockSize = FPD_Config.PageSize;
            Devices[DeviceNr].Start     = Do_TOC_GetOffset(&TOC_Entry_p->entry);
            Devices[DeviceNr].Length    = Do_TOC_GetSize(&TOC_Entry_p->entry);
            )
            TOC_Entry_p = Do_TOC_FindNextEntry(TOC_Entry_p);
        }

        BUFFER_FREE(TempPath_p);
    }

ErrorExit:
    DeviceNr++;

    if (DeviceNr >= MAX_NUMBER_OF_DEVICES) {
        ReturnValue = E_FLASH_APP_INTERNAL_ERROR;
        DeviceNr = 0;
    }

    DEST_MODE_PC(
        ExeContext_p->Running = FALSE;

        B_(printf("flash_applications_functions.c (%d): Do_Flash_ListDevicesImpl......(%x)\n\n", __LINE__, ReturnValue);)
        ReturnValue = Done_Flash_ListDevices(ExeContext_p->Received.SessionNr, ReturnValue, DeviceNr, Devices);
        ASSERT(E_SUCCESS == ReturnValue);
    )
    ListDevices_FreeAllocatedSpace(Devices);
    Do_TOC_DestroyTOCList(TOC_List_p);
    BUFFER_FREE(BootArea_p);
    BUFFER_FREE(TempPath_p);

    return ReturnValue;

}

static ErrorCode_e Delete_Device_Path_Table(device_path_table_t *path_table_p)
{
    device_path_table_t *next = path_table_p;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    VERIFY(NULL != path_table_p, E_INVALID_INPUT_PARAMETERS);

    while (next != NULL) {
        next =  path_table_p->next;
        free(path_table_p->path_p);
        free(path_table_p);
        path_table_p = next;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;

}

static ErrorCode_e Insert_path_in_device_tbl(device_path_table_t *p, char *data)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 len = 0;

    VERIFY(NULL != data, E_INVALID_INPUT_PARAMETERS);
    len = strlen(data); //calculate length of input string
    VERIFY(0 != len, E_INVALID_INPUT_PARAMETERS); //verify that we don't have zero-length paths
    VERIFY(len < MAX_DEVICE_PATH_LENGTH, E_INVALID_INPUT_PARAMETERS); //verify that the path does not exceed the maximum allowed length

    /* if this is the first element */
    if (p->path_p ==  NULL) {

        p->path_p = malloc(len + 2);
        VERIFY(NULL != p->path_p, E_ALLOCATE_FAILED);
        memset(p->path_p, 0x00, len + 2);
        memcpy(p->path_p, data, len);
        p->path_len = len;
        p->next = NULL;
    } else {
        // Go to the last element
        while (p->next != NULL) {
            p = p->next;
        }

        // Allocate memory for the new element and insert data in it
        p->next = (device_path_table_t *)malloc(sizeof(device_path_table_t));
        p = p->next;
        p->path_p = malloc(len + 2); // adding extra byte for (trailing slash) + (null char terminator)
        VERIFY(NULL != p->path_p, E_ALLOCATE_FAILED);
        memset(p->path_p, 0x00, len + 2);
        memcpy(p->path_p, data, len);

        p->path_len = len;
        p->next = NULL;
    }

    /*
     *  Add trailing slash at the end of the path
     */
    if (p->path_p[len] != '/') {
        p->path_p[len] = '/';
        p->path_p[len + 1] = '\0';
        p->path_len++;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}


static boolean Is_path_in_device_tbl(device_path_table_t *p, char *key)
{
    uint8 len = 0;

    len = strlen(key); //calculate length of input string

    C_(printf("\n Searching for key [%d|%s]\n", len, key);)

    while (p != NULL) {
        if (p->path_len == len) {
            if (!strcmp(p->path_p, key)) { //key is found.
                return TRUE;
            }
        }

        p = p -> next;// Search in the next element.
    }

    // Key is not found
    return FALSE;
}

C_(
    static ErrorCode_e  Print_paths_from_device_tbl(device_path_table_t *p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    VERIFY(NULL != p, E_INVALID_INPUT_PARAMETERS);

    while (p != NULL) {
        printf("[%d|%s]\n", p->path_len, p->path_p);
        p = p->next;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
})

static ErrorCode_e SelectLoaderOptions(uint32 Property, uint32 Value)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    FLASH_DEVICE_t FlashDevice = 0;
    SelectLoaderOptions_t SelectedOption = (SelectLoaderOptions_t) Property;

    switch (SelectedOption) {
    case DISABLE_ENHANCED_AREA_CHECK:
        DisabledEnhancedAreaCheck = Value;
        ReturnValue = E_SUCCESS;
        break;

    case DISABLE_SW_RESET:
        ReturnValue = Do_FPD_EnPowerReset(Value, FlashDevice);
        break;

    default:
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
        break;
    }

    if (ReturnValue == E_SUCCESS) {
        return ReturnValue;
    } else {
        goto ErrorExit;
    }

ErrorExit:
    B_(printf("flash_applications_functions.c (%d): Invalid input parameters for selecting loader options\n", __LINE__);)
    return ReturnValue;
}


/** @} */
