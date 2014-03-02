/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/
#ifndef _R_FLASHLAYOUT_PARSER_H_
#define _R_FLASHLAYOUT_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup flashlayout_parser
 *    @{
 *      This module provides functions for parsing a flashlayout file. The
 *      handle for the flashlayout file can be any valid IO handle. @n
 *      The prior intention of the module is to help format the flash and set up
 *      the partitions. @n
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_flashlayout_parser.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief Creates a Flash Layout Parser Context. Allocates memory for
 *        *FlashLayoutParser_pp.
 *
 * The function parses the data in the buffer FileData_p and places the data in
 * FlashLayoutParser_t structure to which *FlashLayoutParser_pp points.
 *
 * @param [in]     FileData_p is pointer to Flashlayout file buffer.
 * @param [in]     FileSize is Flashlayout file size.
 * @param [in,out] FlashLayoutParser_pp Pointer to structure that contains
 *                 Flashlayout file data.
 * @param [out]    NoOfFiles_p pointer to number of files in Flashlayout file.
 * @return         E_SUCCESS - Flashlayout file context is created
 *
 * @return         E_INVALID_INPUT_PARAMETERS - FlashLayoutParser_pp is
 *                                                   NULL.
 * @return         E_ALLOCATE_FAILED - Failed to allocate memory for the *
 *                                          FlashLayoutParser_pp.
 * @return         E_GENERAL_FATAL_ERROR - This might happen if Flashlayout
 *                                              file version is not correct.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */

ErrorCode_e
Do_FlashLayoutParser_Create(char                        *FileData_p,
                            uint32                       FileSize,
                            FlashLayoutParser_t **const FlashLayoutParser_pp,
                            uint32                      *NoOfFiles_p);

/**
 * @brief Destroys a Flash Layout parser context.
 *
 * When the function is called with valid FlashLayoutParser_pp, it will
 * dealocate any memory that has been allocated by Do_FlashLayoutParser_Create
 * and sets *FlashLayoutParser_pp to NULL.
 *
 * @param [in,out] FlashLayoutParser_pp Pointer to a valid Flashlayout parser
 *                 context (pointer created with Do_FlashLayoutParser_Create).
 * @return         E_SUCCESS - All the memory is deallocated and the
 *                                  Flashlayout Parser context is destroyed.
 *
 * @return         E_INVALID_INPUT_PARAMETER - FlashLayoutParser_pp is NULL
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */

ErrorCode_e
Do_FlashLayoutParser_Destroy(FlashLayoutParser_t **const FlashLayoutParser_pp);

/**
 * @brief Gets Flash Layout revision number.
 *
 * The revision number is filled in Do_FlashLayoutParser_Create() function.
 * Supported versions:
 *  - revision 2.0: Legacy flashlayout;
 *  - revision 3.0: Generic flashlayout.
 *
 * @param [in,out] FlashLayoutRevision_p   Pointer where the revision number will
 *                                         be stored.
 *
 * @return none
 *
 * @remark Do_FlashLayoutParser_Create() function must be called first.
 */
void
Do_FlashLayoutParser_GetRevision(FlashLayoutRevision_t *FlashLayoutRevision_p);

/** @}*/
/** @}*/

#endif /*_R_FLASHLAYOUT_PARSER_H_*/
