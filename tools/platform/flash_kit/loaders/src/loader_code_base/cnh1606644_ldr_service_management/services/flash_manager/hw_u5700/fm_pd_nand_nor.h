/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _FM_PD_NAND_NOR_H_
#define _FM_PD_NAND_NOR_H_

/**
 * @addtogroup ldr_service_fpd Flash Physical Driver
 * @{
 *   @addtogroup ldr_service_fpd_hw_u5700
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "flash_manager.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * This function initialize FPD_Config_t structure with flash memory properties
 * from FPD.
 *
 * @return       @ref E_SUCCESS  – Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
void Do_PDNAND_NOR_Init(void);

/**
 * This function initialize Phisycal driver, sets memory properties.
 *
 * @return       @ref E_SUCCESS  – Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_Start(void);

/**
 * Phisycal driver can't be stop. This function need to exist for service
 * manager to work properly.
 *
 * @return       @ref E_SUCCESS  – Success ending.
 */
ErrorCode_e Do_PDNAND_NOR_Stop(void);

/**
 * Retrieves memory properties.
 *
 * @param [out]  Configuration_p - Pointer to configuration structure.
 * @return       @ref E_SUCCESS  – Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 *
 */
ErrorCode_e Do_PDNAND_NOR_GetInfo(FPD_Config_t *Configuration_p);

/**
 * Retrieves memory properties.
 *
 * @param [in] NewConfig_p    - Contains necessary information for
 *                              configuring the FPD.
 * @return     @ref E_SUCCESS – Success ending.
 *
 * @return     @ref E_INVALID_INPUT_PARAMETER - If NULL is passed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_SetInfo(FPD_Config_t *NewConfig_p);

/**
 * Reads one page and/or extra data from FPD. The length of the data buffer
 * Data_p must be "page size" * NbrOfPages. The length of the ExtraData_p must
 * be the ExtraDataLength * NbrOfPages.
 *
 * @param [in]   PageOffset     - An offset expressed in number of pages.
 * @param [out]  Data_p         - Data buffer. May be NULL if extra data is to
 *                                be read.
 * @param [out]  ExtraData_p    - Data buffer. May be NULL if page data is to
 *                                be read.
 * @return       @ref E_SUCCESS – Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_ReadPage(const uint32 PageOffset,
                                   uint8 *Data_p,
                                   uint8 *ExtraData_p);

/**
 * Reads one or several consecutive pages and/or extra data from FPD. The size
 * of the data buffer Data_p must be "page size" * NbrOfPages. The size of the
 * data buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to read.
 * @param [out] Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be read.
 * @param [out] ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be read.
 * @return      @ref E_SUCCESS – Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_ReadConsecutivePages(const uint32 PageOffset,
        const uint32 NbrOfPages,
        uint8 *Data_p,
        uint8 *ExtraData_p);

/**
 * Writes one page and/or extra data to FPD. The size of the data buffer Data_p
 * must be "page size" * NbrOfPages. The size of the data buffer ExtraData_p
 * must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be written.
 * @param [in]  ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be written.
 * @return      @ref E_SUCCESS – Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_WritePage(const uint32 PageOffset,
                                    const uint8 *const Data_p,
                                    const uint8 *const ExtraData_p);

/**
 * Writes one or several consecutive pages and/or extra data to FPD. The size of
 * the data buffer Data_p must be "page size" * NbrOfPages. The size of the data
 * buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to write.
 * @param [in]  Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be written.
 * @param [in]  ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be written.
 * @return      @ref E_SUCCESS – Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_WriteConsecutivePages(const uint32 PageOffset,
        const uint32 NbrOfPages,
        const uint8 *const Data_p,
        const uint8 *const ExtraData_p);

/**
 * Erases one or several blocks in FPD.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfBlocks    - Number of blocks to erase.
 * @return      @ref E_SUCCESS – Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_PDNAND_NOR_Erase(uint32 PageOffset,
                                uint32 NbrOfBlocks);

/**
 * Check for new partitions configuration.
 * @return      @ref E_UNSUPPORTED_FLASH_TYPE flash type is not supported by loader
 */
ErrorCode_e Do_PDNAND_NOR_GetPartitionsConfiguration(void);

/** @} */
/** @} */
#endif /*_FM_PD_NAND_NOR_H_*/
