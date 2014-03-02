/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_MANIFEST_PARSER_H_
#define _R_MANIFEST_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup manifest_parser
 *    @{
 *      This module provides functions for parsing a manifest file. The handle
 *      for the manifest file can be any valid IO handle. @n
 *      The intention of the module is to help flashing files from a
 *      flash archive. @n
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_manifest_parser.h"

/**
 * @brief Creates a Manifest Parser Context.
 *        Allocates memory for *ManifestParser_pp.
 *
 * The function parses the data in the buffer FileData_p and places the data in
 * ManifestParser_t structure to which *ManifestParser_pp points.
 *
 * @param [in]     FileData_p is pointer to Manifest file buffer.
 * @param [in]     FileSize is Manifest file size.
 * @param [in,out] ManifestParser_pp Pointer to structure that contains Manifest
 *                 file data.
 * @param [out]    NoOfFiles_p pointer to number of files in Manifest file.
 * @param [in]     MaxNrOfEntries maximum number of entries (files) in the manifest file.
 * @return         E_SUCCESS - Manifest file context is created.
 *
 * @return         E_INVALID_INPUT_PARAMETERS - ManifestParser_pp is NULL.
 * @return         E_ALLOCATE_FAILED - Failed to allocate memory for the
 *                                          *ManifestParser_pp.
 * @return         E_GENERAL_FATAL_ERROR - This might happen if Manifest
 *                                              file version is not correct.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e
Do_ManifestParser_Create(char *FileData_p,
                         uint32 FileSize,
                         ManifestParser_t **const ManifestParser_pp,
                         uint32 *NoOfFiles_p,
                         uint32 MaxNrOfEntries);

/**
 * @brief Destroys a Manifest parser context.
 *
 * When the function is called with valid ManifestParser_pp, it will
 * deallocate any memory that has been allocated by Do_ManifestParser_Create
 * and sets *ManifestParser_pp to NULL.
 *
 * @param [in,out] ManifestParser_pp Pointer to a valid Manifest parser context
 *                 (pointer created with Do_ManifestParser_Create).
 * @return         E_SUCCESS - All the memory is deallocated and the
 *                                  Manifest Parser context is destroyed.
 *
 * @return         E_INVALID_INPUT_PARAMETER - ManifestParser_pp is NULL.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e
Do_ManifestParser_Destroy(ManifestParser_t **const ManifestParser_pp);

/**
 * @brief Get revision numbers of manifest file.
 *
 * @param [out]  ManifestRevision_p
 *
 * @return       none.
 */
void GetManifestRevision(ManifestRevision_t *ManifestRevision_p);

/** @} */
/** @} */
#endif /*_R_MANIFEST_PARSER_H_*/
