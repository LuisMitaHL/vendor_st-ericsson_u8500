#ifndef _T_FLASH_APPLICATIONS_H_
#define _T_FLASH_APPLICATIONS_H_
/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_flash_app
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "toc_handler.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define MAX_DEVICE_PATH_LENGTH   (256)
#define MAX_FLASH_DEVICES (2)
#define MIN_NUM_SLASHES (2)
#define MAX_NUM_SLASHES (5)
/**
 * Payload for Flash Command.
 */
typedef struct {
    /** Session number*/
    uint16 Session;
    /** Payload length*/
    uint64 Length;
    /** Type of file. Example: Zip.*/
    uint16 FileType;
    /**/
    char  *SourcePath;
} FlashCommandPayLoad_t;

/**
 * Enumerated source type.
 */
typedef enum {
    DEVICE_FPD = 0,
    DEVICE_BAM,
    DEVICE_BDM,
    DEVICE_PARTITION,
    DEVICE_FILLED_PARTITION,
    DEVICE_UNKNOWN
} SourceType_t;

/**
 * Enumerated supported flash devices type.
 */
typedef enum {
    flash0 = 0,
    flash1
} DeviceID_t;

/**
 * Device Info.
 */
typedef struct {
    SourceType_t DeviceType;
    DeviceID_t   DeviceId;
    char         SUBTOCId[TOC_ID_LENGTH + 1];
    char         TOCId[TOC_ID_LENGTH + 1];
} DeviceInfo_t;

/*@}*/
#endif /*_T_FLASH_APPLICATIONS_H_*/
