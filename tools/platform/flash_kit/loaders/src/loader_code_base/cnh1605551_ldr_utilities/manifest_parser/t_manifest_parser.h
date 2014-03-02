/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_MANIFEST_PARSER_H_
#define _T_MANIFEST_PARSER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup manifest_parser
 *    @{
 */

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Maximum name length of structural element. */
#define MAX_NAME_SIZE 256
/** Maximum number of target devises. */
#define MAX_TARGET_TYPE  20
/** Maximum content type. */
#define MAX_CONTENT_TYPE  30
/** Maximum Toc ID. */
#define MAX_TOCID_SIZE 13

typedef struct {
    boolean Set;
    uint32  Pattern;
} Fill_t;

typedef struct {
    /**< Name of structural element */
    char Name[MAX_NAME_SIZE];
    /**< Type of data in structural element */
    char ContentType[MAX_CONTENT_TYPE];
    /**< Type of device to store structural element */
    char TargetType[MAX_TARGET_TYPE];
    /**< Id of device to store structural element */
    uint32 TargetId;
    /**< Address of the start of structural element */
    uint64 TargetStartAddress;
    /**< TOC ID of structural element */
    char TocID[MAX_TOCID_SIZE];
    /**< SubTOC of structural element */
    char SubTOC_Of[MAX_TOCID_SIZE];
    /**< TOC LoadAddress of structural element */
    uint32 TocLoadAddress;
    /**< TOC EntryPoint of structural element */
    uint32 TocEntryPoint;
    /**< TOC Flags of structural element */
    uint32 TocFlags;
    /**< Flag field used for loader internal purposes */
    uint32 FlashingFlags;
    /**< Size of the partition content

         In case of x-empflash/image content type this member contains the size of the file
         that is flashed in the corresponding partition

         In case of x-empflash/x-empflash/initilized-area this member value equals
         the size of the partition itself

         */
    uint64 PartitionContentSize;

    /**< Type of data in structural element */
    char SizeMode[MAX_CONTENT_TYPE];

    /**< Fill partition with data */
    Fill_t Fill;
    /**< Store image in enhanced area */
    boolean Enhanced;
} ManifestParser_t;

typedef struct {
    /**< Major version number of manifest file */
    uint32 RevNrMajor;
    /**< Minor version number of manifest file */
    uint32 RevNrMinor;
} ManifestRevision_t;

/** @} */
/** @} */
#endif /*_T_MANIFEST_PARSER_H_*/
