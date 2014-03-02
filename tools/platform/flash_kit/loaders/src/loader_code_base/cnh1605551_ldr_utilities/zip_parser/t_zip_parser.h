/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_T_ZIP_PARSER_H_
#define _INCLUSION_GUARD_T_ZIP_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup zip_parser
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "error_codes.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** default error value */
#define ZIP_ERROR -1

/**
 * Enumerated type containing the States of the Do_Zip_ParserCreate.
 */
typedef enum {
    /** Initialization of End Of Central Directory*/
    ZIP_INIT_EOCDR = 0,
    /** Processing End Of Central Directory*/
    ZIP_PROCESSING_EOCDR = 1,
    /** Initialization of End Of Central Directory Locator*/
    ZIP64_INIT_EOCD_LOCATOR = 2,
    /** Processing of End Of Central Directory Locator*/
    ZIP64_PROCESSING_EOCD_LOCATOR = 3,
    /** Initialization of Zip64 End Of Central Directory Record*/
    ZIP64_INIT_EOCD_RECORD = 4,
    /** Processing Zip64 End Of Central Directory Record*/
    ZIP64_PROCESSING_EOCD_RECORD = 5,
    /** Initialization of Central Directory*/
    ZIP_INIT_CD = 6,
    /** Processing Central Directory*/
    ZIP_PROCESSING_CD = 7,
    /** Final state. Parser is created*/
    ZIP_FINISHED = 8
} ZipState_t;

/**
 * Enumerated type containing modes of opening files
 * from Zip archive.
 */
typedef enum {
    /** Files will be opened using IO Wrap functionality*/
    ZIP_USE_IO_WRAP = 0,
    /** Files will be opened using IO Secure wrap functionality*/
    ZIP_USE_IO_SEC_WRAP
} ZipOpenFileMode_t;

/** @} */
/** @} */
#endif  /*_INCLUSION_GUARD_T_ZIP_PARSER_H_*/
