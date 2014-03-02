/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef _BAM_EMMC_H_
#define _BAM_EMMC_H_

/**
 * @file  bam_emmc.h
 * @brief Function and type declarations for EMMC boot area management
 *
 * @addtogroup ldr_service_bam Boot Area Manager
 * @{
 *   @addtogroup ldr_service_bam_emmc EMMC Boot Area Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Starts up the EMMC Boot Area Management.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Start(void);

/**
 * Stops the EMMC Boot Area Management.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Stop(void);

/**
 * Reads data from EMMC Boot Area Management.
 *
 * @param[in]  FPDConfig_p Flash device configuration of BAM
 * @param[in]  Offset   Bytes from start of flash where data will be read.
 * @param[out] Data_p   Output buffer.
 * @param[in]  DataSize Bytes to read.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Read(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize);

/**
 * Writes data in EMMC.
 *
 * @param[in]  FPDConfig_p Flash device configuration of BAM
 * @param[in]  Offset   Bytes from start of flash where data will be written.
 * @param[in]  Data_p   Input buffer.
 * @param[in]  DataSize Bytes to write.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Write(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize);

/**
 * Erase all data from EMMC.
 *
 * @param[in]  FPDConfig_p Flash device configuration of BAM
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_CompleteErase(FPD_Config_t *const FPD_Config_p);


/** @} */
/** @} */
#endif /*_BAM_EMMC_H_*/
