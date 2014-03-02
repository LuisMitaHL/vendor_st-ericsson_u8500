/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_R_ZIP_PARSER_H_
#define _INCLUSION_GUARD_R_ZIP_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup zip_parser
 *    @{
 *        This module provides functions for parsing a zip file. The handle
 *      for the zip file can be any valid IO handle, including file system
 *      handle, bulk handle, etc. @n
 *        The prior intention of the module is to provide capability for
 *      flashing of big zip files. @n
 *        The module functions are reentrant, meaning more than one zip file
 *      can be opened at the same time. @n
 *        It supports only uncompressed zip archives, and zip comments are
 *      not allowed due to security reasons. @n
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "r_io_subsystem.h"
#include "error_codes.h"
#include "t_zip_parser.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Creates a Zip Parser Context.
 *
 * Allocates memory for *ZipHandle_pp.
 * The function is implemented as state machine. The state is returned
 * in the variable ZipState_p. When *ZipState_p is equal to ZIP_FINISHED,
 * the function has finished.
 *
 * @param [in]  ReadHandle Valid ReadHandle to a Zip File. This can be a
 *              bulk handle if the file is transmitted over bulk protocol.
 *
 * @param [in,out]  ZipHandle_pp Handle used by the zip parser for holding
 *                  information for the Zip file. When the function is called for the
 *                  first time *ZipHandle_pp must be set to NULL.
 *
 * @param [out] State_p  Returns the State of the Zip parser.
 *
 * @param [in] ZipOpenFileMode  Indicator that select in wich mode files
 *                              will be opened from zip archive.
 *
 * @param [in] Context_p  VerifyContext to use if using secure wrap.
 *
 * @return see \c ErrorCode_e for detail description of the error codes.
 *
 * @retval  E_SUCCESS if the state has finished successfully. This doesn't mean
 *          that the parser context is created. Parser context is created when
 *          State_p == ZIP_FINISHED.
 * @retval  E_INVALID_INPUT_PARAMETERS if ZipHandle_pp is NULL.
 * @retval  E_ALLOCATE_FAILED Failed to allocate memory for the *ZipHandle_pp.
 * @retval  E_GENERAL_FATAL_ERROR This might happen if State_p is not pointing
 *          to a valid Zip State, or when the parsing of the file fails.
 *
 * Example use:
 * @code
 *
 * void ZipExample(ReadHandle)
 * {
 *  ZipHandle_t * ZipHandle_p = NULL;
 *  ZipState_t ZipState;
 *  do
 *  {
 *   RetVal= Do_Zip_CreateParserContext(ReadHandle, &ZipHandle_p, &ZipState, ZIP_USE_IO_WRAP);
 *  }while((ZipState != ZIP_FINISHED)&&(E_SUCCESS==RetVal))
 * }
 * @endcode
 */
ErrorCode_e Do_Zip_CreateParserContext(const IO_Handle_t ReadHandle,
                                       void **const ZipHandle_pp,
                                       ZipState_t *const State_p,
                                       ZipOpenFileMode_t  ZipOpenFileMode,
                                       VerifyContext_t *VerifyContext_p);

/**
 * Destroys a Zip parser context.
 *
 * When the function is called with valid ZipHandle_pp, it will
 * dealocate any memory that has been allocated by Do_Zip_CreateParserContext
 * and sets *ZipHandle_pp to NULL.
 *
 * @param [in,out] ZipHandle_pp Pointer to a valid Zip parser context
 *                (pointer created with Do_Zip_CreateParserContext).
 *
 * @return see \c ErrorCode_e for detail description of the error codes.
 *
 * @retval E_INVALID_INPUT_PARAMETER if ZipHandle_pp is NULL.
 * @retval E_SUCCESS All the memory is dealocated and the Zip Parser context is
 *         destroyed.
 */
ErrorCode_e Do_Zip_DestroyParserContext(void **const ZipHandle_pp);

/**
 * Opens a file from a Zip archive.
 *
 * Opens a file from the archive specified by ZipHandle_p, and returns a handle
 * for reading from the file.
 *
 * @param [in] ZipHandle_p    Valid pointer to a Zip parser context.
 * @param [in] FileName_p     Name of the file to be opened.
 * @param [in] FileNameLength Length of the name of the file.
 * @param [out] IO_Handle_p   pointer to handle of the file opened by
 *                            Do_Zip_OpenFile.
 *
 * @return see \c ErrorCode_e for detail description of the error codes.
 *
 * @retval E_INVALID_INPUT_PARAMETERS Either ZipHandle_p or FileName_p is NULL.
 * @retval E_ZIP_PARSER_FILE_NOT_FOUND The requested file is not found
 *         in the archive specified by ZipHandle_p
 */
ErrorCode_e Do_Zip_OpenFile(const void        *const ZipHandle_p,
                            const char        *const FileName_p,
                            const uint32             FileNameLength,
                            IO_Handle_t *const IO_Handle_p);

/**
 * Get total size of all files in archive
 *
 * @param [in]  ZipHandle_p    Valid pointer to a Zip parser context.
 * @param [out] TotalLength_p  Valid pointer to a place where required
 *                             information will be stored.
 *
 * @retval E_INVALID_INPUT_PARAMETERS
 * @retval E_SUCCESS
 */
ErrorCode_e Do_Zip_GetTotalFilesSize(const void     *const ZipHandle_p,
                                     uint64   *const TotalLength_p);

/**
 * Get total number of entries in archive
 *
 * @param [in]  ZipHandle_p    Valid pointer to a Zip parser context.
 * @param [out] NrOfEntries_p  Valid pointer to a place where required
 *                             information will be stored.
 *
 * @retval E_INVALID_INPUT_PARAMETERS
 * @retval E_SUCCESS
 */
ErrorCode_e Do_Zip_GetTotalNrOfEntries(const void *const ZipHandle_p,
                                       uint32 *const NrOfEntries_p);
/*******************************************************************************
 *  Macros definition
 ******************************************************************************/
/**
 * This macro destroy parser context with sanity check of input parameters.
 *
 * @param [in] ZipHandle_p      Handle to a Zip parser context.
 *
 */
#define DESTROY_PARSER_CONTEXT(ZipHandle_p) \
if (NULL != (ZipHandle_p)) \
{ \
  ErrorCode_e TempResult; \
  \
  TempResult = Do_Zip_DestroyParserContext(&(ZipHandle_p)); \
  if(E_SUCCESS != TempResult) \
  { \
    ReturnValue = TempResult; \
  } \
}

/** @} */
/** @} */
#endif  /*_INCLUSION_GUARD_R_ZIP_PARSER_H_*/
