/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 * @addtogroup ldr_service_gdm Global Data Manager
 * @{
 *   @addtogroup ldr_service_gdm_cspsa Crash save parameter storage area
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "r_service_management.h"
#include "global_data_manager.h"
#include "boot_area_management.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "parameter_applications.h"
#include "cspsa.h"
#include "r_io_subsystem.h"
#include "toc_handler.h"
#include "r_memory_utils.h"
#include "cspsa_core.h"
#include "cspsa_plugin.h"
#include "r_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
extern char *ActiveDevice_p;
static CSPSA_Handle_t Handle;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ErrorCode_e CSPSA_Initialize(uint8 FlashDevice);
static ErrorCode_e ReadSubTOCFromFlash(TOC_List_t *List_p, FPD_Config_t *const FPDConfig_p);
static ErrorCode_e Read_SubTOC_Data(uint32 Offset, uint32 Size, uint8 *Data_p, uint32 BufferSize, FPD_Config_t *const FPDConfig_p);
static ErrorCode_e RegisterCSPSA_Area(TOC_List_t *TrimAreaTOC_Entry_p, FPD_Config_t *const FPDConfig_p);
static ErrorCode_e ConvertCSPSAtoLoaderErrors(CSPSA_Result_t CSPSA_Result);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
void CSPSA_LL_EMMC_SetFlashDevice(uint8 FlashDev);

/**
 * Open a CSPSA partition.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Start(void)
{
    uint8 i;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;
    uint8 available_flash_devices = Do_FPD_GetNrOfAvailableFlashDevices();

    // Check both flash memories for TOC
    for (i = 0; i < available_flash_devices; i++) {
        CSPSA_LL_EMMC_SetFlashDevice(i);

        ReturnValue = CSPSA_Initialize(i);

        if (E_SUCCESS == ReturnValue) {
            break;
        } else if (i == available_flash_devices - 1) {
            goto ErrorExit;
        }
    }

    CSPSA_Result = CSPSA_Open(ActiveDevice_p, &Handle);

    if (T_CSPSA_RESULT_E_NO_VALID_IMAGE == CSPSA_Result) {
        A_(printf("gdm_cspsa.c(%d): Partition cannot be opened. Invalid image found. Try to create new partition according to configuration found in TOC entry.\n", __LINE__);)
        CSPSA_Result = CSPSA_Create(ActiveDevice_p, &Handle);
        ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    } else {
        ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        goto ErrorExit;
    }


ErrorExit:
    return ReturnValue;
}

/**
 * Close CSPSA partitions.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Stop(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;
    CSPSA_Config_t CSPSA_Config = {0};
    uint32 NrOfRegisteredAreas = 0;
    uint32 counter = 0;

    //
    // Close currently opened partition
    //
    /* coverity[check_return] */
    CSPSA_Result = CSPSA_Close(&Handle);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    //
    // Check how match partitions was registered and try to close them.
    //
    NrOfRegisteredAreas = CSPSA_CORE_GetParameterAreaListLength();
    C_(printf("gdm_cspsa.c(%d): %d CSPSA partitions registered!!!\n", __LINE__, NrOfRegisteredAreas);)

    for (counter = 0; counter < NrOfRegisteredAreas; ++counter) {
        CSPSA_Result = CSPSA_CORE_GetParameterArea(counter, &CSPSA_Config);
        ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        CSPSA_Result = CSPSA_RemoveParameterArea(&CSPSA_Config);
        ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    }

ErrorExit:
    BUFFER_FREE(ActiveDevice_p);
    return ReturnValue;
}

