/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_CONFIGURATION_PARSER_H_
#define _R_CONFIGURATION_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup configuration_parser
 *    @{
 *      This module provides functions for parsing a fsconf.dat file.@n
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_configuration_parser.h"

/**
 * @brief Get single volume configuration.
 *
 * The function parses the data from fsconf.dat file and places the data in
 * ConfigurationParser_t structure to which VolumeConfig_p points.
 *
 * @param [in,out] VolumeConfig_p is a pointer which will be set to point on
 *                 valid volume configuration.
 * @param [in]     VolumeInfo_p is a pointer to buffer filed with context of
 *                 fsconf.dat file
 * @param [in]     VolumeInfoLen determine size of VolumeInfo_p buffer.
 * @return         @ref E_SUCCESS - Configuration file context is created.
 *
 * @return         @ref E_INVALID_INPUT_PARAMETERS -if invalid handle is passed.
 * @return         @ref E_GENERAL_FATAL_ERROR - if something unexpected happen.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 *
 * @note This function does not explicetely notify user when and of valid
 *       configurations reached.
 */
ErrorCode_e GetVolumeConfiguration(ConfigurationParser_t *VolumeConfig_p,
                                   uint8  *VolumeInfo_p,
                                   uint64  VolumeInfoLen);

/** @} */
/** @} */
#endif /*_R_CONFIGURATION_PARSER_H_*/
