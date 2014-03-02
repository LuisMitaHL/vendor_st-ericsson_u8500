/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _BAM_MBBS_H_
#define _BAM_MBBS_H_

/**
 * @file  bam_mbbs.h
 * @brief Function and type declarations for MBBS boot area management
 *
 * @addtogroup ldr_service_bam Boot Area Manager
 * @{
 *   @addtogroup ldr_service_bam_mbbs MBBS Boot Area Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Starts up the MBBS Boot Area Management.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_Start(void);

/**
 * Starts up the MBBS Boot Area Management.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_UnregisteredStart(void);

/**
 * Stops the MBBS Boot Area Management.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_Stop(void);

/**
 * Reads data from MBBS Area Management.
 *
 * @param[in]  Unit     Unit to read from.
 * @param[in]  Offset   Bytes from start of flash where data will be read.
 * @param[out] Data_p   Output buffer.
 * @param[in]  DataSize Bytes to read.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_Read(uint32 Unit, uint32 Offset, uint8 *Data_p, uint32 DataSize);

/**
 * Reads data from MBBS Boot Area Management.
 *
 * @param [out] Data_pp    Output buffer.
 * @param [out] DataSize_p Size of the data that will be read, in bytes for
 *                         the specified block number.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_UnregisteredReadBootRecords(uint8 **Data_pp, uint32 *DataSize_p);

/**
 * Writes data in MBBS.
 *
 * @param[in]  Unit     Unit to write in.
 * @param[in]  Offset   Bytes from start of flash where data will be written.
 * @param[in]  Data_p   Input buffer.
 * @param[in]  DataSize Bytes to write.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_Write(const uint32 Unit, uint32 Offset, uint8 *Data_p, uint32 DataSize);

/**
 * Write boot records data.
 *
 * @param[in]  Data_p   Input buffer.
 * @param[in]  DataSize Bytes to write.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_WriteBootRecords(uint8 *Data_p, uint32 DataSize);

/**
 * Erase all data from MBBS for the specified Unit.
 *
 * @param [in] Unit     The unit to erase a block in.
 *
 * @return ErrorCode_e Forwards the response from MBBS module.
 */
ErrorCode_e Do_BAM_MBBS_CompleteErase(uint32 Unit);

/**
 * Query MBBS unit for the size of a specific Reserved Block.
 *
 * @param [in] Unit         The unit to retrieve block size from.
 * @param [in] Reserved     Reserved block to get size for.
 * @param [out] BlockSize_p Size of the Reserved block.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_MBBS_GetBlockSize(uint32 Unit, uint32 Reserved, uint32 *BlockSize_p);

/** @} */
/** @} */
#endif /*_BAM_MBBS_H_*/
