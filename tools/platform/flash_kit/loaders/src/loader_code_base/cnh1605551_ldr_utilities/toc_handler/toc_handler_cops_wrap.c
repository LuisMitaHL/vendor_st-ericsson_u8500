/*******************************************************************************
 * $Copyright Ericsson AB 2011 $
 ******************************************************************************/

/**
 * @file  toc_handler_cops_wrap.c
 * @brief Functions and type declarations for TOC management
 *
 * @addtogroup ldr_toc
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "error_codes.h"
#include "toc_parser.h"
#include "toc_handler.h"
#include "r_io_subsystem.h"
#include "boot_area_management.h"
#include "r_memory_utils.h"

#ifdef MACH_TYPE_STN8500
#include "bam_emmc.h"
#endif

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define WRITE_BUFF_SIZE 512

// We don't need this parameter
typedef int tocparser_subtoc_t;

// This structure is copied from toc parser in boot area.
struct tocparser_handle_t {
    uint32_t magic;
    tocparser_toc_entry_t main_toc[MAX_ROOT_TOC_ENTRIES];
    tocparser_subtoc_t subtoc;
    int fd;
    int toc_location;
    uint8_t *boot_block_buffer;
    uint8_t write_buff[WRITE_BUFF_SIZE];
    tocparser_mode_t mode;
};

static TOC_List_t *TOC_List_p = NULL;

/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
tocparser_error_code_t tocparser_init(char *dev,
                                      tocparser_mode_t mode,
                                      tocparser_handle_t **handle)
{
    ErrorCode_e LoaderRetVal = E_GENERAL_FATAL_ERROR;
    tocparser_error_code_t ReturnValue = TOCPARSER_NOT_INITIALIZED;
    tocparser_handle_t *local_handle = NULL;
    FPD_Config_t FPDConfig = {0,};
    uint8 *BootArea_p = NULL;

    //
    // Initialize empty handle that is not used in loader case
    //
    local_handle = (tocparser_handle_t *)malloc(sizeof(tocparser_handle_t));

    if (NULL == local_handle) {
        A_(printf("toc_handler.c(%d): **ERR Memory allocation fail **\n", __LINE__);)
        return TOCPARSER_OUT_OF_MEMORY;
    }

    memset(local_handle, 0x00, sizeof(tocparser_handle_t));
    *handle = local_handle;

    //
    // Now do the magic
    //

    // For now we will assume that TOC List is placed in boot area of FLASH_0
    LoaderRetVal = Do_FPD_GetInfo(FLASH_0, &FPDConfig);
    VERIFY(E_SUCCESS == LoaderRetVal, TOCPARSER_NOT_INITIALIZED);

    BootArea_p = malloc(BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(NULL != BootArea_p, TOCPARSER_OUT_OF_MEMORY);

    LoaderRetVal = Do_BAM_Read(&FPDConfig, BAM_BOOT_IMAGE_COPY_SIZE, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE);
    VERIFY(E_SUCCESS == LoaderRetVal, TOCPARSER_NOT_INITIALIZED);

    LoaderRetVal = Do_TOC_Create_RootList(&TOC_List_p, BootArea_p, BAM_BOOT_IMAGE_COPY_SIZE, TOC_BAMCOPY_1);
    VERIFY(E_TOC_HANDLER_INPUT_DATA_EMPTY != LoaderRetVal, TOCPARSER_NOT_INITIALIZED);
    VERIFY(E_SUCCESS == LoaderRetVal, TOCPARSER_NOT_INITIALIZED);

    ReturnValue = TOCPARSER_OK;
ErrorExit:
    BUFFER_FREE(BootArea_p);

    return ReturnValue;
}


tocparser_error_code_t tocparser_write_toc_entry_from_buffer(
    tocparser_handle_t *handle,
    const char *toc_id,
    uint8_t *data,
    uint32_t size,
    uint32_t *loadaddr)
{
    ErrorCode_e LoaderRetVal = E_GENERAL_FATAL_ERROR;
    tocparser_error_code_t ReturnValue = TOCPARSER_NOT_INITIALIZED;
    TOC_List_t *TOC_Entry_p = NULL;
    uint64 PartitionStart = 0;
    uint64 PartitionSize = 0;
    IO_Handle_t WriteHandle = IO_INVALID_HANDLE;
    uint64 BytesWritten = IO_ERROR;
    FPD_Config_t FPDConfig = {0,};

    VERIFY(size != 0, TOCPARSER_ERR_INVALID_PARAM);
    VERIFY(data != NULL, TOCPARSER_ERR_INVALID_PARAM);
    VERIFY(TOC_List_p != NULL, TOCPARSER_ERR_INVALID_PARAM);

    TOC_Entry_p = Do_TOC_LocateEntryByNodeId(TOC_List_p, toc_id);
    VERIFY(NULL != TOC_Entry_p, TOCPARSER_ENTRY_NOT_FOUND);

    PartitionStart = TOC_Entry_p->entry.TOC_Offset;
    PartitionSize = TOC_Entry_p->entry.TOC_Size;

    VERIFY(size <= PartitionSize, TOCPARSER_ERR_INVALID_PARAM);

    // TODO: Check if anti roll back table can be written in boot area
    if (PartitionStart < BAM_BOOT_IMAGE_COPY_SIZE) {
        A_(printf("Data cannot be written in boot area!!!");)
        ReturnValue = TOCPARSER_WRITE_FAILED;
        goto ErrorExit;
    }

    LoaderRetVal = Do_FPD_GetInfo(FLASH_0, &FPDConfig);
    VERIFY(E_SUCCESS == LoaderRetVal, TOCPARSER_WRITE_FAILED);

    WriteHandle = Do_IOW_BDMOpen(&FPDConfig);
    VERIFY(IO_INVALID_HANDLE != WriteHandle, TOCPARSER_WRITE_FAILED);

    BytesWritten = Do_IO_PositionedWrite(WriteHandle, data, size, size, PartitionStart);
    VERIFY(size == size, TOCPARSER_WRITE_FAILED);

    ReturnValue = TOCPARSER_OK;
ErrorExit:
    CLOSE_HANDLE(WriteHandle);

    return ReturnValue;
}


tocparser_error_code_t tocparser_uninit(tocparser_handle_t *handle)
{
    BUFFER_FREE(handle);
    (void)Do_TOC_DestroyTOCList(TOC_List_p);
    TOC_List_p = NULL;
    return TOCPARSER_OK;
}

/** @} */
