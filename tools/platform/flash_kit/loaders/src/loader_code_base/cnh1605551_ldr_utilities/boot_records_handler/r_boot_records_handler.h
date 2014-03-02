/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_BOOT_RECORDS_HANDLER_H_
#define _R_BOOT_RECORDS_HANDLER_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *    @addtogroup ldr_boot_records_handler
 *    @{
 *      This module take care about boot records operations.
 *      @li It provide option for writing complete boot records image inside
 *           flash memory.
 *      @li This module keep copy of boot records inside RAM memory. This, so
 *          called boot records containers are constantly refreshed with any
 *          write operations, so they provide fast and reliable way of using
 *          boot record data.
 *      @li This module also provide indexed and non - indexed read
 *          function. This mean that user can read specified container with boot
 *          record data depending if specified record has special partition
 *          index for referencing.
 *      @li Beside read function this parser has possibility of changing
 *          contents in boot records and with that automatically refreshing boot
 *          record containers. Here are also possible two variations of indexed
 *          and non - indexed writing on particular boot record depending if
 *          specified record has special partition index for referencing.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_boot_records_handler.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief This function handle a boot image that comes from a .ssw file.
 *
 * @param [in] BootRecordsImage_p   This is the data buffer containing the boot
 *                                  records image.
 * @param [in] BootRecordsSize      This is the size of the boot records image.
 *
 * @return     @ref E_SUCCESS       Success ending.
 * @return     @ref E_INVALID_INPUT_PARAMETERS - If there is no valid input
 *                                               parameters sent.
 * @return     @ref E_INVALID_BOOTRECORD_IMAGE - If there is no valid boot
 *                                               record image.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_WriteCompleteBootRecordsImage(uint32 *BootRecordsImage_p,
        uint32 BootRecordsSize);

/**
 * @brief This function read the boot record that is identified by BootRecordId,
 *        BootPartitionIndex.
 *
 * @param [in]      BootRecordId - Record Id of specified boot record.
 * @param [in]      BootPartitionIndex - Partition Index.
 * @param [out]     BootRecord_pp - Pointer to boot record data.
 * @param [in, out] BootRecordSize_p - Points to variable in wich size of
 *                                     specified container will be placed.
 *
 * @return     @ref E_SUCCESS - Success ending.
 * @return     @ref E_BOOTRECORD_EMPTY - If flash is empty and has no data to be
 *                                       written.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 *
 * @note Data that are returned contain complete boot record informations
 *       (including Id and Size).
 *
 * @example
 * // The example how to read partition CABS from PARTITION boot record.
 *
 *     ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
 *     uint32 PartitionId = 0x00010042;
 *     uint32 CabsPartitionIndex = 1;
 *     uint32 *CabsRecordStorage_p = NULL;
 *     uint32 CabsRecordSize = 0;
 *
 *     ReturnValue = Do_Read_BootRecordIndexed(PartitionId, CabsPartitionIndex
 *                                       &CabsRecordStorage_p, &CabsRecordSize);
 *     if (ReturnValue == E_SUCCESS)
 *     {
 *       // User may read CabsRecordSize valid bytes from CabsRecordStorage_p.
 *     }
 */
ErrorCode_e Do_Read_BootRecordIndexed(uint32 BootRecordId,
                                      uint32 BootPartitionIndex,
                                      uint32 **BootRecord_pp,
                                      uint32 *BootRecordSize_p);

/**
 * @brief This function read the boot record wich is specified with BootRecordId.
 *
 * This macro serve for simplification of "Do_Read_BootRecordIndexed" function
 * in cases where BootPartitionIndex parameter don't have any significance. In
 * that case only BootRecordId is valid reference parameter while
 * BootPartitionIndex is set to zero.
 *
 * @param [in]      BootRecordId - Record Id of specified boot record.
 * @param [out]     BootRecord_pp - Pointer to boot record data.
 * @param [in, out] BootRecordSize_p - Pointer to size of specified container.
 *
 * @return     @ref E_SUCCESS - Success ending.
 * @return     @ref E_BOOTRECORD_EMPTY - If flash is empty and has no data to be
 *                                       written.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 *
 * @note Data that are returned contain complete boot record informations
 *       (including Id and Size).
 *
 * @example
 * // The example how to read MEMCONG boot record.
 *
 *     ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
 *     uint32 MemconfId = 0x00000001;
 *     uint32 *MemconfRecordStorage_p = NULL;
 *     uint32 MemconfRecordSize = 0;
 *
 *     ReturnValue = Do_Read_BootRecord(MemconfId, &MemconfRecordStorage_p,
 *                                            &MemconfRecordSize);
 *     if (ReturnValue == E_SUCCESS)
 *     {
 *       // User may read MemconfRecordSize valid bytes from MemconfRecordStorage_p.
 *     }
 */
#define Do_Read_BootRecord(BootRecordId, BootRecord_pp, BootRecordSize_p)\
        Do_Read_BootRecordIndexed(BootRecordId, 0, BootRecord_pp,\
                                                        BootRecordSize_p)

/**
 * @brief This function write single boot record that is identified by
 *        BootRecordId and BootPartitionIndex.
 *
 * @param [in]     BootRecordId - Record Id of specified boot record.
 * @param [in]     BootPartitionIndex - Partition Index.
 * @param [out]    BootRecordData_p - Pointer to boot record data.
 * @param [in]     BootRecordSize - Size of data that should be written(pointed
 *                                  by BootRecordData_p pointer).
 *
 * @return     @ref E_SUCCESS - Success ending.
 * @return     @ref E_INVALID_INPUT_PARAMETERS  - If there is no valid input
 *                                                parameters sent.
 * @return     @ref E_BOOTRECORD_FULL - If containers cannot support storage for
 *                                      more boot records. This is software
 *                                      limitation that should be easily fixed.
 * @return     @ref E_BOOT_RECORD_EMPTY - If no valid boot records exist on
 *                                        flash memory. User should wait untill
 *                                        valid boot record image is flashed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 *
 * @note Data that are provided as input parameter in this function should
 *       contain complete boot record information including Id and Size.
 */
ErrorCode_e Do_Write_BootRecordIndexed(uint32 BootRecordId,
                                       uint32 BootPartitionIndex,
                                       uint32 *BootRecordData_p,
                                       uint32 BootRecordSize);

/**
 * @brief This function write the boot record wich is specified with
 *        BootRecordId.
 *
 * This macro serve for simplification of "Do_Write_BootRecordIndexed" function
 * in cases where BootPartitionIndex parameter don't have any significance. In
 * that case only BootRecordId is valid reference parameter while
 * BootPartitionIndex is set to zero.
 *
 * @param [in]     BootRecordId - Record Id of specified boot record.
 * @param [out]    BootRecordData_p - Pointer to boot record data.
 * @param [in]     BootRecordSize - Size of data that should be written(pointed
 *                                  by BootRecordData_p pointer).
 *
 * @return     @ref E_SUCCESS - Success ending.
 * @return     @ref E_INVALID_INPUT_PARAMETERS  - If there is no valid input
 *                                                parameters sent.
 * @return     @ref E_BOOTRECORD_FULL - If containers cannot support storage for
 *                                      more boot records. This is software
 *                                      limitation that should be easily fixed.
 * @return     @ref E_BOOT_RECORD_EMPTY - If no valid boot records exist on
 *                                        flash memory. User should wait untill
 *                                        valid boot record image is flashed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 *
 * @note Data that are provided as input parameter in this function should
 *       contain complete boot record information including Id and Size.
 */
#define Do_Write_BootRecord(BootRecordId, BootRecordData_p, BootRecordSize)\
        Do_Write_BootRecordIndexed(BootRecordId, 0, BootRecordData_p,\
                                                            BootRecordSize)

/**
 * @brief Destroys a Boot Record Containers context.
 *
 * When the function is called and valid boot record containers exist, it will
 * deallocate any memory that has been allocated by Fill_BootRecordContainer
 * and sets BootRecordContainer_pp to NULL.
 */
void Do_BootRecordsContainer_Destroy(void);

/** @} */
/** @} */
#endif /*_R_BOOT_RECORDS_HANDLER_H_*/
