/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

#ifndef FLASH_PROCESS_FILE_INFO_H_
#define FLASH_PROCESS_FILE_INFO_H_

#include "toc_handler.h"
#include "t_manifest_parser.h"
#include "t_flashlayout_parser.h"
/**
 *  @addtogroup ldr_flash_app Loader flash applications
 *  @{
 *  Function and type declarations for flashing info data buffer
 *
 */

/**
 * Initializes flash info collect functions and buffers
 *
 * Note: Deletes all previously collected data!
 */
void InitializeFlashInfo(void);

/**
 * Adds flashing info record to the flashing info data buffer
 *
 * @param [in]      format          Flashing info record format string
 * @param [in]      ...             Flashing info format string values
 *
 * @return E_FLASH_INFO_BUFFER_OVERFLOW on error, E_SUCCESS otherwise
 *
 */
void AddFlashInfoRecord(const char *format, ...);


/**
 * Returns pointer to the flashing info data buffer
 *
 * @param [out]      Data_pp        Pointer to the flashing info data buffer
 *  *
 * @return                          Number of bytes stored in the buffer
 */
uint32 Do_ReadFlashInfoData(void **Data_pp);

/**
 * Prints TOC info into the flashing info buffer
 *
 * @param[in] It_p                pointer to the TOC list
 * @param[in] ManifestParser_p    pointer to the manifest parser
 *
 * @return E_FLASH_INFO_BUFFER_OVERFLOW on error, E_SUCCESS otherwise
 *
 */
void PrintTOCInfo(const TOC_List_t *It_p, ManifestParser_t *ManifestParser_p);

/**
 * Prints the physical address range where the TOC entry is flashed
 *
 * @param[in]   Entry          the TOC entry which physical address is to be printed
 * @param[in]   Flashlayout_p  pointer to the Flash layout parser
 *
 * @return E_FLASH_INFO_BUFFER_OVERFLOW on error, E_SUCCESS otherwise
 *
 */
void PrintPhysicalAddress(const TOC_Entry_t *const Entry_p,  const FlashLayoutParser_t  *Flashlayout_p);

/**
 * Verifies the Condition and prints error message to flash info buffer
 * when the Condition equals FALSE. ErrorCode is also printed
 *
 * @param[in]   Info_p  Message to be printed into the buffer
 * @param[in]   ErrorCode The error code value to be printed into the buffer
 *
 *
 */
void  VerifyAndRecord(boolean Condition, char *Info_p, ErrorCode_e ErrorCode);

#endif /* FLASH_PROCESS_FILE_INFO_H_ */
