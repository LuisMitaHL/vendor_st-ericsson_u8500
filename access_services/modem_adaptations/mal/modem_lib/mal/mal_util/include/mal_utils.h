/**
 * Copyright (C) ST-Ericsson 2010
 * header file for MAL Library Utilities
 * Author: Parthasarathy Nagarajan <parthasarathy.nagarajan@stericsson.com>
 */

#ifndef _MAL_UTILS_H
#define _MAL_UTILS_H

typedef enum {
    MAL_SUCCESS = 0,    /**< Success        */
    MAL_FAIL   = 1,     /**< Failure        */
    MAL_NOT_SUPPORTED = 2,  /**< Not Supported  */
} mal_error_type;

/*path for the mal debug level file*/
#define MAL_DEBUG_LEVEL_FILE  "/data/misc/mal_debug_level"
#define MAL_SETTINGS_FILE  "/data/misc/mal_settings"

/*Position of particular module's debug level in the mal_debug_level file*/
typedef enum {
    UNKNOWN_MODULE  = -1,
    MODULE_FIRST    = 0,
    CALL_SEEK       = MODULE_FIRST,
    SS_SEEK,
    NET_SEEK,
    MCE_SEEK,
    MIS_SEEK,
    GSS_SEEK,
    GPDS_SEEK,
    SMS_SEEK,
    UICC_SEEK,
    NVD_SEEK,
    AT_SEEK,
    MTE_SEEK,
    MON_SEEK,
    RF_SEEK,
    FTD_SEEK,
    /* Add new modules before this line */
    MODULE_COUNT,
} mal_module_id;

typedef enum {
    UNKNOWN_SETTINGS_ID = -1,
    SETTINGS_FIRST = 0,
    GPDS_FOLLOW_ON_BIT = SETTINGS_FIRST,
    /* Add new settings before this line */
    SETTINGS_COUNT,
} mal_settings_id;
/*debug level for mal logs*/
#define DEBUG_LEVEL0        (0) /* No Prints */
#define DEBUG_LEVEL1        (1) /* Entry & Exit Prints */
#define DEBUG_LEVEL2        (2) /* Error Condition Prints */
#define DEBUG_LEVEL3        (3) /* Value/Info Prints */
#define DEBUG_LEVEL4        (4) /* All Prints */

#define DEBUG_LEVEL_MIN     DEBUG_LEVEL0
#define DEBUG_LEVEL_DEFAULT DEBUG_LEVEL2
#define DEBUG_LEVEL_MAX     DEBUG_LEVEL4

#define DEFAULT_FOLLOW_ON_BIT 1

/**
 *  \fn mal_read_debug_level()
 *  \brief  This Api is used to read the debug level from a file
 */
void mal_read_debug_level(mal_module_id module_id, uint8_t *level);

/**
 *  \fn mal_write_debug_level()
 *  \brief  This Api is used to write the debug level to a file
 */
void mal_write_debug_level(mal_module_id module_id, uint8_t level);

/**
 *  \fn mal_read_settings()
 *  \brief  This Api is used to read setting id from the file
 */
void mal_read_settings(mal_settings_id setting_id, uint8_t *level);
#endif // _MAL_UTILS_H
