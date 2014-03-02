/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/
#ifndef _PREFLASH_UTILS_H_
#define _PREFLASH_UTILS_H_

#include "r_io_subsystem.h"
#include "r_manifest_parser.h"
#include "cspsa_handler.h"
#include "toc_handler.h"

/**
 * Enumerated source type.
 */
typedef enum {
    DEVICE_FPD = 0,
    DEVICE_BAM,
    DEVICE_BDM,
    DEVICE_PARTITION,
    DEVICE_FILLED_PARTITION,
    DEVICE_UNKNOWN
} SourceType_t;

/**
 * @brief Serialize TOC entry
 *
 * @param [in] TocEntry_p - TOC entry that needs to be serialized
 * @param [out] Data      - output array of serialized data
 *
 * @return                none
 *
 */
void SerializeTOCEntry(TOC_Entry_t *const TocEntry_p, uint8 Data[sizeof(TOC_Entry_t)]);

/**
 * @brief Get image type for specified manifest entry
 *
 * @param [in] ManifestParser_p - Pointer to manifest entry
 *
 * @return                      Source type for specified entry
 *
 */
SourceType_t GetImageType(ManifestParser_t *ManifestParser_p);

/**
 * @brief Create single SRecord line for input data
 *
 * @param [in] Address          - Memory location of the first data byte
 * @param [in] Data_p           - Pointer to data field
 * @param [in] DataLengthToCopy - Length of input data
 * @param [out] SRecord         - Array that contains created S Record
 *
 * @return                      none
 *
 */
void CreateSRecord(const uint32 Address, uint8 *const Data_p, const uint8 DataLengthToCopy, char SRecord[128]);

/**
 * @brief Write S0 Record to output image
 *
 * @param [in] OutputIOHandle - Handle to output image
 *
 * @return                    Execution status (E_SUCCESS after successful execution)
 *
 */
ErrorCode_e WriteSRec0ToImage(const IO_Handle_t OutputIOHandle);

/**
 * @brief Write S9 Record to output image
 *
 * @param [in] OutputIOHandle - Handle to output image
 *
 * @return                    Execution status (E_SUCCESS after successful execution)
 *
 */
ErrorCode_e WriteSRec9ToImage(const IO_Handle_t OutputIOHandle);

#endif
