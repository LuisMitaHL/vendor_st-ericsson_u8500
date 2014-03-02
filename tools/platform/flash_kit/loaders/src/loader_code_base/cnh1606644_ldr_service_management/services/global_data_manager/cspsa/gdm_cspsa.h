/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _GDM_CSPSA_H_
#define _GDM_CSPSA_H_

/**
 * @addtogroup ldr_service_gdm Global Data Manager
 * @{
 *   @addtogroup ldr_service_gdm_cspsa Crash save parameter storage area
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
 * Open a Trim Area partition.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Start(void);

/**
 * Close an open Trim Area session.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Stop(void);

/**
 * Formats a partition. The partition pointed to by Config_p must not be opened.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Format(void);

/**
 * The current cached parameter storage area data is stored to memory media.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_Flush(void);

/**
 * Get the size of a unit.
 *
 * @param [in]  Unit     Unit to get size for.
 * @param [out] Size     Size of the unit.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_GetUnitSize(uint32 Unit, uint32 *Size);

/**
 * Read a unit.
 *
 * @param [in]  Unit     Unit to read.
 * @param [out] Data_p   Pointer to buffer to put the data in.
 * @param [in]  Size     Size of the unit.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_ReadUnit(uint32 Unit, uint8 *Data_p, uint32 Size);

/**
 * Write a unit.
 *
 * @param [in] Unit         Unit to write.
 * @param [in] Data_p       Pointer to the data to write.
 * @param [in] DataSize     Size to write.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_WriteUnit(uint32 Unit, uint8 *Data_p, uint32 DataSize);

/**
 * Get the size of the next unit in the list.
 *
 * @param [out] Unit_p  The unit number of the next parameter value.
 * @param [out] Size_p  Size of the next unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_GetUnitSizeNext(uint32 *Unit_p, uint32 *Size_p);

/**
 * Get the size of the first unit in the list.
 *
 * @param [out] Unit_p  The unit number of the next parameter value.
 * @param [out] Size_p  Size of the next unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GDM_CSPSA_GetUnitSizeFirst(uint32 *Unit_p, uint32 *Size_p);

/**
 * Register a plug-in functions to CSPSA.
 */
void CSPSA_LL_EMMC_Init(void);

/** @} */
/** @} */
#endif /*_GDM_CSPSA_H_*/
