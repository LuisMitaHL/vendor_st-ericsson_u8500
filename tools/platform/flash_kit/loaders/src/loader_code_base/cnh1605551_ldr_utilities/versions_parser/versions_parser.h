/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/**
 * @file  versions_parser.h
 * @brief Functions and type declarations for versions parser
 *
 * @addtogroup ldr_utilities
 * @{
 *
 */

#ifndef VERSIONS_PARSER_H_
#define VERSIONS_PARSER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_basicdefinitions.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 *  @brief Reads Versions info from versions.txt file
 *
 *  @param[in] Data_p  pointer to buffer where the data is to be placed
 *  @param[in] Size    size of the output buffer pointed by Data_p
 *
 */
void Do_ReadVersionsInfo(char *Data_p, sint32 Size);

/**
 *  @brief Prints Versions info from versions.txt file
 *  to the flashing info buffer in RAM memory
 *
 */
void Do_PrintVersionsInfo(void);

#endif /* VERSIONS_PARSER_H_ */

/** @} */
