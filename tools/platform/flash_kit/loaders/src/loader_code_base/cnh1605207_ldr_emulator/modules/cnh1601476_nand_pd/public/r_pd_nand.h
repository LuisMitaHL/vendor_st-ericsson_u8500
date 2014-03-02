/*******************************************************************************
* $Copyright Ericsson AB 2008 $
********************************************************************************
*
* DESCRIPTION:
*
* Public API for NAND PD.
*
* Note: parameter check will not be performed unless SDE variable
*       CFG_CNH1601476_DEBUG is defined at compile time.
*
*******************************************************************************/

// =============================================================================
#ifndef INCLUSION_GUARD_R_PD_NAND
#define INCLUSION_GUARD_R_PD_NAND
// =============================================================================

// =============================================================================
//  Configurations
// =============================================================================

// - CFG_ENABLE_OS_INDEPENDENT_CODE: When defined NAND PD will use an OS free implementation.

// =============================================================================
//  Include Header Files
// =============================================================================

#include "c_system_v2.h"
#include "t_basicdefinitions.h"
#include "t_pd_nand.h"

// =============================================================================
//  Types
// =============================================================================

// - See "t_pd_nand.h".

// =============================================================================
//  Global Function Declarations
// =============================================================================

/**
 * Reads one page and/or extra data from NAND flash. No ECC is performed.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    PageOffset          An offset expressed in number of pages into the NAND flash.
 * @param   [out]   Data_p              Pointer to a data buffer. The size of the data buffer must be "page size" * NbrOfPages. May be NULL if extra data is to be read.
 * @param   [out]   ExtraData_p         Pointer to a data buffer. The size of the data buffer must be MetaDataLength * NbrOfPages. See PD_NAND_Config_t. May be NULL if page data is to be read.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_ReadPage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    uint8 *Data_p,
    uint8 *ExtraData_p
);

// -----------------------------------------------------------------------------

/**
 * Reads one or several consecutive pages and/or extra data from NAND flash. ECC is performed if enabled in
 * Config_p. The length of the ExtraData_p must be the MetaDataLength
 * times NbrOfPages.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    PageOffset          An offset expressed in number of pages into the NAND flash.
 * @param   [in]    NbrOfPages          Number of pages to read.
 * @param   [out]   Data_p              Pointer to a data buffer. The size of the data buffer must be "page size" * NbrOfPages. Pointer may be NULL if only extra data is to be read.
 * @param   [out]   ExtraData_p         Pointer to a data buffer. The size of the data buffer must be MetaDataLength * NbrOfPages. See PD_NAND_Config_t. Pointer may be NULL if only page data is to be read.
 * @param   [out]   NbrOfReadPages_p    Pointer to a 32-bit variable. Returns the number of pages actually read. If the returned result is not success,
 *                                      then this parameter determines how many pages was read successfully before the problem was encountered.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_ReadConsecutivePages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfPages,
    uint8 *Data_p,
    uint8 *ExtraData_p,
    uint32 *NbrOfReadPages_p
);

// -----------------------------------------------------------------------------

/**
 * Reads one or several scattered pages and/or extra data from NAND flash. ECC is performed if enabled in
 * Config_p. The length of the ExtraData_p must be the MetaDataLength
 * times NbrOfPages.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    PageOffsetArray_p   An array of page offsets expressed in number of pages into the NAND flash. { Page #1 offset; Page #2 offset;... }
 * @param   [in]    NbrOfPages          Number of pages to read.
 * @param   [out]   DataArray_pp        Array of pointers to page data buffers. The size of each data buffer must be a page. Pointer may be NULL if only extra data is to be read.
 * @param   [out]   ExtraDataArray_pp   Array of pointer to extra data buffers. The size of the each data buffer must be "MetaDataLength". See PD_NAND_Config_t. Pointer may be NULL if only page data is to be read.
 * @param   [out]   NbrOfReadPages_p    Pointer to a 32-bit variable. Returns the number of pages actually read. If the returned result is not success,
 *                                      then this parameter determines how many pages was read successfully before the problem was encountered.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_ReadScatteredPages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 *PageOffsetArray_p,
    const uint32 NbrOfPages,
    const uint8 **DataArray_pp,
    const uint8 **ExtraDataArray_pp,
    uint32 *NbrOfReadPages_p
);

// -----------------------------------------------------------------------------

/**
 * Writes one page and/or extra data to NAND flash. No ECC is performed.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    PageOffset          An offset expressed in number of pages into the NAND flash.
 * @param   [in]    Data_p              Pointer to a data buffer. The size of the data buffer must be "page size" * NbrOfPages. Pointer may be NULL if only extra data is to be written.
 * @param   [in]    ExtraData_p         Pointer to a data buffer. The size of the data buffer must be MetaDataLength * NbrOfPages. See PD_NAND_Config_t. Pointer may be NULL if only page data is to be written.
 * @param   [in]    Program             If set to 0 the page is written to the NAND flash, but the final program command is not sent.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_WritePage(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint8 *const Data_p,
    const uint8 *const ExtraData_p,
    const uint32 Program
);

// -----------------------------------------------------------------------------

/**
 * Writes one or several consecutive pages and/or extra data to NAND flash. ECC is performed if enabled in
 * Config_p. The length of the ExtraData_p must be the MetaDataLength
 * times NbrOfPages.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    PageOffset          An offset expressed in number of pages into the NAND flash.
 * @param   [in]    NbrOfPages          Number of pages to write.
 * @param   [in]    Data_p              Pointer to a data buffer. The size of the data buffer must be "page size" * NbrOfPages. Pointer may be NULL if only extra data is to be written.
 * @param   [in]    ExtraData_p         Pointer to a data buffer. The size of the data buffer must be MetaDataLength * NbrOfPages. See PD_NAND_Config_t. Pointer may be NULL if only page data is to be written.
 * @param   [out]   NbrOfWrittenPages_p Pointer to a 32-bit variable. Returns the number of pages actually written. If the returned result is not success,
 *                                      then this parameter determines how many pages was written successfully before the problem was encountered.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_WriteConsecutivePages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfPages,
    const uint8 *const Data_p,
    const uint8 *const ExtraData_p,
    uint32 *NbrOfWrittenPages_p
);

// -----------------------------------------------------------------------------

/**
 * Writes one or several scattered pages and/or extra data to NAND flash. ECC is performed if enabled in
 * Config_p. The length of the ExtraData_p must be the MetaDataLength
 * times NbrOfPages.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    PageOffsetArray_p   An array of page offsets expressed in number of pages into the NAND flash. { Page #1 offset; Page #2 offset;... }
 * @param   [in]    NbrOfPages          Number of pages to write.
 * @param   [in]    DataArray_pp        Array of pointers to page data buffers. The size of each data buffer must be a page. Pointer may be NULL if only extra data is to be written.
 * @param   [in]    ExtraDataArray_pp   Array of pointer to extra data buffers. The size of the each data buffer must be "MetaDataLength". See PD_NAND_Config_t. Pointer may be NULL if only page data is to be written.
 * @param   [out]   NbrOfWrittenPages_p Pointer to a 32-bit variable. Returns the number of pages actually written. If the returned result is not success,
 *                                      then this parameter determines how many pages was written successfully before the problem was encountered.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_WriteScatteredPages(
    const PD_NAND_Config_t *const Config_p,
    const uint32 *const PageOffsetArray_p,
    const uint32 NbrOfPages,
    const uint8 **const DataArray_pp,
    const uint8 **const ExtraDataArray_pp,
    uint32 *NbrOfWrittenPages_p
);

// -----------------------------------------------------------------------------

/**
 * Erases one or several blocks in NAND flash.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    PageOffset          An offset expressed in number of pages into the NAND flash.
 * @param   [in]    NbrOfBlocks         Number of blocks to erase.
 * @param   [out]   NbrOfErasedBlocks_p Pointer to a 32-bit variable. Returns the number of blocks actually erased. If the returned result is not success,
 *                                      then this parameter determines how many blocks was erased successfully before the problem was encountered.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_Erase(
    const PD_NAND_Config_t *const Config_p,
    const uint32 PageOffset,
    const uint32 NbrOfBlocks,
    uint32 *NbrOfErasedBlocks_p
);

// -----------------------------------------------------------------------------

/**
 * Retrieves memory properties.
 *
 * @param   [in]    Config_p                    Pointer to a configuration to be used for this NAND flash access.
 * @param   [out]   MemoryProperties_p          Pointer to a PD_NAND_MemoryProperties_t.
 *
 * @sigbased No
 * @waitmode Wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_GetMemoryProperties(const PD_NAND_Config_t *const Config_p, PD_NAND_MemoryProperties_t *MemoryProperties_p);

// -----------------------------------------------------------------------------

/**
 * Sets memory properties. This function should normally only be called by a loader
 * before performing any other operations on the NAND flash. The NAND flash properties
 * should be fetched from the load module about to be loaded. Nothing is actually written
 * to flash, but the properties are only used for configuration.
 *
 * @param   [in]    Config_p                    Pointer to a configuration to be used for this NAND flash access.
 * @param   [in]    MemoryProperties_p          Pointer to a PD_NAND_MemoryProperties_t.
 *
 * @sigbased No
 * @waitmode Wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_SetMemoryProperties(const PD_NAND_Config_t *const Config_p, const PD_NAND_MemoryProperties_t *const MemoryProperties_p);

// -----------------------------------------------------------------------------

/**
 * Retrieves NAND flash manufacturer id and device id. May be used to detect if a NAND flash
 * is available or not.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p            Pointer to a configuration to be used for this NAND flash access.
 * @param   [out]   ManufacturerId_p    Pointer to where the manufacturer id will be stored.
 * @param   [out]   DeviceId_p          Pointer to where the device id will be stored.
 *
 * @sigbased No
 * @waitmode Wait mode or No wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_GetIDs(
    const PD_NAND_Config_t *const Config_p,
    uint32 *ManufacturerId_p,
    uint32 *DeviceId_p
);

// -----------------------------------------------------------------------------

/**
 * Retrieves last operation's ECC.
 * Note: parameter check will not be performed unless SDE variable
 * CFG_CNH1601476_DEBUG is defined at compile time.
 *
 * @param   [in]    Config_p               Pointer to a configuration to be used for this NAND flash access.
 * @param   [out]   PD_NAND_Ecc2kPage_p    ECC container.
 *
 * @sigbased No
 * @waitmode Wait mode
 *
 * @return          PD_NAND_Result_t.
 */
PD_NAND_Result_t Do_PD_NAND_ReadEcc(
    const PD_NAND_Config_t *const Config_p,
    PD_NAND_Ecc2kPage_t PD_NAND_Ecc2kPage_p
);

// =============================================================================
#endif // INCLUSION_GUARD_R_PD_NAND
// =============================================================================


