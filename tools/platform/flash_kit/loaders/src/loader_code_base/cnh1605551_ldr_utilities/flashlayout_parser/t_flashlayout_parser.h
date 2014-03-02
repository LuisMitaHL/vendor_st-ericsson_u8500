/*******************************************************************************
 * $Copyright ST-Ericsson 2012 $
 ******************************************************************************/
#ifndef _T_FLASHLAYOUT_PARSER_H_
#define _T_FLASHLAYOUT_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup flashlayout_parser
 *    @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define MAX_PATH_SIZE          (256)
#define MAX_CONTENT_TYPE_SIZE  (100)
#define MAX_DESCRIPTION_SIZE   (100)
#define MAX_TOC_ID_SIZE        (13)

/* Major revision number for flash layout version 3.0. */
#define FLASHLAYOUT_3_0_REVISION_MAJOR  3
/* Minor revision number for flash layout version 3.0. */
#define FLASHLAYOUT_3_0_REVISION_MINOR  0

typedef struct {
    /**< Path of structural element */
    char Path[MAX_PATH_SIZE];
    /**< Type of data in structural element */
    char ContentType[MAX_CONTENT_TYPE_SIZE];
    /**< Description of structural element */
    char Description[MAX_DESCRIPTION_SIZE];
    /**< Table of contents identification number */
    char TOC_ID[MAX_TOC_ID_SIZE];
    /**< TOC ID that points to Sub TOC */
    char SubTOC_Of[MAX_TOC_ID_SIZE];
    /**< Address of the start of structural element */
    uint64  StartAddress;
    /**< Address of the end of structural element */
    uint64  EndAddress;
} FlashLayoutParser_t;

typedef struct {
    /**< Major version number of flashlayout file. */
    uint32 RevNrMajor;
    /**< Minor version number of flashlayout file. */
    uint32 RevNrMinor;
} FlashLayoutRevision_t;

/** @}*/
/** @}*/

#endif /*_T_FLASHLAYOUT_PARSER_H_*/