/**
 * Formats a partition.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Format(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;

    CSPSA_Result = CSPSA_Format(Handle);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/**
 * The current cached parameter storage area data is stored to memory media.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Flush(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;

    CSPSA_Result = CSPSA_Flush(Handle);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/**
 * Get the size of a unit.
 *
 * @param [in]  Unit     Unit to get size for.
 * @param [out] Size_p
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_GetUnitSize(uint32 Unit, uint32 *Size_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;

    CSPSA_Result = CSPSA_GetSizeOfValue(Handle, (CSPSA_Key_t) Unit, (CSPSA_Size_t *) Size_p);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/**
 * Read a unit.
 *
 * @param [in]  Unit     Unit to read.
 * @param [out] Data_p   Pointer to buffer to put the data in.
 * @param [in]  Size     Size of the unit.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_ReadUnit(uint32 Unit, uint8 *Data_p, uint32 Size)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;

    CSPSA_Result = CSPSA_ReadValue(Handle, (CSPSA_Key_t) Unit, (CSPSA_Size_t) Size, (CSPSA_Data_t *) Data_p);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/**
 * Write a unit.
 *
 * @param [in] Unit         Unit to write.
 * @param [in] Data_p       Pointer to the data to write.
 * @param [in] DataSize     Size to write.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_WriteUnit(uint32 Unit, uint8 *Data_p, uint32 DataSize)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;

    CSPSA_Result = CSPSA_WriteValue(Handle, (CSPSA_Key_t) Unit, (CSPSA_Size_t) DataSize, (CSPSA_Data_t *) Data_p);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/**
 * Get the size of the first unit in the list.
 *
 * @param [out] Unit_p  The unit number of the next parameter value.
 * @param [out] Size_p  Size of the next unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_GetUnitSizeFirst(uint32 *Unit_p, uint32 *Size_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;

    CSPSA_Result = CSPSA_GetSizeOfFirstValue(Handle, (CSPSA_Key_t *) Unit_p, (CSPSA_Size_t *) Size_p);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/**
 * Get the size of the next unit in the list.
 *
 * @param [out] Unit_p  The unit number of the next parameter value.
 * @param [out] Size_p  Size of the next unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_GetUnitSizeNext(uint32 *Unit_p, uint32 *Size_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;

    CSPSA_Result = CSPSA_GetSizeOfNextValue(Handle, (CSPSA_Key_t *) Unit_p, (CSPSA_Size_t *) Size_p);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ErrorCode_e CSPSA_Initialize(uint8 FlashDevice)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    TOC_List_t *TOC_List_p = NULL;
    TOC_List_t *TOC_Entry_p = NULL;
    uint8 *BootArea_p = NULL;
    FPD_Config_t FPDConfig = {0,};
    uint32 RootTocNeededSize;

    ReturnValue = Do_FPD_GetInfo(FlashDevice, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, E_INVALID_INPUT_PARAMETERS);

    //
    // Allocate space where boot image will be placed.
    //
    RootTocNeededSize = GetSizeForRootToc(FPDConfig.PageSize);
    BootArea_p = (uint8 *) malloc(RootTocNeededSize);
    VERIFY(NULL != BootArea_p, E_ALLOCATE_FAILED);

    //
    // Read the second copy of boot area, first may contain MBR (Master Boot Record)
    // which is created in Android booting phase.
    //
    ReturnValue = Do_BAM_Read(&FPDConfig, BAM_BOOT_IMAGE_COPY_SIZE, BootArea_p, RootTocNeededSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    //
    // Create Root list from Table of contents which reside in boot area
    //
    ReturnValue = Do_TOC_Create_RootList(&TOC_List_p, BootArea_p, RootTocNeededSize, TOC_BAMCOPY_1);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = ReadSubTOCFromFlash(TOC_List_p, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    TOC_Entry_p = Do_TOC_LocateEntryByNodeId(TOC_List_p, ActiveDevice_p);
    VERIFY(NULL != TOC_Entry_p, E_PARTITION_NOT_FOUND);

    ReturnValue = RegisterCSPSA_Area(TOC_Entry_p, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    BUFFER_FREE(BootArea_p);
    Do_TOC_DestroyTOCList(TOC_List_p);

    return ReturnValue;
}

static ErrorCode_e ReadSubTOCFromFlash(TOC_List_t *List_p, FPD_Config_t *const FPDConfig_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    TOC_List_t *Node_p = NULL;
    uint32 Size = 0;
    uint32 Offset = 0;
    uint8 *Data_p = NULL;

    VERIFY(NULL != List_p, E_INVALID_INPUT_PARAMETERS);

    //
    // Get SubTOC entry from list
    //
    Node_p = Do_TOC_GetNext_SubTocNode(List_p);

    if (Node_p != NULL) {
        B_(printf("flash_applications_functions.c (%d): SubTOC entry found, reading SubTOC\n", __LINE__);)
        Offset = Node_p->entry.TOC_Offset;
        Size = Node_p->entry.TOC_Size;

        if ((Offset != 0xFFFFFFFF) && (Size != 0)) {
            Data_p = (uint8 *) malloc(Size);
            VERIFY(NULL != Data_p, E_ALLOCATE_FAILED);

            ReturnValue = Read_SubTOC_Data(Offset, Size, Data_p, Size, FPDConfig_p);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            //
            // Extract SUB TOC list and link it with root list
            //
            if (!Do_TOC_Add_SubTocList(List_p, Data_p, Size, Node_p->entry.TOC_Id, Offset)) {
                A_(printf("gdm_cspsa.c (%d): Failed to add SubTOC list\n");)
                ReturnValue = E_INVALID_SUBTOC_PARAMETERS;
                goto ErrorExit;
            }
        }
    } else {
        B_(printf("gdm_cspsa.c (%d): SubTOC entry not found\n", __LINE__);)
    }

ErrorExit:
    BUFFER_FREE(Data_p);
    return ReturnValue;
}

static ErrorCode_e Read_SubTOC_Data(uint32 Offset, uint32 Size, uint8 *Data_p, uint32 BufferSize, FPD_Config_t *const FPDConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    IO_Handle_t ReadHandle = IO_INVALID_HANDLE;
    uint64 BytesRead = IO_ERROR;

    VERIFY(BufferSize >= Size, E_INVALID_INPUT_PARAMETERS);

    //
    // Read SUB TOC data
    //
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

static ErrorCode_e RegisterCSPSA_Area(TOC_List_t *TrimAreaTOC_Entry_p, FPD_Config_t *const FPDConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_UNDEFINED;
    CSPSA_Config_t CSPSA_Config = {0};
    uint32_t PageSize = 0;

    //
    // Sanity check of input parameter
    //
    VERIFY(NULL != TrimAreaTOC_Entry_p, E_INVALID_INPUT_PARAMETERS);

    //
    // Initialize configuration with default values.
    //
    CSPSA_InitConfig(&CSPSA_Config);

    PageSize = FPDConfig_p->PageSize;

    CSPSA_Config.MemoryType = T_CSPSA_MEMORY_TYPE_EMMC;

    //
    // Make sure StartOffset is page aligned (page size must be a power of 2)
    //
    CSPSA_Config.StartOffsetInBytes = (uint64_t)((TrimAreaTOC_Entry_p->entry.TOC_Offset + (PageSize - 1)) & ~(PageSize - 1));

    CSPSA_Config.SizeInBytes = (uint32_t)(TrimAreaTOC_Entry_p->entry.TOC_Offset + TrimAreaTOC_Entry_p->entry.TOC_Size - CSPSA_Config.StartOffsetInBytes);

    //
    // Make sure SizeInBytes is page aligned
    //
    CSPSA_Config.SizeInBytes = CSPSA_Config.SizeInBytes & ~(PageSize - 1);

    CSPSA_Config.SectorSize = PageSize;
    CSPSA_Config.BlockSize = PageSize;

    memset(CSPSA_Config.Name, 0, sizeof(CSPSA_Config.Name));
    memcpy(CSPSA_Config.Name, TrimAreaTOC_Entry_p->entry.TOC_Id, MIN(sizeof(TrimAreaTOC_Entry_p->entry.TOC_Id), TOC_ID_LENGTH));

    //
    // Register parameter storage area to CSPSA
    //
    CSPSA_Result = CSPSA_AddParameterArea(&CSPSA_Config);
    ReturnValue = ConvertCSPSAtoLoaderErrors(CSPSA_Result);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e ConvertCSPSAtoLoaderErrors(CSPSA_Result_t CSPSA_Result)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    switch (CSPSA_Result) {
    case T_CSPSA_RESULT_OK:
        ReturnValue = E_SUCCESS;
        break;
        //- - -

    case T_CSPSA_RESULT_W_SIZE:
    case T_CSPSA_RESULT_W_NEW_KEY:
    case T_CSPSA_RESULT_W_INVALID_HEADER_FOUND:
        A_(printf("gdm_cspsa.c: Warning %d detected!!!\n", CSPSA_Result);)
        ReturnValue = E_SUCCESS;
        break;

        //- - -
    case T_CSPSA_RESULT_E_INVALID_KEY:
        ReturnValue = E_CSPSA_RESULT_E_INVALID_KEY;
        break;
    case T_CSPSA_RESULT_E_OUT_OF_SPACE:
        ReturnValue = E_CSPSA_RESULT_E_OUT_OF_SPACE;
        break;
    case T_CSPSA_RESULT_E_NO_VALID_IMAGE:
        ReturnValue = E_CSPSA_RESULT_E_NO_VALID_IMAGE;
        break;
    case T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE:
        ReturnValue = E_CSPSA_RESULT_E_MEDIA_INACCESSIBLE;
        break;
    case T_CSPSA_RESULT_E_READ_ONLY:
        ReturnValue = E_CSPSA_RESULT_E_READ_ONLY;
        break;
    case T_CSPSA_RESULT_E_READ_ERROR:
        ReturnValue = E_CSPSA_RESULT_E_READ_ERROR;
        break;
    case T_CSPSA_RESULT_E_WRITE_ERROR:
        ReturnValue = E_CSPSA_RESULT_E_WRITE_ERROR;
        break;
    case T_CSPSA_RESULT_E_END_OF_DATA:
        ReturnValue = E_CSPSA_RESULT_E_END_OF_DATA;
        break;
    case T_CSPSA_RESULT_E_OPEN_ERROR:
        ReturnValue = E_CSPSA_RESULT_E_OPEN_ERROR;
        break;
    case T_CSPSA_RESULT_E_ALREADY_EXISTS:
        ReturnValue = E_CSPSA_RESULT_E_ALREADY_EXISTS;
        break;
    case T_CSPSA_RESULT_E_OUT_OF_MEMORY:
        ReturnValue = E_CSPSA_RESULT_E_OUT_OF_MEMORY;
        break;
        // - - -

    case T_CSPSA_RESULT_UNDEFINED:
        ReturnValue = E_CSPSA_RESULT_UNDEFINED;
        break;

    default:
        ReturnValue = E_CSPSA_RESULT_UNDEFINED;
        break;
    }

    return ReturnValue;
}

/** @} */
/** @} */
