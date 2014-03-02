/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/

/**
 * @file  toc_handler_utils.c
 * @brief Utilities functions for TOC management
 *
 * @addtogroup ldr_toc
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "toc_handler.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * @brief Function that fill entry for given TOC with correct addresses and
 *        flags.
 *
 * This function checks if given TOC Offset or Size is bigger than max uint32.
 * If so they will be written in the input TOCEntry_p entry converted to LBA and
 * the LBA flag will be set. Otherwise the input argument are written as it is
 * in TOCEntry_p entry.
 *
 * @param[in,out] TOCEntry_p  Entry pointer that will be set.
 * @param[in]     TOC_Offset  TOC offset entry to be checked.
 * @param[in]     TOC_Size    TOC size entry to be checked.
 * @param[in]     TOC_Flags   Generic TOC entry flags.
 *
 * @return none
 */
void Do_TOC_FillEntry(TOC_Entry_t *TOCEntry_p, uint64 TOC_Offset, uint64 TOC_Size, uint32 TOC_Flags)
{
    if ((TOC_Size >= 0x00000000FFFFFFFF) || (TOC_Offset >= 0x00000000FFFFFFFF)) {
        TOCEntry_p->TOC_Offset = (uint32)(TOC_Offset / SUPPORTED_SECTOR_SIZE);
        TOCEntry_p->TOC_Size = (uint32)((TOC_Size + SUPPORTED_SECTOR_SIZE - 1) / SUPPORTED_SECTOR_SIZE);
        TOCEntry_p->TOC_Flags = (TOC_Flags | TOC_FLAG_LBA);
    } else {
        TOCEntry_p->TOC_Offset = (uint32)TOC_Offset;
        TOCEntry_p->TOC_Size = (uint32)TOC_Size;
        TOCEntry_p->TOC_Flags = TOC_Flags;
    }
}

/**
 * @brief Get entry offset in bytes.
 *
 * This function checks the entry addressing mode and returns offset of the
 * given entry in bytes.
 *
 * @param[in] TOCEntry_p  Entry to be evaluated.
 *
 * @return    Entry offset in bytes.
 */
uint64 Do_TOC_GetOffset(TOC_Entry_t *TOCEntry_p)
{
    if (TOC_FLAG_LBA == (TOCEntry_p->TOC_Flags & TOC_FLAG_MASK)) {
        return (uint64)TOCEntry_p->TOC_Offset * SUPPORTED_SECTOR_SIZE;
    } else {
        return (uint64)TOCEntry_p->TOC_Offset;
    }
}

/**
 * @brief Get entry size in bytes.
 *
 * This function checks the entry addressing mode and returns size of the
 * given entry in bytes.
 *
 * @param[in] TOCEntry_p  Entry to be evaluated.
 *
 * @return    Entry size in bytes.
 */
uint64 Do_TOC_GetSize(TOC_Entry_t *TOCEntry_p)
{
    if (TOC_FLAG_LBA == (TOCEntry_p->TOC_Flags & TOC_FLAG_MASK)) {
        return (uint64)TOCEntry_p->TOC_Size * SUPPORTED_SECTOR_SIZE;
    } else {
        return (uint64)TOCEntry_p->TOC_Size;
    }
}

/** @} */
