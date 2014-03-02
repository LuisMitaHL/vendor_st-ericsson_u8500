/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _GLOBAL_DATA_MANAGER_H_
#define _GLOBAL_DATA_MANAGER_H_

/**
 * @addtogroup ldr_service_management
 * @{
 *   @addtogroup ldr_service_gdm Global Data Manager
 *   @{
 */

/*******************************************************************************
 * Includes
*******************************************************************************/
#include "error_codes.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/**
* Contains GDM function pointers.
*/
typedef struct {
    ErrorCode_e(*GDM_Start)(void);
    ErrorCode_e(*GDM_Stop)(void);
    ErrorCode_e(*GDM_Format)(void);
    ErrorCode_e(*GDM_Flush)(void);
    ErrorCode_e(*GDM_GetUnitSize)(uint32 Unit, uint32 *Size);
    ErrorCode_e(*GDM_ReadUnit)(uint32 Unit, uint8 *Data_p, uint32 DataSize);
    ErrorCode_e(*GDM_WriteUnit)(uint32 Unit, uint8 *Data_p, uint32 DataSize);
    ErrorCode_e(*GDM_GetUnitSizeFirst)(uint32 *Unit_p, uint32 *Size_p);
    ErrorCode_e(*GDM_GetUnitSizeNext)(uint32 *Unit_p, uint32 *Size_p);
} GDM_Functions_t;

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * Sets valid pointer functions.
 */
void Do_GD_Init(void);

/**
 * Starts up the GDM (Global Data Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_Start(void);

/**
* Stops the GDM (Global Data Management).
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_GD_Stop(void);

/**
 * Format the GDM (Global Data Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_Format(void);

/**
 * The current cached parameter storage area data is stored to memory media.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_Flush(void);

/**
 * Retrieve the size of the given Unit.
 *
 * @param [in]  Unit          Unit whose size will be retrieved.
 * @param [out] Size_p        Pointer to variable to get the size of the Unit.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_GetUnitSize(uint32 Unit, uint32 *Size_p);

/**
 * Reads the contents of the given unit.
 *
 * @param [in]  Unit          Unit to be read.
 * @param [out] Data_p        Pointer to data to be read.
 * @param [in]  Size          Size of the data to be read.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_ReadUnit(uint32 Unit, uint8 *Data_p, uint32 Size);

/**
 * Write a Unit.
 *
 * @param [in] Unit           Unit to be written.
 * @param [in] Data_p         Pointer to data to be written.
 * @param [in] Size           Size of the data to be written.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_WriteUnit(uint32 Unit, uint8 *Data_p, uint32 Size);

/**
 * Gets the size and unit number of the first parameter value.
 *
 * @param [out] Unit_p  The unit number of the first parameter value.
 * @param [out] Size_p  Size of the first unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_GetUnitSizeFirst(uint32 *Unit_p, uint32 *Size_p);

/**
 * Get the size of the next unit in the list.
 *
 * @param [out] Unit_p  The unit number of the next parameter value.
 * @param [out] Size_p  Size of the next unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_GetUnitSizeNext(uint32 *Unit_p, uint32 *Size_p);

/** @} */
/** @} */
#endif /*_GLOBAL_DATA_MANAGER_H_*/
