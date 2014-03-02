/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_SYSTEM_APPLICATIONS_H_
#define _T_SYSTEM_APPLICATIONS_H_

/**
 *  @addtogroup ldr_system_app
 *  @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

#define ADDR_OFFSET  0x44
#define SIZE_OFFSET  0x48
#define HEADER_SIZE  0xE8

/** Debug data is required */
#define DEBUG_PRINTOUTS  (0)

/** Measurement data is required */
#define MEASUREMENT_RESULTS  (1)

/** Flash info data is required */
#define FLASH_INFO (2)

/**
 * Payload for System Command.
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
} SystemCommandPayLoad_t;

/**
 * Reboot mode.
 */
typedef enum {
    REBOOT_NORMAL = 0,
    REBOOT_SVC_MODE,
    REBOOT_JTAG_ENABLED,
    REBOOT_SVC_MODE_JTAG_ENABLED
} RebootMode_t;

/**
 * Enumerated watch dog reset action.
 *
 * This enum is used to set the boot stage parameter
 * for 2SBC needed after WDOG reset.
 */
typedef enum {
    SYSCTRL_WATCH_DOG_ACTION_SHUT_DOWN = 0,
    SYSCTRL_WATCH_DOG_ACTION_RESET = 1,
    SYSCTRL_WATCH_DOG_ACTION_SERVICE_MODE = 2
} SysCtrl_WatchDogAction_t;

/** @} */
#endif /*_T_SYSTEM_APPLICATIONS_H_*/
